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
// Author: Mike Brasher (mbrasher@bmc.com)
//
// Modified By:
//
//%/////////////////////////////////////////////////////////////////////////////

#include "Socket.h"

#ifdef PEGASUS_OS_TYPE_WINDOWS
#include <windows.h>
# ifndef _WINSOCKAPI_
#   include <winsock2.h>
# endif
#else
# include <cctype>
#ifndef PEGASUS_OS_OS400
#   include <unistd.h>
#else
#   include <Pegasus/Common/OS400ConvertChar.h>
#   include <unistd.cleinc>
#endif
#ifdef PEGASUS_OS_ZOS
#   include <string.h>  // added by rk for memcpy
#endif
# include <cstdlib>
# include <errno.h>
# include <fcntl.h>
# include <netdb.h>
# include <netinet/in.h>
# include <arpa/inet.h>
# include <sys/socket.h>
# include <errno.h>
#endif

#include <Pegasus/Common/Sharable.h>
PEGASUS_NAMESPACE_BEGIN

static Uint32 _socketInterfaceRefCount = 0;

Sint32 Socket::read(Sint32 socket, void* ptr, Uint32 size)
{
#ifdef PEGASUS_OS_TYPE_WINDOWS
    return ::recv(socket, (char*)ptr, size, 0);
#elif defined(PEGASUS_OS_ZOS)
    int i=::read(socket, (char*)ptr, size);
#ifdef PEGASUS_HAVE_EBCDIC
    __atoe_l((char *)ptr,size);
#endif
    return i;
#elif defined(PEGASUS_OS_OS400)
    int i=::read(socket, (char*)ptr, size);
    AtoE((char *)ptr, size);
    return i;
#else

#if defined (__GNUC__)
    int ccode = TEMP_FAILURE_RETRY(::read(socket, (char*)ptr, size));
    return ccode;
#else 
    return ::read(socket, (char*)ptr, size);
#endif
#endif
}

Sint32 Socket::write(Sint32 socket, const void* ptr, Uint32 size)
{
#ifdef PEGASUS_OS_TYPE_WINDOWS
    return ::send(socket, (const char*)ptr, size, 0);
#elif defined(PEGASUS_OS_ZOS)
    char * ptr2 = (char *)malloc(size);
    int i;
    memcpy(ptr2,ptr,size);
#ifdef PEGASUS_HAVE_EBCDIC
    __etoa_l(ptr2,size);
#endif
    i = ::write(socket, ptr2, size);
    free(ptr2);
    return i;
#elif defined(PEGASUS_OS_OS400)
    char * ptr2 = (char *)malloc(size);
    int i;
    memcpy(ptr2,ptr,size);
    EtoA(ptr2,size);
    i = ::write(socket, ptr2, size);
    free(ptr2);
    return i;
#else
#if (__GNUC__)
    int ccode = TEMP_FAILURE_RETRY(::write(socket, (char*)ptr, size));
    return ccode;
#else
    return ::write(socket, (char*)ptr, size);
#endif
#endif
}

void Socket::close(Sint32 socket)
{
#ifdef PEGASUS_OS_TYPE_WINDOWS
    closesocket(socket);
#else
#if (__GNUC__)
    TEMP_FAILURE_RETRY(::close(socket));
#else
    ::close(socket);
#endif
#endif
}

void Socket::enableBlocking(Sint32 socket)
{
#ifdef PEGASUS_OS_TYPE_WINDOWS
    unsigned long flag = 0;
    ioctlsocket(socket, FIONBIO, &flag);
#else
    int flags = fcntl(socket, F_GETFL, 0);
    flags &= ~O_NONBLOCK;
    fcntl(socket, F_SETFL, flags);
#endif
}

void Socket::disableBlocking(Sint32 socket)
{
#ifdef PEGASUS_OS_TYPE_WINDOWS
    unsigned long flag = 1;
    ioctlsocket(socket, FIONBIO, &flag);
#else
    int flags = fcntl(socket, F_GETFL, 0);
    flags |= O_NONBLOCK;
    fcntl(socket, F_SETFL, flags);
#endif
}

void Socket::initializeInterface()
{
#ifdef PEGASUS_OS_TYPE_WINDOWS
    if (_socketInterfaceRefCount == 0)
    {
	WSADATA tmp;

	if (WSAStartup(0x202, &tmp) == SOCKET_ERROR)
	    WSACleanup();
    }

    _socketInterfaceRefCount++;
#endif
}

void Socket::uninitializeInterface()
{
#ifdef PEGASUS_OS_TYPE_WINDOWS
    _socketInterfaceRefCount--;

    if (_socketInterfaceRefCount == 0)
	WSACleanup();
#endif
}


class abstract_socket : public Sharable
{
   public:
      abstract_socket(void) { }
      virtual ~abstract_socket(void){ }
      
      virtual operator Sint32() const = 0;
      virtual int socket(int sock_type, int sock_style, int sock_protocol, void *ssl_context = 0) = 0;
      
