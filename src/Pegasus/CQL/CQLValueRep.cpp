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
#include <Pegasus/CQL/CQLIdentifier.h>
#include <Pegasus/CQL/CQLRegularExpression.h>
#include <Pegasus/CQL/CQLFactory.h>



PEGASUS_NAMESPACE_BEGIN
PEGASUS_USING_STD;

#define PEGASUS_ARRAY_T CQLValueRep
#include <Pegasus/Common/ArrayImpl.h>
#undef PEGASUS_ARRAY_T

#define PEGASUS_SINT64_MIN (PEGASUS_SINT64_LITERAL(0x8000000000000000))
#define PEGASUS_UINT64_MAX PEGASUS_UINT64_LITERAL(0xFFFFFFFFFFFFFFFF)
#define CIMTYPE_EMBEDDED 15  //temporary

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
   CString cStr = inString.getCString();
   char *endP;

   switch(inValueType)
   {
      case CQLValue::Hex:
         if(inSign)
         {
            _theValue.set((Uint64)strtoul((const char*)cStr,&endP,16));
            _valueType = CQLValue::Uint64_type;
         }
         else
         {
            _theValue.set((Sint64)strtol((const char *)cStr,&endP,16));
            _valueType = CQLValue::Sint64_type;
         }
         
         break;
      case CQLValue::Binary:
         if(inSign)
         {
            _theValue.set((Uint64)strtoul((const char *)cStr,&endP,2));
            _valueType = CQLValue::Uint64_type;
         }
         else
         {
            _theValue.set((Sint64)strtol((const char *)cStr,&endP,2));
            _valueType = CQLValue::Sint64_type;
         }
         break;
      case CQLValue::Decimal:
         if(inSign)
         {
            _theValue.set((Uint64)strtoul((const char *)cStr,&endP,10));
            _valueType = CQLValue::Uint64_type;
         }
         else
         {
            _theValue.set((Sint64)strtol((const char *)cStr,&endP,10));
            _valueType = CQLValue::Sint64_type;
         }
         break;
      case CQLValue::Real:
         if(inSign)
         {
            _theValue.set((Real64)strtod((const char *)cStr,&endP));
            _valueType = CQLValue::Real_type;
         }
         else
         {
            _theValue.set((Real64)strtod((const char *)cStr,&endP));
            _valueType = CQLValue::Real_type;
         }
         break;
      default:
         throw(Exception(String("CQLValueRep::CQLValueRep")));
         break;
   }
   _isResolved = true;
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
  // TODO:  _theValue.set((CIMObject)inInstance);
}

CQLValueRep::CQLValueRep(const CIMClass& inClass)
  : _isResolved(true),
    _valueType(CQLValue::CIMObject_type)
{
  // TODO:  _theValue.set((CIMObject)inClass);
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
      // TODO: _theValue.set((CIMObject)CI);
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
	}
      else if(propObj.getType() != CIMTYPE_EMBEDDED)
	{
	  // Object is not embedded.
	  _valueType = CQLValue::Null_type;
	  return;
	}
      //objectContext = propObj.getObject()
      classContext = objectContext.getClassName();
    }  
} // end of function

