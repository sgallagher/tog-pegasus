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
  \file proxy_provider.c
  \brief Proxy Provider for CMPI to access remote providers.

  Remote providers are accessed by the MB through this proxy provider.
  The resolver is used to identify the remote peers either with a class-based
  object path or an instance-based one, depending on the provider call.

  Create() calls and cleanup() calls will not be relayed to remote locations,
  i.e. remote provider will be activated and cleaned up by their remote
  brokers automatically. Thus, these calls only gather necessary information,
  such as the broker handle.

  The results from the "real" MI calls will be either passed back directly,
  in case of an instance call with exactly one peer, or as a combined
  result, in case of enumerative calls.

  \author Frank Scheffler
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <error.h>

#include "proxy.h"
#include "resolver.h"
#include "ticket.h"
#include "debug.h"

CMPIBrokerExtFT *CMPI_BrokerExt_Ftab=NULL;
int nativeSide=0;

#ifdef __GNUC__MULTI__

//! Contains the entities to be passed to an enumerative remote MI request.
struct enum_thread {
    provider_comm *comm;
    provider_address *addr;
    CMPIContext *ctx;
};

static CMPI_THREAD_TYPE spawn_enumerate_thread(provider_comm * comm,
		       provider_address * addr,
		       CMPIBroker * broker,
		       CMPIContext * ctx,
		       CMPIStatus * (*__thread) (struct enum_thread *))
{
    CMPI_THREAD_TYPE t;
    struct enum_thread *et = (struct enum_thread *)
	malloc(sizeof(struct enum_thread));

    TRACE_NORMAL(("Spawning enumerative thread for "
		   "comm-layer \"%s\", address \"%s\"",
		   comm->id, addr->dst_address));
    et->comm = comm;
    et->addr = addr;
    et->ctx = CBPrepareAttachThread(broker, ctx);
    t=CMPI_BrokerExt_Ftab->newThread( (void *(*)(void *)) __thread, et,0);
    return t;
}

#endif



static CMPIStatus __InstanceMI_cleanup(CMPIInstanceMI * cThis,
				       CMPIContext * ctx)
{
    RemoteCMPIInstanceMI *rcThis = (RemoteCMPIInstanceMI *) cThis;
    TRACE_NORMAL(("Cleaning up proxy provider handle for: %s",
		   rcThis->provider));
    if (revoke_ticket(&rcThis->ticket)) {
	TRACE_CRITICAL(("ticket could not be revoked."));
	CMReturnWithChars(rcThis->broker, CMPI_RC_ERR_FAILED,
			  "could not revoke ticket");
    };
    free(rcThis->provider);
    free(rcThis);
    CMReturn(CMPI_RC_OK);
};

static CMPIStatus __InstanceMI_enumInstanceNames(CMPIInstanceMI * cThis,
						 CMPIContext * ctx,
						 CMPIResult * rslt,
						 CMPIObjectPath * cop)
{
    CMPIStatus rc = { CMPI_RC_ERR_FAILED, NULL };
    RemoteCMPIInstanceMI *rcThis = (RemoteCMPIInstanceMI *) cThis;
    provider_address *addr, *tmp;
    TRACE_NORMAL(("Executing fan-out remote provider call " "for: %s.",
		   rcThis->provider));
    tmp = addr =
	resolve_class(rcThis->broker, ctx, cop, rcThis->provider, NULL);
    for (; addr != NULL; addr = addr->next) {
	provider_comm *comm =
	    load_provider_comm(addr->comm_layer_id, rcThis->broker, ctx);
	if (comm != NULL) {
	    rc = comm->InstanceMI_enumInstanceNames(addr, rcThis, ctx,
						    rslt, cop);
	} else {
	    tmp->destructor(tmp);
	    CMReturnWithChars(rcThis->broker, CMPI_RC_ERR_FAILED,
			      "comm-layer not found");
	}
	if (rc.rc != CMPI_RC_OK) {
	    tmp->destructor(tmp);
	    return rc;
	}
    }
    CMReturnDone(rslt);
    if (tmp)
	tmp->destructor(tmp);
    else
	CMReturnWithChars(rcThis->broker, CMPI_RC_ERR_FAILED,
			  "could not resolve location");
    return rc;
};



