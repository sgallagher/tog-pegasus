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
// Modified By: Sushma Fernandes (sushma_fernandes@hp.com)
//              Carol Ann Krug Graves, Hewlett-Packard Company
//                (carolann_graves@hp.com)
//              Karl Schopmeyer
//              Roger Kumpf, Hewlett-Packard Company (roger_kumpf@hp.com)
//
//%/////////////////////////////////////////////////////////////////////////////

/* This program tests the generation and resolution of instances.  It creates
   a set of qualifiers and a class and then creates instances of the class
   and confirms both the creation characteristics and the resolution
   characteristics.
*/

#include <cassert>
#include <Pegasus/Common/CIMInstance.h>
#include <Pegasus/Common/CIMClass.h>
#include <Pegasus/Common/CIMName.h>
#include <Pegasus/Common/DeclContext.h>
#include <Pegasus/Common/Resolver.h>
#include <Pegasus/Common/XmlWriter.h>

PEGASUS_USING_PEGASUS;
PEGASUS_USING_STD;

static char * verbose;

void test01()
{
    const CIMNamespaceName NAMESPACE = CIMNamespaceName("/zzz");

    // Create and populate a declaration context:

    SimpleDeclContext* context = new SimpleDeclContext;

    context->addQualifierDecl(
        NAMESPACE, CIMQualifierDecl(CIMName("counter"), false,
        CIMScope::PROPERTY));

    context->addQualifierDecl(
        NAMESPACE, CIMQualifierDecl(CIMName("classcounter"), false,
        CIMScope::CLASS));

    context->addQualifierDecl(
        NAMESPACE, CIMQualifierDecl(CIMName("min"), String(),
        CIMScope::PROPERTY));

    context->addQualifierDecl(
        NAMESPACE, CIMQualifierDecl(CIMName("max"), String(),
        CIMScope::PROPERTY));

    context->addQualifierDecl(NAMESPACE,
        CIMQualifierDecl(CIMName("Description"), String(),
        CIMScope::PROPERTY));

    CIMClass class1(CIMName("MyClass"));

    class1
        .addProperty(CIMProperty(CIMName("count"), Uint32(55))
            .addQualifier(CIMQualifier(CIMName("counter"), true))
            .addQualifier(CIMQualifier(CIMName("min"), String("0")))
            .addQualifier(CIMQualifier(CIMName("max"), String("1"))))
        .addProperty(CIMProperty(CIMName("message"), String("Hello"))
            .addQualifier(CIMQualifier(CIMName("description"),
                String("My Message"))))
        .addProperty(CIMProperty(CIMName("ratio"), Real32(1.5)));

    // Test
    assert(class1.findProperty(CIMName("count")) != PEG_NOT_FOUND);
    assert(class1.findProperty(CIMName("message")) != PEG_NOT_FOUND);
    assert(class1.findProperty(CIMName("ratio")) != PEG_NOT_FOUND);

    Resolver::resolveClass(class1, context, NAMESPACE);
    context->addClass(NAMESPACE, class1);

    if (verbose)
    {
        XmlWriter::printClassElement(class1);
    }

    CIMInstance instance0(CIMName("MyClass"));
    assert(instance0.getClassName().equal(CIMName("MyClass")));
    instance0.setPath(CIMObjectPath("//localhost/root/cimv2:MyClass.Foo=1"));
    assert(instance0.getPath() ==
        CIMObjectPath("//localhost/root/cimv2:MyClass.Foo=1"));

    CIMInstance instance1(CIMName("MyClass"));
    instance1.addQualifier(CIMQualifier(CIMName("classcounter"), true));

    instance1.addProperty(CIMProperty(CIMName("message"), String("Goodbye")));

    assert(instance1.findProperty(CIMName("message")) != PEG_NOT_FOUND);

    assert(instance1.findProperty(CIMName("count")) == PEG_NOT_FOUND);
    assert(instance1.findProperty(CIMName("ratio")) == PEG_NOT_FOUND);
    assert(instance1.findProperty(CIMName("nuts")) == PEG_NOT_FOUND);
    assert(instance1.getPropertyCount() == 1);

    if (verbose)
    {
        XmlWriter::printInstanceElement(instance1);
    }

    Resolver::resolveInstance(instance1, context, NAMESPACE, true);

    if (verbose)
    {
        XmlWriter::printInstanceElement(instance1);
    }

    // Now test for properties after resolution.

    assert(instance1.findProperty(CIMName("message")) != PEG_NOT_FOUND);
    assert(instance1.findProperty(CIMName("count")) != PEG_NOT_FOUND);
    assert(instance1.findProperty(CIMName("ratio")) != PEG_NOT_FOUND);
    assert(instance1.findProperty(CIMName("nuts")) == PEG_NOT_FOUND);
    assert(instance1.getPropertyCount() == 3);

    // Now remove a property

    Uint32 posProperty;
    posProperty = instance1.findProperty(CIMName("count"));
    instance1.removeProperty(posProperty);

    assert(instance1.findProperty(CIMName("message")) != PEG_NOT_FOUND);
    assert(instance1.findProperty(CIMName("count")) == PEG_NOT_FOUND);
    assert(instance1.findProperty(CIMName("ratio")) != PEG_NOT_FOUND);
    assert(instance1.findProperty(CIMName("nuts")) == PEG_NOT_FOUND);

    assert(instance1.getPropertyCount() == 2);

    // Instance qualifier tests

    CIMQualifier cq=instance1.getQualifier(instance1.findQualifier(
        CIMName("classcounter")));

    const CIMInstance instance2 = instance1.clone();
    assert(instance2.identical(instance1));
    assert(instance1.findQualifier(CIMName("nuts")) == PEG_NOT_FOUND);
    assert(instance2.findQualifier(CIMName("nuts")) == PEG_NOT_FOUND);
    assert(instance1.getQualifierCount() != 4);
    assert(instance1.getQualifierCount() == 1);
    assert(instance2.getQualifierCount() == 1);

    if (verbose)
    {
        XmlWriter::printInstanceElement(instance2);
    }

    // Tests for CIMConstInstance
    CIMConstInstance cinstance1(CIMName("MyClass")), cinstance3;
    CIMConstInstance ccopy(cinstance1);

    cinstance1 = instance1;
    assert(cinstance1.identical(instance1));

    assert(cinstance1.getQualifierCount() == 1);
    CIMConstQualifier ccq = cinstance1.getQualifier(cinstance1.findQualifier(
        CIMName("classcounter")));
    assert(cinstance1.findProperty(CIMName("message")) != PEG_NOT_FOUND);
    CIMConstProperty ccp =
        cinstance1.getProperty(cinstance1.findProperty(CIMName("message")));

    cinstance3 = cinstance1;
    assert(cinstance3.identical(cinstance1));

    assert(cinstance1.getClassName() == CIMName("MyClass"));
    assert(cinstance1.getClassName().equal(CIMName("MyClass")));
    assert(cinstance1.getClassName().equal(CIMName("MYCLASS")));
    assert(cinstance1.getClassName().equal(CIMName("myclass")));
    assert(!cinstance1.getClassName().equal(CIMName("blob")));

    assert(cinstance1.getQualifierCount() != 4);
    assert(cinstance1.getPropertyCount() == 2);

    CIMConstInstance cinstance2 = cinstance1.clone();
    assert(cinstance2.identical(cinstance1));

    if (verbose)
    {
        XmlWriter::printInstanceElement(cinstance1);
    }

    cinstance1.buildPath(class1);

    assert( !cinstance1.isUninitialized() );

    delete context;
}

