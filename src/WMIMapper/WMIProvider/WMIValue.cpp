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
// Modified By:	Barbara Packard (barbara_packard@hp.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"

#include "WMIValue.h"
#include "WMIString.h"
#include "WMIType.h"
#include "WMIDateTime.h"
#include "WMIObjectPath.h"

PEGASUS_NAMESPACE_BEGIN

static const char _NULL_INTERVAL_TYPE_STRING[] = "00000000000000.000000:000";
static const char _NULL_DATE_TYPE_STRING[] = "00000000000000.000000-000";

WMIValue::WMIValue(const CIMValue & value) : CIMValue(value)
{
}

WMIValue::WMIValue(const CComVariant & vValue)
{
	CIMValue val;
	val = getValue(vValue);
	*this = CIMValue(val);
}

WMIValue::WMIValue(VARTYPE vt, void *pVal)
{
	CIMValue val;
	val = getCIMValueFromVariant(vt, pVal); 
	*this = CIMValue(val);
}

WMIValue::WMIValue(VARTYPE vt, void *pVal, const CIMTYPE type)
{
	CIMValue val;
	val = getCIMValueFromVariant(vt, pVal, type); 
	*this = CIMValue(val);
}


WMIValue::WMIValue(const VARIANT & value, const CIMTYPE type)
{
	CIMValue val;
	CComVariant vValue;

	vValue = value;

	if ((vValue.vt != VT_NULL) && ((type & ~CIM_FLAG_ARRAY) != CIM_REFERENCE)) 
	{

		val = getValue(vValue, type);
	}
	else
	{
	// for class definitions, the value will be null and will except
		_variant_t vt(value);

		switch(type & ~CIM_FLAG_ARRAY)
		{
		case CIM_BOOLEAN:
			try
			{
				val.set(Boolean(bool(vt)));
			}
			catch(...)
			{
				val.setNullValue(CIMTYPE_BOOLEAN, false, 0);
			}

			break;
		case CIM_BOOLEAN | CIM_FLAG_ARRAY:
			//val.setNullValue(CIMType(CIMTYPE_BOOLEAN), true. 0);

			break;
		case CIM_SINT8:
			try
			{
				val.set(Sint8(BYTE(vt)));
			}
			catch(...)
			{
				val.setNullValue(CIMTYPE_SINT8, false, 0);
			}

			break;
		case CIM_SINT8 | CIM_FLAG_ARRAY:
			//val.setNullValue(CIMTYPE_SINT8, true. 0);

			break;
		case CIM_UINT8:
			try
			{
				val.set(Uint8(BYTE(vt)));
			}
			catch(...)
			{
				val.setNullValue(CIMTYPE_SINT8, false, 0);
			}

			break;
		case CIM_UINT8 | CIM_FLAG_ARRAY:
			//val.setNullValue(CIMTYPE_UINT8, true. 0);

			break;
		case CIM_SINT16:
			try
			{
				val.set(Sint16(short(vt)));
			}
			catch(...)
			{
				val.setNullValue(CIMTYPE_SINT16, false, 0);
			}

			break;
		case CIM_SINT16 | CIM_FLAG_ARRAY:
			//val.setNullValue(CIMTYPE_SINT16, true. 0);

			break;
		case CIM_UINT16:
			try
			{
				val.set(Uint16(short(vt)));
			}
			catch(...)
			{
				val.setNullValue(CIMTYPE_UINT16, false, 0);
			}

			break;
		case CIM_UINT16 | CIM_FLAG_ARRAY:
			//val.setNullValue(CIMTYPE_UINT16, true. 0);

			break;
		case CIM_SINT32:
			try
			{
				val.set(Sint32(long(vt)));
			}
			catch(...)
			{
				val.setNullValue(CIMTYPE_SINT32, false, 0);
			}

			break;
		case CIM_SINT32 | CIM_FLAG_ARRAY:
			//val.setNullValue(CIMTYPE_SINT32, true. 0);

			break;
		case CIM_UINT32:
			try
			{
				val.set(Uint32(long(vt)));
			}
			catch(...)
			{
				val.setNullValue(CIMTYPE_UINT32, false);
			}

			break;
		case CIM_UINT32 | CIM_FLAG_ARRAY:
			//val.setNullValue(CIMTYPE_UINT32, true. 0);

			break;
		case CIM_SINT64:
			try
			{
				_bstr_t bstr(vt);

				val.set(Sint64(_wtoi64(bstr)));
			}
			catch(...)
			{
				val.setNullValue(CIMTYPE_SINT64, false, 0);
			}

			break;
		case CIM_SINT64 | CIM_FLAG_ARRAY:
			//val.setNullValue(CIMTYPE_SINT64, true. 0);

			break;
		case CIM_UINT64:
			try
			{
				_bstr_t bstr(vt);

				val.set(Uint64(_wtoi64(bstr)));
			}
			catch(...)
			{
				val.setNullValue(CIMTYPE_UINT64, false, 0);
			}

			break;
		case CIM_UINT64 | CIM_FLAG_ARRAY:
			//val.setNullValue(CIMTYPE_UINT64, true. 0);

			break;
		case CIM_REAL32:
			try
			{
				val.set(Real32(float(vt)));
			}
			catch(...)
			{
				val.setNullValue(CIMTYPE_REAL32, false, 0);
			}

			break;
		case CIM_REAL32 | CIM_FLAG_ARRAY:
			//val.setNullValue(CIMTYPE_REAL32, true. 0);

			break;
		case CIM_REAL64:
			try
			{
				val.set(Real64(double(vt)));
			}
			catch(...)
			{
				val.setNullValue(CIMTYPE_REAL64, false, 0);
			}

			break;
		case CIM_REAL64 | CIM_FLAG_ARRAY:
			//val.setNullValue(CIMTYPE_REAL64, true. 0);

			break;
		case CIM_STRING:
			try
			{
				val.set(WMIString(_bstr_t(vt)));
			}
			catch(...)
			{
			Tracer::trace(TRC_WMIPROVIDER, Tracer::LEVEL3,
				"WMIValue::WMIValue - Constructor failed; setting NULL string");
				val.setNullValue(CIMTYPE_STRING, false, 0);
			}

			break;
		case CIM_STRING | CIM_FLAG_ARRAY:
			//val.setNullValue(CIMTYPE_STRING, true. 0);

			break;
		case CIM_DATETIME:
			try
			{
				val.set(WMIDateTime(_bstr_t(vt)));
			}
			catch(...)
			{
				val.setNullValue(CIMTYPE_DATETIME, false, 0);
			}

			break;
		case CIM_DATETIME | CIM_FLAG_ARRAY:
			//val.setNullValue(CIMTYPE_DATETIME, true. 0);

			break;
		case CIM_REFERENCE:
			try
			{
				val.set(WMIObjectPath(_bstr_t(vt)));
			}
			catch(...)
			{
				val.setNullValue(CIMTYPE_REFERENCE, false);
			}

			break;
		case CIM_CHAR16:
			try
			{
				val.set(Char16(short(vt)));
			}
			catch(...)
			{
				val.setNullValue(CIMTYPE_CHAR16, false);
			}

			break;
		case CIM_CHAR16 | CIM_FLAG_ARRAY:
			//val.setNullValue(CIMTYPE_CHAR16, true. 0);

			break;
		case CIM_OBJECT:

			break;
		case CIM_EMPTY:

			break;
		case CIM_ILLEGAL:
		default:

			break;
		}
	}	

	*this = CIMValue(val);
}



