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
  // Resolve the value of the left side
  CQLValue leftVal = _leftSide.resolveValue(CI, QueryCtx);

  // If there isn't a right side then operator must by IS_NULL
  // or IS_NOT_NULL
  if (isSimple())
  {
    PEGASUS_ASSERT(_operator == IS_NULL || _operator == IS_NOT_NULL);

    return (_operator == IS_NULL) ? leftVal.isNull() : !leftVal.isNull();
  }

  PEGASUS_ASSERT(_operator != IS_NULL && _operator != IS_NOT_NULL);

  if (leftVal.isNull())
  {
    // The null contagion rule.  See CQLSelectStatementRep for details.  
    // ATTN - change this to a specific CQLException so that it can
    // be caught above
    throw UninitializedObjectException(); 
  }

  if (_operator == ISA)
  {
    // Special processing for ISA.  The CQLValue on the right side of ISA
    // has a CQLChainedIdentifier with one element that contains
    // the class name.  We don't want to resolve the right side because
    // CQLValue would assume that a single element chained identifier
    // refers to an instance of the FROM class.
    if (!_rightSide.isSimpleValue())
    {
      throw Exception("TEMP MSG: CQLSimplePredicate::evaluate - rhs of isa is not simple");
    }

    CQLValue isaRightVal = _rightSide.getTerms()[0].getFactors()[0].getValue();
    CQLChainedIdentifier isaRightId = isaRightVal.getChainedIdentifier();
    return leftVal.isa(isaRightId, QueryCtx);
  }

  if (_operator == LIKE)
  {
    // Special processing for LIKE.  The right side expression must be
    // a simple CQLValue (ie. not nested, and not a function), and
    // must be a literal.  Note that this code will test whether
    // the right side is a literal, but not whether it is a string
    // literal.
    if (!_rightSide.isSimpleValue())
    {
      throw Exception("TEMP MSG: CQLSimplePredicate::evaluate - rhs of like is not simple");
    }

    CQLValue likeRightVal = _rightSide.getTerms()[0].getFactors()[0].getValue();
    if (!likeRightVal.isResolved())
    {
      throw Exception("TEMP MSG: CQLSimplePredicate::evaluate - rhs of like is not a literal");    
    }

    return leftVal.like(likeRightVal);
  }

  // No special processing needed.
  // Resolve the value of the right side
  CQLValue rightVal = _rightSide.resolveValue(CI, QueryCtx);

  if (rightVal.isNull())
  {
    // The null contagion rule.  See CQLSelectStatementRep for details. 
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

  case LIKE:
  case ISA:
    // Never get here due to special processing above.
    PEGASUS_ASSERT(false);
  case IS_NULL:
  case IS_NOT_NULL:
    // Never get here due to the assert.
    break;
  }

  return true;  // keep the compiler happy
}

CQLExpression CQLSimplePredicateRep::getLeftExpression()const
{
	return _leftSide;
}

CQLExpression CQLSimplePredicateRep::getRightExpression()const
{
        return _rightSide;
}

enum ExpressionOpType CQLSimplePredicateRep::getOperation()const
{
	return _operator;
}

void CQLSimplePredicateRep::applyContext(QueryContext& queryContext)
{
  CQLIdentifier _id;

   _id = _leftSide.getTerms()[0].getFactors()[0].
                  getValue().getChainedIdentifier().getLastIdentifier();

   if(_leftSide.isSimpleValue() &&
      _id.isSymbolicConstant() &&
         _id.getName().getString().size() == 0)
   {
      // We have a standalone symbolic constant.
      if(!isSimple() && 
         _rightSide.isSimpleValue() &&
         _rightSide.getTerms()[0].getFactors()[0].
                getValue().getChainedIdentifier().getLastIdentifier().
                getName().getString().size() > 0)
      {
         _rightSide.applyContext(queryContext);

         // We need to add context to the symbolic constant
         _leftSide.applyContext(queryContext,
                                _rightSide.getTerms()[0].getFactors()[0].
                                getValue().getChainedIdentifier()); 
      }
      else
      {
         // There is no valid context for the symbolic constant
         throw(Exception(String("CQLSimplePredicateRep::applyContext -- standalone symbolic constant error")));
      }
   }
   else
   {
      _leftSide.applyContext(queryContext);
   }

   if (!isSimple())
   {
      _id = _rightSide.getTerms()[0].getFactors()[0].
                     getValue().getChainedIdentifier().getLastIdentifier();

      if(_rightSide.isSimpleValue() &&
         _id.isSymbolicConstant() &&
         _id.getName().getString().size() == 0)
      {
         // We have a standalone symbolic constant.
         if(!isSimple() && 
            _leftSide.isSimpleValue() &&
            _leftSide.getTerms()[0].getFactors()[0].
                   getValue().getChainedIdentifier().getLastIdentifier().
                   getName().getString().size() > 0)
         {
            // We need to add context to the symbolic constant
            _rightSide.applyContext(queryContext,
                                   _leftSide.getTerms()[0].getFactors()[0].
                                   getValue().getChainedIdentifier());               
         }
         else
         {
            // There is no valid context for the symbolic constant
            throw(Exception(String("CQLSimplePredicateRep::applyContext -- standalone symbolic constant error")));
         }
      }
      else
      {
        // Right side is not simple OR it is a not a standalone symbolic constant
        if (_operator != ISA)
        {
          // Not doing an ISA, go ahead and applyContext to right side
          _rightSide.applyContext(queryContext);
        }
        else
        {
          // Operation is an ISA.. The right side must be simple.
          // We don't want to applyContext to the right side because
          // it could be a classname unrelated to the FROM class.
          if (!_rightSide.isSimpleValue())
          {
            throw Exception("CQLSimplePredicateRep::applyContext -- right side of ISA is not simple");
          }
        }
      }
   }
}

String CQLSimplePredicateRep::toString()const
{
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
Boolean CQLSimplePredicateRep::isSimple()const{
        return _isSimple;
}
Boolean CQLSimplePredicateRep::isSimpleValue()const{
	return _leftSide.isSimpleValue();
}

void CQLSimplePredicateRep::setOperation(ExpressionOpType op){
	_operator = op;
} 
PEGASUS_NAMESPACE_END
