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

CQLChainedIdentifier::CQLChainedIdentifier(const CQLIdentifier &id)
{
        _rep = new CQLChainedIdentifierRep(id);
}

CQLChainedIdentifier::CQLChainedIdentifier(const CQLChainedIdentifier& cid){
	_rep = new CQLChainedIdentifierRep(*(cid._rep));
}
CQLChainedIdentifier::~CQLChainedIdentifier(){
	if(_rep)
		delete _rep;
}
const Array<CQLIdentifier>& CQLChainedIdentifier::getSubIdentifiers()const
{
	return _rep->getSubIdentifiers();
}

CQLIdentifier CQLChainedIdentifier::getLastIdentifier()const{
	return _rep->getLastIdentifier();
}

String CQLChainedIdentifier::toString()const{
	return _rep->toString();
}

void CQLChainedIdentifier::append(const CQLIdentifier & id){
	_rep->append(id);
}

Boolean CQLChainedIdentifier::isSubChain(const CQLChainedIdentifier & chain)const{
	return _rep->isSubChain(chain);
}

CQLIdentifier& CQLChainedIdentifier::operator[](Uint32 index){
	return _rep->operator[](index);
}

CQLChainedIdentifier& CQLChainedIdentifier::operator=(const CQLChainedIdentifier& rhs){
	if(&rhs != this){
		if(_rep) delete _rep;
        	_rep = new CQLChainedIdentifierRep(rhs._rep);
	}
	return *this;
}

Uint32 CQLChainedIdentifier::size()const{
	return _rep->size();
}

Boolean CQLChainedIdentifier::prepend(const CQLIdentifier & id){
	return _rep->prepend(id);
}

void CQLChainedIdentifier::applyContext(QueryContext& inContext)
{
  _rep->applyContext(inContext);
}

void CQLChainedIdentifier::parse(String & string){
}

PEGASUS_NAMESPACE_END