void test02()
{
    const CIMNamespaceName NAMESPACE = CIMNamespaceName("/zzz");

    CIMClass cimClass(CIMName("MyClass"));

    cimClass
        .addProperty(CIMProperty(CIMName("Last"), String())
            .addQualifier(CIMQualifier(CIMName("key"), true)))
        .addProperty(CIMProperty(CIMName("First"), String())
            .addQualifier(CIMQualifier(CIMName("key"), true)))
        .addProperty(CIMProperty(CIMName("Age"), String())
            .addQualifier(CIMQualifier(CIMName("key"), true)));

    CIMInstance cimInstance(CIMName("MyClass"));
    cimInstance.addProperty(CIMProperty(CIMName("first"), String("John")));
    cimInstance.addProperty(CIMProperty(CIMName("last"), String("Smith")));
    cimInstance.addProperty(CIMProperty(CIMName("age"), Uint8(101)));

    assert(cimInstance.findProperty(CIMName("first")) != PEG_NOT_FOUND);
    assert(cimInstance.findProperty(CIMName("last")) != PEG_NOT_FOUND);
    assert(cimInstance.findProperty(CIMName("age")) != PEG_NOT_FOUND);

    assert(cimInstance.getPropertyCount() == 3);


    CIMObjectPath instanceName =
        cimInstance.buildPath(CIMConstClass(cimClass));

    CIMObjectPath tmp("myclass.age=101,first=\"John\",last=\"Smith\"");

    assert(tmp.makeHashCode() == instanceName.makeHashCode());

    // Test CIMInstance::buildPath with incomplete keys in the instance

    Boolean caughtNoSuchPropertyException = false;

    try
    {
        CIMInstance badInstance(CIMName("MyClass"));
        badInstance.addProperty(CIMProperty(CIMName("first"), String("John")));
        badInstance.addProperty(CIMProperty(CIMName("last"), String("Smith")));
        CIMObjectPath instanceName =
            badInstance.buildPath(CIMConstClass(cimClass));
    }
    catch (const NoSuchProperty&)
    {
        caughtNoSuchPropertyException = true;
    }

    assert(caughtNoSuchPropertyException);
}

