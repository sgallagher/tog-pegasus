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
// Modified By: Seema Gupta (gseema@in.ibm.com) for PEP135
//              Josephine Eskaline Joyce, IBM (jojustin@in.ibm.com) for PEP#101
//
//%/////////////////////////////////////////////////////////////////////////////

#include "JMPIProviderManager.h"

#include "JMPIImpl.h"

#include <Pegasus/Common/CIMMessage.h>
#include <Pegasus/Common/OperationContext.h>
#include <Pegasus/Common/Tracer.h>
#include <Pegasus/Common/StatisticalData.h>
#include <Pegasus/Common/Logger.h>
#include <Pegasus/Common/MessageLoader.h> //l10n
#include <Pegasus/Common/Constants.h>

#include <Pegasus/Config/ConfigManager.h>

#include <Pegasus/ProviderManager2/ProviderName.h>
#include <Pegasus/ProviderManager2/JMPI/JMPIProvider.h>
#include <Pegasus/ProviderManager2/JMPI/JMPIProviderModule.h>
#include <Pegasus/ProviderManager2/ProviderManagerService.h>

#include <Pegasus/ProviderManager2/CMPI/CMPI_SelectExp.h>


PEGASUS_USING_STD;
PEGASUS_NAMESPACE_BEGIN

int JMPIProviderManager::trace=0;

#ifdef PEGASUS_DEBUG
#define DDD(x) if (JMPIProviderManager::trace) x;
#else
#define DDD(x)
#endif
#define JMPI_LOCALONLY false
#define JMPI_DEEPINHERITANCE true

#include "Convert.h"

void JMPIProviderManager::debugPrintMethodPointers (JNIEnv *env, jclass jc)
{
   // cd ${PEGAUSE_HOME}/src/Pegasus/ProviderManager2/JMPI/org/pegasus/jmpi/tests/JMPI_TestPropertyTypes
   // javap -s -p JMPI_TestPropertyTypes
   static const char *methodNames[][2] = {
      // CIMProvider
      //   cimom-2003-11-24/org/snia/wbem/provider/CIMProvider.java
      //   src/Pegasus/ProviderManager2/JMPI/org/pegasus/jmpi/CIMProvider.java
      {"initialize","(Lorg/pegasus/jmpi/CIMOMHandle;)V"},
      {"cleanup","()V"},
      // InstanceProvider
      //   cimom-2003-11-24/org/snia/wbem/provider/InstanceProvider.java
      //   src/Pegasus/ProviderManager2/JMPI/org/pegasus/jmpi/CIMInstanceProvider.java
      {"enumInstances","(Lorg/pegasus/jmpi/CIMObjectPath;ZLorg/pegasus/jmpi/CIMClass;)Ljava/util/Vector;"},
      {"enumInstances","(Lorg/pegasus/jmpi/CIMObjectPath;ZLorg/pegasus/jmpi/CIMClass;Z)Ljava/util/Vector;"},
      {"getInstance","(Lorg/pegasus/jmpi/CIMObjectPath;Lorg/pegasus/jmpi/CIMClass;Z)Lorg/pegasus/jmpi/CIMInstance;"},
      {"createInstance","(Lorg/pegasus/jmpi/CIMObjectPath;Lorg/pegasus/jmpi/CIMInstance;)Lorg/pegasus/jmpi/CIMObjectPath;"},
      {"setInstance","(Lorg/pegasus/jmpi/CIMObjectPath;Lorg/pegasus/jmpi/CIMInstance;)V"},
      {"deleteInstance","(Lorg/pegasus/jmpi/CIMObjectPath;)V"},
      {"execQuery","(Lorg/pegasus/jmpi/CIMObjectPath;Ljava/lang/String;ILorg/pegasus/jmpi/CIMClass;)Ljava/util/Vector;"},
      // MethodProvider
      //   cimom-2003-11-24/org/snia/wbem/provider/MethodProvider.java
      //   src/Pegasus/ProviderManager2/JMPI/org/pegasus/jmpi/CIMMethodProvider.java
      {"invokeMethod","(Lorg/pegasus/jmpi/CIMObjectPath;Ljava/lang/String;Ljava/util/Vector;Ljava/util/Vector;)Lorg/pegasus/jmpi/CIMValue;"},
      // PropertyProvider
      //   cimom-2003-11-24/org/snia/wbem/provider/PropertyProvider.java
      // AssociatorProvider
      //   cimom-2003-11-24/org/snia/wbem/provider20/AssociatorProvider.java
      //   src/Pegasus/ProviderManager2/JMPI/org/pegasus/jmpi/CIMAssociationProvider.java
      // CIMProviderRouter
      //   cimom-2003-11-24/org/snia/wbem/provider20/CIMProviderRouter.java
      // EventProvider
      //   cimom-2003-11-24/org/snia/wbem/provider20/EventProvider.java
      //   src/Pegasus/ProviderManager2/JMPI/org/pegasus/jmpi/EventProvider.java
      {"authorizeFilter","(Lorg/pegasus/jmpi/SelectExp;Ljava/lang/String;Lorg/pegasus/jmpi/CIMObjectPath;Ljava/lang/String;)V"},
      {"mustPoll","(Lorg/pegasus/jmpi/SelectExp;Ljava/lang/String;Lorg/pegasus/jmpi/CIMObjectPath;)Z"},
      {"activateFilter","(Lorg/pegasus/jmpi/SelectExp;Ljava/lang/String;Lorg/pegasus/jmpi/CIMObjectPath;Z)V"},
      {"deActivateFilter","(Lorg/pegasus/jmpi/SelectExp;Ljava/lang/String;Lorg/pegasus/jmpi/CIMObjectPath;Z)V"},
      // IndicationHandler
      //   cimom-2003-11-24/org/snia/wbem/provider20/IndicationHandler.java
      // ProviderAdapter
      //   cimom-2003-11-24/org/snia/wbem/provider20/ProviderAdapter.java
      // JMPI_TestPropertyTypes
      {"findObjectPath","(Lorg/pegasus/jmpi/CIMObjectPath;)I"},
      {"testPropertyTypesValue","(Lorg/pegasus/jmpi/CIMInstance;)V"}
   };

   if (!env)
   {
      DDD(PEGASUS_STD(cout)<<"--- JMPIProviderManager::debugPrintMethodPointers: env is NULL!"<<PEGASUS_STD(endl));
      return;
   }
   if (!jc)
   {
      DDD(PEGASUS_STD(cout)<<"--- JMPIProviderManager::debugPrintMethodPointers: jc is NULL!"<<PEGASUS_STD(endl));
      return;
   }

   for (int i = 0; i < (int)(sizeof (methodNames)/sizeof (methodNames[0])); i++)
   {
      jmethodID id = env->GetMethodID(jc,methodNames[i][0], methodNames[i][1]);
      DDD(PEGASUS_STD(cout)<<"--- JMPIProviderManager::debugPrintMethodPointers: "<<methodNames[i][0]<<", id = "<<PEGASUS_STD(hex)<<(int)id<<PEGASUS_STD(dec)<<PEGASUS_STD(endl));
      env->ExceptionClear();
   }

   env->ExceptionClear();
}

void
debugIntrospectJavaObject (JNIEnv *env, jobject jInst)
{
   jclass       jInstClass             = env->GetObjectClass(jInst);
   jclass       jInstSuperClass        = env->GetSuperclass(jInstClass);
   jmethodID    jmidGetDeclaredMethods = env->GetMethodID(jInstClass, "getDeclaredMethods", "()[Ljava/lang/reflect/Method;");

   if (!jmidGetDeclaredMethods)
   {
      env->ExceptionClear();
      jmidGetDeclaredMethods = env->GetMethodID(jInstSuperClass, "getDeclaredMethods", "()[Ljava/lang/reflect/Method;");
   }

   if (jmidGetDeclaredMethods)
   {
      jobjectArray jarrayDeclaredMethods  = (jobjectArray)env->CallObjectMethod(jInstClass,
                                                                                jmidGetDeclaredMethods);

      if (jarrayDeclaredMethods)
      {
         for (int i = 0, iLen = env->GetArrayLength (jarrayDeclaredMethods); i < iLen; i++)
         {
            JMPIjvm::checkException(env);

            jobject jDeclaredMethod      = env->GetObjectArrayElement (jarrayDeclaredMethods, i);
            jclass  jDeclaredMethodClass = env->GetObjectClass (jDeclaredMethod);

            JMPIjvm::checkException(env);

            jmethodID   jmidToString  = env->GetMethodID (jDeclaredMethodClass, "toString", "()Ljava/lang/String;");
            jstring     jstringResult = (jstring)env->CallObjectMethod (jDeclaredMethod, jmidToString);
            const char *pszResult     = env->GetStringUTFChars(jstringResult, 0);

            PEGASUS_STD(cout)<<"--- JMPIProviderManager::debugIntrospectJavaObject: "<<pszResult<<PEGASUS_STD(endl);

            env->ReleaseStringUTFChars (jstringResult, pszResult);
         }
      }
   }


   env->ExceptionClear();
}

void
debugDumpJavaObject (JNIEnv *env, jobject jInst)
{
   jclass      jInstClass      = env->GetObjectClass(jInst);
   jclass      jInstSuperClass = env->GetSuperclass(jInstClass);
   jmethodID   jmidToString1   = env->GetMethodID(jInstClass,      "toString", "()Ljava/lang/String;");
   jmethodID   jmidToString2   = env->GetMethodID(jInstSuperClass, "toString", "()Ljava/lang/String;");
   if (!jmidToString1 || !jmidToString2)
   {
      env->ExceptionClear();
      return;
   }
   jstring     jstringResult1  = (jstring)env->CallObjectMethod(jInstClass,      jmidToString1);
   jstring     jstringResult2  = (jstring)env->CallObjectMethod(jInstSuperClass, jmidToString2);
   jstring     jstringResult3  = (jstring)env->CallObjectMethod(jInst,           jmidToString1);
   const char *pszResult1      = env->GetStringUTFChars(jstringResult1, 0);
   const char *pszResult2      = env->GetStringUTFChars(jstringResult2, 0);
   const char *pszResult3      = env->GetStringUTFChars(jstringResult3, 0);

   jmethodID jmidCInst = env->GetMethodID(env->GetObjectClass(jInst),JMPIjvm::jv.instanceMethodNames[22].methodName, JMPIjvm::jv.instanceMethodNames[22].signature);

   PEGASUS_STD(cout)<<"--- JMPIProviderManager::debugIntrospectJavaObject: jInstClass = "<<jInstClass<<", jInstSuperClass = "<<jInstSuperClass<<", jClassShouldBe = "<<JMPIjvm::jv.classRefs[18]<<", jmidCInst = "<<jmidCInst<<PEGASUS_STD(endl);
   PEGASUS_STD(cout)<<"pszResult1 = "<<pszResult1<<PEGASUS_STD(endl);
   PEGASUS_STD(cout)<<"pszResult2 = "<<pszResult2<<PEGASUS_STD(endl);
   PEGASUS_STD(cout)<<"pszResult3 = "<<pszResult3<<PEGASUS_STD(endl);

   env->ReleaseStringUTFChars (jstringResult1, pszResult1);
   env->ReleaseStringUTFChars (jstringResult2, pszResult2);
   env->ReleaseStringUTFChars (jstringResult3, pszResult3);

   env->ExceptionClear();
}

JMPIProviderManager::IndProvTab    JMPIProviderManager::provTab;
JMPIProviderManager::IndSelectTab  JMPIProviderManager::selxTab;
JMPIProviderManager::ProvRegistrar JMPIProviderManager::provReg;

JMPIProviderManager::JMPIProviderManager(Mode m)
{
   mode=m;
   _subscriptionInitComplete = false;

#ifdef PEGASUS_DEBUG
   if (getenv("PEGASUS_JMPI_TRACE"))
      JMPIProviderManager::trace = 1;
   else
      JMPIProviderManager::trace = 0;
#else
   JMPIProviderManager::trace = 0;
#endif
}

JMPIProviderManager::~JMPIProviderManager(void)
{
}

Boolean JMPIProviderManager::insertProvider(const ProviderName & name,
            const String &ns, const String &cn)
{
    String key(ns+String("::")+cn+String("::")+CIMValue(name.getCapabilitiesMask()).toString());

    DDD(PEGASUS_STD(cout)<<"--- JMPIProviderManager::insertProvider: "<<key<<PEGASUS_STD(endl));

    return provReg.insert(key,name);
}

