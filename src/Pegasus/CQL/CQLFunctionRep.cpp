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

	if(String::compareNoCase(opType,String("classnameexp")) == 0){
		 _funcOpType = CLASSNAMEEXP;
	}
	else if(String::compareNoCase(opType, String("classname")) == 0){
                 _funcOpType = CLASSNAME;
        }
	else if(String::compareNoCase(opType, String("classpath")) == 0){
                 _funcOpType = CLASSPATH;
        }
	else if(String::compareNoCase(opType, String("count")) == 0){
                 _funcOpType = COUNT;
        }
        else if(String::compareNoCase(opType, String("countdistinct")) == 0){
                 _funcOpType = COUNTDISTINCT;
        }
	else if(String::compareNoCase(opType, String("countdistinctexpr")) == 0){
                 _funcOpType = COUNTDISTINCTEXPR;
        }
        else if(String::compareNoCase(opType, String("createarray")) == 0){
                 _funcOpType = CREATEARRAY;
        }
        else if(String::compareNoCase(opType, String("datetime")) == 0){
                 _funcOpType = DATETIME;
        }
        else if(String::compareNoCase(opType, String("hostname")) == 0){
                 _funcOpType = HOSTNAME;
        }
	else if(String::compareNoCase(opType, String("max")) == 0){
                 _funcOpType = MAX;
        }
        else if(String::compareNoCase(opType, String("mean")) == 0){
                 _funcOpType = MEAN;
        }
        else if(String::compareNoCase(opType, String("median")) == 0){
                 _funcOpType = MEDIAN;
        }
	else if(String::compareNoCase(opType, String("min")) == 0){
                 _funcOpType = MIN;
        }
        else if(String::compareNoCase(opType, String("modelpath")) == 0){
                 _funcOpType = MODELPATH;
        }
        else if(String::compareNoCase(opType, String("namespacename")) == 0){
                 _funcOpType = NAMESPACENAME;
        }
	else if(String::compareNoCase(opType, String("namespacepath")) == 0){
                 _funcOpType = NAMESPACEPATH;
        }
        else if(String::compareNoCase(opType, String("objectpath")) == 0){
                 _funcOpType = OBJECTPATH;
        }
        else if(String::compareNoCase(opType, String("scheme")) == 0){
                 _funcOpType = SCHEME;
        }
	else if(String::compareNoCase(opType, String("sum")) == 0){
                 _funcOpType = SUM;
        }
        else if(String::compareNoCase(opType, String("userinfo")) == 0){
                 _funcOpType = USERINFO;
        }
        else if(String::compareNoCase(opType, String("uppercase")) == 0){
                 _funcOpType = UPPERCASE;
        }
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

   //char buffer[32];  // Should need 21 chars max
   //sprintf(buffer, "%u", _funcOpType);
         
   //returnStr.append(String(buffer));
   switch(_funcOpType)
   {
      case CLASSNAMEEXP:
		returnStr.append("CLASSNAMEEXP");
		break;
      case CLASSNAME:
		returnStr.append("CLASSNAME");
                break;
      case CLASSPATH:
		returnStr.append("CLASSPATH");
                break;
      case COUNT:
		returnStr.append("COUNT");
                break;
      case COUNTDISTINCT:
		returnStr.append("COUNTDISTINCT");
                break;
      case COUNTDISTINCTEXPR:
		returnStr.append("COUNTDISTINCTEXPR");
                break;
      case CREATEARRAY:
		returnStr.append("CREATEARRAY");
                break;
      case DATETIME:
		returnStr.append("DATETIME");
                break;
      case HOSTNAME:
		returnStr.append("HOSTNAME");
                break;
      case MAX:
		returnStr.append("MAX");
                break;
      case MEAN:
		returnStr.append("MEAN");
                break;
      case MEDIAN:
		returnStr.append("MEDIAN");
                break;
      case MIN:
		returnStr.append("MIN");
                break;
      case MODELPATH:
		returnStr.append("MODELPATH");
                break;
      case NAMESPACENAME:
		returnStr.append("NAMESPACENAME");
                break;
      case NAMESPACEPATH:
		returnStr.append("NAMESPACEPATH");
                break;
      case OBJECTPATH:
		returnStr.append("OBJECTPATH");
                break;
      case SCHEME:
		returnStr.append("SCHEME");
                break;
      case SUM:
		returnStr.append("SUM");
                break;
      case USERINFO:
		returnStr.append("USERINFO");
                break;
      case UPPERCASE:
		returnStr.append("UPPERCASE");
                break;
      default:
		break;
   }

	returnStr.append("(");
   for(Uint32 i = 0; i < _parms.size(); ++i)
   {
      returnStr.append(_parms[i].toString());
   }
	returnStr.append(")");
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
