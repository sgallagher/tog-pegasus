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


#ifndef _LSLPDEFS_INCLUDE
#define _LSLPDEFS_INCLUDE


#ifdef __linux__
#include <endian.h>
#else
#define	__LITTLE_ENDIAN__	1234
#define	__BIG_ENDIAN__	4321
#define	__PDP_ENDIAN__	3412
#define __BYTE_ORDER__ __LITTLE_ENDIAN__
#endif


  /********* circular list macros *********************/

  /*---------------------------------------------------------------------**
  ** structures used with these macros MUST have the following elements: **
  ** struct type-name {                                                  **
  ** 	struct type-name *next;                                            **
  ** 	struct type-name *prev;                                            **
  ** 	BOOL isHead;                                                       **
  ** 	}                                                                  **
  **---------------------------------------------------------------------*/

  /*  is node x the head of the list? */
  /* BOOL IS_HEAD(node *x); */
#define _LSLP_IS_HEAD(x) ((x)->isHead )

  /* where h is the head of the list */
  /* BOOL _LSLP_IS_EMPTY(head);          */
#define _LSLP_IS_EMPTY(h) \
	((((h)->next == (h)) && ((h)->prev == (h)) ) ? TRUE : FALSE)

  /* where n is the new node, insert it immediately after node x */
  /* x can be the head of the list                               */
  /* void _LSLP_INSERT(new, after);                                  */
#define _LSLP_INSERT(n, x)   	\
	{(n)->prev = (x); 		\
	(n)->next = (x)->next; 	\
	(x)->next->prev = (n); 	\
	(x)->next = (n); }		

#define _LSLP_INSERT_AFTER _LSLP_INSERT
#define _LSLP_INSERT_BEFORE(n, x)   \
	{(n)->next = (x);					\
	(n)->prev = (x)->prev;				\
	(x)->prev->next = (n);				\
	(x)->prev = (n); }

#define _LSLP_INSERT_WORKNODE_LAST(n, x)    \
        {gettimeofday(&((n)->timer));  \
	(n)->next = (x);	       \
	(n)->prev = (x)->prev;	       \
	(x)->prev->next = (n);	       \
	(x)->prev = (n); }

#define _LSLP_INSERT_WORKNODE_FIRST(n, x)    \
        {gettimeofday(&((n)->timer));  \
	(n)->prev = (x); 	       \
	(n)->next = (x)->next; 	       \
	(x)->next->prev = (n); 	       \
	(x)->next = (n); }		


  /* delete node x  - harmless if mib is empty */
  /* void _LSLP_DELETE_(x);                        */
#define _LSLP_UNLINK(x)				\
	{(x)->prev->next = (x)->next;	\
	(x)->next->prev = (x)->prev;}	

  /* given the head of the list h, determine if node x is the last node */
  /* BOOL _LSLP_IS_LAST(head, x);                                           */
#define _LSLP_IS_LAST(h, x) \
	(((x)->prev == (h) && (h)->prev == (x)) ? TRUE : FALSE)

  /* given the head of the list h, determine if node x is the first node */
  /* BOOL _LSLP_IS_FIRST(head, x);                                           */
#define _LSLP_IS_FIRST(h, x) \
	(((x)->prev == (h) && (h)->next == (x)) ? TRUE : FALSE)

  /* given the head of the list h, determine if node x is the only node */
  /* BOOL _LSLP_IS_ONLY(head, x);                                           */
#define _LSLP_IS_ONLY(h, x) \
	(((x)->next == (h) && (h)->prev == (x)) ? TRUE : FALSE)
  /* void _LSLP_LINK_HEAD(dest, src); */
#define _LSLP_LINK_HEAD(d, s) \
	{(d)->next = (s)->next;  \
	(d)->prev = (s)->prev;  \
	(s)->next->prev = (d);  \
	(s)->prev->next = (d) ; \
        (s)->prev = (s)->next = (s); }


  /************* bit-set macros *********************************/
  /* how many dwords do we need to allocate to hold b bits ? */
#define _LSLP_SIZEOF_BITARRAY(b) (((b) >> 5) + 1)
  /*  operating on an array of dwords */
