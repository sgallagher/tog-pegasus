//%2005////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001, 2002 BMC Software; Hewlett-Packard Development
// Company, L.P.; IBM Corp.; The Open Group; Tivoli Systems.
// Copyright (c) 2003 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation, The Open Group.
// Copyright (c) 2004 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation; VERITAS Software Corporation; The Open Group.
// Copyright (c) 2005 Hewlett-Packard Development Company, L.P.; IBM Corp.;
// EMC Corporation; VERITAS Software Corporation; The Open Group.
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
 *  Description: scripting shell for slp_client - generates an slp ATTR Query
 *
 *  Originated: July 26, 2004
 *	Original Author: Mike Day md@soft-hackle.net
 *                       mdday@us.ibm.com
 *
 *               					                    
 *  Copyright (c) 2004  IBM                                          
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

#define SLP_LIB_IMPORT 1
#include "../slp_client/slp_client.h"

static char *url, *scopes, *tags, *addr, *_interface;
static int16 port = 427, converge ;
static BOOL dir_agent = FALSE;
static BOOL test = FALSE;
static BOOL parsable = FALSE;
static char fs='\t', rs='\n';

void free_globals(void)
{
  if(url != NULL) free(url);
  if(scopes != NULL) free(scopes);
  if(tags != NULL) free(tags);
  if(addr != NULL) free(addr);
  if(_interface != NULL) free(_interface);
  
}


void usage(void)
{
  printf("\nslp_attrreq -- transmit an SLP Attribute Request and print the results.\n");
  printf("-------------------------------------------------------------------------\n");
  printf("slp_attrreq --url=url-string\n");
  printf("         [--tags=tags-string]\n"); 
  printf("         [--scopes=scope-string]\n");
  printf("         [--address=target-IP]\n");
  printf("         [--port=target-port]\n");
  printf("         [--interface=host-IP]\n");
  printf("         [--use_da=true]\n");
  printf("         [--converge=convergence-cycles]\n");
  printf("         [--fs=field-separator]\n");
  printf("         [--rs=record-separator]\n");
  printf("\n");
  printf("All parameters must be a single string containing no spaces.\n");
  printf("Always use the format of <parameter>=<value>.\n");
  printf("Parameters enclosed in brackets are optional.\n");
    
   
}


BOOL get_options(int argc, char *argv[])
{
  int i;
  char *bptr;
  
  for(i = 1; i < argc; i++){
    if((*argv[i] == '-') && (*(argv[i] + 1) == '-')) {
      if(TRUE == lslp_pattern_match(argv[i] + 2, "url=*", FALSE)) {
	bptr = argv[i] + 2;
	while(*bptr != '=') bptr++;
	bptr++;
	/* handle case where type is specified twice on the cmd line */
	if(url != NULL)
	  free(url);
	url = strdup(bptr);
      } else if(TRUE == lslp_pattern_match(argv[i] + 2, "tags=*", FALSE)) {
	bptr = argv[i] + 2;
	while(*bptr != '=') bptr++;
	bptr++;
	if(tags != NULL)
	  free(tags);
	tags = strdup(bptr);
      } else if(TRUE == lslp_pattern_match(argv[i] + 2, "scopes*", FALSE)) {
	bptr = argv[i] + 2;
	while(*bptr != '=') bptr++;
	bptr++;
	if(scopes != NULL)
	  free(scopes);
	scopes = strdup(bptr);
	
      } else if(TRUE == lslp_pattern_match(argv[i] + 2, "address*", FALSE)) {
	bptr = argv[i] + 2;
	while(*bptr != '=') bptr++;
	bptr++;
	if(addr != NULL)
	  free(addr);
	addr = strdup(bptr);

      } else if(TRUE == lslp_pattern_match(argv[i] + 2, "port*", FALSE)) {
	bptr = argv[i] + 2;
	while(*bptr != '=') bptr++;
	bptr++;
	port = (uint16)strtoul(bptr, NULL, 10);
      } else if(TRUE == lslp_pattern_match(argv[i] + 2, "interface*", FALSE)) {
	bptr = argv[i] + 2;
	while(*bptr != '=') bptr++;
	bptr++;
	if(_interface != NULL)
	  free(_interface);
	_interface = strdup(bptr);
      } else if(TRUE == lslp_pattern_match(argv[i] + 2, "use_da=true*", FALSE)) {
	dir_agent = TRUE;
      } else if(TRUE == lslp_pattern_match(argv[i] + 2, "converge=*", FALSE)) {
	bptr = argv[i] + 2;
	while(*bptr != '=') bptr++;
	bptr++;
	converge = (uint16)strtoul(bptr, NULL, 10);
      } else if(TRUE == lslp_pattern_match(argv[i] + 2, "fs=*", FALSE)){
	bptr = argv[i] + 2;
	while(*bptr != '=') bptr++;
	bptr++;
	fs = *bptr;
	parsable=TRUE;
      } else if(TRUE == lslp_pattern_match(argv[i] + 2, "rs=*", FALSE)){
	bptr = argv[i] + 2;
	while(*bptr != '=') bptr++;
	bptr++;
	rs = *bptr;
	parsable=TRUE;
      }
    }
  }
  
  if( url == NULL )
    return FALSE;
  return TRUE;
}

int main(int argc, char **argv)
{

  struct slp_client *client;
  lslpMsg responses, *temp;
  

  if(FALSE == get_options(argc, argv)) {
    usage();
    return 0;
    
  } else {
    if(scopes == NULL)
      scopes = strdup("DEFAULT");
  
    
    if(NULL != (client = create_slp_client(addr, _interface, port, "DSA",
					   scopes, FALSE, dir_agent))) {
      
      if(addr != NULL && inet_addr(addr) == inet_addr("127.0.0.1")) {
	client->local_attr_req(client, url, scopes, tags);
      } else if(converge) {
	client->_convergence = converge ;
	client->converge_attr_req(client, url, scopes, tags);
      } else {
	SOCKADDR_IN address;
	address.sin_port = htons(port);
	address.sin_family = AF_INET;
	if(addr != NULL) {
	  address.sin_addr.s_addr = inet_addr(addr);
	  client->unicast_attr_req(client, url, scopes, tags, &address);
	}
	else {
	  address.sin_addr.s_addr = _LSLP_MCAST;
	  client->converge_attr_req(client, url, scopes, tags);
	}
      } /* end of request  */
      
      responses.isHead = TRUE;
      responses.next = responses.prev = &responses;
      
      client->get_response(client, &responses);
      while( ! _LSLP_IS_EMPTY(&responses) ) {
	temp = responses.next;
	
	if(temp->type == attrRep) {
	  if(parsable == TRUE && temp->msg.attrRep.attrListLen > 0 ) {
	    lslp_print_attr_rply_parse(temp, fs, rs);
	  } else {
	    if( temp->msg.attrRep.attrListLen > 0) {
	      
	      printf("Attr. Reply for %s\n", url);
	      lslp_print_attr_rply(temp); 
	    }
	    
	  }
	  
	}/* if we got an attr rply */ 
	_LSLP_UNLINK(temp);
	lslpDestroySLPMsg(temp, LSLP_DESTRUCTOR_DYNAMIC);	

      } /* while traversing response list */ 
      destroy_slp_client(client);
      
    } /* client successfully created */ 
    
  }
  free_globals();
  return 1 ;
 }
