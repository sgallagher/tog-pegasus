#ifndef Pegasus_CQLTerm_h
#define Pegasus_CQLTerm_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/CQL/CQLValue.h>
#include <Pegasus/CQL/Linkage.h>
#include <Pegasus/CQL/CQLFactor.h>

PEGASUS_NAMESPACE_BEGIN

class PEGASUS_CQL_LINKAGE CQLFactory;
class PEGASUS_CQL_LINKAGE CQLTermRep;
class PEGASUS_CQL_LINKAGE QueryContext;
/** enum of multiply, divide and concatenation operators
    
       The enum is private, but the definition is public.
      */
    enum FactorOpType { mult, divide, concat };

#ifndef PEGASUS_ARRAY_T
#define PEGASUS_ARRAY_T FactorOpType
#include <Pegasus/Common/ArrayInter.h>
#undef PEGASUS_ARRAY_T
#endif

/** The CQLTerm class encapsulates each CQL term in a CQL expression.  

This class contains an array of CQLFactor objects and an array of CQLOperator
objects.
  */
// 
/**
  The CQLTerm class encapsulates a generic CQL term to make it 
   easier to break into pieces (factors) and process the term.  

   A CQL term is made up of factors and operators. 
   For example,  'A * B' is a CQLTerm, where 'A' and 'B' are factors, and '*'
is an operator on a factor.

   There must be exactly one more factor than there are operators.
  */
class PEGASUS_CQL_LINKAGE CQLTerm
{
  public:
   CQLTerm();
    /** constructor takes one CQLFactor object.
      */
    CQLTerm(CQLFactor theFactor);
    CQLTerm(const CQLTerm& inTerm);

    ~CQLTerm();

    /**  the getValue method evaluates the CQL term and returns the value.
          Any property that need to be resolved into a value is taken from the
    CIMInstance.
      */
    CQLValue resolveValue(CIMInstance CI, QueryContext& QueryCtx);

    /** The appendOperation is used by Bison.
          It is invoked 0 or more times for the CQLTerm, and
          when invoked will always pass in an integer that is the Factor operation
          type and a CQLFactor object.
      */
    void appendOperation(FactorOpType inFactorOpType, CQLFactor inFactor);

   String toString();
   Boolean isSimple();
   Boolean isSimpleValue();
   Array<CQLFactor> getFactors();
   Array<FactorOpType> getOperators();
   void applyScopes(Array<CQLScope> inScope); 
   friend class CQLFactory;

  private:
    
   CQLTermRep *_rep;
};

#ifndef PEGASUS_ARRAY_T
#define PEGASUS_ARRAY_T CQLTerm
#include <Pegasus/Common/ArrayInter.h>
#undef PEGASUS_ARRAY_T
#endif

PEGASUS_NAMESPACE_END

#endif 
