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
// Modified By: Sushma Fernandes (sushma_fernandes@hp.com)
//              Carol Ann Krug Graves, Hewlett-Packard Company
//                (carolann_graves@hp.com)
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
    const CIMNamespaceName NAMESPACE = CIMNamespaceName ("/zzz");

    // Create and populate a declaration context:

    SimpleDeclContext* context = new SimpleDeclContext;

    context->addQualifierDecl(
	NAMESPACE, CIMQualifierDecl(CIMName ("counter"), false, 
        CIMScope::PROPERTY));

    context->addQualifierDecl(
	NAMESPACE, CIMQualifierDecl(CIMName ("classcounter"), false, 
        CIMScope::CLASS));


    context->addQualifierDecl(
	NAMESPACE, CIMQualifierDecl(CIMName ("min"), String(), 
        CIMScope::PROPERTY));

    context->addQualifierDecl(
	NAMESPACE, CIMQualifierDecl(CIMName ("max"), String(), 
        CIMScope::PROPERTY));

    context->addQualifierDecl(NAMESPACE,
	CIMQualifierDecl(CIMName ("Description"), String(), 
        CIMScope::PROPERTY));

    CIMClass class1(CIMName ("MyClass"));

    class1
	.addProperty(CIMProperty(CIMName ("count"), Uint32(55))
	    .addQualifier(CIMQualifier(CIMName ("counter"), true))
	    .addQualifier(CIMQualifier(CIMName ("min"), String("0")))
	    .addQualifier(CIMQualifier(CIMName ("max"), String("1"))))
	.addProperty(CIMProperty(CIMName ("message"), String("Hello"))
	    .addQualifier(CIMQualifier(CIMName ("description"), 
                String("My Message"))))
	.addProperty(CIMProperty(CIMName ("ratio"), Real32(1.5)));


    // Test
    assert(class1.findProperty(CIMName ("count")) != PEG_NOT_FOUND);
    assert(class1.findProperty(CIMName ("message")) != PEG_NOT_FOUND);
    assert(class1.findProperty(CIMName ("ratio")) != PEG_NOT_FOUND);

    Resolver::resolveClass (class1, context, NAMESPACE);
    context->addClass(NAMESPACE, class1);

	if(verbose) {
		XmlWriter::printClassElement(class1);
	}

    CIMInstance instance0(CIMName ("MyClass"));
    assert(instance0.getClassName().equal(CIMName ("MyClass")));
    instance0.setPath(CIMObjectPath("//localhost/root/cimv2:MyClass.Foo=1"));
    assert(instance0.getPath() == CIMObjectPath("//localhost/root/cimv2:MyClass.Foo=1"));

    assert(instance0.getPath() == CIMObjectPath("//localhost/root/cimv2:MyClass.Foo=1"));

    CIMInstance instance1(CIMName ("MyClass"));
    instance1.addQualifier(CIMQualifier(CIMName ("classcounter"), true));

    instance1.addProperty(CIMProperty(CIMName ("message"), String("Goodbye")));

    assert(instance1.findProperty(CIMName ("message")) != PEG_NOT_FOUND);

    assert(instance1.findProperty(CIMName ("count")) == PEG_NOT_FOUND);
    assert(instance1.findProperty(CIMName ("ratio")) == PEG_NOT_FOUND);
    assert(instance1.findProperty(CIMName ("nuts")) == PEG_NOT_FOUND);
    assert(instance1.getPropertyCount() == 1);

	if(verbose)
		XmlWriter::printInstanceElement(instance1);

    Resolver::resolveInstance (instance1, context, NAMESPACE, true);

	if(verbose)
		XmlWriter::printInstanceElement(instance1);

    // Now test for properties after resolution.

    assert(instance1.findProperty(CIMName ("message")) != PEG_NOT_FOUND);
    assert(instance1.findProperty(CIMName ("count")) != PEG_NOT_FOUND);
    assert(instance1.findProperty(CIMName ("ratio")) != PEG_NOT_FOUND);
    assert(instance1.findProperty(CIMName ("nuts")) == PEG_NOT_FOUND);

    assert(instance1.getPropertyCount() == 3);
    // Now remove a property

    Uint32 posProperty;
    posProperty = instance1.findProperty(CIMName ("count"));
    instance1.removeProperty(posProperty);

    assert(instance1.findProperty(CIMName ("message")) != PEG_NOT_FOUND);
    assert(instance1.findProperty(CIMName ("count")) == PEG_NOT_FOUND);
    assert(instance1.findProperty(CIMName ("ratio")) != PEG_NOT_FOUND);
    assert(instance1.findProperty(CIMName ("nuts")) == PEG_NOT_FOUND);

    assert(instance1.getPropertyCount() == 2);
 
    // SF-HP

    CIMQualifier cq=instance1.getQualifier(instance1.findQualifier
        (CIMName ("classcounter")));

    const CIMInstance instance2 = instance1.clone();
    assert(instance2.identical(instance1));
    assert(instance1.findQualifier(CIMName ("nuts")) == PEG_NOT_FOUND);
    assert(instance2.findQualifier(CIMName ("nuts")) == PEG_NOT_FOUND);
    assert(instance1.getQualifierCount() != 4);

	// Confirm that the classcounter qualifier is in instance 2
	// NOTE: This is dangerous coding and generates an exception
	// out of bounds error if the qualifier	does not exist.
    CIMConstQualifier ccq=
        instance2.getQualifier(instance2.findQualifier
        (CIMName ("classcounter")));

	if(verbose)
		XmlWriter::printInstanceElement(instance2);

    // Tests for CIMConstInstance 
    CIMConstInstance cinstance1(CIMName ("MyClass")), cinstance3;
    CIMConstInstance ccopy(cinstance1);

    cinstance1 = instance1;
    assert(cinstance1.identical(instance1));

    ccq = cinstance1.getQualifier(cinstance1.findQualifier(CIMName ("classcounter")));
    assert(cinstance1.findProperty(CIMName ("message")) != PEG_NOT_FOUND);
    CIMConstProperty ccp = 
           cinstance1.getProperty(cinstance1.findProperty(CIMName ("message")));

    cinstance3 = cinstance1;
    assert(cinstance3.identical(cinstance1));

    assert(cinstance1.getClassName() == CIMName ("MyClass"));
    assert(cinstance1.getClassName().equal(CIMName ("MyClass")));
    assert(cinstance1.getClassName().equal(CIMName ("MYCLASS")));
    assert(cinstance1.getClassName().equal(CIMName ("myclass")));
    assert(!cinstance1.getClassName().equal(CIMName ("blob")));



    assert(cinstance1.getQualifierCount() != 4);
    assert(cinstance1.getPropertyCount() == 2);
   
    CIMConstInstance cinstance2 = cinstance1.clone();
    assert(cinstance2.identical(cinstance1));


	if(verbose)
		XmlWriter::printInstanceElement(cinstance1);
  
    cinstance1.buildPath(class1);
 
    assert( !cinstance1.isUninitialized() );

    delete context;
}

