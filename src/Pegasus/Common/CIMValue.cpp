//%/////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000 The Open Group, BMC Software, Tivoli Systems, IBM
//
// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation
// the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following conditions:
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
// THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.
//
//==============================================================================
//
// Author: Mike Brasher (mbrasher@bmc.com)
//
// Modified By:
//
//%/////////////////////////////////////////////////////////////////////////////

#include <cstring>
#include <cstdio>
#include <cassert>
#include <cctype>
#include "CIMValue.h"
#include "Indentor.h"
#include "Exception.h"
#include "XmlWriter.h"

PEGASUS_NAMESPACE_BEGIN

#define PEGASUS_ARRAY_T CIMValue
# include "ArrayImpl.h"
#undef PEGASUS_ARRAY_T

template<class T>
inline void _Inc(ArrayRep<T>* rep)
{
    ArrayRep<T>::inc(rep);
}

template<class T>
inline void _Dec(ArrayRep<T>* rep)
{
    ArrayRep<T>::dec(rep);
}

////////////////////////////////////////////////////////////////////////////////
//
// Local helper functions:
//
////////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------
//
// _SignedIntToStr()
//
//------------------------------------------------------------------------------

static void _SignedIntToStr(Sint64 x, char* result)
{
    if (!result)
	return;

    if (x == 0)
    {
	result[0] = '0';
	result[1] = '\0';
	return;
    }

    char buffer[256];
    Uint32 len = 0;
    Boolean negative = false;

    for (Uint32 i = 0; 1; i++)
    {
	Sint64 q = x / 10;
	Sint32 r = x % 10;

	if (q == 0 && r == 0)
	    break;

	if (r < 0)
	{
	    r = -r;
	    negative = true;
	}

	buffer[len++] = r + '0';

	x = q ;
    }

    buffer[len] = '\0';

    // If buffer was negative, prepend sign:

    char* q = result;

    if (negative)
	*q++ = '-';

    // Reverse the buffer:

    char* p = &buffer[len];

    while (len--)
	*q++ = *--p;

    *q++ = '\0';
}

//------------------------------------------------------------------------------
//
// _UnsignedIntToStr()
//
//------------------------------------------------------------------------------

static void _UnsignedIntToStr(Uint64 x, char* result)
{
    if (!result)
	return;

    if (x == 0)
    {
	result[0] = '0';
	result[1] = '\0';
	return;
    }

    char buffer[256];
    Uint32 len = 0;
    Boolean negative = false;

    for (Uint32 i = 0; 1; i++)
    {
	Sint64 q = x / 10;
	Sint32 r = x % 10;

	if (q == 0 && r == 0)
	    break;

	buffer[len++] = r + '0';

	x = q ;
    }

    buffer[len] = '\0';

    // Reverse the buffer onto output:

    char* q = result;

    char* p = &buffer[len];

    while (len--)
	*q++ = *--p;

    *q++ = '\0';
}

//------------------------------------------------------------------------------
//
// _toXml() routines:
//
//------------------------------------------------------------------------------

inline void _toXml(Array<Sint8>& out, Boolean x)
{
    out << (x ? "TRUE" : "FALSE");
}

template<class T>
inline void _integerToXml(Array<Sint8>& out, const T& x)
{
    char buffer[32];
    sprintf(buffer, "%d", x);
    out << (char*)buffer;
}

inline void _toXml(Array<Sint8>& out, Uint8 x) { _integerToXml(out, x); }

inline void _toXml(Array<Sint8>& out, Sint8 x) { _integerToXml(out, x); }

inline void _toXml(Array<Sint8>& out, Uint16 x) { _integerToXml(out, x); }

inline void _toXml(Array<Sint8>& out, Sint16 x) { _integerToXml(out, x); }

inline void _toXml(Array<Sint8>& out, Uint32 x) { _integerToXml(out, x); }

inline void _toXml(Array<Sint8>& out, Sint32 x) { _integerToXml(out, x); }

inline void _toXml(Array<Sint8>& out, Uint64 x) 
{ 
    char buffer[128];
    _UnsignedIntToStr(x, buffer);
    out << buffer;
}

inline void _toXml(Array<Sint8>& out, Sint64 x) 
{
    char buffer[128];
    _SignedIntToStr(x, buffer);
    out << buffer;
}

void _toXml(Array<Sint8>& out, Real32 x)
{
    // ATTN: Does this format match the CIM/XML format?
    char buffer[128];
    sprintf(buffer, "%f", x);
    out << buffer;
}

void _toXml(Array<Sint8>& out, Real64 x)
{
    char buffer[128];
    sprintf(buffer, "%f", x);
    out << buffer;
}

