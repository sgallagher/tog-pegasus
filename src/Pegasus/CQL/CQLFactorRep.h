#ifndef Pegasus_CQLFactorRep_h
#define Pegasus_CQLFactorRep_h
//%2004////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2004 BMC Software; Hewlett-Packard Development Company, L. P.;
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
// Author: Dave Rosckes (rosckes@us.ibm.com)
//
// Modified By:
//
//%/////////////////////////////////////////////////////////////////////////////

#include <Pegasus/Common/Config.h>
#include <Pegasus/CQL/Linkage.h>
#include <Pegasus/Common/CIMInstance.h>
#include <Pegasus/CQL/CQLExpression.h>
#include <Pegasus/CQL/CQLFunction.h>

PEGASUS_NAMESPACE_BEGIN
class PEGASUS_CQL_LINKAGE CQLFactory;
//class PEGASUS_CQL_LINKAGE CQLExpression;
//class PEGASUS_CQL_LINKAGE CQLFunction;
class PEGASUS_CQL_LINKAGE CQLValue;
class PEGASUS_CQL_LINKAGE CQLScope;
class PEGASUS_CQL_LINKAGE QueryContext;

/**  The CQLFactor could contain a CQLValue, CQLExpression or CQLFunction.
        This class evaluates the object to get a CQLValue.
       If it contains a CQLValue, it will resolve the CQLValue. 
  */

class PEGASUS_CQL_LINKAGE CQLFactorRep
{
  public:
   CQLFactorRep():_simpleValue(false){}

    CQLFactorRep(CQLValue& inCQLVal);

    CQLFactorRep(CQLExpression& inCQLExp);

    CQLFactorRep(CQLFunction& inCQLFunc);
    CQLFactorRep(const CQLFactorRep& inCQLFact);

    ~CQLFactorRep(){}

    /** 
           The CQLFactor could contain a CQLValue, CQLExpression or CQLFunction.
           This method evaluates the object to get a CQLValue.
           If it contains a CQLValue, it will resolve the CQLValue by calling the
    resolve method
           on the CQLValue object. 
    
           If it contains a CQLFunction this method will invoke the getValue method
    on 
           CQLFunction, and the returned value will already be resolved.
    
           If it contains a CQLExpression this method will invoke the getValue
    method on 
           CQLExpression, and the returned value will already be resolved.
    
      */

   CQLValue resolveValue(CIMInstance CI, QueryContext& QueryCtx);
   Boolean isSimpleValue();
   CQLValue getValue();
   CQLFunction getCQLFunction();
   CQLExpression getCQLExpression();
   String toString();
   void applyScopes(Array<CQLScope> inScopes);
   Boolean operator==(const CQLFactorRep& rep);
   Boolean operator!=(const CQLFactorRep& rep);
   friend class CQLFactory;
  private:

    CQLExpression _CQLExp;

    CQLValue _CQLVal;

    CQLFunction _CQLFunct;

    /** if _invert is TRUE, multiply by -1 to invert the value.
      */

    Boolean _invert;
    Boolean _simpleValue;
};
/*
#ifndef PEGASUS_ARRAY_T
#define PEGASUS_ARRAY_T CQLFactorRep
#include <Pegasus/Common/ArrayInter.h>
#undef PEGASUS_ARRAY_T
#endif
*/
PEGASUS_NAMESPACE_END

#endif /* CQLFACTOR_H_HEADER */
