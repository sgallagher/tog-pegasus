//%2005////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001, 2002 BMC Software; Hewlett-Packard Development
// Company, L.P.; IBM Corp.; The Open Group; Tivoli Systems.
// Copyright (c) 2003 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation, The Open Group.
// Copyright (c) 2004 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation; VERITAS Software Corporation; The Open Group.
// Copyright (c) 2005 Hewlett-Packard Development Company, L.P.; IBM Corp.;
// EMC Corporation; VERITAS Software Corporation; The Open Group.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// THE ABOVE COPYRIGHT NOTICE AND THIS PERMISSION NOTICE SHALL BE INCLUDED IN
// ALL COPIES OR SUBSTANTIAL PORTIONS OF THE SOFTWARE. THE SOFTWARE IS PROVIDED
// "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT
// LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
// PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
// HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
// ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
// WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
//
//==============================================================================
//
// Author: Roger Kumpf, Hewlett-Packard Company (roger_kumpf@hp.com)
//         Jenny Yu, Hewlett-Packard Company (jenny_yu@hp.com)
//
// Modified By:	Sean Keenan, Hewlett-Packard Company (sean.keenan@hp.com)
//              Carol Ann Krug Graves, Hewlett-Packard Company
//                  (carolann_graves@hp.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/Constants.h>
#include <Pegasus/Common/AutoPtr.h>
#include <Pegasus/Common/ArrayInternal.h>
#include <Pegasus/Common/CIMMessage.h>
#include <Pegasus/Common/CIMMessageSerializer.h>
#include <Pegasus/Common/CIMMessageDeserializer.h>
#include <Pegasus/Common/OperationContextInternal.h>
#include <Pegasus/Common/System.h>
#include <Pegasus/Common/AnonymousPipe.h>
#include <Pegasus/Common/Tracer.h>
#include <Pegasus/Common/Logger.h>
#include <Pegasus/Common/Thread.h>
#include <Pegasus/Common/MessageQueueService.h>
#include <Pegasus/Config/ConfigManager.h>

#if defined (PEGASUS_OS_TYPE_WINDOWS)
# include <windows.h>  // For CreateProcess()
#elif defined (PEGASUS_OS_OS400)
# include <unistd.cleinc>
#elif defined (PEGASUS_OS_VMS)
# include <perror.h>
# include <climsgdef.h>
# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <processes.h>
# include <unixio.h>
#else
# include <unistd.h>  // For fork(), exec(), and _exit()
# include <errno.h>
# include <sys/types.h>
# if defined(PEGASUS_HAS_SIGNALS)
#  include <sys/wait.h>
# endif
#endif

#include "OOPProviderManagerRouter.h"

PEGASUS_USING_STD;

PEGASUS_NAMESPACE_BEGIN

/////////////////////////////////////////////////////////////////////////////
// OutstandingRequestTable and OutstandingRequestEntry
/////////////////////////////////////////////////////////////////////////////

/**
    An OutstandingRequestEntry represents a request message sent to a
    Provider Agent for which no response has been received.  The request
    sender provides the message ID and a location for the response to be
    returned, and then waits on the semaphore.  When a response matching
    the message ID is received, it is placed into the specified location
    and the semaphore is signaled.
 */
class OutstandingRequestEntry
{
public:
    OutstandingRequestEntry(
        String messageId_,
        CIMResponseMessage*& responseMessage_,
        Semaphore* responseReady_)
        : messageId(messageId_),
          responseMessage(responseMessage_),
          responseReady(responseReady_)
    {
    }

    String messageId;
    CIMResponseMessage*& responseMessage;
    Semaphore* responseReady;
};

typedef HashTable<String, OutstandingRequestEntry*, EqualFunc<String>,
    HashFunc<String> > OutstandingRequestTable;


/////////////////////////////////////////////////////////////////////////////
// ProviderAgentContainer
/////////////////////////////////////////////////////////////////////////////

class ProviderAgentContainer
{
public:
    ProviderAgentContainer(
        const String & moduleName,
        const String & userName,
        PEGASUS_INDICATION_CALLBACK indicationCallback,
        Boolean subscriptionInitComplete);

    ~ProviderAgentContainer();

    Boolean isInitialized();

    String getModuleName() const;

    CIMResponseMessage* processMessage(CIMRequestMessage* request);
    void unloadIdleProviders();

private:
    //
    // Private methods
    //

    /** Unimplemented */
    ProviderAgentContainer();
    /** Unimplemented */
    ProviderAgentContainer(const ProviderAgentContainer& pa);
    /** Unimplemented */
    ProviderAgentContainer& operator=(const ProviderAgentContainer& pa);

    /**
        Start a Provider Agent process and establish a pipe connection with it.
        Note: The caller must lock the _agentMutex.
     */
    void _startAgentProcess();

    /**
        Send initialization data to the Provider Agent.
        Note: The caller must lock the _agentMutex.
     */
    void _sendInitializationData();

    /**
        Initialize the ProviderAgentContainer if it is not already
        initialized.  Initialization includes starting the Provider Agent
        process, establishing a pipe connection with it, and starting a
        thread to read response messages from the Provider Agent.

        Note: The caller must lock the _agentMutex.
     */
    void _initialize();

    /**
        Uninitialize the ProviderAgentContainer if it is initialized.
        The connection is closed and outstanding requests are completed
        with an error result.

        Note: The caller must lock the _agentMutex.
     */
    void _uninitialize();

    /**
        Read and process response messages from the Provider Agent until
        the connection is closed.
     */
    void _processResponses();
    static PEGASUS_THREAD_RETURN PEGASUS_THREAD_CDECL
        _responseProcessor(void* arg);

    //
    // Private data
    //

    /**
        The _agentMutex must be locked whenever writing to the Provider
        Agent connection, accessing the _isInitialized flag, or changing
        the Provider Agent state.
     */
    Mutex _agentMutex;

    /**
        Name of the provider module served by this Provider Agent.
     */
    String _moduleName;

    /**
        The user context in which this Provider Agent operates.
     */
    String _userName;

    /**
        Callback function to which all generated indications are sent for
        processing.
     */
    PEGASUS_INDICATION_CALLBACK _indicationCallback;

    /**
        Indicates whether the Provider Agent is active.
     */
    Boolean _isInitialized;