inline void _toXml(Array<Sint8>& out, Char16 x)
{
    XmlWriter::appendSpecial(out, x);
}

inline void _toXml(Array<Sint8>& out, const String& x)
{
    XmlWriter::appendSpecial(out, x);
}

inline void _toXml(Array<Sint8>& out, const CIMDateTime& x)
{
    out << x.getString();
}

template<class T>
void _toXml(Array<Sint8>& out, const T* p, Uint32 size)
{
    while (size--)
    {
	out << "<VALUE>";

	_toXml(out, *p++);

	out << "</VALUE>\n";
    }
}

template<class T>
void _toStr(Array<Sint8>& out, const T* p, Uint32 size)
{
    while (size--)
    {
	_toXml(out, *p++);
	out << " ";
    }
}

////////////////////////////////////////////////////////////////////////////////
//
// CIMValue
//
////////////////////////////////////////////////////////////////////////////////

CIMValue::CIMValue()
{
    _init();
}

CIMValue::CIMValue(const CIMValue& x) 
{ 
    _init(); 
    assign(x); 
}

CIMValue::~CIMValue()
{
    clear();
}

void CIMValue::assign(const CIMValue& x)
{
    if (this == &x)
	return;

    clear();

    _type = x._type;
    _isArray = x._isArray;
    _u._voidPtr = 0;

    if (_isArray)
    {
	switch (_type)
	{
	    case CIMType::BOOLEAN:
		_Inc(_u._booleanArray = x._u._booleanArray);
		break;

	    case CIMType::UINT8:
		_Inc(_u._uint8Array = x._u._uint8Array);
		break;

	    case CIMType::SINT8:
		_Inc(_u._sint8Array = x._u._sint8Array);
		break;

	    case CIMType::UINT16:
		_Inc(_u._uint16Array = x._u._uint16Array);
		break;

	    case CIMType::SINT16:
		_Inc(_u._sint16Array = x._u._sint16Array);
		break;

	    case CIMType::UINT32:
		_Inc(_u._uint32Array = x._u._uint32Array);
		break;

	    case CIMType::SINT32:
		_Inc(_u._sint32Array = x._u._sint32Array);
		break;

	    case CIMType::UINT64:
		_Inc(_u._uint64Array = x._u._uint64Array);
		break;

	    case CIMType::SINT64:
		_Inc(_u._sint64Array = x._u._sint64Array);
		break;

	    case CIMType::REAL32:
		_Inc(_u._real32Array = x._u._real32Array);
		break;

	    case CIMType::REAL64:
		_Inc(_u._real64Array = x._u._real64Array);
		break;

	    case CIMType::CHAR16:
		_Inc(_u._char16Array = x._u._char16Array);
		break;

	    case CIMType::STRING:
		_Inc(_u._stringArray = x._u._stringArray);
		break;

	    case CIMType::DATETIME:
		_Inc(_u._dateTimeArray = x._u._dateTimeArray);
		break;
	}
    }
    else
    {
	switch (_type)
	{
	    case CIMType::NONE:
		break;

	    case CIMType::BOOLEAN:
		_u._booleanValue = x._u._booleanValue;
		break;

	    case CIMType::UINT8:
		_u._uint8Value = x._u._uint8Value;
		break;

	    case CIMType::SINT8:
		_u._sint8Value = x._u._sint8Value;
		break;

	    case CIMType::UINT16:
		_u._uint16Value = x._u._uint16Value;
		break;

	    case CIMType::SINT16:
		_u._sint16Value = x._u._sint16Value;
		break;

	    case CIMType::UINT32:
		_u._uint32Value = x._u._uint32Value;
		break;

	    case CIMType::SINT32:
		_u._sint32Value = x._u._sint32Value;
		break;

	    case CIMType::UINT64:
		_u._uint64Value = x._u._uint64Value;
		break;

	    case CIMType::SINT64:
		_u._sint64Value = x._u._sint64Value;
		break;

	    case CIMType::REAL32:
		_u._real32Value = x._u._real32Value;
		break;

	    case CIMType::REAL64:
		_u._real64Value = x._u._real64Value;
		break;

	    case CIMType::CHAR16:
		_u._char16Value = x._u._char16Value;
		break;

	    case CIMType::STRING:
		_u._stringValue = new String(*(x._u._stringValue));
		break;

	    case CIMType::DATETIME:
		_u._dateTimeValue = new CIMDateTime(*(x._u._dateTimeValue));
		break;

	    case CIMType::REFERENCE:
		_u._referenceValue 
		    = new CIMReference(*(x._u._referenceValue));
		break;
	}
    }
}

