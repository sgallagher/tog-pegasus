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
// Modified By:  Carol Ann Krug Graves, Hewlett-Packard Company
//                   (carolann_graves@hp.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#include <cassert>
#include <cstdlib>
#include <iostream>
#include <Pegasus/Common/CIMObjectPath.h>
#include <Pegasus/Common/CIMName.h>
#include <Pegasus/Common/XmlWriter.h>
#include <Pegasus/Common/MofWriter.h>

PEGASUS_USING_PEGASUS;
PEGASUS_USING_STD;
//#define DDD(X) // X
#define DDD(X) X

static char * verbose;

void test01()
{
    {
    String on1;
    on1 = "//atp:77/root/cimv25:TennisPlayer.last=\"Rafter\",first=\"Patrick\"";

    String on2;
    on2 = "//atp:77/root/cimv25:TennisPlayer.first=\"Patrick\",last=\"Rafter\"";

    CIMObjectPath r = on1;
    assert(r.toString() != on1);
    assert(r.toString() == on2);

    CIMObjectPath r2 = r;
    CIMObjectPath r3 = CIMObjectPath 
        ("//atp:77/root/cimv25:TennisPlayer.first=\"Chris\",last=\"Evert\"");

    if (verbose)
    {
        XmlWriter::printValueReferenceElement(r);
        cout << r.toString() << endl;
    }

    Array<Sint8> mofOut;
    MofWriter::appendValueReferenceElement(mofOut, r);
    r.clear();
    }

    {
    CIMObjectPath r1 = CIMObjectPath 
        ("MyClass.z=true,y=1234,x=\"Hello World\"");
    CIMObjectPath r2 = CIMObjectPath 
        ("myclass.X=\"Hello World\",Z=true,Y=1234");
    CIMObjectPath r3 = CIMObjectPath ("myclass.X=\"Hello\",Z=true,Y=1234");
    // cout << r1.toString() << endl;
    // cout << r2.toString() << endl;
    assert(r1 == r2);
    assert(r1 != r3);
    }

     // Test case independence and order independence of parameters.
    {
	CIMObjectPath r1 = CIMObjectPath ("X.a=123,b=true");
	CIMObjectPath r2 = CIMObjectPath ("x.B=TRUE,A=123");
	assert(r1 == r2);
	assert(r1.makeHashCode() == r2.makeHashCode());

	CIMObjectPath r3 = CIMObjectPath ("x.B=TRUE,A=123,c=FALSE");
	assert(r1 != r3);
        String            keyValue;

        Array<CIMKeyBinding> kbArray;
	{
	    Boolean found = false;
	    kbArray = r3.getKeyBindings();
	    for (Uint32 i = 0; i < kbArray.size(); i++)
	    {
			cout << "keyName= " <<  kbArray[i].getName() << " Value= " 
				 << kbArray[i].getValue() << endl;
		if ( kbArray[i].getName() == CIMName ("B") )
		{
		    keyValue = kbArray[i].getValue();
		    if(keyValue == "TRUE")
			found = true;    
		}
	    }
	    if(!found)
	    {
			cerr << "Key Binding Test error " << endl; 
				exit(1);
	    }
		//ATTN: KS 12 May 2002 P3 DEFER - keybinding manipulation. too simplistic
		// This code demonstrates that it is not easy to manipulate and
		// test keybindings.  Needs better tool both in CIMObjectPath and
		// separate.
	}
    }


    // Test building from component parts of CIM Reference.
    {
	CIMObjectPath r1 ("atp:77", CIMNamespaceName ("root/cimv25"), 
            CIMName ("TennisPlayer"));
	CIMObjectPath r2 ("//atp:77/root/cimv25:TennisPlayer.");
	//cout << "r1 " << r1.toString() << endl;
	//cout << "r2 " << r2.toString() << endl;

	assert(r1 == r2);
	assert(r1.toString() == r2.toString());

    }


    {
	String hostName = "atp:77";
	String nameSpace = "root/cimv2";
	String className = "tennisplayer";

	CIMObjectPath r1;
	r1.setHost(hostName);
	r1.setNameSpace(nameSpace);
	r1.setClassName(className);
	assert(r1.getClassName().equal(CIMName ("TENNISPLAYER")));
	assert(!r1.getClassName().equal(CIMName ("blob")));


	String newHostName = r1.getHost();
	//cout << "HostName = " << newHostName << endl;

	CIMObjectPath r2 (hostName, nameSpace, className);
	assert(r1 == r2);
     }

}

void test02()
{
    // test cases with commas in the key value string

    CIMObjectPath testr1 = CIMObjectPath 
        ("MyClass.z=true,y=1234,x=\"Hello,World\"");

    CIMObjectPath testr2 = CIMObjectPath 
        ("MyClass.z=true,y=1234,x=\"Hello World,\"");

    CIMObjectPath testr3 = CIMObjectPath 
        ("MyClass.z=true,y=1234,x=\"Hello,,World\"");

    CIMObjectPath testr4 = CIMObjectPath 
        ("//atp:77/root/cimv25:test.last=\"Rafter,Smith.Jones long_name:any*char=any123%#@!<>?+^\",first=\"Patrick\"");

    // test error cases

    Boolean errorDetected = false;
    try
    {
       CIMObjectPath testerr1 = CIMObjectPath 
           ("myclass.X=\"Hello World\"Z=trueY=1234");
    }
    catch (Exception& e)
    {
        errorDetected = true;
    }
    assert(errorDetected);

    errorDetected = false;
    try
    {
       CIMObjectPath testerr2 = CIMObjectPath ("myclass.XYZ");
    }
    catch (Exception& e)
    {
        errorDetected = true;
    }
    assert(errorDetected);

    errorDetected = false;
    try
    {
       CIMObjectPath testerr3 = CIMObjectPath 
           ("MyClass.z=true,y=1234abc,x=\"Hello World\"");
    }
    catch (Exception& e)
    {
        errorDetected = true;
    }
    assert(errorDetected);

    errorDetected = false;
    try
    {
       CIMObjectPath testerr4 = CIMObjectPath 
           ("MyClass.z=nottrue,y=1234,x=\"Hello World\"");
    }
    catch (Exception& e)
    {
        errorDetected = true;
    }
    assert(errorDetected);
}

