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
// Author: Dave Rosckes (rosckes@us.ibm.com)
//
// Modified By: Dan Gorey (djgorey@us.ibm.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#include <cstdlib>
#include <iostream>

#include <Pegasus/CQL/CQLValue.h>
#include <Pegasus/CQL/CQLValueRep.h>
#include <Pegasus/Repository/NameSpaceManager.h>
#include <Pegasus/Common/CIMClass.h>
#include <Pegasus/Common/Tracer.h>
#include <Pegasus/CQL/CQLIdentifier.h>
#include <Pegasus/CQL/CQLRegularExpression.h>
#include <Pegasus/CQL/CQLFactory.h>
#include <Pegasus/Query/QueryCommon/QueryException.h>
#include <Pegasus/CQL/CQLUtilities.h>


PEGASUS_NAMESPACE_BEGIN
PEGASUS_USING_STD;

/*
#define PEGASUS_ARRAY_T CQLValueRep
#include <Pegasus/Common/ArrayImpl.h>
#undef PEGASUS_ARRAY_T
*/

#define PEGASUS_SINT64_MIN (PEGASUS_SINT64_LITERAL(0x8000000000000000))
#define PEGASUS_UINT64_MAX PEGASUS_UINT64_LITERAL(0xFFFFFFFFFFFFFFFF)


CQLValueRep::CQLValueRep()
  :_valueType(CQLValue::Null_type)
{
}

CQLValueRep::~CQLValueRep()
{

}


CQLValueRep::CQLValueRep(const CQLValueRep& val)
{
  _theValue = val._theValue;

   _CQLChainId = val._CQLChainId;

   _isResolved = val._isResolved;

   _valueType = val._valueType;

  
}

CQLValueRep::CQLValueRep(const CQLValueRep* val)
{
  _theValue =  val->_theValue;

  _CQLChainId = val->_CQLChainId;

  _isResolved = val->_isResolved;

  _valueType = val->_valueType;
}

CQLValueRep::CQLValueRep(const String& inString, CQLValue::NumericType inValueType, Boolean inSign)
{
   PEG_METHOD_ENTER(TRC_CQL, "CQLValueRep::CQLValueRep()");
   CString cStr = inString.getCString();
   char *endP;

   switch(inValueType)
   {
   case CQLValue::Hex:
     {
       String tmp(inString);
       if(inSign)
	 {
	   _theValue.set(CQLUtilities::stringToUint64(tmp));
	   _valueType = CQLValue::Uint64_type;
	 }
       else
	 {
	   _theValue.set(CQLUtilities::stringToSint64(inString));
	   _valueType = CQLValue::Sint64_type;
	 }
     }
     break;
   case CQLValue::Binary:
     {
       String tmp(inString);;
       if(inSign)
	 {
	   _theValue.set(CQLUtilities::stringToUint64(tmp));
	   _valueType = CQLValue::Uint64_type;
	 }
       else
	 {
	   _theValue.set(CQLUtilities::stringToSint64(tmp));
	   _valueType = CQLValue::Sint64_type;
	 }
       break;
     }
   case CQLValue::Decimal:
     {
       String tmp(inString);
       if(inSign)
	 {
	   _theValue.set(CQLUtilities::stringToUint64(tmp));
	   _valueType = CQLValue::Uint64_type;
	 }
       else
	 {
	   _theValue.set(CQLUtilities::stringToSint64(tmp));
	   _valueType = CQLValue::Sint64_type;
	 }
     }
     break;
   case CQLValue::Real:
     {
       String tmp(inString);
       if(inSign)
	 {
	   _theValue.set(CQLUtilities::stringToReal64(tmp));
	   _valueType = CQLValue::Real_type;
	 }
       else
	 {
	   _theValue.set(CQLUtilities::stringToReal64(tmp));
	   _valueType = CQLValue::Real_type;
	 }
     }
     break;
   default:
     MessageLoaderParms mload(String("CQL.CQLValueRep.CONSTRUCTOR_FAILURE"),
			      String("Undefined case in constructor."));
     throw CQLRuntimeException(mload);
     
     break;
   }
   
   _isResolved = true;
   
   PEG_METHOD_EXIT();
}


CQLValueRep::CQLValueRep(const CQLChainedIdentifier& inCQLIdent)
  : _CQLChainId(inCQLIdent), 
    _isResolved(false),
    _valueType(CQLValue::CQLIdentifier_type)
{
   
}


CQLValueRep::CQLValueRep(const String& inString)
  : _isResolved(true), 
    _valueType(CQLValue::String_type)
{
   _theValue.set(inString);
}

CQLValueRep::CQLValueRep(const CIMInstance& inInstance)
  : _isResolved(true),
    _valueType(CQLValue::CIMObject_type)
{
  _theValue.set((CIMObject)inInstance);
}

CQLValueRep::CQLValueRep(const CIMClass& inClass)
  : _isResolved(true),
    _valueType(CQLValue::CIMObject_type)
{
  _theValue.set((CIMObject)inClass);
}

CQLValueRep::CQLValueRep(const CIMObject& inObject)
  : _isResolved(true),
    _valueType(CQLValue::CIMObject_type)
{
  _theValue.set((CIMObject)inObject);
}

CQLValueRep::CQLValueRep(const CIMObjectPath& inObjPath)
  : _isResolved(true),
    _valueType(CQLValue::CIMReference_type)
{
   _theValue.set(inObjPath);
}

CQLValueRep::CQLValueRep(const CIMDateTime& inDateTime)
  : _isResolved(true),
    _valueType(CQLValue::CIMDateTime_type)
{
   _theValue.set(inDateTime);
}

CQLValueRep::CQLValueRep(Uint64 inUint)
  : _isResolved(true),
    _valueType(CQLValue::Uint64_type)
{
   _theValue.set(inUint);
}

CQLValueRep::CQLValueRep(Boolean inBool)
  : _isResolved(true),
    _valueType(CQLValue::Boolean_type)
{
   _theValue.set(inBool);
}

CQLValueRep::CQLValueRep(Sint64 inSint)
  : _isResolved(true),
    _valueType(CQLValue::Sint64_type)
{
   _theValue.set(inSint);
}

CQLValueRep::CQLValueRep(Real64 inReal)
  : _isResolved(true),
    _valueType(CQLValue::Real_type)
{
   _theValue.set(inReal);
}

CQLValueRep::CQLValueRep(const CIMValue& inVal)
{
  _setValue(inVal);
}

