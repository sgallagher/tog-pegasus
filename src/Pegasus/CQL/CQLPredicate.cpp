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
// Modified By: Vijay Eli, IBM (vijayeli@in.ibm.com) bug#3590
//
//%/////////////////////////////////////////////////////////////////////////////

#include "CQLPredicate.h"
#include "CQLPredicateRep.h"
#include <Pegasus/Common/Array.h>
#include <Pegasus/CQL/CQLFactory.h>
#include <Pegasus/Query/QueryCommon/QueryException.h>

PEGASUS_NAMESPACE_BEGIN

/*
#define PEGASUS_ARRAY_T CQLPredicate
#include <Pegasus/Common/ArrayImpl.h>
#undef PEGASUS_ARRAY_T

#define PEGASUS_ARRAY_T BooleanOpType
#include <Pegasus/Common/ArrayImpl.h>
#undef PEGASUS_ARRAY_T
*/

CQLPredicate::CQLPredicate(){
	_rep = new CQLPredicateRep();
}

CQLPredicate::CQLPredicate(const CQLSimplePredicate& inSimplePredicate, Boolean inVerted)
{
	_rep = new CQLPredicateRep(inSimplePredicate,inVerted);
}

CQLPredicate::CQLPredicate(const CQLPredicate& inPredicate, Boolean inInverted)
{
	_rep = new CQLPredicateRep(inPredicate._rep);
}

CQLPredicate::~CQLPredicate(){
	if(_rep)
		delete _rep;
}

Boolean CQLPredicate::evaluate(CIMInstance CI, QueryContext& QueryCtx)
{
   return _rep->evaluate(CI,QueryCtx);
}

Boolean CQLPredicate::getInverted()const{
	return _rep->getInverted();
}

void CQLPredicate::setInverted(Boolean invert){
	_rep->setInverted(invert);
}

void CQLPredicate::appendPredicate(const CQLPredicate& inPredicate){
	_rep->appendPredicate(inPredicate);
}

void CQLPredicate::appendPredicate(const CQLPredicate& inPredicate, BooleanOpType inBooleanOperator)
{
	_rep->appendPredicate(inPredicate,inBooleanOperator);
}

Array<CQLPredicate> CQLPredicate::getPredicates()const{
	return _rep->getPredicates();
}

CQLSimplePredicate CQLPredicate::getSimplePredicate()const{
	return _rep->getSimplePredicate();
}

Array<BooleanOpType> CQLPredicate::getOperators()const{
	return _rep->getOperators();
}

void CQLPredicate::applyContext(const QueryContext& queryContext) {
  _rep->applyContext(queryContext);
}

Boolean CQLPredicate::isSimple()const{
	return _rep->isSimple();
}

Boolean CQLPredicate::isSimpleValue()const{
        return _rep->isSimpleValue();
}


String CQLPredicate::toString()const{
	return _rep->toString();
}

CQLPredicate& CQLPredicate::operator=(const CQLPredicate& rhs){
	if(&rhs != this){
		if(_rep) delete _rep;
		_rep = new CQLPredicateRep(rhs._rep);
	}
	return *this;
}

PEGASUS_NAMESPACE_END
