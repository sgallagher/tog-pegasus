//%2006////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001, 2002 BMC Software; Hewlett-Packard Development
// Company, L.P.; IBM Corp.; The Open Group; Tivoli Systems.
// Copyright (c) 2003 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation, The Open Group.
// Copyright (c) 2004 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation; VERITAS Software Corporation; The Open Group.
// Copyright (c) 2005 Hewlett-Packard Development Company, L.P.; IBM Corp.;
// EMC Corporation; VERITAS Software Corporation; The Open Group.
// Copyright (c) 2006 Hewlett-Packard Development Company, L.P.; IBM Corp.;
// EMC Corporation; Symantec Corporation; The Open Group.
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
//%/////////////////////////////////////////////////////////////////////////////

//NOCHKSRC

#include "CMPI_Version.h"

#include "CMPI_Result.h"
#include "CMPI_Ftabs.h"
#include "CMPI_Value.h"
#include "CMPI_String.h"
#include <typeinfo>

#include <Pegasus/ProviderManager2/SimpleResponseHandler.h>
#include <Pegasus/Common/System.h>
#include <Pegasus/Common/CIMType.h>
#include <Pegasus/Common/Mutex.h>
#include <string.h>

PEGASUS_USING_STD;
PEGASUS_NAMESPACE_BEGIN

Mutex errorChainMutex;

#define DDD(X)	if (_cmpi_trace) X;
extern int _cmpi_trace;

CMPIStatus resolveEmbeddedInstanceTypes(
    OperationResponseHandler * opRes,
    CIMInstance & inst)
{
    CIMOperationRequestMessage * request =
        dynamic_cast<CIMOperationRequestMessage *>(opRes->getRequest());
    if(request->operationContext.contains(NormalizerContextContainer::NAME) &&
        request->operationContext.contains(CachedClassDefinitionContainer::NAME))
    {
        const NormalizerContextContainer * contextContainer =
            dynamic_cast<const NormalizerContextContainer *>(
                &(request->operationContext.get(
                    NormalizerContextContainer::NAME)));
        PEGASUS_ASSERT(contextContainer);
        const CachedClassDefinitionContainer * classContainer =
            dynamic_cast<const CachedClassDefinitionContainer *>(
                &(request->operationContext.get(
                    CachedClassDefinitionContainer::NAME)));
        CIMClass classDef(classContainer->getClass());
        for(unsigned int i = 0, n = inst.getPropertyCount(); i < n; ++i)
        {
            CIMConstProperty currentProp(inst.getProperty(i));
            if(currentProp.getType() == CIMTYPE_OBJECT)
            {
                Uint32 propIndex = classDef.findProperty(
                    currentProp.getName());
                if(propIndex == PEG_NOT_FOUND)
                {
                    String message = 
                        String("Could not find property ")
                        + currentProp.getName().getString()
                        + " in class definition";
                    CMReturnWithString(CMPI_RC_ERR_FAILED,
                        (CMPIString*)string2CMPIString(message));
                }

                CIMConstProperty propertyDef(
                    classDef.getProperty(propIndex));

                // Normalize the property: this will ensure that the property
                // and any embedded instance/object properties are consistent
                // with the relevant class definitions.
                CIMProperty normalizedProperty(
                    ObjectNormalizer::_processProperty(
                        propertyDef,
                        currentProp,
                        false,
                        false,
                        contextContainer->getContext(),
                        request->nameSpace));

                // Remove the old property, add the new, and adjust the
                // loop counters appropriately.
                inst.removeProperty(i);
                inst.addProperty(normalizedProperty);
                --i;
                --n;
            }
        }
    }
    //else
    //{
        // If the NormalizerContextContainer is not present, then the
        // ObjectNormalizer must be enabled for this operation and the
        // ObjectNormalizer will do the work in the above try block.
    //}

    CMReturn(CMPI_RC_OK);
}

