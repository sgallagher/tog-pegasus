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
//               Marek Szermutzky, IBM (mszermutzky@de.ibm.com)
//
//%/////////////////////////////////////////////////////////////////////////////

/*!
  \file tcpcomm_proxy.c
  \brief TCP/IP communication layer (proxy side module).

  This module implements the proxy side functionality to support TCP/IP
  driven remote providers. This comprises setting up a daemon listening
  for broker service requests, as well as returning function handles for
  the individual MI calls.

  This communication layer can be addressed by the resolver using
  \p TCPComm as identifier. Remote provider destination addresses are
  assumed to be IP addresses or hostnames; no colons or port definitions
  are allowed.

  \author Frank Scheffler

  \sa tcpcomm_remote.c
  \sa socketcomm.h
  \sa proxy.h
*/


#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>

#include "proxy.h"
#include "tcpcomm.h"
#include "ip.h"
#include "serialization.h"
#include "io.h"
#include "debug.h"
#include "socketcomm.h"

/**************************************************************************/

static const struct BinarySerializerFT *__sft = &binarySerializerFT;

static CMPIBroker *__init_broker;
static CMPIContext *__init_context;

extern CMPIBrokerExtFT *CMPI_BrokerExt_Ftab;


static void TCPCOMM_prepareAttachThread(int socket, CMPIBroker * broker,
					CMPIContext * context)
{
    CMPIContext *ctx;
    unsigned long int ctx_id;
    ctx = CBPrepareAttachThread(broker, context);
    ctx_id = save_context(ctx);
    CMAddContextEntry(ctx, RCMPI_CTX_ID, &ctx_id, CMPI_uint32);
    socketcomm_serialize_context(socket, (__sft), ctx);
}

static void TCPCOMM_detachThread(int socket, CMPIBroker * broker,
				   CMPIContext * context)
{
    CMPIStatus rc;
    CMPIData ctxid = CMGetContextEntry(context, RCMPI_CTX_ID, NULL);
    CMPIContext *ctx = get_context(ctxid.value.uint32);
    remove_context(ctxid.value.uint32);
    rc = CBDetachThread(broker, ctx);
    (__sft)->serialize_CMPIStatus(socket, &rc);
}

static void TCPCOMM_deliverIndication(int socket, CMPIBroker * broker,
					CMPIContext * context)
{
    CMPIStatus rc;
    char *ns;
    CMPIInstance *ind;
    ns = (__sft)->deserialize_string(socket, broker);
    ind = (__sft)->deserialize_CMPIInstance(socket, broker);
    rc = CBDeliverIndication(broker, context, ns, ind);
    (__sft)->serialize_CMPIStatus(socket, &rc);
}

static void TCPCOMM_enumInstanceNames(int socket, CMPIBroker * broker,
					CMPIContext * context)
{
    CMPIEnumeration *e;
    CMPIStatus rc;
    CMPIObjectPath *cop;
    cop = (__sft)->deserialize_CMPIObjectPath(socket, broker);
    e = CBEnumInstanceNames(broker, context, cop, &rc);
    (__sft)->serialize_CMPIStatus(socket, &rc);
    (__sft)->serialize_CMPIArray(socket,
				 (e) ? CMToArray(e, NULL) : NULL);
}

static void TCPCOMM_getInstance(int socket, CMPIBroker * broker,
				  CMPIContext * context)
{
    CMPIInstance *inst;
    CMPIStatus rc;
    CMPIObjectPath *cop;
    char **props;
    cop = (__sft)->deserialize_CMPIObjectPath(socket, broker);
    props = socketcomm_deserialize_props(socket, (__sft), broker);
    inst = CBGetInstance(broker, context, cop, props, &rc);
    (__sft)->serialize_CMPIStatus(socket, &rc);
    (__sft)->serialize_CMPIInstance(socket, inst);
    free(props);
}

static void TCPCOMM_createInstance(int socket, CMPIBroker * broker,
				     CMPIContext * context)
{
    CMPIStatus rc;
    CMPIObjectPath *cop, *result;
    CMPIInstance *instance;
    cop = (__sft)->deserialize_CMPIObjectPath(socket, broker);
    instance = (__sft)->deserialize_CMPIInstance(socket, broker);
    result = CBCreateInstance(broker, context, cop, instance, &rc);
    (__sft)->serialize_CMPIStatus(socket, &rc);
    (__sft)->serialize_CMPIObjectPath(socket, result);
}

static void TCPCOMM_setInstance(int socket, CMPIBroker * broker,
				  CMPIContext * context)
{
    CMPIStatus rc;
    CMPIObjectPath *cop;
    CMPIInstance *instance;
    char **props;
    cop = (__sft)->deserialize_CMPIObjectPath(socket, broker);
    instance = (__sft)->deserialize_CMPIInstance(socket, broker);
    props = socketcomm_deserialize_props(socket, (__sft), broker);
    rc = CBSetInstance(broker, context, cop, instance, props);
    (__sft)->serialize_CMPIStatus(socket, &rc);
}

static void TCPCOMM_deleteInstance(int socket, CMPIBroker * broker,
				     CMPIContext * context)
{
    CMPIStatus rc;
    CMPIObjectPath *cop;
    cop = (__sft)->deserialize_CMPIObjectPath(socket, broker);
    rc = CBDeleteInstance(broker, context, cop);
    (__sft)->serialize_CMPIStatus(socket, &rc);
}

static void TCPCOMM_execQuery(int socket, CMPIBroker * broker,
				CMPIContext * context)
{
    CMPIEnumeration *e;
    CMPIStatus rc;
    CMPIObjectPath *cop;
    char *lang, *query;
    cop = (__sft)->deserialize_CMPIObjectPath(socket, broker);
    query = (__sft)->deserialize_string(socket, broker);
    lang = (__sft)->deserialize_string(socket, broker);
    e = CBExecQuery(broker, context, cop, query, lang, &rc);
    (__sft)->serialize_CMPIStatus(socket, &rc);
    (__sft)->serialize_CMPIArray(socket,
				 (e) ? CMToArray(e, NULL) : NULL);
}

