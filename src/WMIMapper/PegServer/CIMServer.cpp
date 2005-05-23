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
// Author: Mike Brasher (mbrasher@bmc.com)
//
// Modified By:
//         Mike Day (mdday@us.ibm.com)s
//         Nitin Upasani, Hewlett-Packard Company (Nitin_Upasani@hp.com)
//         Nag Boranna, Hewlett-Packard Company (nagaraja_boranna@hp.com)
//         Yi Zhou, Hewlett-Packard Company (yi_zhou@hp.com)
//         Jenny Yu, Hewlett-Packard Company (jenny_yu@hp.com)
//         Sushma Fernandes, Hewlett-Packard Company (sushma_fernandes@hp.com)
//         Roger Kumpf, Hewlett-Packard Company (roger_kumpf@hp.com)
//         Bapu Patil, Hewlett-Packard Company (bapu_patil@hp.com)
//		   Barbara Packard, Hewlett-Packard Company (barbara_packard@hp.com)
//		   Jair Santos, Hewlett-Packard Company (jair.santos@hp.com)
//         Dan Gorey, IBM  (djgorey@us.ibm.com)
//         Terry Martin, Hewlett-Packard Company (terry.martin@hp.com)
//         Amit K Arora, IBM (amita@in.ibm.com) for PEP#101
//         Mateus Baur, Hewlett-Packard Company (mateus.baur@hp.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#include <Pegasus/Common/Config.h>

#include <iostream>
#include <cstdio>
#include <cctype>
#include <ctime>
#include <Pegasus/Common/Constants.h>
#include <Pegasus/Common/FileSystem.h>
#include <Pegasus/Common/HTTPAcceptor.h>
#include <Pegasus/Common/Tracer.h>
#include <Pegasus/Common/Cimom.h>
#include <Pegasus/Common/PegasusVersion.h>
#include <Pegasus/Common/Signal.h>
#include <Pegasus/Common/SSLContextManager.h>

#include <Pegasus/Repository/CIMRepository.h>
//#include "ProviderMessageFacade.h"
#include <Pegasus/ExportServer/CIMExportRequestDispatcher.h>
#include <Pegasus/ExportServer/CIMExportResponseEncoder.h>
#include <Pegasus/ExportServer/CIMExportRequestDecoder.h>
#include <Pegasus/Config/ConfigManager.h>
#include <Pegasus/Security/UserManager/UserManager.h>
//#include <Pegasus/HandlerService/IndicationHandlerService.h>
//#include <Pegasus/IndicationService/IndicationService.h>
//#include <Pegasus/ProviderManager2/ProviderManagerService.h>


#include "CIMServer.h"
#include "CIMOperationRequestDispatcher.h"
#include "CIMOperationResponseEncoder.h"
#include "CIMOperationRequestDecoder.h"
#include "CIMOperationRequestAuthorizer.h"
#include "HTTPAuthenticatorDelegator.h"
/*
#include "ShutdownProvider.h"
#include "ShutdownService.h"
#include "BinaryMessageHandler.h"
#include <Pegasus/Common/ModuleController.h>
#include <Pegasus/ControlProviders/ConfigSettingProvider/ConfigSettingProvider.h>
#include <Pegasus/ControlProviders/UserAuthProvider/UserAuthProvider.h>
#include <Pegasus/ControlProviders/ProviderRegistrationProvider/ProviderRegistrationProvider.h>
#include <Pegasus/ControlProviders/NamespaceProvider/NamespaceProvider.h>
*/

// l10n
#include <Pegasus/Common/MessageLoader.h>

PEGASUS_USING_STD;

PEGASUS_NAMESPACE_BEGIN

