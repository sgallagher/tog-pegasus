#include <iostream>
#include <Pegasus/Repository/InheritanceTree.h>

using namespace std;
using namespace Pegasus;

int main()
{
    try
    {
	InheritanceTree it;

	//
	//        A
	//      /   \
	//     B     C
	//   /   \     \
	//  D     E     F
	//

	it.insert("D", "B");
	it.insert("E", "B");
	it.insert("B", "A");
	it.insert("C", "A");
	it.insert("F", "C");
	it.insert("A", "");
	it.check();

	// it.print(cout);
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

    cout << "+++++ passed all tests" << endl;

    return 0;
}
