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
// Modified By:
//
//%/////////////////////////////////////////////////////////////////////////////

#include <cstdlib>
#include <iostream>

#include <Pegasus/CQL/CQLValue.h>
#include <Pegasus/CQL/CQLValueRep.h>
#include <Pegasus/Repository/NameSpaceManager.h>
#include <Pegasus/Common/CIMClass.h>
#include <Pegasus/CQL/CQLIdentifier.h>

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
  _theValue._S = NULL;
}
CQLValueRep::~CQLValueRep()
{
   switch(_valueType)
   {
      case CQLValue::String_type: 
      { 
         if(_theValue._S != NULL){
            delete _theValue._S;
	}
         _theValue._S = NULL;  
         break;
      }
      case CQLValue::CIMDateTime_type:  
       { 
         if(_theValue._DT != NULL)
            delete _theValue._DT;
         _theValue._DT = NULL;  
         break;
      }
      case CQLValue::CIMReference_type:  
      { 
         if(_theValue._OP != NULL)
            delete _theValue._OP;
         _theValue._OP = NULL;  
         break;
      }
      case CQLValue::CIMInstance_type:  
      { 
         if(_theValue._IN != NULL)
            delete _theValue._IN;
         _theValue._IN = NULL;  
         break;
      }
      case CQLValue::CIMClass_type:  
      { 
         if(_theValue._CL != NULL)
            delete _theValue._CL;
         _theValue._CL = NULL;  
         break;
      }
      default:
         break;
   }
}


CQLValueRep::CQLValueRep(const CQLValueRep& val)
{
   switch(val._valueType)
   {
      case CQLValue::Boolean_type:
      {
         _theValue._B = val._theValue._B;
         break;
      }
      case CQLValue::Sint64_type: 
      {
         _theValue._S64 = val._theValue._S64;
         break;
      }
      case CQLValue::Uint64_type: 
      {
         _theValue._U64 = val._theValue._U64;
         break;
      }
      case CQLValue::Real_type: 
      {
         _theValue._R64 = val._theValue._R64;
         break;
      }
      case CQLValue::String_type:  
      {
         _theValue._S = new String(*val._theValue._S);
         break;
      }
      case CQLValue::CIMDateTime_type:  
      {
         _theValue._DT = new CIMDateTime(*val._theValue._DT);
         break;
      }
      case CQLValue::CIMReference_type:  
      {
         _theValue._OP = new CIMObjectPath(*val._theValue._OP);
         break;
      }
      case CQLValue::CIMInstance_type:  
      {
         _theValue._IN = new CIMInstance(val._theValue._IN->clone());
         break;
      }
      case CQLValue::CIMClass_type:  
      {
         _theValue._CL = new CIMClass(val._theValue._CL->clone());
         break;
      }
      default:
         break;
   }

   _CQLChainId = val._CQLChainId;

   _isResolved = val._isResolved;

   _valueType = val._valueType;
}

