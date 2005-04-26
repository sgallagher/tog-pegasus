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

#include "CMPI_Predicate.h"
#include "CMPI_Ftabs.h"
#include "CMPI_Value.h"
#include "CMPI_String.h"

PEGASUS_USING_STD;
PEGASUS_NAMESPACE_BEGIN

extern "C" {

   CMPIStatus prdRelease(CMPIPredicate* sc) {
	  CMPI_Predicate *pred = (CMPI_Predicate*)sc->hdl;
      if (pred) {
         delete pred;
         ((CMPI_Object*)sc)->unlinkAndDelete();
      }
      CMReturn(CMPI_RC_OK);
   }

   CMPIPredicate* prdClone(const CMPIPredicate* ePrd, CMPIStatus* rc) {
         if (rc) CMSetStatus(rc,CMPI_RC_ERR_NOT_SUPPORTED);
         return NULL;
   }

   CMPIStatus prdGetData(const CMPIPredicate* ePrd, CMPIType* type,
                  CMPIPredOp* op, CMPIString** lhs, CMPIString** rhs) {
      //CMPI_Predicate *prd=(CMPI_Predicate*)ePrd;
	  const CMPI_Object *obj=reinterpret_cast<const CMPI_Object*>(ePrd);
	  CMPI_term_el *term =  (CMPI_term_el *)obj->priv;
	 
      if (term) 
	   { 
      	String o1,o2;
      	CMPIPredOp o;
      	CMPIType t;
      	term->toStrings(t,o,o1,o2);

      	if (type) *type=t;
      	if (op) *op=o;
      	if (lhs) *lhs=string2CMPIString(o1);
      	if (rhs) *rhs=string2CMPIString(o2);
      	CMReturn(CMPI_RC_OK);
	  }
	  else
		CMReturn(CMPI_RC_ERR_NOT_SUPPORTED);
   }

   int prdEvaluate(CMPIPredicate* pr, CMPIValue* value,
                  CMPIType type, CMPIStatus* rc) {
		 if (rc) CMSetStatus(rc,CMPI_RC_ERR_NOT_SUPPORTED);
         return 0;
   }

  CMPIBoolean prdEvaluateUsingAccessor (const CMPIPredicate*,  CMPIAccessor *f, void *p, CMPIStatus *rc) {

    	 if (rc) CMSetStatus(rc,CMPI_RC_ERR_NOT_SUPPORTED);
         return 0;
  }

}
   
static CMPIPredicateFT prd_FT={
     CMPICurrentVersion,
     prdRelease,
     prdClone,
     prdGetData,
#if defined (CMPI_VER_87) && !defined(CMPI_VER_100)
     prdEvaluate,
#endif
#if defined(CMPI_VER_100)
     prdEvaluateUsingAccessor
#endif
 };

CMPIPredicateFT *CMPI_Predicate_Ftab=&prd_FT;

CMPI_Predicate::CMPI_Predicate(const CMPI_term_el* t)
  : priv((void*)t) {
   ft=CMPI_Predicate_Ftab;
}


PEGASUS_NAMESPACE_END
