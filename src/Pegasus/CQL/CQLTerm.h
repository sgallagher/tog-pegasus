#ifndef CQLTERM_H_HEADER_INCLUDED_BEE59E28
#define CQLTERM_H_HEADER_INCLUDED_BEE59E28

#include <Pegasus/Common/Config.h>
#include <Pegasus/CQL/CQLValue.h>
#include <Pegasus/CQL/Linkage.h>
#include <Pegasus/CQL/CQLFactor.h>

#define MAXFACTORS 50
PEGASUS_NAMESPACE_BEGIN

//class PEGASUS_CQL_LINKAGE CQLFactor;

/** enum of multiply, divide and concatenation operators
    
       The enum is private, but the definition is public.
      */
    //##ModelId=40FC31A3026D
    enum FactorOpType { mult, div, concat };

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
//##ModelId=40FC317F0302
class PEGASUS_CQL_LINKAGE CQLTerm
{
  public:
   CQLTerm(){}
    /** constructor takes one CQLFactor object.
      */
    //##ModelId=40FD3277018E
    CQLTerm(CQLFactor theFactor);

    ~CQLTerm(){}

    /**  the getValue method evaluates the CQL term and returns the value.
          Any property that need to be resolved into a value is taken from the
    CIMInstance.
      */
    //##ModelId=40FC32680152
    CQLValue getValue(CIMInstance CI, QueryContext& QueryCtx);

    /** The appendOperation is used by Bison.
          It is invoked 0 or more times for the CQLTerm, and
          when invoked will always pass in an integer that is the Factor operation
          type and a CQLFactor object.
      */
    //##ModelId=40FC32BF038F
    void appendOperation(FactorOpType inFactorOpType, CQLFactor inFactor);

  private:
    
    FactorOpType _FactorOpType;

    //##ModelId=40FC317F0304
    FactorOpType _FactorOperators[MAXFACTORS];

    //##ModelId=40FC317F0303
    Array<CQLFactor> _Factors;

};

#ifndef PEGASUS_ARRAY_T
#define PEGASUS_ARRAY_T CQLTerm
#include <Pegasus/Common/ArrayInter.h>
#undef PEGASUS_ARRAY_T
#endif



PEGASUS_NAMESPACE_END

#endif /* CQLTERM_H_HEADER_INCLUDED_BEE59E28 */
