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
// Author: Karl Schopmeyer<k.schopmeyer@opengroup.org>
//
// Modified By: 
// 
//
//%////////////////////////////////////////////////////////////////////////////

/** Function: This CIM Client executable tests the Association provider defined in
 * the providers/testproviders/AssociationTest directory
 * 
 * This code tests associations as provided by the associationtest provider
 * AND the associationtest.mof defined as part of the association test.
 * It tests the following:
 * referencenames, references, associatornames, associators  class request as 
 * defined in the MOF.

 * 
 * */
#define CDEBUG(X) PEGASUS_STD(cout) << "SampleFamilyProvider " << X << PEGASUS_STD(endl)
//#define CDEBUG(X)
//#define DEBUG(X) Logger::put (Logger::DEBUG_LOG, "Linux_ProcessorProvider", Logger::INFORMATION, "$0", X)

// ==========================================================================
// Includes
// ==========================================================================

#include <Pegasus/Client/CIMClient.h>

// ==========================================================================
// Miscellaneous defines
// ==========================================================================

#define NAMESPACE "root/testassoc"
#define HOST      "localhost"
#define PORTNUMBER 5988
#define TIMEOUT   10000                    // timeout value in milliseconds

// ==========================================================================
// Class names.  These values are the names of the classes that
// are common for all of the providers.
// ==========================================================================
static char * verbose;


// ==========================================================================
// Property names.  These values are returned by the provider as
// the property names.
// ==========================================================================

PEGASUS_USING_STD;
PEGASUS_USING_PEGASUS;

CIMClient c;

void errorExit(Exception& e)
{
  cout << "Error: Failed" << endl << e.getMessage() << endl;
  exit(1);
}

// Test a Class by enumerating the instances.
int testClass(const CIMName& className)
{
  Array<CIMObjectPath> refs;

  // =======================================================================
  // enumerateInstanceNames
  // =======================================================================

  cout << "+++++ enumerateInstanceNames(" << className << ") ";
  try
  {
    refs = c.enumerateInstanceNames(NAMESPACE,className);
  }
  catch (Exception& e)
  {
    cout << endl;
    errorExit(e);
  }

  cout << refs.size() << " instances" << endl;
  // if zero instances, not an error, but can't proceed
  if (refs.size() == 0)
  {
    cout << "+++++ test completed early" << endl;
    return 0;
  }
  return 0;
}


// Simply run through the classes and check number of instances
int testClassList(const Array<CIMName>& classList)
{
    int rc;

    for (Uint32 i = 0; i < classList.size() ; i++)
    {
        rc = testClass(classList[i]);
    }
    return 0;
}
int testAssocNames(const CIMObjectPath& objectName,
                   const CIMName& assocClass,
                   const CIMName& resultClass,
                   const String& role,
                   const String& resultRole,
                   Uint32 expectedCount )
{
    //CIMObjectPath instanceName = CIMObjectPath ("Person.name=\"Mike\"");
    CDEBUG ("testReferenceNames " << objectName.toString() << " resultClass " << resultClass << " role " << role); 
    
    Array<CIMObjectPath> result = c.associatorNames(
        	NAMESPACE, 
        	objectName,
            assocClass,
        	resultClass, 
        	role,
            resultRole);
    Array<CIMObject> resultObjects = c.associators(
        	NAMESPACE, 
        	objectName,
            assocClass,
        	resultClass, 
        	role,
            resultClass);

    if (result.size() != resultObjects.size())
    {
        cout << "ERROR, Associator and AssociatorName count returned difference" << endl;
    }
    for (Uint32 i = 0; i < result.size(); i++)
    {
        if (resultObjects[i].getPath().toString() != result[i].toString())
        {
            cout << "Name response Error" << endl;
        }
    }
    if (verbose)
    {
        for (Uint32 i = 0; i < result.size(); i++)
            cout << "[" << result[i].toString() << "]" << endl;
    }

    if (result.size() != expectedCount)
    {
        cout << "AssociatorName Error Object " << objectName.toString() << "Expected count = " << expectedCount << " received " << result.size(); 
        return 1;
    }
    return 0;

}

int testReferences(const CIMObjectPath& objectName, const CIMName& resultClass, 
    const String& role, CIMPropertyList& propertyList, Uint32 expectedCount )
{
    CDEBUG ("testRefrenceNames " << objectName.toString() << " resultClass " << resultClass << " role " << role); 
    Array<CIMObject> result = c.references(
        	NAMESPACE, 
        	objectName, 
        	resultClass, 
        	role,
            false,
            false,
            propertyList);

    if (verbose)
    {
        for (Uint32 i = 0; i < result.size(); i++)
        {
            cout << "[" << result[i].getPath().toString() << "]" << endl;
        }
    }

    if (result.size() != expectedCount)
    {
        cout << "ReferenceName Error Object " << objectName.toString() << "Expected count = " << expectedCount << " received " << result.size(); 
    }

    return 0;
  
}