/*
// Need a static method to act as a callback for the control provider.
// This doesn't belong here, but I don't have a better place to put it.
static Message * controlProviderReceiveMessageCallback(
    Message * message,
    void * instance)
{
    ProviderMessageFacade * mpf =
        reinterpret_cast<ProviderMessageFacade *>(instance);
    return mpf->handleRequestMessage(message);
}

void shutdownSignalHandler(int s_n, PEGASUS_SIGINFO_T * s_info, void * sig)
{
    PEG_METHOD_ENTER(TRC_SERVER, "shutdownSignalHandler");
    Tracer::trace(TRC_SERVER, Tracer::LEVEL2, "Signal %d received.", s_n);

    handleShutdownSignal = true;

    PEG_METHOD_EXIT();
}
*/
static CIMServer *_cimserver = NULL;
Boolean handleShutdownSignal = false;
void CIMServer::shutdownSignal()
{
    PEG_METHOD_ENTER(TRC_SERVER, "CIMServer::shutdownSignal()");
    handleShutdownSignal = true;
	_cimserver->tickle_monitor();
    PEG_METHOD_EXIT();
}


CIMServer::CIMServer(Monitor* monitor)
  : _dieNow(false), _monitor(monitor)
{
    PEG_METHOD_ENTER(TRC_SERVER, "CIMServer::CIMServer()");
    _init();
	_cimserver = this;
    PEG_METHOD_EXIT();
}


