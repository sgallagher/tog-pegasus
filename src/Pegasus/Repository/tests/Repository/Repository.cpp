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
#include <Pegasus/Repository/CIMRepository.h>

PEGASUS_USING_PEGASUS;
PEGASUS_USING_STD;

void test01()
{
    CIMRepository r("./repository");

    const String NAMESPACE = "aa/bb";

    try
    {
	r.createNameSpace(NAMESPACE);
    }
    catch (AlreadyExists&)
    {
	// Ignore this!
    }

    CIMClass c("MyClass");

    r.setQualifier(
	NAMESPACE, CIMQualifierDecl("key", true, CIMScope::PROPERTY));

    c.addProperty(
	CIMProperty("key", Uint32(0))
	    .addQualifier(CIMQualifier("key", true)))
	.addProperty(CIMProperty("ratio", Real32(1.5)))
	.addProperty(CIMProperty("message", "Hello World"));

    r.createClass(NAMESPACE, c);

    CIMConstClass cc;
    cc = r.getClass("aa/bb", "MyClass");

    assert(c.identical(cc));
    assert(cc.identical(c));

    // cc.print();
}

void test02()
{
    //--------------------------------------------------------------------------
    // Create repository:
    //--------------------------------------------------------------------------

    CIMRepository r("./repository");

    const String NAMESPACE = "aa/bb";

    try
    {
	r.createNameSpace(NAMESPACE);
    }
    catch (CIMException& e)
    {
	PEGASUS_ASSERT(e.getCode() == CIMException::ALREADY_EXISTS);
	// Ignore this!
    }

    //--------------------------------------------------------------------------
    // Create Class (SuperClass):
    //--------------------------------------------------------------------------

    CIMClass superClass("SuperClass");

    superClass
	.addProperty(CIMProperty("Last", String())
	    .addQualifier(CIMQualifier("key", true)))
	.addProperty(CIMProperty("First", String())
	    .addQualifier(CIMQualifier("key", true)))
	.addProperty(CIMProperty("Age", Uint8(0))
	    .addQualifier(CIMQualifier("key", true)));

    r.createClass(NAMESPACE, superClass);

    //--------------------------------------------------------------------------
    // Create Class (SubClass):
    //--------------------------------------------------------------------------

    CIMClass subClass("SubClass", "SuperClass");
    subClass.addProperty(CIMProperty("Role", String()));
    r.createClass(NAMESPACE, subClass);

    //--------------------------------------------------------------------------
    // Create Instance (of SubClass):
    //--------------------------------------------------------------------------

    CIMInstance subClassInstance("SubClass");
    subClassInstance.addProperty(CIMProperty("Last", "Smith"));
    subClassInstance.addProperty(CIMProperty("First", "John"));
    subClassInstance.addProperty(CIMProperty("Age", Uint8(101)));
    subClassInstance.addProperty(CIMProperty("Role", "Taylor"));
    r.createInstance(NAMESPACE, subClassInstance);

    //--------------------------------------------------------------------------
    // Get instance back:
    //--------------------------------------------------------------------------

    CIMReference instanceName1 = subClassInstance.getInstanceName(subClass);

    CIMReference instanceName2 =
	"SuperClass.first=\"John\",last=\"Smith\",age=101";

    CIMInstance tmp = r.getInstance(NAMESPACE, instanceName2);

    assert(subClassInstance.identical(tmp));
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
	cout << e.getMessage() << endl;
	exit(1);
    }

    cout << "+++++ passed all tests" << endl;

    return 0;
}
