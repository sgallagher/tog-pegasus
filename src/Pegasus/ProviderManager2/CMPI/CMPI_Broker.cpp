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
// Modified By:
//
//%/////////////////////////////////////////////////////////////////////////////

#define CMPI_VER_86 1

#include "CMPI_Broker.h"
#include "CMPI_Object.h"
#include "CMPI_ContextArgs.h"
#include "CMPI_Enumeration.h"
#include "CMPI_Value.h"
#include "CMPIProviderManager.h"

#include <Pegasus/Common/CIMName.h>
#include <Pegasus/Common/CIMPropertyList.h>
#include <Pegasus/Provider/CIMOMHandle.h>
#include <Pegasus/Common/CIMValue.h>
#include <Pegasus/Common/CIMType.h>


PEGASUS_USING_STD;
PEGASUS_NAMESPACE_BEGIN

#define DDD(X)   if (_cmpi_trace) X;

extern int _cmpi_trace;

CIMPropertyList *getList(char** l) {
   return new CIMPropertyList;
}

CIMClass* mbGetClass(CMPIBroker *mb, const CIMObjectPath &cop) {
   DDD(cout<<"--- mbGetClass()"<<std::endl);
   CMPI_Broker *xBroker=(CMPI_Broker*)mb;
   String clsId=cop.getNameSpace().getString()+":"+cop.getClassName().getString();
   CIMClass *ccp;

   AutoMutex mtx(&((CMPI_Broker*)mb)->mtx);
   if (xBroker->clsCache) {
      if (xBroker->clsCache->lookup(clsId,ccp)) return ccp;
   }
   else xBroker->clsCache=new ClassCache();

   try {
      CIMClass cc=CM_CIMOM(mb)->getClass(
                  OperationContext(),
		  cop.getNameSpace(),
		  cop.getClassName(),
		  (bool)0,
		  (bool)1,
		  (bool)0,
		  CIMPropertyList());
      ccp=new CIMClass(cc);
      xBroker->clsCache->insert(clsId,ccp);
      return ccp;
   }
   catch (CIMException &e) {
      DDD(cout<<"### exception: mbGetClass - code: "<<e.getCode()<<" msg: "<<e.getMessage()<<std::endl);
   }
   return NULL;
}

static CMPIInstance* mbGetInstance(CMPIBroker *mb, CMPIContext *ctx,
                 CMPIObjectPath *cop, char **properties, CMPIStatus *rc) {
   DDD(cout<<"--- mbGetInstance()"<<std::endl);
   CMPIFlags flgs=ctx->ft->getEntry(ctx,CMPIInvocationFlags,NULL).value.uint32;
   CIMPropertyList *props=getList(properties);
   CIMObjectPath qop(String::EMPTY,CIMNamespaceName(),
                     CM_ObjectPath(cop)->getClassName(),
		     CM_ObjectPath(cop)->getKeyBindings());

   AutoMutex mtx(&((CMPI_Broker*)mb)->mtx);
   try {
      CIMInstance ci=CM_CIMOM(mb)->getInstance(
                  OperationContext(*CM_Context(ctx)),
		  CM_ObjectPath(cop)->getNameSpace(),
		  qop, //*CM_ObjectPath(cop),
		  CM_LocalOnly(flgs),
		  CM_IncludeQualifiers(flgs),
		  CM_ClassOrigin(flgs),
		  *props);
      delete props;
      if (rc) CMSetStatus(rc,CMPI_RC_OK);
      return (CMPIInstance*)new CMPI_Object(new CIMInstance(ci));
   }
   catch (CIMException &e) {
      DDD(cout<<"### exception: mbGetInstance - code: "<<e.getCode()<<" msg: "<<e.getMessage()<<std::endl);
      if (rc) CMSetStatus(rc,(CMPIrc)e.getCode());
   }
   delete props;
   return NULL;
}

