//%2006////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001, 2002 BMC Software; Hewlett-Packard Development
// Company, L.P.; IBM Corp.; The Open Group; Tivoli Systems.
// Copyright (c) 2003 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation, The Open Group.
// Copyright (c) 2004 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation; VERITAS Software Corporation; The Open Group.
// Copyright (c) 2005 Hewlett-Packard Development Company, L.P.; IBM Corp.;
// EMC Corporation; VERITAS Software Corporation; The Open Group.
// Copyright (c) 2006 Hewlett-Packard Development Company, L.P.; IBM Corp.;
// EMC Corporation; Symantec Corporation; The Open Group.
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
/*****************************************************************************
 *  Description:   portability routines for nucleus RTOS
 *
 *  Originated: December 17, 2001 
 *	Original Author: Mike Day md@soft-hackle.net
 *                                mdday@us.ibm.com 
 *  $Header: /cvs/MSB/pegasus/src/slp/slp_client/src/cmd-utils/slp_client/Attic/lslp-nucleus.h,v 1.3.16.2 2006/03/07 18:46:16 a.dunfey Exp $ 	                                                            
 *               					                    
 *  Copyright (c) 2001 - 2003  IBM                                          
 *  Copyright (c) 2000 - 2003 Michael Day                                    
 *                                                                           
 *  Permission is hereby granted, free of charge, to any person obtaining a  
 *  copy of this software and associated documentation files (the "Software"),
 *  to deal in the Software without restriction, including without limitation 
 *  the rights to use, copy, modify, merge, publish, distribute, sublicense, 
 *  and/or sell copies of the Software, and to permit persons to whom the     
 *  Software is furnished to do so, subject to the following conditions:       
 * 
 *  The above copyright notice and this permission notice shall be included in 
 *  all copies or substantial portions of the Software.
 * 
 * 
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 *  THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 *  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 *  DEALINGS IN THE SOFTWARE.
 *
 *****************************************************************************/





#ifndef _LSLP_NUCLEUS_INCL_
#define LSLP_NUCLEUS_INCL_

