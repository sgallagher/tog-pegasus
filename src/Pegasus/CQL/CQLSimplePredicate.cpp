#include "CQLSimplePredicate.h"
#include <Pegasus/CQL/CQLExpression.h>

PEGASUS_NAMESPACE_BEGIN
/*
#define PEGASUS_ARRAY_T CQLSimplePredicate
#include <Pegasus/Common/ArrayImpl.h>
#undef PEGASUS_ARRAY_T
*/

CQLSimplePredicate::CQLSimplePredicate(const CQLExpression& inExpression)
{
}

CQLSimplePredicate::CQLSimplePredicate(const CQLExpression& inExpression, const ExpressionOpType inOperator)
{
}

CQLSimplePredicate::CQLSimplePredicate(const CQLExpression& leftSideExpression, const CQLExpression& rightSideExpression, ExpressionOpType inOperator)
{
}

Boolean CQLSimplePredicate::evaluate(CIMInstance CI, QueryContext& QueryCtx)
{
}

CQLExpression CQLSimplePredicate::getLeftExpression()
{
}

CQLExpression CQLSimplePredicate::getRightExpression()
{
}

enum ExpressionOpType CQLSimplePredicate::getOperation()
{
}

String CQLSimplePredicate::toString()
{
}
 
PEGASUS_NAMESPACE_END
