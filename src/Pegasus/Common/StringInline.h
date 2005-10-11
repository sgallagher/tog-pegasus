//%2005////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001, 2002 BMC Software; Hewlett-Packard Development
// Company, L.P.; IBM Corp.; The Open Group; Tivoli Systems.
// Copyright (c) 2003 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation, The Open Group.
// Copyright (c) 2004 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation; VERITAS Software Corporation; The Open Group.
// Copyright (c) 2005 Hewlett-Packard Development Company, L.P.; IBM Corp.;
// EMC Corporation; VERITAS Software Corporation; The Open Group.
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
//%/////////////////////////////////////////////////////////////////////////////

#ifndef _Pegasus_StringInline_h
#define _Pegasus_StringInline_h

#include <Pegasus/Common/Atomic.h>
#include <Pegasus/Common/StringRep.h>

#ifdef PEGASUS_INTERNALONLY
# define PEGASUS_STRING_INLINE inline
#else
# define PEGASUS_STRING_INLINE /* empty */
#endif

PEGASUS_NAMESPACE_BEGIN

PEGASUS_STRING_INLINE CString::CString() : _rep(0)
{
}

PEGASUS_STRING_INLINE CString::CString(char* cstr) : _rep(cstr)
{
}

PEGASUS_STRING_INLINE CString::~CString()
{
    operator delete(_rep);
}

PEGASUS_STRING_INLINE CString::operator const char*() const
{
    return _rep;
}

PEGASUS_STRING_INLINE String::String()
{
    _rep = &StringRep::_empty_rep;
}

PEGASUS_STRING_INLINE String::String(const String& str)
{
    StringRep::ref(_rep = str._rep);
} 

PEGASUS_STRING_INLINE String::~String()
{
    StringRep::unref(_rep);
}

PEGASUS_STRING_INLINE Uint32 String::size() const 
{ 
    return _rep->size; 
}

PEGASUS_STRING_INLINE const Char16* String::getChar16Data() const 
{ 
    return (Char16*)_rep->data; 
}

PEGASUS_STRING_INLINE Char16& String::operator[](Uint32 i) 
{
    _check_bounds(i, _rep->size);

    if (Atomic_get(&_rep->refs) != 1)
	_rep = StringRep::copy_on_write(_rep);

    return (Char16&)_rep->data[i]; 
}

PEGASUS_STRING_INLINE const Char16 String::operator[](Uint32 i) const 
{
    _check_bounds(i, _rep->size);
    return (Char16&)_rep->data[i]; 
}

PEGASUS_STRING_INLINE String& String::operator=(const String& str)
{
    return assign(str);
}

#ifdef PEGASUS_USE_EXPERIMENTAL_INTERFACES
PEGASUS_STRING_INLINE String& String::operator=(const char* str)
{
    return assign(str);
}
#endif /* PEGASUS_USE_EXPERIMENTAL_INTERFACES */

PEGASUS_STRING_INLINE String& String::assign(const Char16* str)
{
    return assign(str, StringRep::length((Uint16*)str));
    return *this;
}

PEGASUS_STRING_INLINE String& String::assign(const char* str)
{
    return assign(str, strlen(str));
}

#ifdef PEGASUS_USE_EXPERIMENTAL_INTERFACES
PEGASUS_STRING_INLINE String& String::assignASCII7(const char* str)
{
    return assignASCII7(str, strlen(str));
}
#endif /* PEGASUS_USE_EXPERIMENTAL_INTERFACES */

PEGASUS_STRING_INLINE Uint32 String::find(const String& s) const
{
    return String_find_aux(_rep, (Char16*)s._rep->data, s._rep->size);
}

PEGASUS_STRING_INLINE String& String::append(const Char16& c)
{
    if (_rep->size == _rep->cap || Atomic_get(&_rep->refs) != 1)
	String_append_char_aux(_rep);

    _rep->data[_rep->size++] = c;
    _rep->data[_rep->size] = 0;
    return *this;
}

