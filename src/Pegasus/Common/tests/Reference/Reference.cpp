//%2004////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001, 2002 BMC Software; Hewlett-Packard Development
// Company, L.P.; IBM Corp.; The Open Group; Tivoli Systems.
// Copyright (c) 2003 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation, The Open Group.
// Copyright (c) 2004 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation; VERITAS Software Corporation; The Open Group.
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
//               Karl Schopmeyer - Add reference object tests.
//               Roger Kumpf, Hewlett-Packard Company (roger_kumpf@hp.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#include <cassert>
#include <cstdlib>
#include <iostream>
#include <Pegasus/Common/CIMObjectPath.h>
#include <Pegasus/Common/CIMName.h>
#include <Pegasus/Common/XmlWriter.h>
#include <Pegasus/Common/MofWriter.h>
#include <Pegasus/Common/InternalException.h>

PEGASUS_USING_PEGASUS;
PEGASUS_USING_STD;

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
			if (verbose)
            {
                cout << "keyName= " <<  kbArray[i].getName() << " Value= " 
    				 << kbArray[i].getValue() << endl;
            }
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

     // Test cases for the Hostname.  CIMObjectPaths allows the
     // host to include the domain. Eg. xyz.company.com

     // First, try a good hostname
     CIMObjectPath h0("//usoPen-9.ustA-1-a.org:77/root/cimv25:"
                      "TennisPlayer.first=\"Chris\",last=\"Evert\"");
     CIMObjectPath h1("//usoPen-9:77/root/cimv25:"
                      "TennisPlayer.first=\"Chris\",last=\"Evert\"");
     CIMObjectPath h2("//usoPen-9/root/cimv25:"
                      "TennisPlayer.first=\"Chris\",last=\"Evert\"");
     CIMObjectPath h3("//usoPen-9.ustA-1-a.org:0/root/cimv25:"
                      "TennisPlayer.first=\"Chris\",last=\"Evert\"");
     CIMObjectPath h4("//usoPen-9.ustA-1-a.org:9876543210/root/cimv25:"
                      "TennisPlayer.first=\"Chris\",last=\"Evert\"");
     CIMObjectPath h6("//usopen-9.usta-1-a.1org:77/root/cimv25:"
                      "TennisPlayer.first=\"Chris\",last=\"Evert\"");
     CIMObjectPath h7("//192.168.1.com:77/root/cimv25:"
                      "TennisPlayer.first=\"Chris\",last=\"Evert\"");
     CIMObjectPath h8("//192.168.0.org/root/cimv25:"
                      "TennisPlayer.first=\"Chris\",last=\"Evert\"");
     CIMObjectPath h9("//192.168.1.80.com:77/root/cimv25:"
                      "TennisPlayer.first=\"Chris\",last=\"Evert\"");
     CIMObjectPath h10("//192.168.0.80.org/root/cimv25:"
                      "TennisPlayer.first=\"Chris\",last=\"Evert\"");
     CIMObjectPath h11("//192.168.1.80.255.com:77/root/cimv25:"
                      "TennisPlayer.first=\"Chris\",last=\"Evert\"");
     CIMObjectPath h12("//192.168.0.80.254.org/root/cimv25:"
                      "TennisPlayer.first=\"Chris\",last=\"Evert\"");
     CIMObjectPath h13("//192.168.257.80.com:77/root/cimv25:"
                      "TennisPlayer.first=\"Chris\",last=\"Evert\"");
     CIMObjectPath h14("//192.256.0.80.org/root/cimv25:"
                      "TennisPlayer.first=\"Chris\",last=\"Evert\"");
     CIMObjectPath h15("//localhost/root/cimv25:"
                      "TennisPlayer.first=\"Chris\",last=\"Evert\"");
     CIMObjectPath h16("//ou812/root/cimv25:"
                      "TennisPlayer.first=\"Chris\",last=\"Evert\"");
     CIMObjectPath h17("//u812/root/cimv25:"
                      "TennisPlayer.first=\"Chris\",last=\"Evert\"");

     // try IPAddress as hostname which should be good
     CIMObjectPath h_ip0("//192.168.1.80:77/root/cimv25:"
                         "TennisPlayer.first=\"Chris\",last=\"Evert\"");
     CIMObjectPath h_ip1("//192.168.0.255/root/cimv25:"
                         "TennisPlayer.first=\"Chris\",last=\"Evert\"");

     // Now, try some bad object paths.
     Boolean errorDetected = false;
     try
     {
        // Octet out of range
        CIMObjectPath h5("//192.168.256.80:77/root/cimv25:"
                         "TennisPlayer.first=\"Chris\",last=\"Evert\"");
     } catch (const Exception&)
     {
        errorDetected = true;
     }
     assert(errorDetected);

     errorDetected = false;
     try
     {
        // Missing port
        CIMObjectPath h5("//192.168.1.80:/root/cimv25:"
                         "TennisPlayer.first=\"Chris\",last=\"Evert\"");
     } catch (const Exception&)
     {
        errorDetected = true;
     }
     assert(errorDetected);

     errorDetected = false;
     try
     {
        // Too many octets
        CIMObjectPath h5("//192.168.1.80.12/root/cimv25:"
                         "TennisPlayer.first=\"Chris\",last=\"Evert\"");
     } catch (const Exception&)
     {
        errorDetected = true;
     }
     assert(errorDetected);

     errorDetected = false;
     try
     {
        // Too few octets
        CIMObjectPath h5("//192.168.80:77/root/cimv25:"
                         "TennisPlayer.first=\"Chris\",last=\"Evert\"");
     } catch (const Exception&)
     {
        errorDetected = true;
     }
     assert(errorDetected);

     errorDetected = false;
     try
     {
        // Missing port
        CIMObjectPath h1("//usopen-9.usta-1-a.org:/root/cimv25:"
                         "TennisPlayer.first=\"Chris\",last=\"Evert\"");
     } catch (Exception&)
     {
        errorDetected = true;
     }
     assert(errorDetected);

     errorDetected = false;
     try
     {
        // Hostname (IP) without trailing '/' (with port)
        CIMObjectPath h5("//192.168.256.80:77");
     } catch (const Exception&)
     {
        errorDetected = true;
     }
     assert(errorDetected);

     errorDetected = false;
     try
     {
        // Hostname (IP) without trailing '/' (without port)
        CIMObjectPath h5("//192.168.256.80");
     } catch (const Exception&)
     {
        errorDetected = true;
     }
     assert(errorDetected);

     errorDetected = false;
     try
     {
        // Hostname without trailing '/' (with port)
        CIMObjectPath h5("//usopen-9.usta-1-a.org:77");
     } catch (const Exception&)
     {
        errorDetected = true;
     }
     assert(errorDetected);

     errorDetected = false;
     try
     {
        // Hostname without trailing '/' (without port)
        CIMObjectPath h5("//usopen-9.usta-1-a.org");
     } catch (const Exception&)
     {
        errorDetected = true;
     }
     assert(errorDetected);

     errorDetected = false;
     try
     {
        // Invalid first character
        CIMObjectPath h1("//+usopen-9.usta-1-a.1org:77/root/cimv25:"
                         "TennisPlayer.first=\"Chris\",last=\"Evert\"");
     } catch (Exception&)
     {
        errorDetected = true;
     }
     assert(errorDetected);

     errorDetected = false;
     try
     {
        // Non-alphanum char (?)
        CIMObjectPath h1("//usopen-9.usta?-1-a.org:77/root/cimv25:"
                         "TennisPlayer.first=\"Chris\",last=\"Evert\"");
     } catch (Exception&)
     {
        errorDetected = true;
     }
     assert(errorDetected);

     errorDetected = false;
     try
     {
        // Leading dot
        CIMObjectPath h2("//.usopen-9.usta-1-a.org:77/root/cimv25:"
                         "TennisPlayer.first=\"Chris\",last=\"Evert\"");
     } catch (Exception&)
     {
        errorDetected = true;
     }
     assert(errorDetected);

     errorDetected = false;
     try
     {
        // Dot in the wrong spot (before a -)
        CIMObjectPath h3("//usopen.-9.usta-1-a.org:77/root/cimv25:"
                         "TennisPlayer.first=\"Chris\",last=\"Evert\"");
     } catch (Exception&)
     {
        errorDetected = true;
     }
     assert(errorDetected);

     errorDetected = false;
     try
     {
        // Two dots in a row
        CIMObjectPath h4("//usopen-9.usta-1-a..org:77/root/cimv25:"
                         "TennisPlayer.first=\"Chris\",last=\"Evert\"");
     } catch (Exception&)
     {
        errorDetected = true;
     }
     assert(errorDetected);

     errorDetected = false;
     try
     {
        // Trailing dot
        CIMObjectPath h5("//usopen-9.usta-1-a.org.:77/root/cimv25:"
                         "TennisPlayer.first=\"Chris\",last=\"Evert\"");
     } catch (Exception&)
     {
        errorDetected = true;
     }
     assert(errorDetected);
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

    // test cases with colon inside keybinding string value

    CIMObjectPath testc1 = CIMObjectPath 
        ("MyClass.z=true,y=1234,x=\"Hello:World\"");

    Boolean colonException = false;
    try
    {
        CIMObjectPath testc2 = CIMObjectPath 
            ("MyNamespace.ns:MyClass.z=true,y=1234,x=\"Hello:World\"");
    }
    catch (Exception&)
    {
        colonException = true;
    }
    PEGASUS_ASSERT(colonException);

    // test error cases

    Boolean errorDetected = false;
    try
    {
       CIMObjectPath testerr1 = CIMObjectPath 
           ("myclass.X=\"Hello World\"Z=trueY=1234");
    }
    catch (Exception&)
    {
        errorDetected = true;
    }
    assert(errorDetected);

    errorDetected = false;
    try
    {
       CIMObjectPath testerr2 = CIMObjectPath ("myclass.XYZ");
    }
    catch (Exception&)
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
    catch (Exception&)
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
    catch (Exception&)
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

