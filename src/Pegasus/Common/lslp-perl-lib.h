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

#include<Pegasus/Common/Config.h>

PEGASUS_USING_STD;
PEGASUS_NAMESPACE_BEGIN


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


PEGASUS_EXPORT BOOL lslp_lib_set_convergence(const int8 *id, int8 convergence) ;
PEGASUS_EXPORT BOOL lslp_lib_set_port(const int8 *id, int16 target_port) ;
PEGASUS_EXPORT BOOL lslp_lib_set_target_addr(const int8 *id, const int8 *target_addr);
PEGASUS_EXPORT BOOL lslp_lib_set_local_interface(const int8 *id, const int8 *);
PEGASUS_EXPORT BOOL lslp_lib_set_delimitor(const int8 *id, const int8 delimitor);
PEGASUS_EXPORT BOOL lslp_lib_set_timout_retry(const int8 *id, uint32 t_sec, uint32 t_usec, int8 retries, uint8 ttl);
PEGASUS_EXPORT BOOL lslp_lib_set_spi(const int8 *id, const int8 *spi) ;
PEGASUS_EXPORT BOOL lslp_lib_init(const int8 *id, 
		   const int8 *target_addr, 
		   const int8 *local_addr, 
		   int16 target_port, 
		   const int8 *spi);
PEGASUS_EXPORT  void lslp_lib_deinit(const int8 *id);
PEGASUS_EXPORT LSLP_LIB_LIST *lslp_lib_converge_srv_req(const int8 *id, 
					   const int8 *type, 
					   const int8 *predicate, 
					   const int8 *scopes) ;
PEGASUS_EXPORT LSLP_LIB_LIST  *lslp_lib_srv_req(const int8 *id,
				   const int8 *type, 
				   const int8 *predicate, 
				   const int8 *scopes);
PEGASUS_EXPORT BOOL lslp_lib_srv_reg(int8 *id, 
			int8 *url,
			int8 *attributes,
			int8 *service_type,
			int8 *scopes,
			int16 lifetime);
  
PEGASUS_EXPORT void lslp_lib_free_list(LSLP_LIB_LIST *list) ;

PEGASUS_EXPORT void lslp_lib_srv_reg_all(int8 *id, 
			  int8 *url,
			  int8 *attributes,
			  int8 *service_type,
			  int8 *scopes,
			  int16 lifetime,
			  int *succeeded,
			  int *failed) ;
PEGASUS_EXPORT int8 *lslp_lib_get_host_name(void);

PEGASUS_EXPORT BOOL lslp_lib_get_addr_from_url(const int8 *url, struct sockaddr_in *addr ) ;
PEGASUS_EXPORT int8 *lslp_lib_get_addr_string_from_url(const int8 *url) ;



PEGASUS_NAMESPACE_END
