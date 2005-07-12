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
// Author: Bapu Patil, Hewlett-Packard Company (bapu_patil@hp.com)
//
// Modified By:
//              Nag Boranna, Hewlett-Packard Company(nagaraja_boranna@hp.com)
//            Sushma Fernandes, Hewlett-Packard Company(sushma_fernandes@hp.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_Authenticator_h
#define Pegasus_Authenticator_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/String.h>
#include <Pegasus/Common/AuthenticationInfo.h>
#include <Pegasus/Common/AuthenticationInfoRep.h>

#include <Pegasus/Security/Authentication/Linkage.h>


PEGASUS_NAMESPACE_BEGIN

/** This is an abstract class that the authentication handlers will extend and
    provide the implementation.
*/
class PEGASUS_SECURITY_LINKAGE Authenticator
{
public:

    /** Constructors  */
    Authenticator() { };

    /** Destructor  */
    virtual ~Authenticator() { };

    /** Verify the authentication of the user passed in the HTTP authorization header.
        @param authHeader String containing the Authorization header
        @param authInfo Reference to AuthenticationInfo object
        @return true on successful authentication, false otherwise
    */
    virtual Boolean authenticate(
        const String& authHeader, 
        AuthenticationInfo* authInfo) = 0;

    /** Construct and return the Basic authentication challenge header
        @param authType String containing the authentication type
        @param userName String containing the user name
        @param authInfo Reference to AuthenticationInfo object
        @return A string containing the authentication challenge header.
    */
    virtual String getAuthResponseHeader(
        const String& authType = String::EMPTY,
        const String& userName = String::EMPTY,
        AuthenticationInfo* authInfo = 0) = 0;

    /**
        Verify whether the user is valid.
        @param userName String containing the user name
        @return true on successful validation, false otherwise
    */
    virtual Boolean validateUser(const String& userName) = 0;
};

PEGASUS_NAMESPACE_END

#endif   /* Pegasus_Authenticator_h*/
