#include <cassert>
#include <iostream>
#include <Pegasus/Common/CIMReference.h>
#include <Pegasus/Common/XmlWriter.h>

PEGASUS_USING_PEGASUS;
PEGASUS_USING_STD;

void test01()
{
    {
    String on1;
    on1 = "//atp:77/root/cimv25:TennisPlayer.last=\"Rafter\",first=\"Patrick\"";

    String on2;
    on2 = "//atp:77/root/cimv25:TennisPlayer.first=\"Patrick\",last=\"Rafter\"";

    CIMReference r = on1;
    assert(r.toString() != on1);
    assert(r.toString() == on2);
    }

    {
    CIMReference r1 = "MyClass.z=true,y=1234,x=\"Hello World\"";
    CIMReference r2 = "myclass.X=\"Hello World\",Z=true,Y=1234";
    CIMReference r3 = "myclass.X=\"Hello\",Z=true,Y=1234";
    // cout << r1.toString() << endl;
    // cout << r2.toString() << endl;
    assert(r1 == r2);
    assert(r1 != r3);
    }

    {
	CIMReference r1 = "X.a=123,b=true";
	CIMReference r2 = "x.B=TRUE,A=123";
	assert(r1 == r2);
	assert(r1.makeHashCode() == r2.makeHashCode());
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
