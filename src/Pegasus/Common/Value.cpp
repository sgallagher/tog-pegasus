//BEGIN_LICENSE
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
//END_LICENSE
//BEGIN_HISTORY
//
// Author:
//
// $Log: Value.cpp,v $
// Revision 1.1  2001/01/14 19:53:20  mike
// Initial revision
//
//
//END_HISTORY

#include <cstring>
#include <cstdio>
#include <cassert>
#include <cctype>
#include "Value.h"
#include "Indentor.h"
#include "Exception.h"
#include "XmlWriter.h"

PEGASUS_NAMESPACE_BEGIN

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

inline void _toXml(Array<Sint8>& out, const DateTime& x)
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
// Value
//
////////////////////////////////////////////////////////////////////////////////

Value::Value()
{
    _init();
}

Value::Value(const Value& x) 
{ 
    _init(); 
    assign(x); 
}

Value::~Value()
{
    clear();
}

void Value::assign(const Value& x)
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
	    case Type::BOOLEAN:
		_Inc(_u._booleanArray = x._u._booleanArray);
		break;

	    case Type::UINT8:
		_Inc(_u._uint8Array = x._u._uint8Array);
		break;

	    case Type::SINT8:
		_Inc(_u._sint8Array = x._u._sint8Array);
		break;

	    case Type::UINT16:
		_Inc(_u._uint16Array = x._u._uint16Array);
		break;

	    case Type::SINT16:
		_Inc(_u._sint16Array = x._u._sint16Array);
		break;

	    case Type::UINT32:
		_Inc(_u._uint32Array = x._u._uint32Array);
		break;

	    case Type::SINT32:
		_Inc(_u._sint32Array = x._u._sint32Array);
		break;

	    case Type::UINT64:
		_Inc(_u._uint64Array = x._u._uint64Array);
		break;

	    case Type::SINT64:
		_Inc(_u._sint64Array = x._u._sint64Array);
		break;

	    case Type::REAL32:
		_Inc(_u._real32Array = x._u._real32Array);
		break;

	    case Type::REAL64:
		_Inc(_u._real64Array = x._u._real64Array);
		break;

	    case Type::CHAR16:
		_Inc(_u._char16Array = x._u._char16Array);
		break;

	    case Type::STRING:
		_Inc(_u._stringArray = x._u._stringArray);
		break;

	    case Type::DATETIME:
		_Inc(_u._dateTimeArray = x._u._dateTimeArray);
		break;
	}
    }
    else
    {
	switch (_type)
	{
	    case Type::NONE:
		break;

	    case Type::BOOLEAN:
		_u._booleanValue = x._u._booleanValue;
		break;

	    case Type::UINT8:
		_u._uint8Value = x._u._uint8Value;
		break;

	    case Type::SINT8:
		_u._sint8Value = x._u._sint8Value;
		break;

	    case Type::UINT16:
		_u._uint16Value = x._u._uint16Value;
		break;

	    case Type::SINT16:
		_u._sint16Value = x._u._sint16Value;
		break;

	    case Type::UINT32:
		_u._uint32Value = x._u._uint32Value;
		break;

	    case Type::SINT32:
		_u._sint32Value = x._u._sint32Value;
		break;

	    case Type::UINT64:
		_u._uint64Value = x._u._uint64Value;
		break;

	    case Type::SINT64:
		_u._sint64Value = x._u._sint64Value;
		break;

	    case Type::REAL32:
		_u._real32Value = x._u._real32Value;
		break;

	    case Type::REAL64:
		_u._real64Value = x._u._real64Value;
		break;

	    case Type::CHAR16:
		_u._char16Value = x._u._char16Value;
		break;

	    case Type::STRING:
		_u._stringValue = new String(*(x._u._stringValue));
		break;

	    case Type::DATETIME:
		_u._dateTimeValue = new DateTime(*(x._u._dateTimeValue));
		break;

	    case Type::REFERENCE:
		_u._referenceValue 
		    = new Reference(*(x._u._referenceValue));
		break;
	}
    }
}

