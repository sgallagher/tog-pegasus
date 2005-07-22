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
#if defined(CMPI_VER_100)
#include <Pegasus/Common/Logger.h>
#include <Pegasus/Common/TraceComponents.h>
#include <Pegasus/Common/Tracer.h>
#endif

#include <Pegasus/Provider/CIMOMHandle.h>
#include <Pegasus/WQL/WQLSelectStatement.h>
#include <Pegasus/WQL/WQLParser.h>

#include <Pegasus/Provider/CIMOMHandleQueryContext.h>
#include <Pegasus/CQL/CQLSelectStatement.h>
#include <Pegasus/CQL/CQLParser.h>
#include <Pegasus/CQL/CQLChainedIdentifier.h>
#include <Pegasus/Provider/CMPI/cmpi_cql.h>

#include <stdarg.h>
#include <string.h>

PEGASUS_USING_STD;
PEGASUS_NAMESPACE_BEGIN
#define DDD(X)   if (_cmpi_trace) X;

extern int _cmpi_trace;

#ifdef PEGASUS_PLATFORM_WIN32_IX86_MSVC
   #define lloonngg __int64
#else
   #define lloonngg long long int
#endif

// Factory section

static String typeToString(CIMType t) {
   switch (t) {
	   case CIMTYPE_BOOLEAN:   return "boolean";
	   case CIMTYPE_UINT8:     return "uint8";
	   case CIMTYPE_SINT8:     return "sint8";
	   case CIMTYPE_UINT16:    return "uint16";
	   case CIMTYPE_SINT16:    return "sint16";
	   case CIMTYPE_UINT32:    return "uint32";
	   case CIMTYPE_SINT32:    return "sint32";
	   case CIMTYPE_UINT64:    return "sint64";
	   case CIMTYPE_SINT64:    return "sint64";
	   case CIMTYPE_REAL32:    return "real32";
	   case CIMTYPE_REAL64:    return "real64";
	   case CIMTYPE_CHAR16:    return "char16";
	   case CIMTYPE_STRING:    return "string";
	   case CIMTYPE_DATETIME:  return "datetime";
       case CIMTYPE_REFERENCE: return "reference";
       case CIMTYPE_OBJECT:    return "object";
      default: return "???";
   }
}

#define CMPIInstance_str "CMPIInstance"
#define CMPIInstance_str_l 13

#define CMPIObjectPath_str "CMPIObjectPath"
#define CMPIObjectPath_str_l 16

#define CMPIArgs_str "CMPIArgs"
#define CMPIArgs_str_l 8

#define CMPIContext_str "CMPIContext"
#define CMPIContext_str_l 11

#define CMPIResult_str "CMPIResult"
#define CMPIResult_str_l 12

#define CMPIDateTime_str "CMPIDateTime"
#define CMPIDateTime_str_l 12

#define CMPIArray_str "CMPIArray"
#define CMPIArray_str_l 9

#define CMPIString_str "CMPIString"
#define CMPIString_str_l 10

#define CMPISelectExp_str "CMPISelectExp"
#define CMPISelectExp_str_l 13

#define CMPISelectCond_str "CMPISelectCond"
#define CMPISelectCond_str_l 14

#define CMPISubCond_str "CMPISubCond"
#define CMPISubCond_str_l 11

#define CMPIPredicate_str "CMPIPredicate"
#define CMPIPredicate_str_l 13

#define CMPIBroker_str "CMPIBroker"
#define CMPIBroker_str_l 10

#define CMPIEnumeration_str "CMPIEnumeration"
#define CMPIEnumeration_str_l 15



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


static inline CIMNamespaceName NameSpaceName(const char *ns) {

    CIMNamespaceName n;
    if (ns==NULL) return n;
    try  {
       n = CIMNamespaceName(ns);
    } catch (...) 
    {
         // n won't be assigned to anything yet, so it is safe
         // to send it off.
    }
    return n;
}