void CQLValueRep::resolve(const CIMInstance& CI, const  QueryContext& inQueryCtx)
{ 
  if(_CQLChainId.size() == 0)
    {
      return;
    }
  

  Array<CQLIdentifier> Idstrings = 
    _CQLChainId.getSubIdentifiers(); // Array of Identifiers to process
  
  Uint32 index = 0;                // Counter for looping through Identifiers
  CIMProperty propObj;  
  
  if(Idstrings.size() == 1)
    {
      // A class was passed in with no property indicated.
      // Set the instance passed in, as a primitive.
       _theValue.set((CIMObject)CI);
      _valueType = CQLValue::CIMObject_type;
      _isResolved = true;
      return; // Done.
    }
  else
    {
      // Symbolic Constant processing
      if(_CQLChainId.getLastIdentifier().isSymbolicConstant() &&
	 Idstrings.size() == 2)
	{
	  _resolveSymbolicConstant(inQueryCtx);
	  return;
	}
      
      // Need to increment index since the first Identifier is a class,
      // and additional identifiers need processing.
      ++index;
     }
  
  CIMName classContext = Idstrings[0].getName();
  CIMObject objectContext = CI;
   
  for(;index < Idstrings.size(); ++index)
    {
      // Now we need to verify that the property is in the class.
      Uint32 propertyIndex = objectContext.findProperty(Idstrings[index].getName());
      
      if(propertyIndex == PEG_NOT_FOUND)
	{
	  _valueType = CQLValue::Null_type;
	  return;
	}
      
      // We will check the property type to determine what processing 
      // needs to be done.
      propObj = objectContext.getProperty(propertyIndex);

      /*      
      if(inQueryCtx.getClassRelation(Idstrings[index].getScope(),classContext)
	 == QueryContext::NOTRELATED)
	{
	  // The chain is not inline with scope.
	  _valueType = CQLValue::Null_type;
	  return;
	}
      */
      
      if(index == Idstrings.size()-1)
	{
	  _process_value(propObj,Idstrings[index],inQueryCtx);
	  return;
	}
      else if(propObj.getType() != CIMTYPE_OBJECT)
	{
	  // Object is not embedded.
	  _valueType = CQLValue::Null_type;
	  return;
	}
      propObj.getValue().get(objectContext);
      classContext = objectContext.getClassName();
    }  
} // end of function

void CQLValueRep::_process_value(CIMProperty& propObj,
				 CQLIdentifier& _id,
				 const QueryContext& inQueryCtx)
{
  if(propObj.getType() == CIMTYPE_OBJECT) 
    {
      CIMObject cimObj;
      propObj.getValue().get(cimObj);

      _theValue.set(cimObj);
      _valueType = CQLValue::CIMObject_type;

    }
  else // Primitive
    {
      if(_id.isArray())
	{
	  // We have an array property.  All we need to do
	  // Is get the index defined by CQLIdentifier.
	  // NOTE: Basic CQL support only allows one index.
	  _setValue(propObj.getValue(),
		    _id.getSubRanges()[0].start);
	  return;
	}
      else if(_id.isSymbolicConstant())
	{
	  // Symbolic Constant processing
	  _resolveSymbolicConstant(inQueryCtx);
	  return;
	}
      else
	{
	  // The property has no special charactors.
	  if(propObj.isArray())
	    {
	      Uint32 qualIndex = propObj.findQualifier(CIMName(String("ArrayType")));
	      propObj.getQualifier(qualIndex).getValue().get(_ArrayType);
	    }
	  _setValue(propObj.getValue());
	  return;
	}
    }
}

CQLValueRep& CQLValueRep::operator=(const CQLValueRep& rhs)
{

  if(&rhs != this)
    {
      _valueType = rhs._valueType;
      _theValue = rhs._theValue;
      _CQLChainId = rhs._CQLChainId;
      _isResolved = rhs._isResolved;
    }
  return *this;
}
Boolean CQLValueRep::operator==(const CQLValueRep& x)
{
  PEG_METHOD_ENTER(TRC_CQL, "CQLValueRep::operator==");
  _validate(x);

  if(_theValue.isArray())
    {
      return _compareArray(x);
    }
  else if((_theValue.getType() == x._theValue.getType()) &&
     (_valueType != CQLValue::CIMObject_type) &&
     (_valueType != CQLValue::CQLIdentifier_type))
    {
      return _theValue == x._theValue;
    }
  else
    {
      Uint64 tmpU64;
      Sint64 tmpS64;
      Real64 tmpR64;
      
      switch(_valueType)
	{
	case CQLValue::Null_type:
	  {
	    if(x._valueType == CQLValue::Null_type)
	      {
		return true;
	      }
	  }
	  break;
	  
	case CQLValue::Sint64_type:
	  {
	    _theValue.get(tmpS64);
	    if(x._valueType == CQLValue::Uint64_type)
	      {
		x._theValue.get(tmpU64);
		
		if(tmpU64 > (Uint64)PEGASUS_SINT64_MIN)
		  {
		    return false;
		  }
		else
		  {
		    return (Sint64)tmpU64 == tmpS64;
		  }
	      }
	    else 
	      {
		x._theValue.get(tmpR64);
		
		return tmpR64 == tmpS64;
	      }
	    break;
	  }
	case CQLValue::Uint64_type:
	  {
	    _theValue.get(tmpU64);
	    if(x._valueType == CQLValue::Sint64_type)
	      {
		x._theValue.get(tmpS64);
		
		if(tmpU64 > (Uint64)PEGASUS_SINT64_MIN)
		  {
		    return false;
		  }
		else
		  {
		    return (Sint64)tmpU64 == tmpS64;
		  }
	      }
	    else 
	      {
		x._theValue.get(tmpR64);
		if(tmpU64 > (Uint64)PEGASUS_SINT64_MIN)
		  {
		    return false;
		  }
		else
		  {
		    return tmpR64 == (Sint64)tmpU64;
		  }
	      }
	    break;
	  }
	case CQLValue::Real_type:
	  {
	    _theValue.get(tmpR64);
	    if(x._valueType == CQLValue::Uint64_type)
	      {
		x._theValue.get(tmpU64);
		
		if(tmpU64 > (Uint64)PEGASUS_SINT64_MIN)
		  {
		    return false;
		  }
		else
		  {
		    return (Sint64)tmpU64 == tmpR64;
		  }
	      }
	    else 
	      {
		x._theValue.get(tmpS64);
		
		return tmpR64 == tmpS64;
	      }
	    break;
	  }
	case CQLValue::CIMObject_type: 
	  { 
	    CIMObject objBase;
	    CIMObject objParm;
	    _theValue.get(objBase);
	    x._theValue.get(objParm);
       
	    return _compareObjects(objBase,objParm);
       
	  }
	  break;
	  
	default:
	MessageLoaderParms mload(String("CQL.CQLValueRep.CONSTRUCTOR_FAILURE"),
				 String("Undefined case in constructor."));
	throw CQLRuntimeException(mload);
	  break;
	}
      return false;
    }
  PEG_METHOD_EXIT();
}
  
Boolean CQLValueRep::operator!=(const CQLValueRep& x)
{
  return !(this->operator==(x));
}


Boolean CQLValueRep::operator<=(const CQLValueRep& x)
{   
  if (this->operator<(x) || this->operator==(x))
    {
      return true;
    }
  
  return false;
}


