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


#include "slp.h"

PEGASUS_USING_STD;

PEGASUS_NAMESPACE_BEGIN

PEGASUS_EXPORT String slp_get_host_name(void)
{

  String s = String();
  Sint8 *buf = new Sint8[255];
  if( 0 == gethostname(buf, 254) ) 
    s += buf;
  delete buf;
  return(s);
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

#ifdef PEGASUS_OS_HPUX

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
    *errnop = h_errno;
    return(-1);
  } 
  return(0);
}
#endif

PEGASUS_EXPORT String slp_get_addr_string_from_url(const String & url) 
{
  String s = String();
  struct sockaddr_in addr;
  if( get_addr_from_url( url, &addr) ) {
    Sint8 *name = new Sint8[255];
#ifdef _WIN32
    _snprintf(name, 254, "%s:%d", inet_ntoa(addr.sin_addr), ntohs(addr.sin_port) );
#else
    snprintf(name, 254, "%s:%d", inet_ntoa(addr.sin_addr), ntohs(addr.sin_port) );
#endif
    s += name;
    delete [] name;
  }
  return(s);
}

PEGASUS_EXPORT Boolean get_addr_from_url(const String & url, struct sockaddr_in *addr )
{
  Sint8 *bptr, *url_dup;
  Boolean ccode = false;

  // isolate the host field 
  bptr = (url_dup = url.allocateCString());
  if(bptr == NULL)
    return(false );

  while( (*bptr != '/') && (*bptr != 0x00) )
    bptr++;

  if(*bptr == '/' && *(bptr + 1) == '/') {
    Sint8 *endptr, *portptr;
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
      addr->sin_port = htons( (Sint16)strtoul(portptr, NULL, 0) );
    else
      addr->sin_port = 0x0000;
    addr->sin_family = AF_INET;
    
    bptr += 2;
    addr->sin_addr.s_addr = inet_addr(bptr);
    if(addr->sin_addr.s_addr == INADDR_NONE) {
      struct hostent *host;
      struct hostent hostbuf;
      Uint8 *temp ;
      Uint32 result, err;
      size_t hostbuflen = 256;
      
      // hopefully a hostname because dotted decimal notation was invalid
      // look for the user@hostname production
      Sint8 *userptr;
      userptr = bptr;
      while( (*userptr != 0x00 ) && (*userptr != '@' ) )
	userptr++;
      if( *userptr == '@' )
	bptr = userptr + 1;
      
      temp = (Uint8 *)malloc(hostbuflen); 
      if(temp != NULL) {
	host = NULL;
	while(temp != NULL && (result = gethostbyname_r(bptr, &hostbuf, (char *)temp, hostbuflen, 
							&host, (int *)&err)) == ERANGE){
	  hostbuflen *= 2;
	  temp = (Uint8 *)realloc(temp, hostbuflen);
	}
	if(host != NULL) {
	  struct in_addr *ptr;
	  if (((ptr = (struct in_addr *)*(host->h_addr_list)) != NULL ) ) {
	    addr->sin_addr.s_addr = ptr->s_addr;
	    ccode = true;
	  }
	}
	free(temp);
      } /* we allocated the temp buffer for gethostbyname */
    } else { ccode = true; } /* host field is not in a valid dotted decimal form */
  } /* isolated the host field in the url  */
  return(ccode);
}
 

static Boolean  slp_join_multicast(SOCKET sock, struct in_addr *addr) 
{
  
  // don't join on the loopback interface
  if (addr->s_addr == inet_addr("127.0.0.1") )
    return(false);

  struct ip_mreq mreq;
  mreq.imr_multiaddr.s_addr = inet_addr("239.255.255.253");
  mreq.imr_interface.s_addr = addr->s_addr;
  
  if(SOCKET_ERROR == setsockopt(sock,IPPROTO_IP, IP_ADD_MEMBERSHIP, (const char *)&mreq, sizeof(mreq))) 
    return(false);
  return(true);
}