static void TCPCOMM_enumInstances(int socket, CMPIBroker * broker,
				    CMPIContext * context)
{
    CMPIEnumeration *e;
    CMPIStatus rc;
    CMPIObjectPath *cop;
    char **props;
    cop = (__sft)->deserialize_CMPIObjectPath(socket, broker);
    props = socketcomm_deserialize_props(socket, (__sft), broker);
    e = CBEnumInstances(broker, context, cop, props, &rc);
    (__sft)->serialize_CMPIStatus(socket, &rc);
    (__sft)->serialize_CMPIArray(socket,
				 (e) ? CMToArray(e, NULL) : NULL);
    free(props);
}

static void TCPCOMM_associators(int socket, CMPIBroker * broker,
				  CMPIContext * context)
{
    CMPIEnumeration *e;
    CMPIStatus rc;
    CMPIObjectPath *cop;
    char *assocClass, *resultClass, *role, *resultRole, **props;
    cop = (__sft)->deserialize_CMPIObjectPath(socket, broker);
    assocClass = (__sft)->deserialize_string(socket, broker);
    resultClass = (__sft)->deserialize_string(socket, broker);
    role = (__sft)->deserialize_string(socket, broker);
    resultRole = (__sft)->deserialize_string(socket, broker);
    props = socketcomm_deserialize_props(socket, (__sft), broker);
    e = CBAssociators(broker, context, cop, assocClass, resultClass,
		      role, resultRole, props, &rc);
    (__sft)->serialize_CMPIStatus(socket, &rc);
    (__sft)->serialize_CMPIArray(socket,
				 (e) ? CMToArray(e, NULL) : NULL);
    free(props);
}

static void TCPCOMM_associatorNames(int socket, CMPIBroker * broker,
				      CMPIContext * context)
{
    CMPIEnumeration *e;
    CMPIStatus rc;
    CMPIObjectPath *cop;
    char *assocClass, *resultClass, *role, *resultRole;
    cop = (__sft)->deserialize_CMPIObjectPath(socket, broker);
    assocClass = (__sft)->deserialize_string(socket, broker);
    resultClass = (__sft)->deserialize_string(socket, broker);
    role = (__sft)->deserialize_string(socket, broker);
    resultRole = (__sft)->deserialize_string(socket, broker);
    e = CBAssociatorNames(broker, context, cop, assocClass, resultClass,
			  role, resultRole, &rc);
    (__sft)->serialize_CMPIStatus(socket, &rc);
    (__sft)->serialize_CMPIArray(socket,
				 (e) ? CMToArray(e, NULL) : NULL);
}

static void TCPCOMM_references(int socket, CMPIBroker * broker,
				 CMPIContext * context)
{
    CMPIEnumeration *e;
    CMPIStatus rc;
    CMPIObjectPath *cop;
    char *resultClass, *role, **props;
    cop = (__sft)->deserialize_CMPIObjectPath(socket, broker);
    resultClass = (__sft)->deserialize_string(socket, broker);
    role = (__sft)->deserialize_string(socket, broker);
    props = socketcomm_deserialize_props(socket, (__sft), broker);
    e = CBReferences(broker, context, cop, resultClass, role, props,
		     &rc);
    (__sft)->serialize_CMPIStatus(socket, &rc);
    (__sft)->serialize_CMPIArray(socket,
				 (e) ? CMToArray(e, NULL) : NULL);
    free(props);
}

static void TCPCOMM_referenceNames(int socket, CMPIBroker * broker,
				     CMPIContext * context)
{
    CMPIEnumeration *e;
    CMPIStatus rc;
    CMPIObjectPath *cop;
    char *resultClass, *role;
    cop = (__sft)->deserialize_CMPIObjectPath(socket, broker);
    resultClass = (__sft)->deserialize_string(socket, broker);
    role = (__sft)->deserialize_string(socket, broker);
    e = CBReferenceNames(broker, context, cop, resultClass, role, &rc);
    (__sft)->serialize_CMPIStatus(socket, &rc);
    (__sft)->serialize_CMPIArray(socket,
				 (e) ? CMToArray(e, NULL) : NULL);
}

static void TCPCOMM_invokeMethod(int socket, CMPIBroker * broker,
				   CMPIContext * context)
{
    CMPIArgs *in, *out;
    CMPIData result;
    CMPIStatus rc;
    CMPIObjectPath *cop;
    char *method;
    cop = (__sft)->deserialize_CMPIObjectPath(socket, broker);
    method = (__sft)->deserialize_string(socket, broker);
    in = (__sft)->deserialize_CMPIArgs(socket, broker);
    out = CMNewArgs(broker, NULL);
    result = CBInvokeMethod(broker, context, cop, method, in, out, &rc);
    (__sft)->serialize_CMPIStatus(socket, &rc);
    (__sft)->serialize_CMPIArgs(socket, out);
    (__sft)->serialize_CMPIData(socket, result);
}

static void TCPCOMM_setProperty(int socket, CMPIBroker * broker,
				  CMPIContext * context)
{
    CMPIStatus rc;
    CMPIObjectPath *cop;
    char *name;
    CMPIValue value;
    CMPIType type;
    cop = (__sft)->deserialize_CMPIObjectPath(socket, broker);
    name = (__sft)->deserialize_string(socket, broker);
    type = (__sft)->deserialize_CMPIType(socket);
    value = (__sft)->deserialize_CMPIValue(socket, type, broker);
    rc = CBSetProperty(broker, context, cop, name, &value, type);
    (__sft)->serialize_CMPIStatus(socket, &rc);
}

static void TCPCOMM_getProperty(int socket, CMPIBroker * broker,
				  CMPIContext * context)
{
    CMPIStatus rc;
    CMPIObjectPath *cop;
    char *name;
    CMPIData result;
    cop = (__sft)->deserialize_CMPIObjectPath(socket, broker);
    name = (__sft)->deserialize_string(socket, broker);
    result = CBGetProperty(broker, context, cop, name, &rc);
    (__sft)->serialize_CMPIStatus(socket, &rc);
    (__sft)->serialize_CMPIData(socket, result);
};

static void TCPCOMM_getKeyNames(int socket, CMPIBroker * broker,
		    CMPIContext * context)
{
    CMPIStatus rc;
    CMPIObjectPath *cop;
    char *name;
    CMPIArray *result;
    cop = (__sft)->deserialize_CMPIObjectPath(socket, broker);
    result =broker->eft->getKeyList(broker, context, cop, &rc);
    (__sft)->serialize_CMPIStatus(socket, &rc);
    (__sft)->serialize_CMPIArray(socket, result);
};