void test02()
{
    const CIMNamespaceName NAMESPACE = CIMNamespaceName ("/zzz");

    CIMClass cimClass(CIMName ("MyClass"));

    cimClass
	.addProperty(CIMProperty(CIMName ("Last"), String())
	    .addQualifier(CIMQualifier(CIMName ("key"), true)))
	.addProperty(CIMProperty(CIMName ("First"), String())
	    .addQualifier(CIMQualifier(CIMName ("key"), true)))
	.addProperty(CIMProperty(CIMName ("Age"), String())
	    .addQualifier(CIMQualifier(CIMName ("key"), true)));

    CIMInstance cimInstance(CIMName ("MyClass"));
    cimInstance.addProperty(CIMProperty(CIMName ("first"), String("John")));
    cimInstance.addProperty(CIMProperty(CIMName ("last"), String("Smith")));
    cimInstance.addProperty(CIMProperty(CIMName ("age"), Uint8(101)));


    assert(cimInstance.findProperty(CIMName ("first")) != PEG_NOT_FOUND);
    assert(cimInstance.findProperty(CIMName ("last")) != PEG_NOT_FOUND);
    assert(cimInstance.findProperty(CIMName ("age")) != PEG_NOT_FOUND);

    assert(cimInstance.getPropertyCount() == 3);


    // ATTN: Should we be doing an instance qualifier add and test


    CIMObjectPath instanceName
	= cimInstance.buildPath(CIMConstClass(cimClass));

    CIMObjectPath tmp("myclass.age=101,first=\"John\",last=\"Smith\"");

    assert(tmp.makeHashCode() == instanceName.makeHashCode());
}

