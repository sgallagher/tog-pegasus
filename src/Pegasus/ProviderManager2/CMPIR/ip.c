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
//
// Author: Frank Scheffler
//
// Modified By:  Adrian Schuur (schuur@de.ibm.com)
//               Marek Szermutzky, IBM (mszermutzky@de.ibm.com)
//
//%/////////////////////////////////////////////////////////////////////////////

/*!
  \file ip.c
  \brief General TCP/IP routines.

  \author Frank Scheffler
*/

#include <stdio.h>
#include <stdlib.h>
#if defined(PEGASUS_PLATFORM_LINUX_GENERIC_GNU)
  #include <error.h>
#else
  #include "debug.h"
#endif
#ifdef PEGASUS_PLATFORM_ZOS_ZSERIES_IBM
#define _XOPEN_SOURCE_EXTENDED 1
#include <arpa/inet.h>
#endif
#include <errno.h>
#include <string.h>
#include <strings.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>


#ifndef CMPI_VER_100
#define CMPI_VER_100
#endif

#include <Pegasus/Provider/CMPI/cmpimacs.h>
#include <Pegasus/Provider/CMPI/cmpidt.h>
#include <Pegasus/Provider/CMPI/cmpift.h>

#include "ip.h"
#include "tcpcomm.h"

struct linger __linger = {
	1,
	15
};

extern CMPIBrokerExtFT *CMPI_BrokerExt_Ftab;
static int _die = 0;
/****************************************************************************/

#ifdef PEGASUS_PLATFORM_ZOS_ZSERIES_IBM
in_addr_t inet_addr_ebcdic(const char * ip_inptr)
{
    in_addr_t return_addr;
	char* ip_ptr2 = strdup(ip_inptr);
	fprintf(stderr,"--ms-->inet_addr_ebcdic(%s)\n",ip_inptr);
    __atoe(ip_ptr2);
    return_addr = inet_addr(ip_ptr2);
    free(ip_ptr2);
	fprintf(stderr,"--ms-->inet_addr_ebcdic()=%x\n",return_addr);
    return return_addr;
}
#endif

int open_connection ( const char * address, int port )
{
	int sockfd;
	struct sockaddr_in sin;
	struct hostent * server_host_name;
// masking unability to transform an ip-address via gethostbyname()
#ifdef PEGASUS_PLATFORM_ZOS_ZSERIES_IBM
	extern int h_errno;
	in_addr_t broker_ip_address;
	broker_ip_address = inet_addr_ebcdic(address);

if ( broker_ip_address != INADDR_NONE )
{
	// HERE COMES THE CALL TO GETHOSTBYADDR
	server_host_name = gethostbyaddr( &(broker_ip_address), sizeof(broker_ip_address), AF_INET);
	if (server_host_name == NULL )
	{
		error_at_line ( 0, 0, __FILE__, __LINE__,strerror(h_errno));
		return -1;
	}
} else {
#endif
	if ( ( server_host_name = gethostbyname ( address ) ) == NULL ) {
		error_at_line ( 0, 0, __FILE__, __LINE__,
#ifdef PEGASUS_PLATFORM_ZOS_ZSERIES_IBM
		strerror ( h_errno ) );
#else
				hstrerror ( h_errno ) );
#endif
		return -1;
	}
// masking end of if case for differing between ip-address and host
#ifdef PEGASUS_PLATFORM_ZOS_ZSERIES_IBM
}
#endif

	sin.sin_family      = AF_INET;
	sin.sin_port        = htons ( port );
	sin.sin_addr.s_addr =
		( (struct in_addr *) ( server_host_name->h_addr ) )->s_addr;

	if ( ( sockfd = socket ( PF_INET,
				 SOCK_STREAM,
#ifdef PEGASUS_PLATFORM_ZOS_ZSERIES_IBM
				 0 ) ) == -1 ) {
#else
				 IPPROTO_TCP ) ) == -1 ) {
#endif

		error_at_line ( 0, errno, __FILE__, __LINE__,
				"failed to create socket" );
		return -1;
	}

	setsockopt ( sockfd,
		     SOL_SOCKET,
		     SO_LINGER,
		     &__linger,
		     sizeof ( struct linger ) );

	if ( connect ( sockfd,
		       (struct sockaddr *) &sin,
		       sizeof ( sin ) ) == -1 ) {

		error_at_line ( 0, errno, __FILE__, __LINE__,
				"could not connect to %s:%d",
				address,
				port );
		return -1;
	}

	return sockfd;
}


void accept_connections ( int port,
			  void (* __connection_handler) ( int ),
			  int multithreaded )
{
	CMPI_THREAD_TYPE t;

	int in_socket, listen_socket =
#ifdef PEGASUS_PLATFORM_ZOS_ZSERIES_IBM
		socket ( PF_INET, SOCK_STREAM, 0 );
#else
		socket ( PF_INET, SOCK_STREAM, IPPROTO_TCP );
#endif
	struct sockaddr_in sin;
	int sin_len = sizeof ( sin );

	int ru = 1;

	setsockopt ( listen_socket,
		     SOL_SOCKET,
		     SO_REUSEADDR, (char *) &ru,
		     sizeof ( ru ) );

	bzero ( &sin, sin_len );

	sin.sin_family = AF_INET;
	sin.sin_addr.s_addr = INADDR_ANY;
	sin.sin_port = htons ( port );

	if ( bind ( listen_socket, (struct sockaddr *) &sin, sin_len ) ||
#ifdef PEGASUS_PLATFORM_ZOS_ZSERIES_IBM
	     listen ( listen_socket, 15 ) ) {
#else
	     listen ( listen_socket, 0 ) ) {
#endif
		error_at_line ( -1, errno, __FILE__, __LINE__, "cannot listen on port %d", port );

	}
	_die = 0;
	while ( ( in_socket = accept ( listen_socket,
				       (struct sockaddr *) &sin,
				       (size_t *) &sin_len ) ) > 0 ) {

		if (_die == 1) {
			close (in_socket);
			break;
		}
		setsockopt ( in_socket,
			     SOL_SOCKET,
			     SO_LINGER,
			     &__linger,
			     sizeof ( struct linger ) );

		if ( multithreaded ) {

                        t=CMPI_BrokerExt_Ftab->newThread(
			        (void *(*)(void *))__connection_handler,
				(void *) in_socket,1);
		} else __connection_handler ( in_socket );
	}
	if (in_socket < 0)
		{
			error_at_line ( -1, errno, __FILE__, __LINE__, "invalid socket descriptor (%d) ", in_socket);
		}
	close (listen_socket);
	listen_socket = 0;
    _die = 0;
}


int close_connection (int port ) 
{
	int socket = 0;

	_die = 1;

	// "tickle" the connection.
	socket = open_connection("127.0.0.1", port);
	if (socket)
	{
		close (socket);

		while (_die == 1)
		{
			sleep(1);
		}
	}
	return _die;
	
}
void get_peer_address ( int socket, char * buf )
{

#define UC(b)   ( ( (int) b ) & 0xFF )

	struct sockaddr_in sin;
	socklen_t sinlen = sizeof ( sin );
        char * p = (char *) &sin.sin_addr;

	getpeername ( socket,
		      (struct sockaddr *) &sin,
		      &sinlen );

        sprintf ( buf, "%d.%d.%d.%d", UC(p[0]), UC(p[1]), UC(p[2]), UC(p[3]) );
}


/****************************************************************************/

/*** Local Variables:  ***/
/*** mode: C           ***/
/*** c-basic-offset: 8 ***/
/*** End:              ***/