/*WMIValue::operator VARIANT(void) const
{
	_variant_t vt;
	//isArray();

	switch(getType())
	{
	case CIMTYPE_BOOLEAN:
		{
			Boolean value;

			get(value);

			vt = value;
		}

		break;
	case CIMTYPE_UINT8:
		{
			Uint8 value;

			get(value);

			vt = value;
		}

		break;
	case CIMTYPE_SINT8:
		{
			Sint8 value;

			get(value);

			vt = (BYTE)value;
		}

		break;
	case CIMTYPE_UINT16:
		{
			Uint16 value;

			get(value);

			vt = (short)value;
		}

		break;
	case CIMTYPE_SINT16:
		{
			Sint16 value;

			get(value);

			vt = value;
		}

		break;
	case CIMTYPE_UINT32:
		{
			Uint32 value;

			get(value);

			vt = (long)value;
		}

		break;
	case CIMTYPE_SINT32:
		{
			Sint32 value;

			get(value);

			vt = (long)value;
		}

		break;
	case CIMTYPE_UINT64:
		{
			Uint64 value;

			get(value);

			wchar_t wsz[20 + 1];

			_i64tow(value, wsz, 10);

			vt = _bstr_t(wsz);
		}

		break;
	case CIMTYPE_SINT64:
		{
			Sint64 value;

			get(value);

			wchar_t wsz[20 + 1];

			_ui64tow(value, wsz, 10);

			vt = _bstr_t(wsz);
		}

		break;
	case CIMTYPE_REAL32:
		{
			Real32 value;

			get(value);

			vt = value;
		}

		break;
	case CIMTYPE_REAL64:
		{
			Real64 value;

			get(value);

			vt = value;
		}

		break;
	case CIMTYPE_CHAR16:
		{
			Char16 value;

			get(value);

			vt = (short)value;
		}

		break;
	case CIMTYPE_STRING:
		{
			String value;

			get(value);

			vt = BSTR(WMIString(value));
		}

		break;
	case CIMTYPE_DATETIME:
		{
			CIMDateTime value;

			get(value);

			vt = _bstr_t(value.getString());
		}

		break;
	case CIMTYPE_REFERENCE:
		{
			CIMReference value;

			get(value);

			vt = _bstr_t(WMIString(value.toString()));
		}

		break;
	default:
		break;
	}

	return(vt);
}


*/

