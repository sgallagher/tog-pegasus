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
// Author:      Adrian Schuur, schuur@de.ibm.com
//
// Modified By:	Seema Gupta (gseema@in.ibm.com) for PEP135
//              Robert Kieninger (kieningr@de.ibm.com) for Bugzilla 2320
//              Josephine Eskaline Joyce(jojustin@in.ibm.com) for PEP#101
//              Aruran, IBM (ashanmug@in.ibm.com) for Bug# 3496
//
//%/////////////////////////////////////////////////////////////////////////////

#include "CMPI_Version.h"

#include "CMPIProviderManager.h"

#include "CMPI_Object.h"
#include "CMPI_ContextArgs.h"
#include "CMPI_Instance.h"
#include "CMPI_ObjectPath.h"
#include "CMPI_Result.h"
#include "CMPI_SelectExp.h"

#include <Pegasus/Common/CIMMessage.h>
#include <Pegasus/Common/OperationContext.h>
#include <Pegasus/Common/Tracer.h>
#include <Pegasus/Common/StatisticalData.h>
#include <Pegasus/Common/Logger.h>
#include <Pegasus/Common/MessageLoader.h> //l10n
#include <Pegasus/Common/Constants.h>

#include <Pegasus/Config/ConfigManager.h>
#include <Pegasus/Server/CIMServer.h>

#include <Pegasus/Provider/CIMOMHandleQueryContext.h>
#include <Pegasus/ProviderManager2/ProviderType.h>
#include <Pegasus/ProviderManager2/ProviderName.h>
#include <Pegasus/ProviderManager2/CMPI/CMPIProviderModule.h>
#include <Pegasus/ProviderManager2/CMPI/CMPIProvider.h>
#include <Pegasus/ProviderManager2/ProviderManagerService.h>
//#include <Pegasus/ProviderManager2/Default/OperationResponseHandler.h>

#ifdef PEGASUS_ZOS_THREADLEVEL_SECURITY
#include <Pegasus/ProviderManager2/ProviderManagerzOS_inline.h>
#endif

PEGASUS_USING_STD;
PEGASUS_NAMESPACE_BEGIN

int _cmpi_trace=0;

#define DDD(x) if (_cmpi_trace) x;

CMPIProviderManager::IndProvTab    CMPIProviderManager::provTab;
CMPIProviderManager::IndSelectTab  CMPIProviderManager::selxTab;
CMPIProviderManager::ProvRegistrar CMPIProviderManager::provReg;
class CMPIPropertyList {
   char **props;
   int pCount;
  public:
   CMPIPropertyList(CIMPropertyList &propertyList) {
      if (!propertyList.isNull()) {
        Array<CIMName> p=propertyList.getPropertyNameArray();
        pCount=p.size();
        props = new char*[1+pCount];
        for (int i=0; i<pCount; i++) {
           props[i]=strdup(p[i].getString().getCString());
        }
        props[pCount]=NULL;
      }
      else props=NULL;
   }
   ~CMPIPropertyList() {
      if (props) {
         for (int i=0; i<pCount; i++)
            free(props[i]);
         delete [] props;
      }
   }
    char  **getList()  {
      return props;
   }
};

CMPIProviderManager::CMPIProviderManager(Mode m)
{
   mode=m;
   #ifdef PEGASUS_DEBUG
   if (getenv("PEGASUS_CMPI_TRACE")) _cmpi_trace=1;
   else _cmpi_trace=0;
   #endif
   _subscriptionInitComplete = false;
   DDD(cerr << "-- CMPI Provider Manager activated" << endl);
}

CMPIProviderManager::~CMPIProviderManager(void)
{
	/* Clean up the hash-tables */
    indProvRecord *prec=NULL;
	for (IndProvTab::Iterator i = provTab.start(); i; i++)
	{
        provTab.lookup(i.key(),prec);
		if (prec->handler)
			delete prec->handler;
		delete prec;
        provTab.remove(i.key());
        prec=NULL;
	}
	indSelectRecord *selx=NULL;
	for (IndSelectTab::Iterator i = selxTab.start(); i; i++)
	{
		selxTab.lookup(i.key(), selx);
		if (selx->eSelx)
			delete selx->eSelx;
        if (selx->qContext)
			delete selx->qContext;
		delete selx;
		selxTab.remove(i.key());
		selx=NULL;
	}
}

Boolean CMPIProviderManager::insertProvider(const ProviderName & name,
            const String &ns, const String &cn)
{
    String key(ns+String("::")+cn+String("::")+CIMValue(name.getCapabilitiesMask()).toString());
    return provReg.insert(key,name);
}


Message * CMPIProviderManager::processMessage(Message * request)
{
      PEG_METHOD_ENTER(TRC_PROVIDERMANAGER,
        "CMPIProviderManager::processMessage()");

    Message * response = 0;

    // pass the request message to a handler method based on message type
    switch(request->getType())
    {
    case CIM_GET_INSTANCE_REQUEST_MESSAGE:
        response = handleGetInstanceRequest(request);

        break;
    case CIM_ENUMERATE_INSTANCES_REQUEST_MESSAGE:
        response = handleEnumerateInstancesRequest(request);

        break;
    case CIM_ENUMERATE_INSTANCE_NAMES_REQUEST_MESSAGE:
        response = handleEnumerateInstanceNamesRequest(request);

        break;
    case CIM_CREATE_INSTANCE_REQUEST_MESSAGE:
        response = handleCreateInstanceRequest(request);

        break;
    case CIM_MODIFY_INSTANCE_REQUEST_MESSAGE:
        response = handleModifyInstanceRequest(request);

        break;
    case CIM_DELETE_INSTANCE_REQUEST_MESSAGE:
        response = handleDeleteInstanceRequest(request);

        break;
    case CIM_EXEC_QUERY_REQUEST_MESSAGE:
        response = handleExecQueryRequest(request);

        break;
    case CIM_ASSOCIATORS_REQUEST_MESSAGE:
        response = handleAssociatorsRequest(request);

        break;
    case CIM_ASSOCIATOR_NAMES_REQUEST_MESSAGE:
        response = handleAssociatorNamesRequest(request);

        break;
    case CIM_REFERENCES_REQUEST_MESSAGE:
        response = handleReferencesRequest(request);

        break;
    case CIM_REFERENCE_NAMES_REQUEST_MESSAGE:
        response = handleReferenceNamesRequest(request);

        break;
    case CIM_INVOKE_METHOD_REQUEST_MESSAGE:
        response = handleInvokeMethodRequest(request);

        break;
    case CIM_CREATE_SUBSCRIPTION_REQUEST_MESSAGE:
        response = handleCreateSubscriptionRequest(request);

        break;
/*    case CIM_MODIFY_SUBSCRIPTION_REQUEST_MESSAGE:
        response = handleModifySubscriptionRequest(request);

        break;
*/  case CIM_DELETE_SUBSCRIPTION_REQUEST_MESSAGE:
        response = handleDeleteSubscriptionRequest(request);

        break;
/*    case CIM_EXPORT_INDICATION_REQUEST_MESSAGE:
        response = handleExportIndicationRequest(request);
        break;
*/
    case CIM_DISABLE_MODULE_REQUEST_MESSAGE:
        response = handleDisableModuleRequest(request);

        break;
    case CIM_ENABLE_MODULE_REQUEST_MESSAGE:
        response = handleEnableModuleRequest(request);

        break;
    case CIM_STOP_ALL_PROVIDERS_REQUEST_MESSAGE:
        response = handleStopAllProvidersRequest(request);

        break;
    case CIM_INITIALIZE_PROVIDER_REQUEST_MESSAGE:
	response = handleInitializeProviderRequest(request);

	break;
    case CIM_SUBSCRIPTION_INIT_COMPLETE_REQUEST_MESSAGE:
        response = handleSubscriptionInitCompleteRequest (request);

        break;
    default:
        response = handleUnsupportedRequest(request);

        break;
    }

    PEG_METHOD_EXIT();

    return(response);
}

Boolean CMPIProviderManager::hasActiveProviders()
{
     return providerManager.hasActiveProviders();
}

void CMPIProviderManager::unloadIdleProviders()
{
     providerManager.unloadIdleProviders();
}


#define CHARS(cstring) (char*)(strlen(cstring)?(const char*)cstring:NULL)


#define HandlerIntroBase(type,type1,message,request,response,handler,respType) \
    CIM##type##RequestMessage * request = \
        dynamic_cast<CIM##type##RequestMessage *>(const_cast<Message *>(message)); \
    PEGASUS_ASSERT(request != 0); \
    CIM##type##ResponseMessage * response = \
        new CIM##type##ResponseMessage( \
        request->messageId, \
        CIMException(), \
        request->queueIds.copyAndPop() \
        respType \
    PEGASUS_ASSERT(response != 0); \
    response->setKey(request->getKey()); \
    response->setHttpMethod(request->getHttpMethod()); \
    type1##ResponseHandler handler(request, response);

#define VOIDINTRO );
#define NOVOIDINTRO(type) ,type);
#define METHODINTRO ,CIMValue(), Array<CIMParamValue>(), request->methodName );


#define HandlerIntroVoid(type,message,request,response,handler) \
     HandlerIntroBase(type,type,message,request,response,handler,VOIDINTRO)

#define HandlerIntroMethod(type,message,request,response,handler) \
     HandlerIntroBase(type,type,message,request,response,handler,METHODINTRO)

#define HandlerIntroInd(type,message,request,response,handler) \
     HandlerIntroBase(type,Operation,message,request,response,handler,VOIDINTRO)

#define HandlerIntroInit(type,message,request,response,handler) \
     HandlerIntroBase(type,Operation,message,request,response,handler,VOIDINTRO)

#define HandlerIntro(type,message,request,response,handler,respType) \
     HandlerIntroBase(type,type,message,request,response,handler,NOVOIDINTRO(respType))

#define HandlerCatch(handler) \
    catch(const CIMException & e)  \
    { PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4, \
                "Exception: " + e.getMessage()); \
        handler.setStatus(e.getCode(), e.getContentLanguages(), e.getMessage()); \
    } \
    catch(const Exception & e) \
    { PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4, \
                "Exception: " + e.getMessage()); \
        handler.setStatus(CIM_ERR_FAILED, e.getContentLanguages(), e.getMessage()); \
    } \
    catch(...) \
    { PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4, \
                "Exception: Unknown"); \
        handler.setStatus(CIM_ERR_FAILED, "Unknown error."); \
    }



