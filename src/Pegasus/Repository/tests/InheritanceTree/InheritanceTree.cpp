#include <iostream>
#include <cassert>
#include <Pegasus/Repository/InheritanceTree.h>

using namespace std;
using namespace Pegasus;

void TestGetSubClassNames(
    const InheritanceTree& it,
    const String& className,
    Boolean deepInheritance,
    const Array<String>& expectedSubClassNames)
{
    Array<String> subClassNames;
    it.getSubClassNames(className, deepInheritance, subClassNames);

#if 0
    for (Uint32 i = 0; i < subClassNames.getSize(); i++)
	cout << subClassNames[i] << endl;
#endif

    Array<String> expected = subClassNames;

    BubbleSort(expected);
    BubbleSort(subClassNames);
    assert(expected == subClassNames);
}

int main()
{
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

	// it.print(cout);

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
	// it.print(cout);
    }
    catch (Exception& e)
    {
	cerr << e.getMessage() << endl;
	exit(1);
    }

#if 0
    InheritanceTree it;
    it.insertFromPath("e:/tog/pegasus_home/repository/root#cimv20/classes");
    it.check();
    it.print(cout);
#endif

    cout << "+++++ passed all tests" << endl;

    return 0;
}
