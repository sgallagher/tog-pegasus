#include "CQLIdentifier.h"
#include "CQLIdentifierRep.h"
#include <iostream>
#include <ctype.h>
#include <cstdlib>
PEGASUS_NAMESPACE_BEGIN

#define PEGASUS_ARRAY_T CQLIdentifier
#include <Pegasus/Common/ArrayImpl.h>
#undef PEGASUS_ARRAY_T

/*
Char16 STAR = '*';
Char16 HASH = '#';
Char16 RBRKT = ']';
Char16 LBRKT = '[';
String SCOPE = "::";
*/
CQLIdentifier::CQLIdentifier()
{
	_rep = new CQLIdentifierRep();
}


CQLIdentifier::CQLIdentifier(String identifier)
{
	_rep = new CQLIdentifierRep(identifier);
}

CQLIdentifier::CQLIdentifier(const CQLIdentifier& id){
	_rep = new CQLIdentifierRep(id._rep);
}

CQLIdentifier::~CQLIdentifier(){
	if(_rep)
		delete _rep;
//	printf("~CQLIdentifier()\n");
}

const CIMName& CQLIdentifier::getName()const
{
	return _rep->getName();
}

const String& CQLIdentifier::getSymbolicConstantName()const
{
	return _rep->getSymbolicConstantName();
}

const  Array<SubRange>& CQLIdentifier::getSubRanges()const
{
	return _rep->getSubRanges();
}

Boolean CQLIdentifier::isArray()const
{
	return _rep->isArray();
}

Boolean CQLIdentifier::isSymbolicConstant()const
{
	return _rep->isSymbolicConstant();
}

Boolean CQLIdentifier::isWildcard()const
{
	return _rep->isWildcard();
}

const String& CQLIdentifier::getScope()const
{
	return _rep->getScope();
}

Boolean CQLIdentifier::isScoped()const
{
	return _rep->isScoped();
}

void CQLIdentifier::applyScope(String scope)
{
        _rep->applyScope(scope);
}


CQLIdentifier& CQLIdentifier::operator=(const CQLIdentifier& rhs){
	if(&rhs != this){
		if(_rep) delete _rep;
		_rep = new CQLIdentifierRep(rhs._rep);
	}
	return *this;
}

Boolean CQLIdentifier::operator==(const CIMName &rhs)const{
	return _rep->operator==(rhs);
}

Boolean CQLIdentifier::operator!=(const CIMName &rhs)const{
	return(!operator==(rhs));
}

Boolean CQLIdentifier::operator==(const CQLIdentifier &rhs)const{
	return _rep == rhs._rep;
}

Boolean CQLIdentifier::operator!=(const CQLIdentifier &rhs)const{
        return(!operator==(rhs));
}

String CQLIdentifier::toString()const{
	return _rep->toString();
}

PEGASUS_NAMESPACE_END
