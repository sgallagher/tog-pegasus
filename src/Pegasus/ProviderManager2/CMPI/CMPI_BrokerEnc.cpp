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

#include "CMPI_Version.h"

#include "CMPI_Object.h"
#include "CMPI_Broker.h"
#include "CMPI_Ftabs.h"
#include "CMPI_String.h"
#include "CMPI_SelectExp.h"

#include <Pegasus/Common/CIMName.h>
#include <Pegasus/Common/CIMPropertyList.h>
#if defined (CMPI_VER_85)
#include <Pegasus/Common/MessageLoader.h>
#endif
#include <Pegasus/Provider/CIMOMHandle.h>
#include <Pegasus/WQL/WQLSelectStatement.h>
#include <Pegasus/WQL/WQLParser.h>

#include <stdarg.h>
#include <string.h>

PEGASUS_USING_STD;
PEGASUS_NAMESPACE_BEGIN

// Factory section

static CMPIInstance* mbEncNewInstance(CMPIBroker* mb, CMPIObjectPath* eCop,
                                      CMPIStatus *rc) {
   CIMObjectPath* cop=(CIMObjectPath*)eCop->hdl;
//   cout<<"--- mbEncNewInstance() "<<cop->getClassName()<<"-"<<cop->getNameSpace()<<endl;
   CIMClass *cls=mbGetClass(mb,*cop);
   CIMInstance *ci=NULL;

   if (cls) {
      ci=new CIMInstance(cop->getClassName());
       CMPIContext *ctx=CMPI_ThreadContext::getContext();
       CMPIFlags flgs=ctx->ft->getEntry(ctx,CMPIInvocationFlags,rc).value.uint32;
       if ((flgs & CMPI_FLAG_IncludeQualifiers)!=0) {
          for (int i=0,m=cls->getQualifierCount(); i<m; i++)
             ci->addQualifier(cls->getQualifier(i).clone());
          for (int i=0,m=cls->getPropertyCount(); i<m; i++)
             ci->addProperty(cls->getProperty(i).clone());
       }
       else {
         for (int i=0,m=cls->getPropertyCount(); i<m; i++) {
            CIMConstProperty p=cls->getProperty(i);
            ci->addProperty(CIMProperty(
	        p.getName(),p.getValue(),
	        p.getArraySize(),p.getReferenceClassName()));
         }
      }
   }
   else {
      if (rc) CMSetStatus(rc,CMPI_RC_ERR_NOT_FOUND);
      return NULL;
   }

   ci->setPath(*cop);
   CMPIInstance* neInst=reinterpret_cast<CMPIInstance*>(new CMPI_Object(ci));
   if (rc) CMSetStatus(rc,CMPI_RC_OK);
   return neInst;
}

static inline CIMNamespaceName NameSpaceName(const char *ns) {
   if (ns==NULL) return CIMNamespaceName();
   return CIMNamespaceName(ns);
}

static inline CIMName Name(const char *n) {
   if (n==NULL) return CIMName();
   return CIMName(n);
}

static CMPIObjectPath* mbEncNewObjectPath(CMPIBroker* mb, const char *ns, const char *cls,
                  CMPIStatus *rc) {
//   cout<<"--- mbEncNewObjectPath() "<<ns<<"-"<<cls<<endl;
   Array<CIMKeyBinding> keyBindings;
   String host;
   CIMName className=Name(cls);
   CIMNamespaceName nameSpace=NameSpaceName(ns);
   CIMObjectPath *cop=new CIMObjectPath(host,nameSpace,className,keyBindings);
   CMPIObjectPath *nePath=reinterpret_cast<CMPIObjectPath*>(new CMPI_Object(cop));
   if (rc) CMSetStatus(rc,CMPI_RC_OK);
   return nePath;
}

static CMPIArgs* mbEncNewArgs(CMPIBroker* mb, CMPIStatus *rc) {
   if (rc) CMSetStatus(rc,CMPI_RC_OK);
   return reinterpret_cast<CMPIArgs*>(new CMPI_Object(new Array<CIMParamValue>()));
}

static CMPIString* mbEncNewString(CMPIBroker* mb, const char *cStr, CMPIStatus *rc) {
   if (rc) CMSetStatus(rc,CMPI_RC_OK);
   return reinterpret_cast<CMPIString*>(new CMPI_Object(cStr));
}

CMPIString* mbIntNewString(const char *s) {
   return mbEncNewString(NULL,s,NULL);
}

static CMPIArray* mbEncNewArray(CMPIBroker* mb, CMPICount count, CMPIType type,
                                CMPIStatus *rc) {
   if (rc) CMSetStatus(rc,CMPI_RC_OK);
   CMPIData *dta=new CMPIData[count+1];
   dta->type=type;
   dta->value.uint32=count;
   for (unsigned int i=1; i<=count; i++) {
      dta[i].type=type;
      dta[i].state=CMPI_nullValue;
      dta[i].value.uint64=0;
   }
   return reinterpret_cast<CMPIArray*>(new CMPI_Object(dta));
}

