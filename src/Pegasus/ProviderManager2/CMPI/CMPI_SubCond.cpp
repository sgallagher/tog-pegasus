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

#include "CMPI_SubCond.h"
#include "CMPI_Ftabs.h"
#include "CMPI_Value.h"
#include "CMPI_String.h"

PEGASUS_USING_STD;
PEGASUS_NAMESPACE_BEGIN

CMPIStatus sbcRelease(CMPISubCond* sc) {
   CMReturn(CMPI_RC_OK);
}

CMPISubCond* sbcClone(CMPISubCond* eSc, CMPIStatus* rc) {
      if (rc) CMSetStatus(rc,CMPI_RC_ERR_NOT_SUPPORTED);
      return NULL;
}

CMPICount sbcGetCount(CMPISubCond* eSbc, CMPIStatus* rc) {
    CMPI_SubCond *sbc=(CMPI_SubCond*)eSbc;
    if (rc) CMSetStatus(rc,CMPI_RC_OK);
    return sbc->row->size();
}

CMPIPredicate* sbcGetPredicateAt(CMPISubCond* eSbc, unsigned int index, CMPIStatus* rc) {
    CMPI_SubCond *sbc=(CMPI_SubCond*)eSbc;
    if (index<=sbc->row->size()) {
       const term_el *term=sbc->row[index].getData();
        std::cout<<"--- "<<term->opn1.toString()<<std::endl;
       CMPIPredicate *prd=(CMPIPredicate*)new CMPI_Predicate(term);
       if (rc) CMSetStatus(rc,CMPI_RC_OK);
       return prd;
    }   
    if (rc) CMSetStatus(rc,CMPI_RC_ERR_FAILED);
    return NULL; 
}

CMPIPredicate* sbcGetPredicate(CMPISubCond* eSbc, char* name, CMPIStatus* rc) {
    CMPI_SubCond *sc=(CMPI_SubCond*)eSbc;
    return NULL;
}

static CMPISubCondFT scnd_FT={ 
     CMPICurrentVersion,
     sbcRelease,
     sbcClone,
     sbcGetCount,
     sbcGetPredicateAt,
     sbcGetPredicate
 };

CMPISubCondFT *CMPI_SubCond_Ftab=&scnd_FT;

CMPI_SubCond::CMPI_SubCond(const TableauRow* tblor)
  : row(tblor) {
   ft=CMPI_SubCond_Ftab;
}


PEGASUS_NAMESPACE_END
