//%/////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001 BMC Software, Hewlett-Packard Company, IBM,
// The Open Group, Tivoli Systems
//
// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation
// the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies of substantial portions of this software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
// THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.
//
//==============================================================================
//
// Author: Bapu Patil, Hewlett-Packard Company (bapu_patil@hp.com)
//
// Modified By:
//
//%/////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_Authenticator_h
#define Pegasus_Authenticator_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/Channel.h>

#include "Linkage.h"

PEGASUS_NAMESPACE_BEGIN
/** 
    Authenticator interface class for implimenting authentication handling
    for CIM Clients.
 
    To ensure security over Client Local Channel connection, there is 
    local authentication security mechanism designed where the CIM clients 
    would require opening a session before sending any CIM requests to the 
    CIM Server.  The session opening will include local clients passing 
    user name to CIM Server for authentication purposes.  CIM Server will 
    challenge the local CIM Client to make sure that the client user has 
    permission to use the application.  These local clients will be 
    authenticated using the local file system based security mechanism. 
 
 */

class PEGASUS_SECURITY_LINKAGE Authenticator
{
public:

    /** 
       Virtual constructor.
     */
    Authenticator();

    /** 
       Virtual destructor.
     */
    virtual ~Authenticator();

    /**
       This is only called on server side. 
       To ensure security over Client Local Channel connection, CIM Server
       verifies whether the Client that is connecting to it has permission
       or not. This is done by challenging CIM Clients over local channel.
     
        @param   channel        Local Channel used to communicate
     
       @return  true           if successfully authenticate
                false          not a valid client
     
     */ 
    virtual Boolean verifyAuthenticate(Channel* channel) = 0;

    /**
       This is only called on client side.
       To ensure security over Client Local Channel connection, CIM Client
       responds back to CIM Server challenge.
     
       @param   channel        Local Channel used to communicate
     
       @param   user           Local client process owner   
     
       @return  true           if successfully connect to CIM Server
                false          not able to connect.
     
     */ 
    virtual Boolean proveAuthenticate(Channel* channel, char* user) = 0;


};
#endif
