#include "SelectStatement.h"

PEGASUS_NAMESPACE_BEGIN

//##ModelId=40D99D1B01F5
SelectStatement::SelectStatement(String inQlang, String inQuery, QueryContext& inCtx)
{
}

//##ModelId=40D99DF40218
String SelectStatement::getQueryLanguage()
{
   return String();
}

//##ModelId=40D99E010217
String SelectStatement::getQuery()
{
   return String();
}

//##ModelId=40D99E0E01ED
Boolean SelectStatement::evaluate(const CIMInstance inCI)
{
   return false;
}

//##ModelId=40D9CE630083
Array<CIMInstance> SelectStatement::executeQuery(Array<CIMInstance> inCIMInstanceArray) throw(Exception)
{
   Array<CIMInstance> arr;
   return arr;
}

//##ModelId=40D9CE7100C9
CIMInstance SelectStatement::applyProjection(CIMInstance inCI) throw(Exception)
{
   return CIMInstance();
}

//##ModelId=4105138C03BA
void SelectStatement::validateClass(const CIMObjectPath& inClassName) throw(Exception)
{
}

//##ModelId=40D9CE7C02F5
void SelectStatement::validateProperties() throw(Exception)
{
}

//##ModelId=40F433600316
const Array<CIMObjectPath> SelectStatement::getClassPathList()
{
   Array<CIMObjectPath> arr;
   return arr;
}

//##ModelId=410511A70180
CIMPropertyList SelectStatement::getPropertyList(const CIMObjectPath& inClassName)
{
}

PEGASUS_NAMESPACE_END
