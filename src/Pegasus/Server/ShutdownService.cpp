//%////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001 BMC Software, Hewlett-Packard Company, IBM,
// The Open Group, Tivoli Systems
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
//=============================================================================
//
// Author: Jenny Yu, Hewlett-Packard Company (jenny_yu@hp.com)
//
// Modified By:
//
//%////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////////
//  ShutdownService
/////////////////////////////////////////////////////////////////////////////

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/Destroyer.h>
#include <Pegasus/Server/ShutdownExceptions.h>
#include <Pegasus/Server/CIMServerState.h>
#include <Pegasus/Server/ShutdownService.h>
#include <Pegasus/Common/XmlWriter.h>
#include <Pegasus/Common/Message.h>
#include <Pegasus/Common/CimomMessage.h>
#include <Pegasus/Common/CIMMessage.h>
#include <Pegasus/Common/MessageQueueService.h>
#include <Pegasus/Common/IPC.h>
#include <Pegasus/Common/Logger.h>

PEGASUS_USING_STD;

PEGASUS_NAMESPACE_BEGIN

/**
    The constant representing the shutdown timeout property name
*/
static String SHUTDOWN_TIMEOUT_PROPERTY = "shutdownTimeout";

/**
Initialize ShutdownService instance
*/
ShutdownService* ShutdownService::_instance = 0;

/**
Initialize all other class variables
*/
CIMServer*                       ShutdownService::_cimserver = 0;
Uint32                           ShutdownService::_shutdownTimeout = 0;
pegasus_internal_identity        ShutdownService::_id = 0;
ModuleController*                ShutdownService::_controller = 0;
ModuleController::client_handle* ShutdownService::_client_handle = 0;

/** Constructor. */
ShutdownService::ShutdownService(CIMServer* cimserver)
{
    _cimserver = cimserver;

    //
    // get client identify
    //
    _id = peg_credential_types::MODULE;

    //
    // get module controller
    //
    _controller = &(ModuleController::get_client_handle(_id, &_client_handle));
    if((_client_handle == NULL))
    {
        ThrowUninitializedHandle();
    }
}

/** Destructor. */
ShutdownService::~ShutdownService()
{
}

/**
    return a pointer to the ShutdownService instance.
*/
ShutdownService* ShutdownService::getInstance(CIMServer* cimserver)
{
    if (!_instance)
    {
        _instance = new ShutdownService(cimserver);
    }
    return _instance;
}

/**
    The shutdown method to be called by the ShutdownProvider to
    process a shutdown request from the CLI client.
*/
void ShutdownService::shutdown(Boolean force, Uint32 timeout)
{
    //
    // Initialize variables
    //
    Boolean timeoutExpired = false;
    Boolean noMoreRequests = false;

    try
    {
        //
        // set CIMServer state to TERMINATING
        //
        _cimserver->setState(CIMServerState::TERMINATING);

        //
        // Tell the CIMServer to stop accepting new client connection requests.
        //
        _cimserver->stopClientConnection();

        //
        // get shutdown timeout values
        //
        _initTimeoutValues(timeout);

        //
        // Determine if there are any outstanding CIM operation requests
        // (take into account that one of the request is the shutdown request).
        //
        Uint32 requestCount = _cimserver->getOutstandingRequestCount();

        if (requestCount > 1)
        {
            noMoreRequests = _waitUntilNoMoreRequests();
        }
        else
        {
            noMoreRequests = true;
        }

        //
        // proceed to shutdown the CIMServer
        //
        _shutdownCIMServer();
    }
    catch(CIMException & e)
    {
        Logger::put(Logger::STANDARD_LOG, "CIMServer", Logger::INFORMATION,
            "Error occured during CIMServer shutdown: $0.", 
            e.getMessage());
    }
    catch(Exception & e)
    {
        Logger::put(Logger::STANDARD_LOG, "CIMServer", Logger::INFORMATION,
            "Error occured during CIMServer shutdown: $0.", 
            e.getMessage());
    }

    //
    // All done
    //
    return;
}

/**********************************************************/
/*  callback                                              */
/**********************************************************/
void ShutdownService::async_callback(Uint32 user_data,
    Message *reply,
    void *parm)
{
   callback_data *cb_data = reinterpret_cast<callback_data *>(parm);
   cb_data->reply = reply;
   cb_data->client_sem.signal();
}

/**********************************************************/
/*  private methods                                       */
/**********************************************************/