// Test references and reference names common elements.

int testReferenceNames(const CIMObjectPath& objectName, const CIMName& resultClass, 
    const String& role, Uint32 expectedCount )
{
    
    //CIMObjectPath instanceName = CIMObjectPath ("Person.name=\"Mike\"");
    CDEBUG ("testReferenceNames " << objectName.toString() << " resultClass " << resultClass << " role " << role); 
            
    Array<CIMObjectPath> result = c.referenceNames(
        	NAMESPACE, 
        	objectName, 
        	resultClass, 
        	role);
    Array<CIMObject> resultObjects = c.references(
        	NAMESPACE, 
        	objectName, 
        	resultClass, 
        	role);

    if (result.size() != resultObjects.size())
    {
        cout << "ERROR, Reference and reference Name size difference" << endl;
    }
    for (Uint32 i = 0; i < result.size(); i++)
    {
        if (resultObjects[i].getPath().toString() != result[i].toString())
        {
            cout << "Name response Error" << endl;
        }
    }
    if (verbose)
    {
        for (Uint32 i = 0; i < result.size(); i++)
            cout << "[" << result[i].toString() << "]" << endl;
    }

    if (result.size() != expectedCount)
    {
        cout << "ReferenceName Error Object " << objectName.toString() << "Expected count = " << expectedCount << " received " << result.size(); 
        return 1;
    }
    return 0;
}
#define ASRT(X) X
//#define ASRT(X) assert X

