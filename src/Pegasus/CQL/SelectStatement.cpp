#include "SelectStatement.h"

PEGASUS_NAMESPACE_BEGIN

SelectStatement::SelectStatement(String inQlang, String inQuery, QueryContext& inCtx)
{
	_qlang = inQlang;
	_query = inQuery;
	_ctx = &inCtx;
}

String SelectStatement::getQueryLanguage()
{
   return _qlang;
}

String SelectStatement::getQuery()
{
   return _query;
}

Boolean SelectStatement::evaluate(const CIMInstance inCI)
{
   return false;
}

Array<CIMInstance> SelectStatement::executeQuery(Array<CIMInstance> inCIMInstanceArray) throw(Exception)
{
   Array<CIMInstance> arr;
   return arr;
}

CIMInstance SelectStatement::applyProjection(CIMInstance inCI) throw(Exception)
{
   return CIMInstance();
}

void SelectStatement::validateClass(const CIMObjectPath& inClassName) throw(Exception)
{
}

void SelectStatement::validateProperties() throw(Exception)
{
}

const Array<CIMObjectPath> SelectStatement::getClassPathList()
{
   Array<CIMObjectPath> arr;
   return arr;
}

CIMPropertyList SelectStatement::getPropertyList(const CIMObjectPath& inClassName)
{
}

PEGASUS_NAMESPACE_END
