//%///////////////////////-*-c++-*-/////////////////////////////////////////////
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

#ifndef Pegasus_Socket_h
#define Pegasus_Socket_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/IPC.h>
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
#endif
#include <Pegasus/Common/Linkage.h>

PEGASUS_NAMESPACE_BEGIN

class PEGASUS_COMMON_LINKAGE Socket
{
   public:


      static Sint32 read(Sint32 socket, void* ptr, Uint32 size);

      static Sint32 write(Sint32 socket, const void* ptr, Uint32 size);

      static void close(Sint32 socket);
      static int close2(Sint32 socket);
      
      static void enableBlocking(Sint32 socket);
      static int  enableBlocking2(Sint32 socket);

      static void disableBlocking(Sint32 socket);
      static int disableBlocking2(Sint32 socket);

      static void initializeInterface(void);
      static void uninitializeInterface(void);
      

   private:

      Socket() { }
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

class abstract_socket;
class socket_factory;


class PEGASUS_COMMON_LINKAGE pegasus_socket 
{

   public:
      pegasus_socket(void);
      pegasus_socket(socket_factory *);
      pegasus_socket(abstract_socket *);
      pegasus_socket(const pegasus_socket& s);
      ~pegasus_socket(void);
      
      pegasus_socket& operator=(const pegasus_socket& s);
      operator Sint32() const;

      int socket(int type, int style, int protocol, void *ssl_context = 0);
                
      Sint32 read(void* ptr, Uint32 size);
      Sint32 write(const void* ptr, Uint32 size);
      int close(void);
      int enableBlocking(void);
      int disableBlocking(void);

      int getsockname (struct sockaddr *addr, size_t *length_ptr);
      int bind (struct sockaddr *addr, size_t length);
     
      // change size_t to size_t for ZOS and windows
      pegasus_socket accept(struct sockaddr *addr, size_t *length_ptr);
      int connect (struct sockaddr *addr, size_t length);
      int shutdown(int how);
      int listen(int q);
      int getpeername (struct sockaddr *addr, size_t *length_ptr);
      int send (void *buffer, size_t size, int flags);
      int recv (void *buffer, size_t size, int flags);
      int sendto(void *buffer, size_t size, int flags, struct sockaddr *addr, size_t length);
      int recvfrom(void *buffer, size_t size, int flags, struct sockaddr *addr, size_t *length_ptr);
      int setsockopt (int level, int optname, void *optval, size_t optlen);
      int getsockopt (int level, int optname, void *optval, size_t *optlen_ptr);


      Boolean incompleteReadOccurred(Sint32 retCode);
      Boolean is_secure(void);
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
};


PEGASUS_NAMESPACE_END

#endif /* Pegasus_Socket_h */
