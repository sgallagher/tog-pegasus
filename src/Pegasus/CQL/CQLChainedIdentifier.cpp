#include "CQLChainedIdentifier.h"
#include "CQLChainedIdentifierRep.h"
#include <Pegasus/CQL/CQLFactory.h>
PEGASUS_NAMESPACE_BEGIN

#define PEGASUS_ARRAY_T CQLChainedIdentifier
#include <Pegasus/Common/ArrayImpl.h>
#undef PEGASUS_ARRAY_T

CQLChainedIdentifier::CQLChainedIdentifier(){
	_rep = new CQLChainedIdentifierRep();
}

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
CQLChainedIdentifier::~CQLChainedIdentifier(){
	if(_rep)
		delete _rep;
	
	//printf("~CQLChainedIdentifier()\n");
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

CQLChainedIdentifier& CQLChainedIdentifier::operator=(const CQLChainedIdentifier& rhs){
	if(&rhs != this){
		//printf("rhs != this\n");
		if(_rep) delete _rep;
        	_rep = new CQLChainedIdentifierRep(rhs._rep);
	}
	//printf("CQLChainedIdentifier::operator=\n");
	return *this;
}

Uint32 CQLChainedIdentifier::size(){
	return _rep->size();
}

Boolean CQLChainedIdentifier::prepend(CQLIdentifier & id){
	return _rep->prepend(id);
}

void CQLChainedIdentifier::applyContext(QueryContext& inContext)
{
  return _rep->applyContext(inContext);
}

void CQLChainedIdentifier::parse(String & string){
}

PEGASUS_NAMESPACE_END
