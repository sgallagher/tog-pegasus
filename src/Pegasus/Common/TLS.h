//%2004////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001, 2002 BMC Software; Hewlett-Packard Development
// Company, L.P.; IBM Corp.; The Open Group; Tivoli Systems.
// Copyright (c) 2003 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation, The Open Group.
// Copyright (c) 2004 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation; VERITAS Software Corporation; The Open Group.
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
// Author: Markus Mueller (sedgewick_de@yahoo.de)
//
// Modified By:
//         Nag Boranna, Hewlett-Packard Company (nagaraja_boranna@hp.com)
//         Heather Sterling, IBM (hsterl@us.ibm.com)
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
#include <Pegasus/Common/IPC.h>
#include <Pegasus/Common/Linkage.h>

// REVIEW: Figure out how this works (note to myself)?


PEGASUS_NAMESPACE_BEGIN


#ifdef PEGASUS_HAS_SSL
class PEGASUS_COMMON_LINKAGE SSLSocket
{
public:

    SSLSocket(Sint32 socket, SSLContext * sslcontext,
        Boolean exportConnection = false)
        throw(SSLException);

    ~SSLSocket();

    Boolean incompleteReadOccurred(Sint32 retCode);

    Sint32 read(void* ptr, Uint32 size);

    Sint32 write(const void* ptr, Uint32 size);

    void close();

    void enableBlocking();

    void disableBlocking();

    static void initializeInterface();

    static void uninitializeInterface();

    Sint32 getSocket() {return _socket;}

    Sint32 accept();

    Sint32 connect();

    Boolean isPeerVerificationEnabled();
    
    Boolean isCertificateVerified();

    SSLCertificateInfo* getPeerCertificate();

    Boolean addTrustedClient(const char* username);

private:

    SSL * _SSLConnection;
    Sint32 _socket;
    SSLContext * _SSLContext;

    SSLCallbackInfo* _SSLCallbackInfo;
    Boolean _certificateVerified;
    Boolean _exportConnection;
};
#else

// offer a non ssl dummy class for use in MP_Socket

class PEGASUS_COMMON_LINKAGE SSLSocket {};

#endif // end of PEGASUS_HAS_SSL

//
// MP_Socket (Multi-purpose Socket class
//

class MP_Socket {

public:
    MP_Socket(Uint32 socket);                          // "normal" socket

    MP_Socket(Uint32 socket, SSLContext * sslcontext,
        Boolean exportConnection = false)
        throw(SSLException);                          // secure socket

    ~MP_Socket();

    Boolean isSecure();

    Boolean incompleteReadOccurred(Sint32 retCode);

    Sint32 getSocket();

    Sint32 read(void* ptr, Uint32 size);

    Sint32 write(const void* ptr, Uint32 size);

    void close();

    void enableBlocking();

    void disableBlocking();

    Sint32 accept();

    Sint32 connect();

    Boolean isPeerVerificationEnabled();

    SSLCertificateInfo* getPeerCertificate();

    Boolean isCertificateVerified();

    Boolean addTrustedClient(const char* username);

    union {
        Uint32    _socket;
        SSLSocket *_sslsock;
    };

private:
    Boolean   _isSecure;
};




PEGASUS_NAMESPACE_END

#endif /* Pegasus_TLS_h */
