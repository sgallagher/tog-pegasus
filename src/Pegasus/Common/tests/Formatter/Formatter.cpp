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

#include <cassert>
#include <iostream>
#include <Pegasus/Common/Formatter.h>

PEGASUS_USING_PEGASUS;
PEGASUS_USING_STD;

#define DDD(X) // X
//#define DDD(X) X

int main()
{
    String str = 
	Formatter::format("\\$dollar\\$ X=$0, Y=$1, Z=$2, X=$0", 88, "Hello World", 7.5);

     DDD(cout << "str[" << str << "]" << endl;)

     // test for Uint32 expressions.

    Uint32 test = 99;
    Uint32 big = 32999;

    String str2 = Formatter::format("test $0 big $1", test, big);
    DDD(cout << "str2[" << str2 << "]" << endl;)

    assert(Formatter::format("[TRUE$0]", test) == "[TRUE99]");

    // Test for booleans.
    Boolean flag = true;
    Boolean flag2 = false;

    String str3 = Formatter::format(" TRUE $0 FALSE $1 FALSE $2",
				     flag, !flag, flag2);

    assert(Formatter::format("TRUE$0", flag ) == "TRUEtrue");
    assert(Formatter::format("FALSE$0", flag2 ) == "FALSEfalse");

    DDD(cout << "str3[" << str3 << "]" << endl;)

    // Test for 64 bit integer
    {
	 Sint64 big = 32999;

    }

    cout << "+++++ passed all tests" << endl;

    return 0;
}
