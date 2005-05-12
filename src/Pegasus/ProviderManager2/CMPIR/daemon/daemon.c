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
  \file daemon.c
  \brief Remote daemon launcher.

  This program is to be run on remote host to enable them for remote
  providers. It loads predefined communication libraries and initializes
  them. Afterwards the remote daemon thread enters the cleanup procedure
  that checks for unused remote providers to be unloaded.

  \author Frank Scheffler

  \sa remote_broker.c
*/

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#ifndef PEGASUS_PLATFORM_ZOS_ZSERIES_IBM
#include <dlfcn.h>
#else
#include <dll.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#endif

#include "../native/mm.h"
#include "remote.h"
#include "native.h"

typedef int (* START_DAEMON) ();

typedef struct {
	const char * libname;
	void * hLibrary;
	CMPI_THREAD_TYPE thread;
} comm_lib;


//! List of communication libraries to be initialized.
static comm_lib __libs[] = {
	{ "CMPIRTCPCommRemote",0 ,0 },
};

int nativeSide=1;

/***************************************************************************/


//! Initializes a remote communication library.
/*!
  Opens the communication library, searches the entry points and executes
  it. The communication layer may then spawn additional listener threads,
  if necessary.

  \param comm pointer to the communication library to be loaded.
 */
static void __init_remote_comm_lib ( comm_lib * comm )
{
	void * hdl = comm->hLibrary = tool_mm_load_lib ( comm->libname );

	if ( hdl ) {
#ifndef PEGASUS_PLATFORM_ZOS_ZSERIES_IBM
		START_DAEMON fp = (START_DAEMON) dlsym ( hdl, "start_remote_daemon" );
#else
		START_DAEMON fp = (START_DAEMON) dllqueryfn ( (dllhandle*) hdl, "start_remote_daemon" );
#endif

		if ( fp ) {
			if ( fp () )
				fprintf ( stderr,
					  "Failed to initialize library." );
			return;
		}
	}
#ifndef PEGASUS_PLATFORM_ZOS_ZSERIES_IBM
	fprintf ( stderr, "%s\n", dlerror () );
#else
	fprintf( stderr, "%s\n", strerror(errno) );
#endif
}


//! Loads all communication libraries.
/*!
  The function initializes the de-/activation context for remote providers,
  then loads all the communication layers. Finally it enters the cleanup
  loop.

  \sa init_activation_context()
  \sa cleanup_remote_brokers()
 */
int main ()
{
	unsigned int i;
	CMPIContext * ctx = native_new_CMPIContext ( TOOL_MM_NO_ADD );

	init_activation_context ( ctx );

	for ( i = 0;
	      i < sizeof ( __libs ) / sizeof ( comm_lib );
	      i++ ) {

		__init_remote_comm_lib ( __libs + i );
	}

	printf ( "All remote daemons started.\n" );
	printf ( "Entering provider cleanup loop ...\n" );

	cleanup_remote_brokers ( 100, 30 );
	return 0;
}

/*** Local Variables:  ***/
/*** mode: C           ***/
/*** c-basic-offset: 8 ***/
/*** End:              ***/
