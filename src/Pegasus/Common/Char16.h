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
// $Log: Char16.h,v $
// Revision 1.1  2001/01/14 19:50:36  mike
// Initial revision
//
//
//END_HISTORY

////////////////////////////////////////////////////////////////////////////////
//
// Char16.h
//
//     	The Char16 class represents a CIM sixteen bit character (char16).
//	This class is a trivial wrapper for a sixteen bit integer. It is used
//	as the element type in the String class (used to represent the CIM
//	string type).
//
////////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_Char16_h
#define Pegasus_Char16_h

#include <iostream>
#include <Pegasus/Common/Config.h>

PEGASUS_NAMESPACE_BEGIN

class PEGASUS_COMMON_LINKAGE Char16 
{
public:

    Char16() : _code(0) { }

    Char16(Uint16 x) : _code(x) { }

    Char16(const Char16& x) : _code(x._code) { }

    Char16& operator=(Uint16 x) { _code = x; return *this; }

    Char16& operator=(const Char16& x) {_code = x._code; return *this;}

    operator Uint16() const { return _code; }

    Uint16 getCode() const { return _code; }

private:

    Uint16 _code;
};

inline Boolean operator==(const Char16& x, const Char16& y)
{
    return x.getCode() == y.getCode();
}

inline Boolean operator==(const Char16& x, char y)
{
    return x.getCode() == y;
}

inline Boolean operator==(char x, const Char16& y)
{
    return x == y.getCode();
}

PEGASUS_COMMON_LINKAGE std::ostream& operator<<(
    std::ostream& os, const Char16& x);

PEGASUS_NAMESPACE_END

#endif /* Pegasus_Char16_h */
