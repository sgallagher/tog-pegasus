//%2006////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001, 2002 BMC Software; Hewlett-Packard Development
// Company, L.P.; IBM Corp.; The Open Group; Tivoli Systems.
// Copyright (c) 2003 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation, The Open Group.
// Copyright (c) 2004 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation; VERITAS Software Corporation; The Open Group.
// Copyright (c) 2005 Hewlett-Packard Development Company, L.P.; IBM Corp.;
// EMC Corporation; VERITAS Software Corporation; The Open Group.
// Copyright (c) 2006 Hewlett-Packard Development Company, L.P.; IBM Corp.;
// EMC Corporation; Symantec Corporation; The Open Group.
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
//%/////////////////////////////////////////////////////////////////////////////


#include <Pegasus/Common/PegasusAssert.h>
#include <Pegasus/Common/StringConversion.h>
#include <cstring>
#include <iostream>

PEGASUS_USING_PEGASUS;
PEGASUS_USING_STD;

const Sint64 HI = 1024 * 100;
const Sint64 LO = -HI;

template<class T>
struct Cvt
{
};

template<>
struct Cvt<Uint8>
{
    static const char* func(char buffer[22], Uint8 x, Uint32& size)
    {
        return Uint8ToString(buffer, x, size);
    }
};

template<>
struct Cvt<Uint16>
{
    static const char* func(char buffer[22], Uint16 x, Uint32& size)
    {
        return Uint16ToString(buffer, x, size);
    }
};

template<>
struct Cvt<Uint32>
{
    static const char* func(char buffer[22], Uint32 x, Uint32& size)
    {
        return Uint32ToString(buffer, x, size);
    }
};

template<>
struct Cvt<Uint64>
{
    static const char* func(char buffer[22], Uint64 x, Uint32& size)
    {
        return Uint64ToString(buffer, x, size);
    }
};

template<>
struct Cvt<Sint8>
{
    static const char* func(char buffer[22], Sint8 x, Uint32& size)
    {
        return Sint8ToString(buffer, x, size);
    }
};

template<>
struct Cvt<Sint16>
{
    static const char* func(char buffer[22], Sint16 x, Uint32& size)
    {
        return Sint16ToString(buffer, x, size);
    }
};

template<>
struct Cvt<Sint32>
{
    static const char* func(char buffer[22], Sint32 x, Uint32& size)
    {
        return Sint32ToString(buffer, x, size);
    }
};

template<>
struct Cvt<Sint64>
{
    static const char* func(char buffer[22], Sint64 x, Uint32& size)
    {
        return Sint64ToString(buffer, x, size);
    }
};

template<class S, class U>
struct Test
{
    static void testUint(const char* format)
    {
        for (Uint64 i = 0; i < Uint64(HI); i++)
        {
            const char* str;
            char buffer1[32];
            Uint32 size;
            str = Cvt<U>::func(buffer1, U(i), size);

            char buffer2[32];
            sprintf(buffer2, format, U(i));

            PEGASUS_TEST_ASSERT(strcmp(str, buffer2) == 0);
            PEGASUS_TEST_ASSERT(strlen(str) == size);
        }
    }

    static void testSint(const char* format)
    {
        for (Sint64 i = LO; i < HI; i++)
        {
            const char* str;
            char buffer1[32];
            Uint32 size;
            str = Cvt<S>::func(buffer1, S(i), size);

            char buffer2[32];
            sprintf(buffer2, format, S(i));

            PEGASUS_TEST_ASSERT(strcmp(str, buffer2) == 0);
            PEGASUS_TEST_ASSERT(strlen(str) == size);
        }
    }
};

int main(int argc, char** argv)
{
    Test<Sint8, Uint8>::testUint("%u");
    Test<Sint16, Uint16>::testUint("%u");
    Test<Sint32, Uint32>::testUint("%u");
    Test<Sint64, Uint64>::testUint("%" PEGASUS_64BIT_CONVERSION_WIDTH "u");
    Test<Sint8, Uint8>::testSint("%d");
    Test<Sint16, Uint16>::testSint("%d");
    Test<Sint32, Uint32>::testSint("%d");
    Test<Sint64, Uint64>::testSint("%" PEGASUS_64BIT_CONVERSION_WIDTH "d");

    cout << argv[0] << " +++++ passed all tests" << endl;

    return 0;
}
