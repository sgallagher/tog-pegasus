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
// Modified By: Jenny Yu, Hewlett-Packard Company (jenny_yu@hp.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#include <cassert>
#include <Pegasus/Common/CIMValue.h>

PEGASUS_USING_PEGASUS;
PEGASUS_USING_STD;

#define IO

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

        // Test toMof
        Array<Sint8> mofout;
        v.toMof(mofout);

        // Test toXml
        String xmlString = v.toXml();

        // Test toString
        String valueString = v.toString();
    }
    catch(Exception& e)
    {
	cerr << "Error: " << e.getMessage() << endl;
	exit(1);
    }
}

template<class T>
void test02(const Array<T>& x)
{
    CIMValue va(x);
    CIMValue va2(va);
    CIMValue va3;
    va3 = va2;
#ifdef IO
    va3.print();
#endif
    try
    {
	Array<T> t;
	va3.get(t);
	assert(t == x);

        // Test toMof
        Array<Sint8> mofout;
        va.toMof(mofout);

        // Test toXml
        String xmlString = va.toXml();

        // Test toString
        String valueString = va.toString();
    }
    catch(Exception& e)
    {
	cerr << "Error: " << e.getMessage() << endl;
	exit(1);
    }
}

int main()
{
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
    test01(String("Hello world"));
    test01(CIMDateTime("19991224120000.000000+360"));

    // Test CIMValue arrays

    Array<Uint8> arr1;
    arr1.append(11);
    arr1.append(22);
    test02(arr1);

    Array<Uint16> arr2;
    arr2.append(333);
    arr2.append(444);
    test02(arr2);

    Array<Uint32> arr3;
    arr3.append(5555);
    arr3.append(6666);
    test02(arr3);

    Array<Uint64> arr4;
    arr4.append(123456789);
    arr4.append(987654321);
    test02(arr4);

    Array<Sint8> arr5;
    arr5.append(-11);
    arr5.append(-22);
    test02(arr5);

    Array<Sint16> arr6;
    arr6.append(333);
    arr6.append(444);
    test02(arr6);

    Array<Sint32> arr7;
    arr7.append(555);
    arr7.append(666);
    test02(arr7);

    Array<Sint64> arr8;
    arr8.append(-123456789);
    arr8.append(-987654321);
    test02(arr8);

    Array<Boolean> arr9;
    arr9.append(true);
    arr9.append(false);
    test02(arr9);

    Array<Real32> arr10;
    arr10.append(1.55);
    arr10.append(2.66);
    test02(arr10);

    Array<Real64> arr11;
    arr11.append(55.55);
    arr11.append(66.66);
    test02(arr11);

    Array<Char16> arr12;
    arr12.append('X');
    arr12.append('Y');
    test02(arr12);

    Array<String> arr13;
    arr13.append("One");
    arr13.append("Two");
    test02(arr13);

    Array<CIMDateTime> arr14;
    arr14.append("20020130120000.000000+360");
    arr14.append("20020201120000.000000+360");
    arr14.append("20020202120000.000000+360");
    test02(arr14);

    cout << "+++++ passed all tests" << endl;

    return 0;
}
