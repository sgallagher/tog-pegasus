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
  \file proxy_comm.c
  \brief Proxy Provider communication layer handling.

  This module provides the functionality to load and manage communication
  layers, as requested by the proxy provider. These are required to
  communicate with remote providers.

  \author Frank Scheffler
*/

#include <stdio.h>
#include <stdlib.h>
#ifdef PEGASUS_PLATFORM_ZOS_ZSERIES_IBM
  #include <dll.h>
#else
#include <dlfcn.h>
#endif
#include <string.h>
#if defined(PEGASUS_PLATFORM_LINUX_GENERIC_GNU)
#include <error.h>
#endif
#include <errno.h>
#include "proxy.h"
//#include "tool.h"
#include "debug.h"

extern CMPIBrokerExtFT *CMPI_BrokerExt_Ftab;
#define INIT_LOCK(l) if (l==NULL) l=CMPI_BrokerExt_Ftab->newMutex(0);

static void * _load_lib ( const char * libname )
{
  char filename[255];
  sprintf ( filename, "lib%s.so", libname );
#ifdef PEGASUS_PLATFORM_ZOS_ZSERIES_IBM
  return dllload( filename );
#else
  return dlopen ( filename, RTLD_LAZY );
#endif
}

static void _unload_lib ( void * libhandle )
{
#ifdef PEGASUS_PLATFORM_ZOS_ZSERIES_IBM
  dllfree( (dllhandle*) libhandle );
#else
  dlclose ( libhandle );
#endif
}


//! Loads a server-side communication layer library.
/*!
  The function tries to load "lib<id>.so" looking for an entry point
  "<id>_InitCommLayer". The latter one is called to obtain a provider_comm
  structure including the function pointer table for MI calls towards
  remote providers.

  \param id the name of the comm-layer for which the library has to be loaded.
  \param broker broker handle as passed to the init function.
  \param ctx context as passed to the init function.

  \return pointer to the provider_comm structure from the comm-layer, or NULL.
 */
static provider_comm * load_comm_library ( const char * id,
					   CONST CMPIBroker * broker,
					   CONST CMPIContext * ctx )
{
	void * hLibrary;
    CMPIStatus rc = {CMPI_RC_OK, NULL};

	TRACE_VERBOSE(("entered function."));
	TRACE_NORMAL(("loading comm-layer library: lib%s.so", id));

	hLibrary = _load_lib ( id );

	if ( hLibrary != NULL ) {
		char function[255];
		INIT_COMM_LAYER fp;
		sprintf ( function, "%s_InitCommLayer", id );
#ifdef PEGASUS_PLATFORM_ZOS_ZSERIES_IBM
		fp = (INIT_COMM_LAYER) dllqueryfn ( (dllhandle *) hLibrary, function );
#else
		fp = (INIT_COMM_LAYER) dlsym ( hLibrary, function );
#endif

		if ( fp != NULL ) {
			provider_comm * result = fp ( broker, ctx );
			result->id = strdup ( id );
			result->handle = hLibrary;

			TRACE_INFO(("comm-layer successfully initialized."));
			TRACE_VERBOSE(("leaving function."));
			return result;
		}
#ifdef PEGASUS_PLATFORM_ZOS_ZSERIES_IBM
		dllfree ( (dllhandle*) hLibrary );
#else
		dlclose ( hLibrary );
#endif
	}
#ifdef PEGASUS_PLATFORM_ZOS_ZSERIES_IBM
	error_at_line ( 0, errno, __FILE__, __LINE__,
			"Unable to load/init communication-layer library.");
#else
	error_at_line ( 0, 0, __FILE__, __LINE__,
			"Unable to load/init communication-layer library: %s",
			dlerror () );
#endif
	TRACE_VERBOSE(("leaving function."));
	return NULL;
}

//! Unloads a server-side communication layer library.
/*!
  The function unloads "lib<id>.so".

  \param id comm the provider_comm structure that can be unloaded

 */
static void unload_comm_library ( provider_comm * comm )
{
	TRACE_VERBOSE(("entered function."));
	TRACE_NORMAL(("unloading comm-layer library: %s", comm->id));

	_unload_lib ( comm->handle );

	TRACE_VERBOSE(("leaving function."));
}


static provider_comm * __comm_layers = NULL;
static CMPI_MUTEX_TYPE __mutex=NULL;

//! Looks up a server-side communication layer or loads it, if necessary.
/*!
  The function maintains a list of previously loaded comm-layers locally.
  A mutex is used to ensure proper access to this list. If a comm-layer
  cannot be found within the list, it is being loaded and inserted at
  the begininng.

  \param comm_id the name of the communication layer to be looked up.
  \param broker broker handle as passed to the init function.
  \param ctx context as passed to the init function.

  \return the comm-layer matching the id or NULL if it cannot be loaded.
 */
provider_comm * load_provider_comm ( const char * comm_id,
				     CONST CMPIBroker * broker,
				     CONST CMPIContext * ctx )
{
	provider_comm * tmp;

	TRACE_VERBOSE(("entered function."));
	TRACE_NORMAL(("loading remote communication layer: %s", comm_id ));

        INIT_LOCK(__mutex);
        CMPI_BrokerExt_Ftab->lockMutex(__mutex);

	for ( tmp = __comm_layers; tmp != NULL; tmp = tmp->next ) {

		if ( strcmp ( tmp->id, comm_id ) == 0 ) {
                        CMPI_BrokerExt_Ftab->unlockMutex(__mutex);

			TRACE_INFO(("found previously loaded comm-layer."));
			TRACE_VERBOSE(("leaving function."));
			return tmp;
		}
	}

	tmp = load_comm_library ( comm_id, broker, ctx );

	if ( tmp != NULL ) {
		tmp->next     = __comm_layers;
		__comm_layers = tmp;
	}

        CMPI_BrokerExt_Ftab->unlockMutex(__mutex);

	TRACE_VERBOSE(("leaving function."));
  	return tmp;
}

//! Unloads communication layers (usually after provider terminsation.)
/*!
  The function maintains a list of previously loaded comm-layers locally.
  A mutex is used to ensure proper access to this list. If a comm-layer
  is found within the list, it is being unloaded.

  \param comm_id the name of the communication layer to be looked up.
  \param broker broker handle as passed to the init function.
  \param ctx context as passed to the cleanup function.

 */
void unload_provider_comms ( void )
{
	provider_comm * tmp;
	provider_comm * next;
	TRACE_VERBOSE(("entered function."));
	TRACE_NORMAL(("unloading remote communication layers"));

        INIT_LOCK(__mutex);
        CMPI_BrokerExt_Ftab->lockMutex(__mutex);

	for ( tmp = __comm_layers; tmp != NULL; tmp = next ) {
		TRACE_INFO(("unloading comm-layer: %s.", tmp->id));
		next = tmp->next;
	    	// IBMKR: Call the cleanup function of comm library.
	    	tmp->terminate();
		// IBMKR: free the library name.
		free (tmp->id); tmp->id = 0;
		unload_comm_library(tmp);
	}

	__comm_layers = NULL;
        CMPI_BrokerExt_Ftab->unlockMutex(__mutex);

	TRACE_VERBOSE(("leaving function."));
}


void cleanup_provider_comms ( void )
{

        CMPI_BrokerExt_Ftab->destroyMutex(__mutex);
		__mutex = 0;
}
/****************************************************************************/

/*** Local Variables:  ***/
/*** mode: C           ***/
/*** c-basic-offset: 8 ***/
/*** End:              ***/
