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


#include <Pegasus/Common/System.h>


#ifdef _WIN32
#include "lslp-perl-windows.h"
#endif	/* win 32 */

#ifdef __linux__
#include "lslp-perl-linux.h"
#endif

#include "lslp-perl.h"



#define LSLP_LIB_HANDLE_SIGNATURE "liBsLpHanDLe\0"
#define LSLP_LIB_HANDLE_SIGNATURE_LEN 13
#define LSLP_LIB_HANDLE_VERSION 1
typedef struct lslp_lib_handle {
  int8 signature[LSLP_LIB_HANDLE_SIGNATURE_LEN + 1] ;
  int8 version;
  uint16 xid;
  int16 target_port;
  uint32 target_addr;
  uint32 local_addr;
  int8 *spi;
  int16 pr_buf_len;
  int8 *pr_buf;
  int8 *msg_buf;
  int8 *id;
  int8 err_buf[255];
  struct timeval tv;
  int8 retries;
  uint8 ttl;
  int8 delimitor;
  int8 convergence;
  void *crypto_context;
} LSLP_HANDLE;

#define NUM_LIB_HANDLES 1
LSLP_HANDLE lslp_lib_handles[1] ;

#define DA_SRVTYPE "service:directory-agent:\0"
#define DA_SRVTYPELEN 25
#define DA_SCOPE "DEFAULT\0"
#define DA_SCOPELEN 8


typedef struct lslp_lib_daadv {
  struct lslp_lib_daadv *next;
  struct lslp_lib_daadv *prev;
  BOOL isHead ;
  int type;
  int8 function;
  uint16 err;
  uint32 stateless_boot;
  int8 *url;
  int8 *scope;
  int8 *attr;
  int8 *spi;
  int8 auth_blocks;
  int8 *auth;
  int8 remote[16]; 
} LSLP_LIB_DAADVERT;

typedef struct lslp_lib_srvrply {
  struct lslp_lib_srvrply *next;
  struct lslp_lib_srvrply *prev;
  BOOL isHead;
  int type;
  int8 function;
  uint16 err;
  uint16 lifetime;
  int8 *url;
  int8 auth_blocks;
  int8 *auth;
  int8 remote[16];
} LSLP_LIB_SRVRPLY;

#define TYPE_DA_LIST 1
#define TYPE_RPLY_LIST 2
typedef struct lslp_lib_list {
  struct lslp_lib_list *next;
  struct lslp_lib_list *prev;
  BOOL isHead;
  int type;
} LSLP_LIB_LIST;

#ifdef _WIN32
#define _LSLP_PERROR(h, a, b) _snprintf(&((h)->err_buf[0]), 254, "%s: Error %s (%s).", (a), (b), strerror(errno))
#else
#define _LSLP_PERROR(h, a, b) snprintf(&((h)->err_buf[0]), 254, "%s: Error %s (%s).", (a), (b), strerror(errno))
#endif




static LSLP_HANDLE *get_handle(const int8 *id) ;
static void put_handle(LSLP_HANDLE *h) ;
PEGASUS_EXPORT BOOL lslp_lib_set_convergence(const int8 *id, int8 convergence) ;
PEGASUS_EXPORT BOOL lslp_lib_set_port(const int8 *id, int16 target_port) ;
static void lslp_lib_set_addr_internal(const int8 *new_addr, uint32 *old_addr);
PEGASUS_EXPORT BOOL lslp_lib_set_target_addr(const int8 *id, const int8 *target_addr);
PEGASUS_EXPORT BOOL lslp_lib_set_local_interface(const int8 *id, const int8 *);
PEGASUS_EXPORT BOOL lslp_lib_set_delimitor(const int8 *id, const int8 delimitor);
static int8 *copy_unescape(int8 *dst, int16 dst_len, int8 *src);
static int8 *copy_escape(int8 *dst, int16 dst_len, int8 *src, int8 esc );
PEGASUS_EXPORT BOOL lslp_lib_set_timout_retry(const int8 *id, uint32 t_sec, uint32 t_usec, int8 retries, uint8 ttl);
PEGASUS_EXPORT BOOL lslp_lib_set_spi(const int8 *id, const int8 *spi) ;
PEGASUS_EXPORT BOOL lslp_lib_init(const int8 *id, 
		   const int8 *target_addr, 
		   const int8 *local_addr, 
		   int16 target_port, 
		   const int8 *spi);
PEGASUS_EXPORT void lslp_lib_deinit(const int8 *id);
static void prepare_pr_buf(LSLP_HANDLE *h, const int8 *a);
static BOOL prepare_query( LSLP_HANDLE *h, 
			   uint16 xid,
			   const int8 *service_type,
			   const int8 *scopes, 
			   const int8 *predicate  ) ;
PEGASUS_EXPORT LSLP_LIB_LIST *lslp_lib_converge_srv_req(const int8 *id, 
					 const int8 *type, 
					 const int8 *predicate, 
					 const int8 *scopes) ;
PEGASUS_EXPORT LSLP_LIB_LIST  *lslp_lib_srv_req(const int8 *id, 
				 const int8 *type, 
				 const int8 *predicate, 
				 const int8 *scopes);
static void lslp_lib_decode_reply(LSLP_HANDLE *, LSLP_LIB_LIST **, struct sockaddr_in *) ;
static void lslp_lib_decode_srvrply(LSLP_HANDLE *, LSLP_LIB_LIST **, struct sockaddr_in *) ;
static void lslp_lib_decode_daadvert(LSLP_HANDLE *, LSLP_LIB_LIST **, struct sockaddr_in *);
PEGASUS_EXPORT BOOL lslp_lib_srv_reg(int8 *id, 
		      int8 *url,
		      int8 *attributes,
		      int8 *service_type,
		      int8 *scopes,
		      int16 lifetime);
PEGASUS_EXPORT void lslp_lib_free_list(LSLP_LIB_LIST *list) ;
PEGASUS_EXPORT int8 *lslp_lib_get_host_name(void) ;

