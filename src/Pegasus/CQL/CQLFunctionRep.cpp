//%2003////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001, 2002  BMC Software, Hewlett-Packard Development
// Company, L. P., IBM Corp., The Open Group, Tivoli Systems.
// Copyright (c) 2003 BMC Software; Hewlett-Packard Development Company, L. P.;
// IBM Corp.; EMC Corporation, The Open Group.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// THE ABOVE COPYRIGHT NOTICE AND THIS PERMISSION NOTICE SHALL BE INCLUDED IN
// ALL COPIES OR SUBSTANTIAL PORTIONS OF THE SOFTWARE. THE SOFTWARE IS PROVIDED
// "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT
// LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
// PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
// HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
// ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
// WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
//
//==============================================================================
//
// Authors: David Rosckes (rosckes@us.ibm.com)
//          Bert Rivero (hurivero@us.ibm.com)
//          Chuck Carmack (carmack@us.ibm.com)
//          Brian Lucier (lucier@us.ibm.com)
//
// Modified By: 
//
//%/////////////////////////////////////////////////////////////////////////////

#include <Pegasus/CQL/CQLFunction.h>
#include <Pegasus/CQL/CQLFunctionRep.h>
//#include <Pegasus/CQL/CQLFunction.h>
//#include <Pegasus/CQL/CQLExpression.h>
//#include <Pegasus/CQL/CQLPredicate.h>
#include <Pegasus/CQL/CQLFactory.h>
#include <Pegasus/Common/XmlReader.h>
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
        
	if(String::compareNoCase(opType,String("DATETIMETOMICROSECOND")) == 0){
		 _funcOpType = DATETIMETOMICROSECOND;
	}
	else if(String::compareNoCase(opType, String("STRINGTOUINT")) == 0){
                 _funcOpType = STRINGTOUINT;
        }
	else if(String::compareNoCase(opType, String("STRINGTOSINT")) == 0){
                 _funcOpType = STRINGTOSINT;
        }
	else if(String::compareNoCase(opType, String("STRINGTOREAL")) == 0){
                 _funcOpType = STRINGTOREAL;
        }
        else if(String::compareNoCase(opType, String("STRINGTONUMERIC")) == 0){
                 _funcOpType = STRINGTONUMERIC;
        }
	else if(String::compareNoCase(opType, String("UPPERCASE")) == 0){
                 _funcOpType = UPPERCASE;
        }
        else if(String::compareNoCase(opType, String("NUMERICTOSTRING")) == 0){
                 _funcOpType = NUMERICTOSTRING;
        }
        else if(String::compareNoCase(opType, String("REFERENCETOSTRING")) == 0){
                 _funcOpType = REFERENCETOSTRING;
        }
        else if(String::compareNoCase(opType, String("CLASSNAME")) == 0){
                 _funcOpType = CLASSNAME;
        }
	else if(String::compareNoCase(opType, String("NAMESPACENAME")) == 0){
                 _funcOpType = NAMESPACENAME;
        }
        else if(String::compareNoCase(opType, String("NAMESPACETYPE")) == 0){
                 _funcOpType = NAMESPACETYPE;
        }
        else if(String::compareNoCase(opType, String("HOSTPORT")) == 0){
                 _funcOpType = HOSTPORT;
        }
	else if(String::compareNoCase(opType, String("MODELPATH")) == 0){
                 _funcOpType = MODELPATH;
        }
        else if(String::compareNoCase(opType, String("CLASSPATH")) == 0){
                 _funcOpType = CLASSPATH;
        }
        else if(String::compareNoCase(opType, String("OBJECTPATH")) == 0){
                 _funcOpType = OBJECTPATH;
        }
	else if(String::compareNoCase(opType, String("INSTANCETOREFERENCE")) == 0){
                 _funcOpType = INSTANCETOREFERENCE;
        }
        else if(String::compareNoCase(opType, String("CURRENTDATETIME")) == 0){
                 _funcOpType = CURRENTDATETIME;
        }
        else if(String::compareNoCase(opType, String("DATETIME")) == 0){
                 _funcOpType = DATETIME;
        }
	else if(String::compareNoCase(opType, String("MICROSECONDTOTIMESTAMP")) == 0){
                 _funcOpType = MICROSECONDTOTIMESTAMP;
        }
        else if(String::compareNoCase(opType, String("MICROSECONDTOINTERVAL")) == 0){
                 _funcOpType = MICROSECONDTOINTERVAL;
        }
}
CQLFunctionRep::CQLFunctionRep(const CQLFunctionRep* rep)
{
   _funcOpType = rep->_funcOpType;
   _parms = rep->_parms; 
}

CQLFunctionRep::~CQLFunctionRep(){
}

