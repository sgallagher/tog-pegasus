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
// Author:  Nag Boranna, Hewlett-Packard Company(nagaraja_boranna@hp.com)
//
// Modified By: Jair Santos, Hewlett-Packard Company(jair.santos@hp.com)
//              Heather Sterling, IBM (hsterl@us.ibm.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_AuthenticationInfoRep_h
#define Pegasus_AuthenticationInfoRep_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/String.h>
#include <Pegasus/Common/Sharable.h>
#include <Pegasus/Common/Linkage.h>
#include <Pegasus/Common/SSLContext.h>

#ifdef PEGASUS_KERBEROS_AUTHENTICATION
#include <Pegasus/Common/CIMKerberosSecurityAssociation.h>
#endif

PEGASUS_NAMESPACE_BEGIN

class AuthenticationInfo;

/**
    This class keeps the authentication information of a connection 
    persistent until the connection is destroyed.
*/
class PEGASUS_COMMON_LINKAGE AuthenticationInfoRep :  public Sharable
{
public:
    enum AuthStatus { NEW_REQUEST, CHALLENGE_SENT, AUTHENTICATED };

	//ATTN: we should be using an enumeration for the authtype instead of a string.
	//In the AuthenticationManager, the authtype is set to Basic, Digest, etc
	//We also need to be able to check whether the type is SSL, so I'm adding a 
	//string here to make it less arbitrary.  PEP165
	static const String AUTH_TYPE_SSL;
    
    AuthenticationInfoRep(Boolean flag);

    ~AuthenticationInfoRep();

    AuthStatus getAuthStatus() const 
    { 
        return _authStatus;
    } 

    void   setAuthStatus(AuthStatus status);

    String getAuthenticatedUser() const 
    { 
        return _authUser;
    } 

    void   setAuthenticatedUser(const String& userName);

    String getAuthenticatedPassword() const 
    { 
        return _authPassword;
    } 

    void   setAuthenticatedPassword(const String& password);

    String getAuthChallenge() const 
    { 
        return _authChallenge;
    } 

    void   setAuthChallenge(const String& challenge);

    String getAuthSecret() const 
    { 
        return _authSecret;
    } 

    void   setAuthSecret(const String& secret);

    Boolean isPrivileged() const 
    { 
        return _privileged;
    } 

    void   setPrivileged(Boolean privileged);

    Boolean isAuthenticated() const 
    { 
        return (_authStatus == AUTHENTICATED) ? true : false;
    } 

    String getAuthType() const 
    { 
        return _authType;
    } 

    void   setAuthType(const String& authType);

#ifdef PEGASUS_KERBEROS_AUTHENTICATION
    CIMKerberosSecurityAssociation* getSecurityAssociation() const 
    { 
        return _securityAssoc.get();
    }
 
    void setSecurityAssociation();
#endif

    Boolean isExportConnection() const
    {
        return _exportConnection;
    }

    void setExportConnection(Boolean exportConnection);

	//PEP187
    SSLCertificateInfo* getClientCertificate()
	{
        return _clientCertificate;
	}

	//PEP187
	void setClientCertificate(SSLCertificateInfo* clientCertificate);

private:

    /** Constructors  */
    AuthenticationInfoRep();

    AuthenticationInfoRep(const AuthenticationInfoRep& x);

    AuthenticationInfoRep& operator=(const AuthenticationInfoRep& x);

    String  _authUser;
    String  _authPassword;
    String  _authChallenge;
    String  _authSecret;
    Boolean _privileged;
    String  _authType;
    AuthStatus _authStatus;
#ifdef PEGASUS_KERBEROS_AUTHENTICATION
    AutoPtr<CIMKerberosSecurityAssociation> _securityAssoc;//PEP101
#endif

    Boolean _exportConnection;
	SSLCertificateInfo* _clientCertificate;
};

PEGASUS_NAMESPACE_END

#endif   /* Pegasus_AuthenticationInfoRep_h*/
