//%2006////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001, 2002 BMC Software; Hewlett-Packard Development
// Company, L.P.; IBM Corp.; The Open Group; Tivoli Systems.
// Copyright (c) 2003 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation, The Open Group.
// Copyright (c) 2004 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation; VERITAS Software Corporation; The Open Group.
// Copyright (c) 2005 Hewlett-Packard Development Company, L.P.; IBM Corp.;
// EMC Corporation; VERITAS Software Corporation; The Open Group.
// Copyright (c) 2006 Hewlett-Packard Development Company, L.P.; IBM Corp.;
// EMC Corporation; Symantec Corporation; The Open Group.
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
//%/////////////////////////////////////////////////////////////////////////////

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/Tracer.h>
#include "AuthenticationInfoRep.h"
#include <Pegasus/Common/SSLContext.h>

PEGASUS_USING_STD;

PEGASUS_NAMESPACE_BEGIN

const String AuthenticationInfoRep::AUTH_TYPE_SSL = "SSL";

AuthenticationInfoRep::AuthenticationInfoRep(Boolean flag)
    :
    _authUser(String::EMPTY),
    _authPassword(String::EMPTY),
    _localAuthSecret(String::EMPTY),
    _authType(String::EMPTY),
    _connectionAuthenticated(false),
    _wasRemotePrivilegedUserAccessChecked(false)
{
    PEG_METHOD_ENTER(
        TRC_AUTHENTICATION, "AuthenticationInfoRep::AuthenticationInfoRep");

    PEG_METHOD_EXIT();
}

AuthenticationInfoRep::~AuthenticationInfoRep()
{
    PEG_METHOD_ENTER(
        TRC_AUTHENTICATION, "AuthenticationInfoRep::~AuthenticationInfoRep");

    PEG_METHOD_EXIT();
}

void AuthenticationInfoRep::setConnectionAuthenticated(
    Boolean connectionAuthenticated)
{
    PEG_METHOD_ENTER(TRC_AUTHENTICATION,
        "AuthenticationInfoRep::setConnectionAuthenticated");

    _connectionAuthenticated = connectionAuthenticated;

    PEG_METHOD_EXIT();
}

void AuthenticationInfoRep::setAuthenticatedUser(const String& userName)
{
    PEG_METHOD_ENTER(
        TRC_AUTHENTICATION, "AuthenticationInfoRep::setAuthenticatedUser");

    _authUser = userName;

    PEG_METHOD_EXIT();
}

void AuthenticationInfoRep::setAuthenticatedPassword(const String& password)
{
    PEG_METHOD_ENTER(
        TRC_AUTHENTICATION, "AuthenticationInfoRep::setAuthenticatedPassword");

    _authPassword = password;

    PEG_METHOD_EXIT();
}

void AuthenticationInfoRep::setLocalAuthSecret(const String& secret)
{
    PEG_METHOD_ENTER(
        TRC_AUTHENTICATION, "AuthenticationInfoRep::setLocalAuthSecret");

    _localAuthSecret = secret;

    PEG_METHOD_EXIT();
}

void AuthenticationInfoRep::setAuthType(const String& authType)
{
    PEG_METHOD_ENTER(
        TRC_AUTHENTICATION, "AuthenticationInfoRep::setAuthType");

    _authType = authType;

    PEG_METHOD_EXIT();
}

#ifdef PEGASUS_KERBEROS_AUTHENTICATION
void AuthenticationInfoRep::setSecurityAssociation()
{
    PEG_METHOD_ENTER(
        TRC_AUTHENTICATION, "AuthenticationInfoRep::setSecurityAssociation");

    if ( !_securityAssoc.get() )
    {
        _securityAssoc.reset(new CIMKerberosSecurityAssociation);
    }

    PEG_METHOD_EXIT();
}
#endif

void AuthenticationInfoRep::setClientCertificateChain(
    Array<SSLCertificateInfo*> clientCertificate)
{
    PEG_METHOD_ENTER(TRC_AUTHENTICATION,
        "AuthenticationInfoRep::setClientCertificateChain");

    _clientCertificate = clientCertificate;

    PEG_METHOD_EXIT();
}

PEGASUS_NAMESPACE_END
