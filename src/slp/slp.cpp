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
//
// Author: Mike Day (mdday@us.ibm.com)
// 
// Modified by:
//         Ramnath Ravindran (Ramnath.Ravindran@compaq.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#include <Pegasus/Common/PegasusVersion.h>
#include "slp.h"

#ifdef PEGASUS_OS_TRU64
# include <unistd.h>
//extern "C" void usleep(unsigned int);
#endif

#if defined(PEGASUS_OS_HPUX)
# include <netdb.h>
#endif

PEGASUS_USING_STD;

PEGASUS_NAMESPACE_BEGIN

// This is necessary to avoid the "reinterpret cast" warning generated
// by the HP C++ compiler.
#define SOCKADDR_IN_CAST (struct sockaddr_in*)(void*)
#define SOCKADDR_CAST (struct sockaddr*)(void*)

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

#if defined(PEGASUS_OS_HPUX) || defined(PEGASUS_OS_TRU64) || defined(PEGASUS_OS_AIX) || defined(PEGASUS_OS_OS400)

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

#ifndef PEGASUS_OS_OS400   
  if(NULL == (*result = gethostbyname(name))) {
#else
  if(NULL == (*result = gethostbyname((char *)name))) {
#endif
    *errnop = h_errno;
    return(-1);
  } 
  return(0);
}

#endif

PEGASUS_EXPORT void slp_get_addr_string_from_url(const Sint8 *url, String &addr) 
{
  Sint8 *name = NULL;
  struct sockaddr_in a;
  if( get_addr_from_url( url, &a, NULL) ) {
    name = new Sint8 [ 255 ] ;
#ifdef _WIN32
    _snprintf(name, 254, "%s:%d", inet_ntoa(a.sin_addr), ntohs(a.sin_port) );
#else

// ATTN: Mike Brasher: Hack to get built of TRU64:
#ifdef PEGASUS_OS_TRU64
    sprintf(name, "%s:%d", inet_ntoa(a.sin_addr), ntohs(a.sin_port) );
#else
    snprintf(name, 254, "%s:%d", inet_ntoa(a.sin_addr), ntohs(a.sin_port) );
#endif

#endif
    addr.clear();
    addr = name;
    delete [] name;
  }
  return ;
}

PEGASUS_EXPORT void slp_get_host_string_from_url(const Sint8 *url, String &host) 
{
  Sint8 *s;
  struct sockaddr_in addr;
  get_addr_from_url(url, &addr, &s );
  host.clear();
  host = s; 
  delete [] s;
  return ;
}