//
//  Test identical() function with keys that are references
//
void test04()
{
    //
    // Create classes A and B referenced classes, C - Association
    //
    CIMClass classA (CIMName ("A"), CIMName ());
    CIMProperty propertyX ("x", String ());
    propertyX.addQualifier (CIMQualifier (CIMName ("Key"), true));
    CIMProperty propertyY ("y", String ());
    propertyY.addQualifier (CIMQualifier (CIMName ("Key"), true));
    CIMProperty propertyZ ("z", String ());
    propertyZ.addQualifier (CIMQualifier (CIMName ("Key"), true));
    classA.addProperty (propertyX);
    classA.addProperty (propertyY);
    classA.addProperty (propertyZ);

    CIMClass classB ("B");
    CIMProperty propertyQ ("q", String ());
    propertyQ.addQualifier (CIMQualifier (CIMName ("Key"), true));
    CIMProperty propertyR ("r", String ());
    propertyR.addQualifier (CIMQualifier (CIMName ("Key"), true));
    CIMProperty propertyS ("s", String ());
    propertyS.addQualifier (CIMQualifier (CIMName ("Key"), true));
    classB.addProperty (propertyQ);
    classB.addProperty (propertyR);
    classB.addProperty (propertyS);

    CIMClass classC ("C");
    CIMProperty propertyA ("a", CIMValue ());
    propertyA.addQualifier (CIMQualifier (CIMName ("Key"), true));
    CIMProperty propertyB ("b", CIMValue ());
    propertyB.addQualifier (CIMQualifier (CIMName ("Key"), true));
    classC.addProperty (propertyA);
    classC.addProperty (propertyB);

    //
    //  Create instances of each classa
    //
    CIMInstance instanceA (CIMName ("A"));
    instanceA.addProperty (CIMProperty (CIMName ("x"), String ("rose")));
    instanceA.addProperty (CIMProperty (CIMName ("y"), String ("lavender")));
    instanceA.addProperty (CIMProperty (CIMName ("z"), String ("rosemary")));
    CIMObjectPath aPath = instanceA.buildPath (classA);
    CIMObjectPath aPath2 ("A.y=\"lavender\",x=\"rose\",z=\"rosemary\"");
    assert (aPath.identical (aPath2));

    CIMInstance instanceB (CIMName ("B"));
    instanceB.addProperty (CIMProperty (CIMName ("q"), String ("pelargonium")));
    instanceB.addProperty (CIMProperty (CIMName ("r"), String ("thyme")));
    instanceB.addProperty (CIMProperty (CIMName ("s"), String ("sage")));
    
    // Test to assure that the buildpath function works.
    CIMObjectPath bPath = instanceB.buildPath (classB);
    CIMObjectPath bPath2 ("B.s=\"sage\",q=\"pelargonium\",r=\"thyme\"");
    assert (bPath.identical (bPath2));

    // Build instance of C and build path from buildPath function.
    CIMInstance instanceC (CIMName ("C"));
    instanceC.addProperty (CIMProperty (CIMName ("a"), aPath, 0, 
        CIMName ("A")));
    instanceC.addProperty (CIMProperty (CIMName ("b"), bPath, 0, 
        CIMName ("B")));
    CIMObjectPath cPath = instanceC.buildPath (classC);

    // Build CIMObjectPath from keybindings.
    Array <CIMKeyBinding> keyBindings;
    CIMKeyBinding aBinding ("a", "A.y=\"lavender\",x=\"rose\",z=\"rosemary\"", 
        CIMKeyBinding::REFERENCE);
    CIMKeyBinding bBinding ("b", "B.s=\"sage\",q=\"pelargonium\",r=\"thyme\"",
        CIMKeyBinding::REFERENCE);
    keyBindings.append (aBinding);
    keyBindings.append (bBinding);

    CIMObjectPath cPath2 ("", CIMNamespaceName (),
        cPath.getClassName (), keyBindings);

    // Assert that the CIMObjectPaths for C from build path and direct from keybindings are equal.
    assert (cPath.identical (cPath2));

    // ATTN: KS 25 Feb 2003 P3 - Think we can extend these tests since this is creation of classes and
    // instnaces for associations and referenced classes.
}