Message * CMPIProviderManager::handleGetInstanceRequest(const Message * message)
{
    PEG_METHOD_ENTER(TRC_PROVIDERMANAGER,
        "CMPIProviderManager::handleGetInstanceRequest");

    HandlerIntro(GetInstance,message,request,response,handler,CIMInstance());

    try {
        Logger::put(Logger::STANDARD_LOG, System::CIMSERVER, Logger::TRACE,
            "CmpiProviderManager::handleGetInstanceRequest - Host name: $0  Name space: $1  Class name: $2",
            System::getHostName(),
            request->nameSpace.getString(),
            request->instanceName.getClassName().getString());

        // make target object path
        CIMObjectPath objectPath(
            System::getHostName(),
            request->nameSpace,
            request->instanceName.getClassName(),
            request->instanceName.getKeyBindings());

        Boolean remote=false;
        CMPIProvider::OpProviderHolder ph;

        // resolve provider name
        ProviderIdContainer pidc = request->operationContext.get(ProviderIdContainer::NAME);
        ProviderName name = _resolveProviderName(pidc);

        if ((remote=pidc.isRemoteNameSpace())) {
           ph = providerManager.getRemoteProvider(name.getLocation(), name.getLogicalName());
	}
	else {
        // get cached or load new provider module
           ph = providerManager.getProvider(name.getPhysicalName(), name.getLogicalName());
        }

        // convert arguments
        OperationContext context;

        context.insert(request->operationContext.get(IdentityContainer::NAME));
        context.insert(request->operationContext.get(AcceptLanguageListContainer::NAME));
        context.insert(request->operationContext.get(ContentLanguageListContainer::NAME));
        // forward request
        CMPIProvider & pr=ph.GetProvider();

        PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
            "Calling provider.getInstance: " + pr.getName());

        DDD(cerr<<"--- CMPIProviderManager::getInstance"<<endl);

        CMPIStatus rc={CMPI_RC_OK,NULL};
        CMPI_ContextOnStack eCtx(context);
        CMPI_ObjectPathOnStack eRef(objectPath);
        CMPI_ResultOnStack eRes(handler,&pr.broker);
        CMPI_ThreadContext thr(&pr.broker,&eCtx);

        CMPIPropertyList props(request->propertyList);

        CMPIFlags flgs=0;
        if (request->includeQualifiers) flgs|=CMPI_FLAG_IncludeQualifiers;
        if (request->includeClassOrigin) flgs|=CMPI_FLAG_IncludeClassOrigin;
        eCtx.ft->addEntry(&eCtx,CMPIInvocationFlags,(CMPIValue*)&flgs,CMPI_uint32);

        const IdentityContainer container =
           request->operationContext.get(IdentityContainer::NAME);
        eCtx.ft->addEntry(&eCtx,
                          CMPIPrincipal,
                          (CMPIValue*)(const char*)container.getUserName().getCString(),
                          CMPI_chars);

        const AcceptLanguageListContainer accept_language=            
           request->operationContext.get(AcceptLanguageListContainer::NAME);     
       const AcceptLanguages acceptLangs = accept_language.getLanguages();

        eCtx.ft->addEntry(&eCtx,  
                          "AcceptLanguage",                         
                          (CMPIValue *)(const char*)acceptLangs.toString().getCString(),
                          CMPI_chars);       
 
        if (remote) {
           CString info=pidc.getRemoteInfo().getCString();
           eCtx.ft->addEntry(&eCtx,"CMPIRRemoteInfo",(CMPIValue*)(const char*)info,CMPI_chars);
        }

        CMPIProvider::pm_service_op_lock op_lock(&pr);

#ifdef PEGASUS_ZOS_THREADLEVEL_SECURITY
                CIMGetInstanceRequestMessage * req = dynamic_cast<CIMGetInstanceRequestMessage *>(const_cast<Message *>(message));
                int err_num=enablePThreadSecurity(context);
                if (err_num!=0)
                {
                        // need a new CIMException for this
                        throw CIMException(CIM_ERR_ACCESS_DENIED,String(strerror(err_num)));
                }
#endif

        STAT_GETSTARTTIME;

        rc=pr.miVector.instMI->ft->getInstance
        (pr.miVector.instMI,&eCtx,&eRes,&eRef,(const char **)props.getList());

        STAT_PMS_PROVIDEREND;

#ifdef PEGASUS_ZOS_THREADLEVEL_SECURITY
                disablePThreadSecurity();
#endif
        if (rc.rc!=CMPI_RC_OK)
           throw CIMException((CIMStatusCode)rc.rc,
               rc.msg ? CMGetCharsPtr(rc.msg,NULL) : String::EMPTY);
    }
    HandlerCatch(handler);

    PEG_METHOD_EXIT();

    STAT_COPYDISPATCHER

    return(response);
}

Message * CMPIProviderManager::handleEnumerateInstancesRequest(const Message * message)
{
    PEG_METHOD_ENTER(TRC_PROVIDERMANAGER,
        "CMPIProviderManager::handleEnumerateInstanceRequest");

    HandlerIntro(EnumerateInstances,message,request,response,
                 handler,Array<CIMInstance>());
    try {
      Logger::put(Logger::STANDARD_LOG, System::CIMSERVER, Logger::TRACE,
            "CMPIProviderManager::handleEnumerateInstancesRequest - Host name: $0  Name space: $1  Class name: $2",
            System::getHostName(),
            request->nameSpace.getString(),
            request->className.getString());

        // make target object path
        CIMObjectPath objectPath(
            System::getHostName(),
            request->nameSpace,
            request->className);

        Boolean remote=false;
        CMPIProvider::OpProviderHolder ph;

        // resolve provider name
        ProviderIdContainer pidc = request->operationContext.get(ProviderIdContainer::NAME);
        ProviderName name = _resolveProviderName(pidc);

        if ((remote=pidc.isRemoteNameSpace())) {
           ph = providerManager.getRemoteProvider(name.getLocation(), name.getLogicalName());
	}
	else {
        // get cached or load new provider module
           ph = providerManager.getProvider(name.getPhysicalName(), name.getLogicalName());
        }

        // convert arguments
        OperationContext context;

        context.insert(request->operationContext.get(IdentityContainer::NAME));
        context.insert(request->operationContext.get(AcceptLanguageListContainer::NAME));
        context.insert(request->operationContext.get(ContentLanguageListContainer::NAME));

        CIMPropertyList propertyList(request->propertyList);

        // forward request
        CMPIProvider & pr=ph.GetProvider();

        PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
            "Calling provider.enumerateInstances: " + pr.getName());

        DDD(cerr<<"--- CMPIProviderManager::enumerateInstances"<<endl);

        CMPIStatus rc={CMPI_RC_OK,NULL};
        CMPI_ContextOnStack eCtx(context);
        CMPI_ObjectPathOnStack eRef(objectPath);
        CMPI_ResultOnStack eRes(handler,&pr.broker);
        CMPI_ThreadContext thr(&pr.broker,&eCtx);

        CMPIPropertyList props(propertyList);

        CMPIFlags flgs=0;
        if (request->includeQualifiers) flgs|=CMPI_FLAG_IncludeQualifiers;
        if (request->includeClassOrigin) flgs|=CMPI_FLAG_IncludeClassOrigin;
        eCtx.ft->addEntry(&eCtx,CMPIInvocationFlags,(CMPIValue*)&flgs,CMPI_uint32);

        const IdentityContainer container =
           request->operationContext.get(IdentityContainer::NAME);
        eCtx.ft->addEntry(&eCtx,
                          CMPIPrincipal,
                          (CMPIValue*)(const char*)container.getUserName().getCString(),
                          CMPI_chars);

        const AcceptLanguageListContainer accept_language=            
           request->operationContext.get(AcceptLanguageListContainer::NAME);     
 
       const AcceptLanguages acceptLangs = accept_language.getLanguages();
        eCtx.ft->addEntry(&eCtx,  
                          "AcceptLanguage",                         
                          (CMPIValue *)(const char*)acceptLangs.toString().getCString(),
                          CMPI_chars);       
        if (remote) {
           CString info=pidc.getRemoteInfo().getCString();
           eCtx.ft->addEntry(&eCtx,"CMPIRRemoteInfo",(CMPIValue*)(const char*)info,CMPI_chars);
        }

        CMPIProvider::pm_service_op_lock op_lock(&pr);

#ifdef PEGASUS_ZOS_THREADLEVEL_SECURITY
                CIMEnumerateInstancesRequestMessage * req = dynamic_cast<CIMEnumerateInstancesRequestMessage *>(const_cast<Message *>(message));
                int err_num=enablePThreadSecurity(context);
                if (err_num!=0)
                {
                        // need a new CIMException for this
                        throw CIMException(CIM_ERR_ACCESS_DENIED,String(strerror(err_num)));
                }
#endif

        STAT_GETSTARTTIME;

        rc=pr.miVector.instMI->ft->enumInstances
	  (pr.miVector.instMI,&eCtx,&eRes,&eRef,(const char **)props.getList());

        STAT_PMS_PROVIDEREND;

#ifdef PEGASUS_ZOS_THREADLEVEL_SECURITY
                disablePThreadSecurity();
#endif

        if (rc.rc!=CMPI_RC_OK)
        throw CIMException((CIMStatusCode)rc.rc,
               rc.msg ? CMGetCharsPtr(rc.msg,NULL) : String::EMPTY);

    }
    HandlerCatch(handler);

    PEG_METHOD_EXIT();

    STAT_COPYDISPATCHER

    return(response);
}

Message * CMPIProviderManager::handleEnumerateInstanceNamesRequest(const Message * message)
{
    PEG_METHOD_ENTER(TRC_PROVIDERMANAGER, "CMPIProviderManager::handleEnumerateInstanceNamesRequest");

    HandlerIntro(EnumerateInstanceNames,message,request,response,
                 handler,Array<CIMObjectPath>());
    try {
        Logger::put(Logger::STANDARD_LOG, System::CIMSERVER, Logger::TRACE,
            "CMPIProviderManager::handleEnumerateInstanceNamesRequest - Host name: $0  Name space: $1  Class name: $2",
            System::getHostName(),
            request->nameSpace.getString(),
            request->className.getString());

       // make target object path
        CIMObjectPath objectPath(
            System::getHostName(),
            request->nameSpace,
            request->className);

        Boolean remote=false;
        CMPIProvider::OpProviderHolder ph;

        // resolve provider name
        ProviderIdContainer pidc = request->operationContext.get(ProviderIdContainer::NAME);
        ProviderName name = _resolveProviderName(pidc);

        if ((remote=pidc.isRemoteNameSpace())) {
           ph = providerManager.getRemoteProvider(name.getLocation(), name.getLogicalName());
        }
        else {
        // get cached or load new provider module
           ph = providerManager.getProvider(name.getPhysicalName(), name.getLogicalName());
        }

        // convert arguments
        OperationContext context;

        context.insert(request->operationContext.get(IdentityContainer::NAME));
        context.insert(request->operationContext.get(AcceptLanguageListContainer::NAME));
        context.insert(request->operationContext.get(ContentLanguageListContainer::NAME));
        CMPIProvider & pr=ph.GetProvider();

        PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
            "Calling provider.enumerateInstanceNames: " + pr.getName());

        DDD(cerr<<"--- CMPIProviderManager::enumerateInstanceNames"<<endl);

        CMPIStatus rc={CMPI_RC_OK,NULL};
        CMPI_ContextOnStack eCtx(context);
        CMPI_ObjectPathOnStack eRef(objectPath);
        CMPI_ResultOnStack eRes(handler,&pr.broker);
        CMPI_ThreadContext thr(&pr.broker,&eCtx);

        CMPIFlags flgs=0;
        eCtx.ft->addEntry(&eCtx,CMPIInvocationFlags,(CMPIValue*)&flgs,CMPI_uint32);

        const IdentityContainer container =
           request->operationContext.get(IdentityContainer::NAME);
        eCtx.ft->addEntry(&eCtx,
                          CMPIPrincipal,
                          (CMPIValue*)(const char*)container.getUserName().getCString(),
                          CMPI_chars);

        const AcceptLanguageListContainer accept_language=            
           request->operationContext.get(AcceptLanguageListContainer::NAME);     
 
       const AcceptLanguages acceptLangs = accept_language.getLanguages();
        eCtx.ft->addEntry(&eCtx,  
                          "AcceptLanguage",                         
                          (CMPIValue *)(const char*)acceptLangs.toString().getCString(),
                          CMPI_chars);       
        if (remote) {
           CString info=pidc.getRemoteInfo().getCString();
           eCtx.ft->addEntry(&eCtx,"CMPIRRemoteInfo",(CMPIValue*)(const char*)info,CMPI_chars);
        }

        CMPIProvider::pm_service_op_lock op_lock(&pr);

#ifdef PEGASUS_ZOS_THREADLEVEL_SECURITY
                CIMEnumerateInstanceNamesRequestMessage * req = dynamic_cast<CIMEnumerateInstanceNamesRequestMessage *>(const_cast<Message *>(message));
                int err_num=enablePThreadSecurity(context);
                if (err_num!=0)
                {
                        // need a new CIMException for this
                        throw CIMException(CIM_ERR_ACCESS_DENIED,String(strerror(err_num)));
                }
#endif
        STAT_GETSTARTTIME;

        rc=pr.miVector.instMI->ft->enumInstanceNames(pr.miVector.instMI,&eCtx,&eRes,&eRef);

        STAT_PMS_PROVIDEREND;

#ifdef PEGASUS_ZOS_THREADLEVEL_SECURITY
                disablePThreadSecurity();
#endif

        if (rc.rc!=CMPI_RC_OK)
           throw CIMException((CIMStatusCode)rc.rc,
               rc.msg ? CMGetCharsPtr(rc.msg,NULL) : String::EMPTY);
    }
    HandlerCatch(handler);


    PEG_METHOD_EXIT();

    STAT_COPYDISPATCHER

    return(response);
}

