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

#ifndef Pegasus_ShutdownService_h
#define Pegasus_ShutdownService_h

#include <cctype>
#include <Pegasus/Common/Config.h>
#include <Pegasus/Config/ConfigManager.h>
#include <Pegasus/Common/String.h>
#include <Pegasus/Common/Array.h>
#include <Pegasus/Common/System.h>
#include <Pegasus/Server/CIMServer.h>
#include <Pegasus/ProviderManager/ProviderManagerService.h>
#include <Pegasus/ProviderManager/ProviderManager.h>

PEGASUS_NAMESPACE_BEGIN

/**
///////////////////////////////////////////////////////////////////////////////
//
// This class defines the ShutdownService necessary to process the shutdown
// request to gracefully shutdown the CIMServer.
//
// In order to shutdown the CIMServer gracefully, the CIMServer state
// information must be maintained.
//
// The state of the CIMServer is kept in the CIMServerState object.  After
// CIMServer starts up, the CIMServer state is set to RUNNING.  When a
// shutdown request is received by the ShutdownService, the CIMServer state
// will be set to TERMINATING.
//
// Whenever a request comes through a client connection, before the request
// is being routed off to the appropriate message queue, the CIMServer state
// is checked.  If the state is set to TERMINATING, this means CIMServer is
// in the process of being shutdown, an error response will be sent back
// to the client to indicate that the request cannot be processed due to
// CIMServer shutting down.
//
// In order to determine if the CIMServer is servicing any CIM requests at
// the time of a shutdown, CIMServer keeps track of the number of CIM requests
// that are outstanding.  This request count is kept in the HTTPConnection
// object.  The request count is incremented everytime a request comes through
// a client connection, and is decremented everytime a response is sent back
// to the client.
//
// Before the ShutdownService shuts down the CIMServer, the request count is
// checked to determine if there are any outstanding CIM requests being
// processed.  If there are no requests outstanding, the CIMServer will be
// shutdown.  If there are requests outstanding, the ShutdownService will
// wait periodically until the requests are all processed or until the
// timeout expires before shutting down CIMServer.
//
// If timeout expires and there are still requests outstanding, and the
// force shutdown option was not specified, CIMServer will be set back to
// its RUNNING state.
//
///////////////////////////////////////////////////////////////////////////////
*/

class PEGASUS_SERVER_LINKAGE ShutdownService
{
public:

    /**
    Construct the singleton instance of the ShutdownService and return a
    pointer to that instance.
    */
    static ShutdownService* getInstance(CIMServer* cimserver);

    /**
    Shutdown CIMOM.
    */
    void shutdown(Boolean force, Uint32 timeout);

private:

    static ShutdownService* _instance;

    static CIMServer*               _cimserver;
    static ProviderManagerService*  _providerManagerService;
    static ProviderManager*         _providerManager;

    static Uint32     _operationTimeout;
    static Uint32     _shutdownTimeout;

    //
    // This is meant to be a singleton, so the constructor and the
    // destructor are made private.
    //

    /** Constructor. */
    ShutdownService(CIMServer* cimserver);

    /** Destructor. */
    ~ShutdownService();

    void _shutdownCIMServer();

    void _resumeCIMServer();

    void _shutdownSubscriptionService();

    void _shutdownProviders();

    void _initTimeoutValues(Uint32 timeoutParmValue);

};

PEGASUS_NAMESPACE_END

#endif /* Pegasus_ShutdownService_h */