void CQLValueRep::_process_value(CIMProperty& propObj,
				 CQLIdentifier& _id,
				 const QueryContext& inQueryCtx)
{
  if(propObj.getType() == CIMTYPE_EMBEDDED)
    {
      CIMObject cimObj;
      //propObj.getValue(cimObj);

      // TODO: _theValue.set(cimObj);
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
  _validate(x);

  if((_theValue.getType() == x._theValue.getType()) &&
     (_valueType != CQLValue::CIMObject_type) &&
     (_valueType != CQLValue::CQLIdentifier_type) &&
     (!_theValue.isArray()))
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
	    // TODO: _theValue.get(objBase);
	    // TODO: x._theValue.get(objParm);
	    
	    _compareObjects(objBase,objParm);
	  }
	  break;
	case CQLValue::CQLIdentifier_type:
	  throw Exception(String("CQLValueRep::operator=="));
	  break;
	  
	default:
	  throw Exception(String("CQLValueRep::operator=="));
	  break;
	}
      return false;
    }
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
	case CQLValue::CIMObject_type:  
     throw Exception(String("CQLValueRep::operator=="));
     break;
   case CQLValue::CQLIdentifier_type:
     throw Exception(String("CQLValueRep::operator=="));
     break;
     
      default:
	throw Exception(String("CQLValueRep::operator=="));
	break;
   }
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
   _validate(x); 

   switch(_valueType)
     {/*
     case CQLValue::Null_type:
       throw(Exception(String("CQLValueRep::operator+")));
       break;
     case CQLValue::Boolean_type:
       throw(Exception(String("CQLValueRep::operator+")));
       break;
     case CQLValue::Sint64_type:
       if(x._valueType == CQLValue::Sint64_type)
         {
	   return CQLValueRep(_theValue._S64 + x._theValue._S64);        
         }
       else if(x._valueType == CQLValue::Uint64_type)
         {
	   return CQLValueRep(_theValue._S64 + x._theValue._U64);
         }
       else if(x._valueType == CQLValue::Real_type)
         {
	   return CQLValueRep(_theValue._S64 + x._theValue._R64);
         }
       break;
     case CQLValue::Uint64_type:
       if(x._valueType == CQLValue::Sint64_type)
         {
	   return CQLValueRep(_theValue._U64 + x._theValue._S64);
         }
       else if(x._valueType == CQLValue::Uint64_type)
         {
	   return CQLValueRep(_theValue._U64 + x._theValue._U64);
         }
       else if(x._valueType == CQLValue::Real_type)
         {
	   Real64 temp;
	   memcpy(&temp, &_theValue._U64, sizeof(temp));
	   return CQLValueRep(temp + x._theValue._R64);
         }
       break;
     case CQLValue::Real_type:
       if(x._valueType == CQLValue::Sint64_type)
         {
	   return CQLValueRep(_theValue._R64 + x._theValue._S64);
         }
       else if(x._valueType == CQLValue::Uint64_type)
         {
	   Real64 temp;
	   memcpy(&temp, &x._theValue._U64, sizeof(temp));
	   return CQLValueRep(_theValue._R64 + temp);
         }
       else if(x._valueType == CQLValue::Real_type)
         {
	   return CQLValueRep(_theValue._R64 + x._theValue._R64);
         }
       break;
      */
     case CQLValue::String_type:
       {
	 String tmpS1;
	 String tmpS2;
	 _theValue.get(tmpS1);
	 x._theValue.get(tmpS2);
	 return CQLValueRep(tmpS1 + tmpS2);
       }
       break;
     case CQLValue::CIMDateTime_type:
       throw(Exception(String("CQLValueRep::operator+")));
       break;
     case CQLValue::CIMReference_type:
       throw(Exception(String("CQLValueRep::operator+")));
       break;
     case CQLValue::CIMObject_type:
       throw(Exception(String("CQLValueRep::operator+")));
       break;
     case CQLValue::CQLIdentifier_type:
       throw(Exception(String("CQLValueRep::operator+")));
       break;
       
     default:
       throw(Exception(String("CQLValueRep::operator+")));
       break;
     }
   
   return x;
}