      virtual Sint32 read(void* ptr, Uint32 size) = 0;
      virtual Sint32 write(const void* ptr, Uint32 size) = 0;
      virtual int close(void) = 0;
      virtual void enableBlocking(void) = 0;
      virtual void disableBlocking(void) = 0;

      virtual int getsockname (struct sockaddr *addr, size_t *length_ptr) = 0;
      virtual int bind (struct sockaddr *addr, size_t length) = 0;
     
      // change size_t to size_t for ZOS and windows
      virtual abstract_socket* accept(struct sockaddr *addr, size_t *length_ptr) = 0;
      virtual int connect (int socket, struct sockaddr *addr, size_t length) = 0;
      virtual int shutdown(int how) = 0;
      virtual int listen(int q) = 0;
      virtual int getpeername (struct sockaddr *addr, size_t *length_ptr) = 0;
      virtual int send (void *buffer, size_t size, int flags) = 0;
      virtual int recv (void *buffer, size_t size, int flags) = 0;
      virtual int sendto(void *buffer, size_t size, int flags, struct sockaddr *addr, size_t length) = 0;
      virtual int recvfrom(void *buffer, size_t size, int flags, struct sockaddr *addr, size_t *length_ptr) = 0;
      virtual int setsockopt (int level, int optname, void *optval, size_t optlen) = 0;
      virtual int getsockopt (int level, int optname, void *optval, size_t *optlen_ptr) = 0;

      virtual Boolean incompleteReadOccurred(Sint32 retCode) = 0;
      virtual Boolean is_secure(void) = 0;
      virtual Boolean is_initialized(void) = 0;
      virtual void set_close_on_exec(void) = 0;
      virtual const char* get_err_string(void) = 0;
      

      
   private:

      abstract_socket(const abstract_socket& );
      abstract_socket& operator=(const abstract_socket& );
};


/** 
 * designed to be overriden by ssl_socket_rep
 */
class bsd_socket_rep : public abstract_socket
{
   public:

      /** 
       * map to pegasus_socket::sock_err 
       */

      bsd_socket_rep(void);
      virtual ~bsd_socket_rep(void);
      
      operator Sint32() const;
      int socket(int sock_type, int sock_style, int sock_protocol, void *ssl_context = 0);
      
      virtual Sint32 read(void* ptr, Uint32 size);
      virtual Sint32 write(const void* ptr, Uint32 size);
      virtual int close(void);
      void enableBlocking(void);
      void disableBlocking(void);

      int getsockname (struct sockaddr *addr, size_t *length_ptr);
      int bind (struct sockaddr *addr, size_t length);
     
      // change size_t to size_t for ZOS and windows
      virtual abstract_socket* accept(struct sockaddr *addr, size_t *length_ptr);
      int connect (int socket, struct sockaddr *addr, size_t length);
      int shutdown(int how);
      int listen(int q);
      int getpeername (struct sockaddr *addr, size_t *length_ptr);
      virtual int send (void *buffer, size_t size, int flags);
      virtual int recv (void *buffer, size_t size, int flags);
      int sendto(void *buffer, size_t size, int flags, struct sockaddr *addr, size_t length);
      int recvfrom(void *buffer, size_t size, int flags, struct sockaddr *addr, size_t *length_ptr);
      int setsockopt (int level, int optname, void *optval, size_t optlen);
      int getsockopt (int level, int optname, void *optval, size_t *optlen_ptr);

      Boolean incompleteReadOccurred(Sint32 retCode);
      virtual Boolean is_secure(void);
      virtual Boolean is_initialized(void);
      void set_close_on_exec(void);
      
   private:

      // use to perform one-time initializion and destruction
      struct _library_init
      {
	    _library_init(void)
	    {
	    }
	    ~_library_init(void)
	    {
	    }
      };

      static struct _library_init _lib_init;
      
      int _socket;
      void *_ssl_context;
      AtomicInt _initialized;
      int _errno;
};

#if defined(__GNUC__) && GCC_VERSION >= 30200
// TEMP_FAILURE_RETRY (expression)
#else

#endif


/** 
 * default constructor for an (uninitialized) bsd socket 
 */
bsd_socket_rep::bsd_socket_rep(void)
   :_errno(0)
{
}


/** 
 * default destructor for bsd_socket_rep
 *
 */
bsd_socket_rep::~bsd_socket_rep(void)
{
   
}

int bsd_socket_rep::close(void)
{
   shutdown(2);
   return ::close(_socket);
}


int bsd_socket_rep::shutdown(int how)
{
   return ::shutdown(_socket, how);
}

bsd_socket_rep::operator Sint32() const
{
   return (Sint32)_socket;
}

int bsd_socket_rep::socket(int sock_type, int sock_style, int sock_protocol, void *ssl_context)
{
   _socket = ::socket(sock_type, sock_style, sock_protocol);
   if(-1 == _socket )
   {
      _errno = errno;
   }
   return _socket;
}


Sint32 bsd_socket_rep::read(void* ptr, Uint32 size)
{
   return Socket::read(_socket, ptr, size);
}



PEGASUS_NAMESPACE_END
