#ifndef Pegasus_CQLSimplePredicate_h 
#define Pegasus_CQLSimplePredicate_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/CQL/Linkage.h>
#include <Pegasus/Common/CIMInstance.h>

PEGASUS_NAMESPACE_BEGIN

class PEGASUS_CQL_LINKAGE CQLExpression;
class PEGASUS_CQL_LINKAGE CQLFactory;
class PEGASUS_CQL_LINKAGE CQLSimplePredicateRep;
class PEGASUS_CQL_LINKAGE QueryContext;

	enum ExpressionOpType { LT, GT, EQ, LE, GE, NE, IS_NULL, IS_NOT_NULL, ISA, LIKE };

/** 
   */
class CQLSimplePredicate
{
  public:
    CQLSimplePredicate():_rep(0){}

    CQLSimplePredicate(const CQLExpression& inExpression);

    CQLSimplePredicate(const CQLExpression& inExpression, ExpressionOpType inOperator);

    CQLSimplePredicate(const CQLExpression& leftSideExpression, const CQLExpression& rightSideExpression, ExpressionOpType inOperator);

     CQLSimplePredicate(const CQLSimplePredicate& inSimplePredicate);

    ~CQLSimplePredicate();

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

    CQLExpression getLeftExpression();

    CQLExpression getRightExpression();

    enum ExpressionOpType getOperation();

    String toString();

    friend class CQLFactory;
  private:
    CQLSimplePredicateRep *_rep;

};
PEGASUS_NAMESPACE_END

#endif 
