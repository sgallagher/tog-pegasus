#include <Pegasus/CQL/CQLFunction.h>
#include <Pegasus/CQL/CQLFunctionRep.h>
//#include <Pegasus/CQL/CQLExpression.h>
#include <Pegasus/CQL/CQLPredicate.h>
#include <Pegasus/CQL/CQLFactory.h>
PEGASUS_NAMESPACE_BEGIN

CQLFunction::CQLFunction()
{
	_rep = new CQLFunctionRep();
} 

CQLFunction::CQLFunction(CQLIdentifier inOpType, Array<CQLPredicate> inParms)
{
        _rep = new CQLFunctionRep(inOpType,inParms);
}

CQLFunction::CQLFunction(const CQLFunction& inFunc)
{
	_rep = new CQLFunctionRep(inFunc._rep);
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

CQLFunction& CQLFunction::operator=(const CQLFunction& rhs){
	if(&rhs != this){
		if(_rep) delete _rep;
		_rep = new CQLFunctionRep(rhs._rep);
	}
	return *this;
}

Boolean CQLFunction::operator==(const CQLFunction& func){
	return (_rep == func._rep);
}
Boolean CQLFunction::operator!=(const CQLFunction& func){
	return (_rep != func._rep);
}
PEGASUS_NAMESPACE_END