Uint32 CIMValue::getArraySize() const
{
    if (!_isArray)
	return 0;

    switch (_type)
    {
	case CIMType::NONE:
	    return 0;
	    break;

	case CIMType::BOOLEAN:
	    return _u._booleanArray->size;
	    break;

	case CIMType::UINT8:
	    return _u._uint8Array->size;
	    break;

	case CIMType::SINT8:
	    return _u._sint8Array->size;
	    break;

	case CIMType::UINT16:
	    return _u._uint16Array->size;
	    break;

	case CIMType::SINT16:
	    return _u._sint16Array->size;
	    break;

	case CIMType::UINT32:
	    return _u._uint32Array->size;
	    break;

	case CIMType::SINT32:
	    return _u._sint32Array->size;
	    break;

	case CIMType::UINT64:
	    return _u._uint64Array->size;
	    break;

	case CIMType::SINT64:
	    return _u._sint64Array->size;
	    break;

	case CIMType::REAL32:
	    return _u._real32Array->size;
	    break;

	case CIMType::REAL64:
	    return _u._real64Array->size;
	    break;

	case CIMType::CHAR16:
	    return _u._char16Array->size;
	    break;

	case CIMType::STRING:
	    return _u._stringArray->size;
	    break;

	case CIMType::DATETIME:
	    return _u._dateTimeArray->size;
	    break;

	case CIMType::REFERENCE:
	    return 0;
    }

    // Unreachable!
    return 0;
}

void CIMValue::clear()
{
    if (_isArray)
    {
	switch (_type)
	{
	    case CIMType::BOOLEAN:
		_Dec(_u._booleanArray);
		break;

	    case CIMType::UINT8:
		_Dec(_u._uint8Array);
		break;

	    case CIMType::SINT8:
		_Dec(_u._sint8Array);
		break;

	    case CIMType::UINT16:
		_Dec(_u._uint16Array);
		break;

	    case CIMType::SINT16:
		_Dec(_u._sint16Array);
		break;

	    case CIMType::UINT32:
		_Dec(_u._uint32Array);
		break;

	    case CIMType::SINT32:
		_Dec(_u._sint32Array);
		break;

	    case CIMType::UINT64:
		_Dec(_u._uint64Array);
		break;

	    case CIMType::SINT64:
		_Dec(_u._sint64Array);
		break;

	    case CIMType::REAL32:
		_Dec(_u._real32Array);
		break;

	    case CIMType::REAL64:
		_Dec(_u._real64Array);
		break;

	    case CIMType::CHAR16:
		_Dec(_u._char16Array);
		break;

	    case CIMType::STRING:
		_Dec(_u._stringArray);
		break;

	    case CIMType::DATETIME:
		_Dec(_u._dateTimeArray);
		break;
	}
    }
    else
    {
	switch (_type)
	{
	    case CIMType::BOOLEAN:
	    case CIMType::UINT8:
	    case CIMType::SINT8:
	    case CIMType::UINT16:
	    case CIMType::SINT16:
	    case CIMType::UINT32:
	    case CIMType::SINT32:
	    case CIMType::UINT64:
	    case CIMType::SINT64:
	    case CIMType::REAL32:
	    case CIMType::REAL64:
	    case CIMType::CHAR16:
		break;

	    case CIMType::STRING:
		delete _u._stringValue;
		break;

	    case CIMType::DATETIME:
		delete _u._dateTimeValue;
		break;

	    case CIMType::REFERENCE:
		delete _u._referenceValue;
		break;
	}
    }

    _init();
}

