#include "CQLScope.h"

PEGASUS_NAMESPACE_BEGIN


#define PEGASUS_ARRAY_T CQLScope
#include <Pegasus/Common/ArrayImpl.h>
#undef PEGASUS_ARRAY_T

CQLScope::CQLScope(CIMName _inCIMName, CQLChainedIdentifier _inChainedIdentifier){

}

CIMName CQLScope::getScope(){
	return _name;
}

CQLChainedIdentifier CQLScope::getTarget(){
	return _chainedIdentifier;
}

Boolean CQLScope::isDefault(){

}

PEGASUS_NAMESPACE_END
