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
// Author:      Adrian Schuur, schuur@de.ibm.com 
//
// Modified By: Seema Gupta (gseema@in.ibm.com) for PEP135
//
//%/////////////////////////////////////////////////////////////////////////////

#include "JMPIProviderManager.h"

#include "JMPIImpl.h"

#include <Pegasus/Common/CIMMessage.h>
#include <Pegasus/Common/OperationContext.h>
#include <Pegasus/Common/Destroyer.h>
#include <Pegasus/Common/Tracer.h>
#include <Pegasus/Common/StatisticalData.h>
#include <Pegasus/Common/Logger.h>
#include <Pegasus/Common/MessageLoader.h> //l10n
#include <Pegasus/Common/Constants.h>

#include <Pegasus/Config/ConfigManager.h>

#include <Pegasus/ProviderManager2/ProviderName.h>
#include <Pegasus/ProviderManager2/JMPI/JMPIProvider.h>
#include <Pegasus/ProviderManager2/ProviderManagerService.h>

#include <Pegasus/ProviderManager2/CMPI/CMPI_SelectExp.h>


PEGASUS_USING_STD;
PEGASUS_NAMESPACE_BEGIN

int _jmpi_trace=0;

#define DDD(x) if (_jmpi_trace) x;

JMPIProviderManager::IndProvTab    JMPIProviderManager::provTab;
JMPIProviderManager::IndSelectTab  JMPIProviderManager::selxTab;
JMPIProviderManager::ProvRegistrar JMPIProviderManager::provReg;

JMPIProviderManager::JMPIProviderManager(Mode m)
{
   mode=m;
   if (getenv("JMPI_TRACE")) _jmpi_trace=1;
   else _jmpi_trace=0;
}

JMPIProviderManager::~JMPIProviderManager(void)
{
}

Boolean JMPIProviderManager::insertProvider(const ProviderName & name,
            const String &ns, const String &cn)
{
    String key(ns+String("::")+cn+String("::")+CIMValue(name.getCapabilitiesMask()).toString());
    return provReg.insert(key,name);
}
         
	    
Message * JMPIProviderManager::processMessage(Message * request) throw()
{
      PEG_METHOD_ENTER(TRC_PROVIDERMANAGER,
        "JMPIProviderManager::processMessage()");

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
/*    case CIM_EXEC_QUERY_REQUEST_MESSAGE:
        response = handleExecuteQueryRequest(request);

*/        break;
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
*/
    case CIM_DELETE_SUBSCRIPTION_REQUEST_MESSAGE:
        response = handleDeleteSubscriptionRequest(request);

        break;
    case CIM_ENABLE_INDICATIONS_REQUEST_MESSAGE:
        response = handleEnableIndicationsRequest(request);

        break;
    case CIM_DISABLE_INDICATIONS_REQUEST_MESSAGE:
        response = handleDisableIndicationsRequest(request);

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
    default:
	PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL2,
        	"*** Unsupported Request "+request->getType());
        response = handleUnsupportedRequest(request);

        break;
    }

    PEG_METHOD_EXIT();

    return(response);
}

Boolean JMPIProviderManager::hasActiveProviders()
{
     return providerManager.hasActiveProviders();
}

void JMPIProviderManager::unloadIdleProviders()
{
     providerManager.unloadIdleProviders();
}

#define STRDUPA(s,o) \
   if (s) { \
      o=(const char*)alloca(strlen(s)); \
      strcpy((char*)(o),(s)); \
   } \
   else o=NULL;

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
    catch(CIMException & e)  \
    { PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4, \
                "Exception: " + e.getMessage()); \
        handler.setStatus(e.getCode(), e.getContentLanguages(), e.getMessage()); \
    } \
    catch(Exception & e) \
    { PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4, \
                "Exception: " + e.getMessage()); \
        handler.setStatus(CIM_ERR_FAILED, e.getContentLanguages(), e.getMessage()); \
    } \
    catch(...) \
    { PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4, \
                "Exception: Unknown"); \
        handler.setStatus(CIM_ERR_FAILED, "Unknown error."); \
    }

static jobjectArray getList(JvmVector *jv, JNIEnv *env, CIMPropertyList &list)
{
    Uint32 s=list.size();
    jobjectArray pl=NULL;
    if (s) {
       jstring initial=env->NewString(NULL,0);
       pl=env->NewObjectArray(s,jv->StringClassRef,initial);
       for (Uint32 i=0; i<s; i++) {
           env->SetObjectArrayElement
              (pl,i,env->NewStringUTF(list[i].getString().getCString()));
       }
    }
    return pl;
}

Message * JMPIProviderManager::handleGetInstanceRequest(const Message * message) throw()
{
    PEG_METHOD_ENTER(TRC_PROVIDERMANAGER,
        "JMPIProviderManager::handleGetInstanceRequest");

    HandlerIntro(GetInstance,message,request,response,handler,CIMInstance());

    JNIEnv *env=NULL;
    Boolean mode24=false;
    CIMPropertyList propertyList;
    jobjectArray pl=NULL;

    try {
	Logger::put(Logger::STANDARD_LOG, System::CIMSERVER, Logger::TRACE,
            "JMPIProviderManager::handleGetInstanceRequest - Host name: $0  Name space: $1  Class name: $2",
            System::getHostName(),
            request->nameSpace.getString(),
            request->instanceName.getClassName().getString());

        // make target object path
        CIMObjectPath objectPath(
            System::getHostName(),
            request->nameSpace,
            request->instanceName.getClassName(),
            request->instanceName.getKeyBindings());

        // resolve provider name
        ProviderName name = _resolveProviderName(
            request->operationContext.get(ProviderIdContainer::NAME));

        // get cached or load new provider module
        JMPIProvider::OpProviderHolder ph = providerManager.getProvider(name.getPhysicalName(),
	                                name.getLogicalName());
        OperationContext context;

	// forward request
	JMPIProvider & pr=ph.GetProvider();

        PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
            "Calling provider.getInstance: " + pr.getName());

        DDD(cerr<<"--- JMPIProviderManager::getInstance"<<endl);

        JvmVector *jv;
        JNIEnv *env=JMPIjvm::attachThread(&jv);
        JMPIjvm::checkException(env);
        jobject jRef=env->NewObject(jv->CIMObjectPathClassRef,
	               jv->CIMObjectPathNewI,(jint)&objectPath);
        JMPIjvm::checkException(env);

        CIMClass cls=pr._cimom_handle->getClass(context,request->nameSpace,
	                       request->instanceName.getClassName(),
                               false,true,true,CIMPropertyList());
        JMPIjvm::checkException(env);
        jobject jCc=env->NewObject(jv->CIMClassClassRef,jv->CIMClassNewI,(jint)&cls);
        JMPIjvm::checkException(env);

	JMPIProvider::pm_service_op_lock op_lock(&pr);

	jmethodID id=env->GetMethodID((jclass)pr.jProviderClass,"getInstance",
           "(Lorg/pegasus/jmpi/CIMObjectPath;Lorg/pegasus/jmpi/CIMClass;Z)Lorg/pegasus/jmpi/CIMInstance;");

	if (id==NULL) {
           env->ExceptionClear();
	   id=env->GetMethodID((jclass)pr.jProviderClass,"getInstance",
              "(Lorg/pegasus/jmpi/CIMObjectPath;ZZZ[Ljava/lang/String;Lorg/pegasus/jmpi/CIMClass;)"
	      "Lorg/pegasus/jmpi/CIMInstance;");
           JMPIjvm::checkException(env);
	   mode24=true;
	   pl=getList(jv,env,request->propertyList);
	}
        JMPIjvm::checkException(env);

	STAT_GETSTARTTIME;

	jobject inst=NULL;
	if (mode24) inst=env->CallObjectMethod((jobject)pr.jProvider,id,jRef,
              request->localOnly,request->includeQualifiers,request->includeClassOrigin,pl,jCc);
	else inst=env->CallObjectMethod((jobject)pr.jProvider,id,jRef,jCc,true);

        STAT_PMS_PROVIDEREND;

        JMPIjvm::checkException(env);
        handler.processing();
	if (inst) {
           CIMInstance *ci=((CIMInstance*)env->CallIntMethod(inst,JMPIjvm::jv.CIMInstanceCInst));
           handler.deliver(*ci);
        }
        handler.complete();
    }
    HandlerCatch(handler);

    if (env) JMPIjvm::detachThread();

    PEG_METHOD_EXIT();

    return(response);
}

