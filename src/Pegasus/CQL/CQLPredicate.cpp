#include "CQLPredicate.h"
#include <Pegasus/CQL/CQLFactory.h>

PEGASUS_NAMESPACE_BEGIN

#define PEGASUS_ARRAY_T CQLPredicate
#include <Pegasus/Common/ArrayImpl.h>
#undef PEGASUS_ARRAY_T

#define PEGASUS_ARRAY_T BooleanOpType
#include <Pegasus/Common/ArrayImpl.h>
#undef PEGASUS_ARRAY_T


CQLPredicate::CQLPredicate(const CQLSimplePredicate& inSimplePredicate, Boolean inVerted)
{
	_simplePredicate = inSimplePredicate;
	_invert = inVerted;
}

CQLPredicate::CQLPredicate(const CQLPredicate& inPredicate, Boolean inInverted)
{
	_predicates.append(inPredicate);
	_invert = inInverted;
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

void CQLPredicate::setInverted(){
	_invert = true;
}

void CQLPredicate::appendPredicate(CQLPredicate inPredicate, BooleanOpType inBooleanOperator)
{
	_predicates.append(inPredicate);
	_operators.append(inBooleanOperator);
}

void CQLPredicate::appendPredicate(CQLSimplePredicate inSimplePredicate, BooleanOpType inBooleanOperator){

}

Array<CQLPredicate> CQLPredicate::getPredicates(){
	return _predicates;
}

CQLSimplePredicate CQLPredicate::getSimplePredicate(){
	return _simplePredicate;
}

Array<BooleanOpType> CQLPredicate::getOperators(){
	return _operators;
}
Array<CQLScope> CQLPredicate::getScopes(){

}
void CQLPredicate::applyScopes(Array<CQLScope> & inScopes){
}


Boolean CQLPredicate::isSimple(){
	return (_predicates.size() == 1);
}

String CQLPredicate::toString(){
	if(_terminal)
		return _simplePredicate.toString();
	if(isSimple())
		return _predicates[0].toString();
	String s;
	for(Uint32 i = 0; i < _predicates.size(); i++){
		s.append(_predicates[i].toString());
		if(i <= _operators.size()){
			switch(_operators[i]){
				case AND: s.append(" AND ");
				case OR: s.append(" OR ");
			}
		}
	}
	return s;
}

PEGASUS_NAMESPACE_END
