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

#include "CMPI_Result.h"
#include "CMPI_Ftabs.h"
#include "CMPI_Value.h"

PEGASUS_USING_STD;
PEGASUS_NAMESPACE_BEGIN

static CMPIStatus resultReturnData(CMPIResult* eRes, CMPIValue* data, CMPIType type) {
   CMPIrc rc;
   CIMValue v=value2CIMValue(data,type,&rc);
   if (eRes->ft==CMPI_ResultMethOnStack_Ftab) {
      MethodResultResponseHandler* res=(MethodResultResponseHandler*)eRes->hdl;
      if (((CMPI_Result*)eRes)->flags & RESULT_set==0) {
         res->processing();
         ((CMPI_Result*)eRes)->flags|=RESULT_set;
      }
      res->deliver(v);
   }
   else {
      ValueResponseHandler* res=(ValueResponseHandler*)eRes->hdl;
      if (((CMPI_Result*)eRes)->flags & RESULT_set==0) {
         res->processing();
         ((CMPI_Result*)eRes)->flags|=RESULT_set;
      }
      res->deliver(v);
   }
   CMReturn(CMPI_RC_OK);
}

static CMPIStatus resultReturnInstance(CMPIResult* eRes, CMPIInstance* eInst) {
   InstanceResponseHandler* res=(InstanceResponseHandler*)eRes->hdl;
   if ((((CMPI_Result*)eRes)->flags & RESULT_set)==0) {
      res->processing();
      ((CMPI_Result*)eRes)->flags|=RESULT_set;
   }
   CIMInstance& inst=*(CIMInstance*)(eInst->hdl);
   CMPI_Result *xRes=(CMPI_Result*)eRes;
   const CIMObjectPath& op=inst.getPath();
   CIMClass *cc=mbGetClass(xRes->xBroker,op);
   CIMObjectPath iop=inst.buildPath(*cc);
   iop.setNameSpace(op.getNameSpace());
   inst.setPath(iop);
   res->deliver(inst);
   CMReturn(CMPI_RC_OK);
}

static CMPIStatus resultReturnObject(CMPIResult* eRes, CMPIInstance* eInst) {
   ObjectResponseHandler* res=(ObjectResponseHandler*)eRes->hdl;
   if ((((CMPI_Result*)eRes)->flags & RESULT_set)==0) {
      res->processing();
      ((CMPI_Result*)eRes)->flags|=RESULT_set;
   }
   CIMInstance& inst=*(CIMInstance*)(eInst->hdl);
   CMPI_Result *xRes=(CMPI_Result*)eRes;
   const CIMObjectPath& op=inst.getPath();
   CIMClass *cc=mbGetClass(xRes->xBroker,op);
   CIMObjectPath iop=inst.buildPath(*cc);
   iop.setNameSpace(op.getNameSpace());
   inst.setPath(iop);
   res->deliver(inst);
   CMReturn(CMPI_RC_OK);
}

static CMPIStatus resultReturnObjectPath(CMPIResult* eRes, CMPIObjectPath* eRef) {
   ObjectPathResponseHandler* res=(ObjectPathResponseHandler*)eRes->hdl;
   if ((((CMPI_Result*)eRes)->flags & RESULT_set)==0) {
      res->processing();
      ((CMPI_Result*)eRes)->flags|=RESULT_set;
   }
   CIMObjectPath& ref=*(CIMObjectPath*)(eRef->hdl);
   res->deliver(ref);
   CMReturn(CMPI_RC_OK);
}

static CMPIStatus resultReturnInstDone(CMPIResult* eRes) {
   InstanceResponseHandler* res=(InstanceResponseHandler*)eRes->hdl;
   if ((((CMPI_Result*)eRes)->flags & RESULT_set)==0) res->processing();
   res->complete();
   ((CMPI_Result*)eRes)->flags|=(RESULT_done | RESULT_set);
   CMReturn(CMPI_RC_OK)}

static CMPIStatus resultReturnRefDone(CMPIResult* eRes) {
   ObjectPathResponseHandler* res=(ObjectPathResponseHandler*)eRes->hdl;
   if ((((CMPI_Result*)eRes)->flags & RESULT_set)==0) res->processing();
   res->complete();
   ((CMPI_Result*)eRes)->flags|=(RESULT_done | RESULT_set);
   CMReturn(CMPI_RC_OK);
}

static CMPIStatus resultReturnDataDone(CMPIResult* eRes) {
   ValueResponseHandler* res=(ValueResponseHandler*)eRes->hdl;
   if ((((CMPI_Result*)eRes)->flags & RESULT_set)==0) res->processing();
   res->complete();
   ((CMPI_Result*)eRes)->flags|=(RESULT_done | RESULT_set);
   CMReturn(CMPI_RC_OK);
}

static CMPIStatus resultReturnMethDone(CMPIResult* eRes) {
   MethodResultResponseHandler* res=(MethodResultResponseHandler*)eRes->hdl;
   if ((((CMPI_Result*)eRes)->flags & RESULT_set)==0) res->processing();
//   res->complete();    // Do not close the handle
   ((CMPI_Result*)eRes)->flags|=(RESULT_done | RESULT_set);
   CMReturn(CMPI_RC_OK);
}

