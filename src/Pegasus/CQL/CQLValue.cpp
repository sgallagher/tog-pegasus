#include <cstdlib>
#include <iostream>

#include <Pegasus/CQL/CQLValue.h>
#include <Pegasus/Repository/NameSpaceManager.h>
#include <Pegasus/Common/CIMClass.h>
#include <Pegasus/CQL/CQLIdentifier.h>




PEGASUS_NAMESPACE_BEGIN
PEGASUS_USING_STD;

#define PEGASUS_ARRAY_T CQLValue
#include <Pegasus/Common/ArrayImpl.h>
#undef PEGASUS_ARRAY_T


#define CIMTYPE_EMBEDDED 15  //temporary

CQLValue::CQLValue(String inString, NumericType inValueType, Boolean inSign)
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


CQLValue::CQLValue(CQLChainedIdentifier inCQLIdent)
{
   _CQLChainId = new CQLChainedIdentifier(inCQLIdent); 
   _isResolved = false;
}


CQLValue::CQLValue(String inString)
{
   _theValue._S = new String(inString);
   _valueType = String_type;
   _isResolved = true;
}

CQLValue::CQLValue(CIMInstance inInstance)
{
   _theValue._IN = new CIMInstance(inInstance);
   _valueType = CIMInstance_type;
   _isResolved = true;
}

CQLValue::CQLValue(CIMObjectPath inObjPath)
{
   _theValue._OP = new CIMObjectPath(inObjPath);
   _valueType = CIMReference_type;
   _isResolved = true;
}

CQLValue::CQLValue(CIMDateTime inDateTime)
{
   _theValue._DT = new CIMDateTime(inDateTime);
   _valueType = CIMDateTime_type;
   _isResolved = true;
}

CQLValue::CQLValue(Uint64 inUint)
{
   _theValue._U64 = inUint;
   _valueType = Uint64_type;
   _isResolved = true;
}

CQLValue::CQLValue(Sint64 inSint)
{
   _theValue._S64 = inSint;
   _valueType = Sint64_type;
   _isResolved = true;
}

CQLValue::CQLValue(Real64 inReal)
{
   _theValue._R64 = inReal;
   _valueType = Real_type;
   _isResolved = true;
}