Message * JMPIProviderManager::processMessage(Message * request) throw()
{
    PEG_METHOD_ENTER(TRC_PROVIDERMANAGER, "JMPIProviderManager::processMessage()");

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

    case CIM_GET_PROPERTY_REQUEST_MESSAGE:
        response = handleGetPropertyRequest(request);
        break;

    case CIM_SET_PROPERTY_REQUEST_MESSAGE:
        response = handleSetPropertyRequest(request);
        break;

    case CIM_INVOKE_METHOD_REQUEST_MESSAGE:
        response = handleInvokeMethodRequest(request);
        break;

    case CIM_CREATE_SUBSCRIPTION_REQUEST_MESSAGE:
        response = handleCreateSubscriptionRequest(request);
        break;

/*  case CIM_MODIFY_SUBSCRIPTION_REQUEST_MESSAGE:
        response = handleModifySubscriptionRequest(request);
        break;
*/
    case CIM_DELETE_SUBSCRIPTION_REQUEST_MESSAGE:
        response = handleDeleteSubscriptionRequest(request);
        break;

/*  case CIM_EXPORT_INDICATION_REQUEST_MESSAGE:
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
        PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL2,
                         "*** Unsupported Request "+request->getType());
        DDD(PEGASUS_STD(cout)<<"--- JMPIProviderManager::processMessage: Unsupported request "<<request->getType ()<<PEGASUS_STD(endl));

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
       pl=(jobjectArray)env->NewObjectArray(s,jv->StringClassRef,initial);
       for (Uint32 i=0; i<s; i++) {
           env->SetObjectArrayElement
              (pl,i,env->NewStringUTF(list[i].getString().getCString()));
       }
    }
    return pl;
}

Message * JMPIProviderManager::handleGetInstanceRequest(const Message * message) throw()
{
    PEG_METHOD_ENTER(TRC_PROVIDERMANAGER,"JMPIProviderManager::handleGetInstanceRequest");

    HandlerIntro(GetInstance,message,request,response,handler,CIMInstance());

    typedef enum {
       METHOD_UNKNOWN = 0,
       METHOD_PEGASUS_24,
       METHOD_SNIA_PROVIDER20,
    } METHOD_VERSION;
    METHOD_VERSION   eMethodFound  = METHOD_UNKNOWN;
    JNIEnv          *env           = NULL;

    try {
        Logger::put(Logger::STANDARD_LOG, System::CIMSERVER, Logger::TRACE,
            "JMPIProviderManager::handleGetInstanceRequest - Host name: $0  Name space: $1  Class name: $2",
            System::getHostName(),
            request->nameSpace.getString(),
            request->instanceName.getClassName().getString());

        DDD(PEGASUS_STD(cout)<<"--- JMPIProviderManager::handleGetInstanceRequest: hostname = "<<System::getHostName()<<", namespace = "<<request->nameSpace.getString()<<", classname = "<<request->instanceName.getClassName().getString()<<PEGASUS_STD(endl));

        // make target object path
        CIMObjectPath *objectPath = new CIMObjectPath (System::getHostName(),
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
        JMPIProvider &pr=ph.GetProvider();

        PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4, "Calling provider.getInstance: " + pr.getName());

        DDD(PEGASUS_STD(cout)<<"--- JMPIProviderManager::handleGetInstanceRequest: Calling provider getInstance: "<<pr.getName()<<PEGASUS_STD(endl));

        JvmVector *jv = 0;

        env = JMPIjvm::attachThread(&jv);

        JMPIProvider::pm_service_op_lock op_lock(&pr);

        STAT_GETSTARTTIME;

        jmethodID id = NULL;

        // public abstract org.pegasus.jmpi.CIMInstance getInstance (org.pegasus.jmpi.CIMObjectPath cop,
        //                                                           org.pegasus.jmpi.CIMClass      cimClass,
        //                                                           boolean                        localOnly)
        //        throws org.pegasus.jmpi.CIMException
        id = env->GetMethodID((jclass)pr.jProviderClass,
                              "getInstance",
                              "(Lorg/pegasus/jmpi/CIMObjectPath;Lorg/pegasus/jmpi/CIMClass;Z)Lorg/pegasus/jmpi/CIMInstance;");

        if (id != NULL)
        {
            eMethodFound = METHOD_SNIA_PROVIDER20;
            DDD (PEGASUS_STD(cout)<<"--- JMPIProviderManager::handleGetInstanceRequest: found METHOD_SNIA_PROVIDER20."<<PEGASUS_STD(endl));
        }

        if (id == NULL)
        {
            env->ExceptionClear();

            // public org.pegasus.jmpi.CIMInstance getInstance (org.pegasus.jmpi.CIMObjectPath op,
            //                                                  boolean                        localOnly,
            //                                                  boolean                        includeQualifiers,
            //                                                  boolean                        includeClassOrigin,
            //                                                  java.lang.String[]             propertyList,
            //                                                  org.pegasus.jmpi.CIMClass      cc)
            //        throws org.pegasus.jmpi.CIMException
            id = env->GetMethodID((jclass)pr.jProviderClass,
                                  "getInstance",
                                  "(Lorg/pegasus/jmpi/CIMObjectPath;ZZZ[Ljava/lang/String;Lorg/pegasus/jmpi/CIMClass;)Lorg/pegasus/jmpi/CIMInstance;");

            if (id != NULL)
            {
                eMethodFound = METHOD_PEGASUS_24;
                DDD (PEGASUS_STD(cout)<<"--- JMPIProviderManager::handleGetInstanceRequest: found METHOD_PEGASUS_24."<<PEGASUS_STD(endl));
            }
        }

        JMPIjvm::checkException(env);

        switch (eMethodFound)
        {
        case METHOD_PEGASUS_24:
        {
            jint    jopRef = DEBUG_ConvertCToJava (CIMObjectPath*, jint, objectPath);
            jobject jop    = env->NewObject(jv->CIMObjectPathClassRef,jv->CIMObjectPathNewI,jopRef);

            JMPIjvm::checkException(env);

            CIMClass cls = pr._cimom_handle->getClass(context,
                                                      request->nameSpace,
                                                      request->instanceName.getClassName(),
                                                      false,
                                                      true,
                                                      true,
                                                      CIMPropertyList());
            CIMClass *pcls = new CIMClass (cls);

            JMPIjvm::checkException(env);

            jint    jcimClassRef = DEBUG_ConvertCToJava (CIMClass*, jint, pcls);
            jobject jcimClass    = env->NewObject(jv->CIMClassClassRef,jv->CIMClassNewI,jcimClassRef);

            JMPIjvm::checkException(env);

            jobjectArray jPropertyList = getList(jv,env,request->propertyList);

            jobject jciRet = env->CallObjectMethod((jobject)pr.jProvider,
                                                   id,
                                                   jop,
                                                   request->localOnly,
                                                   request->includeQualifiers,
                                                   request->includeClassOrigin,
                                                   jPropertyList,
                                                   jcimClass);

            JMPIjvm::checkException(env);

            STAT_PMS_PROVIDEREND;

            handler.processing();

            if (jciRet) {
               jint         jciRetRef = env->CallIntMethod(jciRet,JMPIjvm::jv.CIMInstanceCInst);
               CIMInstance *ciRet     = DEBUG_ConvertJavaToC (jint, CIMInstance*, jciRetRef);

               handler.deliver(*ciRet);
            }
            handler.complete();
            break;
        }

        case METHOD_SNIA_PROVIDER20:
        {
            jint    jopRef = DEBUG_ConvertCToJava (CIMObjectPath*, jint, objectPath);
            jobject jop    = env->NewObject(jv->CIMObjectPathClassRef,jv->CIMObjectPathNewI,jopRef);

            JMPIjvm::checkException(env);

            CIMClass cls = pr._cimom_handle->getClass(context,
                                                      request->nameSpace,
                                                      request->instanceName.getClassName(),
                                                      false,
                                                      true,
                                                      true,
                                                      CIMPropertyList());
            CIMClass *pcls = new CIMClass (cls);

            JMPIjvm::checkException(env);

            jint    jcimClassRef = DEBUG_ConvertCToJava (CIMClass*, jint, pcls);
            jobject jcimClass    = env->NewObject(jv->CIMClassClassRef,jv->CIMClassNewI,jcimClassRef);

            JMPIjvm::checkException(env);

            // Modified for Bugzilla# 3679
            jobject jciRet = env->CallObjectMethod((jobject)pr.jProvider,
                                                   id,
                                                   jop,
                                                   jcimClass,
                                                   JMPI_LOCALONLY);

            JMPIjvm::checkException(env);

            STAT_PMS_PROVIDEREND;

            handler.processing();

            if (jciRet) {
               jint         jciRetRef = env->CallIntMethod(jciRet,JMPIjvm::jv.CIMInstanceCInst);
               CIMInstance *ciRet     = DEBUG_ConvertJavaToC (jint, CIMInstance*, jciRetRef);

               handler.deliver(*ciRet);
            }
            handler.complete();
            break;
        }

        case METHOD_UNKNOWN:
        {
            DDD (PEGASUS_STD(cout)<<"--- JMPIProviderManager::handleGetInstanceRequest: should not be here!"<<PEGASUS_STD(endl));
            break;
        }
        }
    }
    HandlerCatch(handler);

    if (env) JMPIjvm::detachThread();

    PEG_METHOD_EXIT();

    STAT_COPYDISPATCHER

    return(response);
}

Message * JMPIProviderManager::handleEnumerateInstancesRequest(const Message * message) throw()
{
    PEG_METHOD_ENTER(TRC_PROVIDERMANAGER,"JMPIProviderManager::handleEnumerateInstanceRequest");

    HandlerIntro(EnumerateInstances,message,request,response,handler,Array<CIMInstance>());

    typedef enum {
       METHOD_UNKNOWN = 0,
       METHOD_PEGASUS_24,
       METHOD_SNIA_PROVIDER20,
    } METHOD_VERSION;
    METHOD_VERSION   eMethodFound  = METHOD_UNKNOWN;
    JNIEnv          *env           = NULL;

    try {
      Logger::put(Logger::STANDARD_LOG, System::CIMSERVER, Logger::TRACE,
            "JMPIProviderManager::handleEnumerateInstancesRequest - Host name: $0  Name space: $1  Class name: $2",
            System::getHostName(),
            request->nameSpace.getString(),
            request->className.getString());

        DDD(PEGASUS_STD(cout)<<"--- JMPIProviderManager::handleEnumerateInstancesRequest: hostname = "<<System::getHostName()<<", namespace = "<<request->nameSpace.getString()<<", classname = "<<request->className.getString()<<PEGASUS_STD(endl));

        // make target object path
        CIMObjectPath *objectPath = new CIMObjectPath (System::getHostName(),
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

        // forward request
        JMPIProvider &pr = ph.GetProvider();

        PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4, "Calling provider.enumerateInstances: " + pr.getName());

        DDD(PEGASUS_STD(cout)<<"--- JMPIProviderManager::handleEnumerateInstancesRequest: Calling provider enumerateInstances: "<<pr.getName()<<PEGASUS_STD(endl));

        JvmVector *jv = 0;

        env = JMPIjvm::attachThread(&jv);

        JMPIProvider::pm_service_op_lock op_lock(&pr);

        STAT_GETSTARTTIME;

        jmethodID id = NULL;

        // public abstract java.util.Vector enumInstances (org.pegasus.jmpi.CIMObjectPath cop,
        //                                                 boolean                        deep,
        //                                                 org.pegasus.jmpi.CIMClass      cimClass,
        //                                                 boolean                        localOnly)
        //        throws org.pegasus.jmpi.CIMException
        id = env->GetMethodID((jclass)pr.jProviderClass,
                              "enumInstances",
                              "(Lorg/pegasus/jmpi/CIMObjectPath;ZLorg/pegasus/jmpi/CIMClass;Z)Ljava/util/Vector;");

        if (id != NULL)
        {
            eMethodFound = METHOD_SNIA_PROVIDER20;
            DDD (PEGASUS_STD(cout)<<"--- JMPIProviderManager::handleEnumerateInstancesRequest: found METHOD_SNIA_PROVIDER20."<<PEGASUS_STD(endl));
        }

        if (id == NULL)
        {
            env->ExceptionClear();

            // public org.pegasus.jmpi.CIMInstance[] enumerateInstances (org.pegasus.jmpi.CIMObjectPath op,
            //                                                           boolean                        localOnly,
            //                                                           boolean                        includeQualifiers,
            //                                                           boolean                        includeClassOrigin,
            //                                                           java.lang.String[]             propertyList,
            //                                                           org.pegasus.jmpi.CIMClass      cc)
            //         throws org.pegasus.jmpi.CIMException
            id = env->GetMethodID((jclass)pr.jProviderClass,
                                  "enumerateInstances",
                                  "(Lorg/pegasus/jmpi/CIMObjectPath;ZZZ[Ljava/lang/String;Lorg/pegasus/jmpi/CIMClass;)[Lorg/pegasus/jmpi/CIMInstance;");

            if (id != NULL)
            {
                eMethodFound = METHOD_PEGASUS_24;
                DDD (PEGASUS_STD(cout)<<"--- JMPIProviderManager::handleEnumerateInstancesRequest: found METHOD_PEGASUS_24."<<PEGASUS_STD(endl));
            }
        }

        JMPIjvm::checkException(env);

        switch (eMethodFound)
        {
        case METHOD_PEGASUS_24:
        {
            jint    jcopRef = DEBUG_ConvertCToJava (CIMObjectPath*, jint, objectPath);
            jobject jcop    = env->NewObject(jv->CIMObjectPathClassRef,jv->CIMObjectPathNewI,jcopRef);

            JMPIjvm::checkException(env);

            CIMClass cls = pr._cimom_handle->getClass(context,
                                                      request->nameSpace,
                                                      request->className,
                                                      false,
                                                      true,
                                                      true,
                                                      CIMPropertyList());
            CIMClass *pcls = new CIMClass (cls);

            JMPIjvm::checkException(env);

            jint    jccRef = DEBUG_ConvertCToJava (CIMClass*, jint, pcls);
            jobject jcc    = env->NewObject(jv->CIMClassClassRef,jv->CIMClassNewI,jccRef);

            JMPIjvm::checkException(env);

            jobjectArray jPropertyList = getList(jv,env,request->propertyList);

            jobjectArray jAr = (jobjectArray)env->CallObjectMethod((jobject)pr.jProvider,
                                                                   id,
                                                                   jcop,
                                                                   request->localOnly,
                                                                   request->includeQualifiers,
                                                                   request->includeClassOrigin,
                                                                   jPropertyList,
                                                                   jcc);

            JMPIjvm::checkException(env);

            STAT_PMS_PROVIDEREND;

            handler.processing();
            if (jAr) {
                for (int i=0,m=env->GetArrayLength(jAr); i<m; i++) {
                    JMPIjvm::checkException(env);

                    jobject jciRet = env->GetObjectArrayElement(jAr,i);

                    JMPIjvm::checkException(env);

                    jint         jciRetRef = env->CallIntMethod(jciRet,JMPIjvm::jv.CIMInstanceCInst);
                    CIMInstance *ciRet     = DEBUG_ConvertJavaToC (jint, CIMInstance*, jciRetRef);

                    JMPIjvm::checkException(env);

                    handler.deliver(*ciRet);
                }
            }
            handler.complete();
            break;
        }

        case METHOD_SNIA_PROVIDER20:
        {
            jint    jcopRef = DEBUG_ConvertCToJava (CIMObjectPath*, jint, objectPath);
            jobject jcop    = env->NewObject(jv->CIMObjectPathClassRef,jv->CIMObjectPathNewI,jcopRef);

            JMPIjvm::checkException(env);

            CIMClass cls = pr._cimom_handle->getClass(context,
                                                      request->nameSpace,
                                                      request->className,
                                                      false,
                                                      true,
                                                      true,
                                                      CIMPropertyList());
            CIMClass *pcls = new CIMClass (cls);

            JMPIjvm::checkException(env);

            jint    jccRef = DEBUG_ConvertCToJava (CIMClass*, jint, pcls);
            jobject jcc    = env->NewObject(jv->CIMClassClassRef,jv->CIMClassNewI,jccRef);

            JMPIjvm::checkException(env);

            // Modified for Bugzilla# 3679
            jobject jVec = env->CallObjectMethod((jobject)pr.jProvider,
                                                 id,
                                                 jcop,
                                                 JMPI_DEEPINHERITANCE,
                                                 jcc,
                                                 JMPI_LOCALONLY);

            JMPIjvm::checkException(env);

            STAT_PMS_PROVIDEREND;

            handler.processing();
            if (jVec) {
                for (int i=0,m=env->CallIntMethod(jVec,JMPIjvm::jv.VectorSize); i<m; i++) {
                    JMPIjvm::checkException(env);

                    jobject jciRet = env->CallObjectMethod(jVec,JMPIjvm::jv.VectorElementAt,i);

                    JMPIjvm::checkException(env);

                    jint         jciRetRef = env->CallIntMethod(jciRet,JMPIjvm::jv.CIMInstanceCInst);
                    CIMInstance *ciRet     = DEBUG_ConvertJavaToC (jint, CIMInstance*, jciRetRef);

                    handler.deliver(*ciRet);
                }
            }
            handler.complete();
            break;
        }

        case METHOD_UNKNOWN:
        {
            DDD (PEGASUS_STD(cout)<<"--- JMPIProviderManager::handleEnumerateInstancesRequest: should not be here!"<<PEGASUS_STD(endl));
            break;
        }
        }
    }
    HandlerCatch(handler);

    if (env) JMPIjvm::detachThread();

    PEG_METHOD_EXIT();

    STAT_COPYDISPATCHER

    return(response);
}

Message * JMPIProviderManager::handleEnumerateInstanceNamesRequest(const Message * message) throw()
{
    PEG_METHOD_ENTER(TRC_PROVIDERMANAGER, "JMPIProviderManager::handleEnumerateInstanceNamesRequest");

    HandlerIntro(EnumerateInstanceNames,message,request,response, handler,Array<CIMObjectPath>());

    typedef enum {
       METHOD_UNKNOWN = 0,
       METHOD_PEGASUS_24,
       METHOD_SNIA_PROVIDER20,
    } METHOD_VERSION;
    METHOD_VERSION   eMethodFound  = METHOD_UNKNOWN;
    JNIEnv          *env           = NULL;

    try {
        Logger::put(Logger::STANDARD_LOG, System::CIMSERVER, Logger::TRACE,
            "JMPIProviderManager::handleEnumerateInstanceNamesRequest - Host name: $0  Name space: $1  Class name: $2",
            System::getHostName(),
            request->nameSpace.getString(),
            request->className.getString());

        DDD(PEGASUS_STD(cout)<<"--- JMPIProviderManager::handleEnumerateInstanceNamesRequest: hostname = "<<System::getHostName()<<", namespace = "<<request->nameSpace.getString()<<", classname = "<<request->className.getString()<<PEGASUS_STD(endl));

        // make target object path
        CIMObjectPath *objectPath = new CIMObjectPath (System::getHostName(),
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

        JMPIProvider &pr = ph.GetProvider();

        PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4, "Calling provider.enumerateInstanceNames: " + pr.getName());

        DDD(PEGASUS_STD(cout)<<"--- JMPIProviderManager::handleEnumerateInstanceNamesRequest: Calling provider : enumerateInstanceNames"<<pr.getName()<<PEGASUS_STD(endl));

        JvmVector *jv = 0;

        env = JMPIjvm::attachThread(&jv);

        JMPIProvider::pm_service_op_lock op_lock(&pr);

        STAT_GETSTARTTIME;

        jmethodID id = NULL;

        // public abstract java.util.Vector enumInstances (org.pegasus.jmpi.CIMObjectPath cop,
        //                                                 boolean                        deep,
        //                                                 org.pegasus.jmpi.CIMClass      cimClass)
        //        throws org.pegasus.jmpi.CIMException
        id = env->GetMethodID((jclass)pr.jProviderClass,
                              "enumInstances",
                              "(Lorg/pegasus/jmpi/CIMObjectPath;ZLorg/pegasus/jmpi/CIMClass;)Ljava/util/Vector;");

        if (id != NULL)
        {
            eMethodFound = METHOD_SNIA_PROVIDER20;
            DDD (PEGASUS_STD(cout)<<"--- JMPIProviderManager::handleEnumerateInstanceNamesRequest: found METHOD_SNIA_PROVIDER20."<<PEGASUS_STD(endl));
        }

        if (id == NULL)
        {
            env->ExceptionClear();

            // public org.pegasus.jmpi.CIMObjectPath[] enumerateInstanceNames (org.pegasus.jmpi.CIMObjectPath op,
            //                                                                 org.pegasus.jmpi.CIMClass      cc)
            //         throws org.pegasus.jmpi.CIMException
            id = env->GetMethodID((jclass)pr.jProviderClass,
                                  "enumerateInstanceNames",
                                  "(Lorg/pegasus/jmpi/CIMObjectPath;Lorg/pegasus/jmpi/CIMClass;)[Lorg/pegasus/jmpi/CIMObjectPath;");

            if (id != NULL)
            {
                eMethodFound = METHOD_PEGASUS_24;
                DDD (PEGASUS_STD(cout)<<"--- JMPIProviderManager::handleEnumerateInstanceNamesRequest: found METHOD_PEGASUS_24."<<PEGASUS_STD(endl));
            }
        }

        JMPIjvm::checkException(env);

        switch (eMethodFound)
        {
        case METHOD_PEGASUS_24:
        {
            jint    jcopRef = DEBUG_ConvertCToJava (CIMObjectPath*, jint, objectPath);
            jobject jcop    = env->NewObject(jv->CIMObjectPathClassRef,jv->CIMObjectPathNewI,jcopRef);

            JMPIjvm::checkException(env);

            CIMClass cls = pr._cimom_handle->getClass(context,
                                                      request->nameSpace,
                                                      request->className,
                                                      false,
                                                      true,
                                                      true,
                                                      CIMPropertyList());
            CIMClass *pcls = new CIMClass (cls);

            JMPIjvm::checkException(env);

            jint    jcimClassRef = DEBUG_ConvertCToJava (CIMClass*, jint, pcls);
            jobject jcimClass    = env->NewObject(jv->CIMClassClassRef,jv->CIMClassNewI,jcimClassRef);

            JMPIjvm::checkException(env);

            jobjectArray jAr = (jobjectArray)env->CallObjectMethod((jobject)pr.jProvider,
                                                                   id,
                                                                   jcop,
                                                                   jcimClass);

            JMPIjvm::checkException(env);

            STAT_PMS_PROVIDEREND;

            handler.processing();
            if (jAr) {
                for (int i=0,m=env->GetArrayLength(jAr); i<m; i++) {
                    JMPIjvm::checkException(env);

                    jobject jcopRet = env->GetObjectArrayElement(jAr,i);

                    JMPIjvm::checkException(env);

                    jint           jcopRetRef = env->CallIntMethod(jcopRet,JMPIjvm::jv.CIMObjectPathCInst);
                    CIMObjectPath *copRet     = DEBUG_ConvertJavaToC (jint, CIMObjectPath*, jcopRetRef);

                    JMPIjvm::checkException(env);

                    handler.deliver(*copRet);
                }
            }
            handler.complete();
            break;
        }

        case METHOD_SNIA_PROVIDER20:
        {
            jint    jcopRef = DEBUG_ConvertCToJava (CIMObjectPath*, jint, objectPath);
            jobject jcop    = env->NewObject(jv->CIMObjectPathClassRef,jv->CIMObjectPathNewI,jcopRef);

            JMPIjvm::checkException(env);

            CIMClass cls = pr._cimom_handle->getClass(context,
                                                      request->nameSpace,
                                                      request->className,
                                                      false,
                                                      true,
                                                      true,
                                                      CIMPropertyList());
            CIMClass *pcls = new CIMClass (cls);

            JMPIjvm::checkException(env);

            jint    jcimClassRef = DEBUG_ConvertCToJava (CIMClass*, jint, pcls);
            jobject jcimClass    = env->NewObject(jv->CIMClassClassRef,jv->CIMClassNewI,jcimClassRef);

            JMPIjvm::checkException(env);

            jobject jVec = env->CallObjectMethod((jobject)pr.jProvider,
                                                 id,
                                                 jcop,
                                                 true,
                                                 jcimClass);

            JMPIjvm::checkException(env);

            STAT_PMS_PROVIDEREND;

            handler.processing();
            if (jVec) {
                for (int i=0,m=env->CallIntMethod(jVec,JMPIjvm::jv.VectorSize); i<m; i++) {
                    JMPIjvm::checkException(env);

                    jobject jcopRet = env->CallObjectMethod(jVec,JMPIjvm::jv.VectorElementAt,i);

                    JMPIjvm::checkException(env);

                    jint           jcopRetRef = env->CallIntMethod(jcopRet,JMPIjvm::jv.CIMObjectPathCInst);
                    CIMObjectPath *copRet     = DEBUG_ConvertJavaToC (jint, CIMObjectPath*, jcopRetRef);

                    JMPIjvm::checkException(env);

                    handler.deliver(*copRet);
                }
            }
            handler.complete();
            break;
        }

        case METHOD_UNKNOWN:
        {
            DDD (PEGASUS_STD(cout)<<"--- JMPIProviderManager::handleEnumerateInstanceNamesRequest: should not be here!"<<PEGASUS_STD(endl));
            break;
        }
        }
    }
    HandlerCatch(handler);

    if (env) JMPIjvm::detachThread();

    PEG_METHOD_EXIT();

    STAT_COPYDISPATCHER

    return(response);
}

Message * JMPIProviderManager::handleCreateInstanceRequest(const Message * message) throw()
{
    PEG_METHOD_ENTER(TRC_PROVIDERMANAGER,"JMPIProviderManager::handleCreateInstanceRequest");

    HandlerIntro(CreateInstance,message,request,response,handler,CIMObjectPath());

    typedef enum {
       METHOD_UNKNOWN = 0,
       METHOD_SNIA_PROVIDER20,
    } METHOD_VERSION;
    METHOD_VERSION   eMethodFound  = METHOD_UNKNOWN;
    JNIEnv          *env           = NULL;

    try {
        Logger::put(Logger::STANDARD_LOG, System::CIMSERVER, Logger::TRACE,
            "JMPIProviderManager::handleCreateInstanceRequest - Host name: $0  Name space: $1  Class name: $2",
            System::getHostName(),
            request->nameSpace.getString(),
            request->newInstance.getPath().getClassName().getString());

        DDD(PEGASUS_STD(cout)<<"--- JMPIProviderManager::handleCreateInstanceRequest: hostname = "<<System::getHostName()<<", namespace = "<<request->nameSpace.getString()<<", classname = "<<request->newInstance.getPath().getClassName().getString()<<PEGASUS_STD(endl));

        // make target object path
        CIMObjectPath *objectPath = new CIMObjectPath (System::getHostName(),
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
        JMPIProvider &pr = ph.GetProvider();

        PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4, "Calling provider.createInstance: " + ph.GetProvider().getName());

        DDD(PEGASUS_STD(cout)<<"--- JMPIProviderManager::handleCreateInstanceRequest: Calling provider createInstance: "<<pr.getName()<<PEGASUS_STD(endl));

        JvmVector *jv = 0;

        env = JMPIjvm::attachThread(&jv);

        JMPIProvider::pm_service_op_lock op_lock(&pr);

        STAT_GETSTARTTIME;

        jmethodID id = NULL;

        // public abstract org.pegasus.jmpi.CIMObjectPath createInstance (org.pegasus.jmpi.CIMObjectPath cop,
        //                                                                org.pegasus.jmpi.CIMInstance   cimInstance)
        //        throws org.pegasus.jmpi.CIMException
        id = env->GetMethodID((jclass)pr.jProviderClass,
                              "createInstance",
                              "(Lorg/pegasus/jmpi/CIMObjectPath;Lorg/pegasus/jmpi/CIMInstance;)Lorg/pegasus/jmpi/CIMObjectPath;");

        if (id != NULL)
        {
            eMethodFound = METHOD_SNIA_PROVIDER20;
            DDD (PEGASUS_STD(cout)<<"--- JMPIProviderManager::handleCreateInstanceRequest: found METHOD_SNIA_PROVIDER20."<<PEGASUS_STD(endl));
        }

        JMPIjvm::checkException(env);

        switch (eMethodFound)
        {
        case METHOD_SNIA_PROVIDER20:
        {
            jint    jcopRef = DEBUG_ConvertCToJava (CIMObjectPath*, jint, objectPath);
            jobject jcop    = env->NewObject(jv->CIMObjectPathClassRef,jv->CIMObjectPathNewI,jcopRef);

            JMPIjvm::checkException(env);

            CIMInstance *ci     = new CIMInstance (request->newInstance);
            jint         jciRef = DEBUG_ConvertCToJava (CIMInstance*, jint, ci);
            jobject      jci    = env->NewObject(jv->CIMInstanceClassRef,jv->CIMInstanceNewI,jciRef);

            JMPIjvm::checkException(env);

            DDD(PEGASUS_STD(cout)<<"--- JMPIProviderManager::handleCreateInstanceRequest: id = "<<id<<", jcop = "<<jcop<<", jci = "<<jci<<PEGASUS_STD(endl));

            jobject jcopRet = env->CallObjectMethod((jobject)pr.jProvider,
                                                    id,
                                                    jcop,
                                                    jci);

            JMPIjvm::checkException(env);

            STAT_PMS_PROVIDEREND;

            handler.processing();

            if (jcopRet) {
                jint           jCopRetRef = env->CallIntMethod(jcopRet,JMPIjvm::jv.CIMObjectPathCInst);
                CIMObjectPath *copRet     = DEBUG_ConvertJavaToC (jint, CIMObjectPath*, jCopRetRef);

                handler.deliver(*copRet);
            }
            handler.complete();
            break;
        }

        case METHOD_UNKNOWN:
        {
            DDD (PEGASUS_STD(cout)<<"--- JMPIProviderManager::handleCreateInstanceRequest: should not be here!"<<PEGASUS_STD(endl));
            break;
        }
        }
    }
    HandlerCatch(handler);

    if (env) JMPIjvm::detachThread();

    PEG_METHOD_EXIT();

    STAT_COPYDISPATCHER

    return(response);
}

Message * JMPIProviderManager::handleModifyInstanceRequest(const Message * message) throw()
{
    PEG_METHOD_ENTER(TRC_PROVIDERMANAGER,"JMPIProviderManager::handleModifyInstanceRequest");

    HandlerIntroVoid(ModifyInstance,message,request,response,handler);

    typedef enum {
       METHOD_UNKNOWN = 0,
       METHOD_PEGASUS_24,
       METHOD_SNIA_PROVIDER20,
    } METHOD_VERSION;
    METHOD_VERSION   eMethodFound  = METHOD_UNKNOWN;
    JNIEnv          *env           = NULL;

    try {
        Logger::put(Logger::STANDARD_LOG, System::CIMSERVER, Logger::TRACE,
            "JMPIProviderManager::handleModifyInstanceRequest - Host name: $0  Name space: $1  Class name: $2",
            System::getHostName(),
            request->nameSpace.getString(),
            request->modifiedInstance.getPath().getClassName().getString());

        DDD(PEGASUS_STD(cout)<<"--- JMPIProviderManager::handleModifyInstanceRequest: hostname = "<<System::getHostName()<<", namespace = "<<request->nameSpace.getString()<<", classname = "<<request->modifiedInstance.getPath().getClassName().getString()<<PEGASUS_STD(endl));

        // make target object path
        CIMObjectPath *objectPath = new CIMObjectPath (System::getHostName(),
                                                       request->nameSpace,
                                                       request->modifiedInstance.getPath ().getClassName(),
                                                       request->modifiedInstance.getPath ().getKeyBindings());

        // resolve provider name
        ProviderName name = _resolveProviderName(
            request->operationContext.get(ProviderIdContainer::NAME));

        DDD(PEGASUS_STD(cout)<<"--- JMPIProviderManager::handleModifyInstanceRequest: provider name physical = "<<name.getPhysicalName()<<", logical = "<<name.getLogicalName()<<PEGASUS_STD(endl));

        // get cached or load new provider module
        JMPIProvider::OpProviderHolder ph =
            providerManager.getProvider(name.getPhysicalName(), name.getLogicalName(), String::EMPTY);

        // convert arguments
        OperationContext context;

        context.insert(request->operationContext.get(IdentityContainer::NAME));
        context.insert(request->operationContext.get(AcceptLanguageListContainer::NAME));
        context.insert(request->operationContext.get(ContentLanguageListContainer::NAME));

        // forward request
        JMPIProvider &pr = ph.GetProvider();

        PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,"Calling provider.modifyInstance: " + pr.getName());

        DDD(PEGASUS_STD(cout)<<"--- JMPIProviderManager::handleModifyInstanceRequest: Calling provider "<<PEGASUS_STD(hex)<<(int)&pr<<PEGASUS_STD(dec)<<", name = "<<pr.getName ()<<", module = "<<pr.getModule()<<" modifyInstance: "<<pr.getName()<<PEGASUS_STD(endl));
////////DDD(debugPrintMethodPointers (env, (jclass)pr.jProviderClass));

        JvmVector *jv = 0;

        env = JMPIjvm::attachThread(&jv);

        JMPIProvider::pm_service_op_lock op_lock(&pr);

        STAT_GETSTARTTIME;

        jmethodID id = NULL;

        // public abstract void setInstance (org.pegasus.jmpi.CIMObjectPath cop,
        //                                   org.pegasus.jmpi.CIMInstance   cimInstance)
        //        org.pegasus.jmpi.throws CIMException
        id = env->GetMethodID((jclass)pr.jProviderClass,
                              "setInstance",
                              "(Lorg/pegasus/jmpi/CIMObjectPath;Lorg/pegasus/jmpi/CIMInstance;)V");

        if (id != NULL)
        {
            eMethodFound = METHOD_SNIA_PROVIDER20;
            DDD (PEGASUS_STD(cout)<<"--- JMPIProviderManager::handleModifyInstanceRequest: found METHOD_SNIA_PROVIDER20."<<PEGASUS_STD(endl));
        }

        if (id == NULL)
        {
            env->ExceptionClear();

            // public void setInstance (org.pegasus.jmpi.CIMObjectPath op,
            //                          org.pegasus.jmpi.CIMInstance   ci,
            //                          boolean                        includeQualifiers,
            //                          java.lang.String[]             propertyList)
            //        throws org.pegasus.jmpi.CIMException
            id = env->GetMethodID((jclass)pr.jProviderClass,
                                  "setInstance",
                                  "(Lorg/pegasus/jmpi/CIMObjectPath;Z[Ljava/lang/String)V");

            if (id != NULL)
            {
                eMethodFound = METHOD_PEGASUS_24;
                DDD (PEGASUS_STD(cout)<<"--- JMPIProviderManager::handleModifyInstanceRequest: found METHOD_PEGASUS_24."<<PEGASUS_STD(endl));
            }
        }

        JMPIjvm::checkException(env);

        switch (eMethodFound)
        {
        case METHOD_PEGASUS_24:
        {
            jint    jcopRef = DEBUG_ConvertCToJava (CIMObjectPath*, jint, objectPath);
            jobject jcop    = env->NewObject(jv->CIMObjectPathClassRef,jv->CIMObjectPathNewI,jcopRef);

            JMPIjvm::checkException(env);

            CIMInstance *ci     = new CIMInstance (request->modifiedInstance);
            jint         jciRef = DEBUG_ConvertCToJava (CIMInstance*, jint, ci);
            jobject      jci    = env->NewObject(jv->CIMInstanceClassRef,jv->CIMInstanceNewI,jciRef);

            JMPIjvm::checkException(env);

            jobjectArray jPropertyList = getList(jv,env,request->propertyList);

            env->CallVoidMethod((jobject)pr.jProvider,
                                id,
                                jcop,
                                jci,
                                jPropertyList);

            JMPIjvm::checkException(env);

            STAT_PMS_PROVIDEREND;
            break;
        }

        case METHOD_SNIA_PROVIDER20:
        {
            jint    jcopRef = DEBUG_ConvertCToJava (CIMObjectPath*, jint, objectPath);
            jobject jcop    = env->NewObject(jv->CIMObjectPathClassRef,jv->CIMObjectPathNewI,jcopRef);

            JMPIjvm::checkException(env);

            CIMInstance *ci     = new CIMInstance (request->modifiedInstance);
            jint         jciRef = DEBUG_ConvertCToJava (CIMInstance*, jint, ci);
            jobject      jci    = env->NewObject(jv->CIMInstanceClassRef,jv->CIMInstanceNewI,jciRef);

            JMPIjvm::checkException(env);

            env->CallVoidMethod((jobject)pr.jProvider,
                                id,
                                jcop,
                                jci);

            JMPIjvm::checkException(env);

            STAT_PMS_PROVIDEREND;
            break;
        }

        case METHOD_UNKNOWN:
        {
            DDD (PEGASUS_STD(cout)<<"--- JMPIProviderManager::handleModifyInstanceRequest: should not be here!"<<PEGASUS_STD(endl));
            break;
        }
        }
    }
    HandlerCatch(handler);

    if (env) JMPIjvm::detachThread();

    PEG_METHOD_EXIT();

    STAT_COPYDISPATCHER

    return(response);
}

Message * JMPIProviderManager::handleDeleteInstanceRequest(const Message * message) throw()
{
    PEG_METHOD_ENTER(TRC_PROVIDERMANAGER,"JMPIProviderManager::handleDeleteInstanceRequest");

    HandlerIntroVoid(DeleteInstance,message,request,response,handler);

    typedef enum {
       METHOD_UNKNOWN = 0,
       METHOD_SNIA_PROVIDER20,
    } METHOD_VERSION;
    METHOD_VERSION   eMethodFound  = METHOD_UNKNOWN;
    JNIEnv          *env           = NULL;

    try {
        Logger::put(Logger::STANDARD_LOG, System::CIMSERVER, Logger::TRACE,
            "JMPIProviderManager::handleDeleteInstanceRequest - Host name: $0  Name space: $1  Class name: $2",
            System::getHostName(),
            request->nameSpace.getString(),
            request->instanceName.getClassName().getString());

        DDD(PEGASUS_STD(cout)<<"--- JMPIProviderManager::handleDeleteInstanceRequest: hostname = "<<System::getHostName()<<", namespace = "<<request->nameSpace.getString()<<", classname = "<<request->instanceName.getClassName().getString()<<PEGASUS_STD(endl));

        // make target object path
        CIMObjectPath *objectPath = new CIMObjectPath (System::getHostName(),
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
        JMPIProvider &pr = ph.GetProvider();

        PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4, "Calling provider.deleteInstance: " + pr.getName());

        DDD(PEGASUS_STD(cout)<<"--- JMPIProviderManager::handleDeleteInstanceRequest: Calling provider deleteInstance: "<<pr.getName()<<PEGASUS_STD(endl));

        JvmVector *jv = 0;

        env = JMPIjvm::attachThread(&jv);

        JMPIProvider::pm_service_op_lock op_lock(&pr);

        STAT_GETSTARTTIME;

        jmethodID id = NULL;

        // public abstract void deleteInstance (org.pegasus.jmpi.CIMObjectPath cop)
        //        throws org.pegasus.jmpi.CIMException
        id = env->GetMethodID((jclass)pr.jProviderClass,
                              "deleteInstance",
                              "(Lorg/pegasus/jmpi/CIMObjectPath;)V");

        if (id != NULL)
        {
            eMethodFound = METHOD_SNIA_PROVIDER20;
            DDD (PEGASUS_STD(cout)<<"--- JMPIProviderManager::handleDeleteInstanceRequest: found METHOD_SNIA_PROVIDER20."<<PEGASUS_STD(endl));
        }

        JMPIjvm::checkException(env);

        switch (eMethodFound)
        {
        case METHOD_SNIA_PROVIDER20:
        {
            jint    jcopRef = DEBUG_ConvertCToJava (CIMObjectPath*, jint, objectPath);
            jobject jcop    = env->NewObject(jv->CIMObjectPathClassRef,jv->CIMObjectPathNewI,jcopRef);

            JMPIjvm::checkException(env);

            env->CallVoidMethod((jobject)pr.jProvider,id,jcop);

            JMPIjvm::checkException(env);

            STAT_PMS_PROVIDEREND;
            break;
        }

        case METHOD_UNKNOWN:
        {
            DDD (PEGASUS_STD(cout)<<"--- JMPIProviderManager::handleDeleteInstanceRequest: should not be here!"<<PEGASUS_STD(endl));
            break;
        }
        }
    }
    HandlerCatch(handler);

    if (env) JMPIjvm::detachThread();

    PEG_METHOD_EXIT();

    STAT_COPYDISPATCHER

    return(response);
}

Message * JMPIProviderManager::handleExecQueryRequest(const Message * message) throw()
{
    PEG_METHOD_ENTER(TRC_PROVIDERMANAGER,"JMPIProviderManager::handleExecQueryRequest");

    HandlerIntro(ExecQuery,message,request,response,handler,Array<CIMObject>());

    typedef enum {
       METHOD_UNKNOWN = 0,
       METHOD_PEGASUS_24,
       METHOD_SNIA_PROVIDER20,
    } METHOD_VERSION;
    METHOD_VERSION   eMethodFound  = METHOD_UNKNOWN;
    JNIEnv          *env           = NULL;

    try {
        Logger::put(Logger::STANDARD_LOG, System::CIMSERVER, Logger::TRACE,
            "JMPIProviderManager::handleExecQueryRequest - Host name: $0  Name space: $1  Class name: $2",
            System::getHostName(),
            request->nameSpace.getString(),
            request->className.getString());

        DDD(PEGASUS_STD(cout)<<"--- JMPIProviderManager::handleExecQueryRequest: hostname = "<<System::getHostName()<<", namespace = "<<request->nameSpace.getString()<<", classname = "<<request->className.getString()<<PEGASUS_STD(endl));

        // make target object path
        CIMObjectPath *objectPath = new CIMObjectPath (System::getHostName(),
                                                       request->nameSpace,
                                                       request->className);

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
        JMPIProvider &pr = ph.GetProvider();

        PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,"Calling provider.execQuery: " + pr.getName());

        DDD(PEGASUS_STD(cout)<<"--- JMPIProviderManager::handleExecQueryRequest: Calling provider execQuery: "<<pr.getName()<<", queryLanguage: "<<request->queryLanguage<<", query: "<<request->query<<PEGASUS_STD(endl));

        JvmVector *jv = 0;

        env = JMPIjvm::attachThread(&jv);

        JMPIProvider::pm_service_op_lock op_lock(&pr);

        STAT_GETSTARTTIME;

        jmethodID id = NULL;

        // public abstract java.util.Vector execQuery (org.pegasus.jmpi.CIMObjectPath cop,
        //                                             java.lang.String               queryStatement,
        //                                             int                            ql,
        //                                             org.pegasus.jmpi.CIMClass      cimClass)
        //        throws org.pegasus.jmpi.CIMException
        //
        id = env->GetMethodID((jclass)pr.jProviderClass,
                              "execQuery",
                              "(Lorg/pegasus/jmpi/CIMObjectPath;Ljava/lang/String;ILorg/pegasus/jmpi/CIMClass;)Ljava/util/Vector;");

        if (id != NULL)
        {
            eMethodFound = METHOD_SNIA_PROVIDER20;
            DDD (PEGASUS_STD(cout)<<"--- JMPIProviderManager::handleExecQueryRequest: found METHOD_SNIA_PROVIDER20."<<PEGASUS_STD(endl));
        }

        if (id == NULL)
        {
            env->ExceptionClear();

            // public abstract org.pegasus.jmpi.CIMInstance[] execQuery(org.pegasus.jmpi.CIMObjectPath op,
            //                                                          java.lang.String               query,
            //                                                          java.lang.String               ql,
            //                                                          org.pegasus.jmpi.CIMClass      cc)
            //        throws org.pegasus.jmpi.CIMException
            id = env->GetMethodID((jclass)pr.jProviderClass,
                                  "execQuery",
                                  "(Lorg/pegasus/jmpi/CIMObjectPath;Ljava/lang/String;Ljava/lang/String;Lorg/pegasus/jmpi/CIMClass;)[Lorg/pegasus/jmpi/CIMInstance;");

            if (id != NULL)
            {
                eMethodFound = METHOD_PEGASUS_24;
                DDD (PEGASUS_STD(cout)<<"--- JMPIProviderManager::handleExecQueryRequest: found METHOD_PEGASUS_24."<<PEGASUS_STD(endl));
            }
        }

        if (id == NULL)
        {
            DDD (PEGASUS_STD(cout)<<"--- JMPIProviderManager::handleExecQueryRequest: found no method!"<<PEGASUS_STD(endl));
        }

        JMPIjvm::checkException(env);

        switch (eMethodFound)
        {
        case METHOD_PEGASUS_24:
        {
            jint    jcopref = DEBUG_ConvertCToJava (CIMObjectPath*, jint, objectPath);
            jobject jcop    = env->NewObject(jv->CIMObjectPathClassRef, jv->CIMObjectPathNewI, jcopref);

            JMPIjvm::checkException(env);

            jstring jqueryLanguage = env->NewStringUTF(request->queryLanguage.getCString());
            jstring jquery         = env->NewStringUTF(request->query.getCString());

            CIMClass cls = pr._cimom_handle->getClass(context,
                                                      request->nameSpace,
                                                      request->className,
                                                      false,
                                                      true,
                                                      true,
                                                      CIMPropertyList());
            CIMClass *pcls = new CIMClass (cls);

            JMPIjvm::checkException(env);

            jint jcls = DEBUG_ConvertCToJava (CIMClass*, jint, pcls);

            jobject jCc=env->NewObject(jv->CIMClassClassRef,jv->CIMClassNewI,jcls);

            JMPIjvm::checkException(env);

            jobjectArray jAr = (jobjectArray)env->CallObjectMethod((jobject)pr.jProvider,
                                                                   id,
                                                                   jcop,
                                                                   jquery,
                                                                   jqueryLanguage,
                                                                   jCc);

            JMPIjvm::checkException(env);

            STAT_PMS_PROVIDEREND;

            handler.processing();
            if (jAr) {
                for (int i=0,m=env->GetArrayLength(jAr); i<m; i++) {
                    JMPIjvm::checkException(env);

                    jobject jciRet = env->GetObjectArrayElement(jAr,i);

                    JMPIjvm::checkException(env);

                    jint         jciRetRef = env->CallIntMethod(jciRet,JMPIjvm::jv.CIMInstanceCInst);
                    CIMInstance *ciRet     = DEBUG_ConvertJavaToC (jint, CIMInstance*, jciRetRef);

                    JMPIjvm::checkException(env);

                    handler.deliver(*ciRet);
                }
            }
            handler.complete();
            break;
        }

        case METHOD_SNIA_PROVIDER20:
        {
            jint    jcopref = DEBUG_ConvertCToJava (CIMObjectPath*, jint, objectPath);
            jobject jcop    = env->NewObject(jv->CIMObjectPathClassRef, jv->CIMObjectPathNewI, jcopref);

            JMPIjvm::checkException(env);

            jstring jqueryLanguage = env->NewStringUTF(request->queryLanguage.getCString());
            jstring jquery         = env->NewStringUTF(request->query.getCString());

            CIMClass cls = pr._cimom_handle->getClass(context,
                                                      request->nameSpace,
                                                      request->className,
                                                      false,
                                                      true,
                                                      true,
                                                      CIMPropertyList());
            CIMClass *pcls = new CIMClass (cls);

            JMPIjvm::checkException(env);

            jint jcls = DEBUG_ConvertCToJava (CIMClass*, jint, pcls);

            jobject jCc=env->NewObject(jv->CIMClassClassRef,jv->CIMClassNewI,jcls);

            JMPIjvm::checkException(env);

            jint jql = 0; // @BUG - how to convert?

            jobjectArray jVec = (jobjectArray)env->CallObjectMethod((jobject)pr.jProvider,
                                                                    id,
                                                                    jcop,
                                                                    jquery,
                                                                    jql,
                                                                    jCc);

            JMPIjvm::checkException(env);

            STAT_PMS_PROVIDEREND;

            handler.processing();
            if (jVec) {
                for (int i=0,m=env->GetArrayLength(jVec); i<m; i++) {
                    JMPIjvm::checkException(env);

                    jobject jciRet = env->GetObjectArrayElement(jVec,i);

                    JMPIjvm::checkException(env);

                    jint         jciRetRef = env->CallIntMethod(jciRet,JMPIjvm::jv.CIMInstanceCInst);
                    CIMInstance *ciRet     = DEBUG_ConvertJavaToC (jint, CIMInstance*, jciRetRef);

                    JMPIjvm::checkException(env);

                    handler.deliver(*ciRet);
                }
            }
            handler.complete();
            break;
        }

        case METHOD_UNKNOWN:
        {
            DDD (PEGASUS_STD(cout)<<"--- JMPIProviderManager::handleExecQueryRequest: should not be here!"<<PEGASUS_STD(endl));
            break;
        }
        }
    }
    HandlerCatch(handler);

    if (env) JMPIjvm::detachThread();

    PEG_METHOD_EXIT();

    STAT_COPYDISPATCHER

    return(response);
}

Message * JMPIProviderManager::handleAssociatorsRequest(const Message * message) throw()
{
    PEG_METHOD_ENTER(TRC_PROVIDERMANAGER,"JMPIProviderManager::handleAssociatorsRequest");

    HandlerIntro(Associators,message,request,response,handler,Array<CIMObject>());

    typedef enum {
       METHOD_UNKNOWN = 0,
       METHOD_PEGASUS_24,
       METHOD_SNIA_PROVIDER20,
    } METHOD_VERSION;
    METHOD_VERSION   eMethodFound  = METHOD_UNKNOWN;
    JNIEnv          *env           = NULL;

    try {
        Logger::put(Logger::STANDARD_LOG, System::CIMSERVER, Logger::TRACE,
            "JMPIProviderManager::handleAssociatorsRequest - Host name: $0  Name space: $1  Class name: $2",
            System::getHostName(),
            request->nameSpace.getString(),
            request->objectName.getClassName().getString());

        DDD(PEGASUS_STD(cout)<<"--- JMPIProviderManager::handleAssociatorsRequest: hostname = "<<System::getHostName()<<", namespace = "<<request->nameSpace.getString()<<", classname = "<<request->objectName.getClassName().getString()<<PEGASUS_STD(endl));

        // make target object path
        CIMObjectPath *objectPath = new CIMObjectPath (System::getHostName(),
                                                       request->nameSpace,
                                                       request->objectName.getClassName(),
                                                       request->objectName.getKeyBindings());
        CIMObjectPath *assocPath  = new CIMObjectPath (System::getHostName(),
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
        JMPIProvider &pr = ph.GetProvider();

        PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,"Calling provider.associators: " + pr.getName());

        DDD(PEGASUS_STD(cout)<<"--- JMPIProviderManager::handleAssociatorsRequest: Calling provider associators: "<<pr.getName()<<", role: "<<request->role<<", aCls: "<<request->assocClass<<PEGASUS_STD(endl));

        JvmVector *jv = 0;

        env = JMPIjvm::attachThread(&jv);

        JMPIProvider::pm_service_op_lock op_lock(&pr);

        STAT_GETSTARTTIME;

        jmethodID id = NULL;

        // public abstract java.util.Vector associators (org.pegasus.jmpi.CIMObjectPath assocName,
        //                                               org.pegasus.jmpi.CIMObjectPath pathName,
        //                                               java.lang.String               resultClass,
        //                                               java.lang.String               role,
        //                                               java.lang.String               resultRole,
        //                                               boolean                        includeQualifiers,
        //                                               boolean                        includeClassOrigin,
        //                                               java.lang.String[]             propertyList)
        //        throws org.pegasus.jmpi.CIMException
        //
        id = env->GetMethodID((jclass)pr.jProviderClass,
                              "associators",
                              "(Lorg/pegasus/jmpi/CIMObjectPath;Lorg/pegasus/jmpi/CIMObjectPath;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;ZZ[Ljava/lang/String;)Ljava/util/Vector;");
//@BUG was:                   "(Lorg/pegasus/jmpi/CIMObjectPath;                                Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;ZZ[Ljava/lang/String;)Ljava/util/Vector;"

        if (id != NULL)
        {
            eMethodFound = METHOD_SNIA_PROVIDER20;
            DDD (PEGASUS_STD(cout)<<"--- JMPIProviderManager::handleAssociatorsRequest: found METHOD_SNIA_PROVIDER20."<<PEGASUS_STD(endl));
        }

        if (id == NULL)
        {
            env->ExceptionClear();

            // public abstract java.util.Vector associators (org.pegasus.jmpi.CIMObjectPath assocName,
            //                                               java.lang.String               resultClass,
            //                                               java.lang.String               role,
            //                                               java.lang.String               resultRole,
            //                                               boolean                        includeQualifiers,
            //                                               boolean                        includeClassOrigin,
            //                                               java.lang.String[]             propertyList)
            //        throws org.pegasus.jmpi.CIMException
            //
            id = env->GetMethodID((jclass)pr.jProviderClass,
                                  "associators",
                                  "(Lorg/pegasus/jmpi/CIMObjectPath;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;ZZ[Ljava/lang/String;)Ljava/util/Vector;");
//@BUG was:                       "(Lorg/pegasus/jmpi/CIMObjectPath;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;ZZ[Ljava/lang/String;)[Lorg/pegasus/jmpi/CIMInstance;"

            if (id != NULL)
            {
                eMethodFound = METHOD_PEGASUS_24;
                DDD (PEGASUS_STD(cout)<<"--- JMPIProviderManager::handleAssociatorsRequest: found METHOD_PEGASUS_24."<<PEGASUS_STD(endl));
            }
        }

        if (id == NULL)
        {
            DDD (PEGASUS_STD(cout)<<"--- JMPIProviderManager::handleAssociatorsRequest: found no method!"<<PEGASUS_STD(endl));
        }

        JMPIjvm::checkException(env);

        switch (eMethodFound)
        {
        case METHOD_PEGASUS_24:
        {
            jint    jAssociationNameRef = DEBUG_ConvertCToJava (CIMObjectPath*, jint, assocPath);
            jobject jAssociationName    = env->NewObject(jv->CIMObjectPathClassRef,jv->CIMObjectPathNewI,jAssociationNameRef);

            JMPIjvm::checkException(env);

            jstring jResultClass = env->NewStringUTF(request->resultClass.getString().getCString());
            jstring jRole        = env->NewStringUTF(request->role.getCString());
            jstring jResultRole  = env->NewStringUTF(request->resultRole.getCString());

            JMPIjvm::checkException(env);

            jobjectArray jPropertyList = getList(jv,env,request->propertyList);

#ifdef PEGASUS_DEBUG
            DDD(PEGASUS_STD(cerr)<<"--- JMPIProviderManager::handleAssociatorsRequest: assocName          = "<<assocPath->toString ()<<PEGASUS_STD(endl));
            DDD(PEGASUS_STD(cerr)<<"--- JMPIProviderManager::handleAssociatorsRequest: resultClass        = "<<request->resultClass<<PEGASUS_STD(endl));
            DDD(PEGASUS_STD(cerr)<<"--- JMPIProviderManager::handleAssociatorsRequest: role               = "<<request->role<<PEGASUS_STD(endl));
            DDD(PEGASUS_STD(cerr)<<"--- JMPIProviderManager::handleAssociatorsRequest: resultRole         = "<<request->resultRole<<PEGASUS_STD(endl));
            DDD(PEGASUS_STD(cerr)<<"--- JMPIProviderManager::handleAssociatorsRequest: includeQualifiers  = "<<false<<PEGASUS_STD(endl));
            DDD(PEGASUS_STD(cerr)<<"--- JMPIProviderManager::handleAssociatorsRequest: includeClassOrigin = "<<false<<PEGASUS_STD(endl));
#endif

            jobjectArray jVec=(jobjectArray)env->CallObjectMethod((jobject)pr.jProvider,
                                                                  id,
                                                                  jAssociationName,
                                                                  jResultClass,
                                                                  jRole,
                                                                  jResultRole,
                                                                  false,
                                                                  false,
                                                                  jPropertyList);

            JMPIjvm::checkException(env);

            STAT_PMS_PROVIDEREND;

            handler.processing();
            if (jVec) {
                for (int i=0,m=env->GetArrayLength(jVec); i<m; i++) {
                    JMPIjvm::checkException(env);

                    jobject jciRet = env->GetObjectArrayElement(jVec,i);

                    JMPIjvm::checkException(env);

                    jint jciRetRef = env->CallIntMethod(jciRet,JMPIjvm::jv.CIMInstanceCInst);

                    JMPIjvm::checkException(env);

                    CIMInstance         *ciRet = DEBUG_ConvertJavaToC (jint, CIMInstance*, jciRetRef);
                    CIMClass             cls   = pr._cimom_handle->getClass(context,
                                                                            request->nameSpace,
                                                                            ciRet->getClassName(),
                                                                            false,
                                                                            true,
                                                                            true,
                                                                            CIMPropertyList());
                    const CIMObjectPath& op    = ciRet->getPath();
                    CIMObjectPath        iop   = ciRet->buildPath(cls);

                    JMPIjvm::checkException(env);

                    iop.setNameSpace(op.getNameSpace());
                    ciRet->setPath(iop);

                    handler.deliver(*ciRet);
                }
            }
            handler.complete();
            break;
        }

        case METHOD_SNIA_PROVIDER20:
        {
            jint    jAssociationNameRef = DEBUG_ConvertCToJava (CIMObjectPath*, jint, assocPath);
            jobject jAssociationName    = env->NewObject(jv->CIMObjectPathClassRef,jv->CIMObjectPathNewI,jAssociationNameRef);

            JMPIjvm::checkException(env);

            jint    jPathNameRef = DEBUG_ConvertCToJava (CIMObjectPath*, jint, objectPath);
            jobject jPathName    = env->NewObject(jv->CIMObjectPathClassRef,jv->CIMObjectPathNewI,jPathNameRef);

            JMPIjvm::checkException(env);

            jstring jResultClass = env->NewStringUTF(request->resultClass.getString().getCString());
            jstring jRole        = env->NewStringUTF(request->role.getCString());
            jstring jResultRole  = env->NewStringUTF(request->resultRole.getCString());

            JMPIjvm::checkException(env);

            jobjectArray jPropertyList = getList(jv,env,request->propertyList);

#ifdef PEGASUS_DEBUG
            DDD(PEGASUS_STD(cerr)<<"--- JMPIProviderManager::handleAssociatorsRequest: assocName          = "<<assocPath->toString ()<<PEGASUS_STD(endl));
            DDD(PEGASUS_STD(cerr)<<"--- JMPIProviderManager::handleAssociatorsRequest: pathName           = "<<objectPath->toString ()<<PEGASUS_STD(endl));
            DDD(PEGASUS_STD(cerr)<<"--- JMPIProviderManager::handleAssociatorsRequest: resultClass        = "<<request->resultClass<<PEGASUS_STD(endl));
            DDD(PEGASUS_STD(cerr)<<"--- JMPIProviderManager::handleAssociatorsRequest: role               = "<<request->role<<PEGASUS_STD(endl));
            DDD(PEGASUS_STD(cerr)<<"--- JMPIProviderManager::handleAssociatorsRequest: resultRole         = "<<request->resultRole<<PEGASUS_STD(endl));
            DDD(PEGASUS_STD(cerr)<<"--- JMPIProviderManager::handleAssociatorsRequest: includeQualifiers  = "<<false<<PEGASUS_STD(endl));
            DDD(PEGASUS_STD(cerr)<<"--- JMPIProviderManager::handleAssociatorsRequest: includeClassOrigin = "<<false<<PEGASUS_STD(endl));
#endif

            jobjectArray jVec=(jobjectArray)env->CallObjectMethod((jobject)pr.jProvider,
                                                                 id,
                                                                 jAssociationName,
                                                                 jPathName,
                                                                 jResultClass,
                                                                 jRole,
                                                                 jResultRole,
                                                                 false,
                                                                 false,
                                                                 jPropertyList);

            JMPIjvm::checkException(env);

            STAT_PMS_PROVIDEREND;

            handler.processing();
            if (jVec) {
                for (int i=0,m=env->CallIntMethod(jVec,JMPIjvm::jv.VectorSize); i<m; i++) {
                    JMPIjvm::checkException(env);

                    jobject jciRet = env->CallObjectMethod(jVec,JMPIjvm::jv.VectorElementAt,i);

                    JMPIjvm::checkException(env);

                    jint                 jciRetRef = env->CallIntMethod(jciRet,JMPIjvm::jv.CIMInstanceCInst);
                    CIMInstance         *ciRet     = DEBUG_ConvertJavaToC (jint, CIMInstance*, jciRetRef);
                    CIMClass             cls       = pr._cimom_handle->getClass(context,
                                                                                request->nameSpace,
                                                                                ciRet->getClassName(),
                                                                                false,
                                                                                true,
                                                                                true,
                                                                                CIMPropertyList());
                    const CIMObjectPath& op        = ciRet->getPath();
                    CIMObjectPath        iop       = ciRet->buildPath(cls);

                    JMPIjvm::checkException(env);

                    iop.setNameSpace(op.getNameSpace());
                    ciRet->setPath(iop);

                    handler.deliver(*ciRet);
                }
            }
            handler.complete();
            break;
        }

        case METHOD_UNKNOWN:
        {
            DDD (PEGASUS_STD(cout)<<"--- JMPIProviderManager::handleAssociatorsRequest: should not be here!"<<PEGASUS_STD(endl));
            break;
        }
        }
    }
    HandlerCatch(handler);

    if (env) JMPIjvm::detachThread();

    PEG_METHOD_EXIT();

    STAT_COPYDISPATCHER

    return(response);
}

Message * JMPIProviderManager::handleAssociatorNamesRequest(const Message * message) throw()
{
    PEG_METHOD_ENTER(TRC_PROVIDERMANAGER,"JMPIProviderManager::handleAssociatorNamesRequest");

    HandlerIntro(AssociatorNames,message,request,response,handler,Array<CIMObjectPath>());

    typedef enum {
       METHOD_UNKNOWN = 0,
       METHOD_PEGASUS_24,
       METHOD_SNIA_PROVIDER20
    } METHOD_VERSION;
    METHOD_VERSION   eMethodFound  = METHOD_UNKNOWN;
    JNIEnv          *env           = NULL;

    try {
        Logger::put(Logger::STANDARD_LOG, System::CIMSERVER, Logger::TRACE,
            "JMPIProviderManager::handleAssociatorNamesRequest - Host name: $0  Name space: $1  Class name: $2",
            System::getHostName(),
            request->nameSpace.getString(),
            request->objectName.getClassName().getString());

        DDD(PEGASUS_STD(cout)<<"--- JMPIProviderManager::handleAssociatorNamesRequest: hostname = "<<System::getHostName()<<", namespace = "<<request->nameSpace.getString()<<", classname = "<<request->objectName.getClassName().getString()<<", assocName = "<<request->assocClass.getString()<<PEGASUS_STD(endl));

        // make target object path
        CIMObjectPath *objectPath = new CIMObjectPath (System::getHostName(),
                                                       request->nameSpace,
                                                       request->objectName.getClassName(),
                                                       request->objectName.getKeyBindings());
        CIMObjectPath *assocPath  = new CIMObjectPath (System::getHostName(),
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
        JMPIProvider &pr = ph.GetProvider();

        PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,"Calling provider.associatorNames: " + pr.getName());

        DDD(PEGASUS_STD(cout)<<"--- JMPIProviderManager::handleAssociatorNamesRequest: Calling provider associatorNames: "<<pr.getName()<<", role: "<<request->role<<", aCls: "<<request->assocClass<<PEGASUS_STD(endl));

        JvmVector *jv = 0;

        env = JMPIjvm::attachThread(&jv);

        JMPIProvider::pm_service_op_lock op_lock(&pr);

        STAT_GETSTARTTIME;

        jmethodID id = NULL;

        // public abstract java.util.Vector associatorNames (org.pegasus.jmpi.CIMObjectPath assocName,
        //                                                   org.pegasus.jmpi.CIMObjectPath pathName,
        //                                                   java.lang.String               resultClass,
        //                                                   java.lang.String               role,
        //                                                   java.lang.String               resultRole)
        //        throws org.pegasus.jmpi.CIMException
        id = env->GetMethodID((jclass)pr.jProviderClass,
                              "associatorNames",
                              "(Lorg/pegasus/jmpi/CIMObjectPath;Lorg/pegasus/jmpi/CIMObjectPath;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)Ljava/util/Vector;");
//@BUG was:                   "(Lorg/pegasus/jmpi/CIMObjectPath;                                Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)Ljava/util/Vector;"

        if (id != NULL)
        {
            eMethodFound = METHOD_SNIA_PROVIDER20;
            DDD (PEGASUS_STD(cout)<<"--- JMPIProviderManager::handleAssociatorNamesRequest: found METHOD_SNIA_PROVIDER20."<<PEGASUS_STD(endl));
        }

        if (id == NULL)
        {
            env->ExceptionClear();

            // public abstract java.util.Vector associatorNames (org.pegasus.jmpi.CIMObjectPath assocName,
            //                                                   java.lang.String               resultClass,
            //                                                   java.lang.String               role,
            //                                                   java.lang.String               resultRole)
            //        throws org.pegasus.jmpi.CIMException
            id = env->GetMethodID((jclass)pr.jProviderClass,
                                  "associatorNames",
                                  "(Lorg/pegasus/jmpi/CIMObjectPath;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)Ljava/util/Vector;");
//@BUG was:                       "(Lorg/pegasus/jmpi/CIMObjectPath;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)[Lorg/pegasus/jmpi/CIMObjectPath;"

            if (id != NULL)
            {
                eMethodFound = METHOD_PEGASUS_24;
                DDD (PEGASUS_STD(cout)<<"--- JMPIProviderManager::handleAssociatorNamesRequest: found METHOD_PEGASUS_24."<<PEGASUS_STD(endl));
            }
        }

        if (id == NULL)
        {
            DDD (PEGASUS_STD(cout)<<"--- JMPIProviderManager::handleAssociatorNamesRequest: found no method!"<<PEGASUS_STD(endl));
        }

        JMPIjvm::checkException(env);

        switch (eMethodFound)
        {
        case METHOD_PEGASUS_24:
        {
            jint    jAssociationNameRef = DEBUG_ConvertCToJava (CIMObjectPath*, jint, assocPath);
            jobject jAssociationName    = env->NewObject(jv->CIMObjectPathClassRef,jv->CIMObjectPathNewI,jAssociationNameRef);

            JMPIjvm::checkException(env);

            jstring jResultClass = env->NewStringUTF(request->resultClass.getString().getCString());
            jstring jRole        = env->NewStringUTF(request->role.getCString());
            jstring jResultRole  = env->NewStringUTF(request->resultRole.getCString());

            JMPIjvm::checkException(env);

#ifdef PEGASUS_DEBUG
            DDD(PEGASUS_STD(cerr)<<"--- JMPIProviderManager::handleAssociatorNamesRequest: assocName   = "<<assocPath->toString ()<<PEGASUS_STD(endl));
            DDD(PEGASUS_STD(cerr)<<"--- JMPIProviderManager::handleAssociatorNamesRequest: resultClass = "<<request->resultClass<<PEGASUS_STD(endl));
            DDD(PEGASUS_STD(cerr)<<"--- JMPIProviderManager::handleAssociatorNamesRequest: role        = "<<request->role<<PEGASUS_STD(endl));
            DDD(PEGASUS_STD(cerr)<<"--- JMPIProviderManager::handleAssociatorNamesRequest: resultRole  = "<<request->resultRole<<PEGASUS_STD(endl));
#endif

            jobjectArray jVec=(jobjectArray)env->CallObjectMethod((jobject)pr.jProvider,
                                                                  id,
                                                                  jAssociationName,
                                                                  jResultClass,
                                                                  jRole,
                                                                  jResultRole);

            JMPIjvm::checkException(env);

            STAT_PMS_PROVIDEREND;

            handler.processing();
            if (jVec) {
                for (int i=0,m=env->GetArrayLength(jVec); i<m; i++) {
                    JMPIjvm::checkException(env);

                    jobject jcopRet = env->GetObjectArrayElement(jVec,i);

                    JMPIjvm::checkException(env);

                    jint           jcopRetRef = env->CallIntMethod(jcopRet,JMPIjvm::jv.CIMObjectPathCInst);
                    CIMObjectPath *copRet     = DEBUG_ConvertJavaToC (jint, CIMObjectPath*, jcopRet);

                    JMPIjvm::checkException(env);

                    handler.deliver(*copRet);
                }
            }
            handler.complete();
            break;
        }

        case METHOD_SNIA_PROVIDER20:
        {
            jint    jAssociationNameRef = DEBUG_ConvertCToJava (CIMObjectPath*, jint, assocPath);
            jobject jAssociationName    = env->NewObject(jv->CIMObjectPathClassRef,jv->CIMObjectPathNewI,jAssociationNameRef);

            JMPIjvm::checkException(env);

            jint    jPathNameRef = DEBUG_ConvertCToJava (CIMObjectPath*, jint, objectPath);
            jobject jPathName    = env->NewObject(jv->CIMObjectPathClassRef,jv->CIMObjectPathNewI,jPathNameRef);

            JMPIjvm::checkException(env);

            jstring jResultClass = env->NewStringUTF(request->resultClass.getString().getCString());
            jstring jRole        = env->NewStringUTF(request->role.getCString());
            jstring jResultRole  = env->NewStringUTF(request->resultRole.getCString());

            JMPIjvm::checkException(env);

#ifdef PEGASUS_DEBUG
            DDD(PEGASUS_STD(cerr)<<"--- JMPIProviderManager::handleAssociatorNamesRequest: assocName   = "<<assocPath->toString ()<<PEGASUS_STD(endl));
            DDD(PEGASUS_STD(cerr)<<"--- JMPIProviderManager::handleAssociatorNamesRequest: pathName    = "<<objectPath->toString ()<<PEGASUS_STD(endl));
            DDD(PEGASUS_STD(cerr)<<"--- JMPIProviderManager::handleAssociatorNamesRequest: resultClass = "<<request->resultClass<<PEGASUS_STD(endl));
            DDD(PEGASUS_STD(cerr)<<"--- JMPIProviderManager::handleAssociatorNamesRequest: role        = "<<request->role<<PEGASUS_STD(endl));
            DDD(PEGASUS_STD(cerr)<<"--- JMPIProviderManager::handleAssociatorNamesRequest: resultRole  = "<<request->resultRole<<PEGASUS_STD(endl));
#endif

            jobjectArray jVec=(jobjectArray)env->CallObjectMethod((jobject)pr.jProvider,
                                                                  id,
                                                                  jAssociationName,
                                                                  jPathName,
                                                                  jResultClass,
                                                                  jRole,
                                                                  jResultRole);

            JMPIjvm::checkException(env);

            STAT_PMS_PROVIDEREND;

            handler.processing();
            if (jVec) {
                for (int i=0,m=env->CallIntMethod(jVec,JMPIjvm::jv.VectorSize); i<m; i++) {
                    JMPIjvm::checkException(env);

                    jobject jcopRet = env->CallObjectMethod(jVec,JMPIjvm::jv.VectorElementAt,i);

                    JMPIjvm::checkException(env);

                    jint           jcopRetRef = env->CallIntMethod(jcopRet,JMPIjvm::jv.CIMObjectPathCInst);
                    CIMObjectPath *copRet     = DEBUG_ConvertJavaToC (jint, CIMObjectPath*, jcopRetRef);

                    JMPIjvm::checkException(env);

                    handler.deliver(*copRet);
                }
            }
            handler.complete();
            break;
        }

        case METHOD_UNKNOWN:
        {
            DDD (PEGASUS_STD(cout)<<"--- JMPIProviderManager::handleAssociatorNamesRequest: should not be here!"<<PEGASUS_STD(endl));
            break;
        }
        }
    }
    HandlerCatch(handler);

    if (env) JMPIjvm::detachThread();

    PEG_METHOD_EXIT();

    STAT_COPYDISPATCHER

    return(response);
}

Message * JMPIProviderManager::handleReferencesRequest(const Message * message) throw()
{
    PEG_METHOD_ENTER(TRC_PROVIDERMANAGER,"JMPIProviderManager::handleReferencesRequest");

    HandlerIntro(References,message,request,response,handler,Array<CIMObject>());

    typedef enum {
       METHOD_UNKNOWN = 0,
       METHOD_PEGASUS_24,
       METHOD_SNIA_PROVIDER20,
    } METHOD_VERSION;
    METHOD_VERSION   eMethodFound  = METHOD_UNKNOWN;
    JNIEnv          *env           = NULL;

    try {
        Logger::put(Logger::STANDARD_LOG, System::CIMSERVER, Logger::TRACE,
            "JMPIProviderManager::handleReferencesRequest - Host name: $0  Name space: $1  Class name: $2",
            System::getHostName(),
            request->nameSpace.getString(),
            request->objectName.getClassName().getString());

        DDD(PEGASUS_STD(cout)<<"--- JMPIProviderManager::handleReferencesRequest: hostname = "<<System::getHostName()<<", namespace = "<<request->nameSpace.getString()<<", classname = "<<request->objectName.getClassName().getString()<<", result = "<<request->resultClass.getString()<<PEGASUS_STD(endl));

        // make target object path
        CIMObjectPath *objectPath = new CIMObjectPath (System::getHostName(),
                                                       request->nameSpace,
                                                       request->objectName.getClassName(),
                                                       request->objectName.getKeyBindings());
        CIMObjectPath *resultPath = new CIMObjectPath (System::getHostName(),
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
        JMPIProvider &pr = ph.GetProvider();

        PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,"Calling provider.references: " + pr.getName());

        DDD(PEGASUS_STD(cout)<<"--- JMPIProviderManager::handleReferencesRequest: Calling provider references: "<<pr.getName()<<", role: "<<request->role<<" aCls: "<<request->resultClass<<PEGASUS_STD(endl));

        JvmVector *jv = 0;

        env = JMPIjvm::attachThread(&jv);

        JMPIProvider::pm_service_op_lock op_lock(&pr);

        STAT_GETSTARTTIME;

        jmethodID id = NULL;

        // public abstract java.util.Vector references (org.pegasus.jmpi.CIMObjectPath assocName,
        //                                              org.pegasus.jmpi.CIMObjectPath pathName,
        //                                              java.lang.String               role,
        //                                              boolean                        includeQualifiers,
        //                                              boolean                        includeClassOrigin,
        //                                              java.lang.String[]             propertyList)
        //        throws org.pegasus.jmpi.CIMException
        id = env->GetMethodID((jclass)pr.jProviderClass,
                              "references",
                              "(Lorg/pegasus/jmpi/CIMObjectPath;Lorg/pegasus/jmpi/CIMObjectPath;Ljava/lang/String;ZZ[Ljava/lang/String;)Ljava/util/Vector;");
//@BUG was:                   "(Lorg/pegasus/jmpi/CIMObjectPath;                                Ljava/lang/String;ZZ[Ljava/lang/String;)Ljava/util/Vector;"

        if (id != NULL)
        {
            eMethodFound = METHOD_SNIA_PROVIDER20;
            DDD (PEGASUS_STD(cout)<<"--- JMPIProviderManager::handleReferencesRequest: found METHOD_SNIA_PROVIDER20."<<PEGASUS_STD(endl));
        }

        if (id == NULL)
        {
            env->ExceptionClear();

            // public abstract java.util.Vector references (org.pegasus.jmpi.CIMObjectPath assocName,
            //                                              java.lang.String               role,
            //                                              boolean                        includeQualifiers,
            //                                              boolean                        includeClassOrigin,
            //                                              java.lang.String[]             propertyList)
            //        throws org.pegasus.jmpi.CIMException
            id = env->GetMethodID((jclass)pr.jProviderClass,
                                  "references",
                                  "(Lorg/pegasus/jmpi/CIMObjectPath;Ljava/lang/String;ZZ[Ljava/lang/String;)Ljava/util/Vector;");
//@BUG was:                       "(Lorg/pegasus/jmpi/CIMObjectPath;Ljava/lang/String;ZZ[Ljava/lang/String;)[Lorg/pegasus/jmpi/CIMInstance;"

            if (id != NULL)
            {
                eMethodFound = METHOD_PEGASUS_24;
                DDD (PEGASUS_STD(cout)<<"--- JMPIProviderManager::handleReferencesRequest: found METHOD_PEGASUS_24."<<PEGASUS_STD(endl));
            }
        }

        if (id == NULL)
        {
            DDD (PEGASUS_STD(cout)<<"--- JMPIProviderManager::handleReferencesRequest: found no method!"<<PEGASUS_STD(endl));
        }

        JMPIjvm::checkException(env);

        switch (eMethodFound)
        {
        case METHOD_PEGASUS_24:
        {
            jint    jAssociationNameRef = DEBUG_ConvertCToJava (CIMObjectPath*, jint, resultPath);
            jobject jAssociationName    = env->NewObject(jv->CIMObjectPathClassRef,jv->CIMObjectPathNewI,jAssociationNameRef);

            JMPIjvm::checkException(env);

            jstring jRole = env->NewStringUTF(request->role.getCString());

            JMPIjvm::checkException(env);

            jobjectArray jPropertyList = getList(jv,env,request->propertyList);

#ifdef PEGASUS_DEBUG
            DDD(PEGASUS_STD(cerr)<<"--- JMPIProviderManager::handleReferencesRequest: assocName          = "<<resultPath->toString ()<<PEGASUS_STD(endl));
            DDD(PEGASUS_STD(cerr)<<"--- JMPIProviderManager::handleReferencesRequest: role               = "<<request->role<<PEGASUS_STD(endl));
            DDD(PEGASUS_STD(cerr)<<"--- JMPIProviderManager::handleReferencesRequest: includeQualifiers  = "<<false<<PEGASUS_STD(endl));
            DDD(PEGASUS_STD(cerr)<<"--- JMPIProviderManager::handleReferencesRequest: includeClassOrigin = "<<false<<PEGASUS_STD(endl));
#endif

            jobjectArray jVec=(jobjectArray)env->CallObjectMethod((jobject)pr.jProvider,
                                                                  id,
                                                                  jAssociationName,
                                                                  jRole,
                                                                  false,
                                                                  false,
                                                                  jPropertyList);

            JMPIjvm::checkException(env);

            STAT_PMS_PROVIDEREND;

            handler.processing();
            if (jVec) {
                for (int i=0,m=env->GetArrayLength(jVec); i<m; i++) {
                    JMPIjvm::checkException(env);

                    jobject jciRet = env->GetObjectArrayElement(jVec,i);

                    JMPIjvm::checkException(env);

                    jint jciRetRef = env->CallIntMethod(jciRet,JMPIjvm::jv.CIMInstanceCInst);

                    JMPIjvm::checkException(env);

                    CIMInstance         *ciRet = DEBUG_ConvertJavaToC (jint, CIMInstance*, jciRetRef);
                    CIMClass             cls   = pr._cimom_handle->getClass(context,
                                                                            request->nameSpace,
                                                                            ciRet->getClassName(),
                                                                            false,
                                                                            true,
                                                                            true,
                                                                            CIMPropertyList());
                    const CIMObjectPath& op    = ciRet->getPath();
                    CIMObjectPath        iop   = ciRet->buildPath(cls);

                    JMPIjvm::checkException(env);

                    iop.setNameSpace(op.getNameSpace());
                    ciRet->setPath(iop);

                    handler.deliver(*ciRet);
                }
            }
            handler.complete();
            break;
        }

        case METHOD_SNIA_PROVIDER20:
        {
            jint    jAssociationNameRef = DEBUG_ConvertCToJava (CIMObjectPath*, jint, resultPath);
            jobject jAssociationName    = env->NewObject(jv->CIMObjectPathClassRef,jv->CIMObjectPathNewI,jAssociationNameRef);

            JMPIjvm::checkException(env);

            jint    jPathNameRef = DEBUG_ConvertCToJava (CIMObjectPath*, jint, objectPath);
            jobject jPathName    = env->NewObject(jv->CIMObjectPathClassRef,jv->CIMObjectPathNewI,jPathNameRef);

            JMPIjvm::checkException(env);

            jstring jRole = env->NewStringUTF(request->role.getCString());

            JMPIjvm::checkException(env);

            jobjectArray jPropertyList = getList(jv,env,request->propertyList);

#ifdef PEGASUS_DEBUG
            DDD(PEGASUS_STD(cerr)<<"--- JMPIProviderManager::handleReferencesRequest: assocName          = "<<resultPath->toString ()<<PEGASUS_STD(endl));
            DDD(PEGASUS_STD(cerr)<<"--- JMPIProviderManager::handleReferencesRequest: pathName           = "<<objectPath->toString ()<<PEGASUS_STD(endl));
            DDD(PEGASUS_STD(cerr)<<"--- JMPIProviderManager::handleReferencesRequest: role               = "<<request->role<<PEGASUS_STD(endl));
            DDD(PEGASUS_STD(cerr)<<"--- JMPIProviderManager::handleReferencesRequest: includeQualifiers  = "<<false<<PEGASUS_STD(endl));
            DDD(PEGASUS_STD(cerr)<<"--- JMPIProviderManager::handleReferencesRequest: includeClassOrigin = "<<false<<PEGASUS_STD(endl));
#endif

            jobjectArray jVec=(jobjectArray)env->CallObjectMethod((jobject)pr.jProvider,
                                                                  id,
                                                                  jAssociationName,
                                                                  jPathName,
                                                                  jRole,
                                                                  false,
                                                                  false,
                                                                  jPropertyList);

            JMPIjvm::checkException(env);

            STAT_PMS_PROVIDEREND;

            handler.processing();
            if (jVec) {
                for (int i=0,m=env->CallIntMethod(jVec,JMPIjvm::jv.VectorSize); i<m; i++) {
                    JMPIjvm::checkException(env);

                    jobject jciRet = env->CallObjectMethod(jVec,JMPIjvm::jv.VectorElementAt,i);

                    JMPIjvm::checkException(env);

                    jint                 jciRetRef = env->CallIntMethod(jciRet,JMPIjvm::jv.CIMInstanceCInst);
                    CIMInstance         *ciRet     = DEBUG_ConvertJavaToC (jint, CIMInstance*, jciRetRef);
                    CIMClass             cls       = pr._cimom_handle->getClass(context,
                                                                                request->nameSpace,
                                                                                ciRet->getClassName(),
                                                                                false,
                                                                                true,
                                                                                true,
                                                                                CIMPropertyList());
                    const CIMObjectPath& op        = ciRet->getPath();
                    CIMObjectPath        iop       = ciRet->buildPath(cls);

                    JMPIjvm::checkException(env);

                    iop.setNameSpace(op.getNameSpace());
                    ciRet->setPath(iop);

                    handler.deliver(*ciRet);
                }
            }
            handler.complete();
            break;
        }

        case METHOD_UNKNOWN:
        {
            DDD (PEGASUS_STD(cout)<<"--- JMPIProviderManager::handleReferencesRequest: should not be here!"<<PEGASUS_STD(endl));
            break;
        }
        }
    }
    HandlerCatch(handler);

    if (env) JMPIjvm::detachThread();

    PEG_METHOD_EXIT();

    STAT_COPYDISPATCHER

    return(response);
}

Message * JMPIProviderManager::handleReferenceNamesRequest(const Message * message) throw()
{
    PEG_METHOD_ENTER(TRC_PROVIDERMANAGER,"JMPIProviderManager::handleReferenceNamesRequest");

    HandlerIntro(ReferenceNames,message,request,response,handler,Array<CIMObjectPath>());

    typedef enum {
       METHOD_UNKNOWN = 0,
       METHOD_PEGASUS_24,
       METHOD_SNIA_PROVIDER20,
    } METHOD_VERSION;
    METHOD_VERSION   eMethodFound  = METHOD_UNKNOWN;
    JNIEnv          *env           = NULL;

    try {
        Logger::put(Logger::STANDARD_LOG, System::CIMSERVER, Logger::TRACE,
            "JMPIProviderManager::handleReferenceNamesRequest - Host name: $0  Name space: $1  Class name: $2",
            System::getHostName(),
            request->nameSpace.getString(),
            request->objectName.getClassName().getString());

        DDD(PEGASUS_STD(cout)<<"--- JMPIProviderManager::handleReferenceNamesRequest: hostname = "<<System::getHostName()<<", namespace = "<<request->nameSpace.getString()<<", classname = "<<request->objectName.getClassName().getString()<<PEGASUS_STD(endl));

        // make target object path
        CIMObjectPath *objectPath = new CIMObjectPath (System::getHostName(),
                                                       request->nameSpace,
                                                       request->objectName.getClassName(),
                                                       request->objectName.getKeyBindings());
        CIMObjectPath *resultPath = new CIMObjectPath (System::getHostName(),
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

        JMPIProvider &pr = ph.GetProvider();

        PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4, "Calling provider.referenceNames: " + pr.getName());

        DDD(PEGASUS_STD(cout)<<"--- JMPIProviderManager::handleReferenceNamesRequest: Calling provider referenceNames: "<<pr.getName()<<", role: "<<request->role<<", aCls: "<<request->resultClass<<PEGASUS_STD(endl));

        JvmVector *jv = 0;

        env = JMPIjvm::attachThread(&jv);

        JMPIProvider::pm_service_op_lock op_lock(&pr);

        STAT_GETSTARTTIME;

        jmethodID id = NULL;

        // public abstract java.util.Vector referenceNames (org.pegasus.jmpi.CIMObjectPath assocName,
        //                                                  org.pegasus.jmpi.CIMObjectPath pathName,
        //                                                  java.lang.String               role)
        //        throws org.pegasus.jmpi.CIMException
        id = env->GetMethodID((jclass)pr.jProviderClass,
                              "referenceNames",
                              "(Lorg/pegasus/jmpi/CIMObjectPath;Lorg/pegasus/jmpi/CIMObjectPath;Ljava/lang/String;)Ljava/util/Vector;");
//@BUG was:                   "(Lorg/pegasus/jmpi/CIMObjectPath;                                Ljava/lang/String;)Ljava/util/Vector;"

        if (id != NULL)
        {
            eMethodFound = METHOD_SNIA_PROVIDER20;
            DDD (PEGASUS_STD(cout)<<"--- JMPIProviderManager::handleReferenceNamesRequest: found METHOD_SNIA_PROVIDER20."<<PEGASUS_STD(endl));
        }

        if (id == NULL)
        {
            env->ExceptionClear();

            // public abstract java.util.Vector referenceNames (org.pegasus.jmpi.CIMObjectPath assocName,
            //                                                  java.lang.String               role)
            //        throws org.pegasus.jmpi.CIMException
            id = env->GetMethodID((jclass)pr.jProviderClass,
                                  "referenceNames",
                                  "(Lorg/pegasus/jmpi/CIMObjectPath;Ljava/lang/String;)Ljava/util/Vector;");
//@BUG was:                       "(Lorg/pegasus/jmpi/CIMObjectPath;Ljava/lang/String;)[Lorg/pegasus/jmpi/CIMObjectPath;"

            if (id != NULL)
            {
                eMethodFound = METHOD_PEGASUS_24;
                DDD (PEGASUS_STD(cout)<<"--- JMPIProviderManager::handleReferenceNamesRequest: found METHOD_PEGASUS_24."<<PEGASUS_STD(endl));
            }
        }

        if (id == NULL)
        {
            DDD (PEGASUS_STD(cout)<<"--- JMPIProviderManager::handleReferenceNamesRequest: found no method!"<<PEGASUS_STD(endl));
        }

        JMPIjvm::checkException(env);

        switch (eMethodFound)
        {
        case METHOD_PEGASUS_24:
        {
            jint    jAssociationNameRef = DEBUG_ConvertCToJava (CIMObjectPath*, jint, resultPath);
            jobject jAssociationName    = env->NewObject(jv->CIMObjectPathClassRef,jv->CIMObjectPathNewI,jAssociationNameRef);

            JMPIjvm::checkException(env);

            jstring jRole = env->NewStringUTF(request->role.getCString());

            JMPIjvm::checkException(env);

#ifdef PEGASUS_DEBUG
            DDD(PEGASUS_STD(cerr)<<"--- JMPIProviderManager::handleReferenceNamesRequest: assocName          = "<<objectPath->toString ()<<PEGASUS_STD(endl));
            DDD(PEGASUS_STD(cerr)<<"--- JMPIProviderManager::handleReferenceNamesRequest: role               = "<<request->role<<PEGASUS_STD(endl));
#endif

            jobjectArray jVec=(jobjectArray)env->CallObjectMethod((jobject)pr.jProvider,
                                                                  id,
                                                                  jAssociationName,
                                                                  jRole);

            JMPIjvm::checkException(env);

            STAT_PMS_PROVIDEREND;

            handler.processing();
            if (jVec) {
                for (int i=0,m=env->GetArrayLength(jVec); i<m; i++) {
                    JMPIjvm::checkException(env);

                    jobject jcopRet = env->GetObjectArrayElement(jVec,i);

                    JMPIjvm::checkException(env);

                    jint jcopRetRef = env->CallIntMethod(jcopRet,JMPIjvm::jv.CIMObjectPathCInst);

                    JMPIjvm::checkException(env);

                    CIMObjectPath *copRet = DEBUG_ConvertJavaToC (jint, CIMObjectPath*, jcopRetRef);

                    handler.deliver(*copRet);
                }
            }
            handler.complete();
            break;
        }

        case METHOD_SNIA_PROVIDER20:
        {
            jint    jAssociationNameRef = DEBUG_ConvertCToJava (CIMObjectPath*, jint, resultPath);
            jobject jAssociationName    = env->NewObject(jv->CIMObjectPathClassRef,jv->CIMObjectPathNewI,jAssociationNameRef);

            JMPIjvm::checkException(env);

            jint    jPathNameRef = DEBUG_ConvertCToJava (CIMObjectPath*, jint, objectPath);
            jobject jPathName    = env->NewObject(jv->CIMObjectPathClassRef,jv->CIMObjectPathNewI,jPathNameRef);

            JMPIjvm::checkException(env);

            jstring jRole = env->NewStringUTF(request->role.getCString());

            JMPIjvm::checkException(env);

#ifdef PEGASUS_DEBUG
            DDD(PEGASUS_STD(cerr)<<"--- JMPIProviderManager::handleReferenceNamesRequest: assocName          = "<<objectPath->toString ()<<PEGASUS_STD(endl));
            DDD(PEGASUS_STD(cerr)<<"--- JMPIProviderManager::handleReferenceNamesRequest: pathName           = "<<resultPath->toString ()<<PEGASUS_STD(endl));
            DDD(PEGASUS_STD(cerr)<<"--- JMPIProviderManager::handleReferenceNamesRequest: role               = "<<request->role<<PEGASUS_STD(endl));
#endif

            jobjectArray jVec=(jobjectArray)env->CallObjectMethod((jobject)pr.jProvider,
                                                                  id,
                                                                  jAssociationName,
                                                                  jPathName,
                                                                  jRole);

            JMPIjvm::checkException(env);

            STAT_PMS_PROVIDEREND;

            handler.processing();
            if (jVec) {
                for (int i=0,m=env->CallIntMethod(jVec,JMPIjvm::jv.VectorSize); i<m; i++) {
                    JMPIjvm::checkException(env);

                    jobject jcopRet = env->CallObjectMethod(jVec,JMPIjvm::jv.VectorElementAt,i);

                    JMPIjvm::checkException(env);

                    jint jcopRetRef = env->CallIntMethod(jcopRet,JMPIjvm::jv.CIMObjectPathCInst);

                    JMPIjvm::checkException(env);

                    CIMObjectPath *copRet = DEBUG_ConvertJavaToC (jint, CIMObjectPath*, jcopRetRef);

                    handler.deliver(*copRet);
                }
            }
            handler.complete();
            break;
        }

        case METHOD_UNKNOWN:
        {
            DDD (PEGASUS_STD(cout)<<"--- JMPIProviderManager::handleReferenceNamesRequest: should not be here!"<<PEGASUS_STD(endl));
            break;
        }
        }
    }
    HandlerCatch(handler);

    if (env) JMPIjvm::detachThread();

    PEG_METHOD_EXIT();

    STAT_COPYDISPATCHER

    return(response);
}

Message * JMPIProviderManager::handleGetPropertyRequest(const Message * message) throw()
{
    PEG_METHOD_ENTER(TRC_PROVIDERMANAGER,"JMPIProviderManager::handleGetPropertyRequest");

    HandlerIntro(GetProperty,message,request,response,handler,CIMValue());

    typedef enum {
       METHOD_UNKNOWN = 0,
       METHOD_SNIA_PROVIDER20,
    } METHOD_VERSION;
    METHOD_VERSION   eMethodFound  = METHOD_UNKNOWN;
    JNIEnv          *env           = NULL;

    try {
        Logger::put(Logger::STANDARD_LOG, System::CIMSERVER, Logger::TRACE,
            "JMPIProviderManager::handleGetPropertyRequest - Host name: $0  Name space: $1  Class name: $2",
            System::getHostName(),
            request->nameSpace.getString(),
            request->instanceName.getClassName().getString());

        DDD(PEGASUS_STD(cout)<<"--- JMPIProviderManager::handleGetPropertyRequest: hostname = "<<System::getHostName()<<", namespace = "<<request->nameSpace.getString()<<", classname = "<<request->className.getString()<<PEGASUS_STD(endl));

        // make target object path
        CIMObjectPath *objectPath = new CIMObjectPath (System::getHostName(),
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
        JMPIProvider &pr = ph.GetProvider();

        PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,"Calling provider.getPropertyValue: " + pr.getName());

        DDD(PEGASUS_STD(cout)<<"--- JMPIProviderManager::handleGetPropertyRequest: Calling provider getPropertyValue: "<<pr.getName()<<PEGASUS_STD(endl));

        JvmVector *jv = 0;

        env = JMPIjvm::attachThread(&jv);

        JMPIProvider::pm_service_op_lock op_lock(&pr);

        STAT_GETSTARTTIME;

        jmethodID id = NULL;

        // public abstract org.pegasus.jmpi.CIMValue getPropertyValue (org.pegasus.jmpi.CIMObjectPath cop,
        //                                                             java.lang.String               oclass,
        //                                                             java.lang.String               pName)
        //        throws org.pegasus.jmpi.CIMException
        //
        id = env->GetMethodID((jclass)pr.jProviderClass,
                              "getPropertyValue",
                              "(Lorg/pegasus/jmpi/CIMObjectPath;Ljava/lang/String;Ljava/lang/String;)Lorg/pegasus/jmpi/CIMValue;");

        if (id != NULL)
        {
            eMethodFound = METHOD_SNIA_PROVIDER20;
            DDD (PEGASUS_STD(cout)<<"--- JMPIProviderManager::handleGetPropertyRequest: found METHOD_SNIA_PROVIDER20."<<PEGASUS_STD(endl));
        }

        if (id == NULL)
        {
            DDD (PEGASUS_STD(cout)<<"--- JMPIProviderManager::handleGetPropertyRequest: found no method!"<<PEGASUS_STD(endl));
        }

        JMPIjvm::checkException(env);

        switch (eMethodFound)
        {
        case METHOD_SNIA_PROVIDER20:
        {
            jint    jcopref = DEBUG_ConvertCToJava (CIMObjectPath*, jint, objectPath);
            jobject jcop    = env->NewObject(jv->CIMObjectPathClassRef, jv->CIMObjectPathNewI, jcopref);

            JMPIjvm::checkException(env);

            jstring joclass = env->NewStringUTF(request->instanceName.getClassName().getString().getCString());

            JMPIjvm::checkException(env);

            jstring jpName = env->NewStringUTF(request->propertyName.getString().getCString());

            JMPIjvm::checkException(env);

            STAT_GETSTARTTIME;

            jobject jvalRet = env->CallObjectMethod ((jobject)pr.jProvider,
                                                     id,
                                                     jcop,
                                                     joclass,
                                                     jpName);

            JMPIjvm::checkException(env);

            STAT_PMS_PROVIDEREND;

            handler.processing();

            if (jvalRet)
            {
               jint      jvalRetRef = env->CallIntMethod(jvalRet,JMPIjvm::jv.CIMValueCInst);
               CIMValue *valRet     = DEBUG_ConvertJavaToC (jint, CIMValue*, jvalRetRef);

               JMPIjvm::checkException(env);

               handler.deliver(*valRet);
            }
            handler.complete();
            break;
        }

        case METHOD_UNKNOWN:
        {
            DDD (PEGASUS_STD(cout)<<"--- JMPIProviderManager::handleGetPropertyRequest: should not be here!"<<PEGASUS_STD(endl));
            break;
        }
        }
    }
    HandlerCatch(handler);

    if (env) JMPIjvm::detachThread();

    PEG_METHOD_EXIT();

    STAT_COPYDISPATCHER

    return(response);
}

Message * JMPIProviderManager::handleSetPropertyRequest(const Message * message) throw()
{
    PEG_METHOD_ENTER(TRC_PROVIDERMANAGER,"JMPIProviderManager::handleSetPropertyRequest");

    HandlerIntroVoid(SetProperty,message,request,response,handler);

    typedef enum {
       METHOD_UNKNOWN = 0,
       METHOD_SNIA_PROVIDER20,
    } METHOD_VERSION;
    METHOD_VERSION   eMethodFound  = METHOD_UNKNOWN;
    JNIEnv          *env           = NULL;

    try {
        Logger::put(Logger::STANDARD_LOG, System::CIMSERVER, Logger::TRACE,
            "JMPIProviderManager::handleSetPropertyRequest - Host name: $0  Name space: $1  Class name: $2",
            System::getHostName(),
            request->nameSpace.getString(),
            request->instanceName.getClassName().getString());

        DDD(PEGASUS_STD(cout)<<"--- JMPIProviderManager::handleSetPropertyRequest: hostname = "<<System::getHostName()<<", namespace = "<<request->nameSpace.getString()<<", classname = "<<request->className.getString()<<PEGASUS_STD(endl));

        // make target object path
        CIMObjectPath *objectPath = new CIMObjectPath (System::getHostName(),
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
        JMPIProvider &pr = ph.GetProvider();

        PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,"Calling provider.setPropertyValue: " + pr.getName());

        DDD(PEGASUS_STD(cout)<<"--- JMPIProviderManager::handleSetPropertyRequest: Calling provider setPropertyValue: "<<pr.getName()<<PEGASUS_STD(endl));

        JvmVector *jv = 0;

        env = JMPIjvm::attachThread(&jv);

        JMPIProvider::pm_service_op_lock op_lock(&pr);

        STAT_GETSTARTTIME;

        jmethodID id = NULL;

        // public abstract void setPropertyValue (org.pegasus.jmpi.CIMObjectPath cop,
        //                                        java.lang.String               oclass,
        //                                        java.lang.String               pName,
        //                                        org.pegasus.jmpi.CIMValue      val)
        //        throws org.pegasus.jmpi.CIMException
        //
        id = env->GetMethodID((jclass)pr.jProviderClass,
                              "setPropertyValue",
                              "(Lorg/pegasus/jmpi/CIMObjectPath;Ljava/lang/String;Ljava/lang/String;Lorg/pegasus/jmpi/CIMValue;)V");

        if (id != NULL)
        {
            eMethodFound = METHOD_SNIA_PROVIDER20;
            DDD (PEGASUS_STD(cout)<<"--- JMPIProviderManager::handleSetPropertyRequest: found METHOD_SNIA_PROVIDER20."<<PEGASUS_STD(endl));
        }

        if (id == NULL)
        {
            DDD (PEGASUS_STD(cout)<<"--- JMPIProviderManager::handleSetPropertyRequest: found no method!"<<PEGASUS_STD(endl));
        }

        JMPIjvm::checkException(env);

        switch (eMethodFound)
        {
        case METHOD_SNIA_PROVIDER20:
        {
            jint    jcopref = DEBUG_ConvertCToJava (CIMObjectPath*, jint, objectPath);
            jobject jcop    = env->NewObject(jv->CIMObjectPathClassRef, jv->CIMObjectPathNewI, jcopref);

            JMPIjvm::checkException(env);

            jstring joclass = env->NewStringUTF(request->instanceName.getClassName().getString().getCString());

            JMPIjvm::checkException(env);

            jstring jpName = env->NewStringUTF(request->propertyName.getString().getCString());

            JMPIjvm::checkException(env);

            CIMValue *val = new CIMValue (request->newValue);

            JMPIjvm::checkException(env);

            jint    jvalref = DEBUG_ConvertCToJava (CIMValue*, jint, val);
            jobject jval    = env->NewObject(jv->CIMValueClassRef, jv->CIMValueNewI, jvalref);

            JMPIjvm::checkException(env);

            STAT_GETSTARTTIME;

            env->CallVoidMethod ((jobject)pr.jProvider,
                                 id,
                                 jcop,
                                 joclass,
                                 jpName,
                                 jval);

            JMPIjvm::checkException(env);

            STAT_PMS_PROVIDEREND;
            break;
        }

        case METHOD_UNKNOWN:
        {
            DDD (PEGASUS_STD(cout)<<"--- JMPIProviderManager::handleSetPropertyRequest: should not be here!"<<PEGASUS_STD(endl));
            break;
        }
        }
    }
    HandlerCatch(handler);

    if (env) JMPIjvm::detachThread();

    PEG_METHOD_EXIT();

    STAT_COPYDISPATCHER

    return(response);
}

Message * JMPIProviderManager::handleInvokeMethodRequest(const Message * message) throw()
{
    PEG_METHOD_ENTER(TRC_PROVIDERMANAGER,"JMPIProviderManager::handleInvokeMethodRequest");

    HandlerIntroMethod(InvokeMethod,message,request,response,handler);

    typedef enum {
       METHOD_UNKNOWN = 0,
       METHOD_PEGASUS_24,
       METHOD_SNIA_PROVIDER20,
    } METHOD_VERSION;
    METHOD_VERSION   eMethodFound  = METHOD_UNKNOWN;
    JNIEnv          *env           = NULL;

    try {
        Logger::put(Logger::STANDARD_LOG, System::CIMSERVER, Logger::TRACE,
            "JMPIProviderManager::handleInvokeMethodRequest - Host name: $0  Name space: $1  Class name: $2",
            System::getHostName(),
            request->nameSpace.getString(),
            request->instanceName.getClassName().getString());

        DDD(PEGASUS_STD(cout)<<"--- JMPIProviderManager::handleInvokeMethodRequest: hostname = "<<System::getHostName()<<", namespace = "<<request->nameSpace.getString()<<", classname = "<<request->instanceName.getClassName().getString()<<PEGASUS_STD(endl));

        // make target object path
        CIMObjectPath *objectPath = new CIMObjectPath (System::getHostName(),
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

        JMPIProvider &pr=ph.GetProvider();

        PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4, "Calling provider.invokeMethod: " + pr.getName());

        DDD(PEGASUS_STD(cout)<<"--- JMPIProviderManager::handleInvokeMethodRequest: Calling provider invokeMethod: "<<pr.getName()<<PEGASUS_STD(endl));

        JvmVector *jv = 0;

        env = JMPIjvm::attachThread(&jv);

        JMPIProvider::pm_service_op_lock op_lock(&pr);

        STAT_GETSTARTTIME;

        jmethodID id = NULL;

        // public abstract org.pegasus.jmpi.CIMValue invokeMethod (org.pegasus.jmpi.CIMObjectPath cop,
        //                                                         java.lang.String               name,
        //                                                         java.util.Vector               in,
        //                                                         java.util.Vector               out)
        //        throws org.pegasus.jmpi.CIMException
        id = env->GetMethodID((jclass)pr.jProviderClass,
                              "invokeMethod",
                              "(Lorg/pegasus/jmpi/CIMObjectPath;Ljava/lang/String;Ljava/util/Vector;Ljava/util/Vector;)Lorg/pegasus/jmpi/CIMValue;");

        if (id != NULL)
        {
            eMethodFound = METHOD_SNIA_PROVIDER20;
            DDD (PEGASUS_STD(cout)<<"--- JMPIProviderManager::handleInvokeMethodRequest: found METHOD_SNIA_PROVIDER20."<<PEGASUS_STD(endl));
        }

        if (id == NULL)
        {
            env->ExceptionClear();

            // public org.pegasus.jmpi.CIMValue invokeMethod (org.pegasus.jmpi.CIMObjectPath op,
            //                                                java.lang.String               methodName,
            //                                                org.pegasus.jmpi.CIMArgument[] inArgs,
            //                                                org.pegasus.jmpi.CIMArgument[] outArgs)
            //        throws org.pegasus.jmpi.CIMException
            id = env->GetMethodID((jclass)pr.jProviderClass,
                                  "invokeMethod",
                                  "(Lorg/pegasus/jmpi/CIMObjectPath;Ljava/lang/String;[Lorg/pegasus/jmpi/CIMArgument;[Lorg/pegasus/jmpi/CIMArgument;)Lorg/pegasus/jmpi/CIMValue;");

            if (id != NULL)
            {
                eMethodFound = METHOD_PEGASUS_24;
                DDD (PEGASUS_STD(cout)<<"--- JMPIProviderManager::handleInvokeMethodRequest: found METHOD_PEGASUS_24."<<PEGASUS_STD(endl));
            }
        }

        JMPIjvm::checkException(env);

        switch (eMethodFound)
        {
        case METHOD_PEGASUS_24:
        {
            jint    jcopRef = DEBUG_ConvertCToJava (CIMObjectPath*, jint, objectPath);
            jobject jcop    = env->NewObject(jv->CIMObjectPathClassRef,jv->CIMObjectPathNewI,jcopRef);

            JMPIjvm::checkException(env);

            jstring jMethod = env->NewStringUTF(request->methodName.getString().getCString());

            JMPIjvm::checkException(env);

            Uint32 m=request->inParameters.size();

            jobjectArray jArIn=(jobjectArray)env->NewObjectArray(m,jv->CIMArgumentClassRef,NULL);

            for (Uint32 i=0; i<m; i++) {
              CIMParamValue *parm    = new CIMParamValue(request->inParameters[i]);
              jint           jArgRef = DEBUG_ConvertCToJava (CIMParamValue*, jint, parm);
              jobject        jArg    = env->NewObject(jv->CIMArgumentClassRef,jv->CIMArgumentNewI,jArgRef);

              env->SetObjectArrayElement(jArIn,i,jArg);
            }

            jobjectArray jArOut=(jobjectArray)env->NewObjectArray(24,jv->CIMArgumentClassRef,NULL);

            jobject jValueRet = env->CallObjectMethod((jobject)pr.jProvider,
                                                      id,
                                                      jcop,
                                                      jMethod,
                                                      jArIn,
                                                      jArOut);
            JMPIjvm::checkException(env);

            STAT_PMS_PROVIDEREND;

            handler.processing();

            jint      jValueRetRef = env->CallIntMethod(jValueRet,JMPIjvm::jv.CIMValueCInst);
            CIMValue *valueRet     = DEBUG_ConvertJavaToC (jint, CIMValue*, jValueRetRef);

            handler.deliver(*valueRet);

            for (int i=0; i<24; i++) {
                jobject jArg = env->GetObjectArrayElement(jArOut,i);

                JMPIjvm::checkException(env);

                if (jArg==NULL)
                   break;

                jint           jpRef = env->CallIntMethod(jArg,JMPIjvm::jv.CIMArgumentCInst);
                CIMParamValue *p     = DEBUG_ConvertJavaToC (jint, CIMParamValue*, jpRef);

                JMPIjvm::checkException(env);

                handler.deliverParamValue(*p);
            }

            handler.complete();
            break;
        }

        case METHOD_SNIA_PROVIDER20:
        {
            jint    jcopRef = DEBUG_ConvertCToJava (CIMObjectPath*, jint, objectPath);
            jobject jcop    = env->NewObject(jv->CIMObjectPathClassRef,jv->CIMObjectPathNewI,jcopRef);

            JMPIjvm::checkException(env);

            jstring jMethod = env->NewStringUTF(request->methodName.getString().getCString());

            JMPIjvm::checkException(env);

            jobject jVecIn = env->NewObject(jv->VectorClassRef,jv->VectorNew);

            JMPIjvm::checkException(env);

            for (int i=0,m=request->inParameters.size(); i<m; i++)
            {
                const CIMParamValue &parm  = request->inParameters[i];
                const CIMValue       v     = parm.getValue();
                CIMProperty         *p     = new CIMProperty(parm.getParameterName(),v,v.getArraySize());
                jint                 jpRef = DEBUG_ConvertCToJava (CIMProperty*, jint, p);
                jobject              jp    = env->NewObject(jv->CIMPropertyClassRef,jv->CIMPropertyNewI,jpRef);

                env->CallVoidMethod(jVecIn,jv->VectorAddElement,jp);
             }

            jobject jVecOut=env->NewObject(jv->VectorClassRef,jv->VectorNew);
            JMPIjvm::checkException(env);

            jobject jValueRet = env->CallObjectMethod((jobject)pr.jProvider,
                                                      id,
                                                      jcop,
                                                      jMethod,
                                                      jVecIn,
                                                      jVecOut);
            JMPIjvm::checkException(env);

            STAT_PMS_PROVIDEREND;

            handler.processing();

            jint      jValueRetRef = env->CallIntMethod(jValueRet,JMPIjvm::jv.CIMValueCInst);
            CIMValue *valueRet     = DEBUG_ConvertJavaToC (jint, CIMValue*, jValueRetRef);

            handler.deliver(*valueRet);

            for (int i=0,m=env->CallIntMethod(jVecOut,JMPIjvm::jv.VectorSize); i<m; i++)
            {
                JMPIjvm::checkException(env);

                jobject jProp = env->CallObjectMethod(jVecOut,JMPIjvm::jv.VectorElementAt,i);

                JMPIjvm::checkException(env);

                jint         jpRef = env->CallIntMethod(jProp,JMPIjvm::jv.CIMPropertyCInst);
                CIMProperty *p     = DEBUG_ConvertJavaToC (jint, CIMProperty*, jpRef);

                JMPIjvm::checkException(env);

                handler.deliverParamValue(CIMParamValue(p->getName().getString(),p->getValue()));
            }

            handler.complete();
            break;
        }

        case METHOD_UNKNOWN:
        {
            DDD (PEGASUS_STD(cout)<<"--- JMPIProviderManager::handleInvokeMethodRequest: should not be here!"<<PEGASUS_STD(endl));
            break;
        }
        }
    }
    HandlerCatch(handler);

    if (env) JMPIjvm::detachThread();

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

Message * JMPIProviderManager::handleCreateSubscriptionRequest(const Message * message) throw()
{
    PEG_METHOD_ENTER(TRC_PROVIDERMANAGER, "JMPIProviderManager::handleCreateSubscriptionRequest");

    HandlerIntroInd(CreateSubscription,message,request,response,handler);

    typedef enum {
       METHOD_UNKNOWN = 0,
       METHOD_SNIA_PROVIDER20,
    } METHOD_VERSION;
    METHOD_VERSION   eMethodFound  = METHOD_UNKNOWN;
    JNIEnv          *env           = NULL;

    try {
        String               providerName,
                             providerLocation;
        CIMInstance          req_provider,
                             req_providerModule;

        ProviderIdContainer  pidc                = (ProviderIdContainer)request->operationContext.get(ProviderIdContainer::NAME);

        req_provider       = pidc.getProvider();
        req_providerModule = pidc.getModule();

        LocateIndicationProviderNames(req_provider, req_providerModule,providerName,providerLocation);

        Logger::put(Logger::STANDARD_LOG, System::CIMSERVER, Logger::TRACE,
            "JMPIProviderManager::handleCreateSubscriptionRequest - Host name: $0  Name space: $1  Provider name(s): $2",
            System::getHostName(),
            request->nameSpace.getString(),
            providerName);

        DDD(PEGASUS_STD(cout)<<"--- JMPIProviderManager::handleCreateSubscriptionRequest: hostname = "<<System::getHostName()<<", namespace = "<<request->nameSpace.getString()<<", providername = "<<providerName<<PEGASUS_STD(endl));

        String fileName = resolveFileName(providerLocation);

        // get cached or load new provider module
        JMPIProvider::OpProviderHolder ph =
            providerManager.getProvider(fileName, providerName, String::EMPTY);

        indProvRecord *prec = NULL;

        provTab.lookup(providerName,prec);

        if (prec)
        {
            prec->count++;
        }
        else
        {
            prec=new indProvRecord();
            provTab.insert(providerName,prec);
        }

        //
        //  Save the provider instance from the request
        //
        ph.GetProvider ().setProviderInstance (req_provider);

        indSelectRecord     *srec  = new indSelectRecord();
        const CIMObjectPath &sPath = request->subscriptionInstance.getPath();

        selxTab.insert(sPath.toString(),srec);

        // convert arguments
        OperationContext *context=new OperationContext();

        if (prec->ctx==NULL)
        {
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

        JMPIProvider &pr = ph.GetProvider();

        CIMOMHandleQueryContext *qcontext=new CIMOMHandleQueryContext(CIMNamespaceName(request->nameSpace.getString()),
                                                                      *pr._cimom_handle);

        CMPI_SelectExp *eSelx = new CMPI_SelectExp(*context,
                                                   qcontext,
                                                   request->query,
                                                   sub_cntr.getQueryLanguage());

        srec->eSelx=eSelx;
        srec->qContext=qcontext;

        PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4, "Calling provider.createSubscriptionRequest: " + pr.getName());

        DDD(PEGASUS_STD(cout)<<"--- JMPIProviderManager::handleCreateSubscriptionRequest: Calling provider createSubscriptionRequest: "<<pr.getName()<<PEGASUS_STD(endl));

        for(Uint32 i = 0, n = request->classNames.size(); i < n; i++) {
            CIMObjectPath className(
                System::getHostName(),
                request->nameSpace,
                request->classNames[i]);
            eSelx->classNames.append(className);
        }

        CIMPropertyList propertyList = request->propertyList;

        if (!propertyList.isNull())
        {
           Array<CIMName> p      = propertyList.getPropertyNameArray();
           int            pCount = p.size();

           eSelx->props=(const char**)malloc((1+pCount)*sizeof(char*));

           for (int i=0; i<pCount; i++)
           {
              eSelx->props[i]=strdup(p[i].getString().getCString());
           }
           eSelx->props[pCount]=NULL;
        }

        JvmVector *jv = 0;

        env = JMPIjvm::attachThread(&jv);

        JMPIProvider::pm_service_op_lock op_lock(&pr);

        STAT_GETSTARTTIME;

        jmethodID id = NULL;

        // public void authorizeFilter (org.pegasus.jmpi.SelectExp     filter,
        //                              java.lang.String               eventType,
        //                              org.pegasus.jmpi.CIMObjectPath classPath,
        //                              java.lang.String               owner)
        //        throws org.pegasus.jmpi.CIMException
        id = env->GetMethodID((jclass)pr.jProviderClass,
                              "activateFilter",
                              "(Lorg/pegasus/jmpi/SelectExp;Ljava/lang/String;Lorg/pegasus/jmpi/CIMObjectPath;Z)V");

        if (id != NULL)
        {
            eMethodFound = METHOD_SNIA_PROVIDER20;
            DDD (PEGASUS_STD(cout)<<"--- JMPIProviderManager::handleCreateSubscriptionRequest: found METHOD_SNIA_PROVIDER20."<<PEGASUS_STD(endl));
        }

        JMPIjvm::checkException(env);

        switch (eMethodFound)
        {
        case METHOD_SNIA_PROVIDER20:
        {
            jint    jSelRef = DEBUG_ConvertCToJava (CMPI_SelectExp*, jint, eSelx);
            jobject jSel    = env->NewObject(jv->SelectExpClassRef,jv->SelectExpNewI,jSelRef);

            JMPIjvm::checkException(env);

            jint    jcopRef = DEBUG_ConvertCToJava (CIMObjectPath*, jint, &eSelx->classNames[0]);
            jobject jcop    = env->NewObject(jv->CIMObjectPathClassRef,jv->CIMObjectPathNewI,jcopRef);

            JMPIjvm::checkException(env);

            jstring jType = env->NewStringUTF(request->nameSpace.getString().getCString());

            JMPIjvm::checkException(env);

            env->CallVoidMethod((jobject)pr.jProvider,
                                id,
                                jSel,
                                jType,
                                jcop,
                                (jboolean)0);

            JMPIjvm::checkException(env);

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
                    prec->enabled = true;
                    CIMRequestMessage * request = 0;
                    CIMResponseMessage * response = 0;
                    prec->handler = new EnableIndicationsResponseHandler
                        (request, response, req_provider, _indicationCallback);
                }
            }

            STAT_PMS_PROVIDEREND;
            break;
        }

        case METHOD_UNKNOWN:
        {
            DDD (PEGASUS_STD(cout)<<"--- JMPIProviderManager::handleCreateSubscriptionRequest: should not be here!"<<PEGASUS_STD(endl));
            break;
        }
        }

    }
    HandlerCatch(handler);

    if (env) JMPIjvm::detachThread();

    PEG_METHOD_EXIT();

    return(response);
}

Message * JMPIProviderManager::handleDeleteSubscriptionRequest(const Message * message) throw()
{
    PEG_METHOD_ENTER(TRC_PROVIDERMANAGER, "JMPIProviderManager::handleDeleteSubscriptionRequest");

    HandlerIntroInd(DeleteSubscription,message,request,response,handler);

    typedef enum {
       METHOD_UNKNOWN = 0,
       METHOD_SNIA_PROVIDER20,
    } METHOD_VERSION;
    METHOD_VERSION   eMethodFound  = METHOD_UNKNOWN;
    JNIEnv          *env           = NULL;

    try {
        String              providerName,
                            providerLocation;
        CIMInstance         req_provider,
                            req_providerModule;
        ProviderIdContainer pidc               = (ProviderIdContainer)request->operationContext.get(ProviderIdContainer::NAME);

        req_provider       = pidc.getProvider();
        req_providerModule = pidc.getModule();

        LocateIndicationProviderNames(req_provider, req_providerModule, providerName,providerLocation);

        Logger::put(Logger::STANDARD_LOG, System::CIMSERVER, Logger::TRACE,
            "JMPIProviderManager::handleDeleteSubscriptionRequest - Host name: $0  Name space: $1  Provider name(s): $2",
            System::getHostName(),
            request->nameSpace.getString(),
            providerName);

        DDD(PEGASUS_STD(cout)<<"--- JMPIProviderManager::handleDeleteSubscriptionRequest: hostname = "<<System::getHostName()<<", namespace = "<<request->nameSpace.getString()<<", providername = "<<providerName<<PEGASUS_STD(endl));

        String fileName = resolveFileName(providerLocation);

        // get cached or load new provider module
        JMPIProvider::OpProviderHolder ph =
            providerManager.getProvider(fileName, providerName, String::EMPTY);

        indProvRecord *prec = NULL;

        provTab.lookup(providerName,prec);
        if (--prec->count <= 0)
        {
            provTab.remove(providerName);
            prec=NULL;
        }

        indSelectRecord     *srec        = NULL;
        const CIMObjectPath &sPath       = request->subscriptionInstance.getPath();
        String               sPathString = sPath.toString();

        selxTab.lookup(sPathString,srec);

        CMPI_SelectExp          *eSelx    = srec->eSelx;
        CIMOMHandleQueryContext *qContext = srec->qContext;

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

        PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4, "Calling provider.deleteSubscriptionRequest: " + pr.getName());

        DDD(PEGASUS_STD(cout)<<"--- JMPIProviderManager::handleDeleteSubscriptionRequest: Calling provider deleteSubscriptionRequest: "<<pr.getName()<<PEGASUS_STD(endl));

        JvmVector *jv = 0;

        env = JMPIjvm::attachThread(&jv);

        JMPIProvider::pm_service_op_lock op_lock(&pr);

        STAT_GETSTARTTIME;

        jmethodID id = NULL;

        // public void deActivateFilter (org.pegasus.jmpi.SelectExp    filter,
        //                              java.lang.String               eventType,
        //                              org.pegasus.jmpi.CIMObjectPath classPath,
        //                              boolean                        lastActivation)
        //        throws org.pegasus.jmpi.CIMException
        id = env->GetMethodID((jclass)pr.jProviderClass,
                              "deActivateFilter",
                              "(Lorg/pegasus/jmpi/SelectExp;Ljava/lang/String;Lorg/pegasus/jmpi/CIMObjectPath;Z)V");

        if (id != NULL)
        {
            eMethodFound = METHOD_SNIA_PROVIDER20;
            DDD (PEGASUS_STD(cout)<<"--- JMPIProviderManager::handleDeleteSubscriptionRequest: found METHOD_SNIA_PROVIDER20."<<PEGASUS_STD(endl));
        }

        JMPIjvm::checkException(env);

        switch (eMethodFound)
        {
        case METHOD_SNIA_PROVIDER20:
        {
            jint    jObj = DEBUG_ConvertCToJava (CMPI_SelectExp*, jint, eSelx);
            jobject jSel = env->NewObject(jv->SelectExpClassRef,jv->SelectExpNewI,jObj);

            JMPIjvm::checkException(env);

            jObj = DEBUG_ConvertCToJava (CIMObjectPath*, jint, &eSelx->classNames[0]);

            jobject jRef = env->NewObject(jv->CIMObjectPathClassRef,jv->CIMObjectPathNewI,jObj);

            JMPIjvm::checkException(env);

            jstring jType = env->NewStringUTF(request->nameSpace.getString().getCString());

            JMPIjvm::checkException(env);

            env->CallVoidMethod((jobject)pr.jProvider,
                                id,
                                jSel,
                                jType,
                                jRef,
                                (jboolean)(prec==NULL));

            JMPIjvm::checkException(env);

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
                    prec->enabled = false;
                    if (prec->handler) delete prec->handler;
                    prec->handler = NULL;
                }
            }

            STAT_PMS_PROVIDEREND;

            delete eSelx;
            delete qContext;
            delete srec;
            break;
        }

        case METHOD_UNKNOWN:
        {
            DDD (PEGASUS_STD(cout)<<"--- JMPIProviderManager::handleDeleteSubscriptionRequest: should not be here!"<<PEGASUS_STD(endl));
            break;
        }
        }
    }
    HandlerCatch(handler);

    if (env) JMPIjvm::detachThread();

    PEG_METHOD_EXIT();

    STAT_COPYDISPATCHER

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

Message * JMPIProviderManager::handleSubscriptionInitCompleteRequest (const Message * message)
{
    PEG_METHOD_ENTER (TRC_PROVIDERMANAGER,
     "JMPIProviderManager::handleSubscriptionInitCompleteRequest");

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
    Array <JMPIProvider *> enableProviders;
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
            JMPIProvider::OpProviderHolder ph = providerManager.getProvider
                (enableProviders [i]->getModule ()->getFileName (),
                 enableProviders [i]->getName ());

            indProvRecord * prec = NULL;
            provTab.lookup (enableProviders [i]->getName (), prec);
            if (prec)
            {
                prec->enabled = true;
                CIMRequestMessage * request = 0;
                CIMResponseMessage * response = 0;
                prec->handler = new EnableIndicationsResponseHandler
                    (request, response, provider, _indicationCallback);
            }
        }
        catch (CIMException & e)
        {
            PEG_TRACE_STRING (TRC_PROVIDERMANAGER, Tracer::LEVEL2,
                "CIMException: " + e.getMessage ());
        }
        catch (Exception & e)
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

Message * JMPIProviderManager::handleUnsupportedRequest(const Message * message) throw()
{
    PEG_METHOD_ENTER(TRC_PROVIDERMANAGER,"JMPIProviderManager::handleUnsupportedRequest");

    CIMRequestMessage* request =
        dynamic_cast<CIMRequestMessage *>(const_cast<Message *>(message));
    PEGASUS_ASSERT(request != 0 );

    CIMResponseMessage* response = request->buildResponse();
    response->cimException =
        PEGASUS_CIM_EXCEPTION(CIM_ERR_NOT_SUPPORTED, String::EMPTY);

    PEG_METHOD_EXIT();
    return response;
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