Message * CMPIProviderManager::handleCreateInstanceRequest(const Message * message)
{
    PEG_METHOD_ENTER(TRC_PROVIDERMANAGER,
       "CMPIProviderManager::handleCreateInstanceRequest");

    HandlerIntro(CreateInstance,message,request,response,
                 handler,CIMObjectPath());
    try {
        Logger::put(Logger::STANDARD_LOG, System::CIMSERVER, Logger::TRACE,
            "CMPIProviderManager::handleCreateInstanceRequest - Host name: $0  Name space: $1  Class name: $2",
            System::getHostName(),
            request->nameSpace.getString(),
            request->newInstance.getPath().getClassName().getString());

        // make target object path
        CIMObjectPath objectPath(
            System::getHostName(),
            request->nameSpace,
            request->newInstance.getPath().getClassName(),
            request->newInstance.getPath().getKeyBindings());
	request->newInstance.setPath(objectPath);

        Boolean remote=false;
        CMPIProvider::OpProviderHolder ph;

        // resolve provider name
        ProviderIdContainer pidc = request->operationContext.get(ProviderIdContainer::NAME);
        ProviderName name = _resolveProviderName(pidc);

        if ((remote=pidc.isRemoteNameSpace())) {
           ph = providerManager.getRemoteProvider(name.getLocation(), name.getLogicalName());
	}
	else {
        // get cached or load new provider module
           ph = providerManager.getProvider(name.getPhysicalName(), name.getLogicalName());
        }

        // convert arguments
        OperationContext context;

        context.insert(request->operationContext.get(IdentityContainer::NAME));
        context.insert(request->operationContext.get(AcceptLanguageListContainer::NAME));
        context.insert(request->operationContext.get(ContentLanguageListContainer::NAME));
        // forward request
        CMPIProvider & pr=ph.GetProvider();

        PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
            "Calling provider.createInstance: " +
            ph.GetProvider().getName());

        DDD(cerr<<"--- CMPIProviderManager::createInstances"<<endl);
        CMPIStatus rc={CMPI_RC_OK,NULL};
        CMPI_ContextOnStack eCtx(context);
        CMPI_ObjectPathOnStack eRef(objectPath);
        CMPI_ResultOnStack eRes(handler,&pr.broker);
        CMPI_InstanceOnStack eInst(request->newInstance);
        CMPI_ThreadContext thr(&pr.broker,&eCtx);

        CMPIFlags flgs=0;
        eCtx.ft->addEntry(&eCtx,CMPIInvocationFlags,(CMPIValue*)&flgs,CMPI_uint32);

        const IdentityContainer container =
           request->operationContext.get(IdentityContainer::NAME);
        eCtx.ft->addEntry(&eCtx,
                          CMPIPrincipal,
                          (CMPIValue*)(const char*)container.getUserName().getCString(),
                          CMPI_chars);

        const AcceptLanguageListContainer accept_language=            
           request->operationContext.get(AcceptLanguageListContainer::NAME);     
 
       const AcceptLanguages acceptLangs = accept_language.getLanguages();
        eCtx.ft->addEntry(&eCtx,  
                          "AcceptLanguage",                         
                          (CMPIValue *)(const char*)acceptLangs.toString().getCString(),
                          CMPI_chars);       
        if (remote) {
           CString info=pidc.getRemoteInfo().getCString();
           eCtx.ft->addEntry(&eCtx,"CMPIRRemoteInfo",(CMPIValue*)(const char*)info,CMPI_chars);
        }

        CMPIProvider::pm_service_op_lock op_lock(&pr);

#ifdef PEGASUS_ZOS_THREADLEVEL_SECURITY
                CIMCreateInstanceRequestMessage * req = dynamic_cast<CIMCreateInstanceRequestMessage *>(const_cast<Message *>(message));
                int err_num=enablePThreadSecurity(context);
                if (err_num!=0)
                {
                        // need a new CIMException for this
                        throw CIMException(CIM_ERR_ACCESS_DENIED,String(strerror(err_num)));
                }
#endif

        STAT_GETSTARTTIME;

        rc=pr.miVector.instMI->ft->createInstance
           (pr.miVector.instMI,&eCtx,&eRes,&eRef,&eInst);

        STAT_PMS_PROVIDEREND;

#ifdef PEGASUS_ZOS_THREADLEVEL_SECURITY
                disablePThreadSecurity();
#endif

        if (rc.rc!=CMPI_RC_OK)
           throw CIMException((CIMStatusCode)rc.rc,
               rc.msg ? CMGetCharsPtr(rc.msg,NULL) : String::EMPTY);
    }
    HandlerCatch(handler);

    PEG_METHOD_EXIT();

    STAT_COPYDISPATCHER

    return(response);
}

Message * CMPIProviderManager::handleModifyInstanceRequest(const Message * message)
{
    PEG_METHOD_ENTER(TRC_PROVIDERMANAGER,
       "CMPIProviderManager::handleModifyInstanceRequest");

    HandlerIntroVoid(ModifyInstance,message,request,response,
                 handler);
    try {
        Logger::put(Logger::STANDARD_LOG, System::CIMSERVER, Logger::TRACE,
            "CMPIProviderManager::handleModifyInstanceRequest - Host name: $0  Name space: $1  Class name: $2",
            System::getHostName(),
            request->nameSpace.getString(),
            request->modifiedInstance.getPath().getClassName().getString());

        // make target object path
        CIMObjectPath objectPath(
            System::getHostName(),
            request->nameSpace,
            request->modifiedInstance.getPath ().getClassName(),
            request->modifiedInstance.getPath ().getKeyBindings());

        Boolean remote=false;
        CMPIProvider::OpProviderHolder ph;

        // resolve provider name
        ProviderIdContainer pidc = request->operationContext.get(ProviderIdContainer::NAME);
        ProviderName name = _resolveProviderName(pidc);

        if ((remote=pidc.isRemoteNameSpace())) {
           ph = providerManager.getRemoteProvider(name.getLocation(), name.getLogicalName());
        }
        else {
        // get cached or load new provider module
           ph = providerManager.getProvider(name.getPhysicalName(), name.getLogicalName());
        }

        // convert arguments
        OperationContext context;

        context.insert(request->operationContext.get(IdentityContainer::NAME));
        context.insert(request->operationContext.get(AcceptLanguageListContainer::NAME));
        context.insert(request->operationContext.get(ContentLanguageListContainer::NAME));
        // forward request
        CMPIProvider & pr=ph.GetProvider();

        PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
            "Calling provider.modifyInstance: " + pr.getName());

        DDD(cerr<<"--- CMPIProviderManager::modifyInstance"<<endl);

        CMPIStatus rc={CMPI_RC_OK,NULL};
        CMPI_ContextOnStack eCtx(context);
        CMPI_ObjectPathOnStack eRef(objectPath);
        CMPI_ResultOnStack eRes(handler,&pr.broker);
        CMPI_InstanceOnStack eInst(request->modifiedInstance);
        CMPI_ThreadContext thr(&pr.broker,&eCtx);

        CMPIPropertyList props(request->propertyList);

        CMPIFlags flgs=0;
        if (request->includeQualifiers) flgs|=CMPI_FLAG_IncludeQualifiers;
        eCtx.ft->addEntry(&eCtx,CMPIInvocationFlags,(CMPIValue*)&flgs,CMPI_uint32);

        const IdentityContainer container =
           request->operationContext.get(IdentityContainer::NAME);
        eCtx.ft->addEntry(&eCtx,
                          CMPIPrincipal,
                          (CMPIValue*)(const char*)container.getUserName().getCString(),
                          CMPI_chars);

        const AcceptLanguageListContainer accept_language=            
           request->operationContext.get(AcceptLanguageListContainer::NAME);     
       const AcceptLanguages acceptLangs = accept_language.getLanguages();
        eCtx.ft->addEntry(&eCtx,  
                          "AcceptLanguage",                         
                          (CMPIValue *)(const char*)acceptLangs.toString().getCString(),
                          CMPI_chars);       
 
        if (remote) {
           CString info=pidc.getRemoteInfo().getCString();
           eCtx.ft->addEntry(&eCtx,"CMPIRRemoteInfo",(CMPIValue*)(const char*)info,CMPI_chars);
        }

        CMPIProvider::pm_service_op_lock op_lock(&pr);

#ifdef PEGASUS_ZOS_THREADLEVEL_SECURITY
                CIMModifyInstanceRequestMessage * req = dynamic_cast<CIMModifyInstanceRequestMessage *>(const_cast<Message *>(message));
                int err_num=enablePThreadSecurity(context);
                if (err_num!=0)
                {
                        // need a new CIMException for this
                        throw CIMException(CIM_ERR_ACCESS_DENIED,String(strerror(err_num)));
                }
#endif

        STAT_GETSTARTTIME;

        rc=pr.miVector.instMI->ft->modifyInstance
	  (pr.miVector.instMI,&eCtx,&eRes,&eRef,&eInst,(const char **)props.getList());

        STAT_PMS_PROVIDEREND;

#ifdef PEGASUS_ZOS_THREADLEVEL_SECURITY
                disablePThreadSecurity();
#endif

        if (rc.rc!=CMPI_RC_OK)
           throw CIMException((CIMStatusCode)rc.rc,
               rc.msg ? CMGetCharsPtr(rc.msg,NULL) : String::EMPTY);
    }
    HandlerCatch(handler);

    PEG_METHOD_EXIT();

    STAT_COPYDISPATCHER

    return(response);
}

Message * CMPIProviderManager::handleDeleteInstanceRequest(const Message * message)
{
    PEG_METHOD_ENTER(TRC_PROVIDERMANAGER,
       "CMPIProviderManager::handleDeleteInstanceRequest");

    HandlerIntroVoid(DeleteInstance,message,request,response,
                 handler);
    try {
        Logger::put(Logger::STANDARD_LOG, System::CIMSERVER, Logger::TRACE,
            "CMPIProviderManager::handleDeleteInstanceRequest - Host name: $0  Name space: $1  Class name: $2",
            System::getHostName(),
            request->nameSpace.getString(),
            request->instanceName.getClassName().getString());

        // make target object path
        CIMObjectPath objectPath(
            System::getHostName(),
            request->nameSpace,
            request->instanceName.getClassName(),
            request->instanceName.getKeyBindings());

        Boolean remote=false;
        CMPIProvider::OpProviderHolder ph;

        // resolve provider name
        ProviderIdContainer pidc = request->operationContext.get(ProviderIdContainer::NAME);
        ProviderName name = _resolveProviderName(pidc);

        if ((remote=pidc.isRemoteNameSpace())) {
           ph = providerManager.getRemoteProvider(name.getLocation(), name.getLogicalName());
        }
        else {
        // get cached or load new provider module
           ph = providerManager.getProvider(name.getPhysicalName(), name.getLogicalName());
        }

        // convert arguments
        OperationContext context;

        context.insert(request->operationContext.get(IdentityContainer::NAME));
        context.insert(request->operationContext.get(AcceptLanguageListContainer::NAME));
        context.insert(request->operationContext.get(ContentLanguageListContainer::NAME));
        // forward request
        CMPIProvider & pr=ph.GetProvider();

        PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
            "Calling provider.deleteInstance: " + pr.getName());

        CMPIStatus rc={CMPI_RC_OK,NULL};
        CMPI_ContextOnStack eCtx(context);
        CMPI_ObjectPathOnStack eRef(objectPath);
        CMPI_ResultOnStack eRes(handler,&pr.broker);
        CMPI_ThreadContext thr(&pr.broker,&eCtx);

        CMPIFlags flgs=0;
        eCtx.ft->addEntry(&eCtx,CMPIInvocationFlags,(CMPIValue*)&flgs,CMPI_uint32);

        const IdentityContainer container =
           request->operationContext.get(IdentityContainer::NAME);
        eCtx.ft->addEntry(&eCtx,
                          CMPIPrincipal,
                          (CMPIValue*)(const char*)container.getUserName().getCString(),
                          CMPI_chars);

        const AcceptLanguageListContainer accept_language=            
           request->operationContext.get(AcceptLanguageListContainer::NAME);     
 
       const AcceptLanguages acceptLangs = accept_language.getLanguages();
        eCtx.ft->addEntry(&eCtx,  
                          "AcceptLanguage",                         
                          (CMPIValue *)(const char*)acceptLangs.toString().getCString(),
                          CMPI_chars);       
        if (remote) {
           CString info=pidc.getRemoteInfo().getCString();
           eCtx.ft->addEntry(&eCtx,"CMPIRRemoteInfo",(CMPIValue*)(const char*)info,CMPI_chars);
        }

        CMPIProvider::pm_service_op_lock op_lock(&pr);

#ifdef PEGASUS_ZOS_THREADLEVEL_SECURITY
                CIMDeleteInstanceRequestMessage * req = dynamic_cast<CIMDeleteInstanceRequestMessage *>(const_cast<Message *>(message));
                int err_num=enablePThreadSecurity(context);
                if (err_num!=0)
                {
                        // need a new CIMException for this
                        throw CIMException(CIM_ERR_ACCESS_DENIED,String(strerror(err_num)));
                }
#endif

        STAT_GETSTARTTIME;

        rc=pr.miVector.instMI->ft->deleteInstance
           (pr.miVector.instMI,&eCtx,&eRes,&eRef);

        STAT_PMS_PROVIDEREND;

#ifdef PEGASUS_ZOS_THREADLEVEL_SECURITY
                disablePThreadSecurity();
#endif

        if (rc.rc!=CMPI_RC_OK)
           throw CIMException((CIMStatusCode)rc.rc,
               rc.msg ? CMGetCharsPtr(rc.msg,NULL) : String::EMPTY);
    }
    HandlerCatch(handler);

    PEG_METHOD_EXIT();

    STAT_COPYDISPATCHER

    return(response);
}

