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


#include <cctype>
#include "String.h"
#include "Exception.h"
#include "String.h"

// For debugging
#include <iostream>
using namespace std;


PEGASUS_NAMESPACE_BEGIN

const String String::EMPTY;

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
    Uint32 n = getLength() + 1;
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

    Uint32 n = _min(getLength(), length);

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
    if (i > getLength())
	ThrowOutOfBounds();

    return _rep[i];
}

const Char16 String::operator[](Uint32 i) const
{
    if (i > getLength())
	ThrowOutOfBounds();

    return _rep[i];
}

String& String::append(const Char16* str, Uint32 n)
{
    Uint32 m = _min(StrLen(str), n);
    _rep.reserve(_rep.getSize() + m);
    _rep.remove(_rep.getSize() - 1);
    _rep.append(str, m);
    _rep.append('\0');
    return *this;
}

void String::remove(Uint32 pos, Uint32 size)
{
    if (size == Uint32(-1))
	size = getLength() - pos;

    if (pos + size > getLength())
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

Boolean String::equal(const String& x, const String& y)
{
    if (x.getLength() != y.getLength())
	return false;

    return String::compare(x.getData(), y.getData(), x.getLength()) == 0;
}

Boolean String::equal(const String& x, const Char16* y)
{
    if (x.getLength() != StrLen(y))
	return false;

    return String::compare(x.getData(), y, x.getLength()) == 0;
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


String String::subString(Uint32 pos, Uint32 length) const
{
    if (pos < getLength())
    {
	if (length == Uint32(-1))
	    length = getLength() - pos;

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

    return Uint32(-1);
}

Uint32 String::find(const String& s) const
{
    const Char16* pSubStr = s.getData();
    const Char16* pStr = getData();
    Uint32 subStrLen = s.getLength();
    Uint32 strLen = getLength();
    
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
    return Uint32(-1);
}

// ATTN:KS 5 apr 2000 Need to add the Char16* version.
Uint32 String::find(const char* s) const
{
    return find(String(s));
}

Uint32 String::reverseFind(Char16 c) const
{
    const Char16* first = getData();
    const Char16* last = getData() + getLength();

    while (last != first)
    {
	if (*--last == c)
	    return last - first;
    }

    return Uint32(-1);
}

void String::toLower()
{
    for (Char16* p = &_rep[0]; *p; p++)
    {
	if (*p <= 127)
	    *p = tolower(*p);
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

std::ostream& operator<<(std::ostream& os, const String& x)
{
    for (Uint32 i = 0, n = x.getLength(); i < n; i++)
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

    for (Uint32 i = 0, n = tmp.getLength(); i < n; i++)
    {
	Char16 c = tmp[i];

	if (c <= 127)
	    tmp[i] = tolower(c);
    }

    return tmp;
}

int CompareIgnoreCase(const char* s1, const char* s2)
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

Boolean GetLine(istream& is, String& line)
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

PEGASUS_NAMESPACE_END
