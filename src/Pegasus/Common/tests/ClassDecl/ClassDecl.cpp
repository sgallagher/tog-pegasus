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
// Modified By:
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
    cout << "first" << endl;
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
    cout << "first 2" << endl;

    
    // Test the findMethod and isMethod functions
    assert(class1.findMethod("isActive") != -1);
    assert(class1.findMethod("DoesNotExist") == -1);
    cout << "first 3" << endl;

    assert(class1.existsMethod("isActive"));
    assert(!class1.existsMethod("DoesNotExist"));

    cout << "first 4" << endl;

    // Now add another method and reconfirm.

    class1.addMethod(CIMMethod("makeActive", CIMType::BOOLEAN)
	.addParameter(CIMParameter("hostname", CIMType::STRING))
	.addParameter(CIMParameter("port", CIMType::UINT32)));
    cout << "first 5" << endl;

    assert(class1.getMethodCount() == 2);
    cout << "first 6" << endl;

    // Test the findMethod and isMethod functions
    // with two methods defined
    assert(class1.findMethod("isActive") != -1);
    assert(class1.findMethod("makeActive") != -1);
    cout << "first 7" << endl;

    assert(class1.findMethod("DoesNotExist") == -1);
    assert(class1.existsMethod("isActive"));
    assert(class1.existsMethod("makeActive"));

    assert(!class1.existsMethod("DoesNotExist"));
    cout << "first 8" << endl;


    // Test RemoveMethod function
    Uint32 posMethod; 
    posMethod = class1.findMethod("isActive");
    assert(posMethod != -1);
    cout << "first 9" << endl;


    class1.removeMethod(posMethod);

    cout << "first 10" << endl;

    assert(class1.findMethod("isActive") == -1);
    assert(class1.getMethodCount() ==1);
    cout << "first11" << endl;
       
    //ATTN: TODO add tests for different case names

    //ATTN: TODO - Add qualifier manipulation tests

    //ATTN: TODO - Add the property manipulation tests.

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