#define _LSLP_IS_BIT_SET(b, a) (*((a) + ((b) >> 5))	& (1 << (((b)%32) - 1)))
#define _LSLP_SET_BIT(b, a) (*((a) + ((b) >> 5)) |= (1 << (((b)%32) - 1)))
#define _LSLP_CLEAR_BIT(b, a) (*((a) + ((b) >> 5)) ^= (1 << (((b)%32) - 1)))

  /* the usual */
#define _LSLP_MIN(a, b) ((a) < (b) ? (a) : (b))
#define _LSLP_MAX(a, b) ((a) > (b) ? (a) : (b))

  /********************** SLP V2 MACROS *************************/
  /**************************************************************/
#define LSLP_PROTO_VER 2
#define LSLP_PORT 427
#define _LSLP_LOCAL_BCAST inet_addr("255.255.255.255") 
#define _LSLP_MCAST       inet_addr("239.255.255.253")
  /* derive an address for a directed broadcast */
  /* a = uint32 addr, m = uint32 mask */
  /* #define _LSLP_DIR_BCAST(a, m) ((((a) &= (m)) &&  ((a) |= ~(m))) ? (a) : (a) ) */

#define LSLP_EN_US "en-USA\0\0"
#define LSLP_EN_US_LEN 8
  /* the absolute minimum hdr size */
#define LSLP_MIN_HDR  14

  /* slp v2 message types */
#define LSLP_SRVRQST 		 1
#define LSLP_SRVRPLY 		 2
#define LSLP_SRVREG 		 3
#define LSLP_SRVDEREG		 4
#define LSLP_SRVACK		 5
#define LSLP_ATTRREQ		 6
#define LSLP_ATTRRPLY		 7
#define LSLP_DAADVERT		 8
#define LSLP_SRVTYPERQST	 9
#define LSLP_SRVTYPERPLY        10
#define LSLP_SAADVERT		11
#define LSLP_MESHCTRL           12
#define LSLP_MESHCTRL_NULL       0
#define LSLP_MESHCTRL_DATA       2
#define LSLP_MESHCTRL_PEER_IND   3
#define LSLP_MESHCTRL_DA_IND     4
#define LSLP_MESHCTRL_KEEPALIVE  5 
#define LSLP_MESHCTRL_EXT        0x0006
#define LSLP_MESHCTRL_FORWARD    1

  /* slp error codes */
#define LSLP_LANGUAGE_NOT_SUPPORTED 	 1
#define LSLP_PARSE_ERROR 		 2
#define LSLP_INVALID_REGISTRATION	 3
#define LSLP_SCOPE_NOT_SUPPORTED 	 4
#define LSLP_AUTHENTICATION_UNKNOWN 	 5
#define LSLP_AUTHENTICATION_ABSENT       6
#define LSLP_AUTHENTICATION_FAILED       7
#define LSLP_VERSION_NOT_SUPPORTED       9
#define LSLP_INTERNAL_ERROR 	   	10
#define LSLP_DA_BUSY 			11
#define LSLP_OPTION_NOT_UNDERSTOOD      12
#define LSLP_INVALID_UPDATE 		13
#define LSLP_MSG_NOT_SUPPORTED 	        14
#define LSLP_REFRESH_REJECTED 		15

  /*SLP error codes */
#define LSLP_OK				0x00000000
#define LSLPERR_BASE 			0x00002000
#define LSLPERR_INVALID_SEM 	        LSLPERR_BASE + 	0x00000001
#define LSLP_NOT_INITIALIZED            LSLPERR_BASE + 	0x00000002	
#define LSLP_TIMEOUT			LSLPERR_BASE + 	0x00000003
#define LSLP_MALLOC_ERROR		LSLPERR_BASE + 	0x00000004
#define LSLP_FILE_ERROR                 LSLPERR_BASE +  0x00000005
#define LSLP_LDAP_BIND_ERROR            LSLPERR_BASE +  0x00000006
#define LSLP_LDAP_MODIFY_ERROR          LSLPERR_BASE +  0x00000007
#define LSLP_LDAP_NOT_INITIALIZED       LSLPERR_BASE +  0x00000008


  /* offsets into the SLPv2 header */