static CMPIObjectPath* mbCreateInstance(CMPIBroker *mb, CMPIContext *ctx,
                CMPIObjectPath *cop, CMPIInstance *ci, CMPIStatus *rc) {
   DDD(cout<<"--- mbCreateInstance()"<<std::endl);

   AutoMutex mtx(&((CMPI_Broker*)mb)->mtx);
   try {
      CIMObjectPath ncop=CM_CIMOM(mb)->createInstance(
                  OperationContext(*CM_Context(ctx)),
		  CM_ObjectPath(cop)->getNameSpace(),
                  *CM_Instance(ci));
      if (rc) CMSetStatus(rc,CMPI_RC_OK);
      return (CMPIObjectPath*)new CMPI_Object(new CIMObjectPath(ncop));
   }
   catch (CIMException &e) {
      DDD(cout<<"### exception: mbCreateInstance - code: "<<e.getCode()<<" msg: "<<e.getMessage()<<std::endl);
      if (rc) CMSetStatus(rc,(CMPIrc)e.getCode());
   }
   if (rc) CMSetStatus(rc,CMPI_RC_ERROR);
   return NULL;
}

static CMPIStatus mbSetInstance(CMPIBroker *mb, CMPIContext *ctx,
		CMPIObjectPath *cop, CMPIInstance *ci) {
   DDD(cout<<"--- mbSetInstance()"<<std::endl);
   CMPIFlags flgs=ctx->ft->getEntry(ctx,CMPIInvocationFlags,NULL).value.uint32;

   AutoMutex mtx(&((CMPI_Broker*)mb)->mtx);
   try {
      CM_CIMOM(mb)->modifyInstance(
                  OperationContext(*CM_Context(ctx)),
		  CM_ObjectPath(cop)->getNameSpace(),
                  *CM_Instance(ci),
		  CM_IncludeQualifiers(flgs),
		  CIMPropertyList());
      CMReturn(CMPI_RC_OK);
   }
   catch (CIMException &e) {
      DDD(cout<<"### exception: mbSetInstance - code: "<<e.getCode()<<" msg: "<<e.getMessage()<<std::endl);
      CMReturn((CMPIrc)e.getCode());
   }
   CMReturn(CMPI_RC_ERR_FAILED);
}

static CMPIStatus mbDeleteInstance (CMPIBroker *mb, CMPIContext *ctx,
                 CMPIObjectPath *cop) {
   DDD(cout<<"--- mbDeleteInstance()"<<std::endl);
   CIMObjectPath qop(String::EMPTY,CIMNamespaceName(),
                     CM_ObjectPath(cop)->getClassName(),
		     CM_ObjectPath(cop)->getKeyBindings());

   AutoMutex mtx(&((CMPI_Broker*)mb)->mtx);
   try {
      CM_CIMOM(mb)->deleteInstance(
                  OperationContext(*CM_Context(ctx)),
		  CM_ObjectPath(cop)->getNameSpace(),
		  qop); //*CM_ObjectPath(cop));
      CMReturn(CMPI_RC_OK);
   }
   catch (CIMException &e) {
      DDD(cout<<"### exception: mbDeleteInstance - code: "<<e.getCode()<<" msg: "<<e.getMessage()<<std::endl);
      CMReturn((CMPIrc)e.getCode());
   }
   CMReturn(CMPI_RC_ERROR);
}

static CMPIEnumeration* mbExecQuery(CMPIBroker *mb, CMPIContext *ctx,
                 CMPIObjectPath *cop, char *query, char *lang, CMPIStatus *rc) {
   DDD(cout<<"--- mbExecQuery()"<<std::endl);

   AutoMutex mtx(&((CMPI_Broker*)mb)->mtx);
   try {
      Array<CIMObject> const &en=CM_CIMOM(mb)->execQuery(
                  OperationContext(*CM_Context(ctx)),
		  CM_ObjectPath(cop)->getNameSpace(),
		  String(query),
		  String(lang));
      if (rc) CMSetStatus(rc,CMPI_RC_OK);
      return new CMPI_ObjEnumeration(new Array<CIMObject>(en));
   }
   catch (CIMException &e) {
      DDD(cout<<"### exception: mbExecQuery - code: "<<e.getCode()<<" msg: "<<e.getMessage()<<std::endl);
      if (rc) CMSetStatus(rc,(CMPIrc)e.getCode());
   }
   if (rc) CMSetStatus(rc,CMPI_RC_ERR_FAILED);
   return NULL;
}