CQLValue CQLFunctionRep::resolveValue(const CIMInstance& CI,const QueryContext& queryCtx)
{
   switch(_funcOpType)
   {
       case DATETIMETOMICROSECOND:
           return dateTimeToMicrosecond(CI, queryCtx);
       case STRINGTOUINT:
           return stringToUint(CI, queryCtx);
       case STRINGTOSINT:
           return stringToSint(CI, queryCtx);
       case STRINGTOREAL:
           return stringToReal(CI, queryCtx);
       case STRINGTONUMERIC:
           return stringToNumeric(CI, queryCtx);
       case UPPERCASE:
           return upperCase(CI, queryCtx);
       case NUMERICTOSTRING:
           return numericToString(CI, queryCtx);
       case REFERENCETOSTRING:
           return referenceToString(CI, queryCtx);
       case CLASSNAME:
           return className(CI, queryCtx);
       case NAMESPACENAME:
           return nameSpaceName(CI, queryCtx);
       case NAMESPACETYPE:
           return nameSpaceType(CI, queryCtx);
       case HOSTPORT:
           return hostPort(CI, queryCtx);
       case MODELPATH:
           return modelPath(CI, queryCtx);
       case CLASSPATH:
           return classPath(CI, queryCtx);
       case OBJECTPATH:
           return objectPath(CI, queryCtx);
       case INSTANCETOREFERENCE:
           return instanceToReference(CI, queryCtx);
       case CURRENTDATETIME:
           return currentDateTime(CI, queryCtx);
       case DATETIME:
           return dateTime(CI, queryCtx);
       case MICROSECONDTOTIMESTAMP:
           return microsecondToTimestamp(CI, queryCtx);
       case MICROSECONDTOINTERVAL:
           return microsecondToInterval(CI, queryCtx);

       default:
           // TODO: throw exception
           break;
   }
   return CQLValue(Uint64(0));
}

