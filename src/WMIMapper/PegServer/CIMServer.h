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
//     Mike Day (mdday@us.ibm.com)
//	   Nitin Upasani, Hewlett-Packard Company (Nitin_Upasani@hp.com)
//	   Nag Boranna, Hewlett-Packard Company (nagaraja_boranna@hp.com)
//     Dan Gorey (djgorey@us.ibm.com)
//     Terry Martin, Hewlett-Packard Company (terry.martin@hp.com)
//     Amit K Arora, IBM (amita@in.ibm.com) for PEP#101
//     Mateus Baur, Hewlett-Packard Company (mateus.baur@hp.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_Server_h
#define Pegasus_Server_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/InternalException.h>
#include <Pegasus/Server/CIMServerState.h>
#include <Pegasus/Common/Cimom.h>
#include <Pegasus/Server/Linkage.h>
#include <Pegasus/Common/SSLContext.h>
#include <Pegasus/Common/AutoPtr.h>

#include "HTTPAuthenticatorDelegator.h"

PEGASUS_NAMESPACE_BEGIN

struct ServerRep;

class Monitor;
class CIMOperationRequestDispatcher;
class CIMOperationResponseEncoder;
class CIMOperationRequestDecoder;
class CIMOperationRequestAuthorizer;
class CIMExportRequestDispatcher;
class CIMExportResponseEncoder;
class CIMExportRequestDecoder;
class HTTPAcceptor;
class CIMRepository;

/*class ModuleController;
class IndicationHandlerService;
class IndicationService;
class ProviderManagerService;
class ProviderRegistrationManager;
class BinaryMessageHandler;*/
class SSLContextManager;


class PEGASUS_SERVER_LINKAGE CIMServer
{
public:

    enum Protocol { PROPRIETARY, STANDARD };
    
    /** Constructor - Creates a CIMServer object.
        The CIM Server objects establishes a repository object,
        a dispatcher object, and creates a channnel factory and
        acceptor for the Server.
        @param monitor	  monitor object for the server.
        @exception - ATTN
    */
    CIMServer(Monitor* monitor);
    
    ~CIMServer();

    /** Adds a connection acceptor for the specified listen socket.
        @param localConnection Boolean specifying whether the acceptor should
               listen on a local-system-only connection.
        @param portNumber Port number that should be used by the listener.
               This parameter is ignored if localConnection=true.
        @param useSSL Boolean specifying whether SSL should be used for
               connections created by this acceptor.
        @param exportConnection Boolean indicating whether this acceptor is 
               only for export connections. If true, client SSL certificate 
               verification is enabled on the export connection created by 
               this acceptor. Ignored when useSSL is false. 
    */
    void addAcceptor(
        Boolean localConnection,
        Uint32 portNumber,
        Boolean useSSL,
        Boolean exportConnection);

    /** Bind the acceptors to the specified listen sockets.
	@exception - This function may receive exceptions from
	Channel specific subfunctions.
    */
    void bind();

    void tickle_monitor();

    /** runForever Main runloop for the server.
    */
    void runForever();

    /** Call to gracefully shutdown the server.  The server connection socket
        will be closed to disable new connections from clients.
    */
    void stopClientConnection();

    /** Call to gracefully shutdown the server.  It is called when the server
        has been stopped and is ready to be shutdown.  Next time runForever()
        is called, the server shuts down.
    */
    void shutdown();

    /** Return true if the server has shutdown, false otherwise.
    */
    Boolean terminated() { return _dieNow; };

    /** Call to resume the sever.
    */
    void resume();

    /** Call to set the CIMServer state.  Also inform the appropriate
        message queues about the current state of the CIMServer.
    */
    void setState(Uint32 state);

    Uint32 getOutstandingRequestCount();

    /** Signal to shutdown
    */
    static void shutdownSignal();

private:

    Boolean _dieNow;

    Monitor* _monitor;
    CIMRepository* _repository;
	CIMOperationRequestDispatcher* _cimOperationRequestDispatcher;
    CIMOperationResponseEncoder* _cimOperationResponseEncoder;
    CIMOperationRequestDecoder* _cimOperationRequestDecoder;
    CIMOperationRequestAuthorizer* _cimOperationRequestAuthorizer;

    CIMExportRequestDispatcher* _cimExportRequestDispatcher;
    CIMExportResponseEncoder* _cimExportResponseEncoder;
    CIMExportRequestDecoder* _cimExportRequestDecoder;
    HTTPAuthenticatorDelegator* _httpAuthenticatorDelegator;

    Array<HTTPAcceptor*> _acceptors;
    AutoPtr<CIMServerState> _serverState; //PEP101

    /*ModuleController* _controlService;
	IndicationHandlerService* _handlerService;
    IndicationService* _indicationService;
    ProviderManagerService* _providerManager;
    ProviderRegistrationManager* _providerRegistrationManager;
    BinaryMessageHandler *_binaryMessageHandler;*/
    SSLContextManager* _sslContextMgr;
    
    void _init(void);
    SSLContext* _getSSLContext(Uint32 contextType);
};

PEGASUS_NAMESPACE_END

#endif /* Pegasus_Server_h */