void CQLValue::resolve(CIMInstance CI, QueryContext& inQueryCtx)
{   
   if(_isResolved)
   {
      return;
   }

   CQLIdentifier classNameID;
   String className;
   Uint32 index = 0;
   Boolean matchFound = false;
   Uint32 matchIndex;
   CIMClass ScopeClass;
   CIMClass QueryClass;
   Array<CQLIdentifier> Idstrings = _CQLChainId->getSubIdentifiers();
   Array<String> valueMapArray;
   Array<String> valuesArray;
   char * endP;
   Uint64 valueMapNum;
   CIMProperty propObj;
   CIMProperty queryPropObj;
   Uint32 qualIndex;
   CIMValue valueMap;
   Boolean isEmbedded = false;

   classNameID = inQueryCtx.findClass(Idstrings[index].getName().getString());

   className = classNameID.getName().getString();
   
   if(className.size() == 0)
   {  cout << "step 1" << endl;
      // Get className from FromList
      Array<CQLIdentifier> classList;

      classList = inQueryCtx.getFromList();

      if(classList.size() != 1)
      {
         throw(1);
      }

      className = classList[0].getName().getString();   
   
   }
   else if(Idstrings.size() == 1)
   {cout << "step 2" << endl;
      // Set the instance passed in, as a primitive.
      _theValue._IN = (CIMInstance *) new CIMInstance(CI);
      _valueType = CIMInstance_type;
      return; // Done.
   }
   else
   {cout << "step 3" << endl;
      // Need to increment index since the first Identifier is a class
      ++index;
   }
   cout << "step 4" << endl;
   QueryClass = inQueryCtx.getClass(className);

   for(;index < Idstrings.size(); ++index)
   {printf("%d < %d\n",index,Idstrings.size());
cout << "step 5" << endl;
      cout << Idstrings[index].toString().getCString() << endl;
      if(Idstrings[index].isScoped())
      {  // This property is scoped.
      cout << "step 6" << endl;
         ScopeClass = inQueryCtx.getClass(Idstrings[index].getScope());

         if(ScopeClass.findProperty(Idstrings[index].getName()) == 
            PEG_NOT_FOUND)
         {
            // Set the Ignore_type
            _valueType = CQLIgnore_type;
            return;
         }
         else if(!_areClassesInline(ScopeClass,QueryClass,inQueryCtx))
         {
            throw(1);
         }
      }
      else if(isEmbedded)
      {  // all embedded properties must be scoped.
         throw(1);
      }
      Uint32 propertyIndex = QueryClass.findProperty(Idstrings[index].getName());
 cout << "step 7" << endl;
      if(propertyIndex == PEG_NOT_FOUND)
      {
         throw(1);
      }
cout << "step 8" << endl;
      CIMType propertyType = QueryClass.getProperty(propertyIndex).getType();

      if(propertyType == CIMTYPE_EMBEDDED)
      {
         // Do embedded code here.
         
         isEmbedded = true;
         break;
      }
      else // Primitive
      {cout << "step 9" << endl;
         queryPropObj = QueryClass.getProperty(propertyIndex);
         propertyIndex = CI.findProperty(Idstrings[index].getName());
cout << Idstrings[index].getName().getString().getCString() << endl;
         propObj = CI.getProperty(propertyIndex);
cout << "prop value: " << propObj.getValue().toString().getCString() << endl;
cout << "step 10" << endl;
         if(Idstrings[index].isSymbolicConstant())
         {cout << "step 10a" << endl;
            
            qualIndex = queryPropObj.findQualifier(CIMName("ValueMap"));
         
            if(qualIndex == PEG_NOT_FOUND)
            {cout << "step bad" << endl;
               throw(1);
            }
            valueMap = queryPropObj.getQualifier(qualIndex).getValue();

            qualIndex = queryPropObj.findQualifier(CIMName("Values"));
cout << "step 10b" << endl;
            if(qualIndex == PEG_NOT_FOUND)
            {cout << "step 12" << endl;
               // The valueMap must be a list of symbolic constants
               valueMap.get(valueMapArray);

               for(Uint32 i = 0; i < valueMapArray.size(); ++i)
               {
                  if(valueMapArray[i] == 
                        Idstrings[index].getSymbolicConstantName())
                  {
                     matchFound = true;
                     matchIndex = i;
                     break;
                  }
               }
               if(matchFound == false)
               {
                  throw(1);
               }
               else
               {
                  // Verify property value matches SymbolicConstant
                  if(propObj.getValue() == 
                        CIMValue(Idstrings[index].getSymbolicConstantName()))
                  {
                     // Set primitive
                  }
                  else
                  {
                     // Set the Ignore_type
                     _valueType = CQLIgnore_type;
                     return;     
                  }
               }
   
            }
            else
            {cout << "step 10c" << endl;
               // valueMap must be a list of #'s
               CIMValue values = queryPropObj.getQualifier(qualIndex).getValue();
   
               valueMap.get(valueMapArray);
               values.get(valuesArray);
cout << "step 10d" << endl;
               for(Uint32 i = 0; i < valuesArray.size(); ++i)
               {
                  if(valuesArray[i] == Idstrings[index].getSymbolicConstantName())
                  {
                     matchFound = true;
                     matchIndex = i;
                     break;
                  }
               }
               if(matchFound == false)
               {
                  throw(1);
               }
                 
               cout << "step 10e" << endl;
               valueMapNum = strtoul(
                     (const char *)valueMapArray[matchIndex].getCString(),&endP,10);
               cout << "prop value: " << propObj.getValue().toString().getCString() << endl;
               Uint16 ttt;
               propObj.getValue().get(ttt);
               printf("%i %i\n",valueMapNum,ttt);
              cout << "step 10f" << endl;
               if(valueMapArray[matchIndex] == propObj.getValue().toString())
               {cout << "step 10g" << endl;
                  // Set Primitive
                  _setValue(propObj.getValue());
                  return;
               }
               else
               {cout << "step 10h" << endl;
                  // Set the Ignore_type
                  _valueType = CQLIgnore_type;
                  return;
   
               }
            }
         }
         else if(Idstrings[index].isArray())
         {
            CIMValue cimVal = propObj.getValue();

            switch(cimVal.getType())
            {
               case CIMTYPE_UINT8:
               case CIMTYPE_UINT16:
               case CIMTYPE_UINT32:
               case CIMTYPE_UINT64:
               {
                  Array<Uint64> propArray;
                  cimVal.get(propArray);
        
                  _theValue._U64 = 
                        propArray[Idstrings[index].getSubRanges()[0].start];

                  _valueType = Uint64_type;
               } break;
 
               case CIMTYPE_SINT8:
               case CIMTYPE_SINT16:
               case CIMTYPE_SINT32:
               case CIMTYPE_SINT64:
               {
                  Array<Sint64> propArray;
                  cimVal.get(propArray);
        
                  _theValue._S64 = 
                        propArray[Idstrings[index].getSubRanges()[0].start];
                  _valueType = Sint64_type;
               } break;

               case CIMTYPE_REAL32:
               case CIMTYPE_REAL64:
               {
                  Array<Real64> propArray;
                  cimVal.get(propArray);
        
                  _theValue._R64 = 
                        propArray[Idstrings[index].getSubRanges()[0].start];
                  _valueType = Real_type;
               } break;

               case CIMTYPE_CHAR16:
               case CIMTYPE_STRING:
               {
                  Array<String> propArray;
                  cimVal.get(propArray);
        
                  _theValue._S = 
                    new String(propArray[Idstrings[index].getSubRanges()[0].start]);
                  _valueType = String_type;
               } break;

               case CIMTYPE_DATETIME:
               {
                  Array<CIMDateTime> propArray;
                  cimVal.get(propArray);
        
                  _theValue._DT = 
                new CIMDateTime(propArray[Idstrings[index].getSubRanges()[0].start]);
                  _valueType = CIMDateTime_type;
               } break;

               default:
                  throw(1);
            } // switch statement
         }
         else if(Idstrings[index].isWildcard())
         {  
            _theValue._IN = new CIMInstance(CI);
            _valueType = CIMInstance_type;
         }
         else
         {cout << "step 11" << endl;
            CIMValue cimVal = propObj.getValue();
            _setValue(cimVal);
         }
      } // else body
   } // loop
} // end of function