static int slp_join_multicast_all(SOCKET sock)
{


  int num_interfaces = 0;

  String host_name = slp_get_host_name();

  if( 0 < host_name.size() ) {
    struct hostent hostbuf;
    struct hostent *host ;
    
    int result = 0, err;
    size_t hostbuf_len = 512;
    Sint8 *hn = host_name.allocateCString( );
    Sint8 *buf = (Sint8 *)malloc(hostbuf_len) ;
    while( buf != NULL && ( result = gethostbyname_r( hn, 
						      &hostbuf, 
						      buf, 
						      hostbuf_len, 
						      &host, 
						      &err) )  == ERANGE ) {
      hostbuf_len *= 2;
      buf = (Sint8 *)realloc(buf, hostbuf_len);
    } // realloc buffer loop

    if((host != NULL) && (result == 0)) {
      struct in_addr *ptr;
      while ( ( ptr = (struct in_addr *) *(host->h_addr_list)++ ) != NULL ) { 
	if(true == slp_join_multicast(sock, ptr) )
	  num_interfaces++ ;
      } // traversing address list 
      if(buf != NULL)
	free(buf);
    } // gethostnume succeeded
    delete[] hn;
  } // if allocated host buffer
  return(num_interfaces);
}


static SOCKET  slp_open_listen_sock( void )
{

  SOCKET sock  = socket(AF_INET, SOCK_DGRAM, 0) ;
  int err = 1;
  setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (const char *)&err, sizeof(err));
  struct sockaddr_in local;
  local.sin_family = AF_INET;
  local.sin_port = htons(427);
  local.sin_addr.s_addr  = INADDR_ANY;
  bind(sock, (struct sockaddr *)&local, sizeof(local));
  slp_join_multicast_all(sock);
  return(sock);
}


// static class members (windows only )
#ifdef _WIN32
 int slp_client::_winsock_count = 0;
 WSADATA slp_client::_wsa_data ;
#endif 


// insert new node at the back of the list 
template<class L> void slp2_list<L>::insert(L *element)
{
  slp2_list *ins = new slp2_list(false);
  ins->_rep = element;
  ins->_next = this;
  ins->_prev = this->_prev;
  this->_prev->_next = ins;
  this->_prev = ins;
  _count++;
  return;
}


template<class L> inline L *slp2_list<L>::next(L * init) 
{
  if( init == NULL )
    _cur = _next;
  else {
    assert( init = _cur->_rep );
    _cur = _cur->_next;
  }
  return(_cur->_rep);
}

template<class L> inline L *slp2_list<L>::prev( L * init  )
{
  if( init == NULL ) 
    _cur = _next;
  else {
    assert( init = _cur->rep ) ;
    _cur = _cur->_prev;
  }
  return(_cur->_rep);
}

template<class L> inline int slp2_list<L>::count(void) { return(_count) ; }

template<class L> void slp2_list<L>::empty_list(void) 
{
  while( _count > 0 ) {
    slp2_list *temp = _next;
    temp->unlink();
    if(temp->_rep != NULL)
      delete temp->_rep;
    delete temp;
    _count--;
  }
  assert(_count == 0);
  return;
}


template<class L> L *slp2_list<L>::remove( void )
{
  L *ret = NULL;
  if( _count > 0 ) {
    slp2_list *temp = _next;
    temp->unlink();
    ret = temp->_rep;
    // unhinge ret from temp so it doesn't get destroyed 
    temp->_rep = NULL ;
    delete temp;
    _count--;
  }
  return(ret);
}


template<class L> L *slp2_list<L>::remove(Sint8 *key)
{
  L *ret = NULL;
  if( _count > 0 ) {
    slp2_list *temp = _next;
    while ( temp->_isHead == false ) {
      if( temp->_rep->operator==( key )) {
	temp->unlink();
	ret = temp->_rep;
	temp->_rep = NULL;
	delete temp;
	_count--;
	break;
      }
      temp = temp->_next;
    }
  }
  return(ret);
}


template<class L> Boolean slp2_list<L>::exists(Sint8 *key)
{
  if( _count > 0) {
    slp2_list *temp = _next;
    while(temp->_isHead == false ) {
      if( temp->_rep->operator==( key ))
	return(true);
      temp = temp->_next;
    }
  }
  return(false);
}


da_list::~da_list()
{
  //  unlink();
  delete[] url;
  delete[] scope;
  delete[] attr;
  delete[] spi;
  delete[] auth;
}