// Build the instance of an association class and test the build path functions.
void test03()
{
    const CIMNamespaceName NAMESPACE = CIMNamespaceName("/zzz");

    CIMClass myPersonClass(CIMName("MY_PersonClass"));

    myPersonClass
        .addProperty(CIMProperty(CIMName("name"), String())
            .addQualifier(CIMQualifier(CIMName("Key"), true)));

    CIMClass myAssocClass(CIMName("MY_AssocClass"));

    myAssocClass
        .addQualifier(CIMQualifier(CIMName("association"), true))
        .addProperty(CIMProperty(CIMName("parent"), CIMObjectPath(), 0,
            CIMName("MY_PersonClass"))
            .addQualifier(CIMQualifier(CIMName("key"), true)))
        .addProperty(CIMProperty(CIMName("child"), CIMObjectPath(), 0,
            CIMName("MY_PersonClass"))
            .addQualifier(CIMQualifier(CIMName("key"), true)))
        .addProperty(CIMProperty(CIMName("Age"), String()));

    CIMInstance fatherInstance(CIMName("MY_PersonClass"));

    fatherInstance
        .addProperty(CIMProperty(CIMName("name"), String("father")));

    CIMInstance daughterInstance(CIMName("My_PersonClass"));
    daughterInstance
       .addProperty(CIMProperty(CIMName("name"), String("daughter")));

    CIMObjectPath fatherInstancePath =
        fatherInstance.buildPath(CIMConstClass(myPersonClass));

    CIMObjectPath daughterInstancePath =
        daughterInstance.buildPath(CIMConstClass(myPersonClass));

    CIMInstance assocInstance(CIMName("MY_AssocClass"));

    assocInstance.addProperty(CIMProperty(CIMName("parent"),
        CIMObjectPath(fatherInstancePath),0,CIMName("MY_PersonClass")));

    assocInstance.addProperty(CIMProperty(CIMName("child"),
        CIMObjectPath(daughterInstancePath),0, CIMName("MY_PersonClass")));

    CIMObjectPath assocInstancePath =
        assocInstance.buildPath(CIMConstClass(myAssocClass));

    // Now do the asserts, etc.
    // See if the pathing works on Associations and association instances

    if (verbose)
    {
        XmlWriter::printClassElement(myPersonClass);
        XmlWriter::printClassElement(myAssocClass);
        XmlWriter::printInstanceElement(fatherInstance);
        XmlWriter::printInstanceElement(daughterInstance);
        XmlWriter::printInstanceElement(assocInstance);
    }

    if (verbose)
    {
        cout << "Paths " << endl;
        cout << "FatherInstancePath = " << fatherInstancePath.toString() <<
            endl;
        cout << "DaughterInstancePath = " << daughterInstancePath.toString() <<
            endl;
        cout << "AssocInstancePath = " << assocInstancePath.toString() << endl;
    }

    String x ="MY_AssocClass.child=\"My_PersonClass.name=\\\"daughter\\\"\","
        "parent=\"MY_PersonClass.name=\\\"father\\\"\"";
    assert(x == assocInstancePath.toString());
    CIMObjectPath px = x;
    assert(px.identical(assocInstancePath));
}

