#include "CQLFactory.h"

PEGASUS_NAMESPACE_BEGIN

struct CQLObjectPtr {
        void* _ptr;
}_CQLObjectPtr;

#define PEGASUS_ARRAY_T CQLObjectPtr
#include <Pegasus/Common/ArrayImpl.h>
#undef PEGASUS_ARRAY_T

/*
CQLFactory::CQLFactory(){

}

CQLFactory::~CQLFactory(){
	for(Uint32 i = 0; i < _predicates.size(); i++){
		if(_predicates[i]) delete _predicates[i];
	}
}
*/
void* CQLFactory::makeObject(CQLIdentifier* obj, FactoryType target){
printf("CQLFactory::makeObject(identifier)\n");
	_CQLObjectPtr._ptr = new CQLChainedIdentifier(*obj);
        _makeObjectChainedIdentifiers.append(_CQLObjectPtr);
	switch(target){
	  case ChainedIdentifier:
		return _makeObjectChainedIdentifiers[_makeObjectChainedIdentifiers.size()-1]._ptr;
		break;
	  case Identifier:
		return NULL;
		break;
          default:
		return makeObject((CQLChainedIdentifier*)(_CQLObjectPtr._ptr), target);
		break;
        }
}
void* CQLFactory::makeObject(CQLChainedIdentifier* obj, FactoryType target){
printf("CQLFactory::makeObject(chainedidentifier)\n");
	_CQLObjectPtr._ptr = new CQLValue(*obj);
        _makeObjectValues.append(_CQLObjectPtr);
	switch(target){
          case Value:
                return _makeObjectValues[_makeObjectValues.size()-1]._ptr;
                break;
          case ChainedIdentifier:
                return NULL;
                break;
          default:
                return makeObject((CQLValue*)(_CQLObjectPtr._ptr), target);
                break;
        }

}
void* CQLFactory::makeObject(CQLValue* obj, FactoryType target){
printf("CQLFactory::makeObject(value)\n");
	_CQLObjectPtr._ptr = new CQLFactor(*obj);
        _makeObjectFactors.append(_CQLObjectPtr);
	switch(target){
          case Factor:
                return _makeObjectFactors[_makeObjectFactors.size()-1]._ptr;
                break;
          case Value:
                return NULL;
                break;
          default:
                return makeObject((CQLFactor*)(_CQLObjectPtr._ptr), target);
                break;
        }

}
void* CQLFactory::makeObject(CQLFunction* obj, FactoryType target){
printf("CQLFactory::makeObject(function)\n");
	_CQLObjectPtr._ptr = new CQLFactor(*obj);
        _makeObjectFactors.append(_CQLObjectPtr);
        switch(target){
          case Factor:
                return _makeObjectFactors[_makeObjectFactors.size()-1]._ptr;
                break;
          default:
                return makeObject((CQLFactor*)(_CQLObjectPtr._ptr), target);
                break;
        }
}
void* CQLFactory::makeObject(CQLFactor* obj, FactoryType target){
printf("CQLFactory::makeObject(factor)\n");
	_CQLObjectPtr._ptr = new CQLTerm(*obj);
        _makeObjectTerms.append(_CQLObjectPtr);
	switch(target){
          case Term:
                return _makeObjectTerms[_makeObjectTerms.size()-1]._ptr;
                break;
          case Factor:
                return NULL;
                break;
          default:
                return makeObject((CQLTerm*)(_CQLObjectPtr._ptr), target);
                break;
        }

}
void* CQLFactory::makeObject(CQLTerm* obj, FactoryType target){
printf("CQLFactory::makeObject(term)\n");
	_CQLObjectPtr._ptr = new CQLExpression(*obj);
        _makeObjectExpressions.append(_CQLObjectPtr);
	switch(target){
          case Expression:
                return _makeObjectExpressions[_makeObjectExpressions.size()-1]._ptr;
                break;
          case Term:
                return NULL;
                break;
          default:
                return makeObject((CQLExpression*)(_CQLObjectPtr._ptr), target);
                break;
        }

}
void* CQLFactory::makeObject(CQLExpression* obj, FactoryType target){
printf("CQLFactory::makeObject(expression)\n");
	_CQLObjectPtr._ptr = new CQLSimplePredicate(*obj);
	_makeObjectSimplePredicates.append(_CQLObjectPtr);
	switch(target){
          case SimplePredicate:
                return _makeObjectSimplePredicates[_makeObjectSimplePredicates.size()-1]._ptr;
                break;
          case Expression:
                return NULL;
                break;
          default:
                return makeObject((CQLSimplePredicate*)(_CQLObjectPtr._ptr), target);
                break;
        }

}
void* CQLFactory::makeObject(CQLSimplePredicate* obj, FactoryType target){
printf("CQLFactory::makeObject(simplepredicate)\n");
	_CQLObjectPtr._ptr = new CQLPredicate(*obj);
	_makeObjectPredicates.append(_CQLObjectPtr);
	switch(target){
          case Predicate:
                return _makeObjectPredicates[_makeObjectPredicates.size()-1]._ptr;
                break;
          default:
                return NULL;
                break;
        }
}
/*
CQLValue CQLFactory::getValue(CQLFactor* obj){
	if(obj->isSimpleValue()){
		return obj->getValue();
	}
	return CQLValue();
}

CQLValue CQLFactory::getValue(CQLTerm* obj){
	if(obj->isSimpleValue()){
		 _factor = obj->getFactors()[0];
		return getValue(&_factor);
	}
	return CQLValue();
}

CQLValue CQLFactory::getValue(CQLExpression* obj){
        if(obj->isSimpleValue()){
		 _term = (obj->getTerms())[0];
		return getValue(&_term);
	}
	return CQLValue();
}

CQLValue CQLFactory::getValue(CQLSimplePredicate* obj){
	_expression = obj->getLeftExpression();
	return getValue(&_expression);
}

CQLValue CQLFactory::getValue(CQLPredicate* obj){
        if(obj->isSimple()){
		_simplePredicate = obj->getSimplePredicate();
		return getValue(&_simplePredicate);
	}
	return CQLValue();
}
*/
void* CQLFactory::getObject(void* inObject, FactoryType inObjectType, FactoryType targetType){
	switch(inObjectType){
		case Predicate:
			return getObject(((CQLPredicate*)inObject),targetType);
		case SimplePredicate:
			return getObject(((CQLSimplePredicate*)inObject),targetType);
		case Expression:
			return getObject(((CQLExpression*)inObject),targetType);
		case Term:
			return getObject(((CQLTerm*)inObject),targetType);
		case Factor:
			return getObject(((CQLFactor*)inObject),targetType);
		case Function:
			return NULL;
		case Value:
			return getObject(((CQLValue*)inObject),targetType);
		case ChainedIdentifier:
			return getObject(((CQLChainedIdentifier*)inObject),targetType);
		default:
			return NULL;
	}
}

