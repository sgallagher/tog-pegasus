//%/////////////////////////////////////////////////////////////////////////////
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
//==============================================================================
//
// Author: Mike Brasher (mbrasher@bmc.com)
//
// Modified By:	Karl Schopmeyer(k.schopmeyer@opengroup.org)
//
//%/////////////////////////////////////////////////////////////////////////////

#include <cassert>
#include <Pegasus/Common/CIMClass.h>
#include <Pegasus/Common/CIMName.h>

PEGASUS_USING_PEGASUS;
PEGASUS_USING_STD;

void test01()
{
    // class MyClass : YourClass
    // {
    //     string message = "Hello";
    // }

    CIMClass class1("MyClass", "YourClass");
    
    class1
	.addQualifier(CIMQualifier("association", true))
	.addQualifier(CIMQualifier("q1", Uint32(55)))
	.addQualifier(CIMQualifier("q2", "Hello"))
	.addProperty(CIMProperty("message", "Hello"))
	.addProperty(CIMProperty("count", Uint32(77)))
	.addMethod(CIMMethod("isActive", CIMType::BOOLEAN)
	    .addParameter(CIMParameter("hostname", CIMType::STRING))
	    .addParameter(CIMParameter("port", CIMType::UINT32)));

    // Test the method count function
    assert(class1.getMethodCount() ==1);

    
    // Test the findMethod and isMethod functions
    assert(class1.findMethod("isActive") != -1);
    assert(class1.findMethod("DoesNotExist") == -1);

    assert(class1.existsMethod("isActive"));
    assert(!class1.existsMethod("DoesNotExist"));

    // Now add another method and reconfirm.

    class1.addMethod(CIMMethod("makeActive", CIMType::BOOLEAN)
	.addParameter(CIMParameter("hostname", CIMType::STRING))
	.addParameter(CIMParameter("port", CIMType::UINT32)));

    assert(class1.getMethodCount() == 2);

    // Test the findMethod and isMethod functions
    // with two methods defined
    assert(class1.findMethod("isActive") != -1);
    assert(class1.findMethod("makeActive") != -1);

    assert(class1.findMethod("DoesNotExist") == -1);
    assert(class1.existsMethod("isActive"));
    assert(class1.existsMethod("makeActive"));

    assert(!class1.existsMethod("DoesNotExist"));


    // Test RemoveMethod function
    Uint32 posMethod; 
    posMethod = class1.findMethod("isActive");
    assert(posMethod != -1);

    class1.removeMethod(posMethod);

    assert(class1.findMethod("isActive") == -1);
    assert(class1.getMethodCount() ==1);
       
    //ATTN: TODO add tests for different case names

    //Qualifier manipulation tests  (find, exists, remove)

    assert(class1.findQualifier("q1") != -1);
    assert(class1.findQualifier("q2") != -1);
    assert(class1.findQualifier("qx") == -1);

    assert(class1.existsQualifier("q1"));
    assert(class1.existsQualifier("q2"));
    assert(class1.existsQualifier("association"));
    assert(class1.isAssociation());

    // Remove middle Qualifier "q2"
    Uint32 posQualifier;
    posQualifier = class1.findQualifier("q2");

    assert(class1.getQualifierCount() == 3);
    assert(posQualifier <= class1.getQualifierCount());
    class1.removeQualifier(posQualifier);
    assert(class1.getQualifierCount() == 2);

    assert(class1.findQualifier("q2") == -1);
    assert(!class1.existsQualifier("q2"));
    assert(class1.existsQualifier("q1"));
    assert(class1.isAssociation());


    // Remove the first parameter "q1"
    posQualifier = class1.findQualifier("q1");
    
    assert(class1.getQualifierCount() == 2);
    assert(posQualifier <= class1.getQualifierCount());
    class1.removeQualifier(posQualifier);
    assert(class1.getQualifierCount() == 1);

    assert(class1.findQualifier("q1") == -1);
    assert(!class1.existsQualifier("q1"));
    assert(!class1.existsQualifier("q2"));
    assert(class1.isAssociation());





    // ATTH:Add tests for try block for outofbounds



    //The property manipulation tests.

    assert(class1.findProperty("count") != -1);
    assert(class1.findProperty("message") != -1);

    assert(class1.existsProperty("count"));
    assert(class1.existsProperty("message"));

    assert(class1.findProperty("isActive") == -1);
    assert(!class1.existsProperty("isActive"));

    assert(class1.getPropertyCount() == 2);

 
    Uint32  posProperty;
    posProperty = class1.findProperty("count");
    class1.removeProperty(posProperty);
    assert(class1.existsProperty("message"));
    assert(!class1.existsProperty("count"));

    assert(class1.getPropertyCount() == 1);


    // class1.print();
}

int main()
{
    try
    {
	test01();
    }
    catch (Exception& e)
    {
	cout << "Exception: " << e.getMessage() << endl;
    }

    cout << "+++++ passed all tests" << endl;

    return 0;
}
