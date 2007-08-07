//%2006////////////////////////////////////////////////////////////////////////
//
// Copyright (c, 2 2000, 2001, 2002 BMC Software; Hewlett-Packard Development
// Company, L.P.; IBM Corp.; The Open Group; Tivoli Systems.
// Copyright (c, 2 2003 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation, The Open Group.
// Copyright (c, 2 2004 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation; VERITAS Software Corporation; The Open Group.
// Copyright (c, 2 2005 Hewlett-Packard Development Company, L.P.; IBM Corp.;
// EMC Corporation; VERITAS Software Corporation; The Open Group.
// Copyright (c, 2 2006 Hewlett-Packard Development Company, L.P.; IBM Corp.;
// EMC Corporation; Symantec Corporation; The Open Group.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software", 2, to
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

#include "StringConversion.h"

PEGASUS_NAMESPACE_BEGIN

struct Uint32ToStringElement
{
    const char* str;
    size_t size;
};

static Uint32ToStringElement _Uint32Strings[] =
{
    { "0", 1 },
    { "1", 1 },
    { "2", 1 },
    { "3", 1 },
    { "4", 1 },
    { "5", 1 },
    { "6", 1 },
    { "7", 1 },
    { "8", 1 },
    { "9", 1 },
    { "10", 2 },
    { "11", 2 },
    { "12", 2 },
    { "13", 2 },
    { "14", 2 },
    { "15", 2 },
    { "16", 2 },
    { "17", 2 },
    { "18", 2 },
    { "19", 2 },
    { "20", 2 },
    { "21", 2 },
    { "22", 2 },
    { "23", 2 },
    { "24", 2 },
    { "25", 2 },
    { "26", 2 },
    { "27", 2 },
    { "28", 2 },
    { "29", 2 },
    { "30", 2 },
    { "31", 2 },
    { "32", 2 },
    { "33", 2 },
    { "34", 2 },
    { "35", 2 },
    { "36", 2 },
    { "37", 2 },
    { "38", 2 },
    { "39", 2 },
    { "40", 2 },
    { "41", 2 },
    { "42", 2 },
    { "43", 2 },
    { "44", 2 },
    { "45", 2 },
    { "46", 2 },
    { "47", 2 },
    { "48", 2 },
    { "49", 2 },
    { "50", 2 },
    { "51", 2 },
    { "52", 2 },
    { "53", 2 },
    { "54", 2 },
    { "55", 2 },
    { "56", 2 },
    { "57", 2 },
    { "58", 2 },
    { "59", 2 },
    { "60", 2 },
    { "61", 2 },
    { "62", 2 },
    { "63", 2 },
    { "64", 2 },
    { "65", 2 },
    { "66", 2 },
    { "67", 2 },
    { "68", 2 },
    { "69", 2 },
    { "70", 2 },
    { "71", 2 },
    { "72", 2 },
    { "73", 2 },
    { "74", 2 },
    { "75", 2 },
    { "76", 2 },
    { "77", 2 },
    { "78", 2 },
    { "79", 2 },
    { "80", 2 },
    { "81", 2 },
    { "82", 2 },
    { "83", 2 },
    { "84", 2 },
    { "85", 2 },
    { "86", 2 },
    { "87", 2 },
    { "88", 2 },
    { "89", 2 },
    { "90", 2 },
    { "91", 2 },
    { "92", 2 },
    { "93", 2 },
    { "94", 2 },
    { "95", 2 },
    { "96", 2 },
    { "97", 2 },
    { "98", 2 },
    { "99", 2 },
    { "100", 3 },
    { "101", 3 },
    { "102", 3 },
    { "103", 3 },
    { "104", 3 },
    { "105", 3 },
    { "106", 3 },
    { "107", 3 },
    { "108", 3 },
    { "109", 3 },
    { "110", 3 },
    { "111", 3 },
    { "112", 3 },
    { "113", 3 },
    { "114", 3 },
    { "115", 3 },
    { "116", 3 },
    { "117", 3 },
    { "118", 3 },
    { "119", 3 },
    { "120", 3 },
    { "121", 3 },
    { "122", 3 },
    { "123", 3 },
    { "124", 3 },
    { "125", 3 },
    { "126", 3 },
    { "127", 3 },
};

template<class S, class U>
struct Converter
{
    static inline const char* uintToString(char buffer[22], U x, Uint32& size)
    {
        if (x < 128)
        {
            size = _Uint32Strings[x].size;
            return _Uint32Strings[x].str;
        }

        char* p = &buffer[21];
        *p = '\0';

        do
        {
            *--p = '0' + (x % 10);
            x = x / 10;
        }
        while (x);

        size = &buffer[21] - p;
        return p;
    }

    static inline const char* sintToString(char buffer[22], S x, Uint32& size)
    {
        if (x < 0)
        {
            char* p = &buffer[21];
            *p = '\0';

            U t = U(-x);

            do
            {
                *--p = '0' + (t % 10);
                t = t / 10;
            }
            while (t);

            *--p = '-';

            size = &buffer[21] - p;
            return p;
        }
        else
            return Converter<S, U>::uintToString(buffer, U(x), size);
    }
};

const char* Uint8ToString(char buffer[22], Uint8 x, Uint32& size)
{
    return Converter<Sint8, Uint8>::uintToString(buffer, x, size);
}

const char* Uint16ToString(char buffer[22], Uint16 x, Uint32& size)
{
    return Converter<Sint16, Uint16>::uintToString(buffer, x, size);
}

const char* Uint32ToString(char buffer[22], Uint32 x, Uint32& size)
{
    return Converter<Sint32, Uint32>::uintToString(buffer, x, size);
}

const char* Uint64ToString(char buffer[22], Uint64 x, Uint32& size)
{
    return Converter<Sint64, Uint64>::uintToString(buffer, x, size);
}

const char* Sint8ToString(char buffer[22], Sint8 x, Uint32& size)
{
    return Converter<Sint8, Uint8>::sintToString(buffer, x, size);
}

const char* Sint16ToString(char buffer[22], Sint16 x, Uint32& size)
{
    return Converter<Sint16, Uint16>::sintToString(buffer, x, size);
}

const char* Sint32ToString(char buffer[22], Sint32 x, Uint32& size)
{
    return Converter<Sint32, Uint32>::sintToString(buffer, x, size);
}

const char* Sint64ToString(char buffer[22], Sint64 x, Uint32& size)
{
    return Converter<Sint64, Uint64>::sintToString(buffer, x, size);
}

PEGASUS_NAMESPACE_END