extern "C" {

   PEGASUS_STATIC CMPIStatus resultReturnData(
       const CMPIResult* eRes, const CMPIValue* data,  const CMPIType type) 
   {
      CMPIrc rc;
      if ((eRes->hdl == NULL) || (data == NULL))
	     CMReturn(CMPI_RC_ERR_INVALID_PARAMETER);
    try {
      CIMValue v=value2CIMValue((CMPIValue*)data,type,&rc);
      if (eRes->ft==CMPI_ResultMethOnStack_Ftab)
      {
         MethodResultResponseHandler* res=(MethodResultResponseHandler*)eRes->hdl;
         if ((((CMPI_Result*)eRes)->flags & RESULT_set)==0)
         {
            res->processing();
            ((CMPI_Result*)eRes)->flags|=RESULT_set;
         }
#ifdef PEGASUS_EMBEDDED_INSTANCE_SUPPORT
          // If the CMPI type is CMPI_instance, then the CIM return type could
          // be either an EmbeddedObject or EmbeddedInstance. We must find the
          // method signature in the class definition to find out what the
          // return type is.
          if(type == CMPI_instance)
          {
              try 
              {
                  InvokeMethodResponseHandler * opRes =
                    dynamic_cast<InvokeMethodResponseHandler *>(res);
                  CIMInvokeMethodRequestMessage * request =
                      dynamic_cast<CIMInvokeMethodRequestMessage *>(
                          opRes->getRequest());

                  PEGASUS_ASSERT(request != 0);

                  const CachedClassDefinitionContainer * classContainer =
                      dynamic_cast<const CachedClassDefinitionContainer *>(
                          &(request->operationContext.get(
                              CachedClassDefinitionContainer::NAME)));
                  PEGASUS_ASSERT(classContainer != 0);

                  CIMClass classDef(classContainer->getClass());
                  Uint32 methodIndex = classDef.findMethod(
                      request->methodName);
                  if(methodIndex == PEG_NOT_FOUND)
                  {
                      String message(
                          "Method not found in class definition");
                      CMReturnWithString(CMPI_RC_ERR_FAILED,
                          (CMPIString*)string2CMPIString(message));
                  }

                  CIMMethod methodDef(classDef.getMethod(methodIndex));
                  if(methodDef.findQualifier(CIMName("EmbeddedInstance")) !=
                      PEG_NOT_FOUND)
                  {
                      PEGASUS_ASSERT(v.getType() == CIMTYPE_OBJECT);
                      CIMObject tmpObject;
                      v.get(tmpObject);
                      v = CIMValue(CIMInstance(tmpObject));
                  }
              }
              catch(Exception & e)
              {
                  CMReturnWithString(CMPI_RC_ERR_FAILED,
                      (CMPIString*)string2CMPIString(e.getMessage()));
              }
          }
#endif // PEGASUS_EMBEDDED_INSTANCE_SUPPORT
         res->deliver(v);
      }
      else {
         ValueResponseHandler* res=(ValueResponseHandler*)eRes->hdl;
         if ((((CMPI_Result*)eRes)->flags & RESULT_set)==0) {
            res->processing();
            ((CMPI_Result*)eRes)->flags|=RESULT_set;
         }
         res->deliver(v);
      }
    }
    catch (const CIMException &e)
    {
        DDD(cout<<"### exception: resultReturnData - msg: "<<e.getMessage()<<endl);
        CMReturnWithString(CMPI_RC_ERR_FAILED, (CMPIString*)string2CMPIString(e.getMessage()));
    }
    CMReturn(CMPI_RC_OK);
   }

   PEGASUS_STATIC CMPIStatus resultReturnInstance(const CMPIResult* eRes, const CMPIInstance* eInst) {

      InstanceResponseHandler* res=(InstanceResponseHandler*)eRes->hdl;
      if ((res == NULL) || (eInst == NULL))
		CMReturn(CMPI_RC_ERR_INVALID_PARAMETER);

	  if (!eInst->hdl)
		CMReturn(CMPI_RC_ERR_INVALID_PARAMETER);
	try {	
      if ((((CMPI_Result*)eRes)->flags & RESULT_set)==0) {
         res->processing();
         ((CMPI_Result*)eRes)->flags|=RESULT_set;
      }
      CIMInstance& inst=*(CIMInstance*)(eInst->hdl);
      CMPI_Result *xRes=(CMPI_Result*)eRes;
      const CIMObjectPath& op=inst.getPath();

      //Build objectpath if keybindings are not found. This will happen
      //when this instance is created with empty ObjectPath and Provider
      //did not set objectpath in the instance.
      if (op.getKeyBindings().size() == 0)
      {
           CIMClass *cc=mbGetClass(xRes->xBroker,op);
           CIMObjectPath iop=inst.buildPath(*cc);
           iop.setNameSpace(op.getNameSpace());
           inst.setPath(iop);
      }

      // Check if a property filter has been set. If yes throw out
      // all properties which are not found in the filter list.
      char **listroot=(char**)(reinterpret_cast<const CMPI_Object*>(eInst))->priv;

      if (listroot && *listroot)
      {
         int propertyCount = inst.getPropertyCount()-1;
         for (int idx=propertyCount; idx >=0 ; idx--)
         {
            CIMConstProperty prop = inst.getProperty(idx);
            String sName = prop.getName().getString();
            char * name = strdup(sName.getCString());
            char **list = listroot;
            int found = false;
            while (*list)
            {
               if (System::strcasecmp(name,*list)==0)
               {
                  found = true;
                  break;
               }
               list++;
            }
            free(name);
            if (!found)
            {
               inst.removeProperty(idx);
            }
         }
      }

#ifdef PEGASUS_EMBEDDED_INSTANCE_SUPPORT
      // CMPI does not differentiate between EmbeddedInstances and
      // EmbeddedObjects, so any mismatches between the property type
      // in the instance and the property type in the class definition
      // must be resolved.
      EnumerateInstancesResponseHandler * eiRes =
          dynamic_cast<EnumerateInstancesResponseHandler *>(res);
      GetInstanceResponseHandler * giRes = 0;
      CMPIStatus status;
      if(eiRes)
      {
          status = resolveEmbeddedInstanceTypes(eiRes, inst);
      }
      else
      {
          giRes = dynamic_cast<GetInstanceResponseHandler *>(res);
          PEGASUS_ASSERT(giRes);
          status = resolveEmbeddedInstanceTypes(giRes, inst);
      }
      
      if(status.rc != CMPI_RC_OK)
      {
        return status;
      }
#endif // PEGASUS_EMBEDDED_INSTANCE_SUPPORT

      res->deliver(inst);
	} catch (const CIMException &e)
	{
    	DDD(cout<<"### exception: resultReturnInstance - msg: "<<e.getMessage()<<endl);
	    CMReturnWithString(CMPI_RC_ERR_FAILED, (CMPIString*)string2CMPIString(e.getMessage()));
	}
      CMReturn(CMPI_RC_OK);
   }

   PEGASUS_STATIC CMPIStatus resultReturnObject(const CMPIResult* eRes, const CMPIInstance* eInst) {
      ObjectResponseHandler* res=(ObjectResponseHandler*)eRes->hdl;

      if ((res == NULL) || (eInst == NULL))
		CMReturn(CMPI_RC_ERR_INVALID_PARAMETER);

	  if (!eInst->hdl)
		CMReturn(CMPI_RC_ERR_INVALID_PARAMETER);
	try { 
      if ((((CMPI_Result*)eRes)->flags & RESULT_set)==0) {
         res->processing();
         ((CMPI_Result*)eRes)->flags|=RESULT_set;
      }
      CIMInstance& inst=*(CIMInstance*)(eInst->hdl);
      CMPI_Result *xRes=(CMPI_Result*)eRes;
      const CIMObjectPath& op=inst.getPath();
     
      //Build objectpath if keybindings are not found. This will happen
      //when this instance is created with empty ObjectPath and Provider
      //did not set objectpath in the instance.
      if (op.getKeyBindings().size() == 0)
      {
           CIMClass *cc=mbGetClass(xRes->xBroker,op);
           CIMObjectPath iop=inst.buildPath(*cc);
           iop.setNameSpace(op.getNameSpace());
           inst.setPath(iop);
      }

#ifdef PEGASUS_EMBEDDED_INSTANCE_SUPPORT
      // CMPI does not differentiate between EmbeddedInstances and
      // EmbeddedObjects, so any mismatches between the property type
      // in the instance and the property type in the class definition
      // must be resolved.

      CMPIStatus status;

      do
      {
          // Try EnumerateInstancesResponseHandler:

          EnumerateInstancesResponseHandler * eiRes =
              dynamic_cast<EnumerateInstancesResponseHandler *>(res);

          if(eiRes)
          {
              status = resolveEmbeddedInstanceTypes(eiRes, inst);
              break;
          }

          // Try GetInstanceResponseHandler

          GetInstanceResponseHandler * giRes = 0;
              giRes = dynamic_cast<GetInstanceResponseHandler *>(res);

          if (giRes)
          {
              status = resolveEmbeddedInstanceTypes(giRes, inst);
              break;
          }

          // Try AssociatorsResponseHandler:

          AssociatorsResponseHandler* aRes = 0;
              aRes = dynamic_cast<AssociatorsResponseHandler*>(res);

          if (aRes)
          {
              status = resolveEmbeddedInstanceTypes(aRes, inst);
              break;
          }

          // Try AssociatorsResponseHandler:

          ReferencesResponseHandler* rRes = 0;
              rRes = dynamic_cast<ReferencesResponseHandler*>(res);

          if (rRes)
          {
              status = resolveEmbeddedInstanceTypes(rRes, inst);
              break;
          }

          // None of the above.

          PEGASUS_ASSERT(0);
      }
      while (0);
      
      if(status.rc != CMPI_RC_OK)
      {
        return status;
      }
#endif // PEGASUS_EMBEDDED_INSTANCE_SUPPORT

      res->deliver(inst);
	} catch (const CIMException &e)
	{
    	DDD(cout<<"### exception: resultReturnObject - msg: "<<e.getMessage()<<endl);
	    CMReturnWithString(CMPI_RC_ERR_FAILED, (CMPIString*)string2CMPIString(e.getMessage()));
	}
      CMReturn(CMPI_RC_OK);
   }

   PEGASUS_STATIC CMPIStatus resultReturnExecQuery(const CMPIResult* eRes, const CMPIInstance* eInst) {
      ExecQueryResponseHandler* res=(ExecQueryResponseHandler*)eRes->hdl;
      if ((res == NULL) || (eInst == NULL))
		CMReturn(CMPI_RC_ERR_INVALID_PARAMETER);

	  if (!eInst->hdl)
		CMReturn(CMPI_RC_ERR_INVALID_PARAMETER);
	try { 
      if ((((CMPI_Result*)eRes)->flags & RESULT_set)==0) {
         res->processing();
         ((CMPI_Result*)eRes)->flags|=RESULT_set;
      }
      CIMInstance& inst=*(CIMInstance*)(eInst->hdl);
      CMPI_Result *xRes=(CMPI_Result*)eRes;
      // Check if a property filter has been set. If yes throw out
      // all properties which are not found in the filter list.
      char **listroot=(char**)(reinterpret_cast<const CMPI_Object*>(eInst))->priv;

      if (listroot && *listroot)
      {
         int propertyCount = inst.getPropertyCount()-1;
         for (int idx=propertyCount; idx >=0 ; idx--)
         {
            CIMConstProperty prop = inst.getProperty(idx);
            String sName = prop.getName().getString();
            char * name = strdup(sName.getCString());
            char **list = listroot;
            int found = false;
            while (*list)
            {
               if (System::strcasecmp(name,*list)==0)
               {
                  found = true;
                  break;
               }
               list++;
            }
            free(name);
            if (!found)
            {
               inst.removeProperty(idx);
            }
         }
      }

      res->deliver(inst);
	} catch (const CIMException &e)
	{
    	DDD(cout<<"### exception: resultReturnExecQuery - msg: "<<e.getMessage()<<endl);
	    CMReturnWithString(CMPI_RC_ERR_FAILED, (CMPIString*)string2CMPIString(e.getMessage()));
	}
      CMReturn(CMPI_RC_OK);
   }
   PEGASUS_STATIC CMPIStatus resultReturnObjectPath(const CMPIResult* eRes, const CMPIObjectPath* eRef) {
      ObjectPathResponseHandler* res=(ObjectPathResponseHandler*)eRes->hdl;

      if ((res == NULL) || (eRef == NULL))
		CMReturn(CMPI_RC_ERR_INVALID_PARAMETER);

	  if (!eRef->hdl)
		CMReturn(CMPI_RC_ERR_INVALID_PARAMETER);
	try { 
      if ((((CMPI_Result*)eRes)->flags & RESULT_set)==0) {
         res->processing();
         ((CMPI_Result*)eRes)->flags|=RESULT_set;
      }
      CIMObjectPath& ref=*(CIMObjectPath*)(eRef->hdl);
      res->deliver(ref);
	} catch (const CIMException &e)
	{
    	DDD(cout<<"### exception: resultReturnObjectPath - msg: "<<e.getMessage()<<endl);
	    CMReturnWithString(CMPI_RC_ERR_FAILED, (CMPIString*)string2CMPIString(e.getMessage()));
	}
      CMReturn(CMPI_RC_OK);
   }

   PEGASUS_STATIC CMPIStatus resultReturnInstDone(const CMPIResult* eRes) {
      InstanceResponseHandler* res=(InstanceResponseHandler*)eRes->hdl;
	  if (!res)
		CMReturn(CMPI_RC_ERR_INVALID_PARAMETER);
	try { 
      if ((((CMPI_Result*)eRes)->flags & RESULT_set)==0) res->processing();
      res->complete();
      ((CMPI_Result*)eRes)->flags|=(RESULT_done | RESULT_set);
	} catch (const CIMException &e)
	{
    	DDD(cout<<"### exception: resultReturnInstDone - msg: "<<e.getMessage()<<endl);
	    CMReturnWithString(CMPI_RC_ERR_FAILED, (CMPIString*)string2CMPIString(e.getMessage()));
	}
	
      CMReturn(CMPI_RC_OK)
	}

   PEGASUS_STATIC CMPIStatus resultReturnRefDone(const CMPIResult* eRes) {
      ObjectPathResponseHandler* res=(ObjectPathResponseHandler*)eRes->hdl;
	  if (!res)
		CMReturn(CMPI_RC_ERR_INVALID_PARAMETER);
	try {
      if ((((CMPI_Result*)eRes)->flags & RESULT_set)==0) res->processing();
      res->complete();
      ((CMPI_Result*)eRes)->flags|=(RESULT_done | RESULT_set);
	} catch (const CIMException &e)
	{
    	DDD(cout<<"### exception: resultReturnRefDone - msg: "<<e.getMessage()<<endl);
	    CMReturnWithString(CMPI_RC_ERR_FAILED, (CMPIString*)string2CMPIString(e.getMessage()));
	}
      CMReturn(CMPI_RC_OK);
   }

   PEGASUS_STATIC CMPIStatus resultReturnDataDone(const CMPIResult* eRes) {
      ResponseHandler* res=(ResponseHandler*)eRes->hdl;
	  if (!res)
		CMReturn(CMPI_RC_ERR_INVALID_PARAMETER);
	 try {
      if ((((CMPI_Result*)eRes)->flags & RESULT_set)==0) res->processing();
      res->complete();
      ((CMPI_Result*)eRes)->flags|=(RESULT_done | RESULT_set);
	} catch (const CIMException &e)
	{
    	DDD(cout<<"### exception: resultReturnDataDone - msg: "<<e.getMessage()<<endl);
	    CMReturnWithString(CMPI_RC_ERR_FAILED, (CMPIString*)string2CMPIString(e.getMessage()));
	}
      CMReturn(CMPI_RC_OK);
   }

   PEGASUS_STATIC CMPIStatus resultReturnMethDone(const CMPIResult* eRes) {
      MethodResultResponseHandler* res=(MethodResultResponseHandler*)eRes->hdl;
	  if (!res)
		CMReturn(CMPI_RC_ERR_INVALID_PARAMETER);
	try {
      if ((((CMPI_Result*)eRes)->flags & RESULT_set)==0) res->processing();
   //   res->complete();    // Do not close the handle
      ((CMPI_Result*)eRes)->flags|=(RESULT_done | RESULT_set);
	} catch (const CIMException &e)
	{
    	DDD(cout<<"### exception: resultReturnMethDone - msg: "<<e.getMessage()<<endl);
	    CMReturnWithString(CMPI_RC_ERR_FAILED, (CMPIString*)string2CMPIString(e.getMessage()));
	}
      CMReturn(CMPI_RC_OK);
   }

   PEGASUS_STATIC CMPIStatus resultReturnObjDone(const CMPIResult* eRes) {
      ObjectResponseHandler* res=(ObjectResponseHandler*)eRes->hdl;
	  if (!res)
		CMReturn(CMPI_RC_ERR_INVALID_PARAMETER);
	try {
      if ((((CMPI_Result*)eRes)->flags & RESULT_set)==0) res->processing();
      res->complete();
      ((CMPI_Result*)eRes)->flags|=(RESULT_done | RESULT_set);
	} catch (const CIMException &e)
	{
    	DDD(cout<<"### exception: resultReturnObjDone - msg: "<<e.getMessage()<<endl);
	    CMReturnWithString(CMPI_RC_ERR_FAILED, (CMPIString*)string2CMPIString(e.getMessage()));
	}
      CMReturn(CMPI_RC_OK);
   }

   PEGASUS_STATIC CMPIStatus resultReturnExecQueryDone(const CMPIResult* eRes) {
      ExecQueryResponseHandler* res=(ExecQueryResponseHandler*)eRes->hdl;
	  if (!res)
		CMReturn(CMPI_RC_ERR_INVALID_PARAMETER);
	try {
      if ((((CMPI_Result*)eRes)->flags & RESULT_set)==0) res->processing();
      res->complete();
      ((CMPI_Result*)eRes)->flags|=(RESULT_done | RESULT_set);
	} catch (const CIMException &e)
	{
    	DDD(cout<<"### exception: resultReturnExecQueryDone - msg: "<<e.getMessage()<<endl);
	    CMReturnWithString(CMPI_RC_ERR_FAILED, (CMPIString*)string2CMPIString(e.getMessage()));
	}
      CMReturn(CMPI_RC_OK);
   }

   PEGASUS_STATIC CMPIStatus resultReturnError(const CMPIResult* eRes, const CMPIError* er) {

       CMPIStatus rrc={CMPI_RC_OK,NULL};

       if (eRes->hdl == NULL)
          CMReturn(CMPI_RC_ERR_INVALID_PARAMETER);
       if (er == NULL)
          CMReturn(CMPI_RC_ERR_INVALID_HANDLE);

       CMPIError *clonedError = er->ft->clone(er,&rrc);
       if(rrc.rc != CMPI_RC_OK)
       {
           return rrc;
       }

       AutoMutex mtx(errorChainMutex);
       ((CMPI_Error*)clonedError)->nextError = ((CMPI_Result*)eRes)->resError;
       ((CMPI_Result*)eRes)->resError = (CMPI_Error*)clonedError;
      CMReturn(CMPI_RC_OK);
   }

   PEGASUS_STATIC CMPIStatus resultBadReturnData(const CMPIResult* eRes, const CMPIValue* data,  const CMPIType type) {
      CMReturn(CMPI_RC_ERR_NOT_SUPPORTED);
   }

   PEGASUS_STATIC CMPIStatus resultBadReturnInstance(const CMPIResult* eRes, const CMPIInstance* eInst) {
      CMReturn(CMPI_RC_ERR_NOT_SUPPORTED);
   }

   PEGASUS_STATIC CMPIStatus resultBadReturnObjectPath(const CMPIResult* eRes, const CMPIObjectPath* eRef) {
      CMReturn(CMPI_RC_ERR_NOT_SUPPORTED);
   }

}