// Test CIMKeyBinding constructor (CIMValue variety) and equal(CIMValue) method
void test03()
{
    CIMKeyBinding kb0("test0", Real32(3.14159));
    assert(kb0.equal(Real32(3.14159)));
    assert(!kb0.equal(Real32(3.141593)));

    CIMKeyBinding kb1("test1", String("3.14159"), CIMKeyBinding::NUMERIC);
    assert(kb1.equal(Real32(3.14159)));
    assert(!kb1.equal(String("3.14159")));

    CIMKeyBinding kb2("test2", Uint32(1000));
    assert(kb2.equal(Uint32(1000)));
    assert(!kb2.equal(Uint32(1001)));
    assert(kb2.getValue() == "1000");

    CIMKeyBinding kb3("test3", Char16('X'));
    assert(kb3.equal(Char16('X')));
    assert(!kb3.equal(Char16('Y')));
    assert(kb3.getValue() == "X");

    CIMKeyBinding kb4("test4", CIMDateTime("19991224120000.000000+360"));
    assert(kb4.equal(CIMDateTime("19991224120000.000000+360")));
    assert(!kb4.equal(CIMDateTime("19991225120000.000000+360")));
    assert(kb4.getValue() == "19991224120000.000000+360");
    kb4.setValue("0");
    assert(!kb4.equal(CIMDateTime("19991224120000.000000+360")));

    CIMKeyBinding kb5("test5", String("StringTest"));
    assert(kb5.equal(String("StringTest")));
    assert(!kb5.equal(String("StringTest1")));
    assert(kb5.getValue() == "StringTest");

    CIMKeyBinding kb6("test6", Boolean(true));
    assert(kb6.equal(Boolean(true)));
    assert(!kb6.equal(Boolean(false)));
    assert(kb6.getValue() == "TRUE");
    kb6.setValue("true1");
    assert(!kb6.equal(Boolean(true)));

    CIMKeyBinding kb7("test7", CIMObjectPath("//atp:77/root/cimv25:TennisPlayer.last=\"Rafter\",first=\"Patrick\""));
    String path = "//atp:77/root/cimv25:TennisPlayer.last=\"Rafter\",first=\"Patrick\"";
    assert(kb7.equal(CIMObjectPath(path)));
    path = "//atp:77/root/cimv25:TennisPlayer.FIRST=\"Patrick\",LAST=\"Rafter\"";
    assert(kb7.equal(CIMObjectPath(path)));
    path = "//atp:77/root/cimv25:TennisPlayer.last=\"Rafter\"";
    assert(!kb7.equal(CIMObjectPath(path)));

    Boolean exceptionFlag = false;
    try
    {
        CIMKeyBinding kb8("test8", Array<Uint32>());
    }
    catch (TypeMismatchException&)
    {
        exceptionFlag = true;
    }
    assert(exceptionFlag);

    CIMKeyBinding kb9("test9", String("1000"), CIMKeyBinding::STRING);
    assert(!kb9.equal(Uint32(1000)));

    CIMKeyBinding kb10("test10", String("100"), CIMKeyBinding::NUMERIC);
    assert(kb10.equal(Uint64(100)));
    assert(kb10.equal(Uint32(100)));
    assert(kb10.equal(Uint16(100)));
    assert(kb10.equal(Uint8(100)));
    assert(kb10.equal(Sint64(100)));
    assert(kb10.equal(Sint32(100)));
    assert(kb10.equal(Sint16(100)));
    assert(kb10.equal(Sint8(100)));
    assert(!kb10.equal(String("100")));

    CIMKeyBinding kb11("test11", String("+100"), CIMKeyBinding::NUMERIC);
    assert(!kb11.equal(Uint64(100)));  // Unsigned ints may not start with "+"
    assert(!kb11.equal(Uint32(100)));
    assert(!kb11.equal(Uint16(100)));
    assert(!kb11.equal(Uint8(100)));
    assert(kb11.equal(Sint64(100)));
    assert(kb11.equal(Sint32(100)));
    assert(kb11.equal(Sint16(100)));
    assert(kb11.equal(Sint8(100)));
    assert(!kb11.equal(String("100")));
}

int main(int argc, char** argv)
{
    verbose = getenv("PEGASUS_TEST_VERBOSE");

    try
    {
	test01();
	test02();
	test03();

        cout << argv[0] << " +++++ passed all tests" << endl;
    }
    catch (Exception& e)
    {
	cerr << argv[0] << " Exception " << e.getMessage() << endl;
	exit(1);
    }
    return 0;
}