static CMPIEnumeration* mbEnumInstances(CMPIBroker *mb, CMPIContext *ctx,
                 CMPIObjectPath *cop, char **properties, CMPIStatus *rc) {
   DDD(cout<<"--- mbEnumInstances()"<<std::endl);

   CMPIFlags flgs=ctx->ft->getEntry(ctx,CMPIInvocationFlags,NULL).value.uint32;
   CIMPropertyList *props=getList(properties);

   AutoMutex mtx(&((CMPI_Broker*)mb)->mtx);
   try {
      Array<CIMInstance> const &en=CM_CIMOM(mb)->enumerateInstances(
                  OperationContext(*CM_Context(ctx)),
		  CM_ObjectPath(cop)->getNameSpace(),
		  CM_ObjectPath(cop)->getClassName(),
		  CM_DeepInheritance(flgs),
		  CM_LocalOnly(flgs),
		  CM_IncludeQualifiers(flgs),
		  CM_ClassOrigin(flgs),
		  *props);
      if (rc) CMSetStatus(rc,CMPI_RC_OK);
      delete props;
      return new CMPI_InstEnumeration(new Array<CIMInstance>(en));
   }
   catch (CIMException &e) {
      DDD(cout<<"### exception: mbEnumInstances - code: "<<e.getCode()<<" msg: "<<e.getMessage()<<std::endl);
      if (rc) CMSetStatus(rc,(CMPIrc)e.getCode());
   }
   if (rc) CMSetStatus(rc,CMPI_RC_ERR_FAILED);
   delete props;
   return NULL;
}

static CMPIEnumeration* mbEnumInstanceNames(CMPIBroker *mb, CMPIContext *ctx,
                 CMPIObjectPath *cop, CMPIStatus *rc) {
   DDD(cout<<"--- mbEnumInstanceNames()"<<std::endl);

   AutoMutex mtx(&((CMPI_Broker*)mb)->mtx);
  try {
      Array<CIMObjectPath> const &en=CM_CIMOM(mb)->enumerateInstanceNames(
                  OperationContext(*CM_Context(ctx)),
		  CM_ObjectPath(cop)->getNameSpace(),
		  CM_ObjectPath(cop)->getClassName());
      if (rc) CMSetStatus(rc,CMPI_RC_OK);
      return new CMPI_OpEnumeration(new Array<CIMObjectPath>(en));
   }
   catch (CIMException &e) {
      DDD(cout<<"### exception: mbEnumInstances - code: "<<e.getCode()<<" msg: "<<e.getMessage()<<std::endl);
      if (rc) CMSetStatus(rc,(CMPIrc)e.getCode());
   }
   if (rc) CMSetStatus(rc,CMPI_RC_ERR_FAILED);
   return NULL;
}