void CIMValue::toXml(Array<Sint8>& out) const
{
    if (_isArray)
    {
	out << "<VALUE.ARRAY>\n";

	switch (_type)
	{
	    case CIMType::BOOLEAN:
	    {
		for (Uint32 i = 0, n = _u._booleanArray->size; i < n; i++)
		{
		    out << "<VALUE>";
		    _toXml(out, Boolean(_u._booleanArray->data()[i]));
		    out << "</VALUE>\n";
		}
		break;
	    }

	    case CIMType::UINT8:
		_toXml(out, _u._uint8Array->data(), _u._uint8Array->size);
		break;

	    case CIMType::SINT8:
		_toXml(out, _u._sint8Array->data(), _u._sint8Array->size);
		break;

	    case CIMType::UINT16:
		_toXml(out, _u._uint16Array->data(), _u._uint16Array->size);
		break;

	    case CIMType::SINT16:
		_toXml(out, _u._sint16Array->data(), _u._sint16Array->size);
		break;

	    case CIMType::UINT32:
		_toXml(out, _u._uint32Array->data(), _u._uint32Array->size);
		break;

	    case CIMType::SINT32:
		_toXml(out, _u._sint32Array->data(), _u._sint32Array->size);
		break;

	    case CIMType::UINT64:
		_toXml(out, _u._uint64Array->data(), _u._uint64Array->size);
		break;

	    case CIMType::SINT64:
		_toXml(out, _u._sint64Array->data(), _u._sint64Array->size);
		break;

	    case CIMType::REAL32:
		_toXml(out, _u._real32Array->data(), _u._real32Array->size);
		break;

	    case CIMType::REAL64:
		_toXml(out, _u._real64Array->data(), _u._real64Array->size);
		break;

	    case CIMType::CHAR16:
		_toXml(out, _u._char16Array->data(), _u._char16Array->size);
		break;

	    case CIMType::STRING:
		_toXml(out, _u._stringArray->data(), _u._stringArray->size);
		break;

	    case CIMType::DATETIME:
		_toXml(out, _u._dateTimeArray->data(), _u._dateTimeArray->size);
		break;
	}

	out << "</VALUE.ARRAY>\n";
    }
    else if (_type == CIMType::REFERENCE)
    {
	_u._referenceValue->toXml(out);
    }
    else
    {
	out << "<VALUE>";

	switch (_type)
	{
	    case CIMType::BOOLEAN:
		_toXml(out, Boolean(_u._booleanValue != 0));
		break;

	    case CIMType::UINT8:
		_toXml(out, _u._uint8Value);
		break;

	    case CIMType::SINT8:
		_toXml(out, _u._sint8Value);
		break;

	    case CIMType::UINT16:
		_toXml(out, _u._uint16Value);
		break;

	    case CIMType::SINT16:
		_toXml(out, _u._sint16Value);
		break;

	    case CIMType::UINT32:
		_toXml(out, _u._uint32Value);
		break;

	    case CIMType::SINT32:
		_toXml(out, _u._sint32Value);
		break;

	    case CIMType::UINT64:
		_toXml(out, _u._uint64Value);
		break;

	    case CIMType::SINT64:
		_toXml(out, _u._sint64Value);
		break;

	    case CIMType::REAL32:
		_toXml(out, _u._real32Value);
		break;

	    case CIMType::REAL64:
		_toXml(out, _u._real64Value);
		break;

	    case CIMType::CHAR16:
		_toXml(out, Char16(_u._char16Value));
		break;

	    case CIMType::STRING:
		_toXml(out, *_u._stringValue);
		break;

	    case CIMType::DATETIME:
		_toXml(out, *_u._dateTimeValue);
		break;
	}

	out << "</VALUE>\n";
    }
}

void CIMValue::print(PEGASUS_STD(ostream) &os) const
{
    Array<Sint8> tmp;
    toXml(tmp);
    tmp.append('\0');
    os << tmp.getData() << PEGASUS_STD(endl);
}

void CIMValue::set(Boolean x)
{
    clear();
    _u._booleanValue = (Uint8)x;
    _type = CIMType::BOOLEAN;
}

void CIMValue::set(Uint8 x)
{
    clear();
    _u._uint8Value = x;
    _type = CIMType::UINT8;
}

void CIMValue::set(Sint8 x)
{
    clear();
    _u._sint8Value = x;
    _type = CIMType::SINT8;
}

void CIMValue::set(Uint16 x)
{
    clear();
    _u._uint16Value = x;
    _type = CIMType::UINT16;
}

void CIMValue::set(Sint16 x)
{
    clear();
    _u._sint16Value = x;
    _type = CIMType::SINT16;
}

void CIMValue::set(Uint32 x)
{
    clear();
    _u._uint32Value = x;
    _type = CIMType::UINT32;
}

void CIMValue::set(Sint32 x)
{
    clear();
    _u._sint32Value = x;
    _type = CIMType::SINT32;
}

void CIMValue::set(Uint64 x)
{
    clear();
    _u._uint64Value = x;
    _type = CIMType::UINT64;
}

void CIMValue::set(Sint64 x)
{
    clear();
    _u._sint64Value = x;
    _type = CIMType::SINT64;
}

void CIMValue::set(Real32 x)
{
    clear();
    _u._real32Value = x;
    _type = CIMType::REAL32;
}

void CIMValue::set(Real64 x)
{
    clear();
    _u._real64Value = x;
    _type = CIMType::REAL64;
}

void CIMValue::set(const Char16& x)
{
    clear();
    _u._char16Value = x;
    _type = CIMType::CHAR16;
}

void CIMValue::set(const String& x)
{
    clear();
    _u._stringValue = new String(x);
    _type = CIMType::STRING;
}

void CIMValue::set(const char* x) 
{ 
    set(String(x)); 
}