void ShutdownService::_initTimeoutValues(Uint32 timeout)
{
    //
    // get an instance of the ConfigManager
    //
    ConfigManager*  configManager;
    configManager = ConfigManager::getInstance();

    //
    // if timeout was not specified, get timeout value from ConfigManager
    //
    if (timeout > 0)
    {
        _shutdownTimeout = timeout;
    }
    else
    {
        String configTimeout = 
            configManager->getCurrentValue(SHUTDOWN_TIMEOUT_PROPERTY);
        ArrayDestroyer<char> timeoutCString(configTimeout.allocateCString());
        _shutdownTimeout = strtol(timeoutCString.getPointer(), (char **)0,10);
    }

    return;
}

void ShutdownService::_shutdownCIMServer()
{
    //
    // Shutdown the Cimom services
    //
    _shutdownCimomServices();

    //
    // Shutdown the providers
    //
    _shutdownProviders();

    //
    // Tell CIMServer to shutdown completely.
    //
    _cimserver->shutdown();

    return;
}

void ShutdownService::_shutdownCimomServices()
{
    //
    // Shutdown the Indication Service
    //
    _sendShutdownRequestToService(PEGASUS_QUEUENAME_INDICATIONSERVICE);

    // Shutdown the Indication Handler Service
    _sendShutdownRequestToService(PEGASUS_QUEUENAME_INDHANDLERMANAGER);

    //
    // shutdown  Authenticator Delegator Service
    //
    _sendShutdownRequestToService(PEGASUS_QUEUENAME_HTTPAUTHDELEGATOR);

    //
    // shutdown  CIM Operation Request Authorizer Service
    //
    _sendShutdownRequestToService(PEGASUS_QUEUENAME_OPREQAUTHORIZER);

    //
    // shutdown  CIM Operation Request Decoder Service
    //
    _sendShutdownRequestToService(PEGASUS_QUEUENAME_OPREQDECODER);

    //
    // shutdown  CIM Operation Request Dispatcher Service
    //
    _sendShutdownRequestToService(PEGASUS_QUEUENAME_OPREQDISPATCHER);

    //
    // shutdown  CIM Export Request Decoder Service
    //
    _sendShutdownRequestToService(PEGASUS_QUEUENAME_EXPORTREQDECODER);

    //
    // shutdown  CIM Export Request Dispatcher Service
    //
    _sendShutdownRequestToService(PEGASUS_QUEUENAME_EXPORTREQDISPATCHER);

    return;
}

void ShutdownService::_sendShutdownRequestToService(const char * serviceName)
{
    MessageQueue* queue = 
        MessageQueue::lookup(serviceName);

    if (queue == 0)
    {
        // service not found, just return
        return;
    }

    MessageQueueService * _service = dynamic_cast<MessageQueueService *>(queue);
    Uint32 _queueId = _service->getQueueId();

    // 
    // create stop request
    //
    CimServiceStop *stopRequest = 
        new CimServiceStop(
                _service->get_next_xid(),    // routing
                NULL,                        // operation
                _queueId,                    // destination
                _controller->getQueueId(),   // response
                true);                       // blocking

// ATTN-JY-P2-05162002: call ClientSendWait, until asyn_callback is fixed

    //
    // Now send Stop request to service
    //
    AsyncMessage *reply  = _controller->ClientSendWait(*_client_handle,
                                                        _queueId,
                                                        stopRequest);

    delete stopRequest;

// ATTN-YZ-P2-05032002: Temporarily removed, until asyn_callback fixed
/*
    //
    // create callback data structure
    //
    callback_data *cb_data = new callback_data(this);

    //
    // Now send Stop request to service
    //
    if (false  == _controller->ClientSendAsync(*_client_handle,
                                               0,
                                               _queueId,
                                               stopRequest,
                                               ShutdownService::async_callback,
                                               (void *)cb_data) )
    {
        delete cb_data;
        throw CIMException(CIM_ERR_NOT_FOUND);
    }

    cb_data->client_sem.wait();
    AsyncReply * asyncReply = static_cast<AsyncReply *>(cb_data->get_reply()) ;
 
    //
    // check result
    //
    if (asyncReply != NULL)
    {
        if (asyncReply->result == async_results::CIM_STOPPED)
        {
            Logger::put(Logger::STANDARD_LOG, "CIMServer", Logger::INFORMATION,
                "Service $0 $1", serviceName, "stopped.");
        }
        else
        {
            Logger::put(Logger::STANDARD_LOG, "CIMServer", Logger::INFORMATION,
                "Failed to stop service $0.", serviceName);
        }
    }

    delete stopRequest;
    delete asyncReply;
    delete cb_data;
*/

    return;
}