    /**
        Pipe connection used to read responses from the Provider Agent.
     */
    AutoPtr<AnonymousPipe> _pipeFromAgent;
    /**
        Pipe connection used to write requests to the Provider Agent.
     */
    AutoPtr<AnonymousPipe> _pipeToAgent;

#if defined(PEGASUS_HAS_SIGNALS)
    /**
        Process ID of the active Provider Agent.
     */
    pid_t _pid;
#endif

    /**
        The _outstandingRequestTable holds an entry for each request that has
        been sent to this Provider Agent for which no response has been
        received.  Entries are added (by the writing thread) when a request
        is sent, and are removed (by the reading thread) when the response is
        received (or when it is determined that no response is forthcoming).
     */
    OutstandingRequestTable _outstandingRequestTable;
    /**
        The _outstandingRequestTableMutex must be locked whenever reading or
        updating the _outstandingRequestTable.
     */
    Mutex _outstandingRequestTableMutex;

    /**
        Holds the last provider module instance sent to the Provider Agent in
        a ProviderIdContainer.  Since the provider module instance rarely
        changes, an optimization is used to send it only when it differs from
        the last provider module instance sent.
     */
    CIMInstance _providerModuleCache;

    /**
        The number of Provider Agent processes that are currently initialized
        (active).
    */
    static Uint32 _numProviderProcesses;

    /**
        The _numProviderProcessesMutex must be locked whenever reading or
        updating the _numProviderProcesses count.
    */
    static Mutex _numProviderProcessesMutex;

    /**
        The maximum number of Provider Agent processes that may be initialized
        (active) at one time.
    */
    static Uint32 _maxProviderProcesses;

    /**
        Indicates whether the Indication Service has completed initialization.

        For more information, please see the description of the
        ProviderManagerRouter::_subscriptionInitComplete member variable.
     */
    Boolean _subscriptionInitComplete;
};

Uint32 ProviderAgentContainer::_numProviderProcesses = 0;
Mutex ProviderAgentContainer::_numProviderProcessesMutex;
Uint32 ProviderAgentContainer::_maxProviderProcesses = PEG_NOT_FOUND;

ProviderAgentContainer::ProviderAgentContainer(
    const String & moduleName,
    const String & userName,
    PEGASUS_INDICATION_CALLBACK indicationCallback,
    Boolean subscriptionInitComplete)
    : _moduleName(moduleName),
      _userName(userName),
      _indicationCallback(indicationCallback),
      _isInitialized(false),
      _subscriptionInitComplete(subscriptionInitComplete)
{
    PEG_METHOD_ENTER(TRC_PROVIDERMANAGER,
        "ProviderAgentContainer::ProviderAgentContainer");
    PEG_METHOD_EXIT();
}

ProviderAgentContainer::~ProviderAgentContainer()
{
    PEG_METHOD_ENTER(TRC_PROVIDERMANAGER,
        "ProviderAgentContainer::~ProviderAgentContainer");

    // Ensure the destructor does not throw an exception
    try
    {
        if (isInitialized())
        {
            // Stop the responseProcessor thread by closing its connection
            _pipeFromAgent->closeReadHandle();

            // Wait for the responseProcessor thread to exit
            while (isInitialized())
            {
                pegasus_yield();
            }
        }
    }
    catch (...)
    {
    }

    PEG_METHOD_EXIT();
}

