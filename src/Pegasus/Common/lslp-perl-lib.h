/*******************************************************************
 *  Description: slp client library, designed for perl-cgi
 *
 *  Originated: May 24, 2001
 *	Original Author: Mike Day, md@soft-hackle.net
 *
 *  $Header: /cvs/MSB/pegasus/src/Pegasus/Common/Attic/lslp-perl-lib.h,v 1.1 2001/06/14 22:26:45 mday Exp $
 *  $Workfile: dareg.c$
 *
 *  Copyright (C) Michael Day, 2001 
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *******************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

  typedef int BOOL ;
  typedef char int8;
  typedef unsigned char uint8 ;
  typedef short int16 ;
  typedef unsigned short uint16 ;
  typedef int int32 ;
  typedef unsigned int uint32 ;


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
} LSLP_LIB_SRVRPLY;

typedef struct lslp_lib_list {
  struct lslp_lib_list *next;
  struct lslp_lib_list *prev;
  BOOL isHead;
  int type;
} LSLP_LIB_LIST;


BOOL lslp_lib_set_convergence(const int8 *id, int8 convergence) ;
BOOL lslp_lib_set_port(const int8 *id, int16 target_port) ;
BOOL lslp_lib_set_target_addr(const int8 *id, const int8 *target_addr);
BOOL lslp_lib_set_local_interface(const int8 *id, const int8 *);
BOOL lslp_lib_set_delimitor(const int8 *id, const int8 delimitor);
BOOL lslp_lib_set_timout_retry(const int8 *id, uint32 t_sec, uint32 t_usec, int8 retries, uint8 ttl);
BOOL lslp_lib_set_spi(const int8 *id, const int8 *spi) ;
BOOL lslp_lib_init(const int8 *id, 
		   const int8 *target_addr, 
		   const int8 *local_addr, 
		   int16 target_port, 
		   const int8 *spi);
void lslp_lib_deinit(const int8 *id);
LSLP_LIB_LIST *lslp_lib_converge_srv_req(const int8 *id, 
					 const int8 *type, 
					 const int8 *predicate, 
					 const int8 *scopes) ;
LSLP_LIB_LIST  *lslp_lib_srv_req(const int8 *id,
				 const int8 *type, 
				 const int8 *predicate, 
				 const int8 *scopes);
BOOL lslp_lib_srv_reg(int8 *id, 
		      int8 *url,
		      int8 *attributes,
		      int8 *service_type,
		      int8 *scopes,
		      int16 lifetime);

void lslp_lib_free_list(LSLP_LIB_LIST *list) ;

void lslp_lib_srv_reg_all(int8 *id, 
			  int8 *url,
			  int8 *attributes,
			  int8 *service_type,
			  int8 *scopes,
			  int16 lifetime,
			  int *succeeded,
			  int *failed) ;

int8 *lslp_lib_get_host_name(void) ;

BOOL lslp_lib_get_addr_from_url(const int8 *url, struct sockaddr_in *addr ) ;
int8 *lslp_lib_get_addr_string_from_url(const int8 *url) ;

#ifdef __cplusplus
}
#endif
