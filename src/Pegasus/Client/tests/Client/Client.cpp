//BEGIN_LICENSE
//
// Copyright (c) 2000 The Open Group, BMC Software, Tivoli Systems, IBM
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
//END_LICENSE
//BEGIN_HISTORY
//
// Author:
//
// $Log: Client.cpp,v $
// Revision 1.3  2001/02/11 05:45:33  mike
// Added case insensitive logic for files in Repository
//
// Revision 1.2  2001/01/25 02:12:05  mike
// Added meta-qualifiers to LoadRepository program.
//
// Revision 1.1.1.1  2001/01/14 19:50:33  mike
// Pegasus import
//
//
//END_HISTORY

#include <cassert>
#include <Pegasus/Client/Client.h>

using namespace Pegasus;
using namespace std;

const String NAMESPACE = "root/cimv20";

static void TestGetClass(Client& client)
{
    ClassDecl c = client.getClass(
	NAMESPACE, "CIM_ComputerSystem", false, false, true);

    c.print();
}

static void TestClassOperations(Client& client)
{
    // CreateClass:

    ClassDecl c1("SubClass", "CIM_ManagedElement");
    c1.addQualifier(Qualifier("abstract", Boolean(true)));
    c1.addProperty(Property("count", Uint32(99)));
    c1.addProperty(Property("ratio", Real64(66.66)));
    c1.addProperty(Property("message", String("Hello World")));
    client.createClass(NAMESPACE, c1);

    // GetClass:

    ClassDecl c2 = client.getClass(NAMESPACE, "SubClass", false);
    // assert(c1.identical(c2));

    // Modify the class:

    c2.removeProperty(c2.findProperty("message"));
    client.modifyClass(NAMESPACE, c2);

    // GetClass:

    ClassDecl c3 = client.getClass(NAMESPACE, "SubClass", false);
    // assert(c3.identical(c2));

    // EnumerateClassNames:

    Array<String> classNames = client.enumerateClassNames(
	NAMESPACE, "CIM_ManagedElement", false);

    Boolean found = false;

    for (Uint32 i = 0; i < classNames.getSize(); i++)
    {
	if (classNames[i] == "SubClass")
	    found = true;
    }

    assert(found);

    // DeleteClass:

    client.deleteClass(NAMESPACE, "SubClass");

    // Get all the classes;


    classNames = client.enumerateClassNames(NAMESPACE, String(), false);

    Array<ClassDecl> classDecls = client.enumerateClasses(
	NAMESPACE, String(), false, false, true, true);

    assert(classDecls.getSize() == classNames.getSize());

    for (Uint32 i = 0; i < classNames.getSize(); i++)
    {
	ClassDecl tmp = client.getClass(
	    NAMESPACE, classNames[i], false, true, true);

	assert(classDecls[i].getClassName() == classNames[i]);

	assert(tmp.identical(classDecls[i]));
    }
}

static void TestQualifierOperations(Client& client)
{
cout << __LINE__ << endl;
    // Create two qualifier declarations:

    QualifierDecl qd1("qd1", false, Scope::CLASS, Flavor::TOSUBCLASS);
cout << __LINE__ << endl;
    client.setQualifier(NAMESPACE, qd1);
cout << __LINE__ << endl;

cout << __LINE__ << endl;
    QualifierDecl qd2("qd2", "Hello", Scope::PROPERTY | Scope::CLASS, 
	Flavor::OVERRIDABLE);
cout << __LINE__ << endl;
    client.setQualifier(NAMESPACE, qd2);

cout << __LINE__ << endl;
    // Get them and compare:

cout << __LINE__ << endl;
    QualifierDecl tmp1 = client.getQualifier(NAMESPACE, "qd1");
    assert(tmp1.identical(qd1));

    QualifierDecl tmp2 = client.getQualifier(NAMESPACE, "qd2");
    assert(tmp2.identical(qd2));

    // Enumerate the qualifiers:

    Array<QualifierDecl> qualifierDecls = client.enumerateQualifiers(NAMESPACE);

cout << __LINE__ << endl;
    for (Uint32 i = 0; i < qualifierDecls.getSize(); i++)
    {
cout << __LINE__ << endl;
	QualifierDecl tmp = qualifierDecls[i];

	if (tmp.getName() == "qd1")
	    assert(tmp1.identical(tmp));

	if (tmp.getName() == "qd2")
	    assert(tmp2.identical(tmp));
cout << __LINE__ << endl;
    }

    // Delete the qualifiers:
cout << __LINE__ << endl;

    client.deleteQualifier(NAMESPACE, "qd1");
    client.deleteQualifier(NAMESPACE, "qd2");
}

int main(int argc, char** argv)
{
    try
    {
cout << __LINE__ << endl;
	Client client;
cout << __LINE__ << endl;
	client.connect("localhost", 8888);
cout << __LINE__ << endl;
	TestQualifierOperations(client);
cout << __LINE__ << endl;
	TestClassOperations(client);
cout << __LINE__ << endl;
    }
    catch(Exception& e)
    {
	std::cerr << "Error: " << e.getMessage() << std::endl;
	exit(1);
    }

    std::cout << "+++++ passed all tests" << std::endl;

    return 0;
}
