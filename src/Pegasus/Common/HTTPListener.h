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
//
//%/////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_HTTPListener_h
#define Pegasus_HTTPListener_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/MessageQueueService.h>
#include <Pegasus/Common/Monitor.h>
#include <Pegasus/Common/String.h>
#include <Pegasus/Common/Linkage.h>

PEGASUS_NAMESPACE_BEGIN

struct HTTPListenerRep;

/** This class represents an HTTP listener.
*/
class PEGASUS_COMMON_LINKAGE HTTPListener : public MessageQueueService
{
public:

    /** Constructor.
	@param monitor pointer to monitor object which this class uses to
	    solicit SocketMessages on the server port (socket).
    */
  HTTPListener(Monitor* monitor) 
    {
      #error "this class is not implemented"
    }
  

    /** Destructor. */
    ~HTTPListener();

    /** This method is called whenever a SocketMessage is enqueued
	on the input queue of the HTTPListener object.
    */ 
    virtual void handleEnqueue(Message *);
    virtual void handleEnqueue();

    /** Bind to the given port (the port on which this HTTP listener
	listens for connections).
	@param portNumber the number of the port used to listen for
	    connection requests.
	@exception throws BindFailedException if unable to bind to the given
	    port (either because the port number is invalid or the
	    port is in use).
    */
    void bind(Uint32 portNumber);

    /** Unbind from the given port.
    */
    void unbind();

private:

    void _acceptConnection();

    Monitor* _monitor;
    HTTPListenerRep* _rep;
};

PEGASUS_NAMESPACE_END

#endif /* Pegasus_HTTPListener_h */
