#include "CQLPredicate.h"

PEGASUS_NAMESPACE_BEGIN

#define PEGASUS_ARRAY_T CQLPredicate
#include <Pegasus/Common/ArrayImpl.h>
#undef PEGASUS_ARRAY_T

//##ModelId=40FD61E30034
CQLPredicate::CQLPredicate(const CQLExpression& inExpression, ExpressionOpType inOperator)
{
}

//##ModelId=40FD6290025E
CQLPredicate::CQLPredicate(const CQLExpression& leftSideExpression, const CQLExpression& rightSideExpression, ExpressionOpType inOperator)
{
}

//##ModelId=40FD725B025F
CQLPredicate::CQLPredicate(CQLPredicate inPredicate, Boolean inInvert)
{
}

//##ModelId=40FC365903BF
Boolean CQLPredicate::evaluate(CIMInstance CI, QueryContext& QueryCtx)
{
   return false;
}

//##ModelId=40FD6FF202ED
void CQLPredicate::appendPredicate(CQLPredicate inPredicate, BooleanOpType inBooleanOperator)
{
}

PEGASUS_NAMESPACE_END