Boolean _propertyIdentical(
    const char* propertyName,
    CIMInstance& instance1,
    CIMInstance& instance2)
{
    Uint32 pos = instance1.findProperty(propertyName);
    CIMConstProperty p1 = instance1.getProperty(pos);
    pos = instance2.findProperty(propertyName);
    CIMConstProperty p2 = instance2.getProperty(pos);
    return (p1.identical(p2));
}

//
// Test of the Instance creation and instance filtering
//
void test04()
{
    if (verbose)
    {
        cout << "Test04 - Create instance from Class. " << endl;
    }

    const CIMNamespaceName NAMESPACE = CIMNamespaceName("/zzz");

    // Create and populate a declaration context:

    SimpleDeclContext* context = new SimpleDeclContext;

    context->addQualifierDecl(
        NAMESPACE, CIMQualifierDecl(CIMName("counter"), false,
        CIMScope::PROPERTY));

    context->addQualifierDecl(
        NAMESPACE, CIMQualifierDecl(CIMName("classcounter"), false,
        CIMScope::CLASS));

    context->addQualifierDecl(
        NAMESPACE, CIMQualifierDecl(CIMName("min"), String(),
        CIMScope::PROPERTY));

    context->addQualifierDecl(
        NAMESPACE, CIMQualifierDecl(CIMName("max"), String(),
        CIMScope::PROPERTY));

    context->addQualifierDecl(NAMESPACE,
        CIMQualifierDecl(CIMName("Description"), String(),
        CIMScope::PROPERTY));

    CIMClass class1(CIMName("MyClass"));

    class1
        .addProperty(CIMProperty(CIMName("count"), Uint32(55))
            .addQualifier(CIMQualifier(CIMName("counter"), true))
            .addQualifier(CIMQualifier(CIMName("min"), String("0")))
            .addQualifier(CIMQualifier(CIMName("max"), String("1"))))
        .addProperty(CIMProperty(CIMName("message"), String("Hello"))
            .addQualifier(CIMQualifier(CIMName("description"),
                String("My Message"))))
        .addProperty(CIMProperty(CIMName("ratio"), Real32(1.5)));


    Resolver::resolveClass(class1, context, NAMESPACE);
    context->addClass(NAMESPACE, class1);


    if (verbose)
    {
        XmlWriter::printClassElement(class1);
    }

    //
    // Create instance with qualifiers, classorigin, and Null propertyList
    //
    {
        CIMInstance newInstance;

        newInstance = class1.buildInstance(true, true, CIMPropertyList());

        if (verbose)
        {
            XmlWriter::printInstanceElement(newInstance);
        }

        assert(newInstance.getPropertyCount() == class1.getPropertyCount());
        assert(newInstance.getQualifierCount() == class1.getQualifierCount());
        assert(newInstance.findProperty("ratio") != PEG_NOT_FOUND);
        assert(newInstance.findProperty("message") != PEG_NOT_FOUND);

    }

    //
    // Test with include qualifiers false. Should be no qualifiers in result
    //
    {
        CIMInstance newInstance =
            class1.buildInstance(false, true, CIMPropertyList());

        assert(newInstance.getQualifierCount() == 0);
        assert(newInstance.getPropertyCount() == class1.getPropertyCount());
        assert(newInstance.findProperty("ratio") != PEG_NOT_FOUND);
        assert(newInstance.findProperty("message") != PEG_NOT_FOUND);
    }

    //
    // Test with empty property list.  Should have no properties.
    //
    {
        Array<CIMName> pl1Array;
        CIMPropertyList pl1(pl1Array);

        CIMInstance newInstance = class1.buildInstance(false, true, pl1);

        assert(newInstance.getQualifierCount() == 0);
        assert(newInstance.getPropertyCount() == 0);
    }

    //
    // Test with a property that exists in property list.
    //
    {
        Array<CIMName> pl1Array;
        pl1Array.append("ratio");
        CIMPropertyList pl1(pl1Array);

        CIMInstance newInstance = class1.buildInstance(false, true, pl1);

        if (verbose)
        {
            cout << "Test with one property in new instance" << endl;
            XmlWriter::printInstanceElement(newInstance);
        }

        assert(newInstance.getPropertyCount() == 1);
        assert(newInstance.findProperty("ratio") != PEG_NOT_FOUND);
        assert(newInstance.findProperty("message") == PEG_NOT_FOUND);
        assert(newInstance.getQualifierCount() == 0);
    }

    //
    // Test with a property that does/does not exist in property list
    //
    {
        Array<CIMName> pl1Array;
        CIMPropertyList pl1(pl1Array);
        pl1.clear();
        pl1Array.append("blob");
        pl1Array.append("ratio");
        pl1.set(pl1Array);

        CIMInstance newInstance = class1.buildInstance(false, true, pl1);

        assert(newInstance.getPropertyCount() == 1);
        assert(newInstance.findProperty("ratio") != PEG_NOT_FOUND);
        assert(newInstance.findProperty("blob") == PEG_NOT_FOUND);
        assert(newInstance.findProperty("message") == PEG_NOT_FOUND);
        assert(newInstance.getQualifierCount() == 0);
    }

    ///////////////////////////////////////////////////////////////////////
    //
    // Instance Filtering function tests
    //
    ///////////////////////////////////////////////////////////////////////

    // build instance as starting point for tests.
    CIMInstance tstInstance =
        class1.buildInstance(true, true, CIMPropertyList());

    //
    // Test complete copy, no change
    //
    {
        if (verbose)
        {
            cout << "Test1" << endl;
        }

        CIMInstance filterInstance = tstInstance.clone();
        filterInstance.filter(true, true, CIMPropertyList());

        assert(tstInstance.identical(filterInstance));
        assert(filterInstance.getPropertyCount() == 3);
        assert(filterInstance.getQualifierCount() ==
            tstInstance.getQualifierCount());
    }

    //
    // Filter to one property, ratio
    //
    {
        if (verbose)
        {
            cout << "Test2" << endl;
        }

        Array<CIMName> pl1Array;
        pl1Array.append("ratio");
        CIMPropertyList pl1(pl1Array);

        CIMInstance filterInstance = tstInstance.clone();
        filterInstance.filter(true, true, pl1);

        if (verbose)
        {
            XmlWriter::printInstanceElement(filterInstance);
        }

        assert(filterInstance.getPropertyCount() == 1);
        assert(filterInstance.findProperty("ratio") != PEG_NOT_FOUND);
        assert(_propertyIdentical("ratio", filterInstance, tstInstance));
        assert(filterInstance.getQualifierCount() ==
                tstInstance.getQualifierCount());
    }

    //
    // Filter to one property, message
    //
    {
        if (verbose)
        {
            cout << "Test3" << endl;
        }

        Array<CIMName> pl1Array;
        pl1Array.append("message");
        CIMPropertyList pl1(pl1Array);

        CIMInstance filterInstance = tstInstance.clone();
        filterInstance.filter(true, true, pl1);

        if (verbose)
        {
            XmlWriter::printInstanceElement(filterInstance);
        }

        assert(filterInstance.getPropertyCount() == 1);
        assert(filterInstance.findProperty("message") != PEG_NOT_FOUND);
        assert(_propertyIdentical("message", filterInstance, tstInstance));
        assert(filterInstance.getQualifierCount() ==
                tstInstance.getQualifierCount());
    }

    //
    // Filter to one property, count
    //
    {
        if (verbose)
        {
            cout << "Test4" << endl;
        }

        Array<CIMName> pl1Array;
        pl1Array.append("count");
        CIMPropertyList pl1(pl1Array);

        CIMInstance filterInstance = tstInstance.clone();
        filterInstance.filter(true, true, pl1);

        if (verbose)
        {
            XmlWriter::printInstanceElement(filterInstance);
        }

        assert(filterInstance.getPropertyCount() == 1);
        assert(filterInstance.findProperty("count") != PEG_NOT_FOUND);
        assert(filterInstance.getQualifierCount() ==
                tstInstance.getQualifierCount());
    }

    //
    // Filter to no properties
    //
    {
        if (verbose)
        {
            cout << "Test5a" << endl;
        }

        Array<CIMName> pl1Array;
        CIMPropertyList pl1(pl1Array);

        CIMInstance filterInstance = tstInstance.clone();
        filterInstance.filter(true, true, pl1);

        assert(filterInstance.getPropertyCount() == 0);
        assert(filterInstance.findProperty("ratio") == PEG_NOT_FOUND);
        assert(filterInstance.getQualifierCount() ==
            tstInstance.getQualifierCount());
    }

    //
    // Filter to two properties
    //
    {
        if (verbose)
        {
            cout << "Test5b" << endl;
        }

        Array<CIMName> pl1Array;
        pl1Array.append("count");
        pl1Array.append("message");
        CIMPropertyList pl1(pl1Array);

        CIMInstance filterInstance = tstInstance.clone();
        filterInstance.filter(true, true, pl1);

        assert(filterInstance.getPropertyCount() == 2);
        assert(filterInstance.findProperty("ratio") == PEG_NOT_FOUND);
        assert(filterInstance.findProperty("message") != PEG_NOT_FOUND);
        assert(_propertyIdentical("message", filterInstance, tstInstance));
        assert(filterInstance.findProperty("count") != PEG_NOT_FOUND);
        assert(_propertyIdentical("count", filterInstance, tstInstance));
        assert(filterInstance.getQualifierCount() ==
                tstInstance.getQualifierCount());
    }

    //
    // Filter to no qualifiers and all properties.
    //
    {
        if (verbose)
        {
            cout << "Test6" << endl;
        }

        CIMInstance filterInstance = tstInstance.clone();
        filterInstance.filter(false, true, CIMPropertyList());

        assert(filterInstance.getPropertyCount() == 3);
        assert(filterInstance.findProperty("ratio") != PEG_NOT_FOUND);
        assert(filterInstance.getQualifierCount() == 0);
        for (Uint32 i = 0; i < filterInstance.getPropertyCount() ; i++)
        {
            CIMConstProperty p = filterInstance.getProperty(i);
            assert(p.getQualifierCount() == 0);
        }
    }

    //
    // Filter to no qualifiers and no properties.
    //
    {
        if (verbose)
        {
            cout << "Test6a" << endl;
        }

        Array<CIMName> pl1Array;
        CIMPropertyList pl1(pl1Array);

        CIMInstance filterInstance = tstInstance.clone();
        filterInstance.filter(false, true, pl1);

        assert(filterInstance.getPropertyCount() == 0);
        assert(filterInstance.findProperty("ratio") == PEG_NOT_FOUND);
        assert(filterInstance.getQualifierCount() == 0);
    }

    //
    // Test Class Origin Filter
    //
    {
        if (verbose)
        {
            cout << "Test7 Class Origin" << endl;
        }

        // Create a subclass to do classOrigin testing
        CIMClass mySubClass(CIMName("subclass"));
        mySubClass.setSuperClassName(CIMName("MyClass"));

        Resolver::resolveClass(mySubClass, context, NAMESPACE);
        context->addClass(NAMESPACE, mySubClass);

        // build instance
        CIMInstance filterInstance =
            mySubClass.buildInstance(true, true, CIMPropertyList());
        filterInstance.filter(false, true, CIMPropertyList());

        assert(filterInstance.getPropertyCount() == 3);
        assert(filterInstance.findProperty("ratio") != PEG_NOT_FOUND);
        assert(filterInstance.getQualifierCount() == 0);
        for (Uint32 i = 0 ; i < filterInstance.getPropertyCount() ; i++)
        {
            CIMProperty p = filterInstance.getProperty(i);
            assert(!(p.getClassOrigin() == CIMName()));

        }
        filterInstance.filter(false, false, CIMPropertyList());

        for (Uint32 i = 0 ; i < filterInstance.getPropertyCount() ; i++)
        {
            CIMProperty p = filterInstance.getProperty(i);
            assert(p.getClassOrigin() == CIMName());
        }

        CIMInstance filterInstance2 =
            mySubClass.buildInstance(true, false, CIMPropertyList());

        for (Uint32 i = 0 ; i < filterInstance2.getPropertyCount() ; i++)
        {
            CIMProperty p = filterInstance2.getProperty(i);
            assert(p.getClassOrigin() == CIMName());
        }
    }
    delete context;
}

