/*****************************************************************************
 *  Description:   
 *
 *  Originated: December 20, 2001
 *	Original Author: Mike Day md@soft-hackle.net
 *                                mdday@us.ibm.com
 *
 *  $Header: /cvs/MSB/pegasus/src/Unsupported/slp_client/src/cmd-utils/slp_client/Attic/slp_client.c,v 1.3 2003/05/21 19:05:50 mday Exp $ 	                                                            
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




/* make pattern matching work with a length parameter, like strncmp(a,b) */
/* <<< Wed Aug  7 20:37:13 2002 mdd >>> */

/* make all direct calls static (hidden), public calls to be exposed through client->() */
/* << Mon Sep 16 14:00:36 2002 mdd >> */

#include "slp_client.h"

#ifdef _WIN32
 int _winsock_count = 0;
 WSADATA _wsa_data ;
#include <time.h>
#endif 

struct da_list *alloc_da_node(BOOL head)
{
  struct da_list *node = (struct da_list *)calloc(1, sizeof(struct da_list));
  if(node != NULL && head == TRUE){
    node->isHead = TRUE;
    node->next = node->prev = node;
  }
  return node;
}

struct da_list *da_node_exists(struct da_list *head, const void *key)
{
  
  if(head != NULL && _LSLP_IS_HEAD(head) && key != NULL) {
    struct da_list *temp = head->next;
    while( ! _LSLP_IS_HEAD(temp)){
      if(! strcmp(temp->url, (int8 *)key))
	return temp;
      temp = temp->next;
    }
  }
  return NULL;
}


void free_da_list_members(struct da_list *da)
{
  assert( ! _LSLP_IS_HEAD(da));
  if(da->url != NULL)
    free(da->url);
  if(da->scope != NULL)
    free(da->scope);
  if(da->attr != NULL)
    free(da->attr);
  if(da->spi != NULL)
    free(da->spi);
  if(da->auth != NULL)
    free(da->auth);
}

void free_da_list_node(struct da_list *da)
{
  assert( ! _LSLP_IS_HEAD(da));
  free_da_list_members(da);
  free(da);
}


/* DOES NOT free the list head ! */
void free_da_list(struct da_list *list)
{
  struct da_list *temp;
  assert(_LSLP_IS_HEAD(list));
  temp = list->next;
  while( ! _LSLP_IS_HEAD(temp)) {
    _LSLP_UNLINK(temp);
    free_da_list_node(temp);
    temp = list->next;
  }
}

struct rply_list *alloc_rply_list(BOOL head)
{
  struct rply_list *node = (struct rply_list *)calloc(1, sizeof(struct rply_list));
  if(node != NULL && head == TRUE){
    node->isHead = TRUE;
    node->next = node->prev = node;
  }
  return node;
}


struct rply_list *rpl_node_exists(struct rply_list *head, const void *key)
{
  if(head != NULL && _LSLP_IS_HEAD(head) && key != NULL){
    struct rply_list *temp = head->next;
    while( ! _LSLP_IS_HEAD(temp)){
      if( ! strcmp(temp->url, (int8 *)key))
	return temp;
      temp = temp->next;
    }
  }
  return NULL;
}

void free_rply_list_members(struct rply_list *rply)
{
  assert(! _LSLP_IS_HEAD(rply));
  if(rply->url != NULL)
    free(rply->url);
  if(rply->auth != NULL)
    free(rply->auth);
}

void free_rply_list_node(struct rply_list *rply)
{
  assert(! _LSLP_IS_HEAD(rply));
  free_rply_list_members(rply);
  free(rply);
}

/* DOES NOT free the list head ! */
void free_rply_list(struct rply_list *list)
{
  struct rply_list *temp;
  assert(_LSLP_IS_HEAD(list));
  temp = list->next;
  while( ! _LSLP_IS_HEAD(temp)){
    _LSLP_UNLINK(temp);
    free_rply_list_node(temp);
    temp = list->next;
  }
}

struct reg_list *alloc_reg_list(BOOL head)
{
  struct reg_list *node = (struct reg_list *)calloc(1, sizeof(struct reg_list));
  if( node != NULL && head == TRUE){
    node->isHead = TRUE;
    node->next = node->prev = node;
  }
  return node;
}

struct reg_list *reg_node_exists(struct reg_list *head, const void *key)
{
  if(head != NULL && _LSLP_IS_HEAD(head) && key != NULL){
    struct reg_list *temp = head->next;
    while( ! _LSLP_IS_HEAD(temp)){
      if( ! strcmp(temp->url, (int8 *)key))
	return temp;
      temp = temp->next;
    }
  }
  return NULL;
}

void free_reg_list_members(struct reg_list *reg)
{
  assert(! _LSLP_IS_HEAD(reg));
  if(reg->url != NULL)
    free(reg->url);
  if(reg->attributes != NULL)
    free(reg->attributes);
  if(reg->service_type != NULL)
    free(reg->service_type);
  if(reg->scopes != NULL)
    free(reg->scopes);
}

void free_reg_list_node(struct reg_list *reg)
{
  assert(! _LSLP_IS_HEAD(reg));
  free_reg_list_members(reg);
  free(reg);
}

/* DOES NOT free the list head ! */
void free_reg_list(struct reg_list *list)
{
  struct reg_list *temp;
  assert( _LSLP_IS_HEAD(list));
  temp = list->next;
  while(! _LSLP_IS_HEAD(temp)) {
    _LSLP_UNLINK(temp);
    free_reg_list_node(temp);
    temp = list->next;
  }
}

struct url_entry *alloc_url_entry(BOOL head)
{
  struct url_entry *node = (struct url_entry *)calloc(1, sizeof(struct url_entry));
  if(node != NULL && head == TRUE){
    node->isHead = TRUE;
    node->next = node->prev = node;
  }
  return node;
}

struct url_entry *url_node_exists(struct url_entry *head, const void *key)
{
  if(head != NULL && _LSLP_IS_HEAD(head) && key != NULL){
    struct url_entry *temp = head->next;
    while( ! _LSLP_IS_HEAD(temp)){
      if( ! strcmp(temp->url, (int8 *)key))
	return temp;
      temp = temp->next;
    }
  }
  return NULL;
}

void free_url_entry_members(struct url_entry *url)
{
  assert(! _LSLP_IS_HEAD(url));
  if(url->url != NULL)
    free(url->url);
  if(url->auth_blocks != NULL)
    free(url->auth_blocks);
}

void free_url_node(struct url_entry *node)
{
  assert(! _LSLP_IS_HEAD(node));
  free_url_entry_members(node);
  free(node);
}

/* DOES NOT free the list head ! */
void free_url_list(struct url_entry *list)
{
  struct url_entry *temp;
  assert( _LSLP_IS_HEAD(list));
  temp = list->next;
  while(! _LSLP_IS_HEAD(temp)) {
    _LSLP_UNLINK(temp);
    free_url_node(temp);
    temp = list->next;
  }
}

#if defined( _WIN32 ) 
 int gethostbyname_r(const char *name, 
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

#if defined( NUCLEUS )  //jeb

 int gethostbyname_r(const char *name, 
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
  
  if(NULL == (*result = _LSLP_GETHOSTBYNAME(name))) {
#ifdef NUCLEUS
      *errnop = -2;
#else
      *errnop = h_errno;
#endif
    return(-1);
  } 
  return(0);
}

#endif

char *slp_get_addr_string_from_url(const char *url, char *addr, int addr_len) 
{
  char name[255];
  SOCKADDR_IN  a;

  if(addr == NULL || addr_len < 1 )
    return NULL;
  
  if( get_addr_from_url( url, &a, NULL) ) {
#if defined ( _WIN32 )
    _snprintf(name, 254, "%s:%d", inet_ntoa(a.sin_addr), ntohs(a.sin_port) );
#elif defined (NUCLEUS)                                              //jeb
    sprintf(name, "%s:%d", inet_ntoa(a.sin_addr), a.sin_port );  //jeb
#else                                                                   //jeb
    snprintf(name, 254, "%s:%d", inet_ntoa(a.sin_addr), ntohs(a.sin_port) );
#endif
    
    memset(addr, 0x00, addr_len);
    strncpy(addr, name, addr_len - 1 );
    return addr;
  }
  return NULL;
}

char *slp_get_host_string_from_url(const char *url, char *host, int host_len) 
{
  char *s;
  SOCKADDR_IN  addr;

  if ( host == NULL || host_len < 1 )
    return NULL;
  
  if (TRUE == get_addr_from_url(url, &addr, &s )) {
    memset(host, 0x00, host_len);
    strncpy(host, s, host_len - 1);
    return host;
  }
  
  return NULL;
}

BOOL  get_addr_from_url(const int8 *url, SOCKADDR_IN *addr, int8 **host)

{
  int8 *bptr, *url_dup;
  BOOL ccode = FALSE;

  // isolate the host field 
  bptr = (url_dup = strdup(url));
  if(bptr == NULL)
    return( FALSE );

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

    bptr += 2;

    if(host != NULL) {
      *host = (int8 *)malloc(strlen(bptr) + strlen(portptr) + 3);
      strcpy(*host, bptr);
      strcat(*host, ":");
      strcat(*host, portptr);
    }
    if (portptr != NULL)
      addr->sin_port = htons( (int16)strtoul(portptr, NULL, 0) );
    else
      addr->sin_port = 0x0000;
    addr->sin_family = AF_INET;
    
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
	while(temp != NULL && (result = gethostbyname_r(bptr, &hostbuf, 
	    (char *)temp, 
	    hostbuflen, 
#ifdef NUCLEUS
	    &host, (int *)&err)) == -1){
#else
        &host, (int *)&err)) == ERANGE){   //
#endif
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
    } else { 
      ccode = TRUE ;
    } /* host field is not in a valid dotted decimal form */
  } /* isolated the host field in the url  */
  return(ccode);
}
 

/*** effectively reallocates *list -- FREES MEMORY ***/
int slp_get_local_interfaces(uint32 **list)
{
  SOCKETD sock;                         //jeb
  int interfaces = 0;
#if defined ( _WIN32 )
  int buf_size = 256;
#endif
  if( list == NULL )
    return 0;
  if ( *list != NULL )
    free( *list );
  
#if defined ( _WIN32 )

  if ( INVALID_SOCKET != ( sock  = WSASocket(AF_INET,
					   SOCK_RAW, 0, NULL, 0, 0) ) ) {
	int bytes_returned;
    char *output_buf = (char *)malloc(buf_size);
    if (output_buf == NULL)
      return 0;
    
    if ( 0 == WSAIoctl( sock, SIO_ADDRESS_LIST_QUERY, NULL, 0, 
			output_buf, buf_size, &bytes_returned, NULL, NULL) ) {
      socket_addr_list *addr_list;
      socket_addr *addr;
      uint32 *intp;
      struct sockaddr_in *sin;
		  addr_list = (socket_addr_list *)output_buf;
      *list = (uint32 *) malloc(sizeof(sockad_addr) * addr_list->count + 1) ;
      addr = addr_list->list;
      
      for( interfaces = 0, intp = *list, sin = (struct sockaddr_in *)addr ; 
	   interfaces < addr_list->count; 
	   interfaces++ , intp++  ) {
	*intp = sin->sin_addr.s_addr;
	addr++;
	sin = (struct sockaddr_in *)addr;
      }
      *intp = INADDR_ANY;
    }
    free(output_buf);
    _LSLP_CLOSESOCKET(sock);
  }


#elif defined ( NUCLEUS )   //jeb 
  /* only one interface do the following: */
  *list = (uint32 *)malloc(sizeof(uint32));
  
  //get name of external ethernet
  NU_IOCTL_OPTION option;
  option.s_optval = (unsigned char *)ETHER0;

  //get IP address
  if (NU_Ioctl(IOCTL_GETIFADDR, &option, sizeof(option)) == NU_SUCCESS)
  {                                                    
     //get IP address
     memcpy(*list, option.s_ret.s_ipaddr, 4); 
     interfaces = 1;
  }
  else // no interfaces
     interfaces = 0;

  *list = (uint32 *)malloc(sizeof uint32);
  *list[0] = inet_addr(" address of interface "); 
  interfaces = 1;
  
#else
  if( -1 < (sock = socket(AF_INET, SOCK_DGRAM, 0) ) ) {
    struct ifconf conf;
    uint32 *this_addr;
    
    conf.ifc_buf = (char *)malloc( 128 * sizeof(struct ifreq ) );
    conf.ifc_len = 128 * sizeof( struct ifreq ) ;
    if( -1 < ioctl(sock, SIOCGIFCONF, &conf ) ) {
      // count the interfaces 

      struct ifreq *r = conf.ifc_req;
      SOCKADDR_IN *addr ;
      addr = (SOCKADDR_IN *)&r->ifr_addr;
      while(  addr->sin_addr.s_addr != 0 ) {
	interfaces++;
	r++;
	addr = (SOCKADDR_IN *)&r->ifr_addr;
      }

      // now store the addresses

      *list  = (uint32 *)malloc( sizeof(uint32) * interfaces + 1 );
      this_addr = *list;
      r = conf.ifc_req;
      addr = (SOCKADDR_IN *)&r->ifr_addr;
      while(  addr->sin_addr.s_addr != 0 ) {
	*this_addr = addr->sin_addr.s_addr;
	r++;
	this_addr++;
	addr = (SOCKADDR_IN *)&r->ifr_addr;
      }
      *this_addr = INADDR_ANY;
    } // did the ioctl 
    free(conf.ifc_buf);
    _LSLP_CLOSESOCKET(sock);
  } // opened the socket 

#endif 
  // failsafe if the ioctl doesn't work
  if( interfaces == 0 ) {
    *list = (uint32 *)malloc(sizeof(uint32)) ; 
    *list[0] = INADDR_ANY; 
  }

  return(interfaces);
}

BOOL  slp_join_multicast(SOCKETD sock, uint32 addr)      //jeb
{
  
#if defined(NUCLEUS )    //jeb
  //don't support for now  
  return(FALSE);         //jeb

#else 
 struct ip_mreq mreq;
  // don't join on the loopback interface
  if (addr == inet_addr("127.0.0.1") )
    return(FALSE);

 
  mreq.imr_multiaddr.s_addr = inet_addr("239.255.255.253");
  mreq.imr_interface.s_addr = addr;
  
  if(SOCKET_ERROR == setsockopt(sock,IPPROTO_IP, IP_ADD_MEMBERSHIP, (const char *)&mreq, sizeof(mreq))) 
    return(FALSE);
  
  return(TRUE);    //jeb
#endif
}



int slp_join_multicast_all(SOCKETD sock)
{

  uint32 *list = NULL , *lptr = NULL;
  int num_interfaces = slp_get_local_interfaces(&list);
  lptr = list;
  while ( *lptr != INADDR_ANY ) {
    slp_join_multicast(sock, *lptr) ;
    lptr++;
  }
  free(list);
  return(num_interfaces);
}


SOCKETD slp_open_listen_sock( void )       //jeb
{
  SOCKADDR_IN local;
  SOCKETD sock  = _LSLP_SOCKET(AF_INET, SOCK_DGRAM, 0) ;   //jeb
  int err = 1;
  
#ifndef NUCLEUS    //jeb not supported
  _LSLP_SETSOCKOPT(sock, SOL_SOCKET, SO_REUSEADDR, (const char *)&err, sizeof(err));
#endif
  local.sin_family = AF_INET;
  local.sin_port = htons(427);
  local.sin_addr.s_addr  = INADDR_ANY;
  if( 0 == _LSLP_BIND(sock, (struct sockaddr *)&local, sizeof(local)) )
    slp_join_multicast_all(sock);
  return(sock);
}


void prepare_pr_buf(struct slp_client *client, const int8 *address)
{
  if(address == NULL || client == NULL )
    return;;
  if(client->_pr_buf_len > 0) 
    client->_pr_buf[client->_pr_buf_len - 1] = ',';
  do {
    client->_pr_buf[client->_pr_buf_len] = *address;
    address++;
    (client->_pr_buf_len)++;
  }while((*address != 0x00) && (client->_pr_buf_len < LSLP_MTU - 1)); 
  (client->_pr_buf_len)++;
}

/** attn need to role change to getflags line into nucleus **/
void make_srv_ack(struct slp_client *client, SOCKADDR_IN *remote, int8 response, int16 code )
{
  int8 *bptr;
  if(TRUE == ( ((_LSLP_GETFLAGS( client->_rcv_buf )) & (LSLP_FLAGS_MCAST) ) ? FALSE : TRUE   ) ) {
    SOCKETD sock;  //jeb
    
    memset(client->_msg_buf, 0x00, LSLP_MTU);
    bptr = client->_msg_buf;
    _LSLP_SETVERSION(bptr, LSLP_PROTO_VER);
    _LSLP_SETFUNCTION(bptr, response);

    _LSLP_SETFLAGS(bptr, 0);
    _LSLP_SETXID(bptr, _LSLP_GETXID(client->_rcv_buf));
    _LSLP_SETLAN(bptr, LSLP_EN_US, LSLP_EN_US_LEN);
    bptr += _LSLP_HDRLEN(bptr) ;
    _LSLP_SETSHORT(bptr, code, 0 );
    bptr += 2;
    _LSLP_SETLENGTH(client->_msg_buf, bptr - client->_msg_buf);
    if(INVALID_SOCKET != (sock = _LSLP_SOCKET(AF_INET, SOCK_DGRAM, 0))) {
      SOCKADDR_IN local;
      int err = 1;
#ifndef NUCLEUS
      _LSLP_SETSOCKOPT(sock, SOL_SOCKET, SO_REUSEADDR, (const char *)&err, sizeof(err) );
#endif      
      local.sin_family = AF_INET;
      local.sin_port = client->_target_port ; 
      local.sin_addr.s_addr = client->_local_addr;
      if(SOCKET_ERROR != _LSLP_BIND(sock, &local, sizeof(local))) {
	_LSLP_SENDTO(sock, client->_msg_buf, _LSLP_GETLENGTH(client->_msg_buf), 0, 
		     remote, sizeof(SOCKADDR_IN )) ;
      } // successfully bound this socket 
      _LSLP_CLOSESOCKET(sock);
    } // successfully opened this socket
  }
}