String CQLFunctionRep::toString()const
{
	printf("CQLFunctionRep::toString()\n");
   String returnStr;

   switch(_funcOpType)
   {
      case DATETIMETOMICROSECOND:
		returnStr.append("DATETIMETOMICROSECOND");
		break;
      case STRINGTOUINT:
		returnStr.append("STRINGTOUINT");
                break;
      case STRINGTOSINT:
		returnStr.append("STRINGTOSINT");
                break;
      case STRINGTOREAL:
		returnStr.append("STRINGTOREAL");
                break;
      case UPPERCASE:
		returnStr.append("UPPERCASE");
                break;
      case NUMERICTOSTRING:
		returnStr.append("NUMERICTOSTRING");
                break;
      case REFERENCETOSTRING:
		returnStr.append("REFERENCETOSTRING");
                break;
      case CLASSNAME:
		returnStr.append("CLASSNAME");
                break;
      case NAMESPACENAME:
		returnStr.append("NAMESPACENAME");
                break;
      case NAMESPACETYPE:
		returnStr.append("NAMESPACETYPE");
                break;
      case HOSTPORT:
		returnStr.append("HOSTPORT");
                break;
      case MODELPATH:
		returnStr.append("MODELPATH");
                break;
      case CLASSPATH:
		returnStr.append("CLASSPATH");
                break;
      case OBJECTPATH:
		returnStr.append("OBJECTPATH");
                break;
      case INSTANCETOREFERENCE:
		returnStr.append("INSTANCETOREFERENCE");
                break;
      case CURRENTDATETIME:
		returnStr.append("CURRENTDATETIME");
                break;
      case DATETIME:
		returnStr.append("DATETIME");
                break;
      case MICROSECONDTOTIMESTAMP:
		returnStr.append("MICROSECONDTOTIMESTAMP");
                break;
      case MICROSECONDTOINTERVAL:
		returnStr.append("MICROSECONDTOINTERVAL");
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


Array<CQLPredicate> CQLFunctionRep::getParms()const
{
   return _parms;
}

FunctionOpType CQLFunctionRep::getFunctionType()const
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

Boolean CQLFunctionRep::operator==(const CQLFunctionRep& func)const
{
  if(_funcOpType != func._funcOpType || _parms.size() != func._parms.size())
    {
      return false;
    }

  // If we need to ensure that all predicates are the same
  // for this operator then the CQLPredicate class will need
  // to support operator==.
  //for(Uint32 i = 0; i < _parms.size(); ++i)
  //  {
  //    if(!(_parms[i] == func._parms[i]))
  //	{
  //	  return false;
  //	}
  //  }

  return true;
}
Boolean CQLFunctionRep::operator!=(const CQLFunctionRep& func)const{
	return (!operator==(func));
}

CQLValue CQLFunctionRep::dateTimeToMicrosecond(const CIMInstance& CI, const QueryContext& queryCtx)
{
   return CQLValue(Uint64(0));
}

CQLValue CQLFunctionRep::stringToUint(const CIMInstance& CI, const QueryContext& queryCtx)
{
  if(_parms.size() != 1)
    {
      throw(Exception(String("CQLFunctionRep::stringToUint -- too many predicates")));
    }

  CQLValue cqlVal = _parms[0].getSimplePredicate().getLeftExpression().resolveValue(CI,queryCtx);

  if(cqlVal.getValueType() != CQLValue::String_type)
    {
      throw(Exception(String("CQLFunctionRep::stringToUint -- not a String")));
    }

  Uint64 x;

  XmlReader::stringToUnsignedInteger(cqlVal.getString().getCString(),x);
    
  return CQLValue(x);
}

CQLValue CQLFunctionRep::stringToSint(const CIMInstance& CI, const QueryContext& queryCtx)
{
  if(_parms.size() != 1)
    {
      throw(Exception(String("CQLFunctionRep::stringToUint -- too many predicates")));
    }

  CQLValue cqlVal = _parms[0].getSimplePredicate().getLeftExpression().resolveValue(CI,queryCtx);

  if(cqlVal.getValueType() != CQLValue::String_type)
    {
      throw(Exception(String("CQLFunctionRep::stringToUint -- not a String")));
    }

  Sint64 x;

  XmlReader::stringToSignedInteger(cqlVal.getString().getCString(),x);
    
  return CQLValue(x);
}

CQLValue CQLFunctionRep::stringToReal(const CIMInstance& CI, const QueryContext& queryCtx)
{
  if(_parms.size() != 1)
    {
      throw(Exception(String("CQLFunctionRep::stringToUint -- too many predicates")));
    }

  CQLValue cqlVal = _parms[0].getSimplePredicate().getLeftExpression().resolveValue(CI,queryCtx);

  if(cqlVal.getValueType() != CQLValue::String_type)
    {
      throw(Exception(String("CQLFunctionRep::stringToUint -- not a String")));
    }

  Real64 x;

  XmlReader::stringToReal(cqlVal.getString().getCString(),x);
    
   return CQLValue(x);
}

CQLValue CQLFunctionRep::stringToNumeric(const CIMInstance& CI, const QueryContext& queryCtx)
{
   return stringToReal(CI, queryCtx);
}

CQLValue CQLFunctionRep::upperCase(const CIMInstance& CI, const QueryContext& queryCtx)
{
  if(_parms.size() > 1)
    {
      throw(Exception(String("CQLFunctionRep::upperCase -- too many predicates")));
    }

  CQLValue cqlVal = _parms[0].getSimplePredicate().getLeftExpression().resolveValue(CI,queryCtx);

  if(cqlVal.getValueType() != CQLValue::String_type)
    {
      throw(Exception(String("CQLFunctionRep::upperCase -- not a String")));
    }

  String tmpStr = cqlVal.getString();
  tmpStr.toUpper();

  return CQLValue(tmpStr);
}

CQLValue CQLFunctionRep::numericToString(const CIMInstance& CI, const QueryContext& queryCtx)
{
   return CQLValue(Uint64(0));
}

CQLValue CQLFunctionRep::referenceToString(const CIMInstance& CI, const QueryContext& queryCtx)
{
   return CQLValue(Uint64(0));
}

CQLValue CQLFunctionRep::className(const CIMInstance& CI, const QueryContext& queryCtx)
{
   return CQLValue(Uint64(0));
}

CQLValue CQLFunctionRep::nameSpaceName(const CIMInstance& CI, const QueryContext& queryCtx)
{
   return CQLValue(Uint64(0));
}

CQLValue CQLFunctionRep::nameSpaceType(const CIMInstance& CI, const QueryContext& queryCtx)
{
   return CQLValue(Uint64(0));
}

CQLValue CQLFunctionRep::hostPort(const CIMInstance& CI, const QueryContext& queryCtx)
{
   return CQLValue(Uint64(0));
}

CQLValue CQLFunctionRep::modelPath(const CIMInstance& CI, const QueryContext& queryCtx)
{
   return CQLValue(Uint64(0));
}

CQLValue CQLFunctionRep::classPath(const CIMInstance& CI, const QueryContext& queryCtx)
{
   return CQLValue(Uint64(0));
}

CQLValue CQLFunctionRep::objectPath(const CIMInstance& CI, const QueryContext& queryCtx)
{
   return CQLValue(Uint64(0));
}

CQLValue CQLFunctionRep::instanceToReference(const CIMInstance& CI, const QueryContext& queryCtx)
{
   return CQLValue(Uint64(0));
}

CQLValue CQLFunctionRep::currentDateTime(const CIMInstance& CI, const QueryContext& queryCtx)
{
   return CQLValue(Uint64(0));
}

CQLValue CQLFunctionRep::dateTime(const CIMInstance& CI, const QueryContext& queryCtx)
{
   return CQLValue(Uint64(0));
}

CQLValue CQLFunctionRep::microsecondToTimestamp(const CIMInstance& CI, const QueryContext& queryCtx)
{
   return CQLValue(Uint64(0));
}

CQLValue CQLFunctionRep::microsecondToInterval(const CIMInstance& CI, const QueryContext& queryCtx)
{
   return CQLValue(Uint64(0));
}

PEGASUS_NAMESPACE_END
