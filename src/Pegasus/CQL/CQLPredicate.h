#ifndef CQLPREDICATE_H_HEADER_INCLUDED_BEE59A50
#define CQLPREDICATE_H_HEADER_INCLUDED_BEE59A50


#include <Pegasus/Common/Config.h>
#include <Pegasus/CQL/CQLExpression.h>


#include <Pegasus/CQL/Linkage.h>


PEGASUS_NAMESPACE_BEGIN



// enum { <, >, =, <=, >=, <>, IS NULL, IS NOT NULL, ISA, LIKE }
    //##ModelId=40FC35A1023A
    enum  ExpressionOpType { LT, GT, EQ, LTE, GTE, NE, IS_NULL, IS_NOT_NULL, ISA, LIKE };
// enum { AND, OR }
    //##ModelId=40FD705600B3
    enum BooleanOpType { AND, OR } ;
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
//##ModelId=40FC35A10230
class PEGASUS_CQL_LINKAGE CQLPredicate
{
  public:
   CQLPredicate(){}
    //##ModelId=40FD61E30034
    CQLPredicate(const CQLExpression& inExpression, ExpressionOpType inOperator);

    //##ModelId=40FD6290025E
    CQLPredicate(const CQLExpression& leftSideExpression, const CQLExpression& rightSideExpression, ExpressionOpType inOperator);

    //##ModelId=40FD725B025F
    CQLPredicate(CQLPredicate inPredicate, Boolean inInvert);

   ~CQLPredicate(){}
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
    //##ModelId=40FC365903BF
    Boolean evaluate(CIMInstance CI, QueryContext& QueryCtx);

    /** Appends a predicate to the predicate array. This method should only
            be called by Bison.
        */
    //##ModelId=40FD6FF202ED
    void appendPredicate(
        // CQLOperation is an enum similar to the WQLOperation enum in CVS:
        // enum CQLOperation
        // {
        //     <,
        //     >,
        //     =,
        //    >=,
        //    <=,
        //     +,
        //     -,
        //     *,
        //     /,
        //     CQL_IS_NULL,
        //     CQL_IS_NOT_NULL,
        //     CQL_IS_A,
        //     CQL_LIKE
        // };
        CQLPredicate inPredicate, BooleanOpType inBooleanOperator);

  private:
    
   ExpressionOpType _expressionOpType;
    
   BooleanOpType _booleanOpType;
    //##ModelId=40FC35F801B0
    CQLExpression* _leftSide;

    //##ModelId=40FC36270224
    CQLExpression* _rightSide;

    /** 
    TODO:  document this array.   Still need to think about the DNF and how to
    correlate operations and predicates.
       */
    //##ModelId=40FD6E330153
    //Array<CQLPredicate> _predicates;

    /**  If set to TRUE, then the CQLPredicate will be "NOT"d after it is
    evaluated.
          If set to FALSE, then the CQLPredicate will not be "NOT"d after it is
    evaluated.
      */
    //##ModelId=40FD70AA02BA
    Boolean _invert;

    /**  If set to TRUE, then the CQLPredicate contains no other CQLPredicates.
          If set to FALSE, then the CQLPredicate only contains other CQLPredicates
    that need tobe evaluated.
      */
    //##ModelId=40FD715D0190
    Boolean _terminal;

};

 
#ifndef PEGASUS_ARRAY_T
#define PEGASUS_ARRAY_T CQLPredicate
#include <Pegasus/Common/ArrayInter.h>
#undef PEGASUS_ARRAY_T
#endif

PEGASUS_NAMESPACE_END

#endif /* CQLPREDICATE_H_HEADER_INCLUDED_BEE59A50 */
