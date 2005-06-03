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
// Modified By: David Dillard, VERITAS Software Corp.
//                  (david.dillard@veritas.com)
//              Vijay Eli, IBM (vijayeli@in.ibm.com) bug#3590
//
//%/////////////////////////////////////////////////////////////////////////////

#include <Pegasus/CQL/CQLSimplePredicate.h>
#include <Pegasus/CQL/CQLSimplePredicateRep.h>
#include <Pegasus/CQL/CQLExpression.h>
#include <Pegasus/CQL/CQLFactory.h>
#include <Pegasus/Query/QueryCommon/QueryContext.h>

PEGASUS_NAMESPACE_BEGIN

CQLSimplePredicate::CQLSimplePredicate()
{
    _rep = new CQLSimplePredicateRep();
}

CQLSimplePredicate::CQLSimplePredicate(const CQLExpression& inExpression)
{
    _rep = new CQLSimplePredicateRep(inExpression);
}

CQLSimplePredicate::CQLSimplePredicate(const CQLExpression& inExpression, 
                       ExpressionOpType inOperator)
{
    _rep = new CQLSimplePredicateRep(inExpression, inOperator);
}

CQLSimplePredicate::CQLSimplePredicate(const CQLExpression& leftSideExpression,
                       const CQLExpression& rightSideExpression,
                       ExpressionOpType inOperator)
{
    _rep = new CQLSimplePredicateRep(leftSideExpression,rightSideExpression, inOperator);
}

CQLSimplePredicate::CQLSimplePredicate(const CQLSimplePredicate& inSimplePredicate)
{
    _rep = new CQLSimplePredicateRep(inSimplePredicate._rep);
}

CQLSimplePredicate::~CQLSimplePredicate()
{
    delete _rep;
}

Boolean CQLSimplePredicate::evaluate(CIMInstance CI, QueryContext& QueryCtx)
{
    return _rep->evaluate(CI,QueryCtx);
}

CQLExpression CQLSimplePredicate::getLeftExpression() const
{
    return _rep->getLeftExpression();
}

CQLExpression CQLSimplePredicate::getRightExpression() const
{
    return _rep->getRightExpression();
}

enum ExpressionOpType CQLSimplePredicate::getOperation() const
{
    return _rep->getOperation();
}

void CQLSimplePredicate::applyContext(const QueryContext& queryContext)
{
    _rep->applyContext(queryContext);
}

String CQLSimplePredicate::toString() const
{
    return _rep->toString();
}

Boolean CQLSimplePredicate::isSimple() const
{
    return _rep->isSimple();
}

Boolean CQLSimplePredicate::isSimpleValue() const
{
    return _rep->isSimpleValue();
}
 
CQLSimplePredicate& CQLSimplePredicate::operator=(const CQLSimplePredicate& rhs)
{
    if(&rhs != this)
    {
        delete _rep;
        _rep = 0;
        _rep = new CQLSimplePredicateRep(rhs._rep);
    }

    return *this;
}

void CQLSimplePredicate::setOperation(ExpressionOpType op)
{
    _rep->setOperation(op);
}

PEGASUS_NAMESPACE_END