/////////////////////////////////////////////////////////////////////////////
// WMIValue::getCIMValueFromVariant - helper function to convert a variant 
//		value to a CIM value
//
// ///////////////////////////////////////////////////////////////////////////
CIMValue WMIValue::getCIMValueFromVariant(VARTYPE vt, void *pVal, const CIMTYPE Type)
{
	CIMValue cimValue;
	CComBSTR bs;
	CMyString s;

	switch (vt)
	{

	case VT_I2:
		return ( CIMValue((Sint16)*((short *)pVal))); 
		break;

	case VT_I4:
		// have to screw around here because automation type
		// is not necessarily the mof type...
		if (CIM_UINT16 == Type)
		{
			return ( CIMValue((Uint16)*((unsigned short *)pVal)));
		}
		else if (CIM_UINT32 == Type)
		{
			return ( CIMValue((Uint32)*((unsigned long *)pVal)));
		}
		else
		{
			return ( CIMValue((Sint32)*((int *)pVal)));
		}
		break;

	case VT_R4:
		return ( CIMValue((Real32)*((float *)pVal)));
		break;

	case VT_R8:
		return ( CIMValue((Real64)*((double *)pVal)));
		break;

	case VT_DATE:
	{
		////ATTN:  it needs to be checked to see if this really
		// works!!

		DATE date;
		CIMDateTime tmp;
		SYSTEMTIME sTime;

		date = *(DATE *)pVal;

		if (VariantTimeToSystemTime(date, &sTime))
		{
			WBEMTime wTime(sTime);
			bs = wTime.GetDMTF();
			s = bs;
			Tracer::trace(TRC_WMIPROVIDER,Tracer::LEVEL3,
				"WMIValue::getValue() - Date is %s", (LPCTSTR)s);
		}
		else
		{
			////ATTN: just to have something for now
			s = _NULL_DATE_TYPE_STRING;
		}

		tmp.set((LPCTSTR)s);
		return CIMValue(tmp);
	}
		break;

	case VT_BSTR:
		bs = *((BSTR *)pVal);
		if (0 == bs.Length())
		{
			return CIMValue(String(""));
		}
		else
		{
			s = bs;
			return CIMValue(String((LPCTSTR)s));
		}
		break;

	case VT_BOOL:
	{	
		return ( (*(VARIANT_BOOL *)pVal) ? CIMValue(true) : CIMValue(false) );
	}
		break;

	case VT_I1:	
		return ( CIMValue((Sint8)*((char *)pVal)));
		break;

	case VT_UI1:	
		return ( CIMValue((Uint8)*((BYTE *)pVal)));
		break;

	case VT_UI2:	
		return ( CIMValue((Uint16)*((unsigned short *)pVal)));
		break;

	case VT_UI4:	
		return ( CIMValue((Uint32)*((unsigned long *)pVal)));
		break;

	case VT_I8:
		////ATTN:  Will this conversion work??
		return ( CIMValue((Sint64)*((_int64 *)pVal)));
		break;

	case VT_UI8:	
		////ATTN:  Will this conversion work??
		return ( CIMValue((Uint64)*((unsigned _int64 *)pVal)));
		break;

	case VT_INT:	
		if (sizeof(int) == 8)
		{
			return ( CIMValue((Sint64)*((_int64 *)pVal)));
		}
		else
		{
			return ( CIMValue((Sint32)*((long *)pVal)));
		}
		break;

	case VT_UINT:	
		if (sizeof(int) == 8)
		{
			return ( CIMValue((Uint64)*((unsigned _int64 *)pVal)));
		}
		else
		{
			return ( CIMValue((Uint32)*((unsigned long *)pVal)));
		}
		break;

	default:
		throw TypeMismatchException();
		break;
    };
}

