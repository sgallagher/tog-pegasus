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
// $Log: InstanceDecl.cpp,v $
// Revision 1.3  2001/02/20 05:16:57  mike
// Implemented CIMInstance::getInstanceName()
//
// Revision 1.2  2001/02/16 02:06:07  mike
// Renamed many classes and headers.
//
// Revision 1.1.1.1  2001/01/14 19:53:46  mike
// Pegasus import
//
//
//END_HISTORY

#include <cassert>
#include <Pegasus/Common/CIMInstance.h>
#include <Pegasus/Common/CIMClass.h>
#include <Pegasus/Common/DeclContext.h>

using namespace Pegasus;
using namespace std;

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

    class1.resolve(context, NAMESPACE);
    context->addClassDecl(NAMESPACE, class1);
    // class1.print();

    CIMInstance instance1("MyClass");
    instance1.addProperty(CIMProperty("message", "Goodbye"));
    instance1.resolve(context, NAMESPACE);
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

    // ATTN-1: ugly! Should we get rid of Const types?
    String instanceName = cimInstance.getInstanceName(ConstCIMClass(cimClass));

    const char EXPECT[] = "myclass.age=101,first=\"John\",last=\"Smith\"";
    assert(instanceName == EXPECT);
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
