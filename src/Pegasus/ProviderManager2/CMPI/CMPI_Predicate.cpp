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

#include "CMPI_Predicate.h"
#include "CMPI_Ftabs.h"
#include "CMPI_Value.h"
#include "CMPI_String.h"

PEGASUS_USING_STD;
PEGASUS_NAMESPACE_BEGIN

CMPIStatus prdRelease(CMPIPredicate* sc) {
   CMReturn(CMPI_RC_OK);
}

CMPIPredicate* prdClone(CMPIPredicate* ePrd, CMPIStatus* rc) {
      if (rc) CMSetStatus(rc,CMPI_RC_ERR_NOT_SUPPORTED);
      return NULL;
}
     
CMPIStatus prdGetData(CMPIPredicate* ePrd, CMPIType* type,
               CMPIPredOp* op, CMPIString** lhs, CMPIString** rhs) {
     CMPI_Predicate *prd=(CMPI_Predicate*)ePrd;
     String o1,o2;
     CMPIPredOp o;
     CMPIType t;
     prd->term->toStrings(t,o,o1,o2);
     if (type) *type=t;
     if (op) *op=o;
     if (lhs) *lhs=string2CMPIString(o1);
     if (rhs) *rhs=string2CMPIString(o2);
     CMReturn(CMPI_RC_OK);
}

int prdEvaluate(CMPIPredicate* pr, CMPIValue* value,
               CMPIType type, CMPIStatus* rc) {
      return 0;	       
}

static CMPIPredicateFT prd_FT={ 
     CMPICurrentVersion,
     prdRelease,
     prdClone,
     prdGetData,
     prdEvaluate,
 };

CMPIPredicateFT *CMPI_Predicate_Ftab=&prd_FT;

CMPI_Predicate::CMPI_Predicate(const term_el* t)
  : term(t) {
   ft=CMPI_Predicate_Ftab;
}


PEGASUS_NAMESPACE_END