/////////////////////////////////////////////////////////////////////////////
// WMIValue::getValue - converts a variant to a Pegasus CIMValue
//
// ///////////////////////////////////////////////////////////////////////////
CIMValue WMIValue::getValue(const CComVariant & vValue, const CIMTYPE Type)
{
	bool isRef = isReferenceType(vValue.vt);
	bool isArray = isArrayType(vValue.vt);

// 	PEG_METHOD_ENTER(TRC_WMIPROVIDER,"WMIValue::getValue()");


	if (isArray) 
	{
		return getArrayValue(vValue, Type);
	}

	VARTYPE vt = isRef ? vValue.vt ^ VT_BYREF : vValue.vt;

//	Tracer::trace(TRC_WMIPROVIDER, Tracer::LEVEL3,
//		"WMIValue::getValue() - Vartype is %x", vt);

	void *pValue;
	
	// just getting the address of the value field, type doesn't matter
	if ( isRef)
	{
		pValue = (void *)vValue.plVal;
	}
	else
	{
		pValue = (void *)&(vValue.lVal);
	}

//	PEG_METHOD_EXIT();
	return ( getCIMValueFromVariant(vt, pValue, Type) );

}

/////////////////////////////////////////////////////////////////////////////
// WMICollector::getArrayValueAux - helper function for getArrayValue 
//
// ///////////////////////////////////////////////////////////////////////////
template<class T>
CIMValue getArrayValueAux(LPSAFEARRAY psa, VARTYPE vt, const CIMTYPE Type, T*) 
{
	long index, numElements, i;
	UINT uSize;
	BYTE *pValue;
	HRESULT hr;

	Array <T> array;

// 	PEG_METHOD_ENTER(TRC_WMIPROVIDER,"WMIValue::getArrayValueAux()");

	index = psa->rgsabound[0].lLbound;	// get the beginning index
	numElements = psa->rgsabound[0].cElements;	// elements in the array
	uSize = SafeArrayGetElemsize(psa);		// size of each element	

	pValue = new BYTE[uSize];
	void *pVal = (void *)pValue;

	hr = SafeArrayGetVartype(psa, &vt);

	for (i=0; ((i < numElements) && (SUCCEEDED(hr))); i++, index++)
	{
		hr = SafeArrayGetElement(psa, &index, pVal);

		if (SUCCEEDED(hr))
		{
			CIMValue cimValue = WMIValue(vt, pVal, Type);

			T x;
			cimValue.get(x);
			array.append(x);
		}

	}

	delete [] pValue;

	if (SUCCEEDED(hr))
	{
//		PEG_METHOD_EXIT();
		return CIMValue(array);
	}
	else
	{
		Tracer::trace(TRC_WMIPROVIDER,Tracer::LEVEL3,
			"WMIValue::getArrayValueAux() - SafeArray result is %x",  hr);
		throw CIMException(CIM_ERR_FAILED);
	}
}

