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

#include <Pegasus/CQL/CQLFunctionRep.h>

// CQL includes
#include <Pegasus/CQL/CQLFactory.h>
#include <Pegasus/CQL/CQLUtilities.h>
#include <Pegasus/CQL/CQLValue.h>

// Common Query includes
#include <Pegasus/Query/QueryCommon/QueryContext.h>

// Common Pegasus includes
#include <Pegasus/Common/CIMName.h>
#include <Pegasus/Common/CIMObjectPath.h>

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
           return currentDateTime();
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

CQLValue CQLFunctionRep::dateTimeToMicrosecond(const CIMInstance& CI, const QueryContext& queryCtx) const
{
  if(_parms.size() != 1)
  {
    throw(Exception(String("CQLFunctionRep::dateTimeToMicrosecond -- incorrect number of parameters")));
  }

  CQLValue cqlVal = _parms[0].getSimplePredicate().getLeftExpression().resolveValue(CI,queryCtx);

  if(cqlVal.getValueType() != CQLValue::CIMDateTime_type)
  {
    throw(Exception(String("CQLFunctionRep::dateTimeToMicrosecond -- not a DateTime")));
  }

  // TODO:  When Wilis code is in, replace the current dummy return with the following.
  // return(cqlVal.getDateTime().toMicroseconds())
  
   return CQLValue(Uint64(0));
}

CQLValue CQLFunctionRep::stringToUint(const CIMInstance& CI, const QueryContext& queryCtx) const
{
  if(_parms.size() != 1)
    {
      throw(Exception(String("CQLFunctionRep::stringToUint -- incorrect number of parameters")));
    }

  CQLValue cqlVal = _parms[0].getSimplePredicate().getLeftExpression().resolveValue(CI,queryCtx);

  if(cqlVal.getValueType() != CQLValue::String_type)
    {
      throw(Exception(String("CQLFunctionRep::stringToUint -- not a String")));
    }

  return CQLValue(CQLUtilities::stringToUint64(cqlVal.getString()));
}

CQLValue CQLFunctionRep::stringToSint(const CIMInstance& CI, const QueryContext& queryCtx) const
{
  if(_parms.size() != 1)
    {
      throw(Exception(String("CQLFunctionRep::stringToSint -- incorrect number of parameters")));
    }

  CQLValue cqlVal = _parms[0].getSimplePredicate().getLeftExpression().resolveValue(CI,queryCtx);

  if(cqlVal.getValueType() != CQLValue::String_type)
    {
      throw(Exception(String("CQLFunctionRep::stringToSint -- not a String")));
    }

  return CQLValue(CQLUtilities::stringToSint64(cqlVal.getString()));
}

CQLValue CQLFunctionRep::stringToReal(const CIMInstance& CI, const QueryContext& queryCtx) const
{
  if(_parms.size() != 1)
    {
      throw(Exception(String("CQLFunctionRep::stringToReal -- too many predicates")));
    }

  CQLValue cqlVal = _parms[0].getSimplePredicate().getLeftExpression().resolveValue(CI,queryCtx);

  if(cqlVal.getValueType() != CQLValue::String_type)
    {
      throw(Exception(String("CQLFunctionRep::stringToReal -- not a String")));
  }
  
  return CQLValue(CQLUtilities::stringToReal64(cqlVal.getString()));
}

CQLValue CQLFunctionRep::stringToNumeric(const CIMInstance& CI, const QueryContext& queryCtx) const
{
   return stringToReal(CI, queryCtx);
}

