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

//
// Author: Mike Brasher (mbrasher@bmc.com)
//
// Modified By:
//
//%/////////////////////////////////////////////////////////////////////////////

#include <cassert>
#include <Pegasus/Common/CIMInstance.h>
#include <Pegasus/Common/CIMClass.h>
#include <Pegasus/Common/DeclContext.h>

PEGASUS_USING_PEGASUS;
PEGASUS_USING_STD;

void test01()
{
    const String NAMESPACE = "/zzz";

    // Create and populate a declaration context:

    SimpleDeclContext* context = new SimpleDeclContext;

    context->addQualifierDecl(
	NAMESPACE, CIMQualifierDecl("counter", false, CIMScope::PROPERTY));

    context->addQualifierDecl(
	NAMESPACE, CIMQualifierDecl("min", String(), CIMScope::PROPERTY));

    context->addQualifierDecl(
	NAMESPACE, CIMQualifierDecl("max", String(), CIMScope::PROPERTY));

    context->addQualifierDecl(NAMESPACE,
	CIMQualifierDecl("Description", String(), CIMScope::PROPERTY));

    CIMClass class1("MyClass");

    class1
	.addProperty(CIMProperty("count", Uint32(55))
	    .addQualifier(CIMQualifier("counter", true))
	    .addQualifier(CIMQualifier("min", "0"))
	    .addQualifier(CIMQualifier("max", "1")))
	.addProperty(CIMProperty("message", "Hello")
	    .addQualifier(CIMQualifier("description", "My Message")))
	.addProperty(CIMProperty("ratio", Real32(1.5)));


    // Test
    assert(class1.findProperty("count") != PEG_NOT_FOUND);
    assert(class1.findProperty("message") != PEG_NOT_FOUND);
    assert(class1.findProperty("ratio") != PEG_NOT_FOUND);

    assert(class1.existsProperty("count"));
    assert(class1.existsProperty("message"));
    assert(class1.existsProperty("ratio"));

    class1.resolve(context, NAMESPACE);
    context->addClass(NAMESPACE, class1);
    // class1.print();

    CIMInstance instance1("MyClass");
    instance1.addProperty(CIMProperty("message", "Goodbye"));

    assert(instance1.findProperty("message") != PEG_NOT_FOUND);
    assert(instance1.existsProperty("message"));

    assert(!instance1.existsProperty("count"));
    assert(!instance1.existsProperty("ratio"));
    assert(!instance1.existsProperty("nuts"));
    assert(instance1.getPropertyCount() == 1);

    instance1.resolve(context, NAMESPACE);

    // Now test for parameters after resolution.

    assert(instance1.findProperty("message") != PEG_NOT_FOUND);
    assert(instance1.existsProperty("message"));
    assert(instance1.existsProperty("count"));
    assert(instance1.existsProperty("ratio"));
    assert(!instance1.existsProperty("nuts"));

    assert(instance1.getPropertyCount() == 3);

    // Now remove a property

    Uint32 posProperty;
    posProperty = instance1.findProperty("count");
    instance1.removeProperty(posProperty);

    assert(instance1.existsProperty("message"));
    assert(!instance1.existsProperty("count"));
    assert(instance1.existsProperty("ratio"));
    assert(!instance1.existsProperty("nuts"));

    assert(instance1.getPropertyCount() == 2);

}

void test02()
{
    const String NAMESPACE = "/zzz";

    CIMClass cimClass("MyClass");

    cimClass
	.addProperty(CIMProperty("Last", String())
	    .addQualifier(CIMQualifier("key", true)))
	.addProperty(CIMProperty("First", String())
	    .addQualifier(CIMQualifier("key", true)))
	.addProperty(CIMProperty("Age", String())
	    .addQualifier(CIMQualifier("key", true)));

    CIMInstance cimInstance("MyClass");
    cimInstance.addProperty(CIMProperty("first", "John"));
    cimInstance.addProperty(CIMProperty("last", "Smith"));
    cimInstance.addProperty(CIMProperty("age", Uint8(101)));


    assert(cimInstance.existsProperty("first"));
    assert(cimInstance.existsProperty("last"));
    assert(cimInstance.existsProperty("age"));

    assert(cimInstance.getPropertyCount() == 3);




    // ATTN: Should we be doing an instance qualifier add and test




    CIMReference instanceName
	= cimInstance.getInstanceName(CIMConstClass(cimClass));

    CIMReference tmp("myclass.age=101,first=\"John\",last=\"Smith\"");

    assert(tmp.makeHashCode() == instanceName.makeHashCode());
}

int main()
{
    try
    {
	test01();
	test02();
    }
    catch (Exception& e)
    {
	cout << "Exception: " << e.getMessage() << endl;
	exit(1);
    }

    cout << "+++++ passed all tests" << endl;

    return 0;
}