static CMPIEnumeration* mbAssociators(CMPIBroker *mb, CMPIContext *ctx,
                 CMPIObjectPath *cop, char *assocClass, char *resultClass,
                 char *role, char *resultRole, char **properties, CMPIStatus *rc) {
   DDD(cout<<"--- mbAssociators()"<<std::endl);
   CMPIFlags flgs=ctx->ft->getEntry(ctx,CMPIInvocationFlags,NULL).value.uint32;
   CIMPropertyList *props=getList(properties);
   CIMObjectPath qop(String::EMPTY,CIMNamespaceName(),
                     CM_ObjectPath(cop)->getClassName(),
		     CM_ObjectPath(cop)->getKeyBindings());

   AutoMutex mtx(&((CMPI_Broker*)mb)->mtx);
   try {
      Array<CIMObject> const &en=CM_CIMOM(mb)->associators(
                  OperationContext(*CM_Context(ctx)),
		  CM_ObjectPath(cop)->getNameSpace(),
		  qop, 
		  assocClass ? CIMName(assocClass) : CIMName(),
		  resultClass ? CIMName(resultClass) : CIMName(),
		  role ? String(role) : String::EMPTY,
		  resultRole ? String(resultRole) : String::EMPTY,
		  CM_IncludeQualifiers(flgs),
		  CM_ClassOrigin(flgs),
		  *props);
      if (rc) CMSetStatus(rc,CMPI_RC_OK);
      delete props;
      return new CMPI_ObjEnumeration(new Array<CIMObject>(en));
   }
   catch (CIMException &e) {
      DDD(cout<<"### exception: mbAssociators - code: "<<e.getCode()<<" msg: "<<e.getMessage()<<std::endl);
      if (rc) CMSetStatus(rc,(CMPIrc)e.getCode());
   }
   if (rc) CMSetStatus(rc,CMPI_RC_ERR_FAILED);
   delete props;
   return NULL;
}

static CMPIEnumeration* mbAssociatorNames(CMPIBroker *mb, CMPIContext *ctx,
                 CMPIObjectPath *cop, char *assocClass, char *resultClass,
		 char *role, char *resultRole, CMPIStatus *rc) {
   DDD(cout<<"--- mbAssociatorsNames()"<<std::endl);
   CIMObjectPath qop(String::EMPTY,CIMNamespaceName(),
                     CM_ObjectPath(cop)->getClassName(),
		     CM_ObjectPath(cop)->getKeyBindings());

   AutoMutex mtx(&((CMPI_Broker*)mb)->mtx);
   try {
      Array<CIMObjectPath> const &en=CM_CIMOM(mb)->associatorNames(
		  OperationContext(*CM_Context(ctx)),
		  CM_ObjectPath(cop)->getNameSpace(),
		  qop, 
		  assocClass ? CIMName(assocClass) : CIMName(),
		  resultClass ? CIMName(resultClass) : CIMName(),
		  role ? String(role) : String::EMPTY,
		  resultRole ? String(resultRole) : String::EMPTY);
      if (rc) CMSetStatus(rc,CMPI_RC_OK);
      return new CMPI_OpEnumeration(new Array<CIMObjectPath>(en));
   }
   catch (CIMException &e) {
      DDD(cout<<"### exception: mbAssociatorsNames - code: "<<e.getCode()<<" msg: "<<e.getMessage()<<std::endl);
      if (rc) CMSetStatus(rc,(CMPIrc)e.getCode());
   }
   if (rc) CMSetStatus(rc,CMPI_RC_ERR_FAILED);
   return NULL;
}

static CMPIEnumeration* mbReferences(CMPIBroker *mb, CMPIContext *ctx,
                 CMPIObjectPath *cop,  char *resultClass, char *role ,
		 char **properties, CMPIStatus *rc) {
   DDD(cout<<"--- mbReferences()"<<std::endl);
   CMPIFlags flgs=ctx->ft->getEntry(ctx,CMPIInvocationFlags,NULL).value.uint32;
   CIMPropertyList *props=getList(properties);
   CIMObjectPath qop(String::EMPTY,CIMNamespaceName(),
                     CM_ObjectPath(cop)->getClassName(),
		     CM_ObjectPath(cop)->getKeyBindings());
 
   AutoMutex mtx(&((CMPI_Broker*)mb)->mtx);
   try {
      Array<CIMObject> const &en=CM_CIMOM(mb)->references(
		  OperationContext(*CM_Context(ctx)),
		  CM_ObjectPath(cop)->getNameSpace(),
		  qop, 
		  resultClass ? CIMName(resultClass) : CIMName(),
		  role ? String(role) : String::EMPTY,
		  CM_IncludeQualifiers(flgs),
		  CM_ClassOrigin(flgs),
		  *props);
      if (rc) CMSetStatus(rc,CMPI_RC_OK);
      delete props;
      return new CMPI_ObjEnumeration(new Array<CIMObject>(en));
   }
   catch (CIMException &e) {
      DDD(cout<<"### exception: mbReferences - code: "<<e.getCode()<<" msg: "<<e.getMessage()<<std::endl);
      if (rc) CMSetStatus(rc,(CMPIrc)e.getCode());
   }
   if (rc) CMSetStatus(rc,CMPI_RC_ERR_FAILED);
   delete props;
   return NULL;
}