void CIMValue::set(const CIMDateTime& x)
{
    clear();
    _u._dateTimeValue = new CIMDateTime(x);
    _type = CIMType::DATETIME;
}

void CIMValue::set(const CIMReference& x)
{
    clear();
    _u._referenceValue = new CIMReference(x);
    _type = CIMType::REFERENCE;
}

void CIMValue::set(const Array<Boolean>& x)
{
    clear();
    _Inc(_u._booleanArray = x._rep);
    _type = CIMType::BOOLEAN;
    _isArray = true;
}

void CIMValue::set(const Array<Uint8>& x)
{
    clear();
    _Inc(_u._uint8Array = x._rep);
    _type = CIMType::UINT8;
    _isArray = true;
}

void CIMValue::set(const Array<Sint8>& x)
{
    clear();
    _Inc(_u._sint8Array = x._rep);
    _type = CIMType::SINT8;
    _isArray = true;
}

void CIMValue::set(const Array<Uint16>& x)
{
    clear();
    _Inc(_u._uint16Array = x._rep);
    _type = CIMType::UINT16;
    _isArray = true;
}

void CIMValue::set(const Array<Sint16>& x)
{
    clear();
    _Inc(_u._sint16Array = x._rep);
    _type = CIMType::SINT16;
    _isArray = true;
}

void CIMValue::set(const Array<Uint32>& x)
{
    clear();
    _Inc(_u._uint32Array = x._rep);
    _type = CIMType::UINT32;
    _isArray = true;
}

void CIMValue::set(const Array<Sint32>& x)
{
    clear();
    _Inc(_u._sint32Array = x._rep);
    _type = CIMType::SINT32;
    _isArray = true;
}

void CIMValue::set(const Array<Uint64>& x)
{
    clear();
    _Inc(_u._uint64Array = x._rep);
    _type = CIMType::UINT64;
    _isArray = true;
}

void CIMValue::set(const Array<Sint64>& x)
{
    clear();
    _Inc(_u._sint64Array = x._rep);
    _type = CIMType::SINT64;
    _isArray = true;
}

void CIMValue::set(const Array<Real32>& x)
{
    clear();
    _Inc(_u._real32Array = x._rep);
    _type = CIMType::REAL32;
    _isArray = true;
}

void CIMValue::set(const Array<Real64>& x)
{
    clear();
    _Inc(_u._real64Array = x._rep);
    _type = CIMType::REAL64;
    _isArray = true;
}

void CIMValue::set(const Array<Char16>& x)
{
    clear();
    _Inc(_u._char16Array = x._rep);
    _type = CIMType::CHAR16;
    _isArray = true;
}

void CIMValue::set(const Array<String>& x)
{
    clear();
    _Inc(_u._stringArray = x._rep);
    _type = CIMType::STRING;
    _isArray = true;
} 

void CIMValue::set(const Array<CIMDateTime>& x)
{
    clear();
    _Inc(_u._dateTimeArray = x._rep);
    _type = CIMType::DATETIME;
    _isArray = true;
}

void CIMValue::get(Boolean& x) const
{
    if (_type != CIMType::BOOLEAN || _isArray)
	throw TypeMismatch();

    x = _u._booleanValue != 0;
}

void CIMValue::get(Uint8& x) const
{
    if (_type != CIMType::UINT8 || _isArray)
	throw TypeMismatch();

    x = _u._uint8Value;
}

void CIMValue::get(Sint8& x) const
{
    if (_type != CIMType::SINT8 || _isArray)
	throw TypeMismatch();

    x = _u._sint8Value;
}

void CIMValue::get(Uint16& x) const
{
    if (_type != CIMType::UINT16 || _isArray)
	throw TypeMismatch();

    x = _u._uint16Value;
}

void CIMValue::get(Sint16& x) const
{
    if (_type != CIMType::SINT16 || _isArray)
	throw TypeMismatch();

    x = _u._sint16Value;
}

void CIMValue::get(Uint32& x) const
{
    if (_type != CIMType::UINT32 || _isArray)
	throw TypeMismatch();

    x = _u._uint32Value;
}

void CIMValue::get(Sint32& x) const
{
    if (_type != CIMType::SINT32 || _isArray)
	throw TypeMismatch();

    x = _u._sint32Value;
}

void CIMValue::get(Uint64& x) const
{
    if (_type != CIMType::UINT64 || _isArray)
	throw TypeMismatch();

    x = _u._uint64Value;
}

void CIMValue::get(Sint64& x) const
{
    if (_type != CIMType::SINT64 || _isArray)
	throw TypeMismatch();

    x = _u._sint64Value;
}