static BOOL lslp_lib_send_rcv_udp(LSLP_HANDLE *h, 
				  void (*decode_func)(LSLP_HANDLE *, LSLP_LIB_LIST **, struct sockaddr_in *), 
				  LSLP_LIB_LIST **parm) ;

PEGASUS_EXPORT void lslp_lib_srv_reg_all(int8 *id, 
			  int8 *url,
			  int8 *attributes,
			  int8 *service_type,
			  int8 *scopes,
			  int16 lifetime,
			  int *succeeded,
			  int *failed) ;

PEGASUS_EXPORT BOOL lslp_lib_get_addr_from_url(const int8 *url, struct sockaddr_in *addr ) ;
PEGASUS_EXPORT int8 *lslp_lib_get_addr_string_from_url(const int8 *url) ;



static LSLP_HANDLE *get_handle(const int8 *id)
{
  int i;
  LSLP_HANDLE *h;
  assert(id != NULL);
  if(id == NULL)
    return(NULL);
  for( i = 0, h = &lslp_lib_handles[0] ; i < NUM_LIB_HANDLES; i++ , h++) {
    if( ! strncmp(h->signature, LSLP_LIB_HANDLE_SIGNATURE, LSLP_LIB_HANDLE_SIGNATURE_LEN)  ) {
      if(h->version == LSLP_LIB_HANDLE_VERSION) {
	if (! strcmp(id, h->id))
	  return(h);
      }
    }
  }

  for( i = 0, h = &lslp_lib_handles[0] ; i < NUM_LIB_HANDLES; i++ , h++) {
    if(h->id == NULL) {
      memset(h, 0x00, sizeof(LSLP_HANDLE));
      strcpy(&(h->signature[0]), LSLP_LIB_HANDLE_SIGNATURE);
      h->version = LSLP_LIB_HANDLE_VERSION;
      if(NULL != (h->id = strdup(id))) {
	if(NULL != (h->pr_buf = (int8 *)malloc(LSLP_MTU ))) {
	  if(NULL != (h->msg_buf = (int8 *)malloc(LSLP_MTU ))) { 
	    h->local_addr = INADDR_ANY ;
	    h->target_addr = _LSLP_MCAST;
	    h->target_port = htons(LSLP_PORT);
	    h->tv.tv_usec = 150000; 
	    h->ttl = 255;
	    h->delimitor = '~';
	    h->retries = 3;
	    h->convergence = 3;
	    return(h);
	  }
	  free(h->pr_buf);
	}
	free(h->id);
      }
    }
  }
  return(NULL);
}

static void put_handle(LSLP_HANDLE *h)
{
  assert(h != NULL);
  assert(h->id != NULL);
  if(h != NULL) {
    if( ! strncmp(h->signature, LSLP_LIB_HANDLE_SIGNATURE, LSLP_LIB_HANDLE_SIGNATURE_LEN)  ) {
      if(h->version == LSLP_LIB_HANDLE_VERSION) {
	free(h->pr_buf);
	free(h->id);
	free(h->msg_buf);
	memset(h, 0x00, sizeof(LSLP_HANDLE));
      }
    }
  }
  return;
}


PEGASUS_EXPORT BOOL lslp_lib_set_convergence(const int8 *id, int8 convergence) 
{
  LSLP_HANDLE *h;
  assert(id != NULL);
  if(id == NULL)
    return(FALSE);
  if(NULL != (h = get_handle(id))) {
    if(convergence < 0)
      convergence = 1;
    h->convergence = convergence;
    return(TRUE);
  }
  return(FALSE);
}

PEGASUS_EXPORT BOOL lslp_lib_set_port(const int8 *id, int16 target_port) 
{
  LSLP_HANDLE *h;
  assert(id != NULL);
  if(id == NULL)
    return(FALSE);
  if(NULL != (h = get_handle(id))) {
    if(target_port < 0)
      target_port = 0;
    h->target_port = htons(target_port);
    return(TRUE);
  }
  return(FALSE);
}

static void lslp_lib_set_addr_internal(const int8 *new_addr, uint32 *old_addr)
{

  if(new_addr == NULL) 
    *old_addr = INADDR_ANY ;
  else 
    *old_addr = inet_addr(new_addr);
  return;
}

PEGASUS_EXPORT BOOL lslp_lib_set_target_addr(const int8 *id, const int8 *target_addr)
{
  
  LSLP_HANDLE *h;
  assert(id != NULL);
  if(id == NULL)
    return(FALSE);
  if(NULL != (h = get_handle(id))) {
    lslp_lib_set_addr_internal(target_addr, &(h->target_addr)) ;
    return(TRUE);
  }
  return(FALSE);
}

PEGASUS_EXPORT BOOL lslp_lib_set_local_interface(const int8 *id, const int8 *intface)
{
  LSLP_HANDLE *h;
  assert(id != NULL);
  if(id == NULL)
    return(FALSE);
  if(NULL != (h = get_handle(id))) {
    lslp_lib_set_addr_internal(intface, &(h->local_addr)) ;
    return(TRUE);
  }
  return(FALSE);
}

PEGASUS_EXPORT BOOL lslp_lib_set_delimitor(const int8 *id, const int8 delimitor)
{
  LSLP_HANDLE *h;
  assert(id != NULL);
  if(id == NULL)
    return(FALSE);
  if(NULL != (h = get_handle(id))) {
    h->delimitor = delimitor;
    return(TRUE);
  }
  return(FALSE);
}



static int8 *copy_unescape(int8 *dst, int16 dst_len, int8 *src)
{
  int8 *dst_ptr, *src_ptr;
  int8 tmp;
  int16 len = 0;
  dst_ptr = dst;
  src_ptr = src;
  while (*src_ptr != 0x00 && len < dst_len - 1) {
    if(*src_ptr == '\\') {
      if(isxdigit(*(src_ptr + 1)) && isxdigit(*(src_ptr + 2))) {
	tmp = *(src_ptr + 3);
	*(src_ptr + 3) = 0x00;
	*dst_ptr = (int8)atoi(src_ptr + 1);
	src_ptr += 3;
	*src_ptr = tmp;
	dst_ptr++;
	len++;
	continue;
      }
    }
    *dst_ptr = *src_ptr;
    dst_ptr++; src_ptr++; len++;
  }
  *dst_ptr = 0x00;
  return(dst);
}

