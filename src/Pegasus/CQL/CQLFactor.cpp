#include "CQLFactor.h"
#include <Pegasus/CQL/CQLExpression.h>
#include <Pegasus/CQL/CQLFunction.h>
#include <Pegasus/CQL/CQLFactory.h>
PEGASUS_NAMESPACE_BEGIN

#define PEGASUS_ARRAY_T CQLFactor
#include <Pegasus/Common/ArrayImpl.h>
#undef PEGASUS_ARRAY_T

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

CQLFactor::CQLFactor(CQLExpression& inCQLExp)
{
   _CQLExp = new CQLExpression(inCQLExp);
}

CQLFactor::CQLFactor(CQLFunction inCQLFunc)
{
   _CQLFunct = new CQLFunction(inCQLFunc);
}

CQLValue CQLFactor::getValue()
{
   return _CQLVal;
}

CQLValue CQLFactor::resolveValue(CIMInstance CI, QueryContext& QueryCtx)
{
   if(_CQLExp != NULL)
   {
      return _CQLExp->resolveValue(CI,QueryCtx);
   }
   else if (_CQLFunct != NULL)
   {
      return _CQLFunct->resolveValue(CI,QueryCtx);
   }
   else
   {
      _CQLVal.resolve(CI,QueryCtx);
      return _CQLVal;
   }
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
      return _CQLFunct->toString();
   }
   else if(_CQLExp != NULL)
   {
      return _CQLExp->toString();
   }
   else
   {
      return _CQLVal.toString();
   }
}

void CQLFactor::applyScopes(Array<CQLScope> inScopes)
{
   
   if(_CQLFunct != NULL)
   {
      _CQLFunct->applyScopes(inScopes);
   }
   else if(_CQLExp != NULL)
   {
      _CQLExp->applyScopes(inScopes);
   }
   else 
   {
      _CQLVal.applyScopes(inScopes);
   }
   return;
}


PEGASUS_NAMESPACE_END