Boolean CQLValueRep::operator>=(const CQLValueRep& x)
{
  _validate(x);
   
   return !(this->operator<(x));  
}


Boolean CQLValueRep::operator<(const CQLValueRep& x)
{
  PEG_METHOD_ENTER(TRC_CQL, "CQLValueRep::operator<");

  Uint64 tmpU64;
  Sint64 tmpS64;
  Real64 tmpR64;
  
  switch(_valueType)
    {
    case CQLValue::Null_type:
      {
	return false;
      }
      break;
      
    case CQLValue::Sint64_type:
      {
	_theValue.get(tmpS64);
	if(x._valueType == CQLValue::Sint64_type)
	  {
	    Sint64 right;
	    x._theValue.get(right);
	    
	    return tmpS64 < right;
	  }
	else if(x._valueType == CQLValue::Uint64_type)
	  {
	    x._theValue.get(tmpU64);
	    
	    if(tmpU64 > (Uint64)PEGASUS_SINT64_MIN)
	      {
		return true;
	      }
	    else
	      {
		return tmpS64 < (Sint64)tmpU64;
	      }
	  }
	else 
	  {
	    x._theValue.get(tmpR64);
	    
	    return tmpS64 < tmpR64;
	  }
	break;
      }
    case CQLValue::Uint64_type:
      {
	_theValue.get(tmpU64);
	if(x._valueType == CQLValue::Uint64_type)
	  {
	    Uint64 right;
	    x._theValue.get(right);
	    
	    return tmpU64 < right;
	  }
	else if(x._valueType == CQLValue::Sint64_type)
	  {
	    x._theValue.get(tmpS64);
	    
	    if(tmpU64 > (Uint64)PEGASUS_SINT64_MIN)
	      {
		return false;
	      }
	    else
	      {
		return (Sint64)tmpU64 < tmpS64;
	      }
	  }
	else 
	  {
	    x._theValue.get(tmpR64);
	    if(tmpU64 > (Uint64)PEGASUS_SINT64_MIN)
	      {
		return false;
	      }
	   else
	     {
	       return (Sint64)tmpU64 < tmpR64;
	     }
	  }
	break;
      }
    case CQLValue::Real_type:
      {
	_theValue.get(tmpR64);
	if(x._valueType == CQLValue::Real_type)
	  {
	    Real64 right;
	    x._theValue.get(right);
	   
	    return tmpR64 < right;
	  }
	else if(x._valueType == CQLValue::Uint64_type)
	  {
	    x._theValue.get(tmpU64);
	    
	    if(tmpU64 > (Uint64)PEGASUS_SINT64_MIN)
	      {
		return true;
	      }
	    else
	      {
		return tmpR64 < (Sint64)tmpU64;
	      }
	  }
	else 
	  {
	    x._theValue.get(tmpS64);
	    
	    return tmpR64 < tmpS64;
	  }
	break;
      }
    case CQLValue::String_type:
      {
	String tmpS1;
	String tmpS2;
	_theValue.get(tmpS1);
	x._theValue.get(tmpS2);
	return tmpS1 < tmpS2;
      }
      break;
    case CQLValue::CIMDateTime_type:  
      {
	CIMDateTime tmpS1;
	CIMDateTime tmpS2;
	_theValue.get(tmpS1);
	x._theValue.get(tmpS2);
   //	return tmpS1 < tmpS2; datetime
      }
      break;

    default:
	MessageLoaderParms mload(String("CQL.CQLValueRep.CONSTRUCTOR_FAILURE"),
				 String("Undefined case in constructor."));
	throw CQLRuntimeException(mload);
      break;
    }
  PEG_METHOD_EXIT();
  return false;
}


Boolean CQLValueRep::operator>(const CQLValueRep& x)
{
   _validate(x);
  
   if (this->operator<(x) || this->operator==(x))
     {
       return false;
     }  
   return true;
}


CQLValueRep CQLValueRep::operator+(const CQLValueRep x)
{
  PEG_METHOD_ENTER(TRC_CQL,"CQLValueRep::operator+");
   _validate(x); 

   switch(_valueType)
     {
     case CQLValue::String_type:
       {
	 String tmpS1;
	 String tmpS2;
	 _theValue.get(tmpS1);
	 x._theValue.get(tmpS2);
	 return CQLValueRep(tmpS1 + tmpS2);
       }
       break;
       
     default:
       MessageLoaderParms mload(String("CQL.CQLValueRep.CONSTRUCTOR_FAILURE"),
				String("Undefined case in constructor."));
       throw CQLRuntimeException(mload);
       break;
     }

   PEG_METHOD_EXIT();
   return x;
}

CQLValue::CQLValueType CQLValueRep::getValueType()
{
   return _valueType;
}


void CQLValueRep::setNull()
{
   _valueType = CQLValue::Null_type;
   _isResolved = true;
}


Boolean CQLValueRep::isResolved()
{
   return _isResolved;
}


Boolean CQLValueRep::isNull()
{
   if(_valueType == CQLValue::Null_type)
   {
      return true;
   }
   return false;
}


Boolean CQLValueRep::isa(const CQLChainedIdentifier& inID,QueryContext& QueryCtx)
{
  PEG_METHOD_ENTER(TRC_CQL,"CQLValueRep::isa()");
   if(!_isResolved || 
      (_valueType != CQLValue::CIMObject_type))
     {
       MessageLoaderParms mload(String("CQL.CQLValueRep.ISA_TYPE_MISMATCH"),
				String("The type is not an object."));
       throw CQLRuntimeException(mload);
     }
   
   CIMName className;
   CIMObject obj;

   _theValue.get(obj);

   className = obj.getClassName();
   cout << inID[0].getName().getString() << endl;
   Array<CIMName> cimNames = QueryCtx.enumerateClassNames(inID[0].getName());

   for(Uint32 i = 0; i < cimNames.size() ; ++i)
     {
       cout << cimNames[i].getString() << " = " << className.getString() << endl;
       if(cimNames[i] == className)
	 {
	   PEG_METHOD_EXIT();
	   return true;
	 }
     }
   PEG_METHOD_EXIT();
   return false;
}


Boolean CQLValueRep::like(const CQLValueRep& inVal)
{
  PEG_METHOD_ENTER(TRC_CQL,"CQLValueRep::like()");
   if( _valueType != CQLValue::String_type ||
      inVal._valueType != CQLValue::String_type)
   {
       MessageLoaderParms mload(String("CQL.CQLValueRep.LIKE_TYPE_MISMATCH"),
				String("The type is not a string."));
       throw CQLRuntimeException(mload);
   }
   
   String leftside;
   _theValue.get(leftside);

   String rightside;
   inVal._theValue.get(rightside);
   
   CQLRegularExpression re;

   PEG_METHOD_EXIT();
   return re.match(leftside,rightside);

}

