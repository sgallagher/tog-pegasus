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

#ifndef Pegasus_CQLFactory_h
#define Pegasus_CQLFactory_h
                                                                            
#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/ArrayInternal.h>
#include <Pegasus/CQL/Linkage.h>
#include <Pegasus/Common/String.h>
#include <Pegasus/CQL/CQLValue.h>
#include <Pegasus/CQL/CQLValueRep.h>
#include <Pegasus/CQL/CQLChainedIdentifier.h>
#include <Pegasus/CQL/CQLChainedIdentifierRep.h>
//#include <Pegasus/CQL/CQLSelectStatement.h>
#include <Pegasus/CQL/CQLPredicate.h>
#include <Pegasus/CQL/CQLPredicateRep.h>
#include <Pegasus/CQL/CQLSimplePredicate.h>
#include <Pegasus/CQL/CQLSimplePredicateRep.h>
#include <Pegasus/CQL/CQLIdentifier.h>

#include <Pegasus/CQL/CQLTerm.h>
#include <Pegasus/CQL/CQLTermRep.h>
#include <Pegasus/CQL/CQLFactor.h>
#include <Pegasus/CQL/CQLFactorRep.h>
//#include <Pegasus/CQL/CQLParserState.h>
#include <Pegasus/CQL/CQLExpression.h>
#include <Pegasus/CQL/CQLExpressionRep.h>
#include <Pegasus/CQL/CQLFunction.h>
#include <Pegasus/CQL/CQLFunctionRep.h>

#ifdef PEGASUS_USE_EXPERIMENTAL_INTERFACES

PEGASUS_NAMESPACE_BEGIN

typedef struct CQLObjectPtr {
        void* _ptr;
} CQLOBJECTPTR;

/*
#ifndef PEGASUS_ARRAY_T
#define PEGASUS_ARRAY_T CQLObjectPtr
#include <Pegasus/Common/ArrayInter.h>
#undef PEGASUS_ARRAY_T
#endif
*/

enum FactoryType { Identifier, ChainedIdentifier, Value, Function, Factor, Term, Expression, SimplePredicate, Predicate };

class PEGASUS_CQL_LINKAGE CQLFactory
{
  public:
  //CQLFactory();
  ~CQLFactory();
  void* makeObject(CQLIdentifier* obj, FactoryType target);
  void* makeObject(CQLChainedIdentifier* obj, FactoryType target);
  void* makeObject(CQLValue* obj, FactoryType target);
  void* makeObject(CQLFactor* obj, FactoryType target);
  void* makeObject(CQLFunction* obj, FactoryType target);
  void* makeObject(CQLTerm* obj, FactoryType target);
  void* makeObject(CQLExpression* obj, FactoryType target);
  void* makeObject(CQLSimplePredicate* obj, FactoryType target);
  
  void* getObject(void* inObject, FactoryType inObjectType, FactoryType targetType);

  void* getObject(CQLChainedIdentifier* obj, FactoryType target);
  void* getObject(CQLValue* obj, FactoryType target);
  void* getObject(CQLFactor* obj, FactoryType target);
  void* getObject(CQLTerm* obj, FactoryType target);
  void* getObject(CQLExpression* obj, FactoryType target);
  void* getObject(CQLSimplePredicate* obj, FactoryType target);
  void* getObject(CQLPredicate* obj, FactoryType target);
 
  void setObject(CQLPredicate* predicate, void* obj, FactoryType objType);
  void cleanup();
private: 
  void cleanupArray(Array<CQLObjectPtr>& arr, FactoryType type);

  Array<CQLObjectPtr> _makeObjectPredicates;
  Array<CQLObjectPtr> _makeObjectSimplePredicates;
  Array<CQLObjectPtr> _makeObjectExpressions;
  Array<CQLObjectPtr> _makeObjectTerms;
  Array<CQLObjectPtr> _makeObjectFactors;
  Array<CQLObjectPtr> _makeObjectFunctions;
  Array<CQLObjectPtr> _makeObjectValues;
  Array<CQLObjectPtr> _makeObjectChainedIdentifiers;
  Array<CQLObjectPtr> _makeObjectIdentifiers;

  Array<CQLObjectPtr> _getObjectPredicates;
  Array<CQLObjectPtr> _getObjectSimplePredicates;
  Array<CQLObjectPtr> _getObjectExpressions;
  Array<CQLObjectPtr> _getObjectTerms;
  Array<CQLObjectPtr> _getObjectFactors;
  Array<CQLObjectPtr> _getObjectFunctions;
  Array<CQLObjectPtr> _getObjectValues;
  Array<CQLObjectPtr> _getObjectChainedIdentifiers;
  Array<CQLObjectPtr> _getObjectIdentifiers;

};                                                                          
PEGASUS_NAMESPACE_END
#endif
#endif