void ShutdownService::_shutdownProviders()
{
    //
    // get provider manager service
    //
    MessageQueue * queue = 
        MessageQueue::lookup(PEGASUS_QUEUENAME_PROVIDERMANAGER_CPP);

    if (queue == 0)
        return;

    MessageQueueService * _service = dynamic_cast<MessageQueueService *>(queue);
    Uint32 _queueId = _service->getQueueId();

    // 
    // create stop all providers request
    //
    CIMStopAllProvidersRequestMessage * stopRequest =
            new CIMStopAllProvidersRequestMessage(
                XmlWriter::getNextMessageId (),
                QueueIdStack(_queueId));

    //
    // create async request message
    //
    AsyncLegacyOperationStart * asyncRequest =
        new AsyncLegacyOperationStart (
            _service->get_next_xid(),
            NULL,
            _queueId,
            stopRequest,
            _queueId);

// ATTN-JY-P2-05162002: call ClientSendWait, until asyn_callback is fixed

    AsyncReply * asyncReply = _controller->ClientSendWait(*_client_handle,
                                                          _queueId,
                                                          asyncRequest);
    CIMStopAllProvidersResponseMessage * response =
       reinterpret_cast<CIMStopAllProvidersResponseMessage *>(
         (static_cast<AsyncLegacyOperationResult *>(asyncReply))->get_result());

    if (response->cimException.getCode() != CIM_ERR_SUCCESS)
    {
        CIMException e = response->cimException;
        delete stopRequest;
        delete asyncRequest;
        delete asyncReply;
        delete response;
        throw (e);
    }

    //delete stopRequest;
    delete asyncRequest;
    delete asyncReply;
    delete response;

// ATTN-JY-P2-05162002: Comment out, until asyn_callback is fixed
/*
    //
    // create callback data structure
    //
    callback_data *cb_data = new callback_data(this);

    if (false  == _controller->ClientSendAsync(*_client_handle,
                                               0,
                                               _queueId,
                                               asyncRequest,
                                               ShutdownService::async_callback,
                                               (void *)cb_data) )
    {
        delete stopRequest;
        delete asyncRequest;
        delete cb_data;
        throw CIMException(CIM_ERR_NOT_FOUND);
    }

    cb_data->client_sem.wait();
    AsyncReply * asyncReply = static_cast<AsyncReply *>(cb_data->get_reply()) ;

    //
    // check result
    //
    if (asyncReply != NULL)
    {
        if (asyncReply->result == async_results::OK)
        {
            Logger::put(Logger::STANDARD_LOG, "CIMServer", Logger::INFORMATION,
                "Providers terminated successfully.");
        }
        else
        {
            Logger::put(Logger::STANDARD_LOG, "CIMServer", Logger::INFORMATION,
                "Error occured while terminating providers.");
        }

    }

    //delete stopRequest;
    delete asyncRequest;
    delete asyncReply;
    delete cb_data;
*/

    return;
}

Boolean ShutdownService::_waitUntilNoMoreRequests()
{
    Uint32 maxWaitTime = _shutdownTimeout;  // maximum wait time in seconds
    Uint32 waitInterval = 1;                // one second wait interval

    Uint32 requestCount = _cimserver->getOutstandingRequestCount();

    // create a mutex
    Mutex _mutex = Mutex();

    // create a condition 
    Condition _cond = Condition(_mutex);

    //
    // Loop and wait one second until either there is no more requests
    // or until timeout expires.
    //
    while ( requestCount > 1 && maxWaitTime > 0)
    {
         // lock the mutex
         _mutex.lock(pegasus_thread_self());

         try
         {
            _cond.unlocked_timed_wait(waitInterval*1000, pegasus_thread_self());
         }
         catch (TimeOut to)
         {
             requestCount = _cimserver->getOutstandingRequestCount();
             maxWaitTime = maxWaitTime - waitInterval;
         }
         catch (...)
         {
             maxWaitTime = 0;
         }

         // unlock the mutex
         _mutex.unlock();
    } 

    if (requestCount > 1)
        return false;
    else
        return true;
}

PEGASUS_NAMESPACE_END
