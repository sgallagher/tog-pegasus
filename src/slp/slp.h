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

#ifndef SLP_H_INCLUDED
#define SLP_H_INCLUDED

#include <typeinfo>
#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/String.h>

#ifdef PEGASUS_OS_TYPE_WINDOWS
#include "lslp-perl-windows.h"
#endif	/* win 32 */

#ifdef PEGASUS_OS_TYPE_UNIX
#include "lslp-perl-linux.h"
#endif

#include "lslp-perl.h"

PEGASUS_NAMESPACE_BEGIN


PEGASUS_EXPORT  String slp_get_host_name(void) ;

#ifdef _WIN32
PEGASUS_EXPORT int gethostbyname_r(const char *name, 
				   struct hostent *resultbuf, 
				   char *buf, 
				   size_t bufsize, 
				   struct hostent **result, 
				   int *errno) ;
#endif
#if defined(PEGASUS_OS_HPUX) || defined(PEGASUS_OS_TRU64)
PEGASUS_EXPORT int gethostbyname_r(const char *name, 
				   struct hostent *resultbuf, 
				   char *buf, 
				   size_t bufsize, 
				   struct hostent **result, 
				   int *errno) ;
#endif

PEGASUS_EXPORT void slp_get_addr_string_from_url(const Sint8 *url, String & addr)  ;

PEGASUS_EXPORT Boolean get_addr_from_url(const Sint8 *url, struct sockaddr_in *addr , Sint8 **host) ;
PEGASUS_EXPORT void slp_get_host_string_from_url(const Sint8 *url, String & host) ;

#define DA_SRVTYPE "service:directory-agent:\0"
#define DA_SRVTYPELEN 25
#define DA_SCOPE "DEFAULT\0"
#define DA_SCOPELEN 8


#define TYPE_UNKKNOWN 0
#define TYPE_DA_LIST 1
#define TYPE_RPLY_LIST 2



template<class L> class PEGASUS_EXPORT slp2_list {

 private: 
  //  struct list_rep;
  L *_rep;
  slp2_list *_next;
  slp2_list *_prev;
  slp2_list *_cur;
  Boolean _isHead ;
  int _count;
  // unlink this node from whichever list it is on
  inline void unlink( void  ) { _prev->_next = _next; _next->_prev = _prev; }
  // insert this node into list owned by head 
  inline void insert(slp2_list & head) {  _prev = head; 
                      _next = head._next; 
		     head._next->_prev = this; 
		     head._next = this;   
                     head._count++; }
public:
  slp2_list(Boolean head = true) :  _rep(NULL), _isHead(head), _count(0) { _next = this; _prev = this; _cur = this;}
  ~slp2_list() { empty_list() ; }
  void insert(L *);
  void empty_list( void ) ;
  L *remove( void ) ;
  L *remove(Sint8 *key) ;
  L *reference(Sint8 *key);
  L *next( L * ); // poor man's iterators 
  L *prev( L * );
  Boolean exists(Sint8 *key);
  int count(void);
} ;

struct PEGASUS_EXPORT da_list
{
  ~da_list();
  da_list() : url(NULL), scope(NULL), attr(NULL), spi(NULL), auth(NULL) {}
  Boolean operator ==(const Sint8 *key) const ; 
  inline Boolean operator ==(const da_list & b) const { return (operator ==( b.url )); }
  Sint8 function;
  Uint16 err;
  Uint32 stateless_boot;
  Sint8 *url;
  Sint8 *scope;
  Sint8 *attr;
  Sint8 *spi;
  Sint8 auth_blocks;
  Sint8 *auth;
  Sint8 remote[16]; 
} ;

struct PEGASUS_EXPORT rply_list
{
  ~rply_list();
  rply_list() : url(NULL), auth(NULL) {}
  Boolean operator ==(const Sint8 *key ) const ;
  inline Boolean operator ==(const rply_list & b ) const { return (operator ==(b.url)) ; }
  Sint8 function;
  Uint16 err;
  Uint16 lifetime;
  Sint8 *url;
  Sint8 auth_blocks;
  Sint8 *auth;
  Sint8 remote[16];
} ;


struct PEGASUS_EXPORT reg_list
{
  ~reg_list();
  reg_list() : url(NULL), attributes(NULL), service_type(NULL), scopes(NULL) {} 
  reg_list(Sint8 *url, Sint8 *attributes, Sint8 *service_type, Sint8 *scopes, time_t lifetime);
  Boolean operator ==(const Sint8 *key )const;
  inline Boolean operator ==(const reg_list & b) const { return (operator ==(b.url) ) ; }
  Sint8 *url;
  Sint8 *attributes;
  Sint8 *service_type;
  Sint8 *scopes;
  time_t lifetime;
} ;