Uint32 Value::getArraySize() const
{
    if (!_isArray)
	return 0;

    switch (_type)
    {
	case Type::NONE:
	    return 0;
	    break;

	case Type::BOOLEAN:
	    return _u._booleanArray->size;
	    break;

	case Type::UINT8:
	    return _u._uint8Array->size;
	    break;

	case Type::SINT8:
	    return _u._sint8Array->size;
	    break;

	case Type::UINT16:
	    return _u._uint16Array->size;
	    break;

	case Type::SINT16:
	    return _u._sint16Array->size;
	    break;

	case Type::UINT32:
	    return _u._uint32Array->size;
	    break;

	case Type::SINT32:
	    return _u._sint32Array->size;
	    break;

	case Type::UINT64:
	    return _u._uint64Array->size;
	    break;

	case Type::SINT64:
	    return _u._sint64Array->size;
	    break;

	case Type::REAL32:
	    return _u._real32Array->size;
	    break;

	case Type::REAL64:
	    return _u._real64Array->size;
	    break;

	case Type::CHAR16:
	    return _u._char16Array->size;
	    break;

	case Type::STRING:
	    return _u._stringArray->size;
	    break;

	case Type::DATETIME:
	    return _u._dateTimeArray->size;
	    break;

	case Type::REFERENCE:
	    return 0;
    }

    // Unreachable!
    return 0;
}

void Value::clear()
{
    if (_isArray)
    {
	switch (_type)
	{
	    case Type::BOOLEAN:
		_Dec(_u._booleanArray);
		break;

	    case Type::UINT8:
		_Dec(_u._uint8Array);
		break;

	    case Type::SINT8:
		_Dec(_u._sint8Array);
		break;

	    case Type::UINT16:
		_Dec(_u._uint16Array);
		break;

	    case Type::SINT16:
		_Dec(_u._sint16Array);
		break;

	    case Type::UINT32:
		_Dec(_u._uint32Array);
		break;

	    case Type::SINT32:
		_Dec(_u._sint32Array);
		break;

	    case Type::UINT64:
		_Dec(_u._uint64Array);
		break;

	    case Type::SINT64:
		_Dec(_u._sint64Array);
		break;

	    case Type::REAL32:
		_Dec(_u._real32Array);
		break;

	    case Type::REAL64:
		_Dec(_u._real64Array);
		break;

	    case Type::CHAR16:
		_Dec(_u._char16Array);
		break;

	    case Type::STRING:
		_Dec(_u._stringArray);
		break;

	    case Type::DATETIME:
		_Dec(_u._dateTimeArray);
		break;
	}
    }
    else
    {
	switch (_type)
	{
	    case Type::BOOLEAN:
	    case Type::UINT8:
	    case Type::SINT8:
	    case Type::UINT16:
	    case Type::SINT16:
	    case Type::UINT32:
	    case Type::SINT32:
	    case Type::UINT64:
	    case Type::SINT64:
	    case Type::REAL32:
	    case Type::REAL64:
	    case Type::CHAR16:
		break;

	    case Type::STRING:
		delete _u._stringValue;
		break;

	    case Type::DATETIME:
		delete _u._dateTimeValue;
		break;

	    case Type::REFERENCE:
		delete _u._referenceValue;
		break;
	}
    }

    _init();
}