/*
CQLValueRep CQLValueRep::operator-(const CQLValueRep& x)
{
   if(!_validate(x))  
   {
      throw(Exception(String("CQLValueRep::operator-")));
   } 
   if(x._valueType == CQLValue::Null_type ||
      _valueType == CQLValue::Null_type)
   {
      return CQLValueRep();
   }
    
   switch(_valueType)
   {
      case CQLValue::Boolean_type:
         throw(Exception(String("CQLValueRep::operator-")));
         break;
      case CQLValue::Sint64_type:
         if(x._valueType == CQLValue::Sint64_type)
         {
            return CQLValueRep(_theValue._S64 - x._theValue._S64);        
         }
         else if(x._valueType == CQLValue::Uint64_type)
         {
            return CQLValueRep(_theValue._S64 - x._theValue._U64);
         }
         else if(x._valueType == CQLValue::Real_type)
         {
            return CQLValueRep(_theValue._S64 - x._theValue._R64);
         }
         break;
      case CQLValue::Uint64_type:
         if(x._valueType == CQLValue::Sint64_type)
         {
            return CQLValueRep(_theValue._U64 - x._theValue._S64);
         }
         else if(x._valueType == CQLValue::Uint64_type)
         {
            return CQLValueRep(_theValue._U64 - x._theValue._U64);
         }
         else if(x._valueType == CQLValue::Real_type)
         {
         	Real64 temp;
         	memcpy(&temp, &_theValue._U64, sizeof(temp));
            return CQLValueRep(temp - x._theValue._R64);
         }
         break;
      case CQLValue::Real_type:
         if(x._valueType == CQLValue::Sint64_type)
         {
            return CQLValueRep(_theValue._R64 - x._theValue._S64);
         }
         else if(x._valueType == CQLValue::Uint64_type)
         {
         	Real64 temp;
         	memcpy(&temp, &x._theValue._U64, sizeof(temp));
            return CQLValueRep(_theValue._R64 - temp);
         }
         else if(x._valueType == CQLValue::Real_type)
         {
            return CQLValueRep(_theValue._R64 - x._theValue._R64);
         }
         break;
      case CQLValue::String_type:
         throw(Exception(String("CQLValueRep::operator-")));
         break;
      case CQLValue::CIMDateTime_type:
         throw(Exception(String("CQLValueRep::operator-")));
         break;
      case CQLValue::CIMReference_type:
         throw(Exception(String("CQLValueRep::operator-")));
         break;
      case CQLValue::CIMInstance_type:
            throw(Exception(String("CQLValueRep::operator-")));
         break;
      case CQLValue::CQLIdentifier_type:
            throw(Exception(String("CQLValueRep::operator-")));
         break;

      default:
         throw(Exception(String("CQLValueRep::operator-")));
         break;
   }
   
   return x;
}


CQLValueRep CQLValueRep::operator*(const CQLValueRep& x)
{

   if(!_validate(x))  
   {
      throw(Exception(String("CQLValueRep::operator*")));
   } 

   if(x._valueType == CQLValue::Null_type ||
      _valueType == CQLValue::Null_type)
   {
      return CQLValueRep();
   }
    
   switch(_valueType)
   {
      case CQLValue::Boolean_type:
         throw(Exception(String("CQLValueRep::operator*")));
         break;
      case CQLValue::Sint64_type:
         if(x._valueType == CQLValue::Sint64_type)
         {
            return CQLValueRep(_theValue._S64 * x._theValue._S64);        
         }
         else if(x._valueType == CQLValue::Uint64_type)
         {
            return CQLValueRep(_theValue._S64 * x._theValue._U64);
         }
         else if(x._valueType == CQLValue::Real_type)
         {
            return CQLValueRep(_theValue._S64 * x._theValue._R64);
         }
         break;
      case CQLValue::Uint64_type:
         if(x._valueType == CQLValue::Sint64_type)
         {
            return CQLValueRep(_theValue._U64 * x._theValue._S64);
         }
         else if(x._valueType == CQLValue::Uint64_type)
         {
            return CQLValueRep(_theValue._U64 * x._theValue._U64);
         }
         else if(x._valueType == CQLValue::Real_type)
         {
         	Real64 temp;
         	memcpy(&temp, &_theValue._U64, sizeof(temp));
            return CQLValueRep(temp * x._theValue._R64);
         }
         break;
      case CQLValue::Real_type:
         if(x._valueType == CQLValue::Sint64_type)
         {
            return CQLValueRep(_theValue._R64 * x._theValue._S64);
         }
         else if(x._valueType == CQLValue::Uint64_type)
         {
         	Real64 temp;
         	memcpy(&temp, &x._theValue._U64, sizeof(temp));
            return CQLValueRep(_theValue._R64 * temp);
         }
         else if(x._valueType == CQLValue::Real_type)
         {
            return CQLValueRep(_theValue._R64 * x._theValue._R64);
         }
         break;
      case CQLValue::String_type:
         throw(Exception(String("CQLValueRep::operator*")));
         break;
      case CQLValue::CIMDateTime_type:
         throw(Exception(String("CQLValueRep::operator*")));
         break;
      case CQLValue::CIMReference_type:
         throw(Exception(String("CQLValueRep::operator*")));
         break;
      case CQLValue::CIMInstance_type:
            throw(Exception(String("CQLValueRep::operator*")));
         break;
      case CQLValue::CQLIdentifier_type:
            throw(Exception(String("CQLValueRep::operator*")));
         break;

      default:
         throw(Exception(String("CQLValueRep::operator*")));
         break;
   }
   
   return x;
}


CQLValueRep CQLValueRep::operator/(const CQLValueRep& x)
{
   if(!_validate(x) || 
      x._theValue._U64 == 0)  
   {
      throw(Exception(String("CQLValueRep::operator/")));
   } 
   
   if(x._valueType == CQLValue::Null_type ||
      _valueType == CQLValue::Null_type)
   {
      return CQLValueRep();
   }
 
   switch(_valueType)
   {
      case CQLValue::Boolean_type:
         throw(Exception(String("CQLValueRep::operator/")));
         break;
      case CQLValue::Sint64_type:
         if(x._valueType == CQLValue::Sint64_type)
         {
            return CQLValueRep(_theValue._S64 / x._theValue._S64);        
         }
         else if(x._valueType == CQLValue::Uint64_type)
         {
            return CQLValueRep(_theValue._S64 / x._theValue._U64);
         }
         else if(x._valueType == CQLValue::Real_type)
         {
            return CQLValueRep(_theValue._S64 / x._theValue._R64);
         }
         break;
      case CQLValue::Uint64_type:
         if(x._valueType == CQLValue::Sint64_type)
         {
            return CQLValueRep(_theValue._U64 / x._theValue._S64);
         }
         else if(x._valueType == CQLValue::Uint64_type)
         {
            return CQLValueRep(_theValue._U64 / x._theValue._U64);
         }
         else if(x._valueType == CQLValue::Real_type)
         {
         	Real64 temp;
         	memcpy(&temp, &_theValue._U64, sizeof(temp));
            return CQLValueRep(temp / x._theValue._R64);
         }
         break;
      case CQLValue::Real_type:
         if(x._valueType == CQLValue::Sint64_type)
         {
            return CQLValueRep(_theValue._R64 / x._theValue._S64);
         }
         else if(x._valueType == CQLValue::Uint64_type)
         {
         	Real64 temp;
         	memcpy(&temp, &x._theValue._U64, sizeof(temp));
            return CQLValueRep(_theValue._R64 / temp);
         }
         else if(x._valueType == CQLValue::Real_type)
         {
            return CQLValueRep(_theValue._R64 / x._theValue._R64);
         }
         break;
      case CQLValue::String_type:
         throw(Exception(String("CQLValueRep::operator/")));
         break;
      case CQLValue::CIMDateTime_type:
         throw(Exception(String("CQLValueRep::operator/")));
         break;
      case CQLValue::CIMReference_type:
         throw(Exception(String("CQLValueRep::operator/")));
         break;
      case CQLValue::CIMInstance_type:
            throw(Exception(String("CQLValueRep::operator/")));
         break;
      case CQLValue::CQLIdentifier_type:
            throw(Exception(String("CQLValueRep::operator/")));
         break;

      default:
         throw(Exception(String("CQLValueRep::operator/")));
         break;
   }
   
   return x;
}
*/


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
   if(!_isResolved || 
      (_valueType != CQLValue::CIMObject_type))
     {
       throw(Exception(String("CQLValueRep::isa")));
     }
   
   CIMName className;
   CIMObject obj;

   // TODO: _theValue.get(obj);

   className = obj.getClassName();

   Array<CIMName> cimNames = QueryCtx.enumerateClassNames(inID[0].getName());

   for(Uint32 i = 0; i < cimNames.size() ; ++i)
     {
       if(cimNames[i] == className)
	 {
	   return true;
	 }
     }

   return false;
}


