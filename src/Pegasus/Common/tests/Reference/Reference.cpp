#include <cassert>
#include <iostream>
#include <Pegasus/Common/CIMReference.h>
#include <Pegasus/Common/XmlWriter.h>

using namespace Pegasus;
using namespace std;

void test01()
{
    {
    String op;
    op = "//atp:77/root/cimv25:TennisPlayer.first=\"Patrick\",last=\"Rafter\"";
    CIMReference r = op;
    assert(op == r.toObjectPath());
    }

    {
    CIMReference r1 = "MyClass.z=true,y=1234,x=\"Hello World\"";
    CIMReference r2 = "myclass.X=\"Hello World\",Z=true,Y=1234";
    CIMReference r3 = "myclass.X=\"Hello\",Z=true,Y=1234";
    // cout << r1.toObjectPath() << endl;
    // cout << r2.toObjectPath() << endl;
    assert(r1 == r2);
    assert(r1 != r3);
    }
}

int main()
{
    try 
    {
	test01();

	cout << "+++++ passed all tests" << endl;
    }
    catch (Exception& e)
    {
	cerr << e.getMessage() << endl;
	exit(1);
    }
    return 0;
}
