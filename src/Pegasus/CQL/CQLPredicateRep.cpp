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
/*	 printf("CQLPredicateRep()\n");
	_predicates.append(inPredicate);
	printf("CQLPredicateRep() size = %d\n",_predicates.size());
	_invert = inInverted;
	_terminal = false;
*/
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

void CQLPredicateRep::appendPredicate(const CQLPredicate& inPredicate){
	_predicates.append(inPredicate);
	_terminal = false;
}

void CQLPredicateRep::appendPredicate(const CQLPredicate& inPredicate, BooleanOpType inBooleanOperator)
{
	_predicates.append(inPredicate);
	_operators.append(inBooleanOperator);
	_terminal = false;
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

void CQLPredicateRep::applyContext(QueryContext& queryContext)
{
  if (isSimple())
  {
    _simplePredicate.applyContext(queryContext);
  }
  else
  {
    for (Uint32 i = 0; i <_predicates.size(); i++)
    {
      _predicates[i].applyContext(queryContext);
    }
  }
}

Boolean CQLPredicateRep::isSimple(){
	return (_predicates.size() == 0);
}

Boolean CQLPredicateRep::isSimpleValue(){
	return (_simplePredicate.isSimpleValue());
}

String CQLPredicateRep::toString(){
	printf("CQLPredicateRep::toString()\n");
	if(_terminal){
	printf("CQLPredicateRep::toString()_terminal\n");
		String s;
		if(_invert) s = "NOT ";
		s.append(_simplePredicate.toString());
		return s;
	}
	if(isSimple()){
	printf("CQLPredicateRep::toString()isSimple\n");
		String s;
                if(_invert) s = "NOT ";
                s.append(_simplePredicate.toString());
                return s;
	}
	printf("CQLPredicateRep::toString()else\n");
	String s;
	if(_invert) s = "NOT ";
	for(Uint32 i = 0; i < _predicates.size(); i++){
		s.append(_predicates[i].toString());
		if(i < _operators.size()){
			switch(_operators[i]){
				case AND: s.append(" AND ");
					break;
				case OR: s.append(" OR ");
					break;
			}
		}
	}
	return s;
}

PEGASUS_NAMESPACE_END
