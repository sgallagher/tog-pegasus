#include "QueryContext.h"
#include <Pegasus/Common/System.h>

PEGASUS_NAMESPACE_BEGIN                                                                                                                                       
QueryContext::QueryContext(CIMNamespaceName inNS){
	_NS = inNS;
}

String QueryContext::getHost(Boolean fullyQualified){
	if(fullyQualified) return System::getFullyQualifiedHostName();
	return System::getHostName();
}

CIMNamespaceName QueryContext::getNamespace()const{
	return _NS;
}

void QueryContext::insertClassPath(const CQLIdentifier& inIdentifier, String inAlias){
	if(inAlias != String::EMPTY){
		_AliasClassTable.insert(inAlias, inIdentifier);
	}
	_fromList.append(inIdentifier);
}

const CQLIdentifier QueryContext::findClass(String inAlias){
	// look for alias match
	CQLIdentifier _class;
	if(_AliasClassTable.lookup(inAlias, _class)) return _class;

	// look if inAlias is really a class name
	_class = CQLIdentifier(inAlias);
	Array<CQLIdentifier> _identifiers = getFromList();
	for(Uint32 i = 0; i < _identifiers.size(); i++){
		if(_class == _identifiers[i]) return _identifiers[i];
	}	

	// could not find inAlias
	return CQLIdentifier("");
}                                                                                                                                       
Array<CQLIdentifier> QueryContext::getFromList()const{
	return _fromList;
}

PEGASUS_NAMESPACE_END

