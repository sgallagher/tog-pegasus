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
#   include <string.h>  // added by rk for memcpy
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

int Socket::close2(Sint32 socket)
{
#ifdef PEGASUS_OS_TYPE_WINDOWS
    return closesocket(socket);
#else
#if (__GNUC__)
    int ccode = TEMP_FAILURE_RETRY(::close(socket));
    return ccode;
#else
    return ::close(socket);
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

int Socket::enableBlocking2(Sint32 socket)
{
#ifdef PEGASUS_OS_TYPE_WINDOWS
    unsigned long flag = 0;
    return ioctlsocket(socket, FIONBIO, &flag);
#else
    int flags = fcntl(socket, F_GETFL, 0);
    flags &= ~O_NONBLOCK;
    return fcntl(socket, F_SETFL, flags);
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

int Socket::disableBlocking2(Sint32 socket)
{
#ifdef PEGASUS_OS_TYPE_WINDOWS
    unsigned long flag = 1;
    return ioctlsocket(socket, FIONBIO, &flag);
#else
    int flags = fcntl(socket, F_GETFL, 0);
    flags |= O_NONBLOCK;
    return fcntl(socket, F_SETFL, flags);
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
      virtual int enableBlocking(void) = 0;
      virtual int disableBlocking(void) = 0;

      virtual int getsockname (struct sockaddr *addr, size_t *length_ptr) = 0;
      virtual int bind (struct sockaddr *addr, size_t length) = 0;
     
      // change size_t to size_t for ZOS and windows
      virtual abstract_socket* accept(struct sockaddr *addr, size_t *length_ptr) = 0;
      virtual int connect (struct sockaddr *addr, size_t length) = 0;
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
      virtual void set_close_on_exec(void) = 0;
      virtual const char* get_err_string(void) = 0;
      
   private:

      abstract_socket(const abstract_socket& );
      abstract_socket& operator=(const abstract_socket& );
};


/** 
 * internet socket class
 * designed to be overriden by ssl_socket_rep and file_socket_rep
 * 
 */
class bsd_socket_rep : public abstract_socket
{
   public:

      /** 
       * map to pegasus_socket::sock_err 
       */

      bsd_socket_rep(void);
      virtual ~bsd_socket_rep(void);
      // used to allow the accept method to work
      bsd_socket_rep(int sock);

      operator Sint32() const;
      int socket(int sock_type, int sock_style, int sock_protocol, void *ssl_context = 0);
      
      virtual Sint32 read(void* ptr, Uint32 size);
      virtual Sint32 write(const void* ptr, Uint32 size);
      virtual int close(void);
      int enableBlocking(void);
      int disableBlocking(void);

      virtual int getsockname (struct sockaddr *addr, size_t *length_ptr);
      virtual int bind (struct sockaddr *addr, size_t length);
     
      // change size_t to size_t for ZOS and windows
      virtual abstract_socket* accept(struct sockaddr *addr, size_t *length_ptr);
      virtual int connect (struct sockaddr *addr, size_t length);
      virtual int shutdown(int how);
      virtual int listen(int queue_len );
      int getpeername (struct sockaddr *addr, size_t *length_ptr);
      virtual int send (void *buffer, size_t size, int flags);
      virtual int recv (void *buffer, size_t size, int flags);
      virtual int sendto(void *buffer, size_t size, int flags, struct sockaddr *addr, size_t length);
      virtual int recvfrom(void *buffer, size_t size, int flags, struct sockaddr *addr, size_t *length_ptr);
      int setsockopt (int level, int optname, void *optval, size_t optlen);
      int getsockopt (int level, int optname, void *optval, size_t *optlen_ptr);

      virtual Boolean incompleteReadOccurred(Sint32 retCode);
      virtual Boolean is_secure(void);
      void set_close_on_exec(void);
      virtual const char* get_err_string(void);

   private:


      bsd_socket_rep& operator=(const bsd_socket_rep& );
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
      int _errno;
      char _strerr[256];
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

bsd_socket_rep::bsd_socket_rep(int sock)
   : _socket(sock)
{
}


/** 
 * default destructor for bsd_socket_rep
 *
 */
bsd_socket_rep::~bsd_socket_rep(void)
{
   
}

int bsd_socket_rep::shutdown(int how)
{
   int ccode = ::shutdown(_socket, how);
   if(ccode == -1)
      _errno = errno;
   return ccode;
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
   int ccode = Socket::read(_socket, ptr, size);
   if(ccode == -1)
      _errno = errno;
   return ccode;
}


Sint32 bsd_socket_rep::write(const void* ptr, Uint32 size)
{
   int ccode = Socket::write(_socket, ptr, size);
   if(ccode == -1)
      errno = _errno;
   return ccode;
}


int bsd_socket_rep::close(void)
{
   int ccode;
   
   shutdown(2);
   ccode = Socket::close2(_socket);
   if(ccode == -1)
      _errno = errno;
   return ccode;
}



int  bsd_socket_rep::enableBlocking(void)
{
   return Socket::enableBlocking2(_socket);
}

int bsd_socket_rep::disableBlocking(void)
{
   return Socket::disableBlocking2(_socket);
}

int bsd_socket_rep::getsockname (struct sockaddr *addr, size_t *length_ptr)
{
   int ccode = ::getsockname(_socket, addr, length_ptr);
   if(ccode == -1)
      _errno = errno;
   return ccode;
}

/** 
 * default implementation allows reuseof address
 * sockaddr structure needs to be fully initialized or call will fail
 */
int bsd_socket_rep::bind (struct sockaddr *addr, size_t length)
{
   int opt = 1;
   int ccode = setsockopt(SOL_SOCKET, SO_REUSEADDR, (char *)&opt, sizeof(opt));
   if(ccode < 0)
   {
      _errno = errno;
      return ccode;
   }
   return ::bind(_socket, addr, length);
}

abstract_socket* bsd_socket_rep::accept(struct sockaddr *addr, size_t *length_ptr)
{
   int new_sock = ::accept(_socket, addr, length_ptr);
   if(new_sock == -1)
   {
      _errno = errno;
      return 0;
   }

   bsd_socket_rep *rep = new bsd_socket_rep(new_sock);
   // set the close-on-exec flag 
   rep->set_close_on_exec();
   return rep;
}

int bsd_socket_rep::connect (struct sockaddr *addr, size_t length)
{
   int ccode = ::connect(_socket, addr, length);
   if(ccode == -1)
      _errno = errno;
   return ccode;
}


int bsd_socket_rep::listen(int queue_len)
{
   int ccode = ::listen(_socket, queue_len);
   if(ccode == -1)
      _errno = errno;
   return ccode;
}


int bsd_socket_rep::getpeername (struct sockaddr *addr, size_t *length_ptr)
{
   int ccode = ::getpeername(_socket, addr, length_ptr);
   if(ccode == -1 )
      _errno = errno;
   return ccode;
}

int bsd_socket_rep::send (void *buffer, size_t size, int flags)
{
   int ccode = ::send(_socket, buffer, size, flags);
   if(ccode == -1)
      _errno = errno;
   return ccode;
}


int bsd_socket_rep::recv (void *buffer, size_t size, int flags)
{
   int ccode = ::recv(_socket, buffer, size, flags);
   if(ccode == -1)
      _errno = errno;
   return ccode;
}

int bsd_socket_rep::sendto(void *buffer, size_t size, int flags, struct sockaddr *addr, size_t length)
{
   int ccode = ::sendto(_socket, buffer, size, flags, addr, length);
      if(ccode == -1)
      _errno = errno;
   return ccode;
}


int bsd_socket_rep::recvfrom(void *buffer, 
			     size_t size, 
			     int flags, 
			     struct sockaddr *addr, 
			     size_t *length_ptr)
{
   int ccode = ::recvfrom(_socket, buffer, size, flags, addr, length_ptr);
   if(ccode == -1)
      _errno = errno;
   return ccode;
}

int bsd_socket_rep::setsockopt (int level, int optname, void *optval, size_t optlen)
{
   int ccode = ::setsockopt(_socket, level, optname, optval, optlen);
   if(ccode == -1)
      _errno = errno;
   return ccode;
}


int bsd_socket_rep::getsockopt (int level, int optname, void *optval, size_t *optlen_ptr)
{
   int ccode = ::getsockopt(_socket, level, optname, optval, optlen_ptr);
   if(ccode == -1)
      _errno = errno;
   return ccode;
}


Boolean bsd_socket_rep::incompleteReadOccurred(Sint32 retCode)
{
   return false;
}

Boolean bsd_socket_rep::is_secure(void)
{
   return false;
}

void bsd_socket_rep::set_close_on_exec(void)
{
   int sock_flags;
   if( (sock_flags = fcntl(_socket, F_GETFD, 0)) >= 0)
   {
      sock_flags |= FD_CLOEXEC;
      fcntl(_socket, F_SETFD, sock_flags);
   }
}


const char* bsd_socket_rep::get_err_string(void)
{
   strncpy(_strerr, strerror(_errno), 255);
   return _strerr;
}



PEGASUS_NAMESPACE_END

