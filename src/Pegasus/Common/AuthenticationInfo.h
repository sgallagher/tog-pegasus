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

#ifndef Pegasus_AuthenticationInfo_h
#define Pegasus_AuthenticationInfo_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/InternalException.h>
#include <Pegasus/Common/Tracer.h>
#include <Pegasus/Common/AuthenticationInfoRep.h>
#include <Pegasus/Common/Linkage.h>
#include <Pegasus/Common/SSLContext.h>
#include <Pegasus/Common/SessionKey.h>

#ifdef PEGASUS_KERBEROS_AUTHENTICATION
#include <Pegasus/Common/CIMKerberosSecurityAssociation.h>
#endif

PEGASUS_NAMESPACE_BEGIN


/**
    This class keeps the authentication information of a connection
    persistent until the connection is destroyed.

    The HTTPConnection object creates a AuthenticationInfo object on a new
    socket connection and includes this object reference in the HTTPMessage
    that gets passed to the Delegator and in turn to the AuthenticationManager.
    The AuthenticationManager and the related authentication classes use the
    AuthenticationInfo to store and access the persistent authentication
    information for a connection.
*/
class PEGASUS_COMMON_LINKAGE AuthenticationInfo
{
public:

    /** Constructor - Creates an uninitiated new AuthenticationInfo
        object reprenting a AuthenticationInfo class. The class object
        created by this constructor can only be used in an operation such as the
        copy constructor.  It cannot be used to do method calls like
        setAuthStatus, getAuthType, etc. since it is unitiated.

        Use one of the other constructors to create an initiated new
        AuthenticationInfo class object. Throws an exception
        "unitialized handle" if this unitialized handle is used for
        method calls.
    */
    AuthenticationInfo() : _rep(0)
    {
        PEG_METHOD_ENTER(
            TRC_AUTHENTICATION, "AuthenticationInfo::AuthenticationInfo");


        PEG_METHOD_EXIT();
    }

    /** Creates and instantiates a AuthenticationInfo from another
        AuthenticationInfo instance
        @return pointer to the new AuthenticationInfo instance
    */
    AuthenticationInfo(const AuthenticationInfo& x)
    {
        PEG_METHOD_ENTER(
            TRC_AUTHENTICATION, "AuthenticationInfo::AuthenticationInfo");

        Inc(_rep = x._rep);

        PEG_METHOD_EXIT();
    }

    /** Assignment operator */
    AuthenticationInfo& operator=(const AuthenticationInfo& x)
    {
        PEG_METHOD_ENTER(
            TRC_AUTHENTICATION, "AuthenticationInfo::AuthenticationInfo");

        if (x._rep != _rep)
        {
            Dec(_rep);
            Inc(_rep = x._rep);
        }

        PEG_METHOD_EXIT();
        return *this;
    }

    /** Constructor - Instantiates a AuthenticationInfo object.
    @param flag - used only to distinguish from the default constructor.
    */
    AuthenticationInfo(Boolean flag)
    {
        PEG_METHOD_ENTER(
            TRC_AUTHENTICATION, "AuthenticationInfo::AuthenticationInfo");

        _rep = new AuthenticationInfoRep(flag);

        PEG_METHOD_EXIT();
    }

    /** Destructor  */
    ~AuthenticationInfo()
    {
        PEG_METHOD_ENTER(
            TRC_AUTHENTICATION, "AuthenticationInfo::~AuthenticationInfo");

        Dec(_rep);

        PEG_METHOD_EXIT();
    }

    /** Get the authentication status of the request
        @return the current authentication status
    */
    AuthenticationInfoRep::AuthStatus getAuthStatus() const
    {
        _checkRep();
        return _rep->getAuthStatus();
    }

    /** Sets the authentication status of the request to the status
        specified.
        @param status - the new authentication status
    */
    void   setAuthStatus(AuthenticationInfoRep::AuthStatus status)
    {
        _checkRep();
        _rep->setAuthStatus(status);
    }

    /** Get the previously authenticated user name
        @return the authenticated user name
    */
    String getAuthenticatedUser() const
    {
        _checkRep();
        return _rep->getAuthenticatedUser();
    }

    /** Sets the authenticated user name
        @param userName - string containing the authenticated user name
    */
    void   setAuthenticatedUser(const String& userName)
    {
        _checkRep();
        _rep->setAuthenticatedUser(userName);
    }

    /** Get the previously authenticated password
        @return the authenticated password
    */
    String getAuthenticatedPassword() const
    {
        _checkRep();
        return _rep->getAuthenticatedPassword();
    }

