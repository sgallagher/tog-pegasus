#include "CQLSelectStatement.h"
#include "CQLSelectStatementRep.h"

PEGASUS_NAMESPACE_BEGIN

CQLSelectStatementRep::CQLSelectStatementRep(String inQlang, String inQuery, QueryContext& inCtx):
SelectStatementRep(inQlang, inQuery, inCtx)
{
}

CQLSelectStatementRep::CQLSelectStatementRep(const CQLSelectStatementRep* rep){
	_whereIdentifiers = rep->_whereIdentifiers;
	_selectIdentifiers = rep->_selectIdentifiers;
	_predicate = rep->_predicate;
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
	for(Uint32 i = 0; i < x.getSubIdentifiers().size(); i++){
		_selectIdentifiers.append( x.getSubIdentifiers()[i]);
	}
}

Boolean CQLSelectStatementRep::appendWhereIdentifier(const CQLChainedIdentifier& x)
{
	for(Uint32 i = 0; i < x.getSubIdentifiers().size(); i++){
                _whereIdentifiers.append(x.getSubIdentifiers()[i]);
        }
   	return true;
}

PEGASUS_NAMESPACE_END
