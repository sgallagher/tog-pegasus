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
// $Log: String.h,v $
// Revision 1.1.1.1  2001/01/14 19:53:15  mike
// Pegasus import
//
//
//END_HISTORY

////////////////////////////////////////////////////////////////////////////////
//
// String.h
//
//	Simple String type.
//
////////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_String_h
#define Pegasus_String_h

#include <iostream>
#include <cstring>
#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/Char16.h>
#include <Pegasus/Common/Array.h>

PEGASUS_NAMESPACE_BEGIN

PEGASUS_COMMON_LINKAGE void ThrowNullPointer();

PEGASUS_COMMON_LINKAGE Uint32 StrLen(const Char16* str);

inline Uint32 StrLen(const char* str)
{
    if (!str)
	ThrowNullPointer();

    return strlen(str);
}

class PEGASUS_COMMON_LINKAGE String
{
public:

    String();

    String(const String& x);

    String(const String& x, Uint32 n);

    String(const Char16* x);

    String(const Char16* x, Uint32 n);

    String(const char* x);

    String(const char* x, Uint32 n);

    ~String() { }

    String& operator=(const String& x) { _rep = x._rep; return *this; }

    String& operator=(const Char16* x) { assign(x); return *this; }

    String& assign(const String& x) { _rep = x._rep; return *this; }

    String& assign(const Char16* x);

    String& assign(const Char16* x, Uint32 n);

    String& assign(const char* x);

    String& assign(const char* x, Uint32 n);

    void clear() { _rep.clear(); _rep.append('\0'); }

    void reserve(Uint32 capacity) { _rep.reserve(capacity + 1); }

    Uint32 getLength() const { return _rep.getSize() - 1; }

    const Char16* getData() const { return _rep.getData(); }

    // Allocates an 8 bit representation of this string. The user is 
    // responsible for freeing the result. If any characters are truncated, 
    // a TruncatedCharacter exception is thrown. This exception may be 
    // suppressed by passing true as the noThrow argument.

    char* allocateCString(Uint32 extraBytes = 0, Boolean noThrow = false) const;

    // Append the given string to a C-string. If the length is not Uint32(-1),
    // then the lesser of the the length argument and the length of this
    // string is truncated. Otherwise, the entire string is trunctated. The
    // TruncatedCharacter exception is thrown if any characters are
    // truncated.

    void appendToCString(
	char* str, 
	Uint32 length = Uint32(-1),
	Boolean noThrow = false) const;

    Char16& operator[](Uint32 i);

    const Char16 operator[](Uint32 i) const;

    String& append(const Char16& c) 
    { 
	_rep.insert(_rep.getSize() - 1, c); 
	return *this;
    }

    String& append(const Char16* str, Uint32 n);

    String& append(const String& str) 
    {
	return append(str.getData(), str.getLength());
    }

    String& operator+=(const String& x)
    {
	return append(x);
    }

    String& operator+=(Char16 c)
    {
	return append(c);
    }

    String& operator+=(char c)
    {
	return append(Char16(c));
    }

    void remove(Uint32 pos, Uint32 size = Uint32(-1));

    String subString(Uint32 pos, Uint32 length = Uint32(-1)) const;

    Uint32 find(Char16 c) const;

    static int compare(const Char16* s1, const Char16* s2, Uint32 n);

    static int compare(const Char16* s1, const Char16* s2);

    static const String EMPTY;

private:

    static Uint32 _min(Uint32 x, Uint32 y) { return x < y ? x : y; }

    Array<Char16> _rep;	
};

PEGASUS_COMMON_LINKAGE Boolean operator==(const String& x, const String& y);

PEGASUS_COMMON_LINKAGE Boolean operator==(const String& x, const char* y);

PEGASUS_COMMON_LINKAGE Boolean operator==(const char* x, const String& y);

inline Boolean operator!=(const String& x, const String& y)
{
    return !operator==(x, y);
}

PEGASUS_COMMON_LINKAGE std::ostream& operator<<(std::ostream& os, const String& x);

inline String operator+(const String& x, const String& y)
{
    return String(x).append(y);
}

inline Boolean operator<(const String& x, const String& y)
{
    return String::compare(x.getData(), y.getData()) < 0;
}

inline Boolean operator<=(const String& x, const String& y)
{
    return String::compare(x.getData(), y.getData()) <= 0;
}

inline Boolean operator>(const String& x, const String& y)
{
    return String::compare(x.getData(), y.getData()) > 0;
}

inline Boolean operator>=(const String& x, const String& y)
{
    return String::compare(x.getData(), y.getData()) >= 0;
}

PEGASUS_NAMESPACE_END

#endif /* Pegasus_String_h */
