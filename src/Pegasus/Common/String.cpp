//%/////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001 The Open group, BMC Software, Tivoli Systems, IBM
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
// Author: Mike Brasher (mbrasher@bmc.com)
//
// Modified By:
//
//%/////////////////////////////////////////////////////////////////////////////


#include <cctype>
#include "String.h"
#include "Exception.h"
#include "String.h"
#include <iostream>

PEGASUS_NAMESPACE_BEGIN

#define PEGASUS_ARRAY_T String
#include <Pegasus/Common/ArrayImpl.h>
#undef PEGASUS_ARRAY_T

const String String::EMPTY;

static inline void _SkipWhitespace(const Char16*& p)
{
    while (*p && isspace(*p))
        p++;
}

inline Uint32 StrLen(const char* str)
{
    if (!str)
	throw NullPointer();

    return strlen(str);
}

inline Uint32 StrLen(const Char16* str)
{
    if (!str)
	throw NullPointer();

    Uint32 n = 0;

    while (*str++)
	n++;

    return n;
}

String::String()
{
    _rep.append('\0');
}

String::String(const String& x) : _rep(x._rep)
{

}

String::String(const String& x, Uint32 n)
{
    _rep.append('\0');
    append(x.getData(), n);
}

String::String(const Char16* x) : _rep(x, StrLen(x) + 1)
{

}

String::String(const Char16* x, Uint32 n)
{
    assign(x, n);
}

String::String(const char* str)
{
    Uint32 n = ::strlen(str) + 1;
    reserve(n);

    while (n--)
	_rep.append(*str++);
}

String::String(const char* str, Uint32 n_)
{
    Uint32 n = _min(strlen(str), n_);
    reserve(n + 1);

    while (n--)
	_rep.append(*str++);

    _rep.append('\0');
}

String& String::assign(const Char16* x)
{
    _rep.clear();
    _rep.append(x, StrLen(x) + 1);
    return *this;
}

String& String::assign(const Char16* str, Uint32 n)
{
    _rep.clear();
    Uint32 m = _min(StrLen(str), n);
    _rep.append(str, m);
    _rep.append('\0');
    return *this;
}

String& String::assign(const char* x)
{
    _rep.clear();
    Uint32 n = strlen(x);
    _rep.reserve(n + 1);

    while (n--)
	_rep.append(*x++);

    _rep.append('\0');

    return *this;
}

String& String::assign(const char* x, Uint32 n_)
{
    _rep.clear();

    Uint32 n = _min(strlen(x), n_);
    _rep.reserve(n + 1);

    while (n--)
	_rep.append(*x++);

    _rep.append('\0');

    return *this;
}

char* String::allocateCString(Uint32 extraBytes, Boolean noThrow) const
{
    Uint32 n = size() + 1;
    char* str = new char[n + extraBytes];
    char* p = str;
    const Char16* q = getData();

    for (Uint32 i = 0; i < n; i++)
    {
	Uint16 c = *q++;
	*p++ = char(c);

	if ((c & 0xff00) && !noThrow)
	    throw TruncatedCharacter();
    }

    return str;
}

void String::appendToCString(
    char* str,
    Uint32 length,
    Boolean noThrow) const
{
    if (!str)
	throw NullPointer();

    Uint32 n = _min(size(), length);

    char* p = str + strlen(str);
    const Char16* q = getData();

    for (Uint32 i = 0; i < n; i++)
    {
	Uint16 c = *q++;
	*p++ = char(c);

	if ((c & 0xff00) && !noThrow)
	    throw TruncatedCharacter();
    }

    *p = '\0';
}

Char16& String::operator[](Uint32 i)
{
    if (i > size())
	ThrowOutOfBounds();

    return _rep[i];
}

const Char16 String::operator[](Uint32 i) const
{
    if (i > size())
	ThrowOutOfBounds();

    return _rep[i];
}

String& String::append(const Char16* str, Uint32 n)
{
    Uint32 m = _min(StrLen(str), n);
    _rep.reserve(_rep.size() + m);
    _rep.remove(_rep.size() - 1);
    _rep.append(str, m);
    _rep.append('\0');
    return *this;
}

void String::remove(Uint32 pos, Uint32 size)
{
    if (size == PEG_NOT_FOUND)
	size = this->size() - pos;

    if (pos + size > this->size())
	ThrowOutOfBounds();

    if (size)
	_rep.remove(pos, size);
}

int String::compare(const Char16* s1, const Char16* s2, Uint32 n)
{
    while (n--)
    {
	int r = *s1++ - *s2++;

	if (r)
	    return r;
    }

    return 0;
}

int String::compareNoCase(const char* s1, const char* s2, Uint32 n)
{
    while (n--)
    {
	int r = tolower(*s1++) - tolower(*s2++);

	if (r)
	    return r;
    }

    return 0;
}

Boolean String::equal(const String& x, const String& y)
{
    if (x.size() != y.size())
	return false;

    return String::compare(x.getData(), y.getData(), x.size()) == 0;
}