void ProviderAgentContainer::_startAgentProcess()
{
    PEG_METHOD_ENTER(TRC_PROVIDERMANAGER,
        "ProviderAgentContainer::_startAgentProcess");

    AutoPtr<AnonymousPipe> pipeFromAgent(new AnonymousPipe());
    AutoPtr<AnonymousPipe> pipeToAgent(new AnonymousPipe());

    //
    // Start a cimprovagt process for this provider module
    //

#if defined (PEGASUS_OS_TYPE_WINDOWS)
    //
    //  Set up members of the PROCESS_INFORMATION structure
    //
    PROCESS_INFORMATION piProcInfo;
    ZeroMemory (&piProcInfo, sizeof (PROCESS_INFORMATION));

    //
    //  Set up members of the STARTUPINFO structure
    //
    STARTUPINFO siStartInfo;
    ZeroMemory (&siStartInfo, sizeof (STARTUPINFO));
    siStartInfo.cb = sizeof (STARTUPINFO);

    //
    //  Generate the command line
    //
    char cmdLine[2048];
    char readHandle[32];
    char writeHandle[32];
    pipeToAgent->exportReadHandle(readHandle);
    pipeFromAgent->exportWriteHandle(writeHandle);

    sprintf(cmdLine, "\"%s\" %s %s \"%s\"",
        (const char*)ConfigManager::getHomedPath(
            PEGASUS_PROVIDER_AGENT_PROC_NAME).getCString(),
        readHandle, writeHandle, (const char*)_moduleName.getCString());

    //
    //  Create the child process
    //
    if (!CreateProcess (
        NULL,          //
        cmdLine,       //  command line
        NULL,          //  process security attributes
        NULL,          //  primary thread security attributes
        TRUE,          //  handles are inherited
        0,             //  creation flags
        NULL,          //  use parent's environment
        NULL,          //  use parent's current directory
        &siStartInfo,  //  STARTUPINFO
        &piProcInfo))  //  PROCESS_INFORMATION
    {
        Tracer::trace(TRC_PROVIDERMANAGER, Tracer::LEVEL2,
            "CreateProcess() failed.  errno = %d.", GetLastError());
        PEG_METHOD_EXIT();
        throw Exception(MessageLoaderParms(
            "ProviderManager.OOPProviderManagerRouter.CIMPROVAGT_START_FAILED",
            "Failed to start cimprovagt \"$0\".",
            _moduleName));
    }

    CloseHandle(piProcInfo.hProcess);
    CloseHandle(piProcInfo.hThread);

#elif defined (PEGASUS_OS_VMS)

    //
    //  fork and exec the child process
    //
    int status;

    status = vfork ();
    switch (status)
    {
      case 0:
        try
        {
          //
          // Execute the cimprovagt program
          //
          String agentCommandPath =
              ConfigManager::getHomedPath(PEGASUS_PROVIDER_AGENT_PROC_NAME);
          CString agentCommandPathCString = agentCommandPath.getCString();

          char readHandle[32];
          char writeHandle[32];
          pipeToAgent->exportReadHandle(readHandle);
          pipeFromAgent->exportWriteHandle(writeHandle);

          if ((status = execl(agentCommandPathCString, agentCommandPathCString,
              readHandle, writeHandle,
              (const char*)_moduleName.getCString(), (char*)0)) == -1);
          {
            // If we're still here, there was an error
            Tracer::trace(TRC_DISCARDED_DATA, Tracer::LEVEL2,
                "execl() failed.  errno = %d.", errno);
            _exit(1);
          }
        }
        catch (...)
        {
          // There's not much we can do here in no man's land
          try
          {
            PEG_TRACE_STRING(TRC_DISCARDED_DATA, Tracer::LEVEL2,
                "Caught exception before calling execl().");
          }
          catch (...) 
          {
          }
         _exit(1);
        }
        PEG_METHOD_EXIT();
        return;
        break;

      case -1:
        Tracer::trace(TRC_PROVIDERMANAGER, Tracer::LEVEL2,
            "fork() failed.  errno = %d.", errno);
        PEG_METHOD_EXIT();
        throw Exception(MessageLoaderParms(
            "ProviderManager.OOPProviderManagerRouter.CIMPROVAGT_START_FAILED",
            "Failed to start cimprovagt \"$0\".",
            _moduleName));
        break;

      default:
        // Close our copies of the agent's ends of the pipes
        pipeToAgent->closeReadHandle();
        pipeFromAgent->closeWriteHandle();

        _pipeToAgent.reset(pipeToAgent.release());
        _pipeFromAgent.reset(pipeFromAgent.release());

        PEG_METHOD_EXIT();
    }
#elif defined (PEGASUS_OS_OS400)

    //Out of provider support for OS400 goes here when needed.

#else
    pid_t pid = fork();
    if (pid < 0)
    {
        Tracer::trace(TRC_PROVIDERMANAGER, Tracer::LEVEL2,
            "fork() failed.  errno = %d.", errno);
        PEG_METHOD_EXIT();
        throw Exception(MessageLoaderParms(
            "ProviderManager.OOPProviderManagerRouter.CIMPROVAGT_START_FAILED",
            "Failed to start cimprovagt \"$0\".",
            _moduleName));
    }
    else if (pid == 0)
    {
        //
        // Child side of the fork
        //

        try
        {
            // Close our copies of the parent's ends of the pipes
            pipeToAgent->closeWriteHandle();
            pipeFromAgent->closeReadHandle();

            //
            // Execute the cimprovagt program
            //
            String agentCommandPath =
                ConfigManager::getHomedPath(PEGASUS_PROVIDER_AGENT_PROC_NAME);
            CString agentCommandPathCString = agentCommandPath.getCString();

            char readHandle[32];
            char writeHandle[32];
            pipeToAgent->exportReadHandle(readHandle);
            pipeFromAgent->exportWriteHandle(writeHandle);

#ifndef PEGASUS_DISABLE_PROV_USERCTXT
            // Set the user context of the Provider Agent process
            if (_userName != System::getEffectiveUserName())
            {
                if (!System::changeUserContext(_userName.getCString()))
                {
                    Tracer::trace(TRC_DISCARDED_DATA, Tracer::LEVEL2,
                        "System::changeUserContext() failed.  userName = %s.",
                        (const char*)_userName.getCString());
                    Logger::put_l(Logger::ERROR_LOG, System::CIMSERVER,
                        Logger::WARNING,
                        "ProviderManager.OOPProviderManagerRouter."
                            "USER_CONTEXT_CHANGE_FAILED",
                        "Unable to change user context to \"$0\".", _userName);
                    _exit(1);
                }
            }
#endif

            execl(agentCommandPathCString, agentCommandPathCString,
                readHandle, writeHandle,
                (const char*)_moduleName.getCString(), (char*)0);

            // If we're still here, there was an error
            Tracer::trace(TRC_DISCARDED_DATA, Tracer::LEVEL2,
                "execl() failed.  errno = %d.", errno);
            _exit(1);
        }
        catch (...)
        {
            // There's not much we can do here in no man's land
            try
            {
                PEG_TRACE_STRING(TRC_DISCARDED_DATA, Tracer::LEVEL2,
                    "Caught exception before calling execl().");
            }
            catch (...) {}
            _exit(1);
        }
    }
# if defined(PEGASUS_HAS_SIGNALS)
    _pid = pid;
# endif
#endif

    //
    // CIM Server process
    //

    // Close our copies of the agent's ends of the pipes
    pipeToAgent->closeReadHandle();
    pipeFromAgent->closeWriteHandle();

    _pipeToAgent.reset(pipeToAgent.release());
    _pipeFromAgent.reset(pipeFromAgent.release());

    PEG_METHOD_EXIT();
}

// Note: Caller must lock _agentMutex
void ProviderAgentContainer::_sendInitializationData()
{
    PEG_METHOD_ENTER(TRC_PROVIDERMANAGER,
        "ProviderAgentContainer::_sendInitializationData");

    //
    // Gather config properties to pass to the Provider Agent
    //
    ConfigManager* configManager = ConfigManager::getInstance();
    Array<Pair<String, String> > configProperties;

    Array<String> configPropertyNames;
    configManager->getAllPropertyNames(configPropertyNames, true);
    for (Uint32 i = 0; i < configPropertyNames.size(); i++)
    {
        String configPropertyValue =
            configManager->getCurrentValue(configPropertyNames[i]);
        String configPropertyDefaultValue =
            configManager->getDefaultValue(configPropertyNames[i]);
        if (configPropertyValue != configPropertyDefaultValue)
        {
            configProperties.append(Pair<String, String>(
                configPropertyNames[i], configPropertyValue));
        }
    }

    //
    // Create a Provider Agent initialization message
    //
    AutoPtr<CIMInitializeProviderAgentRequestMessage> request(
        new CIMInitializeProviderAgentRequestMessage(
            String("0"),    // messageId
            configManager->getPegasusHome(),
            configProperties,
            System::bindVerbose,
            _subscriptionInitComplete,
            QueueIdStack()));

    //
    // Write the initialization message to the pipe
    //
    AnonymousPipe::Status writeStatus =
        _pipeToAgent->writeMessage(request.get());

    if (writeStatus != AnonymousPipe::STATUS_SUCCESS)
    {
        PEG_METHOD_EXIT();
        throw Exception(MessageLoaderParms(
            "ProviderManager.OOPProviderManagerRouter."
                "CIMPROVAGT_COMMUNICATION_FAILED",
            "Failed to communicate with cimprovagt \"$0\".",
            _moduleName));
    }

    // Do not wait for a response from the Provider Agent.  (It isn't coming.)

    PEG_METHOD_EXIT();
}