CQLChainedIdentifier CQLValueRep::getChainedIdentifier()const
{
   return _CQLChainId;
}

Uint64 CQLValueRep::getUint()const
{
  PEG_METHOD_ENTER(TRC_CQL,"CQLValueRep::getUint()");
   if(_valueType != CQLValue::Uint64_type)
   {
       MessageLoaderParms mload(String("CQL.CQLValueRep.TYPE_MISMATCH"),
				String("The type is not correct for get operation."));
       throw CQLRuntimeException(mload);
   }
   Uint64 tmp;
   _theValue.get(tmp);

   PEG_METHOD_EXIT();
   return tmp;
}

Boolean CQLValueRep::getBool()const
{
  PEG_METHOD_ENTER(TRC_CQL,"CQLValueRep::getBool()");
   if(_valueType != CQLValue::Boolean_type)
   {
       MessageLoaderParms mload(String("CQL.CQLValueRep.TYPE_MISMATCH"),
				String("The type is not correct for get operation."));
       throw CQLRuntimeException(mload);
   }
   Boolean tmp;
   _theValue.get(tmp);

   PEG_METHOD_EXIT();
   return tmp;
}

Sint64 CQLValueRep::getSint()const
{
  PEG_METHOD_ENTER(TRC_CQL,"CQLValueRep::getSint()");

   if(_valueType != CQLValue::Sint64_type)
   {
       MessageLoaderParms mload(String("CQL.CQLValueRep.TYPE_MISMATCH"),
				String("The type is not correct for get operation."));
       throw CQLRuntimeException(mload);
   }
   Sint64 tmp;
   _theValue.get(tmp);

   PEG_METHOD_EXIT();
   return tmp;
}

Real64 CQLValueRep::getReal()const
{
  PEG_METHOD_ENTER(TRC_CQL,"CQLValueRep::getReal()");
   if(_valueType != CQLValue::Real_type)
   {
       MessageLoaderParms mload(String("CQL.CQLValueRep.TYPE_MISMATCH"),
				String("The type is not correct for get operation."));
       throw CQLRuntimeException(mload);
   }
   Real64 tmp;
   _theValue.get(tmp);
   PEG_METHOD_EXIT();
   return tmp;
}

String CQLValueRep::getString()const
{
  PEG_METHOD_ENTER(TRC_CQL,"CQLValueRep::getString()");
   if(_valueType != CQLValue::String_type)
   {
       MessageLoaderParms mload(String("CQL.CQLValueRep.TYPE_MISMATCH"),
				String("The type is not correct for get operation."));
       throw CQLRuntimeException(mload);
   }
   String tmp;
   _theValue.get(tmp);
   PEG_METHOD_EXIT();
   return tmp;
}

CIMDateTime CQLValueRep::getDateTime()const
{
  PEG_METHOD_ENTER(TRC_CQL,"CQLValueRep::getDateTime()");
   if(_valueType != CQLValue::CIMDateTime_type)
   {
       MessageLoaderParms mload(String("CQL.CQLValueRep.TYPE_MISMATCH"),
				String("The type is not correct for get operation."));
       throw CQLRuntimeException(mload);
   }
   CIMDateTime tmp;
   _theValue.get(tmp);
   PEG_METHOD_EXIT();
   return tmp;
}

CIMObjectPath CQLValueRep::getReference()const
{
  PEG_METHOD_ENTER(TRC_CQL,"CQLValueRep::getReference()");

   if(_valueType != CQLValue::CIMReference_type)
   {
       MessageLoaderParms mload(String("CQL.CQLValueRep.TYPE_MISMATCH"),
				String("The type is not correct for get operation."));
       throw CQLRuntimeException(mload);
   }
   CIMObjectPath tmp;
   _theValue.get(tmp);

   PEG_METHOD_EXIT();
   return tmp;
}

CIMObject CQLValueRep::getObject()const
{
  PEG_METHOD_ENTER(TRC_CQL,"CQLValueRep::getObject()");
   if(_valueType != CQLValue::CIMObject_type)
   {
       MessageLoaderParms mload(String("CQL.CQLValueRep.TYPE_MISMATCH"),
				String("The type is not correct for get operation."));
       throw CQLRuntimeException(mload);
   }
   CIMObject tmp;
   _theValue.get(tmp);
   PEG_METHOD_EXIT();
   return tmp.clone();
}

String CQLValueRep::toString()const
{
  if(_valueType == CQLValue::CQLIdentifier_type)
    {
      return _CQLChainId.toString();
    }
  else
    {
      return _theValue.toString();
    }
}



void CQLValueRep::_validate(const CQLValueRep& x)
{
  PEG_METHOD_ENTER(TRC_CQL,"CQLValueRep::_validate()");
   switch(_valueType)
   {
      case CQLValue::Boolean_type:
         if(x._valueType != CQLValue::Boolean_type)
         {
	   MessageLoaderParms mload(String("CQL.CQLValueRep.OP_TYPE_MISMATCH"),
				    String("Invalid types for operation."));
	   throw CQLRuntimeException(mload);
         }
         break;
      case CQLValue::Sint64_type:
      case CQLValue::Uint64_type:
      case CQLValue::Real_type:
         if(x._valueType != CQLValue::Sint64_type &&
            x._valueType != CQLValue::Uint64_type &&
            x._valueType != CQLValue::Real_type)
         {
	   MessageLoaderParms mload(String("CQL.CQLValueRep.OP_TYPE_MISMATCH"),
				    String("Invalid types for operation."));
	   throw CQLRuntimeException(mload);
         }
         break;
      case CQLValue::String_type:
         if(x._valueType != CQLValue::String_type)
         {
	   MessageLoaderParms mload(String("CQL.CQLValueRep.OP_TYPE_MISMATCH"),
				    String("Invalid types for operation."));
	   throw CQLRuntimeException(mload);
         }
         break;
      case CQLValue::CIMDateTime_type:
         if(x._valueType != CQLValue::CIMDateTime_type)
         {
	   MessageLoaderParms mload(String("CQL.CQLValueRep.OP_TYPE_MISMATCH"),
				    String("Invalid types for operation."));
	   throw CQLRuntimeException(mload);
         }
         break;
      case CQLValue::CIMReference_type:
         if(x._valueType != CQLValue::CIMReference_type)
         {
	   MessageLoaderParms mload(String("CQL.CQLValueRep.OP_TYPE_MISMATCH"),
				    String("Invalid types for operation."));
	   throw CQLRuntimeException(mload);
         }
         break;
      case CQLValue::CIMObject_type:
         if(x._valueType != CQLValue::CIMObject_type)
         {
	   MessageLoaderParms mload(String("CQL.CQLValueRep.OP_TYPE_MISMATCH"),
				    String("Invalid types for operation."));
	   throw CQLRuntimeException(mload);
         }
         break;
      case CQLValue::CQLIdentifier_type:
         if(x._valueType != CQLValue::CQLIdentifier_type)
         {
	   MessageLoaderParms mload(String("CQL.CQLValueRep.OP_TYPE_MISMATCH"),
				    String("Invalid types for operation."));
	   throw CQLRuntimeException(mload);
         }
         break;

      default:
	   MessageLoaderParms mload(String("CQL.CQLValueRep.OP_TYPE_MISMATCH"),
				    String("Invalid types for operation."));
	   throw CQLRuntimeException(mload);
         break;
   }
   PEG_METHOD_EXIT();
   return;
}