// Build the instance of an association class and test the build path functions.
void test03()
{
    const CIMNamespaceName NAMESPACE = CIMNamespaceName ("/zzz");

    CIMClass myPersonClass(CIMName ("MY_PersonClass"));

	myPersonClass
        .addProperty(CIMProperty(CIMName ("name"), String())
	        .addQualifier(CIMQualifier(CIMName ("Key"), true)));

    CIMClass myAssocClass(CIMName ("MY_AssocClass"));

    myAssocClass
    .addQualifier(CIMQualifier(CIMName ("association"), true))
	.addProperty(CIMProperty(CIMName ("parent"), CIMObjectPath(),0, CIMName ("MY_PersonClass"))
	    .addQualifier(CIMQualifier(CIMName ("key"), true)))
	.addProperty(CIMProperty(CIMName ("child"), CIMObjectPath(), 0, CIMName ("MY_PersonClass"))
	    .addQualifier(CIMQualifier(CIMName ("key"), true)))
	.addProperty(CIMProperty(CIMName ("Age"), String()));

    CIMInstance fatherInstance(CIMName ("MY_PersonClass"));

    fatherInstance
        .addProperty(CIMProperty(CIMName ("name"), String("father")));

    CIMInstance daughterInstance(CIMName ("My_PersonClass"));
    daughterInstance
       .addProperty(CIMProperty(CIMName ("name"), String("daughter")));
     
    CIMObjectPath fatherInstancePath = fatherInstance.buildPath(CIMConstClass(myPersonClass));

    CIMObjectPath daughterInstancePath = daughterInstance.buildPath(CIMConstClass(myPersonClass));

    CIMInstance assocInstance(CIMName ("MY_AssocClass"));

    assocInstance.addProperty(CIMProperty(CIMName ("parent"),
        CIMObjectPath(fatherInstancePath),0,CIMName("MY_PersonClass")));

    assocInstance.addProperty(CIMProperty(CIMName ("child"),
        CIMObjectPath(daughterInstancePath),0, CIMName("MY_PersonClass")));

    CIMObjectPath assocInstancePath = assocInstance.buildPath(CIMConstClass(myAssocClass));

    // Now do the asserts, etc.   See if the pathing works on Associations and association instances
	
    if(verbose)
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
        cout << "FatherInstancePath = " << fatherInstancePath.toString()<< endl;
        cout << "DaughterInstancePath = " << daughterInstancePath.toString()<< endl;
        cout << "AssocInstancePath = " << assocInstancePath.toString()<< endl;
    }
    String x ="MY_AssocClass.child=R\"My_PersonClass.name=\\\"daughter\\\"\",parent=R\"MY_PersonClass.name=\\\"father\\\"\"";
    assert(x == assocInstancePath.toString());
    CIMObjectPath px = x;
    assert(px.identical(assocInstancePath));

}

int main(int argc, char** argv)
{
    verbose = getenv("PEGASUS_TEST_VERBOSE");
    try
    {
	test01();
	test02();
    test03();
    }
    catch (Exception& e)
    {
	cout << "Exception: " << e.getMessage() << endl;
	exit(1);
    }

    cout << argv[0] << " +++++ passed all tests" << endl;

    return 0;
}
