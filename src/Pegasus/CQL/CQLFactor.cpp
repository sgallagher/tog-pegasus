#include "CQLFactor.h"
#include <Pegasus/CQL/CQLExpression.h>
#include <Pegasus/CQL/CQLFunction.h>
#include <Pegasus/CQL/CQLFactory.h>
#include <Pegasus/CQL/CQLFactorRep.h>
#include <Pegasus/CQL/QueryContext.h>

PEGASUS_NAMESPACE_BEGIN

#define PEGASUS_ARRAY_T CQLFactor
#include <Pegasus/Common/ArrayImpl.h>
#undef PEGASUS_ARRAY_T

CQLFactor::CQLFactor(const CQLFactor& inCQLFact)
{
	_rep = inCQLFact._rep;
}

CQLFactor::CQLFactor(CQLValue& inCQLVal)
{
	_rep = new CQLFactorRep(inCQLVal);
}

CQLFactor::CQLFactor(CQLExpression& inCQLExp)
{
	_rep = new CQLFactorRep(inCQLExp);
}

CQLFactor::CQLFactor(CQLFunction& inCQLFunc)
{
	_rep = new CQLFactorRep(inCQLFunc);
}

CQLFactor::~CQLFactor(){
	if(_rep){
		delete _rep;
	}
}

CQLValue CQLFactor::getValue()
{
   	return _rep->getValue();
}

CQLValue CQLFactor::resolveValue(CIMInstance CI, QueryContext& QueryCtx)
{
	return _rep->resolveValue(CI, QueryCtx);
}
Boolean CQLFactor::isSimple()
{
        return _rep->isSimple();
}
Boolean CQLFactor::isSimpleValue()
{
	return _rep->isSimpleValue();
}

CQLFunction CQLFactor::getCQLFunction()
{
   return _rep->getCQLFunction();
}

CQLExpression CQLFactor::getCQLExpression()
{
   return _rep->getCQLExpression();
}

String CQLFactor::toString()
{
	return _rep->toString();
}

void CQLFactor::applyScopes(Array<CQLScope> inScopes)
{
	_rep->applyScopes(inScopes);   
}

Boolean CQLFactor::operator==(const CQLFactor& factor){
	return (_rep == factor._rep);
}
Boolean CQLFactor::operator!=(const CQLFactor& factor){
	return (!operator==(factor));                                                                                
}

PEGASUS_NAMESPACE_END