static void TCPCOMM_getMessage (int socket, CMPIBroker * broker,
		    CMPIContext * context)
{
    union {
      CMPIUint32  uint;
      CMPIBoolean blean;
      CMPIString  *str;
      CMPIReal64  r64;
      CMPIUint64  u64;
    } value[10];
    CMPIType type[10];

    CMPIStatus rc;
    CMPIString *result;
    char *msgId,*defMsg;
    CMPIUint32 count,i;
    msgId = (__sft)->deserialize_string(socket, broker);
    defMsg = (__sft)->deserialize_string(socket, broker);
    count = (__sft)->deserialize_UINT32(socket);

    for (i=0; i<count; i++) {
       type[i]=(__sft)->deserialize_CMPIType(socket);
       switch (type[i]) {
       case CMPI_uint32:
       case CMPI_sint32:
          value[i].uint=(__sft)->deserialize_UINT32(socket);
	  break;
       case CMPI_boolean:
          value[i].blean=(CMPIBoolean)(__sft)->deserialize_UINT32(socket);
 	  break;
       case CMPI_uint64:
       case CMPI_sint64:
          value[i].u64=(CMPIBoolean)(__sft)->deserialize_UINT64(socket);
 	  break;
       case CMPI_real64: {
             char * real_str;
             real_str = (__sft)->deserialize_string ( socket, broker );
             sscanf ( real_str, "%le", (double *) &value[i].r64 );
          }
	  break;
       case CMPI_string:
          value[i].str = (__sft)->deserialize_CMPIString ( socket, broker );
	  break;
       }
    }


    switch (count) {
    case 0:
        result = broker->eft->getMessage(broker, msgId, defMsg, &rc, count);
	break;
    case 1:
        result = broker->eft->getMessage(broker, msgId, defMsg, &rc, count,
	   type[0],value[0].uint);
	break;
    case 2:
        result = broker->eft->getMessage(broker, msgId, defMsg, &rc, count,
	   type[0],value[0].uint, type[1],value[1].uint);
	break;
    case 3:
        result = broker->eft->getMessage(broker, msgId, defMsg, &rc, count,
	   type[0],value[0].uint, type[1],value[1].uint, type[2],value[2].uint);
	break;
    case 4:
        result = broker->eft->getMessage(broker, msgId, defMsg, &rc, count,
	   type[0],value[0].uint, type[1],value[1].uint, type[2],value[2].uint,
	   type[3],value[3].uint);
    case 5:
        result = broker->eft->getMessage(broker, msgId, defMsg, &rc, count,
	   type[0],value[0].uint, type[1],value[1].uint, type[2],value[2].uint,
	   type[3],value[3].uint, type[4],value[4].uint);
	break;
    case 6:
        result = broker->eft->getMessage(broker, msgId, defMsg, &rc, count,
	   type[0],value[0].uint, type[1],value[1].uint, type[2],value[2].uint,
	   type[3],value[3].uint, type[4],value[4].uint, type[5],value[5].uint);
	break;
    case 7:
        result = broker->eft->getMessage(broker, msgId, defMsg, &rc, count,
	   type[0],value[0].uint, type[1],value[1].uint, type[2],value[2].uint,
	   type[3],value[3].uint, type[4],value[4].uint, type[5],value[5].uint,
	   type[6],value[6].uint);
	break;
     case 8:
        result = broker->eft->getMessage(broker, msgId, defMsg, &rc, count,
	   type[0],value[0].uint, type[1],value[1].uint, type[2],value[2].uint,
	   type[3],value[3].uint, type[4],value[4].uint, type[5],value[5].uint,
	   type[6],value[6].uint, type[7],value[7].uint);
	break;
     case 9:
        result = broker->eft->getMessage(broker, msgId, defMsg, &rc, count,
	   type[0],value[0].uint, type[1],value[1].uint, type[2],value[2].uint,
	   type[3],value[3].uint, type[4],value[4].uint, type[5],value[5].uint,
	   type[6],value[6].uint, type[7],value[7].uint, type[8],value[8].uint);
	break;
     default:
        result = broker->eft->getMessage(broker, msgId, defMsg, &rc, count,
	   type[0],value[0].uint, type[1],value[1].uint, type[2],value[2].uint,
	   type[3],value[3].uint, type[4],value[4].uint, type[5],value[5].uint,
	   type[6],value[6].uint, type[7],value[7].uint, type[8],value[8].uint,
	   type[9],value[9].uint);
   }

    (__sft)->serialize_CMPIStatus(socket, &rc);
    (__sft)->serialize_CMPIString(socket, result);
}

static void TCPCOMM_classPathIsA (int socket, CMPIBroker * broker,
		    CMPIContext * context)
{
//  ( CMPIBroker * broker,
//        CMPIObjectPath * cop, const char * type, CMPIStatus * rc );
    CMPIStatus rc;
    CMPIObjectPath *cop;
    char *type;
    CMPIBoolean result;
    cop = (__sft)->deserialize_CMPIObjectPath(socket, broker);
    type= (__sft)->deserialize_string(socket, broker);
    result = broker->eft->classPathIsA(broker, cop, type, &rc);
    (__sft)->serialize_CMPIStatus(socket, &rc);
    (__sft)->serialize_UINT8(socket, (unsigned char)result);
};


//! Up-call function identifiers.
/*!
  This array holds all possible broker service request names issued by
  remote providers and the appropriate function pointers.
 */
