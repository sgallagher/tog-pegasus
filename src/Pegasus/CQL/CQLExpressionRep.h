#ifndef Pegasus_CQLExpressionRep_h
#define Pegasus_CQLExpressionRep_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/CQL/CQLValue.h>
#include <Pegasus/CQL/Linkage.h>
#include <Pegasus/CQL/CQLTerm.h>

PEGASUS_NAMESPACE_BEGIN

class PEGASUS_CQL_LINKAGE CQLFactory;
class PEGASUS_CQL_LINKAGE QueryContext;

/**
  The CQLExpression class encapsulates a generic CQL expression to make it
easier to break into components and process the expression.  

   A CQL expression is made up of terms and operators. 
   For example,  'A + B' is a CQLExpression, where 'A' and 'B' are terms, and
'+' is an operator.

   There must be exactly one more term than there are operators.
  */
//##ModelId=40FC03230150
class PEGASUS_CQL_LINKAGE CQLExpressionRep
{
  public:
   CQLExpressionRep(){}
    /** constructor takes one CQLTerm object.
      */
    CQLExpressionRep(CQLTerm& theTerm);
   CQLExpressionRep(const CQLExpressionRep& rep);
    /**  the getValue method evaluates the expression and returns the value.
          Any property that need to be resolved into a value is taken from the
    CIMInstance.
      */
    ~CQLExpressionRep();

    CQLValue resolveValue(CIMInstance CI, QueryContext& QueryCtx);

    /** The appendOperation is used by Bison.
          It is invoked 0 or more times for the CQLExpression, and
          when invoked will always pass in an integer that is the Term operation
          type and a CQLTerm object.
      */
    void appendOperation(TermOpType theTermOpType, CQLTerm& theTerm);

   String toString();
   Boolean isSimple();
   Boolean isSimpleValue();
   Array<CQLTerm> getTerms();
   Array<TermOpType> getOperators();
   void applyScopes(Array<CQLScope> inScopes);
   Boolean operator==(const CQLExpressionRep& rep);
   Boolean operator!=(const CQLExpressionRep& rep);
   friend class CQLFactory;

  private:
   

    /**  The _TermOperators member variable is an 
           array of operators that are valid to operate on Terms in a CQL
    expression. 
           Valid operators include concatentation, plus and minus.
    
           The array is ordered according to the operation from left to right.
      */
    
    Array<TermOpType> _TermOperators;

    /**  The _CQLTerms member variable is an 
           array of operands that are valid in a CQL expression. 
    
           The array is ordered according to the operation from left to right.
      */
    Array<CQLTerm> _CQLTerms;

};

PEGASUS_NAMESPACE_END


#endif /* CQLEXPRESSION_H_HEADER_INCLUDED_BEE5929F */
