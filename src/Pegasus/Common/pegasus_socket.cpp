//%2003////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001, 2002  BMC Software, Hewlett-Packard Development
// Company, L. P., IBM Corp., The Open Group, Tivoli Systems.
// Copyright (c) 2003 BMC Software; Hewlett-Packard Development Company, L. P.;
// IBM Corp.; EMC Corporation, The Open Group.
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
// Author: Mike Day (mdday@us.ibm.com)
//
// Modified By:  Amit Arora (amita@in.ibm.com) for Bug#1170
//
//%/////////////////////////////////////////////////////////////////////////////


// << Thu Aug 14 15:00:35 2003 mdd >> domain sockets and socket tests work
#include "pegasus_socket.h"
#include <Pegasus/Common/Socket.h>
#include <Pegasus/Common/TLS.h>
#include <Pegasus/Common/System.h>
#include <Pegasus/Common/Tracer.h>

#ifdef PEGASUS_OS_TYPE_WINDOWS
#include <windows.h>
# ifndef _WINSOCKAPI_
#   include <winsock2.h>
# endif
#else
# include <cctype>
#ifndef PEGASUS_OS_OS400
//#   include <unistd.h>
#else
#   include <unistd.cleinc>
#endif
#   include <string.h>
# include <cstdlib>
# include <errno.h>
#ifdef PEGASUS_OS_TYPE_WINDOWS
#else
# include <fcntl.h>
#endif
# include <netdb.h>
# include <netinet/in.h>
# include <arpa/inet.h>
# include <sys/socket.h>
#endif

# ifdef PEGASUS_LOCAL_DOMAIN_SOCKET
//#  include <unistd.h>
#  include <sys/un.h>
#  include <Pegasus/Common/Constants.h>
# endif


#include <Pegasus/Common/Sharable.h>

PEGASUS_NAMESPACE_BEGIN

#ifdef PEGASUS_OS_TYPE_WINDOWS
 #ifdef errno
 #undef errno
 #endif

 #define errno WSAGetLastError()
#endif

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
   close();
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
      _errno = errno;
   return ccode;
}


