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
// Modified By: Aruran, IBM(ashanmug@in.ibm.com) for Bug# 3589
//
//%/////////////////////////////////////////////////////////////////////////////

#include "CQLIdentifier.h"
#include "CQLIdentifierRep.h"
PEGASUS_NAMESPACE_BEGIN

/*
#define PEGASUS_ARRAY_T CQLIdentifier
#include <Pegasus/Common/ArrayImpl.h>
*/

/*
Char16 STAR = '*';
Char16 HASH = '#';
Char16 RBRKT = ']';
Char16 LBRKT = '[';
String SCOPE = "::";
*/
CQLIdentifier::CQLIdentifier():
  QueryIdentifier()
{
	if(_rep) 
		delete _rep;
	_rep = NULL;
	_rep = new QueryIdentifierRep();
}

CQLIdentifier::CQLIdentifier(const String& identifier):
  QueryIdentifier()
{
	if(_rep) 
		delete _rep;
   _rep = NULL;
	_rep = new CQLIdentifierRep(identifier);
}

CQLIdentifier::CQLIdentifier(const CQLIdentifier& id):
 QueryIdentifier()
{
	if(_rep) 
		delete _rep;
	_rep = NULL;
  _rep = new QueryIdentifierRep(id._rep);
}

CQLIdentifier::CQLIdentifier(const QueryIdentifier& id):
  QueryIdentifier(id)
{

}

CQLIdentifier::~CQLIdentifier()
{
  if(_rep)
    delete _rep;
  _rep = NULL;
}

CQLIdentifier& CQLIdentifier::operator=(const CQLIdentifier& rhs)
{
	if(&rhs != this)
   {
     if(_rep) delete _rep;
     _rep = new QueryIdentifierRep(rhs._rep);
	}
	return *this;
}

PEGASUS_NAMESPACE_END
