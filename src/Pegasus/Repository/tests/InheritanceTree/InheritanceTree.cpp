#include <cassert>
#include <Pegasus/Repository/InheritanceTree.h>

PEGASUS_USING_PEGASUS;
PEGASUS_USING_STD;

static char * verbose;

void TestGetSubClassNames(
    const InheritanceTree& it,
    const String& className,
    Boolean deepInheritance,
    const Array<String>& expectedSubClassNames)
{
    Array<String> subClassNames;
    it.getSubClassNames(className, deepInheritance, subClassNames);

#if 0
    for (Uint32 i = 0; i < subClassNames.size(); i++)
	cout << subClassNames[i] << endl;
#endif

    Array<String> expected = subClassNames;

    BubbleSort(expected);
    BubbleSort(subClassNames);
    assert(expected == subClassNames);
}

int main()
{
    verbose = getenv("PEGASUS_TEST_VERBOSE");

    try
    {
	InheritanceTree it;

	//----------------------------------------------------------------------
	//
	//        A
	//      /   \
	//     B     C
	//   /   \     \
	//  D     E     F
	//
	//----------------------------------------------------------------------

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
	    Array<String> expected;
	    expected.append("B");
	    expected.append("C");
	    expected.append("D");
	    expected.append("E");
	    expected.append("F");
	    TestGetSubClassNames(it, "A", true, expected);
	}
	{
	    Array<String> expected;
	    expected.append("B");
	    expected.append("C");
	    TestGetSubClassNames(it, "A", false, expected);
	}
	{
	    Array<String> expected;
	    expected.append("A");
	    TestGetSubClassNames(it, "", false, expected);
	}
	{
	    Array<String> expected;
	    expected.append("A");
	    expected.append("B");
	    expected.append("C");
	    expected.append("D");
	    expected.append("E");
	    expected.append("F");
	    TestGetSubClassNames(it, "A", true, expected);
	}
	{
	    Array<String> expected;
	    expected.append("F");
	    TestGetSubClassNames(it, "C", true, expected);
	}
	{
	    Array<String> expected;
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
	    cout << e.getMessage() << endl;
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

    cout << "+++++ passed all tests" << endl;

    return 0;
}
