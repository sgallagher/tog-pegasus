//%/////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001 The Open group, BMC Software, Tivoli Systems, IBM
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
//%/////////////////////////////////////////////////////////////////////////////


#ifndef _LSLP_LINUX_INCLUDE_
#define _LSLP_LINUX_INCLUDE_

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <unistd.h> 
#include <fcntl.h>
#include <time.h>
#include <sys/types.h>
#include <sys/time.h>
#include <pthread.h>
#include <semaphore.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <string.h>

#if defined(PEGASUS_OS_HPUX) || defined(PEGASUS_OS_TRU64)
extern "C" char *strtok_r(char *, const char *, char **);
# include <arpa/inet.h>
# include <net/if.h>
#elif defined (PEGASUS_OS_TRU64) || defined(PEGASUS_OS_AIX)
# include <arpa/inet.h>
# include <net/if.h>
#else
#include <linux/inet.h>
#include <linux/if.h>
#endif

#include <netdb.h>
#include <errno.h>
#include <assert.h>
#include <memory.h>
#include <string.h>
#include <ctype.h>
#include <syslog.h>

PEGASUS_USING_STD;

PEGASUS_NAMESPACE_BEGIN


typedef char int8; 
typedef	unsigned char uint8; 
typedef uint8 byte; 
typedef short int16; 
typedef unsigned short uint16; 
typedef int int32; 
typedef unsigned int uint32; 
typedef long long int64; 
typedef unsigned long long uint64; 
typedef int BOOL; 

#define _LSLP_SLEEP(m) usleep((m) * 1000)
#define SOCKET int

#define _LSLP_CLOSESOCKET close
#define _LSLP_IOCTLSOCKET ioctl
#define _LSLP_INIT_NETWORK()
#define _LSLP_DEINIT_NETWORK()

#define _LSLP_SET_TTL(s, t)  setsockopt((s), IPPROTO_IP, IP_MULTICAST_TTL, (const char *)&(t), sizeof((t))) 

#ifndef EINVAL
#define EINVAL 0
#endif
#ifndef EWOULDBLOCK
#define EWOULDBLOCK 0
#endif
#define _LSLP_GETLASTERROR() errno

#define INVALID_SOCKET -1
#define SOCKET_ERROR -1
#define SD_BOTH 2

#define FAR

#define _LSLP_STRTOK(n, d, s) strtok_r((n), (d), (s))
  
#define LSLP_MAXPATH FILENAME_MAX
#define LSLP_NEWLINE \r

#ifndef TRUE
#define TRUE 1
#define FALSE 0
#endif


#define _LSLP_CTIME(t, b)  ctime_r(((const time_t *)(t)), ((char *)(b)))

PEGASUS_NAMESPACE_END

#endif /* _LSLP_LINUX_INCLUDE_ */