BOOL prepare_query( struct slp_client *client, 
		    uint16 xid,
		    const int8 *service_type,
		    const int8 *scopes, 
		    const int8 *predicate  ) 
{
  int16 len, total_len, buf_len;
  int8 *bptr, *bptrSave;
  if(xid != client->_xid) {
    /* this is a new request */
    memset(client->_pr_buf, 0x00, LSLP_MTU);
    client->_pr_buf_len = 0;
    client->_xid = xid;
  }
  memset(client->_msg_buf, 0x00, LSLP_MTU);
  bptr = client->_msg_buf;
  _LSLP_SETVERSION(bptr, LSLP_PROTO_VER);
  _LSLP_SETFUNCTION(bptr, LSLP_SRVRQST);
  /* we don't know the length yet */
  _LSLP_SETFLAGS(bptr, 0);
  _LSLP_SETXID(bptr, xid);
  _LSLP_SETLAN(bptr, LSLP_EN_US, LSLP_EN_US_LEN);
  bptr += ( total_len = _LSLP_HDRLEN(bptr) ) ;
  if(client->_pr_buf_len + total_len < LSLP_MTU) {
    /* set the pr list length */
    _LSLP_SETSHORT(bptr, (len = client->_pr_buf_len), 0);
    if(len)
      memcpy(bptr + 2, client->_pr_buf, len);
    total_len += ( 2 + len );
    bptr += (2 + len);
    if(service_type == NULL)
      len = DA_SRVTYPELEN;
    else
      len = strlen(service_type) ;
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
	len = strlen(scopes);
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
	  len = strlen(predicate) ;
	if( total_len + 2 + len < LSLP_MTU) {
	  _LSLP_SETSHORT(bptr, len, 0);
	  if(predicate != NULL)
	    memcpy(bptr + 2, predicate, len);
	      
	  total_len += (2 + len);
	  bptrSave = (bptr += (2 + len));
	  /* stuff the spi */
	  buf_len = LSLP_MTU - total_len;
	  lslpStuffSPIList(&bptr, &buf_len, client->_spi);
		
	  /* read back the length of the spi */
	  total_len += (2 + _LSLP_GETSHORT(bptrSave, 0));
	  assert(total_len == bptr - client->_msg_buf);
	  /*  always add an attr extension to an srvrq if there is room */
	  if(total_len + 9 <= LSLP_MTU ) {
	    _LSLP_SETNEXTEXT(client->_msg_buf, total_len);
	    _LSLP_SETSHORT(bptr, 0x0002, 0);
	    _LSLP_SET3BYTES(bptr, 0x00000000, 2);
	    _LSLP_SETSHORT(bptr, 0x0000, 5);
	    _LSLP_SETSHORT(bptr, 0x0000, 7);
	    _LSLP_SETBYTE(bptr, 0x00, 9);
	    total_len += 10;
	  }
	  /* now go back and set the length for the entire message */
	  _LSLP_SETLENGTH(client->_msg_buf, total_len );
	  return(TRUE);

	} /*  room for predicate  */
      } /* room for the scope  */
    } /* room for the service type  */
  } /* room for the pr list  */
  return(FALSE);
}


lslpMsg *get_response( struct slp_client *client, lslpMsg *head)
{
  assert(head != NULL && _LSLP_IS_HEAD(head));
  if( _LSLP_IS_EMPTY(&(client->replies)))
    return NULL;
  
  _LSLP_LINK_HEAD(head, &(client->replies));
  return(head);
}


int find_das(struct slp_client *client, 
	     const int8 *predicate, 
	     const int8 *scopes)
{
  converge_srv_req(client, NULL, predicate, scopes);
  time(&(client->_last_da_cycle));
  if( ! _LSLP_IS_EMPTY(&(client->das)))
    client->_use_das = TRUE;
  else
    client->_use_das = FALSE;
  return((int) client->_use_das );
}


/* smart interface to slp discovery. uses das if they are present,  */
/* convergence otherwise. */
/* periodically forces an active da discovery cycle  */

void discovery_cycle ( struct slp_client *client, 
		       const int8 *type, 
		       const int8 *predicate, 
		       const int8 *scopes) 
{
  // see if we have built a cache of directory agents 
  if( _LSLP_IS_EMPTY(&(client->das)) ) {
    // we don't know of any directory agents - see if we need to do active da discovery
    if( ((time(NULL)) - client->_last_da_cycle ) > (60 * 5) )
      find_das(client, NULL, scopes) ;
  }

  // if there are das, unicast a srvreq to each da

  if( ! _LSLP_IS_EMPTY(&(client->das))) {
    struct da_list *da = client->das.next;
    SOCKADDR_IN addr;
    while( ! _LSLP_IS_HEAD(da)  ) {
      addr.sin_port = htons(427);
      addr.sin_family = AF_INET;
      addr.sin_addr.s_addr = inet_addr(da->remote);
      unicast_srv_req(client, type, predicate, scopes, &addr);
      da = da->next;
    }
  } else {
    // do a convergence request because we don't have any das to use 
    converge_srv_req(client, type, predicate, scopes );
  }
  return; 
}

/* this request MUST be retried <_convergence> times on EACH interface  */
/* regardless of how many responses we have received  */
/* it can be VERY time consuming but is the most thorough  */
/* discovery method  */

void converge_srv_req(struct slp_client *client,
		      const int8 *type, 
		      const int8 *predicate, 
		      const int8 *scopes)
{
  uint32 old_local_addr;
  uint32 *p_addr ;
  uint16 convergence; 
  uint32 loopback ;

  
  //  old_target_addr = client->_target_addr;
  old_local_addr = client->_local_addr;
  //  client->_target_addr = inet_addr( "239.255.255.253" ) ;
  
  p_addr = client->_local_addr_list;
  loopback = inet_addr("127.0.0.1");

  do {
    if( *p_addr == loopback ) {
      p_addr++;
      continue; 
    }
    client->_local_addr = *p_addr;
    convergence = client->_convergence;
    if(convergence == 0)
      convergence = 1;
    
    if(prepare_query( client, client->_xid + 1, type, scopes, predicate)) {
	_LSLP_SETFLAGS(client->_msg_buf, LSLP_FLAGS_MCAST) ;
	send_rcv_udp( client );
      }

    while(--convergence > 0) {
      if(prepare_query( client, client->_xid, type, scopes, predicate)) {
	_LSLP_SETFLAGS(client->_msg_buf, LSLP_FLAGS_MCAST) ;
	send_rcv_udp( client );
      }
    }
    p_addr++;
  }   while( *p_addr != INADDR_ANY ) ;


  // always try a local request 
  local_srv_req(client, type, predicate, scopes); 
 
    //  client->_target_addr = old_target_addr;
  client->_local_addr = old_local_addr;
  return ;
}

// this request will be retried MAX <_retries> times 
// but will always end when the first response is received
// This request is best when using a directory agent
void unicast_srv_req( struct slp_client *client, 
		      const int8 *type, 
		      const int8 *predicate, 
		      const int8 *scopes, 
		      SOCKADDR_IN *addr )
{

  uint32 target_addr_save, local_addr_save;
  uint16 target_port_save;
  struct timeval tv_save;
  int retries ;

  target_addr_save = client->_target_addr;
  local_addr_save = client->_local_addr;
  target_port_save = client->_target_port;

  tv_save.tv_sec = client->_tv.tv_sec;
  client->_tv.tv_sec = 1;
  
  // let the host decide which interface to use 
  client->_local_addr = INADDR_ANY; 
  client->_target_addr = addr->sin_addr.s_addr;
  client->_target_port = addr->sin_port;
  
  retries = client->_retries;
  
  srv_req(client, type, predicate, scopes, FALSE) ;

  while( retries && _LSLP_IS_EMPTY(&(client->replies))) {
    printf("retry\n");
    
    srv_req(client, type, predicate, scopes, FALSE);
    retries--;
  }
  client->_target_addr = target_addr_save;
  client->_local_addr = local_addr_save;
  client->_target_port = target_port_save;
  client->_tv.tv_sec = tv_save.tv_sec;
  return;
}

// this request is targeted to the loopback interface, 
// and has a tiny wait timer. It should be resolved quickly. 
// It will never be retried.
void local_srv_req( struct slp_client *client,
		    const int8 *type, 
		    const int8 *predicate, 
		    const int8 *scopes )


{

  uint32 target_addr_save, local_addr_save;
  uint16 target_port_save;
  
  struct timeval tv_save;
  
  target_addr_save = client->_target_addr;
  target_port_save = client->_target_port;
  local_addr_save = client->_local_addr;
  
  tv_save.tv_sec = client->_tv.tv_sec;
  tv_save.tv_usec = client->_tv.tv_usec;
  client->_tv.tv_sec = 0;
#ifdef NUCLEUS     //jeb
  client->_tv.tv_usec = 2*TICKS_PER_SECOND;  //socket call with timer UINT32
#else
  client->_tv.tv_usec = 10000;
#endif  
  // let the host decide which interface to use 
  client->_local_addr = INADDR_ANY; 
  client->_target_addr = inet_addr("127.0.0.1");
  client->_target_port = htons(427);

  srv_req(client, type, predicate, scopes, FALSE) ;

  client->_target_addr = target_addr_save;
  client->_target_port = target_port_save;
  client->_local_addr = local_addr_save;
  client->_tv.tv_sec = tv_save.tv_sec;
  
  client->_tv.tv_sec = tv_save.tv_sec;
  client->_tv.tv_usec = tv_save.tv_usec;

  return;
}


// workhorse request function
void srv_req( struct slp_client *client, 
	      const int8 *type, 
	      const int8 *predicate, 
	      const int8 *scopes, 
	      BOOL retry )
{
  if ((TRUE == prepare_query(client, (retry == TRUE) ? client->_xid : client->_xid + 1, 
			     type, 
			     scopes, 
			     predicate ))) {
    send_rcv_udp( client ) ;
  } /* prepared query  */
  return ;
}

void decode_msg( struct slp_client *client, 
		 SOCKADDR_IN *remote )
{
  int8 function, response;
  
  if( client->_xid == _LSLP_GETXID( client->_rcv_buf ))
    prepare_pr_buf( client, inet_ntoa(remote->sin_addr) );

  function = _LSLP_GETFUNCTION( client->_rcv_buf );

  // <<< Fri Dec 21 15:47:06 2001 mdd >>>
  // increment the correct function counters 

  switch(function) {
  case LSLP_DAADVERT:
    decode_daadvert( client, remote );
    return;		
  case LSLP_SRVRQST:
    decode_srvreq( client, remote );
    return;
  case LSLP_SRVRPLY:
    decode_srvrply( client, remote );
    return;
  case LSLP_SRVACK:
    return;
  case LSLP_ATTRREQ:
    response = LSLP_ATTRRPLY;
    break;
  case LSLP_SRVTYPERQST:
    response = LSLP_SRVTYPERPLY;
    break;
  case LSLP_SRVREG:
    decode_srvreg(client, remote);
    return;
  case LSLP_SRVDEREG:
  default:
    response = LSLP_SRVACK;
    break;
  }
  make_srv_ack(client, remote, response, LSLP_MSG_NOT_SUPPORTED);
  return;
}



/* this is a hack. but it will be consistent with the changes I envision */ 
/* for auth blocks and authenticated url entries */
void decode_srvreg(struct slp_client *client, 
		   SOCKADDR_IN *remote)
{
  int8 *bptr, *url_string, *attr_string, *type_string, *scopes;
  uint16 lifetime ;
  int32 total_len, purported_len;
  BOOL mcast;
  int16 str_len;
  
  mcast = ( ((_LSLP_GETFLAGS( client->_rcv_buf )) & (LSLP_FLAGS_MCAST) ) ? TRUE : FALSE   ) ;
  bptr = client->_rcv_buf;
  purported_len = _LSLP_GETLENGTH(bptr);
  
  bptr += (total_len = _LSLP_HDRLEN(bptr));
  if(purported_len < LSLP_MTU && (total_len < purported_len)) {
    lslpURL *decoded_url;
    int16 diff, err;
    diff = purported_len - total_len;
    
    /* decode the url entry */
    if(NULL != (decoded_url = lslpUnstuffURL(&bptr, &diff, &err))) {
      url_string = decoded_url->url;
      lifetime = decoded_url->lifetime;
      
      /* adjust pointers and size variables */
      /* bptr already adjusted by call to lslpUnstuffURL */
      total_len += ((purported_len - total_len) - diff);
      
      /* decode the service type string */
      if((total_len + 2 + (str_len = _LSLP_GETSHORT(bptr, 0))) < purported_len) {
	if(NULL != (type_string = (int8 *)malloc(str_len + 1))) {
	  memcpy(type_string, bptr + 2, str_len);
	  *(type_string + str_len) = 0x00;

	  /* adjust pointers and size variables */
	  bptr += (2 + str_len);
	  total_len += (2 + str_len);
	  
	  /* decode the scope list */
	  if((total_len + 2 + (str_len = _LSLP_GETSHORT(bptr, 0))) < purported_len) {
	    if(NULL != (scopes = (int8 *)malloc(str_len + 1))) {
	      memcpy(scopes, bptr + 2, str_len);
	      *(scopes + str_len) = 0x00;
	      
	      /* adjust pointers and size variables */
	      bptr += (2 + str_len);
	      total_len += (2 + str_len);
	      
	      /* decode the attr list */
	      if((total_len + 2 + (str_len = _LSLP_GETSHORT(bptr, 0))) < purported_len) {
		if(NULL != (attr_string = (int8 *)malloc(str_len + 1))) {
		  memcpy(attr_string, bptr + 2, str_len);
		  *(attr_string + str_len) = 0x00;
		  /* adjust pointers and size variables */
		  bptr += (2 + str_len);
		  total_len += (2 + str_len);
		  
		  /* decode the auth block list */
		  /* lslpUnstuffAuthList( ) */
		  __srv_reg_local(client, url_string, attr_string, type_string, scopes, lifetime);
		  make_srv_ack(client, remote, LSLP_SRVACK, 0);
		  free(attr_string);
		  free(scopes);
		  free(type_string);
		  lslpFreeURL(decoded_url);
		  return;
		} /* malloced attr_string */
	      } /* attr string sanity check */
	      free(scopes);
	    } /* malloced scopes string */
	  } /* scope list sanity check */
	  free(type_string);
	} /* malloced srv type string */
      } /* srv type sanity check */
      lslpFreeURL(decoded_url);
      make_srv_ack(client, remote, LSLP_SRVACK, LSLP_INTERNAL_ERROR);
      return;
    } /* decoded the url entry */
  } /* initial length sanity check OK */
  make_srv_ack(client, remote, LSLP_SRVACK, LSLP_PARSE_ERROR);
  return;
}


