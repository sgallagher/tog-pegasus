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
// Author: Dave Rosckes (rosckes@us.ibm.com)
//
// Modified By:
//
//%/////////////////////////////////////////////////////////////////////////////

#include <Pegasus/CQL/CQLExpression.h>
#include <Pegasus/CQL/CQLExpressionRep.h>
#include <Pegasus/CQL/CQLFactory.h>
#include <Pegasus/CQL/QueryContext.h>

PEGASUS_NAMESPACE_BEGIN

#define PEGASUS_ARRAY_T TermOpType
#include <Pegasus/Common/ArrayImpl.h>
#undef PEGASUS_ARRAY_T

#define PEGASUS_ARRAY_T CQLExpression
#include <Pegasus/Common/ArrayImpl.h>
#undef PEGASUS_ARRAY_T

CQLExpression::CQLExpression(){
	_rep = new CQLExpressionRep();
}

CQLExpression::CQLExpression(CQLTerm& theTerm)
{
   _rep = new CQLExpressionRep(theTerm);

}

CQLExpression::CQLExpression(const CQLExpression& inExpress) 
{
	_rep = new CQLExpressionRep(inExpress._rep);
}

CQLExpression::~CQLExpression(){
	if(_rep)
		delete _rep;
}

CQLValue CQLExpression::resolveValue(CIMInstance CI, QueryContext& QueryCtx)
{
	return _rep->resolveValue(CI,QueryCtx);
}

void CQLExpression::appendOperation(TermOpType theTermOpType, CQLTerm& theTerm)
{
	_rep->appendOperation(theTermOpType,theTerm);
}

String CQLExpression::toString() 
{
   return _rep->toString();
}
Boolean CQLExpression::isSimple()
{
        return _rep->isSimple();
}
Boolean CQLExpression::isSimpleValue()
{
	return _rep->isSimpleValue();
}

Array<CQLTerm> CQLExpression::getTerms()
{
   return _rep->getTerms();
}

Array<TermOpType> CQLExpression::getOperators()
{
   return _rep->getOperators();
}

void CQLExpression::applyContext(QueryContext& inContext)
{
  _rep->applyContext(inContext);
}

CQLExpression& CQLExpression::operator=(const CQLExpression& rhs){
	if(&rhs != this){
		if(_rep) delete _rep;
		_rep = new CQLExpressionRep(rhs._rep);
	}
	return *this;
}
Boolean CQLExpression::operator==(const CQLExpression& expr){
	return _rep == expr._rep;
}

Boolean CQLExpression::operator!=(const CQLExpression& expr){
	return (!operator==(expr));
}

PEGASUS_NAMESPACE_END

