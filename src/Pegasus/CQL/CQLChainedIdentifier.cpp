#include "CQLChainedIdentifier.h"
#include "CQLChainedIdentifierRep.h"
//#include <Pegasus/CQL/CQLScope.h>
PEGASUS_NAMESPACE_BEGIN

CQLChainedIdentifier::CQLChainedIdentifier(String inString)
{
	_rep = new CQLChainedIdentifierRep(inString);
}

CQLChainedIdentifier::CQLChainedIdentifier(CQLIdentifier &id)
{
        _rep = new CQLChainedIdentifierRep(id);
}

CQLChainedIdentifier::CQLChainedIdentifier(const CQLChainedIdentifier& cid){
	_rep = new CQLChainedIdentifierRep(*(cid._rep));
}

const Array<CQLIdentifier>& CQLChainedIdentifier::getSubIdentifiers()const
{
	return _rep->getSubIdentifiers();
}

CQLIdentifier CQLChainedIdentifier::getLastIdentifier(){
	return _rep->getLastIdentifier();
}

String CQLChainedIdentifier::toString()const{
	return _rep->toString();
}

void CQLChainedIdentifier::append(CQLIdentifier & id){
	_rep->append(id);
}

Boolean CQLChainedIdentifier::isSubChain(CQLChainedIdentifier & chain){
	return _rep->isSubChain(chain);
}

CQLIdentifier& CQLChainedIdentifier::operator[](Uint32 index){
	return _rep->operator[](index);
}

Uint32 CQLChainedIdentifier::size(){
	return _rep->size();
}

Boolean CQLChainedIdentifier::prepend(CQLIdentifier & id){
	return _rep->prepend(id);
}

void CQLChainedIdentifier::applyScopes(Array<CQLScope>& scopes){

}

void CQLChainedIdentifier::parse(String & string){
}

PEGASUS_NAMESPACE_END
