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
// Modified By:  Jenny Yu (jenny_yu@hp.com)
//               Carol Ann Krug Graves, Hewlett-Packard Company 
//                   (carolann_graves@hp.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#include <cassert>
#include <Pegasus/Common/CIMObject.h>
#include <Pegasus/Common/CIMClass.h>
#include <Pegasus/Common/CIMInstance.h>
#include <Pegasus/Common/XmlWriter.h>

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
    CIMClass cimClass1("MyClass");
    cimClass1.setPath(CIMObjectPath ("//localhost/root/cimv2:MyClass"));

    CIMObject oclass1 = cimClass1;
    CIMObject oclass2(cimClass1);
    CIMObject oclass3(oclass2);
    CIMObject oclass4 = oclass3;

    CIMClass cimClass2 = cimClass1;
    cimClass2 = cimClass1; 
    cimClass2 = CIMClass(oclass1);
    CIMClass cimClass3 = CIMClass(oclass1);

    assert(oclass1.getClassName() == "MyClass");
    assert(oclass1.getPath() == CIMObjectPath("//localhost/root/cimv2:MyClass"));
    assert (oclass1.isClass ());
    assert (!oclass1.isInstance ());
    assert (oclass2.isClass ());
    assert (!oclass2.isInstance ());
    assert (oclass3.isClass ());
    assert (!oclass3.isInstance ());
    assert (oclass4.isClass ());
    assert (!oclass4.isInstance ());

    //
    // Construct from CIMInstance
    //
    CIMInstance cimInstance1("MyClass");
    CIMObject oinstance1 = cimInstance1;
    CIMObject instance2(cimInstance1);

    assert (oinstance1.isInstance ());
    assert (!oinstance1.isClass ());
    assert (instance2.isInstance ());
    assert (!instance2.isClass ());

    // Test qualifiers
    oinstance1.addQualifier(CIMQualifier("Key", true));
    oinstance1.addQualifier(CIMQualifier("Description", String("Just a Test")));
    oinstance1.addQualifier(CIMQualifier("q1", true));
    oinstance1.addQualifier(CIMQualifier("q2", true));

    assert(oinstance1.findQualifier("Key") != PEG_NOT_FOUND);
    assert(oinstance1.findQualifier("Description") != PEG_NOT_FOUND);
    assert(oinstance1.findQualifier("q1") != PEG_NOT_FOUND);
    assert(oinstance1.findQualifier("q2") != PEG_NOT_FOUND);
    assert(oinstance1.findQualifier("q3") == PEG_NOT_FOUND);
    assert(oinstance1.getQualifierCount() == 4);

    assert(oinstance1.findQualifier("q1") != PEG_NOT_FOUND);
    assert(oinstance1.findQualifier("q2") != PEG_NOT_FOUND);
    assert(oinstance1.findQualifier("q3") == PEG_NOT_FOUND);
    assert(oinstance1.findQualifier("q4") == PEG_NOT_FOUND);

    Uint32 posQualifier;
    posQualifier = oinstance1.findQualifier("q1");
    assert(posQualifier != PEG_NOT_FOUND);
    assert(posQualifier < oinstance1.getQualifierCount());

    try
    {
        CIMQualifier q1 = oinstance1.getQualifier(posQualifier);
        assert(!q1.isUninitialized());
        CIMConstQualifier cq1 = oinstance1.getQualifier(posQualifier);
        assert(!cq1.isUninitialized());
    }
    catch(IndexOutOfBoundsException& e)
    {
        if(verbose)
            cout << "Exception: " << e.getMessage() << endl;

    }

    oinstance1.removeQualifier(posQualifier);
    assert(oinstance1.getQualifierCount() == 3);
    assert(oinstance1.findQualifier("q1") == PEG_NOT_FOUND);
    assert(oinstance1.findQualifier("q2") != PEG_NOT_FOUND);

    // Test properties
    oinstance1.addProperty(CIMProperty("message", String("Hello There")));
    oinstance1.addProperty(CIMProperty("count", Uint32(77)));

    assert(oinstance1.findProperty("count") != PEG_NOT_FOUND);
    assert(oinstance1.findProperty("message") != PEG_NOT_FOUND);
    assert(oinstance1.findProperty("ratio") == PEG_NOT_FOUND);

    CIMProperty cp =
       oinstance1.getProperty(oinstance1.findProperty("message"));

    Uint32 posProperty;
    posProperty = oinstance1.findProperty("count");
    oinstance1.removeProperty(posProperty);
    assert(oinstance1.findProperty("count") == PEG_NOT_FOUND);

    assert(oinstance1.getPropertyCount() == 1);

    const CIMObject oinstance2 = oinstance1.clone();
    assert(oinstance2.identical(oinstance1));

    if (verbose)
    {
        Array<Sint8> xmlOut;
        XmlWriter::appendObjectElement(xmlOut, oinstance1);
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
    CIMClass class1("MyClass");
    class1.setPath(CIMObjectPath ("//localhost/root/cimv2:MyClass"));

    //
    // Construct from CIMClass
    //
    
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
    obj2.addQualifier(CIMQualifier("Description", String("Just a Test")));
    obj2.addProperty(CIMProperty("message", String("Hello There")));
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
    assert(cobj1.getPath() == CIMObjectPath("//localhost/root/cimv2:MyClass"));

    // clone the instance object
    CIMObject cloneObj = cobj4.clone();
    CIMConstObject ccloneObj = cloneObj;
    assert(ccloneObj.identical(cobj4) == true);
    assert(ccloneObj.identical(cobj2) == false);

    if (verbose)
    {
        Array<Sint8> xmlOut;
        XmlWriter::appendObjectElement(xmlOut, ccloneObj);
    }

    // Test qualifiers
    assert(ccloneObj.getQualifierCount() == 2);
    assert(ccloneObj.findQualifier("Key") != PEG_NOT_FOUND);
    assert(ccloneObj.findQualifier("Description") != PEG_NOT_FOUND);
    assert(ccloneObj.findQualifier("q1") == PEG_NOT_FOUND);

    Uint32 posQualifier;
    posQualifier = ccloneObj.findQualifier("Key");
    assert(posQualifier != PEG_NOT_FOUND);
    assert(posQualifier < ccloneObj.getQualifierCount());

    try
    {
        ccloneObj.getQualifier(posQualifier);
    }
    catch(IndexOutOfBoundsException& e)
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
    catch(IndexOutOfBoundsException& e)
    {
        if(verbose)
            cout << "Exception: " << e.getMessage() << endl;
    }
}