Message * CMPIProviderManager::handleExecQueryRequest(const Message * message)
{
    PEG_METHOD_ENTER(TRC_PROVIDERMANAGER,
       "CMPIProviderManager::handleExecQueryRequest");

    HandlerIntro(ExecQuery,message,request,response,
                 handler,Array<CIMObject>());

    try {
      Logger::put(Logger::STANDARD_LOG, System::CIMSERVER, Logger::TRACE,
            "CMPIProviderManager::ExecQueryRequest - Host name: $0  Name space: $1  Class name: $2",
            System::getHostName(),
            request->nameSpace.getString(),
            request->className.getString());

        // make target object path
        CIMObjectPath objectPath(
            System::getHostName(),
            request->nameSpace,
            request->className);

        Boolean remote=false;

        CMPIProvider::OpProviderHolder ph;

        // resolve provider name
        ProviderIdContainer pidc = request->operationContext.get(ProviderIdContainer::NAME);
        ProviderName name = _resolveProviderName(pidc);

        if ((remote=pidc.isRemoteNameSpace())) {
           ph = providerManager.getRemoteProvider(name.getLocation(), name.getLogicalName());
        }
        else {
        // get cached or load new provider module
           ph = providerManager.getProvider(name.getPhysicalName(), name.getLogicalName());
        }

        // convert arguments
        OperationContext context;

        context.insert(request->operationContext.get(IdentityContainer::NAME));
        context.insert(request->operationContext.get(AcceptLanguageListContainer::NAME));
        context.insert(request->operationContext.get(ContentLanguageListContainer::NAME));

        // forward request
        CMPIProvider & pr=ph.GetProvider();

        PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
            "Calling provider.execQuery: " + pr.getName());

        DDD(cerr<<"--- CMPIProviderManager::execQuery"<<endl);

        const char **props=NULL;

        CMPIStatus rc={CMPI_RC_OK,NULL};
        CMPI_ContextOnStack eCtx(context);
        CMPI_ObjectPathOnStack eRef(objectPath);
        CMPI_ResultOnStack eRes(handler,&pr.broker);
        CMPI_ThreadContext thr(&pr.broker,&eCtx);

        const CString queryLan=request->queryLanguage.getCString();
        const CString query=request->query.getCString();

        CMPIFlags flgs=0;
        eCtx.ft->addEntry(&eCtx,CMPIInvocationFlags,(CMPIValue*)&flgs,CMPI_uint32);

        const IdentityContainer container =
           request->operationContext.get(IdentityContainer::NAME);
        eCtx.ft->addEntry(&eCtx,
                          CMPIPrincipal,
                          (CMPIValue*)(const char*)container.getUserName().getCString(),
                          CMPI_chars);
		eCtx.ft->addEntry(&eCtx,
						  CMPIInitNameSpace,
						  (CMPIValue*)(const char*)request->nameSpace.getString().getCString(),
						  CMPI_chars);
        const AcceptLanguageListContainer accept_language=            
           request->operationContext.get(AcceptLanguageListContainer::NAME);     
 
       const AcceptLanguages acceptLangs = accept_language.getLanguages();
        eCtx.ft->addEntry(&eCtx,  
                          "AcceptLanguage",                         
                          (CMPIValue *)(const char*)acceptLangs.toString().getCString(),
                          CMPI_chars);       
        if (remote) {
           CString info=pidc.getRemoteInfo().getCString();
           eCtx.ft->addEntry(&eCtx,"CMPIRRemoteInfo",(CMPIValue*)(const char*)info,CMPI_chars);
        }

        CMPIProvider::pm_service_op_lock op_lock(&pr);

#ifdef PEGASUS_ZOS_THREADLEVEL_SECURITY
                CIMExecQueryRequestMessage * req = dynamic_cast<CIMExecQueryRequestMessage *>(const_cast<Message *>(message));
                int err_num=enablePThreadSecurity(context);
                if (err_num!=0)
                {
                        // need a new CIMException for this
                        throw CIMException(CIM_ERR_ACCESS_DENIED,String(strerror(err_num)));
                }
#endif

        STAT_GETSTARTTIME;

        rc=pr.miVector.instMI->ft->execQuery
           (pr.miVector.instMI,&eCtx,&eRes,&eRef,CHARS(queryLan),CHARS(query));

        STAT_PMS_PROVIDEREND;

#ifdef PEGASUS_ZOS_THREADLEVEL_SECURITY
                disablePThreadSecurity();
#endif

        if (rc.rc!=CMPI_RC_OK)
           throw CIMException((CIMStatusCode)rc.rc,
               rc.msg ? CMGetCharsPtr(rc.msg,NULL) : String::EMPTY);


    }
    HandlerCatch(handler);

    PEG_METHOD_EXIT();

    STAT_COPYDISPATCHER

    return(response);
}

Message * CMPIProviderManager::handleAssociatorsRequest(const Message * message)
{
    PEG_METHOD_ENTER(TRC_PROVIDERMANAGER,
       "CMPIProviderManager::handleAssociatorsRequest");

    HandlerIntro(Associators,message,request,response,
                 handler,Array<CIMObject>());
    try {
        Logger::put(Logger::STANDARD_LOG, System::CIMSERVER, Logger::TRACE,
            "CMPIProviderManager::handleAssociatorsRequest - Host name: $0  Name space: $1  Class name: $2",
            System::getHostName(),
            request->nameSpace.getString(),
            request->objectName.getClassName().getString());

        // make target object path
        CIMObjectPath objectPath(
            System::getHostName(),
            request->nameSpace,
            request->objectName.getClassName());

        objectPath.setKeyBindings(request->objectName.getKeyBindings());

        CIMObjectPath assocPath(
            System::getHostName(),
            request->nameSpace,
            request->assocClass.getString());

        Boolean remote=false;

        CMPIProvider::OpProviderHolder ph;

        // resolve provider name
        ProviderIdContainer pidc = request->operationContext.get(ProviderIdContainer::NAME);
        ProviderName name = _resolveProviderName(pidc);

        if ((remote=pidc.isRemoteNameSpace())) {
           ph = providerManager.getRemoteProvider(name.getLocation(), name.getLogicalName());
        }
        else {
        // get cached or load new provider module
           ph = providerManager.getProvider(name.getPhysicalName(), name.getLogicalName());
        }

       // convert arguments
        OperationContext context;

        context.insert(request->operationContext.get(IdentityContainer::NAME));
        context.insert(request->operationContext.get(AcceptLanguageListContainer::NAME));
        context.insert(request->operationContext.get(ContentLanguageListContainer::NAME));

        // forward request
        CMPIProvider & pr=ph.GetProvider();

        PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
            "Calling provider.associators: " + pr.getName());

        DDD(cerr<<"--- CMPIProviderManager::associators"<<" role: >"<<request->role<<"< aCls "<<
        request->assocClass<<endl);

        CMPIStatus rc={CMPI_RC_OK,NULL};
        CMPI_ContextOnStack eCtx(context);
        CMPI_ObjectPathOnStack eRef(objectPath);
        CMPI_ResultOnStack eRes(handler,&pr.broker);
        CMPI_ThreadContext thr(&pr.broker,&eCtx);
        const CString aClass=request->assocClass.getString().getCString();
        const CString rClass=request->resultClass.getString().getCString();
        const CString rRole=request->role.getCString();
        const CString resRole=request->resultRole.getCString();

        CMPIPropertyList props(request->propertyList);

        CMPIFlags flgs=0;
        if (request->includeQualifiers) flgs|=CMPI_FLAG_IncludeQualifiers;
        if (request->includeClassOrigin) flgs|=CMPI_FLAG_IncludeClassOrigin;
        eCtx.ft->addEntry(&eCtx,CMPIInvocationFlags,(CMPIValue*)&flgs,CMPI_uint32);

        const IdentityContainer container =
           request->operationContext.get(IdentityContainer::NAME);
        eCtx.ft->addEntry(&eCtx,
                          CMPIPrincipal,
                          (CMPIValue*)(const char*)container.getUserName().getCString(),
                          CMPI_chars);

        const AcceptLanguageListContainer accept_language=            
           request->operationContext.get(AcceptLanguageListContainer::NAME);     
       const AcceptLanguages acceptLangs = accept_language.getLanguages();
        eCtx.ft->addEntry(&eCtx,  
                          "AcceptLanguage",                         
                          (CMPIValue *)(const char*)acceptLangs.toString().getCString(),
                          CMPI_chars);       
 
        if (remote) {
           CString info=pidc.getRemoteInfo().getCString();
           eCtx.ft->addEntry(&eCtx,"CMPIRRemoteInfo",(CMPIValue*)(const char*)info,CMPI_chars);
        }

        CMPIProvider::pm_service_op_lock op_lock(&pr);

#ifdef PEGASUS_ZOS_THREADLEVEL_SECURITY
                CIMAssociatorsRequestMessage * req = dynamic_cast<CIMAssociatorsRequestMessage *>(const_cast<Message *>(message));
                int err_num=enablePThreadSecurity(context);
                if (err_num!=0)
                {
                        // need a new CIMException for this
                        throw CIMException(CIM_ERR_ACCESS_DENIED,String(strerror(err_num)));
                }
#endif

        STAT_GETSTARTTIME;

        rc=pr.miVector.assocMI->ft->associators(
                         pr.miVector.assocMI,&eCtx,&eRes,&eRef,CHARS(aClass),
                         CHARS(rClass),CHARS(rRole),CHARS(resRole),(const char **)props.getList());

        STAT_PMS_PROVIDEREND;

#ifdef PEGASUS_ZOS_THREADLEVEL_SECURITY
                disablePThreadSecurity();
#endif

        if (rc.rc!=CMPI_RC_OK)
           throw CIMException((CIMStatusCode)rc.rc,
               rc.msg ? CMGetCharsPtr(rc.msg,NULL) : String::EMPTY);
    }
    HandlerCatch(handler);

    PEG_METHOD_EXIT();

    STAT_COPYDISPATCHER

    return(response);
}