struct PEGASUS_EXPORT url_entry
{
  url_entry( Uint16 lifetime = 0xffff, 
	     Sint8 *url = NULL, 
	     Uint8 num_auths = 0,
	     Uint8 *auth_blocks = NULL);
  ~url_entry();
  Boolean operator ==(const Sint8 *key )const;
  inline Boolean operator ==(const url_entry & b) const { return (operator ==(b.url) ) ; }
  Uint16 lifetime;
  Sint16 len;
  Sint8 *url;
  Uint8 num_auths;
  Uint8 *auth_blocks;
};


class PEGASUS_EXPORT slp_client 
{

public: 
  slp_client(const Sint8 *target_addr = "239.255.255.253",
	     const Sint8 *local_addr = "0.0.0.0", 
	     Uint16 target_port = 427, 
	     const Sint8 *spi = NULL,
	     const Sint8 *scopes = "DEFAULT");
  ~slp_client();
  
  inline void set_convergence(Sint8 convergence) { _convergence = convergence ; } 
  inline void set_port(Sint16 port) { _target_port = port ; }
  void set_target_addr(const Sint8 *addr);
  void set_local_interface( const Sint8 *iface); 
  inline void set_timout_retry(Uint32 t_sec, 
			       Uint32 t_usec, 
			       Sint8 retries, 
			       Uint8 ttl) { _tv.tv_sec = t_sec; _tv.tv_usec = t_usec;
					    _retries = retries; _ttl = ttl; }

  void set_spi(const Sint8 *spi) ;
  void set_scopes(const Sint8 *scopes) ;
  rply_list *get_response( void );
  int find_das(const Sint8 *predicate, 
	       const Sint8 *scopes) ;

  void discovery_cycle ( const Sint8 *type, 
			 const Sint8 *predicate, 
			 const Sint8 *scopes) ;

  void converge_srv_req( const Sint8 *type, 
			 const Sint8 *predicate, 
			 const Sint8 *scopes) ;

  void unicast_srv_req( const Sint8 *type, 
			const Sint8 *predicate, 
			const Sint8 *scopes, 
			struct sockaddr_in *addr) ;
  void local_srv_req(const Sint8 *type, 
			 const Sint8 *predicate, 
			 const Sint8 *scopes) ;

  void srv_req( const Sint8 *type, 
		const Sint8 *predicate, 
		const Sint8 *scopes, 
		Boolean retry);

  Boolean srv_reg( Sint8 *url,
		   Sint8 *attributes,
		   Sint8 *service_type,
		   Sint8 *scopes,
		   Sint16 lifetime);
  
  int srv_reg_all( Sint8 *url,
		   Sint8 *attributes,
		   Sint8 *service_type,
		   Sint8 *scopes,
		   Sint16 lifetime) ;

  void srv_reg_local ( Sint8 *url,
		       Sint8 *attributes, 
		       Sint8 *service_type, 
		       Sint8 *scopes, 
		       Sint16 lifetime) ;

  Sint32 service_listener( void  ) ;
  
 private:
  Uint16 _pr_buf_len;
  Uint16 _buf_len;
  Sint8 _version;
  Uint16 _xid;
  Uint16 _target_port;
  Uint32 _target_addr;
  Uint32 _local_addr;
  Uint32 *_local_addr_list;
  Sint8 *_spi;
  Sint8 *_scopes;
  Sint8 *_pr_buf;
  Sint8 *_msg_buf;
  Sint8 *_rcv_buf;
  Sint8 _err_buf[255];
  Boolean _use_das;
  time_t _last_da_cycle;
  struct timeval _tv;
  int _retries;
  int _ttl;
  int  _convergence;
  void *_crypto_context;
  SOCKET _rcv_sock;
#ifdef _WIN32
  static int _winsock_count ;
  static WSADATA _wsa_data;
#endif
  slp2_list<da_list> das;
  slp2_list<rply_list> replies;
  slp2_list<reg_list> regs;

  //  void free_list( template<class L>slp2_list &list ) ;
  void prepare_pr_buf(const Sint8 *address);
  Boolean prepare_query(Uint16 xid,
		     const Sint8 *service_type,
		     const Sint8 *scopes, 
		     const Sint8 *predicate  ) ;
  void decode_msg(struct sockaddr_in *remote ) ;
  void decode_srvreq(struct sockaddr_in *remote );
  void decode_srvrply(struct sockaddr_in *remote) ;
  void decode_daadvert(struct sockaddr_in *remote) ;
  Boolean send_rcv_udp(void) ;
  Sint32 service_listener(SOCKET );
  Sint32 service_listener_wait(time_t, SOCKET, Boolean) ;
  Boolean slp_previous_responder(Sint8 *pr_list);
} ;

PEGASUS_NAMESPACE_END

#endif // slp_h_included
