#include <Pegasus/CQL/CQLFunction.h>
#include <Pegasus/CQL/CQLFunctionRep.h>
//#include <Pegasus/CQL/CQLFunction.h>
//#include <Pegasus/CQL/CQLExpression.h>
//#include <Pegasus/CQL/CQLPredicate.h>
#include <Pegasus/CQL/CQLFactory.h>
PEGASUS_NAMESPACE_BEGIN
/*
CQLFunctionRep::CQLFunctionRep(FunctionOpType inFunctionOpType, Array<CQLExpression> inParms)
{
   _funcOpType = inFunctionOpType;
//   _parms = inParms;
} */
CQLFunctionRep::CQLFunctionRep(CQLIdentifier inOpType, Array<CQLPredicate> inParms){
    	_parms = inParms;
	String opType(inOpType.getName().getString());
/*
	if(opType.compareNoCase("classname") == 0)
	else if(opType.compareNoCase("classname") == 0)
	else if(opType.compareNoCase("") == 0)
	else if(opType.compareNoCase("classname") == 0)
        else if(opType.compareNoCase("classname") == 0)
	else if(opType.compareNoCase("classname") == 0)
        else if(opType.compareNoCase("classname") == 0)
        else if(opType.compareNoCase("classname") == 0)
        else if(opType.compareNoCase("classname") == 0)
	else if(opType.compareNoCase("classname") == 0)
        else if(opType.compareNoCase("classname") == 0)
        else if(opType.compareNoCase("classname") == 0)
*/
}
CQLFunctionRep::CQLFunctionRep(const CQLFunctionRep* rep)
{
   _funcOpType = rep->_funcOpType;
   _parms = rep->_parms; 
}

CQLFunctionRep::~CQLFunctionRep(){

}

CQLValue CQLFunctionRep::resolveValue(CIMInstance CI, QueryContext& queryCtx)
{
   switch(_funcOpType)
   {
      case CLASSNAMEEXP:
      case CLASSNAME:
      case CLASSPATH:
      case COUNT:
      case COUNTDISTINCT:
      case COUNTDISTINCTEXPR:
      case CREATEARRAY:
      case DATETIME:
      case HOSTNAME:
      case MAX:
      case MEAN:
      case MEDIAN:
      case MIN:
      case MODELPATH:
      case NAMESPACENAME:
      case NAMESPACEPATH:
      case OBJECTPATH:
      case SCHEME:
      case SUM:
      case USERINFO:
      case UPPERCASE:
      default:
         break;
   }
   return CQLValue(Uint64(0));
}

String CQLFunctionRep::toString()
{
	printf("CQLFunctionRep::toString()\n");
   String returnStr;

   char buffer[32];  // Should need 21 chars max
   sprintf(buffer, "%u", _funcOpType);
         
   returnStr.append(String(buffer));
   for(Uint32 i = 0; i < _parms.size(); ++i)
   {
      returnStr.append(_parms[i].toString());
   }
   return returnStr;
}


Array<CQLPredicate> CQLFunctionRep::getParms()
{
   return _parms;
}

FunctionOpType CQLFunctionRep::getFunctionType()
{
   return _funcOpType;
}
 
void CQLFunctionRep::applyContext(QueryContext& inContext)
{
   for(Uint32 i = 0; i < _parms.size(); ++i)
   {
     _parms[i].applyContext(inContext);
   }
}

Boolean CQLFunctionRep::operator==(const CQLFunctionRep& func){
	return true;
}
Boolean CQLFunctionRep::operator!=(const CQLFunctionRep& func){
	return (!operator==(func));
}
PEGASUS_NAMESPACE_END
