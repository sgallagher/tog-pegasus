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
// Author:  Nag Boranna, Hewlett-Packard Company(nagaraja_boranna@hp.com)
//
// Modified By:
//
//%/////////////////////////////////////////////////////////////////////////////

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/Tracer.h>
#include "AuthenticationInfoRep.h"

PEGASUS_USING_STD;

PEGASUS_NAMESPACE_BEGIN


AuthenticationInfoRep::AuthenticationInfoRep(Boolean flag)
    : 
    _authUser(String::EMPTY),
    _authChallenge(String::EMPTY),
    _authSecret(String::EMPTY),
    _privileged(false),
    _authType(String::EMPTY),
    _authStatus(NEW_REQUEST)
{ 
    PEG_METHOD_ENTER(
        TRC_AUTHENTICATION, "AuthenticationInfoRep::AuthenticationInfoRep");

#ifdef PEGASUS_KERBEROS_AUTHENTICATION
    _securityAssoc = NULL;
#endif

    PEG_METHOD_EXIT();
}


AuthenticationInfoRep::~AuthenticationInfoRep()
{
    PEG_METHOD_ENTER(
        TRC_AUTHENTICATION, "AuthenticationInfoRep::~AuthenticationInfoRep");

#ifdef PEGASUS_KERBEROS_AUTHENTICATION
    if (_securityAssoc)
    {
        delete _securityAssoc;
        _securityAssoc = 0;
    }
#endif

    PEG_METHOD_EXIT();
}

void   AuthenticationInfoRep::setAuthStatus(AuthStatus status)
{
    PEG_METHOD_ENTER(
        TRC_AUTHENTICATION, "AuthenticationInfoRep::setAuthStatus");

    _authStatus = status;

    PEG_METHOD_EXIT();
}

void   AuthenticationInfoRep::setAuthenticatedUser(const String& userName)
{
    PEG_METHOD_ENTER(
        TRC_AUTHENTICATION, "AuthenticationInfoRep::setAuthenticatedUser");

    _authUser = userName;

    PEG_METHOD_EXIT();
}

void   AuthenticationInfoRep::setAuthChallenge(const String& challenge)
{
    PEG_METHOD_ENTER(
        TRC_AUTHENTICATION, "AuthenticationInfoRep::setAuthChallenge");

    _authChallenge = challenge;

    PEG_METHOD_EXIT();
}

void   AuthenticationInfoRep::setAuthSecret(const String& secret)
{
    PEG_METHOD_ENTER(
        TRC_AUTHENTICATION, "AuthenticationInfoRep::setAuthSecret");

    _authSecret = secret;

    PEG_METHOD_EXIT();
}

void   AuthenticationInfoRep::setPrivileged(Boolean privileged)
{
    PEG_METHOD_ENTER(
        TRC_AUTHENTICATION, "AuthenticationInfoRep::setPrivileged");

    _privileged = privileged;

    PEG_METHOD_EXIT();
}

void   AuthenticationInfoRep::setAuthType(const String& authType)
{
    PEG_METHOD_ENTER(
        TRC_AUTHENTICATION, "AuthenticationInfoRep::setAuthType");

    _authType = authType;

    PEG_METHOD_EXIT();
}

#ifdef PEGASUS_KERBEROS_AUTHENTICATION
void   AuthenticationInfoRep::setSecurityAssociation()
{
    PEG_METHOD_ENTER(
        TRC_AUTHENTICATION, "AuthenticationInfoRep::setSecurityAssociation");

    if ( !_securityAssoc )
    {
        _securityAssoc = new CIMKerberosSecurityAssociation;
    }

    PEG_METHOD_EXIT();
}
#endif


PEGASUS_NAMESPACE_END
