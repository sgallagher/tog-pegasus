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
// Modified By: Carol Ann Krug Graves, Hewlett-Packard Company
//              (carolann_graves@hp.com)
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
#include <Pegasus/Common/Config.h>
#include <cassert>
#include <Pegasus/Repository/CIMRepository.h>

PEGASUS_USING_PEGASUS;
PEGASUS_USING_STD;

const CIMNamespaceName NAMESPACE = CIMNamespaceName ("/zzz");
String repositoryRoot;

void print(const Array<CIMName>& array)
{
#if 0
    for (Uint32 i = 0, n = array.size(); i < n; i++)
	cout << "array[" << i << "]: " << array[i] << endl;
#endif
}

void TestCase1()
{
    CIMRepository r (repositoryRoot);

    // Enumerate the class names:

    Array<CIMName> classNames = r.enumerateClassNames(
	NAMESPACE, String::EMPTY, false);

    print(classNames);

    Array<CIMName> tmp;
    tmp.append(CIMName ("A"));
    tmp.append(CIMName ("Class1"));
    tmp.append(CIMName ("Class2"));

    BubbleSort(tmp);
    BubbleSort(classNames);
    assert(tmp.size() == 3);
    assert(tmp.size() == classNames.size());

    assert(tmp == classNames);
}

void TestCase2()
{
    CIMRepository r (repositoryRoot);

    // Enumerate the class names:

    const CIMNamespaceName NAMESPACE = CIMNamespaceName ("/zzz");

    Array<CIMName> classNames = r.enumerateClassNames(
	NAMESPACE, String::EMPTY, true);

    print(classNames);

    Array<CIMName> tmp;
    tmp.append(CIMName ("A"));
    tmp.append(CIMName ("Class1"));
    tmp.append(CIMName ("Class2"));
    tmp.append(CIMName ("X"));
    tmp.append(CIMName ("Y"));
    tmp.append(CIMName ("Z"));
    tmp.append(CIMName ("M"));
    tmp.append(CIMName ("N"));
    tmp.append(CIMName ("Q"));
    tmp.append(CIMName ("R"));
    tmp.append(CIMName ("S"));
    tmp.append(CIMName ("T"));

    BubbleSort(tmp);
    BubbleSort(classNames);
    assert(tmp == classNames);
}

void TestCase3()
{
    CIMRepository r (repositoryRoot);

    // Enumerate the class names:

    const CIMNamespaceName NAMESPACE = CIMNamespaceName ("/zzz");

    Array<CIMName> classNames = r.enumerateClassNames(
	NAMESPACE, CIMName ("X"), false);

    print(classNames);

    Array<CIMName> tmp;
    tmp.append(CIMName ("M"));
    tmp.append(CIMName ("N"));

    BubbleSort(tmp);
    BubbleSort(classNames);
    assert(tmp == classNames);
}

void TestCase4()
{
    CIMRepository r (repositoryRoot);

    // Enumerate the class names:

    const CIMNamespaceName NAMESPACE = CIMNamespaceName ("/zzz");

    Array<CIMName> classNames = r.enumerateClassNames(
	NAMESPACE, CIMName ("X"), true);

    print(classNames);

    Array<CIMName> tmp;
    tmp.append(CIMName ("M"));
    tmp.append(CIMName ("N"));
    tmp.append(CIMName ("Q"));
    tmp.append(CIMName ("R"));
    tmp.append(CIMName ("S"));
    tmp.append(CIMName ("T"));

    BubbleSort(tmp);
    BubbleSort(classNames);
    assert(tmp == classNames);
}

static void CreateClass(
    CIMRepository& r,
    const CIMName& className, 
    const CIMName superClassName = String())
{
    CIMClass c(className, superClassName);
    r.createClass(NAMESPACE, c);
}

int main()
{
    const char* tmpDir;
    tmpDir = getenv ("PEGASUS_TMP");
    if (tmpDir == NULL)
    {
        repositoryRoot = ".";
    }
    else
    {
        repositoryRoot = tmpDir;
    }
    repositoryRoot.append("/repository");

    CIMRepository r (repositoryRoot);

    try 
    {
	r.createNameSpace(NAMESPACE);
	CreateClass(r, CIMName ("Class1"));
	CreateClass(r, CIMName ("Class2"));
	CreateClass(r, CIMName ("A"));
	CreateClass(r, CIMName ("X"), CIMName ("A"));
	CreateClass(r, CIMName ("Y"), CIMName ("A"));
	CreateClass(r, CIMName ("Z"), CIMName ("A"));
	CreateClass(r, CIMName ("M"), CIMName ("X"));
	CreateClass(r, CIMName ("N"), CIMName ("X"));
	CreateClass(r, CIMName ("Q"), CIMName ("M"));
	CreateClass(r, CIMName ("R"), CIMName ("M"));
	CreateClass(r, CIMName ("S"), CIMName ("N"));
	CreateClass(r, CIMName ("T"), CIMName ("N"));
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
