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

#include <Pegasus/Common/InternalException.h>
#include "CQLPredicate.h"
#include "CQLSimplePredicate.h"
#include "CQLPredicateRep.h"
#include <Pegasus/CQL/CQLFactory.h>
#include <Pegasus/CQL/QueryContext.h>

PEGASUS_NAMESPACE_BEGIN

CQLPredicateRep::CQLPredicateRep(const CQLSimplePredicate& inSimplePredicate, Boolean inVerted)
{
	_simplePredicate = inSimplePredicate;
	_invert = inVerted;
}

CQLPredicateRep::CQLPredicateRep(const CQLPredicate& inPredicate, Boolean inInverted)
{
/*	 printf("CQLPredicateRep()\n");
	_predicates.append(inPredicate);
	printf("CQLPredicateRep() size = %d\n",_predicates.size());
	_invert = inInverted;
	_terminal = false;
*/
}

CQLPredicateRep::CQLPredicateRep(const CQLPredicateRep* rep){
	_booleanOpType = rep->_booleanOpType;
	_predicates = rep->_predicates;
	_simplePredicate = rep->_simplePredicate;
	_operators = rep->_operators;
	_invert = rep->_invert;
	_terminal = rep->_terminal;
}

Boolean CQLPredicateRep::evaluate(CIMInstance CI, QueryContext& QueryCtx)
{
  Boolean result = false; 

  if (isSimple())
  {
    result = _simplePredicate.evaluate(CI, QueryCtx);
  }
  else
  {
    PEGASUS_ASSERT(_predicates.size() % 2 == 0);
 
    result = _predicates[0].evaluate(CI, QueryCtx); 
    
    for (Uint32 i = 0; i < _operators.size(); i++)
    {
      if (_operators[i] == AND)
      {
	if (result == false)
	  continue;

	result = result && _predicates[i+1].evaluate(CI, QueryCtx);
      }	
      else
      {
	if (result == true)
	  break;

	result = _predicates[i+1].evaluate(CI, QueryCtx);
      } 
    }
  }

  return (getInverted()) ? !result : result;
}

Boolean CQLPredicateRep::isTerminal(){
	return _terminal;
}

Boolean CQLPredicateRep::getInverted(){
	return _invert;
}

void CQLPredicateRep::setInverted(){
	_invert = true;
}

void CQLPredicateRep::appendPredicate(const CQLPredicate& inPredicate){
	_predicates.append(inPredicate);
	_terminal = false;
}

void CQLPredicateRep::appendPredicate(const CQLPredicate& inPredicate, BooleanOpType inBooleanOperator)
{
	_predicates.append(inPredicate);
	_operators.append(inBooleanOperator);
	_terminal = false;
}

void CQLPredicateRep::appendPredicate(CQLSimplePredicate inSimplePredicate, BooleanOpType inBooleanOperator){

}

Array<CQLPredicate> CQLPredicateRep::getPredicates(){
	return _predicates;
}

CQLSimplePredicate CQLPredicateRep::getSimplePredicate(){
	return _simplePredicate;
}

Array<BooleanOpType> CQLPredicateRep::getOperators(){
	return _operators;
}

void CQLPredicateRep::applyContext(QueryContext& queryContext)
{
  if (isSimple())
  {
    _simplePredicate.applyContext(queryContext);
  }
  else
  {
    for (Uint32 i = 0; i <_predicates.size(); i++)
    {
      _predicates[i].applyContext(queryContext);
    }
  }
}

Boolean CQLPredicateRep::isSimple(){
	return (_predicates.size() == 0);
}

Boolean CQLPredicateRep::isSimpleValue(){
	return (_simplePredicate.isSimpleValue());
}

String CQLPredicateRep::toString(){
	printf("CQLPredicateRep::toString()\n");
	if(_terminal){
	printf("CQLPredicateRep::toString()_terminal\n");
		String s;
		if(_invert) s = "NOT ";
		s.append(_simplePredicate.toString());
		return s;
	}
	if(isSimple()){
	printf("CQLPredicateRep::toString()isSimple\n");
		String s;
                if(_invert) s = "NOT ";
                s.append(_simplePredicate.toString());
                return s;
	}
	printf("CQLPredicateRep::toString()else\n");
	String s;
	if(_invert) s = "NOT ";
	for(Uint32 i = 0; i < _predicates.size(); i++){
		s.append(_predicates[i].toString());
		if(i < _operators.size()){
			switch(_operators[i]){
				case AND: s.append(" AND ");
					break;
				case OR: s.append(" OR ");
					break;
			}
		}
	}
	return s;
}

PEGASUS_NAMESPACE_END