Message * JMPIProviderManager::handleEnumerateInstancesRequest(const Message * message) throw()
{
    PEG_METHOD_ENTER(TRC_PROVIDERMANAGER,
        "JMPIProviderManager::handleEnumerateInstanceRequest");

    HandlerIntro(EnumerateInstances,message,request,response,
                 handler,Array<CIMInstance>());
    JNIEnv *env=NULL;
    Boolean mode24=false;
    CIMPropertyList propertyList;
    jobjectArray pl=NULL;

    try {
      Logger::put(Logger::STANDARD_LOG, System::CIMSERVER, Logger::TRACE,
            "JMPIProviderManager::handleEnumerateInstancesRequest - Host name: $0  Name space: $1  Class name: $2",
            System::getHostName(),
            request->nameSpace.getString(),
            request->className.getString());

        // make target object path
        CIMObjectPath objectPath(
            System::getHostName(),
            request->nameSpace,
            request->className);

        // resolve provider name
        ProviderName name = _resolveProviderName(
            request->operationContext.get(ProviderIdContainer::NAME));

        // get cached or load new provider module
        JMPIProvider::OpProviderHolder ph =
            providerManager.getProvider(name.getPhysicalName(), name.getLogicalName(),
               String::EMPTY);

        // convert arguments
        OperationContext context;

        context.insert(request->operationContext.get(IdentityContainer::NAME));
        context.insert(request->operationContext.get(AcceptLanguageListContainer::NAME));
        context.insert(request->operationContext.get(ContentLanguageListContainer::NAME));

        CIMPropertyList propertyList(request->propertyList);

        // forward request
        JMPIProvider & pr=ph.GetProvider();

        PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
            "Calling provider.enumerateInstances: " + pr.getName());

        DDD(cerr<<"--- JMPIProviderManager::enumerateInstances"<<endl);


        JvmVector *jv;
        env=JMPIjvm::attachThread(&jv);

        jobject jRef=env->NewObject(jv->CIMObjectPathClassRef,
                       jv->CIMObjectPathNewI,(jint)&objectPath);
        JMPIjvm::checkException(env);

        CIMClass cls=pr._cimom_handle->getClass(context,request->nameSpace,
                               request->className,
                               false,true,true,CIMPropertyList());
        JMPIjvm::checkException(env);
        jobject jCc=env->NewObject(jv->CIMClassClassRef,jv->CIMClassNewI,(jint)&cls);
        JMPIjvm::checkException(env);

        JMPIProvider::pm_service_op_lock op_lock(&pr);

        STAT_GETSTARTTIME;

        jmethodID id=env->GetMethodID((jclass)pr.jProviderClass,"enumInstances",
           "(Lorg/pegasus/jmpi/CIMObjectPath;ZLorg/pegasus/jmpi/CIMClass;Z)Ljava/util/Vector;");

	if (id==NULL) {
           env->ExceptionClear();
	   id=env->GetMethodID((jclass)pr.jProviderClass,"enumerateInstances",
              "(Lorg/pegasus/jmpi/CIMObjectPath;ZZZ[Ljava/lang/String;Lorg/pegasus/jmpi/CIMClass;)"
	      "[Lorg/pegasus/jmpi/CIMInstance;");
           JMPIjvm::checkException(env);
	   mode24=true;
	   pl=getList(jv,env,request->propertyList);
	}
        JMPIjvm::checkException(env);

        if (!mode24) {
	   jobject jVec=env->CallObjectMethod((jobject)pr.jProvider,id,jRef,false,jCc,true);
           JMPIjvm::checkException(env);

           STAT_PMS_PROVIDEREND;

           handler.processing();
           if (jVec) {
              for (int i=0,m=env->CallIntMethod(jVec,JMPIjvm::jv.VectorSize); i<m; i++) {
                 JMPIjvm::checkException(env);
	         jobject jInst=env->CallObjectMethod(jVec,JMPIjvm::jv.VectorElementAt,i);
                 JMPIjvm::checkException(env);
                 CIMInstance inst=*((CIMInstance*)env->CallIntMethod(jInst,JMPIjvm::jv.CIMInstanceCInst));
                 JMPIjvm::checkException(env);

                 handler.deliver(inst);
 	      }
           }
           handler.complete();
        }

        else {
	   jobjectArray jAr=(jobjectArray)env->CallObjectMethod((jobject)pr.jProvider,id,jRef,
              request->localOnly,request->includeQualifiers,request->includeClassOrigin,pl,jCc);
           JMPIjvm::checkException(env);

           STAT_PMS_PROVIDEREND;

           handler.processing();
           if (jAr) {
              for (int i=0,m=env->GetArrayLength(jAr); i<m; i++) {
                 JMPIjvm::checkException(env);
	         jobject jInst=env->GetObjectArrayElement(jAr,i);
                 JMPIjvm::checkException(env);
                 CIMInstance inst=*((CIMInstance*)env->CallIntMethod(jInst,JMPIjvm::jv.CIMInstanceCInst));
                 JMPIjvm::checkException(env);

                 handler.deliver(inst);
 	      }
           }
           handler.complete();
        }
    }
    HandlerCatch(handler);

    if (env) JMPIjvm::detachThread();

    PEG_METHOD_EXIT();

    return(response);
}

Message * JMPIProviderManager::handleEnumerateInstanceNamesRequest(const Message * message) throw()
{
    PEG_METHOD_ENTER(TRC_PROVIDERMANAGER, "JMPIProviderManager::handleEnumerateInstanceNamesRequest");

    HandlerIntro(EnumerateInstanceNames,message,request,response,
                 handler,Array<CIMObjectPath>()); 
    JNIEnv *env=NULL;
    Boolean mode24=false;
    CIMPropertyList propertyList;
    jobjectArray pl=NULL;

    try {
        Logger::put(Logger::STANDARD_LOG, System::CIMSERVER, Logger::TRACE,
            "JMPIProviderManager::handleEnumerateInstanceNamesRequest - Host name: $0  Name space: $1  Class name: $2",
            System::getHostName(),
            request->nameSpace.getString(),
            request->className.getString());
       
       // make target object path
        CIMObjectPath objectPath(
            System::getHostName(),
            request->nameSpace,
            request->className);

        // resolve provider name
        ProviderName name = _resolveProviderName(
            request->operationContext.get(ProviderIdContainer::NAME));

        // get cached or load new provider module
        JMPIProvider::OpProviderHolder ph =
            providerManager.getProvider(name.getPhysicalName(), name.getLogicalName());

        // convert arguments
        OperationContext context;

		context.insert(request->operationContext.get(IdentityContainer::NAME));
		context.insert(request->operationContext.get(AcceptLanguageListContainer::NAME)); 
	    context.insert(request->operationContext.get(ContentLanguageListContainer::NAME)); 

	JMPIProvider & pr=ph.GetProvider();

	PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
            "Calling provider.enumerateInstanceNames: " + pr.getName());

        DDD(cerr<<"--- JMPIProviderManager::enumerateInstanceNames"<<endl);


	JvmVector *jv;
        env=JMPIjvm::attachThread(&jv);

        jobject jRef=env->NewObject(jv->CIMObjectPathClassRef,
	               jv->CIMObjectPathNewI,(jint)&objectPath);        
	JMPIjvm::checkException(env);

        CIMClass cls=pr._cimom_handle->getClass(context,request->nameSpace,
	                       request->className,
                               false,true,true,CIMPropertyList());
        JMPIjvm::checkException(env);
        jobject jCc=env->NewObject(jv->CIMClassClassRef,jv->CIMClassNewI,(jint)&cls);
        JMPIjvm::checkException(env);

	JMPIProvider::pm_service_op_lock op_lock(&pr);

	STAT_GETSTARTTIME;

        jmethodID id=env->GetMethodID((jclass)pr.jProviderClass,"enumInstances",
           "(Lorg/pegasus/jmpi/CIMObjectPath;ZLorg/pegasus/jmpi/CIMClass;)Ljava/util/Vector;");

	if (id==NULL) {
           env->ExceptionClear();
	   id=env->GetMethodID((jclass)pr.jProviderClass,"enumerateInstanceNames",
              "(Lorg/pegasus/jmpi/CIMObjectPath;Lorg/pegasus/jmpi/CIMClass;)"
	      "[Lorg/pegasus/jmpi/CIMObjectPath;");
           JMPIjvm::checkException(env);
	   mode24=true;
	}
        JMPIjvm::checkException(env);

        if (!mode24) {
	   jobject jVec=env->CallObjectMethod((jobject)pr.jProvider,id,jRef,true,jCc);
           JMPIjvm::checkException(env);

           STAT_PMS_PROVIDEREND;

           handler.processing();
	   if (jVec) {
	      for (int i=0,m=env->CallIntMethod(jVec,JMPIjvm::jv.VectorSize); i<m; i++) {
                 JMPIjvm::checkException(env);
                 jobject jCop=env->CallObjectMethod(jVec,JMPIjvm::jv.VectorElementAt,i);
                 JMPIjvm::checkException(env);
                 CIMObjectPath *cop=((CIMObjectPath*)env->CallIntMethod
                    (jCop,JMPIjvm::jv.CIMObjectPathCInst));
                 JMPIjvm::checkException(env);
                 handler.deliver(*cop);
              }
 	   }
           handler.complete();
        }
        else {
           jobjectArray jAr=(jobjectArray)env->CallObjectMethod((jobject)pr.jProvider,id,jRef,jCc);
           JMPIjvm::checkException(env);

           STAT_PMS_PROVIDEREND;

           handler.processing();
           if (jAr) {
              for (int i=0,m=env->GetArrayLength(jAr); i<m; i++) {
                 JMPIjvm::checkException(env);
                 jobject jCop=env->GetObjectArrayElement(jAr,i);
                 JMPIjvm::checkException(env);
                 CIMObjectPath *cop=((CIMObjectPath*)env->CallIntMethod
                    (jCop,JMPIjvm::jv.CIMObjectPathCInst));
                 JMPIjvm::checkException(env);
                 handler.deliver(*cop);
              }
           }
           handler.complete();
        }
    }
    HandlerCatch(handler);
    
    if (env) JMPIjvm::detachThread();	

    PEG_METHOD_EXIT();

    return(response);
}