// Test handling of escape characters
void test05()
{
    // Test '\' and '"' characters in a key value
    // This represents MyClass.key1="\\\"\"\\",key2="\"\"\"\"\\\\\\\\"
    String s1 = "MyClass.key1=\"\\\\\\\"\\\"\\\\\","
                "key2=\"\\\"\\\"\\\"\\\"\\\\\\\\\\\\\\\\\"";
    CIMObjectPath r1 = s1;
    assert(r1.toString() == s1);

    // Catch invalid escape sequences in a key value
    Boolean errorDetected;

    // Invalid trailing backslash
    errorDetected = false;
    try
    {
        CIMObjectPath r1("MyClass.key1=\"\\\"");
    } catch (const MalformedObjectNameException&)
    {
       errorDetected = true;
    }
    assert(errorDetected);

    // Invalid "\n" sequence
    errorDetected = false;
    try
    {
        CIMObjectPath r1("MyClass.key1=\"\\n\"");
    } catch (const MalformedObjectNameException&)
    {
       errorDetected = true;
    }
    assert(errorDetected);

    // Invalid hex sequence
    errorDetected = false;
    try
    {
        CIMObjectPath r1("MyClass.key1=\"\\x000A\"");
    } catch (const MalformedObjectNameException&)
    {
       errorDetected = true;
    }
    assert(errorDetected);
}

