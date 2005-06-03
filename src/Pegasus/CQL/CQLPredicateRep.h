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
// Authors: David Rosckes (rosckes@us.ibm.com)
//          Bert Rivero (hurivero@us.ibm.com)
//          Chuck Carmack (carmack@us.ibm.com)
//          Brian Lucier (lucier@us.ibm.com)
//
// Modified By: David Dillard, VERITAS Software Corp.
//                  (david.dillard@veritas.com)
//              Vijay Eli, IBM (vijayeli@in.ibm.com) bug#3590
//
//%/////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_CQLPredicateRep_h
#define Pegasus_CQLPredicateRep_h

#include <Pegasus/CQL/Linkage.h>
#include <Pegasus/CQL/CQLSimplePredicate.h>


PEGASUS_NAMESPACE_BEGIN

class PEGASUS_QUERYCOMMON_LINKAGE QueryContext;
class PEGASUS_CQL_LINKAGE CQLFactory;

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
class PEGASUS_CQL_LINKAGE CQLPredicateRep
{
  public:
    CQLPredicateRep();

    CQLPredicateRep(const CQLSimplePredicate & inSimplePredicate, Boolean inVerted = false);

    CQLPredicateRep(const CQLPredicate & inPredicate, Boolean inVerted = false);

    CQLPredicateRep(const CQLPredicateRep* rep);

   ~CQLPredicateRep(){}
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

    Boolean getInverted()const;

    void setInverted(Boolean invert);

    /** Appends a predicate to the predicate array. This method should only
            be called by Bison.
        */
    void appendPredicate(const CQLPredicate& inPredicate);

    void appendPredicate(const CQLPredicate& inPredicate, BooleanOpType inBooleanOperator);

    Array<CQLPredicate> getPredicates()const;

    CQLSimplePredicate getSimplePredicate()const;

    Array<BooleanOpType> getOperators()const;

    void applyContext(const QueryContext& queryContext);

    Boolean isSimple()const;

    Boolean isSimpleValue()const;
    String toString()const;

    friend class CQLFactory;
  private:

   Array<CQLPredicate> _predicates;
   CQLSimplePredicate _simplePredicate;
   Array<BooleanOpType> _operators;

   Boolean _invert;

};

PEGASUS_NAMESPACE_END

#endif