CQLValueRep::CQLValueRep(const CQLValueRep* val)
{
   switch(val->_valueType)
   {
      case CQLValue::Boolean_type:
      {
         _theValue._B = val->_theValue._B;
         break;
      }
      case CQLValue::Sint64_type: 
      {
         _theValue._S64 = val->_theValue._S64;
         break;
      }
      case CQLValue::Uint64_type: 
      {
         _theValue._U64 = val->_theValue._U64;
         break;
      }
      case CQLValue::Real_type: 
      {
         _theValue._R64 = val->_theValue._R64;
         break;
      }
      case CQLValue::String_type:  
      {
         _theValue._S = new String(*val->_theValue._S);
         break;
      }
      case CQLValue::CIMDateTime_type:  
      {
         _theValue._DT = new CIMDateTime(*val->_theValue._DT);
         break;
      }
      case CQLValue::CIMReference_type:  
      {
         _theValue._OP = new CIMObjectPath(*val->_theValue._OP);
         break;
      }
      case CQLValue::CIMInstance_type:  
      {
         _theValue._IN = new CIMInstance(val->_theValue._IN->clone());
         break;
      }
      case CQLValue::CIMClass_type:  
      {
         _theValue._CL = new CIMClass(val->_theValue._CL->clone());
         break;
      }
      default:
         break;
   }

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
            _theValue._U64 = strtoul((const char*)cStr,&endP,16);
            _valueType = CQLValue::Uint64_type;
         }
         else
         {
            _theValue._S64 = strtol((const char *)cStr,&endP,16);
            _valueType = CQLValue::Sint64_type;
         }
         
         break;
      case CQLValue::Binary:
         if(inSign)
         {
            _theValue._U64 = strtoul((const char *)cStr,&endP,2);
            _valueType = CQLValue::Uint64_type;
         }
         else
         {
            _theValue._S64 = strtol((const char *)cStr,&endP,2);
            _valueType = CQLValue::Sint64_type;
         }
         break;
      case CQLValue::Decimal:
         if(inSign)
         {
            _theValue._U64 = strtoul((const char *)cStr,&endP,10);
            _valueType = CQLValue::Uint64_type;
         }
         else
         {
            _theValue._S64 = strtol((const char *)cStr,&endP,10);
            _valueType = CQLValue::Sint64_type;
         }
         break;
      case CQLValue::Real:
         if(inSign)
         {
            _theValue._R64 = strtod((const char *)cStr,&endP);
            _valueType = CQLValue::Real_type;
         }
         else
         {
            _theValue._R64 = strtod((const char *)cStr,&endP);
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
   _theValue._S = new String(inString);
}

CQLValueRep::CQLValueRep(const CIMInstance& inInstance)
  : _isResolved(true),
    _valueType(CQLValue::CIMInstance_type)
{
   _theValue._IN = new CIMInstance(inInstance.clone());
}

CQLValueRep::CQLValueRep(const CIMClass& inClass)
  : _isResolved(true),
    _valueType(CQLValue::CIMClass_type)
{
   _theValue._CL = new CIMClass(inClass.clone());
}

CQLValueRep::CQLValueRep(const CIMObjectPath& inObjPath)
  : _isResolved(true),
    _valueType(CQLValue::CIMReference_type)
{
   _theValue._OP = new CIMObjectPath(inObjPath);
}

CQLValueRep::CQLValueRep(const CIMDateTime& inDateTime)
  : _isResolved(true),
    _valueType(CQLValue::CIMDateTime_type)
{
   _theValue._DT = new CIMDateTime(inDateTime);
}

CQLValueRep::CQLValueRep(Uint64 inUint)
  : _isResolved(true),
    _valueType(CQLValue::Uint64_type)
{
   _theValue._U64 = inUint;
}

CQLValueRep::CQLValueRep(Boolean inBool)
  : _isResolved(true),
    _valueType(CQLValue::Boolean_type)
{
   _theValue._B = inBool;
}

CQLValueRep::CQLValueRep(Sint64 inSint)
  : _isResolved(true),
    _valueType(CQLValue::Sint64_type)
{
   _theValue._S64 = inSint;
}

