#include "CQLSimplePredicate.h"
#include <Pegasus/CQL/CQLExpression.h>

PEGASUS_NAMESPACE_BEGIN

CQLSimplePredicate::CQLSimplePredicate(const CQLExpression& inExpression)
{
	_leftSide = new CQLExpression(inExpression);
}

CQLSimplePredicate::CQLSimplePredicate(const CQLExpression& inExpression, const ExpressionOpType inOperator)
{
	_leftSide = new CQLExpression(inExpression);
	_operator = inOperator;
}

CQLSimplePredicate::CQLSimplePredicate(const CQLExpression& leftSideExpression, const CQLExpression& rightSideExpression, ExpressionOpType inOperator)
{
	_leftSide = new CQLExpression(leftSideExpression);
	_rightSide = new CQLExpression(rightSideExpression);
	_operator = inOperator;
}

Boolean CQLSimplePredicate::evaluate(CIMInstance CI, QueryContext& QueryCtx)
{
	return false;
}

CQLExpression CQLSimplePredicate::getLeftExpression()
{
	return *_leftSide;
}

CQLExpression CQLSimplePredicate::getRightExpression()
{
        return *_rightSide;
}

enum ExpressionOpType CQLSimplePredicate::getOperation()
{
	return _operator;
}

String CQLSimplePredicate::toString()
{
	return String::EMPTY;
}
 
PEGASUS_NAMESPACE_END