void CIMServer::tickle_monitor(){
    _monitor->tickle();
}
void CIMServer::_init(void)
{

    String repositoryRootPath = String::EMPTY;


#if defined(PEGASUS_OS_HPUX) && defined(PEGASUS_USE_RELEASE_DIRS)
    chdir( PEGASUS_CORE_DIR );
#endif
    // -- Save the monitor or create a new one:
    repositoryRootPath =
            ConfigManager::getHomedPath(ConfigManager::getInstance()->getCurrentValue("repositoryDir"));

    // -- Create a repository:

/*
#ifdef DO_NOT_CREATE_REPOSITORY_ON_STARTUP
    // If this code is enable, the CIMServer will fail to start
    // if the repository directory does not exit. If called,
    // the Repository will create an empty repository.

    // This check has been disabled to allow cimmof to call
    // the CIMServer to build the initial repository.
    if (!FileSystem::isDirectory(repositoryRootPath))
    {
        PEG_METHOD_EXIT();
        throw NoSuchDirectory(repositoryRootPath);

    }
#endif
*/
    _repository = new CIMRepository(repositoryRootPath);

    // -- Create a UserManager object:

    UserManager* userManager = UserManager::getInstance(_repository);

    // -- Create a CIMServerState object:

    _serverState.reset(new CIMServerState());

/*
    _providerRegistrationManager = new ProviderRegistrationManager(_repository);

    // -- Create queue inter-connections:

    _providerManager = new ProviderManagerService(_providerRegistrationManager,_repository);

    _handlerService = new IndicationHandlerService(_repository);

    // Create the control service
    _controlService = new ModuleController(PEGASUS_QUEUENAME_CONTROLSERVICE);

    // Create the Configuration control provider
    ProviderMessageFacade * configProvider =
        new ProviderMessageFacade(new ConfigSettingProvider());
    ModuleController::register_module(PEGASUS_QUEUENAME_CONTROLSERVICE,
                                      PEGASUS_MODULENAME_CONFIGPROVIDER,
                                      configProvider,
                                      controlProviderReceiveMessageCallback,
                                      0, 0);

    // Create the User/Authorization control provider
    ProviderMessageFacade * userAuthProvider =
        new ProviderMessageFacade(new UserAuthProvider(_repository));
    ModuleController::register_module(PEGASUS_QUEUENAME_CONTROLSERVICE,
                                      PEGASUS_MODULENAME_USERAUTHPROVIDER,
                                      userAuthProvider,
                                      controlProviderReceiveMessageCallback,
                                      0, 0);

    // Create the Provider Registration control provider
    ProviderMessageFacade * provRegProvider = new ProviderMessageFacade(
        new ProviderRegistrationProvider(_providerRegistrationManager));
    ModuleController::register_module(PEGASUS_QUEUENAME_CONTROLSERVICE,
                                      PEGASUS_MODULENAME_PROVREGPROVIDER,
                                      provRegProvider,
                                      controlProviderReceiveMessageCallback,
                                      0, 0);

     // Create the Shutdown control provider
     ProviderMessageFacade * shutdownProvider =
         new ProviderMessageFacade(new ShutdownProvider(this));
     ModuleController::register_module(PEGASUS_QUEUENAME_CONTROLSERVICE,
                                       PEGASUS_MODULENAME_SHUTDOWNPROVIDER,
                                       shutdownProvider,
                                       controlProviderReceiveMessageCallback,
                                       0, 0);

     // Create the namespace control provider
     ProviderMessageFacade * namespaceProvider =
         new ProviderMessageFacade(new NamespaceProvider(_repository));
     ModuleController::register_module(PEGASUS_QUEUENAME_CONTROLSERVICE,
                                       PEGASUS_MODULENAME_NAMESPACEPROVIDER,
                                       namespaceProvider,
                                       controlProviderReceiveMessageCallback,
                                       0, 0);

    _cimOperationRequestDispatcher
        = new CIMOperationRequestDispatcher(_repository,
                                            _providerRegistrationManager);
    _binaryMessageHandler =
       new BinaryMessageHandler(_cimOperationRequestDispatcher);
*/

     //
     // Create a SSLContextManager object
     //
     _sslContextMgr = new SSLContextManager();

    _cimOperationRequestDispatcher = 
		new CIMOperationRequestDispatcher;
	
	_cimOperationResponseEncoder
        = new CIMOperationResponseEncoder;

    //
    // get the configured authentication and authorization flags
    //
    ConfigManager* configManager = ConfigManager::getInstance();

    Boolean enableAuthentication = false;

    if (String::equalNoCase(
        configManager->getCurrentValue("enableAuthentication"), "true"))
    {
        enableAuthentication = true;
    }

    //
    // Create Authorization queue only if authentication is enabled
    //
    if ( enableAuthentication )
    {
        _cimOperationRequestAuthorizer = new CIMOperationRequestAuthorizer(
            _cimOperationRequestDispatcher);

        _cimOperationRequestDecoder = new CIMOperationRequestDecoder(
            _cimOperationRequestAuthorizer,
            _cimOperationResponseEncoder->getQueueId());
    }
    else
    {
        _cimOperationRequestAuthorizer = 0;

        _cimOperationRequestDecoder = new CIMOperationRequestDecoder(
            _cimOperationRequestDispatcher,
            _cimOperationResponseEncoder->getQueueId());

    }

    _cimExportRequestDispatcher
        = new CIMExportRequestDispatcher();

    _cimExportResponseEncoder
        = new CIMExportResponseEncoder;

    _cimExportRequestDecoder = new CIMExportRequestDecoder(
        _cimExportRequestDispatcher,
        _cimExportResponseEncoder->getQueueId());

    _httpAuthenticatorDelegator = new HTTPAuthenticatorDelegator(
        _cimOperationRequestDecoder->getQueueId(),
        _cimExportRequestDecoder->getQueueId(), 
		_repository);

    // IMPORTANT-NU-20020513: Indication service must start after ExportService
    // otherwise HandlerService started by indicationService will never
    // get ExportQueue to export indications for existing subscriptions

/*
    _indicationService = 0;
    if (String::equal(
        configManager->getCurrentValue("enableIndicationService"), "true"))
    {
        _indicationService = new IndicationService
            (_repository, _providerRegistrationManager);
    }

    // Enable the signal handler to shutdown gracefully on SIGHUP and SIGTERM
    getSigHandle()->registerHandler(PEGASUS_SIGHUP, shutdownSignalHandler);
    getSigHandle()->activate(PEGASUS_SIGHUP);
    getSigHandle()->registerHandler(PEGASUS_SIGTERM, shutdownSignalHandler);
    getSigHandle()->activate(PEGASUS_SIGTERM);
*/
}