// Note: Caller must lock _agentMutex
void ProviderAgentContainer::_initialize()
{
    PEG_METHOD_ENTER(TRC_PROVIDERMANAGER,
        "ProviderAgentContainer::_initialize");

    if (_isInitialized)
    {
        PEGASUS_ASSERT(0);
        PEG_METHOD_EXIT();
        return;
    }

    if (_maxProviderProcesses == PEG_NOT_FOUND)
    {
        String maxProviderProcesses = ConfigManager::getInstance()->getCurrentValue("maxProviderProcesses");
        CString maxProviderProcessesString = maxProviderProcesses.getCString();
        char* end = 0;
        _maxProviderProcesses = strtol(maxProviderProcessesString, &end, 10);
    }

    {
        AutoMutex lock(_numProviderProcessesMutex);
        if ((_maxProviderProcesses != 0) &&
            (_numProviderProcesses >= _maxProviderProcesses))
        {
            throw PEGASUS_CIM_EXCEPTION(
                CIM_ERR_FAILED,
                MessageLoaderParms(
                    "ProviderManager.OOPProviderManagerRouter."
                        "MAX_PROVIDER_PROCESSES_REACHED",
                    "The maximum number of cimprovagt processes has been "
                        "reached."));
        }
        else
        {
            _numProviderProcesses++;
        }
    }

    try
    {
        _startAgentProcess();

        _isInitialized = true;

        _sendInitializationData();

        // Start a thread to read and process responses from the Provider Agent
        ThreadStatus rtn = PEGASUS_THREAD_OK;
        while ((rtn = MessageQueueService::get_thread_pool()->
                   allocate_and_awaken(this, _responseProcessor)) !=
               PEGASUS_THREAD_OK)
        {
            if (rtn == PEGASUS_THREAD_INSUFFICIENT_RESOURCES)
            {
                pegasus_yield();
            }
            else
            {
                Logger::put(
                    Logger::STANDARD_LOG, System::CIMSERVER, Logger::TRACE,
                    "Not enough threads to process responses from the "
                        "provider agent.");
 
                Tracer::trace(TRC_PROVIDERMANAGER, Tracer::LEVEL2,
                    "Could not allocate thread to process responses from the "
                        "provider agent.");

                throw Exception(MessageLoaderParms(
                    "ProviderManager.OOPProviderManagerRouter."
                        "CIMPROVAGT_THREAD_ALLOCATION_FAILED",
                    "Failed to allocate thread for cimprovagt \"$0\".",
                    _moduleName));
            }
        }
    }
    catch (...)
    {
        // Closing the connection causes the agent process to exit
        _pipeToAgent.reset();
        _pipeFromAgent.reset();

#if defined(PEGASUS_HAS_SIGNALS)
        if (_isInitialized)
        {
            // Harvest the status of the agent process to prevent a zombie
            Boolean keepWaiting = false;
            do
            {
                pid_t status = waitpid(_pid, 0, 0);
                if (status == -1)
                {
                    if (errno == EINTR)
                    {
                        keepWaiting = true;
                    }
                    else
                    {
                        Tracer::trace(TRC_DISCARDED_DATA, Tracer::LEVEL2,
                            "ProviderAgentContainer::_initialize(): "
                                "waitpid failed; errno = %d.", errno);
                    }
                }
            } while (keepWaiting);
        }
#endif

        _isInitialized = false;

        {
            AutoMutex lock(_numProviderProcessesMutex);
            _numProviderProcesses--;
        }

        PEG_METHOD_EXIT();
        throw;
    }

    PEG_METHOD_EXIT();
}

Boolean ProviderAgentContainer::isInitialized()
{
    AutoMutex lock(_agentMutex);
    return _isInitialized;
}

// Note: Caller must lock _agentMutex
void ProviderAgentContainer::_uninitialize()
{
    PEG_METHOD_ENTER(TRC_PROVIDERMANAGER,
        "ProviderAgentContainer::_uninitialize");

    if (!_isInitialized)
    {
        PEGASUS_ASSERT(0);
        PEG_METHOD_EXIT();
        return;
    }

    try
    {
        // Close the connection with the Provider Agent
        _pipeFromAgent.reset();
        _pipeToAgent.reset();

        _providerModuleCache = CIMInstance();

        {
            AutoMutex lock(_numProviderProcessesMutex);
            _numProviderProcesses--;
        }

#if defined(PEGASUS_HAS_SIGNALS)
        // Harvest the status of the agent process to prevent a zombie
        Boolean keepWaiting = false;
        do
        {
            pid_t status = waitpid(_pid, 0, 0);
            if (status == -1)
            {
                if (errno == EINTR)
                {
                    keepWaiting = true;
                }
                else
                {
                    Tracer::trace(TRC_DISCARDED_DATA, Tracer::LEVEL2,
                        "ProviderAgentContainer::_uninitialize(): "
                            "waitpid failed; errno = %d.", errno);
                }
            }
        } while (keepWaiting);
#endif

        _isInitialized = false;

        //
        // Complete with null responses all outstanding requests on this
        // connection
        //
        {
            AutoMutex tableLock(_outstandingRequestTableMutex);

            for (OutstandingRequestTable::Iterator i =
                     _outstandingRequestTable.start();
                 i != 0; i++)
            {
                PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL2,
                    String("Completing messageId \"") + i.value()->messageId +
                        "\" with a null response.");
                i.value()->responseMessage = 0;
                i.value()->responseReady->signal();
            }

            _outstandingRequestTable.clear();
        }
    }
    catch (...)
    {
        // We're uninitializing, so do not propagate the exception
        PEG_TRACE_STRING(TRC_DISCARDED_DATA, Tracer::LEVEL2,
            "Ignoring _uninitialize() exception.");
    }

    PEG_METHOD_EXIT();
}

String ProviderAgentContainer::getModuleName() const
{
    return _moduleName;
}

