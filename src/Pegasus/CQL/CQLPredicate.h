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
// Authors: David Rosckes (rosckes@us.ibm.com)
//          Bert Rivero (hurivero@us.ibm.com)
//          Chuck Carmack (carmack@us.ibm.com)
//          Brian Lucier (lucier@us.ibm.com)
//
// Modified By:
//
//%/////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_CQLPredicate_h
#define Pegasus_CQLPredicate_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/CQL/CQLSimplePredicate.h>
#include <Pegasus/Common/ArrayInternal.h>
#include <Pegasus/CQL/Linkage.h>


#ifdef PEGASUS_USE_EXPERIMENTAL_INTERFACES

PEGASUS_NAMESPACE_BEGIN

    enum BooleanOpType { AND, OR } ;

/*
#ifndef PEGASUS_ARRAY_T
#define PEGASUS_ARRAY_T BooleanOpType
#include <Pegasus/Common/ArrayInter.h>
#undef PEGASUS_ARRAY_T
#endif
*/

class PEGASUS_CQL_LINKAGE CQLPredicateRep;
class PEGASUS_CQL_LINKAGE CQLFactory;
class PEGASUS_QUERYCOMMON_LINKAGE QueryContext;

/** 
   This object is populated by Bison.

   Valid operations for each type of CQLValue are in accordance with the DMTF
CQL Specification.
   
   The CQLPredicate is considered "terminal" if it does not contain any
CQLPredicate objects.
    A 'terminal' predicate can be evaluated to TRUE/FALSE by examining the
CQLExpressions and operator.
    Valid operators are:
            <, >, =, <=, >=, <>, IS NULL, IS NOT NULL, ISA, LIKE

   CQLExpressions: 
      For an expression, CQLExpression::getValue is called and will return a
CQLValue.
      The appropriate operator is then invoked on CQLValue and that operator
function will
      enforce the type restrictions as documented in the DMTF CQL
Specification.
      That operator then determines whether the predicate is TRUE / FALSE.


   CQLPredicates: 
      The CQLPredicate is non-terminal if it contains only CQLPredicate
objects.
      A non-terminal CQLPredicate is evaluated by in turn evaluating the
contained CQLPredicates and
      boolean operator.
     Valid operators are:
              AND, OR

    For the evaluate method on each CQLPredicate. the CQLPredicate is evaluated
to TRUE/FALSE and 
     the result of the evaluation is then applied to the appropriate boolean
operator. 

    The result of the evaluation is and then inverted if the _invert member
variable is set to TRUE
    and then returned to the caller.

   */
class PEGASUS_CQL_LINKAGE CQLPredicate
{
  public:
    CQLPredicate();
    
    CQLPredicate(const CQLSimplePredicate & inSimplePredicate, Boolean inVerted = false);

    CQLPredicate(const CQLPredicate & inPredicate, Boolean inVerted = false);

//   CQLPredicate(const CQLPredicate& inPredicate);

   ~CQLPredicate();
    /**  
      CQLExpressions: 
          For an expression, CQLExpression::getValue is called and will return a
    CQLValue.
          The appropriate operator is then invoked on CQLValue and that operator
    function will
          enforce the type restrictions as documented in the DMTF CQL
    Specification.
          That operator then determines whether the predicate is TRUE / FALSE.
    
    
       CQLPredicates: 
          The CQLPredicate is non-terminal if it contains only CQLPredicate
    objects.
          A non-terminal CQLPredicate is evaluated by in turn evaluating the
    contained CQLPredicates and
          boolean operator.
         Valid operators are:
                  AND, OR
    
         For the evaluate method on each CQLPredicate. the CQLPredicate is
    evaluated to TRUE/FALSE and 
         the result of the evaluation is then applied to the appropriate boolean
    operator. 
    
        The result of the evaluation is and then inverted if the _invert member
    variable is set to TRUE
        and then returned to the caller.
    
      */
    Boolean evaluate(CIMInstance CI, QueryContext& QueryCtx);

    Boolean isTerminal()const;

    Boolean getInverted()const;

    void setInverted();


    void appendPredicate(const CQLPredicate& inPredicate);
    /** Appends a predicate to the predicate array. This method should only
            be called by Bison.
        */
    void appendPredicate(const CQLPredicate& inPredicate, BooleanOpType inBooleanOperator);

    void appendPredicate(CQLSimplePredicate inSimplePredicate, BooleanOpType inBooleanOperator);
  
    Array<CQLPredicate> getPredicates()const;
  
    CQLSimplePredicate getSimplePredicate()const;
    
    Array<BooleanOpType> getOperators()const;

    void applyContext(QueryContext& queryContext);

    Boolean isSimple()const;

    Boolean isSimpleValue()const;

    String toString()const;

    CQLPredicate& operator=(const CQLPredicate& rhs);

    friend class CQLFactory;
  private:
	CQLPredicateRep* _rep;    

};

/*
#ifndef PEGASUS_ARRAY_T
#define PEGASUS_ARRAY_T CQLPredicate
#include <Pegasus/Common/ArrayInter.h>
#undef PEGASUS_ARRAY_T
#endif
*/

PEGASUS_NAMESPACE_END
#endif
#endif 