static CMPIEnumeration* mbReferenceNames(CMPIBroker *mb, CMPIContext *ctx,
                 CMPIObjectPath *cop, char *resultClass, char *role,
                 CMPIStatus *rc) {
   DDD(cout<<"--- mbReferencesNames()"<<std::endl);
   CIMObjectPath qop(String::EMPTY,CIMNamespaceName(),
                     CM_ObjectPath(cop)->getClassName(),
		     CM_ObjectPath(cop)->getKeyBindings());

   AutoMutex mtx(&((CMPI_Broker*)mb)->mtx);
   try {
      Array<CIMObjectPath> const &en=CM_CIMOM(mb)->referenceNames(
		  OperationContext(*CM_Context(ctx)),
		  CM_ObjectPath(cop)->getNameSpace(),
		  qop, 
		  resultClass ? CIMName(resultClass) : CIMName(),
		  role ? String(role) : String::EMPTY);
      if (rc) CMSetStatus(rc,CMPI_RC_OK);
      return new CMPI_OpEnumeration(new Array<CIMObjectPath>(en));
   }
   catch (CIMException &e) {
      DDD(cout<<"### exception: mbReferencesNames - code: "<<e.getCode()<<" msg: "<<e.getMessage()<<std::endl);
      if (rc) CMSetStatus(rc,(CMPIrc)e.getCode());
   }
   if (rc) CMSetStatus(rc,CMPI_RC_ERR_FAILED);
   return NULL;
}

static CMPIData mbInvokeMethod(CMPIBroker *mb, CMPIContext *ctx,
                 CMPIObjectPath *cop, char *method, CMPIArgs *in, CMPIArgs *out,
		 CMPIStatus *rc) {
   CMPIData data={0,0,{0}};
   if (rc) CMSetStatus(rc,CMPI_RC_ERR_NOT_SUPPORTED);
   return data;
}

static CMPIStatus mbSetProperty(CMPIBroker *mb, CMPIContext *ctx,
                 CMPIObjectPath *cop, char *name, CMPIValue *val,
                 CMPIType type) {
   DDD(cout<<"--- mbSetProperty()"<<std::endl);
   CMPIrc rc;
   CIMValue v=value2CIMValue(val,type,&rc);

   AutoMutex mtx(&((CMPI_Broker*)mb)->mtx);
   try {
      CM_CIMOM(mb)->setProperty(
		  OperationContext(*CM_Context(ctx)),
		  CM_ObjectPath(cop)->getNameSpace(),
		  *CM_ObjectPath(cop),
		  String(name),
		  v);
      CMReturn(CMPI_RC_OK);
   }
   catch (CIMException &e) {
      DDD(cout<<"### exception: mbSetProperty - code: "<<e.getCode()<<" msg: "<<e.getMessage()<<std::endl);
      CMReturn((CMPIrc)e.getCode());
   }
   CMReturn(CMPI_RC_ERR_FAILED);
}

