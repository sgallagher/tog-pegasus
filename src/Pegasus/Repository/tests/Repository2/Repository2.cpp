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
//
// Author: Mike Brasher (mbrasher@bmc.com)
//
// Modified By: Roger Kumpf (roger_kumpf@hp.com)
//              Carol Ann Krug Graves, Hewlett-Packard Company
//                  (carolann_graves@hp.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#include <Pegasus/Common/Config.h>
#include <cassert>
#include <Pegasus/Repository/CIMRepository.h>

PEGASUS_USING_PEGASUS;
PEGASUS_USING_STD;

String repositoryRoot;

void TestNameSpaces()
{
    CIMRepository r (repositoryRoot);

    r.createNameSpace("namespace0");
    r.createNameSpace("namespace1");
    r.createNameSpace("namespace2");

    Array<String> nameSpaces;
    nameSpaces = r.enumerateNameSpaces();
    BubbleSort(nameSpaces);

    assert(nameSpaces.size() == 4);
    assert(nameSpaces[0] == "namespace0");
    assert(nameSpaces[1] == "namespace1");
    assert(nameSpaces[2] == "namespace2");
    assert(nameSpaces[3] == "root");

    r.deleteNameSpace("namespace0");
    r.deleteNameSpace("namespace1");

    nameSpaces = r.enumerateNameSpaces();
    assert(nameSpaces.size() == 2);
    BubbleSort(nameSpaces);
    assert(nameSpaces[0] == "namespace2");
    assert(nameSpaces[1] == "root");

    r.deleteNameSpace("namespace2");
    nameSpaces = r.enumerateNameSpaces();
    assert(nameSpaces.size() == 1);
    assert(nameSpaces[0] == "root");
}