Message * JMPIProviderManager::handleCreateInstanceRequest(const Message * message) throw()
{
    PEG_METHOD_ENTER(TRC_PROVIDERMANAGER,
       "JMPIProviderManager::handleCreateInstanceRequest");

    HandlerIntro(CreateInstance,message,request,response,
                 handler,CIMObjectPath());
    JNIEnv *env=NULL;
    try {
        Logger::put(Logger::STANDARD_LOG, System::CIMSERVER, Logger::TRACE,
            "JMPIProviderManager::handleCreateInstanceRequest - Host name: $0  Name space: $1  Class name: $2",
            System::getHostName(),
            request->nameSpace.getString(),
            request->newInstance.getPath().getClassName().getString());

        // make target object path
        CIMObjectPath objectPath(
            System::getHostName(),
            request->nameSpace,
            request->newInstance.getPath().getClassName(),
            request->newInstance.getPath().getKeyBindings());

        // resolve provider name
        ProviderName name = _resolveProviderName(
            request->operationContext.get(ProviderIdContainer::NAME));

        // get cached or load new provider module
        JMPIProvider::OpProviderHolder ph =
            providerManager.getProvider(name.getPhysicalName(), name.getLogicalName(),
	           String::EMPTY);

        // convert arguments
        OperationContext context;

		context.insert(request->operationContext.get(IdentityContainer::NAME));
		context.insert(request->operationContext.get(AcceptLanguageListContainer::NAME)); 
	    context.insert(request->operationContext.get(ContentLanguageListContainer::NAME)); 

        // forward request
 	JMPIProvider & pr=ph.GetProvider();

        PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
            "Calling provider.createInstance: " +
            ph.GetProvider().getName());

        DDD(cerr<<"--- JMPIProviderManager::createInstances"<<endl);

	JvmVector *jv;
        env=JMPIjvm::attachThread(&jv);

        jobject jRef=env->NewObject(jv->CIMObjectPathClassRef,
	               jv->CIMObjectPathNewI,(jint)&objectPath);        
	JMPIjvm::checkException(env);
        jobject jInst=env->NewObject(jv->CIMInstanceClassRef,
	               jv->CIMInstanceNewI,(jint)&request->newInstance);        
	JMPIjvm::checkException(env);

	JMPIProvider::pm_service_op_lock op_lock(&pr);

	STAT_GETSTARTTIME;

        jmethodID id=env->GetMethodID((jclass)pr.jProviderClass,"createInstance",
           "(Lorg/pegasus/jmpi/CIMObjectPath;Lorg/pegasus/jmpi/CIMInstance;)Lorg/pegasus/jmpi/CIMObjectPath;");
        JMPIjvm::checkException(env);

        jobject jCop=env->CallObjectMethod((jobject)pr.jProvider,id,jRef,jInst);
        
        STAT_PMS_PROVIDEREND;
	
        JMPIjvm::checkException(env);
        handler.processing();       
	if (jCop) {
           CIMObjectPath cop=*((CIMObjectPath*)env->CallIntMethod(jCop,JMPIjvm::jv.CIMObjectPathCInst)); 
           handler.deliver(cop);
        }
        handler.complete();
    }
    HandlerCatch(handler);
    
    if (env) JMPIjvm::detachThread();
    
    PEG_METHOD_EXIT();

    return(response);
}

Message * JMPIProviderManager::handleModifyInstanceRequest(const Message * message) throw()
{
    PEG_METHOD_ENTER(TRC_PROVIDERMANAGER,
       "JMPIProviderManager::handleModifyInstanceRequest");

    HandlerIntroVoid(ModifyInstance,message,request,response,handler);
    JNIEnv *env=NULL;
    Boolean mode24=false;
    CIMPropertyList propertyList;
    jobjectArray pl=NULL;

    try {
        Logger::put(Logger::STANDARD_LOG, System::CIMSERVER, Logger::TRACE,
            "JMPIProviderManager::handleModifyInstanceRequest - Host name: $0  Name space: $1  Class name: $2",
            System::getHostName(),
            request->nameSpace.getString(),
            request->modifiedInstance.getPath().getClassName().getString());

        // make target object path
        CIMObjectPath objectPath(
            System::getHostName(),
            request->nameSpace,
            request->modifiedInstance.getPath ().getClassName(),
            request->modifiedInstance.getPath ().getKeyBindings());

        // resolve provider name
        ProviderName name = _resolveProviderName(
            request->operationContext.get(ProviderIdContainer::NAME));

        // get cached or load new provider module
        JMPIProvider::OpProviderHolder ph =
            providerManager.getProvider(name.getPhysicalName(), name.getLogicalName(), String::EMPTY);

        // convert arguments
        OperationContext context;

		context.insert(request->operationContext.get(IdentityContainer::NAME));
		context.insert(request->operationContext.get(AcceptLanguageListContainer::NAME)); 
	    context.insert(request->operationContext.get(ContentLanguageListContainer::NAME)); 

        CIMPropertyList propertyList(request->propertyList);

        // forward request
 	JMPIProvider & pr=ph.GetProvider();

        PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
            "Calling provider.modifyInstance: " + pr.getName());

        DDD(cerr<<"--- JMPIProviderManager::modifyInstance"<<endl);

	JvmVector *jv;
        env=JMPIjvm::attachThread(&jv);

        jobject jRef=env->NewObject(jv->CIMObjectPathClassRef,
	               jv->CIMObjectPathNewI,(jint)&objectPath);
	JMPIjvm::checkException(env);
        jobject jInst=env->NewObject(jv->CIMInstanceClassRef,
	               jv->CIMInstanceNewI,(jint)&request->modifiedInstance);        
	JMPIjvm::checkException(env);

	JMPIProvider::pm_service_op_lock op_lock(&pr);

	STAT_GETSTARTTIME;

        jmethodID id=env->GetMethodID((jclass)pr.jProviderClass,"setInstance",
           "(Lorg/pegasus/jmpi/CIMObjectPath;Lorg/pegasus/jmpi/CIMInstance;)V");

	if (id==NULL) {
           env->ExceptionClear();
           id=env->GetMethodID((jclass)pr.jProviderClass,"setInstance",
              "(Lorg/pegasus/jmpi/CIMObjectPath;Z[Ljava/lang/String)V");
           JMPIjvm::checkException(env);
           mode24=true;
           pl=getList(jv,env,request->propertyList);
        }
        JMPIjvm::checkException(env);

        if (!mode24) env->CallVoidMethod((jobject)pr.jProvider,id,jRef,jInst);
        else env->CallVoidMethod((jobject)pr.jProvider,id,jRef,jInst,pl);

        STAT_PMS_PROVIDEREND;
	
        JMPIjvm::checkException(env);
        JMPIjvm::detachThread();
    }
    HandlerCatch(handler);
    
    if (env) JMPIjvm::detachThread();
    
    PEG_METHOD_EXIT();

    return(response);
}

