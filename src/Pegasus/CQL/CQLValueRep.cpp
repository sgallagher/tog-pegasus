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


#define CIMTYPE_EMBEDDED 15  //temporary

CQLValueRep::CQLValueRep()
{
   _valueType = Null_type;
	_theValue._S = NULL;
}
CQLValueRep::~CQLValueRep()
{
   switch(_valueType)
   {
      case String_type: 
      { 
         if(_theValue._S != NULL){
            delete _theValue._S;
	}
         _theValue._S = NULL;  
         break;
      }
      case CIMDateTime_type:  
       { 
         if(_theValue._DT != NULL)
            delete _theValue._DT;
         _theValue._DT = NULL;  
         break;
      }
      case CIMReference_type:  
      { 
         if(_theValue._OP != NULL)
            delete _theValue._OP;
         _theValue._OP = NULL;  
         break;
      }
      case CIMInstance_type:  
      { 
         if(_theValue._IN != NULL)
            delete _theValue._IN;
         _theValue._IN = NULL;  
         break;
      }
      case CIMClass_type:  
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

CQLValueRep::CQLValueRep(const CQLValueRep* val)
{
   switch(val->_valueType)
   {
      case Boolean_type:
      {
         _theValue._B = val->_theValue._B;
         break;
      }
      case Sint64_type: 
      {
         _theValue._S64 = val->_theValue._S64;
         break;
      }
      case Uint64_type: 
      {
         _theValue._U64 = val->_theValue._U64;
         break;
      }
      case Real_type: 
      {
         _theValue._R64 = val->_theValue._R64;
         break;
      }
      case String_type:  
      {
         _theValue._S = new String(*val->_theValue._S);
         break;
      }
      case CIMDateTime_type:  
      {
         _theValue._DT = new CIMDateTime(*val->_theValue._DT);
         break;
      }
      case CIMReference_type:  
      {
         _theValue._OP = new CIMObjectPath(*val->_theValue._OP);
         break;
      }
      case CIMInstance_type:  
      {
         _theValue._IN = new CIMInstance(*val->_theValue._IN);
         break;
      }
      case CIMClass_type:  
      {
         _theValue._CL = new CIMClass(*val->_theValue._CL);
         break;
      }
      default:
         break;
   }

      _CQLChainId = val->_CQLChainId;

    _isResolved = val->_isResolved;

    Num_Type = val->Num_Type;

   _valueType = val->_valueType;
}

CQLValueRep::CQLValueRep(String inString, NumericType inValueType, Boolean inSign)
{
   CString cStr = inString.getCString();
   char *endP;

   switch(inValueType)
   {
      case Hex:
         if(inSign)
         {
            _theValue._U64 = strtoul((const char*)cStr,&endP,16);
            _valueType = Uint64_type;
         }
         else
         {
            _theValue._S64 = strtol((const char *)cStr,&endP,16);
            _valueType = Sint64_type;
         }
         
         break;
      case Binary:
         if(inSign)
         {
            _theValue._U64 = strtoul((const char *)cStr,&endP,2);
            _valueType = Uint64_type;
         }
         else
         {
            _theValue._S64 = strtol((const char *)cStr,&endP,2);
            _valueType = Sint64_type;
         }
         break;
      case Decimal:
         if(inSign)
         {
            _theValue._U64 = strtoul((const char *)cStr,&endP,10);
            _valueType = Uint64_type;
         }
         else
         {
            _theValue._S64 = strtol((const char *)cStr,&endP,10);
            _valueType = Sint64_type;
         }
         break;
      case Real:
         if(inSign)
         {
            _theValue._R64 = strtod((const char *)cStr,&endP);
            _valueType = Real_type;
         }
         else
         {
            _theValue._R64 = strtod((const char *)cStr,&endP);
            _valueType = Real_type;
         }
         break;
      default:
         throw(1);
         break;
   }
   _isResolved = true;
}


CQLValueRep::CQLValueRep(CQLChainedIdentifier inCQLIdent)
{
   _CQLChainId = inCQLIdent; 
   _valueType = CQLIdentifier_type;
   _isResolved = false;
}


CQLValueRep::CQLValueRep(String inString)
{
   _theValue._S = new String(inString);
   _valueType = String_type;
   _isResolved = true;
}

CQLValueRep::CQLValueRep(CIMInstance inInstance)
{
   _theValue._IN = new CIMInstance(inInstance);
   _valueType = CIMInstance_type;
   _isResolved = true;
}

CQLValueRep::CQLValueRep(CIMClass inClass)
{
   _theValue._CL = new CIMClass(inClass);
   _valueType = CIMClass_type;
   _isResolved = true;
}

CQLValueRep::CQLValueRep(CIMObjectPath inObjPath)
{
   _theValue._OP = new CIMObjectPath(inObjPath);
   _valueType = CIMReference_type;
   _isResolved = true;
}

CQLValueRep::CQLValueRep(CIMDateTime inDateTime)
{
   _theValue._DT = new CIMDateTime(inDateTime);
   _valueType = CIMDateTime_type;
   _isResolved = true;
}

CQLValueRep::CQLValueRep(Uint64 inUint)
{
   _theValue._U64 = inUint;
   _valueType = Uint64_type;
   _isResolved = true;
}

CQLValueRep::CQLValueRep(Boolean inBool)
{
   _theValue._B = inBool;
   _valueType = Boolean_type;
   _isResolved = true;
}

CQLValueRep::CQLValueRep(Sint64 inSint)
{
   _theValue._S64 = inSint;
   _valueType = Sint64_type;
   _isResolved = true;
}

CQLValueRep::CQLValueRep(Real64 inReal)
{
   _theValue._R64 = inReal;
   _valueType = Real_type;
   _isResolved = true;
}

void CQLValueRep::resolve(CIMInstance CI, QueryContext& inQueryCtx)
{   
   if(_isResolved)
   {
      return;
   }

   CQLIdentifier classNameID;       // Determine if we have Alias/Class/Property 
   String className;                // Alias/Class Name
   Array<CQLIdentifier> Idstrings = 
      _CQLChainId.getSubIdentifiers(); // Array of Identifiers to process

   Uint32 index = 0;                // Counter for looping through Identifiers
   
   CIMClass ScopeClass;             // CIMClass for the scope of Identifier
   CIMClass QueryClass;             // CIMClass for the current query

   CIMProperty propObj;             // Property object to be processed
   CIMProperty queryPropObj;        // Property object used for verification

   Uint32 qualIndex;                // Counter for looping through qualifiers
   CIMValue valueMap;               // CIMValue for Value Map Qualifiers
   CIMValue values;                 // CIMValue for Values Qualifiers
   Boolean matchFound = false;      // Indicator for match Qualifier
   Uint32 matchIndex;               // Placeholder for matched Qualifier
   Array<String> valueMapArray;     // Value Map Qualifier for property
   Array<String> valuesArray;       // Values Qualifier for property

   Boolean isEmbedded = false;      // Embedded indicator

   // We need to determine if the first Identifier is an Alias, Class, or Property.
   // If it is a Alias or a Class the CQLIdentifier return will be the
   // Class name. If the first Identifier is a property, the return CQLIdentifier
   // will be empty.

   classNameID = inQueryCtx.findClass(Idstrings[index].getName().getString());

   className = classNameID.getName().getString();

   if(className.size() == 0)
   { 
      // classname is an empty string, the first Identifier must be a property.  
      // A class is needed to proceed. We will get a class from the FromList.
      // NOTE: for basic CQL support only one class will be in the FromList.
      
      Array<CQLIdentifier> classList;

      classList = inQueryCtx.getFromList();

      if(classList.size() != 1)
      {
         throw(1);
      }

      className = classList[0].getName().getString();   
   
   }
   else if(Idstrings.size() == 1)
   {
      // A class was passed in with no property indicated.
      // Set the instance passed in, as a primitive.
      _theValue._IN = (CIMInstance *) new CIMInstance(CI);
      _valueType = CIMInstance_type;
      _isResolved = true;
      return; // Done.
   }
   else
   {
      // Need to increment index since the first Identifier is a class,
      // and additional identifiers need processing.
      ++index;
   }

   // Symbolic Constant processing
   if(_CQLChainId.getLastIdentifier().isSymbolicConstant())
     {
      _resolveSymbolicConstant(inQueryCtx);
     return;
   }
   
   // Now we will loop through the remaining CQLIdentifiers, 
   // and process each one.
   for(;index < Idstrings.size(); ++index)
   {
      // We will get the current class from the repository.
      // We must do this for each loop since the class being 
      // processed may change with each iteration.
      QueryClass = inQueryCtx.getClass(className);

      // We need to do special processing on property if the 
      // property is scoped. 
// TODO: We should be able to delete this code.
/*
      if(Idstrings[index].isScoped())
      {
         // This property is scoped. 
         // We need to get the scoped class from the repository; to verify
         // that the property is in the scope and the scope and QueryClass
         // have a parent child relationship.
         ScopeClass = inQueryCtx.getClass(Idstrings[index].getScope());

         // Verifing property is in Scope.
         if(ScopeClass.findProperty(Idstrings[index].getName()) == 
            PEG_NOT_FOUND)
         {
            // Set the Ignore_type
            _valueType = CQLIgnore_type;
            _isResolved = true;
            return;
         }
         // Verifying the QueryClass and the ScopeClass have a
         // parent child relationship
         else if(!_areClassesInline(ScopeClass,QueryClass,inQueryCtx))
         {
            throw(1);
         }

      }
      // Verifing that embedded properties are scoped.
      else if(isEmbedded)
      {  // all embedded properties must be scoped.

         throw(1);
      }
*/
      // This is a short cut for wildcard special charactor.
      // Since no further processing is necessary for this case.
      if(Idstrings[index].isWildcard())
      {  
         _theValue._IN = new CIMInstance(CI);
         _valueType = CIMInstance_type;
         _isResolved = true;
         return;
      }

      // Now we need to verify that the property is in the class.
      Uint32 propertyIndex = QueryClass.findProperty(Idstrings[index].getName());

      if(propertyIndex == PEG_NOT_FOUND)
      {
         throw(1);
      }

      // We will check the property type to determine what processing 
      // needs to be done.
      queryPropObj = QueryClass.getProperty(propertyIndex);

      if(queryPropObj.getType() == CIMTYPE_EMBEDDED)
      {
         // Do embedded code here.
         
         isEmbedded = true;
         continue;
      }
      else // Primitive
      {
         // We will retrieve the property from the instance,
         // that will be used to set the primitive later in processing.
         propertyIndex = CI.findProperty(Idstrings[index].getName());
         propObj = CI.getProperty(propertyIndex);

         if(Idstrings[index].isArray())
         {
            // We have an array property.  All we need to do
            // Is get the index defined by CQLIdentifier.
            // NOTE: Basic CQL support only allows one index.
            _setValue(propObj.getValue(),
                     Idstrings[index].getSubRanges()[0].start);
            return;
         }
         else
         {
            // The property has no special charactors.
            CIMValue cimVal = propObj.getValue();
            _setValue(cimVal);
            return;
         }
      } // else body
   } // loop
} // end of function

CQLValueRep& CQLValueRep::operator=(const CQLValueRep& rhs){

	if(&rhs != this){
		_valueType = rhs._valueType;
		_theValue = rhs._theValue;
		switch(_valueType){
			case String_type:
				_theValue._S = new String(rhs.getString());
				break;
			case CIMDateTime_type:
         			_theValue._DT = new CIMDateTime(rhs.getDateTime());
         			break;
      			case CIMReference_type:
         			_theValue._OP = new CIMObjectPath(rhs.getReference());
         			break;
      			case CIMInstance_type:
         			_theValue._IN = new CIMInstance(rhs.getInstance());
         			break;
      			case CIMClass_type:
         			_theValue._CL = new CIMClass(rhs.getClass());
         			break;
			case Boolean_type:
      			case Sint64_type:
      			case Uint64_type:
			case Real_type:
			case Null_type:
			case CQLIdentifier_type:
			case CQLIgnore_type:
			default:
				break;
		}
		_CQLChainId = rhs._CQLChainId;
		_isResolved = rhs._isResolved;
		Num_Type = rhs.Num_Type;
	}
	return *this;
}
Boolean CQLValueRep::operator==(const CQLValueRep& x)
{
   if(!_validate(x))
   {
      throw(1);
   } 

   if(x._valueType == Null_type ||
      _valueType == Null_type)
   {
      throw(1);
   }
 
   switch(_valueType)
   {
      case Boolean_type:
      {
         if(x._valueType == Boolean_type)
         {
            if(_theValue._B == x._theValue._B)
            {
               return true;
            }
         }
         break; 
      }     
      case Sint64_type:
      {
         if(x._valueType == Sint64_type)
         {
            if(_theValue._S64 == x._theValue._S64)
            {
               return true;
            }
         }
         else if(x._valueType == Uint64_type)
         {
            if(_theValue._S64 == (Sint64)x._theValue._U64)
            {
               return true;
            }
         }
         else if(x._valueType == Real_type)
         {
            if(_theValue._S64 == x._theValue._R64)
            {
               return true;
            }
         }
         break;
      }
      case Uint64_type:
         if(x._valueType == Sint64_type)
         {
            if((Sint64)_theValue._U64 == x._theValue._S64)
            {
               return true;
            }
         }
         else if(x._valueType == Uint64_type)
         {
            if(_theValue._U64 == x._theValue._U64)
            {
               return true;
            }
         }
         else if(x._valueType == Real_type)
         {
            if(_theValue._U64 == x._theValue._R64)
            {
               return true;
            }
         }
         break;
      case Real_type:
         if(x._valueType == Sint64_type)
         {
            if(_theValue._R64 == x._theValue._S64)
            {
               return true;
            }
         }
         else if(x._valueType == Uint64_type)
         {
            if(_theValue._R64 == x._theValue._U64)
            {
               return true;
            }
         }
         else if(x._valueType == Real_type)
         {
            if(_theValue._R64 == x._theValue._R64)
            {
               return true;
            }
         }
         break;
      case String_type:
         if(*_theValue._S == *x._theValue._S)
         {
            return true;
         }
         break;
      case CIMDateTime_type:
         if(*_theValue._DT == *x._theValue._DT)
         {
            return true;
         }
         break;
         case CIMReference_type:
         if(*_theValue._OP == *x._theValue._OP)
         {
            return true;
         }
         break;
         case CIMInstance_type:
         if((*_theValue._IN).getPath() == (*x._theValue._IN).getPath())
         {
            return true;
         }   
         break;
         case CQLIdentifier_type:
            throw(1);
         break;

      default:
         throw(1);
         break;
   }
   return false;
}

//##ModelId=40FBFF9502BB
Boolean CQLValueRep::operator!=(const CQLValueRep& x)
{
   if(!_validate(x))
   {
      throw(1);
   }  
 
   return !(this->operator==(x));
}


Boolean CQLValueRep::operator<=(const CQLValueRep& x)
{
   if(!_validate(x))
   {
      throw(1);
   }  

   if(x._valueType == Null_type ||
      _valueType == Null_type)
   {
      throw(1);
   }
 
   switch(_valueType)
   {
      case Boolean_type:
      {
         if(x._valueType == Boolean_type)
         {
            if(_theValue._B <= x._theValue._B)
            {
               return true;
            }
         }
         break; 
      }
      case Sint64_type:
         if(x._valueType == Sint64_type)
         {
            if(_theValue._S64 <= x._theValue._S64)
            {
               return true;
            }
         }
         else if(x._valueType == Uint64_type)
         {
            if(_theValue._S64 <= (Sint64)x._theValue._U64)
            {
               return true;
            }
         }
         else if(x._valueType == Real_type)
         {
            if(_theValue._S64 <= x._theValue._R64)
            {
               return true;
            }
         }
         break;
      case Uint64_type:
         if(x._valueType == Sint64_type)
         {
            if((Sint64)_theValue._U64 <= x._theValue._S64)
            {
               return true;
            }
         }
         else if(x._valueType == Uint64_type)
         {
            if(_theValue._U64 <= x._theValue._U64)
            {
               return true;
            }
         }
         else if(x._valueType == Real_type)
         {
            if(_theValue._U64 <= x._theValue._R64)
            {
               return true;
            }
         }
         break;
      case Real_type:
         if(x._valueType == Sint64_type)
         {
            if(_theValue._R64 <= x._theValue._S64)
            {
               return true;
            }
         }
         else if(x._valueType == Uint64_type)
         {
            if(_theValue._R64 <= x._theValue._U64)
            {
               return true;
            }
         }
         else if(x._valueType == Real_type)
         {
            if(_theValue._R64 <= x._theValue._R64)
            {
               return true;
            }
         }
         break;
      case String_type:
         if(*_theValue._S <= *x._theValue._S)
         {
            return true;
         }
         break;
      case CIMDateTime_type:
         throw(1);
         break;
      case CIMReference_type:
         throw(1);
         break;
      case CIMInstance_type:
            throw(1);
         break;
      case CQLIdentifier_type:
            throw(1);
         break;

      default:
         throw(1);
         break;
   }
   return false;
}


Boolean CQLValueRep::operator>=(const CQLValueRep& x)
{
   if(!_validate(x))
   {
      throw(1);
   }  
 
   if(x._valueType == Null_type ||
      _valueType == Null_type)
   {
      throw(1);
   }

   switch(_valueType)
   {
      case Boolean_type:
      {
         if(x._valueType == Boolean_type)
         {
            if(_theValue._B >= x._theValue._B)
            {
               return true;
            }
         }
         break; 
      }
      case Sint64_type:
         if(x._valueType == Sint64_type)
         {
            if(_theValue._S64 >= x._theValue._S64)
            {
               return true;
            }
         }
         else if(x._valueType == Uint64_type)
         {
            if(_theValue._S64 >= (Sint64)x._theValue._U64)
            {
               return true;
            }
         }
         else if(x._valueType == Real_type)
         {
            if(_theValue._S64 >= x._theValue._R64)
            {
               return true;
            }
         }
         break;
      case Uint64_type:
         if(x._valueType == Sint64_type)
         {
            if((Sint64)_theValue._U64 >= x._theValue._S64)
            {
               return true;
            }
         }
         else if(x._valueType == Uint64_type)
         {
            if(_theValue._U64 >= x._theValue._U64)
            {
               return true;
            }
         }
         else if(x._valueType == Real_type)
         {
            if(_theValue._U64 >= x._theValue._R64)
            {
               return true;
            }
         }
         break;
      case Real_type:
         if(x._valueType == Sint64_type)
         {
            if(_theValue._R64 >= x._theValue._S64)
            {
               return true;
            }
         }
         else if(x._valueType == Uint64_type)
         {
            if(_theValue._R64 >= x._theValue._U64)
            {
               return true;
            }
         }
         else if(x._valueType == Real_type)
         {
            if(_theValue._R64 >= x._theValue._R64)
            {
               return true;
            }
         }
         break;
      case String_type:
         if(*_theValue._S >= *x._theValue._S)
         {
            return true;
         }
         break;
      case CIMDateTime_type:
         throw(1);
         break;
      case CIMReference_type:
         throw(1);
         break;
      case CIMInstance_type:
            throw(1);
         break;
      case CQLIdentifier_type:
            throw(1);
         break;

      default:
         throw(1);
         break;
   }
   return false;
}


Boolean CQLValueRep::operator<(const CQLValueRep& x)
{
if(!_validate(x))
   {
      throw(1);
   }  
 
   if(x._valueType == Null_type ||
      _valueType == Null_type)
   {
      throw(1);
   }

   switch(_valueType)
   {
      case Boolean_type:
      {
         if(x._valueType == Boolean_type)
         {
            if(_theValue._B < x._theValue._B)
            {
               return true;
            }
         }
         break; 
      }
      case Sint64_type:
         if(x._valueType == Sint64_type)
         {
            if(_theValue._S64 < x._theValue._S64)
            {
               return true;
            }
         }
         else if(x._valueType == Uint64_type)
         {
            if(_theValue._S64 < (Sint64)x._theValue._U64)
            {
               return true;
            }
         }
         else if(x._valueType == Real_type)
         {
            if(_theValue._S64 < x._theValue._R64)
            {
               return true;
            }
         }
         break;
      case Uint64_type:
         if(x._valueType == Sint64_type)
         {
            if((Sint64)_theValue._U64 < x._theValue._S64)
            {
               return true;
            }
         }
         else if(x._valueType == Uint64_type)
         {
            if(_theValue._U64 < x._theValue._U64)
            {
               return true;
            }
         }
         else if(x._valueType == Real_type)
         {
            if(_theValue._U64 < x._theValue._R64)
            {
               return true;
            }
         }
         break;
      case Real_type:
         if(x._valueType == Sint64_type)
         {
            if(_theValue._R64 < x._theValue._S64)
            {
               return true;
            }
         }
         else if(x._valueType == Uint64_type)
         {
            if(_theValue._R64 < x._theValue._U64)
            {
               return true;
            }
         }
         else if(x._valueType == Real_type)
         {
            if(_theValue._R64 < x._theValue._R64)
            {
               return true;
            }
         }
         break;
      case String_type:
         if(*_theValue._S < *x._theValue._S)
         {
            return true;
         }
         break;
      case CIMDateTime_type:
         throw(1);
         break;
      case CIMReference_type:
         throw(1);
         break;
      case CIMInstance_type:
            throw(1);
         break;
      case CQLIdentifier_type:
            throw(1);
         break;

      default:
         throw(1);
         break;
   }
   return false;
}


Boolean CQLValueRep::operator>(const CQLValueRep& x)
{
   if(!_validate(x))
   {
      throw(1);
   }  
 
   if(x._valueType == Null_type ||
      _valueType == Null_type)
   {
      throw(1);
   }

   switch(_valueType)
   {
      case Boolean_type:
      {
         if(x._valueType == Boolean_type)
         {
            if(_theValue._B > x._theValue._B)
            {
               return true;
            }
         }
         break; 
      }
      case Sint64_type:
         if(x._valueType == Sint64_type)
         {
            if(_theValue._S64 > x._theValue._S64)
            {
               return true;
            }
         }
         else if(x._valueType == Uint64_type)
         {
            if(_theValue._S64 > (Sint64)x._theValue._U64)
            {
               return true;
            }
         }
         else if(x._valueType == Real_type)
         {
            if(_theValue._S64 > x._theValue._R64)
            {
               return true;
            }
         }
         break;
      case Uint64_type:
         if(x._valueType == Sint64_type)
         {
            if((Sint64)_theValue._U64 > x._theValue._S64)
            {
               return true;
            }
         }
         else if(x._valueType == Uint64_type)
         {
            if(_theValue._U64 > x._theValue._U64)
            {
               return true;
            }
         }
         else if(x._valueType == Real_type)
         {
            if(_theValue._U64 > x._theValue._R64)
            {
               return true;
            }
         }
         break;
      case Real_type:
         if(x._valueType == Sint64_type)
         {
            if(_theValue._R64 > x._theValue._S64)
            {
               return true;
            }
         }
         else if(x._valueType == Uint64_type)
         {
            if(_theValue._R64 > x._theValue._U64)
            {
               return true;
            }
         }
         else if(x._valueType == Real_type)
         {
            if(_theValue._R64 > x._theValue._R64)
            {
               return true;
            }
         }
         break;
      case String_type:
         if(*_theValue._S > *x._theValue._S)
         {
            return true;
         }
         break;
      case CIMDateTime_type:
         throw(1);
         break;
      case CIMReference_type:
         throw(1);
         break;
      case CIMInstance_type:
            throw(1);
         break;
      case CQLIdentifier_type:
            throw(1);
         break;

      default:
         throw(1);
         break;
   }
   return false;
}


CQLValueRep CQLValueRep::operator+(const CQLValueRep x)
{
   
   if(!_validate(x))  
   {
      throw(1);
   } 

   if(x._valueType == Null_type ||
      _valueType == Null_type)
   {
      return CQLValueRep();
   }

   switch(_valueType)
   {
      case Boolean_type:
         throw(1);
         break;
      case Sint64_type:
         if(x._valueType == Sint64_type)
         {
            return CQLValueRep(_theValue._S64 + x._theValue._S64);        
         }
         else if(x._valueType == Uint64_type)
         {
            return CQLValueRep(_theValue._S64 + x._theValue._U64);
         }
         else if(x._valueType == Real_type)
         {
            return CQLValueRep(_theValue._S64 + x._theValue._R64);
         }
         break;
      case Uint64_type:
         if(x._valueType == Sint64_type)
         {
            return CQLValueRep(_theValue._U64 + x._theValue._S64);
         }
         else if(x._valueType == Uint64_type)
         {
            return CQLValueRep(_theValue._U64 + x._theValue._U64);
         }
         else if(x._valueType == Real_type)
         {
            return CQLValueRep(_theValue._U64 + x._theValue._R64);
         }
         break;
      case Real_type:
         if(x._valueType == Sint64_type)
         {
            return CQLValueRep(_theValue._R64 + x._theValue._S64);
         }
         else if(x._valueType == Uint64_type)
         {
            return CQLValueRep(_theValue._R64 + x._theValue._U64);
         }
         else if(x._valueType == Real_type)
         {
            return CQLValueRep(_theValue._R64 + x._theValue._R64);
         }
         break;
      case String_type:
         return CQLValueRep(*_theValue._S + *x._theValue._S);
         break;
      case CIMDateTime_type:
         throw(1);
         break;
      case CIMReference_type:
         throw(1);
         break;
      case CIMInstance_type:
            throw(1);
         break;
      case CQLIdentifier_type:
            throw(1);
         break;

      default:
         throw(1);
         break;
   }

   // control should never reach here
   return x;
}


CQLValueRep CQLValueRep::operator-(const CQLValueRep& x)
{
   if(!_validate(x))  
   {
      throw(1);
   } 
   if(x._valueType == Null_type ||
      _valueType == Null_type)
   {
      return CQLValueRep();
   }
    
   switch(_valueType)
   {
      case Boolean_type:
         throw(1);
         break;
      case Sint64_type:
         if(x._valueType == Sint64_type)
         {
            return CQLValueRep(_theValue._S64 - x._theValue._S64);        
         }
         else if(x._valueType == Uint64_type)
         {
            return CQLValueRep(_theValue._S64 - x._theValue._U64);
         }
         else if(x._valueType == Real_type)
         {
            return CQLValueRep(_theValue._S64 - x._theValue._R64);
         }
         break;
      case Uint64_type:
         if(x._valueType == Sint64_type)
         {
            return CQLValueRep(_theValue._U64 - x._theValue._S64);
         }
         else if(x._valueType == Uint64_type)
         {
            return CQLValueRep(_theValue._U64 - x._theValue._U64);
         }
         else if(x._valueType == Real_type)
         {
            return CQLValueRep(_theValue._U64 - x._theValue._R64);
         }
         break;
      case Real_type:
         if(x._valueType == Sint64_type)
         {
            return CQLValueRep(_theValue._R64 - x._theValue._S64);
         }
         else if(x._valueType == Uint64_type)
         {
            return CQLValueRep(_theValue._R64 - x._theValue._U64);
         }
         else if(x._valueType == Real_type)
         {
            return CQLValueRep(_theValue._R64 - x._theValue._R64);
         }
         break;
      case String_type:
         throw(1);
         break;
      case CIMDateTime_type:
         throw(1);
         break;
      case CIMReference_type:
         throw(1);
         break;
      case CIMInstance_type:
            throw(1);
         break;
      case CQLIdentifier_type:
            throw(1);
         break;

      default:
         throw(1);
         break;
   }
   // control should never reach here
   return x;
}


CQLValueRep CQLValueRep::operator*(const CQLValueRep& x)
{

   if(!_validate(x))  
   {
      throw(1);
   } 

   if(x._valueType == Null_type ||
      _valueType == Null_type)
   {
      return CQLValueRep();
   }
    
   switch(_valueType)
   {
      case Boolean_type:
         throw(1);
         break;
      case Sint64_type:
         if(x._valueType == Sint64_type)
         {
            return CQLValueRep(_theValue._S64 * x._theValue._S64);        
         }
         else if(x._valueType == Uint64_type)
         {
            return CQLValueRep(_theValue._S64 * x._theValue._U64);
         }
         else if(x._valueType == Real_type)
         {
            return CQLValueRep(_theValue._S64 * x._theValue._R64);
         }
         break;
      case Uint64_type:
         if(x._valueType == Sint64_type)
         {
            return CQLValueRep(_theValue._U64 * x._theValue._S64);
         }
         else if(x._valueType == Uint64_type)
         {
            return CQLValueRep(_theValue._U64 * x._theValue._U64);
         }
         else if(x._valueType == Real_type)
         {
            return CQLValueRep(_theValue._U64 * x._theValue._R64);
         }
         break;
      case Real_type:
         if(x._valueType == Sint64_type)
         {
            return CQLValueRep(_theValue._R64 * x._theValue._S64);
         }
         else if(x._valueType == Uint64_type)
         {
            return CQLValueRep(_theValue._R64 * x._theValue._U64);
         }
         else if(x._valueType == Real_type)
         {
            return CQLValueRep(_theValue._R64 * x._theValue._R64);
         }
         break;
      case String_type:
         throw(1);
         break;
      case CIMDateTime_type:
         throw(1);
         break;
      case CIMReference_type:
         throw(1);
         break;
      case CIMInstance_type:
            throw(1);
         break;
      case CQLIdentifier_type:
            throw(1);
         break;

      default:
         throw(1);
         break;
   }
   // control should never reach here
   return x;
}


CQLValueRep CQLValueRep::operator/(const CQLValueRep& x)
{
   if(!_validate(x) || 
      x._theValue._U64 == 0)  
   {
      throw(1);
   } 
   
   if(x._valueType == Null_type ||
      _valueType == Null_type)
   {
      return CQLValueRep();
   }
 
   switch(_valueType)
   {
      case Boolean_type:
         throw(1);
         break;
      case Sint64_type:
         if(x._valueType == Sint64_type)
         {
            return CQLValueRep(_theValue._S64 / x._theValue._S64);        
         }
         else if(x._valueType == Uint64_type)
         {
            return CQLValueRep(_theValue._S64 / x._theValue._U64);
         }
         else if(x._valueType == Real_type)
         {
            return CQLValueRep(_theValue._S64 / x._theValue._R64);
         }
         break;
      case Uint64_type:
         if(x._valueType == Sint64_type)
         {
            return CQLValueRep(_theValue._U64 / x._theValue._S64);
         }
         else if(x._valueType == Uint64_type)
         {
            return CQLValueRep(_theValue._U64 / x._theValue._U64);
         }
         else if(x._valueType == Real_type)
         {
            return CQLValueRep(_theValue._U64 / x._theValue._R64);
         }
         break;
      case Real_type:
         if(x._valueType == Sint64_type)
         {
            return CQLValueRep(_theValue._R64 / x._theValue._S64);
         }
         else if(x._valueType == Uint64_type)
         {
            return CQLValueRep(_theValue._R64 / x._theValue._U64);
         }
         else if(x._valueType == Real_type)
         {
            return CQLValueRep(_theValue._R64 / x._theValue._R64);
         }
         break;
      case String_type:
         throw(1);
         break;
      case CIMDateTime_type:
         throw(1);
         break;
      case CIMReference_type:
         throw(1);
         break;
      case CIMInstance_type:
            throw(1);
         break;
      case CQLIdentifier_type:
            throw(1);
         break;

      default:
         throw(1);
         break;
   }
   // control should never reach here
   return x;
}

//##ModelId=40FC3F6F0302
CQLValueType CQLValueRep::getValueType()
{
   return _valueType;
}


void CQLValueRep::setNull()
{
   _valueType = Null_type;
   _isResolved = true;
}


Boolean CQLValueRep::isResolved()
{
   return _isResolved;
}


Boolean CQLValueRep::isNull()
{
   if(_valueType == Null_type)
   {
      return true;
   }
   return false;
}


Boolean CQLValueRep::isa(const CQLValueRep& inVal, QueryContext& QueryCtx)
{
   if(!_isResolved || 
      !inVal._isResolved ||
      _valueType != CIMInstance_type ||
      inVal._valueType != String_type)
   {
      throw(1);
   }
   
   CIMName  className;
   CIMClass classObj;

   className = this->_theValue._IN->getClassName();

   while(!className.isNull())
   {
      if(className == CIMName(*inVal._theValue._S))
      {
         return true;
      }

      classObj = QueryCtx.getClass(className);
      className = classObj.getSuperClassName();
   }

   return false;
}


Boolean CQLValueRep::like(const CQLValueRep& inVal)
{
   if( _valueType != String_type ||
      inVal._valueType != String_type)
   {
      throw(1);
   }

   // Poughkepsie is doing this, Dan Gorey.
   return false;
}

void CQLValueRep::invert()
{
   switch(_valueType)
   {
      case Sint64_type:
         _theValue._S64 = -1 * _theValue._S64;
         break;
      case Real_type:
         _theValue._R64 = -1 * _theValue._R64;
         break;
      case Boolean_type:
         _theValue._B = !_theValue._B;
         break;
      default:
         break;
   }
}

CQLChainedIdentifier CQLValueRep::getChainedIdentifier()const
{
   return _CQLChainId;
}

Uint64 CQLValueRep::getUint()const
{
   if(_valueType != Uint64_type)
   {
      throw(1);
   }
   return _theValue._U64;
}

Boolean CQLValueRep::getBool()const
{
   if(_valueType != Boolean_type)
   {
      throw(1);
   }
   return _theValue._B;
}

Sint64 CQLValueRep::getSint()const
{
   if(_valueType != Sint64_type)
   {
      throw(1);
   }
   return _theValue._S64;
}

Real64 CQLValueRep::getReal()const
{
   if(_valueType != Real_type)
   {
      throw(1);
   }
   return _theValue._R64;
}

String CQLValueRep::getString()const
{
   if(_valueType != String_type)
   {
      throw(1);
   }
   return *_theValue._S;
}

CIMDateTime CQLValueRep::getDateTime()const
{
   if(_valueType != CIMDateTime_type)
   {
      throw(1);
   }
   
   return *_theValue._DT;
}

CIMObjectPath CQLValueRep::getReference()const
{
   if(_valueType != CIMReference_type)
   {
      throw(1);
   }
   return *_theValue._OP;
}

CIMInstance CQLValueRep::getInstance()const
{
   if(_valueType != CIMInstance_type)
   {
      throw(1);
   }
   return *_theValue._IN;
}

CIMClass CQLValueRep::getClass()const
{
   if(_valueType != CIMClass_type)
   {
      throw(1);
   }

   return *_theValue._CL;
}

String CQLValueRep::toString()const
{
   switch(_valueType)
   {
      case Boolean_type:
      {
         return (_theValue._B ? String("TRUE") : String("FALSE"));
         break;
      }
      case Sint64_type: 
      {
         char buffer[32];  // Should need 21 chars max
         sprintf(buffer, "%" PEGASUS_64BIT_CONVERSION_WIDTH "d", _theValue._S64);
         return String(buffer);
         break;
      }
      case Uint64_type: 
      {
         char buffer[32];  // Should need 21 chars max
         sprintf(buffer, "%" PEGASUS_64BIT_CONVERSION_WIDTH "u", _theValue._U64);
         return String(buffer);
         break;
      }
      case Real_type: 
      { 
         char buffer[128];
         sprintf(buffer, "%.6e", _theValue._R64);
         return String(buffer);
         break;
      }
      case String_type:  
         return *_theValue._S;
         break;
      case CIMDateTime_type:  
         return _theValue._DT->toString();
         break;
      case CIMReference_type:  
         return _theValue._OP->toString();
         break;
      case CIMInstance_type:  
         return _theValue._IN->getPath().toString();
         break;
      case CIMClass_type:  
         return _theValue._CL->getPath().toString();
         break;
      case CQLIdentifier_type:
	     return _CQLChainId.toString();
	     break;
      default:
         break;
   }
   return String();
}



Boolean CQLValueRep::_validate(const CQLValueRep& x)
{
   switch(_valueType)
   {
      case Boolean_type:
         if(x._valueType != Boolean_type)
         {
            return false;
         }
         break;
      case Sint64_type:
      case Uint64_type:
      case Real_type:
         if(x._valueType != Sint64_type &&
            x._valueType != Uint64_type &&
            x._valueType != Real_type)
         {
            return false;
         }
         break;
      case String_type:
         if(x._valueType != String_type)
         {
            return false;
         }
         break;
      case CIMDateTime_type:
         if(x._valueType != CIMDateTime_type)
         {
            return false;
         }
         break;
      case CIMReference_type:
         if(x._valueType != CIMReference_type)
         {
            return false;
         }
         break;
      case CIMInstance_type:
         if(x._valueType != CIMInstance_type)
         {
            return false;
         }
         break;
      case CQLIdentifier_type:
         if(x._valueType != CQLIdentifier_type)
         {
            return false;
         }
         break;

      default:
         throw(1);
         break;
   }
   return true;
}

Boolean CQLValueRep::_areClassesInline(CIMClass c1,CIMClass c2,QueryContext& QC)
{
   CIMName superClass;
   CIMName prevClass;

   superClass = c1.getClassName();
   while(!(superClass == prevClass))
   { 
      prevClass = superClass;
      if(superClass == c2.getClassName())
      {
         return true;
      }
      superClass = c1.getSuperClassName();
   }
   prevClass = CIMName();
   superClass = c2.getClassName();
   while(!(superClass == prevClass))
   {
      prevClass = superClass;
      if(superClass == c1.getClassName())
      {
         return true;
      }
      superClass = c2.getSuperClassName();
   }

   return false;
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
            _valueType = Boolean_type;
            break;
         }
         case CIMTYPE_UINT8:
         {
            Array<Uint8> _uint;
            cv.get(_uint);
            _theValue._U64 = _uint[index];
            _valueType = Uint64_type;
            break;
         }
         case CIMTYPE_UINT16:
         {
            Array<Uint16> _uint;
            cv.get(_uint);
            _theValue._U64 = _uint[index];
            _valueType = Uint64_type;
            break;
         }
         case CIMTYPE_UINT32:
         {
            Array<Uint32> _uint;
            cv.get(_uint);
            _theValue._U64 = _uint[index];
            _valueType = Uint64_type;
            break;
         }
         case CIMTYPE_UINT64:
         {
            Array<Uint64> _uint;
            cv.get(_uint);
            _theValue._U64 = _uint[index];
            _valueType = Uint64_type;
            break;
         }
         case CIMTYPE_SINT8:
         {
            Array<Sint8> _sint;
            cv.get(_sint);
            _theValue._S64 = _sint[index];
            _valueType = Sint64_type;
            break;
         }
         case CIMTYPE_SINT16:
         {
            Array<Sint16> _sint;
            cv.get(_sint);
            _theValue._S64 = _sint[index];
            _valueType = Sint64_type;
            break;
         }
         case CIMTYPE_SINT32:
         {
            Array<Sint32> _sint;
            cv.get(_sint);
            _theValue._S64 = _sint[index];
            _valueType = Sint64_type;
            break;
         }
         case CIMTYPE_SINT64:
         {
            Array<Sint64> _sint;
            cv.get(_sint);
            _theValue._S64 = _sint[index];
            _valueType = Sint64_type;
            break;
         }
           
         case CIMTYPE_REAL32:
         {
            Array<Real32> _real;
            cv.get(_real);
            _theValue._R64 = _real[index];
            _valueType = Real_type;
            break;
         }
         case CIMTYPE_REAL64:
         {
            Array<Real64> _real;
            cv.get(_real);
            _theValue._R64 = _real[index];
            _valueType = Real_type;
            break;
         }   
         case CIMTYPE_CHAR16:
         {
            Array<Char16> _str;
            cv.get(_str);
            _theValue._S = new String(&_str[index]);
            _valueType = String_type;
            break;
         }
         case CIMTYPE_STRING:
         {
            Array<String> _str;
            cv.get(_str);
            _theValue._S = new String(_str[index]);
            _valueType = String_type;
            break;
         }  
         case CIMTYPE_DATETIME:
         {
            Array<CIMDateTime> _date;
            cv.get(_date);
            _theValue._DT = new CIMDateTime(_date[index]);
            _valueType = CIMDateTime_type;
            break;
         }
         case CIMTYPE_REFERENCE:
         {
            Array<CIMObjectPath> _path;
            cv.get(_path);
            _theValue._OP = new CIMObjectPath(_path[index]);
            _valueType = CIMReference_type;
            break;
         }   
         default:
            throw(1);
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
            _valueType = Boolean_type;
            break;
         }
         case CIMTYPE_UINT8:
         {
            Uint8 _uint;
            cv.get(_uint);
            _theValue._U64 = _uint;
            _valueType = Uint64_type;
            break;
         }
         case CIMTYPE_UINT16:
         {
            Uint16 _uint;
            cv.get(_uint);
            _theValue._U64 = _uint;
            _valueType = Uint64_type;
            break;
         }
         case CIMTYPE_UINT32:
         {
            Uint32 _uint;
            cv.get(_uint);
            _theValue._U64 = _uint;
            _valueType = Uint64_type;
            break;
         }
         case CIMTYPE_UINT64:
         {
            Uint64 _uint;
            cv.get(_uint);
            _theValue._U64 = _uint;
            _valueType = Uint64_type;
            break;
         }
         case CIMTYPE_SINT8:
         {
            Sint8 _sint;
            cv.get(_sint);
            _theValue._S64 = _sint;
            _valueType = Sint64_type;
            break;
         }
         case CIMTYPE_SINT16:
         {
            Sint16 _sint;
            cv.get(_sint);
            _theValue._S64 = _sint;
            _valueType = Sint64_type;
            break;
         }
         case CIMTYPE_SINT32:

         {
            Sint32 _sint;
            cv.get(_sint);
            _theValue._S64 = _sint;
            _valueType = Sint64_type;
            break;
         }
         case CIMTYPE_SINT64:
         {
            Sint64 _sint;
            cv.get(_sint);
            _theValue._S64 = _sint;
            _valueType = Sint64_type;
            break;
         }
         case CIMTYPE_REAL32:
         {
            Real32 _real;
            cv.get(_real);
            _theValue._R64 = _real;
            _valueType = Real_type;
            break;
         }
         case CIMTYPE_REAL64:
         {
            Real64 _real;
            cv.get(_real);
            _theValue._R64 = _real;
            _valueType = Real_type;
            break;
         }  
         case CIMTYPE_CHAR16:
         {
            Char16 _str;
            cv.get(_str);
            _theValue._S = new String(&_str);
            _valueType = String_type;
            break;
         }
         case CIMTYPE_STRING:
         {
            String _str;
            cv.get(_str);
            _theValue._S = new String(_str);
            _valueType = String_type;
            break;
         }
         case CIMTYPE_DATETIME:
         {
            CIMDateTime _date;
            cv.get(_date);
            _theValue._DT = new CIMDateTime(_date);
            _valueType = CIMDateTime_type;
            break;
         }
         case CIMTYPE_REFERENCE:
         {
            CIMObjectPath _path;
            cv.get(_path);
            _theValue._OP = new CIMObjectPath(_path);
            _valueType = CIMReference_type;
            break;
         }
         default:
            throw(1);
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
      _CQLChainId[0].setName(inCid[inCid.size()-1].getName());
      _CQLChainId[0].applyScope(inCid[inCid.size()-1].getName().getString());

      for(Sint32 i = inCid.size()-2; i >= 0; --i)
      {
         _CQLChainId.prepend(inCid[i]); 
      }
      resolve(CIMInstance(),_ctx);
   }
   else
   {
      _CQLChainId.applyContext(_ctx);        
   }
}

void CQLValueRep::_resolveSymbolicConstant(QueryContext& inQueryCtx)
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
      throw(1);
   }

   CIMProperty queryPropObj = QueryClass.getProperty(propertyIndex);

   // We have a symbolic constant (ex. propName#OK)
   // We need to retrieve the ValueMap and Values Qualifiers for 
   // the property if the exist.
   Uint32 qualIndex = queryPropObj.findQualifier(CIMName("ValueMap"));

   if(qualIndex == PEG_NOT_FOUND)
   {
      // This property can not be processed with a symbolic constant.
      throw(1);
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
         if(valueMapArray[i] == 
              lid.getSymbolicConstantName())
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
         throw(1);
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
         if(valuesArray[i] == lid.getSymbolicConstantName())
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
         throw(1);
      }

      CString cStr = valueMapArray[matchIndex].getCString();
      char *endP;

      // Set Primitive
      _setValue(CIMValue(Uint64(strtoul((const char*)cStr,&endP,10))));
      return;
   }
}

PEGASUS_NAMESPACE_END