static int8 *copy_escape(int8 *dst, int16 dst_len, int8 *src, int8 esc )
{
  int8 *dst_ptr, *src_ptr;
  int16 len = 0;
  dst_ptr = dst;
  src_ptr = src;
  while (*src_ptr != 0x00 && len < dst_len - 1) {
    if(*src_ptr == esc) {
      if(len + 3 < dst_len) {
	sprintf(dst_ptr, "\\%02x", esc) ;
	dst_ptr += 3;
	src_ptr++;
	len += 3;
	continue;
      } else {*dst_ptr = 0x00; return(dst) ;}
    }
    *dst_ptr = *src_ptr;
    dst_ptr++; src_ptr++; len++;
  }
  *dst_ptr = 0x00;
  return(dst);
}

PEGASUS_EXPORT BOOL lslp_lib_set_timout_retry(const int8 *id, uint32 t_sec, uint32 t_usec, int8 retries, uint8 ttl)
{
  LSLP_HANDLE *h;
  assert(id != NULL);
  if(id == NULL)
    return(FALSE);
  if(retries < 0)
    retries = 0;
  if(ttl == 0)
    ttl = 255;
  if(NULL != (h = get_handle(id))) {
    h->tv.tv_sec = t_sec;
    h->tv.tv_usec = t_usec;
    h->retries = retries;
    h->ttl = ttl;
    return(TRUE);
  }
  return(FALSE);
}

PEGASUS_EXPORT BOOL lslp_lib_set_spi(const int8 *id, const int8 *spi) 
{
  spi = spi;
  return(FALSE);
}

PEGASUS_EXPORT BOOL lslp_lib_init(const int8 *id, 
		   const int8 *target_addr, 
		   const int8 *local_addr, 
		   int16 target_port, 
		   const int8 *spi)
{

  LSLP_HANDLE *h;
  assert(id != NULL);
  if(id == NULL)
    return(FALSE);
  if(NULL != ( h = get_handle(id))) {
    if(target_addr != NULL)
      lslp_lib_set_addr_internal(target_addr, &(h->target_addr));
    if(local_addr != NULL)
      lslp_lib_set_addr_internal(local_addr, &(h->local_addr));
    if(target_port >= 0)
      h->target_port = htons(target_port);
    if(spi != NULL) { ; }
#ifdef _WIN32
    {
      WSADATA ws_data;
      WSAStartup(0x0002, &ws_data);
    }
#endif
    return(TRUE);
  }
  return(FALSE);
}


PEGASUS_EXPORT void lslp_lib_deinit(const int8 *id)
{
  assert(id != NULL);
  if(id != NULL)
    put_handle(get_handle(id));
#ifdef _WIN32
  WSACleanup();
#endif
  return;
}

static void prepare_pr_buf(LSLP_HANDLE *h, const int8 *a)
{
  assert(h != NULL && a != NULL);
  if(h->pr_buf_len > 0) 
    *(h->pr_buf + h->pr_buf_len - 1) = ',' ;
  do {
    *(h->pr_buf + h->pr_buf_len) = *a;
    a++;
    (h->pr_buf_len)++;
  }while((*a != 0x00) && (h->pr_buf_len < LSLP_MTU - 1)); 
  (h->pr_buf_len)++;
}

static BOOL prepare_query(LSLP_HANDLE *h, 
			  uint16 xid,
			  const int8 *service_type,
			  const int8 *scopes, 
			  const int8 *predicate  ) 
{
  int16 len, total_len;
  int8 *bptr;
  
  assert(h != NULL );
  if( ! strncmp(h->signature, LSLP_LIB_HANDLE_SIGNATURE, LSLP_LIB_HANDLE_SIGNATURE_LEN)  ) {
    if(h->version == LSLP_LIB_HANDLE_VERSION) {
      assert(h->msg_buf != NULL && h->pr_buf != NULL);
      if(xid != h->xid) {
	/* this is a new request */
	memset(h->pr_buf, 0x00, LSLP_MTU);
	h->pr_buf_len = 0;
	h->xid = xid;
      }
      memset(h->msg_buf, 0x00, LSLP_MTU);
      bptr = h->msg_buf;
      _LSLP_SETVERSION(bptr, LSLP_PROTO_VER);
      _LSLP_SETFUNCTION(bptr, LSLP_SRVRQST);
      /* we don't know the length yet */
      _LSLP_SETFLAGS(bptr, LSLP_FLAGS_MCAST);
      _LSLP_SETXID(bptr, xid);
      _LSLP_SETLAN(bptr, LSLP_EN_US);
      bptr += ( total_len = _LSLP_HDRLEN(bptr) ) ;
    
      if(h->pr_buf_len + total_len < LSLP_MTU) {
	/* set the pr list length */
	_LSLP_SETSHORT(bptr, (len = h->pr_buf_len), 0);
	if(len)
	  memcpy(bptr + 2, h->pr_buf, len);
	total_len += ( 2 + len );
	bptr += (2 + len);
		
	if(service_type == NULL)
	  len = DA_SRVTYPELEN;
	else
	  len = strlen(service_type) + 1;
	if(total_len + 2 + len < LSLP_MTU) {
	  /* set the service type string length */
	  _LSLP_SETSHORT(bptr, len, 0);
	  if(service_type != NULL)
	    memcpy(bptr + 2, service_type, len);
	  else
	    memcpy(bptr + 2, DA_SRVTYPE, len);
	  	  
	  total_len += (2 + len);
	  bptr += (2 + len);

	  /* set the scope len and scope type, advance the buffer */
	  
	  if(scopes == NULL)
	    len = DA_SCOPELEN;
	  else
	    len = strlen(scopes) + 1;
	  if( total_len + 2 + len < LSLP_MTU) {
	    _LSLP_SETSHORT(bptr, len, 0);
	    if(scopes != NULL) 
	      memcpy(bptr + 2, scopes, len);
	    else
	      memcpy(bptr + 2, DA_SCOPE, DA_SCOPELEN);

	    total_len += ( 2 + len);
	    bptr += (2 + len);

	    /* stuff the predicate if there is one  */
	    if(predicate == NULL)
	      len = 0;
	    else
	      len = strlen(predicate) + 1;
	    if( total_len + 2 + len < LSLP_MTU) {
	      _LSLP_SETSHORT(bptr, len, 0);
	      if(predicate != NULL)
		memcpy(bptr + 2, predicate, len);
	      
	      total_len += (2 + len);
	      bptr += (2 + len);
	      
	      /* stuff the spi */
	      
	      /* set the spi len and spi string */
	      if(h->spi == NULL)
		len = 0;
	      else
		len = strlen(h->spi) + 1;
	      
	      if(total_len + 2 + len < LSLP_MTU) {
		_LSLP_SETSHORT(bptr, len, 0);
		
		if(h->spi != NULL)
		  memcpy(bptr + 2, h->spi, len);
		
		total_len += ( 2 + len);
		bptr += (2 + len);
		assert(total_len == bptr - h->msg_buf);
		/* now go back and set the length for the entire message */
		_LSLP_SETLENGTH(h->msg_buf, total_len );
		return(TRUE);
	      } /* room for the spi  */
	    } /*  room for predicate  */
	  } /* room for the scope  */
	} /* room for the service type  */
      } /* room for the pr list  */
      _LSLP_PERROR(h, "prepare_query", "out of room in transmission buffer"); 
    } else {_LSLP_PERROR(h, "prepare_query", "bad library handle version");  }
  } else { _LSLP_PERROR(h, "prepare_query", "bad library handle signature"); }
  return(FALSE);
}



