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
#include <cassert>
#include <Pegasus/Repository/InheritanceTree.h>

PEGASUS_USING_PEGASUS;
PEGASUS_USING_STD;

static char * verbose;

void TestGetSubClassNames(
    const InheritanceTree& it,
    const CIMName& className,
    Boolean deepInheritance,
    const Array<CIMName>& expectedSubClassNames)
{
    Array<CIMName> subClassNames;
    it.getSubClassNames(className, deepInheritance, subClassNames);

#if 0
    for (Uint32 i = 0; i < subClassNames.size(); i++)
	cout << subClassNames[i] << endl;
#endif

    Array<CIMName> expected = subClassNames;

    BubbleSort(expected);
    BubbleSort(subClassNames);
    assert(expected == subClassNames);
}

int main(int argc, char** argv)
{
    verbose = getenv("PEGASUS_TEST_VERBOSE");

    try
    {
	InheritanceTree it;

	/*
	//----------------------------------------------------------------------
	//
	//        A
	//      /   \
	//     B     C
	//   /   \     \
	//  D     E     F
	//
	//----------------------------------------------------------------------
        */

	it.insert("D", "B");
	it.insert("E", "B");
	it.insert("B", "A");
	it.insert("C", "A");
	it.insert("F", "C");
	it.insert("A", "");
	it.check();

        if (verbose)
	    it.print(cout);

	{
	    Array<CIMName> expected;
	    expected.append("B");
	    expected.append("C");
	    expected.append("D");
	    expected.append("E");
	    expected.append("F");
	    TestGetSubClassNames(it, "A", true, expected);
	}
	{
	    Array<CIMName> expected;
	    expected.append("B");
	    expected.append("C");
	    TestGetSubClassNames(it, "A", false, expected);
	}
	{
	    Array<CIMName> expected;
	    expected.append("A");
	    TestGetSubClassNames(it, CIMName(), false, expected);
	}
	{
	    Array<CIMName> expected;
	    expected.append("A");
	    expected.append("B");
	    expected.append("C");
	    expected.append("D");
	    expected.append("E");
	    expected.append("F");
	    TestGetSubClassNames(it, "A", true, expected);
	}
	{
	    Array<CIMName> expected;

	    expected.append("F");
	    TestGetSubClassNames(it, "C", true, expected);
	}
	{
	    Array<CIMName> expected;
	    expected.append("F");
	    TestGetSubClassNames(it, "C", false, expected);
	}
    }
    catch (Exception& e)
    {
	cerr << e.getMessage() << endl;
	exit(1);
    }

    try
    {
	InheritanceTree it;
	it.insertFromPath("./classes");
	it.check();
        if (verbose)
	    it.print(cout);
    }
    catch (Exception& e)
    {
	cerr << e.getMessage() << endl;
	exit(1);
    }

    try
    {
        // build an invalid inheritance tree
	InheritanceTree it;
        it.insert("D", "B");
        it.insert("E", "B");
        it.insert("C", "A");
        it.insert("F", "C");
        it.insert("A", "");
        it.check();
    }
    catch (InvalidInheritanceTree& e)
    {
        if (verbose)
	    cout << argv[0] << " " << e.getMessage() << endl;
    }
    catch (Exception& e)
    {
	cerr << e.getMessage() << endl;
	exit(1);
    }

#if 0
    InheritanceTree it;
    it.insertFromPath("e:/tog/pegasus_home/repository/root#cimv2/classes");
    it.check();
    it.print(cout);
#endif

    cout << argv[0] << " +++++ passed all tests" << endl;

    return 0;
}