Message * JMPIProviderManager::handleDeleteInstanceRequest(const Message * message) throw()
{
    PEG_METHOD_ENTER(TRC_PROVIDERMANAGER,
       "JMPIProviderManager::handleDeleteInstanceRequest");

    HandlerIntroVoid(DeleteInstance,message,request,response,
                 handler);
    JNIEnv *env=NULL;
    try {
        Logger::put(Logger::STANDARD_LOG, System::CIMSERVER, Logger::TRACE,
            "JMPIProviderManager::handleDeleteInstanceRequest - Host name: $0  Name space: $1  Class name: $2",
            System::getHostName(),
            request->nameSpace.getString(),
            request->instanceName.getClassName().getString());

        // make target object path
        CIMObjectPath objectPath(
            System::getHostName(),
            request->nameSpace,
            request->instanceName.getClassName(),
            request->instanceName.getKeyBindings());

        // resolve provider name
        ProviderName name = _resolveProviderName(
            request->operationContext.get(ProviderIdContainer::NAME));

        // get cached or load new provider module
        JMPIProvider::OpProviderHolder ph =
            providerManager.getProvider(name.getPhysicalName(), name.getLogicalName(), String::EMPTY);

        // convert arguments
        OperationContext context;

		context.insert(request->operationContext.get(IdentityContainer::NAME));
		context.insert(request->operationContext.get(AcceptLanguageListContainer::NAME)); 
	    context.insert(request->operationContext.get(ContentLanguageListContainer::NAME)); 

        // forward request
 	JMPIProvider & pr=ph.GetProvider();

        PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
            "Calling provider.deleteInstance: " + pr.getName());

        DDD(cerr<<"--- JMPIProviderManager::modifyInstance"<<endl);

	JvmVector *jv;
        env=JMPIjvm::attachThread(&jv);

        jobject jRef=env->NewObject(jv->CIMObjectPathClassRef,
	               jv->CIMObjectPathNewI,(jint)&objectPath);        
	JMPIjvm::checkException(env);

	JMPIProvider::pm_service_op_lock op_lock(&pr);

	STAT_GETSTARTTIME;

        jmethodID id=env->GetMethodID((jclass)pr.jProviderClass,"deleteInstance",
           "(Lorg/pegasus/jmpi/CIMObjectPath;)V");
        JMPIjvm::checkException(env);

        env->CallVoidMethod((jobject)pr.jProvider,id,jRef);
        
        STAT_PMS_PROVIDEREND;
	
        JMPIjvm::checkException(env);
        JMPIjvm::detachThread();
    }
    HandlerCatch(handler);
    
    if (env) JMPIjvm::detachThread();
    PEG_METHOD_EXIT();

    return(response);
}


Message * JMPIProviderManager::handleAssociatorsRequest(const Message * message) throw()
{
    PEG_METHOD_ENTER(TRC_PROVIDERMANAGER,
       "JMPIProviderManager::handleAssociatorsRequest");

    HandlerIntro(Associators,message,request,response,
                 handler,Array<CIMObject>());
    JNIEnv *env=NULL;
    Boolean mode24=false;
    CIMPropertyList propertyList;
    jobjectArray pl=NULL;

    try {
        Logger::put(Logger::STANDARD_LOG, System::CIMSERVER, Logger::TRACE,
            "JMPIProviderManager::handleAssociatorsRequest - Host name: $0  Name space: $1  Class name: $2",
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

        // resolve provider name
        ProviderName name = _resolveProviderName(
            request->operationContext.get(ProviderIdContainer::NAME));

        // get cached or load new provider module
        JMPIProvider::OpProviderHolder ph =
            providerManager.getProvider(name.getPhysicalName(), name.getLogicalName(), String::EMPTY);

       // convert arguments
        OperationContext context;

		context.insert(request->operationContext.get(IdentityContainer::NAME));
		context.insert(request->operationContext.get(AcceptLanguageListContainer::NAME)); 
	    context.insert(request->operationContext.get(ContentLanguageListContainer::NAME)); 

        // forward request
 	JMPIProvider & pr=ph.GetProvider();

        PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
            "Calling provider.associators: " + pr.getName());

        DDD(cerr<<"--- JMPIProviderManager::associators"<<
	               " role: >"<<request->role<<"< aCls "<<
	   request->assocClass<<endl);

	JvmVector *jv;
        env=JMPIjvm::attachThread(&jv);

        jobject jRef=env->NewObject(jv->CIMObjectPathClassRef,
	               jv->CIMObjectPathNewI,(jint)&objectPath);
	JMPIjvm::checkException(env);

        jstring rClass=env->NewStringUTF(request->resultClass.getString().getCString());
        jstring rRole=env->NewStringUTF(request->role.getCString());
        jstring resRole=env->NewStringUTF(request->resultRole.getCString());
	JMPIjvm::checkException(env);

        JMPIProvider::pm_service_op_lock op_lock(&pr);

        STAT_GETSTARTTIME;

        jmethodID id=env->GetMethodID((jclass)pr.jProviderClass,"associators",
           "(Lorg/pegasus/jmpi/CIMObjectPath;Ljava/lang/String;Ljava/lang/String;"
	   "Ljava/lang/String;ZZ[Ljava/lang/String;)Ljava/util/Vector;");

	if (id==NULL) {
           env->ExceptionClear();
	   id=env->GetMethodID((jclass)pr.jProviderClass,"associators",
             "(Lorg/pegasus/jmpi/CIMObjectPath;Ljava/lang/String;Ljava/lang/String;"
	     "Ljava/lang/String;ZZ[Ljava/lang/String;)[Lorg/pegasus/jmpi/CIMInstance;");
           JMPIjvm::checkException(env);
	   mode24=true;
	}
        JMPIjvm::checkException(env);

	pl=getList(jv,env,request->propertyList);

	if (!mode24) {
           jobject jVec=env->CallObjectMethod((jobject)pr.jProvider,id,jRef,
	                 rClass,rRole,resRole,false,false,pl);
           JMPIjvm::checkException(env);

           STAT_PMS_PROVIDEREND;

           handler.processing();
	   if (jVec) {
	      for (int i=0,m=env->CallIntMethod(jVec,JMPIjvm::jv.VectorSize); i<m; i++) {
                 JMPIjvm::checkException(env);
	         jobject jInst=env->CallObjectMethod(jVec,JMPIjvm::jv.VectorElementAt,i);
                 JMPIjvm::checkException(env);
                 CIMInstance inst=*((CIMInstance*)env->CallIntMethod(jInst,JMPIjvm::jv.CIMInstanceCInst));
                 JMPIjvm::checkException(env);

	         CIMClass cls=pr._cimom_handle->getClass(context,request->nameSpace,
	             inst.getClassName(),false,true,true,CIMPropertyList());
                 const CIMObjectPath& op=inst.getPath();
                 CIMObjectPath iop=inst.buildPath(cls);
                 iop.setNameSpace(op.getNameSpace());
                 inst.setPath(iop);

                 handler.deliver(inst);
 	      }
	   }
        }

	else {
           jobjectArray jAr=(jobjectArray)env->CallObjectMethod((jobject)pr.jProvider,id,jRef,
	                 rClass,rRole,resRole,false,false,pl);
           JMPIjvm::checkException(env);

           STAT_PMS_PROVIDEREND;

           handler.processing();
	   if (jAr) {
	      for (int i=0,m=env->GetArrayLength(jAr); i<m; i++) {
                 JMPIjvm::checkException(env);
	         jobject jInst=env->GetObjectArrayElement(jAr,i);
                 JMPIjvm::checkException(env);
                 CIMInstance inst=*((CIMInstance*)env->CallIntMethod(jInst,JMPIjvm::jv.CIMInstanceCInst));
                 JMPIjvm::checkException(env);

	         CIMClass cls=pr._cimom_handle->getClass(context,request->nameSpace,
	             inst.getClassName(),false,true,true,CIMPropertyList());
                 const CIMObjectPath& op=inst.getPath();
                 CIMObjectPath iop=inst.buildPath(cls);
                 iop.setNameSpace(op.getNameSpace());
                 inst.setPath(iop);

                 handler.deliver(inst);
 	      }
	   }
	}
        handler.complete();
    }
    HandlerCatch(handler);

    if (env) JMPIjvm::detachThread();

    PEG_METHOD_EXIT();

    return(response);
}



