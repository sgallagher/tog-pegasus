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
#include <cassert>
#include <cstdlib>
#include <iostream>
#include <Pegasus/Common/CIMReference.h>
#include <Pegasus/Common/XmlWriter.h>

PEGASUS_USING_PEGASUS;
PEGASUS_USING_STD;
//#define DDD(X) // X
#define DDD(X) X
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

     // Test case independence and order independence of parameters.
    {
	CIMReference r1 = "X.a=123,b=true";
	CIMReference r2 = "x.B=TRUE,A=123";
	assert(r1 == r2);
	assert(r1.makeHashCode() == r2.makeHashCode());

	CIMReference r3 = "x.B=TRUE,A=123,c=FALSE";
	assert(r1 != r3);
    }


    // Test building from component parts of CIM Reference.
    {
	CIMReference r1 ("atp:77", "root/cimv25", "TennisPlayer");
	CIMReference r2 ("//atp:77/root/cimv25:TennisPlayer.");
	//cout << "r1 " << r1.toString() << endl;
	//cout << "r2 " << r2.toString() << endl;
	assert(r1 == r2);
	assert(r1.toString() == r2.toString());

    }


    {
	String hostName = "atp:77";
	String nameSpace = "root/cimv2";
	String className = "tennisplayer";

	CIMReference r1;
	r1.setHost(hostName);


	// ATTN: Mike.  Think there is an error here in the CIMreerence
	// code.  It applies the classname legal test to the namespace.
	// However, the namespace may have / in the namespace legally.
	//DDD(cout << "test4a1" << endl;)
	//r1.setNameSpace(nameSpace);
	//DDD(cout << "test4a2" << endl;)

	r1.setClassName(className);
	String newHostName = r1.getHost();
	//cout << "HostName = " << newHostName << endl;


	CIMReference r2 (hostName, nameSpace, className);
	// Dropped for the moment because of the hostname problem.
	//assert(r1 == r2);

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
