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
// Modified By:   Amit Arora (amita@in.ibm.com) for Bug#1170
//                Heather Sterling, IBM (hsterl@us.ibm.com)
//%/////////////////////////////////////////////////////////////////////////////


#ifndef peg_socket_class_heirarchy_h
#define peg_socket_class_heirarchy_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/IPC.h>
#include <Pegasus/Common/TLS.h>
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
#endif
#ifdef PEGASUS_OS_ZOS
#   include <Pegasus/Common/SocketzOS_inline.h>
#endif
#include <Pegasus/Common/Linkage.h>
#include <Pegasus/Common/Sharable.h> 

PEGASUS_NAMESPACE_BEGIN

#if defined(PEGASUS_PLATFORM_ZOS_ZSERIES_IBM)
typedef size_t PEGASUS_SOCKLEN_SIZE;

#elif defined(PEGASUS_PLATFORM_AIX_RS_IBMCXX) || defined(PEGASUS_PLATFORM_LINUX_GENERIC_GNU) || (defined(PEGASUS_OS_SOLARIS) && !defined(SUNOS_5_6))
typedef socklen_t PEGASUS_SOCKLEN_SIZE;
#else
typedef int PEGASUS_SOCKLEN_SIZE;
#endif




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

class socket_factory;

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

      virtual Boolean isPeerVerificationEnabled(void) = 0;
      virtual SSLCertificateInfo* getPeerCertificate(void) = 0; 
      virtual Boolean isCertificateVerified(void) = 0;  
      virtual Boolean addTrustedClient(const char* username) = 0;

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

      virtual Sint32 read(void* ptr, Uint32 size) { return -1 ;}
      virtual Sint32 write(const void* ptr, Uint32 size){ return -1 ;}
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

      Boolean isPeerVerificationEnabled(void) { return false;}
      SSLCertificateInfo* getPeerCertificate(void) { return NULL; } 
      Boolean isCertificateVerified(void) { return 0;}    
      Boolean addTrustedClient(const char* username) { return false;}   

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

      virtual Boolean isPeerVerificationEnabled(void);
      virtual SSLCertificateInfo* getPeerCertificate(void); 
      virtual Boolean isCertificateVerified(void);    
      virtual Boolean addTrustedClient(const char* username);       

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

   protected:
      static struct _library_init _lib_init;
      int _socket;
      //void *_ssl_context;
      int _errno;
      char _strerr[256];
};





class PEGASUS_COMMON_LINKAGE pegasus_socket 
{

public:
  
  pegasus_socket(void);
  pegasus_socket(socket_factory *);
 #ifdef PEGASUS_HAS_SSL
  pegasus_socket(socket_factory *, SSLContext *);
 #endif
  pegasus_socket(abstract_socket *);
  pegasus_socket(const pegasus_socket& s);
  ~pegasus_socket(void);
      
  pegasus_socket& operator=(const pegasus_socket& s);
  Boolean operator==(const pegasus_socket& s);
  
  operator Sint32() const;

  int socket(int type, int style, int protocol, void *ssl_context = 0);
                
  Sint32 read(void* ptr, Uint32 size);
  Sint32 write(const void* ptr, Uint32 size);
  int close(void);
  int enableBlocking(void);
  int disableBlocking(void);

  int getsockname (struct sockaddr *addr, PEGASUS_SOCKLEN_SIZE *length_ptr);
  int bind (struct sockaddr *addr, PEGASUS_SOCKLEN_SIZE length);
     
  // change size_t to size_t for ZOS and windows
  pegasus_socket accept(struct sockaddr *addr, PEGASUS_SOCKLEN_SIZE *length_ptr);
  int connect (struct sockaddr *addr, PEGASUS_SOCKLEN_SIZE length);
  int shutdown(int how);
  int listen(int q);
  int getpeername (struct sockaddr *addr, PEGASUS_SOCKLEN_SIZE *length_ptr);
  int send (void *buffer, PEGASUS_SOCKLEN_SIZE size, int flags);
  int recv (void *buffer, PEGASUS_SOCKLEN_SIZE size, int flags);
  int sendto(void *buffer, PEGASUS_SOCKLEN_SIZE size, int flags, struct sockaddr *addr, PEGASUS_SOCKLEN_SIZE length);
  int recvfrom(void *buffer, PEGASUS_SOCKLEN_SIZE size, int flags, struct sockaddr *addr, PEGASUS_SOCKLEN_SIZE *length_ptr);
  int setsockopt (int level, int optname, void *optval, PEGASUS_SOCKLEN_SIZE optlen);
  int getsockopt (int level, int optname, void *optval, PEGASUS_SOCKLEN_SIZE *optlen_ptr);


  Boolean incompleteReadOccurred(Sint32 retCode);
  Boolean is_secure(void);

  Boolean isPeerVerificationEnabled(void);
  SSLCertificateInfo* getPeerCertificate(void); 
  Boolean isCertificateVerified(void);    
  Boolean addTrustedClient(const char* username); 

    void set_close_on_exec(void);
      
  const char* get_err_string(void);
  
private:

  abstract_socket * _rep;

};


class PEGASUS_COMMON_LINKAGE socket_factory 
{
public:
  socket_factory(void)
  {
  }
      
  virtual ~socket_factory(void)
  {
  }
      
  virtual abstract_socket *make_socket(void) = 0;
#ifdef PEGASUS_HAS_SSL
  virtual abstract_socket *make_socket(SSLContext *) = 0;
#endif
};



/**
 *  factory class for creating the bsd socket object 
 **/
class PEGASUS_COMMON_LINKAGE bsd_socket_factory : public socket_factory
{
public:
  bsd_socket_factory(void);
  ~bsd_socket_factory(void);
  abstract_socket *make_socket(void);
#ifdef PEGASUS_HAS_SSL
  abstract_socket *make_socket(SSLContext* );
#endif
};



#ifdef PEGASUS_HAS_SSL

class PEGASUS_COMMON_LINKAGE ssl_socket_factory : public socket_factory
{

public:
  ssl_socket_factory(void);
  ~ssl_socket_factory(void);
  abstract_socket* make_socket(void);
  abstract_socket* make_socket(SSLContext* );
};



#endif 


# ifdef PEGASUS_LOCAL_DOMAIN_SOCKET
// << Thu Aug 14 15:01:30 2003 mdd >> domain sockets work 
class PEGASUS_COMMON_LINKAGE unix_socket_factory : public socket_factory
{
public:
  unix_socket_factory(void);
  ~unix_socket_factory(void);
  abstract_socket* make_socket(void);
#ifdef PEGASUS_HAS_SSL
  abstract_socket *make_socket(SSLContext*);  // Do not use
#endif
};


#endif

PEGASUS_NAMESPACE_END

#endif /* peg_socket_class_heirarchy_h */
