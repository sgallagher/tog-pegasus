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

#ifndef Pegasus_LocalAuthenticator_h
#define Pegasus_LocalAuthenticator_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/Channel.h>
#include <Pegasus/Common/Selector.h>

#include "Authenticator.h"
#include "LocalAuthUtils.h"
#include "Linkage.h"


PEGASUS_NAMESPACE_BEGIN


/**
    The Local Authenticator implimentation and this extends Authenticator 
    interface.  This class is more of a utily class, shared by CIM Server  
    and as well a CIM client API. 
 
    To ensure security over Client Local Channel connection, there is
    local authentication security mechanism designed where the CIM clients
    would require opening a session before sending any CIM requests to the
    CIM Server.  The session opening will include local clients passing
    user name to CIM Server for authentication purposes.  CIM Server will
    challenge the local CIM Client to make sure that the client user has
    permission to use the application.  These local clients will be
    authenticated using the local file system based security mechanism.
 
    Local CIM Clients wish to connect to CIM Server must pass user name to
    CIM Server. CIM Server will use specified user and will create a file on
    local system (file name will include a uniquely generated identification),
    generates a random data and writes to this file. It then sets the client
    user as the owner of the file with read-only permission for only this user.
    CIM Server will then send this file name to CIM Client. CIM Client should
    read the file and send the content over to CIM Server. CIM Server will
    authorize the client and open the Channel to communicate.
 */

   
class PEGASUS_SECURITY_LINKAGE LocalAuthenticator 
      : public Authenticator
{
public:

    /**
       Virtual constructor.
     */ 
    LocalAuthenticator();

    /**
       Virtual destructor.
     */
    virtual ~LocalAuthenticator();

    /**
       This is only called on server side.
       To ensure security over Client Local Channel connection, CIM Server
       verifies whether the Client that is connecting to it has permission
       or not. This is done by challenging CIM Clients over local channel.
     
       @param   channel        Local Channel used to communicate
     
       @return  true           if successfully authenticate
                false          not a valid client
     
     */     
    virtual Boolean verifyAuthenticate(Channel* channel);

    /**
       This is only called on client side.
       To ensure security over Client Local Channel connection, CIM Client
       responds back to CIM Server challenge.
     
       @param   channel        Local Channel used to communicate
     
       @param   user           Local client process owner
     
       @return  true           if successfully connect to CIM Server
                false          not able to connect.
     
     */
    virtual Boolean proveAuthenticate(Channel* channel, char* user);


    /**
       sends data over a specified channel. Server may use this.
     
       @param   channel        Local Channel used to communicate
     
       @param   data           data to send
     
       @param   size           size of the data
     
       @return  0              if successful
                1              failed for some reason
     
     */     
    virtual int sendChallenge(Channel* channel, char* data, int size);

    /**
       read data from a specified channel. Server may use this.
     
       @param   channel        Local Channel used to communicate
     
       @param   data           data to send
     
       @return  0              if successful
                1              failed for some reason
     
     */     
    virtual int recvResponse(Channel* channel, char* data);


    /**
       read data from a specified channel. Client may use this.
     
       @param   channel        Local Channel used to communicate
     
       @param   data           data to send
     
       @return  0              if successful
                1              failed for some reason
     
     */     
    virtual int recvChallenge(Channel* channel, char* data);

    /**
       sends data over a specified channel. Client may use this.
     
       @param   channel        Local Channel used to communicate
     
       @param   data           data to send
     
       @param   size           size of the data
     
       @return  0              if successful
                1              failed for some reason
     
     */     
    virtual int sendResponse(Channel* channel, char* data, int size);

};


PEGASUS_NAMESPACE_END

#endif
