//%/////////-*-c++-*-///////////////////////////////////////////////////////////
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
// Author: Markus Mueller (sedgewick_de@yahoo.de)
//
// Modified By:
//         Nag Boranna, Hewlett-Packard Company (nagaraja_boranna@hp.com)
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
#endif // end of PEGASUS_HAS_SSL

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/String.h>
#include <Pegasus/Common/InternalException.h>
#include <Pegasus/Common/SSLContext.h>
#include <Pegasus/Common/Linkage.h>

// REVIEW: Figure out how this works (note to myself)?


PEGASUS_NAMESPACE_BEGIN


#ifdef PEGASUS_HAS_SSL
class PEGASUS_COMMON_LINKAGE SSLSocket
{
public:

    SSLSocket(Sint32 socket, SSLContext * sslcontext)
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

private:

    X509 *   _SSLCertificate;
    SSL * _SSLConnection;

    Sint32 _socket;
    SSLContext * _SSLContext;
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

    MP_Socket(Uint32 socket, SSLContext * sslcontext)
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

    union {
        Uint32    _socket;
        SSLSocket *_sslsock;
    };

private:
    Boolean   _isSecure;
};


//  <<< Thu Jul  3 13:50:29 2003 mdd >>> pep_88
/*****************************************************************
 *
 *  The socket support in pegasus is schizophrenic. Some code uses 
 *  an Sint32 (fd) as a socket, while other code uses a pointer to an 
 *  MP_Socket, which is kind of a container for either an Sint32 socket 
 *  or an SSL socket. 
 *
 *  Then there is also the local socket. (AF_UNIX). 
 *
 *  What we need to make all of this coherent is a general-purpose
 *  socket class that uses polymorphism to provide a good sockets
 *  interface.
 *  Because of what we are planning for the pep_88 connection management
 *  code this general-purpose socket class should be reference counted.
 *
 *****************************************************************/ 



class PEGASUS_COMMON_LINKAGE abstract_socket 
{

   public:
      abstract_socket(void);
      abstract_socket(int type, int style, int protocol);
      abstract_socket(int type, int style, int protocol, SSL_Context * ssl_context);
      abstract_socket(const abstract_socket& s);
      ~abstract_socket(void);
      
      abstract_socket& operator=(const abstract_socket& s);
      operator Sint32() const;

      socket(int type, int style, int protocol);
      socket(int type, int style, int protocol, SSL_Context *ssl_context);
      
      
      
      Sint32 read(void* ptr, Uint32 size);
      Sint32 write(const void* ptr, Uint32 size);
      void close(void);
      void enableBlocking(void);
      void disableBlocking(void);
      void initializeInterface(void);
      void uninitializeInterface(void);
      

      int getsockname (struct sockaddr *addr, socklen_t *length-ptr);
      int bind (socket, struct sockaddr *addr, socklen_t length);
     
      // change socklen_t to size_t for ZOS and windows
      abstract_socket accept (struct sockaddr *addr, socklen_t *length-ptr);
      int connect (int socket, struct sockaddr *addr, socklen_t length);
      int shutdown(int how);
      int listen(int q);
      int getpeername (struct sockaddr *addr, size_t *length-ptr);
      int send (void *buffer, size_t size, int flags);
      int recv (void *buffer, size_t size, int flags);
      int sendto(void *buffer. size_t size, int flags, struct sockaddr *addr, socklen_t length);
      int recvfrom(void *buffer, size_t size, int flags, struct sockaddr *addr, socklen_t *length-ptr);
      int setsockopt (int level, int optname, void *optval, socklen_t optlen);
      int getsockopt (int level, int optname, void *optval, socklen_t *optlen-ptr);


      Boolean incompleteReadOccurred(Sint32 retCode);
      Boolean is_secure(void);
      void set_close_on_exec(void);
      

   private:
      class abstract_socket_rep;
      abstract_socket_rep * _rep;

      int _type;
      int _style;
      int _protocol;

      AtomicInt _initialized;
      
};





PEGASUS_NAMESPACE_END

#endif /* Pegasus_TLS_h */
