#include "CQLTerm.h"

PEGASUS_NAMESPACE_BEGIN
#define PEGASUS_ARRAY_T CQLTerm
#include <Pegasus/Common/ArrayImpl.h>
#undef PEGASUS_ARRAY_T

//##ModelId=40FD3277018E
CQLTerm::CQLTerm(CQLFactor theFactor)
{
}

//##ModelId=40FC32680152
CQLValue CQLTerm::resolveValue(CIMInstance CI, QueryContext& QueryCtx)
{
}

//##ModelId=40FC32BF038F
void CQLTerm::appendOperation(FactorOpType inFactorOpType, CQLFactor inFactor)
{
}

String CQLTerm::toString()
{

}

Boolean CQLTerm::isSimpleValue()
{

}

Array<CQLFactor> getFactors()
{

}

Array<FactorOpType> CQLTerm::getOperators()
{

}

void CQLTerm::applyScopes(Array<CQLScope> inScope)
{

}

PEGASUS_NAMESPACE_END