// Test CIMKeyBinding operator==
void test06()
{
    CIMKeyBinding kb1;
    CIMKeyBinding kb2;

    // Key bindings of different types are not equal
    kb1 = CIMKeyBinding("a", "true", CIMKeyBinding::BOOLEAN);
    kb2 = CIMKeyBinding("a", "true", CIMKeyBinding::STRING);
    assert(!(kb1 == kb2));

    // Key bindings with different names are not equal
    kb1 = CIMKeyBinding("a", "true", CIMKeyBinding::BOOLEAN);
    kb2 = CIMKeyBinding("b", "true", CIMKeyBinding::BOOLEAN);
    assert(!(kb1 == kb2));

    // Key bindings names are not case sensitive
    kb1 = CIMKeyBinding("a", "true", CIMKeyBinding::BOOLEAN);
    kb2 = CIMKeyBinding("A", "true", CIMKeyBinding::BOOLEAN);
    assert(kb1 == kb2);

    // Boolean key bindings are not case sensitive
    kb1 = CIMKeyBinding("a", "true", CIMKeyBinding::BOOLEAN);
    kb2 = CIMKeyBinding("a", "TrUe", CIMKeyBinding::BOOLEAN);
    assert(kb1 == kb2);

    // Boolean key bindings are not equal if they differ other than in case
    kb1 = CIMKeyBinding("a", "true", CIMKeyBinding::BOOLEAN);
    kb2 = CIMKeyBinding("a", "truee", CIMKeyBinding::BOOLEAN);
    assert(!(kb1 == kb2));

    // Numeric key binding comparisons are done based on integer values
    kb1 = CIMKeyBinding("a", "14", CIMKeyBinding::NUMERIC);
    kb2 = CIMKeyBinding("a", "14", CIMKeyBinding::NUMERIC);
    assert(kb1 == kb2);

    kb1 = CIMKeyBinding("a", "14", CIMKeyBinding::NUMERIC);
    kb2 = CIMKeyBinding("a", "140", CIMKeyBinding::NUMERIC);
    assert(!(kb1 == kb2));

    kb1 = CIMKeyBinding("a", "+14", CIMKeyBinding::NUMERIC);
    kb2 = CIMKeyBinding("a", "14", CIMKeyBinding::NUMERIC);
    assert(kb1 == kb2);

    kb1 = CIMKeyBinding("a", "0", CIMKeyBinding::NUMERIC);
    kb2 = CIMKeyBinding("a", "-0", CIMKeyBinding::NUMERIC);
    assert(kb1 == kb2);

    // Hexadecimal format is also understood
    kb1 = CIMKeyBinding("a", "14", CIMKeyBinding::NUMERIC);
    kb2 = CIMKeyBinding("a", "0x0E", CIMKeyBinding::NUMERIC);
    assert(kb1 == kb2);

    kb1 = CIMKeyBinding("a", "0x0E", CIMKeyBinding::NUMERIC);
    kb2 = CIMKeyBinding("a", "0x0e", CIMKeyBinding::NUMERIC);
    assert(kb1 == kb2);

    // Numeric key bindings are not equal if they differ other than in case
    kb1 = CIMKeyBinding("a", "true", CIMKeyBinding::NUMERIC);
    kb2 = CIMKeyBinding("a", "truee", CIMKeyBinding::NUMERIC);
    assert(!(kb1 == kb2));

    // String comparison is used if numeric key binding is not a valid integer
    kb1 = CIMKeyBinding("a", "14a", CIMKeyBinding::NUMERIC);
    kb2 = CIMKeyBinding("a", "14a", CIMKeyBinding::NUMERIC);
    assert(kb1 == kb2);

    kb1 = CIMKeyBinding("a", "14", CIMKeyBinding::NUMERIC);
    kb2 = CIMKeyBinding("a", "14a", CIMKeyBinding::NUMERIC);
    assert(!(kb1 == kb2));

    // String key bindings are compared using a simple string comparison
    kb1 = CIMKeyBinding("a", "My String", CIMKeyBinding::STRING);
    kb2 = CIMKeyBinding("a", "My String", CIMKeyBinding::STRING);
    assert(kb1 == kb2);

    kb1 = CIMKeyBinding("a", "My String", CIMKeyBinding::STRING);
    kb2 = CIMKeyBinding("a", "my string", CIMKeyBinding::STRING);
    assert(!(kb1 == kb2));

    // Reference key bindings are compared as CIMObjectPath objects
    kb1 = CIMKeyBinding("a", "aClass.key1=true,key2=256",
                        CIMKeyBinding::REFERENCE);
    kb2 = CIMKeyBinding("a", "AClass.Key2=0x100,Key1=TRUE",
                        CIMKeyBinding::REFERENCE);
    assert(kb1 == kb2);

    kb1 = CIMKeyBinding("a", "aClass.key1=\"true\"",
                        CIMKeyBinding::REFERENCE);
    kb2 = CIMKeyBinding("a", "AClass.Key1=\"TRUE\"",
                        CIMKeyBinding::REFERENCE);
    assert(!(kb1 == kb2));

    kb1 = CIMKeyBinding("a", "a.ref=\"aClass.key1=true,key2=256\"",
                        CIMKeyBinding::REFERENCE);
    kb2 = CIMKeyBinding("a", "a.ref=\"AClass.Key2=0x100,Key1=TRUE\"",
                        CIMKeyBinding::REFERENCE);
    assert(kb1 == kb2);

    kb1 = CIMKeyBinding("a", "//myHost/ns1/ns2:aClass.key1=1",
                        CIMKeyBinding::REFERENCE);
    kb2 = CIMKeyBinding("a", "//MyHost/NS1/NS2:AClass.key1=0x1",
                        CIMKeyBinding::REFERENCE);
    assert(kb1 == kb2);

    kb1 = CIMKeyBinding("a", "//myHost/ns1/ns2:aClass.key1=1",
                        CIMKeyBinding::REFERENCE);
    kb2 = CIMKeyBinding("a", "ns1/ns2:aClass.key1=1",
                        CIMKeyBinding::REFERENCE);
    assert(!(kb1 == kb2));

    kb1 = CIMKeyBinding("a", "ns1/ns2:aClass.key1=1",
                        CIMKeyBinding::REFERENCE);
    kb2 = CIMKeyBinding("a", "aClass.key1=1",
                        CIMKeyBinding::REFERENCE);
    assert(!(kb1 == kb2));

    kb1 = CIMKeyBinding("a", "aClass.key1=1,key2=2",
                        CIMKeyBinding::REFERENCE);
    kb2 = CIMKeyBinding("a", "aClass.key1=1",
                        CIMKeyBinding::REFERENCE);
    assert(!(kb1 == kb2));

    // String comparison is used if numeric key binding is not a valid integer
    kb1 = CIMKeyBinding("a", "Bad Reference", CIMKeyBinding::REFERENCE);
    kb2 = CIMKeyBinding("a", "Bad Reference", CIMKeyBinding::REFERENCE);
    assert(kb1 == kb2);

    kb1 = CIMKeyBinding("a", "Bad Reference", CIMKeyBinding::REFERENCE);
    kb2 = CIMKeyBinding("a", "bad reference", CIMKeyBinding::REFERENCE);
    assert(!(kb1 == kb2));
}

