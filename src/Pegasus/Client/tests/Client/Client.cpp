//%2005////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001, 2002 BMC Software; Hewlett-Packard Development
// Company, L.P.; IBM Corp.; The Open Group; Tivoli Systems.
// Copyright (c) 2003 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation, The Open Group.
// Copyright (c) 2004 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation; VERITAS Software Corporation; The Open Group.
// Copyright (c) 2005 Hewlett-Packard Development Company, L.P.; IBM Corp.;
// EMC Corporation; VERITAS Software Corporation; The Open Group.
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
// Modified By: Yi Zhou, Hewlett-Packard Company (yi_zhou@hp.com)
//              Bapu Patil, Hewlett-Packard Company (bapu_patil@hp.com)
//              Nag Boranna, Hewlett-Packard Company (nagaraja_boranna@hp.com)
//              Carol Ann Krug Graves, Hewlett-Packard Company
//                  (carolann_graves@hp.com)
//              Amit K Arora, IBM (amita@in.ibm.com) for Bug#1979,#2011
//              David Dillard, VERITAS Software Corp.
//                  (david.dillard@veritas.com)
//              Josephine Eskaline Joyce, IBM (jojustin@in.ibm.com) for Bug#3383
//
//%/////////////////////////////////////////////////////////////////////////////

#include <Pegasus/Common/Config.h>
#include <cassert>
#include <Pegasus/Client/CIMClient.h>
#include <Pegasus/Common/CIMName.h>
#include <Pegasus/Common/OptionManager.h>

PEGASUS_USING_PEGASUS;
PEGASUS_USING_STD;

const CIMNamespaceName NAMESPACE = CIMNamespaceName ("root/cimv2");
const CIMNamespaceName SAMPLEPROVIDER_NAMESPACE = CIMNamespaceName ("root/SampleProvider");


static void TestGetClass(CIMClient& client)
{
    CIMClass c = client.getClass(
	NAMESPACE, CIMName ("CIM_ComputerSystem"), false, false, true);

    // c.print();
}

static void TestClassOperations(CIMClient& client)
{
    // CreateClass:

    CIMClass c1(CIMName ("SubClass"), CIMName ("CIM_ManagedElement"));
    c1.addQualifier(CIMQualifier(CIMName ("abstract"), Boolean(true)));
    c1.addProperty(CIMProperty(CIMName ("count"), Uint32(99)));
    c1.addProperty(CIMProperty(CIMName ("ratio"), Real64(66.66)));
    c1.addProperty(CIMProperty(CIMName ("message"), String("Hello World")));
    client.createClass(SAMPLEPROVIDER_NAMESPACE, c1);

    // GetClass:

    CIMClass c2 = client.getClass(SAMPLEPROVIDER_NAMESPACE, CIMName ("SubClass"), false);
    // assert(c1.identical(c2));

    // Modify the class:

    c2.removeProperty(c2.findProperty(CIMName ("message")));
    client.modifyClass(SAMPLEPROVIDER_NAMESPACE, c2);

    // GetClass:

    CIMClass c3 = client.getClass(SAMPLEPROVIDER_NAMESPACE, CIMName ("SubClass"), false);

    // assert(c3.identical(c2));

    // EnumerateClassNames:

    Array<CIMName> classNames = client.enumerateClassNames(
	SAMPLEPROVIDER_NAMESPACE, CIMName ("CIM_ManagedElement"), false);

    Boolean found = false;

    for (Uint32 i = 0; i < classNames.size(); i++)
    {
	if (classNames[i].equal(CIMName ("SubClass")))
	    found = true;
    }

    assert(found);

    // DeleteClass:

    client.deleteClass(SAMPLEPROVIDER_NAMESPACE, CIMName ("SubClass"));

    // Get all the classes;


    classNames = client.enumerateClassNames(NAMESPACE, CIMName(), false);

    Array<CIMClass> classDecls = client.enumerateClasses(
	NAMESPACE, CIMName(), false, false, true, true);

    assert(classDecls.size() == classNames.size());

    for (Uint32 i = 0; i < classNames.size(); i++)
    {
	CIMClass tmp = client.getClass(
	    NAMESPACE, classNames[i], false, true, true);

	assert(classNames[i].equal(classDecls[i].getClassName()));

	assert(tmp.identical(classDecls[i]));
    }
}