extern CMPIDateTime *newDateTime();

static CMPIDateTime* mbEncNewDateTime(CMPIBroker* mb, CMPIStatus *rc) {
  // cout<<"--- mbEncNewDateTime()"<<endl;
   if (rc) CMSetStatus(rc,CMPI_RC_OK);
   return newDateTime();
}

extern CMPIDateTime *newDateTime(CMPIUint64,CMPIBoolean);

static CMPIDateTime* mbEncNewDateTimeFromBinary(CMPIBroker* mb, CMPIUint64 time,
      CMPIBoolean interval ,CMPIStatus *rc) {
  // cout<<"--- mbEncNewDateTimeFromBinary()"<<endl;
   if (rc) CMSetStatus(rc,CMPI_RC_OK);
   return newDateTime(time,interval);
}

extern CMPIDateTime *newDateTime(char*);

static CMPIDateTime* mbEncNewDateTimeFromString(CMPIBroker* mb, char *t ,CMPIStatus *rc) {
//   cout<<"--- mbEncNewDateTimeFromString()"<<endl;
   if (rc) CMSetStatus(rc,CMPI_RC_OK);
   return newDateTime(t);
}

static CMPIString* mbEncToString(CMPIBroker*,void *o, CMPIStatus *rc) {
   CMPI_Object *obj=(CMPI_Object*)o;
   String str;
   char msg[128];
   if (obj==NULL) {
      sprintf(msg,"** Null object ptr (%p) **",o);
      if (rc) CMSetStatus(rc,CMPI_RC_ERR_FAILED);
      return reinterpret_cast<CMPIString*>(new CMPI_Object(msg));
   }

   if (obj->getHdl()==NULL) {
      sprintf(msg,"** Null object hdl (%p) **",o);
      if (rc) CMSetStatus(rc,CMPI_RC_ERR_FAILED);
      return reinterpret_cast<CMPIString*>(new CMPI_Object(msg));
   }
   
   if (obj->getFtab()==(void*)CMPI_Instance_Ftab ||
       obj->getFtab()==(void*)CMPI_InstanceOnStack_Ftab) {
      sprintf(msg,"** Object not supported (%p) **",o);
      if (rc) CMSetStatus(rc,CMPI_RC_ERR_FAILED);
      return reinterpret_cast<CMPIString*>(new CMPI_Object(msg));
      // str=((CIMInstance*)obj->hdl)->toString();
   }
   else if (obj->getFtab()==(void*)CMPI_ObjectPath_Ftab ||
       obj->getFtab()==(void*)CMPI_ObjectPathOnStack_Ftab) {
       str=((CIMObjectPath*)obj->getHdl())->toString();
   }
   /* else if (obj->ftab==(void*)CMPI_SelectExp_Ftab ||
       obj->ftab==(void*)CMPI_SelectCondDoc_Ftab ||
       obj->ftab==(void*)CMPI_SelectCondCod_Ftab ||
       obj->ftab==(void*)CMPI_SubCond_Ftab ||
       obj->ftab==(void*)CMPI_Predicate_Ftab) {
      sprintf(msg,"** Object not supported (%p) **",o);
      if (rc) CMSetStatus(rc,CMPI_RC_ERR_FAILED);
      return (CMPIString*) new CMPI_Object(msg);
   } */
   else {
      sprintf(msg,"** Object not recognized (%p) **",o);
      if (rc) CMSetStatus(rc,CMPI_RC_ERR_FAILED);
      return reinterpret_cast<CMPIString*>(new CMPI_Object(msg));
   }

   sprintf(msg,"%p: ",o);
   return reinterpret_cast<CMPIString*>(new CMPI_Object(String(msg)+str));
}

static CMPIBoolean mbEncClassPathIsA(CMPIBroker *mb, CMPIObjectPath *eCp, const char *type, CMPIStatus *rc) {
   CIMObjectPath* cop=(CIMObjectPath*)eCp->hdl;
   const CIMName tcn(type);

   if (tcn==cop->getClassName()) return 1;

   CIMClass *cc=mbGetClass(mb,*cop);
   if (cc==NULL) return 0;
   CIMObjectPath  scp(*cop);
   scp.setClassName(cc->getSuperClassName());

   for (; !scp.getClassName().isNull(); ) {
      cc=mbGetClass(mb,scp);
      if (cc==NULL) return 0;
      if (cc->getClassName()==tcn) return 1;
      scp.setClassName(cc->getSuperClassName());
   };
   return 0;
}