PEGASUS_EXPORT LSLP_LIB_LIST *lslp_lib_converge_srv_req(const int8 *id, 
					 const int8 *type, 
					 const int8 *predicate, 
					 const int8 *scopes)
{

  if(TRUE == lslp_lib_set_target_addr(id, "239.255.255.253") ) {
    if(TRUE == lslp_lib_set_convergence(id, 3) ) {
      return(lslp_lib_srv_req(id, type, predicate, scopes) );
    }
  }
  return(NULL);
}


PEGASUS_EXPORT LSLP_LIB_LIST  *lslp_lib_srv_req(const int8 *id, 
				 const int8 *type, 
				 const int8 *predicate, 
				 const int8 *scopes)
{
  LSLP_HANDLE *h;
  LSLP_LIB_LIST *response_list = NULL;

  assert(id != NULL) ;
  if(id != NULL && (NULL != (h = get_handle(id)))) {
    if( ! strncmp(h->signature, LSLP_LIB_HANDLE_SIGNATURE, LSLP_LIB_HANDLE_SIGNATURE_LEN)  ) {
      if(h->version == LSLP_LIB_HANDLE_VERSION) {
      	if ((TRUE == prepare_query(h, (uint16)(h->xid + 1), type, scopes, predicate ))) {
	  uint16 convergence = h->convergence;
	  if(convergence-- > 0) { _LSLP_SETFLAGS(h->msg_buf, LSLP_FLAGS_MCAST) ; }
	  lslp_lib_send_rcv_udp(h, lslp_lib_decode_reply, &response_list) ;
	  while(convergence-- > 0) {
	    _LSLP_SLEEP( h->tv.tv_usec / 1000 ) ;
	    if(prepare_query(h, h->xid, type, scopes, predicate)) {
	      _LSLP_SETFLAGS(h->msg_buf, LSLP_FLAGS_MCAST) ;
	      lslp_lib_send_rcv_udp(h, lslp_lib_decode_reply, &response_list);
	    }
	  }
	  /* if this was a multicast request try it on the local machine too  */
	  if(h->convergence) {
	    /* save current settings  */
	    int8 convergence_save = h->convergence;
	    uint32 target_save = h->target_addr;
	    h->convergence = 0;
	    h->target_addr = inet_addr("127.0.0.1" ) ;
	    if ((TRUE == prepare_query(h, (uint16)(h->xid + 1), type, scopes, predicate ))) {
	      _LSLP_SETFLAGS(h->msg_buf, 0 );
	      lslp_lib_send_rcv_udp(h, lslp_lib_decode_reply, &response_list);
	    }
	    /* restore current settings  */
	    h->convergence = convergence_save;
	    h->target_addr = target_save;
	  } /* if we need to do the query on the local machine  */
	} /* prepared query  */
      } /* handle is right version  */
    } /*  handle has right signature  */
  } /*  if we got our handle  */
  memset(h->msg_buf, 0x00, LSLP_MTU);
  return(response_list);
}

static void lslp_lib_decode_reply(LSLP_HANDLE *h, 
				  LSLP_LIB_LIST **response_list, 
				  struct sockaddr_in *remote)
{
  assert(h != NULL );
  
  if(h->xid == _LSLP_GETXID(h->msg_buf)) {
    int8 function = _LSLP_GETFUNCTION(h->msg_buf);
    switch(function) {
    case LSLP_DAADVERT:
      assert(response_list != NULL);
      lslp_lib_decode_daadvert(h, response_list, remote);
	return;		
    case LSLP_SRVRPLY:
      assert(response_list != NULL);
      lslp_lib_decode_srvrply(h, response_list, remote);
	  return;
    case LSLP_SRVACK:
    default:
      break;
    }
  } /* msg has expected xid */
  return;
}

static void lslp_lib_decode_srvrply(LSLP_HANDLE *h, 
				    LSLP_LIB_LIST **response_list, 
				    struct sockaddr_in *remote)
{
  int8 *bptr;
  int16 str_len, err, count;
  int32 total_len, purported_len;
  LSLP_LIB_SRVRPLY *rply;

