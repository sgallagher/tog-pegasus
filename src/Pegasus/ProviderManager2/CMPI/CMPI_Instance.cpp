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

#include "CMPI_Instance.h"
#include "CMPI_Broker.h"
#include "CMPI_Value.h"
#include "CMPI_String.h"

#include <Pegasus/Common/InternalException.h>
#ifdef PEGASUS_PLATFORM_ZOS_ZSERIES_IBM
#include <strings.h>       // for strcasecmp
#endif
#include <string.h>

PEGASUS_USING_STD;
PEGASUS_NAMESPACE_BEGIN

static CMPIStatus instRelease(CMPIInstance* eInst) {
//   cout<<"--- instRelease()"<<endl;
   CIMInstance* inst=(CIMInstance*)eInst->hdl;
   if (inst) {
      delete inst;
      ((CMPI_Object*)eInst)->unlinkAndDelete();
   }
   CMReturn(CMPI_RC_OK);
}

static CMPIStatus instReleaseNop(CMPIInstance* eInst) {
   CMReturn(CMPI_RC_OK);
}

static CMPIInstance* instClone(CMPIInstance* eInst, CMPIStatus* rc) {
   CIMInstance* inst=(CIMInstance*)eInst->hdl;
   CIMInstance* cInst=new CIMInstance(inst->clone());
   CMPIInstance* neInst=(CMPIInstance*)new CMPI_Object(cInst);//,CMPI_Instance_Ftab);
   if (rc) CMSetStatus(rc,CMPI_RC_OK);
   return neInst;
}

static CMPIData instGetPropertyAt(CMPIInstance* eInst, CMPICount pos, CMPIString** name,
                            CMPIStatus* rc) {
   CIMInstance* inst=(CIMInstance*)eInst->hdl;
   CMPIData data={0,0,{0}};

   if (pos>inst->getPropertyCount()) {
     if (rc) CMSetStatus(rc,CMPI_RC_ERR_NOT_FOUND);
      return data;
   }
   const CIMProperty& p=inst->getProperty(pos);
   const CIMValue& v=p.getValue();
   CIMType pType=p.getType();
   CMPIType t=type2CMPIType(pType,p.isArray());

   value2CMPIData(v,t,&data);

   if (name) {
      String str=p.getName();
      *name=(CMPIString*)string2CMPIString(str);
   }

   if (rc) CMSetStatus(rc,CMPI_RC_OK);
   return data;
}

static CMPIData instGetProperty(CMPIInstance* eInst, char* name, CMPIStatus* rc) {
   CIMInstance* inst=(CIMInstance*)eInst->hdl;
   Uint32 pos=inst->findProperty(String(name));

   if (pos!=PEG_NOT_FOUND) {
      if (rc) CMSetStatus(rc,CMPI_RC_OK);
      return instGetPropertyAt(eInst,pos,NULL,rc);
   }
   CMPIData data={0,0,{0}};
   if (rc) CMSetStatus(rc,CMPI_RC_ERR_NOT_FOUND);
   return data;
}


static CMPICount instGetPropertyCount(CMPIInstance* eInst, CMPIStatus* rc) {
   CIMInstance* inst=(CIMInstance*)eInst->hdl;
   if (rc) CMSetStatus(rc,CMPI_RC_OK);
   return inst->getPropertyCount();
}

#define PEGASUS_CIM_EXCEPTION(CODE, EXTRA_MESSAGE) \
    TraceableCIMException(CODE, EXTRA_MESSAGE, __FILE__, __LINE__)


