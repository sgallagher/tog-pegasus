#ifndef Pegasus_CQLExpression_h
#define Pegasus_CQLExpression_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/CQL/CQLValue.h>
#include <Pegasus/CQL/Linkage.h>
#include <Pegasus/CQL/CQLTerm.h>

PEGASUS_NAMESPACE_BEGIN

class PEGASUS_CQL_LINKAGE CQLFactory;
class PEGASUS_CQL_LINKAGE CQLExpressionRep;
class PEGASUS_CQL_LINKAGE QueryContext;

/**  The enum is private, but the definition is public.
      */
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

class PEGASUS_CQL_LINKAGE CQLExpression
{
  public:
   CQLExpression();
    /** constructor takes one CQLTerm object.
      */
    CQLExpression(CQLTerm& theTerm);
   CQLExpression(const CQLExpression& inExpress);
    /**  the getValue method evaluates the expression and returns the value.
          Any property that need to be resolved into a value is taken from the
    CIMInstance.
      */
    ~CQLExpression();

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
   CQLExpression& operator=(const CQLExpression& rhs);
   Boolean operator==(const CQLExpression& expr);
   Boolean operator!=(const CQLExpression& expr);
   friend class CQLFactory;

  private:

	CQLExpressionRep *_rep;   
};

#ifndef PEGASUS_ARRAY_T
#define PEGASUS_ARRAY_T CQLExpression
#include <Pegasus/Common/ArrayInter.h>
#undef PEGASUS_ARRAY_T
#endif

PEGASUS_NAMESPACE_END


#endif 