static CMPIStatus __InstanceMI_enumInstances(CMPIInstanceMI * cThis,
					     CMPIContext * ctx,
					     CMPIResult * rslt,
					     CMPIObjectPath * cop, char **props)
{
    CMPIStatus rc = { CMPI_RC_ERR_FAILED, NULL };
    RemoteCMPIInstanceMI *rcThis = (RemoteCMPIInstanceMI *) cThis;
    provider_address *addr, *tmp;
    TRACE_NORMAL(("Executing fan-out remote provider call " "for: %s.",
		   rcThis->provider));
    tmp = addr =
	resolve_class(rcThis->broker, ctx, cop, rcThis->provider, NULL);
    for (; addr != NULL; addr = addr->next) {
	provider_comm *comm =
	    load_provider_comm(addr->comm_layer_id, rcThis->broker, ctx);
	if (comm != NULL) {
	    rc = comm->InstanceMI_enumInstances(addr, rcThis, ctx, rslt,
						cop, props);
	} else {
	    tmp->destructor(tmp);
	    CMReturnWithChars(rcThis->broker, CMPI_RC_ERR_FAILED,
			      "comm-layer not found");
	}
	if (rc.rc != CMPI_RC_OK) {
	    tmp->destructor(tmp);
	    return rc;
	}
    }
    CMReturnDone(rslt);
    if (tmp)
	tmp->destructor(tmp);
    else
	CMReturnWithChars(rcThis->broker, CMPI_RC_ERR_FAILED,
			  "could not resolve location");
    return rc;
};





static CMPIStatus __InstanceMI_getInstance(CMPIInstanceMI * cThis,
					   CMPIContext * ctx,
					   CMPIResult * rslt,
					   CMPIObjectPath * cop, char **props)
{
    CMPIStatus rc = { CMPI_RC_ERR_FAILED, NULL };
    RemoteCMPIInstanceMI *rcThis = (RemoteCMPIInstanceMI *) cThis;
    provider_address *addr;
    provider_comm *comm;
    TRACE_NORMAL(("Executing single-targeted remote provider "
		   "call for: %s.", rcThis->provider));
    if ((addr =
	 resolve_instance(rcThis->broker, ctx, cop, rcThis->provider, NULL))) {
	comm = load_provider_comm(addr->comm_layer_id, rcThis->broker, ctx);
	if (comm != NULL) {
	    rc = comm->InstanceMI_getInstance(addr, rcThis, ctx, rslt, cop,
					      props);
	    CMReturnDone(rslt);
	}
	addr->destructor(addr);
    } else
	CMReturnWithChars(rcThis->broker, CMPI_RC_ERR_FAILED,
			  "could not resolve location");
    return rc;
};





static CMPIStatus __InstanceMI_createInstance(CMPIInstanceMI * cThis,
					      CMPIContext * ctx,
					      CMPIResult * rslt,
					      CMPIObjectPath * cop,
					      CMPIInstance * inst)
{
    CMPIStatus rc = { CMPI_RC_ERR_FAILED, NULL };
    RemoteCMPIInstanceMI *rcThis = (RemoteCMPIInstanceMI *) cThis;
    provider_address *addr;
    provider_comm *comm;
    TRACE_NORMAL(("Executing single-targeted remote provider "
		   "call for: %s.", rcThis->provider));
    if ((addr =
	 resolve_instance(rcThis->broker, ctx, cop, rcThis->provider, NULL))) {
	comm = load_provider_comm(addr->comm_layer_id, rcThis->broker, ctx);
	if (comm != NULL) {
	    rc = comm->InstanceMI_createInstance(addr, rcThis, ctx, rslt,
						 cop, inst);
	    CMReturnDone(rslt);
	}
	addr->destructor(addr);
    } else
	CMReturnWithChars(rcThis->broker, CMPI_RC_ERR_FAILED,
			  "could not resolve location");
    return rc;
};





static CMPIStatus __InstanceMI_setInstance(CMPIInstanceMI * cThis,
					   CMPIContext * ctx,
					   CMPIResult * rslt,
					   CMPIObjectPath * cop,
					   CMPIInstance * inst, char **props)
{
    CMPIStatus rc = { CMPI_RC_ERR_FAILED, NULL };
    RemoteCMPIInstanceMI *rcThis = (RemoteCMPIInstanceMI *) cThis;
    provider_address *addr;
    provider_comm *comm;
    TRACE_NORMAL(("Executing single-targeted remote provider "
		   "call for: %s.", rcThis->provider));
    if ((addr =
	 resolve_instance(rcThis->broker, ctx, cop, rcThis->provider, NULL))) {
	comm = load_provider_comm(addr->comm_layer_id, rcThis->broker, ctx);
	if (comm != NULL) {
	    rc = comm->InstanceMI_setInstance(addr, rcThis, ctx, rslt, cop,
					      inst, props);
	    CMReturnDone(rslt);
	}
	addr->destructor(addr);
    } else
	CMReturnWithChars(rcThis->broker, CMPI_RC_ERR_FAILED,
			  "could not resolve location");
    return rc;
};