void Value::toXml(Array<Sint8>& out) const
{
    if (_isArray)
    {
	out << "<VALUE.ARRAY>\n";

	switch (_type)
	{
	    case Type::BOOLEAN:
	    {
		for (Uint32 i = 0, n = _u._booleanArray->size; i < n; i++)
		{
		    out << "<VALUE>";
		    _toXml(out, Boolean(_u._booleanArray->data()[i]));
		    out << "</VALUE>\n";
		}
		break;
	    }

	    case Type::UINT8:
		_toXml(out, _u._uint8Array->data(), _u._uint8Array->size);
		break;

	    case Type::SINT8:
		_toXml(out, _u._sint8Array->data(), _u._sint8Array->size);
		break;

	    case Type::UINT16:
		_toXml(out, _u._uint16Array->data(), _u._uint16Array->size);
		break;

	    case Type::SINT16:
		_toXml(out, _u._sint16Array->data(), _u._sint16Array->size);
		break;

	    case Type::UINT32:
		_toXml(out, _u._uint32Array->data(), _u._uint32Array->size);
		break;

	    case Type::SINT32:
		_toXml(out, _u._sint32Array->data(), _u._sint32Array->size);
		break;

	    case Type::UINT64:
		_toXml(out, _u._uint64Array->data(), _u._uint64Array->size);
		break;

	    case Type::SINT64:
		_toXml(out, _u._sint64Array->data(), _u._sint64Array->size);
		break;

	    case Type::REAL32:
		_toXml(out, _u._real32Array->data(), _u._real32Array->size);
		break;

	    case Type::REAL64:
		_toXml(out, _u._real64Array->data(), _u._real64Array->size);
		break;

	    case Type::CHAR16:
		_toXml(out, _u._char16Array->data(), _u._char16Array->size);
		break;

	    case Type::STRING:
		_toXml(out, _u._stringArray->data(), _u._stringArray->size);
		break;

	    case Type::DATETIME:
		_toXml(out, _u._dateTimeArray->data(), _u._dateTimeArray->size);
		break;
	}

	out << "</VALUE.ARRAY>\n";
    }
    else if (_type == Type::REFERENCE)
    {
	_u._referenceValue->toXml(out);
    }
    else
    {
	out << "<VALUE>";

	switch (_type)
	{
	    case Type::BOOLEAN:
		_toXml(out, Boolean(_u._booleanValue != 0));
		break;

	    case Type::UINT8:
		_toXml(out, _u._uint8Value);
		break;

	    case Type::SINT8:
		_toXml(out, _u._sint8Value);
		break;

	    case Type::UINT16:
		_toXml(out, _u._uint16Value);
		break;

	    case Type::SINT16:
		_toXml(out, _u._sint16Value);
		break;

	    case Type::UINT32:
		_toXml(out, _u._uint32Value);
		break;

	    case Type::SINT32:
		_toXml(out, _u._sint32Value);
		break;

	    case Type::UINT64:
		_toXml(out, _u._uint64Value);
		break;

	    case Type::SINT64:
		_toXml(out, _u._sint64Value);
		break;

	    case Type::REAL32:
		_toXml(out, _u._real32Value);
		break;

	    case Type::REAL64:
		_toXml(out, _u._real64Value);
		break;

	    case Type::CHAR16:
		_toXml(out, Char16(_u._char16Value));
		break;

	    case Type::STRING:
		_toXml(out, *_u._stringValue);
		break;

	    case Type::DATETIME:
		_toXml(out, *_u._dateTimeValue);
		break;
	}

	out << "</VALUE>\n";
    }
}

void Value::print() const
{
    Array<Sint8> tmp;
    toXml(tmp);
    tmp.append('\0');
    std::cout << tmp.getData() << std::endl;
}

void Value::set(Boolean x)
{
    clear();
    _u._booleanValue = (Uint8)x;
    _type = Type::BOOLEAN;
}

void Value::set(Uint8 x)
{
    clear();
    _u._uint8Value = x;
    _type = Type::UINT8;
}

void Value::set(Sint8 x)
{
    clear();
    _u._sint8Value = x;
    _type = Type::SINT8;
}

void Value::set(Uint16 x)
{
    clear();
    _u._uint16Value = x;
    _type = Type::UINT16;
}

void Value::set(Sint16 x)
{
    clear();
    _u._sint16Value = x;
    _type = Type::SINT16;
}

void Value::set(Uint32 x)
{
    clear();
    _u._uint32Value = x;
    _type = Type::UINT32;
}

void Value::set(Sint32 x)
{
    clear();
    _u._sint32Value = x;
    _type = Type::SINT32;
}

void Value::set(Uint64 x)
{
    clear();
    _u._uint64Value = x;
    _type = Type::UINT64;
}

void Value::set(Sint64 x)
{
    clear();
    _u._sint64Value = x;
    _type = Type::SINT64;
}

void Value::set(Real32 x)
{
    clear();
    _u._real32Value = x;
    _type = Type::REAL32;
}

void Value::set(Real64 x)
{
    clear();
    _u._real64Value = x;
    _type = Type::REAL64;
}

void Value::set(const Char16& x)
{
    clear();
    _u._char16Value = x;
    _type = Type::CHAR16;
}

void Value::set(const String& x)
{
    clear();
    _u._stringValue = new String(x);
    _type = Type::STRING;
}

void Value::set(const char* x) 
{ 
    set(String(x)); 
}

void Value::set(const DateTime& x)
{
    clear();
    _u._dateTimeValue = new DateTime(x);
    _type = Type::DATETIME;
}

