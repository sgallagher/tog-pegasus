#include "CQLSelectStatement.h"

PEGASUS_NAMESPACE_BEGIN

CQLSelectStatement::CQLSelectStatement(String inQlang, String inQuery, QueryContext& inCtx):
SelectStatement(inQlang, inQuery, inCtx)
{
}

Boolean CQLSelectStatement::evaluate(CIMInstance const inCI)
{
   return false;
}

Array<CIMInstance> CQLSelectStatement::executeQuery(Array<CIMInstance> inCIMInstanceArray) throw(Exception)
{
   Array<CIMInstance> arr;
   return arr;
}

CIMInstance CQLSelectStatement::applyProjection(CIMInstance inCI) throw(Exception)
{
   CIMInstance arr;
   return arr;
}

void CQLSelectStatement::validateClass(const CIMObjectPath& inClassName) throw(Exception)
{
}

void CQLSelectStatement::validateProperties() throw(Exception)
{
}

Array<CIMObjectPath> const CQLSelectStatement::getClassPathList()
{
   Array<CIMObjectPath> arr;
   return arr;
}

CIMPropertyList CQLSelectStatement::getPropertyList(const CIMObjectPath& inClassName)
{
   CIMPropertyList arr;
   return arr;
}

void CQLSelectStatement::appendClassPath(const CQLIdentifier& inIdentifier)
{
}

void CQLSelectStatement::setPredicate(CQLPredicate inPredicate)
{
}

void CQLSelectStatement::insertClassPathAlias(const CQLIdentifier& inIdentifier, String inAlias)
{
}

void CQLSelectStatement::appendSelectIdentifier(const CQLChainedIdentifier& x)
{
}

Boolean CQLSelectStatement::appendWhereIdentifier(const CQLChainedIdentifier& x)
{
   return false;
}

PEGASUS_NAMESPACE_END