void TestCreateClass()
{
    // -- Create repository and "xyz" namespace:

    CIMRepository r (repositoryRoot);
    const String NS = "xyz";

    try
    {
	r.createNameSpace(NS);
    }
    catch (AlreadyExists&)
    {
	// Ignore this!
    }

    // -- Declare the key qualifier:

    r.setQualifier(NS, CIMQualifierDecl("key",true,CIMScope::PROPERTY));

    // -- Construct new class:

    CIMClass c1("MyClass");
    c1.addProperty(
	CIMProperty("key", Uint32(0))
	    .addQualifier(CIMQualifier("key", true)));

    c1.addProperty(CIMProperty("ratio", Real32(1.5)));
    c1.addProperty(CIMProperty("message", String("Hello World")));

    // -- Create the class (get it back and compare):

    r.createClass(NS, c1);
    CIMConstClass cc1;
    cc1 = r.getClass(NS, "MyClass");
    assert(c1.identical(cc1));
    assert(cc1.identical(c1));

    // -- Now create a sub class (get it back and compare):

    CIMClass c2("YourClass", "MyClass");
    c2.addProperty(CIMProperty("junk", Real32(66.66)));
    r.createClass(NS, c2);
    CIMConstClass cc2;
    cc2 = r.getClass(NS, "YourClass");
    assert(c2.identical(cc2));
    assert(cc2.identical(c2));
    // cc2.print();

    // -- Modify "YourClass" (add new property)

    c2.addProperty(CIMProperty("newProperty", Uint32(888)));
    r.modifyClass(NS, c2);
    cc2 = r.getClass(NS, "YourClass");
    assert(c2.identical(cc2));
    assert(cc2.identical(c2));
    // cc2.print();

    // -- Enumerate the class names: expect "MyClass", "YourClass"

    Array<String> classNames = r.enumerateClassNames(NS, String(), true);
    BubbleSort(classNames);
    assert(classNames.size() == 2);
    assert(classNames[0] == "MyClass");
    assert(classNames[1] == "YourClass");

    // -- Create an instance of each class:

    CIMInstance inst0("MyClass");
    inst0.addProperty(CIMProperty("key", Uint32(111)));
    r.createInstance(NS, inst0);

    CIMInstance inst1("YourClass");
    inst1.addProperty(CIMProperty("key", Uint32(222)));
    r.createInstance(NS, inst1);

    // -- Enumerate instances names:

    Array<CIMObjectPath> instanceNames = 
	r.enumerateInstanceNames(NS, "MyClass");

    assert(instanceNames.size() == 2);

    assert(
	instanceNames[0].toString() == "MyClass.key=111" ||
	instanceNames[0].toString() == "YourClass.key=222");

    assert(
	instanceNames[1].toString() == "MyClass.key=111" ||
	instanceNames[1].toString() == "YourClass.key=222");

    inst0.setPath (CIMObjectPath ("MyClass.key=111"));
    inst1.setPath (CIMObjectPath ("YourClass.key=222"));

    // -- Enumerate instances:

    Array<CIMInstance> namedInstances = r.enumerateInstances(NS, "MyClass");

    assert(namedInstances.size() == 2);

    assert(
	namedInstances[0].identical(inst0) ||
	namedInstances[0].identical(inst1));

    assert(
	namedInstances[1].identical(inst0) ||
	namedInstances[1].identical(inst1));

    // -- Modify one of the instances:

    CIMInstance modifiedInst0("MyClass");
    modifiedInst0.addProperty(CIMProperty("key", Uint32(111)));
    modifiedInst0.addProperty(CIMProperty("message", String("Goodbye World")));
    modifiedInst0.setPath (instanceNames[0]);
    r.modifyInstance(NS, modifiedInst0);
    // modifiedInst0.print();

    // -- Get instance back and see that it is the same as modified one:

    CIMInstance tmpInstance = r.getInstance(NS, "MyClass.key=111");
    tmpInstance.setPath (instanceNames[0]);
    // tmpInstance.print();
    assert(tmpInstance.identical(modifiedInst0));

    // -- Now modify the "message" property:

    CIMValue messageValue = r.getProperty(NS, "MyClass.key=111", "message");
    String message;
    messageValue.get(message);
    assert(message == "Goodbye World");

    r.setProperty(NS, "MyClass.key=111", "message", CIMValue(String("Hello World")));

    messageValue = r.getProperty( NS, "MyClass.key=111", "message");
    messageValue.get(message);
    assert(message == "Hello World");

    // -- Attempt to modify a key property:

    Boolean failed = false;

    try
    {
	r.setProperty(NS, "MyClass.key=111", "key", Uint32(999));
    }
    catch (CIMException& e)
    {
	assert(e.getCode() == CIM_ERR_FAILED);
	failed = true;
    }

    assert(failed);

    // -- Delete the instances:

    r.deleteInstance(NS, "MyClass.key=111");
    r.deleteInstance(NS, "YourClass.key=222");

    // -- Delete the qualifier:

    r.deleteQualifier(NS, "key");

    // -- Clean up classes:

    r.deleteClass(NS, "YourClass");
    r.deleteClass(NS, "MyClass");

    r.deleteNameSpace(NS);
}

void TestQualifiers()
{
    // -- Create repository and "xyz" namespace:

    CIMRepository r (repositoryRoot);

    const String NS = "xyz";

    try
    {
	r.createNameSpace(NS);
    }
    catch (AlreadyExists&)
    {
	// Ignore this!
    }

    // -- Construct a qualifier declaration:

    CIMQualifierDecl q("abstract", true, CIMScope::CLASS);
    r.setQualifier(NS, q);

    CIMQualifierDecl qq = r.getQualifier(NS, "abstract");

    assert(qq.identical(q));
    assert(q.identical(qq));

    // -- Delete the qualifier:

    r.deleteQualifier(NS, "abstract");

    // -- Delete the namespace:

    r.deleteNameSpace(NS);
}

int main()
{
    const char* tmpDir = getenv ("PEGASUS_TMP");
    if (tmpDir == NULL)
    {
        repositoryRoot = ".";
    }
    else
    {
        repositoryRoot = tmpDir;
    }
    repositoryRoot += "/repository";

    try 
    {
	TestNameSpaces();
	TestCreateClass();
	TestQualifiers();

    }
    catch (Exception& e)
    {
	cout << e.getMessage() << endl;
	exit(1);
    }

    cout << "+++++ passed all tests" << endl;

    return 0;
}