CIMServer::~CIMServer()
{
    PEG_METHOD_ENTER(TRC_SERVER, "CIMServer::~CIMServer()");

    // Start deleting the objects. 
     for (Uint32 i = 0, n = _acceptors.size (); i < n; i++) 
       { 
         HTTPAcceptor *p = _acceptors[i]; 
         delete p; 
       } 
     // The order is very important. 
    
     // 13 
     /*if (_indicationService) 
       { 
         delete _indicationService; 
       } */
    
     // 12 
     // HTTPAuthenticationDelegaor depends on 
     // CIMRepository, CIMOperationRequestDecode and 
     // CIMExportRequestDecoder 
     if (_httpAuthenticatorDelegator) 
       { 
         delete _httpAuthenticatorDelegator; 
       } 
     // 11 
     if (_cimExportRequestDecoder) 
       { 
         delete _cimExportRequestDecoder; 
       } 
     // 10 
     if (_cimExportResponseEncoder) 
       { 
         delete _cimExportResponseEncoder; 
       } 
     // 9 
     if (_cimExportRequestDispatcher) 
       { 
         delete _cimExportRequestDispatcher; 
       } 
     // 8 
     // CIMOperationRequestDecoder depends on CIMOperationRequestAuthorizer 
     // and CIMOperationResponseEncoder 
     if (_cimOperationRequestDecoder) 
       { 
         delete _cimOperationRequestDecoder; 
       } 
     // 7 
     if (_cimOperationResponseEncoder) 
       { 
         delete _cimOperationResponseEncoder; 
       } 
     // BinaryMessageHandler depends on CIMOperationRequestDispatcher 
     /*if (_binaryMessageHandler)    //6 
       { 
         delete _binaryMessageHandler; 
       } */
     // CIMOperationRequestAuthorizer depends on 
     // CIMOperationRequestDispatcher 
     if (_cimOperationRequestAuthorizer) 
       { 
         delete _cimOperationRequestAuthorizer; 
       } 
     // IndicationHandlerService , 3. It uses CIMOperationRequestDispatcher 
     /*if (_handlerService) 
       { 
         delete _handlerService; 
       } */
     // CIMOperationRequestDispatcher depends on 
     // CIMRepository and ProviderRegistrationManager 
     if (_cimOperationRequestDispatcher) 
       { 
         // Keeps an internal list of control providers. Must 
         // delete this before ModuleController. 
         delete _cimOperationRequestDispatcher; 
       } 
     // ProviderManager depends on ProcviderRegistrationManager 
     // 5 
     /*if (_providerManager) 
       { 
         delete _providerManager; 
       } 
     // 4 
     if (_controlService) 
       { 
         // ModuleController takes care of deleting all wrappers around 
         // the control providers. 
         delete _controlService; 
       } 

	// IndicationHandlerService, and ProviderRegistrationManager, and thus should be
	// deleted before the ProviderManagerService, IndicationHandlerService, and 
	// ProviderRegistrationManager are deleted. 
	 if (_providerRegistrationManager)
	 {
		 delete _providerRegistrationManager;
	 }

	 // Find all of the control providers (module) 
     // Must delete CIMOperationRequestDispatcher _before_ deleting each 
     // of the control provider. The CIMOperationRequestDispatcher keeps 
     // its own table of the internal providers (pointers). 
     for (Uint32 i = 0, n = _controlProviders.size (); i < n; i++) 
       { 
         ProviderMessageFacade *p = _controlProviders[i]; 
         // The ~ProviderMessageFacade calls 'terminate' on the control providers. 
         delete p; 
       } */
     // The SSL control providers used the SSL context manager. 
     if (_sslContextMgr) 
       { 
         delete _sslContextMgr; 
         _sslContextMgr = 0; 
       } 
     // ConfigManager. Really weird way of doing it. 
     ConfigManager *configManager = ConfigManager::getInstance (); 
     if (configManager) 
       { 
         // Refer to Bug #3537. It will soon have a fix. 
         //delete configManager; 
       } 
     UserManager *userManager = UserManager::getInstance (_repository); 
     if (userManager) 
       { 
         // Bug #3537 will soon fix this 
         //delete userManager; 
       } 
     // Lastly the repository. 
     if (_repository) 
       { 
         delete _repository; 
       } 

    PEG_METHOD_EXIT();
}