static struct socket_mb_function __mb_functions[] = {
    { "TCPCOMM_prepareAttachThread",    TCPCOMM_prepareAttachThread },
    { "TCPCOMM_prepareAttachThread",    TCPCOMM_prepareAttachThread },
    { "TCPCOMM_detachThread",           TCPCOMM_detachThread },
    { "TCPCOMM_deliverIndication",      TCPCOMM_deliverIndication },
    { "TCPCOMM_enumInstanceNames",      TCPCOMM_enumInstanceNames },
    { "TCPCOMM_getInstance",            TCPCOMM_getInstance },
    { "TCPCOMM_createInstance",         TCPCOMM_createInstance },
    { "TCPCOMM_setInstance",            TCPCOMM_setInstance },
    { "TCPCOMM_deleteInstance",         TCPCOMM_deleteInstance },
    { "TCPCOMM_execQuery",              TCPCOMM_execQuery },
    { "TCPCOMM_enumInstances",          TCPCOMM_enumInstances },
    { "TCPCOMM_associators",            TCPCOMM_associators },
    { "TCPCOMM_associatorNames",        TCPCOMM_associatorNames },
    { "TCPCOMM_references",             TCPCOMM_references },
    { "TCPCOMM_referenceNames",         TCPCOMM_referenceNames },
    { "TCPCOMM_invokeMethod",           TCPCOMM_invokeMethod },
    { "TCPCOMM_setProperty",            TCPCOMM_setProperty },
    { "TCPCOMM_getProperty",            TCPCOMM_getProperty },
    { "TCPCOMM_getKeyNames",            TCPCOMM_getKeyNames },
    { "TCPCOMM_classPathIsA",           TCPCOMM_classPathIsA }
};


/****************************************************************************/

//! Dispatches the broker service request handler.
/*!
  This function is running in a separate thread and sets up the environment
  for the actual request handler before calling it. This comprises reading
  the function name and deserializing the calling context.

  \param athread contains socket, broker and context handles.

  \sa __mb_functions
  \sa __handle_MB_call
 */
static void __dispatch_MB_function(struct accept_thread *athread)
{
    char *function;
    unsigned int i;

    TRACE_VERBOSE(("entered function."));
    TRACE_NORMAL(("dispatching broker service function request."));

    CBAttachThread(athread->broker, athread->context);

    function =
	__sft->deserialize_string(athread->socket, athread->broker);

    socketcomm_deserialize_context(athread->socket,  __sft, athread->broker,
				   athread->context);

    for (i = 0;
	 i < sizeof(__mb_functions) / sizeof(struct socket_mb_function);
	 i++) {

	if (strcmp(function, __mb_functions[i].name) == 0) {

	    TRACE_INFO(("calling %s to handle request.",
			__mb_functions[i].name));

	    __mb_functions[i].function(athread->socket, athread->broker,
				       athread->context);
	    break;
	}
    }

    CBDetachThread(athread->broker, athread->context);

    close(athread->socket);
    free(athread);

    TRACE_VERBOSE(("leaving function."));
}


//! Sets up the minimal environment for a broker service request thread.
/*!
  The function evaluates the up-call context id to set up
  the basic entities broker and context, before actually
  spawning a separate thread to handle the request.

  \param socket the connection socket.
  \param broker the broker to forward the request to.

  \sa __dispatch_MB_function
  \sa __verify_MB_call
 */
static void __handle_MB_call(int socket, CMPIBroker * broker)
{
    CMPIContext *ctx;

    struct accept_thread *athread = (struct accept_thread *)
	malloc(sizeof(struct accept_thread));

    TRACE_VERBOSE(("entered function."));
    TRACE_NORMAL(("handling incoming broker service request."));

    ctx = get_context(__sft->deserialize_UINT32(socket));

    athread->socket = socket;
    athread->broker = broker;
    athread->context = CBPrepareAttachThread(broker, ctx);

     CMPI_BrokerExt_Ftab->newThread((void*(*)(void*))__dispatch_MB_function,athread,1);

    TRACE_VERBOSE(("leaving function."));
}


//! Verifies an incoming MB call's ticket.
/*!
  The function evaluates the up-call ticket, retrieves the appropriate
  broker handle, and passes on the request to __handle_MB_call(), if
  successful.

  \param socket the connection socket.
 */
static void __verify_MB_call(int socket)
{
    comm_ticket ticket;
    CMPIBroker *broker;

    TRACE_VERBOSE(("entered function."));
    TRACE_NORMAL(("handling incoming broker service request."));

    io_read_fixed_length(socket, &ticket, sizeof(comm_ticket));
    broker = verify_ticket(&ticket);

    if (broker == NULL) {

	TRACE_CRITICAL(("ticket invalid, could not obtain a broker"
			" handle."));
	// return bad status.
	close(socket);

    } else
	__handle_MB_call(socket, broker);

    TRACE_VERBOSE(("leaving function."));
}


//! Establishes the remote daemon connection.
/*!
  Opens a connection to the given address on port REMOTE_LISTEN_PORT, and
  sends the broker ticket, provider name and module, MI function name,
  context, and object path.

  \param addr the provider address.
  \param provider the remote provider name.
  \param ticket the ticket as issued by the broker.
  \param function the MI function call name.
  \param ctx the context for this MI call.
  \param cop the object path for this call.

  \return the connection socket.
 */
static int
__provider_connect(provider_address * addr,
		   char *provider,
		   comm_ticket * ticket,
		   char *function, CMPIContext * ctx,
		   CMPIObjectPath * cop)
{
    int socket = open_connection(addr->dst_address,
				 REMOTE_LISTEN_PORT);
    char *pnp;

    if (socket < 0)
	return -1;

    if ((pnp = strchr(provider, ':')))
	__sft->serialize_string(socket, pnp + 1);
    else __sft->serialize_string(socket, provider);
    __sft->serialize_string(socket, addr->provider_module);
    io_write_fixed_length(socket, ticket, sizeof(comm_ticket));
    __sft->serialize_string(socket, function);

    socketcomm_serialize_context(socket, __sft, ctx);
    __sft->serialize_CMPIObjectPath(socket, cop);

    return socket;
}

CMPIString *connect_error(provider_address * addr)
{
   CMPIString *str;
   char msg[512]="Unable to connect to ";

   strcat(msg,addr->dst_address);
   str=CMNewString(__init_broker,msg,NULL);
   return str;
}

//! Waits on incoming broker service requests.
/*!
  This function is to be run in a separate thread. There it attaches itself
  to the given context and waits for incoming MB calls. These are passed on
  to __verify_MB_call().

  \param ctx the CMPIContext to attach to.

  \sa accept_connections()
 */
static void __start_proxy_daemon(CMPIContext * ctx)
{
    if (ctx != NULL)
	CBAttachThread(__init_broker, ctx);
    accept_connections(CIMOM_LISTEN_PORT, __verify_MB_call, 0);
}