#define LSLP_VERSION 	 0
#define LSLP_FUNCTION 	 1
#define LSLP_LENGTH 	 2
#define LSLP_FLAGS 		 5
#define LSLP_NEXT_EX 	 7
#define LSLP_XID 		10
#define LSLP_LAN_LEN 	12
#define LSLP_LAN 		14


  /* macros to gain access to SLP header fields */
  /* h = (uint8 *)slpHeader */
  /* o = int32 offset */
  /* i = int32 value */
#if __BYTE_ORDER == __LITTLE_ENDIAN
#define _LSLP_GETBYTE(h, o)  (0x00ff  & *(uint8 *) &((h)[(o)]))
#define _LSLP_GETSHORT(h, o) ((0xff00 & _LSLP_GETBYTE((h), (o)) << 8) + \
				 (0x00ff & _LSLP_GETBYTE((h), (o) + 1)) ) 
#define _LSLP_SETBYTE(h, i, o) ((h)[(o)] = (uint8)i)
#define _LSLP_SETSHORT(h, i, o) { _LSLP_SETBYTE((h), (0xff & ((i) >> 8)), (o)); \
				     _LSLP_SETBYTE((h), (0xff & (i)), (o) + 1); }
#define _LSLP_GETLONG(h, o) ( (0xffff0000 & _LSLP_GETSHORT((h), (o)) << 16) + \
				  (0x0000ffff & _LSLP_GETSHORT((h), (o) + 2)) )
#define _LSLP_SETLONG(h, i, o) 	{ _LSLP_SETSHORT((h), (0xffff & ((i) >> 16)), (o) ); \
				  _LSLP_SETSHORT((h), (0xffff & (i)), (o) + 2); }
  /* length is a 3-byte value */
#define _LSLP_GETLENGTH(h) ((0xff0000 & (*(uint8 *) &((h)[LSLP_LENGTH]) << 16)) + \
			(0x00ff00 & (*(uint8 *) &((h)[LSLP_LENGTH + 1]) << 8)) + \
			(0x0000ff & (*(uint8 *) &((h)[LSLP_LENGTH + 2]))))
#define _LSLP_SETLENGTH(h, i) {_LSLP_SETSHORT( (h), (((i) & 0xffff00) >> 8), LSLP_LENGTH ); \
			  _LSLP_SETBYTE((h), ((i) & 0x0000ff), LSLP_LENGTH + 2);}
  /* next option offset is a 3-byte value */
#define _LSLP_GETFIRSTEXT(h) ((0xff0000 & (*(uint8 *) &((h)[LSLP_NEXT_EX]) << 16)) + \
			(0x00ff00 & (*(uint8 *) &((h)[LSLP_NEXT_EX + 1]) << 8)) + \
				(0x0000ff & (*(uint8 *) &((h)[LSLP_NEXT_EX + 2]))))
#define _LSLP_SETFIRSTEXT(h, i) {_LSLP_SETSHORT((h), (((i) & 0xffff00) >> 8), LSLP_NEXT_EX ); \
			  _LSLP_SETBYTE((h), ((i) & 0x0000ff), LSLP_NEXT_EX + 2);}

#define _LSLP_GETNEXTEXT(h, o) ((0xff0000 & (*(uint8 *) &((h)[(o)]) << 16)) + \
			(0x00ff00 & (*(uint8 *) &((h)[(o) + 1]) << 8)) + \
				(0x0000ff & (*(uint8 *) &((h)[(o) + 2]))))
#define _LSLP_SETNEXTEXT(h, i, o) {_LSLP_SETSHORT((h), (((i) & 0xffff00) >> 8), (o) ); \
			  _LSLP_SETBYTE((h), ((i) & 0x0000ff), (o) + 2);}

#else /* BIG ENDIAN */

#define _LSLP_GETBYTE(h, o)  (0x00ff  & *((uint8 *) &((h)[(o)])))
#define _LSLP_GETSHORT(h, o) (0xffff & *((uint16 *) &((h)[(o)])))
#define _LSLP_SETBYTE(h, i, o) ((h)[(o)] = (uint8)(i))
#define _LSLP_SETSHORT(h, i, o) (*((uint16 *) &((h)[(o)])) = (0xffff & (i)))
#define _LSLP_GETLONG(h, o)  (0xffffffff & *((uint32 *)&((h)[(o)])))
#define _LSLP_SETLONG(h, i, o) 	(*((uint32 *)&((h)[(o)])) = (0xffffffff * (i)))
  /* length is a 3-byte value */
