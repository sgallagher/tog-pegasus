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
// Author:  Nag Boranna, Hewlett-Packard Company(nagaraja_boranna@hp.com)
//
// Modified By:
//
//%/////////////////////////////////////////////////////////////////////////////

#include "AuthenticationInfo.h"

PEGASUS_USING_STD;

PEGASUS_NAMESPACE_BEGIN


AuthenticationInfo::AuthenticationInfo()
    : 
    _authStatus(NEW_REQUEST), 
    _authUser(String::EMPTY),
    _authChallenge(String::EMPTY), 
    _authSecret(String::EMPTY),
    _privileged(false) 
{ 

}


AuthenticationInfo::~AuthenticationInfo()
{

}

//
// Set the authentication status of the request
//
void   AuthenticationInfo::setAuthStatus(Uint32 status)
{
    _authStatus = status;
}

//
// Set the authenticated user name
//
void   AuthenticationInfo::setAuthenticatedUser(String userName)
{
    _authUser = userName;
}

//
// Set the authentication challenge
//
void   AuthenticationInfo::setAuthChallenge(String challenge)
{
    _authChallenge = challenge;
}

//
// Set the authentication secret
//
void   AuthenticationInfo::setAuthSecret(String secret)
{
    _authSecret = secret;
}

//
// Set the privileged flag
//
void   AuthenticationInfo::setPrivileged(Boolean privileged)
{
    _privileged = privileged;
}

PEGASUS_NAMESPACE_END
