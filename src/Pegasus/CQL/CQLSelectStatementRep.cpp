#include "CQLSelectStatement.h"
#include "CQLSelectStatementRep.h"

PEGASUS_NAMESPACE_BEGIN

CQLSelectStatementRep::CQLSelectStatementRep(String inQlang, String inQuery, QueryContext& inCtx):
SelectStatementRep(inQlang, inQuery, inCtx)
{
}

Boolean CQLSelectStatementRep::evaluate(CIMInstance const inCI)
{
   return false;
}

Array<CIMInstance> CQLSelectStatementRep::executeQuery(Array<CIMInstance> inCIMInstanceArray) throw(Exception)
{
   Array<CIMInstance> arr;
   return arr;
}

CIMInstance CQLSelectStatementRep::applyProjection(CIMInstance inCI) throw(Exception)
{
   CIMInstance arr;
   return arr;
}

void CQLSelectStatementRep::validateClass(const CIMObjectPath& inClassName) throw(Exception)
{
}

void CQLSelectStatementRep::validateProperties() throw(Exception)
{
}

Array<CIMObjectPath> const CQLSelectStatementRep::getClassPathList()
{
   Array<CIMObjectPath> arr;
   return arr;
}

CIMPropertyList CQLSelectStatementRep::getPropertyList(const CIMObjectPath& inClassName)
{
   CIMPropertyList arr;
   return arr;
}

void CQLSelectStatementRep::appendClassPath(const CQLIdentifier& inIdentifier)
{
	_ctx->insertClassPath(inIdentifier);
}

void CQLSelectStatementRep::setPredicate(CQLPredicate inPredicate)
{
	_predicate = inPredicate;
}

void CQLSelectStatementRep::insertClassPathAlias(const CQLIdentifier& inIdentifier, String inAlias)
{
	_ctx->insertClassPath(inIdentifier,inAlias);
}

void CQLSelectStatementRep::appendSelectIdentifier(const CQLChainedIdentifier& x)
{
}

Boolean CQLSelectStatementRep::appendWhereIdentifier(const CQLChainedIdentifier& x)
{
   return false;
}

PEGASUS_NAMESPACE_END
