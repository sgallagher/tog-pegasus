#ifndef CQLEXPRESSION_H_HEADER_INCLUDED_BEE5929F
#define CQLEXPRESSION_H_HEADER_INCLUDED_BEE5929F

#include <Pegasus/Common/Config.h>
#include <Pegasus/CQL/CQLValue.h>


#include <Pegasus/CQL/Linkage.h>
#include <Pegasus/CQL/CQLTerm.h>
#define MAXFACTORS 50
PEGASUS_NAMESPACE_BEGIN


/**  The enum is private, but the definition is public.
      */
    //##ModelId=40FC040E0079
    enum TermOpType {plus,minus};

#ifndef PEGASUS_ARRAY_T
#define PEGASUS_ARRAY_T TermOpType
#include <Pegasus/Common/ArrayInter.h>
#undef PEGASUS_ARRAY_T
#endif

/**
  The CQLExpression class encapsulates a generic CQL expression to make it
easier to break into components and process the expression.  

   A CQL expression is made up of terms and operators. 
   For example,  'A + B' is a CQLExpression, where 'A' and 'B' are terms, and
'+' is an operator.

   There must be exactly one more term than there are operators.
  */
//##ModelId=40FC03230150
class PEGASUS_CQL_LINKAGE CQLExpression
{
  public:
   CQLExpression(){}
    /** constructor takes one CQLTerm object.
      */
    //##ModelId=40FD308002EE
    CQLExpression(CQLTerm& theTerm);
   CQLExpression(const CQLExpression& inExpress);
    /**  the getValue method evaluates the expression and returns the value.
          Any property that need to be resolved into a value is taken from the
    CIMInstance.
      */
    ~CQLExpression(){}

    //##ModelId=40FC0323015B
    CQLValue resolveValue(CIMInstance CI, QueryContext& QueryCtx);

    /** The appendOperation is used by Bison.
          It is invoked 0 or more times for the CQLExpression, and
          when invoked will always pass in an integer that is the Term operation
          type and a CQLTerm object.
      */
    //##ModelId=40FD30BB03C3
    void appendOperation(TermOpType theTermOpType, CQLTerm& theTerm);

   String toString();
   Boolean isSimpleValue();
   Array<CQLTerm> getTerms();
   Array<TermOpType> getOperators();
   void applyScopes(Array<CQLScope> inScopes);

  private:
   

    /**  The _TermOperators member variable is an 
           array of operators that are valid to operate on Terms in a CQL
    expression. 
           Valid operators include concatentation, plus and minus.
    
           The array is ordered according to the operation from left to right.
      */
    //##ModelId=40FC036B036A
    
    Array<TermOpType> _TermOperators;

    /**  The _CQLTerms member variable is an 
           array of operands that are valid in a CQL expression. 
    
           The array is ordered according to the operation from left to right.
      */
    //##ModelId=40FC2F2C0341
    Array<CQLTerm> _CQLTerms;

};

#ifndef PEGASUS_ARRAY_T
#define PEGASUS_ARRAY_T CQLExpression
#include <Pegasus/Common/ArrayInter.h>
#undef PEGASUS_ARRAY_T
#endif

PEGASUS_NAMESPACE_END


#endif /* CQLEXPRESSION_H_HEADER_INCLUDED_BEE5929F */
