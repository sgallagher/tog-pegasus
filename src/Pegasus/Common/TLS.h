//%/////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001 BMC Software, Hewlett-Packard Company, IBM, 
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
// Author: Markus Mueller (sedgewick_de@yahoo.de)
//
// Modified By:
//
//%/////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_TLS_h
#define Pegasus_TLS_h

#ifdef PEGASUS_HAS_SSL
#include <openssl/err.h>
#include <openssl/ssl.h>
#include <openssl/rand.h>
#else
#define SSL_CTX void
#endif
#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/String.h>
#include <Pegasus/Common/Exception.h>

// REVIEW: Figure out how this works (note to myself)?

PEGASUS_NAMESPACE_BEGIN

class PEGASUS_EXPORT SSLContext
{
public:

   SSLContext(const String& certPath,
              const String& randomFile = String::EMPTY,
              Boolean isCIMClient = false)
              throw(SSL_Exception);

    ~SSLContext();

    SSL_CTX * getContext();

private:

    SSL_CTX * _SSLContext;

    char * _certPath;
};

#ifdef PEGASUS_HAS_SSL
class PEGASUS_EXPORT SSLSocket
{
public:

    SSLSocket(Sint32 socket, SSLContext * sslcontext)
        throw(SSL_Exception);

    ~SSLSocket();

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

private:

    X509 *   _SSLCertificate;
    SSL * _SSLConnection;

    Sint32 _socket;
    SSLContext * _SSLContext;
};
#else

// offer a non ssl dummy class for use in MP_Socket

typedef PEGASUS_EXPORT void * SSLSocket;

#endif

//
// MP_Socket (Multi-purpose Socket class
//

class MP_Socket {

public:
    MP_Socket(Uint32 socket);                          // "normal" socket

    MP_Socket(Uint32 socket, SSLContext * sslcontext)
         throw(SSL_Exception);                         // secure socket

    ~MP_Socket();

    Boolean isSecure();

    Sint32 getSocket();

    Sint32 read(void* ptr, Uint32 size);

    Sint32 write(const void* ptr, Uint32 size);

    void close();

    void enableBlocking();

    void disableBlocking();

    Sint32 accept();

    Sint32 connect();

    union {
        Uint32    _socket;
        SSLSocket *_sslsock;
    };

private:
    Boolean   _isSecure;
};

PEGASUS_NAMESPACE_END

#endif /* Pegasus_TLS_h */
