#include "CQLSimplePredicate.h"
#include "CQLSimplePredicateRep.h"
//#include <Pegasus/CQL/CQLExpression.h>
#include <Pegasus/CQL/CQLFactory.h>
#include <Pegasus/CQL/QueryContext.h>

PEGASUS_NAMESPACE_BEGIN

CQLSimplePredicateRep::CQLSimplePredicateRep(const CQLExpression& inExpression)
{
	_leftSide = inExpression;
}

CQLSimplePredicateRep::CQLSimplePredicateRep(const CQLExpression& inExpression, const ExpressionOpType inOperator)
{
	_leftSide = inExpression;
	_operator = inOperator;
}

CQLSimplePredicateRep::CQLSimplePredicateRep(const CQLExpression& leftSideExpression, const CQLExpression& rightSideExpression, ExpressionOpType inOperator)
{
	_leftSide = leftSideExpression;
	_rightSide = rightSideExpression;
	_operator = inOperator;
}

CQLSimplePredicateRep::CQLSimplePredicateRep(const CQLSimplePredicateRep& rep){
	_leftSide = rep._leftSide;
        _rightSide = rep._rightSide;
        _operator = rep._operator;
}

Boolean CQLSimplePredicateRep::evaluate(CIMInstance CI, QueryContext& QueryCtx)
{
	return false;
}

CQLExpression CQLSimplePredicateRep::getLeftExpression()
{
	return _leftSide;
}

CQLExpression CQLSimplePredicateRep::getRightExpression()
{
        return _rightSide;
}

enum ExpressionOpType CQLSimplePredicateRep::getOperation()
{
	return _operator;
}

String CQLSimplePredicateRep::toString()
{
	return String::EMPTY;
}
Boolean CQLSimplePredicateRep::isSimple(){
        //return (_rightSide != CQLExpression());
	return true;
}
Boolean CQLSimplePredicateRep::isSimpleValue(){
	return _leftSide.isSimpleValue();
}
 
PEGASUS_NAMESPACE_END
