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

#include "CMPI_Enumeration.h"
#include "CMPI_Object.h"
#include "CMPI_Ftabs.h"

PEGASUS_USING_STD;
PEGASUS_NAMESPACE_BEGIN


static CMPIStatus enumRelease(CMPIEnumeration* eEnum) {
   //cout<<"--- enumRelease()"<<endl;
   CIMInstance* enm=(CIMInstance*)eEnum->hdl;
   if (enm) {
      delete enm;
      ((CMPI_Object*)eEnum)->unlinkAndDelete();
   }
   CMReturn(CMPI_RC_OK);
}

//static CMPIStatus enumReleaseNop(CMPIEnumeration* eEnum) {
//   CMReturn(CMPI_RC_OK);
//}

static CMPIEnumeration* enumClone(CMPIEnumeration* eEnum, CMPIStatus* rc) {
//   CIMInstance* enm=(CIMInstance*)eEnum->hdl;
//   CIMInstance* cInst=new CIMInstance(enum->clone());
//   CMPIEnumeration* neEnum=(CMPIEnumeration*)new CMPI_Object(cInst,CMPI_Instance_Ftab);
//   if (rc) CMSetStatus(rc,CMPI_RC_OK);
//   return neEnum;
   return NULL;
}

CMPIData enumGetNext(CMPIEnumeration* eEnum, CMPIStatus* rc) {
   CMPIData data={0,0,{0}};
   if ((void*)eEnum->ft==(void*)CMPI_ObjEnumeration_Ftab) {
      CMPI_ObjEnumeration* ie=(CMPI_ObjEnumeration*)eEnum;
      data.type=CMPI_instance;
      Array<CIMInstance>* ia=(Array<CIMInstance>*)ie->hdl;
      if (ie->cursor<ie->max) {
         data.value.inst=(CMPIInstance*)
	    new CMPI_Object(new CIMInstance((*ia)[ie->cursor++]));
         if (rc) CMSetStatus(rc,CMPI_RC_OK);
      }
      else if (rc) CMSetStatus(rc,CMPI_RC_ERR_FAILED);
   }

   else if ((void*)eEnum->ft==(void*)CMPI_InstEnumeration_Ftab) {
      CMPI_InstEnumeration* ie=(CMPI_InstEnumeration*)eEnum;
      data.type=CMPI_instance;
      Array<CIMInstance>* ia=(Array<CIMInstance>*)ie->hdl;
      if (ie->cursor<ie->max) {
         data.value.inst=(CMPIInstance*)
	    new CMPI_Object(new CIMInstance((*ia)[ie->cursor++]));
         if (rc) CMSetStatus(rc,CMPI_RC_OK);
      }
      else if (rc) CMSetStatus(rc,CMPI_RC_ERR_FAILED);
   }

   else {
      CMPI_OpEnumeration* oe=(CMPI_OpEnumeration*)eEnum;
      data.type=CMPI_ref;
      Array<CIMObjectPath>* opa=(Array<CIMObjectPath>*)oe->hdl;
      if (oe->cursor<oe->max) {
         data.value.ref=(CMPIObjectPath*)
	    new CMPI_Object(new CIMObjectPath((*opa)[oe->cursor++]));
         if (rc) CMSetStatus(rc,CMPI_RC_OK);
      }
      else if (rc) CMSetStatus(rc,CMPI_RC_ERR_FAILED);
   }
   return data;
}

CMPIBoolean enumHasNext(CMPIEnumeration* eEnum, CMPIStatus* rc) {
   if (rc) CMSetStatus(rc,CMPI_RC_OK);
   if ((void*)eEnum->ft==(void*)CMPI_ObjEnumeration_Ftab) {
      CMPI_ObjEnumeration* ie=(CMPI_ObjEnumeration*)eEnum;
      if (ie->cursor<ie->max) return true;
   }
   else if ((void*)eEnum->ft==(void*)CMPI_InstEnumeration_Ftab) {
      CMPI_InstEnumeration* ie=(CMPI_InstEnumeration*)eEnum;
      if (ie->cursor<ie->max) return true;
   }
   else {
      CMPI_OpEnumeration* oe=(CMPI_OpEnumeration*)eEnum;
      if (oe->cursor<oe->max) return true;
   }
   return false;
}

CMPIArray* enumToArray(CMPIEnumeration* eEnum, CMPIStatus* rc) {
   return NULL;
}


static CMPIEnumerationFT objEnumeration_FT={
     CMPICurrentVersion,
     enumRelease,
     enumClone,
     enumGetNext,
     enumHasNext,
     enumToArray,
};

static CMPIEnumerationFT instEnumeration_FT={
     CMPICurrentVersion,
     enumRelease,
     enumClone,
     enumGetNext,
     enumHasNext,
     enumToArray,
};

static CMPIEnumerationFT opEnumeration_FT={
     CMPICurrentVersion,
     enumRelease,
     enumClone,
     enumGetNext,
     enumHasNext,
     enumToArray,
};

CMPIEnumerationFT *CMPI_ObjEnumeration_Ftab=&objEnumeration_FT;
CMPIEnumerationFT *CMPI_InstEnumeration_Ftab=&instEnumeration_FT;
CMPIEnumerationFT *CMPI_OpEnumeration_Ftab=&opEnumeration_FT;

CMPI_ObjEnumeration::CMPI_ObjEnumeration(Array<CIMObject>* oa) {
   cursor=0;
   max=oa->size();
   hdl=(void*)oa;
   ft=CMPI_ObjEnumeration_Ftab;
}
CMPI_InstEnumeration::CMPI_InstEnumeration(Array<CIMInstance>* ia) {
   cursor=0;
   max=ia->size();
   hdl=(void*)ia;
   ft=CMPI_InstEnumeration_Ftab;
}

CMPI_OpEnumeration::CMPI_OpEnumeration(Array<CIMObjectPath>* opa) {
   cursor=0;
   max=opa->size();
   hdl=(void*)opa;
   ft=CMPI_OpEnumeration_Ftab;
}

PEGASUS_NAMESPACE_END