void Value::set(const Reference& x)
{
    clear();
    _u._referenceValue = new Reference(x);
    _type = Type::REFERENCE;
}

void Value::set(const Array<Boolean>& x)
{
    clear();
    _Inc(_u._booleanArray = x._rep);
    _type = Type::BOOLEAN;
    _isArray = true;
}

void Value::set(const Array<Uint8>& x)
{
    clear();
    _Inc(_u._uint8Array = x._rep);
    _type = Type::UINT8;
    _isArray = true;
}

void Value::set(const Array<Sint8>& x)
{
    clear();
    _Inc(_u._sint8Array = x._rep);
    _type = Type::SINT8;
    _isArray = true;
}

void Value::set(const Array<Uint16>& x)
{
    clear();
    _Inc(_u._uint16Array = x._rep);
    _type = Type::UINT16;
    _isArray = true;
}

void Value::set(const Array<Sint16>& x)
{
    clear();
    _Inc(_u._sint16Array = x._rep);
    _type = Type::SINT16;
    _isArray = true;
}

void Value::set(const Array<Uint32>& x)
{
    clear();
    _Inc(_u._uint32Array = x._rep);
    _type = Type::UINT32;
    _isArray = true;
}

void Value::set(const Array<Sint32>& x)
{
    clear();
    _Inc(_u._sint32Array = x._rep);
    _type = Type::SINT32;
    _isArray = true;
}

void Value::set(const Array<Uint64>& x)
{
    clear();
    _Inc(_u._uint64Array = x._rep);
    _type = Type::UINT64;
    _isArray = true;
}

void Value::set(const Array<Sint64>& x)
{
    clear();
    _Inc(_u._sint64Array = x._rep);
    _type = Type::SINT64;
    _isArray = true;
}

void Value::set(const Array<Real32>& x)
{
    clear();
    _Inc(_u._real32Array = x._rep);
    _type = Type::REAL32;
    _isArray = true;
}

void Value::set(const Array<Real64>& x)
{
    clear();
    _Inc(_u._real64Array = x._rep);
    _type = Type::REAL64;
    _isArray = true;
}

void Value::set(const Array<Char16>& x)
{
    clear();
    _Inc(_u._char16Array = x._rep);
    _type = Type::CHAR16;
    _isArray = true;
}

void Value::set(const Array<String>& x)
{
    clear();
    _Inc(_u._stringArray = x._rep);
    _type = Type::STRING;
    _isArray = true;
} 

void Value::set(const Array<DateTime>& x)
{
    clear();
    _Inc(_u._dateTimeArray = x._rep);
    _type = Type::DATETIME;
    _isArray = true;
}

void Value::get(Boolean& x) const
{
    if (_type != Type::BOOLEAN || _isArray)
	throw TypeMismatch();

    x = _u._booleanValue != 0;
}

void Value::get(Uint8& x) const
{
    if (_type != Type::UINT8 || _isArray)
	throw TypeMismatch();

    x = _u._uint8Value;
}

void Value::get(Sint8& x) const
{
    if (_type != Type::SINT8 || _isArray)
	throw TypeMismatch();

    x = _u._sint8Value;
}

void Value::get(Uint16& x) const
{
    if (_type != Type::UINT16 || _isArray)
	throw TypeMismatch();

    x = _u._uint16Value;
}

void Value::get(Sint16& x) const
{
    if (_type != Type::SINT16 || _isArray)
	throw TypeMismatch();

    x = _u._sint16Value;
}

void Value::get(Uint32& x) const
{
    if (_type != Type::UINT32 || _isArray)
	throw TypeMismatch();

    x = _u._uint32Value;
}

void Value::get(Sint32& x) const
{
    if (_type != Type::SINT32 || _isArray)
	throw TypeMismatch();

    x = _u._sint32Value;
}

void Value::get(Uint64& x) const
{
    if (_type != Type::UINT64 || _isArray)
	throw TypeMismatch();

    x = _u._uint64Value;
}

void Value::get(Sint64& x) const
{
    if (_type != Type::SINT64 || _isArray)
	throw TypeMismatch();

    x = _u._sint64Value;
}

void Value::get(Real32& x) const
{
    if (_type != Type::REAL32 || _isArray)
	throw TypeMismatch();

    x = _u._real32Value;
}

