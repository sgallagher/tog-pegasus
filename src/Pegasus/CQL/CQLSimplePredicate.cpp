#include "CQLSimplePredicate.h"
#include "CQLSimplePredicateRep.h"
#include <Pegasus/CQL/CQLExpression.h>
#include <Pegasus/CQL/CQLFactory.h>
#include <Pegasus/CQL/QueryContext.h>

PEGASUS_NAMESPACE_BEGIN

CQLSimplePredicate::CQLSimplePredicate(){
	_rep = new CQLSimplePredicateRep();
}

CQLSimplePredicate::CQLSimplePredicate(const CQLExpression& inExpression)
{
	_rep = new CQLSimplePredicateRep(inExpression);
}

CQLSimplePredicate::CQLSimplePredicate(const CQLExpression& inExpression, const ExpressionOpType inOperator)
{
	_rep = new CQLSimplePredicateRep(inExpression,inOperator);
}

CQLSimplePredicate::CQLSimplePredicate(const CQLExpression& leftSideExpression, const CQLExpression& rightSideExpression, ExpressionOpType inOperator)
{
	_rep = new CQLSimplePredicateRep(leftSideExpression,rightSideExpression,inOperator);
}

CQLSimplePredicate::CQLSimplePredicate(const CQLSimplePredicate& inSimplePredicate){
        _rep = new CQLSimplePredicateRep(inSimplePredicate._rep);
}

CQLSimplePredicate::~CQLSimplePredicate(){
	if(_rep)
		delete _rep;
}

Boolean CQLSimplePredicate::evaluate(CIMInstance CI, QueryContext& QueryCtx)
{
	return _rep->evaluate(CI,QueryCtx);
}

CQLExpression CQLSimplePredicate::getLeftExpression()
{
	return _rep->getLeftExpression();
}

CQLExpression CQLSimplePredicate::getRightExpression()
{
        return _rep->getRightExpression();
}

enum ExpressionOpType CQLSimplePredicate::getOperation()
{
	return _rep->getOperation();
}

String CQLSimplePredicate::toString()
{
	return _rep->toString();
}
Boolean CQLSimplePredicate::isSimple(){
        return _rep->isSimple();
}
Boolean CQLSimplePredicate::isSimpleValue(){
	return _rep->isSimpleValue();
}
 
CQLSimplePredicate& CQLSimplePredicate::operator=(const CQLSimplePredicate& rhs){
	if(&rhs != this){
		if(_rep) delete _rep;
		_rep = new CQLSimplePredicateRep(rhs._rep);
	}
	return *this;
}

PEGASUS_NAMESPACE_END
