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

#include "CMPI_ContextArgs.h"
#include "CMPI_Ftabs.h"
#include "CMPI_Value.h"
#include "CMPI_String.h"

#include <string.h>

PEGASUS_USING_STD;
PEGASUS_NAMESPACE_BEGIN

// CMPIArgs section

extern "C" {

   static CMPIStatus argsRelease(CMPIArgs* eArg) {
      Array<CIMParamValue>* arg=(Array<CIMParamValue>*)eArg->hdl;
      if (arg) {
         delete arg;
         (reinterpret_cast<CMPI_Object*>(eArg))->unlinkAndDelete();
      }
      CMReturn(CMPI_RC_OK);
   }

   static CMPIStatus argsReleaseNop(CMPIArgs* eArg) {
      CMReturn(CMPI_RC_OK);
   }

   static CMPIArgs* argsClone(const CMPIArgs* eArg, CMPIStatus* rc) {
      Array<CIMParamValue>* arg=(Array<CIMParamValue>*)eArg->hdl;
      Array<CIMParamValue>* cArg=new Array<CIMParamValue>();
      for (long i=0,s=arg->size(); i<s; i++) {
         const CIMParamValue &v=(*arg)[i];
         cArg->append(v.clone());
      }
      CMPI_Object* obj=new CMPI_Object(cArg);
      obj->unlink();
      CMPIArgs* neArg=(CMPIArgs*)obj;
      if (rc) CMSetStatus(rc,CMPI_RC_OK);
      return neArg;
   }

   static long locateArg(const Array<CIMParamValue> &a, const CIMName &eName) {
      for (long i=0,s=a.size(); i<s; i++) {
         const String &n=a[i].getParameterName();
         if (String::equalNoCase(n,eName.getString())) return i;
      }
      return -1;
   }

   static CMPIStatus argsAddArg(const CMPIArgs* eArg, const char *name, const CMPIValue* data,  CMPIType type) {
      Array<CIMParamValue>* arg=(Array<CIMParamValue>*)eArg->hdl;
      CMPIrc rc;
      CIMValue v=value2CIMValue(data,type,&rc);
      CIMName sName(name);

      long i=locateArg(*arg,sName);
      if (i>=0) arg->remove(i);

      arg->append(CIMParamValue(sName.getString(),v));
      CMReturn(CMPI_RC_OK);
   }

   static CMPIData argsGetArgAt(const CMPIArgs* eArg, CMPICount pos, 
				CMPIString** name,
				CMPIStatus* rc) {

      Array<CIMParamValue>* arg=(Array<CIMParamValue>*)eArg->hdl;
      CMPIData data={0,CMPI_nullValue | CMPI_notFound,{0}};

      if (pos>arg->size()) {
         if (rc) CMSetStatus(rc,CMPI_RC_ERR_NOT_FOUND);
         return data;
      }

      CIMValue v=(*arg)[pos].getValue();
      CIMType pType=v.getType();
      CMPIType t=type2CMPIType(pType,v.isArray());

      value2CMPIData(v,t,&data);

      if (name) {
         String n=(*arg)[pos].getParameterName();
         *name=(CMPIString*)string2CMPIString(n);
      }

      if (rc) CMSetStatus(rc,CMPI_RC_OK);
      return data;
   }

   static CMPIData argsGetArg(const CMPIArgs* eArg, const char *name, CMPIStatus* rc) {
      Array<CIMParamValue>* arg=(Array<CIMParamValue>*)eArg->hdl;
      CIMName eName(name);

      long i=locateArg(*arg,eName);
      if (i>=0) return argsGetArgAt(eArg,i,NULL,rc);

      CMPIData data={0,CMPI_nullValue | CMPI_notFound,{0}};
      if (rc) CMSetStatus(rc,CMPI_RC_ERR_NOT_FOUND);
      return data;
   }

   static CMPICount argsGetArgCount(const CMPIArgs* eArg, CMPIStatus* rc) {
      Array<CIMParamValue>* arg=(Array<CIMParamValue>*)eArg->hdl;
      if (rc) CMSetStatus(rc,CMPI_RC_OK);
      return arg->size();
   }

}