static CMPIResultFT resultMethOnStack_FT={
     CMPICurrentVersion,
     NULL,
     NULL,
     resultReturnData,
     resultBadReturnInstance,
     resultBadReturnObjectPath,
     resultReturnMethDone,
     resultReturnError
};

static CMPIResultFT resultObjOnStack_FT={
     CMPICurrentVersion,
     NULL,
     NULL,
     resultBadReturnData,
     resultReturnObject,
     resultBadReturnObjectPath,
     resultReturnObjDone,
     resultReturnError
};

static CMPIResultFT resultExecQueryOnStack_FT={
     CMPICurrentVersion,
     NULL,
     NULL,
     resultBadReturnData,
     resultReturnExecQuery,
     resultBadReturnObjectPath,
     resultReturnExecQueryDone,
     resultReturnError
};

static CMPIResultFT resultData_FT={
     CMPICurrentVersion,
     NULL,
     NULL,
     resultReturnData,
     resultBadReturnInstance,
     resultBadReturnObjectPath,
     resultReturnDataDone,
     resultReturnError
};

static CMPIResultFT resultInstOnStack_FT={
     CMPICurrentVersion,
     NULL,
     NULL,
     resultBadReturnData,
     resultReturnInstance,
     resultBadReturnObjectPath,
     resultReturnInstDone,
     resultReturnError
};