void CQLValueRep::_setValue(CIMValue cv,Sint64 key)
{
  PEG_METHOD_ENTER(TRC_CQL,"CQLValueRep::_setValue()");
  CIMValue tmp;
  Uint32 index;

  if(key != -1)
    {
      index = (Uint32)key;
    }
  if(cv.isArray())
    {
      switch(cv.getType())
	{
	case CIMTYPE_BOOLEAN:
	  {
	    if(key == -1)
	      {
		_theValue = cv;
	      }
	    else
	      {
		Array<Boolean> _bool;
		cv.get(_bool);
		_theValue.set(_bool[index]);
	      }

	    _valueType = CQLValue::Boolean_type;
            break;
	  }
	case CIMTYPE_UINT8:
	  {
	    Array<Uint8> _uint;
	    cv.get(_uint);

	    if(key == -1)
	      {
		Array<Uint64> _uint64;
		for(Uint32 i = 0; i < _uint.size(); ++i)
		  {
		    _uint64.append((Uint64)_uint[i]);
		  }
		_theValue = CIMValue(_uint64);
	      }
	    else
	      {
		_theValue.set((Uint64)_uint[index]);
	      }
            _valueType = CQLValue::Uint64_type;
            break;
	  }
	case CIMTYPE_UINT16:
	  {
	    Array<Uint16> _uint;
	    cv.get(_uint);

	    if(key == -1)
	      {
		Array<Uint64> _uint64;
		for(Uint32 i = 0; i < _uint.size(); ++i)
		  {
		    _uint64.append((Uint64)_uint[i]);
		  }
		_theValue = CIMValue(_uint64);
	      }
	    else
	      {
		_theValue.set((Uint64)_uint[index]);
	      }
            _valueType = CQLValue::Uint64_type;
            break;
	  }
	case CIMTYPE_UINT32:
	  {
	    Array<Uint32> _uint;
	    cv.get(_uint);

	    if(key == -1)
	      {
		Array<Uint64> _uint64;
		for(Uint32 i = 0; i < _uint.size(); ++i)
		  {
		    _uint64.append((Uint64)_uint[i]);
		  }
		_theValue = CIMValue(_uint64);
	      }
	    else
	      {
		_theValue.set((Uint64)_uint[index]);
	      }
            _valueType = CQLValue::Uint64_type;
            break;
	  }
	case CIMTYPE_UINT64:
	  {
	    if(key == -1)
	      {
		_theValue = cv;
	      }
	    else
	      {
		Array<Uint64> _uint;
		cv.get(_uint);
		_theValue.set((Uint64)_uint[index]);
	      }
            _valueType = CQLValue::Uint64_type;
            break;
	  }
	case CIMTYPE_SINT8:
	  {
	    Array<Sint8> _sint;
	    cv.get(_sint);

	    if(key == -1)
	      {
		Array<Sint64> _sint64;
		for(Uint32 i = 0; i < _sint.size(); ++i)
		  {
		    _sint64.append((Sint64)_sint[i]);
		  }
		_theValue = CIMValue(_sint64);
	      }
	    else
	      {
		_theValue.set((Sint64)_sint[index]);
	      }
	    _valueType = CQLValue::Sint64_type;
	    break;
	  }
	case CIMTYPE_SINT16:
	  {
	    Array<Sint16> _sint;
	    cv.get(_sint);

	    if(key == -1)
	      {
		Array<Sint64> _sint64;
		for(Uint32 i = 0; i < _sint.size(); ++i)
		  {
		    _sint64.append((Sint64)_sint[i]);
		  }
		_theValue = CIMValue(_sint64);
	      }
	    else
	      {
		_theValue.set((Sint64)_sint[index]);
	      }
	    _valueType = CQLValue::Sint64_type;
	    break;
	  }
	case CIMTYPE_SINT32:
	  {
	    Array<Sint32> _sint;
	    cv.get(_sint);

	    if(key == -1)
	      {
		Array<Sint64> _sint64;
		for(Uint32 i = 0; i < _sint.size(); ++i)
		  {
		    _sint64.append((Sint64)_sint[i]);
		  }
		_theValue = CIMValue(_sint64);
	      }
	    else
	      {
		_theValue.set((Sint64)_sint[index]);
	      }
            _valueType = CQLValue::Sint64_type;
            break;
	  }
	case CIMTYPE_SINT64:
	  {
	    if(key == -1)
	      {
		_theValue = cv;
	      }
	    else
	      {
		Array<Sint64> _sint;
		cv.get(_sint);
		_theValue.set((Sint64)_sint[index]);
	      }
            _valueType = CQLValue::Sint64_type;
            break;
	  }
	  
	case CIMTYPE_REAL32:
	  {
	    Array<Real32> _real;
	    cv.get(_real);

	    if(key == -1)
	      {
		Array<Real64> _real64;
		for(Uint32 i = 0; i < _real.size(); ++i)
		  {
		    _real64.append((Real64)_real[i]);
		  }
		_theValue = CIMValue(_real64);
	      }
	    else
	      {
		_theValue.set((Real64)_real[index]);
	      }
            _valueType = CQLValue::Real_type;
            break;
	  }
	case CIMTYPE_REAL64:
	  {
	    if(key == -1)
	      {
		_theValue = cv;
	      }
	    else
	      {
		Array<Real64> _real;
		cv.get(_real);
		_theValue.set((Real64)_real[index]);
	      }
            _valueType = CQLValue::Real_type;
            break;
	  }   
	case CIMTYPE_CHAR16:
	  {
	    Array<Char16> _str16;
	    cv.get(_str16);

	    if(key == -1)
	      {
		Array<String> _str;
		for(Uint32 i = 0; i < _str16.size(); ++i)
		  {
		    _str.append(String(&_str16[i]));
		  }
		_theValue = CIMValue(_str);
	      }
	    else
	      {
		_theValue.set(String(&_str16[index]));
	      }
	    _valueType = CQLValue::String_type;
            break;
	  }
	case CIMTYPE_STRING:
	  {
	    if(key == -1)
	      {
		_theValue = cv;
	      }
	    else
	      {
		Array<String> _str;
		cv.get(_str);
		_theValue.set(_str[index]);
	      }
            _valueType = CQLValue::String_type;
            break;
	  }  
	case CIMTYPE_DATETIME:
	  {
	    if(key == -1)
	      {
		_theValue = cv;
	      }
	    else
	      {
		Array<CIMDateTime> _date;
		cv.get(_date);
		_theValue.set(_date[index]);
	      }
            _valueType = CQLValue::CIMDateTime_type;
            break;
	  }
	case CIMTYPE_REFERENCE:
	  {
	    if(key == -1)
	      {
		_theValue = cv;
	      }
	    else
	      {
		Array<CIMObjectPath> _path;
		cv.get(_path);
		_theValue.set(_path[index]);
	      }
            _valueType = CQLValue::CIMReference_type;
            break;
	  }   
	case CIMTYPE_OBJECT:
	  {
	    if(key == -1)
	      {
		_theValue = cv;
	      }
	    else
	      {
		Array<CIMObject> _obj;
		cv.get(_obj);
		_theValue.set(_obj[index]);
	      }
            _valueType = CQLValue::CIMObject_type;
            break;
	  }  
	default:
	   MessageLoaderParms mload(String("CQL.CQLValueRep.SET_VALUE"),
				    String("Unable to set internal object."));
	   throw CQLRuntimeException(mload);
	} // switch statement 
    }
  else
    {
      switch(cv.getType())
	{
	case CIMTYPE_BOOLEAN:
	  {
	    _theValue = cv;
            _valueType = CQLValue::Boolean_type;
            break;
	  }
	case CIMTYPE_UINT8:
	  {
	    Uint8 _tmp;
	    cv.get(_tmp);
	    _theValue = CIMValue((Uint64)_tmp);
	    _valueType = CQLValue::Uint64_type;
	    break;
	  }
	case CIMTYPE_UINT16:
	  {
	    Uint16 _tmp;
	    cv.get(_tmp);
	    _theValue = CIMValue((Uint64)_tmp);
	    _valueType = CQLValue::Uint64_type;
	    break;
	  }
	case CIMTYPE_UINT32:
	  {
	    Uint32 _tmp;
	    cv.get(_tmp);
	    _theValue = CIMValue((Uint64)_tmp);
	    _valueType = CQLValue::Uint64_type;
	    break;
	  }
	case CIMTYPE_UINT64:
	  {
	    Uint64 _tmp;
	    cv.get(_tmp);
	    _theValue = CIMValue((Uint64)_tmp);
	    _valueType = CQLValue::Uint64_type;
	    break;
	  }
	case CIMTYPE_SINT8:
	  {
	    Sint8 _tmp;
	    cv.get(_tmp);
	    _theValue = CIMValue((Sint64)_tmp);
	    _valueType = CQLValue::Sint64_type;
	    break;
	  }
	case CIMTYPE_SINT16:
	  {
	    Sint16 _tmp;
	    cv.get(_tmp);
	    _theValue = CIMValue((Sint64)_tmp);
	    _valueType = CQLValue::Sint64_type;
	    break;
	  }
	case CIMTYPE_SINT32:
	  {
	    Sint32 _tmp;
	    cv.get(_tmp);
	    _theValue = CIMValue((Sint64)_tmp);
	    _valueType = CQLValue::Sint64_type;
	    break;
	  }
	case CIMTYPE_SINT64:
	  {
	    Sint64 _tmp;
	    cv.get(_tmp);
	    _theValue = CIMValue((Sint64)_tmp);
	    _valueType = CQLValue::Sint64_type;
	    break;
	  }
	case CIMTYPE_REAL32:
	  {
	    Real32 _tmp;
	    cv.get(_tmp);
	    _theValue = CIMValue((Real64)_tmp);
	    _valueType = CQLValue::Real_type;
	    break;
	  }
	case CIMTYPE_REAL64:
	  {
	    Real64 _tmp;
	    cv.get(_tmp);
	    _theValue = CIMValue((Real64)_tmp);
	    _valueType = CQLValue::Real_type;
	    break;
	  }   
	case CIMTYPE_CHAR16:
	  {
	    Char16 _tmp;
	    cv.get(_tmp);
	    _theValue = CIMValue(String(&_tmp));
	    _valueType = CQLValue::String_type;
	    break;
	  }  
	case CIMTYPE_STRING:
	  {
	    _theValue = cv;
	    _valueType = CQLValue::String_type;
	    break;
	  }   
	case CIMTYPE_DATETIME:
	  {
	    _theValue = cv;
	    _valueType = CQLValue::CIMDateTime_type;
	    break;
	  } 
	case CIMTYPE_REFERENCE:
	  {
	    _theValue = cv;
            _valueType = CQLValue::CIMReference_type;
            break;
	  }   
	case CIMTYPE_OBJECT:
	  {
	    _theValue = cv;
            _valueType = CQLValue::CIMObject_type;
            break;
	  }   
	default:
	   MessageLoaderParms mload(String("CQL.CQLValueRep.SET_VALUE"),
				    String("Unable to set internal object."));
	   throw CQLRuntimeException(mload);
	}

    }

  _isResolved = true;

  PEG_METHOD_EXIT();
  return;
}

