#include "SelectStatement.h"
#include "SelectStatementRep.h"

PEGASUS_NAMESPACE_BEGIN

SelectStatementRep::SelectStatementRep(String inQlang, String inQuery, QueryContext& inCtx)
{
	_qlang = inQlang;
	_query = inQuery;
	_ctx = &inCtx;
}

String SelectStatementRep::getQueryLanguage()
{
   return _qlang;
}

String SelectStatementRep::getQuery()
{
   return _query;
}

Boolean SelectStatementRep::evaluate(const CIMInstance inCI)
{
   return false;
}

Array<CIMInstance> SelectStatementRep::executeQuery(Array<CIMInstance> inCIMInstanceArray) throw(Exception)
{
   Array<CIMInstance> arr;
   return arr;
}

CIMInstance SelectStatementRep::applyProjection(CIMInstance inCI) throw(Exception)
{
   return CIMInstance();
}

void SelectStatementRep::validateClass(const CIMObjectPath& inClassName) throw(Exception)
{
}

void SelectStatementRep::validateProperties() throw(Exception)
{
}

const Array<CIMObjectPath> SelectStatementRep::getClassPathList()
{
   Array<CIMObjectPath> arr;
   return arr;
}

CIMPropertyList SelectStatementRep::getPropertyList(const CIMObjectPath& inClassName)
{
	return CIMPropertyList();
}

PEGASUS_NAMESPACE_END