Message * JMPIProviderManager::handleAssociatorNamesRequest(const Message * message) throw()
{
    PEG_METHOD_ENTER(TRC_PROVIDERMANAGER,
       "JMPIProviderManager::handleAssociatorNamesRequest");

    HandlerIntro(AssociatorNames,message,request,response,
                 handler,Array<CIMObjectPath>());
    JNIEnv *env=NULL;
    Boolean mode24=false;
    CIMPropertyList propertyList;
    jobjectArray pl=NULL;

    try {
        Logger::put(Logger::STANDARD_LOG, System::CIMSERVER, Logger::TRACE,
            "JMPIProviderManager::handleAssociatorNamesRequest - Host name: $0  Name space: $1  Class name: $2",
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
      
        // resolve provider name
        ProviderName name = _resolveProviderName(
            request->operationContext.get(ProviderIdContainer::NAME));

        // get cached or load new provider module
        JMPIProvider::OpProviderHolder ph =
            providerManager.getProvider(name.getPhysicalName(), name.getLogicalName(), String::EMPTY);

        // convert arguments
        OperationContext context;

		context.insert(request->operationContext.get(IdentityContainer::NAME));
		context.insert(request->operationContext.get(AcceptLanguageListContainer::NAME)); 
	    context.insert(request->operationContext.get(ContentLanguageListContainer::NAME)); 

        // forward request
 	JMPIProvider & pr=ph.GetProvider();

        PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
            "Calling provider.associatorNames: " + pr.getName());

        DDD(cerr<<"--- JMPIProviderManager::associatorNames"<<
	               " role: >"<<request->role<<"< aCls "<<
	   request->assocClass<<endl);

	JvmVector *jv;
        env=JMPIjvm::attachThread(&jv);

        jobject jRef=env->NewObject(jv->CIMObjectPathClassRef,
	               jv->CIMObjectPathNewI,(jint)&objectPath);        
	JMPIjvm::checkException(env);
        
        jstring rClass=env->NewStringUTF(request->resultClass.getString().getCString());
        jstring rRole=env->NewStringUTF(request->role.getCString());
        jstring resRole=env->NewStringUTF(request->resultRole.getCString());
	JMPIjvm::checkException(env);

        JMPIProvider::pm_service_op_lock op_lock(&pr);

        STAT_GETSTARTTIME;

        jmethodID id=env->GetMethodID((jclass)pr.jProviderClass,"associatorNames",
           "(Lorg/pegasus/jmpi/CIMObjectPath;Ljava/lang/String;Ljava/lang/String;"
	   "Ljava/lang/String;)Ljava/util/Vector;");

	if (id==NULL) {
           env->ExceptionClear();
	   id=env->GetMethodID((jclass)pr.jProviderClass,"associatorNames",
              "(Lorg/pegasus/jmpi/CIMObjectPath;Ljava/lang/String;Ljava/lang/String;"
	      "Ljava/lang/String;)[Lorg/pegasus/jmpi/CIMObjectPath;");
           JMPIjvm::checkException(env);
	   mode24=true;
	}
        JMPIjvm::checkException(env);

	if (!mode24) {
           jobject jVec=env->CallObjectMethod((jobject)pr.jProvider,id,jRef,
	                 rClass,rRole,resRole);
           JMPIjvm::checkException(env);

           STAT_PMS_PROVIDEREND;

           handler.processing();
	   if (jVec) {
	      for (int i=0,m=env->CallIntMethod(jVec,JMPIjvm::jv.VectorSize); i<m; i++) {
                 JMPIjvm::checkException(env);
	         jobject jCop=env->CallObjectMethod(jVec,JMPIjvm::jv.VectorElementAt,i);
                 JMPIjvm::checkException(env);
                 CIMObjectPath *cop=((CIMObjectPath*)env->CallIntMethod
	            (jCop,JMPIjvm::jv.CIMObjectPathCInst));
                 JMPIjvm::checkException(env);
                 handler.deliver(*cop);
 	      }
           }
           handler.complete();
	}

	else {
           jobjectArray jAr=(jobjectArray)env->CallObjectMethod((jobject)pr.jProvider,id,jRef,
	                 rClass,rRole,resRole);
           JMPIjvm::checkException(env);

           STAT_PMS_PROVIDEREND;

           handler.processing();
	   if (jAr) {
	      for (int i=0,m=env->GetArrayLength(jAr); i<m; i++) {
                 JMPIjvm::checkException(env);
	         jobject jCop=env->GetObjectArrayElement(jAr,i);
                 JMPIjvm::checkException(env);
                 CIMObjectPath *cop=((CIMObjectPath*)env->CallIntMethod
	            (jCop,JMPIjvm::jv.CIMObjectPathCInst));
                 JMPIjvm::checkException(env);
                 handler.deliver(*cop);
 	      }
           }
           handler.complete();
	}
    }
    HandlerCatch(handler);

    if (env) JMPIjvm::detachThread();	

    PEG_METHOD_EXIT();

    return(response);
}


Message * JMPIProviderManager::handleReferencesRequest(const Message * message) throw()
{
    PEG_METHOD_ENTER(TRC_PROVIDERMANAGER,
       "JMPIProviderManager::handleReferencesRequest");

    HandlerIntro(References,message,request,response,
                 handler,Array<CIMObject>());
    JNIEnv *env=NULL;
    Boolean mode24=false;
    CIMPropertyList propertyList;
    jobjectArray pl=NULL;

    try {
        Logger::put(Logger::STANDARD_LOG, System::CIMSERVER, Logger::TRACE,
            "JMPIProviderManager::handleReferencesRequest - Host name: $0  Name space: $1  Class name: $2",
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

        // resolve provider name
        ProviderName name = _resolveProviderName(
            request->operationContext.get(ProviderIdContainer::NAME));

        // get cached or load new provider module
        JMPIProvider::OpProviderHolder ph =
            providerManager.getProvider(name.getPhysicalName(), name.getLogicalName(), String::EMPTY);

        // convert arguments
        OperationContext context;

        context.insert(request->operationContext.get(IdentityContainer::NAME));
        context.insert(request->operationContext.get(AcceptLanguageListContainer::NAME));
        context.insert(request->operationContext.get(ContentLanguageListContainer::NAME));

        // forward request
        JMPIProvider & pr=ph.GetProvider();

        PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
            "Calling provider.references: " + pr.getName());

        DDD(cerr<<"--- JMPIProviderManager::references"<<" role: >"<<request->role<<"< aCls "<<
           request->resultClass<<endl);

        JvmVector *jv;
        env=JMPIjvm::attachThread(&jv);

        jobject jRef=env->NewObject(jv->CIMObjectPathClassRef,
                       jv->CIMObjectPathNewI,(jint)&objectPath);
        JMPIjvm::checkException(env);

        jstring rRole=env->NewStringUTF(request->role.getCString());
        JMPIjvm::checkException(env);

        JMPIProvider::pm_service_op_lock op_lock(&pr);

        STAT_GETSTARTTIME;

        jmethodID id=env->GetMethodID((jclass)pr.jProviderClass,"references",
           "(Lorg/pegasus/jmpi/CIMObjectPath;Ljava/lang/String;"
           "ZZ[Ljava/lang/String;)Ljava/util/Vector;");

	if (id==NULL) {
           env->ExceptionClear();
	   id=env->GetMethodID((jclass)pr.jProviderClass,"references",
              "(Lorg/pegasus/jmpi/CIMObjectPath;Ljava/lang/String;"
              "ZZ[Ljava/lang/String;)[Lorg/pegasus/jmpi/CIMInstance;");
           JMPIjvm::checkException(env);
	   mode24=true;
	}
        JMPIjvm::checkException(env);

	pl=getList(jv,env,request->propertyList);

	if (!mode24) {
           jobject jVec=env->CallObjectMethod((jobject)pr.jProvider,id,jRef,
                         rRole,false,false,pl);
           JMPIjvm::checkException(env);

           STAT_PMS_PROVIDEREND;

           handler.processing();
           if (jVec) {
              for (int i=0,m=env->CallIntMethod(jVec,JMPIjvm::jv.VectorSize); i<m; i++) {
                 JMPIjvm::checkException(env);
                 jobject jInst=env->CallObjectMethod(jVec,JMPIjvm::jv.VectorElementAt,i);
                 JMPIjvm::checkException(env);
                 CIMInstance inst=*((CIMInstance*)env->CallIntMethod(jInst,JMPIjvm::jv.CIMInstanceCInst));
                 JMPIjvm::checkException(env);

                 CIMClass cls=pr._cimom_handle->getClass(context,request->nameSpace,
                     inst.getClassName(),false,true,true,CIMPropertyList());
                 const CIMObjectPath& op=inst.getPath();
                 CIMObjectPath iop=inst.buildPath(cls);
                 iop.setNameSpace(op.getNameSpace());
                 inst.setPath(iop);

                 handler.deliver(inst);
              }
           }
           handler.complete();
	}

	else {
           jobjectArray jAr=(jobjectArray)env->CallObjectMethod((jobject)pr.jProvider,id,jRef,
                         rRole,false,false,pl);
           JMPIjvm::checkException(env);

           STAT_PMS_PROVIDEREND;

           handler.processing();
           if (jAr) {
              for (int i=0,m=env->GetArrayLength(jAr); i<m; i++) {
                 JMPIjvm::checkException(env);
                 jobject jInst=env->GetObjectArrayElement(jAr,i);
                 JMPIjvm::checkException(env);
                 CIMInstance inst=*((CIMInstance*)env->CallIntMethod(jInst,JMPIjvm::jv.CIMInstanceCInst));
                 JMPIjvm::checkException(env);

                 CIMClass cls=pr._cimom_handle->getClass(context,request->nameSpace,
                     inst.getClassName(),false,true,true,CIMPropertyList());
                 const CIMObjectPath& op=inst.getPath();
                 CIMObjectPath iop=inst.buildPath(cls);
                 iop.setNameSpace(op.getNameSpace());
                 inst.setPath(iop);

                 handler.deliver(inst);
              }
           }
           handler.complete();
	}

    }
    HandlerCatch(handler);

    if (env) JMPIjvm::detachThread();

    PEG_METHOD_EXIT();

    return(response);
}