void* CQLFactory::getObject(CQLChainedIdentifier* obj, FactoryType target){
	printf("CQLFactory::getObject(CQLChainedIdentifier* obj)\n");
	switch(target){
          case Identifier:
		if(obj->_rep->_subIdentifiers.size() > 0){
			_CQLObjectPtr._ptr = new CQLIdentifier(obj->_rep->_subIdentifiers[0]);
			_getObjectIdentifiers.append(_CQLObjectPtr);
                	return _getObjectIdentifiers[_getObjectIdentifiers.size()-1]._ptr;
		}
		return NULL;
          default:
                return NULL;
        }
}

void* CQLFactory::getObject(CQLValue* obj, FactoryType target){
	printf("CQLFactory::getObject(CQLValue* obj)\n");
	switch(target){
          case ChainedIdentifier:
		_CQLObjectPtr._ptr = new CQLChainedIdentifier(obj->_CQLChainId);
		_getObjectChainedIdentifiers.append(_CQLObjectPtr);
		return _getObjectChainedIdentifiers[_getObjectChainedIdentifiers.size()-1]._ptr;
	  case Identifier:
		return getObject(&(obj->_CQLChainId),target);
          default:
                return NULL;
                break;
        }
}

void* CQLFactory::getObject(CQLFactor* obj, FactoryType target){
	printf("CQLFactory::getObject(CQLFactor* obj)\n");
        switch(target){
          case Function:
		_CQLObjectPtr._ptr = new CQLFunction(obj->_rep->_CQLFunct);
                _getObjectFunctions.append(_CQLObjectPtr);
		return _getObjectFunctions[_getObjectFunctions.size()-1]._ptr;
	  case Value:
		_CQLObjectPtr._ptr = new CQLValue(obj->_rep->_CQLVal);
                _getObjectValues.append(_CQLObjectPtr);
		return _getObjectValues[_getObjectValues.size()-1]._ptr;
          case ChainedIdentifier:
          case Identifier:
		return getObject(&(obj->_rep->_CQLVal),target);
          default:
                return NULL;
                break;
        }                                                                                                                                                    
}