void CIMServer::addAcceptor(
    Boolean localConnection,
    Uint32 portNumber,
    Boolean useSSL,
    Boolean exportConnection)
{
    HTTPAcceptor* acceptor;
    if (exportConnection)
    {
      //
      // On export connection, create SSLContext with a indication
      // trust store.
      //
      acceptor = new HTTPAcceptor(
          _monitor,
          _httpAuthenticatorDelegator,
          localConnection,
          portNumber,
          useSSL ? _getSSLContext(SSLContextManager::EXPORT_CONTEXT) : 0,
          exportConnection,
          useSSL ? _sslContextMgr->getSSLContextObjectLock() : 0 );
    }
    else
    {
      acceptor = new HTTPAcceptor(
          _monitor,
          _httpAuthenticatorDelegator,
          localConnection,
          portNumber,
          useSSL ? _getSSLContext(SSLContextManager::SERVER_CONTEXT) : 0,
          exportConnection,
          useSSL ? _sslContextMgr->getSSLContextObjectLock() : 0 );
    }
    _acceptors.append(acceptor);  
}

void CIMServer::bind()
{
    PEG_METHOD_ENTER(TRC_SERVER, "CIMServer::bind()");

    if (_acceptors.size() == 0)
    {
        MessageLoaderParms mlp = MessageLoaderParms(
            "Server.CIMServer.BIND_FAILED",
            "No CIM Server connections are enabled.");

        throw BindFailedException(mlp);
    }

	for (Uint32 i=0; i<_acceptors.size(); i++)
	{
		_acceptors[i]->bind();
	}
    
    PEG_METHOD_EXIT();
}

void CIMServer::runForever()
{ 
    // Note: Trace code in this method will be invoked frequently.

    if(!_dieNow)
	{
    
		
	if(false == _monitor->run(500000))
    {
      try
      {
        MessageQueueService::_check_idle_flag = 1;
        MessageQueueService::_polling_sem.signal();
        
		//NOT USED ON WMI MAPPER
        //_providerManager->unloadIdleProviders();
      }
      catch(...)
      {
      }
    }

/*
        if (handleShutdownSignal)
          {
            Tracer::trace(TRC_SERVER, Tracer::LEVEL3,
                          "CIMServer::runForever - signal received.  Shutting down.");
        
            ShutdownService::getInstance(this)->shutdown(true, 10, false);
            handleShutdownSignal = false;
          }
*/
      }  
}


void CIMServer::stopClientConnection()
{
    PEG_METHOD_ENTER(TRC_SERVER, "CIMServer::stopClientConnection()");

    // tell Monitor to stop listening for client connections
    _monitor->stopListeningForConnections(false);

    //
    // Wait 150 milliseconds to allow time for the Monitor to stop 
    // listening for client connections.  
    //
    // This wait time is the timeout value for the select() call
    // in the Monitor's run() method (currently set to 100 
    // milliseconds) plus a delta of 50 milliseconds.  The reason
    // for the wait here is to make sure that the Monitor entries
    // are updated before closing the connection sockets.
    //
    // PEG_TRACE_STRING(TRC_SERVER, Tracer::LEVEL4, "Wait 150 milliseconds.");
    //  pegasus_sleep(150);  not needed anymore due to the semaphore
    // in the monitor

    for (Uint32 i=0; i<_acceptors.size(); i++)
    {
		_acceptors[i]->closeConnectionSocket();
    }
    
    PEG_METHOD_EXIT();
}

void CIMServer::shutdown()
{
    PEG_METHOD_ENTER(TRC_SERVER, "CIMServer::shutdown()");

    _dieNow = true;
	_cimserver->tickle_monitor();

    PEG_METHOD_EXIT();
}

void CIMServer::resume()
{
    PEG_METHOD_ENTER(TRC_SERVER, "CIMServer::resume()");

    for (Uint32 i=0; i<_acceptors.size(); i++)
    {
        _acceptors[i]->reopenConnectionSocket();
    }

    PEG_METHOD_EXIT();
}

