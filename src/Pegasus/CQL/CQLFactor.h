#ifndef CQLFACTOR_H_HEADER_INCLUDED_BEE5D279
#define CQLFACTOR_H_HEADER_INCLUDED_BEE5D279

#include <Pegasus/Common/Config.h>
#include <Pegasus/CQL/CQLValue.h>
//#include <Pegasus/CQL/CQLExpression.h>
//#include <Pegasus/CQL/CQLFunction.h>
#include <Pegasus/CQL/Linkage.h>

PEGASUS_NAMESPACE_BEGIN

class PEGASUS_CQL_LINKAGE CQLExpression;
class PEGASUS_CQL_LINKAGE CQLFunction;

/**  The CQLFactor could contain a CQLValue, CQLExpression or CQLFunction.
       This class evaluates the object to get a CQLValue.
       If it contains a CQLValue, it will resolve the CQLValue. 
  */
//##ModelId=40FC33B7024E
class PEGASUS_CQL_LINKAGE CQLFactor
{
  public:
   CQLFactor(){}
    //##ModelId=40FC34920351
    CQLFactor(CQLValue inCQLVal);

    //##ModelId=40FC34BC0061
    CQLFactor(CQLExpression& inCQLExp);

    //##ModelId=40FC34E30391
    CQLFactor(CQLFunction inCQLFunc);

    ~CQLFactor(){}

    /** 
           The CQLFactor could contain a CQLValue, CQLExpression or CQLFunction.
           This method evaluates the object to get a CQLValue.
           If it contains a CQLValue, it will resolve the CQLValue by calling the
    resolve method
           on the CQLValue object. 
    
           If it contains a CQLFunction this method will invoke the getValue method
    on 
           CQLFunction, and the returned value will already be resolved.
    
           If it contains a CQLExpression this method will invoke the getValue
    method on 
           CQLExpression, and the returned value will already be resolved.
    
      */
    //##ModelId=40FC33B70262
    CQLValue getValue(CIMInstance CI, QueryContext& QueryCtx);

  private:
    //##ModelId=40FC33B7024F
    CQLExpression* _CQLExp;

    //##ModelId=40FC33B70258
    CQLValue _CQLVal;

    //##ModelId=40FC34150214
    CQLFunction* _CQLFunct;

    /** if _invert is TRUE, multiply by -1 to invert the value.
      */
    //##ModelId=410516920084
    Boolean _invert;

};

#ifndef PEGASUS_ARRAY_T
#define PEGASUS_ARRAY_T CQLFactor
#include <Pegasus/Common/ArrayInter.h>
#undef PEGASUS_ARRAY_T
#endif

PEGASUS_NAMESPACE_END

#endif /* CQLFACTOR_H_HEADER_INCLUDED_BEE5D279 */