void* CQLFactory::getObject(CQLTerm* obj, FactoryType target){
	printf("CQLFactory::getObject(CQLTerm* obj)\n");
        switch(target){
	  case Factor:
		_CQLObjectPtr._ptr = new CQLFactor(obj->getFactors()[0]);
                _getObjectFactors.append(_CQLObjectPtr);
		return _getObjectFactors[_getObjectFactors.size()-1]._ptr;
          case Function:
	  case Value:
          case ChainedIdentifier:
          case Identifier:
		return getObject(&(obj->getFactors()[0]),target);
          default:
                return NULL;
                break;
        }
}

void* CQLFactory::getObject(CQLExpression* obj, FactoryType target){
	printf("CQLFactory::getObject(CQLExpression* obj)\n");
        switch(target){
	  case Term:
		_CQLObjectPtr._ptr = new CQLTerm(obj->getTerms()[0]);
                _getObjectTerms.append(_CQLObjectPtr);
                return _getObjectTerms[_getObjectTerms.size()-1]._ptr;
          case Factor:
          case Function:
	  case Value:
          case ChainedIdentifier:
          case Identifier:
		return getObject(&(obj->getTerms()[0]), target);
          default:
		return NULL;
                break;
        }                                                                                                                                                    
}

void* CQLFactory::getObject(CQLSimplePredicate* obj, FactoryType target){
	printf("CQLFactory::getObject(CQLSimplePredicate* obj)\n");
        switch(target){
	  case Expression:
		_CQLObjectPtr._ptr = new CQLExpression(obj->_rep->_leftSide);
                _getObjectExpressions.append(_CQLObjectPtr);
                return _getObjectExpressions[_getObjectExpressions.size()-1]._ptr;
          case Term:
          case Factor:
          case Function:
	  case Value:
          case ChainedIdentifier:
          case Identifier:
		return getObject(&(obj->_rep->_leftSide), target);
          default:
                return NULL;
                break;
        }
}

void* CQLFactory::getObject(CQLPredicate* obj, FactoryType target){
	printf("CQLFactory::getObject(CQLPredicate* obj)\n");
        switch(target){
          case SimplePredicate:
		_CQLObjectPtr._ptr = new CQLSimplePredicate(obj->_rep->_simplePredicate);
                _getObjectSimplePredicates.append(_CQLObjectPtr);
                return _getObjectSimplePredicates[_getObjectSimplePredicates.size()-1]._ptr;
	  case Expression:
	  case Term:
	  case Factor:
	  case Function:
	  case Value:
	  case ChainedIdentifier:
	  case Identifier:
		return getObject(&(obj->_rep->_simplePredicate), target);
          default:
                return NULL;
                break;
        }
}

void CQLFactory::setObject(CQLPredicate* predicate, void* obj, FactoryType objType){
	switch(objType){
	  case SimplePredicate:
		predicate->_rep->_simplePredicate = *((CQLSimplePredicate*)obj);
		break;
	  case Expression:
		predicate->_rep->_simplePredicate._rep->_leftSide = *((CQLExpression*)obj);
		break;
          case Term:
          	predicate->_rep->_simplePredicate._rep->_leftSide._rep->_CQLTerms[0] =  
			*((CQLTerm*)obj);
		break;
          case Factor:
		predicate->_rep->_simplePredicate._rep->_leftSide._rep->_CQLTerms[0]._rep->_Factors[0] = 
			*((CQLFactor*)obj);
		break;
          case Function:
		predicate->_rep->_simplePredicate._rep->_leftSide._rep->_CQLTerms[0]._rep->_Factors[0]._rep->_CQLFunct =
                        *((CQLFunction*)obj);
		break;
	  case Value:
		predicate->_rep->_simplePredicate._rep->_leftSide._rep->_CQLTerms[0]._rep->_Factors[0]._rep->_CQLVal =
                        *((CQLValue*)obj);
		break;
	  case ChainedIdentifier:
		predicate->_rep->_simplePredicate._rep->_leftSide._rep->_CQLTerms[0]._rep->_Factors[0]._rep->_CQLVal._CQLChainId = *((CQLChainedIdentifier*)obj);
		break;
	  case Identifier:
		break;
	  default:
		break;
	}
} 
//void CQLFactory::print(){printf("%s\n",(const char*)(_chainedIdentifier.toString().getCString()));}
PEGASUS_NAMESPACE_END