CIMResponseMessage* ProviderAgentContainer::processMessage(
    CIMRequestMessage* request)
{
    PEG_METHOD_ENTER(TRC_PROVIDERMANAGER,
        "ProviderAgentContainer::processMessage");

    CIMResponseMessage* response;
    String originalMessageId = request->messageId;

    // These three variables are used for the provider module optimization.
    // See the _providerModuleCache member description for more information.
    AutoPtr<ProviderIdContainer> origProviderId;
    Boolean doProviderModuleOptimization = false;
    Boolean updateProviderModuleCache = false;

    try
    {
        // The messageId attribute is used to correlate response messages
        // from the Provider Agent with request messages, so it is imperative
        // that the ID is unique for each request.  The incoming ID cannot be
        // trusted to be unique, so we substitute a unique one.  The memory
        // address of the request is used as the source of a unique piece of
        // data.  (The message ID is only required to be unique while the
        // request is outstanding.)
        char messagePtrString[20];
        sprintf(messagePtrString, "%p", request);
        String uniqueMessageId = messagePtrString;

        //
        // Set up the OutstandingRequestEntry for this request
        //
        Semaphore waitSemaphore(0);
        OutstandingRequestEntry outstandingRequestEntry(
            uniqueMessageId, response, &waitSemaphore);

        //
        // Lock the Provider Agent Container while initializing the
        // agent and writing the request to the connection
        //
        {
            AutoMutex lock(_agentMutex);

            //
            // Initialize the Provider Agent, if necessary
            //
            if (!_isInitialized)
            {
                _initialize();
            }

            //
            // Add an entry to the OutstandingRequestTable for this request
            //
            {
                AutoMutex tableLock(_outstandingRequestTableMutex);

                _outstandingRequestTable.insert(
                    uniqueMessageId, &outstandingRequestEntry);
            }

            // Get the provider module from the ProviderIdContainer to see if
            // we can optimize out the transmission of this instance to the
            // Provider Agent.  (See the _providerModuleCache description.)
            try
            {
                ProviderIdContainer pidc = request->operationContext.get(
                    ProviderIdContainer::NAME);
                origProviderId.reset(new ProviderIdContainer(
                    pidc.getModule(), pidc.getProvider(),
                    pidc.isRemoteNameSpace(), pidc.getRemoteInfo()));
                if (_providerModuleCache.isUninitialized() ||
                    (!pidc.getModule().identical(_providerModuleCache)))
                {
                    // We haven't sent this provider module instance to the
                    // Provider Agent yet.  Update our cache after we send it.
                    updateProviderModuleCache = true;
                }
                else
                {
                    // Replace the provider module in the ProviderIdContainer
                    // with an uninitialized instance.  We'll need to put the
                    // original one back after the message is sent.
                    request->operationContext.set(ProviderIdContainer(
                        CIMInstance(), pidc.getProvider(),
                        pidc.isRemoteNameSpace(), pidc.getRemoteInfo()));
                    doProviderModuleOptimization = true;
                }
            }
            catch (...)
            {
                // No ProviderIdContainer to optimize
            }

            //
            // Write the message to the pipe
            //
            try
            {
                PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL3,
                    String("Sending request to agent with messageId ") +
                        uniqueMessageId);

                request->messageId = uniqueMessageId;
                AnonymousPipe::Status writeStatus =
                    _pipeToAgent->writeMessage(request);
                request->messageId = originalMessageId;

                if (doProviderModuleOptimization)
                {
                    request->operationContext.set(*origProviderId.get());
                }

                if (writeStatus != AnonymousPipe::STATUS_SUCCESS)
                {
                    Tracer::trace(TRC_PROVIDERMANAGER, Tracer::LEVEL2,
                        "Failed to write message to pipe.  writeStatus = %d.",
                        writeStatus);
                    throw Exception(MessageLoaderParms(
                        "ProviderManager.OOPProviderManagerRouter."
                            "CIMPROVAGT_COMMUNICATION_FAILED",
                        "Failed to communicate with cimprovagt \"$0\".",
                        _moduleName));
                }

                if (updateProviderModuleCache)
                {
                    _providerModuleCache = origProviderId->getModule();
                }
            }
            catch (...)
            {
                request->messageId = originalMessageId;

                if (doProviderModuleOptimization)
                {
                    request->operationContext.set(*origProviderId.get());
                }

                Tracer::trace(TRC_PROVIDERMANAGER, Tracer::LEVEL2,
                    "Failed to write message to pipe.");
                // Remove the OutstandingRequestTable entry for this request
                {
                    AutoMutex tableLock(_outstandingRequestTableMutex);
                    Boolean removed =
                        _outstandingRequestTable.remove(uniqueMessageId);
                    PEGASUS_ASSERT(removed);
                }
                throw;
            }
        }

        //
        // Wait for the response
        //
        try
        {
            // Must not hold _agentMutex while waiting for the response
            waitSemaphore.wait();
        }
        catch (...)
        {
            // Remove the OutstandingRequestTable entry for this request
            {
                AutoMutex tableLock(_outstandingRequestTableMutex);
                Boolean removed =
                    _outstandingRequestTable.remove(uniqueMessageId);
                PEGASUS_ASSERT(removed);
            }
            throw;
        }

        // A null response is returned when an agent connection is closed
        // while requests remain outstanding.
        if (response == 0)
        {
            response = request->buildResponse();
            response->cimException = PEGASUS_CIM_EXCEPTION(
                CIM_ERR_FAILED,
                MessageLoaderParms(
                    "ProviderManager.OOPProviderManagerRouter."
                        "CIMPROVAGT_CONNECTION_LOST",
                    "Lost connection with cimprovagt \"$0\".",
                    _moduleName));
        }
    }
    catch (CIMException& e)
    {
        PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL2,
            String("Caught exception: ") + e.getMessage());
        response = request->buildResponse();
        response->cimException = e;
    }
    catch (Exception& e)
    {
        PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL2,
            String("Caught exception: ") + e.getMessage());
        response = request->buildResponse();
        response->cimException = PEGASUS_CIM_EXCEPTION(
            CIM_ERR_FAILED, e.getMessage());
    }
    catch (...)
    {
        PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL2,
            "Caught unknown exception");
        response = request->buildResponse();
        response->cimException = PEGASUS_CIM_EXCEPTION(
            CIM_ERR_FAILED, String::EMPTY);
    }

    response->messageId = originalMessageId;

    PEG_METHOD_EXIT();
    return response;
}

