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

#include "CMPI_Object.h"
#include "CMPI_Broker.h"
#include "CMPI_Ftabs.h"
#include "CMPI_String.h"

#include <Pegasus/Common/CIMName.h>
#include <Pegasus/Common/CIMPropertyList.h>
#if defined (CMPI_VER_85)
#include <Pegasus/Common/MessageLoader.h>
#endif
#include <Pegasus/Provider/CIMOMHandle.h>
#include <stdarg.h>

PEGASUS_USING_STD;
PEGASUS_NAMESPACE_BEGIN

// Factory section

static CMPIInstance* mbEncNewInstance(CMPIBroker* mb, CMPIObjectPath* eCop,
                                      CMPIStatus *rc) {
   CIMObjectPath* cop=(CIMObjectPath*)eCop->hdl;
//   cout<<"--- mbEncNewInstance() "<<cop->getClassName()<<"-"<<cop->getNameSpace()<<endl;
   CIMClass *cls=mbGetClass(mb,*cop);
   CIMInstance *ci=new CIMInstance(cop->getClassName());

   if (cls) {
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

   ci->setPath(*cop);
   CMPIInstance* neInst=(CMPIInstance*)new CMPI_Object(ci);
   if (rc) CMSetStatus(rc,CMPI_RC_OK);
   return neInst;
}

static inline CIMNamespaceName NameSpaceName(char *ns) {
   if (ns==NULL) return CIMNamespaceName();
   return CIMNamespaceName(ns);
}

static inline CIMName Name(char *n) {
   if (n==NULL) return CIMName();
   return CIMName(n);
}

static CMPIObjectPath* mbEncNewObjectPath(CMPIBroker* mb, char *ns, char *cls,
                  CMPIStatus *rc) {
//   cout<<"--- mbEncNewObjectPath() "<<ns<<"-"<<cls<<endl;
   Array<CIMKeyBinding> keyBindings;
   String host;
   CIMName className=Name(cls);
   CIMNamespaceName nameSpace=NameSpaceName(ns);
   CIMObjectPath *cop=new CIMObjectPath(host,nameSpace,className,keyBindings);
   CMPIObjectPath *nePath=(CMPIObjectPath*)new CMPI_Object(cop);
   if (rc) CMSetStatus(rc,CMPI_RC_OK);
   return nePath;
}

static CMPIArgs* mbEncNewArgs(CMPIBroker* mb, CMPIStatus *rc) {
   if (rc) CMSetStatus(rc,CMPI_RC_OK);
   return (CMPIArgs*)new CMPI_Object(new Array<CIMParamValue>());
}

static CMPIString* mbEncNewString(CMPIBroker* mb, char *cStr, CMPIStatus *rc) {
   if (rc) CMSetStatus(rc,CMPI_RC_OK);
   return (CMPIString*)new CMPI_Object(cStr);
}

CMPIString* mbIntNewString(char *s) {
   return mbEncNewString(NULL,s,NULL);
}

static CMPIArray* mbEncNewArray(CMPIBroker* mb, CMPICount count, CMPIType type,
                                CMPIStatus *rc) {
   if (rc) CMSetStatus(rc,CMPI_RC_OK);
   CMPIData *dta=new CMPIData[count+1];
   dta->type=type;
   dta->value.uint32=count;
   for (uint i=1; i<=count; i++) {
      dta[i].type=type;
      dta[i].state=CMPI_nullValue;
      dta[i].value.uint64=0;
   }
   return (CMPIArray*)new CMPI_Object(dta);
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
      sprintf(msg,"** Null object ptr (0x%x) **",(int)o);
      if (rc) CMSetStatus(rc,CMPI_RC_ERR_FAILED);
      return (CMPIString*)new CMPI_Object(msg);
   }

   if (obj->getHdl()==NULL) {
      sprintf(msg,"** Null object hdl (0x%x) **",(int)o);
      if (rc) CMSetStatus(rc,CMPI_RC_ERR_FAILED);
      return (CMPIString*)new CMPI_Object(msg);
   }
   
   if (obj->getFtab()==(void*)CMPI_Instance_Ftab ||
       obj->getFtab()==(void*)CMPI_InstanceOnStack_Ftab) {
      sprintf(msg,"** Object not supported (0x%x) **",(int)o);
      if (rc) CMSetStatus(rc,CMPI_RC_ERR_FAILED);
      return (CMPIString*) new CMPI_Object(msg);
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
      sprintf(msg,"** Object not supported (0x%x) **",(int)o);
      if (rc) CMSetStatus(rc,CMPI_RC_ERR_FAILED);
      return (CMPIString*) new CMPI_Object(msg);
   } */
   else {
      sprintf(msg,"** Object not recognized (0x%x) **",(int)o);
      if (rc) CMSetStatus(rc,CMPI_RC_ERR_FAILED);
      return (CMPIString*) new CMPI_Object(msg);
   }

   sprintf(msg,"0x%X: ",(int)o);
   return (CMPIString*) new CMPI_Object(String(msg)+str);
}