PEGASUS_STRING_INLINE Boolean String::equalNoCase(
    const String& s1, const String& s2)
{
#ifdef PEGASUS_HAS_ICU
	return equalNoCase_aux(s1, s2);
#else
    if (s1._rep->size == s2._rep->size)
	return String_equalNoCase_aux(s1, s2);

    return false;
#endif
}

#ifdef PEGASUS_USE_EXPERIMENTAL_INTERFACES
PEGASUS_STRING_INLINE String& String::append(const char* str)
{
    append(str, strlen(str));
    return *this;
}
#endif /* PEGASUS_USE_EXPERIMENTAL_INTERFACES */

PEGASUS_STRING_INLINE Boolean operator==(const String& s1, const String& s2)
{
    return String::equal(s1, s2);
}

PEGASUS_STRING_INLINE Boolean operator==(const String& s1, const char* s2)
{
    return String::equal(s1, s2);
}

PEGASUS_STRING_INLINE Boolean operator==(const char* s1, const String& s2)
{
    return String::equal(s2, s1);
}

PEGASUS_STRING_INLINE Boolean operator!=(const String& s1, const String& s2)
{
    return !String::equal(s1, s2);
}

PEGASUS_STRING_INLINE Boolean operator!=(const String& s1, const char* s2)
{
    return !String::equal(s1, s2);
}

PEGASUS_STRING_INLINE Boolean operator!=(const char* s1, const String& s2)
{
    return !String::equal(s2, s1);
}

PEGASUS_STRING_INLINE Boolean operator<(const String& s1, const String& s2)
{
    return String::compare(s1, s2) < 0;
}

PEGASUS_STRING_INLINE Boolean operator<(const String& s1, const char* s2)
{
    return String::compare(s1, s2) < 0;
}

PEGASUS_STRING_INLINE Boolean operator<(const char* s1, const String& s2)
{
    return String::compare(s2, s1) > 0;
}

PEGASUS_STRING_INLINE Boolean operator>(const String& s1, const String& s2)
{
    return String::compare(s1, s2) > 0;
}

PEGASUS_STRING_INLINE Boolean operator>(const String& s1, const char* s2)
{
    return String::compare(s1, s2) > 0;
}

PEGASUS_STRING_INLINE Boolean operator>(const char* s1, const String& s2)
{
    return String::compare(s2, s1) < 0;
}

PEGASUS_STRING_INLINE Boolean operator<=(const String& s1, const String& s2)
{
    return String::compare(s1, s2) <= 0;
}

PEGASUS_STRING_INLINE Boolean operator<=(const String& s1, const char* s2)
{
    return String::compare(s1, s2) <= 0;
}

PEGASUS_STRING_INLINE Boolean operator<=(const char* s1, const String& s2)
{
    return String::compare(s2, s1) >= 0;
}

PEGASUS_STRING_INLINE Boolean operator>=(const String& s1, const String& s2)
{
    return String::compare(s1, s2) >= 0;
}

PEGASUS_STRING_INLINE Boolean operator>=(const String& s1, const char* s2)
{
    return String::compare(s1, s2) >= 0;
}

PEGASUS_STRING_INLINE Boolean operator>=(const char* s1, const String& s2)
{
    return String::compare(s2, s1) <= 0;
}

PEGASUS_STRING_INLINE String operator+(const String& s1, const String& s2)
{
#ifdef PEGASUS_USE_EXPERIMENTAL_INTERFACES
    return String(s1, s2);
#else
    String tmp;
    tmp.reserveCapacity(s1.size() + s2.size());
    tmp.append(s1);
    tmp.append(s2);
    return tmp;
#endif
}

#ifdef PEGASUS_USE_EXPERIMENTAL_INTERFACES
PEGASUS_STRING_INLINE String operator+(const String& s1, const char* s2)
{
    return String(s1, s2);
}
#endif /* PEGASUS_USE_EXPERIMENTAL_INTERFACES */

#ifdef PEGASUS_USE_EXPERIMENTAL_INTERFACES
PEGASUS_STRING_INLINE String operator+(const char* s1, const String& s2)
{
    return String(s1, s2);
}
#endif /* PEGASUS_USE_EXPERIMENTAL_INTERFACES */

PEGASUS_NAMESPACE_END

#endif /* _Pegasus_StringInline_h */