static CMPIStatus __InstanceMI_deleteInstance(CMPIInstanceMI * cThis,
					      CMPIContext * ctx,
					      CMPIResult * rslt,
					      CMPIObjectPath * cop)
{
    CMPIStatus rc = { CMPI_RC_ERR_FAILED, NULL };
    RemoteCMPIInstanceMI *rcThis = (RemoteCMPIInstanceMI *) cThis;
    provider_address *addr;
    provider_comm *comm;
    TRACE_NORMAL(("Executing single-targeted remote provider "
		   "call for: %s.", rcThis->provider));
    if ((addr =
	 resolve_instance(rcThis->broker, ctx, cop, rcThis->provider, NULL))) {
	comm = load_provider_comm(addr->comm_layer_id, rcThis->broker, ctx);
	if (comm != NULL) {
	    rc = comm->InstanceMI_deleteInstance(addr, rcThis, ctx, rslt, cop);
	    CMReturnDone(rslt);
	}
	addr->destructor(addr);
    } else
	CMReturnWithChars(rcThis->broker, CMPI_RC_ERR_FAILED,
			  "could not resolve location");
    return rc;
};



static CMPIStatus __InstanceMI_execQuery(CMPIInstanceMI * cThis,
					 CMPIContext * ctx,
					 CMPIResult * rslt,
					 CMPIObjectPath * cop, char *lang,
					 char *query)
{
    CMPIStatus rc = { CMPI_RC_ERR_FAILED, NULL };
    RemoteCMPIInstanceMI *rcThis = (RemoteCMPIInstanceMI *) cThis;
    provider_address *addr, *tmp;
    TRACE_NORMAL(("Executing fan-out remote provider call " "for: %s.",
		   rcThis->provider));
    tmp = addr =
	resolve_class(rcThis->broker, ctx, cop, rcThis->provider, NULL);
    for (; addr != NULL; addr = addr->next) {
	provider_comm *comm =
	    load_provider_comm(addr->comm_layer_id, rcThis->broker, ctx);
	if (comm != NULL) {
	    rc = comm->InstanceMI_execQuery(addr, rcThis, ctx, rslt, cop,
					    lang, query);
	} else {
	    tmp->destructor(tmp);
	    CMReturnWithChars(rcThis->broker, CMPI_RC_ERR_FAILED,
			      "comm-layer not found");
	}
	if (rc.rc != CMPI_RC_OK) {
	    tmp->destructor(tmp);
	    return rc;
	}
    }
    CMReturnDone(rslt);
    if (tmp)
	tmp->destructor(tmp);
    else
	CMReturnWithChars(rcThis->broker, CMPI_RC_ERR_FAILED,
			  "could not resolve location");
    return rc;
};






CMPIInstanceMI *_Generic_Create_InstanceMI(CMPIBroker * broker,
					   CMPIContext * context,
					   const char *provider)
{
    static CMPIInstanceMIFT miFT =
	{ CMPICurrentVersion,
	  CMPICurrentVersion,
	  "Instance" "RemoteCMPI",
	__InstanceMI_cleanup,
	__InstanceMI_enumInstanceNames,
	__InstanceMI_enumInstances,
	__InstanceMI_getInstance,
	__InstanceMI_createInstance,
	__InstanceMI_setInstance,
	__InstanceMI_deleteInstance,
	__InstanceMI_execQuery
    };
    RemoteCMPIInstanceMI *mi =
	(RemoteCMPIInstanceMI *) calloc(1, sizeof(RemoteCMPIInstanceMI));
    TRACE_NORMAL(("Creating proxy provider handle for: %s", provider));

    CMPI_BrokerExt_Ftab=broker->xft;
    mi->ref.ft = &miFT;
    mi->provider = strdup(provider);
    mi->broker = broker;
    mi->ticket = generate_ticket(broker);
    return (CMPIInstanceMI *) mi;
};


static CMPIStatus __AssociationMI_cleanup(CMPIAssociationMI * cThis,
					  CMPIContext * ctx)
{
    RemoteCMPIAssociationMI *rcThis = (RemoteCMPIAssociationMI *) cThis;
    TRACE_NORMAL(("Cleaning up proxy provider handle for: %s",
		   rcThis->provider));
    if (revoke_ticket(&rcThis->ticket)) {
	TRACE_CRITICAL(("ticket could not be revoked."));
	CMReturnWithChars(rcThis->broker, CMPI_RC_ERR_FAILED,
			  "could not revoke ticket");
    };
    free(rcThis->provider);
    free(rcThis);
    CMReturn(CMPI_RC_OK);
};