void CIMServer::setState(Uint32 state)
{
    PEG_METHOD_ENTER(TRC_SERVER, "CIMServer::setState()");

    _serverState->setState(state);

    //
    // get the configured authentication and authorization flags
    //
    ConfigManager* configManager = ConfigManager::getInstance();

    Boolean enableAuthentication = false;
    Boolean enableNamespaceAuthorization = false;

    if (String::equal(
        configManager->getCurrentValue("enableAuthentication"), "true"))
    {
        enableAuthentication = true;
    }

    if (String::equal(
        configManager->getCurrentValue("enableNamespaceAuthorization"), "true"))
    {
        enableNamespaceAuthorization = true;
    }

    if (state == CIMServerState::TERMINATING)
    {
        // tell decoder that CIMServer is terminating
        _cimOperationRequestDecoder->setServerTerminating(true);
        _cimExportRequestDecoder->setServerTerminating(true);

        // tell authorizer that CIMServer is terminating ONLY if
        // authentication and authorization are enabled
        //
        if ( enableAuthentication && enableNamespaceAuthorization )
        {
            _cimOperationRequestAuthorizer->setServerTerminating(true);
        }
    }
    else
    {
        // tell decoder that CIMServer is not terminating
        _cimOperationRequestDecoder->setServerTerminating(false);
        _cimExportRequestDecoder->setServerTerminating(false);

        // tell authorizer that CIMServer is terminating ONLY if
        // authentication and authorization are enabled
        //
        if ( enableAuthentication && enableNamespaceAuthorization )
        {
            _cimOperationRequestAuthorizer->setServerTerminating(false);
        }
    }
    PEG_METHOD_EXIT();
}

Uint32 CIMServer::getOutstandingRequestCount()
{
    PEG_METHOD_ENTER(TRC_SERVER, "CIMServer::getOutstandingRequestCount()");

    Uint32 requestCount = 0;
    
	for (Uint32 i=0; i<_acceptors.size(); i++)
	{
		requestCount += _acceptors[i]->getOutstandingRequestCount();
	}

    PEG_METHOD_EXIT();
    return requestCount;
}