static CMPIStatus instSetProperty(CMPIInstance* eInst, char* name,
                          CMPIValue* data, CMPIType type) {
   CIMInstance *inst=(CIMInstance*)eInst->hdl;
   char **list=(char**)((CMPI_Object*)eInst)->priv;
   CMPIrc rc;

   if (list) {
      while (*list) {
         if (strcasecmp(name,*list)==0) goto ok;
         list++;
      }
      CMReturn(CMPI_RC_OK);
   }

  ok:
   CIMValue v=value2CIMValue(data,type,&rc);
   CIMName sName(name);
   Uint32 pos;
   int count=0;

   if ((pos=inst->findProperty(sName))!=PEG_NOT_FOUND) {
      CIMProperty cp=inst->getProperty(pos);
      try {
         cp.setValue(v);
      }
      catch (TypeMismatchException &e) {
         cerr<<"-+- TypeMisMatch exception for: "<<name<<endl;
	 if (getenv("CMPI_CHECKTYPES")!=NULL) {
            cerr<<"-+- Aborting because of CMPI_CHECKTYPES"<<endl;
	    abort();
	 }
         CMReturn(CMPI_RC_ERR_TYPE_MISMATCH);
      }
      catch (Exception &e) {
         cerr<<"-+- "<<e.getMessage()<<" exception for: "<<name<<endl;
	 if (getenv("CMPI_CHECKTYPES")!=NULL) {
         cerr<<"-+- Aborting because of CMPI_CHECKTYPES"<<endl;
	    abort();
	 }
         CMReturnWithString(CMPI_RC_ERR_FAILED,
	     (CMPIString*)new CMPI_Object(e.getMessage()));
      }
   }
   else {
      if (type==CMPI_ref) {
        CIMObjectPath *ref=(CIMObjectPath*)(data->ref->hdl);
        inst->addProperty(CIMProperty(sName,v,count,ref->getClassName()));
      }

      else inst->addProperty(CIMProperty(sName,v,count));
   }
   CMReturn(CMPI_RC_OK);
}

static CMPIObjectPath* instGetObjectPath(CMPIInstance* eInst, CMPIStatus* rc) {
   CIMInstance* inst=(CIMInstance*)eInst->hdl;
   const CIMObjectPath &clsRef=inst->getPath();
   CMPIObjectPath *cop=NULL;
   if (clsRef.getKeyBindings().size()==0) {
      CIMClass *cc=mbGetClass(CMPI_ThreadContext::getBroker(),clsRef);
      const CIMObjectPath &ref=inst->buildPath((const CIMConstClass&)*cc);
      cop=(CMPIObjectPath*)new CMPI_Object(new CIMObjectPath(ref));
   }
   else cop=(CMPIObjectPath*)new CMPI_Object(new CIMObjectPath(clsRef));
   if (rc) CMSetStatus(rc,CMPI_RC_OK);
   return cop;
}

static CMPIStatus instSetPropertyFilter(CMPIInstance* eInst,
            char** propertyList, char **keys){
   CMPI_Object *inst=(CMPI_Object*)eInst;
   char **list=(char**)inst->priv;    // Thank you Warren !
   int i,s;

   if (inst->priv) {
      while (*list) {
         free (*list);
         list++;
      }
      free(inst->priv);
   }
   inst->priv=NULL;

   if (propertyList==NULL) CMReturn(CMPI_RC_OK);
   if (keys==NULL) CMReturn(CMPI_RC_ERR_FAILED);

   for (s=0,i=0; propertyList[i]; i++,s++);
   for (i=0; keys[i]; i++,s++);
   list=(char**)malloc((s+2)*sizeof(char*));
   for (s=0,i=0; propertyList[i]; i++,s++) list[s]=strdup(propertyList[i]);
   for (i=0; keys[i]; i++,s++) list[s]=strdup(keys[i]);
   list[s]=NULL;
   inst->priv=(void*)list;

   CMReturn(CMPI_RC_OK);
}

static CMPIStatus instSetPropertyFilterIgnore(CMPIInstance* eInst,
            char** propertyList, char **keys){
   CMReturn(CMPI_RC_OK);
}

static CMPIInstanceFT instance_FT={
     CMPICurrentVersion,
     instRelease,
     instClone,
     instGetProperty,
     instGetPropertyAt,
     instGetPropertyCount,
     instSetProperty,
     instGetObjectPath,
     instSetPropertyFilter,
};

static CMPIInstanceFT instanceOnStack_FT={
     CMPICurrentVersion,
     instReleaseNop,
     instClone,
     instGetProperty,
     instGetPropertyAt,
     instGetPropertyCount,
     instSetProperty,
     instGetObjectPath,
     instSetPropertyFilterIgnore,
};

CMPIInstanceFT *CMPI_Instance_Ftab=&instance_FT;
CMPIInstanceFT *CMPI_InstanceOnStack_Ftab=&instanceOnStack_FT;


CMPI_InstanceOnStack::CMPI_InstanceOnStack(const CIMInstance& ci) {
      hdl=(void*)&ci;
      ft=CMPI_InstanceOnStack_Ftab;
   }


PEGASUS_NAMESPACE_END
