void ProviderAgentContainer::unloadIdleProviders()
{
    PEG_METHOD_ENTER(TRC_PROVIDERMANAGER,
        "ProviderAgentContainer::unloadIdleProviders");

    AutoMutex lock(_agentMutex);
    if (_isInitialized)
    {
        // Send a "wake up" message to the Provider Agent.
        // Don't bother checking whether the operation is successful.
        Uint32 messageLength = 0;
        _pipeToAgent->writeBuffer((const char*)&messageLength, sizeof(Uint32));
    }

    PEG_METHOD_EXIT();
}

void ProviderAgentContainer::_processResponses()
{
    PEG_METHOD_ENTER(TRC_PROVIDERMANAGER,
        "ProviderAgentContainer::_processResponses");

    //
    // Process responses until the pipe is closed
    //
    while (1)
    {
        try
        {
            CIMMessage* message;

            //
            // Read a response from the Provider Agent
            //
            AnonymousPipe::Status readStatus =
                _pipeFromAgent->readMessage(message);

            // Ignore interrupts
            if (readStatus == AnonymousPipe::STATUS_INTERRUPT)
            {
                continue;
            }

            // Handle an error the same way as a closed connection
            if ((readStatus == AnonymousPipe::STATUS_ERROR) ||
                (readStatus == AnonymousPipe::STATUS_CLOSED))
            {
                AutoMutex lock(_agentMutex);
                _uninitialize();
                return;
            }

            if (message->getType() == CIM_PROCESS_INDICATION_REQUEST_MESSAGE)
            {
                // Forward indications to the indication callback
                _indicationCallback(
                    reinterpret_cast<CIMProcessIndicationRequestMessage*>(
                        message));
            }
            else
            {
                CIMResponseMessage* response;
                response = dynamic_cast<CIMResponseMessage*>(message);
                PEGASUS_ASSERT(response != 0);

                // Give the response to the waiting OutstandingRequestEntry
                OutstandingRequestEntry* _outstandingRequestEntry = 0;
                {
                    AutoMutex tableLock(_outstandingRequestTableMutex);
                    Boolean foundEntry = _outstandingRequestTable.lookup(
                        response->messageId, _outstandingRequestEntry);
                    PEGASUS_ASSERT(foundEntry);

                    // Remove the completed request from the table
                    Boolean removed =
                        _outstandingRequestTable.remove(response->messageId);
                    PEGASUS_ASSERT(removed);
                }

                _outstandingRequestEntry->responseMessage = response;
                _outstandingRequestEntry->responseReady->signal();
            }
        }
        catch (Exception& e)
        {
            PEG_TRACE_STRING(TRC_DISCARDED_DATA, Tracer::LEVEL2,
                String("Ignoring exception: ") + e.getMessage());
        }
        catch (...)
        {
            PEG_TRACE_STRING(TRC_DISCARDED_DATA, Tracer::LEVEL2,
                "Ignoring exception");
        }
    }

}

PEGASUS_THREAD_RETURN PEGASUS_THREAD_CDECL
ProviderAgentContainer::_responseProcessor(void* arg)
{
    ProviderAgentContainer* pa =
        reinterpret_cast<ProviderAgentContainer*>(arg);

    pa->_processResponses();

    return(PEGASUS_THREAD_RETURN(0));
}

/////////////////////////////////////////////////////////////////////////////
// OOPProviderManagerRouter
/////////////////////////////////////////////////////////////////////////////

OOPProviderManagerRouter::OOPProviderManagerRouter(
    PEGASUS_INDICATION_CALLBACK indicationCallback)
{
    PEG_METHOD_ENTER(TRC_PROVIDERMANAGER,
        "OOPProviderManagerRouter::OOPProviderManagerRouter");

    _indicationCallback = indicationCallback;
    _subscriptionInitComplete = false;

    PEG_METHOD_EXIT();
}

OOPProviderManagerRouter::~OOPProviderManagerRouter()
{
    PEG_METHOD_ENTER(TRC_PROVIDERMANAGER,
        "OOPProviderManagerRouter::~OOPProviderManagerRouter");

    try
    {
        // Clean up the ProviderAgentContainers
        AutoMutex lock(_providerAgentTableMutex);
        ProviderAgentTable::Iterator i = _providerAgentTable.start();
        for(; i != 0; i++)
        {
            delete i.value();
        }
    }
    catch (...) {}

    PEG_METHOD_EXIT();
}

// Private, unimplemented constructor
OOPProviderManagerRouter::OOPProviderManagerRouter()
{
}

// Private, unimplemented constructor
OOPProviderManagerRouter::OOPProviderManagerRouter(
    const OOPProviderManagerRouter&)
    : ProviderManagerRouter(*this)
{
}

// Private, unimplemented assignment operator
OOPProviderManagerRouter& OOPProviderManagerRouter::operator=(
    const OOPProviderManagerRouter&)
{
    return *this;
}

