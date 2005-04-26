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

#include "CMPI_SelectCond.h"
#include "CMPI_Ftabs.h"
#include "CMPI_Value.h"
#include "CMPI_String.h"

PEGASUS_USING_STD;
PEGASUS_NAMESPACE_BEGIN

extern "C" {

   CMPIStatus scndRelease(CMPISelectCond* eSc) {
	  CMPI_SelectCond *sc = (CMPI_SelectCond*)eSc->hdl;
      if (sc) {
		 CMPI_SelectCondData *data = (CMPI_SelectCondData *)sc->priv;
		 if (data) 
		 	delete data;
         delete sc;
         ((CMPI_Object*)eSc)->unlinkAndDelete();
      }
 
      CMReturn(CMPI_RC_OK);
   }

   CMPISelectCond* scndClone(const CMPISelectCond* eSc, CMPIStatus* rc) {
         if (rc) CMSetStatus(rc,CMPI_RC_ERR_NOT_SUPPORTED);
         return NULL;
   }

   CMPICount scndGetCountAndType(const CMPISelectCond* eSc, int* type, CMPIStatus* rc) {

	  CMPI_SelectCond *sc=(CMPI_SelectCond*)eSc;
	  CMPI_SelectCondData *data = (CMPI_SelectCondData *)sc->priv;

	  if (data)
	   {
      		if (type!=NULL) *type=data->type;
      		if (rc) CMSetStatus(rc,CMPI_RC_OK);
      		return data->tableau->size();
		}
	  return 0;
   }

   CMPISubCond* scndGetSubCondAt(const CMPISelectCond* eSc, unsigned int index, CMPIStatus* rc) {
      CMPI_SelectCond *sc=(CMPI_SelectCond*)eSc;
      CMPI_SelectCondData *data = (CMPI_SelectCondData *)sc->priv;
	  if (data)
		{
      	  if (index<=data->tableau->size()) {
         	const CMPI_TableauRow *row=(data->tableau->getData())+index;

         	CMPISubCond *sbc=(CMPISubCond*)new CMPI_SubCond(row);
			CMPI_Object *obj = new CMPI_Object(sbc);
			obj->priv = ((CMPI_SubCond *)(sbc))->priv;
         	if (rc) CMSetStatus(rc,CMPI_RC_OK);
         	return reinterpret_cast<CMPISubCond *>(obj);
		}
      } else  
      	if (rc) CMSetStatus(rc,CMPI_RC_ERR_FAILED);
      return NULL; 
   }

}

static CMPISelectCondFT scnd_FT={
     CMPICurrentVersion,
     scndRelease,
     scndClone,
     scndGetCountAndType,
     scndGetSubCondAt,
 };

CMPISelectCondFT *CMPI_SelectCond_Ftab=&scnd_FT;

CMPI_SelectCondData::CMPI_SelectCondData(CMPI_Tableau *tblo, int t)
  : tableau(tblo), type(t) { }

CMPI_SelectCond::CMPI_SelectCond(CMPI_Tableau* tblo, int t)
  {
   priv = new CMPI_SelectCondData(tblo, t);
   ft=CMPI_SelectCond_Ftab;
}


PEGASUS_NAMESPACE_END