CQLValueRep::CQLValueRep(Real64 inReal)
  : _isResolved(true),
    _valueType(CQLValue::Real_type)
{
   _theValue._R64 = inReal;
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
      _theValue._IN = (CIMInstance *) new CIMInstance(CI.clone());
      _valueType = CQLValue::CIMInstance_type;
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
      
      if(inQueryCtx.getClassRelation(Idstrings[index].getScope(),classContext)
	 == QueryContext::NOTRELATED)
	{
	  // The chain is not inline with scope.
	  _valueType = CQLValue::Null_type;
	  return;
	}
      
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

      if(cimObj.isInstance())
	{
	  _theValue._IN = new CIMInstance(cimObj.clone());
	}
      else if(cimObj.isClass())
	{
	  _theValue._CL = new CIMClass(cimObj.clone());
	}
 
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

CQLValueRep& CQLValueRep::operator=(const CQLValueRep& rhs){

	if(&rhs != this){
		_valueType = rhs._valueType;
		_theValue = rhs._theValue;
		switch(_valueType){
			case CQLValue::String_type:
				_theValue._S = new String(rhs.getString());
				break;
			case CQLValue::CIMDateTime_type:
         			_theValue._DT = new CIMDateTime(rhs.getDateTime());
         			break;
      			case CQLValue::CIMReference_type:
         			_theValue._OP = new CIMObjectPath(rhs.getReference());
         			break;
      			case CQLValue::CIMInstance_type:
         			_theValue._IN = new CIMInstance(rhs.getInstance().clone());
         			break;
      			case CQLValue::CIMClass_type:
         			_theValue._CL = new CIMClass(rhs.getClass().clone());
         			break;
			case CQLValue::Boolean_type:
      			case CQLValue::Sint64_type:
      			case CQLValue::Uint64_type:
			case CQLValue::Real_type:
			case CQLValue::Null_type:
			case CQLValue::CQLIdentifier_type:

			default:
				break;
		}
		_CQLChainId = rhs._CQLChainId;
		_isResolved = rhs._isResolved;
	}
	return *this;
}
Boolean CQLValueRep::operator==(const CQLValueRep& x)
{
  _validate(x);

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
   case CQLValue::Boolean_type:
     {
       if(x._valueType == CQLValue::Boolean_type)
         {
	   if(_theValue._B == x._theValue._B)
	     {
               return true;
	     }
         }
       break; 
     }     
   case CQLValue::Sint64_type:
     {
       if(x._valueType == CQLValue::Sint64_type)
         {
	   if(_theValue._S64 == x._theValue._S64)
	     {
               return true;
	     }
         }
       else if(x._valueType == CQLValue::Uint64_type)
         {
	   if(x._theValue._U64 > (Uint64)PEGASUS_SINT64_MIN)
	     {
	       return false;
	     }
	   if(_theValue._S64 == (Sint64)x._theValue._U64)
	     {
               return true;
	     }
         }
       else if(x._valueType == CQLValue::Real_type)
         {
	   if(_theValue._S64 == x._theValue._R64)
	     {
               return true;
	     }
         }
       break;
     }
   case CQLValue::Uint64_type:
     if(x._valueType == CQLValue::Sint64_type)
       {
	 if(_theValue._U64 > (Uint64)PEGASUS_SINT64_MIN)
	   {
	     return false;
	   }
	 if((Sint64)_theValue._U64 == x._theValue._S64)
	   {
	     return true;
	   }
       }
     else if(x._valueType == CQLValue::Uint64_type)
       {
	 if(_theValue._U64 == x._theValue._U64)
	   {
	     return true;
	   }
       }
     else if(x._valueType == CQLValue::Real_type)
       {
	 Real64 temp;
	 memcpy(&temp, &_theValue._U64, sizeof(temp));
	 if(temp == x._theValue._R64)
	   {
	     return true;
	   }
       }
     break;
   case CQLValue::Real_type:
     if(x._valueType == CQLValue::Sint64_type)
       {
	 if(_theValue._R64 == x._theValue._S64)
	   {
	     return true;
	   }
       }
     else if(x._valueType == CQLValue::Uint64_type)
       {
	 Real64 temp;
	 memcpy(&temp, &x._theValue._U64, sizeof(temp));
            if(_theValue._R64 == temp)
	      {
		return true;
	      }
       }
     else if(x._valueType == CQLValue::Real_type)
       {
	 if(_theValue._R64 == x._theValue._R64)
            {
	      return true;
            }
       }
     break;
   case CQLValue::String_type:
     if(*_theValue._S == *x._theValue._S)
       {
            return true;
       }
     break;
   case CQLValue::CIMDateTime_type:
     if(*_theValue._DT == *x._theValue._DT)
       {
	 return true;
        }
     break;
   case CQLValue::CIMReference_type:
     if(_theValue._OP->identical(*x._theValue._OP))
       {
	 return true;
       }
     break;
   case CQLValue::CIMInstance_type:  // TODO: need to define Instance comparison
     return true; // _compareInstance(*_theValue._IN, *x._theValue._IN)
     break;
   case CQLValue::CIMClass_type: 
     if(_theValue._CL->identical(*x._theValue._CL))
         {
	   return true;
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

//##ModelId=40FBFF9502BB
Boolean CQLValueRep::operator!=(const CQLValueRep& x)
{
  _validate(x);
 
  return !(this->operator==(x));
}


Boolean CQLValueRep::operator<=(const CQLValueRep& x)
{
  _validate(x);  
   
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
  _validate(x);

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
    case CQLValue::Boolean_type:
      {
	if(x._valueType == CQLValue::Boolean_type)
	  {
	    if(_theValue._B < x._theValue._B)
	      {
		return true;
	      }
	  }
	break; 
      }
    case CQLValue::Sint64_type:
      if(x._valueType == CQLValue::Sint64_type)
	{
	  if(_theValue._S64 < x._theValue._S64)
	    {
	      return true;
	    }
	}
      else if(x._valueType == CQLValue::Uint64_type)
	{
	  if(x._theValue._U64 > (Uint64)PEGASUS_SINT64_MIN)
	     {
	       return true;
	     }
	  if(_theValue._S64 < (Sint64)x._theValue._U64)
	    {
	      return true;
	    }
	}
      else if(x._valueType == CQLValue::Real_type)
	{
	  if(_theValue._S64 < x._theValue._R64)
	    {
	      return true;
	    }
	}
      break;
    case CQLValue::Uint64_type:
      if(x._valueType == CQLValue::Sint64_type)
	{
	  if(_theValue._U64 > (Uint64)PEGASUS_SINT64_MIN)
	    {
	      return false;
	    }
	  if((Sint64)_theValue._U64 < x._theValue._S64)
	    {
	      return true;
	    }
	}
      else if(x._valueType == CQLValue::Uint64_type)
	{
	  if(_theValue._U64 < x._theValue._U64)
	    {
	      return true;
	    }
	}
      else if(x._valueType == CQLValue::Real_type)
	{
	  Real64 temp;
	  memcpy(&temp, &_theValue._U64, sizeof(temp));
	  if(temp < x._theValue._R64)
	    {
	      return true;
	    }
	}
      break;
    case CQLValue::Real_type:
      if(x._valueType == CQLValue::Sint64_type)
	{
	  if(_theValue._R64 < x._theValue._S64)
	    {
	      return true;
	    }
	}
      else if(x._valueType == CQLValue::Uint64_type)
	{
	  Real64 temp;
	  memcpy(&temp, &x._theValue._U64, sizeof(temp));
	  if(_theValue._R64 < temp)
	    {
	      return true;
	    }
	}
      else if(x._valueType == CQLValue::Real_type)
	{
	  if(_theValue._R64 < x._theValue._R64)
	    {
	      return true;
	    }
	}
      break;
    case CQLValue::String_type:
      if(*_theValue._S < *x._theValue._S)
	{
	  return true;
	}
      break;
    case CQLValue::CIMDateTime_type:
      throw(Exception(String("CQLValueRep::operator<")));
      break;
    case CQLValue::CIMReference_type:
      throw(Exception(String("CQLValueRep::operator<")));
      break;
    case CQLValue::CIMInstance_type:
      throw(Exception(String("CQLValueRep::operator<")));
      break;
    case CQLValue::CIMClass_type:
      throw(Exception(String("CQLValueRep::operator<")));
      break;
    case CQLValue::CQLIdentifier_type:
      throw(Exception(String("CQLValueRep::operator<")));
      break;
      
    default:
      throw(Exception(String("CQLValueRep::operator<")));
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
     {
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
     case CQLValue::String_type:
       return CQLValueRep(*_theValue._S + *x._theValue._S);
       break;
     case CQLValue::CIMDateTime_type:
       throw(Exception(String("CQLValueRep::operator+")));
       break;
     case CQLValue::CIMReference_type:
       throw(Exception(String("CQLValueRep::operator+")));
       break;
     case CQLValue::CIMInstance_type:
       throw(Exception(String("CQLValueRep::operator+")));
       break;
     case CQLValue::CIMClass_type:
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
      (_valueType != CQLValue::CIMClass_type &&
       _valueType != CQLValue::CIMInstance_type))
     {
       throw(Exception(String("CQLValueRep::isa")));
     }
   
   CIMName className;

   if(_valueType == CQLValue::CIMInstance_type)
     {
       className = _theValue._IN->getClassName();
     }
   if(_valueType == CQLValue::CIMClass_type)
     {
       className = _theValue._CL->getClassName();
     }

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
   
   String leftside(*_theValue._S);
   String rightside(*inVal._theValue._S);

   // Poughkepsie is doing this, Dan Gorey.
   // return someLikefunction(leftside,rightside);
   return false;
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
   return _theValue._U64;
}

Boolean CQLValueRep::getBool()const
{
   if(_valueType != CQLValue::Boolean_type)
   {
      throw(Exception(String("CQLValueRep::getBool")));
   }
   return _theValue._B;
}

Sint64 CQLValueRep::getSint()const
{
   if(_valueType != CQLValue::Sint64_type)
   {
      throw(Exception(String("CQLValueRep::getSint")));
   }
   return _theValue._S64;
}

Real64 CQLValueRep::getReal()const
{
   if(_valueType != CQLValue::Real_type)
   {
      throw(Exception(String("CQLValueRep::getReal")));
   }
   return _theValue._R64;
}

String CQLValueRep::getString()const
{
   if(_valueType != CQLValue::String_type)
   {
      throw(Exception(String("CQLValueRep::getString")));
   }
   return *_theValue._S;
}

CIMDateTime CQLValueRep::getDateTime()const
{
   if(_valueType != CQLValue::CIMDateTime_type)
   {
      throw(Exception(String("CQLValueRep::getDateTime")));
   }
   
   return *_theValue._DT;
}

CIMObjectPath CQLValueRep::getReference()const
{
   if(_valueType != CQLValue::CIMReference_type)
   {
      throw(Exception(String("CQLValueRep::getReference")));
   }
   return *_theValue._OP;
}

CIMInstance CQLValueRep::getInstance()const
{
   if(_valueType != CQLValue::CIMInstance_type)
   {
      throw(Exception(String("CQLValueRep::getInstance")));
   }
   return _theValue._IN->clone();
}

CIMClass CQLValueRep::getClass()const
{
   if(_valueType != CQLValue::CIMClass_type)
   {
      throw(Exception(String("CQLValueRep::getClass")));
   }

   return _theValue._CL->clone();
}

String CQLValueRep::toString()const
{
   switch(_valueType)
   {
      case CQLValue::Boolean_type:
      {
         return (_theValue._B ? String("TRUE") : String("FALSE"));
         break;
      }
      case CQLValue::Sint64_type: 
      {
         char buffer[32];  // Should need 21 chars max
         sprintf(buffer, "%" PEGASUS_64BIT_CONVERSION_WIDTH "d", _theValue._S64);
         return String(buffer);
         break;
      }
      case CQLValue::Uint64_type: 
      {
         char buffer[32];  // Should need 21 chars max
         sprintf(buffer, "%" PEGASUS_64BIT_CONVERSION_WIDTH "u", _theValue._U64);
         return String(buffer);
         break;
      }
      case CQLValue::Real_type: 
      { 
         char buffer[128];
         sprintf(buffer, "%.6e", _theValue._R64);
         return String(buffer);
         break;
      }
      case CQLValue::String_type:  
         return *_theValue._S;
         break;
      case CQLValue::CIMDateTime_type:  
         return _theValue._DT->toString();
         break;
      case CQLValue::CIMReference_type:  
         return _theValue._OP->toString();
         break;
      case CQLValue::CIMInstance_type:  
         return _theValue._IN->getPath().toString();
         break;
      case CQLValue::CIMClass_type:  
         return _theValue._CL->getPath().toString();
         break;
      case CQLValue::CQLIdentifier_type:
	     return _CQLChainId.toString();
	     break;
      default:
         break;
   }
   return String();
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
      case CQLValue::CIMInstance_type:
         if(x._valueType != CQLValue::CIMInstance_type)
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

void CQLValueRep::_setValue(CIMValue cv,Uint64 index)
{
   CIMValue tmp;
   if(cv.isArray())
   {
      switch(cv.getType())
      {
         case CIMTYPE_BOOLEAN:
         {
            Array<Boolean> _bool;
            cv.get(_bool);
            _theValue._B = _bool[index];
            _valueType = CQLValue::Boolean_type;
            break;
         }
         case CIMTYPE_UINT8:
         {
            Array<Uint8> _uint;
            cv.get(_uint);
            _theValue._U64 = _uint[index];
            _valueType = CQLValue::Uint64_type;
            break;
         }
         case CIMTYPE_UINT16:
         {
            Array<Uint16> _uint;
            cv.get(_uint);
            _theValue._U64 = _uint[index];
            _valueType = CQLValue::Uint64_type;
            break;
         }
         case CIMTYPE_UINT32:
         {
            Array<Uint32> _uint;
            cv.get(_uint);
            _theValue._U64 = _uint[index];
            _valueType = CQLValue::Uint64_type;
            break;
         }
         case CIMTYPE_UINT64:
         {
            Array<Uint64> _uint;
            cv.get(_uint);
            _theValue._U64 = _uint[index];
            _valueType = CQLValue::Uint64_type;
            break;
         }
         case CIMTYPE_SINT8:
         {
            Array<Sint8> _sint;
            cv.get(_sint);
            _theValue._S64 = _sint[index];
            _valueType = CQLValue::Sint64_type;
            break;
         }
         case CIMTYPE_SINT16:
         {
            Array<Sint16> _sint;
            cv.get(_sint);
            _theValue._S64 = _sint[index];
            _valueType = CQLValue::Sint64_type;
            break;
         }
         case CIMTYPE_SINT32:
         {
            Array<Sint32> _sint;
            cv.get(_sint);
            _theValue._S64 = _sint[index];
            _valueType = CQLValue::Sint64_type;
            break;
         }
         case CIMTYPE_SINT64:
         {
            Array<Sint64> _sint;
            cv.get(_sint);
            _theValue._S64 = _sint[index];
            _valueType = CQLValue::Sint64_type;
            break;
         }
           
         case CIMTYPE_REAL32:
         {
            Array<Real32> _real;
            cv.get(_real);
            _theValue._R64 = _real[index];
            _valueType = CQLValue::Real_type;
            break;
         }
         case CIMTYPE_REAL64:
         {
            Array<Real64> _real;
            cv.get(_real);
            _theValue._R64 = _real[index];
            _valueType = CQLValue::Real_type;
            break;
         }   
         case CIMTYPE_CHAR16:
         {
            Array<Char16> _str;
            cv.get(_str);
            _theValue._S = new String(&_str[index]);
            _valueType = CQLValue::String_type;
            break;
         }
         case CIMTYPE_STRING:
         {
            Array<String> _str;
            cv.get(_str);
            _theValue._S = new String(_str[index]);
            _valueType = CQLValue::String_type;
            break;
         }  
         case CIMTYPE_DATETIME:
         {
            Array<CIMDateTime> _date;
            cv.get(_date);
            _theValue._DT = new CIMDateTime(_date[index]);
            _valueType = CQLValue::CIMDateTime_type;
            break;
         }
         case CIMTYPE_REFERENCE:
         {
            Array<CIMObjectPath> _path;
            cv.get(_path);
            _theValue._OP = new CIMObjectPath(_path[index]);
            _valueType = CQLValue::CIMReference_type;
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
            Boolean _bool;
            cv.get(_bool);
            _theValue._B = _bool;
            _valueType = CQLValue::Boolean_type;
            break;
         }
         case CIMTYPE_UINT8:
         {
            Uint8 _uint;
            cv.get(_uint);
            _theValue._U64 = _uint;
            _valueType = CQLValue::Uint64_type;
            break;
         }
         case CIMTYPE_UINT16:
         {
            Uint16 _uint;
            cv.get(_uint);
            _theValue._U64 = _uint;
            _valueType = CQLValue::Uint64_type;
            break;
         }
         case CIMTYPE_UINT32:
         {
            Uint32 _uint;
            cv.get(_uint);
            _theValue._U64 = _uint;
            _valueType = CQLValue::Uint64_type;
            break;
         }
         case CIMTYPE_UINT64:
         {
            Uint64 _uint;
            cv.get(_uint);
            _theValue._U64 = _uint;
            _valueType = CQLValue::Uint64_type;
            break;
         }
         case CIMTYPE_SINT8:
         {
            Sint8 _sint;
            cv.get(_sint);
            _theValue._S64 = _sint;
            _valueType = CQLValue::Sint64_type;
            break;
         }
         case CIMTYPE_SINT16:
         {
            Sint16 _sint;
            cv.get(_sint);
            _theValue._S64 = _sint;
            _valueType = CQLValue::Sint64_type;
            break;
         }
         case CIMTYPE_SINT32:

         {
            Sint32 _sint;
            cv.get(_sint);
            _theValue._S64 = _sint;
            _valueType = CQLValue::Sint64_type;
            break;
         }
         case CIMTYPE_SINT64:
         {
            Sint64 _sint;
            cv.get(_sint);
            _theValue._S64 = _sint;
            _valueType = CQLValue::Sint64_type;
            break;
         }
         case CIMTYPE_REAL32:
         {
            Real32 _real;
            cv.get(_real);
            _theValue._R64 = _real;
            _valueType = CQLValue::Real_type;
            break;
         }
         case CIMTYPE_REAL64:
         {
            Real64 _real;
            cv.get(_real);
            _theValue._R64 = _real;
            _valueType = CQLValue::Real_type;
            break;
         }  
         case CIMTYPE_CHAR16:
         {
            Char16 _str;
            cv.get(_str);
            _theValue._S = new String(&_str);
            _valueType = CQLValue::String_type;
            break;
         }
         case CIMTYPE_STRING:
         {
            String _str;
            cv.get(_str);
            _theValue._S = new String(_str);
            _valueType = CQLValue::String_type;
            break;
         }
         case CIMTYPE_DATETIME:
         {
            CIMDateTime _date;
            cv.get(_date);
            _theValue._DT = new CIMDateTime(_date);
            _valueType = CQLValue::CIMDateTime_type;
            break;
         }
         case CIMTYPE_REFERENCE:
         {
            CIMObjectPath _path;
            cv.get(_path);
            _theValue._OP = new CIMObjectPath(_path);
            _valueType = CQLValue::CIMReference_type;
            break;
         }
         default:
            throw(Exception(String("CQLValueRep::setValue")));
      } // switch statement
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
        _CQLChainId[0].setName(inCid[inCid.size()-1].getName());
        _CQLChainId[0].applyScope(inCid[inCid.size()-1].getScope());
      
	for(Sint32 i = inCid.size()-2; i >= 0; --i)
	  {
	    _CQLChainId.prepend(inCid[i]); 
	  }
	
	CIMInstance temp;
	resolve(temp,_ctx);
     }
   else
     {
       _CQLChainId.applyContext(_ctx); 
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

   QueryClass = inQueryCtx.getClass(className);

   Uint32 propertyIndex = 
         QueryClass.findProperty(lid.getName());

   if(propertyIndex == PEG_NOT_FOUND)
   {
      throw(Exception(String("CQLValueRep::_resolveSymbolicConstant")));
   }

   CIMProperty queryPropObj = QueryClass.getProperty(propertyIndex);

   // We have a symbolic constant (ex. propName#OK)
   // We need to retrieve the ValueMap and Values Qualifiers for 
   // the property if the exist.
   Uint32 qualIndex = queryPropObj.findQualifier(CIMName("ValueMap"));

   if(qualIndex == PEG_NOT_FOUND)
   {
      // This property can not be processed with a symbolic constant.
      throw(Exception(String("CQLValueRep::_resolveSymbolicConstant")));
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
	   throw(Exception(String("CQLValueRep::_resolveSymbolicConstant")));
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
	   throw(Exception(String("CQLValueRep::_resolveSymbolicConstant"))); 
	 }
       
       CString cStr = valueMapArray[matchIndex].getCString();
       char *endP;
       
       // Set Primitive
       _setValue(CIMValue(Uint64(strtoul((const char*)cStr,&endP,10))));
       return;
     }
}

Boolean CQLValueRep::_compareInstance(CIMInstance& _in1, CIMInstance& _in2)
{
  return true;
}

PEGASUS_NAMESPACE_END
