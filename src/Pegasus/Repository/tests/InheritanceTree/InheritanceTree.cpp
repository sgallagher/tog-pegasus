#include <iostream>
#include <Pegasus/Repository/InheritanceTree.h>

using namespace std;
using namespace Pegasus;

int main()
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

    it.print(cout);

    return 0;
}
