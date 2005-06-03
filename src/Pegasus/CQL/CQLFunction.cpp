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

#include <Pegasus/CQL/CQLFunction.h>
#include <Pegasus/CQL/CQLFunctionRep.h>
//#include <Pegasus/CQL/CQLExpression.h>
#include <Pegasus/CQL/CQLPredicate.h>
#include <Pegasus/CQL/CQLFactory.h>
PEGASUS_NAMESPACE_BEGIN

CQLFunction::CQLFunction()
{
	_rep = new CQLFunctionRep();
} 

CQLFunction::CQLFunction(CQLIdentifier inOpType, Array<CQLPredicate> inParms)
{
        _rep = new CQLFunctionRep(inOpType,inParms);
}

CQLFunction::CQLFunction(const CQLFunction& inFunc)
{
	_rep = new CQLFunctionRep(inFunc._rep);
}

CQLFunction::~CQLFunction(){
	if(_rep)
		delete _rep;
}

CQLValue CQLFunction::resolveValue(const CIMInstance& CI, const QueryContext& queryCtx)
{
	return _rep->resolveValue(CI,queryCtx);
}

String CQLFunction::toString()const
{
   return _rep->toString();
}


Array<CQLPredicate> CQLFunction::getParms()const
{
   return _rep->getParms();
}

FunctionOpType CQLFunction::getFunctionType()const
{
   return _rep->getFunctionType();
}
 
void CQLFunction::applyContext(const QueryContext& inContext)
{
  _rep->applyContext(inContext);
}

CQLFunction& CQLFunction::operator=(const CQLFunction& rhs){
	if(&rhs != this){
		if(_rep) delete _rep;
		_rep = new CQLFunctionRep(rhs._rep);
	}
	return *this;
}

/*
Boolean CQLFunction::operator==(const CQLFunction& func)const{
	return (*_rep == *(func._rep));
}
Boolean CQLFunction::operator!=(const CQLFunction& func)const{
	return (_rep != func._rep);
}
*/
PEGASUS_NAMESPACE_END
