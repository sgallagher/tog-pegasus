//%///////////////////////-*-c++-*-/////////////////////////////////////////////
//
// Copyright (c) 2001 - 2003  BMC Software, Hewlett-Packard Company, IBM,
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
// Author: Mike Day (mdday@us.ibm.com)
//
// Modified By:
//
//%/////////////////////////////////////////////////////////////////////////////


// << Thu Aug 14 15:00:35 2003 mdd >> domain sockets and socket tests work
#include "pegasus_socket.h"
#include <Pegasus/Common/Socket.h>
#include <Pegasus/Common/TLS.h>
#include <Pegasus/Common/System.h>

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

class PEGASUS_COMMON_LINKAGE abstract_socket : public Sharable
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

      virtual int getsockname (struct sockaddr *addr, PEGASUS_SOCKLEN_SIZE *length_ptr) = 0;
      virtual int bind (struct sockaddr *addr, PEGASUS_SOCKLEN_SIZE length) = 0;

      virtual abstract_socket* accept(struct sockaddr *addr, PEGASUS_SOCKLEN_SIZE *length_ptr) = 0;
      virtual int connect (struct sockaddr *addr, PEGASUS_SOCKLEN_SIZE length) = 0;
      virtual int shutdown(int how) = 0;
      virtual int listen(int q) = 0;
      virtual int getpeername (struct sockaddr *addr, PEGASUS_SOCKLEN_SIZE *length_ptr) = 0;
      virtual int send (void *buffer, PEGASUS_SOCKLEN_SIZE size, int flags) = 0;
      virtual int recv (void *buffer, PEGASUS_SOCKLEN_SIZE size, int flags) = 0;
      virtual int sendto(void *buffer, PEGASUS_SOCKLEN_SIZE size, int flags, struct sockaddr *addr, PEGASUS_SOCKLEN_SIZE length) = 0;
      virtual int recvfrom(void *buffer, PEGASUS_SOCKLEN_SIZE size, int flags, struct sockaddr *addr, PEGASUS_SOCKLEN_SIZE *length_ptr) = 0;
      virtual int setsockopt (int level, int optname, void *optval, PEGASUS_SOCKLEN_SIZE optlen) = 0;
      virtual int getsockopt (int level, int optname, void *optval, PEGASUS_SOCKLEN_SIZE *optlen_ptr) = 0;

      virtual Boolean incompleteReadOccurred(Sint32 retCode) = 0;
      virtual Boolean is_secure(void) = 0;
      virtual void set_close_on_exec(void) = 0;
      virtual const char* get_err_string(void) = 0;

   private:

      abstract_socket(const abstract_socket& );
      abstract_socket& operator=(const abstract_socket& );
};



/**
 * null socket class -
 * error handling rep for empty pegasus_sockets -
 *
 */
class empty_socket_rep : public abstract_socket
{
   public:
      empty_socket_rep(void){ }
      ~empty_socket_rep(void){ }
      operator Sint32() const { return -1 ;}

      int socket(int sock_type, int sock_style,
		 int sock_protocol, void *ssl_context = 0) { return -1 ;}

      Sint32 read(void* ptr, Uint32 size) { return -1 ;}
      Sint32 write(const void* ptr, Uint32 size){ return -1 ;}
      int close(void){ return -1 ;}
      int enableBlocking(void){ return -1 ;}
      int disableBlocking(void){ return -1 ;}

       int getsockname (struct sockaddr *addr, PEGASUS_SOCKLEN_SIZE *length_ptr){ return -1 ;}
       int bind (struct sockaddr *addr, PEGASUS_SOCKLEN_SIZE length) { return -1;}

       abstract_socket* accept(struct sockaddr *addr, PEGASUS_SOCKLEN_SIZE *length_ptr) { return 0;}
       int connect (struct sockaddr *addr, PEGASUS_SOCKLEN_SIZE length) { return -1;}
       int shutdown(int how) { return -1;}
       int listen(int queue_len ) { return -1;}
      int getpeername (struct sockaddr *addr, PEGASUS_SOCKLEN_SIZE *length_ptr) { return -1;}
       int send (void *buffer, PEGASUS_SOCKLEN_SIZE size, int flags) { return -1;}
       int recv (void *buffer, PEGASUS_SOCKLEN_SIZE size, int flags) { return -1;}
       int sendto(void *buffer, PEGASUS_SOCKLEN_SIZE size, int flags, struct sockaddr *addr, PEGASUS_SOCKLEN_SIZE length) { return -1;}
       int recvfrom(void *buffer, PEGASUS_SOCKLEN_SIZE size, int flags, struct sockaddr *addr, PEGASUS_SOCKLEN_SIZE *length_ptr) { return -1;}
      int setsockopt (int level, int optname, void *optval, PEGASUS_SOCKLEN_SIZE optlen) { return -1;}
      int getsockopt (int level, int optname, void *optval, PEGASUS_SOCKLEN_SIZE *optlen_ptr) { return -1;}