//*********************************************************************
//  CIMObject setPath tests
//
//*********************************************************************
void test03()
{
    CIMClass class1("MyClass");
    CIMObjectPath ref1("//localhost/root/cimv2:MyClass");
    class1.setPath(ref1);

    CIMObject obj1;
    CIMObject obj2 = CIMObject (class1);
    obj2.setPath (ref1);
    CIMObject obj3(obj2);
    CIMObject obj4 = obj3;

    CIMClass class2("YourClass");
    CIMObjectPath ref2("//localhost/root/cimv2:YourClass");
    class2.setPath(ref2);

    obj3 = CIMObject (class2);
    obj3.setPath (ref2);
    CIMObjectPath ref3 = obj3.getPath ();
   
    CIMObject myObj = obj3;

    if(verbose)
    {
        Array<Sint8> xmlOut;
        XmlWriter::appendObjectElement(xmlOut, myObj);
    }
}

//*********************************************************************
//  CIMObject, CIMClass, CIMInstance tests using setPath and getPath
//*********************************************************************
void test04()
{
    //
    //  Test CIMClass
    //
    CIMClass class1 ("MyClass");
    class1.setPath(CIMObjectPath ("//localhost/root/cimv2:MyClass"));
    CIMProperty prop1;
    prop1 = CIMProperty ("message", CIMValue (CIMTYPE_STRING));
    prop1.addQualifier (CIMQualifier ("Key", true));
    CIMProperty prop2;
    prop2 = CIMProperty ("count", CIMValue (CIMTYPE_UINT32));
    prop2.addQualifier (CIMQualifier ("Key", true));
    class1.addProperty (prop1);
    class1.addProperty (prop2);

    CIMObjectPath cpath = class1.getPath ();
    class1.setPath (CIMObjectPath (class1.getClassName ()));
    CIMObjectPath cpath2 = class1.getPath ();
    class1.setPath (cpath);
    CIMObjectPath cpath3 = class1.getPath ();
    assert (cpath3 == cpath);

    if (verbose)
    {
        cout << "Class object path from getPath: " << cpath << endl;
        cout << "Class object path from getPath after setPath: " << cpath2 
             << endl;
        cout << "Class object path from getPath after second setPath: " 
             << cpath3 << endl;
    }


    //
    //  Test class CIMObject
    //
    CIMObject oclass1 = class1;
    CIMObjectPath ocpath = class1.getPath ();
    class1.setPath (CIMObjectPath (class1.getClassName ()));
    CIMObjectPath ocpath2 = class1.getPath ();
    class1.setPath (ocpath);
    CIMObjectPath ocpath3 = class1.getPath ();
    assert (ocpath3 == ocpath);

    if (verbose)
    {
        cout << "Class object path from getPath: " << ocpath << endl;
        cout << "Class object path from getPath after setPath: " << ocpath2 
             << endl;
        cout << "Class object path from getPath after second setPath: " 
             << ocpath3 << endl;
    }


    //
    //  Test CIMInstance
    //
    CIMInstance instance1 ("MyClass");
    instance1.addProperty (CIMProperty ("message", String("Hello There")));
    instance1.addProperty (CIMProperty ("count", Uint32 (77)));
    CIMObjectPath path = instance1.buildPath (class1);
    CIMObjectPath path2 = instance1.getPath ();
    instance1.setPath (path);
    CIMObjectPath path3 = instance1.getPath ();
    assert (path3 == path);

    if (verbose)
    {
        cout << "Instance object path from buildPath: " << path << endl;
        cout << "Instance object path from getPath: " << path2 << endl;
        cout << "Instance object path from getPath after setPath: " << path3 
             << endl;
    }


    //
    //  Test instance CIMObject
    //
    CIMInstance instance2 ("MyClass");
    instance2.addProperty (CIMProperty ("message", String("Good-bye...")));
    instance2.addProperty (CIMProperty ("count", Uint32 (88)));
    CIMObject oinstance1 = instance2;
    CIMObjectPath opath = instance2.buildPath (class1);
    CIMObjectPath opath1 = oinstance1.getPath ();
    oinstance1.setPath (opath);
    CIMObjectPath opath2 = oinstance1.getPath ();
    assert (opath2 == opath);

    if (verbose)
    {
        cout << "Instance object path from buildPath: " << opath << endl;
        cout << "Instance object path from getPath: " << opath1 << endl;
        cout << "Instance object path from getPath after setPath: " << opath2 
             << endl;
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
       test04();
    }
    catch (Exception& e)
    {
        cout << "Exception: " << e.getMessage() << endl;
    }

    cout << argv[0] << " +++++ passed all tests" << endl;
    return 0;
}
 