static CMPIResultFT resultRefOnStack_FT={
     CMPICurrentVersion,
     NULL,
     NULL,
     resultBadReturnData,
     resultBadReturnInstance,
     resultReturnObjectPath,
     resultReturnRefDone,
     resultReturnError
};

static CMPIResultFT resultResponseOnStack_FT={
     CMPICurrentVersion,
     NULL,
     NULL,
     resultBadReturnData,
     resultBadReturnInstance,
     resultBadReturnObjectPath,
     resultReturnDataDone,
     resultReturnError
};

CMPIResultFT *CMPI_ResultMeth_Ftab=&resultMethOnStack_FT;
CMPIResultFT *CMPI_ResultData_Ftab=&resultData_FT;
CMPIResultFT *CMPI_ResultMethOnStack_Ftab=&resultMethOnStack_FT;
CMPIResultFT *CMPI_ResultInstOnStack_Ftab=&resultInstOnStack_FT;
CMPIResultFT *CMPI_ResultObjOnStack_Ftab=&resultObjOnStack_FT;
CMPIResultFT *CMPI_ResultRefOnStack_Ftab=&resultRefOnStack_FT;
CMPIResultFT *CMPI_ResultResponseOnStack_Ftab=&resultResponseOnStack_FT;
CMPIResultFT *CMPI_ResultExecQueryOnStack_Ftab=&resultExecQueryOnStack_FT;