static CMPIStatus resultReturnObjDone(CMPIResult* eRes) {
   ObjectResponseHandler* res=(ObjectResponseHandler*)eRes->hdl;
   if ((((CMPI_Result*)eRes)->flags & RESULT_set)==0) res->processing();
   res->complete();
   ((CMPI_Result*)eRes)->flags|=(RESULT_done | RESULT_set);
   CMReturn(CMPI_RC_OK);
}

static CMPIStatus resultBadReturnData(CMPIResult* eRes, CMPIValue* data, CMPIType type) {
   CMReturn(CMPI_RC_ERR_NOT_SUPPORTED);
}

static CMPIStatus resultBadReturnInstance(CMPIResult* eRes, CMPIInstance* eInst) {
   CMReturn(CMPI_RC_ERR_NOT_SUPPORTED);
}

static CMPIStatus resultBadReturnObjectPath(CMPIResult* eRes, CMPIObjectPath* eRef) {
   CMReturn(CMPI_RC_ERR_NOT_SUPPORTED);
}

static CMPIResultFT resultMethOnStack_FT={
     CMPICurrentVersion,
     NULL,
     NULL,
     resultReturnData,
     resultBadReturnInstance,
     resultBadReturnObjectPath,
     resultReturnMethDone,
};

static CMPIResultFT resultObjOnStack_FT={
     CMPICurrentVersion,
     NULL,
     NULL,
     resultBadReturnData,
     resultReturnObject,
     resultBadReturnObjectPath,
     resultReturnObjDone,
};

static CMPIResultFT resultData_FT={
     CMPICurrentVersion,
     NULL,
     NULL,
     resultReturnData,
     resultBadReturnInstance,
     resultBadReturnObjectPath,
     resultReturnDataDone,
};

static CMPIResultFT resultInstOnStack_FT={
     CMPICurrentVersion,
     NULL,
     NULL,
     resultBadReturnData,
     resultReturnInstance,
     resultBadReturnObjectPath,
     resultReturnInstDone,
};

static CMPIResultFT resultRefOnStack_FT={
     CMPICurrentVersion,
     NULL,
     NULL,
     resultBadReturnData,
     resultBadReturnInstance,
     resultReturnObjectPath,
     resultReturnRefDone,
};

static CMPIResultFT resultResponseOnStack_FT={
     CMPICurrentVersion,
     NULL,
     NULL,
     resultBadReturnData,
     resultBadReturnInstance,
     resultBadReturnObjectPath,
     resultReturnDataDone,
};

CMPIResultFT *CMPI_ResultMeth_Ftab=&resultMethOnStack_FT;
CMPIResultFT *CMPI_ResultData_Ftab=&resultData_FT;
CMPIResultFT *CMPI_ResultMethOnStack_Ftab=&resultMethOnStack_FT;
CMPIResultFT *CMPI_ResultInstOnStack_Ftab=&resultInstOnStack_FT;
CMPIResultFT *CMPI_ResultObjOnStack_Ftab=&resultObjOnStack_FT;
CMPIResultFT *CMPI_ResultRefOnStack_Ftab=&resultRefOnStack_FT;
CMPIResultFT *CMPI_ResultResponseOnStack_Ftab=&resultResponseOnStack_FT;

CMPI_ResultOnStack::CMPI_ResultOnStack(const ObjectPathResponseHandler & handler,
         CMPI_Broker *xMb) {
      hdl=(void*)&handler;
      xBroker=xMb;
      ft=CMPI_ResultRefOnStack_Ftab;
      flags=RESULT_ObjectPath;
   }

CMPI_ResultOnStack::CMPI_ResultOnStack(const InstanceResponseHandler& handler,
         CMPI_Broker *xMb) {
      hdl=(void*)&handler;
      xBroker=xMb;
      ft=CMPI_ResultInstOnStack_Ftab;
      flags=RESULT_Instance;
   }

CMPI_ResultOnStack::CMPI_ResultOnStack(const ObjectResponseHandler& handler,
         CMPI_Broker *xMb) {
      hdl=(void*)&handler;
      xBroker=xMb;
      ft=CMPI_ResultObjOnStack_Ftab;
      flags=RESULT_Object;
   }

CMPI_ResultOnStack::CMPI_ResultOnStack(const MethodResultResponseHandler& handler,
         CMPI_Broker *xMb) {
      hdl=(void*)&handler;
      xBroker=xMb;
      ft=CMPI_ResultMethOnStack_Ftab;
      flags=RESULT_Method;
   }

CMPI_ResultOnStack::CMPI_ResultOnStack(const ResponseHandler& handler,
         CMPI_Broker *xMb) {
      hdl=(void*)&handler;
      xBroker=xMb;
      ft=CMPI_ResultResponseOnStack_Ftab;
      flags=RESULT_Response;
   }

CMPI_ResultOnStack::~CMPI_ResultOnStack() {
//      cout<<"--- ~CMPI_ResultOnStack()"<<endl;
      if (flags & RESULT_set==0)  ((ResponseHandler*)hdl)->processing();
      if (flags & RESULT_done==0) ((ResponseHandler*)hdl)->complete();
  }

PEGASUS_NAMESPACE_END