void CIMValue::get(Real32& x) const
{
    if (_type != CIMType::REAL32 || _isArray)
	throw TypeMismatch();

    x = _u._real32Value;
}

void CIMValue::get(Real64& x) const
{
    if (_type != CIMType::REAL64 || _isArray)
	throw TypeMismatch();

    x = _u._real64Value;
}

void CIMValue::get(Char16& x) const
{
    if (_type != CIMType::CHAR16 || _isArray)
	throw TypeMismatch();

    x = _u._char16Value;
}

void CIMValue::get(String& x) const
{
    if (_type != CIMType::STRING || _isArray)
	throw TypeMismatch();

    x = *_u._stringValue;
}

void CIMValue::get(CIMDateTime& x) const
{
    if (_type != CIMType::DATETIME || _isArray)
	throw TypeMismatch();

    x = *_u._dateTimeValue;
}

void CIMValue::get(CIMReference& x) const
{
    if (_type != CIMType::REFERENCE || _isArray)
	throw TypeMismatch();

    x = *_u._referenceValue;
}

void CIMValue::get(Array<Boolean>& x) const
{
    if (_type != CIMType::BOOLEAN || !_isArray)
	throw TypeMismatch();

    x.set(_u._booleanArray);
}

void CIMValue::get(Array<Uint8>& x) const
{
    if (_type != CIMType::UINT8 || !_isArray)
	throw TypeMismatch();

    x.set(_u._uint8Array);
}

void CIMValue::get(Array<Sint8>& x) const
{
    if (_type != CIMType::SINT8 || !_isArray)
	throw TypeMismatch();

    x.set(_u._sint8Array);
}

void CIMValue::get(Array<Uint16>& x) const
{
    if (_type != CIMType::UINT16 || !_isArray)
	throw TypeMismatch();

    x.set(_u._uint16Array);
}

void CIMValue::get(Array<Sint16>& x) const
{
    if (_type != CIMType::SINT16 || !_isArray)
	throw TypeMismatch();

    x.set(_u._sint16Array);
}

void CIMValue::get(Array<Uint32>& x) const
{
    if (_type != CIMType::UINT32 || !_isArray)
	throw TypeMismatch();

    x.set(_u._uint32Array);
}

void CIMValue::get(Array<Sint32>& x) const
{
    if (_type != CIMType::SINT32 || !_isArray)
	throw TypeMismatch();

    x.set(_u._sint32Array);
}

void CIMValue::get(Array<Uint64>& x) const
{
    if (_type != CIMType::UINT64 || !_isArray)
	throw TypeMismatch();

    x.set(_u._uint64Array);
}

void CIMValue::get(Array<Sint64>& x) const
{
    if (_type != CIMType::SINT64 || !_isArray)
	throw TypeMismatch();

    x.set(_u._sint64Array);
}

void CIMValue::get(Array<Real32>& x) const
{
    if (_type != CIMType::REAL32 || !_isArray)
	throw TypeMismatch();

    x.set(_u._real32Array);
}

void CIMValue::get(Array<Real64>& x) const
{
    if (_type != CIMType::REAL64 || !_isArray)
	throw TypeMismatch();

    x.set(_u._real64Array);
}

void CIMValue::get(Array<Char16>& x) const
{
    if (_type != CIMType::CHAR16 || !_isArray)
	throw TypeMismatch();

    x.set(_u._char16Array);
}

void CIMValue::get(Array<String>& x) const
{
    if (_type != CIMType::STRING || !_isArray)
	throw TypeMismatch();

    x.set(_u._stringArray);
}

void CIMValue::get(Array<CIMDateTime>& x) const
{
    if (_type != CIMType::DATETIME || !_isArray)
	throw TypeMismatch();

    x.set(_u._dateTimeArray);
}

void CIMValue::_init()
{
    _type = CIMType::NONE;
    _isArray = false;
    _u._voidPtr = 0;
}