Message * JMPIProviderManager::handleReferenceNamesRequest(const Message * message) throw()
{
    PEG_METHOD_ENTER(TRC_PROVIDERMANAGER,
        "JMPIProviderManager::handleReferenceNamesRequest");

    HandlerIntro(ReferenceNames,message,request,response,
                 handler,Array<CIMObjectPath>());
    JNIEnv *env=NULL;
    Boolean mode24=false;
    CIMPropertyList propertyList;
    jobjectArray pl=NULL;

    try {
        Logger::put(Logger::STANDARD_LOG, System::CIMSERVER, Logger::TRACE,
            "JMPIProviderManager::handleReferenceNamesRequest - Host name: $0  Name space: $1  Class name: $2",
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

        // resolve provider name
        ProviderName name = _resolveProviderName(
            request->operationContext.get(ProviderIdContainer::NAME));

        // get cached or load new provider module
        JMPIProvider::OpProviderHolder ph =
            providerManager.getProvider(name.getPhysicalName(), name.getLogicalName(), String::EMPTY);

        // convert arguments
        OperationContext context;

        context.insert(request->operationContext.get(IdentityContainer::NAME));
        context.insert(request->operationContext.get(AcceptLanguageListContainer::NAME));
        context.insert(request->operationContext.get(ContentLanguageListContainer::NAME));

  	JMPIProvider & pr=ph.GetProvider();

        PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
            "Calling provider.referenceNames: " + pr.getName());


        DDD(cerr<<"--- JMPIProviderManager::referenceNames"<<" role: >"<<request->role<<"< aCls "<<
	   request->resultClass<<endl);

	JvmVector *jv;
        env=JMPIjvm::attachThread(&jv);

        jobject jRef=env->NewObject(jv->CIMObjectPathClassRef,
	               jv->CIMObjectPathNewI,(jint)&objectPath);
	JMPIjvm::checkException(env);

        jstring rRole=env->NewStringUTF(request->role.getCString());
	JMPIjvm::checkException(env);

        JMPIProvider::pm_service_op_lock op_lock(&pr);

        STAT_GETSTARTTIME;

        jmethodID id=env->GetMethodID((jclass)pr.jProviderClass,"referenceNames",
           "(Lorg/pegasus/jmpi/CIMObjectPath;Ljava/lang/String;)Ljava/util/Vector;");

	if (id==NULL) {
           env->ExceptionClear();
	   id=env->GetMethodID((jclass)pr.jProviderClass,"referenceNames",
              "(Lorg/pegasus/jmpi/CIMObjectPath;Ljava/lang/String;)[Lorg/pegasus/jmpi/CIMObjectPath;");
           JMPIjvm::checkException(env);
	   mode24=true;
	}
        JMPIjvm::checkException(env);

	if (!mode24) {
           jobject jVec=env->CallObjectMethod((jobject)pr.jProvider,id,jRef,rRole);
           JMPIjvm::checkException(env);

           STAT_PMS_PROVIDEREND;

           handler.processing();
	   if (jVec) {
	      for (int i=0,m=env->CallIntMethod(jVec,JMPIjvm::jv.VectorSize); i<m; i++) {
                 JMPIjvm::checkException(env);
	         jobject jCop=env->CallObjectMethod(jVec,JMPIjvm::jv.VectorElementAt,i);
                 JMPIjvm::checkException(env);
                 CIMObjectPath *cop=((CIMObjectPath*)env->CallIntMethod
	            (jCop,JMPIjvm::jv.CIMObjectPathCInst));
                 JMPIjvm::checkException(env);
                 handler.deliver(*cop);
 	      }
           }
           handler.complete();
	}

	else {
           jobjectArray jAr=(jobjectArray)env->CallObjectMethod((jobject)pr.jProvider,id,jRef,rRole);
           JMPIjvm::checkException(env);

           STAT_PMS_PROVIDEREND;

           handler.processing();
	   if (jAr) {
	      for (int i=0,m=env->GetArrayLength(jAr); i<m; i++) {
                 JMPIjvm::checkException(env);
	         jobject jCop=env->GetObjectArrayElement(jAr,i);
                 JMPIjvm::checkException(env);
                 CIMObjectPath *cop=((CIMObjectPath*)env->CallIntMethod
	            (jCop,JMPIjvm::jv.CIMObjectPathCInst));
                 JMPIjvm::checkException(env);
                 handler.deliver(*cop);
 	      }
           }
           handler.complete();
	}
    }
    HandlerCatch(handler);

    if (env) JMPIjvm::detachThread();

    PEG_METHOD_EXIT();

    return(response);
}


Message * JMPIProviderManager::handleInvokeMethodRequest(const Message * message) throw()
{
    PEG_METHOD_ENTER(TRC_PROVIDERMANAGER,
        "JMPIProviderManager::handleInvokeMethodRequest");

    HandlerIntroMethod(InvokeMethod,message,request,response,
                 handler);
    JNIEnv *env=NULL;
    Boolean mode24=false;

    try {
        Logger::put(Logger::STANDARD_LOG, System::CIMSERVER, Logger::TRACE,
            "JMPIProviderManager::handleInvokeMethodRequest - Host name: $0  Name space: $1  Class name: $2",
            System::getHostName(),
            request->nameSpace.getString(),
            request->instanceName.getClassName().getString());

        // make target object path
        CIMObjectPath objectPath(
            System::getHostName(),
            request->nameSpace,
            request->instanceName.getClassName(),
            request->instanceName.getKeyBindings());

        // resolve provider name
        ProviderName name = _resolveProviderName(
            request->operationContext.get(ProviderIdContainer::NAME));

        // get cached or load new provider module
        JMPIProvider::OpProviderHolder ph =
            providerManager.getProvider(name.getPhysicalName(), name.getLogicalName(), String::EMPTY);

        // convert arguments
        OperationContext context;

        context.insert(request->operationContext.get(IdentityContainer::NAME));
        context.insert(request->operationContext.get(AcceptLanguageListContainer::NAME));
        context.insert(request->operationContext.get(ContentLanguageListContainer::NAME));

 	JMPIProvider & pr=ph.GetProvider();

        PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
            "Calling provider.invokeMethod: " + pr.getName());

	JvmVector *jv;
        env=JMPIjvm::attachThread(&jv);

        jobject jRef=env->NewObject(jv->CIMObjectPathClassRef,
	               jv->CIMObjectPathNewI,(jint)&objectPath);
	JMPIjvm::checkException(env);

        jstring jMethod=env->NewStringUTF(request->methodName.getString().getCString());
	JMPIjvm::checkException(env);

        STAT_GETSTARTTIME;

        jmethodID id=env->GetMethodID((jclass)pr.jProviderClass,"invokeMethod",
           "(Lorg/pegasus/jmpi/CIMObjectPath;Ljava/lang/String;"
	    "Ljava/util/Vector;Ljava/util/Vector;)Lorg/pegasus/jmpi/CIMValue;");

	if (id==NULL) {
           env->ExceptionClear();
	   id=env->GetMethodID((jclass)pr.jProviderClass,"invokeMethod",
              "(Lorg/pegasus/jmpi/CIMObjectPath;Ljava/lang/String;"
	      "[Lorg/pegasus/jmpi/CIMArgument;[Lorg/pegasus/jmpi/CIMArgument;)Lorg/pegasus/jmpi/CIMValue;");
           JMPIjvm::checkException(env);
	   mode24=true;
	}
        JMPIjvm::checkException(env);

	if (!mode24) {
           jobject jVecIn=env->NewObject(jv->VectorClassRef,jv->VectorNew);
           JMPIjvm::checkException(env);
           for (int i=0,m=request->inParameters.size(); i<m; i++) {
              const CIMParamValue & parm=request->inParameters[i];
	      const CIMValue v=parm.getValue();
              CIMProperty *p=new CIMProperty(parm.getParameterName(),v,v.getArraySize());
              jobject prop=env->NewObject(jv->CIMPropertyClassRef,
	         jv->CIMPropertyNewI,(jint)p);
              env->CallVoidMethod(jVecIn,jv->VectorAddElement,prop);
	   }

           jobject jVecOut=env->NewObject(jv->VectorClassRef,jv->VectorNew);
           JMPIjvm::checkException(env);

           jobject jValue=env->CallObjectMethod((jobject)pr.jProvider,id,jRef,jMethod,jVecIn,jVecOut);
           JMPIjvm::checkException(env);

           STAT_PMS_PROVIDEREND;

           handler.processing();

           CIMValue *v=((CIMValue*)env->CallIntMethod(jValue,JMPIjvm::jv.CIMValueCInst));
	   handler.deliver(*v);

           for (int i=0,m=env->CallIntMethod(jVecOut,JMPIjvm::jv.VectorSize); i<m; i++) {
              JMPIjvm::checkException(env);
	      jobject jProp=env->CallObjectMethod(jVecOut,JMPIjvm::jv.VectorElementAt,i);
              JMPIjvm::checkException(env);
              CIMProperty *p=((CIMProperty*)env->CallIntMethod(jProp,JMPIjvm::jv.PropertyCInst));
              JMPIjvm::checkException(env);
              handler.deliverParamValue(CIMParamValue(p->getName().getString(),p->getValue()));
           }
           handler.complete();
	}
        else {
	   Uint32 m=request->inParameters.size();

	   jobjectArray jArIn=env->NewObjectArray(m,jv->CIMArgumentClassRef,NULL);
           for (Uint32 i=0; i<m; i++) {
              CIMParamValue *parm=new CIMParamValue(request->inParameters[i]);
              jobject jArg=env->NewObject(jv->CIMArgumentClassRef,jv->CIMArgumentNewI,(jint)parm);
              env->SetObjectArrayElement(jArIn,i,jArg);
	   }
	   jobjectArray jArOut=env->NewObjectArray(24,jv->CIMArgumentClassRef,NULL);

           jobject jValue=env->CallObjectMethod((jobject)pr.jProvider,id,jRef,jMethod,jArIn,jArOut);
           JMPIjvm::checkException(env);

           STAT_PMS_PROVIDEREND;

           handler.processing();

           CIMValue *v=((CIMValue*)env->CallIntMethod(jValue,JMPIjvm::jv.CIMValueCInst));
	   handler.deliver(*v);

           for (int i=0; i<24; i++) {
	      jobject jArg=env->GetObjectArrayElement(jArOut,i);
              JMPIjvm::checkException(env);
	      if (jArg==NULL) break;
              CIMParamValue *p=((CIMParamValue*)env->CallIntMethod(jArg,JMPIjvm::jv.ArgumentCInst));
              JMPIjvm::checkException(env);
              handler.deliverParamValue(*p);
           }
           handler.complete();
	}
    }
    HandlerCatch(handler);

    if (env) JMPIjvm::detachThread();

    PEG_METHOD_EXIT();

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

