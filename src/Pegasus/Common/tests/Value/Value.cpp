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
// Modified By: Jenny Yu, Hewlett-Packard Company (jenny_yu@hp.com)
//              Karl Schopmeyer (k.schopmeyer@opengroup.org)
//                  20 Feb 2002 - Add tests for new constructor and extend array tests
//              Roger Kumpf, Hewlett-Packard Company (roger_kumpf@hp.com)
//              Carol Ann Krug Graves, Hewlett-Packard Company
//                (carolann_graves@hp.com)
//
//%/////////////////////////////////////////////////////////////////////////////

/*
    This test module tests the functions associated with CIMvalue.
    Feb 2k - Expanded to include isNULL tests and tests of NULL CIMValues
*/

/* ATTN: P3 KS feb 2002 tests to do
    Test for array size
    Test that a !isnull and an is null are not equal
    Test that arrays of different size are not equal, etc.
*/

#include <cassert>
#include <Pegasus/Common/CIMValue.h>
#include <Pegasus/Common/XmlWriter.h>
#include <Pegasus/Common/MofWriter.h>

PEGASUS_USING_PEGASUS;
PEGASUS_USING_STD;

/* The following define is used to control displays of output from the
    test.  Comment out the define to turn off the large quantity of prinout.
    Added the verbose variable which comes from an environment variable.
    IO occurs onlly when the environment variable is set.
*/
#define IO 1

static char * verbose;


/* This template provides a complete set of tests of simple CIMValues for
    the different possible data types.  It tests creating, assigning,
    the equals, creating XML and MOF, and the functions associated with
    clearing testing for the null attribute.
*/
template<class T>
void test01(const T& x)
{
    CIMValue v(x);
    CIMValue v2(v);
    CIMValue v3;
    // Create a null constructor
    CIMType type = v.getType();            // get the type of v
    CIMValue v4(type,false);
    v3 = v2;
#ifdef IO
    if (verbose)
    {
	cout << "\n----------------------\n";
	XmlWriter::printValueElement(v3, cout);
    }
#endif
    try
    {
	T t;
	v3.get(t);
        assert (!v.isNull());
        assert (!v.isArray());
        assert (!v2.isNull());
	assert(t == x);
        assert (v3.typeCompatible(v2));
        // Confirm that the constructor created Null, not array and correct type
        assert (v4.isNull());
        assert (!v4.isArray());
        assert (v4.typeCompatible(v));

        // Test toMof
        Array<Sint8> mofout;
        MofWriter::appendValueElement(mofout, v);
        mofout.append('\0');

        // Test toXml
        Array<Sint8> out;
        XmlWriter::appendValueElement(out, v);
        XmlWriter::appendValueElement(out, v);

        // Test toString
        String valueString = v.toString();
#ifdef IO
	if (verbose)
	{
	    cout << "MOF = [" << mofout.getData() << "]" << endl;
	    cout << "toString Output [" << valueString << "]" << endl;
	}
#endif

    // There should be no exceptions to this point in the test.
    }
    catch(Exception& e)
    {
	cerr << "Error: " << e.getMessage() << endl;
	exit(1);
    }

    // From here forward, in the future we may have exceptions because
    // of the isNull which should generate an exception on the getdata.
    // ATTN: KS 12 Feb 2002 This is work in progress until we complete
    // adding the exception return to the CIMValue get functions.
    try
    {
        // Test for isNull and the setNullValue function
        CIMType type = v.getType();
        v.setNullValue(type, false);
        assert(v.isNull());

        // get the String and XML outputs for v
        String valueString2 = v.toString();
        Array<Sint8> xmlBuffer;
        XmlWriter::appendValueElement(xmlBuffer, v);
        xmlBuffer.append('\0');

        Array<Sint8> mofOutput2;
        MofWriter::appendValueElement(mofOutput2, v);
        mofOutput2.append('\0');
#ifdef IO
	if (verbose)
	{
	    cout << "MOF NULL = [" << mofOutput2.getData() << "]" << endl;
	    cout << "toString NULL Output [" << valueString2 << "]" << endl;
	    cout << " XML NULL = [" << xmlBuffer.getData() << "]" << endl;
	}
#endif
        v.clear();
        assert(v.isNull());
        //v2.clear();
        //assert(v2.isNull();

    }
    catch(Exception& e)
    {
	cerr << "Error: " << e.getMessage() << endl;
	exit(1);
    }

}
/* This template defines the set of tests used for arrays
*/

