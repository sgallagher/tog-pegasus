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
// Modified By:
//
//%/////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
//
// The repository contains empty class files (don't try to read them in).
//
// The inheritance structure for this test is:
//
//	Class1
//	Class2
//	A
//	  X
//	    M
//	      Q
//	      R
//	    N
//	      S
//	      T
//	  Y
//	  Z
//
////////////////////////////////////////////////////////////////////////////////
#include <cassert>
#include <Pegasus/Repository/CIMRepository.h>

PEGASUS_USING_PEGASUS;
PEGASUS_USING_STD;

const String NAMESPACE = "/zzz";

void print(const Array<String>& array)
{
#if 0
    for (Uint32 i = 0, n = array.size(); i < n; i++)
	cout << "array[" << i << "]: " << array[i] << endl;
#endif
}

void TestCase1()
{
    CIMRepository r("./repository");

    // Enumerate the class names:

    Array<String> classNames = r.enumerateClassNames(
	NAMESPACE, String::EMPTY, false);

    print(classNames);

    Array<String> tmp;
    tmp.append("A");
    tmp.append("Class1");
    tmp.append("Class2");

    BubbleSort(tmp);
    BubbleSort(classNames);
    assert(tmp.size() == 3);
    assert(tmp.size() == classNames.size());

    assert(Equal(tmp, classNames));
}

void TestCase2()
{
    CIMRepository r("./repository");

    // Enumerate the class names:

    const String NAMESPACE = "/zzz";

    Array<String> classNames = r.enumerateClassNames(
	NAMESPACE, String::EMPTY, true);

    print(classNames);

    Array<String> tmp;
    tmp.append("A");
    tmp.append("Class1");
    tmp.append("Class2");
    tmp.append("X");
    tmp.append("Y");
    tmp.append("Z");
    tmp.append("M");
    tmp.append("N");
    tmp.append("Q");
    tmp.append("R");
    tmp.append("S");
    tmp.append("T");

    BubbleSort(tmp);
    BubbleSort(classNames);
    assert(tmp == classNames);
}

void TestCase3()
{
    CIMRepository r("./repository");

    // Enumerate the class names:

    const String NAMESPACE = "/zzz";

    Array<String> classNames = r.enumerateClassNames(
	NAMESPACE, "X", false);

    print(classNames);

    Array<String> tmp;
    tmp.append("M");
    tmp.append("N");

    BubbleSort(tmp);
    BubbleSort(classNames);
    assert(tmp == classNames);
}

void TestCase4()
{
    CIMRepository r("./repository");

    // Enumerate the class names:

    const String NAMESPACE = "/zzz";

    Array<String> classNames = r.enumerateClassNames(
	NAMESPACE, "X", true);

    print(classNames);

    Array<String> tmp;
    tmp.append("M");
    tmp.append("N");
    tmp.append("Q");
    tmp.append("R");
    tmp.append("S");
    tmp.append("T");

    BubbleSort(tmp);
    BubbleSort(classNames);
    assert(tmp == classNames);
}

static void CreateClass(
    CIMRepository& r,
    const String& className, 
    const String superClassName = String())
{
    CIMClass c(className, superClassName);
    r.createClass(NAMESPACE, c);
}

int main()
{
    CIMRepository r("./repository");

    try 
    {
	r.createNameSpace(NAMESPACE);
	CreateClass(r, "Class1");
	CreateClass(r, "Class2");
	CreateClass(r, "A");
	CreateClass(r, "X", "A");
	CreateClass(r, "Y", "A");
	CreateClass(r, "Z", "A");
	CreateClass(r, "M", "X");
	CreateClass(r, "N", "X");
	CreateClass(r, "Q", "M");
	CreateClass(r, "R", "M");
	CreateClass(r, "S", "N");
	CreateClass(r, "T", "N");
	TestCase1();
	TestCase2();
	TestCase3();
	TestCase4();
    }
    catch (Exception& e)
    {
	cout << e.getMessage() << endl;
    }

    cout << "+++++ passed all tests" << endl;

    return 0;
}