Message * JMPIProviderManager::handleCreateSubscriptionRequest(const Message * message) throw()
{
    PEG_METHOD_ENTER(TRC_PROVIDERMANAGER, "JMPIProviderManager::handleCreateSubscriptionRequest");

    HandlerIntroInd(CreateSubscription,message,request,response,
                 handler);
    JNIEnv *env=NULL;
    try {
        const CIMObjectPath &x=request->subscriptionInstance.getPath();

        String providerName,providerLocation;
        CIMInstance req_provider, req_providerModule;
        ProviderIdContainer pidc = (ProviderIdContainer)request->operationContext.get(ProviderIdContainer::NAME);
        req_provider = pidc.getProvider();
        req_providerModule = pidc.getModule();
        LocateIndicationProviderNames(req_provider, req_providerModule,providerName,providerLocation);

        Logger::put(Logger::STANDARD_LOG, System::CIMSERVER, Logger::TRACE,
            "JMPIProviderManager::handleCreateSubscriptionRequest - Host name: $0  Name space: $1  Provider name(s): $2",
            System::getHostName(),
            request->nameSpace.getString(),
            providerName);

        String fileName = resolveFileName(providerLocation);

        // get cached or load new provider module
        JMPIProvider::OpProviderHolder ph =
            providerManager.getProvider(fileName, providerName, String::EMPTY);

        indProvRecord *prec=NULL;
        provTab.lookup(providerName,prec);
        if (prec) prec->count++;
        else {
           prec=new indProvRecord();
           provTab.insert(providerName,prec);
        }

        indSelectRecord *srec=new indSelectRecord();
        const CIMObjectPath &sPath=request->subscriptionInstance.getPath();
        selxTab.insert(sPath.toString(),srec);

        // convert arguments
        OperationContext *context=new OperationContext();

	if (prec->ctx==NULL) {
           prec->ctx=context;
	}

        context->insert(request->operationContext.get(IdentityContainer::NAME));
        context->insert(request->operationContext.get(AcceptLanguageListContainer::NAME));
        context->insert(request->operationContext.get(ContentLanguageListContainer::NAME));
        context->insert(request->operationContext.get(SubscriptionInstanceContainer::NAME));
        context->insert(request->operationContext.get(SubscriptionLanguageListContainer::NAME));
        context->insert(request->operationContext.get(SubscriptionFilterConditionContainer::NAME));

        CIMObjectPath subscriptionName = request->subscriptionInstance.getPath();

        SubscriptionFilterConditionContainer sub_cntr =  request->operationContext.get
                (SubscriptionFilterConditionContainer::NAME);

        CMPI_SelectExp *eSelx=new CMPI_SelectExp(*context,request->query,
                sub_cntr.getQueryLanguage());
        srec->eSelx=eSelx;

        JMPIProvider & pr=ph.GetProvider();

        PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
            "Calling provider.createSubscriptionRequest: " + pr.getName());

        DDD(cerr<<"--- JMPIProviderManager::createSubscriptionRequest"<<endl);

        for(Uint32 i = 0, n = request->classNames.size(); i < n; i++) {
            CIMObjectPath className(
                System::getHostName(),
                request->nameSpace,
                request->classNames[i]);
            eSelx->classNames.append(className);
        }

        CIMPropertyList propertyList = request->propertyList;
        if (!propertyList.isNull()) {
           Array<CIMName> p=propertyList.getPropertyNameArray();
           int pCount=p.size();
           eSelx->props=(const char**)malloc((1+pCount)*sizeof(char*));
           for (int i=0; i<pCount; i++) {
              eSelx->props[i]=strdup(p[i].getString().getCString());
	   }
           eSelx->props[pCount]=NULL;
        }

	JvmVector *jv;
        env=JMPIjvm::attachThread(&jv);

        jobject jSel=env->NewObject(jv->SelectExpClassRef,
	               jv->SelectExpNewI,(jint)eSelx);
	JMPIjvm::checkException(env);

        jobject jRef=env->NewObject(jv->CIMObjectPathClassRef,
	               jv->CIMObjectPathNewI,(jint)&eSelx->classNames[0]);
	JMPIjvm::checkException(env);

        jstring jType=env->NewStringUTF(request->nameSpace.getString().getCString());
	JMPIjvm::checkException(env);

        JMPIProvider::pm_service_op_lock op_lock(&pr);

        STAT_GETSTARTTIME;

        jmethodID id=env->GetMethodID((jclass)pr.jProviderClass,"activateFilter",
           "(Lorg/pegasus/jmpi/SelectExp;Ljava/lang/String;"
	    "Lorg/pegasus/jmpi/CIMObjectPath;Z)V");
        JMPIjvm::checkException(env);

        env->CallVoidMethod((jobject)pr.jProvider,id,jSel,jType,
	   jRef,(jboolean)0);
        JMPIjvm::checkException(env);

       STAT_PMS_PROVIDEREND;

    }
    HandlerCatch(handler);

    PEG_METHOD_EXIT();

    return(response);
}

Message * JMPIProviderManager::handleDeleteSubscriptionRequest(const Message * message) throw()
{
    PEG_METHOD_ENTER(TRC_PROVIDERMANAGER, "JMPIProviderManager::handleDeleteSubscriptionRequest");

    HandlerIntroInd(DeleteSubscription,message,request,response,
                 handler);
    JNIEnv *env=NULL;
    try {
        String providerName,providerLocation;
        CIMInstance req_provider, req_providerModule;
        ProviderIdContainer pidc = (ProviderIdContainer)request->operationContext.get(ProviderIdContainer::NAME);
        req_provider = pidc.getProvider();
        req_providerModule = pidc.getModule();

        LocateIndicationProviderNames(req_provider, req_providerModule,
           providerName,providerLocation);

        Logger::put(Logger::STANDARD_LOG, System::CIMSERVER, Logger::TRACE,
            "JMPIProviderManager::handleDeleteSubscriptionRequest - Host name: $0  Name space: $1  Provider name(s): $2",
            System::getHostName(),
            request->nameSpace.getString(),
            providerName);

        String fileName = resolveFileName(providerLocation);

        // get cached or load new provider module
        JMPIProvider::OpProviderHolder ph =
            providerManager.getProvider(fileName, providerName, String::EMPTY);


        indProvRecord *prec=NULL;
        provTab.lookup(providerName,prec);
        if (--prec->count<=0) {
           provTab.remove(providerName);
           prec=NULL;
        }

        indSelectRecord *srec=NULL;
        const CIMObjectPath &sPath=request->subscriptionInstance.getPath();
        String sPathString=sPath.toString();
        selxTab.lookup(sPathString,srec);

        CMPI_SelectExp *eSelx=srec->eSelx;
        selxTab.remove(sPathString);

        // convert arguments
        OperationContext context;

        context.insert(request->operationContext.get(IdentityContainer::NAME));
        context.insert(request->operationContext.get(AcceptLanguageListContainer::NAME));
        context.insert(request->operationContext.get(ContentLanguageListContainer::NAME));
        context.insert(request->operationContext.get(SubscriptionInstanceContainer::NAME));
        context.insert(request->operationContext.get(SubscriptionLanguageListContainer::NAME));

        CIMObjectPath subscriptionName = request->subscriptionInstance.getPath();

        JMPIProvider & pr=ph.GetProvider();

        PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
            "Calling provider.deleteSubscriptionRequest: " + pr.getName());

        DDD(cerr<<"--- JMPIProviderManager::deleteSubscriptionRequest"<<endl);

	JvmVector *jv;
        env=JMPIjvm::attachThread(&jv);

        jobject jSel=env->NewObject(jv->SelectExpClassRef,
	               jv->SelectExpNewI,(jint)eSelx);
	JMPIjvm::checkException(env);

        jobject jRef=env->NewObject(jv->CIMObjectPathClassRef,
	               jv->CIMObjectPathNewI,(jint)&eSelx->classNames[0]);
	JMPIjvm::checkException(env);

        jstring jType=env->NewStringUTF(request->nameSpace.getString().getCString());
	JMPIjvm::checkException(env);

        JMPIProvider::pm_service_op_lock op_lock(&pr);

        STAT_GETSTARTTIME;

        jmethodID id=env->GetMethodID((jclass)pr.jProviderClass,"deActivateFilter",
           "(Lorg/pegasus/jmpi/SelectExp;Ljava/lang/String;"
	    "Lorg/pegasus/jmpi/CIMObjectPath;Z)V");
        JMPIjvm::checkException(env);

        env->CallVoidMethod((jobject)pr.jProvider,id,jSel,jType,
	   jRef,(jboolean)(prec==NULL));
        JMPIjvm::checkException(env);

       STAT_PMS_PROVIDEREND;

       delete eSelx;

    }
    HandlerCatch(handler);

    PEG_METHOD_EXIT();

    return(response);
}