Boolean operator==(const CIMValue& x, const CIMValue& y)
{
    if (!x.typeCompatible(y))
	return false;

    if (x._isArray)
    {
	switch (x._type)
	{
	    case CIMType::BOOLEAN:
		return Array<Boolean>(x._u._booleanArray) ==
		    Array<Boolean>(y._u._booleanArray);

	    case CIMType::UINT8:
		return Array<Uint8>(x._u._uint8Array) ==
		    Array<Uint8>(y._u._uint8Array);

	    case CIMType::SINT8:
		return Array<Sint8>(x._u._sint8Array) ==
		    Array<Sint8>(y._u._sint8Array);

	    case CIMType::UINT16:
		return Array<Uint16>(x._u._uint16Array) ==
		    Array<Uint16>(y._u._uint16Array);

	    case CIMType::SINT16:
		return Array<Sint16>(x._u._sint16Array) ==
		    Array<Sint16>(y._u._sint16Array);

	    case CIMType::UINT32:
		return Array<Uint32>(x._u._uint32Array) ==
		    Array<Uint32>(y._u._uint32Array);

	    case CIMType::SINT32:
		return Array<Sint32>(x._u._sint32Array) ==
		    Array<Sint32>(y._u._sint32Array);

	    case CIMType::UINT64:
		return Array<Uint64>(x._u._uint64Array) ==
		    Array<Uint64>(y._u._uint64Array);

	    case CIMType::SINT64:
		return Array<Sint64>(x._u._sint64Array) ==
		    Array<Sint64>(y._u._sint64Array);

	    case CIMType::REAL32:
		return Array<Real32>(x._u._real32Array) ==
		    Array<Real32>(y._u._real32Array);

	    case CIMType::REAL64:
		return Array<Real64>(x._u._real64Array) ==
		    Array<Real64>(y._u._real64Array);

	    case CIMType::CHAR16:
		return Array<Char16>(x._u._char16Array) ==
		    Array<Char16>(y._u._char16Array);

	    case CIMType::STRING:
		return Array<String>(x._u._stringArray) ==
		    Array<String>(y._u._stringArray);

	    case CIMType::DATETIME:
		return Array<CIMDateTime>(x._u._dateTimeArray) ==
		    Array<CIMDateTime>(y._u._dateTimeArray);
	}
    }
    else
    {
	switch (x._type)
	{
	    case CIMType::BOOLEAN:
		return x._u._booleanValue == y._u._booleanValue;

	    case CIMType::UINT8:
		return x._u._uint8Value == y._u._uint8Value;

	    case CIMType::SINT8:
		return x._u._sint8Value == y._u._sint8Value;

	    case CIMType::UINT16:
		return x._u._uint16Value == y._u._uint16Value;

	    case CIMType::SINT16:
		return x._u._sint16Value == y._u._sint16Value;

	    case CIMType::UINT32:
		return x._u._uint32Value == y._u._uint32Value;

	    case CIMType::SINT32:
		return x._u._sint32Value == y._u._sint32Value;

	    case CIMType::UINT64:
		return x._u._uint64Value == y._u._uint64Value;

	    case CIMType::SINT64:
		return x._u._sint64Value == y._u._sint64Value;

	    case CIMType::REAL32:
		return x._u._real32Value == y._u._real32Value;

	    case CIMType::REAL64:
		return x._u._real64Value == y._u._real64Value;

	    case CIMType::CHAR16:
		return x._u._char16Value == y._u._char16Value;

	    case CIMType::STRING:
		return String::equal(*x._u._stringValue, *y._u._stringValue);

	    case CIMType::DATETIME:
		return *x._u._dateTimeValue == *y._u._dateTimeValue;

	    case CIMType::REFERENCE:
		return *x._u._referenceValue == *y._u._referenceValue;
	}
    }

    // Unreachable!
    return false;
}

void CIMValue::setNullValue(CIMType type, Boolean isArray, Uint32 arraySize)
{
    clear();

    if (isArray)
    {
	switch (type)
	{
	    case CIMType::BOOLEAN:
		set(Array<Boolean>(arraySize));
		break;

	    case CIMType::UINT8:
		set(Array<Uint8>(arraySize));
		break;

	    case CIMType::SINT8:
		set(Array<Sint8>(arraySize));
		break;

	    case CIMType::UINT16:
		set(Array<Uint16>(arraySize));
		break;

	    case CIMType::SINT16:
		set(Array<Sint16>(arraySize));
		break;

	    case CIMType::UINT32:
		set(Array<Uint32>(arraySize));
		break;

	    case CIMType::SINT32:
		set(Array<Sint32>(arraySize));
		break;

	    case CIMType::UINT64:
		set(Array<Uint64>(arraySize));
		break;

	    case CIMType::SINT64:
		set(Array<Sint64>(arraySize));
		break;

	    case CIMType::REAL32:
		set(Array<Real32>(arraySize));
		break;

	    case CIMType::REAL64:
		set(Array<Real64>(arraySize));
		break;

	    case CIMType::CHAR16:
		set(Array<Char16>(arraySize));
		break;

	    case CIMType::STRING:
		set(Array<String>(arraySize));
		break;

	    case CIMType::DATETIME:
		set(Array<CIMDateTime>(arraySize));
		break;
	}
    }
    else
    {
	switch (type)
	{
	    case CIMType::BOOLEAN:
		set(false);
		break;

	    case CIMType::UINT8:
		set(Uint8(0));
		break;

	    case CIMType::SINT8:
		set(Sint8(0));
		break;

	    case CIMType::UINT16:
		set(Uint16(0));
		break;

	    case CIMType::SINT16:
		set(Sint16(0));
		break;

	    case CIMType::UINT32:
		set(Uint32(0));
		break;

	    case CIMType::SINT32:
		set(Sint32(0));
		break;

	    case CIMType::UINT64:
		set(Uint64(0));
		break;

	    case CIMType::SINT64:
		set(Sint64(0));
		break;

	    case CIMType::REAL32:
		set(Real32(0.0));
		break;

	    case CIMType::REAL64:
		set(Real64(0.0));
		break;

	    case CIMType::CHAR16:
		set(Char16(0));
		break;

	    case CIMType::STRING:
		set(String());
		break;

	    case CIMType::DATETIME:
		set(CIMDateTime());
		break;

	    case CIMType::REFERENCE:
		set(CIMReference());
		break;
	}
    }
}

