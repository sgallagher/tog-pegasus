//%2003////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001, 2002  BMC Software, Hewlett-Packard Development
// Company, L. P., IBM Corp., The Open Group, Tivoli Systems.
// Copyright (c) 2003 BMC Software; Hewlett-Packard Development Company, L. P.;
// IBM Corp.; EMC Corporation, The Open Group.
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
//
//%/////////////////////////////////////////////////////////////////////////////

/*!
  \file tool.c
  \brief General tooling facility.

  This module offers general tooling methods that may be used by different
  components on the CIMOM as well as on the remote side.

  \author Frank Scheffler
*/

#include <stdio.h>
#include <dlfcn.h>
#include "tool.h"

#define GENERIC_ENTRY_POINT(n) \
        typedef CMPI##n##MI * (* GENERIC_##n##MI) ( CMPIBroker * broker, \
					            CMPIContext * ctx, \
                                                    const char * provider )
#define FIXED_ENTRY_POINT(n) \
        typedef CMPI##n##MI * (* FIXED_##n##MI) ( CMPIBroker * broker, \
					          CMPIContext * ctx )

#define LOAD_MI(n) \
        GENERIC_ENTRY_POINT(n); \
        FIXED_ENTRY_POINT(n); \
\
        CMPI##n##MI * tool_load_##n##MI ( const char * provider, \
                                          void * library, \
					  CMPIBroker * broker, \
					  CMPIContext * ctx ) \
{ \
	GENERIC_##n##MI g = \
		(GENERIC_##n##MI) \
                 __get_generic_entry_point ( library, #n ); \
\
	if ( g == NULL ) { \
		FIXED_##n##MI f = \
			(FIXED_##n##MI) \
			__get_fixed_entry_point ( provider, \
						  library, \
						  #n ); \
\
		if ( f == NULL ) return NULL; \
		return ( f ) ( broker, ctx ); \
	} \
	return ( g ) ( broker, ctx, provider ); \
}


/*****************************************************************************/

//! Loads a dynamic load library.
/*!
  Loads the libary named lib<libname>.so using dlopen().

  \param libname the library name.
  \return the library handle.
 */
void * tool_load_lib ( const char * libname )
{
  char filename[255];
  sprintf ( filename, "lib%s.so", libname );
  return dlopen ( filename, RTLD_LAZY );
}


static void * __get_generic_entry_point ( void * library,
					  const char * ptype )
{
	char entry_point[255];
	sprintf ( entry_point, "_Generic_Create_%sMI", ptype );

	return dlsym ( library, entry_point );
}


static void * __get_fixed_entry_point ( const char * provider,
					void * library,
					const char * ptype )
{
	char entry_point[255];
	sprintf ( entry_point, "%s_Create_%sMI", provider, ptype );

	return dlsym ( library, entry_point );
}


LOAD_MI(Instance);
LOAD_MI(Association);
LOAD_MI(Method);
LOAD_MI(Property);
LOAD_MI(Indication);


/****************************************************************************/

/*** Local Variables:  ***/
/*** mode: C           ***/
/*** c-basic-offset: 8 ***/
/*** End:              ***/