void decode_srvrply( struct slp_client *client,
		     SOCKADDR_IN *remote )
{
  int8 *bptr, *xtptr, *xtn_limit;
  lslpMsg *reply;
  
  int16 err, count, buf_len;
  int32 total_len, purported_len;

  bptr = client->_rcv_buf;
  purported_len = _LSLP_GETLENGTH(bptr);

  /* marshall the header data */
  reply = alloc_slp_msg(FALSE);
  if(reply == NULL) abort();
  reply->hdr.ver = _LSLP_GETVERSION(bptr);
  reply->type = reply->hdr.msgid = _LSLP_GETFUNCTION(bptr);
  reply->hdr.len = purported_len;
  reply->hdr.flags = _LSLP_GETFLAGS(bptr);
  reply->hdr.nextExt = _LSLP_GETNEXTEXT(bptr);
  reply->hdr.xid = _LSLP_GETXID(bptr);
  
  if((reply->hdr.nextExt != 0x00000000) && (reply->hdr.nextExt < reply->hdr.len )) {
    xtptr = client->_rcv_buf + reply->hdr.nextExt;
    xtn_limit = xtptr + reply->hdr.len;
  }
  else { xtptr = NULL; } 
  
  reply->hdr.langLen = _LSLP_GETLANLEN(bptr);
  memcpy(reply->hdr.lang, bptr + LSLP_LAN_LEN + 2, (_LSLP_MIN(reply->hdr.langLen, 19)));
  bptr += (total_len = _LSLP_HDRLEN(bptr));
  if(total_len < purported_len) {
    int32 next_ext = 0;
    
    /* process the srvrply */
    reply->hdr.errCode = reply->msg.srvRply.errCode = _LSLP_GETSHORT(bptr, 0);
    reply->msg.srvRply.urlCount = (count = _LSLP_GETSHORT(bptr, 2));
    bptr += 4;
    total_len += 4;
    if( count ){
      lslpURL *url;
      if( NULL == (reply->msg.srvRply.urlList = lslpAllocURLList()))
	abort();
      if(NULL != xtptr) {
	if( NULL == (reply->msg.srvRply.attr_list = lslpAllocAtomList())) 
	  abort();
      }
            
      buf_len = LSLP_MTU - total_len;
      /* get the urls */
      while(count-- && buf_len > 0 ) {
	url = lslpUnstuffURL(&bptr, &buf_len, &err);
	if( NULL != url ){
	  reply->msg.srvRply.urlLen = url->len;
	  _LSLP_INSERT(url, reply->msg.srvRply.urlList);
	}
      }

      /* get the attributes if they are present */

/*        0                   1                   2                   3 */
/*        0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 */
/*       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ */
/*       |      Extension ID = 0x0002    |     Next Extension Offset     | */
/*       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ */
/*       | Offset, contd.|      Service URL Length       |  Service URL  / */
/*       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ */
/*       |     Attribute List Length     |         Attribute List        / */
/*       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ */
/*       |# of AttrAuths |(if present) Attribute Authentication Blocks.../ */
/*       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ */

      while((NULL != xtptr) && (xtptr + 9 < xtn_limit )) {
	next_ext = _LSLP_GET3BYTES(xtptr, 2);
	if(0x0002 == _LSLP_GETSHORT(xtptr, 0)) {
	  
	  //BOOL lslp_pattern_match(const int8 *string, const int8 *pattern, BOOL case_sensitive)
	  /* find the correct url to associate this attribute extension. */
	  lslpURL *rply_url = reply->msg.srvRply.urlList;
	  if(rply_url != 0 && ! _LSLP_IS_HEAD(rply_url->next)){
	    int8 *url_buf;
	    int16 url_len = _LSLP_GETSHORT(xtptr, 5);
	    rply_url = rply_url->next;
	    url_buf = (int8 *)calloc(1, url_len + 1);
	    if(url_buf == 0 ) abort();
	    memcpy(url_buf, xtptr + 7, url_len);
	    while(! _LSLP_IS_HEAD(rply_url)) {
	      if(TRUE == lslp_pattern_match(url_buf, rply_url->url, FALSE)) {
		/* this is the correct url to associate with the next attribute */
		int16 attr_len;
		int32 attr_offset = 7 + _LSLP_GETSHORT(xtptr, 5);
		attr_len = _LSLP_GETSHORT(xtptr, attr_offset);
		attr_offset += 2;
		if( (xtptr + attr_offset + attr_len) < xtn_limit) {
		  lslpAtomList *temp = lslpAllocAtom();
		  if(temp != NULL){
		    temp->str = (int8 *)malloc(attr_len + 1);
		    if(temp->str != NULL) {
		      memcpy(temp->str, xtptr + attr_offset, attr_len);
		      temp->str[attr_len] = 0x00;
		      /* allocate an attr list head */
		      if(rply_url->attrs == NULL)
			rply_url->attrs = lslpAllocAtomList();
		      if(rply_url->attrs != NULL){
			_LSLP_INSERT(temp, rply_url->attrs);
		      }
		    } else { lslpFreeAtom(temp); }
		  } /* alloced atom */
		} /* sanity check */
	      } /* if we found the right url */
	      else {
	      }
	      rply_url = rply_url->next;
	    } /* while traversing urls in the reply */
	    free(url_buf);
	  } /* if the reply contains urls */
	} /* correct extension */
	if( next_ext == 0 ) 
	  break;
	xtptr = client->_rcv_buf + next_ext;
      } /* traversing extensions */
    }
    /* link the response to the client */
    _LSLP_INSERT(reply, &(client->replies));
  }
  return;
}

void decode_daadvert(struct slp_client *client, SOCKADDR_IN *remote)
{
  int8 *bptr;
  int16 str_len;
  int32 total_len, purported_len;

  bptr = client->_rcv_buf;
  purported_len = _LSLP_GETLENGTH(bptr);
  bptr += (total_len = _LSLP_HDRLEN(bptr));
  if(total_len < purported_len) {
    struct da_list *adv = alloc_da_node(FALSE);
    if(adv == NULL) abort();
    adv->function = LSLP_DAADVERT;
    adv->err = _LSLP_GETSHORT(bptr, 0);
    adv->stateless_boot = _LSLP_GETLONG(bptr, 2);
    total_len += (8 + (str_len = _LSLP_GETSHORT(bptr, 6)));
    if(total_len < purported_len) {
      /* decode and capture the url  - note: this is a string, not a url-entry structure */
      if(NULL == (adv->url = (int8 *)malloc(str_len + 1)))
	abort();
      memcpy(adv->url, bptr + 8, str_len);
      *((adv->url) + str_len) = 0x00;
      /* advance the pointer past the url string */
      bptr += (str_len + 8);
      total_len += (2 + (str_len = _LSLP_GETSHORT(bptr, 0)));
      if(total_len < purported_len) {
	if(str_len > 0) {
	  if(NULL == (adv->scope = (int8 *)malloc(str_len + 1)))
	    abort();
	  memcpy(adv->scope, bptr + 2, str_len);
	  *((adv->scope) + str_len) = 0x00;
	}
	/* advance the pointer past the scope string  */
	bptr += (str_len + 2);
	total_len += (2 + (str_len = _LSLP_GETSHORT(bptr, 0)));
	if(total_len < purported_len) {
	  if(str_len > 0) {
	    if(NULL == (adv->attr = (int8 *)malloc(str_len + 1)))
	      abort();
	    memcpy(adv->attr, bptr + 2, str_len);
	    *((adv->attr) + str_len) = 0x00;
	  }
	  /* advance the pointer past the attr string */
	  bptr += (str_len + 2);
	  total_len += (2 + (str_len = _LSLP_GETSHORT(bptr, 0)));
	  if(total_len < purported_len) {
	    struct da_list * exists;
	    
	    if(str_len > 0 ) {
	      if(NULL == (adv->spi = (int8 *)malloc(str_len + 1)))
		abort();
	      memcpy(adv->spi, bptr + 2, str_len);
	      *((adv->spi) + str_len) = 0x00;
	    } /*  if there is an spi  */
		
	    /* advance the pointer past the spi string  */
	    bptr += (str_len + 2);
	    adv->auth_blocks = _LSLP_GETBYTE(bptr, 0);
	    
	    // if we already know about this da, remove the existing
	    // entry from our cache and insert this new entry
	    // maybe the stateless boot field changed or the da
	    // supports new scopes, etc.
	    exists  = da_node_exists(&(client->das),  adv->url);
	    if(NULL != exists){
	      _LSLP_UNLINK(exists);
	      free_da_list_node(exists);
	    }
	    /* need code here to handle authenticated urls */
	    strcpy(&(adv->remote[0]), inet_ntoa(remote->sin_addr)) ;
	    _LSLP_INSERT(adv, &(client->das)) 
	    return;
	  } /*  spi length field is consistent with hdr */
	} /* attr length field is consistent with hdr */
      } /*  scope length field is consistent with hdr */
    } 
    free_da_list_node(adv);
  }
  return;
}


void decode_srvreq(struct slp_client *client, SOCKADDR_IN *remote )
{
  
  int8 *bptr, *bptrSave;
  int32 total_len, purported_len;
  BOOL mcast;
  struct lslp_srv_rply_out *rp_out = NULL;
  struct lslp_srv_req *rq = NULL;
  int16 str_len, buf_len, err = LSLP_PARSE_ERROR ;
  
  mcast = ( ((_LSLP_GETFLAGS( client->_rcv_buf )) & (LSLP_FLAGS_MCAST) ) ? TRUE : FALSE   ) ;
  bptr = client->_rcv_buf;
  purported_len = _LSLP_GETLENGTH(bptr);
  
  bptr += (total_len = _LSLP_HDRLEN(bptr));
  if(purported_len < LSLP_MTU && (total_len < purported_len)) {
    if( ! _LSLP_IS_EMPTY((lslpSrvRegList *)&(client->regs))) {
      // advance past the slp v2 header
      // get the previous responder list 
      str_len = _LSLP_GETSHORT(bptr, 0);
      if ((str_len + total_len + 2) < purported_len ) {
	if( FALSE == slp_previous_responder( client, (str_len ? bptr + 2 : NULL ) )) {
	  rq = (struct lslp_srv_req *)calloc(1, sizeof(struct lslp_srv_req));
	  if( NULL != rq) {
	    bptr += 2 + str_len;
	    total_len += 2 + str_len;
	    // extract the service type string 
	    rq->srvcTypeLen = (str_len = _LSLP_GETSHORT(bptr, 0));
	    if(str_len && (str_len + total_len + 2 < purported_len )) {
	      rq->srvcType = (int8 *)malloc(str_len + 1);
	      if(rq->srvcType == NULL) {
		free(rq);
		return ;
		}
	      memcpy(rq->srvcType, bptr + 2, str_len);
	      *(rq->srvcType + str_len) = 0x00;
	      bptr += 2 + str_len;
	      total_len += 2 + str_len;
	      
	      bptrSave = bptr;
	      buf_len = LSLP_MTU - total_len;
	      rq->scopeList = lslpUnstuffScopeList(&bptr, &buf_len, &err);
	      total_len += _LSLP_GETSHORT(bptrSave, 0);
	      if (TRUE == lslp_scope_intersection(client->_scopes, rq->scopeList) ) {
		/* continue marshalling data */
		
		if(total_len < LSLP_MTU - 2){
		  /* get the predicate */
		  rq->predicateLen = (str_len = _LSLP_GETSHORT(bptr, 0));
		  if( str_len && (str_len + total_len + 2) < LSLP_MTU ){
		    rq->predicate = (int8 *)malloc(str_len + 1);
		    if(rq->predicate != NULL){
		      memcpy(rq->predicate, bptr + 2, str_len);
		      *(rq->predicate + str_len) = 0x00;
		    }
		  } /* predicate */
		  
		  bptr += str_len + 2;
		  total_len += str_len + 2;
		  /* get the spi list */
		  buf_len = LSLP_MTU - total_len ;
		  rq->spiList = lslpUnstuffSPIList(&bptr, &buf_len, &err);

		  /* check for an extension */
		  rq->next_ext = lslp_get_next_ext(client->_rcv_buf);
		  if(rq->next_ext) {
		    /* derive the offset limit for the next extension and do a sanity check */
		    rq->ext_limit = (int8 *)(purported_len + client->_rcv_buf);
		    if(rq->ext_limit < rq->next_ext) {
		      rq->next_ext = NULL;
		    }
		  }
		  /* initialize the scratch buffer before processing the srvrq */
		  /* that way we can know if there are extensions if the first 5 bytes changed */
		  _LSLP_SETSHORT(client->_scratch, 0x0000, 0);
		  _LSLP_SET3BYTES(client->_scratch, 0x00000000, 2);
		  rp_out = _lslpProcessSrvReq(client, rq, 0);
		} /* sanity check on remaining buffer */
	      } /* my scopes intersect */
	    } /* service type sanity check */
	  } /* allocated req struct */
	} /* not a previous responder */
      } /* pr list sanity check */
    } /* we have regs */

    if(mcast == FALSE || (rp_out != NULL && rp_out->urlCount > 0 )) {
      SOCKETD sock; //jeb 
      int8 *extptr, *next_extptr, *next_extptr_save;
      int32 ext_offset;
      
      // we need to respond to this message
      _LSLP_SETVERSION(client->_msg_buf, LSLP_PROTO_VER);
      _LSLP_SETFUNCTION(client->_msg_buf, LSLP_SRVRPLY);
      // skip the length for now
      _LSLP_SETFLAGS(client->_msg_buf, 0);
      
      _LSLP_SETNEXTEXT(client->_msg_buf, 0);
      _LSLP_SETXID( client->_msg_buf, ( _LSLP_GETXID(client->_rcv_buf)));
      _LSLP_SETLAN(client->_msg_buf, LSLP_EN_US, LSLP_EN_US_LEN);
      total_len = _LSLP_HDRLEN(client->_msg_buf);
      bptr = client->_msg_buf + total_len;
      if(rp_out != NULL) {
	if( rp_out->urlLen < (LSLP_MTU - total_len)) {
	  memcpy(bptr, rp_out->urlList, rp_out->urlLen);
	  bptr += rp_out->urlLen;
	  total_len += rp_out->urlLen;
	}
	else {
	  _LSLP_SETSHORT( bptr, 0, 0 );
	  _LSLP_SETFLAGS(client->_msg_buf, LSLP_FLAGS_OVERFLOW);
	  total_len += 2;
	  bptr += 2;
	  /* clear the extension ptr, we don't have room for it. */
	  rq->next_ext = NULL;
	}
      } else {
	/* set the error code */
	_LSLP_SETSHORT(bptr, 0, 0);
	/* set the url count */
	_LSLP_SETSHORT(bptr, 0, 2);
	bptr += 4;
	total_len += 4;
      }
      extptr = client->_scratch;
      next_extptr = client->_msg_buf + LSLP_NEXT_EX;
      ext_offset = bptr - client->_msg_buf;
      
      if ( 0x0002 == (_LSLP_GETSHORT(extptr, 0))) {
	/* set the next extension field in the header */
	_LSLP_SET3BYTES(next_extptr, ext_offset, 0);
	while( 0x0002 == (_LSLP_GETSHORT(extptr, 0))) {
	  /* check see if it fits */
	  int16 ext_len = 5;
	  /* accumulate url len */
	  ext_len += (2 + _LSLP_GETSHORT(extptr, ext_len));
	  /* accumulate attr len */
	  ext_len += (2 + _LSLP_GETSHORT(extptr, ext_len));
	  /* add the auths byte */
	  ext_len++;
	  if((ext_len + total_len) < LSLP_MTU) {
	    /* set the next extension pointer to be the third byte of the first extension */
	    next_extptr = client->_msg_buf + ext_offset + 2;

	    /* copy the extension */
	    memcpy(client->_msg_buf + ext_offset, extptr, ext_len);
	    bptr += ext_len;
	    total_len += ext_len;

	    /* set the offset of the next extension */
	    _LSLP_SET3BYTES((next_extptr_save = next_extptr), ext_offset + ext_len, 0);
	    extptr += ext_len;

	    /* adjust the pointers */
	    ext_offset += ext_len;
	    next_extptr = client->_msg_buf + ext_offset + 2;
	    
	    /* minimum length of attr extension is 10 bytes - see if we should continue */
	    if(total_len + 10 >= LSLP_MTU) {
	      /* no room, set next ext field in this extension to zero */
	      _LSLP_SET3BYTES(next_extptr_save, 0x00000000, 0);
	      /* if there is another extension, set the truncated flag */
	      if ( 0x0002 == (_LSLP_GETSHORT(extptr, 0))) {
		_LSLP_SETFLAGS(client->_msg_buf, LSLP_FLAGS_OVERFLOW);
	      }
	      break;
	    } /* no more room */
	  }  else  {
	    _LSLP_SET3BYTES(next_extptr_save, 0x00000000, 0); 
	  } 
	} /* while there are extensions */
	/* terminate the last extension */
	_LSLP_SET3BYTES(next_extptr_save, 0x00000000, 0); 
      } /* if there is at least one extension */
	
      // ok, now we can set the length
      _LSLP_SETLENGTH(client->_msg_buf, total_len );
      
      _LSLP_SETSHORT(client->_scratch, 0x0000, 0);
      _LSLP_SET3BYTES(client->_scratch, 0x00000000, 2);
      // client->_msg_buf is stuffed with the service reply. now we need 
      // to allocate a socket and send it back to the requesting node 

      if(INVALID_SOCKET != (sock = _LSLP_SOCKET(AF_INET, SOCK_DGRAM, 0))) {
	SOCKADDR_IN local;
	int err = 1;
#ifndef NUCLEUS
	_LSLP_SETSOCKOPT(sock, SOL_SOCKET, SO_REUSEADDR, (const char *)&err, sizeof(err) );
#endif	
	local.sin_family = AF_INET;
	local.sin_port = client->_target_port ; 
	local.sin_addr.s_addr = client->_local_addr;
	if(SOCKET_ERROR != _LSLP_BIND(sock, &local, sizeof(local))) {
	  if(mcast == TRUE ) {
	    _LSLP_SLEEP(rand() % 30 );
	  }
	  _LSLP_SENDTO(sock, client->_msg_buf, total_len , 0, 
		       remote, sizeof(SOCKADDR_IN )) ;
	} // successfully bound this socket 
	_LSLP_CLOSESOCKET(sock);
      } // successfully opened this socket
    } // we must respond to this request 

    /* free resources */
    if(rq != NULL){
      if(rq->prList != NULL)
	free(rq->prList);
      if(rq->srvcType != NULL)
	free(rq->srvcType);
      if(rq->scopeList != NULL)
	lslpFreeScopeList(rq->scopeList);
      if(rq->predicate != NULL)
	free(rq->predicate);
      if(rq->spiList != NULL)
	lslpFreeSPIList(rq->spiList);
      free(rq);
    }
    if(rp_out != NULL){
      if(rp_out->urlList != NULL)
	free(rp_out->urlList);
      free(rp_out);
    }
  } /* header sanity check */
}