Boolean String::equal(const String& x, const Char16* y)
{
    if (x.size() != StrLen(y))
	return false;

    return String::compare(x.getData(), y, x.size()) == 0;
}

Boolean String::equal(const Char16* x, const String& y)
{
    return equal(y, x);
}

Boolean String::equal(const String& x, const char* y)
{
    return equal(x, String(y));
}

Boolean String::equal(const char* x, const String& y)
{
    return equal(String(x), y);
}

Boolean String::equalNoCase(const String& x, const String& y)
{
    if (x.size() != y.size())
	return false;

    const Char16* p = x.getData();
    const Char16* q = y.getData();

    Uint32 n = x.size();

    while (n--)
    {
	if (*p <= 127 && *q <= 127)
	{
	    if (tolower(*p++) != tolower(*q++))
		return false;
	}
	else if (*p++ != *q++)
	    return false;
    }

    return true;
}

String String::subString(Uint32 pos, Uint32 length) const
{
    if (pos < size())
    {
	if (length == PEG_NOT_FOUND)
	    length = size() - pos;

	return String(getData() + pos, length);
    }
    else
	return String();
}

Uint32 String::find(Char16 c) const
{
    const Char16* first = getData();

    for (const Char16* p = first; *p; p++)
    {
	if (*p == c)
	    return  p - first;
    }

    return PEG_NOT_FOUND;
}

Uint32 String::find(const String& s) const
{
    const Char16* pSubStr = s.getData();
    const Char16* pStr = getData();
    Uint32 subStrLen = s.size();
    Uint32 strLen = size();

    // loop to find first char match
    Uint32 loc = 0;
    for( ; loc <= (strLen-subStrLen); loc++)
    {
	if (*pStr++ == *pSubStr)  // match first char
	{
	    // point to substr 2nd char
	    const Char16* p = pSubStr + 1;

	    // Test remaining chars for equal
	    Uint32 i = 1;
	    for (; i < subStrLen; i++)
		if (*pStr++ != *p++ )
		    {pStr--; break;} // break from loop
	    if (i == subStrLen)
		return loc;
	}
    }
    return PEG_NOT_FOUND;
}

// ATTN:KS 5 apr 2000 Need to add the Char16* version.
Uint32 String::find(const char* s) const
{
    return find(String(s));
}

Uint32 String::reverseFind(Char16 c) const
{
    const Char16* first = getData();
    const Char16* last = getData() + size();

    while (last != first)
    {
	if (*--last == c)
	    return last - first;
    }

    return PEG_NOT_FOUND;
}

void String::toLower()
{
    for (Char16* p = &_rep[0]; *p; p++)
    {
	if (*p <= 127)
	    *p = tolower(*p);
    }
}

void String::translate(Char16 fromChar, Char16 toChar)
{
    for (Char16* p = &_rep[0]; *p; p++)
    {
	if (*p == fromChar)
	    *p = toChar;
    }
}

int String::compare(const Char16* s1, const Char16* s2)
{
    while (*s1 && *s2)
    {
	int r = *s1++ - *s2++;

	if (r)
	    return r;
    }

    if (*s2)
	return -1;
    else if (*s1)
	return 1;

    return 0;
}

PEGASUS_STD(ostream)& operator<<(PEGASUS_STD(ostream)& os, const String& x)
{
    for (Uint32 i = 0, n = x.size(); i < n; i++)
	os << x[i];

    return os;
}

void String::toLower(char* str)
{
    while (*str)
	tolower(*str++);
}

String ToLower(const String& str)
{
    String tmp(str);

    for (Uint32 i = 0, n = tmp.size(); i < n; i++)
    {
	Char16 c = tmp[i];

	if (c <= 127)
	    tmp[i] = tolower(c);
    }

    return tmp;
}

int CompareNoCase(const char* s1, const char* s2)
{
    while (*s1 && *s2)
    {
	int r = tolower(*s1++) - tolower(*s2++);

	if (r)
	    return r;
    }

    if (*s2)
	return -1;
    else if (*s1)
	return 1;

    return 0;
}

Boolean GetLine(PEGASUS_STD(istream)& is, String& line)
{
    line.clear();

    Boolean gotChar = false;
    char c;

    while (is.get(c))
    {
	gotChar = true;

	if (c == '\n')
	    break;

	line.append(c);
    }

    return gotChar;
}

String::~String()
{
}

String& String::assign(const String& x)
{
    _rep = x._rep;
    return *this;
}

String& String::append(const Char16& c)
{
    _rep.insert(_rep.size() - 1, c);
    return *this;
}

void String::clear()
{
    _rep.clear();
    _rep.append('\0');
}

void String::reserve(Uint32 capacity)
{
    _rep.reserve(capacity + 1);
}

const Array<String>& EmptyStringArray()
{
    static Array<String> tmp;
    return tmp;
}

PEGASUS_NAMESPACE_END
