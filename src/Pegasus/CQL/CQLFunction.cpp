#include <Pegasus/CQL/CQLFunction.h>
#include <Pegasus/CQL/CQLFunctionRep.h>
//#include <Pegasus/CQL/CQLExpression.h>
#include <Pegasus/CQL/CQLPredicate.h>
#include <Pegasus/CQL/CQLFactory.h>
PEGASUS_NAMESPACE_BEGIN
/*
CQLFunction::CQLFunction(FunctionOpType inFunctionOpType, Array<CQLExpression> inParms)
{
	_rep = new CQLFunctionRep(inFunctionOpType,inParms);
} */

CQLFunction::CQLFunction(CQLIdentifier inOpType, Array<CQLPredicate> inParms)
{
        _rep = new CQLFunctionRep(inOpType,inParms);
}

CQLFunction::CQLFunction(const CQLFunction& inFunc)
{
	_rep = inFunc._rep;
}

CQLFunction::~CQLFunction(){
	if(_rep)
		delete _rep;
}

CQLValue CQLFunction::resolveValue(CIMInstance CI, QueryContext& queryCtx)
{
	return _rep->resolveValue(CI,queryCtx);
}

String CQLFunction::toString()
{
   return _rep->toString();
}


Array<CQLPredicate> CQLFunction::getParms()
{
   return _rep->getParms();
}

FunctionOpType CQLFunction::getFunctionType()
{
   return _rep->getFunctionType();
}
 
void CQLFunction::applyScopes(Array<CQLScope> inScopes)
{
	return _rep->applyScopes(inScopes);
}

Boolean CQLFunction::operator==(const CQLFunction& func){
	return (_rep == func._rep);
}
Boolean CQLFunction::operator!=(const CQLFunction& func){
	return (_rep != func._rep);
}
PEGASUS_NAMESPACE_END
