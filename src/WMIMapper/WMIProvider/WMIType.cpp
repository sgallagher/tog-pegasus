//%/////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001, 2002 BMC Software, Hewlett-Packard Company, IBM,
// The Open Group, Tivoli Systems
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
// Author: Chip Vincent (cvincent@us.ibm.com)
//
// Modified By: Barbara Packard (barbara_packard@hp.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"

#include "WMIType.h"

PEGASUS_NAMESPACE_BEGIN


CIMType WMITypeToCIMType(const CIMTYPE type)
{
   CIMType _type;

   switch(type) {
   case CIM_BOOLEAN:
      _type = CIMTYPE_BOOLEAN;

      break;
   case CIM_SINT8:
      _type = CIMTYPE_SINT8;

      break;
   case CIM_UINT8:
      _type = CIMTYPE_UINT8;

      break;
   case CIM_SINT16:
      _type = CIMTYPE_SINT16;

      break;
   case CIM_UINT16:
      _type = CIMTYPE_UINT16;

      break;
   case CIM_SINT32:
      _type = CIMTYPE_SINT32;

      break;
   case CIM_UINT32:
      _type = CIMTYPE_UINT32;

      break;
   case CIM_SINT64:
      _type = CIMTYPE_SINT64;

      break;
   case CIM_UINT64:
      _type = CIMTYPE_UINT64;

      break;
   case CIM_REAL32:
      _type = CIMTYPE_REAL32;

      break;
   case CIM_REAL64:
      _type = CIMTYPE_REAL64;

      break;
   case CIM_CHAR16:
      _type = CIMTYPE_CHAR16;

      break;
   case CIM_STRING:
      _type = CIMTYPE_STRING;

      break;
   case CIM_DATETIME:
      _type = CIMTYPE_DATETIME;

      break;
   case CIM_REFERENCE:
      _type = CIMTYPE_REFERENCE;

      break;
   case CIM_OBJECT:
   case CIM_FLAG_ARRAY:
   case CIM_EMPTY:
   case CIM_ILLEGAL:
   default:
      throw TypeMismatchException();

      break;
   }

   return _type;
}

CIMType variantToCIMType(const CComVariant & vValue)
{
	return vartypeToCIMType(vValue.vt);
}

CIMType vartypeToCIMType(const VARTYPE vt)
{
   CIMType type;

   switch(vt & ~VT_ARRAY) {
   case VT_I2:
      type = CIMTYPE_SINT16;

      break;
   case VT_UI2:
      type = CIMTYPE_UINT16;

      break;
   case VT_I4:
      type = CIMTYPE_SINT32;

      break;
   case VT_UI4:
      type = CIMTYPE_UINT32;

      break;
   case VT_I8:
      type = CIMTYPE_SINT64;

      break;
   case VT_UI8:
      type = CIMTYPE_UINT64;

      break;
   case VT_R4:
      type = CIMTYPE_REAL32;

      break;
   case VT_R8:
      type = CIMTYPE_REAL64;

      break;
   case VT_BSTR:
      //type = CIMTYPE_REFERENCE;
      //type = CIMTYPE_DATETIME;

      type = CIMTYPE_STRING;

      break;
	case VT_DATE:
		type = CIMTYPE_DATETIME;

		break;
   case VT_BOOL:
      type = CIMTYPE_BOOLEAN;

      break;
	case VT_INT:	
		type = (sizeof(int) == 8) ? CIMTYPE_SINT64 : CIMTYPE_SINT32;
		break;

	case VT_UINT:	
		type = (sizeof(int) == 8) ? CIMTYPE_UINT64 : CIMTYPE_UINT32;

		break;
   case VT_EMPTY:
   case VT_NULL:
   default:
		throw TypeMismatchException();
		break;
   }

   return type;
}


PEGASUS_NAMESPACE_END
