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
#include <Pegasus/Common/CIMValue.h>

using namespace Pegasus;
using namespace std;

// #define IO

template<class T>
void test01(const T& x)
{
    CIMValue v(x);
    CIMValue v2(v);
    CIMValue v3;
    v3 = v2;
#ifdef IO
    v3.print();
#endif
    try
    {
	T t;
	v3.get(t);
	assert(t == x);
    }
    catch(Exception& e)
    {
	cerr << "Error: " << e.getMessage() << endl;
	exit(1);
    }
}

int main()
{
    Array<Uint32> aa;
    CIMValue vv(aa);

    test01(Boolean(true));
    test01(Boolean(false));
    test01(Char16('Z'));
    test01(Uint8(77));
    test01(Sint8(-77));
    test01(Sint16(77));
    test01(Uint16(-77));
    test01(Sint32(77));
    test01(Uint32(-77));
    test01(Sint64(77));
    test01(Uint64(-77));
    test01(Real32(1.5));
    test01(Real64(55.5));
    test01(Uint64(123456789));
    test01(Sint64(-123456789));

    Array<String> arr;
    arr.append("One");
    arr.append("Two");
    arr.append("Three");
    CIMValue v(arr);

#ifdef IO
    v.print();
#endif

    cout << "+++++ passed all tests" << endl;

    return 0;
}
