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

#ifndef Pegasus_TLS_h
#define Pegasus_TLS_h

#ifdef PEGASUS_HAS_SSL
#define OPENSSL_NO_KRB5 1
#include <openssl/err.h>
#include <openssl/ssl.h>
#include <openssl/rand.h>
#else
#define SSL_CTX void
typedef void SSL_Context;

#endif // end of PEGASUS_HAS_SSL

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/Socket.h>
#include <Pegasus/Common/String.h>
#include <Pegasus/Common/InternalException.h>
#include <Pegasus/Common/SSLContext.h>
#include <Pegasus/Common/Linkage.h>
#include <Pegasus/Common/AutoPtr.h>
#include <Pegasus/Common/ReadWriteSem.h>

// REVIEW: Figure out how this works (note to myself)?


PEGASUS_NAMESPACE_BEGIN


#ifdef PEGASUS_HAS_SSL
class PEGASUS_COMMON_LINKAGE SSLSocket
{
public:

    SSLSocket(
        SocketHandle socket,
        SSLContext * sslcontext,
        ReadWriteSem * sslContextObjectLock);

    ~SSLSocket();

    Boolean incompleteSecureReadOccurred(Sint32 retCode);

    Sint32 read(void* ptr, Uint32 size);

    Sint32 timedWrite(const void* ptr,
                      Uint32 size,
                      Uint32 socketWriteTimeout);

    void close();

    void disableBlocking();

    static void initializeInterface();

    static void uninitializeInterface();

    SocketHandle getSocket() {return _socket;}

    /**
        Accepts the connection, performing the necessary SSL handshake.

        @return Returns -1 on failure, 0 if not enough data is available to
        complete the operation (retry needed), and 1 on success.
     */
    Sint32 accept();

    Sint32 connect(Uint32 timeoutMilliseconds);

    Boolean isPeerVerificationEnabled();

    Boolean isCertificateVerified();

    /**
        Gets peer certificate chain.

        @return array of SSLCertificateInfo pointers if there is an
                    SSLCallbackInfo pointer,
                Otherwise an empty array
     */
    Array<SSLCertificateInfo*> getPeerCertificateChain();

private:

    SSL * _SSLConnection;
    SocketHandle _socket;
    SSLContext * _SSLContext;
    ReadWriteSem * _sslContextObjectLock;
    Uint32 _sslReadErrno;

    AutoPtr<SSLCallbackInfo> _SSLCallbackInfo;
    Boolean _certificateVerified;
};
#else

// offer a non ssl dummy class for use in MP_Socket

class SSLSocket {};

#endif // end of PEGASUS_HAS_SSL

//
// MP_Socket (Multi-purpose Socket class
//

class MP_Socket {

public:
    MP_Socket(SocketHandle socket);                          // "normal" socket

    MP_Socket(
        SocketHandle socket,
        SSLContext * sslcontext,
        ReadWriteSem * sslContextObjectLock);

    ~MP_Socket();

    Boolean isSecure();

    Boolean incompleteSecureReadOccurred(Sint32 retCode);

    SocketHandle getSocket();

    Sint32 read(void* ptr, Uint32 size);

    Sint32 write(const void* ptr, Uint32 size);

    void close();

    void disableBlocking();

    /**
        Accepts the connection, performing an SSL handshake if applicable.

        @return Returns -1 on failure, 0 if not enough data is available to
        complete the operation (retry needed), and 1 on success.
     */
    Sint32 accept();

    Sint32 connect(Uint32 timeoutMilliseconds);

    Boolean isPeerVerificationEnabled();

    Array<SSLCertificateInfo*> getPeerCertificateChain();

    Boolean isCertificateVerified();

    void setSocketWriteTimeout(Uint32 socketWriteTimeout);

#ifdef PEGASUS_OS_ZOS
    // Return the authenicated user name
    String getAuthenticatedUser() { return String(_username); }
    Boolean isClientAuthenticated() { return _userAuthenticated; }
#endif

    union {
        SocketHandle _socket;
        SSLSocket *_sslsock;
    };

private:
    Boolean   _isSecure;
    Uint32    _socketWriteTimeout;
#ifdef PEGASUS_OS_ZOS

    int ATTLS_zOS_query();
    // The user name if authenticated through ATTLS.
    char _username[10];
    Boolean _userAuthenticated;
#endif

};




PEGASUS_NAMESPACE_END

#endif /* Pegasus_TLS_h */