static CMPIArgsFT args_FT={
     CMPICurrentVersion,
     argsRelease,
     argsClone,
     argsAddArg,
     argsGetArg,
     argsGetArgAt,
     argsGetArgCount,
};

CMPIArgsFT *CMPI_Args_Ftab=&args_FT;

static CMPIArgsFT argsOnStack_FT={
     CMPICurrentVersion,
     argsReleaseNop,
     argsClone,
     argsAddArg,
     argsGetArg,
     argsGetArgAt,
     argsGetArgCount,
};

CMPIArgsFT *CMPI_ArgsOnStack_Ftab=&argsOnStack_FT;



// CMPIContext Session

extern "C" {

static CMPIStatus contextReleaseNop(CMPIContext* eCtx) {
   CMReturn(CMPI_RC_OK);
}

   static CMPIData contextGetEntry(const CMPIContext* eCtx, const char *name, CMPIStatus* rc) {
      return argsGetArg((CMPIArgs*)eCtx,name,rc);
   }

   CMPIData contextGetEntryAt(const CMPIContext* eCtx, CMPICount pos,
               CMPIString** name, CMPIStatus* rc) {
      return argsGetArgAt((CMPIArgs*)eCtx,pos,name,rc);
   }

   static CMPICount contextGetEntryCount(const CMPIContext* eCtx, CMPIStatus* rc) {
      return argsGetArgCount((CMPIArgs*)eCtx,rc);
   }

   static CMPIStatus contextAddEntry(const CMPIContext* eCtx, const char *name,
               const CMPIValue* data,  CMPIType type) {
      if (strcmp(name,SnmpTrapOidContainer::NAME.getCString())==0) {
         OperationContext *ctx=((CMPI_Context*)eCtx)->ctx;
         if (type==CMPI_chars) {
            ctx->insert(SnmpTrapOidContainer((char*)data));
            CMReturn(CMPI_RC_OK);
         }
         else if (type==CMPI_string) {
            ctx->insert(SnmpTrapOidContainer((char*)data->string->hdl));
            CMReturn(CMPI_RC_OK);
         }
      }
      return argsAddArg((CMPIArgs*)eCtx,name,data,type);
   }

}

static CMPIContextFT context_FT={
     CMPICurrentVersion,
     contextReleaseNop,
     NULL,
     contextGetEntry,
     contextGetEntryAt,
     contextGetEntryCount,
     contextAddEntry,
};

CMPIContextFT *CMPI_Context_Ftab=&context_FT;

static CMPIContextFT contextOnStack_FT={
     CMPICurrentVersion,
     contextReleaseNop,
     NULL,
     contextGetEntry,
     contextGetEntryAt,
     contextGetEntryCount,
     contextAddEntry,
};

CMPIContextFT *CMPI_ContextOnStack_Ftab=&contextOnStack_FT;


CMPI_Context::CMPI_Context(const OperationContext& ct) {
      ctx=(OperationContext*)&ct;
      thr=NULL;
      hdl=(void*)new Array<CIMParamValue>();
      ft=CMPI_Context_Ftab;
   }
CMPI_Context::~CMPI_Context() {
      delete (Array<CIMParamValue>*)hdl;
      delete ctx;
}
CMPI_ContextOnStack::CMPI_ContextOnStack(const OperationContext& ct) {
      ctx=(OperationContext*)&ct;
      hdl=(void*)new Array<CIMParamValue>();
      ft=CMPI_ContextOnStack_Ftab;
   }

CMPI_ContextOnStack::~CMPI_ContextOnStack() {
      delete (Array<CIMParamValue>*)hdl;
   }

CMPI_ArgsOnStack::CMPI_ArgsOnStack(const Array<CIMParamValue>& args) {
      hdl=(void*)&args;
      ft=CMPI_ArgsOnStack_Ftab;
   }

PEGASUS_NAMESPACE_END