BOOL  srv_reg(struct slp_client *client,
	      const int8 *url,
	      const int8 *attributes,
	      const int8 *service_type,
	      const int8 *scopes,
	      int16 lifetime) 
{
  int32 len;
  int16 str_len, buf_len;
  int8 *bptr;
  lslpURL *url_entry = NULL;
  
  /* this is always a new request */
  memset( client->_pr_buf, 0x00, LSLP_MTU);
  client->_pr_buf_len = 0;
  client->_xid++ ;

  memset(client->_msg_buf, 0x00, LSLP_MTU);
  bptr = client->_msg_buf;
  _LSLP_SETVERSION(bptr, LSLP_PROTO_VER);
  _LSLP_SETFUNCTION(bptr, LSLP_SRVREG);
  /* we don't know the length yet */
  _LSLP_SETXID(bptr, client->_xid);
  _LSLP_SETLAN(bptr, LSLP_EN_US, LSLP_EN_US_LEN);
  bptr += (len =  _LSLP_HDRLEN(bptr) ) ;
	
  /* construct a url-entry  */

  url_entry = (lslpURL *)calloc(1, sizeof(lslpURL));
  if(url_entry == NULL)
    return FALSE;
  url_entry->lifetime = lifetime + time(NULL);
  url_entry->len = strlen(url);
  url_entry->url = strdup(url);
  url_entry->auths = 0;
  buf_len = LSLP_MTU - len;
  
  if( TRUE == lslpStuffURL(&bptr, &buf_len, url_entry )) {
    len = LSLP_MTU - buf_len ;
    /* stuff the service type string */

    /* stuff the service type  */
    str_len = strlen(service_type) ;
    if(len + 2 + str_len < LSLP_MTU) {
      int retries;
      _LSLP_SETSHORT(bptr, str_len, 0);
      memcpy(bptr + 2, service_type, str_len);
      bptr += (2 + str_len);
      len += (2 + str_len);
      /* stuff the scope list if there is one */
      if(scopes == NULL)
	str_len = 0;
      else 
	str_len = strlen(scopes);	  
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
	  str_len = strlen(attributes);
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
	  _LSLP_SETLENGTH( client->_msg_buf, len );
	  
	  retries = client->_retries;
	  while( --retries ) {
	    if(TRUE == send_rcv_udp(client)) {
	      if(LSLP_SRVACK == _LSLP_GETFUNCTION( client->_rcv_buf )) {
		if(0x0000 == _LSLP_GETSHORT( client->_rcv_buf, (_LSLP_HDRLEN( client->_rcv_buf )))) {
		  memset(client->_msg_buf, 0x00, LSLP_MTU);
		  if(url_entry != NULL)
		    lslpFreeURL(url_entry);
		  return(TRUE); 
		}
	      }
	    } // received a response 
	  } // retrying the unicast 
	} /* attribute string fits into buffer */
      } /* scope string fits into buffer  */
    } /* service type fits into buffer  */
  } /* url fits into buffer  */
  memset( client->_msg_buf, 0x00, LSLP_MTU);
  if(url_entry != NULL)
    lslpFreeURL(url_entry);
  return(FALSE);
}



BOOL send_rcv_udp( struct slp_client *client )
{
  SOCKETD sock;               //jeb
  SOCKADDR_IN target, local;
  BOOL ccode = FALSE;
  if(INVALID_SOCKET != (sock = _LSLP_SOCKET(AF_INET, SOCK_DGRAM, 0))) {
    int err = 1;
#ifndef NUCLEUS    //jeb
    _LSLP_SETSOCKOPT(sock, SOL_SOCKET, SO_REUSEADDR, (const char *)&err, sizeof(err) );
    local.sin_port = 0;
#else
      local.sin_port = 427;  for loopback to work the first time client inits
#endif
    local.sin_family = AF_INET;

//jeb    
    local.sin_addr.s_addr = client->_local_addr;
    if(SOCKET_ERROR != _LSLP_BIND(sock, &local, sizeof(local))) {
      int bcast = ( (_LSLP_GETFLAGS(client->_msg_buf)) & LSLP_FLAGS_MCAST) ? 1 : 0 ;
      if(bcast) {
	if( (SOCKET_ERROR ==  _LSLP_SET_TTL(sock, client->_ttl) )  ||  
	    (SOCKET_ERROR == _LSLP_SETSOCKOPT(sock, SOL_SOCKET, SO_BROADCAST, (const int8 *)&bcast, sizeof(bcast)))) {
	  _LSLP_CLOSESOCKET(sock);
	  return(FALSE);
	}
	if(client->_local_addr != INADDR_ANY ) {
	  SOCKADDR_IN ma;
	  memset(&ma, 0x00, sizeof(ma));
	  ma.sin_addr.s_addr = client->_local_addr;
	  if( (SOCKET_ERROR == _LSLP_SETSOCKOPT(sock, IPPROTO_IP, IP_MULTICAST_IF, (const char *)&ma, sizeof(SOCKADDR_IN))) ) {
	    _LSLP_CLOSESOCKET(sock);
	    return(FALSE);
	  }
	}
      }
      target.sin_family = AF_INET;
      target.sin_port = client->_target_port;
      target.sin_addr.s_addr = client->_target_addr;


      if(SOCKET_ERROR == (err = _LSLP_SENDTO(sock, 
					     client->_msg_buf, 
					     _LSLP_GETLENGTH(client->_msg_buf), 
					     0, 
					     &target, sizeof(target) ))) {
	_LSLP_CLOSESOCKET(sock);
	return(FALSE);
      } /* oops - error sending data */

      while ( 0 < __service_listener(client, sock ) ) { ccode = TRUE; }
    } // bound the socket 
    _LSLP_CLOSESOCKET(sock);
  } /*  got the socket */
  return(ccode);
}
     

int32 service_listener_wait(struct slp_client *client, 
			       time_t wait, 
			       SOCKETD extra_sock, 
			       BOOL one_only, 
			       lslpMsg *list)
{
  int32 ccode = 0;
  
  if(list != NULL) {
    list->isHead = TRUE;
    list->prev = list->next = list;
    ccode = __service_listener_wait(client, wait, extra_sock, one_only);
    get_response(client, list);
  }
  return(ccode);
}


int32 __service_listener_wait(struct slp_client *client, time_t wait, SOCKETD extra_sock, BOOL one_only)       //jeb
{
  int32 rcv = 0;
  time_t now;
  time_t start = time(NULL);

  while( time(&now) && ((now - wait ) <= start )  ) {
    rcv += __service_listener(client, extra_sock);
    if(rcv > 0)
      if(one_only == TRUE)
	return(rcv);

    _LSLP_SLEEP(10);
  }
  rcv += __service_listener(client, extra_sock);
  return(rcv);
}


int32 service_listener(struct slp_client *client, SOCKETD extra_sock, lslpMsg *list)
{
  int32 ccode = 0;
  
  if(list != NULL ) {
    list->isHead = TRUE;
    list->prev = list->next = list;
    
    ccode = __service_listener(client, extra_sock);
    get_response(client, list);
  }
  return(ccode);
}


int32 __service_listener(struct slp_client *client, SOCKETD extra_sock )     //jeb
{

  struct timeval tv; 
  int32 err;
  LSLP_FD_SET fds;
  _LSLP_FD_ZERO(&fds);
  if(client->_rcv_sock != INVALID_SOCKET) {
    _LSLP_FD_SET(client->_rcv_sock, &fds);
  }
  if(extra_sock)
    _LSLP_FD_SET( extra_sock, &fds);

#ifdef NUCLEUS
    const char fun_name[] = "service_listener";
    err = -1;
    tv.tv_sec = client->_tv.tv_sec;
    tv.tv_usec = client->_tv.tv_usec;
    err = _LSLP_SELECT(((client->_rcv_sock != INVALID_SOCKET) && 
			(client->_rcv_sock > extra_sock)) ? 
		       client->_rcv_sock + 1: extra_sock + 1, &fds, NULL, NULL, &tv);

    // was there some data?
    if (err == NU_SUCCESS)
    {   //yes
        SOCKADDR_IN remote;
        int size = sizeof(SOCKADDR_IN);
        
        if (client->_rcv_sock != INVALID_SOCKET) 
        {
          if (_LSLP_FD_ISSET(client->_rcv_sock, &fds)) 
          {
     	    err = _LSLP_RECV_FROM(client->_rcv_sock, client->_rcv_buf, LSLP_MTU, 0, &remote, &size);
          }
        }
        //did we receive data or invalid socket 
        if (err > 0 ) 
        {
          //go decode the inbound message 
          decode_msg( client, &remote );
          return(err);
        }
        else //error on receive or invalid socket
        {
          _system_info(TRUE,"%s:Receive error=%d,skt=%d",fun_name, err, client->_rcv_sock);
          //close our socket 
          _LSLP_CLOSESOCKET(client->_rcv_sock );

          //go check if the interface is still up
          if ((slp_get_local_interfaces( &(client->_local_addr_list))) > 0  ) 
          {
             if(client->_rcv_sock != INVALID_SOCKET)
      	        client->_rcv_sock = slp_open_listen_sock();
          }

          return(err);
        }
    }
    //was it just no data yet 
    else if (err == NU_NO_DATA) 
    {
      //This is OK just return to retry select again later
      return(err);
    }
    //see if other than we time-out with no data 
    else  
    {
      _system_info(TRUE,"%s:Select:sck error=%d,sck=%d",fun_name, err,client->_rcv_sock);
      
      // our interfaces could be disconnected or we could be a laptop that 
      // just got pulled from the network, etc. 
      //close our socket 
      _LSLP_CLOSESOCKET(client->_rcv_sock );

      //go check if the interface is still up
      if ((slp_get_local_interfaces( &(client->_local_addr_list))) > 0  ) 
      {
         if(client->_rcv_sock != INVALID_SOCKET)
      	    client->_rcv_sock = slp_open_listen_sock();
      }
    }

 return(err);

#else //all other OS's

   do { 
     tv.tv_sec = client->_tv.tv_sec;
     tv.tv_usec = client->_tv.tv_usec;
     err = _LSLP_SELECT(((client->_rcv_sock != INVALID_SOCKET) && 
			 (client->_rcv_sock > extra_sock)) ? 
			client->_rcv_sock + 1: extra_sock + 1, &fds, NULL, NULL, &tv); 
   } while ( (err < 0 )&& (errno == EINTR)) ;
  
   
   if( 0 < err ) {
     SOCKADDR_IN remote;
     int size = sizeof(SOCKADDR_IN);
     if(client->_rcv_sock != INVALID_SOCKET) {
       if(_LSLP_FD_ISSET(client->_rcv_sock, &fds)) {
 	 err = _LSLP_RECV_FROM(client->_rcv_sock, client->_rcv_buf, LSLP_MTU, 0, &remote, &size); 
 	 decode_msg( client, &remote ); 
       }
     }

     if(extra_sock && _LSLP_FD_ISSET(extra_sock, &fds) ) {
       err = _LSLP_RECV_FROM(extra_sock, client->_rcv_buf, LSLP_MTU, 0, &remote, &size);
	 decode_msg( client, &remote );
     }
   }

   if (err == SOCKET_ERROR) {
     // our interfaces could be disconnected or we could be a laptop that 
     // just got pulled from the network, etc. 
     _LSLP_CLOSESOCKET(client->_rcv_sock );
     if( 0 < slp_get_local_interfaces( &(client->_local_addr_list) ) ) {
       if(client->_rcv_sock != INVALID_SOCKET)
	 client->_rcv_sock = slp_open_listen_sock();
     }
   } 
   return(err);
#endif

}

int srv_reg_all( struct slp_client *client,
		 const int8 *url,
		 const int8 *attributes,
		 const int8 *service_type,
		 const int8 *scopes,
		 int16 lifetime)
{
  uint32 target_addr_save;
  int convergence_save;
  
  // keep track of how many times we register
  int registrations = 0;

  assert(client != NULL && url != NULL && attributes != NULL && service_type != NULL && scopes != NULL);
  if(client == NULL || url == NULL || attributes == NULL || service_type == NULL || scopes == NULL )
    return(0);

  // save target and convergence parameters 
  target_addr_save = client->_target_addr;
  convergence_save = client->_convergence;
  client->_convergence = 0;
  
  if(  client->_use_das == TRUE ) {
    find_das(client, NULL, scopes) ;
    if( ! _LSLP_IS_EMPTY(&(client->das)) ) {
      struct da_list *da = client->das.next;
      while( ! _LSLP_IS_HEAD(da) ) {
	client->_target_addr = inet_addr(da->remote);
	if( TRUE == srv_reg( client, url, attributes, service_type, scopes, lifetime) )
	  registrations++;
	da = da->next;;
      }
    }
  }
  // restore parameters 
  client->_convergence = convergence_save;
  client->_target_addr = target_addr_save;
  /* do a unicast srvreg if we have the target set */
  if(client->_target_addr != _LSLP_MCAST) {
    if( TRUE == srv_reg( client, url, attributes, service_type, scopes, lifetime) )
      registrations++;
  } 
  return(registrations);
}

int  srv_reg_local(struct slp_client *client, 
		   const int8 *url,
		   const int8 *attributes,
		   const int8 *service_type,
		   const int8 *scopes,
		   uint16 lifetime)
     
{
  int count = 0;
    
  uint32 target_addr_save;
  int convergence_save;
  assert(client != NULL && url != NULL && attributes != NULL && service_type != NULL && scopes != NULL);
  if(client == NULL || url == NULL || attributes == NULL || service_type == NULL || scopes == NULL )
    return 0;

  target_addr_save = client->_target_addr;
  convergence_save = client->_convergence;
  client->_convergence = 0;
  client->_target_addr = inet_addr("127.0.0.1");
  if(TRUE == srv_reg(client, url, attributes, service_type, scopes, lifetime))
    count = 1;
  
  client->_convergence = convergence_save;
  client->_target_addr = target_addr_save;
  return count;
  
}

/* Note: this is how srv_reg_local will look with authentication support */

/* void srv_reg_local(struct slp_client *client,  */
/* 		   const int8 *url,  */
/* 		   int url_auth_count, */
/* 		   void *url_auth_blob, */
/* 		   const int8 *attributes,  */
/* 		   const int8 *service_type, */
/* 		   const int8 *scopes, */
/* 		   uint16 lifetime,  */
/* 		   int attr_auth_count,  */
/* 		   void *attr_auth_blob) */
/* { */
/* } */


void __srv_reg_local ( struct slp_client *client, 
		       const int8 *url,
		       const int8 *attributes, 
		       const int8 *service_type, 
		       const int8 *scopes, 
		       uint16 lifetime)   //jeb int16 to uint16  
{

  int8 *url_copy;
  lslpSrvRegList *reg;
  
  assert(client != NULL && url != NULL && attributes != NULL && \
	 service_type != NULL && scopes != NULL);
  
  url_copy = strdup(url);
  if(url_copy == NULL)
    abort();
  
  /* update the attributes if an existing registration */
  reg   = client->regs.next;
  while(! _LSLP_IS_HEAD(reg)) { 
    if ( 0 == lslp_string_compare(url_copy, reg->url->url)){ 
      free(url_copy);
      reg->directoryTime = lifetime + time(NULL);
      reg->url->lifetime = lifetime + time(NULL); 
      if(reg->attrList != NULL)
	lslpFreeAttrList(reg->attrList, LSLP_DESTRUCTOR_DYNAMIC); 
      reg->attrList = _lslpDecodeAttrString((int8 *)attributes); 
      return; 
    } 
    reg = reg->next; 
  }
  reg = (lslpSrvRegList *)calloc(1, sizeof(lslpSrvRegList));
  if(reg != NULL){
    int16 len;
    int8 *scope_copy = strdup(scopes);
    if(scope_copy == NULL)
      return;
    if(NULL == (reg->url = lslpAllocURL()))
      abort();
    reg->directoryTime = lifetime + time(NULL);
    reg->url->atomized = _lslpDecodeURLs(&url_copy, 1);
    reg->url->url = url_copy;
    reg->url->lifetime = lifetime + time(NULL);
    reg->url->auths = 0;
    reg->url->len = strlen(url_copy);
    reg->srvType = strdup(service_type);
    len = strlen(scopes) + 1;
    reg->scopeList  = lslpScopeStringToList(scope_copy, len);
    len = strlen(attributes);
    reg->attrList  = _lslpDecodeAttrString((int8 *)attributes);
    _LSLP_INSERT(reg, (lslpSrvRegList *)&(client->regs));
  }
  return;
}

BOOL slp_previous_responder(struct slp_client *client, int8 *pr_list)
{

  int8 *a, *s = NULL;
  uint32 addr, *list_addr;
  if(pr_list == NULL || 0 == strlen(pr_list))
    return(FALSE);

  a = _LSLP_STRTOK(pr_list, ",", &s);
  while(NULL != a ) {
    if(INADDR_NONE != (addr = inet_addr(a))) {
      list_addr = client->_local_addr_list;
      while( INADDR_ANY != *list_addr ) {
	if(*list_addr == addr)
	  return(TRUE);
	list_addr++;
      }
    }
    a = _LSLP_STRTOK(NULL, ",", &s);
  }
  return(FALSE);
}