Message* OOPProviderManagerRouter::processMessage(Message* message)
{
    PEG_METHOD_ENTER(TRC_PROVIDERMANAGER,
        "OOPProviderManagerRouter::processMessage");

    CIMRequestMessage* request = dynamic_cast<CIMRequestMessage *>(message);
    PEGASUS_ASSERT(request != 0);

    AutoPtr<CIMResponseMessage> response;

    //
    // Get the provider information from the request
    //
    CIMInstance providerModule;

    if ((dynamic_cast<CIMOperationRequestMessage*>(request) != 0) ||
        (dynamic_cast<CIMIndicationRequestMessage*>(request) != 0) ||
        (request->getType() == CIM_EXPORT_INDICATION_REQUEST_MESSAGE))
    {
        // Provider information is in the OperationContext
        ProviderIdContainer pidc = (ProviderIdContainer)
            request->operationContext.get(ProviderIdContainer::NAME);
        providerModule = pidc.getModule();
    }
    else if (request->getType() == CIM_ENABLE_MODULE_REQUEST_MESSAGE)
    {
        CIMEnableModuleRequestMessage* emReq =
            dynamic_cast<CIMEnableModuleRequestMessage*>(request);
        providerModule = emReq->providerModule;
    }
    else if (request->getType() == CIM_DISABLE_MODULE_REQUEST_MESSAGE)
    {
        CIMDisableModuleRequestMessage* dmReq =
            dynamic_cast<CIMDisableModuleRequestMessage*>(request);
        providerModule = dmReq->providerModule;
    }
    else if ((request->getType() == CIM_STOP_ALL_PROVIDERS_REQUEST_MESSAGE) ||
             (request->getType() ==
                 CIM_SUBSCRIPTION_INIT_COMPLETE_REQUEST_MESSAGE) ||
             (request->getType() == CIM_NOTIFY_CONFIG_CHANGE_REQUEST_MESSAGE))
    {
        // This operation is not provider-specific
    }
    else
    {
        // Unrecognized message type.  This should never happen.
        PEGASUS_ASSERT(0);
        response.reset(request->buildResponse());
        response->cimException = PEGASUS_CIM_EXCEPTION(
            CIM_ERR_FAILED, "Unrecognized message type.");
        PEG_METHOD_EXIT();
        return response.release();
    }

    //
    // Process the request message
    //
    if (request->getType() == CIM_STOP_ALL_PROVIDERS_REQUEST_MESSAGE)
    {
        // Forward the CIMStopAllProvidersRequest to all providers
        response.reset(_forwardRequestToAllAgents(request));

        // Note: Do not uninitialize the ProviderAgentContainers here.
        // Just let the selecting thread notice when the agent connections
        // are closed.
    }
    else if (request->getType () == 
        CIM_SUBSCRIPTION_INIT_COMPLETE_REQUEST_MESSAGE)
    {
        _subscriptionInitComplete = true;

        //
        //  Forward the CIMSubscriptionInitCompleteRequestMessage to 
        //  all providers
        //
        response.reset (_forwardRequestToAllAgents (request));
    }
    else if (request->getType() == CIM_NOTIFY_CONFIG_CHANGE_REQUEST_MESSAGE)
    {
        CIMNotifyConfigChangeRequestMessage* notifyRequest =
            dynamic_cast<CIMNotifyConfigChangeRequestMessage*>(request);
        PEGASUS_ASSERT(notifyRequest != 0);

        if (notifyRequest->currentValueModified)
        {
            // Forward the CIMNotifyConfigChangeRequestMessage to all providers
            response.reset(_forwardRequestToAllAgents(request));
        }
        else
        {
            // No need to notify provider agents about changes to planned value
            response.reset(request->buildResponse());
        }
    }
    else if (request->getType() == CIM_DISABLE_MODULE_REQUEST_MESSAGE)
    {
        // Fan out the request to all Provider Agent processes for this module

        // Retrieve the provider module name
        String moduleName;
        CIMValue nameValue = providerModule.getProperty(
            providerModule.findProperty("Name")).getValue();
        nameValue.get(moduleName);

        // Look up the Provider Agents for this module
        Array<ProviderAgentContainer*> paArray =
            _lookupProviderAgents(moduleName);

        for (Uint32 i=0; i<paArray.size(); i++)
        {
            //
            // Do not start up an agent process just to disable the module
            //
            if (paArray[i]->isInitialized())
            {
                //
                // Forward the request to the provider agent
                //
                response.reset(paArray[i]->processMessage(request));

                // Note: Do not uninitialize the ProviderAgentContainer here
                // when a disable module operation is successful.  Just let the
                // selecting thread notice when the agent connection is closed.

                // Determine the success of the disable module operation
                CIMDisableModuleResponseMessage* dmResponse =
                    dynamic_cast<CIMDisableModuleResponseMessage*>(
                        response.get());
                PEGASUS_ASSERT(dmResponse != 0);

                Boolean isStopped = false;
                for (Uint32 i=0; i < dmResponse->operationalStatus.size(); i++)
                {
                    if (dmResponse->operationalStatus[i] ==
                        CIM_MSE_OPSTATUS_VALUE_STOPPED)
                    {
                        isStopped = true;
                        break;
                    }
                }

                // If the operation is unsuccessful, stop and return the error
                if ((dmResponse->cimException.getCode() != CIM_ERR_SUCCESS) ||
                    !isStopped)
                {
                    break;
                }
            }
        }

        // Use a default response if no Provider Agents were called
        if (!response.get())
        {
            response.reset(request->buildResponse());

            CIMDisableModuleResponseMessage* dmResponse =
                dynamic_cast<CIMDisableModuleResponseMessage*>(response.get());
            PEGASUS_ASSERT(dmResponse != 0);

            Array<Uint16> operationalStatus;
            operationalStatus.append(CIM_MSE_OPSTATUS_VALUE_STOPPED);
            dmResponse->operationalStatus = operationalStatus;
        }
    }
    else if (request->getType() == CIM_ENABLE_MODULE_REQUEST_MESSAGE)
    {
        // Fan out the request to all Provider Agent processes for this module

        // Retrieve the provider module name
        String moduleName;
        CIMValue nameValue = providerModule.getProperty(
            providerModule.findProperty("Name")).getValue();
        nameValue.get(moduleName);

        // Look up the Provider Agents for this module
        Array<ProviderAgentContainer*> paArray =
            _lookupProviderAgents(moduleName);

        for (Uint32 i=0; i<paArray.size(); i++)
        {
            //
            // Do not start up an agent process just to enable the module
            //
            if (paArray[i]->isInitialized())
            {
                //
                // Forward the request to the provider agent
                //
                response.reset(paArray[i]->processMessage(request));

                // Determine the success of the enable module operation
                CIMEnableModuleResponseMessage* emResponse =
                    dynamic_cast<CIMEnableModuleResponseMessage*>(
                        response.get());
                PEGASUS_ASSERT(emResponse != 0);

                Boolean isOk = false;
                for (Uint32 i=0; i < emResponse->operationalStatus.size(); i++)
                {
                    if (emResponse->operationalStatus[i] ==
                        CIM_MSE_OPSTATUS_VALUE_OK)
                    {
                        isOk = true;
                        break;
                    }
                }

                // If the operation is unsuccessful, stop and return the error
                if ((emResponse->cimException.getCode() != CIM_ERR_SUCCESS) ||
                    !isOk)
                {
                    break;
                }
            }
        }

        // Use a default response if no Provider Agents were called
        if (!response.get())
        {
            response.reset(request->buildResponse());

            CIMEnableModuleResponseMessage* emResponse =
                dynamic_cast<CIMEnableModuleResponseMessage*>(response.get());
            PEGASUS_ASSERT(emResponse != 0);

            Array<Uint16> operationalStatus;
            operationalStatus.append(CIM_MSE_OPSTATUS_VALUE_OK);
            emResponse->operationalStatus = operationalStatus;
        }
    }
    else
    {
        // Retrieve the provider module name
        String moduleName;
        CIMValue nameValue = providerModule.getProperty(
            providerModule.findProperty("Name")).getValue();
        nameValue.get(moduleName);

        // Retrieve the provider user context configuration
        Uint16 userContext = 0;
        Uint32 pos = providerModule.findProperty(
            PEGASUS_PROPERTYNAME_MODULE_USERCONTEXT);
        if (pos != PEG_NOT_FOUND)
        {
            CIMValue userContextValue =
                providerModule.getProperty(pos).getValue();
            if (!userContextValue.isNull())
            {
                userContextValue.get(userContext);
            }
        }

        if (userContext == 0)
        {
            userContext = PG_PROVMODULE_USERCTXT_PRIVILEGED;
        }

        String userName;

        if (userContext == PG_PROVMODULE_USERCTXT_REQUESTOR)
        {
            try
            {
                // User Name is in the OperationContext
                IdentityContainer ic = (IdentityContainer)
                    request->operationContext.get(IdentityContainer::NAME);
                userName = ic.getUserName();
            }
            catch (Exception& e)
            {
                // If no IdentityContainer is present, default to the CIM
                // Server's user context
            }

            // If authentication is disabled, use the CIM Server's user context
            if (!userName.size())
            {
                userName = System::getEffectiveUserName();
            }
        }
        else if (userContext == PG_PROVMODULE_USERCTXT_DESIGNATED)
        {
            // Retrieve the provider module name
            providerModule.getProperty(providerModule.findProperty(
                PEGASUS_PROPERTYNAME_MODULE_DESIGNATEDUSER)).getValue().
                get(userName);
        }
        else if (userContext == PG_PROVMODULE_USERCTXT_CIMSERVER)
        {
            userName = System::getEffectiveUserName();
        }
        else    // Privileged User
        {
            PEGASUS_ASSERT(userContext == PG_PROVMODULE_USERCTXT_PRIVILEGED);
            userName = System::getPrivilegedUserName();
        }

        PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
            "Module name = " + moduleName);
        Tracer::trace(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
            "User context = %hd.", userContext);
        PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
            "User name = " + userName);

        // Look up the Provider Agent for this module and user
        ProviderAgentContainer* pa = _lookupProviderAgent(moduleName, userName);
        PEGASUS_ASSERT(pa != 0);

        //
        // Forward the request to the provider agent
        //
        response.reset(pa->processMessage(request));
    }

    response->syncAttributes(request);

    PEG_METHOD_EXIT();
    return response.release();
}