static CMPIStatus __AssociationMI_associators(CMPIAssociationMI * cThis,
					      CMPIContext * ctx,
					      CMPIResult * rslt,
					      CMPIObjectPath * cop,
					      const char *assocclass,
					      const char *resultclass,
					      const char *role, const char *resultrole,
					      char **props)
{
    CMPIStatus rc = { CMPI_RC_ERR_FAILED, NULL };
    RemoteCMPIAssociationMI *rcThis = (RemoteCMPIAssociationMI *) cThis;
    provider_address *addr, *tmp;
    TRACE_NORMAL(("Executing fan-out remote provider call " "for: %s.",
		   rcThis->provider));
    tmp = addr =
	resolve_class(rcThis->broker, ctx, cop, rcThis->provider, NULL);
    for (; addr != NULL; addr = addr->next) {
	provider_comm *comm =
	    load_provider_comm(addr->comm_layer_id, rcThis->broker, ctx);
	if (comm != NULL) {
	    rc = comm->AssociationMI_associators(addr, rcThis, ctx, rslt,
						 cop, assocclass,
						 resultclass, role,
						 resultrole, props);
	} else {
	    tmp->destructor(tmp);
	    CMReturnWithChars(rcThis->broker, CMPI_RC_ERR_FAILED,
			      "comm-layer not found");
	}
	if (rc.rc != CMPI_RC_OK) {
	    tmp->destructor(tmp);
	    return rc;
	}
    }
    CMReturnDone(rslt);
    if (tmp)
	tmp->destructor(tmp);
    else
	CMReturnWithChars(rcThis->broker, CMPI_RC_ERR_FAILED,
			  "could not resolve location");
    return rc;
};


static CMPIStatus __AssociationMI_associatorNames(CMPIAssociationMI *
						  cThis, CMPIContext * ctx,
						  CMPIResult * rslt,
						  CMPIObjectPath * cop,
						  const char *assocclass,
						  const char *resultclass,
						  const char *role, const char *resultrole)
{
    CMPIStatus rc = { CMPI_RC_ERR_FAILED, NULL };
    RemoteCMPIAssociationMI *rcThis = (RemoteCMPIAssociationMI *) cThis;
    provider_address *addr, *tmp;
    TRACE_NORMAL(("Executing fan-out remote provider call " "for: %s.",
		   rcThis->provider));
    tmp = addr =
	resolve_class(rcThis->broker, ctx, cop, rcThis->provider, NULL);
    for (; addr != NULL; addr = addr->next) {
	provider_comm *comm =
	    load_provider_comm(addr->comm_layer_id, rcThis->broker, ctx);
	if (comm != NULL) {
	    rc = comm->AssociationMI_associatorNames(addr, rcThis, ctx,
						     rslt, cop, assocclass,
						     resultclass, role,
						     resultrole);
	} else {
	    tmp->destructor(tmp);
	    CMReturnWithChars(rcThis->broker, CMPI_RC_ERR_FAILED,
			      "comm-layer not found");
	}
	if (rc.rc != CMPI_RC_OK) {
	    tmp->destructor(tmp);
	    return rc;
	}
    }
    CMReturnDone(rslt);
    if (tmp)
	tmp->destructor(tmp);
    else
	CMReturnWithChars(rcThis->broker, CMPI_RC_ERR_FAILED,
			  "could not resolve location");
    return rc;
};


static CMPIStatus __AssociationMI_references(CMPIAssociationMI * cThis,
					     CMPIContext * ctx,
					     CMPIResult * rslt,
					     CMPIObjectPath * cop,
					     const char *assocclass, const char *role,
					     char **props)
{
    CMPIStatus rc = { CMPI_RC_ERR_FAILED, NULL };
    RemoteCMPIAssociationMI *rcThis = (RemoteCMPIAssociationMI *) cThis;
    provider_address *addr, *tmp;
    TRACE_NORMAL(("Executing fan-out remote provider call " "for: %s.",
		   rcThis->provider));
    tmp = addr =
	resolve_class(rcThis->broker, ctx, cop, rcThis->provider, NULL);
    for (; addr != NULL; addr = addr->next) {
	provider_comm *comm =
	    load_provider_comm(addr->comm_layer_id, rcThis->broker, ctx);
	if (comm != NULL) {
	    rc = comm->AssociationMI_references(addr, rcThis, ctx, rslt,
						cop, assocclass, role, props);
	} else {
	    tmp->destructor(tmp);
	    CMReturnWithChars(rcThis->broker, CMPI_RC_ERR_FAILED,
			      "comm-layer not found");
	}
	if (rc.rc != CMPI_RC_OK) {
	    tmp->destructor(tmp);
	    return rc;
	}
    }
    CMReturnDone(rslt);
    if (tmp)
	tmp->destructor(tmp);
    else
	CMReturnWithChars(rcThis->broker, CMPI_RC_ERR_FAILED,
			  "could not resolve location");
    return rc;
};







