#include "CQLSelectStatement.h"

PEGASUS_NAMESPACE_BEGIN

//##ModelId=40D9CF6C0307
CQLSelectStatement::CQLSelectStatement(String inQlang, String inQuery, QueryContext& inCtx):
SelectStatement(inQlang, inQuery, inCtx)
{
}

//##ModelId=40D9CF4B01C0
Boolean CQLSelectStatement::evaluate(CIMInstance const inCI)
{
   return false;
}

//##ModelId=40D9CF5000C3
Array<CIMInstance> CQLSelectStatement::executeQuery(Array<CIMInstance> inCIMInstanceArray) throw(Exception)
{
   Array<CIMInstance> arr;
   return arr;
}

//##ModelId=40D9CF5403CC
CIMInstance CQLSelectStatement::applyProjection(CIMInstance inCI) throw(Exception)
{
   CIMInstance arr;
   return arr;
}

//##ModelId=410513ED03D0
void CQLSelectStatement::validateClass(const CIMObjectPath& inClassName) throw(Exception)
{
}

//##ModelId=40D9CF5E0054
void CQLSelectStatement::validateProperties() throw(Exception)
{
}

//##ModelId=40F43FE503C2
Array<CIMObjectPath> const CQLSelectStatement::getClassPathList()
{
   Array<CIMObjectPath> arr;
   return arr;
}

//##ModelId=410512AC0316
CIMPropertyList CQLSelectStatement::getPropertyList(const CIMObjectPath& inClassName)
{
   CIMPropertyList arr;
   return arr;
}

//##ModelId=40E01E7B01EE
void CQLSelectStatement::appendClassPath(const CQLIdentifier& inIdentifier)
{
}

//##ModelId=40E0398E0346
void CQLSelectStatement::setPredicate(CQLPredicate inPredicate)
{
}

//##ModelId=40E03D3A03CC
void CQLSelectStatement::insertClassPathAlias(const CQLIdentifier& inIdentifier, String inAlias)
{
}

//##ModelId=40F428D000A4
void CQLSelectStatement::appendSelectIdentifier(const CQLChainedIdentifier& x)
{
}

//##ModelId=40F42AED028E
Boolean CQLSelectStatement::appendWhereIdentifier(const CQLChainedIdentifier& x)
{
   return false;
}

PEGASUS_NAMESPACE_END