    /** Sets the authenticated password
        @param password - string containing the authenticated password
    */
    void   setAuthenticatedPassword(const String& password)
    {
        _checkRep();
        _rep->setAuthenticatedPassword(password);
    }

    /** Get the authentication challenge that was sent to the client
        @return string containing the authentication challenge
    */
    String getAuthChallenge() const
    {
        _checkRep();
        return _rep->getAuthChallenge();
    }

    /** Sets the authentication challenge to the specified challenge
        @param challenge - string containing the authentication challenge
    */
    void   setAuthChallenge(const String& challenge)
    {
        _checkRep();
        _rep->setAuthChallenge(challenge);
    }

    /** Get the authentication secret that was sent to client
        @return string containing the authentication secret
    */
    String getAuthSecret() const
    {
        _checkRep();
        return _rep->getAuthSecret();
    }

    /** Set the authentication secret to the specified secret
        @param secret - string containing the authentication secret
    */
    void   setAuthSecret(const String& secret)
    {
        _checkRep();
        _rep->setAuthSecret(secret);
    }

    /** Returns the connection type of the previous authenticated request
        @return true if the connection is privileged, false otherwise
    */
    Boolean isPrivileged() const
    {
        _checkRep();
        return _rep->isPrivileged();
    }

    /** Set the privileged flag to the specified value
        @param privileged - boolean flag indicating the connection type
    */
    void   setPrivileged(Boolean privileged)
    {
        _checkRep();
        _rep->setPrivileged(privileged);
    }

    /** Is the request authenticated
    */
    /** Returns the authentication status of the current connection.
        @return true if the connection was authenticated, false otherwise
    */
    Boolean isAuthenticated() const
    {
        _checkRep();
        return _rep->isAuthenticated();
    }

    /** Set the authentication type to the specified type
        @param string containing the authentication type
    */
    void   setAuthType(const String& authType)
    {
        _checkRep();
        _rep->setAuthType(authType);
    }

    /** Get the authentication type of the connection
        @return string containing the authentication type
    */
    String getAuthType() const
    {
        _checkRep();
        return _rep->getAuthType();
    }

    /**
        Set the IP address to the specified IP address
        @param string containing the IP address
    */
    void setIpAddress(const String& ipAddress)
    {
        _checkRep();
        _rep->setIpAddress(ipAddress);
    }

    /**
        Get the IP address of the connection

        NOTE: The IP address is for debug use only.
        It should not be used for authentication purposes.

        @return string containing the IP address
    */
    String getIpAddress() const
    {
        _checkRep();
        return _rep->getIpAddress();
    }


#ifdef PEGASUS_KERBEROS_AUTHENTICATION
    /** Get the CIM Security Association
        @return a pointer to the CIM Security Association
    */
    CIMKerberosSecurityAssociation* getSecurityAssociation() const
    {
        _checkRep();
        return _rep->getSecurityAssociation();
    }

    /** Set the CIM Security Association
        The pointer will only be set once. If it is already set it will
        not reset it.
    */
    void setSecurityAssociation()
    {
        _checkRep();
        _rep->setSecurityAssociation();
    }
#endif

    Array<SSLCertificateInfo*> getClientCertificateChain()
    {
        _checkRep();
        return _rep->getClientCertificateChain();
    }

    void setClientCertificateChain(Array<SSLCertificateInfo*> clientCertificate)
    {
        _checkRep();
        _rep->setClientCertificateChain(clientCertificate);
    }

    /** Set flag to show that isRemotePrivilegedUserAccess check has been done
        this function should only be used by OpenPegasus AuthenticationHandlers
    */
    void setRemotePrivilegedUserAccessChecked()
    {
        _checkRep();
        _rep->setRemotePrivilegedUserAccessChecked();
    }

    /** Indicates whether the isRemotePrivilegedUserAccess check has been
        performed.  This method should only be used by OpenPegasus
        AuthenticationHandlers
    */
    Boolean getRemotePrivilegedUserAccessChecked()
    {
        _checkRep();
        return _rep->getRemotePrivilegedUserAccessChecked();
    }

    /** Set the session key.
    */
    void setSessionKey(const SessionKey& sessionKey)
    {
        _checkRep();
        return _rep->setSessionKey(sessionKey);
    }

    /** Get the session key.
    */
    const SessionKey& getSessionKey() const
    {
        _checkRep();
        return _rep->getSessionKey();
    }

private:

    AuthenticationInfo(AuthenticationInfoRep* rep) : _rep(rep)
    {

    }

    void _checkRep() const
    {
        if (!_rep)
            throw UninitializedObjectException();
    }

    AuthenticationInfoRep* _rep;
};

PEGASUS_NAMESPACE_END

#endif   /* Pegasus_AuthenticationInfo_h*/