//! Launches __start_proxy_daemon() in a separate thread.
/*!
  The function prepares a context for the MB listen thread and launches it.
 */
static void __launch_proxy_daemon()
{
    CMPIContext *ctx = (__init_context && __init_broker) ?
	CBPrepareAttachThread(__init_broker, __init_context) : NULL;

//    printf("--- starting __start_proxy_daemon: %p\n",ctx);
    CMPI_BrokerExt_Ftab->newThread((void*(*)(void*))__start_proxy_daemon,ctx,1);
}

/**************************************************************************/

static CMPIStatus TCPCOMM_InstanceMI_enumInstanceNames(provider_address * addr,
				     RemoteCMPIInstanceMI * cThis,
				     CMPIContext * context,
				     CMPIResult * result,
				     CMPIObjectPath * cop)
{
    int socket;
    unsigned long int ctxid = save_context(context);
    TRACE_NORMAL(("Executing remote MI call."));
    CMAddContextEntry(context, RCMPI_CTX_ID, &ctxid, CMPI_uint32);
    socket = __provider_connect(addr, cThis->provider, &cThis->ticket,
			   __FUNCTION__, context, cop);
    if (socket < 0)
	CMReturnWithString(CMPI_RC_ERR_FAILED,connect_error(addr));
    {
	CMPIStatus rc;
	CMPIArray *r;
	rc = (__sft)->deserialize_CMPIStatus(socket, cThis->broker);
	r = (__sft)->deserialize_CMPIArray(socket, cThis->broker);
	close(socket);
	remove_context(ctxid);
	socketcomm_array2result(r, result);
	return rc;
    };
}


static CMPIStatus TCPCOMM_InstanceMI_enumInstances(provider_address * addr,
				 RemoteCMPIInstanceMI * cThis,
				 CMPIContext * context,
				 CMPIResult * result,
				 CMPIObjectPath * cop, char **props)
{
    int socket;
    unsigned long int ctxid = save_context(context);
    TRACE_NORMAL(("Executing remote MI call."));
    CMAddContextEntry(context, RCMPI_CTX_ID, &ctxid, CMPI_uint32);
    socket = __provider_connect(addr, cThis->provider, &cThis->ticket,
			   __FUNCTION__, context, cop);
    if (socket < 0)
	CMReturnWithString(CMPI_RC_ERR_FAILED,connect_error(addr));
    socketcomm_serialize_props(socket, (__sft), props);
    {
	CMPIStatus rc;
	CMPIArray *r;
	rc = (__sft)->deserialize_CMPIStatus(socket, cThis->broker);
	r = (__sft)->deserialize_CMPIArray(socket, cThis->broker);
	close(socket);
	remove_context(ctxid);
	socketcomm_array2result(r, result);
	return rc;
    };
}


static CMPIStatus TCPCOMM_InstanceMI_getInstance(provider_address * addr,
			       RemoteCMPIInstanceMI * cThis,
			       CMPIContext * context,
			       CMPIResult * result,
			       CMPIObjectPath * cop, char **props)
{
    int socket;
    unsigned long int ctxid = save_context(context);
    TRACE_NORMAL(("Executing remote MI call."));
    CMAddContextEntry(context, RCMPI_CTX_ID, &ctxid, CMPI_uint32);
    socket = __provider_connect(addr, cThis->provider, &cThis->ticket,
			   __FUNCTION__, context, cop);
    if (socket < 0)
	CMReturnWithString(CMPI_RC_ERR_FAILED,connect_error(addr));
    socketcomm_serialize_props(socket, (__sft), props);
    {
	CMPIStatus rc;
	CMPIArray *r;
	rc = (__sft)->deserialize_CMPIStatus(socket, cThis->broker);
	r = (__sft)->deserialize_CMPIArray(socket, cThis->broker);
	close(socket);
	remove_context(ctxid);
	socketcomm_array2result(r, result);
	return rc;
    };
}


static CMPIStatus TCPCOMM_InstanceMI_createInstance(provider_address * addr,
				  RemoteCMPIInstanceMI * cThis,
				  CMPIContext * context,
				  CMPIResult * result,
				  CMPIObjectPath * cop,
				  CMPIInstance * inst)
{
    int socket;
    unsigned long int ctxid = save_context(context);
    TRACE_NORMAL(("Executing remote MI call."));
    CMAddContextEntry(context, RCMPI_CTX_ID, &ctxid, CMPI_uint32);
    socket = __provider_connect(addr, cThis->provider, &cThis->ticket,
			   __FUNCTION__, context, cop);
    if (socket < 0)
	CMReturnWithString(CMPI_RC_ERR_FAILED,connect_error(addr));
    (__sft)->serialize_CMPIInstance(socket, inst);
    {
	CMPIStatus rc;
	CMPIArray *r;
	rc = (__sft)->deserialize_CMPIStatus(socket, cThis->broker);
	r = (__sft)->deserialize_CMPIArray(socket, cThis->broker);
	close(socket);
	remove_context(ctxid);
	socketcomm_array2result(r, result);
	return rc;
    };
}


static CMPIStatus TCPCOMM_InstanceMI_setInstance(provider_address * addr,
			       RemoteCMPIInstanceMI * cThis,
			       CMPIContext * context,
			       CMPIResult * result,
			       CMPIObjectPath * cop,
			       CMPIInstance * inst, char **props)
{
    int socket;
    unsigned long int ctxid = save_context(context);
    TRACE_NORMAL(("Executing remote MI call."));
    CMAddContextEntry(context, RCMPI_CTX_ID, &ctxid, CMPI_uint32);
    socket = __provider_connect(addr, cThis->provider, &cThis->ticket,
			   __FUNCTION__, context, cop);
    if (socket < 0)
	CMReturnWithString(CMPI_RC_ERR_FAILED,connect_error(addr));
    (__sft)->serialize_CMPIInstance(socket, inst);
    socketcomm_serialize_props(socket, (__sft), props);
    {
	CMPIStatus rc;
	CMPIArray *r;
	rc = (__sft)->deserialize_CMPIStatus(socket, cThis->broker);
	r = (__sft)->deserialize_CMPIArray(socket, cThis->broker);
	close(socket);
	remove_context(ctxid);
	socketcomm_array2result(r, result);
	return rc;
    };
}