/////////////////////////////////////////////////////////////////////////////
// WMIValue::getArrayValue- converts a variant array to a 
//		Pegasus CIMValue of type array
//		- uses all the SafeArray garbage
//	NOTE:
//		We are assuming one-dimensional arrays
//
// ///////////////////////////////////////////////////////////////////////////
CIMValue WMIValue::getArrayValue(const CComVariant& vValue, const CIMTYPE Type)
{

	LPSAFEARRAY psa = NULL;
	HRESULT hr;

	CIMTYPE type = Type & (~CIM_FLAG_ARRAY);

// 	PEG_METHOD_ENTER(TRC_WMIPROVIDER,"WMIValue::getArrayValue()");

	if (!(isArrayType(vValue.vt)))
	{
		return (getValue(vValue));
	}

	if (isReferenceType(vValue.vt))
	{
		psa = *V_ARRAYREF(&vValue);
	}
	else
	{
		psa = V_ARRAY(&vValue);
	}

	VARTYPE vt = VT_NULL;
	CIMType cimType;

	hr = SafeArrayGetVartype(psa, &vt);
//	CIMType cimType = WMIType(vt);

	if (VT_I4 == vt)
	// check for Mof and Automation type discrepancy
	{
		if (CIM_UINT16 == type)
		{
			cimType = CIMTYPE_UINT16;
		}
		else if (CIM_UINT32 == type)
		{
			cimType = CIMTYPE_UINT32;
		}
		else
		{
			cimType = vartypeToCIMType(vt);
		}
	}
	else
	{
		cimType = vartypeToCIMType(vt);
	}

	if (SUCCEEDED(hr))
	{
        switch (cimType)
        {
		case CIMTYPE_BOOLEAN: 
		    return getArrayValueAux(psa, vt, type, (Boolean*)0);

		case CIMTYPE_STRING:
		    return getArrayValueAux(psa, vt, type, (String*)0);

		case CIMTYPE_CHAR16:
		    return getArrayValueAux(psa, vt, type, (Char16*)0);

		case CIMTYPE_UINT8:
		    return getArrayValueAux(psa, vt, type, (Uint8*)0);

		case CIMTYPE_UINT16:
		    return getArrayValueAux(psa, vt, type, (Uint16*)0);

		case CIMTYPE_UINT32:
		    return getArrayValueAux(psa, vt, type, (Uint32*)0);

		case CIMTYPE_UINT64:
		    return getArrayValueAux(psa, vt, type, (Uint64*)0);

		case CIMTYPE_SINT8:
		    return getArrayValueAux(psa, vt, type, (Sint8*)0);

		case CIMTYPE_SINT16:
		    return getArrayValueAux(psa, vt, type, (Sint16*)0);

		case CIMTYPE_SINT32:
		    return getArrayValueAux(psa, vt, type, (Sint32*)0);

		case CIMTYPE_SINT64:
		    return getArrayValueAux(psa, vt, type, (Sint64*)0);

		case CIMTYPE_DATETIME:
		    return getArrayValueAux(psa, vt, type, (CIMDateTime*)0);

		case CIMTYPE_REAL32:
		    return getArrayValueAux(psa, vt, type, (Real32*)0);

		case CIMTYPE_REAL64:
		    return getArrayValueAux(psa, vt, type, (Real64*)0);

		default:
		    break;
		}
    }

	// if we get here, we are in trouble...
	throw CIMException(CIM_ERR_FAILED);

}


VARIANT WMIValue::toVariant()
{
	CComVariant cv;
	VARIANT v;
	cv.Detach(&v);
	return v;

}
	

PEGASUS_NAMESPACE_END