static CMPIStatus __AssociationMI_referenceNames(CMPIAssociationMI * cThis,
						 CMPIContext * ctx,
						 CMPIResult * rslt,
						 CMPIObjectPath * cop,
						 const char *assocclass, const char *role)
{
    CMPIStatus rc = { CMPI_RC_ERR_FAILED, NULL };
    RemoteCMPIAssociationMI *rcThis = (RemoteCMPIAssociationMI *) cThis;
    provider_address *addr, *tmp;
    TRACE_NORMAL(("Executing fan-out remote provider call " "for: %s.",
		   rcThis->provider));
    tmp = addr =
	resolve_class(rcThis->broker, ctx, cop, rcThis->provider, NULL);
    for (; addr != NULL; addr = addr->next) {
	provider_comm *comm =
	    load_provider_comm(addr->comm_layer_id, rcThis->broker, ctx);
	if (comm != NULL) {
	    rc = comm->AssociationMI_referenceNames(addr, rcThis, ctx,
						    rslt, cop, assocclass,
						    role);
	} else {
	    tmp->destructor(tmp);
	    CMReturnWithChars(rcThis->broker, CMPI_RC_ERR_FAILED,
			      "comm-layer not found");
	}
	if (rc.rc != CMPI_RC_OK) {
	    tmp->destructor(tmp);
	    return rc;
	}
    }
    CMReturnDone(rslt);
    if (tmp)
	tmp->destructor(tmp);
    else
	CMReturnWithChars(rcThis->broker, CMPI_RC_ERR_FAILED,
			  "could not resolve location");
    return rc;
};






CMPIAssociationMI *_Generic_Create_AssociationMI(CMPIBroker * broker,
						 CMPIContext * context,
						 const char *provider)
{
    static CMPIAssociationMIFT miFT = { CMPICurrentVersion, CMPICurrentVersion,
	"Association" "RemoteCMPI", __AssociationMI_cleanup,
	    __AssociationMI_associators,
	__AssociationMI_associatorNames, __AssociationMI_references,
	__AssociationMI_referenceNames
    };
    RemoteCMPIAssociationMI *mi = (RemoteCMPIAssociationMI *) calloc(1,
								     sizeof
								     (RemoteCMPIAssociationMI));
    TRACE_NORMAL(("Creating proxy provider handle for: %s", provider));

    CMPI_BrokerExt_Ftab=broker->xft;
    mi->ref.ft = &miFT;
    mi->provider = strdup(provider);
    mi->broker = broker;
    mi->ticket = generate_ticket(broker);
    return (CMPIAssociationMI *) mi;
};


static CMPIStatus __MethodMI_cleanup(CMPIMethodMI * cThis, CMPIContext * ctx)
{
    RemoteCMPIMethodMI *rcThis = (RemoteCMPIMethodMI *) cThis;
    TRACE_NORMAL(("Cleaning up proxy provider handle for: %s",
		   rcThis->provider));
    if (revoke_ticket(&rcThis->ticket)) {
	TRACE_CRITICAL(("ticket could not be revoked."));
	CMReturnWithChars(rcThis->broker, CMPI_RC_ERR_FAILED,
			  "could not revoke ticket");
    };
    free(rcThis->provider);
    free(rcThis);
    CMReturn(CMPI_RC_OK);
};

static CMPIStatus __MethodMI_invokeMethod(CMPIMethodMI * cThis,
					  CMPIContext * ctx,
					  CMPIResult * rslt,
					  CMPIObjectPath * cop,
					  const char *method, CMPIArgs * in,
					  CMPIArgs * out)
{
    CMPIStatus rc = { CMPI_RC_ERR_FAILED, NULL };
    RemoteCMPIMethodMI *rcThis = (RemoteCMPIMethodMI *) cThis;
    provider_address *addr;
    provider_comm *comm;
    TRACE_NORMAL(("Executing single-targeted remote provider "
		   "call for: %s.", rcThis->provider));
    if ((addr =
	 resolve_instance(rcThis->broker, ctx, cop, rcThis->provider, NULL))) {
	comm = load_provider_comm(addr->comm_layer_id, rcThis->broker, ctx);
	if (comm != NULL) {
	    rc = comm->MethodMI_invokeMethod(addr, rcThis, ctx, rslt, cop,
					     method, in, out);
	    CMReturnDone(rslt);
	}
	addr->destructor(addr);
    } else
	CMReturnWithChars(rcThis->broker, CMPI_RC_ERR_FAILED,
			  "could not resolve location");
    return rc;
};