PEGASUS_EXPORT Boolean get_addr_from_url(const Sint8 *url, struct sockaddr_in *addr, Sint8 **host)
{
  Sint8 *bptr, *url_dup;
  Boolean ccode = false;

  // isolate the host field 
  bptr = (url_dup = strdup(url));
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

    bptr += 2;

    if(host != NULL) {
      *host = new Sint8[ strlen(bptr) + strlen(portptr) + 3] ;
      strcpy(*host, bptr);
      strcat(*host, ":");
      strcat(*host, portptr);
    }
    if (portptr != NULL)
      addr->sin_port = htons( (Sint16)strtoul(portptr, NULL, 0) );
    else
      addr->sin_port = 0x0000;
    addr->sin_family = AF_INET;
    

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
	while(temp != NULL && (result = gethostbyname_r(bptr, &hostbuf, 
	    (char *)temp, 
	    hostbuflen, 
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
 


static int slp_get_local_interfaces(Uint32 **list)
{
  SOCKET sock;
  int interfaces = 0;

  delete [] *list;

#ifdef PEGASUS_OS_TYPE_WINDOWS

  if ( INVALID_SOCKET != ( sock  = WSASocket(AF_INET,
					   SOCK_RAW, 0, NULL, 0, 0) ) ) {
    char *output_buf = new char[1024];
    DWORD buf_size = 1024, bytes_returned = 0;

    if ( 0 == WSAIoctl( sock, SIO_ADDRESS_LIST_QUERY, NULL, 0, 
			output_buf, buf_size, &bytes_returned, NULL, NULL) ) {
      socket_addr_list *addr_list = (socket_addr_list *)output_buf;
      *list = new Uint32 [ addr_list->count + 1 ] ;
      socket_addr *addr = addr_list->list;
      Uint32 *intp;
      sockaddr_in *sin;
      for( interfaces = 0, intp = *list, sin = (sockaddr_in *)addr ; 
	   interfaces < addr_list->count; 
	   interfaces++ , intp++  ) {
	*intp = sin->sin_addr.s_addr;
	addr++;
	sin = (sockaddr_in *)addr;
      }
      *intp = INADDR_ANY;
    }
    delete [] output_buf;
    _LSLP_CLOSESOCKET(sock);
  }
  
#else
  if( -1 < (sock = socket(AF_INET, SOCK_DGRAM, 0) ) ) {
    struct ifconf conf;

    conf.ifc_buf = new char [ 128 * sizeof(struct ifreq )  ];
    conf.ifc_len = 128 * sizeof( struct ifreq ) ;
    if( -1 < ioctl(sock, SIOCGIFCONF, &conf ) ) {
      
      // count the interfaces 


      struct ifreq *r = conf.ifc_req;
      struct sockaddr_in *addr ;
      addr = SOCKADDR_IN_CAST&r->ifr_addr;
      while(  addr->sin_addr.s_addr != 0 ) {
	interfaces++;
	r++;
	addr = SOCKADDR_IN_CAST&r->ifr_addr;
      }

      // now store the addresses

      *list  = new Uint32 [interfaces + 1 ];
      Uint32 *this_addr = *list;
      r = conf.ifc_req;
      addr = SOCKADDR_IN_CAST&r->ifr_addr;
      while(  addr->sin_addr.s_addr != 0 ) {
	*this_addr = addr->sin_addr.s_addr;
	r++;
	this_addr++;
	addr = SOCKADDR_IN_CAST&r->ifr_addr;
      }
      *this_addr = INADDR_ANY;
    } // did the ioctl 
    delete [] conf.ifc_buf;
    _LSLP_CLOSESOCKET(sock);
  } // opened the socket 

#endif 
  // failsafe if the ioctl doesn't work
  if( interfaces == 0 ) {
    *list = new Uint32 [1] ;
    *list[0] = INADDR_ANY; 
  }

  return(interfaces);
}


static Boolean  slp_join_multicast(SOCKET sock, Uint32 addr) 
{
  
  // don't join on the loopback interface
  if (addr == inet_addr("127.0.0.1") )
    return(false);

  struct ip_mreq mreq;
  mreq.imr_multiaddr.s_addr = inet_addr("239.255.255.253");
  mreq.imr_interface.s_addr = addr;

#ifndef PEGASUS_OS_OS400  
  if(SOCKET_ERROR == setsockopt(sock,IPPROTO_IP, IP_ADD_MEMBERSHIP, (const char *)&mreq, sizeof(mreq)))
#else
  if(SOCKET_ERROR == setsockopt(sock,IPPROTO_IP, IP_ADD_MEMBERSHIP, (char *)&mreq, sizeof(mreq)))
#endif 
    return(false);
  return(true);
}



static int slp_join_multicast_all(SOCKET sock)
{

  Uint32 *list = NULL , *lptr = NULL;
  int num_interfaces = slp_get_local_interfaces(&list);
  lptr = list;
  while ( *lptr != INADDR_ANY ) {
    slp_join_multicast(sock, *lptr) ;
    lptr++;
  }
  delete [] list;
  return(num_interfaces);
}


static SOCKET slp_open_listen_sock( void )
{

  SOCKET sock  = socket(AF_INET, SOCK_DGRAM, 0) ;
  int err = 1;
#ifndef PEGASUS_OS_OS400
  setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (const char *)&err, sizeof(err));
#else
  setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (char *)&err, sizeof(err));
#endif
  struct sockaddr_in local;
  local.sin_family = AF_INET;
  local.sin_port = htons(427);
  local.sin_addr.s_addr  = INADDR_ANY;
  if( 0 == bind(sock, SOCKADDR_CAST&local, sizeof(local)) )
    slp_join_multicast_all(sock);
  return(sock);
}


static Boolean slp_scope_intersection(const Sint8 *our_scopes, Sint8 *his_scopes)
{
  const Sint8 delimiters [] = " ," ;
  Sint8 *saveptr = NULL, *bptr;

  if(our_scopes == NULL || his_scopes == NULL)
    return(true);
  bptr = _LSLP_STRTOK(his_scopes, delimiters, &saveptr);
  while (NULL != bptr) {

    // convert hist scope string  upper case
    for( Sint8 *cp = bptr; *cp; ++cp) {
      if( 'a' <= *cp && *cp <= 'z' )
	*cp += 'A' - 'a' ;
    }

    if( NULL != strstr( bptr , our_scopes) )
      return(true);
    bptr = _LSLP_STRTOK(NULL, delimiters, &saveptr);
  }
  return(false);
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


template<class L> inline L *slp2_list<L>::next(const L * init) 
{
  if( init == NULL )
    _cur = _next;
  else {
    assert( (init = _cur->_rep) != 0 );
    _cur = _cur->_next;
  }
  return(_cur->_rep);
}

template<class L> inline L *slp2_list<L>::prev( const L * init  )
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


template<class L> L *slp2_list<L>::remove(const Sint8 *key)
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


template<class L> L *slp2_list<L>::reference(const Sint8 *key)
{
  if( _count > 0 ) {
    slp2_list *temp = _next;
    while(temp->_isHead == false ) {
      if( temp->_rep->operator==(key))
	return(temp->_rep);
      temp = temp->_next;
    }
  }
  return(NULL);
}

template<class L> Boolean slp2_list<L>::exists(const Sint8 *key)
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


reg_list::reg_list(const Sint8 *r_url, 
		   const Sint8 *r_attributes, 
		   const Sint8 *r_service_type, 
		   const Sint8 *r_scopes, 
		   time_t r_lifetime)
{
  if( r_url != NULL ) {
    url = new Sint8[ strlen(r_url + 1 ) ];
    strcpy(url, r_url);
  }

  if( r_attributes != NULL ) {
    attributes = new Sint8[ strlen(r_attributes) + 1 ];
    strcpy(attributes, r_attributes);
  }

  if(r_service_type != NULL ) {
    service_type = new Sint8[ strlen(r_service_type) + 1 ]; 
    strcpy(service_type, r_service_type);
  }

  if(r_scopes != NULL) {
    scopes = new Sint8[ strlen(r_scopes ) + 1 ];
    strcpy(scopes, r_scopes);
  }
  lifetime = r_lifetime;

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

url_entry::url_entry( Uint16 u_lifetime,
		      Sint8 *u_url, 
		      Uint8 u_num_auths,
		      Uint8 *u_auth_blocks)
{

  lifetime = u_lifetime;
  if(u_url != NULL && (len = strlen(u_url) )) {
    len++;
    url = new Sint8[len];
    strcpy(url, u_url);
  } else { len = 0 ; }

  num_auths = 0;
  auth_blocks = NULL;
}

url_entry::~url_entry() 
{
  delete [] url;
  delete [] auth_blocks;
}


Boolean url_entry::operator ==(const Sint8 *key) const
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
#ifndef PEGASUS_OS_OS400
    _target_addr = inet_addr(addr);
#else
    _target_addr = inet_addr((char *)addr);
#endif
}

void slp_client::set_local_interface(const Sint8 *iface)
{

  if(iface == NULL)
    _local_addr = INADDR_ANY;
  else
#ifndef PEGASUS_OS_OS400
    _local_addr = inet_addr(iface);
#else
    _local_addr = inet_addr((char *)iface);
#endif
}

void slp_client::set_spi(const Sint8 *spi) 
{
  delete [] _spi;
  if(spi != NULL && strlen(spi) ) {
    _spi = new Sint8[strlen(spi) + 1 ];
    strcpy(_spi, spi);
  }
  return ;
}

void slp_client::set_scopes(const Sint8 *scopes)
{
  delete [] _scopes;
  if( scopes != NULL && strlen(scopes) ) {
    _scopes = new Sint8[ strlen(scopes) + 1 ];
    strcpy(_scopes, scopes);
  }
  return;
}

slp_client::slp_client(const Sint8 *target_addr, 
		       const Sint8 *local_addr, 
		       Uint16 target_port, 
		       const Sint8 *spi, 
		       const Sint8 *scopes )

  : _pr_buf_len(0), _buf_len (LSLP_MTU), _version((Uint8)1),
    _xid(1),  _target_port(htons(target_port)), _local_addr_list(NULL), 
    _spi(NULL), _scopes(NULL),  _use_das(false), _last_da_cycle(0), _retries(3), 
    _ttl(255),  _convergence(5), _crypto_context(NULL), das( ), 
    replies( ), regs( )

{
  set_target_addr(target_addr);
  set_local_interface(local_addr);
  set_spi(spi);
  set_scopes(scopes);
  _pr_buf = new Sint8[LSLP_MTU];
  _msg_buf = new Sint8[LSLP_MTU] ; 
  _rcv_buf = new Sint8[LSLP_MTU] ;
  _tv.tv_sec = 0;
  _tv.tv_usec = 200000;
#ifdef _WIN32
  if(_winsock_count == 0)
    WSAStartup(0x0002, &_wsa_data);
  _winsock_count++;
#endif 

  // build our local address list
  slp_get_local_interfaces( &_local_addr_list ) ;

  // before opening the listen socket we need to see if the local machine is a da
  // if it is, don't open the listen socket
  _rcv_sock = INVALID_SOCKET;
  local_srv_req(NULL, NULL, "DEFAULT");
  if(0 < das.count() )
    _rcv_sock = INVALID_SOCKET;
  else
    _rcv_sock = slp_open_listen_sock( );
}


slp_client::~slp_client()
{
  // close the receive socket 
  _LSLP_CLOSESOCKET( _rcv_sock ) ;
#ifdef _WIN32
  _winsock_count--;
  if(_winsock_count == 0)
    WSACleanup();
#endif 

  delete [] _pr_buf;
  delete [] _msg_buf;
  delete [] _rcv_buf;
  delete [] _local_addr_list;
  delete [] _spi;
  delete [] _scopes;

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
  _LSLP_SETFLAGS(bptr, 0);
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
  
  return(replies.remove());
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


// smart interface to slp discovery. uses das if they are present, 
// convergence otherwise.
// periodically forces an active da discovery cycle 

void slp_client::discovery_cycle ( const Sint8 *type, 
				   const Sint8 *predicate, 
				   const Sint8 *scopes) 

{
  // see if we have built a cache of directory agents 
  if(  0 == das.count() ) {
    // we don't know of any directory agents - see if we need to do active da discovery
    if( ((time(NULL)) - _last_da_cycle ) > (60 * 5) )
      find_das(NULL, scopes) ;
  }

  // if there are das, unicast a srvreq to each da

  if( 0 < das.count() ) {

    da_list *da = das.next(NULL);
    struct sockaddr_in addr;
    while( da != NULL ) {
      addr.sin_port = htons(427);
      addr.sin_family = AF_INET;
      addr.sin_addr.s_addr = inet_addr(da->remote);
      unicast_srv_req(type, predicate, scopes, &addr);
      da = das.next(da);
    }
  } else {
    // do a convergence request because we don't have any das to use 

    converge_srv_req(type, predicate, scopes );
  }
  return; 
}

// this request MUST be retried <_convergence> times on EACH interface 
// regardless of how many responses we have received 
// it can be VERY time consuming but is the most thorough 
// discovery method 
void slp_client::converge_srv_req( const Sint8 *type, 
				   const Sint8 *predicate, 
				   const Sint8 *scopes)
{

  Uint32 old_target_addr = _target_addr;
  Uint32 old_local_addr = _local_addr;
  set_target_addr( "239.255.255.253" ) ;
  
  Uint32 *p_addr = _local_addr_list;
  Uint16 convergence; 
  Uint32 loopback = inet_addr("127.0.0.1");

  do {
    if( *p_addr == loopback ) {
      p_addr++;
      continue; 
    }
    _local_addr = *p_addr;
    convergence = _convergence;

    if(prepare_query( _xid + 1, type, scopes, predicate)) {
	_LSLP_SETFLAGS(_msg_buf, LSLP_FLAGS_MCAST) ;
	send_rcv_udp( );
      }

    while(--convergence > 0) {
      if(prepare_query( _xid, type, scopes, predicate)) {
	_LSLP_SETFLAGS(_msg_buf, LSLP_FLAGS_MCAST) ;
	send_rcv_udp( );
      }
    }
    p_addr++;
  }   while( *p_addr != INADDR_ANY ) ;


  // always try a local request
  local_srv_req(type, predicate, scopes);

  _target_addr = old_target_addr;
  _local_addr = old_local_addr;
  return ;
}



// this request will be retried MAX <_retries> times 
// but will always end when the first response is received
// This request is best when using a directory agent
void slp_client::unicast_srv_req( const Sint8 *type, 
				  const Sint8 *predicate, 
				  const Sint8 *scopes, 
				  struct sockaddr_in *addr )
{

  Uint32 target_addr_save, local_addr_save;
  Uint16 target_port_save;
  struct timeval tv_save;
  
  target_addr_save = _target_addr;
  local_addr_save = _local_addr;
  target_port_save = _target_port;

  tv_save.tv_sec = _tv.tv_sec;
  _tv.tv_sec = 1;
  
  // let the host decide which interface to use 
  _local_addr = INADDR_ANY; 
  _target_addr = addr->sin_addr.s_addr;
  _target_port = addr->sin_port;

  
  int retries = _retries ;
  int old_count = replies.count() ;
  srv_req(type, predicate, scopes, false) ;
  while( --retries > 0 && replies.count() == old_count ) {
    srv_req(type, predicate, scopes, true );
  }
  _target_addr = target_addr_save;
  _local_addr = local_addr_save;
  _target_port = target_port_save;
  _tv.tv_sec = tv_save.tv_sec;
  return;
}

// this request is targeted to the loopback interface, 
// and has a tiny wait timer. It should be resolved quickly. 
// It will never be retried.
void slp_client::local_srv_req( const Sint8 *type, 
				const Sint8 *predicate, 
				const Sint8 *scopes )


{

  Uint32 target_addr_save;
  struct timeval tv_save;
  
  target_addr_save = _target_addr;

  tv_save.tv_sec = _tv.tv_sec;
  tv_save.tv_usec = _tv.tv_usec;
  _tv.tv_sec = 0;
  _tv.tv_usec = 1000000;
  
  // let the host decide which interface to use 
  _local_addr = INADDR_ANY; 
  _target_addr = inet_addr("127.0.0.1");
  _target_port = htons(427);

  
  srv_req(type, predicate, scopes, false) ;

  _target_addr = target_addr_save;

  _tv.tv_sec = tv_save.tv_sec;
  _tv.tv_usec = tv_save.tv_usec;
  return;

}


// workhorse request function
void slp_client::srv_req( const Sint8 *type, 
			  const Sint8 *predicate, 
			  const Sint8 *scopes, 
			  Boolean retry )
{
  if ((true == prepare_query( (retry == true) ? _xid : _xid + 1, 
			      type, 
			      scopes, 
			      predicate ))) {
    send_rcv_udp(  ) ;
  } /* prepared query  */
  return ;
}

void slp_client::decode_msg( struct sockaddr_in *remote )
{
  
  if( _xid == _LSLP_GETXID( _rcv_buf ))
    prepare_pr_buf( inet_ntoa(remote->sin_addr) );

  Sint8 function = _LSLP_GETFUNCTION( _rcv_buf );
  switch(function) {
  case LSLP_DAADVERT:
    decode_daadvert( remote );
    return;		
  case LSLP_SRVRQST:
    decode_srvreq( remote );
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


void slp_client::decode_srvreq(struct sockaddr_in *remote )
{
  
  Sint8 *bptr;
  Sint16 str_len, err = LSLP_PARSE_ERROR ;
  Sint32 total_len, purported_len;
		    time_t current;
  
  bptr = _rcv_buf;
  purported_len = _LSLP_GETLENGTH(bptr);
  bptr += (total_len = _LSLP_HDRLEN(bptr));
  if(total_len < purported_len) {
    if( 0 < regs.count() ) {
      // advance past the slp v2 header
      // get the previous responder list 
      str_len = _LSLP_GETSHORT(bptr, 0);
      if ( (str_len + total_len + 2 < purported_len )) {
	if( false == slp_previous_responder( (str_len ? bptr + 2 : NULL ) )) {
	  Sint8 *service_type = NULL;
	  Sint8 *scopes = NULL;
	  Sint8 *spi = NULL;
	  slp2_list<url_entry> url_list ;

	  bptr += 2 + str_len;
	  total_len += 2 + str_len;
	  // extract the service type string 
	  str_len = _LSLP_GETSHORT(bptr, 0);
	  if(str_len && (str_len + total_len + 2 < purported_len )) {
	    service_type = new Sint8[str_len + 1]; // one extra byte 
	    strcpy(service_type, bptr + 2);

	    // set the error code to zero 
	    err = 0; 
	    bptr += 2 + str_len;
	    total_len += 2 + str_len;

	    // extract the scope list

	    str_len = _LSLP_GETSHORT(bptr, 0);
	    if( str_len + total_len + 2 < purported_len) {
	      if(str_len > 0 ) {
		scopes = new Sint8[str_len + 1] ; // one extra byte
		strcpy(scopes, bptr + 2);
	      }

	      // see if the requested scopes intersect with our scopes 

	      if (true == slp_scope_intersection(_scopes, scopes) ) {

		bptr += 2 + str_len;
		total_len += 2 + str_len;

		// this is a very lightweight client and currently 
		// doesn't support predicate evaluation. 
		// we will answer srvreq messages with no predicate
		// however, if there is a predicate, we can't evaluate it
		// so we won't answer it. 

		// extract the predicate
		str_len = _LSLP_GETSHORT(bptr, 0) ;
		if(str_len == 0 ) {
		  // ignore the spi for now. 
		  // look for a srvtype match 

		  reg_list *reg = regs.next(NULL);
		  while( reg != 0 ) {
		    // check the lifetime 

		    if( (current = time(NULL)) > reg->lifetime ) {
		      // this guy is stale, unlink him and start over 
		      regs.remove(reg->url);
		      delete reg;
		      reg = regs.next(NULL);
		      continue;
		    }
		    if( ! strcasecmp( reg->service_type, service_type ) ) {
		      // found a match
		      
		      url_entry *entry = new url_entry((reg->lifetime - time(NULL) ), 
						       reg->url ) ;
		      url_list.insert(entry);
		    }
		    reg = regs.next(reg);
		  } // traversing list

		} // if there is no predicate 
	      } // scopes intersect 
	    } // scope string fits
	  } // svc type string fits
	  
	  Boolean mcast = ( ((_LSLP_GETFLAGS( _rcv_buf )) & (LSLP_FLAGS_MCAST) ) ? true : false   ) ;
	  if(mcast == false || url_list.count() ) {

	    // we need to respond to this message
	    
	    _LSLP_SETVERSION(_msg_buf, LSLP_PROTO_VER);
	    _LSLP_SETFUNCTION(_msg_buf, LSLP_SRVRPLY);
	    // skip the length for now
	    _LSLP_SETFLAGS(_msg_buf, 0);
	    _LSLP_SETNEXTEXT(_msg_buf, 0, LSLP_NEXT_EX);
	    _LSLP_SETXID( _msg_buf, ( _LSLP_GETXID(_rcv_buf)  )  );
	    _LSLP_SETLAN(_msg_buf, LSLP_EN_US );
	    Sint32 msg_len = _LSLP_HDRLEN(_msg_buf);
	    Sint8 *bptr = _msg_buf + msg_len;
	    _LSLP_SETSHORT(bptr, err, 0);
	    _LSLP_SETSHORT( bptr, url_list.count() , 2 );
	    
	    bptr += 4;
	    msg_len += 4;
	    while ( 0 < url_list.count() ) {
	      url_entry *entry = url_list.remove() ;
	      assert(entry != NULL);

	      // check the length 
	      if ( (msg_len + 6 + entry->len ) <= LSLP_MTU ) {
		_LSLP_SETSHORT(bptr, entry->lifetime, 1);
		_LSLP_SETSHORT(bptr, entry->len, 3);
		memcpy(bptr + 5, entry->url, entry->len);

		bptr += (5 + entry->len );
		msg_len += (5 + entry->len); 

		// for now don't support authentication
		_LSLP_SETBYTE(bptr, 0, 0);
		bptr++;
		msg_len++;
	      
	      } else  {   
		// set the overvlow flag because we ran out of room 
		// also decrement the count of urls
		// possibly a subsequent url will fit so kee going 
		_LSLP_SETFLAGS(_msg_buf,  LSLP_FLAGS_OVERFLOW);
		Sint8 *save = bptr;
		bptr = _msg_buf + _LSLP_HDRLEN(_msg_buf) ;
		_LSLP_SETSHORT(bptr, ( _LSLP_GETSHORT(bptr, 2) - 1 ) , 2 );
		bptr = save;
	      }
	      delete entry;
	    }

	    // ok, now we can set the length
	    _LSLP_SETLENGTH(_msg_buf, msg_len );

	    // _msg_buf is stuffed with the service reply. now we need 
	    // to allocate a socket and send it back to the requesting node 
	    SOCKET sock;
	    if(INVALID_SOCKET != (sock = socket(AF_INET, SOCK_DGRAM, 0))) {
	      int err = 1;
#ifndef PEGASUS_OS_OS400
	      setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (const char *)&err, sizeof(err) );
#else
	      setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (char *)&err, sizeof(err) );
#endif
	      struct sockaddr_in local;
	      local.sin_family = AF_INET;
	      local.sin_port = _target_port ; 
	      local.sin_addr.s_addr = _local_addr;
	      if(SOCKET_ERROR != bind(sock, SOCKADDR_CAST&local, sizeof(local))) {
		sendto(sock, _msg_buf, msg_len , 0, 
		      SOCKADDR_CAST(remote), sizeof(struct sockaddr_in )) ;
	      } // successfully bound this socket 
	      _LSLP_CLOSESOCKET(sock);
	    } // successfully opened this socket
	  } // we must respond to this request 
	  delete [] spi; 
	  delete [] scopes;
	  delete [] service_type; 
	} // not on the pr list 
      } // pr list length is consistent 
    } // if there are local registrations
  } // if length is consistent
}


Boolean slp_client::srv_reg(const Sint8 *url,
			 const Sint8 *attributes,
			 const Sint8 *service_type,
			 const Sint8 *scopes,
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
	  int retries = _retries;
	  while( --retries ) {
	    if(true == send_rcv_udp(  )) {
	      if(LSLP_SRVACK == _LSLP_GETFUNCTION( _rcv_buf )) {
		if(0x0000 == _LSLP_GETSHORT( _rcv_buf, (_LSLP_HDRLEN( _rcv_buf )))) {
		  memset(_msg_buf, 0x00, LSLP_MTU);
		  return(true); 
		}
	      }
	    } // received a response 
	  } // retrying the unicast 
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
#ifndef PEGASUS_OS_OS400
    setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (const char *)&err, sizeof(err) );
#else
    setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (char *)&err, sizeof(err) );
#endif

    local.sin_family = AF_INET;
    local.sin_port = 0;
    local.sin_addr.s_addr = _local_addr;
    if(SOCKET_ERROR != bind(sock, SOCKADDR_CAST&local, sizeof(local))) {
      int bcast = ( (_LSLP_GETFLAGS(_msg_buf)) & LSLP_FLAGS_MCAST) ? 1 : 0 ;
      if(bcast) {
	if( (SOCKET_ERROR ==  _LSLP_SET_TTL(sock, _ttl) )  ||
#ifndef PEGASUS_OS_OS400  
	    (SOCKET_ERROR == setsockopt(sock, SOL_SOCKET, SO_BROADCAST, (const Sint8 *)&bcast, sizeof(bcast)))) {
#else
	    (SOCKET_ERROR == setsockopt(sock, SOL_SOCKET, SO_BROADCAST, (char *)&bcast, sizeof(bcast)))) {
#endif
	  _LSLP_CLOSESOCKET(sock);
	  return(false);
	}
	if(_local_addr != INADDR_ANY ) {
	  struct sockaddr_in ma;
	  memset(&ma, 0x00, sizeof(ma));
	  ma.sin_addr.s_addr = _local_addr;
#ifndef PEGASUS_OS_OS400
	  if( (SOCKET_ERROR == setsockopt(sock, IPPROTO_IP, IP_MULTICAST_IF, (const char *)&ma, sizeof(struct sockaddr_in))) ) {
#else
	  if( (SOCKET_ERROR == setsockopt(sock, IPPROTO_IP, IP_MULTICAST_IF, (char *)&ma, sizeof(struct sockaddr_in))) ) {
#endif
	    _LSLP_CLOSESOCKET(sock);
	    return(false);
	  }
	}
      }
      target.sin_family = AF_INET;
      target.sin_port = _target_port;
      target.sin_addr.s_addr = _target_addr;
      if(SOCKET_ERROR == (err = sendto(sock, 
				       _msg_buf, 
				       _LSLP_GETLENGTH(_msg_buf), 
				       0, 
				       SOCKADDR_CAST&target, sizeof(target) ))) {
	_LSLP_CLOSESOCKET(sock);
	return(false);
      } /* oops - error sending data */


      while ( 0 < service_listener( sock ) ) { ccode = true; }

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

#ifdef PEGASUS_OS_TRU64
    usleep(10 * 1000);
#else
    _LSLP_SLEEP(10);
#endif
  }
  rcv += service_listener(extra_sock);
  return(rcv);
}

Sint32 slp_client::service_listener(SOCKET extra_sock )
{

  struct timeval tv; 
  fd_set fds;
  FD_ZERO(&fds);
  if(_rcv_sock != INVALID_SOCKET) {
    FD_SET(_rcv_sock, &fds);
  }
  if(extra_sock)
    FD_SET( extra_sock, &fds);
  Sint32 err;
  do { 
    tv.tv_sec = _tv.tv_sec;
    tv.tv_usec = _tv.tv_usec;
    err = select(_rcv_sock > extra_sock ? _rcv_sock + 1: extra_sock + 1, &fds, NULL, NULL, &tv); 
  } while ( (err < 0 )&& (errno == EINTR)) ;
  if( 0 < err ) {
    struct sockaddr_in remote;

#if defined(PEGASUS_OS_HPUX) || defined(PEGASUS_OS_TRU64)
    int size = sizeof(remote);
#else
    socklen_t size = sizeof(remote);
#endif

    if(extra_sock && FD_ISSET(extra_sock, &fds) )
      err = recvfrom(extra_sock, _rcv_buf, LSLP_MTU, 0, SOCKADDR_CAST&remote, &size);
    if(_rcv_sock != INVALID_SOCKET) {
      if(FD_ISSET(_rcv_sock, &fds)) 
	err = recvfrom(_rcv_sock, _rcv_buf, LSLP_MTU, 0, SOCKADDR_CAST&remote, &size);
    }

    if(err && err != SOCKET_ERROR)
      decode_msg( &remote );
  } 
  if (err == SOCKET_ERROR) {
    // our interfaces could be disconnected or we could be a laptop that 
    // just got pulled from the network, etc. 
    _LSLP_CLOSESOCKET(_rcv_sock );
    if( 0 < slp_get_local_interfaces( & _local_addr_list ) ) {
      if(_rcv_sock != INVALID_SOCKET)
      	_rcv_sock = slp_open_listen_sock( );
    }
  } 
  return(err);
}

int slp_client::srv_reg_all( const Sint8 *url,
			     const Sint8 *attributes,
			     const Sint8 *service_type,
			     const Sint8 *scopes,
			     Sint16 lifetime)
{

  assert(url != NULL && attributes != NULL && service_type != NULL && scopes != NULL);
  if(url == NULL || attributes == NULL || service_type == NULL || scopes == NULL )
    return(0);
    
  // see if we have built a cache of directory agents 
  if(  0 == das.count() ) {
    // we don't know of any directory agents - see if we need to do active da discovery
    if( ((time(NULL)) - _last_da_cycle ) > (60 * 5) )
      find_das(NULL, scopes) ;
  }

  // keep track of how many times we register
  int registrations = 0;

  // save target and convergence parameters 
  Uint32 target_addr_save = _target_addr;
  int convergence_save = _convergence;
  _convergence = 0;

  // if there are das, unicast a srvreg to each da

  if( 0 < das.count() ) {

    da_list *da = das.next(NULL);
    while( da != NULL ) {
      set_target_addr(da->remote);
      if( true == srv_reg( url, attributes, service_type, scopes, lifetime) )
	registrations++;
      da = das.next(da);;
    }
  }

  // restore parameters 
  _convergence = convergence_save;
  _target_addr = target_addr_save;
  

  // if we have registered with any das, act like a service agent and cache our
  // own registration. This provides a failsafe in case the DA is not available 
  srv_reg_local(url, attributes, service_type, scopes, lifetime);
  registrations++;
  
  return(registrations);
}


void  slp_client::srv_reg_local ( const Sint8 *url,
				  const Sint8 *attributes, 
				  const Sint8 *service_type, 
				  const Sint8 *scopes, 
				  Sint16 lifetime) 
{
  reg_list *reg = NULL;
  // first see if the reg already exists. if it does, just update the lifetime
  if ( NULL != (reg = regs.reference(url))) {
    reg->lifetime = lifetime + time(NULL);
  } else {  // new reg 
    reg = new reg_list(url, attributes, service_type, scopes, (lifetime + time(NULL)));
    regs.insert(reg);
  }
  return;
}

Boolean slp_client::slp_previous_responder(Sint8 *pr_list)
{

  Sint8 *a, *s = NULL;
  Uint32 addr, *list_addr;
  if(pr_list == NULL || 0 == strlen(pr_list))
    return(false);

  a = _LSLP_STRTOK(pr_list, ",", &s);
  while(NULL != a ) {
    if(INADDR_NONE != (addr = inet_addr(a))) {
      list_addr = _local_addr_list;
      while( INADDR_ANY != *list_addr ) {
	if(*list_addr == addr)
	  return(true);
	list_addr++;
      }
    }
    a = _LSLP_STRTOK(NULL, ",", &s);
  }
  return(false);
}

PEGASUS_NAMESPACE_END