Boolean da_list::operator ==(const Sint8 *key) const
{
  if( ! strcasecmp(url, key) ) 
    return(true);
  return(false);
}

rply_list::~rply_list()
{
  //  unlink();
  delete[] url;
  delete[] auth;
}


Boolean rply_list::operator ==(const Sint8 *key ) const 
{
  if (! strcasecmp(url, key) )
    return(true);
  return(false);
}


reg_list::~reg_list()
{
  delete[] url;
  delete[] attributes;
  delete[] service_type;
  delete[] scopes;
}


Boolean reg_list::operator ==(const Sint8 *key ) const
{
  if( ! strcasecmp(url, key) )
    return(true);
  return(false);
}


void slp_client::set_target_addr(const Sint8 *addr)
{
  if(addr == NULL)
    _target_addr = inet_addr("239.255.255.253") ;
  else 
    _target_addr = inet_addr(addr);
}

void slp_client::set_local_interface(const Sint8 *iface)
{

  if(iface == NULL)
    _local_addr = INADDR_ANY;
  else
    _local_addr = inet_addr(iface);
}

void slp_client::set_spi(const Sint8 *spi) 
{
  if(_spi != NULL) 
    free(_spi);
  if(spi != NULL)
    _spi = strdup(spi);
  else 
    _spi = NULL;
  return ;
}

slp_client::slp_client(const Sint8 *target_addr, 
		       const Sint8 *local_addr, 
		       Uint16 target_port, 
		       const Sint8 *spi )

  : _pr_buf_len(0), _buf_len (LSLP_MTU), _version((Uint8)1), 
    _xid(1),  _target_port(target_port), _spi(NULL), _use_das(false),
    _last_da_cycle(0), _retries(3), _ttl(255),  _convergence(1), 
    _crypto_context(NULL), das( ), replies( ), regs( )

{
  set_target_addr(target_addr);
  set_local_interface(local_addr);
  set_spi(spi);
  _pr_buf = new Sint8[LSLP_MTU];
  _msg_buf = new Sint8[LSLP_MTU] ; 
  _rcv_buf = new Sint8[LSLP_MTU] ;
  _tv.tv_sec = 1;
  _tv.tv_usec = 0;
#ifdef _WIN32
  if(_winsock_count == 0)
    WSAStartup(0x0002, &_wsa_data);
  _winsock_count++;
#endif 

  // initialize the recieve socket 
  _rcv_sock = slp_open_listen_sock( );
}


slp_client::~slp_client()
{
  // close the receive socket 


#ifdef _WIN32
  _winsock_count--;
  if(_winsock_count == 0)
    WSACleanup();
#endif 

  delete _pr_buf;;
  delete _msg_buf;
  delete _rcv_buf;
  if(_spi != NULL)
    free(_spi);
  if(_crypto_context != NULL)
    free(_crypto_context);
  das.empty_list();
  replies.empty_list();

}


void slp_client::prepare_pr_buf(const Sint8 *a)
{
  if(a == NULL)
    return;;
  if(_pr_buf_len > 0) 
    *(_pr_buf + _pr_buf_len - 1) = ',' ;
  do {
    *(_pr_buf + _pr_buf_len) = *a;
    a++;
    _pr_buf_len++;
  }while((*a != 0x00) && (_pr_buf_len < LSLP_MTU - 1)); 
  _pr_buf_len++;
}