static CMPIStatus TCPCOMM_InstanceMI_deleteInstance(provider_address * addr,
				  RemoteCMPIInstanceMI * cThis,
				  CMPIContext * context,
				  CMPIResult * result,
				  CMPIObjectPath * cop)
{
    int socket;
    unsigned long int ctxid = save_context(context);
    TRACE_NORMAL(("Executing remote MI call."));
    CMAddContextEntry(context, RCMPI_CTX_ID, &ctxid, CMPI_uint32);
    socket = __provider_connect(addr, cThis->provider, &cThis->ticket,
			   __FUNCTION__, context, cop);
    if (socket < 0)
	CMReturnWithString(CMPI_RC_ERR_FAILED,connect_error(addr));
    {
	CMPIStatus rc;
	CMPIArray *r;
	rc = (__sft)->deserialize_CMPIStatus(socket, cThis->broker);
	r = (__sft)->deserialize_CMPIArray(socket, cThis->broker);
	close(socket);
	remove_context(ctxid);
	socketcomm_array2result(r, result);
	return rc;
    };
}

static CMPIStatus TCPCOMM_InstanceMI_execQuery(provider_address * addr,
			     RemoteCMPIInstanceMI * cThis,
			     CMPIContext * context, CMPIResult * result,
			     CMPIObjectPath * cop, char *lang,
			     char *query)
{
    int socket;
    unsigned long int ctxid = save_context(context);
    TRACE_NORMAL(("Executing remote MI call."));
    CMAddContextEntry(context, RCMPI_CTX_ID, &ctxid, CMPI_uint32);
    socket = __provider_connect(addr, cThis->provider, &cThis->ticket,
			   __FUNCTION__, context, cop);
    if (socket < 0)
	CMReturnWithString(CMPI_RC_ERR_FAILED,connect_error(addr));
    (__sft)->serialize_string(socket, lang);
    (__sft)->serialize_string(socket, query);
    {
	CMPIStatus rc;
	CMPIArray *r;
	rc = (__sft)->deserialize_CMPIStatus(socket, cThis->broker);
	r = (__sft)->deserialize_CMPIArray(socket, cThis->broker);
	close(socket);
	remove_context(ctxid);
	socketcomm_array2result(r, result);
	return rc;
    };
};


static CMPIStatus TCPCOMM_AssociationMI_associators(provider_address * addr,
				  RemoteCMPIAssociationMI * cThis,
				  CMPIContext * context,
				  CMPIResult * result,
				  CMPIObjectPath * cop,
				  const char *assocclass, const char *resultclass,
				  const char *role, const char *resultrole,
				  char **props)
{
    int socket;
    unsigned long int ctxid = save_context(context);
    TRACE_NORMAL(("Executing remote MI call."));
    CMAddContextEntry(context, RCMPI_CTX_ID, &ctxid, CMPI_uint32);
    socket = __provider_connect(addr, cThis->provider, &cThis->ticket,
			   __FUNCTION__, context, cop);
    if (socket < 0)
	CMReturnWithString(CMPI_RC_ERR_FAILED,connect_error(addr));
    (__sft)->serialize_string(socket, assocclass);
    (__sft)->serialize_string(socket, resultclass);
    (__sft)->serialize_string(socket, role);
    (__sft)->serialize_string(socket, resultrole);
    socketcomm_serialize_props(socket, (__sft), props);
    {
	CMPIStatus rc;
	CMPIArray *r;
	rc = (__sft)->deserialize_CMPIStatus(socket, cThis->broker);
	r = (__sft)->deserialize_CMPIArray(socket, cThis->broker);
	close(socket);
	remove_context(ctxid);
	socketcomm_array2result(r, result);
	return rc;
    };
}


static CMPIStatus TCPCOMM_AssociationMI_associatorNames(provider_address * addr,
				      RemoteCMPIAssociationMI * cThis,
				      CMPIContext * context,
				      CMPIResult * result,
				      CMPIObjectPath * cop,
				      const char *assocclass,
				      const char *resultclass, const char *role,
				      const char *resultrole)
{
    int socket;
    unsigned long int ctxid = save_context(context);
    TRACE_NORMAL(("Executing remote MI call."));
    CMAddContextEntry(context, RCMPI_CTX_ID, &ctxid, CMPI_uint32);
    socket = __provider_connect(addr, cThis->provider, &cThis->ticket,
			   __FUNCTION__, context, cop);
    if (socket < 0)
	CMReturnWithString(CMPI_RC_ERR_FAILED,connect_error(addr));
    (__sft)->serialize_string(socket, assocclass);
    (__sft)->serialize_string(socket, resultclass);
    (__sft)->serialize_string(socket, role);
    (__sft)->serialize_string(socket, resultrole);
    {
	CMPIStatus rc;
	CMPIArray *r;
	rc = (__sft)->deserialize_CMPIStatus(socket, cThis->broker);
	r = (__sft)->deserialize_CMPIArray(socket, cThis->broker);
	close(socket);
	remove_context(ctxid);
	socketcomm_array2result(r, result);
	return rc;
    };
}


static CMPIStatus TCPCOMM_AssociationMI_references(provider_address * addr,
				 RemoteCMPIAssociationMI * cThis,
				 CMPIContext * context,
				 CMPIResult * result,
				 CMPIObjectPath * cop, const char *assocclass,
				 const char *role, char **props)
{
    int socket;
    unsigned long int ctxid = save_context(context);
    TRACE_NORMAL(("Executing remote MI call."));
    CMAddContextEntry(context, RCMPI_CTX_ID, &ctxid, CMPI_uint32);
    socket = __provider_connect(addr, cThis->provider, &cThis->ticket,
			   __FUNCTION__, context, cop);
    if (socket < 0)
	CMReturnWithString(CMPI_RC_ERR_FAILED,connect_error(addr));
    (__sft)->serialize_string(socket, assocclass);
    (__sft)->serialize_string(socket, role);
    socketcomm_serialize_props(socket, (__sft), props);
    {
	CMPIStatus rc;
	CMPIArray *r;
	rc = (__sft)->deserialize_CMPIStatus(socket, cThis->broker);
	r = (__sft)->deserialize_CMPIArray(socket, cThis->broker);
	close(socket);
	remove_context(ctxid);
	socketcomm_array2result(r, result);
	return rc;
    };
}


