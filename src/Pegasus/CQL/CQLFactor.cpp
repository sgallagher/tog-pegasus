#include "CQLFactor.h"
#include <Pegasus/CQL/CQLExpression.h>
#include <Pegasus/CQL/CQLFunction.h>
PEGASUS_NAMESPACE_BEGIN

#define PEGASUS_ARRAY_T CQLFactor
#include <Pegasus/Common/ArrayImpl.h>
#undef PEGASUS_ARRAY_T

//##ModelId=40FC34920351
CQLFactor::CQLFactor(const CQLFactor& inCQLFact)
{
   _CQLVal = inCQLFact._CQLVal;
   _CQLFunct = inCQLFact._CQLFunct;
   _CQLExp = inCQLFact._CQLExp;
   _invert = inCQLFact._invert;
}

CQLFactor::CQLFactor(CQLValue inCQLVal)
{
   _CQLVal = inCQLVal;
}

//##ModelId=40FC34BC0061
CQLFactor::CQLFactor(CQLExpression& inCQLExp)
{
   _CQLExp = new CQLExpression(inCQLExp);
}

//##ModelId=40FC34E30391
CQLFactor::CQLFactor(CQLFunction inCQLFunc)
{
   _CQLFunct = new CQLFunction(inCQLFunc);
}

//##ModelId=40FC33B70262
CQLValue CQLFactor::getValue()
{
   return _CQLVal;
}

CQLValue CQLFactor::resolveValue(CIMInstance CI, QueryContext& QueryCtx)
{
   _CQLVal.resolve(CI,QueryCtx);
   return _CQLVal;
}

Boolean CQLFactor::isSimpleValue()
{
   return (_CQLExp == NULL && _CQLFunct == NULL);
}

CQLFunction CQLFactor::getCQLFunction()
{
   return *_CQLFunct;
}

CQLExpression CQLFactor::getCQLExpression()
{
   return *_CQLExp;
}

String CQLFactor::toString()
{
   if(_CQLFunct != NULL)
   {
      _CQLFunct->toString();
   }
   else if(_CQLExp != NULL)
   {
      _CQLExp->toString();
   }
   else 
   {
      _CQLVal.toString();
   }
}

void CQLFactor::applyScopes(Array<CQLScope> inScopes)
{
   // TODO:
   return;
}


PEGASUS_NAMESPACE_END
