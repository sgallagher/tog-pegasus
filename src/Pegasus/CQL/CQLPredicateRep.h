#ifndef Pegasus_CQLPredicateRep_h
#define Pegasus_CQLPredicateRep_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/CQL/CQLSimplePredicate.h>
#include <Pegasus/CQL/CQLScope.h>
#include <Pegasus/CQL/Linkage.h>


PEGASUS_NAMESPACE_BEGIN

class PEGASUS_CQL_LINKAGE QueryContext;
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
    CQLPredicateRep(){}
    
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

    Boolean isTerminal();

    Boolean getInverted();

    void setInverted();

    /** Appends a predicate to the predicate array. This method should only
            be called by Bison.
        */
    void appendPredicate(CQLPredicate inPredicate, BooleanOpType inBooleanOperator);

    void appendPredicate(CQLSimplePredicate inSimplePredicate, BooleanOpType inBooleanOperator);
  
    Array<CQLPredicate> getPredicates();
  
    CQLSimplePredicate getSimplePredicate();

    Array<BooleanOpType> getOperators();

    Array<CQLScope> getScopes();

    void applyScopes(Array<CQLScope> & inScopes);

    Boolean isSimple();

    Boolean isSimpleValue();
    String toString();
    friend class CQLFactory;
  private:
    
    
   BooleanOpType _booleanOpType;

   Array<CQLPredicate> _predicates;
   CQLSimplePredicate _simplePredicate;
   Array<BooleanOpType> _operators;

    Boolean _invert;

    /**  If set to TRUE, then the CQLPredicate contains no other CQLPredicates.
          If set to FALSE, then the CQLPredicate only contains other CQLPredicates
    that need tobe evaluated.
      */
    Boolean _terminal;

};

PEGASUS_NAMESPACE_END

#endif 
