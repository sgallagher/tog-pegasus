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

CQLSimplePredicateRep::CQLSimplePredicateRep(const CQLExpression& inExpression, const ExpressionOpType inOperator)
{
	_leftSide = inExpression;
	_operator = inOperator;
	_isSimple = true;
}

CQLSimplePredicateRep::CQLSimplePredicateRep(const CQLExpression& leftSideExpression, const CQLExpression& rightSideExpression, ExpressionOpType inOperator)
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
	return false;
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

String CQLSimplePredicateRep::toString()
{
	printf("CQLSimplePredicate::toString()\n");
	String s = _leftSide.toString();
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
	return s;
}
Boolean CQLSimplePredicateRep::isSimple(){
        return _isSimple;
}
Boolean CQLSimplePredicateRep::isSimpleValue(){
	return _leftSide.isSimpleValue();
}
 
PEGASUS_NAMESPACE_END
