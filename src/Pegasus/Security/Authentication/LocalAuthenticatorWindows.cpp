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


#include "LocalAuthenticator.h"

PEGASUS_NAMESPACE_BEGIN


LocalAuthenticator::LocalAuthenticator() { }

/** 
 * destructor.
 */
LocalAuthenticator::~LocalAuthenticator() { }

//
// handles local Channel authentication  Server Side
//
Boolean LocalAuthenticator::verifyAuthenticate(Channel* channel)
{
       
      // TODO - Handle one for windows


        return true;
}

//
//  challenge client
//
int LocalAuthenticator::sendChallenge(Channel* channel, char* data, int size)
{
    return (channel->write(data, size));
}

//
//  recv response
//
int LocalAuthenticator::recvResponse(Channel* channel, char* data)
{
    return (channel->read(data, sizeof(data)));
}

//
//  recv challenge
//
int LocalAuthenticator::recvChallenge(Channel* channel, char* data)
{
    return (channel->read(data, sizeof(data)) );
}

//
//  challenge client
//
int LocalAuthenticator::sendResponse(Channel* channel, char* data, int size)
{
    return (channel->write(data, size));
}

//
//  handles local Channel authentication Client side
//
Boolean LocalAuthenticator::proveAuthenticate(Channel* channel, char* user)
{
     // TODO : implement windows one

        return true;
}

PEGASUS_NAMESPACE_END