Boolean CQLValueRep::like(const CQLValueRep& inVal)
{
   if( _valueType != CQLValue::String_type ||
      inVal._valueType != CQLValue::String_type)
   {
      throw(Exception(String("CQLValueRep::like")));
   }
   
   String leftside;
   _theValue.get(leftside);

   String rightside;
   inVal._theValue.get(rightside);
   
   CQLRegularExpression re;
   return re.match(leftside,rightside);

}

/*
void CQLValueRep::invert()
{
   switch(_valueType)
   {
      case CQLValue::Sint64_type:
         _theValue._S64 = -1 * _theValue._S64;
         break;
      case CQLValue::Real_type:
         _theValue._R64 = -1 * _theValue._R64;
         break;
      case CQLValue::Boolean_type:
         _theValue._B = !_theValue._B;
         break;
      default:
         break;
   }
}
*/
CQLChainedIdentifier CQLValueRep::getChainedIdentifier()const
{
   return _CQLChainId;
}

Uint64 CQLValueRep::getUint()const
{
   if(_valueType != CQLValue::Uint64_type)
   {
      throw(Exception(String("CQLValueRep::getUint")));
   }
   Uint64 tmp;
   _theValue.get(tmp);
   return tmp;
}

Boolean CQLValueRep::getBool()const
{
   if(_valueType != CQLValue::Boolean_type)
   {
      throw(Exception(String("CQLValueRep::getBool")));
   }
   Boolean tmp;
   _theValue.get(tmp);
   return tmp;
}

