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

#include "CMPI_SelectExp.h"
#include "CMPI_Ftabs.h"
#include "CMPI_Value.h"
#include "CMPI_String.h"

PEGASUS_USING_STD;
PEGASUS_NAMESPACE_BEGIN

static CMPIStatus selxRelease(CMPISelectExp* eSx) {
   CMReturn(CMPI_RC_OK);
}

static CMPISelectExp* selxClone(CMPISelectExp* eSx, CMPIStatus* rc) {
      if (rc) CMSetStatus(rc,CMPI_RC_ERR_NOT_SUPPORTED);
      return NULL;
}

static CMPIBoolean selxEvaluate(CMPISelectExp* eSx, CMPIInstance* inst, CMPIStatus* rc) {
      return false;
}

static CMPIString* selxGetString(CMPISelectExp* eSx, CMPIStatus* rc) {
   CMPI_SelectExp *sx=(CMPI_SelectExp*)eSx;
   return string2CMPIString(sx->cond);
}

static CMPISelectCond* selxGetDOC(CMPISelectExp* eSx, CMPIStatus* rc) {
   CMPI_SelectExp *sx=(CMPI_SelectExp*)eSx;
   if (sx->dnf==NULL) {
      sx->dnf=new CMPI_Wql2Dnf(String(sx->cond),String::EMPTY);
      sx->tableau=sx->dnf->getTableau();
   }
   CMPISelectCond *sc=(CMPISelectCond*)new CMPI_SelectCond(sx->tableau,0);
   if (rc) CMSetStatus(rc,CMPI_RC_OK);
   return sc; 
}
/*
static CMPISelectCond* selxGetCOD(CMPISelectExp* eSx, CMPIStatus* rc) {
      return NULL;
}
*/
static CMPISelectExpFT selx_FT={
     CMPICurrentVersion,
     selxRelease,
     selxClone,
     selxEvaluate,
     selxGetString,
     selxGetDOC,
     NULL  //selxGetCOD
};

CMPISelectExpFT *CMPI_SelectExp_Ftab=&selx_FT;

CMPI_SelectExp::CMPI_SelectExp(const OperationContext& ct, String cond_, String lang_)
  : ctx(ct), cond(cond_), lang(lang_) {
   props=NULL;
   ft=CMPI_SelectExp_Ftab;
   dnf=NULL;
}

PEGASUS_NAMESPACE_END
