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
// Author: Nag Boranna, Hewlett-Packard Company(nagaraja_boranna@hp.com)
//
// Modified By:
//
//%/////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_LocalAuthenticationHandler_h
#define Pegasus_LocalAuthenticationHandler_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/String.h>
#include <Pegasus/Security/Authentication/LocalAuthenticator.h>
#include "Linkage.h"
#include "Authenticator.h"


PEGASUS_NAMESPACE_BEGIN


/** 
   Implements the AuthenticationHandler for Basic authentication. Extends
   the Authenticator and provides the implementation.
*/

class PEGASUS_SECURITY_LINKAGE LocalAuthenticationHandler : public Authenticator
{
public:

    /** Constructors  */
    LocalAuthenticationHandler();

    /** Destructor  */
    ~LocalAuthenticationHandler();
    
    /**
    Authenticate the request
    */
    Boolean authenticate(String authHeader, String authInfo);
    
    /**
    Construct and return the authentication response header
    */
    String getAuthResponseHeader(String realm, String& challenge);    

private:

    LocalAuthenticator* _localAuthenticator;
};

PEGASUS_NAMESPACE_END

#endif /* Pegasus_LocalAuthenticationHandler_h*/