Sint64 CQLValueRep::getSint()const
{
   if(_valueType != CQLValue::Sint64_type)
   {
      throw(Exception(String("CQLValueRep::getSint")));
   }
   Sint64 tmp;
   _theValue.get(tmp);
   return tmp;
}

Real64 CQLValueRep::getReal()const
{
   if(_valueType != CQLValue::Real_type)
   {
      throw(Exception(String("CQLValueRep::getReal")));
   }
   Real64 tmp;
   _theValue.get(tmp);
   return tmp;
}

String CQLValueRep::getString()const
{
   if(_valueType != CQLValue::String_type)
   {
      throw(Exception(String("CQLValueRep::getString")));
   }
   String tmp;
   _theValue.get(tmp);
   return tmp;
}

CIMDateTime CQLValueRep::getDateTime()const
{
   if(_valueType != CQLValue::CIMDateTime_type)
   {
      throw(Exception(String("CQLValueRep::getDateTime")));
   }
   CIMDateTime tmp;
   _theValue.get(tmp);
   return tmp;
}

CIMObjectPath CQLValueRep::getReference()const
{
   if(_valueType != CQLValue::CIMReference_type)
   {
      throw(Exception(String("CQLValueRep::getReference")));
   }
   CIMObjectPath tmp;
   _theValue.get(tmp);
   return tmp;
}

CIMObject CQLValueRep::getObject()const
{
   if(_valueType != CQLValue::CIMObject_type)
   {
      throw(Exception(String("CQLValueRep::getInstance")));
   }
   CIMObject tmp;
   // TODO: _theValue.get(tmp);
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
   switch(_valueType)
   {
      case CQLValue::Boolean_type:
         if(x._valueType != CQLValue::Boolean_type)
         {
             throw TypeMismatchException(String("CQLValueRep::_validate"));
         }
         break;
      case CQLValue::Sint64_type:
      case CQLValue::Uint64_type:
      case CQLValue::Real_type:
         if(x._valueType != CQLValue::Sint64_type &&
            x._valueType != CQLValue::Uint64_type &&
            x._valueType != CQLValue::Real_type)
         {
            throw TypeMismatchException(String("CQLValueRep::_validate"));
         }
         break;
      case CQLValue::String_type:
         if(x._valueType != CQLValue::String_type)
         {
            throw TypeMismatchException(String("CQLValueRep::_validate"));
         }
         break;
      case CQLValue::CIMDateTime_type:
         if(x._valueType != CQLValue::CIMDateTime_type)
         {
            throw TypeMismatchException(String("CQLValueRep::_validate"));
         }
         break;
      case CQLValue::CIMReference_type:
         if(x._valueType != CQLValue::CIMReference_type)
         {
            throw TypeMismatchException(String("CQLValueRep::_validate"));
         }
         break;
      case CQLValue::CIMObject_type:
         if(x._valueType != CQLValue::CIMObject_type)
         {
            throw TypeMismatchException(String("CQLValueRep::_validate"));
         }
         break;
      case CQLValue::CQLIdentifier_type:
         if(x._valueType != CQLValue::CQLIdentifier_type)
         {
            throw TypeMismatchException(String("CQLValueRep::_validate"));
         }
         break;

      default:
         throw TypeMismatchException(String("CQLValueRep::_validate"));
         break;
   }
   return;
}

