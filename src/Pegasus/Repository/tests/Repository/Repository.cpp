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
//                 (carolann_graves@hp.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#include <Pegasus/Common/Config.h>
#include <cassert>
#include <Pegasus/Repository/CIMRepository.h>

PEGASUS_USING_PEGASUS;
PEGASUS_USING_STD;

String repositoryRoot;

void test01()
{
    CIMRepository r (repositoryRoot);

    const String NAMESPACE = "aa/bb";

    try
    {
	r.createNameSpace(NAMESPACE);
    }
    catch (AlreadyExistsException&)
    {
	// Ignore this!
    }

    CIMClass c(CIMName ("MyClass"));

    r.setQualifier(
	NAMESPACE, CIMQualifierDecl(CIMName ("key"), true, CIMScope::PROPERTY));

    c.addProperty(
	CIMProperty(CIMName ("key"), Uint32(0))
	    .addQualifier(CIMQualifier(CIMName ("key"), true)))
	.addProperty(CIMProperty(CIMName ("ratio"), Real32(1.5)))
	.addProperty(CIMProperty(CIMName ("message"), String("Hello World")));

    r.createClass(NAMESPACE, c);

    CIMConstClass cc;
    cc = r.getClass(CIMNamespaceName ("aa/bb"), CIMName ("MyClass"));

    assert(c.identical(cc));
    assert(cc.identical(c));

    // cc.print();
}

void test02()
{
    //--------------------------------------------------------------------------
    // Create repository:
    //--------------------------------------------------------------------------

    CIMRepository r (repositoryRoot);

    const String NAMESPACE = "aa/bb";
    const CIMName SUPERCLASS = "SuperClass";
    const CIMName SUBCLASS = "SubClass";

    try
    {
	r.createNameSpace(NAMESPACE);
    }
    catch (CIMException& e)
    {
	PEGASUS_ASSERT(e.getCode() == CIM_ERR_ALREADY_EXISTS);
	// Ignore this!
    }

    //--------------------------------------------------------------------------
    // Create Class (SuperClass):
    //--------------------------------------------------------------------------

    CIMClass superClass(SUPERCLASS);

    superClass
	.addProperty(CIMProperty(CIMName ("Last"), String())
	    .addQualifier(CIMQualifier(CIMName ("key"), true)))
	.addProperty(CIMProperty(CIMName ("First"), String())
	    .addQualifier(CIMQualifier(CIMName ("key"), true)))
	.addProperty(CIMProperty(CIMName ("Age"), Uint8(0))
	    .addQualifier(CIMQualifier(CIMName ("key"), true)));

    r.createClass(NAMESPACE, superClass);

    //--------------------------------------------------------------------------
    // Create Class (SubClass):
    //--------------------------------------------------------------------------

    CIMClass subClass(SUBCLASS, SUPERCLASS);
    subClass.addProperty(CIMProperty(CIMName ("Role"), String()));
    r.createClass(NAMESPACE, subClass);

    //--------------------------------------------------------------------------
    // Create Instance (of SubClass):
    //--------------------------------------------------------------------------

    CIMInstance subClassInstance(SUBCLASS);
    subClassInstance.addProperty(CIMProperty(CIMName ("Last"), String("Smith")));
    subClassInstance.addProperty(CIMProperty(CIMName ("First"), String("John")));
    subClassInstance.addProperty(CIMProperty(CIMName ("Age"), Uint8(101)));
    subClassInstance.addProperty(CIMProperty(CIMName ("Role"), String("Taylor")));
    r.createInstance(NAMESPACE, subClassInstance);

    //--------------------------------------------------------------------------
    // Get instance back:
    //--------------------------------------------------------------------------

    CIMObjectPath instanceName1 = subClassInstance.buildPath(subClass);

    CIMObjectPath instanceName2 =
	CIMObjectPath ("SuperClass.first=\"John\",last=\"Smith\",age=101");

    CIMInstance tmp = r.getInstance(NAMESPACE, instanceName2);

    assert(subClassInstance.identical(tmp));

    //--------------------------------------------------------------------------
    // Miscellaneous tests
    //--------------------------------------------------------------------------
    
    try
    {
	r.execQuery("WQL", "myquery");
    }
    catch (CIMException& e)
    {
        // execQuery operation is not supported yet
	PEGASUS_ASSERT(e.getCode() == CIM_ERR_NOT_SUPPORTED);
    }

    try
    {
        // delete a non-empty namespace
	r.deleteNameSpace(NAMESPACE);
    }
    catch (NonEmptyNameSpace& e)
    {
        // expected exception 
    }

//    ATTN:2.0:ENHANCE:DEFERRED:getProviderName() is not supported.
//    String providerName = r.getProviderName();
//    assert (providerName == "repository");

    Array<CIMName> subClassNames;
    r.getSubClassNames(NAMESPACE, SUPERCLASS, true, subClassNames);
    assert(subClassNames.size() == 1);
    assert(subClassNames[0] == SUBCLASS);

    Array<CIMName> superClassNames;
    r.getSuperClassNames(NAMESPACE, SUBCLASS, superClassNames);
    assert(superClassNames.size() == 1);
    assert(superClassNames[0] == SUPERCLASS);
}

void test03()
{
    const char* home = getenv("PEGASUS_HOME");

    if (!home)
    {
	cerr << "PEGASUS_HOME environment variable not set" << endl;
	exit(1);
    }

    String repositoryRoot = home;
    repositoryRoot.append("/repository");
    CIMRepository r(repositoryRoot);

    Array<CIMObjectPath> names = r.associatorNames(
	CIMNamespaceName ("root/cimv2"),
	CIMObjectPath ("X.key=\"John Smith\""));

    for (Uint32 i = 0; i < names.size(); i++)
    {
	cout << "names[i]=[" << names[i].toString() << "]" << endl;
    }
}

int main()
{
    const char* tmpDir = getenv ("PEGASUS_TMP");
    if (tmpDir == NULL)
    {
        repositoryRoot = ".";
    }
    else
    {
        repositoryRoot = tmpDir;
    }
    repositoryRoot.append("/repository");

    try 
    {
	test01();
	test02();
	// test03();
    }
    catch (Exception& e)
    {
	cout << e.getMessage() << endl;
	exit(1);
    }

    cout << "+++++ passed all tests" << endl;

    return 0;
}