Boolean slp_client::prepare_query( Uint16 xid,
				const Sint8 *service_type,
				const Sint8 *scopes, 
				const Sint8 *predicate  ) 
{
  Sint16 len, total_len;
  Sint8 *bptr;
  if(_msg_buf == NULL || _pr_buf == NULL)
    return(false);
  if(xid != _xid) {
    /* this is a new request */
    memset(_pr_buf, 0x00, LSLP_MTU);
    _pr_buf_len = 0;
    _xid = xid;
  }
  memset(_msg_buf, 0x00, LSLP_MTU);
  bptr = _msg_buf;
  _LSLP_SETVERSION(bptr, LSLP_PROTO_VER);
  _LSLP_SETFUNCTION(bptr, LSLP_SRVRQST);
  /* we don't know the length yet */
  _LSLP_SETFLAGS(bptr, LSLP_FLAGS_MCAST);
  _LSLP_SETXID(bptr, xid);
  _LSLP_SETLAN(bptr, LSLP_EN_US);
  bptr += ( total_len = _LSLP_HDRLEN(bptr) ) ;
    
  if(_pr_buf_len + total_len < LSLP_MTU) {
    /* set the pr list length */
    _LSLP_SETSHORT(bptr, (len = _pr_buf_len), 0);
    if(len)
      memcpy(bptr + 2, _pr_buf, len);
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
	  if(_spi == NULL)
	    len = 0;
	  else
	    len = strlen(_spi) + 1;
	      
	  if(total_len + 2 + len < LSLP_MTU) {
	    _LSLP_SETSHORT(bptr, len, 0);
		
	    if(_spi != NULL)
	      memcpy(bptr + 2, _spi, len);
		
	    total_len += ( 2 + len);
	    bptr += (2 + len);
	    assert(total_len == bptr - _msg_buf);
	    /* now go back and set the length for the entire message */
	    _LSLP_SETLENGTH(_msg_buf, total_len );
	    return(true);
	  } /* room for the spi  */
	} /*  room for predicate  */
      } /* room for the scope  */
    } /* room for the service type  */
  } /* room for the pr list  */
  return(false);
}


rply_list *slp_client::get_response( void  )
{
  
  rply_list *ret = replies.remove();
  return(ret);
}


int slp_client::find_das(const Sint8 *predicate, 
			 const Sint8 *scopes)
{
  converge_srv_req(NULL, predicate, scopes);
  time(&_last_da_cycle);
  if(0 < das.count() )
    _use_das = true;
  else
    _use_das = false;
  return( das.count( ) );
}

int slp_client::converge_srv_req( const Sint8 *type, 
				  const Sint8 *predicate, 
				  const Sint8 *scopes)
{

  Uint32 old_addr = _target_addr;
  set_target_addr( "239.255.255.253" ) ;
  srv_req( type, predicate, scopes ) ;
  _target_addr = old_addr;
  
  return( replies.count( ) );
}


void slp_client::srv_req( const Sint8 *type, 
			  const Sint8 *predicate, 
			  const Sint8 *scopes)
{
  if ((true == prepare_query( _xid + 1, type, scopes, predicate ))) {
    replies.empty_list();
    Uint16 convergence = _convergence;
    if(convergence-- > 0) { _LSLP_SETFLAGS( _msg_buf, LSLP_FLAGS_MCAST) ; }
    send_rcv_udp(  ) ;
    while(convergence-- > 0) {
      _LSLP_SLEEP( _tv.tv_usec / 1000 ) ;
      if(prepare_query( _xid, type, scopes, predicate)) {
	_LSLP_SETFLAGS(_msg_buf, LSLP_FLAGS_MCAST) ;
	send_rcv_udp( );
      }
    }
    /* if this was a multicast request try it on the local machine too  */
    if(_convergence) {
      /* save current settings  */
      Sint8 convergence_save = _convergence;
      Uint32 target_save = _target_addr;
      _convergence = 0;
      _target_addr = inet_addr("127.0.0.1" ) ;
      if ((true == prepare_query( _xid + 1, type, scopes, predicate ))) {
	_LSLP_SETFLAGS( _msg_buf, 0 );
	send_rcv_udp(  );
      }
      /* restore current settings  */
      _convergence = convergence_save;
      _target_addr = target_save;
    } /* if we need to do the query on the local machine  */
  } /* prepared query  */
  memset( _msg_buf, 0x00, LSLP_MTU);
  return ;
}

void slp_client::decode_reply( struct sockaddr_in *remote )
{
  
  if( _xid == _LSLP_GETXID( _rcv_buf ))
    prepare_pr_buf( inet_ntoa(remote->sin_addr) );

  Sint8 function = _LSLP_GETFUNCTION( _rcv_buf );
  switch(function) {
  case LSLP_DAADVERT:
    decode_daadvert( remote );
    return;		
  case LSLP_SRVRPLY:
    decode_srvrply( remote );
    return;
  case LSLP_SRVACK:
  default:
    break;
  }
  return;
}