void CQLValueRep::_setValue(CIMValue cv,Sint64 index)
{
  CIMValue tmp;
  if(cv.isArray() && index != -1)
    {
      switch(cv.getType())
	{
	case CIMTYPE_BOOLEAN:
	  {
            Array<Boolean> _bool;
            cv.get(_bool);
            _theValue.set(_bool[index]);
            _valueType = CQLValue::Boolean_type;
            break;
	  }
	case CIMTYPE_UINT8:
	  {
            Array<Uint8> _uint;
            cv.get(_uint);
            _theValue.set((Uint64)_uint[index]);
            _valueType = CQLValue::Uint64_type;
            break;
	  }
	case CIMTYPE_UINT16:
	  {
            Array<Uint16> _uint;
            cv.get(_uint);
            _theValue.set((Uint64)_uint[index]);
            _valueType = CQLValue::Uint64_type;
            break;
	  }
	case CIMTYPE_UINT32:
	  {
            Array<Uint32> _uint;
            cv.get(_uint);
            _theValue.set((Uint64)_uint[index]);
            _valueType = CQLValue::Uint64_type;
            break;
	  }
	case CIMTYPE_UINT64:
	  {
            Array<Uint64> _uint;
            cv.get(_uint);
            _theValue.set((Uint64)_uint[index]);
            _valueType = CQLValue::Uint64_type;
            break;
	  }
	case CIMTYPE_SINT8:
	  {
	    Array<Sint8> _sint;
	    cv.get(_sint);
	    _theValue.set((Sint64)_sint[index]);
	    _valueType = CQLValue::Sint64_type;
	    break;
	  }
	case CIMTYPE_SINT16:
	  {
	    Array<Sint16> _sint;
	    cv.get(_sint);
	    _theValue.set((Sint64)_sint[index]);
	    _valueType = CQLValue::Sint64_type;
	    break;
	  }
	case CIMTYPE_SINT32:
	  {
            Array<Sint32> _sint;
            cv.get(_sint);
            _theValue.set((Sint64)_sint[index]);
            _valueType = CQLValue::Sint64_type;
            break;
	  }
	case CIMTYPE_SINT64:
	  {
            Array<Sint64> _sint;
            cv.get(_sint);
            _theValue.set((Sint64)_sint[index]);
            _valueType = CQLValue::Sint64_type;
            break;
	  }
	  
	case CIMTYPE_REAL32:
	  {
            Array<Real32> _real;
            cv.get(_real);
            _theValue.set((Real64)_real[index]);
            _valueType = CQLValue::Real_type;
            break;
	  }
	case CIMTYPE_REAL64:
	  {
            Array<Real64> _real;
            cv.get(_real);
            _theValue.set((Real64)_real[index]);
            _valueType = CQLValue::Real_type;
            break;
	  }   
	case CIMTYPE_CHAR16:
	  {
            Array<Char16> _str;
            cv.get(_str);
            _theValue.set(String(&_str[index]));
            _valueType = CQLValue::String_type;
            break;
	  }
	case CIMTYPE_STRING:
	  {
            Array<String> _str;
            cv.get(_str);
            _theValue.set(_str[index]);
            _valueType = CQLValue::String_type;
            break;
	  }  
	case CIMTYPE_DATETIME:
	  {
            Array<CIMDateTime> _date;
            cv.get(_date);
            _theValue.set(_date[index]);
            _valueType = CQLValue::CIMDateTime_type;
            break;
	  }
	case CIMTYPE_REFERENCE:
	  {
            Array<CIMObjectPath> _path;
            cv.get(_path);
            _theValue.set(_path[index]);
            _valueType = CQLValue::CIMReference_type;
            break;
	  }   
	case CIMTYPE_EMBEDDED:
	  {
            Array<CIMObject> _obj;
            // TODO: cv.get(_obj);
            // TODO: _theValue.set(_obj[index]);
            _valueType = CQLValue::CIMObject_type;
            break;
	  }   
	default:
	  throw(Exception(String("CQLValueRep::_setValue")));
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
	    Real64 _tmp;
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
	case CIMTYPE_EMBEDDED:
	  {
	    _theValue = cv;
            _valueType = CQLValue::CIMObject_type;
            break;
	  }   
	default:
	  throw(Exception(String("CQLValueRep::_setValue")));
	}

    }

  _isResolved = true;
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
      throw(Exception(String("CQLValueRep::_resolveSymbolicConstant1")));
   }

   CIMProperty queryPropObj = QueryClass.getProperty(propertyIndex);

   // We have a symbolic constant (ex. propName#OK)
   // We need to retrieve the ValueMap and Values Qualifiers for 
   // the property if the exist.
   Uint32 qualIndex = queryPropObj.findQualifier(CIMName("ValueMap"));

   if(qualIndex == PEG_NOT_FOUND)
   {
      // This property can not be processed with a symbolic constant.
      throw(Exception(String("CQLValueRep::_resolveSymbolicConstant2")));
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
	   throw(Exception(String("CQLValueRep::_resolveSymbolicConstant3")));
	 }
       
       // The symbolic constant defined in the CQLIdentifier is 
       // valid for this property. Now we need to set the value.
       // Set primitive
       _setValue(CIMValue(lid.getSymbolicConstantName()));
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
	   throw(Exception(String("CQLValueRep::_resolveSymbolicConstant4"))); 
	 }
       
       CString cStr = valueMapArray[matchIndex].getCString();
       char *endP;
       
       // Set Primitive
       _setValue(CIMValue(Uint64(strtoul((const char*)cStr,&endP,10))));
       return;
     }
}

Boolean CQLValueRep::_compareObjects(CIMObject& _in1, CIMObject& _in2)
{
  return true;
}

PEGASUS_NAMESPACE_END
