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
  \file resolver.c
  \brief Sample resolver. 

  This is a sample resolver component, showing the general functionality
  of a resolver. The results returned upon requests issued by the proxy
  provider are hard-coded, no real lookup is yet done.

  \author Frank Scheffler
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CMPI_VERSION 90

#include <Pegasus/Provider/CMPI/cmpimacs.h>
#include <Pegasus/Provider/CMPI/cmpidt.h>
#include <Pegasus/Provider/CMPI/cmpift.h>

#include "resolver.h"
#include "debug.h"


/****************************************************************************/


static void _free_addresses ( provider_address * addr ) 
{
	if ( addr ) {
		_free_addresses ( addr->next );
		free ( addr->dst_address );
		free ( addr->provider_module );
		free ( addr );
	}
}


static provider_address * outofprocess_resolver ( CMPIBroker * broker, const char * provider )
{
	provider_address * addr;
	char *module;
        
        module=broker->xft->resolveFileName(provider);

	addr = (provider_address *) calloc ( 1, sizeof ( provider_address ) );

	addr->comm_layer_id   = "CMPIROutOfProcessComm";
	addr->dst_address     = strdup ( "0,0" ); /* uid/gid */
	addr->provider_module = strdup ( module );
	addr->destructor = _free_addresses;

	return addr;
}


static provider_address * namespace_resolver ( CMPIBroker * broker, 
        const char * provider, char *hostname)
{
	provider_address * addr;
	char *module,*pnp;

        pnp=strchr(provider,':');

	addr = (provider_address *) calloc ( 1, sizeof ( provider_address ) );

	addr->comm_layer_id   = "CMPIRTCPComm";
        addr->dst_address     = strdup ( hostname );
        if ((pnp=strchr(provider,':'))) {
	   *pnp=0;
	   addr->provider_module = strdup ( provider );
	   *pnp=':';
	}
	else addr->provider_module = strdup ( provider );
	addr->destructor = _free_addresses;

	return addr;
}


provider_address * resolve_instance ( CMPIBroker * broker,
        CMPIContext * ctx, CMPIObjectPath * cop, const char * provider, CMPIStatus * rc)
{
        CMPIStatus irc;
        char *ip;
        provider_address *a=NULL;
        
        CMPIData info=CMGetContextEntry(ctx,"CMPIRRemoteInfo",&irc);
        
        if (irc.rc==CMPI_RC_OK) {
           ip=CMGetCharsPtr(info.value.string,&irc);
           switch (ip[1]) {
           case '0':
              a=outofprocess_resolver(broker,provider);
              break;
           case '1':
              a=namespace_resolver(broker,provider,ip+3);
              break;
           }
        }
        else {
        }
         
	TRACE_NORMAL(("Resolve requested for provider: %s", provider ));
        if (rc) *rc=irc;
	return a;
}


provider_address * resolve_class ( CMPIBroker * broker,
        CMPIContext * ctx, CMPIObjectPath * cop, const char * provider, CMPIStatus * rc)
{
	TRACE_NORMAL(("Resolve requested for provider: %s", provider ));
	return resolve_instance ( broker, ctx, cop, provider, rc );
}




/****************************************************************************/

/*** Local Variables:  ***/
/*** mode: C           ***/
/*** c-basic-offset: 8 ***/
/*** End:              ***/