CMPIMethodMI *_Generic_Create_MethodMI(CMPIBroker * broker,
				       CMPIContext * context,
				       const char *provider)
{
    static CMPIMethodMIFT miFT =
	{ CMPICurrentVersion, CMPICurrentVersion, "Method" "RemoteCMPI",
	__MethodMI_cleanup, __MethodMI_invokeMethod
    };
    RemoteCMPIMethodMI *mi =
	(RemoteCMPIMethodMI *) calloc(1, sizeof(RemoteCMPIMethodMI));
    TRACE_NORMAL(("Creating proxy provider handle for: %s", provider));

    CMPI_BrokerExt_Ftab=broker->xft;
    mi->ref.ft = &miFT;
    mi->provider = strdup(provider);
    mi->broker = broker;
    mi->ticket = generate_ticket(broker);
    return (CMPIMethodMI *) mi;
};


static CMPIStatus __PropertyMI_cleanup(CMPIPropertyMI * cThis,
				       CMPIContext * ctx)
{
    RemoteCMPIPropertyMI *rcThis = (RemoteCMPIPropertyMI *) cThis;
    TRACE_NORMAL(("Cleaning up proxy provider handle for: %s",
		   rcThis->provider));
    if (revoke_ticket(&rcThis->ticket)) {
	TRACE_CRITICAL(("ticket could not be revoked."));
	CMReturnWithChars(rcThis->broker, CMPI_RC_ERR_FAILED,
			  "could not revoke ticket");
    };
    free(rcThis->provider);
    free(rcThis);
    CMReturn(CMPI_RC_OK);
};

static CMPIStatus __PropertyMI_setProperty(CMPIPropertyMI * cThis,
					   CMPIContext * ctx,
					   CMPIResult * rslt,
					   CMPIObjectPath * cop,
					   const char *name, CMPIData data)
{
    CMPIStatus rc = { CMPI_RC_ERR_FAILED, NULL };
    RemoteCMPIPropertyMI *rcThis = (RemoteCMPIPropertyMI *) cThis;
    provider_address *addr;
    provider_comm *comm;
    TRACE_NORMAL(("Executing single-targeted remote provider "
		   "call for: %s.", rcThis->provider));
    if ((addr =
	 resolve_instance(rcThis->broker, ctx, cop, rcThis->provider, NULL))) {
	comm = load_provider_comm(addr->comm_layer_id, rcThis->broker, ctx);
	if (comm != NULL) {
	    rc = comm->PropertyMI_setProperty(addr, rcThis, ctx, rslt, cop,
					      name, data);
	    CMReturnDone(rslt);
	}
	addr->destructor(addr);
    } else
	CMReturnWithChars(rcThis->broker, CMPI_RC_ERR_FAILED,
			  "could not resolve location");
    return rc;
};






static CMPIStatus __PropertyMI_getProperty(CMPIPropertyMI * cThis,
					   CMPIContext * ctx,
					   CMPIResult * rslt,
					   CMPIObjectPath * cop, const char *name)
{
    CMPIStatus rc = { CMPI_RC_ERR_FAILED, NULL };
    RemoteCMPIPropertyMI *rcThis = (RemoteCMPIPropertyMI *) cThis;
    provider_address *addr;
    provider_comm *comm;
    TRACE_NORMAL(("Executing single-targeted remote provider "
		   "call for: %s.", rcThis->provider));
    if ((addr =
	 resolve_instance(rcThis->broker, ctx, cop, rcThis->provider, NULL))) {
	comm = load_provider_comm(addr->comm_layer_id, rcThis->broker, ctx);
	if (comm != NULL) {
	    rc = comm->PropertyMI_getProperty(addr, rcThis, ctx, rslt, cop,
					      name);
	    CMReturnDone(rslt);
	}
	addr->destructor(addr);
    } else
	CMReturnWithChars(rcThis->broker, CMPI_RC_ERR_FAILED,
			  "could not resolve location");
    return rc;
};





