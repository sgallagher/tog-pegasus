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
// Modified By:  Jenny Yu (jenny_yu@hp.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#include <cassert>
#include <Pegasus/Common/CIMObject.h>
#include <Pegasus/Common/CIMClass.h>
#include <Pegasus/Common/CIMInstance.h>

PEGASUS_USING_PEGASUS;
PEGASUS_USING_STD;

static char * verbose;

//*********************************************************************
//  CIMObject tests
//
//  The CIMObject class refers either to a CIMInstance or a CIMClass.
//*********************************************************************
void test01()
{
    CIMObject obj;
    //
    // Construct from CIMClass
    //
    CIMClass cimClass1("//localhost/root/cimv2:MyClass");
    CIMObject oclass1 = cimClass1;
    CIMObject oclass2(cimClass1);
    CIMObject oclass3(oclass2);
    CIMObject oclass4 = oclass3;

    CIMClass cimClass2 = cimClass1;
    cimClass2 = cimClass1;
    cimClass2 = CIMClass(oclass1);
    CIMClass cimClass3 = CIMClass(oclass1);

    assert(oclass1.getClassName() == "MyClass");
    assert(oclass1.getPath() == CIMReference("//localhost/root/cimv2:MyClass"));

    //
    // Construct from CIMInstance
    //
    CIMInstance cimInstance1("MyClass");
    CIMObject oinstance1 = cimInstance1;
    CIMObject instance2(cimInstance1);

    // Test qualifiers
    oinstance1.addQualifier(CIMQualifier("Key", true));
    oinstance1.addQualifier(CIMQualifier("Description", "Just a Test"));
    oinstance1.addQualifier(CIMQualifier("q1", true));
    oinstance1.addQualifier(CIMQualifier("q2", true));

    assert(oinstance1.findQualifier("Key") != PEG_NOT_FOUND);
    assert(oinstance1.findQualifier("Description") != PEG_NOT_FOUND);
    assert(oinstance1.findQualifier("q1") != PEG_NOT_FOUND);
    assert(oinstance1.findQualifier("q2") != PEG_NOT_FOUND);
    assert(oinstance1.findQualifier("q3") == PEG_NOT_FOUND);
    assert(oinstance1.getQualifierCount() == 4);

    assert(oinstance1.existsQualifier("q1"));
    assert(oinstance1.existsQualifier("q2"));
    assert(!oinstance1.existsQualifier("q3"));
    assert(!oinstance1.existsQualifier("q4"));

    Uint32 posQualifier;
    posQualifier = oinstance1.findQualifier("q1");
    assert(posQualifier != PEGASUS_NOT_FOUND);
    assert(posQualifier < oinstance1.getQualifierCount());

    try
    {
        CIMQualifier q1 = oinstance1.getQualifier(posQualifier);
        assert(q1);
        CIMConstQualifier cq1 = oinstance1.getQualifier(posQualifier);
        assert(cq1);
    }
    catch(OutOfBounds& e)
    {
        if(verbose)
            cout << "Exception: " << e.getMessage() << endl;

    }

    oinstance1.removeQualifier(posQualifier);
    assert(oinstance1.getQualifierCount() == 3);
    assert(!oinstance1.existsQualifier("q1"));
    assert(oinstance1.existsQualifier("q2"));

    // Test properties
    oinstance1.addProperty(CIMProperty("message", "Hello There"));
    oinstance1.addProperty(CIMProperty("count", Uint32(77)));

    assert(oinstance1.findProperty("count") != PEG_NOT_FOUND);
    assert(oinstance1.findProperty("message") != PEG_NOT_FOUND);
    assert(oinstance1.findProperty("ratio") == PEG_NOT_FOUND);

    assert(oinstance1.existsProperty("count"));
    assert(oinstance1.existsProperty("message"));
    assert(!oinstance1.existsProperty("ratio"));

    CIMProperty cp =
       oinstance1.getProperty(oinstance1.findProperty("message"));

    Uint32 posProperty;
    posProperty = oinstance1.findProperty("count");
    oinstance1.removeProperty(posProperty);
    assert(!oinstance1.existsProperty("count"));

    assert(oinstance1.getPropertyCount() == 1);

    const CIMObject oinstance2 = oinstance1.clone();
    assert(oinstance2.identical(oinstance1));

    if (verbose)
    {
        Array<Sint8> xmlOut;
        oinstance1.toXml(xmlOut);
    }
}