struct slp_client *create_slp_client(const int8 *target_addr, 
				     const int8 *local_interface, 
				     uint16 target_port, 
				     const int8 *spi, 
				     const int8 *scopes,
				     BOOL should_listen, 
				     BOOL use_das)
{
  int16 len;
  struct slp_client *client;
  int8 *scope_copy;
  
  
  if(spi == NULL || scopes == NULL )
    return NULL;
  
  client = (struct slp_client *)calloc(1, sizeof(struct slp_client)); //jeb
  if(client == NULL)
    return NULL;
  
#ifdef _WIN32
  WindowsStartNetwork();
#endif

  /* initialize the random number generator for randomizing the 
     timing of multicast responses */
  srand(time(NULL));
  
  client->_buf_len = LSLP_MTU;
  client->_version = 1;
  client->_xid = 1;
  client->_target_port = htons(target_port);
  if( target_addr == NULL)
    client->_target_addr = inet_addr("239.255.255.253");
  else
    client->_target_addr = inet_addr(target_addr);
  if(local_interface == NULL)
    client->_local_addr = INADDR_ANY;
  else
    client->_local_addr = inet_addr(local_interface);
  scope_copy = strdup(spi);
  if(scope_copy == NULL){
    free(client);
    return NULL;
  }
  
  len = strlen(scope_copy) + 1;
  client->_spi = lslpScopeStringToList(scope_copy, len);
  free(scope_copy);
  scope_copy = strdup(scopes);
  if(scope_copy == NULL){
    free(client);
    return NULL;
  }
  len = strlen(scope_copy) + 1;
  client->_scopes = lslpScopeStringToList(scope_copy, len);
  free(scope_copy);
#ifdef NUCLEUS     //jeb
  client->_tv.tv_usec = TICKS_PER_SECOND; //socket call with timer UINT32
#else
  client->_tv.tv_usec = 200000;     //.2 sec
#endif 

  client->_use_das = use_das;
  client->_retries = 3;
  client->_ttl = 255;
  client->_convergence = 5;

  client->das.isHead = TRUE;
  client->das.next = client->das.prev = &(client->das);
  
  client->replies.isHead = TRUE;
  client->replies.next = client->replies.prev = &(client->replies);
  
  client->regs.isHead = TRUE;
  client->regs.next = client->regs.prev = (lslpSrvRegList *)&(client->regs);
  
  client->_local_addr_list = NULL;
  slp_get_local_interfaces(&(client->_local_addr_list));

  /* see if we can use a local DA. If not, open up the listen socket */
  client->_rcv_sock = INVALID_SOCKET;
  if(should_listen == TRUE ){
    client->_rcv_sock = slp_open_listen_sock();
  }
  if(client->_use_das == TRUE) {
    local_srv_req(client, NULL, NULL, "DEFAULT"); 
    if( ! _LSLP_IS_EMPTY(&(client->das))) {
      client->_rcv_sock = INVALID_SOCKET;
      client->_use_das = TRUE;
      client->_da_target_port = htons(427);
      client->_da_target_addr = inet_addr("127.0.0.1");
    }
  }
  
  client->get_response = get_response;
  client->find_das = find_das;
  client->discovery_cycle = discovery_cycle;
  client->converge_srv_req = converge_srv_req;
  client->unicast_srv_req = unicast_srv_req;
  client->local_srv_req = local_srv_req;
  client->srv_req = srv_req;
  client->srv_reg = srv_reg;
  client->srv_reg_all = srv_reg_all;
  client->srv_reg_local = srv_reg_local;
  client->service_listener = service_listener;
  client->prepare_pr_buf = prepare_pr_buf;
  client->decode_msg = decode_msg;
  client->decode_srvreq = decode_srvreq;
  client->decode_srvrply = decode_srvrply;
  client->decode_daadvert = decode_daadvert;
  client->send_rcv_udp = send_rcv_udp;
  client->service_listener_wait = service_listener_wait;
  client->slp_previous_responder = slp_previous_responder;
  
  return client;
}

void destroy_slp_client(struct slp_client *client)
{
  
  if(client == NULL)
    return;
  
  _LSLP_CLOSESOCKET(client->_rcv_sock);
  _LSLP_FREE_DEINIT(client->_local_addr_list);
  lslpFreeScopeList((lslpScopeList *)(client->_spi));
  lslpFreeScopeList(client->_scopes);
  _LSLP_FREE_DEINIT(client->_crypto_context);
  free(client);
  return;
}

/* returns a length-preceded escaped string compliant with an slp opaque value */
/* length is encoded as two-byte value at the beginning of the string */
/* size of returned buffer will be ((3 * length) + 3 + 2) */
int8 *encode_opaque(void *buffer, int16 length)
{
  static int8 transcode[16] = 
    { 
      '0', '1', '2', '3', '4', '5', '6', '7', 
      '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'
    };
  
  int8 *bptr, *srcptr, *buf;
  int32 encoded_length;
  
  if(buffer == NULL || length == 0)
    return(NULL);
  
  encoded_length = (length * 3) + 5;
  /* see if the encoded length will overflow the max size of an slp string */
  if( 0xffff0000 & encoded_length )
    return NULL;
  
  buf = malloc(encoded_length);
  if(buf == NULL)
    return NULL;
  bptr = buf;
  
  /* encode the length */
  _LSLP_SETSHORT(bptr, (int16)encoded_length, 0);
  bptr += 2;
  srcptr = (int8 *)buffer;
  *bptr = 0x5c; *(bptr + 1) = 0x66; *(bptr + 2) = 0x66;
  bptr += 3;
  
  while(length){
    *bptr = 0x5c;
    *(bptr + 1) = transcode[((*srcptr) & 0xf0) >> 4];
    *(bptr + 2) = transcode[(*srcptr) & 0x0f];
    bptr += 3;
    srcptr++;
    length--;
  }
  return buf;
}

/* returns an opaque buffer. size of opaque buffer will be */
/* ((size of escaped opaque string - 2) / 3) - 1  */
void *decode_opaque(int8 *buffer)
{
  
  int16 encoded_length, alloc_length;
  int8 *bptr, *srcptr, *retptr;

  if(buffer == NULL)
    return NULL;

  srcptr = buffer;
  retptr = bptr = NULL;
  /* get the length */
  encoded_length = _LSLP_GETSHORT(srcptr, 0);
  if(0 == encoded_length || 0 > encoded_length)
    return NULL;
  alloc_length = (encoded_length / 3) - 1;
  
  srcptr += 2;
  encoded_length -= 2; 
  
  /* check the header */
  if( *srcptr == 0x5c ){
    if( (*(srcptr + 1) == 0x46) || (*(srcptr + 1) == 0x66 ) ) {
      if( (*(srcptr + 2) == 0x46) || (*(srcptr + 2) == 0x66 ) ) {
	retptr = (bptr = malloc(alloc_length));
	if(bptr == NULL)
	  return NULL;
	/* adjust the encoded length to reflect that we consumed the header */
	encoded_length -= 3;
	srcptr += 3;
	
	while(encoded_length && alloc_length){
	  int8 accumulator = 0;
	  if(*srcptr == 0x5c){
	    /* encode the high nibble */
	    if( *(srcptr + 1) < 0x3a && *(srcptr + 1) > 0x2f){
	      /* asci decimal char */
	      accumulator = (*(srcptr + 1) - 0x30) * 0x10;
	    }
	    else if( *(srcptr + 1) < 0x47 && *(srcptr + 1) > 0x40){
	      accumulator = (*(srcptr + 1) - 0x37) * 0x10;
	    }
	    else if( *(srcptr + 1) < 0x67 && *(srcptr + 1) > 0x60){
	      accumulator = (*(srcptr + 1) - 0x57) * 0x10;
	    }
	    /* encode the low nibble */
	    if( *(srcptr + 2) < 0x3a && *(srcptr + 2) > 0x2f){
	      /* asci decimal char */
	      accumulator += *(srcptr + 2) - 0x30;
	    } 
	    else if( *(srcptr + 2) < 0x47 && *(srcptr + 2) > 0x40){
	      accumulator += *(srcptr + 2) - 0x37;
	    }
	    else if( *(srcptr + 2) < 0x67 && *(srcptr + 2) > 0x60){
	      accumulator += *(srcptr + 2) - 0x57;
	    }
	    /* store the byte */
	    *bptr = accumulator;
	  }
	  else
	    break;

	  /* update pointers and counters */
	  srcptr += 3;
	  bptr++;
	  encoded_length -= 3;
	  alloc_length--;
	}
	if( alloc_length || encoded_length) {
	  free(retptr);
	  retptr = NULL;
	}
      }
    }
  }
  return (void *)retptr;
}



/* right now we don't support the attr request unless it specifies a complete url. */

void decode_attrreq(struct slp_client *client, SOCKADDR_IN *remote)
{
  int8 *bptr;
  lslpURL *url;
  lslpScopeList *scopes;
  lslpAttrList *attr_tags, *attr_return;
  lslpSrvRegList *regs;
  
  int16 str_len, buf_len, err = LSLP_PARSE_ERROR;
  int32 total_len, purported_len;
  
  /* read the length from the slp header */
  bptr = client->_rcv_buf;
  purported_len = _LSLP_GETLENGTH(bptr);
  if(purported_len > LSLP_MTU )
    return;
  
  bptr += (total_len = _LSLP_HDRLEN(bptr));
  
  /* use total_len as a running count of the msg length, ensure it is */
  /* consistent with the purported length to protect against overflow */
  if(total_len < purported_len){
    /* get the length of the PR list and look at it */
    str_len = _LSLP_GETSHORT(bptr, 0);
    if( (str_len + total_len + 2) < purported_len) {
      if(FALSE == slp_previous_responder(client, (str_len ? bptr + 2 : NULL))){
	bptr += str_len + 2;
	total_len += str_len + 2;

	/* decode the url */
	str_len = _LSLP_GETSHORT(bptr, 0);
	if( (str_len + total_len + 2) < purported_len ) {
	  if(NULL != (url = lslpAllocURL())) {
	     if(NULL != (url->url = (int8 *)malloc(str_len + 1))) {
	       memcpy(url->url, bptr, str_len);
	       *(bptr + str_len) = 0x00;
	       url->atomized = _lslpDecodeURLs( (int8 **)&(url->url), 1);
	       bptr += str_len + 2;
	       total_len += str_len + 2;
	       buf_len = LSLP_MTU - total_len;
	       str_len = _LSLP_GETSHORT(bptr, 0);
	       scopes = lslpUnstuffScopeList(&bptr, &buf_len, &err);
	       total_len += str_len + 2;
	       /* see if we even support the scopes */
	       if( NULL != url->atomized && TRUE == lslp_scope_intersection(client->_scopes, scopes)) {
		 /* decode the attribute tags */
		 int8 *bptrSave;
		 SOCKETD sock;                      //jeb
		 str_len = _LSLP_GETSHORT(bptr, 0 );
		 attr_tags = lslpUnstuffAttr(&bptr, &buf_len, &err);
		 total_len += str_len + 2;
		 /* see if we have url match */

		 regs = client->regs.next;
		 attr_return = NULL;
		 
		 while(! _LSLP_IS_HEAD(regs) ){
		   if( ! lslp_string_compare(regs->url->url, url->url)) {
		     /* we have a hit on the url, see if the scopes intersect */
		     if( TRUE == lslp_scope_intersection(regs->scopeList, scopes)){
		       attr_return = regs->attrList;
		       break;
		     }
		   }
		   regs = regs->next;
		 }
		 
		 /* stuff the attribute reply */

		 _LSLP_SETVERSION(client->_msg_buf, LSLP_PROTO_VER);
		 _LSLP_SETFUNCTION(client->_msg_buf, LSLP_ATTRRPLY);
		 // skip the length for now
		 _LSLP_SETFLAGS(client->_msg_buf, 0);
		 _LSLP_SETNEXTEXT(client->_msg_buf, 0);
		 _LSLP_SETXID( client->_msg_buf, ( _LSLP_GETXID(client->_rcv_buf)));
		 _LSLP_SETLAN(client->_msg_buf, LSLP_EN_US, LSLP_EN_US_LEN);
		 total_len = _LSLP_HDRLEN(client->_msg_buf);

		 
		 bptr = client->_msg_buf + total_len;
		 _LSLP_SETSHORT(bptr, err, 0);
		 bptr += 2;
		 total_len += 2;
		 
		 buf_len = LSLP_MTU - (total_len + 1);
		 bptrSave = bptr;
		 
		 /* stuff the attr list */

		 if( FALSE == lslpStuffAttrList(&bptr, &buf_len, attr_return, attr_tags)){
		   /* overflow, set the flag, stuff a zero attr list */
		   _LSLP_SETFLAGS(client->_msg_buf,  LSLP_FLAGS_OVERFLOW);
		   buf_len = LSLP_MTU - total_len;
		   bptr = bptrSave;
		   lslpStuffAttrList(&bptr, &buf_len, NULL, NULL);
		 }
		 /* for now don't support auth blocks */

		 _LSLP_SETBYTE(bptr, 0, 0);
		 
		 /* to calculate the length, re-read the attr list length */
		 total_len +=(2 + _LSLP_GETSHORT(bptrSave, 0));
		 /* add the length of the auth block (zero plus the number of auths) */
		 total_len += 1;
		 
		// ok, now we can set the length
		_LSLP_SETLENGTH(client->_msg_buf, total_len );
		
/* 		client->_msg_buf is stuffed with the attr reply. now we need  */
/* 		to allocate a socket and send it back to the requesting node  */
		
		if(INVALID_SOCKET != (sock = _LSLP_SOCKET(AF_INET, SOCK_DGRAM, 0))) {
		  SOCKADDR_IN local;
		  int err = 1;
#ifndef NUCLEUS
		  _LSLP_SETSOCKOPT(sock, SOL_SOCKET, SO_REUSEADDR, (const char *)&err, sizeof(err) );
#endif		  
		  local.sin_family = AF_INET;
		  local.sin_port = client->_target_port ; 
		  local.sin_addr.s_addr = client->_local_addr;
		  if(SOCKET_ERROR != _LSLP_BIND(sock, &local, sizeof(local))) {
		    _LSLP_SENDTO(sock, client->_msg_buf, total_len , 0, 
				 (remote), sizeof(SOCKADDR_IN )) ;
		  } /*  successfully bound this socket  */
		  _LSLP_CLOSESOCKET(sock);
		} /* successfully opened this socket */
		if(attr_tags)
		  lslpFreeAttrList(attr_tags, TRUE);
	       } /* scopes intersect */
	       if(scopes)
		 lslpFreeScopeList(scopes);
	       
	     } /* url string allocated */
	     if(url)
	       lslpFreeURL(url);
	  } /* url allocated */
	}
      } /* not on the pr list */
    }
  }
}


/* folds extra whitespace out of the string - in place */
/* destroys parameter ! */
int8 *lslp_foldString(int8 *s)
  {
  int8 *src, *dst;
  assert(s != 0);
  src = s;
  while(isspace(*src))
    src++;
  if(*src == 0x00) {
    /* return a string of exactly one space  */
    *s = 0x20;
    *(s + 1) = 0x00;
    return(s);
  }
  dst = s;
  while(*src != 0x00) {
    *dst = *src;
    dst++;
    src++;
    if(isspace(*src)) {
      *dst = *src;
      dst++; 
      src++;
      while(isspace(*src))
	src++;
    }
  }
  *dst = 0x00;
  if(isspace(*(dst - 1)))
    *(dst - 1) = 0x00;
  return(s);
}

BOOL lslpStuffAttrList(int8 **buf, int16 *len, lslpAttrList *list, lslpAttrList *include)
{
		
  lslpAttrList *attrs, *included;
  int16 attrLen = 0, lenSave;
  int8 *bptr;
  BOOL ccode = FALSE, hit = TRUE;
  assert(buf != NULL);
  assert(len != NULL && *len > 3);
/*   assert(list != NULL); */
  if (buf == NULL || len == NULL || *len < 3 )
    return(FALSE);
  /* always return TRUE on an empty list so we can continue to build the */
  /* msg buffer - an empty list is not an error! */
  if (list == NULL || _LSLP_IS_EMPTY(list)) {
    _LSLP_SETSHORT((*buf), 0, 0);  
    (*buf) += 2;
    (*len) -= 2;
    return(TRUE);
  }
  /* attr list */
  lenSave = *len;
  attrs = list->next;
  bptr = *buf;
  memset(bptr, 0x00, *len);
  (*buf) += 2;	 /* reserve space for the attrlist length short */
  (*len) -= 2;
  while (! _LSLP_IS_HEAD(attrs) && attrLen + 1 < *len) {
    assert(attrs->type != head);
    if(include != NULL && _LSLP_IS_HEAD(include) && (! _LSLP_IS_EMPTY(include))) {
      included = include->next;
      hit = FALSE;
      
      while(! _LSLP_IS_HEAD(included)){
	if( ! lslp_string_compare(attrs->name, included->name)){
	  hit = TRUE;
	  break;
	}
	included = included->next;
      }
    }
    
    if(hit == FALSE){
      attrs = attrs->next;
      continue;
    }
    
    if (attrLen + (int16)strlen(attrs->name) + 3 < *len)
      {
	if (attrs->type != tag)
	  {
	    **buf = '(';  
	    (*buf)++; 
	    attrLen++;
	    (*len)--;
	  }
	strcpy(*buf, attrs->name);
	(*buf) += strlen(attrs->name); 
	attrLen += strlen(attrs->name);
	(*len) -= strlen(attrs->name);
	if (attrs->type != tag)
	  {
	    **buf = '='; 
	    (*buf)++; 
	    attrLen++;
	    (*len)--;
	  }
	switch (attrs->type)
	  {
	  case tag:
	    break;
	  case string:
	    if (attrLen + (int16)strlen(attrs->val.stringVal) + 2 < *len)
	      {
		strcpy(*buf, (attrs->val.stringVal));
		(*buf) += strlen(attrs->val.stringVal); 
		attrLen +=  strlen(attrs->val.stringVal);
		(*len) -= strlen(attrs->val.stringVal);
		ccode = TRUE;
	      }
	    else
	      ccode = FALSE;
	    break;
	  case integer:
	    if (attrLen + 33 + 2 < *len)
	      {
		_itoa( attrs->val.intVal, *buf, 10 );
		attrLen += strlen(*buf);
		(*buf) += strlen(*buf);
		(*len) -= strlen(*buf);
		ccode = TRUE;
	      }
	    else
	      ccode = FALSE;
	    break;
	  case bool_type:
	    if (attrLen + 6 + 2 < *len)
	      {
		if (attrs->val.boolVal)
		  strcpy(*buf, "TRUE");
		else
		  strcpy(*buf, "FALSE");
		attrLen += strlen(*buf);
		(*buf) += strlen(*buf);
		(*len) -= strlen(*buf);
		ccode = TRUE;
	      }
	    else
	      ccode = FALSE;
	    break;
	  case opaque:
	    {
	      int16 opLen;
	      opLen = (_LSLP_GETSHORT(((int8 *)attrs->val.opaqueVal), 0));
	      if (attrLen + opLen + 3 < *len)
		{
		  memcpy(*buf, (((int8 *)attrs->val.opaqueVal) + 2), opLen);
		  (*buf) += opLen;
		  attrLen += opLen;
		  (*len) -= opLen;
		  ccode = TRUE;
		}
	      else
		ccode = FALSE;
	      break;
	    }
	  default:
	    ccode = FALSE;
	    break;
	  }
	if (ccode == TRUE && attrLen + 2 < *len && attrs->type != tag)
	  {
	    **buf = ')'; 
	    (*buf)++; 
	    attrLen++;
	    (*len)--;
	  }
	if (ccode == TRUE && ! _LSLP_IS_HEAD(attrs->next) && attrLen + 1 < *len)
	  {
	    **buf = ','; 
	    (*buf)++; 
	    attrLen++;
	    (*len)--;
	  }
      }  /* if room for the attr name */
    else
      {
	ccode = FALSE;
	break;
      }
    attrs = attrs->next;
  } /* while we are traversing the attr list */
  /* set the length short */
  if (ccode == TRUE)
    {
      _LSLP_SETSHORT(bptr, attrLen, 0);
    }
  else
    {
      (*buf) = bptr; 
      (*len) = lenSave;
      memset(*buf, 0x00, *len);
    }
  return(ccode);
}	


