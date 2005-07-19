//%2005////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001, 2002 BMC Software; Hewlett-Packard Development
// Company, L.P.; IBM Corp.; The Open Group; Tivoli Systems.
// Copyright (c) 2003 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation, The Open Group.
// Copyright (c) 2004 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation; VERITAS Software Corporation; The Open Group.
// Copyright (c) 2005 Hewlett-Packard Development Company, L.P.; IBM Corp.;
// EMC Corporation; VERITAS Software Corporation; The Open Group.
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
// Modified By: Roger Kumpf (roger_kumpf@hp.com)
//              Carol Ann Krug Graves, Hewlett-Packard Company
//                  (carolann_graves@hp.com)
//              Karl Schopmeyer - Add tests for getclass options
//                      enumerateinstances, etc.
//
//%/////////////////////////////////////////////////////////////////////////////

#include <Pegasus/Common/Config.h>
#include <cassert>
#include <Pegasus/Repository/CIMRepository.h>
#include <Pegasus/Common/XmlWriter.h>
#include <Pegasus/Common/MofWriter.h>
#include <Pegasus/Common/CIMPropertyList.h>

PEGASUS_USING_PEGASUS;
PEGASUS_USING_STD;
static char * verbose;

String repositoryRoot;

void TestNameSpaces(CIMRepository_Mode mode)
{
  CIMRepository r (repositoryRoot, mode);

    r.createNameSpace(CIMNamespaceName ("namespace0"));
    r.createNameSpace(CIMNamespaceName ("namespace1"));
    r.createNameSpace(CIMNamespaceName ("namespace2"));
    r.createNameSpace(CIMNamespaceName ("namespace2/subnamespace"));
    r.createNameSpace(CIMNamespaceName ("namespace2/subnamespace/subsubnamespace"));

    Array<CIMNamespaceName> nameSpaces;
    nameSpaces = r.enumerateNameSpaces();
    BubbleSort(nameSpaces);

    assert(nameSpaces.size() == 6);
    assert(nameSpaces[0] == CIMNamespaceName ("namespace0"));
    assert(nameSpaces[1] == CIMNamespaceName ("namespace1"));
    assert(nameSpaces[2] == CIMNamespaceName ("namespace2"));
    assert(nameSpaces[3] == CIMNamespaceName ("namespace2/subnamespace"));
    assert(nameSpaces[4] == CIMNamespaceName ("namespace2/subnamespace/subsubnamespace"));
    assert(nameSpaces[5] == CIMNamespaceName ("root"));

    r.deleteNameSpace(CIMNamespaceName ("namespace0"));
    r.deleteNameSpace(CIMNamespaceName ("namespace1"));

    nameSpaces = r.enumerateNameSpaces();
    assert(nameSpaces.size() == 4);
    BubbleSort(nameSpaces);
    assert(nameSpaces[0] == CIMNamespaceName ("namespace2"));
    assert(nameSpaces[1] == CIMNamespaceName ("namespace2/subnamespace"));
    assert(nameSpaces[2] == CIMNamespaceName ("namespace2/subnamespace/subsubnamespace"));
    assert(nameSpaces[3] == CIMNamespaceName ("root"));

    r.deleteNameSpace(CIMNamespaceName ("namespace2"));
    r.deleteNameSpace(CIMNamespaceName ("namespace2/subnamespace"));
    r.deleteNameSpace(CIMNamespaceName ("namespace2/subnamespace/subsubnamespace"));
    nameSpaces = r.enumerateNameSpaces();
    assert(nameSpaces.size() == 1);
    assert(nameSpaces[0] == CIMNamespaceName ("root"));
}