void CQLValueRep::applyContext(QueryContext& _ctx,
                              CQLChainedIdentifier& inCid)
{
   if(inCid.size() != 0 && _CQLChainId.size() == 1)
     {  
       // If we get here we have a stand alone Symbolic constant.
       // We need to take the chain and create a complete context
       // for the symbolic constant.  We will use the context from
       // inCid to populate the context for chain.
       /*
        _CQLChainId[0].setName(inCid[inCid.size()-1].getName());
        _CQLChainId[0].applyScope(inCid[inCid.size()-1].getScope());
       */
       
       CQLIdentifier id = _CQLChainId[0];
       id.setName(inCid[inCid.size()-1].getName());
       id.applyScope(inCid[inCid.size()-1].getScope());

       CQLChainedIdentifier chainId(id);

       for(Sint32 i = inCid.size()-2; i >= 0; --i)
       {
         chainId.prepend(inCid[i]); 
       }

       _CQLChainId = chainId;


	CIMInstance temp;
	resolve(temp,_ctx);
     }
   else
     {
       _CQLChainId.applyContext(_ctx); 
     }

   // Add the chained identifier to the WHERE identifier list.
   // Note: CQLValue's are only used in WHERE processing.
   if (_CQLChainId.size() > 0)
   {
     _ctx.addWhereIdentifier(_CQLChainId);
   }
}