// ===========================================================================
// Main
// Options:
//  Test or show - TBD
// ===========================================================================
int main()
{
  
  //verbose = getenv("PEGASUS_TEST_VERBOSE");
  verbose = "test";  
  cout << "+++++ Testing AssociationTest Provider" << endl;
  
  // Build array of classes
  Array<CIMName> Classes;
  Classes.append("TST_ClassA");
  Classes.append("TST_ClassB");
  Classes.append("TST_ClassC");
  Classes.append("TST_ClassD");

  Array<CIMName> AssocClasses;
  AssocClasses.append("TST_Assoc1");
  AssocClasses.append("TST_Assoc2");

  // Connect
  try
  {
    c.connect (HOST, PORTNUMBER, String::EMPTY, String::EMPTY);
  }
  catch (Exception& e)
  {
    errorExit(e);
  }

  int rc;
  // Start by confirming the existence of the classes, etc.
  if ((rc = testClassList(Classes)) != 0) return rc;
  if ((rc = testClassList(AssocClasses)) != 0) return rc;
  // Now start the association tests.
  // Reference Names Test

  // Class A Refrence Names Test
  ASRT(testReferenceNames(CIMObjectPath("TST_ClassA"),CIMName(), "", 2));
  ASRT(testReferenceNames(CIMObjectPath("TST_ClassA"),CIMName(), "to", 2));
  ASRT(testReferenceNames(CIMObjectPath("TST_ClassA"),CIMName(), "from", 2));
  
  ASRT(testReferenceNames(CIMObjectPath("TST_ClassA"),CIMName("TST_Assoc1"), "", 1));
  ASRT(testReferenceNames(CIMObjectPath("TST_ClassA"),CIMName("TST_Assoc1"), "to", 1));
  ASRT(testReferenceNames(CIMObjectPath("TST_ClassA"),CIMName("TST_Assoc1"), "from", 1));
  
  ASRT(testReferenceNames(CIMObjectPath("TST_ClassA"),CIMName("TST_Assoc3"), "", 1));
  ASRT(testReferenceNames(CIMObjectPath("TST_ClassA"),CIMName("TST_Assoc3"), "to", 0));
  ASRT(testReferenceNames(CIMObjectPath("TST_ClassA"),CIMName("TST_Assoc3"), "from", 1));
  
  ASRT(testReferenceNames(CIMObjectPath("TST_ClassA"),CIMName("TST_Assoc5"), "", 0));
  ASRT(testReferenceNames(CIMObjectPath("TST_ClassA"),CIMName("TST_Assoc5"), "to", 0));
  ASRT(testReferenceNames(CIMObjectPath("TST_ClassA"),CIMName("TST_Assoc5"), "from", 0));
  
  // Class B Refrence Names Test
  ASRT(testReferenceNames(CIMObjectPath("TST_ClassB"),CIMName(), "", 4));
  ASRT(testReferenceNames(CIMObjectPath("TST_ClassB"),CIMName(), "to", 2));
  ASRT(testReferenceNames(CIMObjectPath("TST_ClassB"),CIMName(), "from", 4));
  
  ASRT(testReferenceNames(CIMObjectPath("TST_ClassB"),CIMName("TST_Assoc2"), "", 1));
  ASRT(testReferenceNames(CIMObjectPath("TST_ClassB"),CIMName("TST_Assoc2"), "to", 1));
  ASRT(testReferenceNames(CIMObjectPath("TST_ClassB"),CIMName("TST_Assoc2"), "from", 1));
  
  ASRT(testReferenceNames(CIMObjectPath("TST_ClassB"),CIMName("TST_Assoc2"), "", 1));
  ASRT(testReferenceNames(CIMObjectPath("TST_ClassB"),CIMName("TST_Assoc2"), "to", 0));
  ASRT(testReferenceNames(CIMObjectPath("TST_ClassB"),CIMName("TST_Assoc2"), "from", 1));
  
  ASRT(testReferenceNames(CIMObjectPath("TST_ClassB"),CIMName("TST_Assoc5"), "", 1));
  ASRT(testReferenceNames(CIMObjectPath("TST_ClassB"),CIMName("TST_Assoc5"), "to", 1));
  ASRT(testReferenceNames(CIMObjectPath("TST_ClassB"),CIMName("TST_Assoc5"), "from", 0));


  // TODO TestReferences for class c and class d

  // testRefernceName Instances from static store
  ASRT(testReferenceNames(CIMObjectPath("TST_ClassA.name=\"InstanceA1\""),CIMName(), "", 2));
  ASRT(testReferenceNames(CIMObjectPath("TST_ClassA.name=\"InstanceA1\""),CIMName(), "to", 2));
  ASRT(testReferenceNames(CIMObjectPath("TST_ClassA.name=\"InstanceA1\""),CIMName(), "from", 2));
  
  ASRT(testReferenceNames(CIMObjectPath("TST_ClassA.name=\"InstanceA1\""),CIMName("TST_Assoc1"), "", 1));
  ASRT(testReferenceNames(CIMObjectPath("TST_ClassA.name=\"InstanceA1\""),CIMName("TST_Assoc1"), "to", 1));
  ASRT(testReferenceNames(CIMObjectPath("TST_ClassA.name=\"InstanceA1\""),CIMName("TST_Assoc1"), "from", 1));
  
  ASRT(testReferenceNames(CIMObjectPath("TST_ClassA.name=\"InstanceA1\""),CIMName("TST_Assoc3"), "", 1));
  ASRT(testReferenceNames(CIMObjectPath("TST_ClassA.name=\"InstanceA1\""),CIMName("TST_Assoc3"), "to", 0));
  ASRT(testReferenceNames(CIMObjectPath("TST_ClassA.name=\"InstanceA1\""),CIMName("TST_Assoc3"), "from", 1));
  
  ASRT(testReferenceNames(CIMObjectPath("TST_ClassA.name=\"InstanceA1\""),CIMName("TST_Assoc5"), "", 0));
  ASRT(testReferenceNames(CIMObjectPath("TST_ClassA.name=\"InstanceA1\""),CIMName("TST_Assoc5"), "to", 0));
  ASRT(testReferenceNames(CIMObjectPath("TST_ClassA.name=\"InstanceA1\""),CIMName("TST_Assoc5"), "from", 0));

  // Lets make the previous a common test between ref and ref names.

  // References
  // Limited test since we learned most in the previous test of reference names
  CIMPropertyList NullPL;
  NullPL.clear();

  CIMPropertyList emptyPL;

  CIMPropertyList fullPL;
  Array<CIMName> fullPLinput;
  fullPLinput.append("name");
  fullPL.set(fullPLinput);

  
  CIMPropertyList errorPL ;
  Array<CIMName> errorPLinput;
  errorPLinput.append("junk");
  errorPL.set(errorPLinput);

  ASRT(testReferences(CIMObjectPath("TST_ClassA"),CIMName(), "", emptyPL,2));
  
  ASRT(testReferences(CIMObjectPath("TST_ClassA.name=\"InstanceA1\""),CIMName(), "", fullPL,2));

  // Testing associators and and associator names functions.

  ASRT(testAssocNames(CIMObjectPath("TST_ClassA"),CIMName(),CIMName(),"", "", 3));

  cout << "+++++ passed all tests" << endl;
  return 0;
}

