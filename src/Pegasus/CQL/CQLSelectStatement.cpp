#include "CQLSelectStatement.h"
#include "CQLSelectStatementRep.h"

PEGASUS_NAMESPACE_BEGIN

CQLSelectStatement::CQLSelectStatement(String inQlang, String inQuery, QueryContext& inCtx)
{
	_rep = new CQLSelectStatementRep(inQlang,inQuery,inCtx);
}

CQLSelectStatement::~CQLSelectStatement(){
	if(_rep)
		delete _rep;
}

Boolean CQLSelectStatement::evaluate(CIMInstance const inCI)
{
   return _rep->evaluate(inCI);
}

Array<CIMInstance> CQLSelectStatement::executeQuery(Array<CIMInstance> inCIMInstanceArray) throw(Exception)
{
   return _rep->executeQuery(inCIMInstanceArray);
}

CIMInstance CQLSelectStatement::applyProjection(CIMInstance inCI) throw(Exception)
{
   return _rep->applyProjection(inCI);
}

void CQLSelectStatement::validateClass(const CIMObjectPath& inClassName) throw(Exception)
{
	_rep->validateClass(inClassName);
}

void CQLSelectStatement::validateProperties() throw(Exception)
{
	_rep->validateProperties();
}

Array<CIMObjectPath> const CQLSelectStatement::getClassPathList()
{
 	return _rep->getClassPathList();
}

CIMPropertyList CQLSelectStatement::getPropertyList(const CIMObjectPath& inClassName)
{
   return _rep->getPropertyList(inClassName);
}

void CQLSelectStatement::appendClassPath(const CQLIdentifier& inIdentifier)
{
	_rep->appendClassPath(inIdentifier);
}

void CQLSelectStatement::setPredicate(CQLPredicate inPredicate)
{
	_rep->setPredicate(inPredicate);
}

void CQLSelectStatement::insertClassPathAlias(const CQLIdentifier& inIdentifier, String inAlias)
{
	_rep->insertClassPathAlias(inIdentifier,inAlias);
}

void CQLSelectStatement::appendSelectIdentifier(const CQLChainedIdentifier& x)
{
	_rep->appendSelectIdentifier(x);
}

Boolean CQLSelectStatement::appendWhereIdentifier(const CQLChainedIdentifier& x)
{
   return _rep->appendWhereIdentifier(x);
}

PEGASUS_NAMESPACE_END