CMPI_ResultOnStack::CMPI_ResultOnStack(const ObjectPathResponseHandler & handler,
         CMPI_Broker *xMb) {
      hdl=(void*)&handler;
      xBroker=xMb;
      ft=CMPI_ResultRefOnStack_Ftab;
      flags=RESULT_ObjectPath;
      resError=NULL;
   }

CMPI_ResultOnStack::CMPI_ResultOnStack(const InstanceResponseHandler& handler,
         CMPI_Broker *xMb) {
      hdl=(void*)&handler;
      xBroker=xMb;
      ft=CMPI_ResultInstOnStack_Ftab;
      flags=RESULT_Instance;
      resError=NULL;
   }

CMPI_ResultOnStack::CMPI_ResultOnStack(const ObjectResponseHandler& handler,
         CMPI_Broker *xMb) {
      hdl=(void*)&handler;
      xBroker=xMb;
      ft=CMPI_ResultObjOnStack_Ftab;
      flags=RESULT_Object;
      resError=NULL;
   }

CMPI_ResultOnStack::CMPI_ResultOnStack(const MethodResultResponseHandler& handler,
         CMPI_Broker *xMb) {
      hdl=(void*)&handler;
      xBroker=xMb;
      ft=CMPI_ResultMethOnStack_Ftab;
      flags=RESULT_Method;
      resError=NULL;
   }