void Value::get(Real64& x) const
{
    if (_type != Type::REAL64 || _isArray)
	throw TypeMismatch();

    x = _u._real64Value;
}

void Value::get(Char16& x) const
{
    if (_type != Type::CHAR16 || _isArray)
	throw TypeMismatch();

    x = _u._char16Value;
}

void Value::get(String& x) const
{
    if (_type != Type::STRING || _isArray)
	throw TypeMismatch();

    x = *_u._stringValue;
}

void Value::get(DateTime& x) const
{
    if (_type != Type::DATETIME || _isArray)
	throw TypeMismatch();

    x = *_u._dateTimeValue;
}

void Value::get(Reference& x) const
{
    if (_type != Type::REFERENCE || _isArray)
	throw TypeMismatch();

    x = *_u._referenceValue;
}

void Value::get(Array<Boolean>& x) const
{
    if (_type != Type::BOOLEAN || !_isArray)
	throw TypeMismatch();

    x.set(_u._booleanArray);
}

void Value::get(Array<Uint8>& x) const
{
    if (_type != Type::UINT8 || !_isArray)
	throw TypeMismatch();

    x.set(_u._uint8Array);
}

void Value::get(Array<Sint8>& x) const
{
    if (_type != Type::SINT8 || !_isArray)
	throw TypeMismatch();

    x.set(_u._sint8Array);
}

void Value::get(Array<Uint16>& x) const
{
    if (_type != Type::UINT16 || !_isArray)
	throw TypeMismatch();

    x.set(_u._uint16Array);
}

void Value::get(Array<Sint16>& x) const
{
    if (_type != Type::SINT16 || !_isArray)
	throw TypeMismatch();

    x.set(_u._sint16Array);
}

void Value::get(Array<Uint32>& x) const
{
    if (_type != Type::UINT32 || !_isArray)
	throw TypeMismatch();

    x.set(_u._uint32Array);
}

void Value::get(Array<Sint32>& x) const
{
    if (_type != Type::SINT32 || !_isArray)
	throw TypeMismatch();

    x.set(_u._sint32Array);
}

void Value::get(Array<Uint64>& x) const
{
    if (_type != Type::UINT64 || !_isArray)
	throw TypeMismatch();

    x.set(_u._uint64Array);
}

void Value::get(Array<Sint64>& x) const
{
    if (_type != Type::SINT64 || !_isArray)
	throw TypeMismatch();

    x.set(_u._sint64Array);
}

void Value::get(Array<Real32>& x) const
{
    if (_type != Type::REAL32 || !_isArray)
	throw TypeMismatch();

    x.set(_u._real32Array);
}

void Value::get(Array<Real64>& x) const
{
    if (_type != Type::REAL64 || !_isArray)
	throw TypeMismatch();

    x.set(_u._real64Array);
}

void Value::get(Array<Char16>& x) const
{
    if (_type != Type::CHAR16 || !_isArray)
	throw TypeMismatch();

    x.set(_u._char16Array);
}

void Value::get(Array<String>& x) const
{
    if (_type != Type::STRING || !_isArray)
	throw TypeMismatch();

    x.set(_u._stringArray);
}

void Value::get(Array<DateTime>& x) const
{
    if (_type != Type::DATETIME || !_isArray)
	throw TypeMismatch();

    x.set(_u._dateTimeArray);
}

void Value::_init()
{
    _type = Type::NONE;
    _isArray = false;
    _u._voidPtr = 0;
}