Message * CMPIProviderManager::handleAssociatorNamesRequest(const Message * message)
{
    PEG_METHOD_ENTER(TRC_PROVIDERMANAGER,
       "CMPIProviderManager::handleAssociatorNamesRequest");

    HandlerIntro(AssociatorNames,message,request,response,
                 handler,Array<CIMObjectPath>());
    try {
        Logger::put(Logger::STANDARD_LOG, System::CIMSERVER, Logger::TRACE,
            "CMPIProviderManager::handleAssociatorNamesRequest - Host name: $0  Name space: $1  Class name: $2",
            System::getHostName(),
            request->nameSpace.getString(),
            request->objectName.getClassName().getString());

        // make target object path
        CIMObjectPath objectPath(
            System::getHostName(),
            request->nameSpace,
            request->objectName.getClassName());

        objectPath.setKeyBindings(request->objectName.getKeyBindings());

        CIMObjectPath assocPath(
            System::getHostName(),
            request->nameSpace,
            request->assocClass.getString());

        Boolean remote=false;
        CMPIProvider::OpProviderHolder ph;

        // resolve provider name
        ProviderIdContainer pidc = request->operationContext.get(ProviderIdContainer::NAME);
        ProviderName name = _resolveProviderName(pidc);

        if ((remote=pidc.isRemoteNameSpace())) {
           ph = providerManager.getRemoteProvider(name.getLocation(), name.getLogicalName());
        }
        else {
        // get cached or load new provider module
           ph = providerManager.getProvider(name.getPhysicalName(), name.getLogicalName());
        }

        // convert arguments
        OperationContext context;

        context.insert(request->operationContext.get(IdentityContainer::NAME));
        context.insert(request->operationContext.get(AcceptLanguageListContainer::NAME));
        context.insert(request->operationContext.get(ContentLanguageListContainer::NAME));

        // forward request
        CMPIProvider & pr=ph.GetProvider();

        PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
            "Calling provider.associatorNames: " + pr.getName());

        DDD(cerr<<"--- CMPIProviderManager::associatorNames"<<" role: >"<<request->role<<"< aCls "<<
           request->assocClass<<endl);

        CMPIStatus rc={CMPI_RC_OK,NULL};
        CMPI_ContextOnStack eCtx(context);
        CMPI_ObjectPathOnStack eRef(objectPath);
        CMPI_ResultOnStack eRes(handler,&pr.broker);
        CMPI_ThreadContext thr(&pr.broker,&eCtx);
        const CString aClass=request->assocClass.getString().getCString();
        const CString rClass=request->resultClass.getString().getCString();
        const CString rRole=request->role.getCString();
        const CString resRole=request->resultRole.getCString();

        CMPIFlags flgs=0;
        eCtx.ft->addEntry(&eCtx,CMPIInvocationFlags,(CMPIValue*)&flgs,CMPI_uint32);

        const IdentityContainer container =
           request->operationContext.get(IdentityContainer::NAME);
        eCtx.ft->addEntry(&eCtx,
                          CMPIPrincipal,
                          (CMPIValue*)(const char*)container.getUserName().getCString(),
                          CMPI_chars);

        const AcceptLanguageListContainer accept_language=            
           request->operationContext.get(AcceptLanguageListContainer::NAME);     
       const AcceptLanguages acceptLangs = accept_language.getLanguages();
        eCtx.ft->addEntry(&eCtx,  
                          "AcceptLanguage",                         
                          (CMPIValue *)(const char*)acceptLangs.toString().getCString(),
                          CMPI_chars);       
 
        if (remote) {
           CString info=pidc.getRemoteInfo().getCString();
           eCtx.ft->addEntry(&eCtx,"CMPIRRemoteInfo",(CMPIValue*)(const char*)info,CMPI_chars);
        }

        CMPIProvider::pm_service_op_lock op_lock(&pr);

#ifdef PEGASUS_ZOS_THREADLEVEL_SECURITY
                CIMAssociatorNamesRequestMessage * req = dynamic_cast<CIMAssociatorNamesRequestMessage *>(const_cast<Message *>(message));
                int err_num=enablePThreadSecurity(context);
                if (err_num!=0)
                {
                        // need a new CIMException for this
                        throw CIMException(CIM_ERR_ACCESS_DENIED,String(strerror(err_num)));
                }
#endif

        STAT_GETSTARTTIME;

        rc=pr.miVector.assocMI->ft->associatorNames(
                         pr.miVector.assocMI,&eCtx,&eRes,&eRef,CHARS(aClass),
                         CHARS(rClass),CHARS(rRole),CHARS(resRole));

        STAT_PMS_PROVIDEREND;

#ifdef PEGASUS_ZOS_THREADLEVEL_SECURITY
                disablePThreadSecurity();
#endif

        if (rc.rc!=CMPI_RC_OK)
           throw CIMException((CIMStatusCode)rc.rc,
               rc.msg ? CMGetCharsPtr(rc.msg,NULL) : String::EMPTY);
    }
    HandlerCatch(handler);

    PEG_METHOD_EXIT();

    STAT_COPYDISPATCHER

    return(response);
}

Message * CMPIProviderManager::handleReferencesRequest(const Message * message)
{
    PEG_METHOD_ENTER(TRC_PROVIDERMANAGER,
       "CMPIProviderManager::handleReferencesRequest");

    HandlerIntro(References,message,request,response,
                 handler,Array<CIMObject>());
    try {
        Logger::put(Logger::STANDARD_LOG, System::CIMSERVER, Logger::TRACE,
            "CMPIProviderManager::handleReferencesRequest - Host name: $0  Name space: $1  Class name: $2",
            System::getHostName(),
            request->nameSpace.getString(),
            request->objectName.getClassName().getString());

        // make target object path
        CIMObjectPath objectPath(
            System::getHostName(),
            request->nameSpace,
            request->objectName.getClassName());

        objectPath.setKeyBindings(request->objectName.getKeyBindings());

        CIMObjectPath resultPath(
            System::getHostName(),
            request->nameSpace,
            request->resultClass.getString());

        Boolean remote=false;
        CMPIProvider::OpProviderHolder ph;

        // resolve provider name
        ProviderIdContainer pidc = request->operationContext.get(ProviderIdContainer::NAME);
        ProviderName name = _resolveProviderName(pidc);

        if ((remote=pidc.isRemoteNameSpace())) {
           ph = providerManager.getRemoteProvider(name.getLocation(), name.getLogicalName());
        }
        else {
        // get cached or load new provider module
           ph = providerManager.getProvider(name.getPhysicalName(), name.getLogicalName());
        }

        // convert arguments
        OperationContext context;

        context.insert(request->operationContext.get(IdentityContainer::NAME));
        context.insert(request->operationContext.get(AcceptLanguageListContainer::NAME));
        context.insert(request->operationContext.get(ContentLanguageListContainer::NAME));
        // forward request
        CMPIProvider & pr=ph.GetProvider();

        PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
            "Calling provider.references: " + pr.getName());

        DDD(cerr<<"--- CMPIProviderManager::references"<<" role: >"<<request->role<<"< aCls "<<
        request->resultClass<<endl);

        CMPIStatus rc={CMPI_RC_OK,NULL};
        CMPI_ContextOnStack eCtx(context);
        CMPI_ObjectPathOnStack eRef(objectPath);
        CMPI_ResultOnStack eRes(handler,&pr.broker);
        CMPI_ThreadContext thr(&pr.broker,&eCtx);
        const CString rClass=request->resultClass.getString().getCString();
        const CString rRole=request->role.getCString();

        CMPIPropertyList props(request->propertyList);

        CMPIFlags flgs=0;
        if (request->includeQualifiers) flgs|=CMPI_FLAG_IncludeQualifiers;
        if (request->includeClassOrigin) flgs|=CMPI_FLAG_IncludeClassOrigin;
        eCtx.ft->addEntry(&eCtx,CMPIInvocationFlags,(CMPIValue*)&flgs,CMPI_uint32);

        const IdentityContainer container =
           request->operationContext.get(IdentityContainer::NAME);
        eCtx.ft->addEntry(&eCtx,
                          CMPIPrincipal,
                          (CMPIValue*)(const char*)container.getUserName().getCString(),
                          CMPI_chars);

        const AcceptLanguageListContainer accept_language=            
           request->operationContext.get(AcceptLanguageListContainer::NAME);     
       const AcceptLanguages acceptLangs = accept_language.getLanguages();
        eCtx.ft->addEntry(&eCtx,  
                          "AcceptLanguage",                         
                          (CMPIValue *)(const char*)acceptLangs.toString().getCString(),
                          CMPI_chars);       
 
        if (remote) {
           CString info=pidc.getRemoteInfo().getCString();
           eCtx.ft->addEntry(&eCtx,"CMPIRRemoteInfo",(CMPIValue*)(const char*)info,CMPI_chars);
        }

        CMPIProvider::pm_service_op_lock op_lock(&pr);

#ifdef PEGASUS_ZOS_THREADLEVEL_SECURITY
                CIMReferencesRequestMessage * req = dynamic_cast<CIMReferencesRequestMessage *>(const_cast<Message *>(message));
                int err_num=enablePThreadSecurity(context);
                if (err_num!=0)
                {
                        // need a new CIMException for this
                        throw CIMException(CIM_ERR_ACCESS_DENIED,String(strerror(err_num)));
                }
#endif

        STAT_GETSTARTTIME;

        rc=pr.miVector.assocMI->ft->references(
                         pr.miVector.assocMI,&eCtx,&eRes,&eRef,
                         CHARS(rClass),CHARS(rRole),(const char **)props.getList());

        STAT_PMS_PROVIDEREND;

#ifdef PEGASUS_ZOS_THREADLEVEL_SECURITY
                disablePThreadSecurity();
#endif

        if (rc.rc!=CMPI_RC_OK)
            throw CIMException((CIMStatusCode)rc.rc,
               rc.msg ? CMGetCharsPtr(rc.msg,NULL) : String::EMPTY);
    }
    HandlerCatch(handler);

    PEG_METHOD_EXIT();

    STAT_COPYDISPATCHER

    return(response);
}

