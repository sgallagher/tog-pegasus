#include <Pegasus/CQL/CQLTerm.h>
#include <Pegasus/CQL/CQLTermRep.h>
#include <Pegasus/CQL/CQLFactory.h>
#include <Pegasus/CQL/QueryContext.h>
PEGASUS_NAMESPACE_BEGIN

#define PEGASUS_ARRAY_T FactorOpType
#include <Pegasus/Common/ArrayImpl.h>
#undef PEGASUS_ARRAY_T

#define PEGASUS_ARRAY_T CQLTerm
#include <Pegasus/Common/ArrayImpl.h>
#undef PEGASUS_ARRAY_T


CQLTerm::CQLTerm():_rep(0){

}

CQLTerm::CQLTerm(const CQLTerm& inTerm){
	_rep = inTerm._rep;
}

CQLTerm::CQLTerm(CQLFactor theFactor)
{
	_rep = new CQLTermRep(theFactor);
}

CQLTerm::~CQLTerm(){
        if(_rep)
                delete _rep;
}

CQLValue CQLTerm::resolveValue(CIMInstance CI, QueryContext& QueryCtx)
{
	return _rep->resolveValue(CI,QueryCtx);
}

void CQLTerm::appendOperation(FactorOpType inFactorOpType, CQLFactor inFactor)
{
	_rep->appendOperation(inFactorOpType,inFactor);
}

String CQLTerm::toString()
{
   return _rep->toString();
}

Boolean CQLTerm::isSimpleValue()
{
   return _rep->isSimpleValue();
}

Array<CQLFactor> CQLTerm::getFactors()
{
   return _rep->getFactors();
}

Array<FactorOpType> CQLTerm::getOperators()
{
   return _rep->getOperators();
}

void CQLTerm::applyScopes(Array<CQLScope> inScope)
{
	_rep->applyScopes(inScope);
}

PEGASUS_NAMESPACE_END