//
// Test instance qualifier operations
//
void test05()
{
    CIMInstance instance1(CIMName("MyClass"));
    instance1.addQualifier(CIMQualifier(CIMName("Qualifier1"), true));
    instance1.addQualifier(CIMQualifier(CIMName("Qualifier2"), Uint32(1)));
    instance1.addQualifier(CIMQualifier(CIMName("Qualifier3"), String("a")));

    Boolean caughtException = false;

    try
    {
        instance1.addQualifier(CIMQualifier(CIMName("Qualifier1"), true));
    }
    catch (const AlreadyExistsException&)
    {
        caughtException = true;
    }

    assert(caughtException);
    assert(instance1.getQualifierCount() == 3);

    // Test that the correct qualifiers exist in the instance

    Uint32 qualifierIndex;

    qualifierIndex = instance1.findQualifier(CIMName("Qualifier1"));
    assert(qualifierIndex != PEG_NOT_FOUND);

    qualifierIndex = instance1.findQualifier(CIMName("Qualifier2"));
    assert(qualifierIndex != PEG_NOT_FOUND);

    qualifierIndex = instance1.findQualifier(CIMName("Qualifier3"));
    assert(qualifierIndex != PEG_NOT_FOUND);

    qualifierIndex = instance1.findQualifier(CIMName("Qualifier4"));
    assert(qualifierIndex == PEG_NOT_FOUND);

    // Clone the instance and test for the correct qualifiers

    CIMConstInstance instance2 = instance1.clone();
    assert(instance2.getQualifierCount() == 3);

    qualifierIndex = instance2.findQualifier(CIMName("Qualifier1"));
    assert(qualifierIndex != PEG_NOT_FOUND);

    qualifierIndex = instance2.findQualifier(CIMName("Qualifier2"));
    assert(qualifierIndex != PEG_NOT_FOUND);

    qualifierIndex = instance2.findQualifier(CIMName("Qualifier3"));
    assert(qualifierIndex != PEG_NOT_FOUND);

    qualifierIndex = instance2.findQualifier(CIMName("Qualifier4"));
    assert(qualifierIndex == PEG_NOT_FOUND);

    // Remove Qualifier2 from instance1

    qualifierIndex = instance1.findQualifier(CIMName("Qualifier2"));
    instance1.removeQualifier(qualifierIndex);
    assert(instance1.getQualifierCount() == 2);

    qualifierIndex = instance1.findQualifier(CIMName("Qualifier2"));
    assert(qualifierIndex == PEG_NOT_FOUND);

    qualifierIndex = instance1.findQualifier(CIMName("Qualifier1"));
    assert(qualifierIndex != PEG_NOT_FOUND);

    // Confirm that the qualifiers are still in instance2

    assert(instance2.getQualifierCount() == 3);
    qualifierIndex = instance2.findQualifier(CIMName("Qualifier1"));
    assert(qualifierIndex != PEG_NOT_FOUND);

    qualifierIndex = instance2.findQualifier(CIMName("Qualifier2"));
    assert(qualifierIndex != PEG_NOT_FOUND);

    // Remove a non-existant qualifier

    caughtException = false;

    try
    {
        instance1.removeQualifier(10);
    }
    catch (const IndexOutOfBoundsException&)
    {
        caughtException = true;
    }

    assert(caughtException);

    // Remove Qualifier1 and Qualifier3 from instance1

    qualifierIndex = instance1.findQualifier(CIMName("Qualifier1"));
    instance1.removeQualifier(qualifierIndex);
    assert(instance1.getQualifierCount() == 1);

    qualifierIndex = instance1.findQualifier(CIMName("Qualifier3"));
    instance1.removeQualifier(qualifierIndex);
    assert(instance1.getQualifierCount() == 0);

    qualifierIndex = instance1.findQualifier(CIMName("Qualifier1"));
    assert(qualifierIndex == PEG_NOT_FOUND);

    qualifierIndex = instance1.findQualifier(CIMName("Qualifier3"));
    assert(qualifierIndex == PEG_NOT_FOUND);

    // Confirm that the qualifiers are still in instance2

    assert(instance2.getQualifierCount() == 3);
    qualifierIndex = instance2.findQualifier(CIMName("Qualifier1"));
    assert(qualifierIndex != PEG_NOT_FOUND);

    qualifierIndex = instance2.findQualifier(CIMName("Qualifier2"));
    assert(qualifierIndex != PEG_NOT_FOUND);

    qualifierIndex = instance2.findQualifier(CIMName("Qualifier3"));
    assert(qualifierIndex != PEG_NOT_FOUND);
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
    }
    catch (Exception& e)
    {
        cout << "Exception: " << e.getMessage() << endl;
        exit(1);
    }

    cout << argv[0] << " +++++ passed all tests" << endl;

    return 0;
}
