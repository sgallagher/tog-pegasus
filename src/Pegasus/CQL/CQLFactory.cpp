#include "CQLFactory.h"

PEGASUS_NAMESPACE_BEGIN
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
	_chainedIdentifier = CQLChainedIdentifier(*obj);
	switch(target){
	  case ChainedIdentifier:
		return &_chainedIdentifier;
		break;
	  case Identifier:
		return NULL;
		break;
          default:
		return makeObject(&_chainedIdentifier, target);
		break;
        }
}
void* CQLFactory::makeObject(CQLChainedIdentifier* obj, FactoryType target){
printf("CQLFactory::makeObject(chainedidentifier)\n");
	_value = CQLValue(*obj);
	switch(target){
          case Value:
                return &_value;
                break;
          case ChainedIdentifier:
                return NULL;
                break;
          default:
                return makeObject(&_value, target);
                break;
        }

}
void* CQLFactory::makeObject(CQLValue* obj, FactoryType target){
printf("CQLFactory::makeObject(value)\n");
	_factor = CQLFactor(*obj);
	CQLValue val = _factor._rep->_CQLVal;
	switch(target){
          case Factor:
                return &_factor;
                break;
          case Value:
                return NULL;
                break;
          default:
                return makeObject(&_factor, target);
                break;
        }

}
void* CQLFactory::makeObject(CQLFunction* obj, FactoryType target){
printf("CQLFactory::makeObject(function)\n");
	_factor = CQLFactor(*obj);
        switch(target){
          case Factor:
                return &_factor;
                break;
          default:
                return makeObject(&_factor, target);
                break;
        }
}
void* CQLFactory::makeObject(CQLFactor* obj, FactoryType target){
printf("CQLFactory::makeObject(factor)\n");
	_term = CQLTerm(*obj);
	switch(target){
          case Term:
                return &_term;
                break;
          case Factor:
                return NULL;
                break;
          default:
                return makeObject(&_term, target);
                break;
        }

}
void* CQLFactory::makeObject(CQLTerm* obj, FactoryType target){
printf("CQLFactory::makeObject(term)\n");
	_expression = CQLExpression(*obj);
	switch(target){
          case Expression:
                return &_expression;
                break;
          case Term:
                return NULL;
                break;
          default:
                return makeObject(&_expression, target);
                break;
        }

}
void* CQLFactory::makeObject(CQLExpression* obj, FactoryType target){
printf("CQLFactory::makeObject(expression)\n");
	_simplePredicate = CQLSimplePredicate(*obj);
	switch(target){
          case SimplePredicate:
                return &_simplePredicate;
                break;
          case Expression:
                return NULL;
                break;
          default:
                return makeObject(&_simplePredicate, target);
                break;
        }

}
void* CQLFactory::makeObject(CQLSimplePredicate* obj, FactoryType target){
printf("CQLFactory::makeObject(simplepredicate)\n");
	_predicate = CQLPredicate(*obj);
	switch(target){
          case Predicate:
                return &_predicate;
                break;
          default:
                return NULL;
                break;
        }
}

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
	switch(target){
          case Identifier:
		if(obj->_rep->_subIdentifiers.size() > 0){
                	return &(obj->_rep->_subIdentifiers[0]);
                	//return &_identifier;
		}
		return NULL;
          default:
                return NULL;
        }
}

void* CQLFactory::getObject(CQLValue* obj, FactoryType target){
	switch(target){
          case ChainedIdentifier:
                return &(obj->_CQLChainId);
                //return &_chainedIdentifier;
          default:
                return NULL;
                break;
        }
}

void* CQLFactory::getObject(CQLFactor* obj, FactoryType target){
        switch(target){
          case Function:
		return &(obj->_rep->_CQLFunct);
		//return &_function;
          default:
                return NULL;
                break;
        }                                                                                                                                                    
}


void* CQLFactory::getObject(CQLTerm* obj, FactoryType target){
        switch(target){
	  case Factor:
		return &(obj->getFactors()[0]);
		//return &_factor;
          case Function:
		_factor = obj->getFactors()[0];
		return getObject(&(obj->getFactors()[0]),target);
          default:
                return NULL;
                break;
        }
}

void* CQLFactory::getObject(CQLExpression* obj, FactoryType target){
        switch(target){
	  case Term:
		return  &(obj->getTerms()[0]);
		//return &_term;
          case Factor:
          case Function:
		_term = obj->getTerms()[0];
		return getObject(&(obj->getTerms()[0]), target);
          default:
		return NULL;
                break;
        }                                                                                                                                                    
}

void* CQLFactory::getObject(CQLSimplePredicate* obj, FactoryType target){
        switch(target){
	  case Expression:
		return &(obj->_rep->_leftSide);
		//return &_expression;
          case Term:
          case Factor:
          case Function:
		_expression =  obj->getLeftExpression();
		return getObject(&(obj->_rep->_leftSide), target);
          default:
                return NULL;
                break;
        }
}

void* CQLFactory::getObject(CQLPredicate* obj, FactoryType target){
        switch(target){
          case SimplePredicate:
		return &(obj->_rep->_simplePredicate);
                //return &_simplePredicate;
                break;
	  case Expression:
	  case Term:
	  case Factor:
	  case Function:
		_simplePredicate = obj->getSimplePredicate();
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
	  default:
		break;
	}
} 
void CQLFactory::print(){printf("%s\n",(const char*)(_chainedIdentifier.toString().getCString()));}
PEGASUS_NAMESPACE_END