// Test hash code generation
void test07()
{
    CIMObjectPath op1;
    CIMObjectPath op2;

    // Hostname case does not affect the hash code
    op1 = CIMObjectPath("//myhost/ns1/ns2:aClass.key1=1");
    op2 = CIMObjectPath("//MyHost/ns1/ns2:aClass.key1=1");
    assert(op1.makeHashCode() == op2.makeHashCode());

    // Namespace case does not affect the hash code
    op1 = CIMObjectPath("//myhost/ns1/ns2:aClass.key1=1");
    op2 = CIMObjectPath("//myhost/NS1/Ns2:aClass.key1=1");
    assert(op1.makeHashCode() == op2.makeHashCode());

    // Class name case does not affect the hash code
    op1 = CIMObjectPath("//myhost/ns1/ns2:aClass.key1=1");
    op2 = CIMObjectPath("//myhost/ns1/ns2:ACLASS.key1=1");
    assert(op1.makeHashCode() == op2.makeHashCode());

    //
    // Equivalent key bindings generate the same hash code
    //
    Array<CIMKeyBinding> kba1;
    Array<CIMKeyBinding> kba2;

    // Key name case does not affect the hash code
    kba1.clear();
    kba2.clear();
    kba1.append(CIMKeyBinding("key1", "1", CIMKeyBinding::NUMERIC));
    kba2.append(CIMKeyBinding("KeY1", "1", CIMKeyBinding::NUMERIC));
    op1.set("myhost", CIMNamespaceName("ns1/ns2"), CIMName("aClass"), kba1);
    op2.set("myhost", CIMNamespaceName("ns1/ns2"), CIMName("aClass"), kba2);
    assert(op1.makeHashCode() == op2.makeHashCode());

    // Key binding order does not affect the hash code
    kba1.clear();
    kba2.clear();
    kba1.append(CIMKeyBinding("key1", "1", CIMKeyBinding::NUMERIC));
    kba1.append(CIMKeyBinding("key2", "2", CIMKeyBinding::NUMERIC));
    kba2.append(CIMKeyBinding("key2", "2", CIMKeyBinding::NUMERIC));
    kba2.append(CIMKeyBinding("key1", "1", CIMKeyBinding::NUMERIC));
    op1.set("myhost", CIMNamespaceName("ns1/ns2"), CIMName("aClass"), kba1);
    op2.set("myhost", CIMNamespaceName("ns1/ns2"), CIMName("aClass"), kba2);
    assert(op1.makeHashCode() == op2.makeHashCode());

    // Boolean key value case does not affect the hash code
    kba1.clear();
    kba2.clear();
    kba1.append(CIMKeyBinding("key1", "true", CIMKeyBinding::BOOLEAN));
    kba2.append(CIMKeyBinding("KeY1", "True", CIMKeyBinding::BOOLEAN));
    op1.set("myhost", CIMNamespaceName("ns1/ns2"), CIMName("aClass"), kba1);
    op2.set("myhost", CIMNamespaceName("ns1/ns2"), CIMName("aClass"), kba2);
    assert(op1.makeHashCode() == op2.makeHashCode());

    // Equivalent numeric key value forms do not affect the hash code
    kba1.clear();
    kba2.clear();
    kba1.append(CIMKeyBinding("key1", "1023", CIMKeyBinding::NUMERIC));
    kba2.append(CIMKeyBinding("KeY1", "0x3FF", CIMKeyBinding::NUMERIC));
    op1.set("myhost", CIMNamespaceName("ns1/ns2"), CIMName("aClass"), kba1);
    op2.set("myhost", CIMNamespaceName("ns1/ns2"), CIMName("aClass"), kba2);
    assert(op1.makeHashCode() == op2.makeHashCode());

    kba1.clear();
    kba2.clear();
    kba1.append(CIMKeyBinding("key1", "0", CIMKeyBinding::NUMERIC));
    kba2.append(CIMKeyBinding("KeY1", "-0", CIMKeyBinding::NUMERIC));
    op1.set("myhost", CIMNamespaceName("ns1/ns2"), CIMName("aClass"), kba1);
    op2.set("myhost", CIMNamespaceName("ns1/ns2"), CIMName("aClass"), kba2);
    assert(op1.makeHashCode() == op2.makeHashCode());

    // Invalid numeric key value forms generate a consistent hash code
    kba1.clear();
    kba2.clear();
    kba1.append(CIMKeyBinding("key1", "abc", CIMKeyBinding::NUMERIC));
    kba2.append(CIMKeyBinding("KeY1", "abc", CIMKeyBinding::NUMERIC));
    op1.set("myhost", CIMNamespaceName("ns1/ns2"), CIMName("aClass"), kba1);
    op2.set("myhost", CIMNamespaceName("ns1/ns2"), CIMName("aClass"), kba2);
    assert(op1.makeHashCode() == op2.makeHashCode());

    // Equivalent reference key value forms do not affect the hash code
    kba1.clear();
    kba2.clear();
    kba1.append(CIMKeyBinding("key1", "aClass.key1=true,key2=256",
                CIMKeyBinding::REFERENCE));
    kba2.append(CIMKeyBinding("KeY1", "AClass.Key2=0x100,Key1=TRUE",
                CIMKeyBinding::REFERENCE));
    op1.set("myhost", CIMNamespaceName("ns1/ns2"), CIMName("aClass"), kba1);
    op2.set("myhost", CIMNamespaceName("ns1/ns2"), CIMName("aClass"), kba2);
    assert(op1.makeHashCode() == op2.makeHashCode());

    kba1.clear();
    kba2.clear();
    kba1.append(CIMKeyBinding("key2", "a.ref=\"aClass.key1=true,key2=256\"",
                CIMKeyBinding::REFERENCE));
    kba2.append(CIMKeyBinding("Key2", "A.ref=\"AClass.Key2=0x100,Key1=TRUE\"",
                CIMKeyBinding::REFERENCE));
    op1.set("myhost", CIMNamespaceName("ns1/ns2"), CIMName("aClass"), kba1);
    op2.set("myhost", CIMNamespaceName("ns1/ns2"), CIMName("aClass"), kba2);
    assert(op1.makeHashCode() == op2.makeHashCode());

    kba1.clear();
    kba2.clear();
    kba1.append(CIMKeyBinding("key", "//myHost/ns1/ns2:aClass.key1=1",
                CIMKeyBinding::REFERENCE));
    kba2.append(CIMKeyBinding("Key", "//MyHost/NS1/NS2:AClass.key1=0x1",
                CIMKeyBinding::REFERENCE));
    op1.set("myhost", CIMNamespaceName("ns1/ns2"), CIMName("aClass"), kba1);
    op2.set("myhost", CIMNamespaceName("ns1/ns2"), CIMName("aClass"), kba2);
    assert(op1.makeHashCode() == op2.makeHashCode());

    // Kitchen sink
    kba1.clear();
    kba2.clear();
    kba1.append(CIMKeyBinding("bck", "true", CIMKeyBinding::BOOLEAN));
    kba1.append(CIMKeyBinding("rk2", "a.ref=\"aClass.key1=true,key2=256\"",
                CIMKeyBinding::REFERENCE));
    kba1.append(CIMKeyBinding("neK1", "1023", CIMKeyBinding::NUMERIC));
    kba1.append(CIMKeyBinding("SEk", "abc", CIMKeyBinding::STRING));
    kba1.append(CIMKeyBinding("rk3", "//myHost/ns1/ns2:aClass.key1=1",
                CIMKeyBinding::REFERENCE));
    kba1.append(CIMKeyBinding("NEk2", "0", CIMKeyBinding::NUMERIC));
    kba1.append(CIMKeyBinding("NiK", "abc", CIMKeyBinding::NUMERIC));
    kba1.append(CIMKeyBinding("rK1", "aClass.key1=true,key2=256",
                CIMKeyBinding::REFERENCE));
    kba2.append(CIMKeyBinding("rk1", "AClass.Key2=0x100,Key1=TRUE",
                CIMKeyBinding::REFERENCE));
    kba2.append(CIMKeyBinding("nek2", "-0", CIMKeyBinding::NUMERIC));
    kba2.append(CIMKeyBinding("BcK", "True", CIMKeyBinding::BOOLEAN));
    kba2.append(CIMKeyBinding("rk2", "A.ref=\"AClass.Key2=0x100,Key1=TRUE\"",
                CIMKeyBinding::REFERENCE));
    kba2.append(CIMKeyBinding("Nek1", "0x3FF", CIMKeyBinding::NUMERIC));
    kba2.append(CIMKeyBinding("Nik", "abc", CIMKeyBinding::NUMERIC));
    kba2.append(CIMKeyBinding("Sek", "abc", CIMKeyBinding::STRING));
    kba2.append(CIMKeyBinding("rk3", "//MyHost/NS1/NS2:AClass.key1=0x1",
                CIMKeyBinding::REFERENCE));
    op1.set("myhost", CIMNamespaceName("ns1/ns2"), CIMName("aClass"), kba1);
    op2.set("MyHOST", CIMNamespaceName("Ns1/NS2"), CIMName("Aclass"), kba2);
    assert(op1.makeHashCode() == op2.makeHashCode());
}

int main(int argc, char** argv)
{
    verbose = getenv("PEGASUS_TEST_VERBOSE");

    try
    {
	test01();
	test02();
	test03();
	test04();
	test05();
	test06();
	test07();

        cout << argv[0] << " +++++ passed all tests" << endl;
    }
    catch (Exception& e)
    {
	cerr << argv[0] << " Exception " << e.getMessage() << endl;
	exit(1);
    }
    return 0;
}
