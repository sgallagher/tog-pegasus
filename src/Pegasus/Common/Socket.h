//%2005////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001, 2002 BMC Software; Hewlett-Packard Development
// Company, L.P.; IBM Corp.; The Open Group; Tivoli Systems.
// Copyright (c) 2003 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation, The Open Group.
// Copyright (c) 2004 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation; VERITAS Software Corporation; The Open Group.
// Copyright (c) 2005 Hewlett-Packard Development Company, L.P.; IBM Corp.;
// EMC Corporation; VERITAS Software Corporation; The Open Group.
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
#include <Pegasus/Common/Linkage.h>
#ifdef PEGASUS_OS_ZOS
#   include <Pegasus/Common/SocketzOS_inline.h>
#endif

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



PEGASUS_NAMESPACE_END

#endif /* Pegasus_Socket_h */
