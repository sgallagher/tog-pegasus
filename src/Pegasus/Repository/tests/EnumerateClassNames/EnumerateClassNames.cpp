//%2005////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001, 2002 BMC Software; Hewlett-Packard Development
// Company, L.P.; IBM Corp.; The Open Group; Tivoli Systems.
// Copyright (c) 2003 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation, The Open Group.
// Copyright (c) 2004 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation; VERITAS Software Corporation; The Open Group.
// Copyright (c) 2005 Hewlett-Packard Development Company, L.P.; IBM Corp.;
// EMC Corporation; VERITAS Software Corporation; The Open Group.
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

static char * verbose;

const CIMNamespaceName NAMESPACE = CIMNamespaceName ("test/zzz");
String repositoryRoot;

void print(const Array<CIMName>& array)
{
    if (verbose)
	{
		for (Uint32 i = 0, n = array.size(); i < n; i++)
		cout << "array[" << i << "]: " << array[i].getString() << endl;
	}
}

void TestCase1()
{
	if (verbose)
	{
		cout << "TestCase1" << endl;
	}
    CIMRepository r (repositoryRoot);

    // Enumerate the class names:

    Array<CIMName> classNames = r.enumerateClassNames(
			NAMESPACE, CIMName(), false);

    print(classNames);
    
	Array<CIMName> tmp;
    tmp.append(CIMName ("TST_A"));
    tmp.append(CIMName ("TST_Class1"));
    tmp.append(CIMName ("TST_Class2"));
	
    BubbleSort(tmp);
    BubbleSort(classNames);
    assert(tmp.size() == 3);
    assert(tmp.size() == classNames.size());

    assert(tmp == classNames);
}

void TestCase2()
{
	if (verbose)
	{
		cout << "TestCase2" << endl;
	}
    CIMRepository r (repositoryRoot);

    // Enumerate the class names:

    const CIMNamespaceName NAMESPACE = CIMNamespaceName ("test/zzz");

    Array<CIMName> classNames = r.enumerateClassNames(
	NAMESPACE, CIMName(), true);

    print(classNames);

    Array<CIMName> tmp;
    tmp.append(CIMName ("TST_A"));
    tmp.append(CIMName ("TST_Class1"));
    tmp.append(CIMName ("TST_Class2"));
    tmp.append(CIMName ("TST_X"));
    tmp.append(CIMName ("TST_Y"));
    tmp.append(CIMName ("TST_Z"));
    tmp.append(CIMName ("TST_M"));
    tmp.append(CIMName ("TST_N"));
    tmp.append(CIMName ("TST_Q"));
    tmp.append(CIMName ("TST_R"));
    tmp.append(CIMName ("TST_S"));
    tmp.append(CIMName ("TST_T"));

    BubbleSort(tmp);
    BubbleSort(classNames);
    assert(tmp == classNames);
}

void TestCase3()
{
	if (verbose)
	{
		cout << "TestCase3" << endl;
	}
    CIMRepository r (repositoryRoot);

    // Enumerate the class names:

    const CIMNamespaceName NAMESPACE = CIMNamespaceName ("test/zzz");

    Array<CIMName> classNames = r.enumerateClassNames(
	NAMESPACE, CIMName ("TST_X"), false);

    print(classNames);

    Array<CIMName> tmp;
    tmp.append(CIMName ("TST_M"));
    tmp.append(CIMName ("TST_N"));

    BubbleSort(tmp);
    BubbleSort(classNames);
    assert(tmp == classNames);
}

void TestCase4()
{
	if (verbose)
	{
		cout << "TestCase4" << endl;
	}
    CIMRepository r (repositoryRoot);

    // Enumerate the class names:

    const CIMNamespaceName NAMESPACE = CIMNamespaceName ("test/zzz");

    Array<CIMName> classNames = r.enumerateClassNames(
	NAMESPACE, CIMName ("TST_X"), true);

    print(classNames);

    Array<CIMName> tmp;
    tmp.append(CIMName ("TST_M"));
    tmp.append(CIMName ("TST_N"));
    tmp.append(CIMName ("TST_Q"));
    tmp.append(CIMName ("TST_R"));
    tmp.append(CIMName ("TST_S"));
    tmp.append(CIMName ("TST_T"));

    BubbleSort(tmp);
    BubbleSort(classNames);
    assert(tmp == classNames);
}

static void CreateClass(
    CIMRepository& r,
    const CIMName& className, 
    const CIMName superClassName = CIMName())
{
    CIMClass c(className, superClassName);
    r.createClass(NAMESPACE, c);
}

int main(int argc, char** argv)
{
    verbose = getenv("PEGASUS_TEST_VERBOSE");
    
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

    CIMRepository_Mode mode;
    if (!strcmp(argv[1],"XML") )
      {
	mode.flag = CIMRepository_Mode::NONE;
	if (verbose) cout << argv[0]<< ": using XML mode repository" << endl;
      }
      else if (!strcmp(argv[1],"BIN") )
	{
	  mode.flag = CIMRepository_Mode::BIN;
	  if (verbose) cout << argv[0]<< ": using BIN mode repository" << endl;
	}
      else
	{
	  cout << argv[0] << ": invalid argument: " << argv[1] << endl;
	  return 0;
	}

    CIMRepository r (repositoryRoot, mode);

    try 
    {
		r.createNameSpace(NAMESPACE);
		CreateClass(r, CIMName ("TST_Class1"));
		CreateClass(r, CIMName ("TST_Class2"));
		CreateClass(r, CIMName ("TST_A"));
		CreateClass(r, CIMName ("TST_X"), CIMName ("TST_A"));
		CreateClass(r, CIMName ("TST_Y"), CIMName ("TST_A"));
		CreateClass(r, CIMName ("TST_Z"), CIMName ("TST_A"));
		CreateClass(r, CIMName ("TST_M"), CIMName ("TST_X"));
		CreateClass(r, CIMName ("TST_N"), CIMName ("TST_X"));
		CreateClass(r, CIMName ("TST_Q"), CIMName ("TST_M"));
		CreateClass(r, CIMName ("TST_R"), CIMName ("TST_M"));
		CreateClass(r, CIMName ("TST_S"), CIMName ("TST_N"));
		CreateClass(r, CIMName ("TST_T"), CIMName ("TST_N"));
		TestCase1();
		TestCase2();
		TestCase3();
		TestCase4();
    }
    catch (Exception& e)
    {
		cout << "Exception " << e.getMessage() << endl;
		assert(false);
    }

    cout << argv[0] << " " << argv[1] << " +++++ passed all tests" << endl;

    return 0;
}
