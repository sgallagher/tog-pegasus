//%2005////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001, 2002 BMC Software; Hewlett-Packard Development
// Company, L.P.; IBM Corp.; The Open Group; Tivoli Systems.
// Copyright (c) 2003 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation, The Open Group.
// Copyright (c) 2004 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation; VERITAS Software Corporation; The Open Group.
// Copyright (c) 2005 Hewlett-Packard Development Company, L.P.; IBM Corp.;
// EMC Corporation; VERITAS Software Corporation; The Open Group.
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

#include "QueryIdentifier.h"
#include "QueryIdentifierRep.h"
#include <iostream>
#include <ctype.h>
#include <cstdlib>
PEGASUS_NAMESPACE_BEGIN

/*
#define PEGASUS_ARRAY_T QueryIdentifier
#include <Pegasus/Common/ArrayImpl.h>
#undef PEGASUS_ARRAY_T
*/

/*
Char16 STAR = '*';
Char16 HASH = '#';
Char16 RBRKT = ']';
Char16 LBRKT = '[';
String SCOPE = "::";
*/
QueryIdentifier::QueryIdentifier()
{
	_rep = new QueryIdentifierRep();
}

QueryIdentifier::QueryIdentifier(const QueryIdentifier& id){
	_rep = new QueryIdentifierRep(id._rep);
}

QueryIdentifier::~QueryIdentifier(){
	if(_rep)
		delete _rep;
}

const CIMName& QueryIdentifier::getName()const
{
	return _rep->getName();
}

void QueryIdentifier::setName(const CIMName& inName)
{
   _rep->setName(inName);
}

const String& QueryIdentifier::getSymbolicConstantName()const
{
	return _rep->getSymbolicConstantName();
}

const  Array<SubRange>& QueryIdentifier::getSubRanges()const
{
	return _rep->getSubRanges();
}

Boolean QueryIdentifier::isArray()const
{
	return _rep->isArray();
}

Boolean QueryIdentifier::isSymbolicConstant()const
{
	return _rep->isSymbolicConstant();
}

Boolean QueryIdentifier::isWildcard()const
{
	return _rep->isWildcard();
}

const String& QueryIdentifier::getScope()const
{
	return _rep->getScope();
}

Boolean QueryIdentifier::isScoped()const
{
	return _rep->isScoped();
}

void QueryIdentifier::applyScope(String scope)
{
        _rep->applyScope(scope);
}


QueryIdentifier& QueryIdentifier::operator=(const QueryIdentifier& rhs){
	if(&rhs != this){
		if(_rep) delete _rep;
		_rep = new QueryIdentifierRep(rhs._rep);
	}
	return *this;
}

Boolean QueryIdentifier::operator==(const CIMName &rhs)const{
	return _rep->operator==(rhs);
}

Boolean QueryIdentifier::operator!=(const CIMName &rhs)const{
	return(!operator==(rhs));
}

Boolean QueryIdentifier::operator==(const QueryIdentifier &rhs)const{
	return _rep->operator==(rhs._rep);
}

Boolean QueryIdentifier::operator!=(const QueryIdentifier &rhs)const{
  return(!operator==(rhs));
}

String QueryIdentifier::toString()const{
	return _rep->toString();
}

PEGASUS_NAMESPACE_END