Message * CMPIProviderManager::handleReferenceNamesRequest(const Message * message)
{
    PEG_METHOD_ENTER(TRC_PROVIDERMANAGER,
        "CMPIProviderManager::handleReferenceNamesRequest");

    HandlerIntro(ReferenceNames,message,request,response,
                 handler,Array<CIMObjectPath>());
    try {
        Logger::put(Logger::STANDARD_LOG, System::CIMSERVER, Logger::TRACE,
            "CMPIProviderManager::handleReferenceNamesRequest - Host name: $0  Name space: $1  Class name: $2",
            System::getHostName(),
            request->nameSpace.getString(),
            request->objectName.getClassName().getString());

        // make target object path
        CIMObjectPath objectPath(
            System::getHostName(),
            request->nameSpace,
            request->objectName.getClassName());

        objectPath.setKeyBindings(request->objectName.getKeyBindings());

        CIMObjectPath resultPath(
            System::getHostName(),
            request->nameSpace,
            request->resultClass.getString());

        Boolean remote=false;
        CMPIProvider::OpProviderHolder ph;

        // resolve provider name
        ProviderIdContainer pidc = request->operationContext.get(ProviderIdContainer::NAME);
        ProviderName name = _resolveProviderName(pidc);

        if ((remote=pidc.isRemoteNameSpace())) {
           ph = providerManager.getRemoteProvider(name.getLocation(), name.getLogicalName());
        }
        else {
        // get cached or load new provider module
           ph = providerManager.getProvider(name.getPhysicalName(), name.getLogicalName());
        }

        // convert arguments
        OperationContext context;

		context.insert(request->operationContext.get(IdentityContainer::NAME));
		context.insert(request->operationContext.get(AcceptLanguageListContainer::NAME));
	    context.insert(request->operationContext.get(ContentLanguageListContainer::NAME));
        CMPIProvider & pr=ph.GetProvider();

        PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
            "Calling provider.referenceNames: " + pr.getName());

        DDD(cerr<<"--- CMPIProviderManager::referenceNames"<<" role: >"<<request->role<<"< aCls "<<
           request->resultClass<<endl);

        CMPIStatus rc={CMPI_RC_OK,NULL};
        CMPI_ContextOnStack eCtx(context);
        CMPI_ObjectPathOnStack eRef(objectPath);
        CMPI_ResultOnStack eRes(handler,&pr.broker);
        CMPI_ThreadContext thr(&pr.broker,&eCtx);
        const CString rClass=request->resultClass.getString().getCString();
        const CString rRole=request->role.getCString();

        CMPIFlags flgs=0;
        eCtx.ft->addEntry(&eCtx,CMPIInvocationFlags,(CMPIValue*)&flgs,CMPI_uint32);

        const IdentityContainer container =
           request->operationContext.get(IdentityContainer::NAME);
        eCtx.ft->addEntry(&eCtx,
                          CMPIPrincipal,
                          (CMPIValue*)(const char*)container.getUserName().getCString(),
                          CMPI_chars);

        const AcceptLanguageListContainer accept_language=            
           request->operationContext.get(AcceptLanguageListContainer::NAME);     
 
       const AcceptLanguages acceptLangs = accept_language.getLanguages();
        eCtx.ft->addEntry(&eCtx,  
                          "AcceptLanguage",                         
                          (CMPIValue *)(const char*)acceptLangs.toString().getCString(),
                          CMPI_chars);       
        if (remote) {
           CString info=pidc.getRemoteInfo().getCString();
           eCtx.ft->addEntry(&eCtx,"CMPIRRemoteInfo",(CMPIValue*)(const char*)info,CMPI_chars);
        }

        CMPIProvider::pm_service_op_lock op_lock(&pr);

#ifdef PEGASUS_ZOS_THREADLEVEL_SECURITY
                CIMReferenceNamesRequestMessage * req = dynamic_cast<CIMReferenceNamesRequestMessage *>(const_cast<Message *>(message));
                int err_num=enablePThreadSecurity(context);
                if (err_num!=0)
                {
                        // need a new CIMException for this
                        throw CIMException(CIM_ERR_ACCESS_DENIED,String(strerror(err_num)));
                }
#endif

        STAT_GETSTARTTIME;

        rc=pr.miVector.assocMI->ft->referenceNames(
                         pr.miVector.assocMI,&eCtx,&eRes,&eRef,
                         CHARS(rClass),CHARS(rRole));

        STAT_PMS_PROVIDEREND;

#ifdef PEGASUS_ZOS_THREADLEVEL_SECURITY
                disablePThreadSecurity();
#endif

        if (rc.rc!=CMPI_RC_OK)
           throw CIMException((CIMStatusCode)rc.rc,
               rc.msg ? CMGetCharsPtr(rc.msg,NULL) : String::EMPTY);
    }
    HandlerCatch(handler);

    PEG_METHOD_EXIT();

    STAT_COPYDISPATCHER

    return(response);
}

Message * CMPIProviderManager::handleInvokeMethodRequest(const Message * message)
{
    PEG_METHOD_ENTER(TRC_PROVIDERMANAGER,
        "CMPIProviderManager::handleInvokeMethodRequest");

    HandlerIntroMethod(InvokeMethod,message,request,response,
                 handler);
    try {
        Logger::put(Logger::STANDARD_LOG, System::CIMSERVER, Logger::TRACE,
            "CMPIProviderManager::handleInvokeMethodRequest - Host name: $0  Name space: $1  Class name: $2",
            System::getHostName(),
            request->nameSpace.getString(),
            request->instanceName.getClassName().getString());

        // make target object path
        CIMObjectPath objectPath(
            System::getHostName(),
            request->nameSpace,
            request->instanceName.getClassName(),
            request->instanceName.getKeyBindings());

        Boolean remote=false;
        CMPIProvider::OpProviderHolder ph;

        // resolve provider name
        ProviderIdContainer pidc = request->operationContext.get(ProviderIdContainer::NAME);
        ProviderName name = _resolveProviderName(pidc);

        if ((remote=pidc.isRemoteNameSpace())) {
           ph = providerManager.getRemoteProvider(name.getLocation(), name.getLogicalName());
        }
        else {
        // get cached or load new provider module
           ph = providerManager.getProvider(name.getPhysicalName(), name.getLogicalName());
        }

        // convert arguments
        OperationContext context;

        context.insert(request->operationContext.get(IdentityContainer::NAME));
        context.insert(request->operationContext.get(AcceptLanguageListContainer::NAME));
        context.insert(request->operationContext.get(ContentLanguageListContainer::NAME));

        CIMObjectPath instanceReference(request->instanceName);

        // ATTN: propagate namespace
        instanceReference.setNameSpace(request->nameSpace);

        // forward request
        CMPIProvider & pr=ph.GetProvider();

        PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
            "Calling provider.invokeMethod: " + pr.getName());

        CMPIStatus rc={CMPI_RC_OK,NULL};
        CMPI_ContextOnStack eCtx(context);
        CMPI_ObjectPathOnStack eRef(objectPath);
        CMPI_ResultOnStack eRes(handler,&pr.broker);
        CMPI_ThreadContext thr(&pr.broker,&eCtx);
        CMPI_ArgsOnStack eArgsIn(request->inParameters);
        Array<CIMParamValue> outArgs;
        CMPI_ArgsOnStack eArgsOut(outArgs);
        CString mName=request->methodName.getString().getCString();

        CMPIFlags flgs=0;
        eCtx.ft->addEntry(&eCtx,CMPIInvocationFlags,(CMPIValue*)&flgs,CMPI_uint32);

        const IdentityContainer container =
           request->operationContext.get(IdentityContainer::NAME);
        eCtx.ft->addEntry(&eCtx,
                          CMPIPrincipal,
                          (CMPIValue*)(const char*)container.getUserName().getCString(),
                          CMPI_chars);

        const AcceptLanguageListContainer accept_language=            
           request->operationContext.get(AcceptLanguageListContainer::NAME);     
 
       const AcceptLanguages acceptLangs = accept_language.getLanguages();
        eCtx.ft->addEntry(&eCtx,  
                          "AcceptLanguage",                         
                          (CMPIValue *)(const char*)acceptLangs.toString().getCString(),
                          CMPI_chars);       
        if (remote) {
           CString info=pidc.getRemoteInfo().getCString();
           eCtx.ft->addEntry(&eCtx,"CMPIRRemoteInfo",(CMPIValue*)(const char*)info,CMPI_chars);
        }

        CMPIProvider::pm_service_op_lock op_lock(&pr);

#ifdef PEGASUS_ZOS_THREADLEVEL_SECURITY
                CIMInvokeMethodRequestMessage * req = dynamic_cast<CIMInvokeMethodRequestMessage *>(const_cast<Message *>(message));
                int err_num=enablePThreadSecurity(context);
                if (err_num!=0)
                {
                        // need a new CIMException for this
                        throw CIMException(CIM_ERR_ACCESS_DENIED,String(strerror(err_num)));
                }
#endif
        STAT_GETSTARTTIME;

        rc=pr.miVector.methMI->ft->invokeMethod(
           pr.miVector.methMI,&eCtx,&eRes,&eRef,CHARS(mName),&eArgsIn,&eArgsOut);

        STAT_PMS_PROVIDEREND;

#ifdef PEGASUS_ZOS_THREADLEVEL_SECURITY
                disablePThreadSecurity();
#endif

        if (rc.rc!=CMPI_RC_OK)
           throw CIMException((CIMStatusCode)rc.rc,
               rc.msg ? CMGetCharsPtr(rc.msg,NULL) : String::EMPTY);

       for (int i=0,s=outArgs.size(); i<s; i++)
           handler.deliverParamValue(outArgs[i]);
       handler.complete();
    }
    HandlerCatch(handler);

    PEG_METHOD_EXIT();

    STAT_COPYDISPATCHER

    return(response);
}

int LocateIndicationProviderNames(const CIMInstance& pInstance, const CIMInstance& pmInstance,
                                  String& providerName, String& location)
{
    Uint32 pos = pInstance.findProperty(CIMName ("Name"));
    pInstance.getProperty(pos).getValue().get(providerName);

    pos = pmInstance.findProperty(CIMName ("Location"));
    pmInstance.getProperty(pos).getValue().get(location);
    return 0;
}

