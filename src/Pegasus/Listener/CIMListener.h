//%/////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001, 2002 BMC Software, Hewlett-Packard Company, IBM,
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
//==============================================================================
//
// Author: Nitin Upasani, Hewlett-Packard Company (Nitin_Upasani@hp.com)
//
// Modified By:
//
//%/////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_Listener_h
#define Pegasus_Listener_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/Exception.h>
#include <Pegasus/Listener/Linkage.h>

PEGASUS_NAMESPACE_BEGIN

struct ListenerRep;

class Monitor;
class CIMExportRequestDispatcher;
class CIMExportResponseEncoder;
class CIMExportRequestDecoder;
class HTTPAcceptor;

class PEGASUS_LISTENER_LINKAGE CIMListener
{
public:

    enum Protocol { PROPRIETARY, STANDARD };

    /** Constructor - Creates a CIMListener object.
        The CIM Listener objects establishes a repository object,
        a dispatcher object, and creates a channnel factory and
        acceptor for the Listener.
        @param monitor	  monitor object for the server.
        @param rootPath Directory path to the repository.
    */

    CIMListener(
	Monitor* monitor,
	const String& rootPath,
        Boolean dynamicReg,
	Boolean staticConsumers,
	Boolean persistence);

    ~CIMListener();

    /** bind Binds the port address to the Listener.
	@param address char* to the port address for TCP.
	@exception - This function may receive exceptions from
	Channel specific subfunctions.
    */
    void bind(Uint32 port);

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

    Uint32 getOutstandingRequestCount();

private:

    Boolean _dieNow;

    String _rootPath;
    String _repositoryRootPath;

    Boolean _dynamicReg;
    Boolean _staticConsumers;
    Boolean _persistence;

    Monitor* _monitor;
    
    CIMExportRequestDispatcher* _cimExportRequestDispatcher;
    CIMExportResponseEncoder* _cimExportResponseEncoder;
    CIMExportRequestDecoder* _cimExportRequestDecoder;

    HTTPAcceptor*   _acceptor;
};

PEGASUS_NAMESPACE_END

#endif /* Pegasus_Listener_h */
