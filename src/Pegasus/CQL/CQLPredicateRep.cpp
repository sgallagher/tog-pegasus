#include "CQLPredicate.h"
#include "CQLPredicateRep.h"
#include <Pegasus/CQL/CQLFactory.h>
#include <Pegasus/CQL/QueryContext.h>

PEGASUS_NAMESPACE_BEGIN

CQLPredicateRep::CQLPredicateRep(const CQLSimplePredicate& inSimplePredicate, Boolean inVerted)
{
	_simplePredicate = inSimplePredicate;
	_invert = inVerted;
}

CQLPredicateRep::CQLPredicateRep(const CQLPredicate& inPredicate, Boolean inInverted)
{
	_predicates.append(inPredicate);
	_invert = inInverted;
}

CQLPredicateRep::CQLPredicateRep(const CQLPredicateRep* rep){
	_booleanOpType = rep->_booleanOpType;
	_predicates = rep->_predicates;
	_simplePredicate = rep->_simplePredicate;
	_operators = rep->_operators;
	_invert = rep->_invert;
	_terminal = rep->_terminal;
}

Boolean CQLPredicateRep::evaluate(CIMInstance CI, QueryContext& QueryCtx)
{
   return false;
}

Boolean CQLPredicateRep::isTerminal(){
	return _terminal;
}
Boolean CQLPredicateRep::getInverted(){
	return _invert;
}

void CQLPredicateRep::setInverted(){
	_invert = true;
}

void CQLPredicateRep::appendPredicate(CQLPredicate inPredicate, BooleanOpType inBooleanOperator)
{
	_predicates.append(inPredicate);
	_operators.append(inBooleanOperator);
}

void CQLPredicateRep::appendPredicate(CQLSimplePredicate inSimplePredicate, BooleanOpType inBooleanOperator){

}

Array<CQLPredicate> CQLPredicateRep::getPredicates(){
	return _predicates;
}

CQLSimplePredicate CQLPredicateRep::getSimplePredicate(){
	return _simplePredicate;
}

Array<BooleanOpType> CQLPredicateRep::getOperators(){
	return _operators;
}
Array<CQLScope> CQLPredicateRep::getScopes(){

}
void CQLPredicateRep::applyScopes(Array<CQLScope> & inScopes){
}


Boolean CQLPredicateRep::isSimple(){
	return (_predicates.size() == 0);
}
Boolean CQLPredicateRep::isSimpleValue(){
	return (_simplePredicate.isSimpleValue());
}
String CQLPredicateRep::toString(){
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
