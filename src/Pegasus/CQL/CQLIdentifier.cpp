//%2003////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001, 2002  BMC Software, Hewlett-Packard Development
// Company, L. P., IBM Corp., The Open Group, Tivoli Systems.
// Copyright (c) 2003 BMC Software; Hewlett-Packard Development Company, L. P.;
// IBM Corp.; EMC Corporation, The Open Group.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// THE ABOVE COPYRIGHT NOTICE AND THIS PERMISSION NOTICE SHALL BE INCLUDED IN
// ALL COPIES OR SUBSTANTIAL PORTIONS OF THE SOFTWARE. THE SOFTWARE IS PROVIDED
// "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT
// LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
// PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
// HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
// ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
// WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
//
//==============================================================================
//
// Authors: David Rosckes (rosckes@us.ibm.com)
//          Bert Rivero (hurivero@us.ibm.com)
//          Chuck Carmack (carmack@us.ibm.com)
//          Brian Lucier (lucier@us.ibm.com)
//
// Modified By: 
//
//%/////////////////////////////////////////////////////////////////////////////

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

void CQLIdentifier::setName(CIMName inName)
{
   _rep->setName(inName);
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