int bsd_socket_rep::close(void)
{
   int ccode;

   shutdown(2);
   ccode = Socket::close2(_socket);
   if(ccode == -1)
   {
      _errno = errno;
   }
   
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

int bsd_socket_rep::getsockname (struct sockaddr *addr, PEGASUS_SOCKLEN_SIZE *length_ptr)
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
int bsd_socket_rep::bind (struct sockaddr *addr, PEGASUS_SOCKLEN_SIZE length)
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

abstract_socket* bsd_socket_rep::accept(struct sockaddr *addr, PEGASUS_SOCKLEN_SIZE *length_ptr)
{

#if defined(PEGASUS_OS_TYPE_WINDOWS)
   int new_sock = ::accept(_socket, addr, length_ptr);
#else 
   int new_sock = ::accept(_socket, addr, length_ptr);
   if(new_sock == -1 && errno == EAGAIN)
   {
      int retries = 0;
      
      do 
      {
	 pegasus_sleep(1);
	 new_sock = ::accept(_socket, addr, length_ptr);
	 retries++;
      } while(new_sock == -1 && errno == EAGAIN && retries < 20);
      
   }
#endif 
   if(new_sock == -1)
   {
      _errno = errno;
//      return 0;
   }

   bsd_socket_rep *rep = new bsd_socket_rep(new_sock);
   // set the close-on-exec flag
   rep->set_close_on_exec();
   return rep;
}

int bsd_socket_rep::connect (struct sockaddr *addr, PEGASUS_SOCKLEN_SIZE length)
{
   int ccode = ::connect(_socket, addr, length);
   if(ccode == -1)
   {
      _errno = errno;
   }
   
   return ccode;
}


int bsd_socket_rep::listen(int queue_len)
{
   int ccode = ::listen(_socket, queue_len);
   if(ccode == -1)
      _errno = errno;
   return ccode;
}


int bsd_socket_rep::getpeername (struct sockaddr *addr, PEGASUS_SOCKLEN_SIZE *length_ptr)
{
   int ccode = ::getpeername(_socket, addr, length_ptr);
   if(ccode == -1 )
      _errno = errno;
   return ccode;
}

int bsd_socket_rep::send (void *buffer, PEGASUS_SOCKLEN_SIZE size, int flags)
{
   #if defined(PEGASUS_OS_TYPE_WINDOWS) || defined(PEGASUS_OS_OS400) || defined(SUNOS_5_6)
   int ccode = ::send(_socket, (char *)buffer, size, flags);
   #else
   int ccode = ::send(_socket, buffer, size, flags);
   #endif

   if(ccode == -1)
      _errno = errno;
   return ccode;
}


int bsd_socket_rep::recv (void *buffer, PEGASUS_SOCKLEN_SIZE size, int flags)
{
   #if defined(PEGASUS_OS_TYPE_WINDOWS) || defined(PEGASUS_OS_OS400) || defined(SUNOS_5_6)
   int ccode = ::recv(_socket, (char *)buffer, size, flags);
   #else
   int ccode = ::recv(_socket, buffer, size, flags);
   #endif

   if(ccode == -1)
      _errno = errno;
   return ccode;
}

int bsd_socket_rep::sendto(void *buffer, PEGASUS_SOCKLEN_SIZE size, int flags, struct sockaddr *addr, PEGASUS_SOCKLEN_SIZE length)
{
   #if defined(PEGASUS_OS_TYPE_WINDOWS) || defined(PEGASUS_OS_OS400) || defined(SUNOS_5_6)
   int ccode = ::sendto(_socket, (char *)buffer, size, flags, addr, length);
   #else
   int ccode = ::sendto(_socket, buffer, size, flags, addr, length);
   #endif

   if(ccode == -1)
      _errno = errno;
   return ccode;
}


int bsd_socket_rep::recvfrom(void *buffer,
			     PEGASUS_SOCKLEN_SIZE size,
			     int flags,
			     struct sockaddr *addr,
			     PEGASUS_SOCKLEN_SIZE *length_ptr)
{
   #if defined(PEGASUS_OS_TYPE_WINDOWS) || defined(PEGASUS_OS_OS400) || defined(SUNOS_5_6)
   int ccode = ::recvfrom(_socket, (char *)buffer, size, flags, addr, length_ptr);
   #else
   int ccode = ::recvfrom(_socket, buffer, size, flags, addr, length_ptr);
   #endif

   if(ccode == -1)
      _errno = errno;
   return ccode;
}

int bsd_socket_rep::setsockopt (int level, int optname, void *optval, PEGASUS_SOCKLEN_SIZE optlen)
{
   #if defined(PEGASUS_OS_TYPE_WINDOWS) || defined(PEGASUS_OS_OS400) || defined(SUNOS_5_6)
   int ccode = ::setsockopt(_socket, level, optname, (char *)optval, optlen);
   #else
   int ccode = ::setsockopt(_socket, level, optname, optval, optlen);
   #endif

   if(ccode == -1)
      _errno = errno;
   return ccode;
}


int bsd_socket_rep::getsockopt (int level, int optname, void *optval, PEGASUS_SOCKLEN_SIZE *optlen_ptr)
{
   #if defined(PEGASUS_OS_TYPE_WINDOWS) || defined(PEGASUS_OS_OS400) || defined(SUNOS_5_6)
   int ccode = ::getsockopt(_socket, level, optname, (char *)optval, optlen_ptr);
   #else
   int ccode = ::getsockopt(_socket, level, optname, optval, optlen_ptr);
   #endif

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
   #ifdef PEGASUS_OS_TYPE_WINDOWS
   // ATTN: needs implementation
   #else
   int sock_flags;
   if( (sock_flags = fcntl(_socket, F_GETFD, 0)) >= 0)
   {
      sock_flags |= FD_CLOEXEC;
      fcntl(_socket, F_SETFD, sock_flags);
   }
   #endif
}


const char* bsd_socket_rep::get_err_string(void)
{
   strncpy(_strerr, strerror(_errno), 255);
   return _strerr;
}


bsd_socket_factory::bsd_socket_factory(void)
{
}

bsd_socket_factory::~bsd_socket_factory(void)
{
}

abstract_socket* bsd_socket_factory::make_socket(void)
{
   return new bsd_socket_rep();
}



#ifdef PEGASUS_HAS_SSL

abstract_socket* bsd_socket_factory::make_socket(SSLContext *ctx)
{
   return new bsd_socket_rep();
}


/**
 * ssl_socket_rep - derived from bsd_socket
 *
 **/


class ssl_socket_rep : public bsd_socket_rep
{
   public:

      /**
       * map to pegasus_socket::sock_err
       */
      typedef bsd_socket_rep Base;

      ssl_socket_rep(void);
      ~ssl_socket_rep(void);
      ssl_socket_rep(SSLContext *);

      virtual Sint32 read(void* ptr, Uint32 size);
      virtual Sint32 write(const void* ptr, Uint32 size);
      virtual int close(void);
      virtual abstract_socket* accept(struct sockaddr *addr, PEGASUS_SOCKLEN_SIZE *length_ptr);
      virtual int connect (struct sockaddr *addr, PEGASUS_SOCKLEN_SIZE length);

      virtual Boolean incompleteReadOccurred(Sint32 retCode);
      virtual Boolean is_secure(void);
      const char* get_err_string(void);

      void set_ctx(SSLContext *);

   private:

      ssl_socket_rep& operator=(const ssl_socket_rep& );
      // use to perform one-time initializion and destruction
      struct ssl_library_init
      {
	    ssl_library_init(void)
	    {
	    }
	    ~ssl_library_init(void)
	    {
	    }
      };

      static struct ss_library_init _lib_init;

      Boolean _init(void);


      // << Thu Aug 14 12:29:21 2003 mdd >> _internal_socket
      // is created by this class, it is ok to delete
      // upon instance destruction
      SSLSocket* _internal_socket;

      // << Thu Aug 14 12:28:54 2003 mdd >> never delete
      // the ctx, it is created outside of this class
      SSLContext* _ctx;
      AtomicInt _initialized;

};

ssl_socket_rep::ssl_socket_rep(void)
   : Base(), _internal_socket(0), _ctx(0), _initialized(0)
{
}


ssl_socket_rep::ssl_socket_rep(SSLContext *ctx)
   :Base(), _internal_socket(0), _ctx(ctx), _initialized(0)
{
}

ssl_socket_rep::~ssl_socket_rep()
{
   delete _internal_socket;
}

void ssl_socket_rep::set_ctx(SSLContext *ctx)
{
   _ctx = ctx;
}

abstract_socket* ssl_socket_rep::accept(struct sockaddr *addr, PEGASUS_SOCKLEN_SIZE *length_ptr) {
  
#if defined(PEGASUS_OS_TYPE_WINDOWS)
  int new_sock = ::accept(_socket, addr, length_ptr);
#else
  int new_sock = ::accept(_socket, addr, length_ptr);
  if(new_sock == -1 && errno == EAGAIN)
    {
      int retries = 0;      
      do
	{
	  pegasus_sleep(1);
	  new_sock = ::accept(_socket, addr, length_ptr);
	  retries++;
        } while(new_sock == -1 && errno == EAGAIN && retries < 20);
      
    }
#endif
  
  if(new_sock == -1)
    {
      _errno = errno;
    }
  
     ssl_socket_rep *rep = new ssl_socket_rep(_ctx);
     
     rep->_socket = new_sock;
     if (rep->_init() == true) {
       _errno = rep->_internal_socket->accept();
     }
     rep->set_close_on_exec();
     return rep;
}

Boolean ssl_socket_rep::_init(void)
{
  if(_ctx == 0 ) {
    // This is a serious issue. If we don't have the SSL context we cannot do
    // anything.
    PEG_TRACE_STRING(TRC_SSL, Tracer::LEVEL4, "ssl_socket_rep::_init: Missing SSL context? Do you have the certs? ");    
      return false;
  }
   if(_internal_socket && _initialized.value())
      return true;
   if(_internal_socket != 0 && _initialized.value() == 0 )
      delete _internal_socket;

   _internal_socket = new SSLSocket(_socket, _ctx);
   if(_internal_socket)
   {
      _initialized = 1;
      return true;
   }
   return false;
}
int ssl_socket_rep::connect (struct sockaddr *addr, PEGASUS_SOCKLEN_SIZE length) {

  // IBM-KR: I've not had a chance to test this. 
  if (true == _init())
    return _internal_socket->connect();
  else
    return -1;

}
Sint32 ssl_socket_rep::read(void* ptr, Uint32 size)
{
   if(true == _init())
      return _internal_socket->read(ptr, size);
   else
      return -1;

}

Sint32 ssl_socket_rep::write(const void* ptr, Uint32 size)
{
   if(true == _init())
      return _internal_socket->write(ptr, size);
   else
      return -1;
}

int ssl_socket_rep::close(void)
{
   if(true == _init())
      _internal_socket->close();
   return Base::close();
}

Boolean ssl_socket_rep::incompleteReadOccurred(Sint32 retCode)
{
   if(true == _init())
      return _internal_socket->incompleteReadOccurred(retCode);
   else
      return false;
}

Boolean ssl_socket_rep::is_secure(void)
{
   return _init();
}

const char* ssl_socket_rep::get_err_string(void)
{
   return "ssl error string";
}


ssl_socket_factory::ssl_socket_factory(void)
{
}


ssl_socket_factory::~ssl_socket_factory(void)
{
}

abstract_socket* ssl_socket_factory::make_socket(void)
{
   return new ssl_socket_rep();
}

abstract_socket* ssl_socket_factory::make_socket(SSLContext *ctx)
{
   return new ssl_socket_rep(ctx);
}


#endif // PEGASUS_HAS_SSL
/**
 * pegasus_socket - the high level socket object in pegasus
 *
 **/

pegasus_socket::pegasus_socket(void)
{
   _rep = new empty_socket_rep();
}


pegasus_socket::pegasus_socket(socket_factory *factory)
{
  // This factory does NOT pass the SSL context information. It is good
  // _only_ for non-SSL sockets.
    _rep = factory->make_socket();
}
#ifdef PEGASUS_HAS_SSL
pegasus_socket::pegasus_socket(socket_factory *factory, SSLContext *ctx)
{
    _rep = factory->make_socket(ctx);
}
#endif

pegasus_socket::pegasus_socket(const pegasus_socket& s)
{
   if(this != &s)
   {
      Inc(this->_rep = s._rep);
   }
   
};

pegasus_socket::pegasus_socket(abstract_socket* s)
{

      _rep = s;
}


pegasus_socket::~pegasus_socket(void)
{
   Dec(_rep);
}

pegasus_socket& pegasus_socket::operator =(const pegasus_socket& s)
{

   if(this != &s)
   {
      Dec(_rep);
      Inc(this->_rep = s._rep);
   }
   return *this;
}


Boolean pegasus_socket::operator==(const pegasus_socket& s)
{
  if( this == &s )
    return true;
  if( _rep->operator Sint32()  == (Sint32)s )
    return true;
  return false;
}


pegasus_socket::operator Sint32() const
{
   return _rep->operator Sint32();
}

int pegasus_socket::socket(int type, int style, int protocol, void *ssl_context)
{
   return _rep->socket(type, style, protocol, ssl_context);
}


Sint32 pegasus_socket::read(void* ptr, Uint32 size)
{
   return _rep->read(ptr, size);
}

Sint32 pegasus_socket::write(const void* ptr, Uint32 size)
{
   return _rep->write(ptr, size);
}

int pegasus_socket::close(void)
{
   return _rep->close();
}

int pegasus_socket::enableBlocking(void)
{
   return _rep->enableBlocking();
}

int pegasus_socket::disableBlocking(void)
{
   return _rep->disableBlocking();
}

int pegasus_socket::getsockname (struct sockaddr *addr, PEGASUS_SOCKLEN_SIZE *length_ptr)
{
   return _rep->getsockname(addr, length_ptr);
}

int pegasus_socket::bind (struct sockaddr *addr, PEGASUS_SOCKLEN_SIZE length)
{
   return _rep->bind(addr, length);
}


pegasus_socket pegasus_socket::accept(struct sockaddr *addr, PEGASUS_SOCKLEN_SIZE *length_ptr)
{
   return pegasus_socket(_rep->accept(addr, length_ptr));
}

int pegasus_socket::connect (struct sockaddr *addr, PEGASUS_SOCKLEN_SIZE length)
{
   return _rep->connect(addr, length);
}

int pegasus_socket::shutdown(int how)
{
   return _rep->shutdown(how);
}

int pegasus_socket::listen(int q)
{
   return _rep->listen(q);
}

int pegasus_socket::getpeername (struct sockaddr *addr, PEGASUS_SOCKLEN_SIZE *length_ptr)
{
   return _rep->getpeername(addr, length_ptr);
}

int pegasus_socket::send(void *buffer, PEGASUS_SOCKLEN_SIZE size, int flags)
{
   return _rep->send(buffer, size, flags);
}

int pegasus_socket::recv(void *buffer, PEGASUS_SOCKLEN_SIZE size, int flags)
{
   return _rep->recv(buffer, size, flags);
}

int pegasus_socket::sendto(void *buffer, PEGASUS_SOCKLEN_SIZE size, int flags, struct sockaddr *addr, PEGASUS_SOCKLEN_SIZE length)
{
   return _rep->sendto(buffer, size, flags, addr, length);
}

int pegasus_socket::recvfrom(void *buffer, PEGASUS_SOCKLEN_SIZE size, int flags, struct sockaddr *addr, PEGASUS_SOCKLEN_SIZE *length_ptr)
{
   return _rep->recvfrom(buffer, size, flags, addr, length_ptr);
}

int pegasus_socket::setsockopt (int level, int optname, void *optval, PEGASUS_SOCKLEN_SIZE optlen)
{
   return _rep->setsockopt(level, optname, optval, optlen);
}

int pegasus_socket::getsockopt (int level, int optname, void *optval, PEGASUS_SOCKLEN_SIZE *optlen_ptr)
{
   return _rep->getsockopt(level, optname, optval, optlen_ptr);
}

Boolean pegasus_socket::incompleteReadOccurred(Sint32 retCode)
{
   return _rep->incompleteReadOccurred(retCode);
}

Boolean pegasus_socket::is_secure(void)
{
   return _rep->is_secure();
}

void pegasus_socket::set_close_on_exec(void)
{
   _rep->set_close_on_exec();
}


const char* pegasus_socket::get_err_string(void)
{
   return _rep->get_err_string();
}


#ifdef PEGASUS_LOCAL_DOMAIN_SOCKET

/**
 * unix domain socket class
 *
 */
class unix_socket_rep : public bsd_socket_rep
{
   public:

      typedef bsd_socket_rep Base;

      /**
       * map to pegasus_socket::sock_err
       */

      unix_socket_rep(void);
      virtual ~unix_socket_rep(void);
      // used to allow the accept method to work
      unix_socket_rep(int sock);

      virtual int socket(int, int, int, void* );
      virtual int close(void);
      virtual int enableBlocking(void);
      virtual int disableBlocking(void);

      virtual int bind (struct sockaddr *addr, PEGASUS_SOCKLEN_SIZE length);

      virtual abstract_socket* accept(struct sockaddr *addr, PEGASUS_SOCKLEN_SIZE *length_ptr);


      virtual Sint32 read(void*, Uint32);
      virtual Sint32 write(const void*, Uint32);
      


   private:

      unix_socket_rep& operator=(const unix_socket_rep& );
};


/**
 * default constructor for an (uninitialized) bsd socket
 */
unix_socket_rep::unix_socket_rep(void)
   : Base()
{

}

unix_socket_rep::unix_socket_rep(int sock)
   : Base(sock)
{
   
}



/**
 * default destructor for bsd_socket_rep
 *
 */
unix_socket_rep::~unix_socket_rep(void)
{
   close();
}

int unix_socket_rep::socket(int a, int b, int c, void* d)
{
   _socket = ::socket(AF_UNIX, SOCK_STREAM, 0);
   return _socket;
}


int unix_socket_rep::close(void)
{
   int ccode = Socket::close2(_socket);
   return ccode;
}



int  unix_socket_rep::enableBlocking(void)
{
   return Base::enableBlocking();
}

int unix_socket_rep::disableBlocking(void)
{
   return Base::disableBlocking();
}


/**
 * default implementation allows reuseof address
 * sockaddr structure needs to be fully initialized or call will fail
 */
int unix_socket_rep::bind (struct sockaddr *addr, PEGASUS_SOCKLEN_SIZE length)
{

   // first remove the local domain file if it exists
   System::removeFile(PEGASUS_LOCAL_DOMAIN_SOCKET_PATH);
   return Base::bind(addr, length);
}

abstract_socket* unix_socket_rep::accept(struct sockaddr *addr, PEGASUS_SOCKLEN_SIZE *length_ptr)
{

#if defined(PEGASUS_OS_TYPE_WINDOWS)
   int new_sock = ::accept(_socket, addr, length_ptr);
#else 
   int new_sock = ::accept(_socket, addr, length_ptr);
   if(new_sock == -1 && errno == EAGAIN)
   {
      int retries = 0;
      
      do 
      {
	 pegasus_sleep(1);
	 new_sock = ::accept(_socket, addr, length_ptr);
	 retries++;
      } while(new_sock == -1 && errno == EAGAIN && retries < 20);
      
   }
#endif 

   if(new_sock == -1)
   {
      _errno = errno;
   }

   unix_socket_rep *rep = new unix_socket_rep(new_sock);
   // set the close-on-exec flag
   rep->set_close_on_exec();
   return rep;
}


Sint32 unix_socket_rep::read(void* ptr, Uint32 size)
{
   int ccode = Socket::read(_socket, ptr, size);
   if(ccode == -1)
      _errno = errno;
   return ccode;
}

Sint32 unix_socket_rep::write(const void* ptr, Uint32 size)
{
   int ccode = Socket::write(_socket, ptr, size);
   if(ccode == -1)
      _errno = errno;
   return ccode;
}

unix_socket_factory::unix_socket_factory(void)
{
}

unix_socket_factory::~unix_socket_factory(void)
{
}

abstract_socket* unix_socket_factory::make_socket(void)
{
   return new unix_socket_rep();
}

#ifdef PEGASUS_HAS_SSL

abstract_socket* unix_socket_factory::make_socket(SSLContext *ctx)
{
   return new unix_socket_rep();
}

#endif // PEGASUS_HAS_SSL

#endif // domain socket

PEGASUS_NAMESPACE_END
