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


#include "CMPI_Object.h"
#include "CMPI_Ftabs.h"

PEGASUS_USING_STD;
PEGASUS_NAMESPACE_BEGIN

static CMPIStatus arrayRelease(CMPIArray* eArray) {
//   cout<<"--- arrayRelease()"<<endl;
   CMPIData *dta=(CMPIData*)eArray->hdl;
   if (dta) {
      if (dta->type & CMPI_ENC) {
         for (uint i=1; i<=dta->value.uint32; i++) {
	//    if (dta[i].state==0)
	//     ((CMPIString*)dta[i].value.string)->ft->release((CMPIString*)dta[i].value.string);
	 }
      }
      delete[] dta;
      ((CMPI_Object*)eArray)->unlinkAndDelete();
   }
   CMReturn(CMPI_RC_OK);
}

static CMPIArray* arrayClone(CMPIArray* eArray, CMPIStatus* rc) {
   CMPIData* dta=(CMPIData*)eArray->hdl;
   CMPIData* nDta=new CMPIData[dta->value.uint32+1];
   CMPIArray* nArray=(CMPIArray*)new CMPI_Object(nDta,CMPI_Array_Ftab);
   CMPIStatus rrc={CMPI_RC_OK,NULL};

   if (dta->type & CMPI_ENC) for (uint i=1; i<=dta->value.uint32; i++)
       nDta[i].state=CMPI_nullValue;

   for (uint i=0; i<=dta->value.uint32; i++) {
      nDta[i]=dta[i];
      if (dta->type & CMPI_ENC && dta[i].state==0) {
         nDta[i].value.string=
         ((CMPIString*)dta[i].value.string)->ft->clone((CMPIString*)dta[i].value.string,&rrc);
	 if (rrc.rc) {
            arrayRelease(nArray);
	    if (rc) *rc=rrc;
	    return NULL;
	 }
      }
   }
   if (rc) CMSetStatus(rc,CMPI_RC_OK);
   return nArray;
}

static CMPIData arrayGetElementAt(CMPIArray* eArray, CMPICount pos, CMPIStatus* rc) {
   CMPIData *dta=(CMPIData*)eArray->hdl;
   if (rc) CMSetStatus(rc,CMPI_RC_OK);
   if (pos<dta->value.uint32) return dta[pos+1];

   CMPIData data={0,0,{0}};
   if (rc) CMSetStatus(rc,CMPI_RC_ERR_NOT_FOUND);
   return data;
}

static CMPIStatus arraySetElementAt(CMPIArray* eArray, CMPICount pos,
                                    CMPIValue *val, CMPIType type) {
   CMPIData *dta=(CMPIData*)eArray->hdl;

   if (pos<dta->value.uint32) {
      if ((dta->type&~CMPI_ARRAY)==type) {
         dta[pos+1].state=0;
         dta[pos+1].value=*val;
         CMReturn(CMPI_RC_OK);
      }
      cout<<"--- arraySetElementAt(): CMPI_RC_ERR_TYPE_MISMATCH is "
         <<(void*)(long)type<<" should be "<<(void*)(long)dta->type<<endl;
      CMReturn(CMPI_RC_ERR_TYPE_MISMATCH);
   }
   CMReturn(CMPI_RC_ERR_NOT_FOUND);
}

static CMPICount arrayGetSize(CMPIArray* eArray, CMPIStatus* rc) {
   CMPIData *dta=(CMPIData*)eArray->hdl;
   if (rc) CMSetStatus(rc,CMPI_RC_OK);
   return dta->value.uint32;
}

static CMPIType arrayGetType(CMPIArray* eArray, CMPIStatus* rc) {
   CMPIData *dta=(CMPIData*)eArray->hdl;
   if (rc) CMSetStatus(rc,CMPI_RC_OK);
   return dta->type;
}

static CMPIArrayFT array_FT={
     CMPICurrentVersion,
     arrayRelease,
     arrayClone,
     arrayGetSize,
     arrayGetType,
     arrayGetElementAt,
     arraySetElementAt,
};

CMPIArrayFT *CMPI_Array_Ftab=&array_FT;

PEGASUS_NAMESPACE_END