#ifdef	__cplusplus
extern "C" {
#endif


#include "time.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sp_cmn.hc"
#include "tcpsprt.h" //define for net version
#include "net\target.h"
#include "net\inc\nu_net.h"
#include "net\inc\arp.h"
#include "net\inc\socketd.h"
#include "net\inc\sockdefs.h"
//#include "tcpst.hc"
UINT32  tcpst_inet_addr(char *);

  /* DELETE these lines after you are ready to proceed ! */
//jeb#error "Remember to define the correct BYTE ORDER in the file endian.h"
//jeb#error "Check to ensure there is a nucleus implementation of inet_addr(char *) - I can write one if you need it"
//jeb#error "Check to ensure there is a nucleus implementation of htons(short int) - I can write one if you need it"
//jeb#error "Check to ensure there is a nucleus implementation of inet_ntoa(unsigned int) - I can write one if you need it"
//jeb#error "Need a gethostname function (returns a string holding the hostname of the management module)"
//jeb#error "prototype is int gethostname(char *buffer, int buffer_size) ; return zero upon success "
//jeb#error "need to define _LSLP_STRTOK( char *string, char *delimitors, char **state), which is a thread-safe version of strtok"
/* #error "errno is a global variable holding the code for the most recent failure" */

/*** nucleus include files ***/

/** below are posix include files that may or may not be appropriate to Nucleus ***/
/*** modify to include the correct files for the nucleus runtimes ***/
//jeb#include <signal.h>
//jeb#include <stdio.h>
//jeb#include <stdlib.h>
//jeb#include <stdarg.h>
//jeb#include <unistd.h> 
//jeb#include <time.h>
//jeb#include <sys/types.h>
//jeb#include <sys/time.h>
//jeb#include <netinet/in.h>
//jeb#include <arpa/inet.h>
//jeb#include <sys/ioctl.h>
//jeb#include <sys/socket.h>
//jeb#include <sys/select.h>
//jeb#include <netdb.h>
//jeb#include <assert.h>
//jeb#include <memory.h>
//jeb#include <string.h>
//jeb#include <ctype.h>
  
  
  /*** typedefs ***/
  typedef char int8;
  typedef CHAR int8;
  typedef INT8 int8;
  

  typedef unsigned char uint8;
  typedef unsigned char byte;
  typedef UINT8 uint8;

  typedef short int16;
  typedef INT16 int16;
  
  typedef unsigned short uint16;
  typedef UINT16 uint16;

  typedef int int32;
  typedef unsigned long uint32;

//jeb chg  typedef int INT;
  typedef int SOCKETD;
  typedef int SOCKET;
  
  
/* ATTN << Thu Dec 20 11:35:36 2001 mdd >> */

#ifdef BOOL   //jeb 
#undef BOOL
  typedef uint32 BOOL;
#endif  
  
  
  /* ATTN << Tue Dec 18 09:40:43 2001 mdd >> */
  /* this is a variable structure that probably has an option
     id and then a variable sized buffer to hold the results */
/*   typedef unsigned int SCK_IOCTL_OPTION ; */
  
  /* probably don't need long longs  */
  /**  typedef __int64 int64; **/
  /**  typedef unsigned __int64 uint64; **/  //jeb fix
  
  typedef int STATUS;
  
  struct in_addr 
  {
    uint32 s_addr;
  };
  
  typedef struct sockaddr
  {
    uint8 sa_len;
    uint8 sa_family;
    char sa_data[14];
  } SOCKADDR ;
  
                 
  typedef struct sockaddr_in     
  {
//jeb    uint8 sin_len;
    int16 sin_family;
    uint16 sin_port;
    struct in_addr sin_addr;
    char   *sin_zero;
//    char sin_zero[8];
  } SOCKADDR_IN ;
  
  struct hostent
  {
    char *h_name;
    char **h_alias;
    INT16 h_addrtype;
    INT16 h_length;
//    char *h_addr;               //jeb
    char	**h_addr_list;
  };
  
//jeb  extern NU_Host_Ent * nucleus_gethostbyname(const char *name);
//jeb   extern NU_Host_Ent * nucleus_gethostbyaddr( const void *addr, int len, int type);
  extern NU_HOSTENT * nucleus_gethostbyname(const char *name);
  extern NU_HOSTENT * nucleus_gethostbyaddr( const void *addr, int len, int type);
#ifndef FALSE
#define FALSE 0
#endif

#ifndef TRUE
#define TRUE 1
#endif

#define SOCK_DGRAM NU_TYPE_DGRAM
#define SOCK_STREAM NU_TYPE_STREAM
#define UNUSED_NU_PARM 0

#define INVALID_SOCKET NU_INVALID_SOCKET 
#define SOCKET_ERROR NU_INVALID_SOCKET  
//jeb strt add
#define AF_INET SK_FAM_IP 
#define INADDR_NONE 0 
#define INADDR_ANY INADDR_NONE 
//jeb end add

/* ATTN need to define these error constants for nucleus << Tue Dec 18 16:22:28 2001 mdd >> */

#define EINTR     /* call was interrupted */
#define ENOTSOCK INVALID_SOCKET
#define EADDRNOTAVAIL 
#define EAFNOSUPPORT 
#define EISCONN 
#define ETIMEDOUT 
#define ECONNREFUSED 
#define ENETUNREACH 
#define EADDRINUSE 
#define EINPROGRESS 
#define EALREADY 
#define EWOULDBLOCK 


    /*** function macros ***/

  /*** << Mon Dec 17 17:28:24 2001 mdd >> 

       NOTE: Many of these calls require unused or null values for parameters. 
       I've substituted (hardcoded) these parameters where I could find them. 
       This is not very portable nor maintainable; perhaps you will find a better solution !

  ***/

#define _LSLP_CLOSESOCKET(a)  NU_Close_Socket((int)(a)) 
#define _LSLP_SOCKET(a, b, c) NU_Socket( NU_FAMILY_IP, ((INT16)(b)), NU_NONE )
//jeb
#define _LSLP_BIND(a, b, c) \
           NU_Bind ((INT)(a),((struct addr_struct *)(b)),UNUSED_NU_PARM )
#define _LSLP_CONNECT(a, b, c ) \
           NU_Connect ((INT)(a), \
              ((struct addr_struct *)(b)), \
                    UNUSED_NU_PARM)
#define _LSLP_LISTEN(a, b) NU_Listen ((INT)(a), (UINT16)(b))
#define _LSLP_ACCEPT(a, b, c) \
           NU_Accept ((INT)(a), \
		      ((struct addr_struct *)(b)), \
                      UNUSED_NU_PARM )
#define _LSLP_SEND(a, b, c, d) NU_Send ((INT)(a), (CHAR *)(b), (UINT16)(c), UNUSED_NU_PARM)
#define _LSLP_SENDTO(a, b, c, d, e, f) \
             NU_Send_To ((INT)(a), (char *)(b), (UINT16)(c), (UNUSED_NU_PARM), \
                         ((struct addr_struct *)(e)), \
                         UNUSED_NU_PARM )
#define _LSLP_RECV(a, b, c, d)  NU_Recv ((INT)(a), (char *)(b), (UINT16)(c), UNUSED_NU_PARM )
#define _LSLP_RECV_FROM(a, b, c, d, e, f) \
             NU_Recv_From ((INT)(a), (char *)(b), (INT16)(c), (UNUSED_NU_PARM), \
             ((struct addr_struct *)(e)), \
             UNUSED_NU_PARM)

/* << Tue Dec 18 07:53:12 2001 mdd >> ATTN need to redefine these gethost functions   */
/* for all platforms  */

#define _LSLP_GETHOSTBYNAME(a) \
         (struct hostent *) nucleus_gethostbyname((const char *)(a))
#define _LSLP_GETHOSTBYADDR(a, b, c ) \
           (struct hostent *) nucleus_gethostbyaddr( (const void *)(a), (int)(b), (int)(c))

#define _LSLP_SETSOCKOPT(a, b, c, d, e) \
             NU_Setsockopt(((INT)(a)), ((INT)(b)), ((INT)(c)), ((void *)(d)), ((INT)(e))) 
#define _LSLP_GETSOCKOPT(a, b, c, d, e) \
             NU_Getsockopt(((INT)(a)), ((INT)(b)), ((INT)(c)), ((void *)(d)), ((INT *)(e)))

#define _LSLP_SET_TTL(s, t)  NU_Setsockopt((s), IPPROTO_IP, IP_MULTICAST_TTL, (void *)&(t), sizeof((t)))      //jeb

/* << Tue Dec 18 08:16:59 2001 mdd >> ATTN need to redefine abort to be shutdown & close */
#define _LSLP_ABORT(a) NU_Abort(((INT)(a))) 

#define _LSLP_SLEEP(a) _sleep((int)(a))

//jeb strtok
#define _LSLP_STRTOK(s, d, x) strtok(((char*)(s)),((const char *)(d)))  
//jeb assert

#define SLPDBG
//#ifdef SLPDBG
//void assert(int);
#define assert(a) _system_info((!(a)),"ASSERT:%s/%d", __FILE__, __LINE__)
//#else
//#define assert(a) ((void)0)
//#endif

#define htons(x)	INTSWAP((unsigned short)(x))
#define ntohs(x)    INTSWAP((unsigned short)(x))
#define ntohl(x)	LONGSWAP((unsigned long)(x))
#define inet_addr(x) tcpst_inet_addr((char *)(x))

#define _itoa(a,b,c) itoa(((int)(a)),((char*)(b)),((int)(c)))
//end jeb

/* << Tue Dec 18 09:54:25 2001 mdd >> ATTN the final parameter is a pointer on windows */
/* and unix, but is an integer on Nucleus that specifies timer ticks */

  struct _lslp_timeval 
  {
    uint32 tv_sec;
    uint32 tv_usec;
  } ;
//jeb 
  struct timeval 
  {
    uint32 tv_sec;
    uint32 tv_usec;
  } ;
//jeb 

//jeb  typedef TIMEVAL timeval;
  
#ifdef PLUS
#define _LSLP_SELECT(a, b, c, d, e ) \
             NU_Select(((INT)(a)), ((FD_SET *)(b)), ((FD_SET *)(c)), ((FD_SET *)(d)), \
                       ((UNSIGNED)(((struct _lslp_timeval *)(e))->tv_usec)))
#else 
#define _LSLP_SELECT(a, b, c, d, e ) \
             NU_Select(((INT)(a)), ((FD_SET *)(b)), ((FD_SET *)(c)), ((FD_SET *)(d)), \ 
		       (sint)((((struct _lslp_timeval *)(e))->tv_usec) & 0x7fffffff  ) )
#endif 
#define LSLP_FD_SET FD_SET
#define _LSLP_FD_ISSET(a, b) NU_FD_Check(((INT)(a)), ((FD_SET *)(b)))
#define _LSLP_FD_SET(a, b)  NU_FD_Set(((INT)(a)), ((FD_SET *)(b)))
#define _LSLP_FD_ZERO(a) NU_FD_Init(((FD_SET *)(a)))
#define _LSLP_FD_CLR(a, b)  NU_FD_Reset(((INT)(a)), ((FD_SET *)(b)))

       
/* ATTN << Tue Dec 18 10:42:19 2001 mdd >> */

//jeb #define _LSLP_IOCTLSOCKET(a, b, c) #error "See Nucleus NET Reference Manual for limitations on usage of ioctl\n"

/* ATTN << Tue Dec 18 12:54:40 2001 mdd >> */
#define _LSLP_GETLASTERROR() 


  /***** 

  We probably don't need these to be defined << Tue Dec 18 16:21:23 2001 mdd >> 

  STATUS NU_Send_To_Raw(INT socketd, char *buff, UINT16 nbytes, INT16 flags,
  struct addr_struct *to, INT16 addrlen);
  
  INT16  NU_Get_IPRaw_PCB(INT socketd, struct sock_struct *);
  
  jeb start add
  STATUS NU_Recv_From_No_Suspend (INT16, char *, INT16, INT16, struct addr_struct *, INT16 *);
  STATUS NU_Find_Open_Sckts_For_Dev (UINT8 sck_type, INT skt, DV_DEVICE_ENTRY *dev_ptr);
  jeb end add
  
  STATUS NU_Recv_From_Raw (INT, char *, INT16, INT16, struct addr_struct *, INT16 *);
  
  STATUS NU_Push (INT);
  STATUS NU_Is_Connected (INT);
  INT16  NU_GetPnum (struct sock_struct *);
  INT16  NU_Get_UDP_Pnum (struct sock_struct *);
  STATUS NU_Fcntl (INT socketd, INT16 command, INT16 arguement);
  STATUS NU_Get_Peer_Name(INT, struct sockaddr_struct *, INT16 *);
    
  *****/

//prototypes  //jeb
char *inet_ntoa(struct in_addr in);



#ifdef	__cplusplus
}
#endif

#endif /* nucleus include */