lslpAttrList *lslpUnstuffAttr(int8 **buf, int16 *len, int16 *err)  
{
  int16 tempLen;
  lslpAttrList *temp = NULL;
  assert(buf != NULL && *buf != NULL);
  assert(len != NULL);
  assert(err != NULL);
  *err = 0;
  tempLen = _LSLP_GETSHORT(*buf, 0);
  if(tempLen > 0) {
    (*buf) += sizeof(int16);
    (*len) -= sizeof(int16);
    if(*len >= tempLen) {
      if(NULL != (temp = _lslpDecodeAttrString(*buf))) {
      (*buf) += tempLen;
      (*len) -= tempLen;
      } else {*err = LSLP_PARSE_ERROR; }
    } else {*err = LSLP_INTERNAL_ERROR; }
  }
  return(temp);
}



lslpURL *lslpAllocURL(void)
{
  lslpURL *url = (lslpURL *)calloc(1, sizeof(lslpURL));

  return(url);
}	

lslpURL *lslpAllocURLList(void)
{
  lslpURL *head;
  if (NULL != (head = lslpAllocURL()))
    {
      head->next = head->prev = head;
      head->isHead = TRUE;
      return(head);
    }
  return(NULL);	
}	

/* url MUST be unlinked from list */
void lslpFreeURL(lslpURL *url)
{
  assert(url != NULL);
  if (url->url != NULL)
    free(url->url);
  if ( url->authBlocks != NULL)
      lslpFreeAuthList(url->authBlocks);
  if(url->atomized != NULL)
    lslpFreeAtomizedURLList(url->atomized, TRUE);
  if(url->attrs)
    lslpFreeAtomList(url->attrs, LSLP_DESTRUCTOR_DYNAMIC);
  free(url);
  return;
}	

void lslpFreeURLList(lslpURL *list)
{								
  lslpURL *temp;

  assert(list != NULL);
  assert(_LSLP_IS_HEAD(list));
  while(! (_LSLP_IS_EMPTY(list)))
    {
      temp = list->next;
      _LSLP_UNLINK(temp);
      lslpFreeURL(temp);
    }
  lslpFreeURL(list);
  return;
}	


/* for protected scopes, we need to change stuff URL so that it */
/* only stuffs url auth blocks that match spis in the srv req */


/* this routine may return FALSE AND alter the buffer and length */
BOOL  lslpStuffURL(int8 **buf, int16 *len, lslpURL *url) 
{
  assert((buf != NULL) && (*buf != NULL));
  assert((len != NULL) && (*len > 8));
  assert((url != NULL) && (! _LSLP_IS_HEAD(url)));
  if(_LSLP_IS_HEAD(url))
    return(FALSE);
  memset(*buf, 0x00, *len);
  /* advanced past the reserved portion */
  (*buf) += sizeof(int8);
  _LSLP_SETSHORT(*buf, url->lifetime - time(NULL), 0);
  (*buf) += sizeof(int16);
  url->len = strlen(url->url);
  /* url->url is a null terminated string, but we only stuff the non-null bytes */
  _LSLP_SETSHORT(*buf, url->len, 0);

  (*buf) += sizeof(int16);
  (*len) -= 5;
  if(*len < url->len - 1)
    return(FALSE);
  memcpy(*buf, url->url, url->len);
  (*buf) += url->len;
  (*len) -= url->len;
  return(lslpStuffAuthList(buf, len, url->authBlocks));
}

/* this routine may  return FALSE AND alter the buffer and length */
BOOL lslpStuffURLList(int8 **buf, int16 *len, lslpURL *list) 
{
  BOOL ccode = TRUE;
  assert((buf != NULL) && (*buf != NULL));
  assert((len != NULL) && (*len > 8));
  assert((list != NULL) && (_LSLP_IS_HEAD(list)));
  if(! _LSLP_IS_HEAD(list))
    return(FALSE);
  while((ccode == TRUE) && (! _LSLP_IS_HEAD(list->next))) {
    list = list->next;
    ccode = lslpStuffURL(buf, len, list);
  }
  return(ccode);
}

lslpURL *lslpUnstuffURL(int8 **buf, int16 *len, int16 *err) 
{

  int8 *tempurl;
  lslpURL *temp;
  assert(buf != NULL && *buf != NULL);
  assert(len != NULL && *len > 8);
  assert(err != NULL);
  *err = 0;
  if(NULL != (temp = lslpAllocURL())) {
    temp->lifetime = _LSLP_GETSHORT((*buf), 1);
    temp->len = _LSLP_GETSHORT((*buf), 3);
    (*buf) += 5;
    (*len) -= 5;
    if(*len >= temp->len) { 
      if (NULL != (temp->url = (int8 *)calloc(1, temp->len + 1))) {
	memcpy(temp->url, *buf, temp->len);
	*((temp->url) + temp->len) = 0x00;
	tempurl = temp->url;
	temp->atomized = _lslpDecodeURLs((int8 **)&tempurl, 1);
	(*buf) += temp->len;
	(*len) -= temp->len;
	if(temp->atomized != NULL)
	temp->authBlocks = lslpUnstuffAuthList(buf, len, err);
      } else {*err = LSLP_INTERNAL_ERROR;}
    } else {*err = LSLP_PARSE_ERROR; }
  } else {*err = LSLP_INTERNAL_ERROR;}
  if(*err != 0 && temp != NULL) {
    lslpFreeURL(temp);
    temp = NULL;
  }
  return(temp);
}


void lslpFreeFilterTree(lslpLDAPFilter *root)
{
  assert(root != NULL);
  if( ! _LSLP_IS_EMPTY( &(root->children) ) ) {
    lslpFreeFilterTree((lslpLDAPFilter *)root->children.next);
  }
  if( ! (_LSLP_IS_HEAD(root->next)) && (! _LSLP_IS_EMPTY(root->next)) )
    lslpFreeFilterTree(root->next);
  
  if(root->attrs.next != NULL) {
    while(! (_LSLP_IS_EMPTY(&(root->attrs)))) {
      lslpAttrList *attrs = root->attrs.next;
      _LSLP_UNLINK(attrs);
      lslpFreeAttr(attrs);
    }
  }
}


BOOL lslpEvaluateOperation(int compare_result, int operation)
{
  switch(operation) {
  case expr_eq:
    if(compare_result == 0)	/*  a == b */
      return(TRUE);
    break;
  case expr_gt:
    if(compare_result >= 0)	/*  a >= b  */
      return(TRUE);
    break;

  case expr_lt:			/* a <= b  */
    if(compare_result <= 0)
      return(TRUE);
    break; 
  case expr_present:
  case expr_approx:
  default:
    return(TRUE);
    break;
  }
  return(FALSE);
}

/* evaluates attr values, not names */
BOOL lslpEvaluateAttributes(const lslpAttrList *filter, const lslpAttrList *registered, int op)
{
  assert(filter != NULL && registered != NULL && (! _LSLP_IS_HEAD(filter)) && (! _LSLP_IS_HEAD(registered))) ;
  /* first ensure they are the same type  */
  switch(filter->type) {
  case opaque:
  case string:
    if(registered->type != string && registered->type != opaque)
      return(FALSE);
    
    if( registered->type == opaque || filter->type == opaque) {
      return(lslpEvaluateOperation(memcmp(registered->val.stringVal, 
					  filter->val.stringVal, 
					  _LSLP_MIN(registered->attr_len, 
						    strlen(filter->val.stringVal))), op));
    } else {
      if( TRUE == lslp_pattern_match(registered->val.stringVal, 
				     filter->val.stringVal, 
				     FALSE)) {
	return( lslpEvaluateOperation(0, op)) ;
      } else {
	return (lslpEvaluateOperation(1, op));
      }
    }
    break;
  case integer:
    return( lslpEvaluateOperation( filter->val.intVal - registered->val.intVal, op));
  case tag:			/* equivalent to a presence test  */
    return(TRUE);
  case bool_type:
    if((filter->val.boolVal != 0) && (registered->val.boolVal != 0))
      return(TRUE);
    if((filter->val.boolVal == 0) && (registered->val.boolVal == 0))
      return(TRUE);
    break;
  default: 
    break;
  }
  return(FALSE);
}

/* filter is a filter tree, attrs is ptr to an attr listhead */

BOOL lslpEvaluateFilterTree(lslpLDAPFilter *filter, const lslpAttrList *attrs)
{
  assert(filter != NULL);
  assert(attrs != NULL);
  assert(! _LSLP_IS_HEAD(filter));
  if(filter == NULL || (_LSLP_IS_HEAD(filter)) || attrs == NULL)
    return FALSE;
  
  if(! _LSLP_IS_HEAD(filter->children.next) ) {
    lslpEvaluateFilterTree((lslpLDAPFilter *)filter->children.next, attrs);
  }
  if( ! (_LSLP_IS_HEAD(filter->next)) && (! _LSLP_IS_EMPTY(filter->next)) ) {
    lslpEvaluateFilterTree(filter->next, attrs);
  }
  if(filter->_operator == ldap_and || filter->_operator == ldap_or || filter->_operator == ldap_not) {
    /* evaluate ldap logical _operators by evaluating filter->children as a list of filters */
    lslpLDAPFilter *child_list = (lslpLDAPFilter *)filter->children.next;
    /* initialize  the filter's logical value to TRUE */
    if(filter->_operator == ldap_or)
      filter->logical_value = FALSE;
    else
      filter->logical_value = TRUE;
    while(! _LSLP_IS_HEAD(child_list)) {
      if(child_list->logical_value == TRUE)  {
	if(filter->_operator == ldap_or) {
	  filter->logical_value = TRUE;
	  break;
	}
	if(filter->_operator == ldap_not) {
	  filter->logical_value = FALSE;
	  break;
	}
	/* for an & _operator keep going  */
      } else {
	/* child is FALSE */
	if(filter->_operator == ldap_and) {
	  filter->logical_value = FALSE;
	  break;
	}
      }
      child_list = child_list->next;
    }
  } else  {
    /* find the first matching attribute and set the logical value */
    filter->logical_value = FALSE;
    if(! _LSLP_IS_HEAD(filter->attrs.next) ) {
      attrs = attrs->next;
      while( (! _LSLP_IS_HEAD(attrs )) && 
	     ( FALSE  == lslp_pattern_match(filter->attrs.next->name, attrs->name, FALSE)) ) {
	attrs = attrs->next ; 
      }
      /* either we have traversed the list or found the first matching attribute */
      if( ! _LSLP_IS_HEAD(attrs) ) {
	/* we found the first matching attribute, now do the comparison */
	if (filter->_operator == expr_present || filter->_operator == expr_approx) 
	  filter->logical_value = TRUE;
	else
	  filter->logical_value = lslpEvaluateAttributes(filter->attrs.next, attrs, filter->_operator );
      }
    }
  }
  return(filter->logical_value);
}

lslpScopeList *lslpScopeStringToList(int8 *s, int16 len) 
{
  int8 *p, *saveP, *t;
  lslpScopeList *h, *temp;
  if(s == NULL)
    return(lslpAllocScopeList());
  if(NULL != (h  = lslpAllocScopeList())) {
    saveP = (p = (int8 *)malloc(len + 1));
    if( p == NULL)
      return NULL;
    memcpy(p, s, len);
    *(p + len) = 0x00;
    
    while(NULL != (t = strchr(p, ','))) {
      *t = 0x00;
      t++;
      p = lslp_foldString(p);
      if((lslp_islegal_scope(p)) && (NULL != (temp = lslpAllocScope()))) {
	if(NULL != (temp->scope = strdup(p))) {
	  _LSLP_INSERT(temp, h);
	}
      }
      p = t;
    }
    p = lslp_foldString(p);
    if((lslp_islegal_scope(p)) && (NULL != (temp = lslpAllocScope()))) {
      if(NULL != (temp->scope = strdup(p))) {
	_LSLP_INSERT(temp, h);
      }
    }
    free(saveP);
  }
  return(h);
}

BOOL lslpStuffScopeList(int8 **buf, int16 *len, lslpScopeList *list)
{
  lslpScopeList *scopes;
  int16 scopeLen = 0, lenSave;
  int8 *bptr;
  BOOL ccode = FALSE;
  assert(buf != NULL);
  assert(len != NULL && *len > 3);
  assert(list != NULL);
  if (buf == NULL || len == NULL || *len < 3 || list == NULL)
    return(FALSE);
  /* always return TRUE on an empty list so we can continue to build the */
  /* msg buffer - an empty list is not an error! */
  if (_LSLP_IS_EMPTY(list))
    return(TRUE);
  /* scope list */
  lenSave = *len;
  scopes = list->next;
  bptr = *buf;
  memset(*buf, 0x00, *len);
  (*buf) += 2;	 /* reserve space for the scopelist length short */
  (*len) -= 2;
  while (! _LSLP_IS_HEAD(scopes) && scopeLen + 1 < *len)
    { 
      if (scopeLen + (int16)strlen(scopes->scope) < *len)
	{
	  ccode = TRUE;
	  strcpy(*buf, scopes->scope);
	  (*buf) += strlen(scopes->scope); 
	  scopeLen += strlen(scopes->scope) ;
	  if (! _LSLP_IS_HEAD(scopes->next) )
	    {
	      **buf = ','; 
	      (*buf)++; 
	      scopeLen++;
	    }
	}  /* if room for the attr name */
      else
	{
	  ccode = FALSE;
	  break;
	}
      scopes = scopes->next;
    } /* while we are traversing the attr list */
  /* set the length short */
  if (ccode == TRUE)
    {
      //      **buf = 0x00;
      //      (*buf)++;
      //      scopeLen++;
      //      << Thu May 30 16:18:57 2002 mdd >>
      (*len) -= scopeLen;
      _LSLP_SETSHORT(bptr, scopeLen, 0);
    }
  else
    {
      *len = lenSave;
      *buf = bptr;
      memset(*buf, 0x00, *len);
    }
  return(ccode);
	
}	


lslpScopeList *lslpUnstuffScopeList(int8 **buf, int16 *len, int16 *err)
{
  int16 tempLen;
  lslpScopeList *temp = NULL;
  assert(buf != NULL && *buf != NULL);
  assert(len != NULL);
  assert(err != NULL);
  *err = 0;
  tempLen = _LSLP_GETSHORT(*buf, 0);
  (*buf += 2);
  (*len -= 2);  
  if(tempLen != 0) {
    if (tempLen <= *len ) {
      if(NULL != (temp = lslpScopeStringToList(*buf, tempLen))) {
	(*buf) += tempLen;
	(*len) -= tempLen;
      } else{*err = LSLP_INTERNAL_ERROR;}
    } else {*err = LSLP_PARSE_ERROR;}
  }
  return(temp);
}


lslpSPIList *lslpUnstuffSPIList(int8 **buf, int16 *len, int16 *err)
{
  return((lslpSPIList *)lslpUnstuffScopeList(buf, len, err));
}


BOOL lslpStuffSPIList(int8 **buf, int16 *len, lslpSPIList *list)
{
  return(lslpStuffScopeList(buf, len, (lslpScopeList *)list));
}


BOOL lslp_scope_intersection(lslpScopeList *a, lslpScopeList *b) 
{
  if((a == NULL) || (b == NULL))
    return(TRUE);

  assert(_LSLP_IS_HEAD(a));
  assert(_LSLP_IS_HEAD(b));
  if((_LSLP_IS_EMPTY(a)) || (_LSLP_IS_EMPTY(b))) {
    return(FALSE);
  }
  while(! (_LSLP_IS_HEAD(a->next))) {
    a = a->next;
    while(!(_LSLP_IS_HEAD(b->next))) {
      b = b->next;
      assert((a->scope != NULL) && (b->scope != NULL));
      if(! strcasecmp(a->scope, b->scope)) {
	return(TRUE);
      }
    }
    /* reset b */
    b = b->next;
  }
  return(FALSE);
}

/* caseless compare that folds whitespace */
int lslp_string_compare(int8 *s1, int8 *s2)
{
  assert(s1 != NULL);
  assert(s2 != NULL);
  lslp_foldString(s1);
  lslp_foldString(s2);
  if( TRUE == lslp_pattern_match(s1, s2, FALSE))
    return 0;
  return -1;
}

