//%/////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2001 The Open Group, BMC Software, IBM
//
// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation
// the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in 
// all copies or substantial portions of the Software.
//
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
// THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.
//
//==============================================================================
//
// Author: Mike Day (mdday@us.ibm.com)
//
//%/////////////////////////////////////////////////////////////////////////////


#ifndef _LSLP_WINDOWS_INCL_
#define _LSLP_WINDOWS_INCL_


#ifdef _WIN32 
#ifndef _MT
#define _MT
#endif
#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#include <stdio.h>
#include <process.h>
#include <winbase.h>
#include <tchar.h>
#include <sys/types.h>
#include <sys/timeb.h> 
#include <errno.h>
#include <assert.h>
#include <io.h>

PEGASUS_USING_STD;

PEGASUS_NAMESPACE_BEGIN

typedef char int8;
typedef	unsigned char uint8;
typedef uint8 byte;
typedef short int16;
typedef unsigned short uint16;
typedef int int32;
typedef unsigned int uint32;
typedef __int64 int64;
typedef unsigned __int64 uint64;
  typedef int socklen_t;

/* void SLEEP(int32 milliseconds) */
#define _LSLP_SLEEP Sleep
#define _LSLP_CLOSESOCKET closesocket
#define _LSLP_IOCTLSOCKET ioctlsocket
#define _LSLP_INIT_NETWORK WindowsStartNetwork
#define _LSLP_DEINIT_NETWORK WSACleanup
  //#define _LSLP_SET_TTL(s, ttl) WSAIoctl((s), SIO_MULTICAST_SCOPE, &(ttl), sizeof((ttl))  , NULL, 0 , NULL, NULL, NULL)
#define _LSLP_SET_TTL(s, t)  setsockopt((s), IPPROTO_IP, IP_MULTICAST_TTL, (const char *)&(t), sizeof((t))) 

#define strcasecmp _stricmp

#define ENOTSOCK WSAENOTSOCK
#define EADDRNOTAVAIL WSAEADDRNOTAVAIL 
#define EAFNOSUPPORT WSAEAFNOSUPPORT 
#define EISCONN WSAEISCONN 
#define ETIMEDOUT WSAETIMEDOUT
#define ECONNREFUSED WSAECONNREFUSED
#define ENETUNREACH WSAENETUNREACH
#define EADDRINUSE WSAEADDRINUSE
#define EINPROGRESS WSAEINPROGRESS 
#define EALREADY WSAEALREADY 
#define EWOULDBLOCK WSAEWOULDBLOCK
#ifndef EINVAL
#define EINVAL WSAEINVAL
#endif
  /* to start as a service */

#define _LSLP_GETLASTERROR() WSAGetLastError()


#define _LSLP_STRTOK(n, d, s) strtok((n), (d) )
#define _LSLP_SIG_ACTION()
#define _LSLP_SIG_ACTION_THREAD()
#define _LSLP_PUSH_HANDLER(a, b)
#define _LSLP_POP_HANDLER(a, b)

#define strncasecmp strnicmp

/* maximum path length */
#define LSLP_MAXPATH _MAX_PATH
#define LSLP_NEWLINE \r\n

#define _LSLP_CTIME(t, b) memcpy(((char *)(b)), ctime(((const time_t *)(t))), 26)

PEGASUS_NAMESPACE_END

#endif	 /* WIN 32 */


#endif /*_LSLP_WINDOWS_INCL_*/