void slp_client::decode_srvrply( struct sockaddr_in *remote )
{
  Sint8 *bptr;
  Sint16 str_len, err, count;
  Sint32 total_len, purported_len;

  bptr = _rcv_buf;
  purported_len = _LSLP_GETLENGTH(bptr);
  bptr += (total_len = _LSLP_HDRLEN(bptr));
  if(total_len < purported_len) {
    rply_list *reply = new rply_list();
    if(reply == NULL) abort();
    err = _LSLP_GETSHORT(bptr, 0);
    count = _LSLP_GETSHORT(bptr, 2);
    bptr += 4;
    total_len += 4;
    /* loop on the url entries  */
    while( ( total_len <= purported_len ) && ( count > 0 ) && reply != NULL ) {
      reply->function = LSLP_SRVRPLY;
      reply->err = err;
      reply->lifetime = _LSLP_GETSHORT(bptr, 1);
      total_len += (5 + (str_len = _LSLP_GETSHORT(bptr, 3)));
      if(total_len <= purported_len) {
	Sint8 num_auths;
	if(str_len > 0) {
	  reply->url = new Sint8[str_len + 1];
	  memcpy(reply->url, bptr + 5, str_len);
	  *((reply->url) + str_len) = 0x00;
	  bptr += (5 + str_len);
	  reply->auth_blocks = (num_auths = _LSLP_GETBYTE(bptr, 0));
	  total_len += 1;
	  bptr += 1;
	  while(num_auths && (total_len <= purported_len )) {
	    /* iterate past the authenticators for now  */
	    /* need extra code here to authenticate url entries  */
	    total_len += (str_len = _LSLP_GETSHORT(bptr, 2));
	    bptr += str_len;
	    num_auths--;
	  }
	  // handling duplicate responses is really inefficient, and we're 
	  // not helping things out by checking for dupes this late in the game
	  // however, in order to get to any further urls that may be in the
	  // reply buffer we need to unstuff it anyway. So we are optimizing
	  // for the case where there are no dupes. 
	  if( false == replies.exists(reply->url) ) {
	    strcpy(&(reply->remote[0]), inet_ntoa( remote->sin_addr )) ;
	    replies.insert(reply);
	  } else { delete reply ; }
	  count--;
	  if((total_len <= purported_len) && (count > 0) )
	    reply = new rply_list( ); 
	  else 
	    reply = NULL;
	} else { delete reply;  reply = NULL ; } /* bad packet */
      } else { delete reply;  reply = NULL ; }  /*  bad packet */
    } // while unwrapping multi-response message  
  } /* if the hdr length field is consistent with reality */
  return;
}

  
void slp_client::decode_daadvert(struct sockaddr_in *remote)
{
  Sint8 *bptr;
  Sint16 str_len;
  Sint32 total_len, purported_len;

  bptr = _rcv_buf;
  purported_len = _LSLP_GETLENGTH(bptr);
  bptr += (total_len = _LSLP_HDRLEN(bptr));
  if(total_len < purported_len) {
    da_list *adv = new da_list( );
    if(adv == NULL) abort();

    adv->function = LSLP_DAADVERT;
    adv->err = _LSLP_GETSHORT(bptr, 0);
    adv->stateless_boot = _LSLP_GETLONG(bptr, 2);
    total_len += (8 + (str_len = _LSLP_GETSHORT(bptr, 6)));
    if(total_len < purported_len) {
      /* decode and capture the url  - note: this is a string, not a url-entry structure */
      adv->url = new Sint8[str_len + 1] ;
      memcpy(adv->url, bptr + 8, str_len);
      *((adv->url) + str_len) = 0x00;
      /* advance the pointer past the url string */
      bptr += (str_len + 8);
      total_len += (2 + (str_len = _LSLP_GETSHORT(bptr, 0)));
      if(total_len < purported_len) {
	if(str_len > 0) {
	  adv->scope = new Sint8[str_len + 1] ;
	  memcpy(adv->scope, bptr + 2, str_len);
	  *((adv->scope) + str_len) = 0x00;
	}
	/* advance the pointer past the scope string  */
	bptr += (str_len + 2);
	total_len += (2 + (str_len = _LSLP_GETSHORT(bptr, 0)));
	if(total_len < purported_len) {
	  if(str_len > 0) {
	    adv->attr = new Sint8[str_len + 1] ;
	    memcpy(adv->attr, bptr + 2, str_len);
	    *((adv->attr) + str_len) = 0x00;
	  }
	  /* advance the pointer past the attr string */
	  bptr += (str_len + 2);
	  total_len += (2 + (str_len = _LSLP_GETSHORT(bptr, 0)));
	  if(total_len < purported_len) {
	    if(str_len > 0 ) {
	      adv->spi = new Sint8[str_len + 1];
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
	    da_list * exists = das.remove(adv->url);
	    delete exists;
	      
	    /* need code here to handle authenticated urls */
	    strcpy(&(adv->remote[0]), inet_ntoa(remote->sin_addr)) ;
	    das.insert(adv); 
	    return;
	  } /*  spi length field is consistent with hdr */
	} /* attr length field is consistent with hdr */
      } /*  scope length field is consistent with hdr */
    } 
  }
  return;
}


Boolean slp_client::srv_reg(Sint8 *url,
			 Sint8 *attributes,
			 Sint8 *service_type,
			 Sint8 *scopes,
			 Sint16 lifetime) 
{
  Sint32 len;
  Sint16 str_len;
  Sint8 *bptr;


  /* this is always a new request */
  memset( _pr_buf, 0x00, LSLP_MTU);
  _pr_buf_len = 0;
  _xid++ ;

  memset(_msg_buf, 0x00, LSLP_MTU);
  bptr = _msg_buf;
  _LSLP_SETVERSION(bptr, LSLP_PROTO_VER);
  _LSLP_SETFUNCTION(bptr, LSLP_SRVREG);
  /* we don't know the length yet */
  _LSLP_SETXID(bptr, _xid);
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
	  _LSLP_SETLENGTH( _msg_buf, len );
	  if(true == send_rcv_udp(  )) {
	    if(LSLP_SRVACK == _LSLP_GETFUNCTION( _rcv_buf )) {
	      if(0x0000 == _LSLP_GETSHORT( _rcv_buf, (_LSLP_HDRLEN( _rcv_buf )))) {
		memset(_msg_buf, 0x00, LSLP_MTU);
		return(true); 
	      }
	    }
	  }
	} /* attribute string fits into buffer */
      } /* scope string fits into buffer  */
    } /* service type fits into buffer  */
  } /* url fits into buffer  */
  memset( _msg_buf, 0x00, LSLP_MTU);
  return(false);
}