SSLContext* CIMServer::_getSSLContext(Uint32 contextType)
{
    PEG_METHOD_ENTER(TRC_SERVER, "CIMServer::_getSSLContext()");

    static const String PROPERTY_NAME__SSL_CERT_FILEPATH =
                                           "sslCertificateFilePath";
    static const String PROPERTY_NAME__SSL_KEY_FILEPATH  = "sslKeyFilePath";
    static const String PROPERTY_NAME__SSL_TRUST_STORE  = "sslTrustStore";
    static const String PROPERTY_NAME__EXPORT_SSL_TRUST_STORE =
                                           "exportSSLTrustStore";
    static const String PROPERTY_NAME__SSL_CRL_STORE  = "crlStore";
    static const String PROPERTY_NAME__SSL_CLIENT_VERIFICATION =
                                           "sslClientVerificationMode";
    static const String PROPERTY_NAME__SSL_AUTO_TRUST_STORE_UPDATE =
                                           "enableSSLTrustStoreAutoUpdate";
    static const String PROPERTY_NAME__SSL_TRUST_STORE_USERNAME =
                                           "sslTrustStoreUserName";
    static const String PROPERTY_NAME__HTTP_ENABLED =
                                           "enableHttpConnection";

    String verifyClient = String::EMPTY;
    String trustStore = String::EMPTY;
    String exportTrustStore = String::EMPTY;
    SSLContext* sslContext = 0;

    //
    // Get a config manager instance
    //
    ConfigManager* configManager = ConfigManager::getInstance();

    if ( contextType == SSLContextManager::SERVER_CONTEXT )
    {
        // Note that if invalid values were set for either sslKeyFilePath,
        // sslCertificateFilePath, crlStore or sslTrustStore, the invalid
        // paths would have been detected in SecurityPropertyOwner and
        // terminated the server startup. This happens regardless of whether
        // or not HTTPS is enabled (not a great design, but that seems to
        // be how other properties are validated as well)
        //
        // Get the sslClientVerificationMode property from the Config
        // Manager.
        //
        verifyClient = configManager->getCurrentValue(
                                  PROPERTY_NAME__SSL_CLIENT_VERIFICATION);

        //
        // Get the sslTrustStore property from the Config Manager.
        //
        trustStore = configManager->getCurrentValue(
                                 PROPERTY_NAME__SSL_TRUST_STORE);

        if (trustStore != String::EMPTY)
        {
            trustStore = ConfigManager::getHomedPath(trustStore);
        }

        PEG_TRACE_STRING(TRC_SERVER, Tracer::LEVEL4,
            "Server trust store name: " + trustStore);

        //
        // Get the sslTrustStoreUserName property from the Config Manager.
        //
        String trustStoreUserName = String::EMPTY;
        trustStoreUserName = configManager->getCurrentValue(
                                  PROPERTY_NAME__SSL_TRUST_STORE_USERNAME);

        if (!String::equal(verifyClient, "disabled"))
        {
            //
            // 'required' setting must have a valid truststore
            // 'optional' setting can be used with or without a truststore;
            // log a warning if a truststore is not specified
            //
            if (trustStore == String::EMPTY)
            {
                if (String::equal(verifyClient, "required"))
                {
                    MessageLoaderParms parms(
                        "Pegasus.Server.CIMServer.SSL_CLIENT_VERIFICATION_EMPTY_TRUSTSTORE",
                        "The \"sslTrustStore\" configuration property must be set if \"sslClientVerificationMode\" is 'required'. cimserver not started.");
                    PEG_METHOD_EXIT();
                    throw SSLException(parms);
                }
                else if (String::equal(verifyClient, "optional"))
                {
                    Logger::put(Logger::STANDARD_LOG, System::CIMSERVER,
                        Logger::WARNING,
                        "SSL client verification is enabled but no truststore was specified.");
                }
            }
            //
            // ATTN: 'required' setting must have http port enabled.
            // If only https is enabled, and a call to shutdown the
            // cimserver is given, the call will hang and a forced shutdown
            // will ensue. This is because the CIMClient::connectLocal call
            // cannot (and should not) specify a truststore to validate the
            // local server against.  This limitation is being investigated.
            //
            if (String::equal(verifyClient, "required"))
            {
                String httpEnabled = configManager->getCurrentValue(
                                              PROPERTY_NAME__HTTP_ENABLED);

                if (!String::equal(httpEnabled, "true"))
                {
                    MessageLoaderParms parms(
                        "Pegasus.Server.CIMServer.SSL_CLIENT_VERIFICATION_HTTP_NOT_ENABLED_WITH_REQUIRED",
                        "The HTTP port must be enabled if \"sslClientVerificationMode\" is 'required' in order for the cimserver to properly shutdown. cimserver not started.");
                    PEG_METHOD_EXIT();
                    throw SSLException(parms);
                }
            }

            //
            // A truststore username must be specified if
            // sslClientVerificationMode is enabled and the truststore is a
            // single CA file.  If the truststore is a directory, then the
            // CertificateProvider should be used to register users with
            // certificates.
            //
            if ((trustStore != String::EMPTY) &&
                (!FileSystem::isDirectory(trustStore)))
            {
                if (trustStoreUserName == String::EMPTY)
                {
                    MessageLoaderParms parms(
                        "Pegasus.Server.CIMServer.SSL_CLIENT_VERIFICATION_EMPTY_USERNAME",
                        "The \"sslTrustStoreUserName\" property must specify a valid username if \"sslClientVerificationMode\" is 'required' or 'optional' and the truststore is a single CA file. To register individual certificates to users, you must use a truststore directory along with the CertificateProvider.  cimserver not started.");
                    PEG_METHOD_EXIT();
                    throw SSLException(parms);
                }
            }
        }
    }
    else if ( contextType == SSLContextManager::EXPORT_CONTEXT )
    {
        //
        // Get the exportSSLTrustStore property from the Config Manager.
        //
        exportTrustStore = configManager->getCurrentValue(
                                  PROPERTY_NAME__EXPORT_SSL_TRUST_STORE);

        if (exportTrustStore == String::EMPTY)
        {
            MessageLoaderParms parms(
                "Pegasus.Server.CIMServer.EXPORT_TRUST_EMPTY",
                "The \"exportSSLTrustStore\" configuration property must be set when \"enableSSLExportClientVerification\" is true. cimserver not started.");

            PEG_METHOD_EXIT();
            throw Exception(parms);
        }

        exportTrustStore = ConfigManager::getHomedPath(exportTrustStore);

        PEG_TRACE_STRING(TRC_SERVER, Tracer::LEVEL4,
            "Export trust store name: " + exportTrustStore);
    }

    //
    // Get the crlStore property from the Config Manager.
    //
    String crlStore = configManager->getCurrentValue(
                               PROPERTY_NAME__SSL_CRL_STORE);

    if (crlStore != String::EMPTY)
    {
        crlStore = ConfigManager::getHomedPath(crlStore);
    }

    //
    // Get the sslCertificateFilePath property from the Config Manager.
    //
    String certPath;
    certPath = ConfigManager::getHomedPath(
        configManager->getCurrentValue(PROPERTY_NAME__SSL_CERT_FILEPATH));

    //
    // Get the sslKeyFilePath property from the Config Manager.
    //
    String keyPath;
    keyPath = ConfigManager::getHomedPath(
        configManager->getCurrentValue(PROPERTY_NAME__SSL_KEY_FILEPATH));

    String randFile = String::EMPTY;

#ifdef PEGASUS_SSL_RANDOMFILE
    // NOTE: It is technically not necessary to set up a random file on
    // the server side, but it is easier to use a consistent interface
    // on the client and server than to optimize out the random file on
    // the server side.
    randFile = ConfigManager::getHomedPath(PEGASUS_SSLSERVER_RANDOMFILE);
#endif

    if ( contextType == SSLContextManager::SERVER_CONTEXT )
    {
        //
        // Create the SSLContext defined by the configuration properties
        //
        if (String::equal(verifyClient, "required"))
        {
            PEG_TRACE_STRING(TRC_SERVER, Tracer::LEVEL2,
                "SSL Client verification REQUIRED.");

            _sslContextMgr->createSSLContext(SSLContextManager::SERVER_CONTEXT,
                trustStore, certPath, keyPath, crlStore, false, randFile);
        }
        else if (String::equal(verifyClient, "optional"))
        {
            PEG_TRACE_STRING(TRC_SERVER, Tracer::LEVEL2,
                "SSL Client verification OPTIONAL.");

            _sslContextMgr->createSSLContext(SSLContextManager::SERVER_CONTEXT,
                trustStore, certPath, keyPath, crlStore, true, randFile);
        }
        else if (String::equal(verifyClient, "disabled") ||
                 verifyClient == String::EMPTY)
        {
            PEG_TRACE_STRING(TRC_SERVER, Tracer::LEVEL2,
                "SSL Client verification DISABLED.");

            _sslContextMgr->createSSLContext(SSLContextManager::SERVER_CONTEXT,
                String::EMPTY, certPath, keyPath, crlStore, false, randFile);
        }
        sslContext = _sslContextMgr->getSSLContext(SSLContextManager::SERVER_CONTEXT);
    }
    else if ( contextType == SSLContextManager::EXPORT_CONTEXT )
    {
        //
        // Note: Trust store is used by default on Export connections,
        // verification callback function is not used.
        //
        _sslContextMgr->createSSLContext(SSLContextManager::EXPORT_CONTEXT,
            exportTrustStore, certPath, keyPath, crlStore, false, randFile);

        sslContext = _sslContextMgr->getSSLContext(SSLContextManager::EXPORT_CONTEXT);
    }

    PEG_METHOD_EXIT();
    return sslContext;
}

PEGASUS_NAMESPACE_END