Boolean operator==(const Value& x, const Value& y)
{
    if (!x.typeCompatible(y))
	return false;

    if (x._isArray)
    {
	switch (x._type)
	{
	    case Type::BOOLEAN:
		return Array<Boolean>(x._u._booleanArray) ==
		    Array<Boolean>(y._u._booleanArray);

	    case Type::UINT8:
		return Array<Uint8>(x._u._uint8Array) ==
		    Array<Uint8>(y._u._uint8Array);

	    case Type::SINT8:
		return Array<Sint8>(x._u._sint8Array) ==
		    Array<Sint8>(y._u._sint8Array);

	    case Type::UINT16:
		return Array<Uint16>(x._u._uint16Array) ==
		    Array<Uint16>(y._u._uint16Array);

	    case Type::SINT16:
		return Array<Sint16>(x._u._sint16Array) ==
		    Array<Sint16>(y._u._sint16Array);

	    case Type::UINT32:
		return Array<Uint32>(x._u._uint32Array) ==
		    Array<Uint32>(y._u._uint32Array);

	    case Type::SINT32:
		return Array<Sint32>(x._u._sint32Array) ==
		    Array<Sint32>(y._u._sint32Array);

	    case Type::UINT64:
		return Array<Uint64>(x._u._uint64Array) ==
		    Array<Uint64>(y._u._uint64Array);

	    case Type::SINT64:
		return Array<Sint64>(x._u._sint64Array) ==
		    Array<Sint64>(y._u._sint64Array);

	    case Type::REAL32:
		return Array<Real32>(x._u._real32Array) ==
		    Array<Real32>(y._u._real32Array);

	    case Type::REAL64:
		return Array<Real64>(x._u._real64Array) ==
		    Array<Real64>(y._u._real64Array);

	    case Type::CHAR16:
		return Array<Char16>(x._u._char16Array) ==
		    Array<Char16>(y._u._char16Array);

	    case Type::STRING:
		return Array<String>(x._u._stringArray) ==
		    Array<String>(y._u._stringArray);

	    case Type::DATETIME:
		return Array<DateTime>(x._u._dateTimeArray) ==
		    Array<DateTime>(y._u._dateTimeArray);
	}
    }
    else
    {
	switch (x._type)
	{
	    case Type::BOOLEAN:
		return x._u._booleanValue == y._u._booleanValue;

	    case Type::UINT8:
		return x._u._uint8Value == y._u._uint8Value;

	    case Type::SINT8:
		return x._u._sint8Value == y._u._sint8Value;

	    case Type::UINT16:
		return x._u._uint16Value == y._u._uint16Value;

	    case Type::SINT16:
		return x._u._sint16Value == y._u._sint16Value;

	    case Type::UINT32:
		return x._u._uint32Value == y._u._uint32Value;

	    case Type::SINT32:
		return x._u._sint32Value == y._u._sint32Value;

	    case Type::UINT64:
		return x._u._uint64Value == y._u._uint64Value;

	    case Type::SINT64:
		return x._u._sint64Value == y._u._sint64Value;

	    case Type::REAL32:
		return x._u._real32Value == y._u._real32Value;

	    case Type::REAL64:
		return x._u._real64Value == y._u._real64Value;

	    case Type::CHAR16:
		return x._u._char16Value == y._u._char16Value;

	    case Type::STRING:
		return *x._u._stringValue == *y._u._stringValue;

	    case Type::DATETIME:
		return *x._u._dateTimeValue == *y._u._dateTimeValue;

	    case Type::REFERENCE:
		return *x._u._referenceValue == *y._u._referenceValue;
	}
    }

    // Unreachable!
    return false;
}

void Value::setNullValue(Type type, Boolean isArray, Uint32 arraySize)
{
    clear();

    if (isArray)
    {
	switch (type)
	{
	    case Type::BOOLEAN:
		set(Array<Boolean>(arraySize));
		break;

	    case Type::UINT8:
		set(Array<Uint8>(arraySize));
		break;

	    case Type::SINT8:
		set(Array<Sint8>(arraySize));
		break;

	    case Type::UINT16:
		set(Array<Uint16>(arraySize));
		break;

	    case Type::SINT16:
		set(Array<Sint16>(arraySize));
		break;

	    case Type::UINT32:
		set(Array<Uint32>(arraySize));
		break;

	    case Type::SINT32:
		set(Array<Sint32>(arraySize));
		break;

	    case Type::UINT64:
		set(Array<Uint64>(arraySize));
		break;

	    case Type::SINT64:
		set(Array<Sint64>(arraySize));
		break;

	    case Type::REAL32:
		set(Array<Real32>(arraySize));
		break;

	    case Type::REAL64:
		set(Array<Real64>(arraySize));
		break;

	    case Type::CHAR16:
		set(Array<Char16>(arraySize));
		break;

	    case Type::STRING:
		set(Array<String>(arraySize));
		break;

	    case Type::DATETIME:
		set(Array<DateTime>(arraySize));
		break;
	}
    }
    else
    {
	switch (type)
	{
	    case Type::BOOLEAN:
		set(false);
		break;

	    case Type::UINT8:
		set(Uint8(0));
		break;

	    case Type::SINT8:
		set(Sint8(0));
		break;

	    case Type::UINT16:
		set(Uint16(0));
		break;

	    case Type::SINT16:
		set(Sint16(0));
		break;

	    case Type::UINT32:
		set(Uint32(0));
		break;

	    case Type::SINT32:
		set(Sint32(0));
		break;

	    case Type::UINT64:
		set(Uint64(0));
		break;

	    case Type::SINT64:
		set(Sint64(0));
		break;

	    case Type::REAL32:
		set(Real32(0.0));
		break;

	    case Type::REAL64:
		set(Real64(0.0));
		break;

	    case Type::CHAR16:
		set(Char16(0));
		break;

	    case Type::STRING:
		set(String());
		break;

	    case Type::DATETIME:
		set(DateTime());
		break;

	    case Type::REFERENCE:
		set(Reference());
		break;
	}
    }
}

