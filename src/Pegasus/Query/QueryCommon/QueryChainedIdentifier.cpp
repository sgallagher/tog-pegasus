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

#include "QueryChainedIdentifier.h"
#include "QueryChainedIdentifierRep.h"
#include <Pegasus/Query/QueryCommon/QueryContext.h>

PEGASUS_NAMESPACE_BEGIN

/*
#define PEGASUS_ARRAY_T QueryChainedIdentifier
#include <Pegasus/Common/ArrayImpl.h>
#undef PEGASUS_ARRAY_T
*/

QueryChainedIdentifier::QueryChainedIdentifier(){
	_rep = new QueryChainedIdentifierRep();
}

QueryChainedIdentifier::QueryChainedIdentifier(const QueryIdentifier &id)
{
  _rep = new QueryChainedIdentifierRep(id);
}

QueryChainedIdentifier::QueryChainedIdentifier(const QueryChainedIdentifier& cid){
	_rep = new QueryChainedIdentifierRep(*(cid._rep));
}

QueryChainedIdentifier::~QueryChainedIdentifier(){
	if(_rep)
		delete _rep;
}

const Array<QueryIdentifier>& QueryChainedIdentifier::getSubIdentifiers()const
{
	return _rep->getSubIdentifiers();
}

QueryIdentifier QueryChainedIdentifier::getLastIdentifier()const{
	return _rep->getLastIdentifier();
}

String QueryChainedIdentifier::toString()const{
	return _rep->toString();
}

void QueryChainedIdentifier::append(const QueryIdentifier & id){
	_rep->append(id);
}

QueryIdentifier QueryChainedIdentifier::operator[](Uint32 index)const
{
	return _rep->operator[](index);
}

QueryChainedIdentifier& QueryChainedIdentifier::operator=(const QueryChainedIdentifier& rhs){
	if(&rhs != this){
		if(_rep) delete _rep;
        	_rep = new QueryChainedIdentifierRep(rhs._rep);
	}
	return *this;
}

Uint32 QueryChainedIdentifier::size()const{
	return _rep->size();
}

Boolean QueryChainedIdentifier::prepend(const QueryIdentifier & id){
	return _rep->prepend(id);
}

void QueryChainedIdentifier::applyContext(QueryContext& inContext)
{
  _rep->applyContext(inContext);
}

PEGASUS_NAMESPACE_END
