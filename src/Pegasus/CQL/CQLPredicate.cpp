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

CQLPredicate::CQLPredicate(){
	_rep = new CQLPredicateRep();
}

CQLPredicate::CQLPredicate(const CQLSimplePredicate& inSimplePredicate, Boolean inVerted)
{
	_rep = new CQLPredicateRep(inSimplePredicate,inVerted);
}

CQLPredicate::CQLPredicate(const CQLPredicate& inPredicate, Boolean inInverted)
{
	_rep = new CQLPredicateRep(inPredicate._rep);
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

void CQLPredicate::appendPredicate(const CQLPredicate& inPredicate){
	_rep->appendPredicate(inPredicate);
}

void CQLPredicate::appendPredicate(const CQLPredicate& inPredicate, BooleanOpType inBooleanOperator)
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

void CQLPredicate::applyContext(QueryContext& queryContext) {
  _rep->applyContext(queryContext);
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

CQLPredicate& CQLPredicate::operator=(const CQLPredicate& rhs){
	if(&rhs != this){
		if(_rep) delete _rep;
		_rep = new CQLPredicateRep(rhs._rep);
	}
	return *this;
}

PEGASUS_NAMESPACE_END