template<class T>
void test02(const Array<T>& x)
{
    CIMValue va(x);
    CIMValue va2(va);
    CIMValue va3;
    va3 = va2;
    // Create a null constructor
    CIMType type = va.getType();            // get the type of v
    CIMValue va4(type,true);
#ifdef IO
    if (verbose)
    {
	cout << "\n----------------------\n";
	XmlWriter::printValueElement(va3, cout);
    }
#endif
    try
    {
	Array<T> t;
	va3.get(t);
	assert(t == x);
        assert (va3.typeCompatible(va2));
        assert (!va.isNull());
        assert (va.isArray());
        assert (!va2.isNull());
        assert (va2.isArray());
        assert (!va3.isNull());
        assert (va3.isArray());

        // Note that this test depends on what is built.  Everything has 2 entries.
        assert (va.getArraySize() == 3);

        // Confirm that va4 is Null, and array and zero length
        assert (va4.isNull());
        assert (va4.isArray());
        assert (va4.getArraySize() == 0);
        assert (va4.typeCompatible(va));


        // Test toMof
        Array<Sint8> mofOutput;
        MofWriter::appendValueElement(mofOutput, va);
        mofOutput.append('\0');


        // Test toXml
        Array<Sint8> out;
        XmlWriter::appendValueElement(out, va);
        XmlWriter::appendValueElement(out, va);
        // Test toString
        String valueString = va.toString();
#ifdef IO
	if (verbose)
	{
	    cout << "MOF = [" << mofOutput.getData() << "]" << endl;
	    cout << "toString Output [" << valueString << "]" << endl;
	}
#endif
        // There should be no exceptions to this point so the
        // catch simply terminates.
    }
    catch(Exception& e)
    {
	cerr << "Error: " << e.getMessage() << endl;
	exit(1);
    }

    // Test the Null Characteristics
    try
    {
        // Set the initial one to Null
        CIMType type = va.getType();
        va.setNullValue(type, true, 0);
        assert(va.isNull());
        assert(va.isArray());
        assert(va.getArraySize() == 0);
        va.setNullValue(type, false);
        assert(va.isNull());
        assert(!va.isArray());

        // get the String and XML outputs for v
        String valueString2 = va.toString();
        Array<Sint8> xmlBuffer;
        XmlWriter::appendValueElement(xmlBuffer, va);
        xmlBuffer.append('\0');

        Array<Sint8> mofOutput2;
        MofWriter::appendValueElement(mofOutput2, va);
        mofOutput2.append('\0');
#ifdef IO
	if (verbose)
	{
	    cout << "MOF NULL = [" << mofOutput2.getData() << "]" << endl;
	    cout << "toString NULL Output [" << valueString2 << "]" << endl;
	    cout << " XML NULL = [" << xmlBuffer.getData() << "]" << endl;
	}
#endif
        va.clear();
        assert(va.isNull());
    }
    catch(Exception& e)
    {
	cerr << "Error: " << e.getMessage() << endl;
	exit(1);
    }

}

int main(int argc, char** argv)
{
#ifdef IO
    verbose = getenv("PEGASUS_TEST_VERBOSE");
    if (verbose)
	cout << "Test CIMValue. To turn off display, compile with IO undefined\n";
#endif
    // Test the primitive CIMValue types with test01
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
    test01(CIMObjectPath("//host1:77/root/test:Class1.key1=\"key1Value\",key2=\"key2Value\""));

    // Test CIMValue arrays

    Array<Uint8> arr1;
    arr1.append(11);
    arr1.append(22);
    arr1.append(23);
    test02(arr1);

    Array<Uint16> arr2;
    arr2.append(333);
    arr2.append(444);
    arr2.append(445);
    test02(arr2);

    Array<Uint32> arr3;
    arr3.append(5555);
    arr3.append(6666);
    arr3.append(6667);
    test02(arr3);

    Array<Uint64> arr4;
    arr4.append(123456789);
    arr4.append(987654321);
    arr4.append(987654322);
    test02(arr4);

    Array<Sint8> arr5;
    arr5.append(-11);
    arr5.append(-22);
    arr5.append(-23);
    test02(arr5);

    Array<Sint16> arr6;
    arr6.append(333);
    arr6.append(444);
    arr6.append(555);
    test02(arr6);

    Array<Sint32> arr7;
    arr7.append(555);
    arr7.append(666);
    arr7.append(777);
    test02(arr7);

    Array<Sint64> arr8;
    arr8.append(-123456789);
    arr8.append(-987654321);
    arr8.append(-987654321);
    test02(arr8);

    Array<Boolean> arr9;
    arr9.append(true);
    arr9.append(false);
    arr9.append(false);
    test02(arr9);

    Array<Real32> arr10;
    arr10.append(1.55);
    arr10.append(2.66);
    arr10.append(3.77);
    test02(arr10);

    Array<Real64> arr11;
    arr11.append(55.55);
    arr11.append(66.66);
    arr11.append(77.77);
    test02(arr11);

    Array<Char16> arr12;
    arr12.append('X');
    arr12.append('Y');
    arr12.append('Z');
    test02(arr12);

    Array<String> arr13;
    arr13.append("One");
    arr13.append("Two");
    arr13.append("Three");
    test02(arr13);

    Array<CIMDateTime> arr14;
    arr14.append(CIMDateTime ("20020130120000.000000+360"));
    arr14.append(CIMDateTime ("20020201120000.000000+360"));
    arr14.append(CIMDateTime ("20020202120000.000000+360"));
    test02(arr14);

    Array<CIMObjectPath> arr15;
    arr15.append(CIMObjectPath("//host1:77/root/test:Class1.key1=\"key1Value\",key2=\"key2Value\""));
    arr15.append(CIMObjectPath("//host2:88/root/static:Class2.keyA=\"keyAValue\",keyB=\"keyBValue\""));
    arr15.append(CIMObjectPath("//host3:99/root/test/static:Class3.keyX=\"keyXValue\",keyY=\"keyYValue\""));
    test02(arr15);

    cout << argv[0] << " +++++ passed all tests" << endl;

    return 0;
}
