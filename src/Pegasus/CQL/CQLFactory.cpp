#include "CQLFactory.h"

PEGASUS_NAMESPACE_BEGIN

void* CQLFactory::makeObject(CQLIdentifier* obj, FactoryType target){
	//printf("%s\n",(const char*)(obj->toString().getCString()));
	//printf("%s\n",(const char*)(_cid.toString().getCString()));
	switch(target){
	  case ChainedIdentifier:
		_chainedIdentifier = CQLChainedIdentifier(*obj);
		//printf("%d\n",_chainedIdentifier.size());
		//printf("%s\n",(const char*)(_chainedIdentifier.toString().getCString()));
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
	switch(target){
          case Value:
		_value = CQLValue(*obj);
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
	switch(target){
          case Factor:
		_factor = CQLFactor(*obj);
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
        switch(target){
          case Factor:
		_factor = CQLFactor(*obj);
                return &_factor;
                break;
          default:
                return makeObject(&_factor, target);
                break;
        }
}
void* CQLFactory::makeObject(CQLFactor* obj, FactoryType target){
	switch(target){
          case Term:
		_term = CQLTerm(*obj);
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
	switch(target){
          case Expression:
		_expression = CQLExpression(*obj);
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
	switch(target){
          case SimplePredicate:
		_simplePredicate = CQLSimplePredicate(*obj);
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
	switch(target){
          case Predicate:
		_predicate = CQLPredicate(*obj);
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
                	_identifier = obj->_rep->_subIdentifiers[0];
                	return &_identifier;
		}
		return NULL;
          default:
                return NULL;
        }
}

void* CQLFactory::getObject(CQLValue* obj, FactoryType target){
	switch(target){
          case ChainedIdentifier:
                _chainedIdentifier = obj->getChainedIdentifier();
                return &_chainedIdentifier;
          default:
                return NULL;
                break;
        }
}

void* CQLFactory::getObject(CQLFactor* obj, FactoryType target){
        switch(target){
          case Function:
		_function =  obj->getCQLFunction();
		return &_function;
          default:
                return NULL;
                break;
        }                                                                                                                                                    
}


void* CQLFactory::getObject(CQLTerm* obj, FactoryType target){
        switch(target){
	  case Factor:
		_factor = obj->getFactors()[0];
		return &_factor;
          case Function:
		_factor = obj->getFactors()[0];
		return getObject(&_factor,target);
          default:
                return NULL;
                break;
        }
}

void* CQLFactory::getObject(CQLExpression* obj, FactoryType target){
        switch(target){
	  case Term:
		_term = obj->getTerms()[0];
		return &_term;
          case Factor:
          case Function:
		_term = obj->getTerms()[0];
		return getObject(&_term, target);
          default:
		return NULL;
                break;
        }                                                                                                                                                    
}

void* CQLFactory::getObject(CQLSimplePredicate* obj, FactoryType target){
        switch(target){
	  case Expression:
		_expression =  obj->getLeftExpression();
          case Term:
          case Factor:
          case Function:
		_expression =  obj->getLeftExpression();
		return getObject(&_expression, target);
          default:
                return NULL;
                break;
        }
}

void* CQLFactory::getObject(CQLPredicate* obj, FactoryType target){
        switch(target){
          case SimplePredicate:
		_simplePredicate = obj->getSimplePredicate();
                return &_simplePredicate;
                break;
	  case Expression:
	  case Term:
	  case Factor:
	  case Function:
		_simplePredicate = obj->getSimplePredicate();
		return getObject(&_simplePredicate, target);
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