#define _LSLP_GETLENGTH(h) (0x00ffffff & *((uint32 *)(&h[LSLP_LENGTH])))
#define _LSLP_SETLENGTH(h, i) {_LSLP_SETSHORT((h), (0x00ffff00 & (i)), LSLP_LENGTH) ; \
                                _LSLP_SETBYTE((h), (0x000000ff & (i)), LSLP_LENGTH + 2);}
  /* next option offset is a 3-byte value */
#define _LSLP_GETFIRSTEXT(h) (0x00ffffff & *((uint32 *)(&h[LSLP_NEXT_EX])))
#define _LSLP_SETFIRSTEXT(h, i) {_LSLP_SETSHORT((h), (0x00ffff00 & (i)), LSLP_NEXT_EX) ; \
                                _LSLP_SETBYTE((h), (0x000000ff & (i)), LSLP_NEXT_EX + 2);}
#define _LSLP_GETNEXTEXT(h, o) (0x00ffffff & *((uint32 *)(&h[(o)])))
#define _LSLP_SETNEXTEXT(h, i, o) {_LSLP_SETSHORT((h), (0x00ffff00 & (i)), (o)) ; \
                                _LSLP_SETBYTE((h), (0x000000ff & (i)), (o) + 2);}


#endif /* ENDIAN definitions */

  /* macros to get and set header fields */
#define _LSLP_GETVERSION(h) _LSLP_GETBYTE((h), LSLP_VERSION)
#define _LSLP_SETVERSION(h, i) _LSLP_SETBYTE((h), (i), LSLP_VERSION)
#define _LSLP_GETFUNCTION(h) _LSLP_GETBYTE((h), LSLP_FUNCTION)
#define _LSLP_SETFUNCTION(h, i) _LSLP_SETBYTE((h), (i), LSLP_FUNCTION)
#define _LSLP_GETFLAGS(h) _LSLP_GETBYTE((h), LSLP_FLAGS)
#define _LSLP_SETFLAGS(h, i) _LSLP_SETBYTE((h), (i), LSLP_FLAGS)
#define _LSLP_GETLANLEN(h) _LSLP_GETSHORT((h), LSLP_LAN_LEN)
#define _LSLP_SETLANLEN(h, i) _LSLP_SETSHORT((h), (i), LSLP_LAN_LEN)
  /* s = int8 *language-string */
#define _LSLP_SETLAN(h, s) {_LSLP_SETSHORT((h),(_LSLP_MIN( (strlen((s)) + 2), 17)), LSLP_LAN_LEN); \
		    memcpy(&(h)[LSLP_LAN],(s),(_LSLP_MIN( (strlen((s)) + 2) , 17)));}
#define _LSLP_GETXID(h) _LSLP_GETSHORT((h), LSLP_XID) 
#define _LSLP_SETXID(h, i) _LSLP_SETSHORT((h), (i), LSLP_XID)
#define _LSLP_HDRLEN(h) (14 + _LSLP_GETLANLEN((h))) 
  /* give (work *), how big is the msg header ? */
#define _LSLP_HDRLEN_WORK(work) (14 + (work)->hdr.langLen ) 
  /* given (work *), how big is the data field ? */
#define _LSLP_DATALEN_WORK(work) (((work)->hdr.len) - _LSLP_HDRLEN_WORK((work)))

#define LSLP_FLAGS_OVERFLOW 0x80
#define LSLP_FLAGS_FRESH    0x40
#define LSLP_FLAGS_MCAST    0x20

  /******** internal slp message definitions ********/

  /* URL entry definitions */
#define LSLP_URL_LIFE    1
#define LSLP_URL_LEN	 3
#define _LSLP_GETURLLIFE(h) _LSLP_GETSHORT((h), LSLP_URL_LIFE)
#define _LSLP_SETURLLIFE(h, i) _LSLP_SETSHORT((h), (i), LSLP_URL_LIFE)
#define _LSLP_GETURLLEN(h) _LSLP_GETSHORT((h), LSLP_URL_LEN)
#define _LSLP_SETURLLEN(h, i) _LSLP_SETSHORT((h), (i), LSLP_URL_LEN)

#define LSLP_MTU 1432



#endif /* _LSLPDEFS_INCLUDE */