CMPIPropertyMI *_Generic_Create_PropertyMI(CMPIBroker * broker,
					   CMPIContext * context,
					   const char *provider)
{
    static CMPIPropertyMIFT miFT =
	{ CMPICurrentVersion, CMPICurrentVersion, "Property" "RemoteCMPI",
	__PropertyMI_cleanup, __PropertyMI_setProperty, __PropertyMI_getProperty
    };
    RemoteCMPIPropertyMI *mi =
	(RemoteCMPIPropertyMI *) calloc(1, sizeof(RemoteCMPIPropertyMI));
    TRACE_NORMAL(("Creating proxy provider handle for: %s", provider));

    CMPI_BrokerExt_Ftab=broker->xft;
    mi->ref.ft = &miFT;
    mi->provider = strdup(provider);
    mi->broker = broker;
    mi->ticket = generate_ticket(broker);
    return (CMPIPropertyMI *) mi;
};


static CMPIStatus __IndicationMI_cleanup(CMPIIndicationMI * cThis,
					 CMPIContext * ctx)
{
    RemoteCMPIIndicationMI *rcThis = (RemoteCMPIIndicationMI *) cThis;
    TRACE_NORMAL(("Cleaning up proxy provider handle for: %s",
		   rcThis->provider));
    if (revoke_ticket(&rcThis->ticket)) {
	TRACE_CRITICAL(("ticket could not be revoked."));
	CMReturnWithChars(rcThis->broker, CMPI_RC_ERR_FAILED,
			  "could not revoke ticket");
    };
    free(rcThis->provider);
    free(rcThis);
    CMReturn(CMPI_RC_OK);
};

static CMPIStatus __IndicationMI_authorizeFilter(CMPIIndicationMI * cThis,
						 CMPIContext * ctx,
						 CMPIResult * rslt,
						 CMPISelectExp * filter,
						 const char *indType,
						 CMPIObjectPath * cop,
						 const char *owner)
{
    CMPIStatus rc = { CMPI_RC_ERR_FAILED, NULL };
    RemoteCMPIIndicationMI *rcThis = (RemoteCMPIIndicationMI *) cThis;
    provider_address *addr, *tmp;
    TRACE_NORMAL(("Executing fan-out remote provider call " "for: %s.",
		   rcThis->provider));
    tmp = addr =
	resolve_class(rcThis->broker, ctx, cop, rcThis->provider, NULL);
    for (; addr != NULL; addr = addr->next) {
	provider_comm *comm =
	    load_provider_comm(addr->comm_layer_id, rcThis->broker, ctx);
	if (comm != NULL) {
	    rc = comm->IndicationMI_authorizeFilter(addr, rcThis, ctx,
						    rslt, cop, filter,
						    indType, owner);
	} else {
	    tmp->destructor(tmp);
	    CMReturnWithChars(rcThis->broker, CMPI_RC_ERR_FAILED,
			      "comm-layer not found");
	}
	if (rc.rc != CMPI_RC_OK) {
	    tmp->destructor(tmp);
	    return rc;
	}
    }
    CMReturnDone(rslt);
    if (tmp)
	tmp->destructor(tmp);
    else
	CMReturnWithChars(rcThis->broker, CMPI_RC_ERR_FAILED,
			  "could not resolve location");
    return rc;
};







static CMPIStatus __IndicationMI_mustPoll(CMPIIndicationMI * cThis,
					  CMPIContext * ctx,
					  CMPIResult * rslt,
					  CMPISelectExp * filter,
					  const char *indType, CMPIObjectPath * cop)
{
    CMPIStatus rc = { CMPI_RC_ERR_FAILED, NULL };
    RemoteCMPIIndicationMI *rcThis = (RemoteCMPIIndicationMI *) cThis;
    provider_address *addr, *tmp;
    TRACE_NORMAL(("Executing fan-out remote provider call " "for: %s.",
		   rcThis->provider));
    tmp = addr =
	resolve_class(rcThis->broker, ctx, cop, rcThis->provider, NULL);
    for (; addr != NULL; addr = addr->next) {
	provider_comm *comm =
	    load_provider_comm(addr->comm_layer_id, rcThis->broker, ctx);
	if (comm != NULL) {
	    rc = comm->IndicationMI_mustPoll(addr, rcThis, ctx, rslt, cop,
					     filter, indType);
	} else {
	    tmp->destructor(tmp);
	    CMReturnWithChars(rcThis->broker, CMPI_RC_ERR_FAILED,
			      "comm-layer not found");
	}
	if (rc.rc != CMPI_RC_OK) {
	    tmp->destructor(tmp);
	    return rc;
	}
    }
    CMReturnDone(rslt);
    if (tmp)
	tmp->destructor(tmp);
    else
	CMReturnWithChars(rcThis->broker, CMPI_RC_ERR_FAILED,
			  "could not resolve location");
    return rc;
};