/* return 1 if char is legal for scope value, 0 otherwise */
int lslp_isscope(int c) 
{  
  int i;
  static int8 scope_reserved[] = { '(', ')', ',', 0x5c, '!', '<', '=', '>', '~', ';', '*', '+', 0x7f };
  
  if(! _LSLP_ISASCII(c))
    return 0;
  
  if(c < 0x20)
    return(0);
  for(i = 0; i < 13; i++) {
    if(c == scope_reserved[i])
      return(0);
  }
  return(1);
}

BOOL lslp_islegal_scope(int8 *s)
{
  int8 *temp;
  assert(s != NULL);
  if(! strlen(s)) {
    return(FALSE);
  }
  temp = s;
  while(*temp != 0x00) {
    if(! lslp_isscope(*temp)) {
      return(FALSE);
    }
    temp++;
  }
  return(TRUE);
}

BOOL lslp_srvtype_match(const int8 *s, int8 *r)
{
  int8 *sp;
  assert(s != NULL);
  assert(r != NULL);
  /* check to see if search is for an abstract service type */
  sp = (int8 *)r;
  while(*sp != 0x00) { sp++; }
  /* if this is a search for an abstract service type, use a wildcard */
  /* to collect all concrete registrations under this type */
  if(*(sp - 1) == ':') {*(sp - 1) = '*'; }
  /* check for a dangling semicolon and kill it  */
  else if ( *(sp - 1) == ';') { *(sp - 1) = 0x00 ; }

  return(lslp_pattern_match(s, r, FALSE));
}


lslpScopeList *lslpAllocScope(void)
{
  return((lslpScopeList *)calloc(1, sizeof(lslpScopeList)));
}

lslpScopeList *lslpAllocScopeList(void)
{
  lslpScopeList *head;
  if (NULL != (head = lslpAllocScope()))
    {
      head->next = head->prev = head;
      head->isHead = TRUE;	
    }
  return(head);
}	

void lslpFreeScope(lslpScopeList *s)
{
  assert(s != NULL);
  if (s->scope != NULL)
    free(s->scope);
  free(s);
}	

void lslpFreeScopeList(lslpScopeList *head)
{
  lslpScopeList *temp;
  assert(head != NULL);
  assert(_LSLP_IS_HEAD(head));
  while (! (_LSLP_IS_EMPTY(head)))
    {
      temp = head->next;
      _LSLP_UNLINK(temp);
      lslpFreeScope(temp);
    }
  lslpFreeScope(head);
  return;
}	



lslpSrvRegList *lslpAllocSrvReg(void)
{
  lslpSrvRegList *l;
  if (NULL != (l = (lslpSrvRegList *)calloc(1, sizeof(lslpSrvRegList)))) {
    if(NULL != (l->url = lslpAllocURL())) {
      if ( NULL != (l->scopeList = lslpAllocScopeList())) {
	if ( NULL != (l->attrList = lslpAllocAttrList())) {
	  if (NULL != (l->authList = lslpAllocAuthList())) {
	    return(l);
	  }
	  lslpFreeAttrList(l->attrList, TRUE);     
	}
	lslpFreeScopeList(l->scopeList);
      }
      lslpFreeURL(l->url); 
    }
    free(l);
  }
  return(NULL);
}	

lslpSrvRegHead *lslpAllocSrvRegList(void)
{
  lslpSrvRegHead *head;
  if (NULL != (head = (lslpSrvRegHead *)calloc(1, sizeof(lslpSrvRegHead)))) {
    head->next = head->prev = (struct lslp_srv_reg_instance *)head;
    head->isHead = TRUE;
    return(head);
  }
  return(NULL);
}	

void lslpFreeSrvReg(lslpSrvRegList *s)
{
  assert(s != NULL);
  if(s->url != NULL)
    lslpFreeURL(s->url);
  if (s->srvType != NULL)
    free(s->srvType);
  if (s->scopeList != NULL)
    lslpFreeScopeList(s->scopeList);
  if (s->attrList != NULL)
    lslpFreeAttrList(s->attrList, TRUE);
  if(s->authList != NULL)
    lslpFreeAuthList(s->authList);
  free(s);
}	

void lslpFreeSrvRegList(lslpSrvRegHead *head)
{
  lslpSrvRegList *temp;
  assert(head != NULL);
  assert(_LSLP_IS_HEAD(head));
  while (!  (_LSLP_IS_EMPTY((struct lslp_srv_reg_instance *)head)))
    {
      temp = head->next;
      _LSLP_UNLINK(temp);
      lslpFreeSrvReg(temp);
    }
  free(head);
}	


lslpAuthBlock *lslpAllocAuthBlock(void)
{
  return((lslpAuthBlock *)calloc(1, sizeof(lslpAuthBlock)));
}	

lslpAuthBlock *lslpAllocAuthList(void)
{
  lslpAuthBlock *head;
  if (NULL != (head = lslpAllocAuthBlock()))
    {
      head->next = head->prev = head;
      head->isHead = TRUE;
      return(head);
    }
  return(NULL);
}

	
/* NOTE: be CERTAIN block is not linked to a list !!! */
void lslpFreeAuthBlock(lslpAuthBlock *auth)
{
  if (auth->spi != NULL)
    free(auth->spi);
  if (auth->block != NULL)
    free(auth->block); 
  free(auth);
  return;		
}	

void lslpFreeAuthList(lslpAuthBlock *list)
{								
  lslpAuthBlock *temp;

  assert(list != NULL);
  assert(_LSLP_IS_HEAD(list));
  while(! (_LSLP_IS_EMPTY(list)))
    {
      temp = list->next;
      _LSLP_UNLINK(temp);
      lslpFreeAuthBlock(temp);
    }
  lslpFreeAuthBlock(list);
  return;
}	

/* for protected scopes, we need to change this routine (or add another one) */
/* that uses an SPI list as an input and stuffs only auth only stuffs  auth blocks*/
/* that match spis in the input list */

/* this routine will return FALSE AND alter the buffer and length */
BOOL lslpStuffAuthList(int8 **buf, int16 *len, lslpAuthBlock *list)
{
  int8 *bptr;
  int16 numAuths = 0;
  lslpAuthBlock *auths;
  BOOL ccode = TRUE;
  assert(buf != NULL);
  assert(len != NULL);
  /*   assert(list != NULL); */
  if (buf == NULL || *buf == NULL || len == NULL )
    return(FALSE);
  /* always return true on an empty list so we can continue to build the */
  /* msg buffer - an empty list is not an error! */
  bptr = *buf;
  _LSLP_SETBYTE(bptr, numAuths, 0);

  (*buf)+= 1;
  (*len) -= 1;
  if(list == NULL || _LSLP_IS_EMPTY(list))
    return(TRUE);
  /* auth list */
  auths  = list->next;
  memset(*buf, 0x00, *len);
  while (! _LSLP_IS_HEAD(auths) && auths->len <= *len)
    {
      /* store the auth bsd */ 
      _LSLP_SETSHORT(*buf, auths->descriptor, 0);
      /* store the the bsd size */
      _LSLP_SETSHORT(*buf, auths->len, 2);
      /* store the timestamp */
      _LSLP_SETLONG(*buf, auths->timestamp, 4);
      /* store spi string length */
      _LSLP_SETSHORT(*buf, auths->spiLen, 8);
      assert(strlen(auths->spi) + 1 == auths->spiLen );
      /* store the spi string */
      strcpy((*buf) + 10, auths->spi);
      /* store the authentication block */
      if (auths->block != NULL && (auths->len - (auths->spiLen + 10) ) > 0 )
	memcpy(((*buf) + 10 + auths->spiLen), auths->block, (auths->len - (auths->spiLen + 10)));
      (*buf) += auths->len; 
      (*len) -= auths->len;
      numAuths++;
      auths = auths->next;
    } /* while we are traversing the attr list */
  if (! _LSLP_IS_HEAD(auths))
    {
      /* we terminated the loop before copying all the auth blocks */
      ccode = FALSE;
    }
  else
    {
      _LSLP_SETBYTE(bptr, numAuths, 0);
    }
  return(ccode);
}	

lslpAuthBlock *lslpUnstuffAuthList(int8 **buf, int16 *len, int16 *err)
{
  int16 tempLen;
  lslpAuthBlock *list = NULL, *temp;
  assert(buf != NULL && *buf != NULL);
  assert(len != NULL);
  assert(err != NULL);
  *err = 0;
  tempLen = _LSLP_GETBYTE(*buf, 0);        /* get the number of auth-blocks */
  (*buf) += 1;                                /* advance to the auth blocks */
  (*len) -= 1;
  if((tempLen == 0) || (NULL != (list = lslpAllocAuthList()))) {
    if(list != NULL) {
      while((tempLen > 0) && (*len > 10) && (*err == 0)) {
	if(NULL != (temp = lslpAllocAuthBlock())) {
	  temp->descriptor = _LSLP_GETSHORT(*buf, 0); /* get the block structure descriptor */
	  (*buf) += 2;                                  /* advance to the block length */
	  temp->len = _LSLP_GETSHORT(*buf, 0);        /* get the block length */
	  (*buf) += 2;                                  /* advance to the timestamp */
	  temp->timestamp = _LSLP_GETLONG(*buf, 0);   /* get the timestamp */
	  (*buf) += 4;                                  /* advance to the spi length */
	  temp->spiLen = _LSLP_GETSHORT(*buf, 0);     /* get the spi length */
	  (*buf) += 2;                                  /* advance to the spi */ 
	  *len -= 10;
	  if(*len >= (temp->spiLen)) {
	    if(NULL != (temp->spi = (uint8 *)calloc(temp->spiLen + 1, sizeof(uint8)))) {
	      memcpy(temp->spi, *buf, temp->spiLen);  /* copy the spi */
	      (*buf) += temp->spiLen;                   /* advance to the next block */
	      (*len) -= temp->spiLen;
	      if(*len >= (temp->len - (10 + temp->spiLen))) {
		if (NULL != (temp->block = 
			     (uint8 *)calloc((temp->len - (10 + temp->spiLen)) + 1, 
					     sizeof(uint8)))) {
		  memcpy(temp->block, *buf, (temp->len - (10 + temp->spiLen)) );
		  _LSLP_INSERT(temp, list);                /* insert the auth block into the list */ 
		  (*buf) += (temp->len - (10 + temp->spiLen));
		  (*len) -= (temp->len - (10 + temp->spiLen));
		  temp = NULL;
		} else {*err = LSLP_INTERNAL_ERROR; } /* if we alloced the auth block buffer */
	      } else { *err = LSLP_PARSE_ERROR;}
	    } else { *err = LSLP_INTERNAL_ERROR ;}/* if we alloced the spi buffer */
	  } else { *err = LSLP_PARSE_ERROR;} 
	}else { *err = LSLP_INTERNAL_ERROR; } /* if we alloced the auth block */
	tempLen--; /* decrement the number of auth blocks */
      } /* while there is room and there are auth blocks to process */
    } else {
      if(tempLen == 0)
	*err = 0;
      else
	*err = LSLP_INTERNAL_ERROR ; 
    }
  }
  if(*err != 0 && list != NULL) {
    lslpFreeAuthList(list);
    list = NULL;
  }
  return(list);
}


uint32 lslpCheckSum(int8 *s, int16 l)
{
	uint32 c;
	BOOL pad = 0;
	uint16 a = 0, b = 0, *p;
	assert(s != NULL);
	if (l == 1)
		return(0);
	assert(l > 1);
	if (l & 0x01)
		pad = TRUE;
	p = (uint16 *)s;
	while (l > 1)
	{
		a += *p++;
		b += a;
		l -= 2;
	}
	/* "pad" the string with a zero word */
	if (pad == TRUE)
		b += a;
	/* return the value as a dword with containing two shorts in */
	/* network order -- ab */
	_LSLP_SETSHORT((uint8 *)&c, a, 0 );
	_LSLP_SETSHORT((uint8 *)&c, a, 2 );
	return(c);
}	


lslpHdr * lslpAllocHdr(void)
{
  return((lslpHdr *)calloc(1, sizeof(lslpHdr)));
}	

void lslpFreeHdr(lslpHdr *hdr)
{
  assert(hdr != NULL);
  if (hdr->data != NULL)
    free(hdr->data);
  free(hdr);
}	

void lslpDestroySrvReq(struct lslp_srv_req *r, int8 flag)
{
  assert(r != NULL);
  if (r->prList != NULL)
    free(r->prList);
  if (r->srvcType != NULL)
    free(r->srvcType);
  if (r->scopeList != NULL)
    lslpFreeScopeList(r->scopeList);
  if (r->predicate != NULL)
    free(r->predicate);
  if(r->spiList != NULL)
    lslpFreeSPIList(r->spiList);
  if (flag == LSLP_DESTRUCTOR_DYNAMIC)
    free(r);
}	

void lslpDestroySrvRply(struct lslp_srv_rply *r, int8 flag) 
{
  assert(r != NULL);
  if (r->urlList != NULL)
    lslpFreeURLList(r->urlList);
  if(r->attr_list != NULL)
    lslpFreeAtomList(r->attr_list, LSLP_DESTRUCTOR_DYNAMIC);
  if (flag == LSLP_DESTRUCTOR_DYNAMIC)
    free(r);
}	

void lslpDestroySrvReg(struct lslp_srv_reg *r, int8 flag)
{
  assert(r != NULL);
  if (r->url != NULL)
    lslpFreeURLList(r->url);
  if (r->srvType != NULL)
    free(r->srvType);
  if (r->scopeList != NULL)
    lslpFreeScopeList(r->scopeList);
  if (r->attrList != NULL)
    lslpFreeAttrList(r->attrList, TRUE);
  if (r->attrAuthList != NULL)
    lslpFreeAuthList(r->attrAuthList);
  if (flag == LSLP_DESTRUCTOR_DYNAMIC)
    free(r);
}	

void lslpDestroySrvAck(struct lslp_srv_ack *r, int8 flag)
{
  assert(r != NULL);
  if (flag == LSLP_DESTRUCTOR_DYNAMIC)
    free(r);
}	

void lslpDestroyDAAdvert(struct lslp_da_advert *r, int8 flag)
{
  assert(r != NULL);
  if (r->url != NULL)
    free(r->url);
  if (r->scopeList != NULL)
    free(r->scopeList);
  if (r->attrList != NULL)
    free(r->attrList);
  if (r->spiList != NULL)
    free(r->spiList);
  if (r->authBlocks != NULL)
    lslpFreeAuthList(r->authBlocks);
  if (flag == LSLP_DESTRUCTOR_DYNAMIC)
    free(r);
}	

void lslpDestroySAAdvert(struct lslp_sa_advert *r, int8 flag)
{
  assert(r != NULL);
  if (r->url != NULL)
    free(r->url);
  if (r->scopeList != NULL)
    free(r->scopeList);
  if (r->attrList != NULL)
    free(r->attrList);
  if (r->authBlocks != NULL)
    lslpFreeAuthList(r->authBlocks);
  if (flag == LSLP_DESTRUCTOR_DYNAMIC)
    free(r);
}	

void lslpDestroySrvTypeReq(struct lslp_srvtype_req *r, int8 flag)
{
  assert(r != NULL);
  if (r->prList != NULL)
    free(r->prList);
  if (r->nameAuth != NULL)
    free(r->nameAuth);
  if (r->scopeList != NULL)
    lslpFreeScopeList(r->scopeList);
  if (flag == LSLP_DESTRUCTOR_DYNAMIC)
    free(r);
  return;
}	

void lslpDestroySrvTypeReply(struct lslp_srvtype_rep *r, int8 flag)
{
  assert(r != NULL);
  if (r->srvTypeList != NULL)
    free(r->srvTypeList);
  if (flag == LSLP_DESTRUCTOR_DYNAMIC)
    free(r);
  return;	
}	

void lslpDestroyAttrReq(struct lslp_attr_req *r, int8 flag)
{
  assert(r != NULL);
  if (r->prList != NULL)
    free(r->prList);
  if (r->url != NULL)
    free(r->url);
  if (r->scopeList != NULL)
    free(r->scopeList);
  if (r->tagList != NULL)
    free(r->tagList);
  if (r->spiList != NULL)
    lslpFreeSPIList(r->spiList);			
  if (flag == LSLP_DESTRUCTOR_DYNAMIC)
    free(r);
  return;	
}	

void lslpDestroyAttrReply(struct lslp_attr_rep *r, int8 flag)
{
  assert(r != NULL);
  if (r->attrList != NULL)
    free(r->attrList);
  if (r->authBlocks != NULL)
    lslpFreeAuthList(r->authBlocks);
  if (flag == LSLP_DESTRUCTOR_DYNAMIC)
    free(r);
  return;	
	
}	

void lslpDestroySrvDeReg(struct lslp_srv_dereg *r, int8 flag)
{
  assert(r != NULL);
  if (r->scopeList != NULL)
    free(r->scopeList);
  if (r->urlList != NULL)
    lslpFreeURLList(r->urlList);
  if (r->tagList != NULL)
    free(r->tagList);
  if (flag == LSLP_DESTRUCTOR_DYNAMIC)
    free(r);
  return;	
}	


lslpMsg *alloc_slp_msg(BOOL head)
{
  lslpMsg *ret = (lslpMsg *)calloc(1, sizeof(lslpMsg));
  if(ret != NULL ) {
    ret->dynamic = LSLP_DESTRUCTOR_DYNAMIC;
    if( head == TRUE ){  
      ret->next = ret->prev = ret;
      ret->isHead = TRUE;
    }
  }
  return ret;
}


