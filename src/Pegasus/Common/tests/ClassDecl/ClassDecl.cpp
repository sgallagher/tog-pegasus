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
// Modified By:	Karl Schopmeyer(k.schopmeyer@opengroup.org)
//              Sushma Fernandes (sushma_fernandes@hp.com)
//              Carol Ann Krug Graves, Hewlett-Packard Company
//                (carolann_graves@hp.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#include <cassert>
#include <iostream>
#include <Pegasus/Common/CIMClass.h>
#include <Pegasus/Common/CIMName.h>
#include <Pegasus/Common/XmlWriter.h>
#include <Pegasus/Common/MofWriter.h>

// ATTN-P3-KS - 20 March 2002 - Extend exception tests.
PEGASUS_USING_PEGASUS;
PEGASUS_USING_STD;
static char * verbose;

void test01()
{
    // class MyClass : YourClass
    // {
    //     string message = "Hello";
    // }

    CIMClass class0("//localhost/root/cimv2:MyClass", "YourClass");

    assert(class0.getPath() == CIMObjectPath("//localhost/root/cimv2:MyClass"));

    CIMClass class1("MyClass", "YourClass");

    class1
	.addQualifier(CIMQualifier("association", true))
	.addQualifier(CIMQualifier("q1", Uint32(55)))
	.addQualifier(CIMQualifier("q2", String("Hello")))
	.addProperty(CIMProperty("message", String("Hello")))
	.addProperty(CIMProperty("count", Uint32(77)))
	.addMethod(CIMMethod("isActive", CIMTYPE_BOOLEAN)
	    .addParameter(CIMParameter("hostname", CIMTYPE_STRING))
	    .addParameter(CIMParameter("port", CIMTYPE_UINT32)));

    // Test the method count function
    assert(class1.getClassName().equal("myclass"));
    assert(class1.getSuperClassName() == "YourClass");

    assert(class1.getMethodCount() ==1);


    // Test the findMethod and isMethod functions
    assert(class1.findMethod("isActive") != PEG_NOT_FOUND);
    assert(class1.findMethod("DoesNotExist") == PEG_NOT_FOUND);

    assert(class1.findMethod("isActive") != PEG_NOT_FOUND);
    assert(class1.findMethod("DoesNotExist") == PEG_NOT_FOUND);

    // Now add another method and reconfirm.

    class1.addMethod(CIMMethod("makeActive", CIMTYPE_BOOLEAN)
	.addParameter(CIMParameter("hostname", CIMTYPE_STRING))
	.addParameter(CIMParameter("port", CIMTYPE_UINT32)));

    assert(class1.getMethodCount() == 2);

    // Test the findMethod and isMethod functions
    // with two methods defined
    assert(class1.findMethod("isActive") != PEG_NOT_FOUND);
    assert(class1.findMethod("makeActive") != PEG_NOT_FOUND);

    assert(class1.findMethod("DoesNotExist") == PEG_NOT_FOUND);
    assert(class1.findMethod("isActive") != PEG_NOT_FOUND);
    assert(class1.findMethod("makeActive") != PEG_NOT_FOUND);

    assert(class1.findMethod("DoesNotExist") == PEG_NOT_FOUND);


    // Test RemoveMethod function
    Uint32 posMethod;
    posMethod = class1.findMethod("isActive");
    assert(posMethod != PEG_NOT_FOUND);

    class1.removeMethod(posMethod);

    assert(class1.findMethod("isActive") == PEG_NOT_FOUND);
    assert(class1.getMethodCount() == 1);

    //ATTN: P3 TODO add tests for different case names

    //Qualifier manipulation tests  (find, remove)

    assert(class1.findQualifier("q1") != PEG_NOT_FOUND);
    assert(class1.findQualifier("q2") != PEG_NOT_FOUND);
    assert(class1.findQualifier("qx") == PEG_NOT_FOUND);

    assert(class1.findQualifier("q1") != PEG_NOT_FOUND);
    assert(class1.findQualifier("q2") != PEG_NOT_FOUND);
    assert(class1.findQualifier("association") != PEG_NOT_FOUND);
    assert(class1.isAssociation());

    // Remove middle Qualifier "q2"
    Uint32 posQualifier;
    posQualifier = class1.findQualifier("q2");
    CIMConstQualifier qconst = class1.getQualifier(posQualifier);

    assert(class1.getQualifierCount() == 3);
    assert(posQualifier <= class1.getQualifierCount());
    class1.removeQualifier(posQualifier);
    assert(class1.getQualifierCount() == 2);

    assert(class1.findQualifier("q2") == PEG_NOT_FOUND);
    assert(class1.findQualifier("q1") != PEG_NOT_FOUND);
    assert(class1.isAssociation());


    // Remove the first parameter "q1"
    posQualifier = class1.findQualifier("q1");

    assert(class1.getQualifierCount() == 2);
    CIMQualifier cq = class1.getQualifier( class1.findQualifier("q1"));
    assert(posQualifier <= class1.getQualifierCount());
    class1.removeQualifier(posQualifier);
    assert(class1.getQualifierCount() == 1);

    assert(class1.findQualifier("q1") == PEG_NOT_FOUND);
    assert(class1.findQualifier("q2") == PEG_NOT_FOUND);
    assert(class1.isAssociation());


    // ATTH: P3 Add tests for try block for outofbounds



    //The property manipulation tests.

    assert(class1.findProperty("count") != PEG_NOT_FOUND);
    assert(class1.findProperty("message") != PEG_NOT_FOUND);

    assert(class1.findProperty("isActive") == PEG_NOT_FOUND);

    assert(class1.getPropertyCount() == 2);


    Uint32  posProperty;
    posProperty = class1.findProperty("count");
    CIMConstProperty constprop = class1.getProperty(posProperty);
    class1.removeProperty(posProperty);
    assert(class1.findProperty("message") != PEG_NOT_FOUND);
    assert(class1.findProperty("count") == PEG_NOT_FOUND);

    assert(class1.getPropertyCount() == 1);
    CIMProperty cp = class1.getProperty( class1.findProperty("message"));

	if(verbose)
	{
		XmlWriter::printClassElement(class1);
		MofWriter::printClassElement(class1);
	}

    Array<Sint8> out;
    MofWriter::appendClassElement(out, class1);
    out.clear();
    XmlWriter::appendClassElement(out, class1);
    
    assert(!class1.isAbstract());

    String squal("q1");
    assert(class1.findQualifier(squal) == PEG_NOT_FOUND);
    
    assert(!class1.hasKeys());
    
    Array<CIMName> keyNames;
    class1.getKeyNames(keyNames);

    CIMClass c2("MyClass");

    assert(c2.getClassName().equal("myclass"));


    // Error uninitialized handle
    c2.setSuperClassName("CIM_Element");
    assert(c2.getSuperClassName() == "CIM_Element");

    CIMClass c3 = c2.clone();
    c3 = c2;


    try
    {
        CIMMethod cm = c2.getMethod(0);
    }
    catch(OutOfBounds& e)
    {
		if(verbose)
			cout << "Exception: " << e.getMessage() << endl;
    }

    const CIMClass c4("MyClass", "YourClass");

    CIMConstClass c5("MyClass", "YourClass");
    CIMConstClass c6("MyClass");
    CIMConstClass cc7(c6);
    CIMClass c7 = c5.clone();
    const CIMClass c8(class1);

    // Test the findMethod and isMethod functions
    assert(c7.findMethod("DoesNotExist") == PEG_NOT_FOUND);

    assert(c7.findQualifier("dummy") == PEG_NOT_FOUND);

    try
    {
        CIMConstMethod cm = c8.getMethod(0);
    }
    catch(OutOfBounds& e)
    {
		if(verbose)
			cout << "Exception: " << e.getMessage() << endl;
    }

    try
    {
        CIMConstProperty ccp = c8.getProperty(c8.findProperty("count"));
    }
    catch(OutOfBounds& e)
    {
		if(verbose)
			cout << "Exception: " << e.getMessage() << endl;
    }

    if(verbose) 
    {
	XmlWriter::printClassElement(c5);
    }

    try
    {
        CIMConstMethod cm = cc7.getMethod(0);
    }
    catch(OutOfBounds& e)
    {
	if(verbose)
	    cout << "Exception: " << e.getMessage() << endl;
    }
    // Test the findMethod and isMethod functions
    assert(c4.findMethod("DoesNotExist") == PEG_NOT_FOUND);

    //Qualifier manipulation tests  (find, remove)

    assert(c4.findQualifier("qx") == PEG_NOT_FOUND);

    assert(c4.findQualifier("q1") == PEG_NOT_FOUND);
    assert(c4.findQualifier("q2") == PEG_NOT_FOUND);
    assert(c4.findQualifier("association") == PEG_NOT_FOUND);

    posProperty = c4.findProperty("count");

    try
    {
        CIMConstQualifier ccq = c4.getQualifier(c4.findQualifier("q1"));
    }
    catch (OutOfBounds& e)
    {
		if(verbose)
			cout << "Exception: " << e.getMessage() << endl;
    }

    assert(c4.findProperty("count") == PEG_NOT_FOUND);

    assert(c4.getClassName() == "MyClass");
    assert(c4.getClassName().equal("MyClass"));
    assert(c4.getClassName().equal("MYCLASS"));
    assert(c4.getClassName().equal("myclass"));
    assert(!c4.getClassName().equal("blob"));


    assert(c4.getSuperClassName() == "YourClass");

    // test the setSuperClassName function
    /* ATTN KS 29 April.  This test has problems.  Relook later. Think test, not code.
    c4.setSuperClassName("JunkClass"); 
    assert(c4.getSuperClassName() == "JunkClass"); 
    c4.setSuperClassName("YourClass");
    */
    assert(c5.getSuperClassName() == "YourClass");

    assert(c5.getQualifierCount() == 0);
    posQualifier = c5.findQualifier("q2");

    // throws out of bounds
    try
    {
        CIMConstQualifier qconst1 = c5.getQualifier(posQualifier);
    }
    catch(OutOfBounds& e)
    {
		if(verbose)
			cout << "Exception: " << e.getMessage() << endl;
    }
	if(verbose)
	{
		cout << "All tests" << endl;
	}
}

int main(int argc, char** argv)
{
    verbose = getenv("PEGASUS_TEST_VERBOSE");
    
	try
    {
	test01();
    }
    catch (Exception& e)
    {
	cout << "Exception: " << e.getMessage() << endl;
    }

    cout << argv[0] << " +++++ passed all tests" << endl;

    return 0;
}
