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
#include <cstdlib>
#include <Pegasus/Common/CIMClass.h>
#include <Pegasus/Common/CIMQualifierDecl.h>
#include <Pegasus/Common/DeclContext.h>
#include <Pegasus/Common/CIMFlavor.h>
#include <Pegasus/Common/CIMValue.h>

PEGASUS_USING_PEGASUS;
PEGASUS_USING_STD;

#define TESTIO

void test01()
{
    const String NAMESPACE = "/ttt";

    SimpleDeclContext* context = new SimpleDeclContext;

    // Not sure about this one. How do I get NULL as CIMValue
    // This generates an empty string, not NULL
    CIMQualifierDecl q1("q1",String(),CIMScope::CLASS);

    CIMQualifierDecl q2("Abstract", Boolean(true), CIMScope::CLASS , CIMFlavor::NONE);
    
    CIMValue v1(CIMType::UINT32,false);
    CIMQualifierDecl q3("q1",v1,CIMScope::CLASS);

#ifdef TESTIO
    q1.print();
    q2.print();
    q3.print();
#endif

    context->addQualifierDecl(NAMESPACE, q1);
    context->addQualifierDecl(NAMESPACE, q2);

    CIMClass class2("SuperClass", "");
    
    context->addClass(NAMESPACE, class2);
    class2.resolve(context, NAMESPACE);
    
    CIMClass class1("SubClass", "SuperClass");

    class1
        .addQualifier(CIMQualifier("Abstract", Boolean(true)))
        .addQualifier(CIMQualifier("q1", "Hello"))
	.addQualifier(CIMQualifier("q3", Uint32(55)))
	.addProperty(CIMProperty("message", String("Hello")))
	.addProperty(CIMProperty("count", Uint32(77)))
	// .addProperty(CIMProperty("ref1", Reference("MyClass.key1=\"fred\"")))
	.addMethod(CIMMethod("isActive", CIMType::BOOLEAN)
	    .addParameter(CIMParameter("hostname", CIMType::STRING))
	    .addParameter(CIMParameter("port", CIMType::UINT32)));

 #ifdef TESTIO
    class1.print();
    class2.print();
 #endif
    try{
        class1.resolve(context, NAMESPACE);
        cout << "Passed basic resolution test" << endl;

        // Add assertions on the resolution.
        // Abstract did not move to subclass
        // 2. et.
 #ifdef TESTIO
    cout << "after resolve " << endl;
    class1.print();
    class2.print();
 #endif

    }
    catch (Exception& e)
    {
        cout << "Resolution Error " << e.getMessage() << endl;
    }
}
// Test for qualifier not declared
void test02()
{
    const String NAMESPACE = "/ttt";
    Boolean resolved = false;
    SimpleDeclContext* context = new SimpleDeclContext;

      CIMQualifierDecl q1("q1",String(),CIMScope::CLASS,
        CIMFlavor::DEFAULTS);

      CIMQualifierDecl q2("Abstract", Boolean(true), CIMScope::CLASS , 0);

      //CIMvalue v1(CIMType::UINT32,false);
      //CIMQualifierDecl q3("q1",v1,CIMScope::CLASS);

  #ifdef TESTIO
      q1.print();
      q2.print();
      //q3.print();
  #endif

    context->addQualifierDecl(NAMESPACE, q1);
    context->addQualifierDecl(NAMESPACE, q2);

    CIMClass class2("SuperClass", "");
    
    context->addClass(NAMESPACE, class2);
    class2.resolve(context, NAMESPACE);
    
    CIMClass class1("SubClass", "SuperClass");

    class1
        .addQualifier(CIMQualifier("Abstract", Boolean(true)))
        .addQualifier(CIMQualifier("q1", "Hello"))
	.addQualifier(CIMQualifier("q3", Uint32(55)))
	.addProperty(CIMProperty("message", String("Hello")))
	.addProperty(CIMProperty("count", Uint32(77)))
	// .addProperty(CIMProperty("ref1", Reference("MyClass.key1=\"fred\"")))
	.addMethod(CIMMethod("isActive", CIMType::BOOLEAN)
	    .addParameter(CIMParameter("hostname", CIMType::STRING))
	    .addParameter(CIMParameter("port", CIMType::UINT32)));
 #ifdef TESTIO
    class1.print();
    class2.print();
 #endif
    try{
        class1.resolve(context, NAMESPACE);
        resolved = true;
    }
    catch (Exception& e)
    {
        resolved = false;
        cout << " Found Resolution Error " << e.getMessage() << endl;
    }
    // Should have gotten the error
    if (resolved){
        cout << "Failed find undeclared qualifier test" << endl;
        assert(false);
    }
    else{
        cout << "Passed find undeclared qualifier test" << endl;
    }

}

//ATTN: KS P1 Mar 7 2002.  Add tests propagation qual, method, propertys as follows:
//  Confirm that qualifiers are propogated correctly based on flavors
//  Confirm that properties and methods are propagated correctly based on flavors
//  

int main()
{
    try
    {
	test01();
        test02();
        //test03();
    }
    catch (Exception& e)
    {
	cout << "Exception: " << e.getMessage() << endl;
	exit(1);
    }

    cout << "+++++ passed all tests" << endl;

    return 0;
}
