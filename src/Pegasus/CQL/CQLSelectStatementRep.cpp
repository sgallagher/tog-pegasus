//%2004////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001, 2002  BMC Software, Hewlett-Packard Development
// Company, L. P., IBM Corp., The Open Group, Tivoli Systems.
// Copyright (c) 2004 BMC Software; Hewlett-Packard Development Company, L. P.;
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

#include "CQLSelectStatement.h"
#include "CQLSelectStatementRep.h"

PEGASUS_NAMESPACE_BEGIN

CQLSelectStatementRep::CQLSelectStatementRep()
  :SelectStatementRep()
{
}

CQLSelectStatementRep::CQLSelectStatementRep(String& inQlang, String& inQuery, QueryContext* inCtx)
  :SelectStatementRep(inQlang, inQuery, inCtx)
{
}

CQLSelectStatementRep::CQLSelectStatementRep(const CQLSelectStatementRep& rep)
  :SelectStatementRep(rep),
   _selectIdentifiers(rep._selectIdentifiers),
   _whereIdentifiers(rep._whereIdentifiers),
   _predicate(rep._predicate)
{
}

CQLSelectStatementRep::~CQLSelectStatementRep()
{
}

CQLSelectStatementRep& CQLSelectStatementRep::operator=(const CQLSelectStatementRep& rhs)
{
  if (this ==  &rhs)
    return *this;

  SelectStatementRep::operator=(rhs);

  _whereIdentifiers = rhs._whereIdentifiers;
  _selectIdentifiers = rhs._selectIdentifiers;
  _predicate = rhs._predicate;

  return *this;
}

Boolean CQLSelectStatementRep::evaluate(const CIMInstance& inCI)
{
   return false;
}

void CQLSelectStatementRep::applyProjection(CIMInstance& inCI) throw(Exception)
{

}

void CQLSelectStatementRep::validateClass(const CIMObjectPath& inClassName) throw(Exception)
{
}

void CQLSelectStatementRep::validateProperties() throw(Exception)
{
}

Array<CIMObjectPath> CQLSelectStatementRep::getClassPathList()
{
   Array<CIMObjectPath> arr;
   return arr;
}

CIMPropertyList CQLSelectStatementRep::getPropertyList(const CIMObjectPath& inClassName)
{
   CIMPropertyList arr;
   return arr;
}

void CQLSelectStatementRep::appendClassPath(const CQLIdentifier& inIdentifier)
{
  _ctx->insertClassPath(inIdentifier);
}

void CQLSelectStatementRep::setPredicate(CQLPredicate inPredicate)
{
  _predicate = inPredicate;
}

void CQLSelectStatementRep::insertClassPathAlias(const CQLIdentifier& inIdentifier, String inAlias)
{
  _ctx->insertClassPath(inIdentifier,inAlias);
}

void CQLSelectStatementRep::appendSelectIdentifier(const CQLChainedIdentifier& x)
{
  _selectIdentifiers.append(x);
}

Boolean CQLSelectStatementRep::appendWhereIdentifier(const CQLChainedIdentifier& x)
{
  _whereIdentifiers.append(x);
  return true;
}

PEGASUS_NAMESPACE_END