static CMPIBoolean mbEncIsOfType(CMPIBroker *mb, void *o, const char *type, CMPIStatus *rc) {
   CMPI_Object *obj=(CMPI_Object*)o;
   char msg[128];
   if (obj==NULL) {
      sprintf(msg,"** Null object ptr (%p) **",o);
      if (rc) { CMSetStatusWithChars(mb,rc,CMPI_RC_ERR_FAILED,msg); }
      return 0;
   }

   if (rc) CMSetStatus(rc,CMPI_RC_OK);

   if (obj->getFtab()==(void*)CMPI_Instance_Ftab &&
         strcmp(type,"CMPIInstance")==0) return 1;
   if (obj->getFtab()!=(void*)CMPI_ObjectPath_Ftab &&
         strcmp(type,"CMPIObjectPath")==0) return 1;
/* if (obj->ftab!=(void*)CMPI_SelectExp_Ftab &&
         strcmp(type,"CMPISelectExp")==0) return 1;
   if (obj->ftab!=(void*)CMPI_SelectCondDoc_Ftab &&
         strcmp(type,"CMPISelectCondDoc")==0) return 1;
   if (obj->ftab!=(void*)CMPI_SelectCondCod_Ftab &&
         strcmp(type,"CMPISelectCondCod")==0) return 1;
   if (obj->ftab!=(void*)CMPI_SubCond_Ftab &&
         strcmp(type,"CMPISubCond")==0) return 1;
   if (obj->ftab!=(void*)CMPI_Predicate_Ftab &&
         strcmp(type,"CMPIPredicate")==0) return 1;
*/ if (obj->getFtab()!=(void*)CMPI_Array_Ftab &&
         strcmp(type,"CMPIArray")==0) return 1;

   sprintf(msg,"** Object not recognized (%p) **",o);
   if (rc) { CMSetStatusWithChars(mb,rc,CMPI_RC_ERR_FAILED,msg); }
   return 0;
}

static CMPIString* mbEncGetType(CMPIBroker *mb, void* o, CMPIStatus *rc) {
   CMPI_Object *obj=(CMPI_Object*)o;
   char msg[128];
   if (obj==NULL) {
      sprintf(msg,"** Null object ptr (%p) **",o);
      if (rc) { CMSetStatusWithChars(mb,rc,CMPI_RC_ERR_FAILED,msg); }
      return 0;
   }

   if (rc) CMSetStatus(rc,CMPI_RC_OK);

   if (obj->getFtab()==(void*)CMPI_Instance_Ftab)
         return mb->eft->newString(mb,"CMPIInstance",rc);
   if (obj->getFtab()!=(void*)CMPI_ObjectPath_Ftab)
         return mb->eft->newString(mb,"CMPIObjectPath",rc);
/* if (obj->ftab!=(void*)CMPI_SelectExp_Ftab)
         return mb->eft->newString(mb,"CMPISelectExp",rc);
   if (obj->ftab!=(void*)CMPI_SelectCondDoc_Ftab)
         return mb->eft->newString(mb,"CMPISelectCondDo",rc);
   if (obj->ftab!=(void*)CMPI_SelectCondCod_Ftab)
         return mb->eft->newString(mb,"CMPISelectCondCod",rc);
   if (obj->ftab!=(void*)CMPI_SubCond_Ftab)
         return mb->eft->newString(mb,"CMPISubCond",rc);
   if (obj->ftab!=(void*)CMPI_Predicate_Ftab)
       return mb->eft->newString(mb,"CMPIPredicate",rc);
*/ if (obj->getFtab()!=(void*)CMPI_Array_Ftab)
         return mb->eft->newString(mb,"CMPIArray",rc);

   sprintf(msg,"** Object not recognized (%p) **",o);
   if (rc) { CMSetStatusWithChars(mb,rc,CMPI_RC_ERR_FAILED,msg); }
   return 0;
}

#if defined (CMPI_VER_85)

#ifdef PEGASUS_PLATFORM_WIN32_IX86_MSVC
#define lloonngg __int64
#else
#define lloonngg long long int
#endif