String Value::toString() const
{
    Array<Sint8> out;

    if (_isArray)
    {
	switch (_type)
	{
	    case Type::BOOLEAN:
	    {
		for (Uint32 i = 0, n = _u._booleanArray->size; i < n; i++)
		{
		    _toXml(out, Boolean(_u._booleanArray->data()[i]));

		    out << " ";
		}
		break;
	    }

	    case Type::UINT8:
		_toStr(out, _u._uint8Array->data(), _u._uint8Array->size);
		break;

	    case Type::SINT8:
		_toStr(out, _u._sint8Array->data(), _u._sint8Array->size);
		break;

	    case Type::UINT16:
		_toStr(out, _u._uint16Array->data(), _u._uint16Array->size);
		break;

	    case Type::SINT16:
		_toStr(out, _u._sint16Array->data(), _u._sint16Array->size);
		break;

	    case Type::UINT32:
		_toStr(out, _u._uint32Array->data(), _u._uint32Array->size);
		break;

	    case Type::SINT32:
		_toStr(out, _u._sint32Array->data(), _u._sint32Array->size);
		break;

	    case Type::UINT64:
		_toStr(out, _u._uint64Array->data(), _u._uint64Array->size);
		break;

	    case Type::SINT64:
		_toStr(out, _u._sint64Array->data(), _u._sint64Array->size);
		break;

	    case Type::REAL32:
		_toStr(out, _u._real32Array->data(), _u._real32Array->size);
		break;

	    case Type::REAL64:
		_toStr(out, _u._real64Array->data(), _u._real64Array->size);
		break;

	    case Type::CHAR16:
		_toStr(out, _u._char16Array->data(), _u._char16Array->size);
		break;

	    case Type::STRING:
		_toStr(out, _u._stringArray->data(), _u._stringArray->size);
		break;

	    case Type::DATETIME:
		_toStr(out, _u._dateTimeArray->data(), _u._dateTimeArray->size);
		break;
	}
    }
    else if (_type == Type::REFERENCE)
    {
	// ATTN: work on this:
    }
    else
    {
	switch (_type)
	{
	    case Type::BOOLEAN:
		_toXml(out, Boolean(_u._booleanValue != 0));
		break;

	    case Type::UINT8:
		_toXml(out, _u._uint8Value);
		break;

	    case Type::SINT8:
		_toXml(out, _u._sint8Value);
		break;

	    case Type::UINT16:
		_toXml(out, _u._uint16Value);
		break;

	    case Type::SINT16:
		_toXml(out, _u._sint16Value);
		break;

	    case Type::UINT32:
		_toXml(out, _u._uint32Value);
		break;

	    case Type::SINT32:
		_toXml(out, _u._sint32Value);
		break;

	    case Type::UINT64:
		_toXml(out, _u._uint64Value);
		break;

	    case Type::SINT64:
		_toXml(out, _u._sint64Value);
		break;

	    case Type::REAL32:
		_toXml(out, _u._real32Value);
		break;

	    case Type::REAL64:
		_toXml(out, _u._real64Value);
		break;

	    case Type::CHAR16:
		_toXml(out, Char16(_u._char16Value));
		break;

	    case Type::STRING:
		_toXml(out, *_u._stringValue);
		break;

	    case Type::DATETIME:
		_toXml(out, *_u._dateTimeValue);
		break;
	}
    }

    out.append('\0');
    return out.getData();
}

PEGASUS_NAMESPACE_END
