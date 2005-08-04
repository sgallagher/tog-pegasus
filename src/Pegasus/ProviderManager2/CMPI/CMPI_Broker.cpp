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
// Modified By: Robert Kieninger, kieningr@de.ibm.com, for bug#2642
//
//%/////////////////////////////////////////////////////////////////////////////

#include "CMPI_Version.h"

#include "CMPI_Broker.h"
#include "CMPI_Object.h"
#include "CMPI_ContextArgs.h"
#include "CMPI_Enumeration.h"
#include "CMPI_Value.h"
#include "CMPIProviderManager.h"
#include "CMPI_String.h"

#include <Pegasus/Common/CIMName.h>
#include <Pegasus/Common/CIMPropertyList.h>
#include <Pegasus/Provider/CIMOMHandle.h>
#include <Pegasus/Common/CIMValue.h>
#include <Pegasus/Common/CIMType.h>


PEGASUS_USING_STD;
PEGASUS_NAMESPACE_BEGIN

#define DDD(X)   if (_cmpi_trace) X;

extern int _cmpi_trace;

static CIMPropertyList getList(const char** l) {
  CIMPropertyList pl;
  if (l) {
    Array<CIMName> n;
    while (*l) {
      n.append(*l++);
    }
    pl.set(n);
  }
  return pl;
}

CIMClass* mbGetClass(const CMPIBroker *mb, const CIMObjectPath &cop) {
   DDD(cout<<"--- mbGetClass()"<<endl);
   mb=CM_BROKER;
   CMPI_Broker *xBroker=(CMPI_Broker*)mb;
   String clsId=cop.getNameSpace().getString()+":"+cop.getClassName().getString();
   CIMClass *ccp;

   AutoMutex mtx(((CMPI_Broker*)mb)->mtx);
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
   catch (const CIMException &e) {
      DDD(cout<<"### exception: mbGetClass - code: "<<e.getCode()<<" msg: "<<e.getMessage()<<endl);
   }
   return NULL;
}

