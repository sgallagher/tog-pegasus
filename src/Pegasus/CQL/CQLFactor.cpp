#include "CQLFactor.h"
#include <Pegasus/CQL/CQLExpression.h>
#include <Pegasus/CQL/CQLFunction.h>
PEGASUS_NAMESPACE_BEGIN

#define PEGASUS_ARRAY_T CQLFactor
#include <Pegasus/Common/ArrayImpl.h>
#undef PEGASUS_ARRAY_T

//##ModelId=40FC34920351
CQLFactor::CQLFactor(CQLValue inCQLVal)
{
}

//##ModelId=40FC34BC0061
CQLFactor::CQLFactor(CQLExpression& inCQLExp)
{
}

//##ModelId=40FC34E30391
CQLFactor::CQLFactor(CQLFunction inCQLFunc)
{
}

//##ModelId=40FC33B70262
CQLValue CQLFactor::getValue(CIMInstance CI, QueryContext& QueryCtx)
{
   return CQLValue(Uint64(0));
}

PEGASUS_NAMESPACE_END