Boolean slp_client::send_rcv_udp( void )
{
  SOCKET sock;
  struct sockaddr_in target, local;
  Boolean ccode = false;
  if(INVALID_SOCKET != (sock = socket(AF_INET, SOCK_DGRAM, 0))) {
    int err = 1;
    setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (const char *)&err, sizeof(err) );

    local.sin_family = AF_INET;
    local.sin_port = 0;
    local.sin_addr.s_addr = _local_addr;
    if(SOCKET_ERROR != bind(sock, (struct sockaddr *)&local, sizeof(local))) {
      int bcast = ( (_LSLP_GETFLAGS(_msg_buf)) & LSLP_FLAGS_MCAST) ? 1 : 0 ;
      if(bcast) {
	if( (SOCKET_ERROR ==  _LSLP_SET_TTL(sock, _ttl) )  ||  
	    (SOCKET_ERROR == setsockopt(sock, SOL_SOCKET, SO_BROADCAST, (const Sint8 *)&bcast, sizeof(bcast)))) {
	  _LSLP_CLOSESOCKET(sock);
	  return(false);
	}
	if(_local_addr != INADDR_ANY ) {
	  struct sockaddr_in ma;
	  memset(&ma, 0x00, sizeof(ma));
	  ma.sin_addr.s_addr = _local_addr;
	  if( (SOCKET_ERROR == setsockopt(sock, IPPROTO_IP, IP_MULTICAST_IF, (const char *)&ma, sizeof(struct sockaddr_in))) ) {
	    _LSLP_CLOSESOCKET(sock);
	    return(false);
	  }
	}
      }
      target.sin_family = AF_INET;
      target.sin_port = htons(_target_port);
      target.sin_addr.s_addr = _target_addr;
      if(SOCKET_ERROR == (err = sendto(sock, 
				       _msg_buf, 
				       _LSLP_GETLENGTH(_msg_buf), 
				       0, 
				       (struct sockaddr *)&target, sizeof(target) ))) {
	_LSLP_CLOSESOCKET(sock);
	return(false);
      } /* oops - error sending data */
      if(bcast)
	service_listener_wait(_convergence, sock, false) ;
      else
	service_listener_wait(_convergence, sock, true);
    } // bound the socket 
    _LSLP_CLOSESOCKET(sock);
  } /*  got the socket */
  return(ccode);
}
     