void CQLValueRep::_resolveSymbolicConstant(const QueryContext& inQueryCtx)
{
  PEG_METHOD_ENTER(TRC_CQL,"CQLValueRep::_resolveSymbolicConstant()");

   Array<String> valueMapArray;     // Value Map Qualifier for property
   Array<String> valuesArray;       // Values Qualifier for property
   CIMName className;
   CQLIdentifier lid = _CQLChainId.getLastIdentifier();
   CIMClass QueryClass;
   CIMValue valueMap;               // CIMValue for Value Map Qualifiers
   CIMValue values;                 // CIMValue for Values Qualifiers
   Boolean matchFound = false;      // Indicator for match Qualifier
   Uint32 matchIndex;               // Placeholder for matched Qualifier

   if(lid.isScoped())
   {
     className = lid.getScope();
   }
   else
   {
     className = _CQLChainId[0].getName();
   }

   PEGASUS_STD(cout) << _CQLChainId.toString() << PEGASUS_STD(endl);

   QueryClass = inQueryCtx.getClass(className);

   Uint32 propertyIndex = 
         QueryClass.findProperty(lid.getName());

   if(propertyIndex == PEG_NOT_FOUND)
   {
     MessageLoaderParms mload(String("CQL.CQLValueRep.PROP_NOT_FOUND"),
			      String("Property $0 not found on class $1."),
			      lid.getName().getString(),
			      className.getString());
     throw CQLRuntimeException(mload);
   }

   CIMProperty queryPropObj = QueryClass.getProperty(propertyIndex);

   // We have a symbolic constant (ex. propName#OK)
   // We need to retrieve the ValueMap and Values Qualifiers for 
   // the property if the exist.
   Uint32 qualIndex = queryPropObj.findQualifier(CIMName("ValueMap"));

   if(qualIndex == PEG_NOT_FOUND)
   {
      // This property can not be processed with a symbolic constant.
     MessageLoaderParms mload(String("CQL.CQLValueRep.QUALIFIER_NOT_FOUND"),
			      String("Qualifier $0 not found on Property $1 in class $2."),
			      String("ValueMap"),
			      lid.getName().getString(),
			      className.getString());
     throw CQLRuntimeException(mload);
   }

   valueMap = queryPropObj.getQualifier(qualIndex).getValue();
   qualIndex = queryPropObj.findQualifier(CIMName("Values"));

   if(qualIndex == PEG_NOT_FOUND)
     {
       // This property does not have a Values Qualifier,
       // therefore the valueMap must be the list of symbolic constants.
       
       valueMap.get(valueMapArray);
       
       // We will loop through the list of Symbolic constants to 
       // determine if we have a match with the Symbolic constant
       // defined in the CQLIdentifier.
       for(Uint32 i = 0; i < valueMapArray.size(); ++i)
	 {
	   if(String::equalNoCase(valueMapArray[i],lid.getSymbolicConstantName()))
	     {
	       matchFound = true;
	       matchIndex = i;
	       break;
	     }
	 }
       if(matchFound == false)
	 {
	   // The symbolic constant provided is not valid
	   // for this property.
	   MessageLoaderParms mload(String("CQL.CQLValueRep.INVALID_SYMBOLIC_CONSTANT"),
				    String("Provided symbolic constant $0 is not valid for property $1 in class $2."),
				    lid.getSymbolicConstantName(),
				    lid.getName().getString(),
				    className.getString());
	   throw CQLRuntimeException(mload);
	 }
       
       // The symbolic constant defined in the CQLIdentifier is 
       // valid for this property. Now we need to set the value.
       // Set primitive
       _setValue(CIMValue(lid.getSymbolicConstantName()));

       PEG_METHOD_EXIT();
       return;
     }
   else
     {
       // The qualifier Values is defined for the property.
       // valueMap must be a list of #'s.
       
       values = queryPropObj.getQualifier(qualIndex).getValue();
       
       valueMap.get(valueMapArray);
       values.get(valuesArray);
       
       // We will loop through the list of Symbolic constants to 
       // determine if we have a match with the Symbolic constant
       // defined in the CQLIdentifier.
       for(Uint32 i = 0; i < valuesArray.size(); ++i)
	 {
	   if(String::equalNoCase(valuesArray[i],lid.getSymbolicConstantName()))
	     {
	       matchFound = true;
	       matchIndex = i;
	       break;
	     }
	 }
       if(matchFound == false)
	 {
	   // The symbolic constant provided is not valid
	   // for this property.
	   MessageLoaderParms mload(String("CQL.CQLValueRep.INVALID_SYMBOLIC_CONSTANT"),
				    String("Provided symbolic constant $0 is not valid for property $1 in class $2."),
				    lid.getSymbolicConstantName(),
				    lid.getName().getString(),
				    className.getString());
	   throw CQLRuntimeException(mload); 
	 }
       
       CString cStr = valueMapArray[matchIndex].getCString();
       char *endP;
       
       // Set Primitive
       _setValue(CIMValue(Uint64(strtoul((const char*)cStr,&endP,10))));

       PEG_METHOD_EXIT();
       return;
     }
 
}

Boolean CQLValueRep::_compareObjects(CIMObject& _in1, CIMObject& _in2)
{
  if(_in1.isClass() != _in2.isClass())
    {
      return false;
    }
  else if(_in1.isClass())
    { // objects are classes 
      return _in1.identical(_in2);
    }
  else
    { // objects are instances

      if(_in1.getPropertyCount() !=
	 _in2.getPropertyCount())
	{
	  return false;
	}

      Array<CIMProperty> prop1;
      Array<CIMProperty> prop2;
      Boolean result;

      for(Uint32 i = 0; i < _in1.getPropertyCount(); ++i)
	{
	  prop1.append(_in1.getProperty(i));
	  prop2.append(_in2.getProperty(i));
	}

      for(Uint32 i = 0; i < _in1.getPropertyCount(); ++i)
	{
	  result = false;

	  for(Uint32 j = 0; j < _in2.getPropertyCount(); ++j)
	    {
	      if(prop1[i].getName() == prop2[j].getName())
		{
		  if(prop1[i].isArray() != prop2[j].isArray())
		    {
		      break;
		    }
		  if(prop1[i].isArray())
		    {
		      CQLValueRep left;
		      CQLValueRep right;
		      left._setValue(prop1[i].getValue());
		      right._setValue(prop2[j].getValue());
		      result = left._compareArray(right);
		    }
		  else
		    {
		      if(CQLValue(prop1[i].getValue()) == CQLValue(prop2[j].getValue()))
			{
			  result = true;
			  break;
			}
		      else
			{
			  cout << "object:   " << _in1.getPath().toString() << endl;
			  cout << "property: " << prop1[i].getName().getString() << endl;

			  cout << "object:   " << _in2.getPath().toString() << endl;
			  cout << "property: " << prop2[j].getName().getString() << endl;
			  result = false;
			  break;
			}
		    }
		}
	    }
	  if(result == false)
	    {
	      return false;
	    }
	}
    }
  return true;
}

