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
	printf("CQLSimplePredicateRep::%d\n",_operator);
        if(_operator == EQ) printf("CQLSimplePredicateRep:: EQ\n");
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
  _leftSide.applyContext(queryContext);

  if (!isSimple())
  {
    _rightSide.applyContext(queryContext);
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
	return s;
}
Boolean CQLSimplePredicateRep::isSimple(){
        return _isSimple;
}
Boolean CQLSimplePredicateRep::isSimpleValue(){
	return _leftSide.isSimpleValue();
}
 
PEGASUS_NAMESPACE_END
