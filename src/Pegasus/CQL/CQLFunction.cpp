#include "CQLFunction.h"
#include <Pegasus/CQL/CQLExpression.h>
PEGASUS_NAMESPACE_BEGIN

//##ModelId=40FD653E0390
CQLFunction::CQLFunction(FunctionOpType inFunctionOpType, Array<CQLExpression> inParms)
{
}

//##ModelId=40FC3BEA01F9
CQLValue CQLFunction::getValue(CIMInstance CI, QueryContext& queryCtx)
{
   return CQLValue(Uint64(0));
}

PEGASUS_NAMESPACE_END
