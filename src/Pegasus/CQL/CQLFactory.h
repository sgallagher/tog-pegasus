#ifndef Pegasus_CQLFactory_h
#define Pegasus_CQLFactory_h
                                                                            
#include <Pegasus/Common/Config.h>
#include <Pegasus/CQL/Linkage.h>
#include <Pegasus/Common/String.h>
#include <Pegasus/CQL/CQLValue.h>
//#include <Pegasus/CQL/CQLChainedIdentifier.h>
//#include <Pegasus/CQL/CQLSelectStatement.h>
#include <Pegasus/CQL/CQLPredicate.h>
#include <Pegasus/CQL/CQLPredicateRep.h>
#include <Pegasus/CQL/CQLSimplePredicate.h>
#include <Pegasus/CQL/CQLSimplePredicateRep.h>
//#include <Pegasus/CQL/CQLIdentifier.h>

#include <Pegasus/CQL/CQLTerm.h>
#include <Pegasus/CQL/CQLTermRep.h>
#include <Pegasus/CQL/CQLFactor.h>
#include <Pegasus/CQL/CQLFactorRep.h>
//#include <Pegasus/CQL/CQLParserState.h>
#include <Pegasus/CQL/CQLExpression.h>
#include <Pegasus/CQL/CQLExpressionRep.h>
#include <Pegasus/CQL/CQLFunction.h>
#include <Pegasus/CQL/CQLFunctionRep.h>

PEGASUS_NAMESPACE_BEGIN

enum FactoryType { Identifier, ChainedIdentifier, Value, Function, Factor, Term, Expression, SimplePredicate, Predicate };

class PEGASUS_CQL_LINKAGE CQLFactory
{
  public:

  void* makeObject(CQLIdentifier* obj, FactoryType target);
  void* makeObject(CQLChainedIdentifier* obj, FactoryType target);
  void* makeObject(CQLValue* obj, FactoryType target);
  void* makeObject(CQLFactor* obj, FactoryType target);
  void* makeObject(CQLTerm* obj, FactoryType target);
  void* makeObject(CQLExpression* obj, FactoryType target);
  void* makeObject(CQLSimplePredicate* obj, FactoryType target);
  CQLValue getValue(CQLFactor* obj);
  CQLValue getValue(CQLTerm* obj);
  CQLValue getValue(CQLExpression* obj);
  CQLValue getValue(CQLSimplePredicate* obj);
  CQLValue getValue(CQLPredicate* obj);
 void* getObject(void* inObject, FactoryType inObjectType, FactoryType targetType);
  void* getObject(CQLFactor* obj, FactoryType target);
  void* getObject(CQLTerm* obj, FactoryType target);
  void* getObject(CQLExpression* obj, FactoryType target);
  void* getObject(CQLSimplePredicate* obj, FactoryType target);
  void* getObject(CQLPredicate* obj, FactoryType target);
 
  void setObject(CQLPredicate* predicate, void* obj, FactoryType objType);
 
  CQLPredicate _predicate;
  CQLSimplePredicate _simplePredicate;
  CQLExpression _expression;
  CQLTerm _term;
  CQLFactor _factor;
  CQLFunction _function;
};                                                                          
PEGASUS_NAMESPACE_END
#endif
