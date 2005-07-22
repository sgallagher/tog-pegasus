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
// Modified By: David Dillard, VERITAS Software Corp.
//                  (david.dillard@veritas.com)
//              Vijay Eli, IBM, (vijayeli@in.ibm.com) bug#3495
//              Aruran, IBM (ashanmug@in.ibm.com) for Bug# 3496
//
//%/////////////////////////////////////////////////////////////////////////////

#include "CMPI_Version.h"

#include "CMPI_Instance.h"
#include "CMPI_Broker.h"
#include "CMPI_Value.h"
#include "CMPI_String.h"

#include <Pegasus/Common/InternalException.h>
#include <Pegasus/Common/System.h>
#include <string.h>
#include <new>

PEGASUS_USING_STD;
PEGASUS_NAMESPACE_BEGIN

extern int _cmpi_trace;

extern "C" {

   static CMPIStatus instRelease(CMPIInstance* eInst) {
      CIMInstance* inst=(CIMInstance*)eInst->hdl;
      if (inst) {
         delete inst;
         (reinterpret_cast<CMPI_Object*>(eInst))->unlinkAndDelete();
      }
      CMReturn(CMPI_RC_OK);
   }

   static CMPIStatus instReleaseNop(CMPIInstance* eInst) {
      CMReturn(CMPI_RC_OK);
   }

   static CMPIInstance* instClone(const CMPIInstance* eInst, CMPIStatus* rc) {
      if (!eInst->hdl)  {
        if (rc) CMSetStatus(rc, CMPI_RC_ERR_INVALID_PARAMETER);
        return NULL;
      }
      CIMInstance* inst=(CIMInstance*)eInst->hdl;
      try {
            AutoPtr<CIMInstance> cInst(new CIMInstance(inst->clone()));
            AutoPtr<CMPI_Object> obj(new CMPI_Object(cInst.get()));
            cInst.release();
            obj->unlink();
            if (rc) CMSetStatus(rc,CMPI_RC_OK);
            return reinterpret_cast<CMPIInstance *>(obj.release());
      } catch(const PEGASUS_STD(bad_alloc)&) {
          if (rc) CMSetStatus(rc, CMPI_RC_ERROR_SYSTEM);
          return NULL;
      }
   }

   static CMPIData instGetPropertyAt(const CMPIInstance* eInst, CMPICount pos, CMPIString** name,
                              CMPIStatus* rc) {
      CIMInstance* inst=(CIMInstance*)eInst->hdl;
      CMPIData data={0,CMPI_nullValue,{0}};

      if (!inst)  {
        if (rc) CMSetStatus(rc, CMPI_RC_ERR_INVALID_PARAMETER);
        return data;
      }

      if (pos>inst->getPropertyCount()) {
      if (rc) CMSetStatus(rc,CMPI_RC_ERR_NOT_FOUND);
         CMPIData data={0,CMPI_nullValue|CMPI_notFound,{0}};
         return data;
      }
      const CIMProperty& p=inst->getProperty(pos);
      const CIMValue& v=p.getValue();
      CIMType pType=p.getType();
      CMPIType t=type2CMPIType(pType,p.isArray());

      value2CMPIData(v,t,&data);

      if (name) {
         String str=p.getName().getString();
         *name=(CMPIString*)string2CMPIString(str);
      }

      if (rc) CMSetStatus(rc,CMPI_RC_OK);
      return data;
   }

   static CMPIData instGetProperty(const CMPIInstance* eInst, const char *name, CMPIStatus* rc)   {

      CMPIData data={0,CMPI_nullValue|CMPI_notFound,{0}};

      if (!eInst->hdl)  {
        if (rc) CMSetStatus(rc, CMPI_RC_ERR_INVALID_PARAMETER);
        return data;
      }
      CIMInstance* inst=(CIMInstance*)eInst->hdl;
      Uint32 pos=inst->findProperty(String(name));

      if (pos!=PEG_NOT_FOUND) {
         if (rc) CMSetStatus(rc,CMPI_RC_OK);
         return instGetPropertyAt(eInst,pos,NULL,rc);
      }
      if (rc) CMSetStatus(rc,CMPI_RC_ERR_NOT_FOUND);
      return data;
   }


   static CMPICount instGetPropertyCount(const CMPIInstance* eInst, CMPIStatus* rc) {
      CIMInstance* inst=(CIMInstance*)eInst->hdl;
      if (!inst)  {
        if (rc) CMSetStatus(rc, CMPI_RC_ERR_INVALID_PARAMETER);
        return 0;
      }
      if (rc) CMSetStatus(rc,CMPI_RC_OK);
      return inst->getPropertyCount();
   }

   static CMPIStatus instSetProperty(const CMPIInstance* eInst, const char *name,
                           const CMPIValue* data, CMPIType type) {
      CIMInstance *inst=(CIMInstance*)eInst->hdl;
      if (!inst)  {
        CMReturn(CMPI_RC_ERR_INVALID_PARAMETER);
      }
      char **list=(char**)(reinterpret_cast<const CMPI_Object*>(eInst))->priv;
      CMPIrc rc;

      if (list) {
         while (*list) {
            if (System::strcasecmp(name,*list)==0) goto ok;
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
         catch (const TypeMismatchException &) {
	     if (_cmpi_trace) {
           cerr<<"-+- TypeMisMatch exception for: "<<name<<endl;
           if (getenv("PEGASUS_CMPI_CHECKTYPES")!=NULL) {
               cerr<<"-+- Aborting because of CMPI_CHECKTYPES"<<endl;
               abort();
             }
		    }
            CMReturn(CMPI_RC_ERR_TYPE_MISMATCH);
         }
         catch (const Exception &e) {
		    if (_cmpi_trace) {
              cerr<<"-+- "<<e.getMessage()<<" exception for: "<<name<<endl;
              if (getenv("PEGASUS_CMPI_CHECKTYPES")!=NULL) {
                 cerr<<"-+- Aborting because of CMPI_CHECKTYPES"<<endl;
                 abort();
               }
            }
            CMReturnWithString(CMPI_RC_ERR_FAILED,
            reinterpret_cast<CMPIString*>(new CMPI_Object(e.getMessage())));
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

   static CMPIObjectPath* instGetObjectPath(const CMPIInstance* eInst,
                                            CMPIStatus* rc) {
      CIMInstance* inst=(CIMInstance*)eInst->hdl;
      if (!inst)  {
        if (rc) CMSetStatus(rc, CMPI_RC_ERR_INVALID_PARAMETER);
        return NULL;
      }
      const CIMObjectPath &clsRef=inst->getPath();
      AutoPtr<CIMObjectPath> objPath(NULL);
      AutoPtr<CMPI_Object> obj(NULL);
      try {
            if (clsRef.getKeyBindings().size()==0) {
              CIMClass *cc=mbGetClass(CMPI_ThreadContext::getBroker(),clsRef);
              const CIMObjectPath &ref=inst->buildPath(
                      *(reinterpret_cast<const CIMConstClass*>(cc)));
              objPath.reset(new CIMObjectPath(ref));
            }
            else 
              objPath.reset(new CIMObjectPath(clsRef));
            obj.reset(new CMPI_Object(objPath.get()));
            objPath.release();
            if (rc) CMSetStatus(rc,CMPI_RC_OK);
            return reinterpret_cast<CMPIObjectPath*> (obj.release()); 
      } catch(const PEGASUS_STD(bad_alloc)&) {
          if (rc) CMSetStatus(rc, CMPI_RC_ERROR_SYSTEM);
          return NULL;
      }
   }

   static CMPIStatus instSetObjectPath( CMPIInstance* eInst, const CMPIObjectPath *obj)
   {
      CIMInstance* inst=(CIMInstance*)eInst->hdl;
      if ((inst==NULL) || (obj==NULL))  {
        CMReturn ( CMPI_RC_ERR_INVALID_PARAMETER);
      }
     CIMObjectPath &ref = *(CIMObjectPath*)(obj->hdl);
	try {
    	inst->setPath(ref); 
     } catch (const TypeMismatchException &e) {
	   CMReturnWithString(CMPI_RC_ERR_FAILED,
            reinterpret_cast<CMPIString*>(new CMPI_Object(e.getMessage())));
	}
    CMReturn ( CMPI_RC_OK);
   }

   static CMPIStatus instSetPropertyFilter(CMPIInstance* eInst,
               const char** propertyList, const char **keys){
      if (!eInst->hdl)  {
        CMReturn(CMPI_RC_ERR_INVALID_PARAMETER);
      }

      CMPI_Object *inst=reinterpret_cast<CMPI_Object*>(eInst);
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
      list = new char*[s+2];
      for (s=0,i=0; propertyList[i]; i++,s++) list[s]=strdup(propertyList[i]);
      for (i=0; keys[i]; i++,s++) list[s]=strdup(keys[i]);
      list[s]=NULL;
      inst->priv=(void*)list;

      CMReturn(CMPI_RC_OK);
   }

   static CMPIStatus instSetPropertyFilterIgnore(CMPIInstance* eInst,
               const char** propertyList, const char **keys){
    /* We ignore it.  */
     CMReturn ( CMPI_RC_OK);
   }

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
#if defined(CMPI_VER_100)
     instSetObjectPath,
#endif
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
#if defined(CMPI_VER_100)
     instSetObjectPath,
#endif
};

CMPIInstanceFT *CMPI_Instance_Ftab=&instance_FT;
CMPIInstanceFT *CMPI_InstanceOnStack_Ftab=&instanceOnStack_FT;


CMPI_InstanceOnStack::CMPI_InstanceOnStack(const CIMInstance& ci) {
      hdl=(void*)&ci;
      ft=CMPI_InstanceOnStack_Ftab;
   }


PEGASUS_NAMESPACE_END