CQLValue CQLFunctionRep::upperCase(const CIMInstance& CI, const QueryContext& queryCtx) const
{
  if(_parms.size() != 1)
    {
      throw(Exception(String("CQLFunctionRep::upperCase -- must have only one parameter")));
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

CQLValue CQLFunctionRep::numericToString(const CIMInstance& CI, const QueryContext& queryCtx) const
{
  char buffer[128];

  if(_parms.size() != 1)
    {
      throw(Exception(String("CQLFunctionRep::numericToString -- incorrect number of parameters")));
    }

  CQLValue cqlVal = _parms[0].getSimplePredicate().getLeftExpression().resolveValue(CI,queryCtx);
  CQLValue::CQLValueType valType = cqlVal.getValueType();  
  if (valType == CQLValue::Sint64_type)
  {
    sprintf(buffer, "%" PEGASUS_64BIT_CONVERSION_WIDTH "u", cqlVal.getSint());
  }
  else if (valType == CQLValue::Uint64_type)
  {
    sprintf(buffer, "%" PEGASUS_64BIT_CONVERSION_WIDTH "u", cqlVal.getUint());
  }
  else if (valType == CQLValue::Real_type)
    // %.16e gives '[-]m.dddddddddddddddde+/-xx', which seems compatible with the format
    // given in the CIM/XML spec, and the precision required by the CIM 2.2 spec
    // (8 byte IEEE floating point)
    sprintf(buffer, "%.16e", cqlVal.getReal());
  else
  {
    throw(Exception(String("CQLFunctionRep::numericToString -- not a number")));
  }
  
   return CQLValue(String(buffer));
}

CQLValue CQLFunctionRep::referenceToString(const CIMInstance& CI, const QueryContext& queryCtx) const
{
  if(_parms.size() != 1)
    {
      throw(Exception(String("CQLFunctionRep::numericToString -- incorrect number of parameters")));
    }

  CQLValue cqlVal = _parms[0].getSimplePredicate().getLeftExpression().resolveValue(CI,queryCtx);
  if (cqlVal.getValueType() != CQLValue::CIMReference_type)
  {
    throw(Exception(String("CQLFunctionRep::referenceToString -- parameter is not a CIM Reference")));
  }
  // references are currently an Object Path    
   return CQLValue(cqlVal.getReference().toString());
}

CQLValue CQLFunctionRep::className(const CIMInstance& CI, const QueryContext& queryCtx) const
{
  int parmSize = _parms.size();
  if(parmSize != 0 && parmSize != 1)
  {
    throw(Exception(String("CQLFunctionRep::className -- incorrect number of parameters")));
  }

  // If there are no parameters, the default behavior is to return the class name for the instance being examined (CI).
  if (parmSize == 0)
    return CQLValue(CI.getClassName().getString());

  // We have a parameter, so resolve it first before we use it.
  CQLValue cqlVal = _parms[0].getSimplePredicate().getLeftExpression().resolveValue(CI,queryCtx);

  // If we have a String parameter, then we'll use it to create a CIMObjectPath in order to verify the format is correct.  We will then get the class from the object path and return it.
  if (cqlVal.getValueType() == CQLValue::String_type)
  {
    CIMObjectPath objPath(cqlVal.getString());
    return CQLValue(objPath.getClassName().getString());
  }

  // If we have a CIMReference parameter, then we will just get the class name from the reference and return it.  A refernce is a CIMObjectPath.
  if (cqlVal.getValueType() == CQLValue::CIMReference_type)
    return CQLValue(cqlVal.getReference().getClassName().getString());

  // If we have a CIMObject, then we return the class name of the obejct
  if (cqlVal.getValueType() == CQLValue::CIMObject_type)
    return CQLValue(cqlVal.getObject().getClassName().getString());

  // If it makes it to this block of code, then no valid type was found, and hence no return was made.  Throw invalid parameter type exception.
    throw(Exception(String("CQLFunctionRep::className -- parameter type is invalid")));
}

CQLValue CQLFunctionRep::nameSpaceName(const CIMInstance& CI, const QueryContext& queryCtx) const
{
  int parmSize = _parms.size();
  if(parmSize != 0 && parmSize != 1)
  {
    throw(Exception(String("CQLFunctionRep::nameSpaceName -- incorrect number of parameters")));
  }

  // The default beavior for this function will first look at the object path of the instance being examined (CI).  If the path contains a namespace, we will return it.  If it does not, then we will return the default namespace from the query context.
  // *** NOTE ***  This does not function entirely according to the CQL spec.  The CQL spec says to return the namespace of the instance regardless if it is set or not.  However, with the current implementation (CQL phase 1 PEP 193) we only operate a query engine within a single namespace and so we can assume the default namespace.
  if (parmSize == 0)
  {
    CIMNamespaceName ns = CI.getPath().getNameSpace();
    if (ns.isNull() || String::equal(ns.getString(), String::EMPTY))
      ns = queryCtx.getNamespace();
    return CQLValue(ns.getString());    
  }

  // We have a parameter, so resolve it first before we use it.
  CQLValue cqlVal = _parms[0].getSimplePredicate().getLeftExpression().resolveValue(CI,queryCtx);

  // If we have a String parameter, then we'll use it to create a CIMObjectPath in order to verify the format is correct.  We will then get the namespace from the object path and return it.  If there is no namespace in the path given, then a blank string will be returned.
  if (cqlVal.getValueType() == CQLValue::String_type)
  {
    CIMObjectPath objPath(cqlVal.getString());
    return CQLValue(objPath.getNameSpace().getString());
  }

  // If we have a CIMReference parameter, then we will just get the namespace name from the reference and return it.  A refernce is a CIMObjectPath.  If there is no namespace in the path given, then a blank string will be returned.
  if (cqlVal.getValueType() == CQLValue::CIMReference_type)
    return CQLValue(cqlVal.getReference().getNameSpace().getString());

  // If we have a CIMObject, then we retrieve the path of the obejct and check to see if it has a namespace set in it.  If the path contains a namespace, we will return it.  If it does not, then we will return the default namespace from the query context.
  // *** NOTE ***  This does not function entirely according to the CQL spec.  The CQL spec says to return the namespace of the instance regardless if it is set or not.  However, with the current implementation (CQL phase 1 PEP 193) we only operate a query engine within a single namespace and so we can assume the default namespace. 
  if (cqlVal.getValueType() == CQLValue::CIMObject_type)
  {
    CIMNamespaceName ns = cqlVal.getObject().getPath().getNameSpace();
    if (ns.isNull() || String::equal(ns.getString(), String::EMPTY))
      ns = queryCtx.getNamespace();
    return CQLValue(ns.getString());
  }
  
  // If it makes it to this block of code, then no valid type was found, and hence no return was made.  Throw invalid parameter type exception.
    throw(Exception(String("CQLFunctionRep::nameSpaceName -- parameter type is invalid")));
}

CQLValue CQLFunctionRep::nameSpaceType(const CIMInstance& CI, const QueryContext& queryCtx) const
{
  // This is currently (as of CQL Phase 1, PEP 193) not supported in Pegasus since Pegasus does not yet support WEBM URI references.  Nothing in the current object path can be used to represent the name space type (i.e. the protocol).
  throw(Exception(String("CQLFunctionRep::nameSpaceType -- Function not supported")));
  
  int parmSize = _parms.size();
  if(parmSize != 0 && parmSize != 1)
  {
    throw(Exception(String("CQLFunctionRep::nameSpaceType -- incorrect number of parameters")));
  }

  return CQLValue(Uint64(0));
}

CQLValue CQLFunctionRep::hostPort(const CIMInstance& CI, const QueryContext& queryCtx) const
{
  // Pegasus currently (as of CQL Phase 1, PEP 193) does not support WEBM URI references, however the current object path dose have a host on it which we will return.  Until Pegasus supportse WEBM URI, it is not guaranteed that this will onform to the format defined in WBEM URI, and therefore this function is not entirely in accordance with the CQL Specification.
  // In addition, the CQL specification allows for the parameter to be left off when the query is executed as a result  of an ExecuteQuery operation.  However, right now (as of CQL Phase 1, PEP 193) we are only using this for Indications, and so we are assuming a "Mode" of operation.  For this function to be completely compliant with the CQL specification, it will eventually need to be "mode" aware.
  
  if(_parms.size() != 1)
  {
    throw(Exception(String("CQLFunctionRep::hostPort -- incorrect number of parameters")));
  }

  // We have a parameter, so resolve it first before we use it.
  CQLValue cqlVal = _parms[0].getSimplePredicate().getLeftExpression().resolveValue(CI,queryCtx);
  
  // If we have a String parameter, then we'll use it to create a CIMObjectPath in order to verify the format is correct.  We will then get the host from the object path and return it.  If there is no host in the path given, then a blank string will be returned.
  if (cqlVal.getValueType() == CQLValue::String_type)
  {
    CIMObjectPath objPath(cqlVal.getString());
    return CQLValue(objPath.getHost());
  }

  // If we have a CIMReference parameter, then we will just get the host name from the reference and return it.  A reference is a CIMObjectPath.  If there is no host in the path given, then a blank string will be returned.
  if (cqlVal.getValueType() == CQLValue::CIMReference_type)
    return CQLValue(cqlVal.getReference().getHost());

  // If we have a CIMObject, then we retrieve the path of the obejct and return the host from the path.  If there is no host in the path given, then a blank string will be returned.
  if (cqlVal.getValueType() == CQLValue::CIMObject_type)
    return CQLValue(cqlVal.getObject().getPath().getHost());  

  // If it makes it to this block of code, then no valid type was found, and hence no return was made.  Throw invalid parameter type exception.
    throw(Exception(String("CQLFunctionRep::hostPort -- parameter type is invalid")));
}

CQLValue CQLFunctionRep::modelPath(const CIMInstance& CI, const QueryContext& queryCtx) const
{
  // This method returns the model path portion of an object path.  The model path is the class name and the key bindings (if included)..
  
  int parmSize = _parms.size();
  if(parmSize != 0 && parmSize != 1)
  {
    throw(Exception(String("CQLFunctionRep::modelPath -- incorrect number of parameters")));
  }

  // The default behavior for this function will be to retrieve the object path from the instance being examined (CI) and then return the model path from that.
  if (parmSize == 0)
    return buildModelPath(CI.getPath());

  // We have a parameter, so resolve it first before we use it.
  CQLValue cqlVal = _parms[0].getSimplePredicate().getLeftExpression().resolveValue(CI,queryCtx);
  
  // If we have a String parameter, then we'll use it to create a CIMObjectPath in order to verify the format is correct.  We will then get model path from the object path and return it.
  if (cqlVal.getValueType() == CQLValue::String_type)
  {
    CIMObjectPath objPath(cqlVal.getString());
    return buildModelPath(objPath);
  }

  // If we have a CIMReference parameter, then we will get the model path from the reference and return it.  A reference is a CIMObjectPath.
  if (cqlVal.getValueType() == CQLValue::CIMReference_type)
    return buildModelPath(cqlVal.getReference());

  // If we have a CIMObject, then we retrieve the path of the obejct and return the model path.
  if (cqlVal.getValueType() == CQLValue::CIMObject_type)
    return buildModelPath(cqlVal.getObject().getPath());  

  // If it makes it to this block of code, then no valid type was found, and hence no return was made.  Throw invalid parameter type exception.
    throw(Exception(String("CQLFunctionRep::modelPath -- parameter type is invalid")));
}

CQLValue CQLFunctionRep::buildModelPath(const CIMObjectPath& objPath) const
{
  // This method will take an existing objet path, pick out the key bindings and the class name, and use those parts to build a new object path with just those parts in it.  This is used to represent the model path.
  CIMObjectPath newPath;
  newPath.setClassName(objPath.getClassName());
  newPath.setKeyBindings(objPath.getKeyBindings());
  return CQLValue(newPath.toString());
}

CQLValue CQLFunctionRep::classPath(const CIMInstance& CI, const QueryContext& queryCtx) const
{
  // This method returns a class path.  The class path will only have a namespace and a class name in it.  All other path information will be stripped off.  
  int parmSize = _parms.size();
  if(parmSize != 0 && parmSize != 1)
  {
    throw(Exception(String("CQLFunctionRep::classPath -- incorrect number of parameters")));
  }

  // The default behavior for this function will be to retrieve the object path from the instance being examined (CI) and build the class path from it.  If the path does not have a namespace, then the default namespace is used.
  if (parmSize == 0)
  {
    CIMObjectPath objPath(CI.getPath());
    CIMNamespaceName ns = objPath.getNameSpace();
    if (ns.isNull() || String::equal(ns.getString(), String::EMPTY))
      ns = queryCtx.getNamespace();
    return buildClassPath(objPath, ns);
  }

  // We have a parameter, so resolve it first before we use it.
  CQLValue cqlVal = _parms[0].getSimplePredicate().getLeftExpression().resolveValue(CI,queryCtx);
  
  // If we have a String parameter, then we'll use it to create a CIMObjectPath in order to verify the format is correct.  We will then build the class path from the object path and return it.  If the namespace is NOT set in the object path, it will remain unset in the returned reference.
  if (cqlVal.getValueType() == CQLValue::String_type)
  {
    CIMObjectPath objPath(cqlVal.getString());
    return buildClassPath(objPath, objPath.getNameSpace());
  }

  // If we have a CIMReference parameter, then we will build the class path from the reference and return it.  If the namespace is NOT set in the object path, it will remain unset in the returned reference.
  if (cqlVal.getValueType() == CQLValue::CIMReference_type)
  {
    CIMObjectPath objPath = cqlVal.getReference();
    return buildClassPath(objPath, objPath.getNameSpace());
  }

  // If we have a CIMObject, then we retrieve the object path  and build the class path from it.  If the path does not have a namespace, then the default namespace is used.
  if (cqlVal.getValueType() == CQLValue::CIMObject_type)
  {
    CIMObjectPath objPath = cqlVal.getObject().getPath();
    CIMNamespaceName ns = objPath.getNameSpace();
    if (ns.isNull() || String::equal(ns.getString(), String::EMPTY))
      ns = queryCtx.getNamespace();
    return buildClassPath(objPath, ns);
  }

  // If it makes it to this block of code, then no valid type was found, and hence no return was made.  Throw invalid parameter type exception.
    throw(Exception(String("CQLFunctionRep::classPath -- parameter type is invalid")));
}

CQLValue CQLFunctionRep::buildClassPath(const CIMObjectPath& objPath, const CIMNamespaceName& ns) const
{
  // This method will take the object path pass in and pick out the class name and the namespace.  The 2 parts are then combined together into a new object path which will be used as the class path and returned.
  CIMObjectPath newPath;
  newPath.setClassName(objPath.getClassName());  
  newPath.setNameSpace(ns);
  return CQLValue(newPath);
}


CQLValue CQLFunctionRep::objectPath(const CIMInstance& CI, const QueryContext& queryCtx) const
{
  // This method returns an object path.  The object path will only have a namespace, a class name, and key bindings if it is a path to an instance.  All other path information will be stripped off.
  
  int parmSize = _parms.size();
  if(parmSize != 0 && parmSize != 1)
  {
    throw(Exception(String("CQLFunctionRep::objectPath -- incorrect number of parameters")));
  }

  // The default behavior for this function will be to retrieve the object path from the instance being examined (CI) and build the object path from it.  If the path does not have a namespace, then the default namespace is used.
  if (parmSize == 0)
  {
    CIMObjectPath objPath(CI.getPath());
    CIMNamespaceName ns = objPath.getNameSpace();
    if (ns.isNull() || String::equal(ns.getString(), String::EMPTY))
      ns = queryCtx.getNamespace();
    return buildObjectPath(objPath, ns);
  }

  // We have a parameter, so resolve it first before we use it.
  CQLValue cqlVal = _parms[0].getSimplePredicate().getLeftExpression().resolveValue(CI,queryCtx);
  
  // If we have a String parameter, then we'll use it to create a CIMObjectPath in order to verify the format is correct.  We will then build the object path from the object path and return it.  If the namespace is NOT set in the object path, it will remain unset in the returned reference.
  if (cqlVal.getValueType() == CQLValue::String_type)
  {
    CIMObjectPath objPath(cqlVal.getString());
    return buildObjectPath(objPath, objPath.getNameSpace());
  }

  // If we have a CIMReference parameter, then we will build the object path from the reference and return it.  If the namespace is NOT set in the object path, it will remain unset in the returned reference.
  if (cqlVal.getValueType() == CQLValue::CIMReference_type)
  {
    CIMObjectPath objPath = cqlVal.getReference();
    return buildObjectPath(objPath, objPath.getNameSpace());
  }

  // If we have a CIMObject, then we retrieve the object path of the obejct and build the object path from it.  If the path does not have a namespace, then the default namespace is used.
  if (cqlVal.getValueType() == CQLValue::CIMObject_type)
  {
    CIMObjectPath objPath = cqlVal.getObject().getPath();
    CIMNamespaceName ns = objPath.getNameSpace();
    if (ns.isNull() || String::equal(ns.getString(), String::EMPTY))
      ns = queryCtx.getNamespace();
    return buildObjectPath(objPath, ns);
  }

  // If it makes it to this block of code, then no valid type was found, and hence no return was made.  Throw invalid parameter type exception.
    throw(Exception(String("CQLFunctionRep::objectPath -- parameter type is invalid")));
}

CQLValue CQLFunctionRep::buildObjectPath(const CIMObjectPath& objPath, const CIMNamespaceName& ns) const
{
  // This method will take the object path pass in and pick out the class name, the namespace, and the key bindings.  The parts are then combined together into a new object path which will be used as the object path and returned.
  CIMObjectPath newPath;
  newPath.setClassName(objPath.getClassName());  
  newPath.setNameSpace(ns);
  newPath.setKeyBindings(objPath.getKeyBindings());
  return CQLValue(newPath);
}

CQLValue CQLFunctionRep::instanceToReference(const CIMInstance& CI, const QueryContext& queryCtx) const
{
  // The parameter to this function MUST be an instance object.  We will use buildPath on the instance to make the path.  If there is no namespace on the instance, then the default namespace will be inserted.  The completed path is then returned.  Note, this could, and should be a more complete reference than the other path functions.
  
  int parmSize = _parms.size();
  if(parmSize != 0 && parmSize != 1)
  {
    throw(Exception(String("CQLFunctionRep::instanceToReference -- incorrect number of parameters")));
  }
  CIMInstance *inst = NULL;

  // The default behavior is to use the instance being examined as the source instance (CI).
  if (parmSize == 0)
    inst = (CIMInstance *)&CI;

  // We have a parameter, so resolve it first before we use it.
  CQLValue cqlVal = _parms[0].getSimplePredicate().getLeftExpression().resolveValue(CI,queryCtx);

  // Parameter MUST be an instance object
  if (cqlVal.getValueType() != CQLValue::CIMObject_type)
  {
    throw(Exception(String("CQLFunctionRep::instanceToReference -- parameter must be an instance")));    
  }

  // REVIEW question.  Inefficient since the CIMobject is copied via the return by value, then it is copied again via the assignment.  Is there a better way to handle this?
  CIMObject obj = cqlVal.getObject();
  if (!obj.isInstance())
  {
    throw(Exception(String("CQLFunctionRep::instanceToReference -- parameter must be an instance")));    
  }

  // Make a CIM Instance
  CIMInstance tempInst(obj);
  inst = &tempInst;

  // Get the class and build the path
  CIMConstClass cls = queryCtx.getClass(inst->getClassName());
  CIMObjectPath objPath = inst->buildPath(cls);
  CIMNamespaceName ns = objPath.getNameSpace();
  if (ns.isNull() || String::equal(ns.getString(), String::EMPTY))
    objPath.setNameSpace(queryCtx.getNamespace());
  
   return CQLValue(objPath);
}

CQLValue CQLFunctionRep::currentDateTime() const
{
  if(_parms.size() != 0)
  {
    throw(Exception(String("CQLFunctionRep::currentDateTime -- incorrect number of parameters")));
  }

  // TODO:  When Willis code is in, replace the current dummy return with the following.
  // return(CQLValue(CIMDateTime::getCurrentDateTime()))
  
   return CQLValue(Uint64(0));
}

CQLValue CQLFunctionRep::dateTime(const CIMInstance& CI, const QueryContext& queryCtx) const
{
  if(_parms.size() != 1)
  {
    throw(Exception(String("CQLFunctionRep::dateTime -- incorrect number of parameters")));
  }

  CQLValue cqlVal = _parms[0].getSimplePredicate().getLeftExpression().resolveValue(CI,queryCtx);  
  if(cqlVal.getValueType() != CQLValue::String_type)
  {
    throw(Exception(String("CQLFunctionRep::dateTime -- parm not a String_type")));
  }

  // TODO:  When Willis code is in, replace the current dummy return with the following.
  // return(CQLValue(CIMDateTime(cqlVal.getString()))
  
   return CQLValue(Uint64(0));
}

CQLValue CQLFunctionRep::microsecondToTimestamp(const CIMInstance& CI, const QueryContext& queryCtx) const
{
  if(_parms.size() != 0)
  {
    throw(Exception(String("CQLFunctionRep::microsecondToTimestamp -- incorrect number of parameters")));
  }

  CQLValue cqlVal = _parms[0].getSimplePredicate().getLeftExpression().resolveValue(CI,queryCtx);
  CQLValue::CQLValueType valType = cqlVal.getValueType();  
  if(valType != CQLValue::Uint64_type &&
     valType != CQLValue::Sint64_type)
  {
    throw(Exception(String("CQLFunctionRep::microsecondToTimestamp -- parm not an integer")));
  }
  
  Uint64 uIntVal = 0;
  if (valType == CQLValue::Sint64_type)
  {
    Sint64 intVal = cqlVal.getSint();
    if (intVal < 0)
    {
      throw(Exception(String("CQLFunctionRep::microsecondToTimestamp -- parm must be a non-negative integer")));
    }
    uIntVal = intVal;
  }
  else
    uIntVal = cqlVal.getUint();

  // TODO:  When Willis code is in, replace the current dummy return with the following.
  // return(CQLValue(CIMDateTime(uIntVal, false))  
   return CQLValue(Uint64(0));
}

CQLValue CQLFunctionRep::microsecondToInterval(const CIMInstance& CI, const QueryContext& queryCtx) const
{
  if(_parms.size() != 0)
  {
    throw(Exception(String("CQLFunctionRep::microsecondToInterval -- incorrect number of parameters")));
  }

  CQLValue cqlVal = _parms[0].getSimplePredicate().getLeftExpression().resolveValue(CI,queryCtx);
  CQLValue::CQLValueType valType = cqlVal.getValueType();  
  if(valType != CQLValue::Uint64_type &&
     valType != CQLValue::Sint64_type)
  {
    throw(Exception(String("CQLFunctionRep::microsecondToInterval -- parm not an integer")));
  }
  
  Uint64 uIntVal = 0;
  
  if (valType == CQLValue::Sint64_type)
  {
    Sint64 intVal = cqlVal.getSint();
    if (intVal < 0)
    {
      throw(Exception(String("CQLFunctionRep::microsecondToTimestamp -- parm must be a non-negative integer")));
    }
    uIntVal = intVal;
  }
  else
    uIntVal = cqlVal.getUint();

  // TODO:  When Willis code is in, replace the current dummy return with the following.
  // return(CQLValue(CIMDateTime(uIntVal, true))  
   return CQLValue(Uint64(0));
}

PEGASUS_NAMESPACE_END
