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
#include "CQLSimplePredicate.h"
#include "CQLSimplePredicateRep.h"
//#include <Pegasus/CQL/CQLExpression.h>
#include <Pegasus/CQL/CQLFactory.h>
#include <Pegasus/CQL/QueryContext.h>

PEGASUS_NAMESPACE_BEGIN

CQLSimplePredicateRep::CQLSimplePredicateRep(){
	_isSimple = true;
}

CQLSimplePredicateRep::CQLSimplePredicateRep(const CQLExpression& inExpression)
{
	_leftSide = inExpression;
	_isSimple = true;
}

CQLSimplePredicateRep::CQLSimplePredicateRep(const CQLExpression& inExpression, 
					     const ExpressionOpType inOperator)
{
	_leftSide = inExpression;
	_operator = inOperator;
	_isSimple = true;
}

CQLSimplePredicateRep::CQLSimplePredicateRep(const CQLExpression& leftSideExpression, 
					     const CQLExpression& rightSideExpression,
					     ExpressionOpType inOperator)
{
	_leftSide = leftSideExpression;
	_rightSide = rightSideExpression;
	_operator = inOperator;
	_isSimple = false;
}

CQLSimplePredicateRep::CQLSimplePredicateRep(const CQLSimplePredicateRep* rep){
	_leftSide = rep->_leftSide;
        _rightSide = rep->_rightSide;
        _operator = rep->_operator;
	_isSimple = rep->_isSimple;
}

Boolean CQLSimplePredicateRep::evaluate(CIMInstance CI, QueryContext& QueryCtx)
{
  CQLValue leftVal = _leftSide.resolveValue(CI, QueryCtx);

  if (isSimple())
  {
    PEGASUS_ASSERT(_operator == IS_NULL || _operator == IS_NOT_NULL);

    return (_operator == IS_NULL) ? leftVal.isNull() : !leftVal.isNull();
  }

  PEGASUS_ASSERT(_operator != IS_NULL && _operator != IS_NOT_NULL);

  if (leftVal.isNull())
  {
    // The null contagion rule
    // ATTN - change this to a specific CQLException so that it can
    // be caught above
    throw UninitializedObjectException(); 
  }

  CQLValue rightVal = _rightSide.resolveValue(CI, QueryCtx);

  if (rightVal.isNull())
  {
    // The null contagion rule
    // ATTN - change this to a specific CQLException so that it can
    // be caught above
    throw UninitializedObjectException(); 
  }

  switch(_operator)
  {
  case LT:
    return leftVal < rightVal;
    break;

  case GT:
    return leftVal > rightVal;
    break;

  case LE:
    return leftVal <= rightVal;
    break;

  case GE:
    return leftVal >= rightVal;
    break;

  case EQ:
    return leftVal == rightVal;
    break;

  case NE:
    return leftVal != rightVal;
    break;

  case ISA:
    return leftVal.isa(rightVal, QueryCtx);
    break;

  case LIKE:
    return leftVal.like(rightVal);
    break;

  case IS_NULL:
  case IS_NOT_NULL:
    // Never get here due to the assert.
    break;
  }

  return true;  // keep the compiler happy
}

CQLExpression CQLSimplePredicateRep::getLeftExpression()
{
	return _leftSide;
}

CQLExpression CQLSimplePredicateRep::getRightExpression()
{
        return _rightSide;
}

enum ExpressionOpType CQLSimplePredicateRep::getOperation()
{
	return _operator;
}

void CQLSimplePredicateRep::applyContext(QueryContext& queryContext)
{
   CQLIdentifier _id;

   if(_leftSide.isSimpleValue())
   {
      _id = _leftSide.getTerms()[0].getFactors()[0].
                     getValue().getChainedIdentifier().getLastIdentifier();

      if(_id.isSymbolicConstant())
      {
         if(_id.getName().getString().size() == 0)
         {
            // We have a standalone symbolic constant.
            if(!isSimple() && _rightSide.isSimpleValue())
            {
               // We need to add context to the symbolic constant
            /*   _leftSide.applyContext(queryContext,
                  _rightSide.getTerms()[0].getFactors()[0].
                     getValue().getChainedIdentifier());
*/
            }
            else
            {
               // There is no valid context for the symbolic constant
               throw(Exception(String("CQLSimplePredicateRep::applyContext -- standalone symbolic constant error")));
            }
         }
      }
   }
   else
   {
      _leftSide.applyContext(queryContext);
   }

   if (!isSimple())
   {
      if(_rightSide.isSimpleValue())
      {
         _id = _rightSide.getTerms()[0].getFactors()[0].
                     getValue().getChainedIdentifier().getLastIdentifier();

         if(_id.isSymbolicConstant())
         {
            if(_id.getName().getString().size() == 0)
            {
               // We have a standalone symbolic constant.
               if(_leftSide.isSimpleValue())
               {
                  // We need to add context to the symbolic constant
               /*   _rightSide.applyContext(queryContext,
                     _leftSide.getTerms()[0].getFactors()[0].
                        getValue().getChainedIdentifier());
   */
               }
               else
               {
                  // There is no valid context for the symbolic constant
                  throw(Exception(String("CQLSimplePredicateRep::applyContext -- standalone symbolic constant error")));
               }
            }
         }
      }
      else
      {
         _rightSide.applyContext(queryContext);
      }
   }
}

String CQLSimplePredicateRep::toString()
{
	printf("CQLSimplePredicate::toString()\n");
	String s = _leftSide.toString();
	if(!_isSimple){
	   switch(_operator){
		case LT:
			s.append(" < ");
			break;
		case GT:
			s.append(" > ");
			break;
		case LE:
			s.append(" <= ");
			break;
		case GE:
			s.append(" >= ");
			break;
		case EQ:
			s.append(" = ");
			break;
		case NE:
			s.append(" != ");
			break;
		case IS_NULL:
			s.append(" IS NULL ");
			break;
		case IS_NOT_NULL:
			s.append(" IS NOT NULL ");
			break;
		case ISA:
			s.append(" ISA ");
			break;
		case LIKE:
			s.append(" LIKE ");
			break;
	   }
	   s.append(_rightSide.toString());
	}
	switch(_operator){
		case IS_NULL:
        		s.append(" IS NULL ");
                        break;
                case IS_NOT_NULL:
                        s.append(" IS NOT NULL ");
                        break;
		default:
			break;
	}
	return s;
}
Boolean CQLSimplePredicateRep::isSimple(){
        return _isSimple;
}
Boolean CQLSimplePredicateRep::isSimpleValue(){
	return _leftSide.isSimpleValue();
}
 
PEGASUS_NAMESPACE_END
