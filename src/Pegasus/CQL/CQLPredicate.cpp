#include "CQLPredicate.h"

PEGASUS_NAMESPACE_BEGIN

#define PEGASUS_ARRAY_T CQLPredicate
#include <Pegasus/Common/ArrayImpl.h>
#undef PEGASUS_ARRAY_T

CQLPredicate::CQLPredicate(const CQLSimplePredicate& inSimplePredicate, Boolean inVerted)
{
}

CQLPredicate::CQLPredicate(const CQLPredicate& inPredicate, Boolean inInverted)
{
}

Boolean CQLPredicate::evaluate(CIMInstance CI, QueryContext& QueryCtx)
{
   return false;
}

Boolean CQLPredicate::isTerminal(){
	return _terminal;
}
Boolean CQLPredicate::getInverted(){
	return _invert;
}

Boolean CQLPredicate::setInverted(){
	_invert = true;
}

void CQLPredicate::appendPredicate(CQLPredicate inPredicate, BooleanOpType inBooleanOperator)
{
}

void CQLPredicate::appendPredicate(CQLSimplePredicate inSimplePredicate, BooleanOpType inBooleanOperator){

}

Array<CQLPredicate> CQLPredicate::getPredicates(){
//	return _predicates;
}

CQLSimplePredicate CQLPredicate::getSimplePredicate(){
	return _simplePredicate;
}

BooleanOpType* CQLPredicate::getOperators(){
	return _operators;
}
/*
Array<CQLScope> CQLPredicate::getScopes(){

}

void CQLPredicate::applyScopes(Array<CQLScope> & inScopes){

}
*/

Boolean CQLPredicate::isSimple(){

}

String CQLPredicate::toString(){

}
PEGASUS_NAMESPACE_END