  if(*response_list == NULL) {
    if(NULL == (*response_list = (LSLP_LIB_LIST *)malloc(sizeof(LSLP_LIB_LIST))))
      abort();
    (*response_list)->next = (*response_list)->prev = *response_list;
    (*response_list)->isHead = TRUE;
    (*response_list)->type = TYPE_RPLY_LIST;
  }

  bptr = h->msg_buf;
  purported_len = _LSLP_GETLENGTH(bptr);
  bptr += (total_len = _LSLP_HDRLEN(bptr));
  if(total_len < purported_len) {
    if(NULL != (rply = (LSLP_LIB_SRVRPLY *)calloc(1, sizeof(LSLP_LIB_SRVRPLY)))) {
      rply->type = TYPE_RPLY_LIST;
      err = _LSLP_GETSHORT(bptr, 0);
      count = _LSLP_GETSHORT(bptr, 2);
      bptr += 4;
      total_len += 4;
      /* loop on the url entries  */
      while( ( total_len <= purported_len ) && ( count > 0 ) && ( rply != NULL ) ) {
	rply->function = LSLP_SRVRPLY;
	rply->err = err;
	rply->lifetime = _LSLP_GETSHORT(bptr, 1);
	total_len += (5 + (str_len = _LSLP_GETSHORT(bptr, 3)));
	if(total_len <= purported_len) {
	  int8 num_auths;
	  if(str_len > 0) {
	    if(NULL != (rply->url = (int8 *)malloc(str_len + 1))) {
	      memcpy(rply->url, bptr + 5, str_len);
	      *((rply->url) + str_len) = 0x00;
	    } else { abort(); }
	    bptr += (5 + str_len);
	    rply->auth_blocks = (num_auths = _LSLP_GETBYTE(bptr, 0));
	    total_len += 1;
	    bptr += 1;
	    while(num_auths && (total_len <= purported_len )) {
	    /* iterate past the authenticators for now  */
	      /* need extra code here to authenticate url entries  */
	      total_len += (str_len = _LSLP_GETSHORT(bptr, 2));
	      bptr += str_len;
	      num_auths--;
	    }
	    strcpy(&(rply->remote[0]), inet_ntoa( remote->sin_addr )) ;
	   
	    _LSLP_INSERT(((LSLP_LIB_LIST *)rply), (*response_list));
	    count--;
	    if((total_len <= purported_len) && (count > 0) )
	      rply = (LSLP_LIB_SRVRPLY *)calloc(1, sizeof(LSLP_LIB_SRVRPLY));
	    else
	      rply = NULL;
	  } else { goto bad_packet; } /* bad packet */
	} else { goto bad_packet; }  /*  bad packet */
      } 
    } else { abort(); } 
  } /* if the hdr length field is consistent with reality */
  return;
bad_packet:
  _LSLP_PERROR(h, "lslp_lib_decode_srvrply", "recieved corrupt service reply");
  free(rply); 
  return;
}

  
static void lslp_lib_decode_daadvert(LSLP_HANDLE *h, 
				     LSLP_LIB_LIST **response_list,
				     struct sockaddr_in *remote)
{
  int8 *bptr;
  int16 str_len;
  int32 total_len, purported_len;
  LSLP_LIB_DAADVERT *adv;
  
  if(*response_list == NULL) {
    if(NULL == (*response_list = (LSLP_LIB_LIST *)malloc(sizeof(LSLP_LIB_LIST))))
      abort();
    (*response_list)->next = (*response_list)->prev = *response_list;
    (*response_list)->isHead = TRUE;
    (*response_list)->type = TYPE_DA_LIST;
  }
  
  bptr = h->msg_buf;
  purported_len = _LSLP_GETLENGTH(bptr);
  bptr += (total_len = _LSLP_HDRLEN(bptr));
  if(total_len < purported_len) {
    if(NULL != (adv = (LSLP_LIB_DAADVERT *)calloc(1, sizeof(LSLP_LIB_DAADVERT)))) {
      adv->type = TYPE_DA_LIST;
      adv->function = LSLP_DAADVERT;
      adv->err = _LSLP_GETSHORT(bptr, 0);
      adv->stateless_boot = _LSLP_GETLONG(bptr, 2);
      total_len += (8 + (str_len = _LSLP_GETSHORT(bptr, 6)));
      if(total_len < purported_len) {
	/* decode and capture the url  - note: this is a string, not a url-entry structure */
	if(NULL != (adv->url = (int8 *)malloc(str_len + 1))) {
	  memcpy(adv->url, bptr + 8, str_len);
	  *((adv->url) + str_len) = 0x00;
	  /* advance the pointer past the url string */
	  bptr += (str_len + 8);
	  total_len += (2 + (str_len = _LSLP_GETSHORT(bptr, 0)));
	  if(total_len < purported_len) {
	    if(str_len > 0) {
	      if(NULL != (adv->scope = (int8 *)malloc(str_len + 1))) {
		memcpy(adv->scope, bptr + 2, str_len);
		*((adv->scope) + str_len) = 0x00;
	      } else { abort() ;}
	    }
	    /* advance the pointer past the scope string  */
	    bptr += (str_len + 2);
	    total_len += (2 + (str_len = _LSLP_GETSHORT(bptr, 0)));
	    if(total_len < purported_len) {
	      if(str_len > 0) {
		if(NULL != (adv->attr = (int8 *)malloc(str_len + 1))) {
		  memcpy(adv->attr, bptr + 2, str_len);
		  *((adv->attr) + str_len) = 0x00;
		} else { abort() ; }
	      }
	      /* advance the pointer past the attr string */
	      bptr += (str_len + 2);
	      total_len += (2 + (str_len = _LSLP_GETSHORT(bptr, 0)));
	      if(total_len < purported_len) {
		if(str_len > 0 ) {
		  if(NULL != (adv->spi = (int8 *)malloc(str_len + 1))) {
		    memcpy(adv->spi, bptr + 2, str_len);
		    *((adv->spi) + str_len) = 0x00;
		  } else { abort(); }
		} /*  if there is an spi  */
		
		/* advance the pointer past the spi string  */
		bptr += (str_len + 2);
		adv->auth_blocks = _LSLP_GETBYTE(bptr, 0);
		
		/* need code here to handle authenticated urls */
		strcpy(&(adv->remote[0]), inet_ntoa(remote->sin_addr)) ;
		_LSLP_INSERT(((LSLP_LIB_LIST *)adv), (*response_list));
		return;

	      } /*  spi length field is consistent with hdr */
	    } /* attr length field is consistent with hdr */
	  } /*  scope length field is consistent with hdr */
	  _LSLP_PERROR(h, "lslp_lib_decode_daadvert", "message had bad hdr"); 
	} else { abort() ; } /* allocated url buffer  */
      } else { _LSLP_PERROR(h, "lslp_lib_decode_daadvert", "message had bad hdr"); }
    } else { abort() ; }
  } else { _LSLP_PERROR(h, "lslp_lib_decode_daadvert", "message had bad hdr");  }
  return;
}


