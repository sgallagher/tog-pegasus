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


PEGASUS_NAMESPACE_END