static CMPIData mbGetProperty(CMPIBroker *mb, CMPIContext *ctx,
                 CMPIObjectPath *cop,char *name, CMPIStatus *rc) {
   DDD(cout<<"--- mbGetProperty()"<<std::endl);
   CMPIData data={0,0,{0}};

   AutoMutex mtx(&((CMPI_Broker*)mb)->mtx);
   try {
      CIMValue v=CM_CIMOM(mb)->getProperty(
		  OperationContext(*CM_Context(ctx)),
		  CM_ObjectPath(cop)->getNameSpace(),
		  *CM_ObjectPath(cop),
		  String(name));
      CIMType vType=v.getType();
      CMPIType t=type2CMPIType(vType,v.isArray());
      value2CMPIData(v,t,&data);
      if (rc) CMSetStatus(rc,CMPI_RC_OK);
      return data;
   }
   catch (CIMException &e) {
      DDD(cout<<"### exception: mbGetProperty - code: "<<e.getCode()<<" msg: "<<e.getMessage()<<std::endl);
      if (rc) CMSetStatus(rc,(CMPIrc)e.getCode());
   }
   if (rc) CMSetStatus(rc,CMPI_RC_ERR_FAILED);
   return data;
}

static CMPIContext* mbPrepareAttachThread(CMPIBroker* mb, CMPIContext* eCtx) {
   DDD(cout<<"--- mbPrepareAttachThread()"<<std::endl);
   OperationContext *ctx=(OperationContext*)((CMPI_Context*)eCtx)->ctx;
   OperationContext nctx=*ctx;
   CMPIContext* neCtx=new CMPI_Context(*(new OperationContext(nctx)));
   CMPIString *name;
   for (int i=0,s=CMPI_Args_Ftab->getArgCount((CMPIArgs*)eCtx,NULL); i<s; i++) {
      CMPIData data=CMPI_Args_Ftab->getArgAt((CMPIArgs*)eCtx,i,&name,NULL);
      CMPI_Args_Ftab->addArg((CMPIArgs*)neCtx,CMGetCharPtr(name),&data.value,data.type);
   }
  return neCtx;
}

static CMPIStatus mbAttachThread(CMPIBroker* mb, CMPIContext* eCtx) {
    DDD(cout<<"--- mbAttachThread()"<<std::endl);
    ((CMPI_Context*)eCtx)->thr=new CMPI_ThreadContext(mb,eCtx);
    CMReturn(CMPI_RC_OK);
}

static CMPIStatus mbDetachThread(CMPIBroker* mb, CMPIContext* eCtx) {
   DDD(cout<<"--- mbDetachThread()"<<std::endl);
   delete ((CMPI_Context*)eCtx)->thr;
   CMReturn(CMPI_RC_OK);
}

static CMPIStatus mbDeliverIndication(CMPIBroker* eMb, CMPIContext* eCtx,
             char* ns, CMPIInstance* ind) {
   DDD(cout<<"--- mbDeliverIndication()"<<std::endl);
   CMPI_Broker *mb=(CMPI_Broker*)eMb;
   CMPIProviderManager::indProvRecord *prec;
   
   if (CMPIProviderManager::provTab.lookup(mb->name,prec)) {
      if (prec->enabled) {
         CIMIndication cimIndication(*CM_Instance(ind));
         prec->handler->deliver(cimIndication);
         CMReturn(CMPI_RC_OK);
     }
   }
   CMReturn(CMPI_RC_ERR_FAILED);
}	     

static CMPIBrokerFT broker_FT={
     0, // brokerClassification;
     CMPICurrentVersion,
     "Pegasus",
     mbPrepareAttachThread,
     mbAttachThread,
     mbDetachThread,
     mbDeliverIndication,
     mbEnumInstanceNames,
     mbGetInstance,
     mbCreateInstance,
     mbSetInstance,
     mbDeleteInstance,
     mbExecQuery,
     mbEnumInstances,
     mbAssociators,
     mbAssociatorNames,
     mbReferences,
     mbReferenceNames,
     mbInvokeMethod,
     mbSetProperty,
     mbGetProperty,
};

CMPIBrokerFT *CMPI_Broker_Ftab=& broker_FT;

PEGASUS_NAMESPACE_END