// must be called regularly to process responses 

Sint32 slp_client::service_listener(void)
{
  return service_listener((SOCKET) 0);
}


Sint32 slp_client::service_listener_wait(time_t wait, SOCKET extra_sock, Boolean one_only)
{
  Sint32 rcv = 0;
  time_t now;
  time_t start = time(NULL);

  while( time(&now) && ((now - wait ) <= start )  ) {
    rcv += service_listener(extra_sock);
    if(rcv > 0)
      if(one_only == true)
	return(rcv);
    _LSLP_SLEEP(10);
  }
  rcv += service_listener(extra_sock);
  return(rcv);
}

Sint32 slp_client::service_listener(SOCKET extra_sock )
{

  fd_set fds;
  struct timeval tv = {0, 10}; 
  FD_ZERO(&fds);
  FD_SET(_rcv_sock, &fds);
  if(extra_sock)
    FD_SET( extra_sock, &fds);
  Sint32 err;
  do { 
    err = select(_rcv_sock > extra_sock ? _rcv_sock + 1: extra_sock + 1, &fds, NULL, NULL, &tv); 
  } while ( (err < 0 )&& (errno == EINTR)) ;
  if( 0 < err ) {
    struct sockaddr_in remote;
    int size = sizeof(remote);
    if(extra_sock && FD_ISSET(extra_sock, &fds) ) {
#ifdef PEGASUS_OS_HPUX
      err = recvfrom(extra_sock, _rcv_buf, LSLP_MTU, 0, (struct sockaddr *)&remote, &size);
#else
      err = recvfrom(extra_sock, _rcv_buf, LSLP_MTU, 0, (struct sockaddr *)&remote, (socklen_t *)&size);
#endif
      if(err && err != SOCKET_ERROR)
	decode_reply( &remote );
    }
    if(FD_ISSET(_rcv_sock, &fds)) {
#ifdef PEGASUS_OS_HPUX
      err = recvfrom(_rcv_sock, _rcv_buf, LSLP_MTU, 0, (struct sockaddr *)&remote, &size);
#else
      err = recvfrom(_rcv_sock, _rcv_buf, LSLP_MTU, 0, (struct sockaddr *)&remote, (socklen_t *)&size);
#endif
      if(err && err != SOCKET_ERROR)
	decode_reply( &remote );
    }
  }
  return(err);
}

int slp_client::srv_reg_all( Sint8 *url,
			     Sint8 *attributes,
			     Sint8 *service_type,
			     Sint8 *scopes,
			     Sint16 lifetime)
{

  assert(url != NULL && attributes != NULL && service_type != NULL && scopes != NULL);
  if(url == NULL || attributes == NULL || service_type == NULL || scopes == NULL )
    return(0);
    
  // see if we have built a cache of directory agents 
  if(  0 == das.count() ) {
    // we don't know of any directory agents - see if we can find some 
    if( 0 == find_das(NULL, scopes) )
      return(0);
  }

  // unicast a srvreg to each da
  int registrations = 0;
  Uint32 target_addr_save = _target_addr;
  int convergence_save = _convergence;
  _convergence = 0;

  da_list *da = das.next(NULL);
  while( da != NULL ) {
    set_target_addr(da->remote);
    if( true == srv_reg( url, attributes, service_type, scopes, lifetime) )
      registrations++;
    da = das.next(da);;
  }

  /* now try to register with the local host  */
  set_target_addr("127.0.0.1");
  if(true == srv_reg( url, attributes, service_type, scopes, lifetime) )
    registrations++;

  _convergence = convergence_save;
  _target_addr = target_addr_save;
  
  return(registrations);
}


PEGASUS_NAMESPACE_END