extern "C" {

   static CMPIInstance* mbGetInstance(const CMPIBroker *mb, const CMPIContext *ctx,
                  const CMPIObjectPath *cop, const char **properties, CMPIStatus *rc) {
      DDD(cout<<"--- mbGetInstance()"<<endl);
      mb=CM_BROKER;
      CMPIFlags flgs=ctx->ft->getEntry(ctx,CMPIInvocationFlags,NULL).value.uint32;
      const CIMPropertyList props=getList(properties);
      CIMObjectPath qop(String::EMPTY,CIMNamespaceName(),
                        CM_ObjectPath(cop)->getClassName(),
            CM_ObjectPath(cop)->getKeyBindings());

      AutoMutex mtx(((CMPI_Broker*)mb)->mtx);
      try {
         CIMInstance ci=CM_CIMOM(mb)->getInstance(
                     OperationContext(*CM_Context(ctx)),
         CM_ObjectPath(cop)->getNameSpace(),
         qop, //*CM_ObjectPath(cop),
         CM_LocalOnly(flgs),
         CM_IncludeQualifiers(flgs),
         CM_ClassOrigin(flgs),
         props);
         ci.setPath(*CM_ObjectPath(cop));
         if (rc) CMSetStatus(rc,CMPI_RC_OK);
         return (CMPIInstance*)new CMPI_Object(new CIMInstance(ci));
      }
      catch (const CIMException &e) {
         DDD(cout<<"### exception: mbGetInstance - code: "<<e.getCode()<<" msg: "<<e.getMessage()<<endl);
         if (rc) CMSetStatusWithString(rc,(CMPIrc)e.getCode(),
            (CMPIString*)string2CMPIString(e.getMessage()));
         return NULL;
      }
      if (rc) CMSetStatusWithChars(mb,rc,CMPI_RC_ERROR,"Internal error - CMPIBoker.cpp-0");
      return NULL;
   }

   static CMPIObjectPath* mbCreateInstance(const CMPIBroker *mb, const CMPIContext *ctx,
                  const CMPIObjectPath *cop, const CMPIInstance *ci, CMPIStatus *rc) {
      DDD(cout<<"--- mbCreateInstance()"<<endl);
      mb=CM_BROKER;

      AutoMutex mtx(((CMPI_Broker*)mb)->mtx);
      try {
         CIMObjectPath ncop=CM_CIMOM(mb)->createInstance(
                     OperationContext(*CM_Context(ctx)),
         CM_ObjectPath(cop)->getNameSpace(),
                     *CM_Instance(ci));
         if (rc) CMSetStatus(rc,CMPI_RC_OK);
         return (CMPIObjectPath*)new CMPI_Object(new CIMObjectPath(ncop));
      }
      catch (const CIMException &e) {
         DDD(cout<<"### exception: mbCreateInstance - code: "<<e.getCode()<<" msg: "<<e.getMessage()<<endl);
         if (rc) CMSetStatusWithString(rc,(CMPIrc)e.getCode(),
            (CMPIString*)string2CMPIString(e.getMessage()));
         return NULL;
      }
      if (rc) CMSetStatusWithChars(mb,rc,CMPI_RC_ERROR,"Internal error - CMPIBoker.cpp-1");
      return NULL;
   }

   static CMPIStatus mbModifyInstance(const CMPIBroker *mb, const CMPIContext *ctx,
         const CMPIObjectPath *cop, const CMPIInstance *ci,const  char ** properties) {
      DDD(cout<<"--- mbSetInstance()"<<endl);
      mb=CM_BROKER;
      CMPIFlags flgs=ctx->ft->getEntry(ctx,CMPIInvocationFlags,NULL).value.uint32;
      const CIMPropertyList props=getList(properties);

      AutoMutex mtx(((CMPI_Broker*)mb)->mtx);
      try {
   	 CIMInstance cmi(*CM_Instance(ci));
	 cmi.setPath(*CM_ObjectPath(cop));
         CM_CIMOM(mb)->modifyInstance(
                     OperationContext(*CM_Context(ctx)),
		     CM_ObjectPath(cop)->getNameSpace(),
                     cmi,
         CM_IncludeQualifiers(flgs),
         props);
         CMReturn(CMPI_RC_OK);
      }
      catch (const CIMException &e) {
         DDD(cout<<"### exception: mbSetInstance - code: "<<e.getCode()<<" msg: "<<e.getMessage()<<endl);
         CMReturnWithString((CMPIrc)e.getCode(),
            (CMPIString*)string2CMPIString(e.getMessage()));
      }
      CMReturnWithChars(mb,CMPI_RC_ERROR,"Internal error - CMPIBoker.cpp-2");
   }

   static CMPIStatus mbDeleteInstance (const CMPIBroker *mb, const CMPIContext *ctx,
                  const CMPIObjectPath *cop) {
      DDD(cout<<"--- mbDeleteInstance()"<<endl);
      mb=CM_BROKER;
      CIMObjectPath qop(String::EMPTY,CIMNamespaceName(),
                        CM_ObjectPath(cop)->getClassName(),
            CM_ObjectPath(cop)->getKeyBindings());

      AutoMutex mtx(((CMPI_Broker*)mb)->mtx);
      try {
         CM_CIMOM(mb)->deleteInstance(
                     OperationContext(*CM_Context(ctx)),
         CM_ObjectPath(cop)->getNameSpace(),
         qop); //*CM_ObjectPath(cop));
         CMReturn(CMPI_RC_OK);
      }
      catch (const CIMException &e) {
         DDD(cout<<"### exception: mbDeleteInstance - code: "<<e.getCode()<<" msg: "<<e.getMessage()<<endl);
         CMReturnWithString((CMPIrc)e.getCode(),
            (CMPIString*)string2CMPIString(e.getMessage()));
      }
      CMReturnWithChars(mb,CMPI_RC_ERROR,"Internal error - CMPIBoker.cpp-3");
   }

   static CMPIEnumeration* mbExecQuery(const CMPIBroker *mb, 
				       const CMPIContext *ctx,
				       const CMPIObjectPath *cop, 
				       const char *query, const char *lang, CMPIStatus *rc) {
      DDD(cout<<"--- mbExecQuery()"<<endl);
      mb=CM_BROKER;

      AutoMutex mtx(((CMPI_Broker*)mb)->mtx);
      try {
         Array<CIMObject> const &en=CM_CIMOM(mb)->execQuery(
                     OperationContext(*CM_Context(ctx)),
         CM_ObjectPath(cop)->getNameSpace(),
         String(query),
         String(lang));
         if (rc) CMSetStatus(rc,CMPI_RC_OK);
         CMPI_Object *obj =
             new CMPI_Object(new CMPI_ObjEnumeration(new Array<CIMObject>(en)));
         return (CMPI_ObjEnumeration *)obj->getHdl();

      }
      catch (const CIMException &e) {
         DDD(cout<<"### exception: mbExecQuery - code: "<<e.getCode()<<" msg: "<<e.getMessage()<<endl);
         if (rc) CMSetStatusWithString(rc,(CMPIrc)e.getCode(),
            (CMPIString*)string2CMPIString(e.getMessage()));
      }
      if (rc) CMSetStatusWithChars(mb,rc,CMPI_RC_ERR_FAILED,"Internal error - CMPIBoker.cpp-4");
      return NULL;
   }

   static CMPIEnumeration* mbEnumInstances(const CMPIBroker *mb, const CMPIContext *ctx,
                  const CMPIObjectPath *cop, const char **properties, CMPIStatus *rc) {
      DDD(cout<<"--- mbEnumInstances()"<<endl);
      mb=CM_BROKER;

      CMPIFlags flgs=ctx->ft->getEntry(ctx,CMPIInvocationFlags,NULL).value.uint32;
      const CIMPropertyList props=getList(properties);

      AutoMutex mtx(((CMPI_Broker*)mb)->mtx);
      try {
         Array<CIMInstance> const &en=CM_CIMOM(mb)->enumerateInstances(
                     OperationContext(*CM_Context(ctx)),
         CM_ObjectPath(cop)->getNameSpace(),
         CM_ObjectPath(cop)->getClassName(),
         CM_DeepInheritance(flgs),
         CM_LocalOnly(flgs),
         CM_IncludeQualifiers(flgs),
         CM_ClassOrigin(flgs),
         props);
         if (rc) CMSetStatus(rc,CMPI_RC_OK);
         CMPI_Object *obj =
             new CMPI_Object(new CMPI_InstEnumeration(new Array<CIMInstance>(en)));
         return (CMPI_InstEnumeration*)obj->getHdl();

      }
      catch (const CIMException &e) {
         DDD(cout<<"### exception: mbEnumInstances - code: "<<e.getCode()<<" msg: "<<e.getMessage()<<endl);
         if (rc) CMSetStatusWithString(rc,(CMPIrc)e.getCode(),
            (CMPIString*)string2CMPIString(e.getMessage()));
         return NULL;
      }
      if (rc) CMSetStatusWithChars(mb,rc,CMPI_RC_ERROR,"Internal error - CMPIBoker.cpp-5");
      return NULL;
   }

   static CMPIEnumeration* mbEnumInstanceNames(const CMPIBroker *mb, const CMPIContext *ctx,
                  const CMPIObjectPath *cop, CMPIStatus *rc) {
      DDD(cout<<"--- mbEnumInstanceNames()"<<endl);
      mb=CM_BROKER;

      AutoMutex mtx(((CMPI_Broker*)mb)->mtx);
   try {
         Array<CIMObjectPath> const &en=CM_CIMOM(mb)->enumerateInstanceNames(
                     OperationContext(*CM_Context(ctx)),
         CM_ObjectPath(cop)->getNameSpace(),
         CM_ObjectPath(cop)->getClassName());
         if (rc) CMSetStatus(rc,CMPI_RC_OK);
         CMPI_Object *obj =
             new CMPI_Object(new CMPI_OpEnumeration(new Array<CIMObjectPath>(en)));
         return (CMPI_OpEnumeration *)obj->getHdl();

      }
      catch (const CIMException &e) {
         DDD(cout<<"### exception: mbEnumInstances - code: "<<e.getCode()<<" msg: "<<e.getMessage()<<endl);
         if (rc) CMSetStatusWithString(rc,(CMPIrc)e.getCode(),
            (CMPIString*)string2CMPIString(e.getMessage()));
      }
      if (rc) CMSetStatusWithChars(mb,rc,CMPI_RC_ERROR,"Internal error - CMPIBoker.cpp-6");
      return NULL;
   }

   static CMPIEnumeration* mbAssociators(const CMPIBroker *mb, const CMPIContext *ctx,
                  const CMPIObjectPath *cop, const char *assocClass, const char *resultClass,
                  const char *role, const char *resultRole, const char **properties, CMPIStatus *rc) {
      DDD(cout<<"--- mbAssociators()"<<endl);
      mb=CM_BROKER;
      CMPIFlags flgs=ctx->ft->getEntry(ctx,CMPIInvocationFlags,NULL).value.uint32;
      const CIMPropertyList props=getList(properties);
      CIMObjectPath qop(String::EMPTY,CIMNamespaceName(),
                        CM_ObjectPath(cop)->getClassName(),
            CM_ObjectPath(cop)->getKeyBindings());

      AutoMutex mtx(((CMPI_Broker*)mb)->mtx);
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
         props);
         if (rc) CMSetStatus(rc,CMPI_RC_OK);
         CMPI_Object *obj =
             new CMPI_Object(new CMPI_ObjEnumeration(new Array<CIMObject>(en)));
         return (CMPI_ObjEnumeration *)obj->getHdl();

      }
      catch (const CIMException &e) {
         DDD(cout<<"### exception: mbAssociators - code: "<<e.getCode()<<" msg: "<<e.getMessage()<<endl);
         if (rc) CMSetStatusWithString(rc,(CMPIrc)e.getCode(),
            (CMPIString*)string2CMPIString(e.getMessage()));
         return NULL;
      }
      if (rc) CMSetStatusWithChars(mb,rc,CMPI_RC_ERROR,"Internal error - CMPIBoker.cpp-7");
      return NULL;
   }

   static CMPIEnumeration* mbAssociatorNames(const CMPIBroker *mb, 
					     const CMPIContext *ctx,
					     const CMPIObjectPath *cop, 
					     const char *assocClass, 
					     const char *resultClass,
         const char *role, const char *resultRole, CMPIStatus *rc) {
      DDD(cout<<"--- mbAssociatorsNames()"<<endl);
      mb=CM_BROKER;
      CIMObjectPath qop(String::EMPTY,CIMNamespaceName(),
                        CM_ObjectPath(cop)->getClassName(),
            CM_ObjectPath(cop)->getKeyBindings());

      AutoMutex mtx(((CMPI_Broker*)mb)->mtx);
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
         CMPI_Object *obj =
             new CMPI_Object(new CMPI_OpEnumeration(new Array<CIMObjectPath>(en)));
         return (CMPI_OpEnumeration *)obj->getHdl();
      }
      catch (const CIMException &e) {
         DDD(cout<<"### exception: mbAssociatorsNames - code: "<<e.getCode()<<" msg: "<<e.getMessage()<<endl);
         if (rc) CMSetStatusWithString(rc,(CMPIrc)e.getCode(),
            (CMPIString*)string2CMPIString(e.getMessage()));
      }
      if (rc) CMSetStatusWithChars(mb,rc,CMPI_RC_ERROR,"Internal error - CMPIBoker.cpp-8");
      return NULL;
   }

   static CMPIEnumeration* mbReferences(const CMPIBroker *mb, const CMPIContext *ctx,
                  const CMPIObjectPath *cop,  const char *resultClass, const char *role ,
         const char **properties, CMPIStatus *rc) {
      DDD(cout<<"--- mbReferences()"<<endl);
      mb=CM_BROKER;
      CMPIFlags flgs=ctx->ft->getEntry(ctx,CMPIInvocationFlags,NULL).value.uint32;
      CIMPropertyList props=getList(properties);
      CIMObjectPath qop(String::EMPTY,CIMNamespaceName(),
                        CM_ObjectPath(cop)->getClassName(),
            CM_ObjectPath(cop)->getKeyBindings());

      AutoMutex mtx(((CMPI_Broker*)mb)->mtx);
      try {
         Array<CIMObject> const &en=CM_CIMOM(mb)->references(
         OperationContext(*CM_Context(ctx)),
         CM_ObjectPath(cop)->getNameSpace(),
         qop,
         resultClass ? CIMName(resultClass) : CIMName(),
         role ? String(role) : String::EMPTY,
         CM_IncludeQualifiers(flgs),
         CM_ClassOrigin(flgs),
         props);
         if (rc) CMSetStatus(rc,CMPI_RC_OK);
         CMPI_Object *obj =
             new CMPI_Object(new CMPI_ObjEnumeration(new Array<CIMObject>(en)));
         return (CMPI_ObjEnumeration *)obj->getHdl();
      }
      catch (const CIMException &e) {
         DDD(cout<<"### exception: mbReferences - code: "<<e.getCode()<<" msg: "<<e.getMessage()<<endl);
         if (rc) CMSetStatusWithString(rc,(CMPIrc)e.getCode(),
            (CMPIString*)string2CMPIString(e.getMessage()));
         return NULL;
      }
      if (rc) CMSetStatusWithChars(mb,rc,CMPI_RC_ERROR,"Internal error - CMPIBoker.cpp-10");
      return NULL;
   }

   static CMPIEnumeration* mbReferenceNames(const CMPIBroker *mb, const CMPIContext *ctx,
                  const CMPIObjectPath *cop, const char *resultClass, const char *role,
                  CMPIStatus *rc) {
      DDD(cout<<"--- mbReferencesNames()"<<endl);
      mb=CM_BROKER;
      CIMObjectPath qop(String::EMPTY,CIMNamespaceName(),
                        CM_ObjectPath(cop)->getClassName(),
            CM_ObjectPath(cop)->getKeyBindings());

      AutoMutex mtx(((CMPI_Broker*)mb)->mtx);
      try {
         Array<CIMObjectPath> const &en=CM_CIMOM(mb)->referenceNames(
         OperationContext(*CM_Context(ctx)),
         CM_ObjectPath(cop)->getNameSpace(),
         qop,
         resultClass ? CIMName(resultClass) : CIMName(),
         role ? String(role) : String::EMPTY);
         if (rc) CMSetStatus(rc,CMPI_RC_OK);
         CMPI_Object *obj =
             new CMPI_Object(new CMPI_OpEnumeration(new Array<CIMObjectPath>(en)));
         return (CMPI_OpEnumeration *)obj->getHdl();
      }
      catch (const CIMException &e) {
         DDD(cout<<"### exception: mbReferencesNames - code: "<<e.getCode()<<" msg: "<<e.getMessage()<<endl);
         if (rc) CMSetStatusWithString(rc,(CMPIrc)e.getCode(),
            (CMPIString*)string2CMPIString(e.getMessage()));
      }
      if (rc) CMSetStatusWithChars(mb,rc,CMPI_RC_ERROR,"Internal error - CMPIBoker.cpp-11");
      return NULL;
   }

   static CMPIData mbInvokeMethod(const CMPIBroker *mb, const CMPIContext *ctx,
                  const CMPIObjectPath *cop, const char *method, const CMPIArgs *in, CMPIArgs *out,
         CMPIStatus *rc) {
      CMPIData data={0,CMPI_nullValue,{0}};
      mb=CM_BROKER;
      if (rc) CMSetStatus(rc,CMPI_RC_ERR_NOT_SUPPORTED);
      return data;
   }

   static CMPIStatus mbSetProperty(const CMPIBroker *mb, const CMPIContext *ctx,
                  const CMPIObjectPath *cop, const char *name, const CMPIValue *val,
                   CMPIType type) {
      DDD(cout<<"--- mbSetProperty()"<<endl);
      mb=CM_BROKER;
      CMPIrc rc;
      CIMValue v=value2CIMValue(val,type,&rc);

      AutoMutex mtx(((CMPI_Broker*)mb)->mtx);
      try {
         CM_CIMOM(mb)->setProperty(
         OperationContext(*CM_Context(ctx)),
         CM_ObjectPath(cop)->getNameSpace(),
         *CM_ObjectPath(cop),
         String(name),
         v);
         CMReturn(CMPI_RC_OK);
      }
      catch (const CIMException &e) {
         DDD(cout<<"### exception: mbSetProperty - code: "<<e.getCode()<<" msg: "<<e.getMessage()<<endl);
         CMReturnWithString((CMPIrc)e.getCode(),
            (CMPIString*)string2CMPIString(e.getMessage()));
      }
      CMReturnWithChars(mb,CMPI_RC_ERROR,"Internal error - CMPIBoker.cpp-12");
   }

   static CMPIData mbGetProperty(const CMPIBroker *mb, const CMPIContext *ctx,
                  const CMPIObjectPath *cop, const char *name, CMPIStatus *rc) {
      DDD(cout<<"--- mbGetProperty()"<<endl);
      mb=CM_BROKER;
      CMPIData data={0,CMPI_nullValue,{0}};

      AutoMutex mtx(((CMPI_Broker*)mb)->mtx);
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
      catch (const CIMException &e) {
         DDD(cout<<"### exception: mbGetProperty - code: "<<e.getCode()<<" msg: "<<e.getMessage()<<endl);
         if (rc) CMSetStatus(rc,(CMPIrc)e.getCode());
      }
      if (rc) CMSetStatus(rc,CMPI_RC_ERR_FAILED);
      return data;
   }

   static CMPIContext* mbPrepareAttachThread(const CMPIBroker* mb, const CMPIContext* eCtx) {
      DDD(cout<<"--- mbPrepareAttachThread()"<<endl);
      mb=CM_BROKER;
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

   static CMPIStatus mbAttachThread(const CMPIBroker* mb, const CMPIContext* eCtx) {
      DDD(cout<<"--- mbAttachThread()"<<endl);
      ((CMPI_Context*)eCtx)->thr=new CMPI_ThreadContext(mb,eCtx);
      CMReturn(CMPI_RC_OK);
   }

   static CMPIStatus mbDetachThread(const CMPIBroker* mb, const CMPIContext* eCtx) {
      DDD(cout<<"--- mbDetachThread()"<<endl);
      mb=CM_BROKER;  
      CMPI_Context *neCtx = (CMPI_Context *)eCtx;
      delete neCtx->thr;
      // Delete also CMPIContext
      delete neCtx; 
      CMReturn(CMPI_RC_OK);
   }

   static CMPIStatus mbDeliverIndication(const CMPIBroker* eMb, const CMPIContext* ctx,
               const char *ns, const CMPIInstance* ind) {
      DDD(cout<<"--- mbDeliverIndication()"<<endl);
      eMb=CM_BROKER;
      CMPI_Broker *mb=(CMPI_Broker*)eMb;
      CMPIProviderManager::indProvRecord *prec;
      OperationContext* context=CM_Context(ctx);

      if (CMPIProviderManager::provTab.lookup(mb->name,prec)) {
         if (prec->enabled) {
            try {
               context->get(SubscriptionInstanceNamesContainer::NAME);
            }
            catch (const Exception &) {
               Array<CIMObjectPath> subscriptionInstanceNames;
               context->insert(SubscriptionInstanceNamesContainer(subscriptionInstanceNames));
            }
            CIMIndication cimIndication(*CM_Instance(ind));
            AutoMutex mtx(((CMPI_Broker*)mb)->mtx);
            try {
               prec->handler->deliver(
                  *context,
   //               OperationContext(*CM_Context(ctx)),
                  cimIndication);
            CMReturn(CMPI_RC_OK);
      }
            catch (const CIMException &e) {
               DDD(cout<<"### exception: mbSetProperty - code: "<<e.getCode()<<" msg: "<<e.getMessage()<<endl);
               CMReturn((CMPIrc)e.getCode());
            }
         }
      }
      CMReturn(CMPI_RC_ERR_FAILED);
   }

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
     mbModifyInstance,
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