ProviderAgentContainer* OOPProviderManagerRouter::_lookupProviderAgent(
    const String& moduleName,
    const String& userName)
{
    ProviderAgentContainer* pa = 0;
    String key = moduleName + ":" + userName;

    AutoMutex lock(_providerAgentTableMutex);
    if (!_providerAgentTable.lookup(key, pa))
    {
        pa = new ProviderAgentContainer(
            moduleName, userName, _indicationCallback,
            _subscriptionInitComplete);
        _providerAgentTable.insert(key, pa);
    }
    return pa;
}

Array<ProviderAgentContainer*> OOPProviderManagerRouter::_lookupProviderAgents(
    const String& moduleName)
{
    Array<ProviderAgentContainer*> paArray;

    AutoMutex lock(_providerAgentTableMutex);
    for (ProviderAgentTable::Iterator i = _providerAgentTable.start(); i; i++)
    {
        if (i.value()->getModuleName() == moduleName)
        {
            paArray.append(i.value());
        }
    }
    return paArray;
}

CIMResponseMessage* OOPProviderManagerRouter::_forwardRequestToAllAgents(
    CIMRequestMessage* request)
{
    PEG_METHOD_ENTER(TRC_PROVIDERMANAGER,
        "OOPProviderManagerRouter::_forwardRequestToAllAgents");

    // Get a list of the ProviderAgentContainers.  We need our own array copy
    // because we cannot hold the _providerAgentTableMutex while calling
    // _ProviderAgentContainer::processMessage().
    Array<ProviderAgentContainer*> paContainerArray;
    {
        AutoMutex tableLock(_providerAgentTableMutex);
        for (ProviderAgentTable::Iterator i = _providerAgentTable.start();
             i != 0; i++)
        {
            paContainerArray.append(i.value());
        }
    }

    CIMException responseException;

    // Forward the request to each of the initialized provider agents
    for (Uint32 j = 0; j < paContainerArray.size(); j++)
    {
        ProviderAgentContainer* pa = paContainerArray[j];
        if (pa->isInitialized())
        {
            // Note: The ProviderAgentContainer could become uninitialized
            // before _ProviderAgentContainer::processMessage() processes
            // this request.  In this case, the Provider Agent process will
            // (unfortunately) be started to process this message.
            AutoPtr<CIMResponseMessage> response;
            response.reset(pa->processMessage(request));
            if (response.get() != 0)
            {
                // If the operation failed, save the exception data
                if ((response->cimException.getCode() != CIM_ERR_SUCCESS) &&
                    (responseException.getCode() == CIM_ERR_SUCCESS))
                {
                    responseException = response->cimException;
                }
            }
        }
    }

    CIMResponseMessage* response = request->buildResponse();
    response->cimException = responseException;

    PEG_METHOD_EXIT();
    return response;
}

Boolean OOPProviderManagerRouter::hasActiveProviders()
{
    PEG_METHOD_ENTER(TRC_PROVIDERMANAGER,
        "OOPProviderManagerRouter::hasActiveProviders");

    // Iterate through the _providerAgentTable looking for initialized agents
    AutoMutex lock(_providerAgentTableMutex);
    ProviderAgentTable::Iterator i = _providerAgentTable.start();
    for(; i != 0; i++)
    {
        if (i.value()->isInitialized())
        {
            PEG_METHOD_EXIT();
            return true;
        }
    }

    // No initialized Provider Agents were found
    PEG_METHOD_EXIT();
    return false;
}

void OOPProviderManagerRouter::unloadIdleProviders()
{
    PEG_METHOD_ENTER(TRC_PROVIDERMANAGER,
        "OOPProviderManagerRouter::unloadIdleProviders");

    // Iterate through the _providerAgentTable unloading idle providers
    AutoMutex lock(_providerAgentTableMutex);
    ProviderAgentTable::Iterator i = _providerAgentTable.start();
    for(; i != 0; i++)
    {
        i.value()->unloadIdleProviders();
    }

    PEG_METHOD_EXIT();
}

PEGASUS_NAMESPACE_END