void TestCreateClass(CIMRepository_Mode mode)
{
  //
  // -- Create repository and "xyz" namespace:
  //
  CIMRepository r (repositoryRoot, mode);
    const CIMNamespaceName NS = CIMNamespaceName ("TestCreateClass");

    try
    {
	r.createNameSpace(NS);
    }
    catch (AlreadyExistsException&)
    {
	// Ignore this!
    }

    //
    // -- Declare the key qualifier:
    //
    r.setQualifier(NS, CIMQualifierDecl(CIMName ("key"),true,
				   CIMScope::PROPERTY));
    r.setQualifier(NS, CIMQualifierDecl(CIMName ("description"),String(),
				   (CIMScope::PROPERTY + CIMScope::CLASS)));
    r.setQualifier(NS, CIMQualifierDecl(CIMName ("junk"),String(),
				   (CIMScope::PROPERTY + CIMScope::CLASS)));

    // -- Construct new class:"*REMOVED*"
    //	CIMQualifier d(CIMName("description"), String("Test info in SuperClass"));
    // Updated test to ensure it works with enabled PEGASUS_REMOVE_DESCRIPTIONS
    // as well as not enabled.

    CIMQualifier d(CIMName("description"), String("*REMOVED*"));
    CIMClass c1(CIMName ("SuperClass"));
    c1.addQualifier(d);
    c1.addProperty(CIMProperty(CIMName ("key"), Uint32(0))
		   .addQualifier(CIMQualifier(CIMName ("key"), true)));

    c1.addProperty(CIMProperty(CIMName ("ratio"), Real32(1.5)));
    c1.addProperty(CIMProperty(CIMName ("message"), String("Hello World")));

    //
    //-- Create the class (get it back and compare):
    //
    r.createClass(NS, c1);
    CIMConstClass cc1;
    cc1 = r.getClass(NS, CIMName ("SuperClass"),true,true, true);
    assert(c1.identical(cc1));
    assert(cc1.identical(c1));

    // -- Now create a sub class (get it back and compare):
    // c22 has one additional property than c1 (junk)
    //

    CIMClass c2(CIMName ("SubClass"), CIMName ("SuperClass"));

    //
    // Add new qualifier that will be local
    //
    CIMQualifier j(CIMName("junk"), String("TestQualifier"));
    c2.addQualifier(j);

    c2.addProperty(CIMProperty(CIMName ("junk"), Real32(66.66)));
    r.createClass(NS, c2);
    CIMConstClass cc2;
    cc2 = r.getClass(NS, CIMName ("SubClass"), false, true, true);
    //XmlWriter::printClassElement(c2);
    //XmlWriter::printClassElement(cc2);
    
    assert(c2.identical(cc2));
    assert(cc2.identical(c2));

    //
    // -- Modify "SubClass" (add new property)
    //
    c2.addProperty(CIMProperty(CIMName ("newProperty"), Uint32(888)));
    r.modifyClass(NS, c2);
    cc2 = r.getClass(NS, CIMName ("SubClass"), false, true, true);
    assert(c2.identical(cc2));
    assert(cc2.identical(c2));
    // should test for this new property on subclass also.

    //
    // -- Enumerate the class names: expect "SuperClass", "SubClass"
    //
    Array<CIMName> classNames = r.enumerateClassNames(NS, CIMName (), true);
    if (verbose)
      {
	for (Uint32 i = 0 ; i < classNames.size(); i++)
	  {
	    cout << classNames[i].getString();
	  }
      }
    BubbleSort(classNames);
    assert(classNames.size() == 2);
    assert(classNames[1] == CIMName ("SuperClass"));
    assert(classNames[0] == CIMName ("SubClass"));

    //
    // Test the getClass operation options, localonly,
    //		includeQualifiers, includeClassOrigin, propertyList
    //

    // test: localonly true, includequalifiers true, classorigin true
    cc2 = r.getClass(NS, CIMName ("SubClass"), true, true, true);
    if (verbose)
      {
	XmlWriter::printClassElement(c1);
	XmlWriter::printClassElement(cc2);
      }

    assert(cc2.findProperty("ratio") == PEG_NOT_FOUND);
    assert(cc2.findProperty("message") == PEG_NOT_FOUND);
    assert(cc2.findProperty("junk") != PEG_NOT_FOUND);

    //
    // Test to assure that propagated class qualifier removed and
    // local one not removed
    // The following test does not work because propagated, etc. not set.
    //assert (cc2.findQualifier("Description") == PEG_NOT_FOUND);
    assert (cc2.findQualifier("junk") != PEG_NOT_FOUND);

    // test for qualifier on the junk property.
    // ATTN: TODO

    //
    // test localonly == false
    // (localonly false, includequalifiers true, classorigin true)
    //
    cc2 = r.getClass(NS, CIMName ("SubClass"), false, true, true);
    assert(cc2.findProperty("ratio") != PEG_NOT_FOUND);
    assert(cc2.findProperty("message") != PEG_NOT_FOUND);

    //
    // test includeQualifiers set true
    // (localonly true, includequalifiers true, classorigin true)
    //
    cc2 = r.getClass(NS, CIMName ("SuperClass"), true, true, true);
    assert(cc2.getQualifierCount() != 0);

    //
    // test includeQualifiers set false
    // (localonly true, includequalifiers false, classorigin true)
    //
    cc2 = r.getClass(NS, CIMName ("SuperClass"), true, false, true);
    assert(cc2.getQualifierCount() == 0);

    for (Uint32 i = 0; i < cc2.getPropertyCount(); i++)
      {
	CIMConstProperty p = cc2.getProperty(i);
	assert(p.getQualifierCount() == 0);
      }
    for (Uint32 i = 0; i < cc2.getMethodCount(); i++)
      {
	CIMConstMethod m = cc2.getMethod(i);
	assert(m.getQualifierCount() == 0);
      }

    //	
    // Test for Class origin set true
    // (localonly false, includequalifiers true, classorigin true)
    //
    cc2 = r.getClass(NS, CIMName ("SubClass"), false, true, true);
    CIMConstProperty p;
    Uint32 pos  =  cc2.findProperty("ratio");
    assert(pos != PEG_NOT_FOUND);
    p = cc2.getProperty(pos);
    assert(p.getClassOrigin() == CIMName("SuperClass"));

    //
    // Test for Class origin set false. Should return null CIMName.
    // (localonly false, includequalifiers true, classorigin false)
    //
    cc2 = r.getClass(NS, CIMName ("SubClass"), false, true, false);
    CIMConstProperty p1;
    Uint32 pos1  =  cc2.findProperty("ratio");
    assert(pos1 != PEG_NOT_FOUND);
    p1 = cc2.getProperty(pos);
    assert(p1.getClassOrigin() == CIMName());
	

    //
    // Test for propertylist set
    //

    //
    // Test with empty property in list.
    //
    Array<CIMName> pls_empty;
    CIMPropertyList pl(pls_empty); 
    cc2 = r.getClass(NS, CIMName ("SuperClass"), false, true, true, pl);
    assert(cc2.findProperty("ratio") == PEG_NOT_FOUND);
    assert(cc2.findProperty("message") == PEG_NOT_FOUND);
    assert(cc2.getPropertyCount() == 0);

    //
    // Test with one property in list.
    //
    Array<CIMName> pls;
    pls.append(CIMName("ratio"));
    pl.clear();
    pl.set(pls);
    cc2 = r.getClass(NS, CIMName ("SuperClass"), false, true, true, pl);
    assert(cc2.findProperty("ratio") != PEG_NOT_FOUND);
    assert(cc2.findProperty("message") == PEG_NOT_FOUND);
    assert(cc2.getPropertyCount() == 1);

    //
    // retest with two entries in the list.
    //
    pls.append(CIMName("message"));
    pl.clear();
    pl.set(pls);
    cc2 = r.getClass(NS, CIMName ("SuperClass"), false, true, true, pl);
    assert(cc2.findProperty("ratio") != PEG_NOT_FOUND);
    assert(cc2.findProperty("message") != PEG_NOT_FOUND);
    assert(cc2.getPropertyCount() == 2);

    //
    // Test with an invalid property in the list. It should be ignored 
    // and the results should be identical to the previous.
    //
    pls.append(CIMName("herroyalhighnessofyork"));
    pl.clear();
    pl.set(pls);
    cc2 = r.getClass(NS, CIMName ("SuperClass"), false, true, true, pl);
    assert(cc2.findProperty("ratio") != PEG_NOT_FOUND);
    assert(cc2.findProperty("message") != PEG_NOT_FOUND);
    assert(cc2.getPropertyCount() == 2);

	
    //
    // -- Create an instance of each class:
    //
    CIMInstance inst0(CIMName ("SuperClass"));
    inst0.addProperty(CIMProperty(CIMName ("key"), Uint32(111)));
    r.createInstance(NS, inst0);

    CIMInstance inst1(CIMName ("SubClass"));
    inst1.addProperty(CIMProperty(CIMName ("key"), Uint32(222)));
    r.createInstance(NS, inst1);

    //
    // -- Enumerate instances names:
    //
    Array<CIMObjectPath> instanceNames = 
	r.enumerateInstanceNames(NS, CIMName ("SuperClass"));

    assert(instanceNames.size() == 2);

    assert(
	instanceNames[0].toString() == "SuperClass.key=111" ||
	instanceNames[0].toString() == "SubClass.key=222");

    assert(
	instanceNames[1].toString() == "SuperClass.key=111" ||
	instanceNames[1].toString() == "SubClass.key=222");

    inst0.setPath (CIMObjectPath ("SuperClass.key=111"));
    inst1.setPath (CIMObjectPath ("SubClass.key=222"));


    //
    // -- Enumerate instances:
    //
    Array<CIMInstance> namedInstances = r.enumerateInstances(NS, 
        CIMName ("SuperClass"),true, false, true, true);

    assert(namedInstances.size() == 2);

    //XmlWriter::printInstanceElement(namedInstances[0], cout);
    //XmlWriter::printInstanceElement(inst0, cout);
    //XmlWriter::printInstanceElement(inst1, cout);
    assert(
	namedInstances[0].identical(inst0) ||
	namedInstances[0].identical(inst1));

    assert(
	namedInstances[1].identical(inst0) ||
	namedInstances[1].identical(inst1));

    //   
    // Repeat the above tests for the enumerateInstancesFor Class function
    //
    namedInstances = r.enumerateInstancesForClass(NS, 
        CIMName ("SuperClass"),true, false, true, true);

    assert(namedInstances.size() == 1);

    //XmlWriter::printInstanceElement(namedInstances[0], cout);
    //XmlWriter::printInstanceElement(inst0, cout);
    //XmlWriter::printInstanceElement(inst1, cout);
    assert( namedInstances[0].identical(inst0));

    namedInstances = r.enumerateInstancesForClass(NS, 
        CIMName ("SubClass"),true, false, true, true);

    assert(namedInstances.size() == 1);

    assert(namedInstances[0].identical(inst1));
    
#ifdef PEGASUS_ENABLE_REPOSITORY_INSTANCE_FILTER
    //
    // Test enumerating with classOrigin false
    //

    namedInstances = r.enumerateInstances(NS, 
        CIMName ("SuperClass"),true, false, true, false);

    assert(namedInstances.size() == 2);

    for (Uint32 i = 0 ; i < namedInstances.size() ; i++)
    {
        // Check all properties for classorigin
        for (Uint32 j = 0 ; j < namedInstances[i].getPropertyCount() ; j++)
        {
            CIMProperty p = namedInstances[i].getProperty(j);
        }
        
    }

    //
    // Repeat the above for enumerateinstancesForClass
    //
    namedInstances = r.enumerateInstancesForClass(NS, 
        CIMName ("SuperClass"),true, false, true, false, false);

    assert(namedInstances.size() == 1);

    for (Uint32 i = 0 ; i < namedInstances.size() ; i++)
    {
        // Check all properties for classorigin
        for (Uint32 j = 0 ; j < namedInstances[i].getPropertyCount() ; j++)
        {
            CIMProperty p = namedInstances[i].getProperty(j);
        }
    }

    //
    // Test for qualifier removal from enumerateinstances
    //
    namedInstances = r.enumerateInstances(NS, 
        CIMName ("SuperClass"),true, false, false, false);

    assert(namedInstances.size() == 2);

    for (Uint32 i = 0 ; i < namedInstances.size() ; i++)
    {
        assert(namedInstances[i].getQualifierCount() == 0);
        // Check all properties for qualifiers
        for (Uint32 j = 0 ; j < namedInstances[i].getPropertyCount() ; j++)
        {
            CIMProperty p = namedInstances[i].getProperty(j);
            assert(p.getQualifierCount() == 0);
        }
    }

    //
    // Repeat the above for the enumerateinstancesFor Class
    //
    namedInstances = r.enumerateInstancesForClass(NS, 
        CIMName ("SuperClass"),true, false, false, false, false);

    assert(namedInstances.size() == 1);

    for (Uint32 i = 0 ; i < namedInstances.size() ; i++)
    {
        assert(namedInstances[i].getQualifierCount() == 0);
        // Check all properties for qualifiers
        for (Uint32 j = 0 ; j < namedInstances[i].getPropertyCount() ; j++)
        {
            CIMProperty p = namedInstances[i].getProperty(j);
            assert(p.getQualifierCount() == 0);
        }
        
    }

    // *******************************************************************
    //
    // Test for property filtering
    //
    // *******************************************************************

    //
    // test with empty property list
    //
    CIMPropertyList pl1(pls_empty);

    // deepInheritance=true,localOnly=false,
    // includeQualifiers=false,includeClassOrigin=false,
    namedInstances = r.enumerateInstances(NS,
        CIMName ("SuperClass"),true, false, false, false, pl1);

    assert(namedInstances.size() == 2);

    for (Uint32 i = 0 ; i < namedInstances.size() ; i++)
    {
        // Check all properties for qualifiers
        for (Uint32 j = 0 ; j < namedInstances[i].getPropertyCount() ; j++)
	{
            assert(namedInstances[i].findProperty("key") == PEG_NOT_FOUND);
            assert(namedInstances[i].findProperty("ratio") == PEG_NOT_FOUND);
	    assert(namedInstances[i].findProperty("message") == PEG_NOT_FOUND);
	    assert(namedInstances[i].getPropertyCount() == 0);
        }
    }


    //
    // test with property list with property "ratio"
    //
    Array<CIMName> pls1;
    pls1.append(CIMName("ratio"));
    pl1.clear();
    pl1.set(pls1);

    namedInstances = r.enumerateInstances(NS,
        CIMName ("SuperClass"),true, false, false, false, pl1);

    assert(namedInstances.size() == 2);

    for (Uint32 i = 0 ; i < namedInstances.size() ; i++)
    {
        // Check all properties for qualifiers
        for (Uint32 j = 0 ; j < namedInstances[i].getPropertyCount() ; j++)
	{
            assert(namedInstances[i].findProperty("key") == PEG_NOT_FOUND);
            assert(namedInstances[i].findProperty("ratio") != PEG_NOT_FOUND);
	    assert(namedInstances[i].findProperty("message") == PEG_NOT_FOUND);
	    assert(namedInstances[i].getPropertyCount() == 1);
        }
    }


    //
    // retest with two entries in the list.
    //
    pls1.append(CIMName("message"));
    pl1.clear();
    pl1.set(pls1);
    // deepInheritance=true,localOnly=false,
    // includeQualifiers=false,includeClassOrigin=false,
    namedInstances = r.enumerateInstances(NS,
        CIMName ("SuperClass"),true, false, false, false, pl1);

    assert(namedInstances.size() == 2);

    for (Uint32 i = 0 ; i < namedInstances.size() ; i++)
    {
        // Check all properties for qualifiers
        for (Uint32 j = 0 ; j < namedInstances[i].getPropertyCount() ; j++)
	{
            assert(namedInstances[i].findProperty("key") == PEG_NOT_FOUND);
            assert(namedInstances[i].findProperty("ratio") != PEG_NOT_FOUND);
	    assert(namedInstances[i].findProperty("message") != PEG_NOT_FOUND);
	    assert(namedInstances[i].getPropertyCount() == 2);
        }
    }


    //
    // retest with two entries in the list and localOnly=true.
    // results should be same as with LocalOnly=false since it
    // has been deprecated.
    //
    pls1.append(CIMName("message"));
    pl1.clear();
    pl1.set(pls1);
    // deepInheritance=true,localOnly=true,
    // includeQualifiers=false,includeClassOrigin=false,
    namedInstances = r.enumerateInstances(NS,
        CIMName ("SuperClass"),true, true, false, false, pl1);

    assert(namedInstances.size() == 2);

    for (Uint32 i = 0 ; i < namedInstances.size() ; i++)
    {
        // Check all properties for qualifiers
        for (Uint32 j = 0 ; j < namedInstances[i].getPropertyCount() ; j++)
	{
            assert(namedInstances[i].findProperty("key") == PEG_NOT_FOUND);
            assert(namedInstances[i].findProperty("ratio") != PEG_NOT_FOUND);
	    assert(namedInstances[i].findProperty("message") != PEG_NOT_FOUND);
	    assert(namedInstances[i].getPropertyCount() == 2);
        }
    }


    //
    // Test with an invalid property in the list. It should be ignored 
    // and the results should be identical to the previous.
    //
    pls1.append(CIMName("herroyalhighnessofyork"));
    pl1.clear();
    pl1.set(pls1);

    namedInstances = r.enumerateInstances(NS,
        CIMName ("SuperClass"),true, false, false, false, pl1);

    assert(namedInstances.size() == 2);

    for (Uint32 i = 0 ; i < namedInstances.size() ; i++)
    {
        // Check all properties for qualifiers
        for (Uint32 j = 0 ; j < namedInstances[i].getPropertyCount() ; j++)
	{
            assert(namedInstances[i].findProperty("key") == PEG_NOT_FOUND);
            assert(namedInstances[i].findProperty("ratio") != PEG_NOT_FOUND);
	    assert(namedInstances[i].findProperty("message") != PEG_NOT_FOUND);
	    assert(namedInstances[i].getPropertyCount() == 2);
        }
    }


#endif // PEGASUS_ENABLE_REPOSITORY_INSTANCE_FILTER 


    //
    // -- Modify one of the instances:
    //

    CIMInstance modifiedInst0(CIMName ("SuperClass"));
    modifiedInst0.addProperty(CIMProperty(CIMName ("key"), Uint32(111)));
    modifiedInst0.addProperty(CIMProperty(CIMName ("message"), String("Goodbye World")));
    modifiedInst0.setPath (instanceNames[0]);
    r.modifyInstance(NS, modifiedInst0);

    //
    // -- Get instance back and see that it is the same as modified one:
    //
    CIMInstance tmpInstance = r.getInstance(NS, CIMObjectPath 
        ("SuperClass.key=111"), false, true, true);
    tmpInstance.setPath (instanceNames[0]);
	//XmlWriter::printInstanceElement(tmpInstance, cout);
	//XmlWriter::printInstanceElement(modifiedInst0, cout);
    
    assert(tmpInstance.identical(modifiedInst0));

    //
    // -- Now modify the "message" property:
    //
    CIMValue messageValue = r.getProperty(NS, CIMObjectPath 
        ("SuperClass.key=111"), CIMName ("message"));
    String message;
    messageValue.get(message);
    assert(message == "Goodbye World");

    r.setProperty(NS, CIMObjectPath ("SuperClass.key=111"), CIMName ("message"), 
        CIMValue(String("Hello World")));

    messageValue = r.getProperty( NS, CIMObjectPath ("SuperClass.key=111"), 
        CIMName ("message"));
    messageValue.get(message);
    assert(message == "Hello World");

    //
    // Future test -  modify key property and attempt to write
    // TODO

    //
    // -- Attempt to modify a key property:
    //
    Boolean failed = false;

    try
    {
	r.setProperty(NS, CIMObjectPath ("SuperClass.key=111"), CIMName ("key"), 
            Uint32(999));
    }
    catch (CIMException& e)
    {
	assert(e.getCode() == CIM_ERR_FAILED);
	failed = true;
    }

    assert(failed);

    //
    // -- Delete the instances:
    //
    r.deleteInstance(NS, CIMObjectPath ("SuperClass.key=111"));
    r.deleteInstance(NS, CIMObjectPath ("SubClass.key=222"));

    //
    // -- Delete the qualifier:
    //
    r.deleteQualifier(NS, CIMName ("key"));
    r.deleteQualifier(NS, CIMName ("description"));
    r.deleteQualifier(NS, CIMName ("junk"));

    //
    // -- Clean up classes:
    //
    r.deleteClass(NS, CIMName ("SubClass"));
    r.deleteClass(NS, CIMName ("SuperClass"));
    r.deleteNameSpace(NS);
}

