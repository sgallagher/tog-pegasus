#include "CQLScope.h"
#include <Pegasus/CQL/QueryContext.h>

PEGASUS_NAMESPACE_BEGIN


#define PEGASUS_ARRAY_T CQLScope
#include <Pegasus/Common/ArrayImpl.h>
#undef PEGASUS_ARRAY_T

CQLScope::CQLScope(CIMName _inCIMName, CQLChainedIdentifier _inChainedIdentifier, QueryContext* inQueryContext){
	_name = _inCIMName;
	_chainedIdentifier = _inChainedIdentifier;
	_queryContext = inQueryContext;
}

CIMName CQLScope::getScope(){
	return _name;
}

CQLChainedIdentifier CQLScope::getTarget(){
	return _chainedIdentifier;
}

QueryContext* CQLScope::getQueryContext(){
	return _queryContext;
}

Boolean CQLScope::isDefault(){

}

PEGASUS_NAMESPACE_END