static inline CIMName Name(const char *n) {
    CIMName name;
    if (n==NULL) return name;
    try {
       name =CIMName(n);
     } catch ( ...)
     {
     }
     return name;
}


extern "C" {

   static CMPIString* mbEncToString(const CMPIBroker*,const void *o, CMPIStatus *rc);

   static CMPIInstance* mbEncNewInstance(const CMPIBroker* mb, const CMPIObjectPath* eCop,
                                       CMPIStatus *rc) {
      if (!eCop) {
		if (rc) CMSetStatus(rc, CMPI_RC_ERR_INVALID_PARAMETER);
	    return NULL;
	  }
      CIMObjectPath* cop=(CIMObjectPath*)eCop->hdl;
	  if (!cop) {
		if (rc) CMSetStatus(rc, CMPI_RC_ERR_INVALID_PARAMETER);
	    return NULL;
	  }
		 
   //   cout<<"--- mbEncNewInstance() "<<cop->getClassName()<<"-"<<cop->getNameSpace()<<endl;
      CIMClass *cls=mbGetClass(mb,*cop);
      CIMInstance *ci=NULL;

      if (cls) {
         ci=new CIMInstance(cop->getClassName());
         const CMPIContext *ctx=CMPI_ThreadContext::getContext();
         CMPIFlags flgs=ctx->ft->getEntry(ctx,CMPIInvocationFlags,rc).value.uint32;
         if ((flgs & CMPI_FLAG_IncludeQualifiers)!=0) {
            for (int i=0,m=cls->getQualifierCount(); i<m; i++)
               ci->addQualifier(cls->getQualifier(i).clone());
   //          for (int i=0,m=cls->getPropertyCount(); i<m; i++)
   //             ci->addProperty(cls->getProperty(i).clone());
            for (int i=0,m=cls->getPropertyCount(); i<m; i++) {
               CIMConstProperty p=cls->getProperty(i);
               CIMProperty np(p.getName(),p.getValue(),
                              p.getArraySize(),p.getReferenceClassName(),
                              p.getClassOrigin());
               for (int q=0,qm=p.getQualifierCount(); q<qm; q++) {
                  np.addQualifier(p.getQualifier(q).clone());
               }
               ci->addProperty(np);
            }
         }
         else {
            for (int i=0,m=cls->getPropertyCount(); i<m; i++) {
               CIMConstProperty p=cls->getProperty(i);
               ci->addProperty(CIMProperty(
                  p.getName(),p.getValue(),
                  p.getArraySize(),p.getReferenceClassName(),
                  p.getClassOrigin())); //,
                  //p.getPropagated()));
            }
         }
      }
      else {
         if (rc) CMSetStatus(rc,CMPI_RC_ERR_NOT_FOUND);
         return NULL;
      }

      ci->setPath(*cop);
      CMPIInstance* neInst=reinterpret_cast<CMPIInstance*>(new CMPI_Object(ci));
      neInst->ft->ftVersion = 100;
      if (rc) CMSetStatus(rc,CMPI_RC_OK);
   //   CMPIString *str=mbEncToString(mb,neInst,NULL);
      return neInst;
   }

   static CMPIObjectPath* mbEncNewObjectPath(const CMPIBroker* mb, const char *ns, const char *cls,
                     CMPIStatus *rc) {
   //   cout<<"--- mbEncNewObjectPath() "<<ns<<"-"<<cls<<endl;
      Array<CIMKeyBinding> keyBindings;
      String host;
      CIMName className;
	  if (cls)	className=Name(cls);
		else
				className=Name("");
      CIMNamespaceName nameSpace;
	  if (ns)	nameSpace =NameSpaceName(ns);
		else
				nameSpace=NameSpaceName("");	
      CIMObjectPath *cop=new CIMObjectPath(host,nameSpace,className,keyBindings);
      CMPIObjectPath *nePath=reinterpret_cast<CMPIObjectPath*>(new CMPI_Object(cop));
      if (rc) CMSetStatus(rc,CMPI_RC_OK);
      return nePath;
   }

   static CMPIArgs* mbEncNewArgs(const CMPIBroker* mb, CMPIStatus *rc) {
      if (rc) CMSetStatus(rc,CMPI_RC_OK);
      return reinterpret_cast<CMPIArgs*>(new CMPI_Object(new Array<CIMParamValue>()));
   }

   static CMPIString* mbEncNewString(const CMPIBroker* mb, const char *cStr, CMPIStatus *rc) {
      if (rc) CMSetStatus(rc,CMPI_RC_OK);
	  if (cStr == NULL)
      	return reinterpret_cast<CMPIString*>(new CMPI_Object(""));
      return reinterpret_cast<CMPIString*>(new CMPI_Object(cStr));
   }

   CMPIString* mbIntNewString(const char *s) {
      return mbEncNewString(NULL,s,NULL);
   }

   CMPIArray* mbEncNewArray(const CMPIBroker* mb, CMPICount count, CMPIType type,
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

   static CMPIDateTime* mbEncNewDateTime(const CMPIBroker* mb, CMPIStatus *rc) {
   // cout<<"--- mbEncNewDateTime()"<<endl;
      if (rc) CMSetStatus(rc,CMPI_RC_OK);
      return newDateTime();
   }

   extern CMPIDateTime *newDateTimeBin(CMPIUint64,CMPIBoolean);

   static CMPIDateTime* mbEncNewDateTimeFromBinary(const CMPIBroker* mb, CMPIUint64 time,
         CMPIBoolean interval ,CMPIStatus *rc) {
   // cout<<"--- mbEncNewDateTimeFromBinary()"<<endl;
      if (rc) CMSetStatus(rc,CMPI_RC_OK);
      return newDateTimeBin(time,interval);
   }

   extern CMPIDateTime *newDateTimeChar(const char*);

   static CMPIDateTime* mbEncNewDateTimeFromString(const CMPIBroker* mb, const char *t ,CMPIStatus *rc) {
   //   cout<<"--- mbEncNewDateTimeFromString()"<<endl;
	  CMPIDateTime *date = NULL;
      if (rc) CMSetStatus(rc,CMPI_RC_OK);
      date=newDateTimeChar(t);
	  if (!date)
		if (rc) CMSetStatus(rc, CMPI_RC_ERR_INVALID_PARAMETER);
	  return date;
   }

   static CMPIString* mbEncToString(const CMPIBroker*,const void *o, CMPIStatus *rc) {
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
         CIMInstance *ci=(CIMInstance*)obj->getHdl();
         str="Instance of "+ci->getClassName().getString()+" {\n";
         for (int i=0,m=ci->getPropertyCount(); i<m; i++) {
            CIMConstProperty p=ci->getProperty(i);
            str.append("  "+typeToString(p.getType())+
               " "+p.getName().getString()+
               " = "+p.getValue().toString()+";\n");
         }
         str.append("};\n");
      }

      else if (obj->getFtab()==(void*)CMPI_ObjectPath_Ftab ||
         obj->getFtab()==(void*)CMPI_ObjectPathOnStack_Ftab) {
         str=((CIMObjectPath*)obj->getHdl())->toString();
      }
	  else if (obj->getFtab()==(void*)CMPI_String_Ftab) {
		str=String((const char*)obj->getHdl());
	  }
	  else if (obj->getFtab()==(void*)CMPI_Args_Ftab || 
			  obj->getFtab()==(void*)CMPI_ArgsOnStack_Ftab) 
			  {
			      const Array<CIMParamValue>* arg=(Array<CIMParamValue>*)obj->getHdl();
				  for (int i=0,m=arg->size(); i < m; i++) {
						  const CIMParamValue &p=(*arg)[i];
				  		  str.append(p.getParameterName()+":"+p.getValue().toString()+"\n");
				 }
			  }
      else {
         sprintf(msg,"** Object not recognized (%p) **",o);
         if (rc) CMSetStatus(rc,CMPI_RC_ERR_FAILED);
         return reinterpret_cast<CMPIString*>(new CMPI_Object(msg));
      }

      sprintf(msg,"%p: ",o);
      return reinterpret_cast<CMPIString*>(new CMPI_Object(String(msg)+str));
   }

   static CMPIBoolean mbEncClassPathIsA(const CMPIBroker *mb, const CMPIObjectPath *eCp, const char *type, CMPIStatus *rc) {
	
	  if ((eCp==NULL) || (type==NULL)) {
		if (rc) CMSetStatus(rc, CMPI_RC_ERR_INVALID_PARAMETER);
	    return false;
	  }
      if (CIMName::legal(type) == false)
		{
      if (rc) CMSetStatus(rc,CMPI_RC_ERR_INVALID_CLASS);
	   return 0;
		}	
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

   static CMPIBoolean mbEncIsOfType(const CMPIBroker *mb, const void *o, const char *type, CMPIStatus *rc) {
      CMPI_Object *obj=(CMPI_Object*)o;
      char msg[128];
      void *Ftab = NULL;

      if (obj==NULL) {
         sprintf(msg,"** Null object ptr (%p) **",o);
         if (rc) { CMSetStatusWithChars(mb,rc,CMPI_RC_ERR_FAILED,msg); }
         return 0;
      }

      if (rc) CMSetStatus(rc,CMPI_RC_OK);

      Ftab = obj->getFtab();

      if (((Ftab==(void*)CMPI_Instance_Ftab) || (Ftab==(void*)CMPI_InstanceOnStack_Ftab)) && 
            strncmp(type,CMPIInstance_str, CMPIInstance_str_l)==0) return 1;
      if (((Ftab==(void*)CMPI_ObjectPath_Ftab) || (Ftab==(void*)CMPI_ObjectPathOnStack_Ftab)) &&
            strncmp(type,CMPIObjectPath_str, CMPIObjectPath_str_l)==0) return 1;

      if (((Ftab==(void*)CMPI_Args_Ftab) || (Ftab==(void*)CMPI_ArgsOnStack_Ftab)) &&
            strncmp(type,CMPIArgs_str, CMPIArgs_str_l)==0) return 1;

      if (((Ftab==(void*)CMPI_Context_Ftab) || (Ftab==(void*)CMPI_ContextOnStack_Ftab)) &&
            strncmp(type,CMPIContext_str, CMPIContext_str_l)==0) return 1;

      if (((Ftab==(void*)CMPI_ResultRefOnStack_Ftab) || (Ftab==(void*)CMPI_ResultInstOnStack_Ftab) ||
		   (Ftab==(void*)CMPI_ResultData_Ftab) || (Ftab==(void*)CMPI_ResultMethOnStack_Ftab) ||
           (Ftab==(void*)CMPI_ResultResponseOnStack_Ftab) || (Ftab==(void*)CMPI_ResultExecQueryOnStack_Ftab)) &&
            strncmp(type,CMPIResult_str, CMPIResult_str_l)==0) return 1;

      if (Ftab==(void*)CMPI_DateTime_Ftab &&
            strncmp(type,CMPIDateTime_str, CMPIDateTime_str_l)==0) return 1;

      if (Ftab==(void*)CMPI_Array_Ftab &&
            strncmp(type,CMPIArray_str, CMPIArray_str_l)==0) return 1;

      if (Ftab==(void*)CMPI_String_Ftab &&
            strncmp(type,CMPIString_str, CMPIString_str_l)==0) return 1;

      if (Ftab==(void*)CMPI_SelectExp_Ftab &&
            strncmp(type,CMPISelectExp_str, CMPISelectExp_str_l)==0) return 1;

      if (Ftab==(void*)CMPI_SelectCond_Ftab &&
            strncmp(type,CMPISelectCond_str, CMPISelectCond_str_l)==0) return 1;

      if (Ftab==(void*)CMPI_SubCond_Ftab &&
            strncmp(type,CMPISubCond_str, CMPISubCond_str_l)==0) return 1;

      if (Ftab==(void*)CMPI_Predicate_Ftab &&
            strncmp(type,CMPIPredicate_str, CMPIPredicate_str_l)==0) return 1;

      if (Ftab==(void*)CMPI_Broker_Ftab &&
            strncmp(type,CMPIBroker_str, CMPIBroker_str_l)==0) return 1;

      if (((Ftab==(void*)CMPI_ObjEnumeration_Ftab) || 
		  (Ftab==(void*)CMPI_InstEnumeration_Ftab) || 
          (Ftab==(void*)CMPI_OpEnumeration_Ftab)) &&
            strncmp(type,CMPIEnumeration_str, CMPIEnumeration_str_l)==0) return 1;

      sprintf(msg,"** Object not recognized (%p) **",o);
      if (rc) { CMSetStatusWithChars(mb,rc,CMPI_RC_ERR_FAILED,msg); }
      return 0;
   }

   static CMPIString* mbEncGetType(const CMPIBroker *mb,const  void* o, CMPIStatus *rc) {
      CMPI_Object *obj=(CMPI_Object*)o;
      char msg[128];
      void *Ftab= NULL;

      if (obj==NULL) {
         sprintf(msg,"** Null object ptr (%p) **",o);
         if (rc) { CMSetStatusWithChars(mb,rc,CMPI_RC_ERR_FAILED,msg); }
         return 0;
      }

      if (rc) CMSetStatus(rc,CMPI_RC_OK);
	  Ftab = obj->getFtab();

      if ((Ftab==(void*)CMPI_Instance_Ftab) || (Ftab==(void*)CMPI_InstanceOnStack_Ftab))
            return mb->eft->newString(mb,CMPIInstance_str,rc);

      if ((Ftab==(void*)CMPI_ObjectPath_Ftab) || (Ftab==(void*)CMPI_ObjectPathOnStack_Ftab))
            return mb->eft->newString(mb,CMPIObjectPath_str,rc);

      if ((Ftab==(void*)CMPI_Args_Ftab) || (Ftab==(void*)CMPI_ArgsOnStack_Ftab))
            return mb->eft->newString(mb,CMPIArgs_str,rc);

      if ((Ftab==(void*)CMPI_Context_Ftab) || (Ftab==(void*)CMPI_ContextOnStack_Ftab))
            return mb->eft->newString(mb,CMPIContext_str,rc);

      if ((Ftab==(void*)CMPI_ResultRefOnStack_Ftab) || (Ftab==(void*)CMPI_ResultInstOnStack_Ftab) ||
		   (Ftab==(void*)CMPI_ResultData_Ftab) || (Ftab==(void*)CMPI_ResultMethOnStack_Ftab) ||
           (Ftab==(void*)CMPI_ResultResponseOnStack_Ftab) || (Ftab==(void*)CMPI_ResultExecQueryOnStack_Ftab))
            return mb->eft->newString(mb,CMPIResult_str,rc);

      if (Ftab==(void*)CMPI_DateTime_Ftab)
            return mb->eft->newString(mb,CMPIDateTime_str,rc);

      if (Ftab==(void*)CMPI_Array_Ftab)
            return mb->eft->newString(mb,CMPIArray_str,rc);

      if (Ftab==(void*)CMPI_String_Ftab)
            return mb->eft->newString(mb,CMPIString_str,rc);

      if (Ftab==(void*)CMPI_SelectExp_Ftab)
            return mb->eft->newString(mb,CMPISelectExp_str,rc);

      if (Ftab==(void*)CMPI_SelectCond_Ftab)
            return mb->eft->newString(mb,CMPISelectCond_str,rc);

      if (Ftab==(void*)CMPI_SubCond_Ftab)
            return mb->eft->newString(mb,CMPISubCond_str,rc);

      if (Ftab==(void*)CMPI_Predicate_Ftab)
            return mb->eft->newString(mb,CMPIPredicate_str,rc);

      if (Ftab==(void*)CMPI_Broker_Ftab)
            return mb->eft->newString(mb,CMPIBroker_str,rc);

      if ((Ftab==(void*)CMPI_ObjEnumeration_Ftab) || 
		  (Ftab==(void*)CMPI_InstEnumeration_Ftab) || 
          (Ftab==(void*)CMPI_OpEnumeration_Ftab))
            return mb->eft->newString(mb,CMPIEnumeration_str,rc);

      sprintf(msg,"** Object not recognized (%p) **",o);
      if (rc) { CMSetStatusWithChars(mb,rc,CMPI_RC_ERR_FAILED,msg); }
      return 0;
   }

   #if defined (CMPI_VER_85)

   static CMPIString* mbEncGetMessage(const CMPIBroker *mb, const char *msgId, const char *defMsg,
               CMPIStatus* rc, unsigned int count, ...) {

      MessageLoaderParms parms(msgId,defMsg);
      DDD(cout<<"--- mbEncGetMessage() count: "<<count<<endl);
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

#if defined(CMPI_VER_100)
  CMPIStatus mbEncLogMessage 
       (const CMPIBroker*,int severity ,const char *id,const char *text,
	const CMPIString *string) {

	  if ( !id || !(text || string))
			  CMReturn(CMPI_RC_ERR_INVALID_PARAMETER);

	  String logString = id;
      Uint32 logSeverity = Logger::INFORMATION;


	  logString.append(":");

	  if (string)
		{
           logString.append( ( char*)CMGetCharsPtr(string,NULL));
		}
	  else
	    {
           logString.append( text );     
		}
    // There are no notion in CMPI spec about what 'severity' means.
	// So we are going to try to map 
	if (severity <= 1)
			logSeverity = Logger::INFORMATION;
	else if (severity == 2)
			logSeverity = Logger::WARNING;
	else if (severity == 3)
			logSeverity = Logger::SEVERE;
	else if (severity == 4)
			logSeverity = Logger::FATAL;

	Logger::put(Logger::STANDARD_LOG, id, logSeverity, logString); 
    CMReturn ( CMPI_RC_OK);
  }

  CMPIStatus mbEncTracer      (const CMPIBroker*,int level,const char *component,const char *text,
			      const CMPIString *string)
  {
	if ( !component || !(text || string))
		  CMReturn(CMPI_RC_ERR_INVALID_PARAMETER);

	String traceString = String::EMPTY;
	Uint32 traceComponent = TRC_PROVIDERMANAGER;
    Uint32 traceLevel = Tracer::LEVEL1;

    // There are no notion in CMPI spec about what 'level' means.
	// So we are going to try to map . We don't want to map to LEVEL1
	// as it requires the PEG_METHOD_ENTER and PEG_METHOD_EXIT macros.
	if (level <= 2)
			traceLevel = Tracer::LEVEL2;
	else if (level == 3)
			traceLevel = Tracer::LEVEL3;
	else if (level == 4)
			traceLevel = Tracer::LEVEL4;

	// Next is figuring if 'component' maps to the Pegasus types;
	{
		Uint32 i =0;
		Uint32 m =sizeof(TRACE_COMPONENT_LIST)/sizeof(TRACE_COMPONENT_LIST[0]); 
		for (; i < m; i++)
		{
			if (System::strcasecmp(component, TRACE_COMPONENT_LIST[i]) == 0)
			{
					traceComponent = i;
					break;
			}
		}
		// if not found, just use TRC_PROVIDERMANAGER and put the 
		// 'component' in the traceString.
    	if ((m = i) && (traceComponent == TRC_PROVIDERMANAGER))
		{
			traceString=String(component);
			traceString.append(":");
		}
	}
	if (string)
	   {
         traceString.append( ( char*)CMGetCharsPtr(string,NULL));
	   }
	else
	   {
         traceString.append( text );
	   }

	Tracer::trace(traceComponent, traceLevel, traceString.getCString()); 
    CMReturn ( CMPI_RC_OK);
  }
#endif

  static CMPISelectExp *mbEncNewSelectExp (const CMPIBroker * mb, const char *query,
                                           const char *lang,
                                           CMPIArray ** projection,
                                           CMPIStatus * st)
  {
    int exception = 1;
    int useShortNames = 0;
    CMPIStatus rc = { CMPI_RC_OK, NULL };

    if (strncmp (lang, CALL_SIGN_WQL, CALL_SIGN_WQL_SIZE) == 0)
      {
        WQLSelectStatement *stmt = new WQLSelectStatement ();
        try
        {
          WQLParser::parse (query, *stmt);
          exception = 0;
        }
        catch (const ParseError &)
        {
          if (st)
            CMSetStatus (st, CMPI_RC_ERR_INVALID_QUERY);
        }
        catch (const MissingNullTerminator &)
        {
          if (st)
            CMSetStatus (st, CMPI_RC_ERR_INVALID_QUERY);
        }
        if (exception)
          {
            delete stmt;
            if (projection)
              *projection = NULL;
            return NULL;
          }
		if (projection)
			{
        if (stmt->getAllProperties ())
          {
            	*projection = NULL;
          }
        else
          {
            *projection =
              mbEncNewArray (mb, stmt->getSelectPropertyNameCount (),
                             CMPI_string, NULL);
            for (int i = 0, m = stmt->getSelectPropertyNameCount (); i < m;
                 i++)
              {
                const CIMName & n = stmt->getSelectPropertyName (i);
                //cerr << "Property: " << n.getString() << endl;
                // Since the array and the CIMName disappear when this function
                // exits we use CMPI data storage - the CMPI_Object keeps a list of
                // data and cleans it up when the provider API function is exited.
                CMPIString *str_data =
                  reinterpret_cast < CMPIString * >(new CMPI_Object (n.getString()));
                CMPIValue value;
                value.string = str_data;

                rc = CMSetArrayElementAt (*projection, i,
                                          &value, CMPI_string);
                if (rc.rc != CMPI_RC_OK)
                  {
                    if (st)
                      CMSetStatus (st, rc.rc);
                    return NULL;
				  }
              }
		    }
          }
        stmt->hasWhereClause ();
        if (st)
            CMSetStatus (st, CMPI_RC_OK);
        return (CMPISelectExp *) new CMPI_SelectExp (stmt);
      }

    if ((strncmp (lang, "CIMxCQL", CALL_SIGN_CQL_SIZE) == 0) || (strncmp (lang, CALL_SIGN_CQL, CALL_SIGN_CQL_SIZE) == 0))
      {
        /* IBMKR: This will have to be removed when the CMPI spec is updated
           with a clear explanation of what properties array can have as
           strings. For right now, if useShortNames is set to true, _only_
           the last chained identifier is used. */
        if (strncmp (lang, CALL_SIGN_CQL, CALL_SIGN_CQL_SIZE) == 0)
		{
          useShortNames = 1;
		}
        // Get the namespace.
        const CMPIContext *ctx = CMPI_ThreadContext::getContext ();

        CMPIData data = ctx->ft->getEntry (ctx, CMPIInitNameSpace, &rc);
        if (rc.rc != CMPI_RC_OK)
          {
            if (st)
              CMSetStatus (st, CMPI_RC_ERR_FAILED);
            return NULL;
          }

        // Create the CIMOMHandle wrapper.
        CIMOMHandle *cm_handle = CM_CIMOM (mb);
        CIMOMHandleQueryContext
          qcontext (CIMNamespaceName (CMGetCharPtr (data.value.string)),
                    *cm_handle);

        String sLang (lang);
        String sQuery (query);

        CQLSelectStatement *selectStatement =
          new CQLSelectStatement (sLang, sQuery, qcontext);
        try
        {
          CQLParser::parse (query, *selectStatement);
          selectStatement->validate ();
          exception = 0;
        }
        catch (...)
        {
          if (st)
            CMSetStatus (st, CMPI_RC_ERR_INVALID_QUERY);
        }
	
        if (exception)
          {
            delete selectStatement;
            if (projection)
              *projection = NULL;
            return NULL;
          }
        else
          {
	    if (projection)
	      {
		Array < CQLChainedIdentifier > select_Array =
		  selectStatement->getSelectChainedIdentifiers ();
		
		// Special check. Remove it when useShortNames is not neccessary
		if ((select_Array.size() == 1) && (useShortNames) && (select_Array[0].getLastIdentifier().getName().getString() == String::EMPTY))

		  {
		    *projection= NULL;
		    
		  }
		else {
		  *projection =
		    mbEncNewArray (mb, select_Array.size (), CMPI_string, NULL);
		  
		  CQLIdentifier identifier;
		  String name;
		  
		  for (Uint32 i = 0; i < select_Array.size (); i++)
		    {
		      if (useShortNames)
			{
			  identifier = select_Array[i].getLastIdentifier ();
			  name = identifier.getName ().getString ();
			}
		      else
			{
			  name = select_Array[i].toString ();
			}
		      // Since the array and the CIMName disappear when this function
		      // exits we use CMPI data storage - the CMPI_Object keeps a list of
		      // data and cleans it up when the provider API function is exited.
		      //cerr << "Property: " << name << endl;
		      CMPIString *str_data =
			reinterpret_cast < CMPIString * >(new CMPI_Object (name));
		      CMPIValue value;
		      value.string = str_data;
		      
		      rc = CMSetArrayElementAt (*projection, i,
						&value, CMPI_string);
		      
		      if (rc.rc != CMPI_RC_OK)
			{
			  if (st)
			    CMSetStatus (st, rc.rc);
			  return NULL;
			}
		    }
		}
	      }
          }
        if (st)
	  CMSetStatus (st, CMPI_RC_OK);
        return (CMPISelectExp *) new CMPI_SelectExp (selectStatement);
      }
    if (st)
      CMSetStatus (st, CMPI_RC_ERR_QUERY_LANGUAGE_NOT_SUPPORTED);
    return NULL;
  }

#if defined (CMPI_VER_90) && !defined(CMPI_VER_100)

   static CMPIArray * mbEncGetKeyList(CMPIBroker *mb, CMPIContext *ctx,
                  CMPIObjectPath *cop, CMPIStatus *rc) {
	  if ((cop==NULL) || (ctx==NULL)) {
		if (rc) CMSetStatus(rc, CMPI_RC_ERR_INVALID_PARAMETER);
	    return NULL;
	  }
      CIMObjectPath *op=(CIMObjectPath*)cop->hdl;

	  if (!op) {
		if (rc) CMSetStatus(rc, CMPI_RC_ERR_INVALID_PARAMETER);
	    return NULL;
	  }
      CIMClass *cls=mbGetClass(mb,*op);
      Array<String> keys;
      for (int i=0,m=cls->getPropertyCount(); i<m; i++) {
         CIMConstProperty p=cls->getProperty(i);
         Uint32 k=p.findQualifier("key");
         if (k!=PEG_NOT_FOUND) {
            keys.append(p.getName().getString());
         }
      }
      CMPIArray *ar=mb->eft->newArray(mb,keys.size(),CMPI_string,NULL);
      for (Uint32 i=0,m=keys.size(); i<m; i++) {
         String s=keys[i];
         CMPIString *str=string2CMPIString(s);
         ar->ft->setElementAt(ar,i,(CMPIValue*)&str,CMPI_string);
      }
      if (rc) CMSetStatus(rc,CMPI_RC_OK);
      return ar;
   }


   #endif

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
#if defined (CMPI_VER_90) && !defined(CMPI_VER_100)
     mbEncGetKeyList
#endif
#if defined (CMPI_VER_100)
     mbEncLogMessage,
     mbEncTracer
#endif
};

CMPIBrokerEncFT *CMPI_BrokerEnc_Ftab=&brokerEnc_FT;


PEGASUS_NAMESPACE_END