Message * CMPIProviderManager::handleCreateSubscriptionRequest(const Message * message)
{
    PEG_METHOD_ENTER(TRC_PROVIDERMANAGER, "CMPIProviderManager::handleCreateSubscriptionRequest");

    HandlerIntroInd(CreateSubscription,message,request,response,
                 handler);
    try {
        const CIMObjectPath &x=request->subscriptionInstance.getPath();
        CIMInstance req_provider, req_providerModule;
        ProviderIdContainer pidc = (ProviderIdContainer)request->operationContext.get(ProviderIdContainer::NAME);
        req_provider = pidc.getProvider();
        req_providerModule = pidc.getModule();

        String providerName,providerLocation;
        LocateIndicationProviderNames(req_provider, req_providerModule,
        providerName,providerLocation);

        Logger::put(Logger::STANDARD_LOG, System::CIMSERVER, Logger::TRACE,
            "CMPIProviderManager::handleCreateSubscriptionRequest - Host name: $0  Name space: $1  Provider name(s): $2",
            System::getHostName(),
            request->nameSpace.getString(),
            providerName);

        Boolean remote=false;
        CMPIProvider::OpProviderHolder ph;

        if ((remote=pidc.isRemoteNameSpace())) {
	  ph = providerManager.getRemoteProvider(providerLocation, providerName);
        }
        else {
        // get cached or load new provider module
	  ph = providerManager.getProvider(providerLocation, providerName);
        }

        indProvRecord *prec=NULL;
        provTab.lookup(ph.GetProvider().getName(),prec);
        if (prec) prec->count++;
        else {
           prec=new indProvRecord();
           provTab.insert(ph.GetProvider().getName(),prec);
        }

        //
        //  Save the provider instance from the request
        //
        ph.GetProvider ().setProviderInstance (req_provider);

        indSelectRecord *srec=new indSelectRecord();
        const CIMObjectPath &sPath=request->subscriptionInstance.getPath();

        selxTab.insert(sPath.toString(),srec);

        // convert arguments
        OperationContext context;
        context.insert(request->operationContext.get(IdentityContainer::NAME));
        context.insert(request->operationContext.get(AcceptLanguageListContainer::NAME));
        context.insert(request->operationContext.get(ContentLanguageListContainer::NAME));
        context.insert(request->operationContext.get(SubscriptionInstanceContainer::NAME));
        context.insert(request->operationContext.get(SubscriptionLanguageListContainer::NAME));
        context.insert(request->operationContext.get(SubscriptionFilterConditionContainer::NAME));

        CIMObjectPath subscriptionName = request->subscriptionInstance.getPath();

        CMPIProvider & pr=ph.GetProvider();

        CMPIStatus rc={CMPI_RC_OK,NULL};
        CMPI_ContextOnStack eCtx(context);

		SubscriptionFilterConditionContainer sub_cntr =  request->operationContext.get
								(SubscriptionFilterConditionContainer::NAME);

		CIMOMHandleQueryContext *_context= new CIMOMHandleQueryContext(CIMNamespaceName(request->nameSpace.getString()),
										*pr._cimom_handle);

		CMPI_SelectExp *eSelx=new CMPI_SelectExp(context,
						_context,
        				request->query,
        				sub_cntr.getQueryLanguage());

        srec->eSelx=eSelx;
	    srec->qContext=_context;

        CMPI_ThreadContext thr(&pr.broker,&eCtx);

        PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
            "Calling provider.createSubscriptionRequest: " + pr.getName());

        DDD(cerr<<"--- CMPIProviderManager::createSubscriptionRequest"<<endl);

        for(Uint32 i = 0, n = request->classNames.size(); i < n; i++) {
            CIMObjectPath className(
                System::getHostName(),
                request->nameSpace,
                request->classNames[i]);
            eSelx->classNames.append(className);
        }
        CMPI_ObjectPathOnStack eRef(eSelx->classNames[0]);

        CIMPropertyList propertyList = request->propertyList;
        if (!propertyList.isNull()) {
           Array<CIMName> p=propertyList.getPropertyNameArray();
           int pCount=p.size();
           eSelx->props = new const char*[1+pCount];
           for (int i=0; i<pCount; i++) {
              eSelx->props[i]=strdup(p[i].getString().getCString());
           }
           eSelx->props[pCount]=NULL;
        }

        Uint16 repeatNotificationPolicy = request->repeatNotificationPolicy;

        const IdentityContainer container =
           request->operationContext.get(IdentityContainer::NAME);
        eCtx.ft->addEntry(&eCtx,
                          CMPIPrincipal,
                          (CMPIValue*)(const char*)container.getUserName().getCString(),
                          CMPI_chars);

		eCtx.ft->addEntry(&eCtx,
						  CMPIInitNameSpace,
						  (CMPIValue*)(const char*)request->nameSpace.getString().getCString(),
						  CMPI_chars);

        const AcceptLanguageListContainer accept_language=            
           request->operationContext.get(AcceptLanguageListContainer::NAME);     
 
       const AcceptLanguages acceptLangs = accept_language.getLanguages();
        eCtx.ft->addEntry(&eCtx,  
                          "AcceptLanguage",                         
                          (CMPIValue *)(const char*)acceptLangs.toString().getCString(),
                          CMPI_chars);       
        if (remote) {
           CString info=pidc.getRemoteInfo().getCString();
           eCtx.ft->addEntry(&eCtx,"CMPIRRemoteInfo",(CMPIValue*)(const char*)info,CMPI_chars);
        }

        CMPIProvider::pm_service_op_lock op_lock(&pr);

#ifdef PEGASUS_ZOS_THREADLEVEL_SECURITY
                CIMCreateSubscriptionRequestMessage * req = dynamic_cast<CIMCreateSubscriptionRequestMessage *>(const_cast<Message *>(message));
                int err_num=enablePThreadSecurity(context);
                if (err_num!=0)
                {
                        // need a new CIMException for this
                        throw CIMException(CIM_ERR_ACCESS_DENIED,String(strerror(err_num)));
                }
#endif
        STAT_GETSTARTTIME;
		if (pr.miVector.indMI->ft->ftVersion >= 100) 
		{
        rc=pr.miVector.indMI->ft->activateFilter(
           pr.miVector.indMI,&eCtx,eSelx,
           CHARS(eSelx->classNames[0].getClassName().getString().getCString()),
           &eRef,false);
		}
		else
		{
			// Older version of (pre 1.00) also pass in a CMPIResult

		rc = ((CMPIStatus (*)(CMPIIndicationMI*, CMPIContext*,
						CMPIResult*, CMPISelectExp*,
						const char *, CMPIObjectPath*,
						CMPIBoolean))
						pr.miVector.indMI->ft->activateFilter)(
						   pr.miVector.indMI,&eCtx,NULL,eSelx,
           CHARS(eSelx->classNames[0].getClassName().getString().getCString()),
           &eRef,false);
		}
		

       STAT_PMS_PROVIDEREND;

#ifdef PEGASUS_ZOS_THREADLEVEL_SECURITY
                disablePThreadSecurity();
#endif

        if (rc.rc!=CMPI_RC_OK)
        {
           throw CIMException((CIMStatusCode)rc.rc,
               rc.msg ? CMGetCharsPtr(rc.msg,NULL) : String::EMPTY);
        }
        else
        {
            //
            //  Increment count of current subscriptions for this provider
            //
            if (ph.GetProvider ().testIfZeroAndIncrementSubscriptions ())
            {
                //
                //  If there were no current subscriptions before the increment,
                //  the first subscription has been created
                //  Call the provider's enableIndications method
                //
                if (_subscriptionInitComplete)
                {
                    _callEnableIndications (req_provider, _indicationCallback,
                        ph);
                }
            }
        }
    }
    HandlerCatch(handler);

    PEG_METHOD_EXIT();

    return(response);
}

Message * CMPIProviderManager::handleDeleteSubscriptionRequest(const Message * message)
{
    PEG_METHOD_ENTER(TRC_PROVIDERMANAGER, "CMPIProviderManager::handleDeleteSubscriptionRequest");

    HandlerIntroInd(DeleteSubscription,message,request,response,
                 handler);
    try {
        String providerName,providerLocation;

        CIMInstance req_provider, req_providerModule;
        ProviderIdContainer pidc = (ProviderIdContainer)request->operationContext.get(ProviderIdContainer::NAME);
        req_provider = pidc.getProvider();
        req_providerModule = pidc.getModule();

        LocateIndicationProviderNames(req_provider, req_providerModule,
           providerName,providerLocation);

        Logger::put(Logger::STANDARD_LOG, System::CIMSERVER, Logger::TRACE,
            "CMPIProviderManager::handleDeleteSubscriptionRequest - Host name: $0  Name space: $1  Provider name(s): $2",
            System::getHostName(),
            request->nameSpace.getString(),
            providerName);

        Boolean remote=false;
        CMPIProvider::OpProviderHolder ph;

        if ((remote=pidc.isRemoteNameSpace())) {
           ph = providerManager.getRemoteProvider(providerLocation, providerName);
        }
        else {
        // get cached or load new provider module
           ph = providerManager.getProvider(providerLocation, providerName);
        }


        indProvRecord *prec=NULL;
        provTab.lookup(ph.GetProvider().getName(),prec);
        if (--prec->count<=0) {
		   if (prec->handler)
				   delete prec->handler;
		   delete prec;
           provTab.remove(ph.GetProvider().getName());
           prec=NULL;
        }

        indSelectRecord *srec=NULL;
        const CIMObjectPath &sPath=request->subscriptionInstance.getPath();
        String sPathString=sPath.toString();
        selxTab.lookup(sPathString,srec);

        CMPI_SelectExp *eSelx=srec->eSelx;
		CIMOMHandleQueryContext *qContext=srec->qContext;

        CMPI_ObjectPathOnStack eRef(eSelx->classNames[0]);
        selxTab.remove(sPathString);

        // convert arguments
        OperationContext context;
        context.insert(request->operationContext.get(IdentityContainer::NAME));
        context.insert(request->operationContext.get(AcceptLanguageListContainer::NAME));
        context.insert(request->operationContext.get(ContentLanguageListContainer::NAME));
        context.insert(request->operationContext.get(SubscriptionInstanceContainer::NAME));
        context.insert(request->operationContext.get(SubscriptionLanguageListContainer::NAME));

        CIMObjectPath subscriptionName = request->subscriptionInstance.getPath();

        CMPIProvider & pr=ph.GetProvider();

        CMPIStatus rc={CMPI_RC_OK,NULL};
        CMPI_ContextOnStack eCtx(context);
        CMPI_ThreadContext thr(&pr.broker,&eCtx);

        PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
            "Calling provider.deleteSubscriptionRequest: " + pr.getName());

        DDD(cerr<<"--- CMPIProviderManager::deleteSubscriptionRequest"<<endl);

        const IdentityContainer container =
           request->operationContext.get(IdentityContainer::NAME);
        eCtx.ft->addEntry(&eCtx,
                          CMPIPrincipal,
                          (CMPIValue*)(const char*)container.getUserName().getCString(),
                          CMPI_chars);

		eCtx.ft->addEntry(&eCtx,
						  CMPIInitNameSpace,
						  (CMPIValue*)(const char*)request->nameSpace.getString().getCString(),
						  CMPI_chars);

        const AcceptLanguageListContainer accept_language=            
           request->operationContext.get(AcceptLanguageListContainer::NAME);     
       const AcceptLanguages acceptLangs = accept_language.getLanguages();
        eCtx.ft->addEntry(&eCtx,  
                          "AcceptLanguage",                         
                          (CMPIValue *)(const char*)acceptLangs.toString().getCString(),
                          CMPI_chars);       
 
        if (remote) {
           CString info=pidc.getRemoteInfo().getCString();
           eCtx.ft->addEntry(&eCtx,"CMPIRRemoteInfo",(CMPIValue*)(const char*)info,CMPI_chars);
        }

        CMPIProvider::pm_service_op_lock op_lock(&pr);

#ifdef PEGASUS_ZOS_THREADLEVEL_SECURITY
                CIMDeleteSubscriptionRequestMessage * req = dynamic_cast<CIMDeleteSubscriptionRequestMessage *>(const_cast<Message *>(message));
                int err_num=enablePThreadSecurity(context);
                if (err_num!=0)
                {
                        // need a new CIMException for this
                        throw CIMException(CIM_ERR_ACCESS_DENIED,String(strerror(err_num)));
                }
#endif
        STAT_GETSTARTTIME;
		if (pr.miVector.indMI->ft->ftVersion >= 100) 
		{
        rc=pr.miVector.indMI->ft->deActivateFilter(
						   pr.miVector.indMI,&eCtx,eSelx,
           CHARS(eSelx->classNames[0].getClassName().getString().getCString()),
           &eRef,prec==NULL);
		}
		else
		{
			// Older version of (pre 1.00) also pass in a CMPIResult

		rc = ((CMPIStatus (*)(CMPIIndicationMI*, CMPIContext*,
						CMPIResult*, CMPISelectExp*,
						const char *, CMPIObjectPath*,
						CMPIBoolean))
						pr.miVector.indMI->ft->deActivateFilter)(
						   pr.miVector.indMI,&eCtx,NULL,eSelx,
           CHARS(eSelx->classNames[0].getClassName().getString().getCString()),
           &eRef,prec==NULL);
		}

       STAT_PMS_PROVIDEREND;

	   delete qContext;
	   delete eSelx;
	   delete srec;

#ifdef PEGASUS_ZOS_THREADLEVEL_SECURITY
                disablePThreadSecurity();
#endif

        if (rc.rc!=CMPI_RC_OK)
        {
           throw CIMException((CIMStatusCode)rc.rc,
               rc.msg ? CMGetCharsPtr(rc.msg,NULL) : String::EMPTY);
        }
        else
        {
            //
            //  Decrement count of current subscriptions for this provider
            //
            if (ph.GetProvider ().decrementSubscriptionsAndTestIfZero ())
            {
                //
                //  If there are no current subscriptions after the decrement,
                //  the last subscription has been deleted
                //  Call the provider's disableIndications method
                //
                if (_subscriptionInitComplete)
                {
                    _callDisableIndications (ph);
                }
            }
        }
    }
    HandlerCatch(handler);

    PEG_METHOD_EXIT();

    return(response);
}

Message * CMPIProviderManager::handleDisableModuleRequest(const Message * message)
{
    PEG_METHOD_ENTER(TRC_PROVIDERMANAGER, "CMPIProviderManager::handleDisableModuleRequest");

    CIMDisableModuleRequestMessage * request =
        dynamic_cast<CIMDisableModuleRequestMessage *>(const_cast<Message *>(message));

    PEGASUS_ASSERT(request != 0);

    // get provider module name
    Boolean disableProviderOnly = request->disableProviderOnly;

    Array<Uint16> operationalStatus;
    // Assume success.
    operationalStatus.append(CIM_MSE_OPSTATUS_VALUE_STOPPED);

    //
    // Unload providers
    //
    Array<CIMInstance> _pInstances = request->providers;
    Array <Boolean> _indicationProviders = request->indicationProviders;
	/* The CIMInstances on request->providers array is completly _different_ than
	   the request->providerModule CIMInstance. Hence  */

    String physicalName=(request->providerModule.getProperty(
              request->providerModule.findProperty("Location")).getValue().toString());

    for(Uint32 i = 0, n = _pInstances.size(); i < n; i++)
    {
        String providerName;
        _pInstances [i].getProperty (_pInstances [i].findProperty
            (CIMName ("Name"))).getValue ().get (providerName);

		Uint32 pos = _pInstances[i].findProperty("Name");

        //
        //  Reset the indication provider's count of current
        //  subscriptions since it has been disabled
        //
        if (_indicationProviders [i])
        {
            if (physicalName.size () > 0)
            {
		try {
                CMPIProvider::OpProviderHolder ph = providerManager.getProvider
                    (physicalName, providerName);
                ph.GetProvider ().resetSubscriptions ();
		} catch (const Exception &e) { 
        		PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL2,
            			 e.getMessage());
		}
            }
        }
        providerManager.unloadProvider(physicalName, _pInstances[i].getProperty(
										_pInstances[i].findProperty("Name")
										).getValue ().toString ());
    }

    CIMDisableModuleResponseMessage * response =
        new CIMDisableModuleResponseMessage(
        request->messageId,
        CIMException(),
        request->queueIds.copyAndPop(),
        operationalStatus);

    PEGASUS_ASSERT(response != 0);

    // preserve message key
    response->setKey(request->getKey());

    //
    //  Set HTTP method in response from request
    //
    response->setHttpMethod (request->getHttpMethod ());

    PEG_METHOD_EXIT();

    return(response);
}

