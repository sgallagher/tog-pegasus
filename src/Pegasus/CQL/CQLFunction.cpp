#include "CQLFunction.h"
#include <Pegasus/CQL/CQLExpression.h>
PEGASUS_NAMESPACE_BEGIN

//##ModelId=40FD653E0390
CQLFunction::CQLFunction(FunctionOpType inFunctionOpType, Array<CQLExpression> inParms)
{
}

CQLFunction::CQLFunction(const CQLFunction& inFunc)
{

}

//##ModelId=40FC3BEA01F9
CQLValue CQLFunction::getValue(CIMInstance CI, QueryContext& queryCtx)
{
   return CQLValue(Uint64(0));
}

String CQLFunction::toString()
{
   return String();
}

PEGASUS_NAMESPACE_END