CMPIBoolean mbEncClassPathIsA(CMPIBroker *mb, CMPIObjectPath *eCp, char *type, CMPIStatus *rc) {
   CIMObjectPath* cop=(CIMObjectPath*)eCp->hdl;
   const CIMName tcn(type);

   if (tcn==cop->getClassName()) return 1;

   CIMClass *cc=mbGetClass(mb,*cop);
   CIMObjectPath  scp(*cop);
   scp.setClassName(cc->getSuperClassName());

   for (; !scp.getClassName().isNull(); ) {
      cc=mbGetClass(mb,scp);
      if (cc->getClassName()==tcn) return 1;
      scp.setClassName(cc->getSuperClassName());
   };
   return 0;
}

CMPIBoolean mbEncIsOfType(CMPIBroker *mb, void *o, char *type, CMPIStatus *rc) {
   CMPI_Object *obj=(CMPI_Object*)o;
   char msg[128];
   if (obj==NULL) {
      sprintf(msg,"** Null object ptr (0x%x) **",(int)o);
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

   sprintf(msg,"** Object not recognized (0x%x) **",(int)o);
   if (rc) { CMSetStatusWithChars(mb,rc,CMPI_RC_ERR_FAILED,msg); }
   return 0;
}

CMPIString* mbEncGetType(CMPIBroker *mb, void* o, CMPIStatus *rc) {
   CMPI_Object *obj=(CMPI_Object*)o;
   char msg[128];
   if (obj==NULL) {
      sprintf(msg,"** Null object ptr (0x%x) **",(int)o);
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

   sprintf(msg,"** Object not recognized (0x%x) **",(int)o);
   if (rc) { CMSetStatusWithChars(mb,rc,CMPI_RC_ERR_FAILED,msg); }
   return 0;
}

#if defined (CMPI_VER_85)
static Formatter::Arg formatValue(va_list *argptr, CMPIStatus *rc) {

   CMPIValue *val=va_arg(*argptr,CMPIValue*);
   CMPIType type=va_arg(*argptr,int);
   if (rc) CMSetStatus(rc,CMPI_RC_OK);
   
   if ((type & (CMPI_UINT|CMPI_SINT))==CMPI_UINT) {
     CMPIUint64 u64;
      switch (type) {
      case CMPI_uint8:  u64=(CMPIUint64)val->uint8;  break;    
      case CMPI_uint16: u64=(CMPIUint64)val->uint16; break; 
      case CMPI_uint32: u64=(CMPIUint64)val->uint32; break;
      case CMPI_uint64: u64=(CMPIUint64)val->uint64; break;
      }
      return Formatter::Arg(u64);
   }
   else if ((type & (CMPI_UINT|CMPI_SINT))==CMPI_SINT) {
    CMPISint64 s64;
     switch (type) {
      case CMPI_sint8:  s64=(CMPISint64)val->sint8;  break;    
      case CMPI_sint16: s64=(CMPISint64)val->sint16; break; 
      case CMPI_sint32: s64=(CMPISint64)val->sint32; break;
      case CMPI_sint64: s64=(CMPISint64)val->sint64; break;
      }
      return Formatter::Arg(s64);
   } 
   else if (type==CMPI_chars) return Formatter::Arg((const char*)val);
   else if (type==CMPI_string) 
      return Formatter::Arg((const char*)CMGetCharsPtr(val->string,NULL));
   else if (type==CMPI_real32)  return Formatter::Arg((CMPIReal64)val->real32);  
   else if (type==CMPI_real64)  return Formatter::Arg(val->real64);
   else if (type==CMPI_boolean) return Formatter::Arg((Boolean)val->boolean);
    
   if (rc) CMSetStatus(rc,CMPI_RC_ERR_INVALID_PARAMETER);
   return Formatter::Arg((Boolean)0);
}

CMPIString* mbEncGetMessage(CMPIBroker *mb, char *msgId, char *defMsg, 
            CMPIStatus* rc, unsigned int count, ...) {
   MessageLoaderParms parms(msgId,defMsg);  
   cerr<<"::: mbEncGetMessage() count: "<<count<<endl;
   if (count>0) {
      va_list argptr;
      va_start(argptr,count); 
      for (;;) {
         if (count>0) parms.arg0=formatValue(&argptr,rc);
         else break;
         if (count>1) parms.arg1=formatValue(&argptr,rc);
         else break;
         if (count>2) parms.arg2=formatValue(&argptr,rc);
         else break;
         if (count>3) parms.arg3=formatValue(&argptr,rc);
         else break;
         if (count>4) parms.arg4=formatValue(&argptr,rc);
         else break;
         if (count>5) parms.arg5=formatValue(&argptr,rc);
         else break;
         if (count>6) parms.arg6=formatValue(&argptr,rc);
         else break;
         if (count>7) parms.arg7=formatValue(&argptr,rc);
         else break;
         if (count>8) parms.arg8=formatValue(&argptr,rc);
         else break;
         if (count>9) parms.arg9=formatValue(&argptr,rc);
         break;
      }	 
   } 
   String nMsg=MessageLoader::getMessage(parms);
   return string2CMPIString(nMsg); 
}
#endif     

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
     NULL,
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



