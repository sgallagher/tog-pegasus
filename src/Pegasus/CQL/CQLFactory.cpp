#include "CQLFactory.h"

PEGASUS_NAMESPACE_BEGIN

void* CQLFactory::makeObject(CQLIdentifier* obj, FactoryType target){
	switch(target){
	  case ChainedIdentifier:
		return new CQLChainedIdentifier(*obj);
		break;
	  case Identifier:
		return NULL;
		break;
          default:
		return makeObject(new CQLChainedIdentifier(*obj), target);
		break;
        }
}
void* CQLFactory::makeObject(CQLChainedIdentifier* obj, FactoryType target){
	switch(target){
          case Value:
                return new CQLValue(*obj);
                break;
          case ChainedIdentifier:
                return NULL;
                break;
          default:
                return makeObject(new CQLValue(*obj), target);
                break;
        }

}
void* CQLFactory::makeObject(CQLValue* obj, FactoryType target){
	switch(target){
          case Factor:
                return new CQLFactor(*obj);
                break;
          case Value:
                return NULL;
                break;
          default:
                return makeObject(new CQLFactor(*obj), target);
                break;
        }

}
void* CQLFactory::makeObject(CQLFactor* obj, FactoryType target){
	switch(target){
          case Term:
                return new CQLTerm(*obj);
                break;
          case Factor:
                return NULL;
                break;
          default:
                return makeObject(new CQLTerm(*obj), target);
                break;
        }

}
void* CQLFactory::makeObject(CQLTerm* obj, FactoryType target){
	switch(target){
          case Expression:
                return new CQLExpression(*obj);
                break;
          case Term:
                return NULL;
                break;
          default:
                return makeObject(new CQLExpression(*obj), target);
                break;
        }

}
void* CQLFactory::makeObject(CQLExpression* obj, FactoryType target){
	switch(target){
          case SimplePredicate:
                return new CQLSimplePredicate(*obj);
                break;
          case Expression:
                return NULL;
                break;
          default:
                return makeObject(new CQLSimplePredicate(*obj), target);
                break;
        }

}
void* CQLFactory::makeObject(CQLSimplePredicate* obj, FactoryType target){
	switch(target){
          case Predicate:
                return new CQLPredicate(*obj);
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
	/* 
	   We cast inObject based on inObjectType, traverse the tree until
 	   we find the targetType and return that.  inObjectType must be higher in 
	   the inheritance heirarchy than targetType.
	*/
	
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
			return NULL;
		default:
			return NULL;
	}
}

void* CQLFactory::getObject(CQLFactor* obj, FactoryType target){
        switch(target){
          case Function:
		return obj->_CQLFunct;
          default:
                return NULL;
                break;
        }                                                                                                                                                    
}


void* CQLFactory::getObject(CQLTerm* obj, FactoryType target){
        switch(target){
	  case Factor:
		return &(obj->_Factors[0]);
          case Function:
		return getObject(&(obj->_Factors[0]),target);
          default:
                return NULL;
                break;
        }
}

void* CQLFactory::getObject(CQLExpression* obj, FactoryType target){
        switch(target){
	  case Term:
		return &(obj->_CQLTerms[0]);
          case Factor:
          case Function:
		return getObject(&(obj->_CQLTerms[0]), target);
          default:
		return NULL;
                break;
        }                                                                                                                                                    
}

void* CQLFactory::getObject(CQLSimplePredicate* obj, FactoryType target){
        switch(target){
	  case Expression:
		return obj->_leftSide;
          case Term:
          case Factor:
          case Function:
		return getObject(obj->_leftSide, target);
          default:
                return NULL;
                break;
        }
}

void* CQLFactory::getObject(CQLPredicate* obj, FactoryType target){
        switch(target){
          case SimplePredicate:
                return &(obj->_simplePredicate);
                break;
	  case Expression:
	  case Term:
	  case Factor:
	  case Function:
		return getObject(&(obj->_simplePredicate), target);
          default:
                return NULL;
                break;
        }
}


PEGASUS_NAMESPACE_END