       Boolean incompleteReadOccurred(Sint32 retCode) { return false;}
       Boolean is_secure(void) { return false;}
      void set_close_on_exec(void) { }
      const char* get_err_string(void)
      {
	 static const char* msg = "Unsupported.";
	 return msg;
      }
   private:
      empty_socket_rep(int);
};


/**
 * internet socket class
 * designed to be overriden by ssl_socket_rep
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

      virtual operator Sint32() const;
      int socket(int sock_type, int sock_style, int sock_protocol, void *ssl_context = 0);

      virtual Sint32 read(void* ptr, Uint32 size);
      virtual Sint32 write(const void* ptr, Uint32 size);
      virtual int close(void);
      int enableBlocking(void);
      int disableBlocking(void);

      virtual int getsockname (struct sockaddr *addr, PEGASUS_SOCKLEN_SIZE *length_ptr);
      virtual int bind (struct sockaddr *addr, PEGASUS_SOCKLEN_SIZE length);

      virtual abstract_socket* accept(struct sockaddr *addr, PEGASUS_SOCKLEN_SIZE *length_ptr);
      virtual int connect (struct sockaddr *addr, PEGASUS_SOCKLEN_SIZE length);
      virtual int shutdown(int how);
      virtual int listen(int queue_len );
      int getpeername (struct sockaddr *addr, PEGASUS_SOCKLEN_SIZE *length_ptr);
      virtual int send (void *buffer, PEGASUS_SOCKLEN_SIZE size, int flags);
      virtual int recv (void *buffer, PEGASUS_SOCKLEN_SIZE size, int flags);
      virtual int sendto(void *buffer, PEGASUS_SOCKLEN_SIZE size, int flags, struct sockaddr *addr, PEGASUS_SOCKLEN_SIZE length);
      virtual int recvfrom(void *buffer, PEGASUS_SOCKLEN_SIZE size, int flags, struct sockaddr *addr, PEGASUS_SOCKLEN_SIZE *length_ptr);
      int setsockopt (int level, int optname, void *optval, PEGASUS_SOCKLEN_SIZE optlen);
      int getsockopt (int level, int optname, void *optval, PEGASUS_SOCKLEN_SIZE *optlen_ptr);

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
#ifdef PEGASUS_OS_TYPE_WINDOWS
	       Socket::initializeInterface();
#endif
	    }
	    ~_library_init(void)
	    {
#ifdef PEGASUS_OS_TYPE_WINDOWS
	       Socket::uninitializeInterface();
#endif
	    }
      };

   public:
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
      _errno = errno;
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
   int new_sock = ::accept(_socket, addr, length_ptr);
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


int bsd_socket_rep::getpeername (struct sockaddr *addr, PEGASUS_SOCKLEN_SIZE *length_ptr)
{
   int ccode = ::getpeername(_socket, addr, length_ptr);
   if(ccode == -1 )
      _errno = errno;
   return ccode;
}

int bsd_socket_rep::send (void *buffer, PEGASUS_SOCKLEN_SIZE size, int flags)
{
   #ifdef PEGASUS_OS_TYPE_WINDOWS
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
   #ifdef PEGASUS_OS_TYPE_WINDOWS
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
   #ifdef PEGASUS_OS_TYPE_WINDOWS
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
   #ifdef PEGASUS_OS_TYPE_WINDOWS
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
   #ifdef PEGASUS_OS_TYPE_WINDOWS
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
   #ifdef PEGASUS_OS_TYPE_WINDOWS
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

      Sint32 read(void* ptr, Uint32 size);
      Sint32 write(const void* ptr, Uint32 size);
      int close(void);

      Boolean incompleteReadOccurred(Sint32 retCode);
      Boolean is_secure(void);
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


Boolean ssl_socket_rep::_init(void)
{
   if(_ctx == 0 )
      return false;

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
    _rep = factory->make_socket();
}

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

      virtual int close(void);
      int enableBlocking(void);
      int disableBlocking(void);

      virtual int bind (struct sockaddr *addr, PEGASUS_SOCKLEN_SIZE length);

      virtual abstract_socket* accept(struct sockaddr *addr, PEGASUS_SOCKLEN_SIZE *length_ptr);


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
}


int unix_socket_rep::close(void)
{
   int ccode = Base::close();
//   System::removeFile(PEGASUS_LOCAL_DOMAIN_SOCKET_PATH);
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
   int new_sock = ::accept(_socket, addr, length_ptr);
   if(new_sock == -1)
   {
      _errno = errno;
   }

   unix_socket_rep *rep = new unix_socket_rep(new_sock);
   // set the close-on-exec flag
   rep->set_close_on_exec();
   return rep;
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
