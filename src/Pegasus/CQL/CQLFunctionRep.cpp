#include <Pegasus/CQL/CQLFunction.h>
#include <Pegasus/CQL/CQLFunctionRep.h>
//#include <Pegasus/CQL/CQLFunction.h>
#include <Pegasus/CQL/CQLExpression.h>
#include <Pegasus/CQL/CQLFactory.h>
PEGASUS_NAMESPACE_BEGIN

CQLFunctionRep::CQLFunctionRep(FunctionOpType inFunctionOpType, Array<CQLExpression> inParms)
{
   _funcOpType = inFunctionOpType;
   _parms = inParms;
} 

CQLFunctionRep::CQLFunctionRep(const CQLFunctionRep& inFunc)
{
   _funcOpType = inFunc._funcOpType;
   _parms = inFunc._parms; 
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


Array<CQLExpression> CQLFunctionRep::getParms()
{
   return _parms;
}

FunctionOpType CQLFunctionRep::getFunctionType()
{
   return _funcOpType;
}
 
void CQLFunctionRep::applyScopes(Array<CQLScope> inScopes)
{
   for(Uint32 i = 0; i < _parms.size(); ++i)
   {
      _parms[i].applyScopes(inScopes);
   }
}

Boolean CQLFunctionRep::operator==(const CQLFunctionRep& func){
	return true;
}
Boolean CQLFunctionRep::operator!=(const CQLFunctionRep& func){
	return (!operator==(func));
}
PEGASUS_NAMESPACE_END