static CMPIStatus TCPCOMM_AssociationMI_referenceNames(provider_address * addr,
				     RemoteCMPIAssociationMI * cThis,
				     CMPIContext * context,
				     CMPIResult * result,
				     CMPIObjectPath * cop,
				     const char *assocclass, const char *role)
{
    int socket;
    unsigned long int ctxid = save_context(context);
    TRACE_NORMAL(("Executing remote MI call."));
    CMAddContextEntry(context, RCMPI_CTX_ID, &ctxid, CMPI_uint32);
    socket = __provider_connect(addr, cThis->provider, &cThis->ticket,
			   __FUNCTION__, context, cop);
    if (socket < 0)
	CMReturnWithString(CMPI_RC_ERR_FAILED,connect_error(addr));
    (__sft)->serialize_string(socket, assocclass);
    (__sft)->serialize_string(socket, role);
    {
	CMPIStatus rc;
	CMPIArray *r;
	rc = (__sft)->deserialize_CMPIStatus(socket, cThis->broker);
	r = (__sft)->deserialize_CMPIArray(socket, cThis->broker);
	close(socket);
	remove_context(ctxid);
	socketcomm_array2result(r, result);
	return rc;
    };
};


static CMPIStatus TCPCOMM_MethodMI_invokeMethod(provider_address * addr,
			      RemoteCMPIMethodMI * cThis,
			      CMPIContext * context,
			      CMPIResult * result, CMPIObjectPath * cop,
			      const char *method, CMPIArgs * in,
			      CMPIArgs * out)
{
    int socket;
    unsigned long int ctxid = save_context(context);
    TRACE_NORMAL(("Executing remote MI call."));
    CMAddContextEntry(context, RCMPI_CTX_ID, &ctxid, CMPI_uint32);
    socket = __provider_connect(addr, cThis->provider, &cThis->ticket,
			   __FUNCTION__, context, cop);
    if (socket < 0)
	CMReturnWithString(CMPI_RC_ERR_FAILED,connect_error(addr));
    (__sft)->serialize_string(socket, method);
    (__sft)->serialize_CMPIArgs(socket, in);
    {
	CMPIStatus rc;
	CMPIArray *r;
	rc = (__sft)->deserialize_CMPIStatus(socket, cThis->broker);
	r = (__sft)->deserialize_CMPIArray(socket, cThis->broker);
	{
	    CMPIArgs *a =
		(__sft)->deserialize_CMPIArgs(socket, cThis->broker);
	    socketcomm_copy_args(a, (out));
	}
	close(socket);
	remove_context(ctxid);
	socketcomm_array2result(r, result);
	return rc;
    };
};


static CMPIStatus TCPCOMM_PropertyMI_setProperty(provider_address * addr,
			       RemoteCMPIPropertyMI * cThis,
			       CMPIContext * context,
			       CMPIResult * result,
			       CMPIObjectPath * cop, const char *name,
			       CMPIData data)
{
    int socket;
    unsigned long int ctxid = save_context(context);
    TRACE_NORMAL(("Executing remote MI call."));
    CMAddContextEntry(context, RCMPI_CTX_ID, &ctxid, CMPI_uint32);
    socket = __provider_connect(addr, cThis->provider, &cThis->ticket,
			   __FUNCTION__, context, cop);
    if (socket < 0)
	CMReturnWithString(CMPI_RC_ERR_FAILED,connect_error(addr));
    (__sft)->serialize_string(socket, name);
    (__sft)->serialize_CMPIData(socket, data);
    {
	CMPIStatus rc;
	CMPIArray *r;
	rc = (__sft)->deserialize_CMPIStatus(socket, cThis->broker);
	r = (__sft)->deserialize_CMPIArray(socket, cThis->broker);
	close(socket);
	remove_context(ctxid);
	socketcomm_array2result(r, result);
	return rc;
    };
}

static CMPIStatus TCPCOMM_PropertyMI_getProperty(provider_address * addr,
			       RemoteCMPIPropertyMI * cThis,
			       CMPIContext * context,
			       CMPIResult * result,
			       CMPIObjectPath * cop, const char *name)
{
    int socket;
    unsigned long int ctxid = save_context(context);
    TRACE_NORMAL(("Executing remote MI call."));
    CMAddContextEntry(context, RCMPI_CTX_ID, &ctxid, CMPI_uint32);
    socket = __provider_connect(addr, cThis->provider, &cThis->ticket,
			   __FUNCTION__, context, cop);
    if (socket < 0)
	CMReturnWithString(CMPI_RC_ERR_FAILED,connect_error(addr));
    (__sft)->serialize_string(socket, name);
    {
	CMPIStatus rc;
	CMPIArray *r;
	rc = (__sft)->deserialize_CMPIStatus(socket, cThis->broker);
	r = (__sft)->deserialize_CMPIArray(socket, cThis->broker);
	close(socket);
	remove_context(ctxid);
	socketcomm_array2result(r, result);
	return rc;
    };
};


static CMPIStatus TCPCOMM_IndicationMI_authorizeFilter(provider_address * addr,
				     RemoteCMPIIndicationMI * cThis,
				     CMPIContext * context,
				     CMPIResult * result,
				     CMPIObjectPath * cop,
				     CMPISelectExp * filter,
				     const char *indType, const char *owner)
{
    int socket;
    unsigned long int ctxid = save_context(context);
    TRACE_NORMAL(("Executing remote MI call."));
    CMAddContextEntry(context, RCMPI_CTX_ID, &ctxid, CMPI_uint32);
    socket = __provider_connect(addr, cThis->provider, &cThis->ticket,
			   __FUNCTION__, context, cop);
    if (socket < 0)
	CMReturnWithString(CMPI_RC_ERR_FAILED,connect_error(addr));
    (__sft)->serialize_CMPISelectExp(socket, filter);
    (__sft)->serialize_string(socket, indType);
    (__sft)->serialize_string(socket, owner);
    {
	CMPIStatus rc;
	CMPIArray *r;
	rc = (__sft)->deserialize_CMPIStatus(socket, cThis->broker);
	r = (__sft)->deserialize_CMPIArray(socket, cThis->broker);
	close(socket);
	remove_context(ctxid);
	socketcomm_array2result(r, result);
	return rc;
    };
}