//*********************************************************************
//  CIMConstObject tests
//*********************************************************************
void test02()
{
    CIMConstObject obj;
    //
    // Construct from CIMClass
    //
    CIMClass class1("//localhost/root/cimv2:MyClass");
    CIMObject obj1 = class1;

    CIMConstClass cclass1("MyClass");
    CIMConstObject cobj1(class1);
    CIMConstObject cobj2(cclass1);
    CIMConstObject cobj3(obj1);

    //
    // Construct from CIMInstance
    //
    CIMInstance instance("MyClass");
    CIMObject obj2 = instance;

    obj2.addQualifier(CIMQualifier("Key", true));
    obj2.addQualifier(CIMQualifier("Description", "Just a Test"));
    obj2.addProperty(CIMProperty("message", "Hello There"));
    obj2.addProperty(CIMProperty("count", Uint32(77)));

    CIMConstInstance cinstance("MyClass");
    CIMConstObject cobj4(instance);
    CIMConstObject cobj5(cinstance);
    CIMConstObject cobj6(obj2);

    CIMConstObject cobj7 = obj1;
    CIMConstObject cobj8 = cobj7;
    CIMConstObject cobj9 = class1;
    CIMConstObject cobj10 = cclass1;
    CIMConstObject cobj11 = instance;
    CIMConstObject cobj12 = cinstance;

    assert(cobj1.getClassName() == "MyClass");
    assert(cobj1.getPath() == CIMReference("//localhost/root/cimv2:MyClass"));

    // clone the instance object
    CIMObject cloneObj = cobj4.clone();
    CIMConstObject ccloneObj = cloneObj;
    assert(ccloneObj.identical(cobj4) == true);
    assert(ccloneObj.identical(cobj2) == false);

    if (verbose)
    {
        ccloneObj.print(cout);

        Array<Sint8> xmlOut;
        ccloneObj.toXml(xmlOut);
    }

    // Test qualifiers
    assert(ccloneObj.getQualifierCount() == 2);
    assert(ccloneObj.findQualifier("Key") != PEG_NOT_FOUND);
    assert(ccloneObj.findQualifier("Description") != PEG_NOT_FOUND);
    assert(ccloneObj.findQualifier("q1") == PEG_NOT_FOUND);

    Uint32 posQualifier;
    posQualifier = ccloneObj.findQualifier("Key");
    assert(posQualifier != PEGASUS_NOT_FOUND);
    assert(posQualifier < ccloneObj.getQualifierCount());

    try
    {
        ccloneObj.getQualifier(posQualifier);
    }
    catch(OutOfBounds& e)
    {
        if(verbose)
            cout << "Exception: " << e.getMessage() << endl;
    }

    // Test properties
    assert(ccloneObj.getPropertyCount() == 2);
    assert(ccloneObj.findProperty("count") != PEG_NOT_FOUND);
    assert(ccloneObj.findProperty("message") != PEG_NOT_FOUND);
    assert(ccloneObj.findProperty("ratio") == PEG_NOT_FOUND);

    Uint32 posProperty;
    posProperty = ccloneObj.findProperty("message");
    try
    {
        ccloneObj.getProperty(posProperty);
    }
    catch(OutOfBounds& e)
    {
        if(verbose)
            cout << "Exception: " << e.getMessage() << endl;
    }
}

//*********************************************************************
//  CIMObjectWithPath tests
//
//  The CIMObjectWithPath encapsulates a CIMReference and a CIMObject.
//*********************************************************************
void test03()
{
    CIMClass class1("//localhost/root/cimv2:MyClass");
    CIMReference ref1("//localhost/root/cimv2:MyClass");

    CIMObjectWithPath obj1;
    CIMObjectWithPath obj2(ref1, class1);
    CIMObjectWithPath obj3(obj2);
    CIMObjectWithPath obj4 = obj3;

    CIMClass class2("//localhost/root/cimv2:YourClass");
    CIMReference ref2("//localhost/root/cimv2:YourClass");

    obj3.set(ref2, class2);
    CIMReference ref3 = obj3.getReference();
   
    CIMObject myObj = obj3.getObject();

    if(verbose)
    {
        Array<Sint8> xmlOut;
        myObj.toXml(xmlOut);
    }
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
    }

    cout << argv[0] << " +++++ passed all tests" << endl;
    return 0;
}
 
