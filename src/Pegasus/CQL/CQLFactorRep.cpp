#include "CQLFactorRep.h"
#include <Pegasus/CQL/CQLExpression.h>
#include <Pegasus/CQL/CQLFunction.h>
#include <Pegasus/CQL/CQLFactory.h>
#include <Pegasus/CQL/CQLValue.h>
#include <Pegasus/CQL/CQLScope.h>
#include <Pegasus/CQL/QueryContext.h>
 
PEGASUS_NAMESPACE_BEGIN
/*
#define PEGASUS_ARRAY_T CQLFactorRep
#include <Pegasus/Common/ArrayImpl.h>
#undef PEGASUS_ARRAY_T
*/
CQLFactorRep::CQLFactorRep(const CQLFactorRep* rep)
{
   _CQLVal = rep->_CQLVal;
   _CQLFunct = rep->_CQLFunct;
   _CQLExp = rep->_CQLExp;
   _invert = rep->_invert;
   _simpleValue = rep->_simpleValue;
   _containedType = rep->_containedType;
}

CQLFactorRep::CQLFactorRep(const CQLValue& inCQLVal)
{
   _CQLVal = inCQLVal;
   _simpleValue = true;
   _containedType = VALUE;
}

CQLFactorRep::CQLFactorRep(CQLExpression& inCQLExp)
{
   _CQLExp = inCQLExp;
   _simpleValue = false;
   _containedType = EXPRESSION;
}

CQLFactorRep::CQLFactorRep(CQLFunction& inCQLFunc)
{
   _CQLFunct = inCQLFunc;
   _simpleValue = false;
   _containedType = FUNCTION;
}

CQLValue CQLFactorRep::getValue()
{
   return _CQLVal;
}

CQLValue CQLFactorRep::resolveValue(CIMInstance CI, QueryContext& QueryCtx)
{

   if(_containedType == EXPRESSION)
   {
      return _CQLExp.resolveValue(CI,QueryCtx);
   }
   else if (_containedType == FUNCTION)
   {
      return _CQLFunct.resolveValue(CI,QueryCtx);
   }
   else
   {
      _CQLVal.resolve(CI,QueryCtx);
      return _CQLVal;
   }
}

Boolean CQLFactorRep::isSimple()
{
   return _simpleValue;
}

Boolean CQLFactorRep::isSimpleValue()
{
   return _simpleValue;
}

CQLFunction CQLFactorRep::getCQLFunction()
{
   return _CQLFunct;
}

CQLExpression CQLFactorRep::getCQLExpression()
{
   return _CQLExp;
}

String CQLFactorRep::toString()
{
  printf("CQLFactorRep::toString()\n");
  if(_containedType == VALUE){
    return _CQLVal.toString();
  }

  if(_containedType == FUNCTION)
   {
      return _CQLFunct.toString();
   }else
   {
      return _CQLExp.toString();
   }
}

void CQLFactorRep::applyContext(QueryContext& inContext)
{
   
   if(_containedType == FUNCTION)
   {
     _CQLFunct.applyContext(inContext);
   }
   else if(_containedType == EXPRESSION)
   {
     _CQLExp.applyContext(inContext);
   }
   else 
   {
     _CQLVal.applyContext(inContext);
   }
   return;
}

Boolean CQLFactorRep::operator==(const CQLFactorRep& rep){
	  return true;
}
Boolean CQLFactorRep::operator!=(const CQLFactorRep& rep){
	return (!operator==(rep));
}
PEGASUS_NAMESPACE_END