static CMPIStatus TCPCOMM_IndicationMI_mustPoll(provider_address * addr,
			      RemoteCMPIIndicationMI * cThis,
			      CMPIContext * context,
			      CMPIResult * result, CMPIObjectPath * cop,
			      CMPISelectExp * filter, const char *indType)
{
    int socket;
    unsigned long int ctxid = save_context(context);
    TRACE_NORMAL(("Executing remote MI call."));
    CMAddContextEntry(context, RCMPI_CTX_ID, &ctxid, CMPI_uint32);
    socket = __provider_connect(addr, cThis->provider, &cThis->ticket,
			   __FUNCTION__, context, cop);
    if (socket < 0)
	CMReturnWithString(CMPI_RC_ERR_FAILED,connect_error(addr));
    (__sft)->serialize_CMPISelectExp(socket, filter);
    (__sft)->serialize_string(socket, indType);
    {
	CMPIStatus rc;
	CMPIArray *r;
	rc = (__sft)->deserialize_CMPIStatus(socket, cThis->broker);
	r = (__sft)->deserialize_CMPIArray(socket, cThis->broker);
	close(socket);
	remove_context(ctxid);
	socketcomm_array2result(r, result);
	return rc;
    };
}


static CMPIStatus TCPCOMM_IndicationMI_activateFilter(provider_address * addr,
				    RemoteCMPIIndicationMI * cThis,
				    CMPIContext * context,
				    CMPIResult * result,
				    CMPIObjectPath * cop,
				    CMPISelectExp * filter,
				    const char *indType,
				    CMPIBoolean firstActivation)
{
    int socket;
    unsigned long int ctxid = save_context(context);
    TRACE_NORMAL(("Executing remote MI call."));
    CMAddContextEntry(context, RCMPI_CTX_ID, &ctxid, CMPI_uint32);
    socket = __provider_connect(addr, cThis->provider, &cThis->ticket,
			   __FUNCTION__, context, cop);
    if (socket < 0)
	CMReturnWithString(CMPI_RC_ERR_FAILED,connect_error(addr));
    (__sft)->serialize_CMPISelectExp(socket, filter);
    (__sft)->serialize_string(socket, indType);
    (__sft)->serialize_UINT8(socket, firstActivation);
    {
	CMPIStatus rc;
	CMPIArray *r;
	rc = (__sft)->deserialize_CMPIStatus(socket, cThis->broker);
	r = (__sft)->deserialize_CMPIArray(socket, cThis->broker);
	close(socket);
	remove_context(ctxid);
	socketcomm_array2result(r, result);
	return rc;
    };
}


static CMPIStatus TCPCOMM_IndicationMI_deActivateFilter(provider_address * addr,
				      RemoteCMPIIndicationMI * cThis,
				      CMPIContext * context,
				      CMPIResult * result,
				      CMPIObjectPath * cop,
				      CMPISelectExp * filter,
				      const char *indType,
				      CMPIBoolean lastActivation)
{
    int socket;
    unsigned long int ctxid = save_context(context);
    TRACE_NORMAL(("Executing remote MI call."));
    CMAddContextEntry(context, RCMPI_CTX_ID, &ctxid, CMPI_uint32);
    socket = __provider_connect(addr, cThis->provider, &cThis->ticket,
			   __FUNCTION__, context, cop);
    if (socket < 0)
	CMReturnWithString(CMPI_RC_ERR_FAILED,connect_error(addr));
    (__sft)->serialize_CMPISelectExp(socket, filter);
    (__sft)->serialize_string(socket, indType);
    (__sft)->serialize_UINT8(socket, lastActivation);
    {
	CMPIStatus rc;
	CMPIArray *r;
	rc = (__sft)->deserialize_CMPIStatus(socket, cThis->broker);
	r = (__sft)->deserialize_CMPIArray(socket, cThis->broker);
	close(socket);
	remove_context(ctxid);
	socketcomm_array2result(r, result);
	return rc;
    };
};


//! Initializes this communication layer.
/*!
  This method, if called by the proxy provider sets up the daemon thread
  to listen for MB calls, then returns a provider_comm to receive MI
  requests.

  \param broker the CMPIBroker requesting this service.
  \param ctx the context of the requesting caller.
 */
provider_comm *CMPIRTCPComm_InitCommLayer(CMPIBroker * broker,
					  CMPIContext * ctx)
{
   static provider_comm comm = {
        "TCPComm",
        TCPCOMM_InstanceMI_enumInstanceNames,
        TCPCOMM_InstanceMI_enumInstances,
        TCPCOMM_InstanceMI_getInstance,
        TCPCOMM_InstanceMI_createInstance,
        TCPCOMM_InstanceMI_setInstance,
        TCPCOMM_InstanceMI_deleteInstance,
        TCPCOMM_InstanceMI_execQuery,
        TCPCOMM_AssociationMI_associators,
        TCPCOMM_AssociationMI_associatorNames,
        TCPCOMM_AssociationMI_references,
        TCPCOMM_AssociationMI_referenceNames,
        TCPCOMM_MethodMI_invokeMethod,
        TCPCOMM_PropertyMI_setProperty,
        TCPCOMM_PropertyMI_getProperty,
        TCPCOMM_IndicationMI_authorizeFilter,
        TCPCOMM_IndicationMI_mustPoll,
        TCPCOMM_IndicationMI_activateFilter,
        TCPCOMM_IndicationMI_deActivateFilter,
        NULL
   };

#ifndef PEGASUS_PLATFORM_ZOS_ZSERIES
    static int  __once = 0;
#else
		pthread_once_t __once = PTHREAD_ONCE_INIT;
#endif

     __init_broker = broker;
     __init_context = ctx;

     CMPI_BrokerExt_Ftab->threadOnce(&__once, __launch_proxy_daemon);

     return &comm;
}


/****************************************************************************/

/*** Local Variables:  ***/
/*** mode: C           ***/
/*** c-basic-offset: 8 ***/
/*** End:              ***/