PEGASUS_EXPORT BOOL lslp_lib_srv_reg(int8 *id, 
		      int8 *url,
		      int8 *attributes,
		      int8 *service_type,
		      int8 *scopes,
		      int16 lifetime) 
{
  int32 len;
  int16 str_len;
  int8 *bptr;
  LSLP_HANDLE *h;
  assert(id != NULL && url != NULL && service_type != NULL ) ;
  if(id != NULL && (NULL != (h = get_handle(id)))) {

    if( ! strncmp(h->signature, LSLP_LIB_HANDLE_SIGNATURE, LSLP_LIB_HANDLE_SIGNATURE_LEN)  ) {
      if(h->version == LSLP_LIB_HANDLE_VERSION) {
	assert(h->msg_buf != NULL && h->pr_buf != NULL);

	/* this is always a new request */
	memset(h->pr_buf, 0x00, LSLP_MTU);
	h->pr_buf_len = 0;
	h->xid++ ;

	memset(h->msg_buf, 0x00, LSLP_MTU);
	bptr = h->msg_buf;
	_LSLP_SETVERSION(bptr, LSLP_PROTO_VER);
	_LSLP_SETFUNCTION(bptr, LSLP_SRVREG);
	/* we don't know the length yet */
	_LSLP_SETXID(bptr, h->xid);
	_LSLP_SETLAN(bptr, LSLP_EN_US);
	bptr += (len =  _LSLP_HDRLEN(bptr) ) ;
	
	/* construct a url-entry  */

	_LSLP_SETSHORT(bptr, lifetime, 1);
	_LSLP_SETSHORT(bptr, (str_len = (strlen(url) + 1)), 3);
	len += (5 + str_len );
	if(len + 1 < LSLP_MTU ) {
	  memcpy(bptr + 5, url, str_len);
	  bptr += (5 + str_len);
	  /* no auth blocks for now  */
	  _LSLP_SETBYTE(bptr, 0x00, 0);
	  bptr++;
	  len++;
	  /* stuff the service type  */
	  str_len = strlen(service_type) + 1;
	  if(len + 2 + str_len < LSLP_MTU) {
	    _LSLP_SETSHORT(bptr, str_len, 0);
	    memcpy(bptr + 2, service_type, str_len);
	    bptr += (2 + str_len);
	    len += (2 + str_len);
	    /* stuff the scope list if there is one */
	    if(scopes == NULL)
	      str_len = 0;
	    else 
	      str_len = strlen(scopes) + 1;	  
	    if(str_len == 1)
	      str_len = 0;
	    if(len + 2 + str_len < LSLP_MTU) {
	      _LSLP_SETSHORT(bptr, str_len, 0);
	      if(str_len) 
		memcpy(bptr + 2, scopes, str_len);
	      
	      
	      len += (2 + str_len);
	      bptr += (2 + str_len);
	      /* stuff the attribute string if there is one */
	      if(attributes == NULL)
		str_len = 0;
	      else
		str_len = strlen(attributes) + 1;
	      if(str_len == 1)
		str_len = 0;
	      if(len + 2 + str_len < LSLP_MTU) {
		_LSLP_SETSHORT(bptr, str_len, 0);
		if(str_len)
		  memcpy(bptr + 2, attributes, str_len);
		  
		len += ( 2 + str_len);
		bptr += (2 + str_len);

		/* no attribute auths for now */
		if(len + 1 < LSLP_MTU) {
		  _LSLP_SETBYTE(bptr, 0x00, 0);
		}
		len += 1;
		/* set the length field in the header */
		_LSLP_SETLENGTH(h->msg_buf, len );
		if(TRUE == lslp_lib_send_rcv_udp(h, NULL, NULL)) {
		  if(LSLP_SRVACK == _LSLP_GETFUNCTION(h->msg_buf)) {
		    if(0x0000 == _LSLP_GETSHORT(h->msg_buf, (_LSLP_HDRLEN(h->msg_buf)))) {
		      memset(h->msg_buf, 0x00, LSLP_MTU);
		      return(TRUE); 
		    }
		  }
		}
	      } /* attribute string fits into buffer */
	    } /* scope string fits into buffer  */
	  } /* service type fits into buffer  */
	} /* url fits into buffer  */
	_LSLP_PERROR(h, "lslp_lib_srv_reg", "registration request overflowed transmission buffer"); 
      } else { _LSLP_PERROR(h, "lslp_lib_srv_reg", "library handle has wrong version"); } 
    } else  { _LSLP_PERROR(h, "lslp_lib_srv_reg", "unable to verify library handle signature" ); } 
  } else { _LSLP_PERROR(h, "lslp_lib_srv_reg", "unable to get a library handle");  } 
  memset(h->msg_buf, 0x00, LSLP_MTU);
  return(FALSE);
}

