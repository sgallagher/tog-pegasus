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
// Author: Mike Brasher (mbrasher@bmc.com)
//
// Modified By:
//
//%/////////////////////////////////////////////////////////////////////////////

#ifndef X_h
#define X_h

#include <cstring>
#include <iostream>
#include <Pegasus/Common/Config.h>

PEGASUS_USING_PEGASUS;

class Str
{
public:

    static int _constructions;
    static int _destructions;

    static char* _clone(const char* str)
    {
	return strcpy(new char[strlen(str) + 1], str);
    }

    Str() 
    { 
	_constructions++; 
	_str = _clone(""); 
    }

    Str(const Str& x) 
    { 
	_constructions++; 
	_str = _clone(x._str); 
    }

    Str(const char* str) 
    { 
	_constructions++; 
	_str = _clone(str); 
    }

    Str& operator=(const Str& x) 
    { 
	_constructions++; 
	delete [] _str; 
	_str = _clone(x._str); 
	return *this; 
    }

    ~Str() 
    {
	_destructions++;
	delete [] _str; 
    }

    const char* getStr() const { return _str; }

private:
    char* _str;
};

inline PEGASUS_STD(ostream)& operator<<(PEGASUS_STD(ostream)& os, const Str& x)
{
    os << x.getStr();
    return os;
}

inline Boolean operator==(const Str& x1, const Str& x2)
{
    return ::strcmp(x1.getStr(), x2.getStr()) == 0;
}

#endif /* X_h */