String CIMValue::toString() const
{
    Array<Sint8> out;

    if (_isArray)
    {
	switch (_type)
	{
	    case CIMType::BOOLEAN:
	    {
		for (Uint32 i = 0, n = _u._booleanArray->size; i < n; i++)
		{
		    _toXml(out, Boolean(_u._booleanArray->data()[i]));

		    out << " ";
		}
		break;
	    }

	    case CIMType::UINT8:
		_toStr(out, _u._uint8Array->data(), _u._uint8Array->size);
		break;

	    case CIMType::SINT8:
		_toStr(out, _u._sint8Array->data(), _u._sint8Array->size);
		break;

	    case CIMType::UINT16:
		_toStr(out, _u._uint16Array->data(), _u._uint16Array->size);
		break;

	    case CIMType::SINT16:
		_toStr(out, _u._sint16Array->data(), _u._sint16Array->size);
		break;

	    case CIMType::UINT32:
		_toStr(out, _u._uint32Array->data(), _u._uint32Array->size);
		break;

	    case CIMType::SINT32:
		_toStr(out, _u._sint32Array->data(), _u._sint32Array->size);
		break;

	    case CIMType::UINT64:
		_toStr(out, _u._uint64Array->data(), _u._uint64Array->size);
		break;

	    case CIMType::SINT64:
		_toStr(out, _u._sint64Array->data(), _u._sint64Array->size);
		break;

	    case CIMType::REAL32:
		_toStr(out, _u._real32Array->data(), _u._real32Array->size);
		break;

	    case CIMType::REAL64:
		_toStr(out, _u._real64Array->data(), _u._real64Array->size);
		break;

	    case CIMType::CHAR16:
		_toStr(out, _u._char16Array->data(), _u._char16Array->size);
		break;

	    case CIMType::STRING:
		_toStr(out, _u._stringArray->data(), _u._stringArray->size);
		break;

	    case CIMType::DATETIME:
		_toStr(out, _u._dateTimeArray->data(), _u._dateTimeArray->size);
		break;
	}
    }
    else if (_type == CIMType::REFERENCE)
    {
	return _u._referenceValue->toString();
    }
    else
    {
	switch (_type)
	{
	    case CIMType::BOOLEAN:
		_toXml(out, Boolean(_u._booleanValue != 0));
		break;

	    case CIMType::UINT8:
		_toXml(out, _u._uint8Value);
		break;

	    case CIMType::SINT8:
		_toXml(out, _u._sint8Value);
		break;

	    case CIMType::UINT16:
		_toXml(out, _u._uint16Value);
		break;

	    case CIMType::SINT16:
		_toXml(out, _u._sint16Value);
		break;

	    case CIMType::UINT32:
		_toXml(out, _u._uint32Value);
		break;

	    case CIMType::SINT32:
		_toXml(out, _u._sint32Value);
		break;

	    case CIMType::UINT64:
		_toXml(out, _u._uint64Value);
		break;

	    case CIMType::SINT64:
		_toXml(out, _u._sint64Value);
		break;

	    case CIMType::REAL32:
		_toXml(out, _u._real32Value);
		break;

	    case CIMType::REAL64:
		_toXml(out, _u._real64Value);
		break;

	    case CIMType::CHAR16:
		_toXml(out, Char16(_u._char16Value));
		break;

	    case CIMType::STRING:
		_toXml(out, *_u._stringValue);
		break;

	    case CIMType::DATETIME:
		_toXml(out, *_u._dateTimeValue);
		break;
	}
    }

    out.append('\0');
    return out.getData();
}

PEGASUS_NAMESPACE_END