Message * CMPIProviderManager::handleEnableModuleRequest(const Message * message)
{
    PEG_METHOD_ENTER(TRC_PROVIDERMANAGER, "CMPIProviderManager::handleEnableModuleRequest");

    CIMEnableModuleRequestMessage * request =
        dynamic_cast<CIMEnableModuleRequestMessage *>(const_cast<Message *>(message));

    PEGASUS_ASSERT(request != 0);

    Array<Uint16> operationalStatus;
    operationalStatus.append(CIM_MSE_OPSTATUS_VALUE_OK);

    CIMEnableModuleResponseMessage * response =
        new CIMEnableModuleResponseMessage(
        request->messageId,
        CIMException(),
        request->queueIds.copyAndPop(),
        operationalStatus);

    PEGASUS_ASSERT(response != 0);

    // preserve message key
    response->setKey(request->getKey());

    //  Set HTTP method in response from request
    response->setHttpMethod (request->getHttpMethod ());

    PEG_METHOD_EXIT();

    return(response);
}

Message * CMPIProviderManager::handleStopAllProvidersRequest(const Message * message)
{
    PEG_METHOD_ENTER(TRC_PROVIDERMANAGER, "CMPIProviderManager::handleStopAllProvidersRequest");

    CIMStopAllProvidersRequestMessage * request =
        dynamic_cast<CIMStopAllProvidersRequestMessage *>(const_cast<Message *>(message));

    PEGASUS_ASSERT(request != 0);

    CIMStopAllProvidersResponseMessage * response =
        new CIMStopAllProvidersResponseMessage(
        request->messageId,
        CIMException(),
        request->queueIds.copyAndPop());

    PEGASUS_ASSERT(response != 0);

    // preserve message key
    response->setKey(request->getKey());

    //  Set HTTP method in response from request
    response->setHttpMethod (request->getHttpMethod ());

    // tell the provider manager to shutdown all the providers
    providerManager.shutdownAllProviders();

    PEG_METHOD_EXIT();

    return(response);
}

Message * CMPIProviderManager::handleInitializeProviderRequest(const Message * message)
{
    PEG_METHOD_ENTER(TRC_PROVIDERMANAGER, "CMPIProviderManager::handleInitializeProviderRequest");

    HandlerIntroInit(InitializeProvider,message,request,response,handler);

    try
    {
        // resolve provider name
	ProviderName name = _resolveProviderName(
	    request->operationContext.get(ProviderIdContainer::NAME));

        // get cached or load new provider module
        CMPIProvider::OpProviderHolder ph =
           providerManager.getProvider(name.getPhysicalName(), name.getLogicalName());

    }
    HandlerCatch(handler);

    PEG_METHOD_EXIT();

    return(response);
}

Message * CMPIProviderManager::handleSubscriptionInitCompleteRequest
    (const Message * message)
{
    PEG_METHOD_ENTER (TRC_PROVIDERMANAGER,
     "CMPIProviderManager::handleSubscriptionInitCompleteRequest");

    CIMSubscriptionInitCompleteRequestMessage * request =
        dynamic_cast <CIMSubscriptionInitCompleteRequestMessage *>
            (const_cast <Message *> (message));

    PEGASUS_ASSERT (request != 0);

    CIMSubscriptionInitCompleteResponseMessage * response =
        dynamic_cast <CIMSubscriptionInitCompleteResponseMessage *>
            (request->buildResponse ());

    PEGASUS_ASSERT (response != 0);

    //
    //  Set indicator
    //
    _subscriptionInitComplete = true;

    //
    //  For each provider that has at least one subscription, call
    //  provider's enableIndications method
    //
    Array <CMPIProvider *> enableProviders;
    enableProviders = providerManager.getIndicationProvidersToEnable ();

    Uint32 numProviders = enableProviders.size ();
    for (Uint32 i = 0; i < numProviders; i++)
    {
        try
        {
            CIMInstance provider;
            provider = enableProviders [i]->getProviderInstance ();

            //
            //  Get cached or load new provider module
            //
            CMPIProvider::OpProviderHolder ph = providerManager.getProvider
                (enableProviders [i]->getModule ()->getFileName (),
                 enableProviders [i]->getName ());

            _callEnableIndications (provider, _indicationCallback, ph);
        }
        catch (const CIMException & e)
        {
            PEG_TRACE_STRING (TRC_PROVIDERMANAGER, Tracer::LEVEL2,
                "CIMException: " + e.getMessage ());
        }
        catch (const Exception & e)
        {
            PEG_TRACE_STRING (TRC_PROVIDERMANAGER, Tracer::LEVEL2,
                "Exception: " + e.getMessage ());
        }
        catch(...)
        {
            PEG_TRACE_STRING (TRC_PROVIDERMANAGER, Tracer::LEVEL2,
                "Unknown error in handleSubscriptionInitCompleteRequest");
        }
    }

    PEG_METHOD_EXIT ();
    return (response);
}

Message * CMPIProviderManager::handleUnsupportedRequest(const Message * message)
{
    PEG_METHOD_ENTER(TRC_PROVIDERMANAGER,
        "CMPIProviderManager::handleUnsupportedRequest");
    CIMRequestMessage* request =
        dynamic_cast<CIMRequestMessage *>(const_cast<Message *>(message));
    PEGASUS_ASSERT(request != 0 );

    CIMResponseMessage* response = request->buildResponse();
    response->cimException =
        PEGASUS_CIM_EXCEPTION(CIM_ERR_NOT_SUPPORTED, String::EMPTY);

    PEG_METHOD_EXIT();
    return response;
}

ProviderName CMPIProviderManager::_resolveProviderName(
    const ProviderIdContainer & providerId)
{
    String providerName;
    String fileName;
    String location;
    CIMValue genericValue;

    genericValue = providerId.getProvider().getProperty(
        providerId.getProvider().findProperty("Name")).getValue();
    genericValue.get(providerName);

    genericValue = providerId.getModule().getProperty(
        providerId.getModule().findProperty("Location")).getValue();
    genericValue.get(location);
    fileName = _resolvePhysicalName(location);

    ProviderName name(providerName, fileName, String::EMPTY, 0);
    name.setLocation(location);
    return name;
//    return ProviderName(providerName, fileName, interfaceName, 0);
}

void CMPIProviderManager::_callEnableIndications
    (CIMInstance & req_provider,
     PEGASUS_INDICATION_CALLBACK _indicationCallback,
     CMPIProvider::OpProviderHolder & ph)
{
    PEG_METHOD_ENTER (TRC_PROVIDERMANAGER,
        "CMPIProviderManager::_callEnableIndications");

    try
    {
        indProvRecord *provRec;
        if (provTab.lookup (ph.GetProvider ().getName (), provRec))
        {
            provRec->enabled = true;
            CIMRequestMessage * request = 0;
            CIMResponseMessage * response = 0;
            provRec->handler=new EnableIndicationsResponseHandler
                (request, response, req_provider, _indicationCallback);
        }

        CMPIProvider & pr=ph.GetProvider();

        //
        //  Versions prior to 86 did not include enableIndications routine
        //
        if (pr.miVector.indMI->ft->ftVersion >= 86)
        {
            OperationContext context;
            CMPIStatus rc={CMPI_RC_OK,NULL};
            CMPI_ContextOnStack eCtx(context);
            CMPI_ThreadContext thr(&pr.broker,&eCtx);

            PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
                "Calling provider.enableIndications: " + pr.getName());

            DDD(cerr<<"--- provider.enableIndications"<<endl);

            CMPIProvider::pm_service_op_lock op_lock(&pr);
            ph.GetProvider().protect();

            pr.miVector.indMI->ft->enableIndications(pr.miVector.indMI,&eCtx);
        }
        else
        {
            PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
                "Not calling provider.enableIndications: " + pr.getName() +
                " routine as it is an earlier version that does not support this function");

            DDD(cerr<<"--- provider.enableIndications " \
                "cannot be called as the provider uses an earlier version " \
                "that does not support this function"<<endl);
        }
    }
    catch (const CIMException & e)
    {
        PEG_TRACE_STRING (TRC_PROVIDERMANAGER, Tracer::LEVEL2,
            "CIMException: " + e.getMessage ());

        Logger::put_l (Logger::ERROR_LOG, System::CIMSERVER, Logger::WARNING,
            "ProviderManager.CMPI.CMPIProviderManager."
                "ENABLE_INDICATIONS_FAILED",
            "Failed to enable indications for provider $0: $1.",
            ph.GetProvider ().getName (), e.getMessage ());
    }
    catch (const Exception & e)
    {
        PEG_TRACE_STRING (TRC_PROVIDERMANAGER, Tracer::LEVEL2,
            "Exception: " + e.getMessage ());

        Logger::put_l (Logger::ERROR_LOG, System::CIMSERVER, Logger::WARNING,
            "ProviderManager.CMPI.CMPIProviderManager."
                "ENABLE_INDICATIONS_FAILED",
            "Failed to enable indications for provider $0: $1.",
            ph.GetProvider ().getName (), e.getMessage ());
    }
    catch(...)
    {
        PEG_TRACE_STRING (TRC_PROVIDERMANAGER, Tracer::LEVEL2,
            "Unexpected error in _callEnableIndications");

        Logger::put_l (Logger::ERROR_LOG, System::CIMSERVER, Logger::WARNING,
            "ProviderManager.CMPI.CMPIProviderManager."
                "ENABLE_INDICATIONS_FAILED_UNKNOWN",
            "Failed to enable indications for provider $0.",
            ph.GetProvider ().getName ());
    }

    PEG_METHOD_EXIT ();
}

void CMPIProviderManager::_callDisableIndications
    (CMPIProvider::OpProviderHolder & ph)
{
    PEG_METHOD_ENTER (TRC_PROVIDERMANAGER,
        "CMPIProviderManager::_callDisableIndications");

    indProvRecord * provRec;
    if (provTab.lookup (ph.GetProvider ().getName (), provRec))
    {
        provRec->enabled = false;
        if (provRec->handler) delete provRec->handler;
        provRec->handler = NULL;
    }

    CMPIProvider & pr=ph.GetProvider();

    //
    //  Versions prior to 86 did not include disableIndications routine
    //
    if (pr.miVector.indMI->ft->ftVersion >= 86)
    {
        OperationContext context;
        CMPIStatus rc={CMPI_RC_OK,NULL};
        CMPI_ContextOnStack eCtx(context);
        CMPI_ThreadContext thr(&pr.broker,&eCtx);

        PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
            "Calling provider.disableIndications: " + pr.getName());

        DDD(cerr<<"--- provider.disableIndications"<<endl);

        CMPIProvider::pm_service_op_lock op_lock(&pr);

        pr.miVector.indMI->ft->disableIndications(pr.miVector.indMI, &eCtx);

        ph.GetProvider().unprotect();
    }
    else
    {
        PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
            "Not calling provider.disableIndications: "
            + pr.getName() +
            " routine as it is an earlier version that does not support this function");

        DDD(cerr<<"--- provider.disableIndications " \
            "cannot be called as the provider uses an earlier version " \
            "that does not support this function"<<endl);

    }

    PEG_METHOD_EXIT ();
}

PEGASUS_NAMESPACE_END