Message * JMPIProviderManager::handleEnableIndicationsRequest(const Message * message) throw()
{
    PEG_METHOD_ENTER(TRC_PROVIDERMANAGER, "JMPIProviderManager:: handleEnableIndicationsRequest");

    HandlerIntroInd(EnableIndications,message,request,response,
                 handler);
    try {
        String providerName,providerLocation;
		CIMInstance req_provider, req_providerModule;
		ProviderIdContainer pidc = (ProviderIdContainer)request->operationContext.get(ProviderIdContainer::NAME);
		req_provider = pidc.getProvider();
		req_providerModule = pidc.getModule();

	LocateIndicationProviderNames(req_provider, req_providerModule,
	   providerName,providerLocation);

        indProvRecord *provRec;
        if (provTab.lookup(providerName,provRec)) {
           provRec->enabled=true;
           provRec->handler=new EnableIndicationsResponseHandler(
               request, response, req_provider, _indicationCallback);
        }

        String fileName = resolveFileName(providerLocation);

        // get cached or load new provider module
        JMPIProvider::OpProviderHolder ph =
            providerManager.getProvider(fileName, providerName, String::EMPTY);

    }
    HandlerCatch(handler);

    PEG_METHOD_EXIT();

    return(response);
}

Message * JMPIProviderManager::handleDisableIndicationsRequest(const Message * message) throw()
{
    PEG_METHOD_ENTER(TRC_PROVIDERMANAGER, "JMPIProviderManager:: handleDisableIndicationsRequest");

    HandlerIntroInd(DisableIndications,message,request,response,
                 handler);
    try {
        String providerName,providerLocation;
		CIMInstance req_provider, req_providerModule;
		ProviderIdContainer pidc = (ProviderIdContainer)request->operationContext.get(ProviderIdContainer::NAME);

		req_provider = pidc.getProvider();
		req_providerModule = pidc.getModule();

	LocateIndicationProviderNames(req_provider, req_providerModule,
	   providerName,providerLocation);

        indProvRecord *provRec;
        if (provTab.lookup(providerName,provRec)) {
           provRec->enabled=false;
           if (provRec->handler) delete provRec->handler;
	   provRec->handler=NULL;
        }

        String fileName = resolveFileName(providerLocation);

        // get cached or load new provider module
        JMPIProvider::OpProviderHolder ph =
            providerManager.getProvider(fileName, providerName, String::EMPTY);
    }
    HandlerCatch(handler);

    PEG_METHOD_EXIT();

    return(response);
}

Message * JMPIProviderManager::handleDisableModuleRequest(const Message * message) throw()
{
    PEG_METHOD_ENTER(TRC_PROVIDERMANAGER, "JMPIProviderManager::handleDisableModuleRequest");

    CIMDisableModuleRequestMessage * request =
        dynamic_cast<CIMDisableModuleRequestMessage *>(const_cast<Message *>(message));

    PEGASUS_ASSERT(request != 0);

    // get provider module name
    String moduleName;
    CIMInstance mInstance = request->providerModule;
    Uint32 pos = mInstance.findProperty(CIMName ("Name"));

    if(pos != PEG_NOT_FOUND)
    {
        mInstance.getProperty(pos).getValue().get(moduleName);
    }

    Boolean disableProviderOnly = request->disableProviderOnly;

    Array<Uint16> operationalStatus;
    // Assume success.
    operationalStatus.append(CIM_MSE_OPSTATUS_VALUE_STOPPED);

    //
    // Unload providers
    //
    Array<CIMInstance> _pInstances = request->providers;

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

Message * JMPIProviderManager::handleEnableModuleRequest(const Message * message) throw()
{
    PEG_METHOD_ENTER(TRC_PROVIDERMANAGER, "JMPIProviderManager::handleEnableModuleRequest");

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

Message * JMPIProviderManager::handleStopAllProvidersRequest(const Message * message) throw()
{
    PEG_METHOD_ENTER(TRC_PROVIDERMANAGER, "JMPIProviderManager::handleStopAllProvidersRequest");

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

Message * JMPIProviderManager::handleInitializeProviderRequest(const Message * message)
{
    PEG_METHOD_ENTER(TRC_PROVIDERMANAGER, "JMPIProviderManager::handleInitializeProviderRequest");

    HandlerIntroInit(InitializeProvider,message,request,response,handler);

    try
    {
        // resolve provider name
	ProviderName name = _resolveProviderName(
	    request->operationContext.get(ProviderIdContainer::NAME));

        // get cached or load new provider module
        JMPIProvider::OpProviderHolder ph =
            providerManager.getProvider(name.getPhysicalName(), 
		name.getLogicalName(), String::EMPTY);

    }
    HandlerCatch(handler);

    PEG_METHOD_EXIT();

    return(response);
}

Message * JMPIProviderManager::handleUnsupportedRequest(const Message * message) throw()
{
    PEG_METHOD_ENTER(TRC_PROVIDERMANAGER, "JMPIProviderManager::handleUnsupportedRequest");

    PEG_METHOD_EXIT();

    // a null response implies unsupported or unknown operation
    return(0);
}

ProviderName JMPIProviderManager::_resolveProviderName(
    const ProviderIdContainer & providerId)
{
    String providerName;
    String fileName;
    String interfaceName;
    CIMValue genericValue;

    genericValue = providerId.getProvider().getProperty(
        providerId.getProvider().findProperty("Name")).getValue();
    genericValue.get(providerName);

    genericValue = providerId.getModule().getProperty(
        providerId.getModule().findProperty("Location")).getValue();
    genericValue.get(fileName);
    fileName = resolveFileName(fileName);

    // ATTN: This attribute is probably not required
    genericValue = providerId.getModule().getProperty(
        providerId.getModule().findProperty("InterfaceType")).getValue();
    genericValue.get(interfaceName);

    return ProviderName(providerName, fileName, interfaceName, 0);
}

String JMPIProviderManager::resolveFileName(String fileName)
{
    String name;
    #if defined(PEGASUS_OS_TYPE_WINDOWS)
    name = fileName; // + String(".dll");
    #elif defined(PEGASUS_OS_HPUX) && defined(PEGASUS_PLATFORM_HPUX_PARISC_ACC)
    name = ConfigManager::getHomedPath(ConfigManager::getInstance()->getCurrentValue("providerDir"));
    name.append(String("/") + fileName); // + String(".sl"));
    #elif defined(PEGASUS_OS_HPUX) && !defined(PEGASUS_PLATFORM_HPUX_PARISC_ACC)
    name = ConfigManager::getHomedPath(ConfigManager::getInstance()->getCurrentValue("providerDir"));
    name.append(String("/") + fileName); // + String(".so"));
    #elif defined(PEGASUS_OS_OS400)
    name = filrName;
    #else
    name = ConfigManager::getHomedPath(ConfigManager::getInstance()->getCurrentValue("providerDir"));
    name.append(String("/") + fileName); // + String(".so"));
    #endif
    return name;
}

PEGASUS_NAMESPACE_END