void TestQualifiers(CIMRepository_Mode mode)
{
  //
  // -- Create repository and "xyz" namespace:
  //
  CIMRepository r (repositoryRoot, mode);

    const CIMNamespaceName NS = CIMNamespaceName ("TestQualifiers");

    try
    {
	r.createNameSpace(NS);
    }
    catch (AlreadyExistsException&)
    {
	// Ignore this!
    }

    //
    // -- Construct a qualifier declaration:
    //
    CIMQualifierDecl q(CIMName ("abstract"), true, CIMScope::CLASS);
    r.setQualifier(NS, q);

    CIMQualifierDecl qq = r.getQualifier(NS, CIMName ("abstract"));

    assert(qq.identical(q));
    assert(q.identical(qq));

    //
    // -- Delete the qualifier:
    //
    r.deleteQualifier(NS, CIMName ("abstract"));

    //
    // -- Delete the namespace:
    //
    r.deleteNameSpace(NS);
}


int main(int argc, char** argv)
{
    verbose = getenv("PEGASUS_TEST_VERBOSE");
    
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
      CIMRepository_Mode mode;
      if (!strcmp(argv[1],"XML") )
	{
	  mode.flag = CIMRepository_Mode::NONE;
	  if (verbose) cout << argv[0]<< ": using XML mode repository" << endl;
	}
      else if (!strcmp(argv[1],"BIN") )
	{
	  mode.flag = CIMRepository_Mode::BIN;
	  if (verbose) cout << argv[0]<< ": using BIN mode repository" << endl;
	}
      else
	{
	  cout << argv[0] << ": invalid argument: " << argv[1] << endl;
	  return 0;
	}

	TestNameSpaces(mode);
	TestCreateClass(mode);
	TestQualifiers(mode);

    }
    catch (Exception& e)
    {
	cout << argv[0] << " " << argv[1] << " " << e.getMessage() << endl;
	exit(1);
    }

    cout << argv[0] << " " << argv[1] << " +++++ passed all tests" << endl;

    return 0;
}