Boolean CQLValue::operator==(const CQLValue& x)
{
   if(!_validate(x))
   {
      throw(1);
   }  
 
   switch(_valueType)
   {
      case Null_type:
         return true;
         break;
      case Sint64_type:
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
Boolean CQLValue::operator!=(const CQLValue& x)
{
if(!_validate(x))
   {
      throw(1);
   }  
 
   switch(_valueType)
   {
      case Null_type:
         return false;
         break;
      case Sint64_type:
         if(x._valueType == Sint64_type)
         {
            if(_theValue._S64 != x._theValue._S64)
            {
               return true;
            }
         }
         else if(x._valueType == Uint64_type)
         {
            if(_theValue._S64 != (Sint64)x._theValue._U64)
            {
               return true;
            }
         }
         else if(x._valueType == Real_type)
         {
            if(_theValue._S64 != x._theValue._R64)
            {
               return true;
            }
         }
         break;
      case Uint64_type:
         if(x._valueType == Sint64_type)
         {
            if((Sint64)_theValue._U64 != x._theValue._S64)
            {
               return true;
            }
         }
         else if(x._valueType == Uint64_type)
         {
            if(_theValue._U64 != x._theValue._U64)
            {
               return true;
            }
         }
         else if(x._valueType == Real_type)
         {
            if(_theValue._U64 != x._theValue._R64)
            {
               return true;
            }
         }
         break;
      case Real_type:
         if(x._valueType == Sint64_type)
         {
            if(_theValue._R64 != x._theValue._S64)
            {
               return true;
            }
         }
         else if(x._valueType == Uint64_type)
         {
            if(_theValue._R64 != x._theValue._U64)
            {
               return true;
            }
         }
         else if(x._valueType == Real_type)
         {
            if(_theValue._R64 != x._theValue._R64)
            {
               return true;
            }
         }
         break;
      case String_type:
         if(*_theValue._S != *x._theValue._S)
         {
            return true;
         }
         break;
      case CIMDateTime_type:
         if(*_theValue._DT != *x._theValue._DT)
         {
            return true;
         }
         break;
         case CIMReference_type:
         if(*_theValue._OP != *x._theValue._OP)
         {
            return true;
         }
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


Boolean CQLValue::operator<=(const CQLValue& x)
{
if(!_validate(x))
   {
      throw(1);
   }  
 
   switch(_valueType)
   {
      case Null_type:
         return true;
         break;
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


Boolean CQLValue::operator>=(const CQLValue& x)
{
if(!_validate(x))
   {
      throw(1);
   }  
 
   switch(_valueType)
   {
      case Null_type:
         return true;
         break;
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


Boolean CQLValue::operator<(const CQLValue& x)
{
if(!_validate(x))
   {
      throw(1);
   }  
 
   switch(_valueType)
   {
      case Null_type:
         return true;
         break;
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


Boolean CQLValue::operator>(const CQLValue& x)
{
   if(!_validate(x))
   {
      throw(1);
   }  
 
   switch(_valueType)
   {
      case Null_type:
         return true;
         break;
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


CQLValue CQLValue::operator+(const CQLValue& x)
{
   if(!_validate(x))  
   {
      throw(1);
   } 

    
   switch(_valueType)
   {
      case Null_type:
         throw(1);
         break;
      case Sint64_type:
         if(x._valueType == Sint64_type)
         {
            return CQLValue(_theValue._S64 + x._theValue._S64);        
         }
         else if(x._valueType == Uint64_type)
         {
            return CQLValue(_theValue._S64 + x._theValue._U64);
         }
         else if(x._valueType == Real_type)
         {
            return CQLValue(_theValue._S64 + x._theValue._R64);
         }
         break;
      case Uint64_type:
         if(x._valueType == Sint64_type)
         {
            return CQLValue(_theValue._U64 + x._theValue._S64);
         }
         else if(x._valueType == Uint64_type)
         {
            return CQLValue(_theValue._U64 + x._theValue._U64);
         }
         else if(x._valueType == Real_type)
         {
            return CQLValue(_theValue._U64 + x._theValue._R64);
         }
         break;
      case Real_type:
         if(x._valueType == Sint64_type)
         {
            return CQLValue(_theValue._R64 + x._theValue._S64);
         }
         else if(x._valueType == Uint64_type)
         {
            return CQLValue(_theValue._R64 + x._theValue._U64);
         }
         else if(x._valueType == Real_type)
         {
            return CQLValue(_theValue._R64 + x._theValue._R64);
         }
         break;
      case String_type:
         return CQLValue(*_theValue._S + *x._theValue._S);
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


CQLValue CQLValue::operator-(const CQLValue& x)
{
   if(!_validate(x))  
   {
      throw(1);
   } 

    
   switch(_valueType)
   {
      case Null_type:
         throw(1);
         break;
      case Sint64_type:
         if(x._valueType == Sint64_type)
         {
            return CQLValue(_theValue._S64 - x._theValue._S64);        
         }
         else if(x._valueType == Uint64_type)
         {
            return CQLValue(_theValue._S64 - x._theValue._U64);
         }
         else if(x._valueType == Real_type)
         {
            return CQLValue(_theValue._S64 - x._theValue._R64);
         }
         break;
      case Uint64_type:
         if(x._valueType == Sint64_type)
         {
            return CQLValue(_theValue._U64 - x._theValue._S64);
         }
         else if(x._valueType == Uint64_type)
         {
            return CQLValue(_theValue._U64 - x._theValue._U64);
         }
         else if(x._valueType == Real_type)
         {
            return CQLValue(_theValue._U64 - x._theValue._R64);
         }
         break;
      case Real_type:
         if(x._valueType == Sint64_type)
         {
            return CQLValue(_theValue._R64 - x._theValue._S64);
         }
         else if(x._valueType == Uint64_type)
         {
            return CQLValue(_theValue._R64 - x._theValue._U64);
         }
         else if(x._valueType == Real_type)
         {
            return CQLValue(_theValue._R64 - x._theValue._R64);
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


CQLValue CQLValue::operator*(const CQLValue& x)
{

   if(!_validate(x))  
   {
      throw(1);
   } 

    
   switch(_valueType)
   {
      case Null_type:
         throw(1);
         break;
      case Sint64_type:
         if(x._valueType == Sint64_type)
         {
            return CQLValue(_theValue._S64 * x._theValue._S64);        
         }
         else if(x._valueType == Uint64_type)
         {
            return CQLValue(_theValue._S64 * x._theValue._U64);
         }
         else if(x._valueType == Real_type)
         {
            return CQLValue(_theValue._S64 * x._theValue._R64);
         }
         break;
      case Uint64_type:
         if(x._valueType == Sint64_type)
         {
            return CQLValue(_theValue._U64 * x._theValue._S64);
         }
         else if(x._valueType == Uint64_type)
         {
            return CQLValue(_theValue._U64 * x._theValue._U64);
         }
         else if(x._valueType == Real_type)
         {
            return CQLValue(_theValue._U64 * x._theValue._R64);
         }
         break;
      case Real_type:
         if(x._valueType == Sint64_type)
         {
            return CQLValue(_theValue._R64 * x._theValue._S64);
         }
         else if(x._valueType == Uint64_type)
         {
            return CQLValue(_theValue._R64 * x._theValue._U64);
         }
         else if(x._valueType == Real_type)
         {
            return CQLValue(_theValue._R64 * x._theValue._R64);
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


CQLValue CQLValue::operator/(const CQLValue& x)
{
   if(!_validate(x) || 
      x._theValue._U64 == 0)  
   {
      throw(1);
   } 
    
   switch(_valueType)
   {
      case Null_type:
         throw(1);
         break;
      case Sint64_type:
         if(x._valueType == Sint64_type)
         {
            return CQLValue(_theValue._S64 / x._theValue._S64);        
         }
         else if(x._valueType == Uint64_type)
         {
            return CQLValue(_theValue._S64 / x._theValue._U64);
         }
         else if(x._valueType == Real_type)
         {
            return CQLValue(_theValue._S64 / x._theValue._R64);
         }
         break;
      case Uint64_type:
         if(x._valueType == Sint64_type)
         {
            return CQLValue(_theValue._U64 / x._theValue._S64);
         }
         else if(x._valueType == Uint64_type)
         {
            return CQLValue(_theValue._U64 / x._theValue._U64);
         }
         else if(x._valueType == Real_type)
         {
            return CQLValue(_theValue._U64 / x._theValue._R64);
         }
         break;
      case Real_type:
         if(x._valueType == Sint64_type)
         {
            return CQLValue(_theValue._R64 / x._theValue._S64);
         }
         else if(x._valueType == Uint64_type)
         {
            return CQLValue(_theValue._R64 / x._theValue._U64);
         }
         else if(x._valueType == Real_type)
         {
            return CQLValue(_theValue._R64 / x._theValue._R64);
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
CQLValueType CQLValue::getValueType()
{
   return _valueType;
}


void CQLValue::setNull()
{
   _valueType = Null_type;
}


Boolean CQLValue::isResolved()
{
   return _isResolved;
}


Boolean CQLValue::isNull()
{
   if(_valueType == Null_type)
   {
      return true;
   }
   return false;
}


Boolean CQLValue::isa(const CQLValue& inVal, QueryContext& QueryCtx)
{
   if(!_isResolved && !inVal._isResolved)
   {
      throw(1);
   }
   if(_valueType != CIMInstance_type)
   {
      throw(1);
   }
   if(inVal._valueType != String_type)
   {
      throw(1);
   }

   CIMName  className;
   CIMClass classObj = QueryCtx.getClass(this->_theValue._IN->getClassName());

   className = classObj.getSuperClassName();

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


Boolean CQLValue::like(const CQLValue& inVal)
{
   return false;
}

Boolean CQLValue::_validate(const CQLValue& x)
{
   switch(_valueType)
   {
      case Null_type:
         if(x._valueType != Null_type)
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

Boolean CQLValue::_areClassesInline(CIMClass c1,CIMClass c2,QueryContext& QC)
{ cout << "_areClassesInline" << endl;
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

void CQLValue::_setValue(CIMValue cv,Uint64 index)
{
   CIMValue tmp;
   if(cv.isArray())
   {
      switch(cv.getType())
      {
         case CIMTYPE_UINT8:
         case CIMTYPE_UINT16:
         case CIMTYPE_UINT32:
         case CIMTYPE_UINT64:
         {
            Array<Uint64> u64;
            cv.get(u64);
            _theValue._U64 = u64[index];
            _valueType = Uint64_type;
            break;
         }
         case CIMTYPE_SINT8:
         case CIMTYPE_SINT16:
         case CIMTYPE_SINT32:
         case CIMTYPE_SINT64:
           
         case CIMTYPE_REAL32:
         case CIMTYPE_REAL64:
            
         case CIMTYPE_CHAR16:
         case CIMTYPE_STRING:
           
         case CIMTYPE_DATETIME:
            
         default:
            throw(1);
      } // switch statement

   }
   else
   {
      switch(cv.getType())
      {
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
         case CIMTYPE_STRING:
         {
            String tmp1;
            cv.get(tmp1);
            _theValue._S = new String(tmp1);
            _valueType = String_type;
            break;
         }
         case CIMTYPE_DATETIME:
         {
            CIMDateTime tmp1;
            cv.get(tmp1);
            _theValue._DT = new CIMDateTime(tmp1);
            _valueType = CIMDateTime_type;
            break;
         }
         default:
            throw(1);
      } // switch statement
   }
   
}

void CQLValue::print()
{
   switch(_valueType)
   {
      case Sint64_type:
         printf("%d\n",(int)_theValue._S64);
         break;
      case Uint64_type:
         printf("%u\n",(unsigned int)_theValue._U64);
         break;
      case Real_type:
         printf("%f\n",_theValue._R64);
         break;
      case String_type:
      {
         String a1(*_theValue._S);
         printf("%s\n",(const char*)a1.getCString());
      }
         break;
      case CIMDateTime_type:
      {
         cout << _theValue._DT->toString().getCString() << endl; 
      }
         break;
      case CIMReference_type:
      {
         cout << _theValue._OP->toString().getCString() << endl;
      }
         break;
      case CIMInstance_type:
      {
         cout << _theValue._IN->getPath().toString().getCString() << endl;
      }
         break;
      case CQLIgnore_type:
      {
         cout << "CQLIgnore_type" << endl;
      }
         break;
      default:
         break;
   }
   return;
}

PEGASUS_NAMESPACE_END