static void TestQualifierOperations(CIMClient& client)
{
    // Create two qualifier declarations:

    CIMQualifierDecl qd1(CIMName ("qd1"), false, CIMScope::CLASS,
        CIMFlavor::TOSUBCLASS);
    client.setQualifier(SAMPLEPROVIDER_NAMESPACE, qd1);

    CIMQualifierDecl qd2(CIMName ("qd2"), String("Hello"),
        CIMScope::PROPERTY + CIMScope::CLASS, CIMFlavor::OVERRIDABLE);
    client.setQualifier(SAMPLEPROVIDER_NAMESPACE, qd2);

    // Get them and compare:

    CIMQualifierDecl tmp1 = client.getQualifier(SAMPLEPROVIDER_NAMESPACE, CIMName ("qd1"));
    assert(tmp1.identical(qd1));

    CIMQualifierDecl tmp2 = client.getQualifier(SAMPLEPROVIDER_NAMESPACE, CIMName ("qd2"));
    assert(tmp2.identical(qd2));

    // Enumerate the qualifiers:

    Array<CIMQualifierDecl> qualifierDecls
	= client.enumerateQualifiers(SAMPLEPROVIDER_NAMESPACE);

    for (Uint32 i = 0; i < qualifierDecls.size(); i++)
    {
	CIMQualifierDecl tmp = qualifierDecls[i];

	if (tmp.getName().equal(CIMName ("qd1")))
	    assert(tmp1.identical(tmp));

	if (tmp.getName().equal(CIMName ("qd2")))
	    assert(tmp2.identical(tmp));
    }

    // Delete the qualifiers:

    client.deleteQualifier(SAMPLEPROVIDER_NAMESPACE, CIMName ("qd1"));
    client.deleteQualifier(SAMPLEPROVIDER_NAMESPACE, CIMName ("qd2"));
}

static void TestInstanceOperations(CIMClient& client)
{
    // Do some cleanup first.
    // Delete the instances and the class if it already exists.
    // If it doesn't exist, we get invalid-name OR not-found exception,
    // which is perfectly fine. Thus we ignore these exceptions.

    try
    {
        Array<CIMObjectPath> instanceNames =
           client.enumerateInstanceNames(SAMPLEPROVIDER_NAMESPACE, CIMName ("MyClass"));
	for (Uint32 i = 0; i < instanceNames.size(); i++)
        {
           client.deleteInstance(SAMPLEPROVIDER_NAMESPACE, instanceNames[i]);
        }
    }
    catch (const CIMException& e)
    {
       CIMStatusCode code = e.getCode();

       // Ignore CIM_ERR_INVALID_CLASS and CIM_ERR_NOT_FOUND exceptions
       if(code != CIM_ERR_INVALID_CLASS && code != CIM_ERR_NOT_FOUND)
       {
          throw;
       }
    }

    try
    {
        client.deleteClass(SAMPLEPROVIDER_NAMESPACE, CIMName ("myclass"));
    }
    catch (const CIMException& e)
    {
       // Ignore CIM_ERR_NOT_FOUND exception
       if(e.getCode() != CIM_ERR_NOT_FOUND)
       {
          throw;
       }
    }

    // Create a new class:

    CIMClass cimClass(CIMName ("MyClass"));
    cimClass
	.addProperty(CIMProperty(CIMName ("last"), String())
	    .addQualifier(CIMQualifier(CIMName ("key"), true)))
	.addProperty(CIMProperty(CIMName ("first"), String())
	    .addQualifier(CIMQualifier(CIMName ("key"), true)))
	.addProperty(CIMProperty(CIMName ("age"), Uint8(0))
	    .addQualifier(CIMQualifier(CIMName ("key"), true)));
    client.createClass(SAMPLEPROVIDER_NAMESPACE, cimClass);

    // Create an instance of that class:

    CIMInstance cimInstance(CIMName ("MyClass"));
    cimInstance.addProperty(CIMProperty(CIMName ("last"), String("Smith")));
    cimInstance.addProperty(CIMProperty(CIMName ("first"), String("John")));
    cimInstance.addProperty(CIMProperty(CIMName ("age"), Uint8(101)));
    CIMObjectPath instanceName = cimInstance.buildPath(cimClass);
    CIMObjectPath createdinstanceName = client.createInstance(SAMPLEPROVIDER_NAMESPACE, cimInstance);

    // Get the instance and compare with created one:

    CIMInstance tmp = client.getInstance(SAMPLEPROVIDER_NAMESPACE, instanceName);

    // cimInstance.print();
    // tmp.print();

    if(!cimInstance.identical(tmp))
    {
       Exception e("Instances do not match.");
       throw e;
    }

    client.deleteInstance(SAMPLEPROVIDER_NAMESPACE, instanceName);
}