static Formatter::Arg formatValue(va_list *argptr, CMPIStatus *rc, int *err) {

   CMPIType type=va_arg(*argptr,int);
   if (rc) CMSetStatus(rc,CMPI_RC_OK);

   if (*err) return Formatter::Arg("*failed*");

   switch(type) {
     case CMPI_sint8:
     case CMPI_sint16:
     case CMPI_sint32:
        return Formatter::Arg((int)va_arg(*argptr,int));
     case CMPI_uint8:
     case CMPI_uint16:
     case CMPI_uint32:
        return Formatter::Arg((unsigned int)va_arg(*argptr,unsigned int));
     case CMPI_boolean:
        return Formatter::Arg((Boolean)va_arg(*argptr,int));
     case CMPI_real32:
     case CMPI_real64:
        return Formatter::Arg((double)va_arg(*argptr,double));
     case CMPI_sint64:
        return Formatter::Arg((lloonngg)va_arg(*argptr,lloonngg));
     case CMPI_uint64:
        return Formatter::Arg((unsigned lloonngg)va_arg(*argptr,unsigned lloonngg));
     case CMPI_chars:
        return Formatter::Arg((char*)va_arg(*argptr,char*));
     case CMPI_string: {
        CMPIString *s=va_arg(*argptr,CMPIString*);
        return Formatter::Arg((char*)CMGetCharsPtr(s,NULL));
     }
     default:
        *err=1;
        if (rc) rc->rc=CMPI_RC_ERR_INVALID_PARAMETER;
        return Formatter::Arg("*bad value type*");
   }
}

static CMPIString* mbEncGetMessage(CMPIBroker *mb, const char *msgId, const char *defMsg,
            CMPIStatus* rc, unsigned int count, ...) {
   MessageLoaderParms parms(msgId,defMsg);
   cerr<<"::: mbEncGetMessage() count: "<<count<<endl;
   int err=0;
   if (rc) rc->rc=CMPI_RC_OK;

   if (count>0) {
      va_list argptr;
      va_start(argptr,count);
      for (;;) {
         if (count>0) parms.arg0=formatValue(&argptr,rc,&err);
         else break;
         if (count>1) parms.arg1=formatValue(&argptr,rc,&err);
         else break;
         if (count>2) parms.arg2=formatValue(&argptr,rc,&err);
         else break;
         if (count>3) parms.arg3=formatValue(&argptr,rc,&err);
         else break;
         if (count>4) parms.arg4=formatValue(&argptr,rc,&err);
         else break;
         if (count>5) parms.arg5=formatValue(&argptr,rc,&err);
         else break;
         if (count>6) parms.arg6=formatValue(&argptr,rc,&err);
         else break;
         if (count>7) parms.arg7=formatValue(&argptr,rc,&err);
         else break;
         if (count>8) parms.arg8=formatValue(&argptr,rc,&err);
         else break;
         if (count>9) parms.arg9=formatValue(&argptr,rc,&err);
         break;
      }
      va_end(argptr);
   }
   String nMsg=MessageLoader::getMessage(parms);
   return string2CMPIString(nMsg);
}

#endif

static CMPISelectExp* mbEncNewSelectExp(CMPIBroker* mb, const char *query, const char *lang,
                  CMPIArray** projection, CMPIStatus* st) {
   WQLSelectStatement *stmt=new WQLSelectStatement();
   int exception=1;
   
   if (strcmp(lang,"WQL")!=0) {
      if (st) CMSetStatus(st,CMPI_RC_ERR_QUERY_LANGUAGE_NOT_SUPPORTED);
      return NULL;
   }
   
   try {
      WQLParser::parse(query, *stmt);
      exception=0;
   }
   catch (ParseError& e) {
      if (st) CMSetStatus(st,CMPI_RC_ERR_INVALID_QUERY);
   }
   catch (MissingNullTerminator& e) {
      if (st) CMSetStatus(st,CMPI_RC_ERR_INVALID_QUERY);
   }
   if (exception) {
      delete stmt;
      if (projection) *projection=NULL;
      return NULL;
   }
   
   if (stmt->getAllProperties()) {
      *projection=NULL;
   }
   else {
      *projection=mbEncNewArray(mb,stmt->getSelectPropertyNameCount(),CMPI_chars,NULL);
      for (int i=0,m=stmt->getSelectPropertyNameCount(); i<m; i++) {
         const CIMName &n=stmt->getSelectPropertyName(i);
	 CMSetArrayElementAt(*projection,i,(const char*)n.getString().getCString(),CMPI_chars);
      }
   }
   stmt->hasWhereClause();
   return (CMPISelectExp*) new CMPI_SelectExp(stmt);
}  


static CMPIBrokerEncFT brokerEnc_FT={
     CMPICurrentVersion,
     mbEncNewInstance,
     mbEncNewObjectPath,
     mbEncNewArgs,
     mbEncNewString,
     mbEncNewArray,
     mbEncNewDateTime,
     mbEncNewDateTimeFromBinary,
     mbEncNewDateTimeFromString,
     mbEncNewSelectExp,
     mbEncClassPathIsA,
     mbEncToString,
     mbEncIsOfType,
     mbEncGetType,
#if defined (CMPI_VER_85)     
     mbEncGetMessage,
#endif     
};

CMPIBrokerEncFT *CMPI_BrokerEnc_Ftab=&brokerEnc_FT;


PEGASUS_NAMESPACE_END



