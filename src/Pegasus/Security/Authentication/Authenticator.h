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
//              Nag Boranna, Hewlett-Packard Company(nagaraja_boranna@hp.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_Authenticator_h
#define Pegasus_Authenticator_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/String.h>
#include <Pegasus/Common/AuthenticationInfo.h>
#include "Linkage.h"


PEGASUS_NAMESPACE_BEGIN

/**
    An authentication interface that the authentication handler will implement.
*/

class PEGASUS_SECURITY_LINKAGE Authenticator
{
public:

    /** Constructors  */
    Authenticator() { };

    /** Destructor  */
    virtual ~Authenticator() { };

    /**
    Authenticate the request
    */
    virtual Boolean authenticate(
        const String& authHeader, 
        AuthenticationInfo* authInfo) = 0;

    /**
    Construct and return the authentication response header
    */
    virtual String getAuthResponseHeader(
        const String& authType,
        const String& userName,
        AuthenticationInfo* authInfo = 0) = 0;

    /**
    Construct and return the authentication response header
    */
    virtual String getAuthResponseHeader(
        const String& authHeader,
        AuthenticationInfo* authInfo = 0) = 0;

};

PEGASUS_NAMESPACE_END

#endif   /* Pegasus_Authenticator_h*/
