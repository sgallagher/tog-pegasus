#include "CQLPredicate.h"
#include "CQLPredicateRep.h"
#include <Pegasus/CQL/CQLFactory.h>
#include <Pegasus/CQL/QueryContext.h>

PEGASUS_NAMESPACE_BEGIN

#define PEGASUS_ARRAY_T CQLPredicate
#include <Pegasus/Common/ArrayImpl.h>
#undef PEGASUS_ARRAY_T

#define PEGASUS_ARRAY_T BooleanOpType
#include <Pegasus/Common/ArrayImpl.h>
#undef PEGASUS_ARRAY_T


CQLPredicate::CQLPredicate(const CQLSimplePredicate& inSimplePredicate, Boolean inVerted)
{
	_rep = new CQLPredicateRep(inSimplePredicate,inVerted);
}

CQLPredicate::CQLPredicate(const CQLPredicate& inPredicate, Boolean inInverted)
{
	_rep = new CQLPredicateRep(inPredicate,inInverted);
}

CQLPredicate::~CQLPredicate(){
	if(_rep)
		delete _rep;
}

Boolean CQLPredicate::evaluate(CIMInstance CI, QueryContext& QueryCtx)
{
   return _rep->evaluate(CI,QueryCtx);
}

Boolean CQLPredicate::isTerminal(){
	return _rep->isTerminal();
}
Boolean CQLPredicate::getInverted(){
	return _rep->getInverted();
}

void CQLPredicate::setInverted(){
	_rep->setInverted();
}

void CQLPredicate::appendPredicate(CQLPredicate inPredicate, BooleanOpType inBooleanOperator)
{
	_rep->appendPredicate(inPredicate,inBooleanOperator);
}

void CQLPredicate::appendPredicate(CQLSimplePredicate inSimplePredicate, BooleanOpType inBooleanOperator){
	_rep->appendPredicate(inSimplePredicate,inBooleanOperator);
}

Array<CQLPredicate> CQLPredicate::getPredicates(){
	return _rep->getPredicates();
}

CQLSimplePredicate CQLPredicate::getSimplePredicate(){
	return _rep->getSimplePredicate();
}

Array<BooleanOpType> CQLPredicate::getOperators(){
	return _rep->getOperators();
}
Array<CQLScope> CQLPredicate::getScopes(){

}
void CQLPredicate::applyScopes(Array<CQLScope> & inScopes){
}


Boolean CQLPredicate::isSimple(){
	return _rep->isSimple();
}

Boolean CQLPredicate::isSimpleValue(){
        return _rep->isSimpleValue();
}


String CQLPredicate::toString(){
	return _rep->toString();
}

PEGASUS_NAMESPACE_END