static CMPIStatus __IndicationMI_activateFilter(CMPIIndicationMI * cThis,
						CMPIContext * ctx,
						CMPIResult * rslt,
						CMPISelectExp * filter,
						const char *indType,
						CMPIObjectPath * cop,
						CMPIBoolean firstActivation)
{
    CMPIStatus rc = { CMPI_RC_ERR_FAILED, NULL };
    RemoteCMPIIndicationMI *rcThis = (RemoteCMPIIndicationMI *) cThis;
    provider_address *addr, *tmp;
    TRACE_NORMAL(("Executing fan-out remote provider call " "for: %s.",
		   rcThis->provider));
    tmp = addr =
	resolve_class(rcThis->broker, ctx, cop, rcThis->provider, NULL);
    for (; addr != NULL; addr = addr->next) {
	provider_comm *comm =
	    load_provider_comm(addr->comm_layer_id, rcThis->broker, ctx);
	if (comm != NULL) {
	    rc = comm->IndicationMI_activateFilter(addr, rcThis, ctx, rslt,
						   cop, filter, indType,
						   firstActivation);
	} else {
	    tmp->destructor(tmp);
	    CMReturnWithChars(rcThis->broker, CMPI_RC_ERR_FAILED,
			      "comm-layer not found");
	}
	if (rc.rc != CMPI_RC_OK) {
	    tmp->destructor(tmp);
	    return rc;
	}
    }
    CMReturnDone(rslt);
    if (tmp)
	tmp->destructor(tmp);
    else
	CMReturnWithChars(rcThis->broker, CMPI_RC_ERR_FAILED,
			  "could not resolve location");
    return rc;
};







static CMPIStatus __IndicationMI_deActivateFilter(CMPIIndicationMI * cThis,
						  CMPIContext * ctx,
						  CMPIResult * rslt,
						  CMPISelectExp * filter,
						  const char *indType,
						  CMPIObjectPath * cop,
						  CMPIBoolean lastActivation)
{
    CMPIStatus rc = { CMPI_RC_ERR_FAILED, NULL };
    RemoteCMPIIndicationMI *rcThis = (RemoteCMPIIndicationMI *) cThis;
    provider_address *addr, *tmp;
    TRACE_NORMAL(("Executing fan-out remote provider call " "for: %s.",
		   rcThis->provider));
    tmp = addr =
	resolve_class(rcThis->broker, ctx, cop, rcThis->provider, NULL);
    for (; addr != NULL; addr = addr->next) {
	provider_comm *comm =
	    load_provider_comm(addr->comm_layer_id, rcThis->broker, ctx);
	if (comm != NULL) {
	    rc = comm->IndicationMI_deActivateFilter(addr, rcThis, ctx,
						     rslt, cop, filter,
						     indType, lastActivation);
	} else {
	    tmp->destructor(tmp);
	    CMReturnWithChars(rcThis->broker, CMPI_RC_ERR_FAILED,
			      "comm-layer not found");
	}
	if (rc.rc != CMPI_RC_OK) {
	    tmp->destructor(tmp);
	    return rc;
	}
    }
    CMReturnDone(rslt);
    if (tmp)
	tmp->destructor(tmp);
    else
	CMReturnWithChars(rcThis->broker, CMPI_RC_ERR_FAILED,
			  "could not resolve location");
    return rc;
};


static void __IndicationMI_enableIndications(CMPIIndicationMI * cThis)
{
    TRACE_NORMAL(("enableIndications ignored"));
}


static void __IndicationMI_disableIndications(CMPIIndicationMI * cThis)
{
    TRACE_NORMAL(("disableIndications ignored"));
}




CMPIIndicationMI *_Generic_Create_IndicationMI(CMPIBroker * broker,
					       CMPIContext * context,
					       const char *provider)
{
    static CMPIIndicationMIFT miFT = { CMPICurrentVersion, CMPICurrentVersion,
	"Indication" "RemoteCMPI", __IndicationMI_cleanup,
	    __IndicationMI_authorizeFilter,
	    __IndicationMI_mustPoll,
	    __IndicationMI_activateFilter,
	    __IndicationMI_deActivateFilter,
	    __IndicationMI_enableIndications,
            __IndicationMI_disableIndications,
    };
    RemoteCMPIIndicationMI *mi = (RemoteCMPIIndicationMI *) calloc(1,
								   sizeof
								   (RemoteCMPIIndicationMI));
    TRACE_NORMAL(("Creating proxy provider handle for: %s", provider));

    CMPI_BrokerExt_Ftab=broker->xft;
    mi->ref.ft = &miFT;
    mi->provider = strdup(provider);
    mi->broker = broker;
    mi->ticket = generate_ticket(broker);
    return (CMPIIndicationMI *) mi;
};