PEGASUS_EXPORT int8 *lslp_lib_get_host_name(void)
{
  int8 *buf = (int8 *)malloc(255);
  if(buf != NULL) {
    if( 0 == gethostname(buf, 255)) {
      buf = (int8 *)realloc(buf, strlen(buf) + 1);
    } else {
      free(buf);
      buf = NULL;
    }
  }
  return(buf);
}

static BOOL lslp_lib_send_rcv_udp(LSLP_HANDLE *h, 
			    void (*decode_func)(LSLP_HANDLE *, LSLP_LIB_LIST **, struct sockaddr_in *), 
			   LSLP_LIB_LIST **parm)
{
  SOCKET sock;
  struct sockaddr_in target, local;
  BOOL ccode = FALSE;
  assert(h != NULL);
  if(INVALID_SOCKET != (sock = socket(AF_INET, SOCK_DGRAM, 0))) {
    local.sin_family = AF_INET;
    local.sin_port = 0;
    local.sin_addr.s_addr = h->local_addr;
    if(SOCKET_ERROR != bind(sock, (struct sockaddr *)&local, sizeof(local))) {
      int err;
      fd_set fds;
      struct timeval tv;
      int bcast = ( (_LSLP_GETFLAGS(h->msg_buf)) & LSLP_FLAGS_MCAST) ? 1 : 0 ;
      if(bcast) {
	if( (SOCKET_ERROR == _LSLP_SET_TTL(sock, h->ttl))  ||  
	    (SOCKET_ERROR == setsockopt(sock, SOL_SOCKET, SO_BROADCAST, (const int8 *)&bcast, sizeof(bcast))) ) {
	  _LSLP_PERROR(h, "lslp_lib_converge_udp", "writing to datagram socket");
	  _LSLP_CLOSESOCKET(sock);
	  return(FALSE);
	}
      }
      target.sin_family = AF_INET;
      target.sin_port = h->target_port;
      target.sin_addr.s_addr = h->target_addr;
      if(SOCKET_ERROR == (err = sendto(sock, 
				       h->msg_buf, 
				       _LSLP_GETLENGTH(h->msg_buf), 
				       0, 
				       (struct sockaddr *)&target, sizeof(target) ))) {
	_LSLP_PERROR(h, "lslp_lib_converge_udp", "writing to datagram socket");
	_LSLP_CLOSESOCKET(sock);
	return(FALSE);
      } /* oops - error sending data */
      while(0 < err) {
	FD_ZERO(&fds);
	FD_SET(sock, &fds);
	do {
	  tv.tv_sec = h->tv.tv_sec;
	  tv.tv_usec = h->tv.tv_usec;
	  err = select(sock + 1, &fds, NULL, NULL, &tv);
	} while( (err < 0) && (errno == EINTR));
	
	if( 0 < err ) {
	  int size = sizeof(target);
	  err = recvfrom(sock, h->msg_buf, LSLP_MTU, 0, (struct sockaddr *)&target, (socklen_t *)&size);
	  if(err && err != SOCKET_ERROR) {
	    ccode = TRUE;
	    if(decode_func != NULL)
	      decode_func(h, parm, &target);
	    if(bcast == 0)
	      break;
	    prepare_pr_buf(h, inet_ntoa(target.sin_addr));
	  } /* read a response  */
	} /* socket has data  */
      } /*  while selecting */
    } /* socket bound */
    _LSLP_CLOSESOCKET(sock);
  } /*  got the socket */
  return(ccode);
}
     

static void lslp_lib_free_daadvert(LSLP_LIB_DAADVERT *dav)
{
  if(dav->url != NULL)
    free(dav->url);
  if(dav->scope != NULL)
    free(dav->scope);
  if(dav->attr != NULL)
    free(dav->attr);
  if(dav->spi != NULL)
    free(dav->spi);
  if(dav->auth != NULL)
    free(dav->auth);
  free(dav);
  return;
}

static void lslp_lib_free_rply(LSLP_LIB_SRVRPLY *rply)
{
  if(rply->url != NULL)
    free(rply->url);
  if(rply->auth != NULL)
    free(rply->auth);
  free(rply);
}

PEGASUS_EXPORT void lslp_lib_free_list(LSLP_LIB_LIST *list)
{
  LSLP_LIB_LIST *temp;
  temp = list->next;
  while( ! _LSLP_IS_HEAD(temp)) {
    _LSLP_UNLINK(temp);
    if(temp->type == TYPE_DA_LIST) 
      lslp_lib_free_daadvert((LSLP_LIB_DAADVERT *)temp);
    else if (temp->type == TYPE_RPLY_LIST)
      lslp_lib_free_rply((LSLP_LIB_SRVRPLY *)temp);
    else { free(temp) ; }
    temp = list->next;
  }
  free(list);
  return;
}

PEGASUS_EXPORT void lslp_lib_srv_reg_all(int8 *id, 
			  int8 *url,
			  int8 *attributes,
			  int8 *service_type,
			  int8 *scopes,
			  int16 lifetime,
			  int *succeeded,
			  int *failed) 
     
