#include <Pegasus/CQL/CQLFunction.h>
#include <Pegasus/CQL/CQLExpression.h>
#include <Pegasus/CQL/CQLFactory.h>
PEGASUS_NAMESPACE_BEGIN

//##ModelId=40FD653E0390
CQLFunction::CQLFunction(FunctionOpType inFunctionOpType, Array<CQLExpression> inParms)
{
   _funcOpType = inFunctionOpType;
   _parms = inParms;
} 

CQLFunction::CQLFunction(const CQLFunction& inFunc)
{
   _funcOpType = inFunc._funcOpType;
   _parms = inFunc._parms; 
}

//##ModelId=40FC3BEA01F9
CQLValue CQLFunction::resolveValue(CIMInstance CI, QueryContext& queryCtx)
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

String CQLFunction::toString()
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


Array<CQLExpression> CQLFunction::getParms()
{
   return _parms;
}

FunctionOpType CQLFunction::getFunctionType()
{
   return _funcOpType;
}
 
void CQLFunction::applyScopes(Array<CQLScope> inScopes)
{
   for(Uint32 i = 0; i < _parms.size(); ++i)
   {
      _parms[i].applyScopes(inScopes);
   }
}

Boolean CQLFunction::operator==(const CQLFunction& func){
	return true;
}
Boolean CQLFunction::operator!=(const CQLFunction& func){
	return (!operator==(func));
}
PEGASUS_NAMESPACE_END
