#ifndef Pegasus_CQLFactory_h
#define Pegasus_CQLFactory_h
                                                                            
#include <Pegasus/Common/Config.h>
#include <Pegasus/CQL/Linkage.h>
#include <Pegasus/Common/String.h>
#include <Pegasus/CQL/CQLValue.h>
#include <Pegasus/CQL/CQLChainedIdentifier.h>
#include <Pegasus/CQL/CQLSelectStatement.h>
#include <Pegasus/CQL/CQLPredicate.h>
#include <Pegasus/CQL/CQLSimplePredicate.h>
#include <Pegasus/CQL/CQLIdentifier.h>
#include <Pegasus/CQL/CQLTerm.h>
#include <Pegasus/CQL/CQLFactor.h>
#include <Pegasus/CQL/CQLParserState.h>
#include <Pegasus/CQL/CQLExpression.h>

PEGASUS_NAMESPACE_BEGIN

enum FactoryType { Identifier, ChainedIdentifier, Value, Factor, Term, Expression, SimplePredicate, Predicate };

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
  
};                                                                          
PEGASUS_NAMESPACE_END
#endif