{
  LSLP_LIB_DAADVERT *das;
  

  
  assert(id != NULL && url != NULL && attributes != NULL && service_type != NULL && 
	 scopes != NULL && succeeded != NULL && failed != NULL);
  if(id == NULL || url == NULL || attributes == NULL || service_type == NULL ||
     scopes == NULL || succeeded == NULL || failed == NULL) 
    return;
  
  *succeeded = 0;
  *failed = 0;
  lslp_lib_set_target_addr(id, "239.255.255.253");
  lslp_lib_set_convergence(id, 3);
  if(NULL != (das = (LSLP_LIB_DAADVERT *)lslp_lib_srv_req(id, NULL, NULL, "DEFAULT"))) {
    LSLP_LIB_DAADVERT *thisDA = das->next;
    while( ! _LSLP_IS_HEAD(thisDA) ) {
      lslp_lib_set_target_addr(id, thisDA->remote);
      if(TRUE == lslp_lib_srv_reg(id, url, attributes, service_type, scopes, lifetime) )
	(*succeeded)++;
      else
	(*failed)++;
      thisDA = thisDA->next;
    }
    lslp_lib_free_list((LSLP_LIB_LIST *)das);
  }
  /* now try to register with the local host  */
  lslp_lib_set_convergence(id, 0);
  lslp_lib_set_target_addr(id, "127.0.0.1");
  if(TRUE == lslp_lib_srv_reg(id, url, attributes, service_type, scopes, lifetime))
    (*succeeded)++;
  /* its not really a failure if the localhost doesn't respond, because  */
  /* there may not be a da on the localhost  */
  return;
}
#ifdef _WIN32
PEGASUS_EXPORT int gethostbyname_r(const char *name, 
		    struct hostent *resultbuf, 
		    char *buf, 
		    size_t bufsize, 
		    struct hostent **result, 
		    int *errnop) 
{
  name = name;
  resultbuf = resultbuf;
  buf = buf;
  bufsize = bufsize;
  
  if(NULL == (*result = gethostbyname(name))) {
    *errnop = WSAGetLastError();
    return(-1);
  } 
  return(0);
}

#endif

PEGASUS_EXPORT int8 *lslp_lib_get_addr_string_from_url(const int8 *url) 
{
  struct sockaddr_in addr;
  if( lslp_lib_get_addr_from_url( url, &addr) ) {
    int8 *name = (int8 *)malloc(255);
    if(name != NULL) {
#ifdef _WIN32
      _snprintf(name, 254, "%s:%d", inet_ntoa(addr.sin_addr), ntohs(addr.sin_port) );
#else
      snprintf(name, 254, "%s:%d", inet_ntoa(addr.sin_addr), ntohs(addr.sin_port) );
#endif
      realloc(name, strlen(name) + 1);
    }
    return(name);
  }
  return(NULL);
}
PEGASUS_EXPORT BOOL lslp_lib_get_addr_from_url(const int8 *url, struct sockaddr_in *addr )
{
  int8 *bptr, *url_dup;
  BOOL ccode = FALSE;
  assert(url != NULL && addr != NULL); 
  if(url == NULL || addr == NULL)
    return(FALSE);

  // isolate the host field 
  bptr = (url_dup = strdup(url));
  if(bptr == NULL)
    return(FALSE );

  while( (*bptr != '/') && (*bptr != 0x00) )
    bptr++;

  if(*bptr == '/' && *(bptr + 1) == '/') {
    int8 *endptr, *portptr;
    endptr = bptr + 2;
    while (*endptr != 0x00 && *endptr != '/' && *endptr != ';')
      endptr++;
    *endptr = 0x00;
    portptr = (endptr - 1) ;

    while( ( portptr > ( bptr + 2 ) ) && ( *portptr != ':' ) )
      portptr--;

    if( *portptr == ':') {
      *portptr = 0x00;
      portptr++;
    } else { portptr = NULL; }
    
    // bptr points to the host name or address
    // portptr points to the port or is null

    if (portptr != NULL)
      addr->sin_port = htons( (int16)strtoul(portptr, NULL, 0) );
    else
      addr->sin_port = 0x0000;
    addr->sin_family = AF_INET;
    
    bptr += 2;
    addr->sin_addr.s_addr = inet_addr(bptr);
    if(addr->sin_addr.s_addr == INADDR_NONE) {
      struct hostent *host;
      struct hostent hostbuf;
      uint8 *temp ;
      uint32 result, err;
      size_t hostbuflen = 256;
      
      // hopefully a hostname because dotted decimal notation was invalid
      // look for the user@hostname production
      int8 *userptr;
      userptr = bptr;
      while( (*userptr != 0x00 ) && (*userptr != '@' ) )
	userptr++;
      if( *userptr == '@' )
	bptr = userptr + 1;
      
      temp = (uint8 *)malloc(hostbuflen); 
      if(temp != NULL) {
	host = NULL;
	while(temp != NULL && (result = gethostbyname_r(bptr, &hostbuf, (char *)temp, hostbuflen, 
							&host, (int *)&err)) == ERANGE){
	  hostbuflen *= 2;
	  temp = (uint8 *)realloc(temp, hostbuflen);
	}
	if(host != NULL) {
	  struct in_addr *ptr;
	  if (((ptr = (struct in_addr *)*(host->h_addr_list)) != NULL ) ) {
	    addr->sin_addr.s_addr = ptr->s_addr;
	    ccode = TRUE;
	  }
	}
	free(temp);
      } /* we allocated the temp buffer for gethostbyname */
    } else { ccode = TRUE; } /* host field is not in a valid dotted decimal form */
  } /* isolated the host field in the url  */
  return(ccode);
}



/*****************************************************************
 * test harness 
 * 
 *
 ****************************************************************/

#ifdef TEST_HARNESS

#define TEST_HARNESS_ID "test_harness"
int main(int argc, char *argv[])
{

  if(TRUE == lslp_lib_init(TEST_HARNESS_ID, "239.255.255.253", "0.0.0.0", 427, "none" )) {
    LSLP_LIB_DAADVERT *daadvert;
    daadvert = (LSLP_LIB_DAADVERT *)lslp_lib_srv_req(TEST_HARNESS_ID,
						     NULL, NULL, "DEFAULT"); 
    if(TRUE == lslp_lib_srv_reg(TEST_HARNESS_ID, 
				"service:nothing://localhost/",
				"(one = one), (two = 2)",
				"service:nothing",
				"DEFAULT, special",
				0x00ff)) {
      LSLP_LIB_SRVRPLY *rply;
      rply = (LSLP_LIB_SRVRPLY *)lslp_lib_srv_req(TEST_HARNESS_ID, 
						  "service:nothing",
						  "(one =*)",
						  "DEFAULT");
    }
    
    lslp_lib_deinit(TEST_HARNESS_ID);
  }
  return(1);
}



#endif