Boolean CQLValueRep::_compareArray(const CQLValueRep& _in)
{
  PEG_METHOD_ENTER(TRC_CQL,"CQLValueRep::_compareArray()");
  // Currently pegasus has no concept of different array types( Bagged, Ordered, or Indexed)
  // We will do bag comparisons.
  
  Boolean result;
  Array<Boolean>       _bool1;
  Array<Boolean>       _bool2;
  Array<Uint64>        _uint1;
  Array<Uint64>        _uint2;
  Array<Sint64>        _sint1;
  Array<Sint64>        _sint2;
  Array<Real64>        _real1;
  Array<Real64>        _real2;
  Array<String>        _str1;
  Array<String>        _str2;
  Array<CIMDateTime>   _date1;
  Array<CIMDateTime>   _date2;
  Array<CIMObjectPath> _path1;
  Array<CIMObjectPath> _path2;
  Array<CIMObject>     _obj1;
  Array<CIMObject>     _obj2;

  Array<CQLValue>      _cqlVal1;
  Array<CQLValue>      _cqlVal2;

  CIMValue _in1 = _theValue;
  CIMValue _in2 = _in._theValue;

  String _arrayType1 = _ArrayType;
  String _arrayType2 = _in._ArrayType;

  switch(_in1.getType())
    {
    case CIMTYPE_BOOLEAN:
      {
	_in1.get(_bool1);
	for(Uint32 i = 0; i < _bool1.size(); ++i)
	  {
	    _cqlVal1.append(CQLValue(_bool1[i]));
	  }
	break;
      }
    case CIMTYPE_UINT64:
      {
	_in1.get(_uint1);
	for(Uint32 i = 0; i < _uint1.size(); ++i)
	  {
	    _cqlVal1.append(CQLValue(_uint1[i]));
	  }
	break;
      }
    case CIMTYPE_SINT64:
      {
	_in1.get(_sint1);
	for(Uint32 i = 0; i < _sint1.size(); ++i)
	  {
	    _cqlVal1.append(CQLValue(_sint1[i]));
	  }
	break;
      }
    case CIMTYPE_REAL64:
      {
	_in1.get(_real1);
	for(Uint32 i = 0; i < _real1.size(); ++i)
	  {
	    _cqlVal1.append(CQLValue(_real1[i]));
	  }
	break;
      }   
    case CIMTYPE_STRING:
      {
	_in1.get(_str1);
	for(Uint32 i = 0; i < _str1.size(); ++i)
	  {
	    _cqlVal1.append(CQLValue(_str1[i]));
	  }
	break;
      }  
    case CIMTYPE_DATETIME:
      {
	_in1.get(_date1);
	for(Uint32 i = 0; i < _date1.size(); ++i)
	  {
	    _cqlVal1.append(CQLValue(_date1[i]));
	  }
	break;
      }
    case CIMTYPE_REFERENCE:
      {
	_in1.get(_path1);
	for(Uint32 i = 0; i < _path1.size(); ++i)
	  {
	    _cqlVal1.append(CQLValue(_path1[i]));
	  }
	break;
      }   
    case CIMTYPE_OBJECT:
      {
	_in1.get(_obj1);
	for(Uint32 i = 0; i < _obj1.size(); ++i)
	  {
	    _cqlVal1.append(CQLValue(_obj1[i]));
	  }
	break;
      } 
    default:
      MessageLoaderParms mload(String("CQL.CQLValueRep.INVALID_ARRAY_COMPARISON"),
			       String("Invalid array comparison type."));
      throw CQLRuntimeException(mload); 
    } // switch statement 
  
  switch(_in2.getType())
    {
    case CIMTYPE_BOOLEAN:
      {
	_in2.get(_bool2);
	for(Uint32 i = 0; i < _bool2.size(); ++i)
	  {
	    _cqlVal2.append(CQLValue(_bool2[i]));
	  }
	break;
      }
    case CIMTYPE_UINT64:
      {
	_in2.get(_uint2);
	for(Uint32 i = 0; i < _uint2.size(); ++i)
	  {
	    _cqlVal2.append(CQLValue(_uint2[i]));
	  }
	break;
      }
    case CIMTYPE_SINT64:
      {
	_in2.get(_sint2);
	for(Uint32 i = 0; i < _sint2.size(); ++i)
	  {
	    _cqlVal2.append(CQLValue(_sint2[i]));
	  }
	break;
      }
    case CIMTYPE_REAL64:
      {
	_in2.get(_real2);
	for(Uint32 i = 0; i < _real2.size(); ++i)
	  {
	    _cqlVal2.append(CQLValue(_real2[i]));
	  }
	break;
      }   
    case CIMTYPE_STRING:
      {
	_in2.get(_str2);
	for(Uint32 i = 0; i < _str2.size(); ++i)
	  {
	    _cqlVal2.append(CQLValue(_str2[i]));
	  }
	break;
      }  
    case CIMTYPE_DATETIME:
      {
	_in2.get(_date2);
	for(Uint32 i = 0; i < _date2.size(); ++i)
	  {
	    _cqlVal2.append(CQLValue(_date2[i]));
	  }
	break;
      }
    case CIMTYPE_REFERENCE:
      {
	_in2.get(_path2);
	for(Uint32 i = 0; i < _path2.size(); ++i)
	  {
	    _cqlVal2.append(CQLValue(_path2[i]));
	  }
	break;
      }
    case CIMTYPE_OBJECT:
      {
	_in2.get(_obj2);
	for(Uint32 i = 0; i < _obj2.size(); ++i)
	  {
	    _cqlVal2.append(CQLValue(_obj2[i]));
	  }
	break;
      }   
    default:
      MessageLoaderParms mload(String("CQL.CQLValueRep.INVALID_ARRAY_COMPARISON"),
			       String("Invalid array comparison type."));
      throw CQLRuntimeException(mload); 
    } // switch statement 

  if((_arrayType1 == String("Indexed") ||
      _arrayType1 == String("Ordered")) &&
     (_arrayType2 == String("Indexed") ||
      _arrayType2 == String("Ordered")))
    { // Handle the indexed or ordered case.
      for(Uint32 i = 0; i < _cqlVal1.size(); ++i)
	{ 
	  if(_cqlVal1[i] != _cqlVal2[i])
	    {
	      PEG_METHOD_EXIT();
	      return false;
	    }
	}
    }  
  else
    {
      for(Uint32 i = 0; i < _cqlVal1.size(); ++i)
	{
	  result = false;
	  
	  for(Uint32 j = 0; j < _cqlVal2.size(); ++j)
	    {
	      if(_cqlVal1[i] == _cqlVal2[j])
		{
		  result = true;
		  break;
		}
	    }
	  if(result == false)
	    {
	      PEG_METHOD_EXIT();
	      return false;
	    }
	}
      
      for(Uint32 i = 0; i < _cqlVal2.size(); ++i)
	{
	  result = false;
	  
	  for(Uint32 j = 0; j < _cqlVal1.size(); ++j)
	    {
	      if(_cqlVal2[i] == _cqlVal1[j])
		{
		  result = true;
		  break;
		}
	    }
	  if(result == false)
	    {
	      PEG_METHOD_EXIT();
	      return false;
	    }
	}
    }
  PEG_METHOD_EXIT();
  return true;
}

PEGASUS_NAMESPACE_END
