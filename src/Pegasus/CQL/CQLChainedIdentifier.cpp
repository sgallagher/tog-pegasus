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
// Modified By: Aruran, IBM(ashanmug@in.ibm.com) for Bug# 3588
//
//%/////////////////////////////////////////////////////////////////////////////

#include "CQLChainedIdentifier.h"
#include "CQLChainedIdentifierRep.h"
#include <Pegasus/CQL/CQLFactory.h>
PEGASUS_NAMESPACE_BEGIN

/*
#define PEGASUS_ARRAY_T CQLChainedIdentifier
#include <Pegasus/Common/ArrayImpl.h>
#undef PEGASUS_ARRAY_T
*/

CQLChainedIdentifier::CQLChainedIdentifier():
	QueryChainedIdentifier()
{
	if(_rep) 
		delete _rep;
	_rep = NULL;
	_rep = new CQLChainedIdentifierRep();
}

CQLChainedIdentifier::CQLChainedIdentifier(const String& inString):
	QueryChainedIdentifier()
{
	if(_rep) 
		delete _rep;
	_rep = NULL;
	_rep = new CQLChainedIdentifierRep(inString);
}

CQLChainedIdentifier::CQLChainedIdentifier(const CQLIdentifier &id):
	QueryChainedIdentifier()
{
	if(_rep) 
		delete _rep;
	_rep = NULL;
   _rep = new CQLChainedIdentifierRep(id);
}

CQLChainedIdentifier::CQLChainedIdentifier(const CQLChainedIdentifier& cid):
QueryChainedIdentifier()
{
	if(_rep) 
		delete _rep;
	_rep = NULL;
   _rep = new QueryChainedIdentifierRep(cid._rep);
}

CQLChainedIdentifier::~CQLChainedIdentifier()
{
	if(_rep)
		delete _rep;
   _rep = NULL;
}

Array<CQLIdentifier> CQLChainedIdentifier::getSubIdentifiers()const
{
  Array<CQLIdentifier> result;
  Array<QueryIdentifier> qIds = _rep->getSubIdentifiers();
  for (Uint32 i = 0; i < qIds.size(); i++)
  {
    CQLIdentifier cid(qIds[i]);
    result.append(cid);
  }

  return result;
}

CQLIdentifier CQLChainedIdentifier::getLastIdentifier()const{
	QueryIdentifier qid = _rep->getLastIdentifier();
   return CQLIdentifier(qid);
}

CQLIdentifier CQLChainedIdentifier::operator[](Uint32 index)const
{
	QueryIdentifier qid = _rep->operator[](index);
   return CQLIdentifier(qid);
}

CQLChainedIdentifier& CQLChainedIdentifier::operator=(const CQLChainedIdentifier& rhs)
{
	if(&rhs != this)
   {
     if(_rep) delete _rep;
     _rep = new QueryChainedIdentifierRep(rhs._rep);
	}
	return *this;
}

PEGASUS_NAMESPACE_END