void lslpDestroySLPMsg(lslpMsg *msg, int8 flag)
{
  assert(msg != NULL);
  switch (msg->type)
    {
    case srvReq:
      lslpDestroySrvReq(&(msg->msg.srvReq), LSLP_DESTRUCTOR_STATIC);
      break;
    case srvRply:
      lslpDestroySrvRply(&(msg->msg.srvRply), LSLP_DESTRUCTOR_STATIC);
      break;
    case srvReg:
      lslpDestroySrvReg(&(msg->msg.srvReg), LSLP_DESTRUCTOR_STATIC);
      break;
    case srvAck:
      lslpDestroySrvAck(&(msg->msg.srvAck), LSLP_DESTRUCTOR_STATIC);
      break;
    case daAdvert:
      lslpDestroyDAAdvert(&(msg->msg.daAdvert), LSLP_DESTRUCTOR_STATIC);
      break;
    case saAdvert:
      lslpDestroySAAdvert(&(msg->msg.saAdvert), LSLP_DESTRUCTOR_STATIC);
      break;
    case srvTypeReq:
      lslpDestroySrvTypeReq(&(msg->msg.srvTypeReq), LSLP_DESTRUCTOR_STATIC);
      break;
    case srvTypeRep:
      lslpDestroySrvTypeReply(&(msg->msg.srvTypeRep), LSLP_DESTRUCTOR_STATIC);
      break;
    case attrReq:
      lslpDestroyAttrReq(&(msg->msg.attrReq), LSLP_DESTRUCTOR_STATIC);
      break;
    case attrRep:
      lslpDestroyAttrReply(&(msg->msg.attrRep), LSLP_DESTRUCTOR_STATIC);
      break;
    case srvDereg:
      lslpDestroySrvDeReg(&(msg->msg.srvDereg), LSLP_DESTRUCTOR_STATIC);
      break;
    default:
      break;
    }
  if (msg->dynamic == LSLP_DESTRUCTOR_DYNAMIC)
    free(msg);
  return;
}	

/* a is an attribute list, while b is a string representation of an ldap filter  */
BOOL lslp_predicate_match(lslpAttrList *a, int8 *b)
{
  BOOL ccode;
  lslpLDAPFilter *ftree;
  if(a == NULL)
    return FALSE;
  assert(_LSLP_IS_HEAD(a));
  if(b == NULL || ! strlen(b))
    return(TRUE);		/*  no predicate - aways tests TRUE  */
  if(NULL != (ftree = _lslpDecodeLDAPFilter(b))) {
    ccode = lslpEvaluateFilterTree(ftree, a);
    lslpFreeFilterTree(ftree);
    return(ccode);
  }
  return(FALSE);
}

struct lslp_srv_rply_out *_lslpProcessSrvReq(struct slp_client *client, 
					     struct lslp_srv_req *msg, 
					     int16 errCode)
{
  int8 *buf;
  int16 bufLen, avail;
  lslpSrvRegList *reg;
  int16 ext_offset;
  int8 *extptr, *next_extptr;
  BOOL pile_up_attrs = FALSE;

  struct lslp_srv_rply_out *temp_rply = 
    (struct lslp_srv_rply_out *)calloc(1, sizeof(struct lslp_srv_rply_out));
  assert(msg != NULL);
  if(msg != NULL && temp_rply != NULL) {
    if((temp_rply->errCode = errCode) == 0) {
      buf = (int8 *)calloc(LSLP_MTU, sizeof(int8));
      if(buf != NULL) {
	bufLen = LSLP_MTU;
	temp_rply->urlList = buf;
	buf += 4;
	avail = LSLP_MTU - 4;

	/* check for the attr list extension */
	/* remember to subtract 5 bytes from the limit for each extension we traverse */
	if(msg->next_ext != NULL) {
	  /* we are looking for extension ID 0x0002 */
	  extptr = msg->next_ext;
	  while(extptr && extptr < msg->ext_limit) {
	    if( 0x0002 == _LSLP_GETSHORT(extptr, 0)) {
	      /* make sure it doesn't overflow */
	      if (extptr + 9 > msg->ext_limit) {
		break;
	      }
	      /* got our extension */
	      /* do a sanity check offset five should be 0x0000, */
	      /* offset 7 should be 0x0000*/
	      if( (0x0000 == _LSLP_GETSHORT(extptr, 5)) && 
		  (0x0000 == _LSLP_GETSHORT(extptr, 7)) ) {
		/* ok, accumulate attribute data */
		pile_up_attrs = TRUE;
	      }
	      break;
	    }
	    /* see if there are any more extensions */
	    extptr += _LSLP_GET3BYTES(extptr, 2);
	  } /* while traversing extensions */ 
	} /* if there is an extension */
	

	reg = client->regs.next;
	extptr = client->_scratch;
	next_extptr = extptr + 2;
	ext_offset = 0;
	while(! _LSLP_IS_HEAD(reg)) {
	  if(time(NULL) > reg->directoryTime) { 
	    /* the lifetime for this registration has expired */
	    /* unlink and free it */
	    lslpSrvRegList *temp_reg = reg;
	    reg = reg->next;
	    _LSLP_UNLINK(temp_reg);
	    lslpFreeSrvReg(temp_reg);
	    continue;
	  }
	  if(TRUE == lslp_srvtype_match(reg->srvType, msg->srvcType)) {
	    /* we need to check for the special case - of a srvreq with service type directory-agent */
	    /* - it is allowed to omit scopes */
	    if((msg->scopeList == NULL) ||  
	       (_LSLP_IS_EMPTY(msg->scopeList) && (! strncasecmp(msg->srvcType, DA_SRVTYPE, DA_SRVTYPELEN ))) || 
	       (TRUE == lslp_scope_intersection(reg->scopeList, msg->scopeList))) {
	      if(TRUE == lslp_predicate_match(reg->attrList, msg->predicate)) {
		/* we have a match */
		/* for protected scopes, we need to change stuff URL so that it */
		/* only stuffs url auth blocks that match spis in the srv req */
		if(TRUE == lslpStuffURL(&buf, &avail, reg->url)) {
		  if(TRUE == pile_up_attrs && ((reg->url->len + ext_offset + 2) < LSLP_MTU)) {
		    /* use the scratch buffer as temporary storage for the attribute extensions */
		    /* go back and set the length of the next extension in prev extension */
		    _LSLP_SET3BYTES(next_extptr, ext_offset, 0);
		    next_extptr = extptr + 2;
		    /* set the extension id */
		    _LSLP_SETSHORT(extptr, 0x0002, 0);
		    /* init the next ext to zero */
		    _LSLP_SET3BYTES(next_extptr, 0x00000000, 0);
		    /* length of url string */
		    _LSLP_SETSHORT(extptr, reg->url->len, 5);
		    memcpy(extptr + 7, reg->url->url, reg->url->len);
		    ext_offset += (7 + reg->url->len);
		    extptr += (7 + reg->url->len);
		    /* attr list length */
		    if( ext_offset + 2 + reg->attrList->attr_string_len < LSLP_MTU) {
		      _LSLP_SETSHORT(extptr, reg->attrList->attr_string_len, 0);
		      memcpy(extptr + 2, reg->attrList->attr_string, reg->attrList->attr_string_len);
		      ext_offset += (2 + reg->attrList->attr_string_len);
		      extptr += (2 + reg->attrList->attr_string_len);
		      if( ext_offset + 1 < LSLP_MTU) {
			/* set the number of attr auths to zero */
//jeb attrib set to 1??			_LSLP_SETBYTE(extptr, 1, 0);
			_LSLP_SETBYTE(extptr, 0, 0);
			extptr += 1;
			ext_offset += 1;
		      } else { pile_up_attrs = FALSE; _LSLP_SETSHORT(client->_scratch, 0x0000, 0);}
		    } else { pile_up_attrs = FALSE; _LSLP_SETSHORT(client->_scratch, 0x0000, 0);}
		  } else { pile_up_attrs = FALSE; _LSLP_SETSHORT(client->_scratch, 0x0000, 0);}
		  temp_rply->urlCount++;
		}else {
		  temp_rply->errCode = LSLP_INTERNAL_ERROR;
		  return(temp_rply);
		}
	      } /* if we have a match */
	    } /* if we found a scope intersection */
	  } /* if the types match */
	  reg = reg->next;
	} /* while we are traversing the reg list for this da */
	/* stuff the error code and url count */
	_LSLP_SETSHORT(temp_rply->urlList, temp_rply->errCode, 0);
	_LSLP_SETSHORT(temp_rply->urlList, temp_rply->urlCount, 2);
	/* resize to the actual size needed */
	temp_rply->urlLen = (buf - temp_rply->urlList) ;
      } /* if we alloced our buffer */
    } /* if we need to look for matches */
    else {
      if(NULL != (temp_rply->urlList = (int8 *)calloc(8, sizeof(int8)))) {  //jeb
	_LSLP_SETSHORT(temp_rply->urlList, temp_rply->errCode, 0);
      }
    } /* error reply */
    if(temp_rply->urlList == NULL) {
      free(temp_rply);
      temp_rply = NULL;
    }
  } /* if we alloced our reply structure */
  return(temp_rply);
}


#define lslp_to_lower(c)  (((c) > 0x40 && (c) < 0x5b) ? ((c) + 0x20) : (c))
/* based upon TclStringCaseMatch */
#define MAX_RECURSION  10
BOOL lslp_pattern_match(const int8 *string, const int8 *pattern, BOOL case_sensitive)
{
  
  int8 s, p;
  static int recursion_level;
  if(recursion_level == MAX_RECURSION) {
    return FALSE;
 }
  recursion_level++;
  
  assert(string != NULL && pattern != NULL);

  while(1) {
    p = *pattern;
    if( p == '\0' ) {
      recursion_level--;
      if (*string == '\0')  //jeb
          return TRUE;
      else
          return FALSE;
//jeb      return (*string == '\0');
    }

    if((*string == '\0') && (p != '*')) {
      recursion_level--;
      return FALSE;
    }

    if(p == '*') {
      while(*(++pattern) == '*') { ; }
      p = *pattern;
      if( p == '\0') {
	recursion_level--;
	return TRUE;
      }
      while(1) {
	if((p != '[') && (p != '?') && (p != '\\')) {
	  /* advance the string until there is a match */
	  while(*string) {
	    s = *string;
	    if(case_sensitive == FALSE) {
	      p = lslp_to_lower(p);
	      s = lslp_to_lower(s);
	    }
	    if(s == p)
	      break;
	    string++;
	  }
	}
	if(TRUE == lslp_pattern_match(string, pattern, case_sensitive)) {
	  recursion_level--;
	  return TRUE;
	}
	if(*string == '\0') {
	  recursion_level--;
	  return FALSE;
	}
	string++;
      }
    }
    if(p == '?') {
      pattern++;
      string++;
      continue;
    }
    
    if(p == '[') {
      int8 start, end;
      pattern++;
      s = *(string++);
      if(case_sensitive == FALSE) {
	s = lslp_to_lower(s);
      }
      while(1) {
	if(*pattern == ']' || *pattern == '\0') {
	  recursion_level--;
	  return FALSE ;
	}
	start = *(pattern++);
	if(case_sensitive == FALSE){
	  start = lslp_to_lower(start);
	}
	if(*pattern == '-') {
	  pattern++;
	  if(*pattern == '\0') {
	    recursion_level--;
	    return FALSE;
	  }
	  end = *(pattern++);
	  if(case_sensitive == FALSE) {
	    end = lslp_to_lower(end);
	  }
	  if(((start <= s) && (s <= end )) || ((end <= s) && (s <= start))) {
	    break;
	  }
	} else if (start == s) {
	  
	  break;
	}
      }
      while(*pattern != ']') {
	if(*pattern == '\0') {
	  pattern--;
	  break;
	}
	pattern++;
      }
      pattern++;
      continue;
    }

    if (p == '\\') {
      pattern++;
      if (*pattern == '\0') {
	recursion_level--;
	return FALSE;
//jeb	return 0;
      }
    }
    s = *(string++);
    p = *(pattern++);
    if(case_sensitive == FALSE) {
      p = lslp_to_lower(p);
      s = lslp_to_lower(s);
    }
    if(s == p) {
      continue;
    }
    recursion_level--;
    return FALSE;
  }
}


int8 * lslp_get_next_ext(int8 *hdr_buf)
{
  int32 len;
  assert(hdr_buf != NULL);
  if( 0 != _LSLP_GETNEXTEXT(hdr_buf)) {
    len = _LSLP_GETLENGTH(hdr_buf);
    if(len > _LSLP_GETNEXTEXT(hdr_buf)) {
      return( hdr_buf + _LSLP_GETNEXTEXT(hdr_buf));
    }
  }
  return NULL;
}


/** uses a newline as the field separator, two consecutive newlines as the record separator **/
void lslp_print_srv_rply(lslpMsg *srvrply)
{
  lslpURL *url_list;
  if(srvrply != NULL && srvrply->type == srvRply) {
    /* output errCode, urlCount, urlLen */
    printf("%d\n%d\n%d\n", srvrply->msg.srvRply.errCode, 
                           srvrply->msg.srvRply.urlCount, 
                           srvrply->msg.srvRply.urlLen );
    if((NULL != srvrply->msg.srvRply.urlList) && 
       (! _LSLP_IS_EMPTY( srvrply->msg.srvRply.urlList))) {
      
      url_list = srvrply->msg.srvRply.urlList->next;
      while( ! _LSLP_IS_HEAD(url_list)){
	
	if(NULL != url_list->url)
	  printf("URL: %s\n", url_list->url);
	else
	  printf("URL: \n");
	
	/* see if there are attributes to print */
	if(NULL != url_list->attrs && ! _LSLP_IS_HEAD(url_list->attrs->next)){
	  lslpAtomList *attrs = url_list->attrs->next;
	  while(! _LSLP_IS_HEAD(attrs)){
	    printf("ATTR: %s\n", attrs->str);
	    attrs = attrs->next;
	  } /* while traversing attr list */
	} /* if attr list */
	url_list = url_list->next;
	
      } /* while traversing url list */
    } /* if there are urls to print */
    /* print the record separator */
    printf("\n\n");
  }
}

/* string must not be null and must start with "service:" */
SLP_STORAGE_DECL BOOL test_service_type(int8 *type)
{
  int8 * temp;
  BOOL ccode;
  temp = strdup(type);
  if(type == NULL || ! strlen(type))
    return FALSE;
  if(temp == NULL) abort();
  ccode = lslp_pattern_match( "service:*", temp, FALSE);
  free(temp);
    return ccode;
}


SLP_STORAGE_DECL BOOL test_service_type_reg(int8 *type)
{
  int8 * temp;
  BOOL ccode;
   
  if(type == NULL || ! strlen(type))
    return FALSE;
  temp = strdup(type);
  if(temp == NULL) abort();
  ccode = lslp_pattern_match(temp, "service:*", FALSE);
  free(temp);
    return ccode;
}


SLP_STORAGE_DECL BOOL test_url(int8 *url)
{

  int8 *temp;
  lslpAtomizedURL *aurl;
  
  if(url == NULL)
    return FALSE;
  temp = strdup(url);
  if(temp == NULL) abort();
  aurl = _lslpDecodeURLs(&temp, 1);
  free(temp);
  if(aurl != NULL){
    lslpFreeAtomizedURLList(aurl, TRUE);
    return TRUE;
  }
  return FALSE;
}

SLP_STORAGE_DECL BOOL test_attribute(int8 *attr)
{
  int8 *temp;
  lslpAttrList *attr_list;
  
  if(attr == NULL)
    return FALSE;
  if(! strlen(attr))
    return TRUE;
  
  temp = strdup(attr);
  if(temp == NULL) abort();
  
  attr_list = _lslpDecodeAttrString(temp);
  free(temp);
  
  if(attr_list != NULL){
    lslpFreeAttrList(attr_list, TRUE);
    return TRUE;
  }
  return FALSE;

}

SLP_STORAGE_DECL BOOL test_scopes(int8 *scopes)
{
  BOOL ccode;
  int8 *temp;
  
  if(scopes == 0 ){
    return FALSE;
  }
  
  if(! strlen(scopes))
    return TRUE;
    
  temp = strdup(scopes);
  if(temp == 0 ) abort();
  ccode = lslp_islegal_scope(temp);
  free(temp);

  
  return ccode;
}

SLP_STORAGE_DECL BOOL test_predicate(int8 *predicate)
{
  int8 * temp;
  lslpLDAPFilter *filter;
  
  if(predicate == 0)
    return FALSE;
  if(! strlen(predicate))
    return TRUE;
  
  temp = strdup(predicate);
  if(temp == 0) abort();
  filter = _lslpDecodeLDAPFilter(temp);
  free(temp);
  if(filter != NULL) {
    lslpFreeFilter(filter);
    return TRUE;
  }
  return FALSE;
}


SLP_STORAGE_DECL uint32 test_srv_reg(int8 *type, 
				     int8 *url,   
				     int8 *attr, 
				     int8 *scopes)
{

  /* it is legal for the scope string and attribute list to be empty */
  if(type == NULL || 0 == strlen(type) || FALSE == test_service_type_reg(type))
    return 1;
  if(url == NULL || 0 == strlen(url) || FALSE == test_url(url) )
    return 2;
  if(attr != NULL && FALSE == test_attribute(attr))
    return 3;
  if(scopes != NULL && FALSE == test_scopes(scopes))
    return 4;
  return 0;
}



SLP_STORAGE_DECL uint32 test_query(int8 *type,
				   int8 *predicate, 
				   int8 *scopes)
{
  /* it is legal for predicate and scopes to be empty */
  
  if(type == NULL || 0 == strlen(type) || FALSE == test_service_type(type))
    return 1;

  if(predicate != NULL && FALSE == test_predicate(predicate))
    return 2;
    
  if(scopes != NULL && FALSE == test_scopes(scopes))
    return 3;
 
  return 0;
}