CMPI_ResultOnStack::CMPI_ResultOnStack(const ResponseHandler& handler,
         CMPI_Broker *xMb) {
      hdl=(void*)&handler;
      xBroker=xMb;
      ft=CMPI_ResultResponseOnStack_Ftab;
      flags=RESULT_Response;
      resError=NULL;
   }

CMPI_ResultOnStack::CMPI_ResultOnStack(const ExecQueryResponseHandler& handler,
         CMPI_Broker *xMb) {
      hdl=(void*)&handler;
      xBroker=xMb;
      ft=CMPI_ResultExecQueryOnStack_Ftab;
      flags=RESULT_Object;
      resError=NULL;
   }

CMPI_ResultOnStack::~CMPI_ResultOnStack() {
	try {
      if ((flags & RESULT_set)==0)  {
         if (ft==CMPI_ResultRefOnStack_Ftab) ((ObjectPathResponseHandler*)hdl)->processing();
         else if (ft==CMPI_ResultInstOnStack_Ftab) ((InstanceResponseHandler*)hdl)->processing();
         else if (ft==CMPI_ResultObjOnStack_Ftab) ((ObjectResponseHandler*)hdl)->processing();
         else if (ft==CMPI_ResultMethOnStack_Ftab) ((MethodResultResponseHandler*)hdl)->processing();
         else if (ft==CMPI_ResultResponseOnStack_Ftab) ((ResponseHandler*)hdl)->processing();
         else if (ft==CMPI_ResultExecQueryOnStack_Ftab) ((ExecQueryResponseHandler*)hdl)->processing();
         else ((ResponseHandler*)hdl)->processing();  // shoul not get here
      }
      if ((flags & RESULT_done)==0) {
         if (ft==CMPI_ResultRefOnStack_Ftab) ((ObjectPathResponseHandler*)hdl)->complete();
         else if (ft==CMPI_ResultInstOnStack_Ftab) ((InstanceResponseHandler*)hdl)->complete();
         else if (ft==CMPI_ResultObjOnStack_Ftab) ((ObjectResponseHandler*)hdl)->complete();
         else if (ft==CMPI_ResultMethOnStack_Ftab) ((MethodResultResponseHandler*)hdl)->complete();
         else if (ft==CMPI_ResultResponseOnStack_Ftab) ((ResponseHandler*)hdl)->complete();
         else if (ft==CMPI_ResultExecQueryOnStack_Ftab) ((ExecQueryResponseHandler*)hdl)->complete();
         else ((ResponseHandler*)hdl)->complete();  // shoul not get here
      }
	} catch (const CIMException &)
	{
		// Ignore the exception
	}
  }

PEGASUS_NAMESPACE_END