static void TestAssociators(CIMClient& client)
{
    CIMObjectPath instanceName = CIMObjectPath ("Person.name=\"Mike\"");

    Array<CIMObject> result = client.associators(
	SAMPLEPROVIDER_NAMESPACE,
	instanceName,
	CIMName ("Lineage"),
	CIMName ("Person"),
	"parent",
	"child",
	true,
	true);

    for (Uint32 i = 0; i < result.size(); i++)
    {
	CIMObject current = result[i];
	CIMObjectPath ref = current.getPath ();
	cout << "[" << ref.toString() << "]" << endl;
    }

    cout << endl;
}

static void TestAssociatorNames(CIMClient& client)
{
    CIMObjectPath instanceName = CIMObjectPath ("Person.name=\"Mike\"");

    Array<CIMObjectPath> result = client.associatorNames(
	SAMPLEPROVIDER_NAMESPACE,
	instanceName,
	CIMName ("Lineage"),
	CIMName ("Person"),
	"parent",
	"child");

    for (Uint32 i = 0; i < result.size(); i++)
	cout << "[" << result[i].toString() << "]" << endl;

    cout << endl;
}

static void TestAssociatorClassNames(CIMClient& client)
{
    CIMObjectPath className = CIMObjectPath ("Person");

    Array<CIMObjectPath> result = client.associatorNames(
	SAMPLEPROVIDER_NAMESPACE,
	className,
	CIMName ("Lineage"),
	CIMName ("Person"),
	"parent",
	"child");

    for (Uint32 i = 0; i < result.size(); i++)
	cout << "[" << result[i].toString() << "]" << endl;

    cout << endl;
}

static void TestReferenceNames(CIMClient& client)
{
    CIMObjectPath instanceName = CIMObjectPath ("Person.name=\"Mike\"");

    Array<CIMObjectPath> result = client.referenceNames(
	SAMPLEPROVIDER_NAMESPACE,
	instanceName,
	CIMName ("Lineage"),
	"parent");

    for (Uint32 i = 0; i < result.size(); i++)
	cout << "[" << result[i].toString() << "]" << endl;

    cout << endl;
}

static void TestReferences(CIMClient& client)
{
    CIMObjectPath instanceName = CIMObjectPath ("Person.name=\"Mike\"");

    Array<CIMObject> result = client.references(
	SAMPLEPROVIDER_NAMESPACE,
	instanceName,
	CIMName ("Lineage"),
	"parent");

    for (Uint32 i = 0; i < result.size(); i++)
    {
	CIMObject current = result[i];
	CIMObjectPath ref = current.getPath ();
	cout << "[" << ref.toString() << "]" << endl;
    }

    cout << endl;
}

static void TestReferenceClassNames(CIMClient& client)
{
    CIMObjectPath className = CIMObjectPath ("Person");

    Array<CIMObjectPath> result = client.referenceNames(
	SAMPLEPROVIDER_NAMESPACE,
	className,
	CIMName(),
	"parent");

    for (Uint32 i = 0; i < result.size(); i++)
	cout << "[" << result[i].toString() << "]" << endl;

    cout << endl;
}

int main(int argc, char** argv)
{
    try
    {
	CIMClient client;

        client.connectLocal();

	TestGetClass(client);
	TestQualifierOperations(client);
	TestClassOperations(client);
	TestInstanceOperations(client);

	// To run the following test, first compile "test.mof" into the
	// repository!
	TestAssociatorNames(client);
	TestAssociators(client);
	TestReferenceNames(client);
	TestReferences(client);
	TestAssociatorClassNames(client);
	TestReferenceClassNames(client);
    }
    catch(Exception& e)
    {
	PEGASUS_STD(cerr) << "Error: " << e.getMessage() << PEGASUS_STD(endl);
	exit(1);
    }

    PEGASUS_STD(cout) << "+++++ passed all tests" << PEGASUS_STD(endl);

    return 0;
}
