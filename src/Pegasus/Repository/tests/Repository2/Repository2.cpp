//%/////////////////////////////////////////////////////////////////////////////
//
// Copyright (c1) 2000 The Open Group, BMC Software, Tivoli Systems, IBM
//
// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation
// the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following conditions:
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
// THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.
//
//==============================================================================
//
// Author: Mike Brasher (mbrasher@bmc.com)
//
// Modified By:
//
//%/////////////////////////////////////////////////////////////////////////////

#include <cassert>
#include <Pegasus/Repository/CIMRepository.h>

using namespace Pegasus;
using namespace std;

void TestNameSpaces()
{
    CIMRepository r("./repository");

    r.createNameSpace("namespace0");
    r.createNameSpace("namespace1");
    r.createNameSpace("namespace2");

    Array<String> nameSpaces;
    nameSpaces = r.enumerateNameSpaces();
    BubbleSort(nameSpaces);

    assert(nameSpaces.size() == 3);
    assert(nameSpaces[0] == "namespace0");
    assert(nameSpaces[1] == "namespace1");
    assert(nameSpaces[2] == "namespace2");

    r.deleteNameSpace("namespace0");
    r.deleteNameSpace("namespace1");

    nameSpaces = r.enumerateNameSpaces();
    assert(nameSpaces.size() == 1);
    assert(nameSpaces[0] == "namespace2");

    r.deleteNameSpace("namespace2");
    nameSpaces = r.enumerateNameSpaces();
    assert(nameSpaces.size() == 0);
}

void TestCreateClass()
{
    // -- Create repository and "xyz" namespace:

    CIMRepository r("./repository");
    const String NAMESPACE = "xyz";

    try
    {
	r.createNameSpace(NAMESPACE);
    }
    catch (AlreadyExists&)
    {
	// Ignore this!
    }

    // -- Declare the key qualifier:

    r.setQualifier(NAMESPACE, CIMQualifierDecl("key",true,CIMScope::PROPERTY));

    // -- Construct new class:

    CIMClass c1("MyClass");
    c1.addProperty(
	CIMProperty("key", Uint32(0))
	    .addQualifier(CIMQualifier("key", true)));

    c1.addProperty(CIMProperty("ratio", Real32(1.5)));
    c1.addProperty(CIMProperty("message", "Hello World"));

    // -- Create the class (get it back and compare):

    r.createClass(NAMESPACE, c1);
    CIMConstClass cc1;
    cc1 = r.getClass(NAMESPACE, "MyClass");
    assert(c1.identical(cc1));
    assert(cc1.identical(c1));

    // -- Now create a sub class (get it back and compare):

    CIMClass c2("YourClass", "MyClass");
    c2.addProperty(CIMProperty("junk", Real32(66.66)));
    r.createClass(NAMESPACE, c2);
    CIMConstClass cc2;
    cc2 = r.getClass(NAMESPACE, "YourClass");
    assert(c2.identical(cc2));
    assert(cc2.identical(c2));
    // cc2.print();

    // -- Modify "YourClass" (add new property)

    c2.addProperty(CIMProperty("newProperty", Uint32(888)));
    r.modifyClass(NAMESPACE, c2);
    cc2 = r.getClass(NAMESPACE, "YourClass");
    assert(c2.identical(cc2));
    assert(cc2.identical(c2));
    // cc2.print();

    // -- Enumerate the class names: expect "MyClass", "YourClass"

    Array<String> classNames = r.enumerateClassNames(NAMESPACE, String(), true);
    BubbleSort(classNames);
    assert(classNames.size() == 2);
    assert(classNames[0] == "MyClass");
    assert(classNames[1] == "YourClass");

    // -- Create an instance of each class:

    CIMInstance inst0("MyClass");
    inst0.addProperty(CIMProperty("key", Uint32(111)));
    r.createInstance(NAMESPACE, inst0);

    CIMInstance inst1("YourClass");
    inst1.addProperty(CIMProperty("key", Uint32(222)));
    r.createInstance(NAMESPACE, inst1);

    // -- Enumerate instances names:

    Array<CIMReference> instanceNames = 
	r.enumerateInstanceNames(NAMESPACE, "MyClass");

    assert(instanceNames.size() == 2);

    assert(
	instanceNames[0].toString() == "MyClass.key=111" ||
	instanceNames[0].toString() == "YourClass.key=222");

    assert(
	instanceNames[1].toString() == "MyClass.key=111" ||
	instanceNames[1].toString() == "YourClass.key=222");

    // -- Enumerate instances:

    Array<CIMInstance> instances = r.enumerateInstances(NAMESPACE, "MyClass");

    assert(instances.size() == 2);

    assert(
	instances[0].identical(inst0) ||
	instances[0].identical(inst1));

    assert(
	instances[1].identical(inst0) ||
	instances[1].identical(inst1));

    // -- Modify one of the instances:

    CIMInstance modifiedInst0("MyClass");
    modifiedInst0.addProperty(CIMProperty("key", Uint32(111)));
    modifiedInst0.addProperty(CIMProperty("message", "Goodbye World"));
    r.modifyInstance(NAMESPACE, modifiedInst0);

    // -- Get instance back and see that it is the same as modified one:

    CIMInstance tmpInstance = r.getInstance(NAMESPACE, "MyClass.key=111");
    assert(tmpInstance.identical(modifiedInst0));
    // tmpInstance.print();

    // -- Delete the instances:

    r.deleteInstance(NAMESPACE, "MyClass.key=111");
    r.deleteInstance(NAMESPACE, "YourClass.key=222");

    // -- Delete the qualifier:

    r.deleteQualifier(NAMESPACE, "key");

    // -- Clean up classes:

    r.deleteClass(NAMESPACE, "YourClass");
    r.deleteClass(NAMESPACE, "MyClass");

    r.deleteNameSpace(NAMESPACE);
}

void TestQualifiers()
{
    // -- Create repository and "xyz" namespace:

    CIMRepository r("./repository");
    const String NAMESPACE = "xyz";

    try
    {
	r.createNameSpace(NAMESPACE);
    }
    catch (AlreadyExists&)
    {
	// Ignore this!
    }

    // -- Construct a qualifier declaration:

    CIMQualifierDecl q("abstract", true, CIMScope::CLASS);
    r.setQualifier(NAMESPACE, q);

    CIMQualifierDecl qq = r.getQualifier(NAMESPACE, "abstract");

    assert(qq.identical(q));
    assert(q.identical(qq));

    // -- Delete the qualifier:

    r.deleteQualifier(NAMESPACE, "abstract");

    // -- Delete the namespace:

    r.deleteNameSpace(NAMESPACE);
}

int main()
{
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
