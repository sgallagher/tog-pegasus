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
// Modified By: Karl Schopmeyer (k.schopmeyer@opengroup.org)
//         Mike Day (mdday@us.ibm.com)
//         Jenny Yu (jenny_yu@hp.com)
//         Bapu Patil ( bapu_patil@hp.com )
//         Warren Otsuka (warren_otsuka@hp.com)
//         Nag Boranna(nagaraja_boranna@hp.com)
//         Carol Ann Krug Graves, Hewlett-Packard Company
//               (carolann_graves@hp.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/Constants.h>
#include <Pegasus/Common/PegasusVersion.h>
#include <cassert>
#include <Pegasus/Common/TLS.h>
#include <Pegasus/Client/CIMClient.h>
#include <Pegasus/Common/CIMName.h>
#include <Pegasus/Common/OptionManager.h>
#include <Pegasus/Common/FileSystem.h>
#include <Pegasus/Common/Stopwatch.h>
#include <Pegasus/Common/Exception.h>
#include <Pegasus/Common/XmlWriter.h>
#if !defined(PEGASUS_OS_ZOS) && ! defined(PEGASUS_OS_HPUX) && !defined(PEGASUS_OS_LINUX)
#include <slp/slp.h>
#endif

PEGASUS_USING_PEGASUS;
PEGASUS_USING_STD;

CIMNamespaceName globalNamespace = CIMNamespaceName ("root/cimv2");
static const CIMNamespaceName __NAMESPACE_NAMESPACE = CIMNamespaceName ("root");

/** ErrorExit - Print out the error message as an
    and get out.
    @param - Text for error message
    @return - None, Terminates the program
    @execption - This function terminates the program
    ATTN: Should write to stderr
*/
void ErrorExit(const String& message)
{

    cout << message << endl;
    exit(1);
}


/* Status display of the various steps.  Shows message of function and
time to execute.  Grow this to a class so we have start and stop and time
display with success/failure for each function.
*/
static void testStart(const String& message)
{
    cout << "++++ " << message << " ++++" << endl;

}

static void testEnd(const double elapsedTime)
{
    cout << "In " << elapsedTime << " Seconds\n\n";
}
/*****************************************************************
//    Testing Namespace operations
******************************************************************/

static void TestNameSpaceOperations(CIMClient& client, Boolean activeTest,
		 		 Boolean verboseTest)
{
    // Enumerate NameSpaces using the old technique
    CIMName className = CIMName ("__Namespace");

    Array<CIMObjectPath> instanceNames;

    // Call enumerate Instances for the namespace list
    try
    {
		 instanceNames = client.enumerateInstanceNames(
		     __NAMESPACE_NAMESPACE, className);

		 cout << "Found " << instanceNames.size() << " Namespaces" << endl;

		 if (verboseTest)
		 {
		     // Array<String> tmpInstanceNames;

		     for (Uint32 i = 0; i < instanceNames.size(); i++)
		 		 cout << instanceNames[i].toString() << endl;
		 }
	 }
    catch(Exception& e)
    {
    	PEGASUS_STD(cerr) << "Exception Namespace Enumeration: " 
            << e.getMessage() << PEGASUS_STD(endl);
        return;
    }
    
    // If conducting active test, try to create and delete a namespace.
    if(activeTest)
    {
    	if(verboseTest)
    	    cout << "Conducting Create / Delete namespace test " << endl;
    
    	// Build the instance name for __namespace
    	CIMNamespaceName testNamespaceName = CIMNamespaceName ("karl/junk");
    	String instanceName = className.getString();
    	instanceName.append( ".Name=\"");
    	instanceName.append(testNamespaceName.getString());
    	instanceName.append("\"");
    	if(verboseTest)
    	{
    	    cout << "Creating " << instanceName << endl;
    	}
    	//if(testNamespace)      Add the test for existance code here ATTN
    	//{
    	//    instanceNames
    	//}
    	// Create the new instance
    
        CIMObjectPath newInstanceName;
    	try
    	{
    	    // Build the new instance
    	    CIMInstance newInstance(instanceName);
    	    newInstance.addProperty(CIMProperty(CIMName ("name"), 
                testNamespaceName.getString()));
    	    newInstanceName = client.createInstance(__NAMESPACE_NAMESPACE, newInstance);
    	}
    	catch(CIMException& e)
    	{
    	     if (e.getCode() == CIM_ERR_ALREADY_EXISTS)
             {
    	           newInstanceName = CIMObjectPath(instanceName);
    	     }
    	     else
             {
    	          PEGASUS_STD(cerr) << "CIMException NameSpace Creation: "
                      << e.getMessage() << " Creating " << instanceName
                      << PEGASUS_STD(endl);
    	          return;
    	     }
    	}
    	catch(Exception& e)
    	{
    	    PEGASUS_STD(cerr) << "Exception NameSpace Creation: " << e.getMessage() << PEGASUS_STD(endl);
    	    return;
    	}
            // Now try getting the instance.
    	try
    	{
    	    client.getInstance(__NAMESPACE_NAMESPACE, newInstanceName);
    	}
    	catch(Exception& e)
    	{
    	     PEGASUS_STD(cerr) << "Exception NameSpace getInstance: "
    			<< e.getMessage() << " Retrieving " << instanceName
    		    << PEGASUS_STD(endl);
    	     return;
    	}
    
    	// Now delete it
    
    	try
    	{
    	    CIMObjectPath myReference(instanceName);
    	    cout << "Deleting namespace = " << instanceName << endl;
    	    client.deleteInstance(__NAMESPACE_NAMESPACE, myReference);
    	}
    	catch(Exception& e)
    	{
    	     PEGASUS_STD(cerr) << "Exception NameSpace Deletion: "
    			<< e.getMessage() << " Deleting " << instanceName
    		        << PEGASUS_STD(endl);
    	     return;
    	}
    }
}


static void TestEnumerateClassNames (CIMClient& client, Boolean activeTest,
		 		 Boolean verboseTest)
{
    try
    {
		 Boolean deepInheritance = true;
		 CIMName className;
		 Array<CIMName> classNames = client.enumerateClassNames(
		     globalNamespace, className, deepInheritance);

		 if (verboseTest)
		 {
		     for (Uint32 i = 0, n = classNames.size(); i < n; i++)
		 		 cout << classNames[i] << endl;
		 }

		 cout << classNames.size() << " ClassNames" << endl;
    }
    catch(Exception& e)
    {
		 cout << "Error NameSpace Enumeration:" << endl;
		 cout << e.getMessage() << endl;
    }

}

static void TestGetClass(CIMClient& client, Boolean activeTest,
		     Boolean verboseTest)
{
    CIMClass c = client.getClass(
        globalNamespace, CIMName ("CIM_ComputerSystem"), false, false, true);

    XmlWriter::printClassElement(c);
}

/* This is both an active and passive class test and so uses the
    activetest variable
*/
static void TestClassOperations(CIMClient& client, Boolean ActiveTest,
		 		 		     Boolean verboseTest) {

    // Name of Class to use in create/delete test
    CIMName testClass = CIMName ("PEG_TestSubClass");
    // NOte that this creates a subclass of ManagedElement so will fail if
    // if managedelement not intalled.

    //Test for class already existing
    Array<CIMName> classNames = client.enumerateClassNames(
		 globalNamespace, CIMName ("CIM_ManagedElement"), false);
    cout << "KSTEST verboseTest = " << verboseTest << " ActiveTest ' " << ActiveTest << endl;
    if (ActiveTest)
    {
        if (verboseTest)
        {
            cout << "Test to create, modify and delete test class " << testClass << endl;
        }
        for (Uint32 i = 0; i < classNames.size(); i++)
        {
    		 if (classNames[i].equal(testClass))
    		     try
    		     {
    		     client.deleteClass(globalNamespace, testClass);
    		 	 }
                catch (CIMException& e)
                {
    					cout << "TestClass " << testClass << " delete failed " << e.getMessage() << endl;
    		    }
        }
    
        // CreateClass:
    
        CIMClass c1(testClass, CIMName ("CIM_ManagedElement"));
        c1.addProperty(CIMProperty(CIMName ("count"), Uint32(99)));
        c1.addProperty(CIMProperty(CIMName ("ratio"), Real64(66.66)));
        c1.addProperty(CIMProperty(CIMName ("message"), String("Hello World")));
    
        try
        {
        	client.createClass(globalNamespace, c1);
    	}
    	catch (CIMException& e)
    	{
    	    CIMStatusCode code = e.getCode();
    	    if (code == CIM_ERR_ALREADY_EXISTS)
    	    	cout << "TestClass " << testClass << " already exists during create " << endl;
    	    else
    	    {
    			cout << "TestClass " << testClass << " create failed " <<  e.getMessage() << endl;
    			return;
    		}
    	}
    
    
        // GetClass:
    
        CIMClass c2 = client.getClass(globalNamespace, testClass, true);
        if (!c1.identical(c2))
        {
    	cout << "Class SubClass Returned not equal to created" << endl;
        }
        // ATTN: This test should be uncommented when the repository implements
        // the localOnly flag.
        //assert(c1.identical(c2));
    
        // Modify the class:
    
        c2.removeProperty(c2.findProperty(CIMName ("message")));
        try
        {
        	client.modifyClass(globalNamespace, c2);
    	}
    	catch (CIMException& e)
    	{
    		cout << "Testclass Modification failed " << e.getMessage() << endl;
     	}
        // GetClass:
    
        CIMClass c3 = client.getClass(globalNamespace, testClass, true);
    
        if (!c3.identical(c2))
        {
    	cout << "Test Failed. Rtned class c3 not equal to c2" << endl;
        }
        //assert(c3.identical(c2));
    
        // Determine if the new Class exists in Enumerate
    
        classNames = client.enumerateClassNames(
    		 globalNamespace, CIMName ("CIM_ManagedElement"), false);
    
        Boolean found = false;
    
        for (Uint32 i = 0; i < classNames.size(); i++)
        {
    		 if (classNames[i].equal(testClass))
    		     found = true;
        }
        if (!found)
        {
        	cout << "Test Class " << testClass << " Not found in enumeration " << endl;
        	return;
    	}
        //assert(found);
    
        // DeleteClass:
    
    	try
    	{
        	client.deleteClass(globalNamespace, testClass);
    	}
    	catch (CIMException& e)
    	{
    		cout << "Testclass delete failed " << e.getMessage() << endl;
    		return;
     	}
    }

    // Get all the classes and compare enum names with enum classes from before

    classNames = client.enumerateClassNames(globalNamespace, CIMName(), false);

    Array<CIMClass> classDecls = client.enumerateClasses(
		 globalNamespace, CIMName(), false, false, true, true);

    //assert(classDecls.size() == classNames.size());
    if (classDecls.size() == classNames.size())
    {
        cout << "Class total count before and after test. Before = " << classNames.size() 
            << " after = " <<  classDecls.size() << endl;
        return;
    }

    for (Uint32 i = 0; i < classNames.size(); i++)
    {
		 CIMClass tmp = client.getClass(
		     globalNamespace, classNames[i], false, true, true);

		 assert(classNames[i].equal(classDecls[i].getClassName()));

		 assert(tmp.identical(classDecls[i]));
    }

}

static void TestQualifierOperations(CIMClient& client, Boolean activeTest,
		 		 		 Boolean verboseTest) {

    Array<CIMQualifierDecl> qualifierDecls = client.enumerateQualifiers(globalNamespace);

    if (verboseTest)
    {
		 for (Uint32 i = 0; i < qualifierDecls.size(); i++)
		 {
		   cout << qualifierDecls[i].getName() << endl;
		 }

    }
    cout << qualifierDecls.size() << " Qualifiers" <<endl;

    if (activeTest)
    {
		 // Create two qualifier declarations:

		 CIMQualifierDecl qd1(CIMName ("qd1"), false, CIMScope::CLASS, 
                     CIMFlavor::TOSUBCLASS);
		 client.setQualifier(globalNamespace, qd1);

		 CIMQualifierDecl qd2(CIMName ("qd2"), String("Hello"), 
                     CIMScope::PROPERTY + CIMScope::CLASS,
		     CIMFlavor::OVERRIDABLE);
		 client.setQualifier(globalNamespace, qd2);

		 // Get them and compare:

		 CIMQualifierDecl tmp1 = client.getQualifier(globalNamespace, 
                     CIMName ("qd1"));
		 assert(tmp1.identical(qd1));

		 CIMQualifierDecl tmp2 = client.getQualifier(globalNamespace, 
                     CIMName ("qd2"));
		 assert(tmp2.identical(qd2));

		 // Enumerate the qualifiers:

		 Array<CIMQualifierDecl> qualifierDecls = client.enumerateQualifiers(globalNamespace);

		 for (Uint32 i = 0; i < qualifierDecls.size(); i++)
		 {
		     CIMQualifierDecl tmp = qualifierDecls[i];

		     if (tmp.getName().equal(CIMName ("qd1")))
		 		 assert(tmp1.identical(tmp));

		     if (tmp.getName().equal(CIMName ("qd2")))
		 		 assert(tmp2.identical(tmp));
		 }

		 // Delete the qualifiers:

		 client.deleteQualifier(globalNamespace, CIMName ("qd1"));
		 client.deleteQualifier(globalNamespace, CIMName ("qd2"));

    }

}

static void TestInstanceGetOperations(CIMClient& client, Boolean activeTest,
		 		 		 		    Boolean verboseTest)
{
    // Get all instances
    // Get all classes

    //Array<CIMName> classNames = client.enumerateClassNames(
    //    globalNamespace, "CIM_ManagedElement", false);

    Array<CIMName> classNames = client.enumerateClassNames(
		 globalNamespace, CIMName(), true);

    cout <<  classNames.size() << " Classes found " << endl;

    Array<CIMObjectPath> instanceNames;
    Uint16 numberOfNotSupportedClassesFound = 0;

    for (Uint32 i = 0; i < classNames.size(); i++)
    {
       //if (classNames[i] == "PG_ShutdownService")
       //{
       //    // Skip the PG_ObjectManager class.  It currently has no
       //    // instance provider and no instances.
       //}
       //else
       //{
           try
           {
               instanceNames = client.enumerateInstanceNames(globalNamespace,classNames[i]);
               if (instanceNames.size() > 0)
		   cout << "Class " << classNames[i] << " "
 		        << instanceNames.size() << " Instances" << endl;
           }
           catch(CIMException& e)
           {
                 if (e.getCode() == CIM_ERR_NOT_SUPPORTED)
                 {
                    numberOfNotSupportedClassesFound++;
                 }
                 else
                 {
                    cerr << "CIMException : " << classNames[i] << endl;
                    cerr << e.getMessage() << endl;
                 }
           }
	   catch(Exception& e)
	   {
	       PEGASUS_STD(cerr) << "Error: " << e.getMessage() << PEGASUS_STD(endl);
	       exit(1);
	   }
       //}
    }
    if (numberOfNotSupportedClassesFound > 0)
    {
       cout << "Number of Not Supported Classes Found = "
            << numberOfNotSupportedClassesFound << endl;
    }
    /*
    virtual Array<CIMObjectPath> enumerateInstanceNames(
        const CIMNamespaceName& nameSpace,
        const CIMName& className) = 0;

    virtual Array<CIMInstance> enumerateInstances(
        const CIMNamespaceName& nameSpace,
        const CIMName& className,
        Boolean deepInheritance = true,
        Boolean localOnly = true,
        Boolean includeQualifiers = false,
        Boolean includeClassOrigin = false,
    */

}
static void TestInstanceModifyOperations(CIMClient& client, Boolean
		 		 	 activeTest, Boolean verboseTest)
{
    if (!activeTest)
    {
        cout << "InstanceModify bypassed because it modifys repository. Set active to execute"
             << endl;
        return;
    }
    // name of class to play with
    CIMName className = CIMName ("PEG_TESTLocalClass");

    // Delete the class if it already exists:
    try
    {
		 client.deleteClass(globalNamespace, className);
    }
    catch (Exception&)
    {
		 // Ignore delete class!
    }

    // Create a new class:

    CIMClass cimClass(className);
    cimClass
		 .addProperty(CIMProperty(CIMName ("last"), String())
		     .addQualifier(CIMQualifier(CIMName ("key"), true)))
		 .addProperty(CIMProperty(CIMName ("first"), String())
		     .addQualifier(CIMQualifier(CIMName ("key"), true)))
		 .addProperty(CIMProperty(CIMName ("age"), Uint32(0))
		     .addQualifier(CIMQualifier(CIMName ("key"), true)))
		 .addProperty(CIMProperty(CIMName ("nick"), String())
		      .addQualifier(CIMQualifier(CIMName ("key"), false)));
    client.createClass(globalNamespace, cimClass);

    // Create an instance of that class:
    cout << "Create one Instance of class " << className << endl;

    CIMInstance cimInstance(className);
    cimInstance.addProperty(CIMProperty(CIMName ("last"), String("Smith")));
    cimInstance.addProperty(CIMProperty(CIMName ("first"), String("John")));
    cimInstance.addProperty(CIMProperty(CIMName ("age"), Uint32(1010)));
    cimInstance.addProperty(CIMProperty(CIMName ("nick"), String("Duke")));
    CIMObjectPath instanceName = cimInstance.buildPath(cimClass);
    client.createInstance(globalNamespace, cimInstance);

    // Get the instance and compare with created one:

    //CIMObjectPath ref;
    //CIMObjectPath::instanceNameToReference(instanceName, ref);
    CIMInstance tmp = client.getInstance(globalNamespace, instanceName);

    // XmlWriter::printInstanceElement(cimInstance);
    // XmlWriter::printInstanceElement(tmp);
    // assert(cimInstance.identical(tmp));

    // Test timeout methods

    const Uint32 TEST_TIMEOUT = 10000;
    Uint32 origTimeout = client.getTimeout();
    client.setTimeout( TEST_TIMEOUT );
    Uint32 newTimeout = client.getTimeout();
    assert( newTimeout == TEST_TIMEOUT );
    client.setTimeout( origTimeout );

    // Test get/set property methods

    const String TESTPROPVAL    = "JR";
    const String TESTPROPVALNAME = "nick";
    cout << "Set property " << endl;
    const CIMValue testPropVal = CIMValue( TESTPROPVAL );
    client.setProperty( globalNamespace, instanceName, TESTPROPVALNAME,
			testPropVal );
    cout << "Get property " << endl;
    CIMValue returnedPropVal = client.getProperty(  globalNamespace,
						    instanceName,
						    TESTPROPVALNAME );
    assert( returnedPropVal == testPropVal );

    // Test modify instance client method
    // Change the "nick" property and compare.

    CIMInstance testInstance =
      client.getInstance(globalNamespace, instanceName);
    Uint32 propertyPos = testInstance.findProperty( TESTPROPVALNAME );
    testInstance.removeProperty( propertyPos );
    CIMProperty nickProperty = CIMProperty( TESTPROPVALNAME, String("Duke") );
    nickProperty.setClassOrigin( className );
    testInstance.addProperty( nickProperty );
    testInstance.setPath (instanceName);
    client.modifyInstance( globalNamespace,
                           testInstance
                           );
    CIMInstance currentInstance =
      client.getInstance( globalNamespace, instanceName );
    currentInstance.setPath( instanceName );
    assert( currentInstance.identical( testInstance ) );

    client.deleteInstance(globalNamespace, instanceName);

    // Repeat to create multiple instances
    Uint32 repeatCount = 30;
    Array<CIMObjectPath> instanceNames;
    cout << "Create " << repeatCount << " Instances" << endl;
    for (Uint32 i = 0; i < repeatCount; i++)
    {
        CIMInstance cimInstance(className);
        cimInstance.addProperty(CIMProperty(CIMName ("last"), String("Smith")));
        cimInstance.addProperty(CIMProperty(CIMName ("first"), String("John")));
        cimInstance.addProperty(CIMProperty(CIMName ("age"), Uint32(i)));
        instanceNames.append( cimInstance.buildPath(cimClass) );
        client.createInstance(globalNamespace, cimInstance);
    }
    cout << "Delete the Instances " << endl;
    for (Uint32 i = 0; i < repeatCount; i++)
    {
        client.deleteInstance(globalNamespace,instanceNames[i]);
    }

    cout << "Delete the Class " << endl;
    client.deleteClass(globalNamespace,className);
}

static void TestAssociationOperations(CIMClient& client, Boolean
		 		 	 activeTest, Boolean verboseTest)
{
    return;
}

/* Creates an instance with a method, executes the method and then deletes
the instance

Warning: This test works only as long as there is a provider, in this case, it goes
to the repository and gets dropped on the floor.
*/

static void TestMethodOperations( CIMClient& client, Boolean
		 		 	 activeTest, Boolean verboseTest)

{
    // Since the test modifies the repository, don't do it unless active set.
    if (!activeTest)
        return;
    
    //Indication to be created
    CIMClass cimClass = client.getClass(globalNamespace, 
        CIMName ("TestSoftwarePkg"), false);
    CIMInstance cimInstance(CIMName ("TestSoftwarePkg"));
    cimInstance.addProperty(CIMProperty(CIMName ("PkgName"), String("WBEM")));
    cimInstance.addProperty(CIMProperty(CIMName ("PkgIndex"), Uint32(101)));
    cimInstance.addProperty(CIMProperty(CIMName ("trapOid"), 
        String("1.3.6.1.4.1.11.2.3.1.7.0.4")));
    cimInstance.addProperty(CIMProperty(CIMName ("computerName"), 
        String("NU744781")));
    CIMObjectPath instanceName = cimInstance.buildPath(cimClass);
    instanceName.setNameSpace(globalNamespace);
    client.createInstance(globalNamespace, cimInstance);
    
    try
    {
    	Array<CIMParamValue> inParams;
    	Array<CIMParamValue> outParams;
    	inParams.append(CIMParamValue("param1", 
            CIMValue(String("Hewlett-Packard"))));
    	inParams.append(CIMParamValue("param2", 
            CIMValue(String("California"))));
            Uint32 testRepeat = 100;
            
    	for (Uint32 i = 0; i < testRepeat; i++)        // repeat the test x time
            {
                CIMValue retValue = client.invokeMethod(
                    globalNamespace,
                    instanceName,
                    CIMName ("ChangeName"),
                    inParams,
                    outParams);
                if (verboseTest)
                {
                        cout << "Output : " << retValue.toString() << endl;
                        for (Uint8 i = 0; i < outParams.size(); i++)
                            cout << outParams[i].getParameterName()
                                << " : "
                                << outParams[i].getValue().toString()
                                << endl;
                }
            }
            cout << "Executed " << testRepeat << " methods" << endl;
    }

    catch(Exception& e)
    {
    	PEGASUS_STD(cerr) << "Error: " << e.getMessage() << PEGASUS_STD(endl);
    	exit(1);
    }

    client.deleteInstance(globalNamespace, instanceName);
}

/*
   Tests the invoke method request via the sample method provider.
*/

static void TestInvokeMethod( CIMClient& client,
			      Boolean activeTest,
			      Boolean verboseTest )
{
  const CIMNamespaceName NAMESPACE  = CIMNamespaceName ("root/SampleProvider");
  const CIMName classname  = CIMName ("Sample_MethodProviderClass");
  const CIMName methodName = CIMName ("SayHello");
  const String OUTSTRING = "Yoda";
  const String GOODREPLY = "Hello, " + OUTSTRING + "!";
  const String GOODPARAM = "From Neverland";
  const CIMObjectPath instanceName = CIMObjectPath 
                            ("Sample_MethodProviderClass.Identifier=1");

  try
  {
      Array<CIMParamValue> inParams;
      Array<CIMParamValue> outParams;

      Uint32 testRepeat = 10;
      inParams.append( CIMParamValue(  "Name", CIMValue( OUTSTRING ) ) );
      
      for (Uint32 i = 0; i < testRepeat; i++)        // repeat the test x time
      {
          CIMValue retValue = client.invokeMethod(
                              NAMESPACE,
                              instanceName,
                              methodName,
                              inParams,
                              outParams);
          if( retValue.toString() != GOODREPLY )
            {
                PEGASUS_STD(cerr) << "Error: bad reply \"" 
                  << retValue.toString() << "\"" << PEGASUS_STD(endl);
                return;
            }
          else
          {
                if ( outParams.size() > 0 )
                {
                    String outReply = String::EMPTY;
                    CIMValue paramVal = outParams[0].getValue();
                    paramVal.get( outReply );
                    if ( outReply != GOODPARAM )
                    {
                        PEGASUS_STD(cerr) << "Error: bad output parameter: \"" <<
                        outReply << "\"" << PEGASUS_STD(endl);
                        return;
                    }
                }
                else
                {
                    PEGASUS_STD(cerr) << "Error: output parameter missing. Reply: \"" <<
                    retValue.toString() << "\"" << PEGASUS_STD(endl);
                    return;
                }
            }
            if (verboseTest)
            {
                cout << "Output : " << retValue.toString() << endl;
                for (Uint8 i = 0; i < outParams.size(); i++)
                    cout << outParams[i].getParameterName()
                    << " : "
                    << outParams[i].getValue().toString()
                    << endl;
            }
      }
  cout << "Executed " << testRepeat << " methods" << endl;
  }
  catch(Exception& e)
  {
        PEGASUS_STD(cerr) << "Error: " << e.getMessage() << PEGASUS_STD(endl);
        return;
  }
}

/*
   Tests the enumerate instances from the sample instance provider.
*/

static void TestEnumerateInstances( CIMClient& client,
				    Boolean activeTest,
				    Boolean verboseTest )
{

  const CIMNamespaceName NAMESPACE = CIMNamespaceName ("root/SampleProvider");
  const String INSTANCE0 = "instance 0Sample_InstanceProviderClass";
  const String INSTANCE1 = "instance 1Sample_InstanceProviderClass";
  const String INSTANCE2 = "instance 2Sample_InstanceProviderClass";
  const CIMName CLASSNAME = CIMName ("Sample_InstanceProviderClass");

  try
    {
      const CIMName classname = CLASSNAME;
      Boolean deepInheritance = true;
      Boolean localOnly = true;
      Boolean includeQualifiers = false;
      Boolean includeClassOrigin = false;
      Uint32 testRepeat = 10;
      Uint32 numberInstances;

      for (Uint32 i = 0; i < testRepeat; i++)        // repeat the test x time
        {
	  Array<CIMInstance> cimNInstances =
	    client.enumerateInstances(NAMESPACE,  classname, deepInheritance,
				      localOnly,  includeQualifiers,
				      includeClassOrigin );

	  assert(cimNInstances.size() == 3);
          numberInstances =  cimNInstances.size();
	  for (Uint32 i = 0; i < cimNInstances.size(); i++)
	    {
	      String instanceRef = cimNInstances[i].getPath ().toString();

	      //ATTN P2 WO 4 April 2002
	      // Test for INSTANCE0..2 when getInstanceName returns
	      // the full reference string

	      if( !(String::equal(  instanceRef, CLASSNAME.getString() ) ) )
		{
		  PEGASUS_STD(cerr) << "Error: EnumInstances failed" <<
		    PEGASUS_STD(endl);
		  return;
		}
	    }
	}
      cout << "Enumerate " << numberInstances << " instances " << testRepeat << " times" << endl;
    }
  catch(Exception& e)
    {
      PEGASUS_STD(cerr) << "Error: " << e.getMessage() << PEGASUS_STD(endl);
      return;
    }
}

static Boolean verifyCertificate(SSLCertificateInfo &certInfo)
{
    //ATTN-NB-03-05132002: Add code to handle server certificate verification.
    return true;
}

///////////////////////////////////////////////////////////////
//    OPTION MANAGEMENT
///////////////////////////////////////////////////////////////

/** GetOptions function - This function defines the Options Table
    and sets up the options from that table using the option manager.
    const char* optionName;
    const char* defaultValue;
    int required;
    Option::Type type;
    char** domain;
    Uint32 domainSize;
    const char* commandLineOptionName;
    const char* optionHelpMessage;

*/
void GetOptions(
    OptionManager& om,
    int& argc,
    char** argv,
    const String& pegasusHome)
{
    static struct OptionRow optionsTable[] =
        //     optionname defaultvalue rqd  type domain domainsize clname hlpmsg
    {
		 {"active", "false", false, Option::BOOLEAN, 0, 0, "a",
		 		      "If set allows test that modify the repository" },

		 {"repeat", "1", false, Option::WHOLE_NUMBER, 0, 0, "r",
		 		       "Specifies a Repeat Count Entire test repeated this many times" },

		 {"namespace", "root/cimv2", false, Option::STRING, 0, 0, "-n",
		 		 		 "specifies namespace to use for test" },

		 {"version", "false", false, Option::BOOLEAN, 0, 0, "version",
		 		 		 "Displays TestClient Version "},

		 {"verbose", "false", false, Option::BOOLEAN, 0, 0, "verbose",
		 		 		 "If set, outputs extra information "},

		 {"help", "false", false, Option::BOOLEAN, 0, 0, "h",
		 		     "Prints help message with command line options "},
		 {"debug", "false", false, Option::BOOLEAN, 0, 0, "d",
		              "Not Used "},
		 {"slp", "false", false, Option::BOOLEAN, 0, 0, "slp",
		 		 		 "use SLP to find cim servers to test"},
		 {"ssl", "false", false, Option::BOOLEAN, 0, 0, "ssl",
		 		 		 "use SSL"},

		 {"local", "false", false, Option::BOOLEAN, 0, 0, "local",
		 		 		 "Use local connection mechanism"},
		 {"user", "", false, Option::STRING, 0, 0, "user",
		 		 		 "Specifies user name" },

		 {"password", "", false, Option::STRING, 0, 0, "password",
		 		 		 "Specifies password" }

    };
    const Uint32 NUM_OPTIONS = sizeof(optionsTable) / sizeof(optionsTable[0]);

    om.registerOptions(optionsTable, NUM_OPTIONS);

    String configFile = pegasusHome + "/cimserver.conf";

    cout << "Config file from " << configFile << endl;

    if (FileSystem::exists(configFile))
		 om.mergeFile(configFile);

    om.mergeCommandLine(argc, argv);

    om.checkRequiredOptions();
}


///////////////////////////////////////////////////////////////
//    MAIN
///////////////////////////////////////////////////////////////

int main(int argc, char** argv)
{

  // char connection[50] = "localhost:5988";
  char *address_string = NULL;

    Uint32 repetitions = 1;

    // Get environment variables:

    String pegasusHome;
    pegasusHome = "/";
    // GetEnvironmentVariables(argv[0], pegasusHome);

    // Get options (from command line and from configuration file); this
    // removes corresponding options and their arguments fromt he command
    // line.

    // Get options (from command line and from configuration file); this
    // removes corresponding options and their arguments fromt he command
    // line.

    OptionManager om;

    try
    {
		 GetOptions(om, argc, argv, pegasusHome);
    }
    catch (Exception& e)
    {
		 cerr << argv[0] << ": " << e.getMessage() << endl;
		 exit(1);
    }

    // Check to see if user asked for help (-h otpion):
    if (om.valueEquals("help", "true"))
    {
                String header = "Usage ";
                header.append(argv[0]);
                header.append(" -parameters host [host]");

                String trailer = "Assumes localhost:5988 if host not specified";
                trailer.append("\nHost may be of the form name or name:port");
                trailer.append("\nPort 5988 assumed if port number missing.");
                om.printOptionsHelpTxt(header, trailer);

		 exit(0);
    }

    String tmp;
    om.lookupValue("namespace", tmp);
    CIMNamespaceName localNameSpace = CIMNamespaceName (tmp);
    globalNamespace = localNameSpace;
    cout << "Namespace = " << localNameSpace << endl;

    String userName;
    om.lookupValue("user", userName);
    if (userName != String::EMPTY)
    {
       cout << "Username = " << userName << endl;
    }
    Boolean verboseTest = om.isTrue("verbose");

    String password;
    om.lookupValue("password", password);
    if (password != String::EMPTY)
    {
       cout << "password = " << password << endl;
    }

	// Set up number of test repetitions.  Will repeat entire test this number of times
	// Default is zero
	String repeats;
	Uint32 repeatTestCount = 0;
	/* ATTN: KS P0 Test and fix function added to Option Manager
	*/
	if (!om.lookupIntegerValue("repeat", repeatTestCount))
	    repeatTestCount = 1;
	/*
	if (om.lookupValue("repeat", repeats))
        {
		repeatTestCount = atol(repeats.getCString());
        }
	else
		repeatTestCount = 1;
	*/
	if(verboseTest)
		cout << "Test repeat count " << repeatTestCount << endl;

	// Setup the active test flag.  Determines if we change repository.
    Boolean activeTest = false;
    if (om.valueEquals("active", "true"))
		 activeTest = true;

    // here we determine the list of systems to test.
    // All arguments remaining in argv go into list.
    // if SLP option set, SLP list goes into set.
    // if SLP false and no args, use default localhost:5988
    Boolean useSLP =  om.isTrue("slp");
    cout << "SLP " << (useSLP ? "true" : "false") << endl;

    Boolean localConnection = (om.valueEquals("local", "true"))? true: false;
    cout << "localConnection " << (localConnection ? "true" : "false") << endl;

    Array<String> connectionList;
    if (argc > 1 && !localConnection)
		 for (Sint32 i = 1; i < argc; i++)
		     connectionList.append(argv[i]);

    Boolean useSSL =  om.isTrue("ssl");

    // use the ssl port if ssl us specified
    if (useSSL)
      connectionList.append("localhost:5989");
    // substitute the default only if noslp and no params
    else if(useSLP == false && argc < 2)
      connectionList.append("localhost:5988");

    // Expand host to add port if not defined

#if !defined(PEGASUS_OS_ZOS) && ! defined(PEGASUS_OS_HPUX) && !defined(PEGASUS_OS_LINUX)
    if( useSLP )
    {
      slp_client discovery = slp_client();
      discovery.discovery_cycle ( "service:cim.pegasus",
		 		  NULL,
		 		  "DEFAULT" ) ;

      struct rply_list *replies = discovery.get_response( );
      String host ;
      while( replies != NULL )
		 {
		   slp_get_addr_string_from_url(replies->url, host) ;
		   connectionList.append( host ) ;
		   delete replies;
		   replies = discovery.get_response( ) ;
		 }
    }
#endif

	// Show the connectionlist
    cout << "Connection List size " << connectionList.size() << endl;
    for (Uint32 i = 0; i < connectionList.size(); i++)
	cout << "Connection " << i << " address " << connectionList[i] << endl;

    for(Uint32 numTests = 1; numTests <= repeatTestCount; numTests++)
	{
		cout << "Test Repetition # " << numTests << endl;
		for (Uint32 i = 0; i < connectionList.size(); i++)
		{
			cout << "Start Try Block" << endl;
		  try
		  {
		     cout << "Set Stopwatch" << endl;
		     Stopwatch elapsedTime;
		     cout << "Create client" << endl;
		     CIMClient client;
		     client.setTimeout(360 * 1000);
		     cout << "Client created" << endl;

                     //
                     //  Get host and port number from connection list entry
                     //
                     Uint32 index = connectionList[i].find (':');
                     String host = connectionList[i].subString (0, index);
                     Uint32 portNumber = 0;
                     if (index != PEG_NOT_FOUND)
                     {
                         String portStr = connectionList[i].subString 
                             (index + 1, connectionList[i].size ());
                         sscanf (portStr.getCString (), "%u", &portNumber);
                     }

                     if (useSSL)
		     {
			if (om.isTrue("local"))
			{
			     cout << "Using local connection mechanism " << endl;
     			     client.connectLocal();
			}
			else
			{
                            //
                            // Get environment variables:
                            //
                            const char* pegasusHome = getenv("PEGASUS_HOME");

                            String certpath = FileSystem::getAbsolutePath(
                                pegasusHome, PEGASUS_SSLCLIENT_CERTIFICATEFILE);

                            String randFile = String::EMPTY;
#ifdef PEGASUS_SSL_RANDOMFILE
                            randFile = FileSystem::getAbsolutePath(
                                pegasusHome, PEGASUS_SSLCLIENT_RANDOMFILE);
#endif
                            SSLContext sslcontext(
                                certpath, verifyCertificate, randFile);

                            cout << "connecting to " << connectionList[i] << " using SSL" << endl;
                            client.connect (host, portNumber, sslcontext, 
                                            userName, password);
                        }
		      }
	              else
		      {
			if (om.isTrue("local"))
			{
			     cout << "Using local connection mechanism " << endl;
     			     client.connectLocal();
			}
			else
			{
						   cout << "Connecting to " << connectionList[i] << endl;
                            client.connect (host, portNumber,
                                            userName, password);
			}
		      }
		      cout << "Client Connected" << endl;

		           testStart("Test NameSpace Operations");

			   TestNameSpaceOperations(client, activeTest, verboseTest);
			           testEnd(elapsedTime.getElapsed());

			   testStart("Test Qualifier Operations");
			   elapsedTime.reset();
			   TestQualifierOperations(client,activeTest,verboseTest);
				   testEnd(elapsedTime.getElapsed());

			   testStart("Test EnumerateClassNames");
			   elapsedTime.reset();
			   TestEnumerateClassNames(client,activeTest,verboseTest);
				   testEnd(elapsedTime.getElapsed());


			   testStart("Test Class Operations");
			   elapsedTime.reset();
			   TestClassOperations(client,activeTest,verboseTest);
				   testEnd(elapsedTime.getElapsed());


			   testStart("Test Instance Get Operations");
			   elapsedTime.reset();
			   TestInstanceGetOperations(client,activeTest,verboseTest);
				   testEnd(elapsedTime.getElapsed());

			   testStart("Test Instance Modification Operations");
			   elapsedTime.reset();
			   TestInstanceModifyOperations(client, activeTest, verboseTest);
				   testEnd(elapsedTime.getElapsed());

			   testStart("Test Associations");
				   TestAssociationOperations(client, activeTest, verboseTest);
				   testEnd(elapsedTime.getElapsed());

				   /* Turn this one off until we get valid method to execute

			   testStart("Test Method Execution");
				   TestMethodOperations(client, activeTest, verboseTest);
				   testEnd(elapsedTime.getElapsed());
				   */

			   testStart("Test Invoke Method Execution");
			           elapsedTime.reset();
				   TestInvokeMethod(client, activeTest,
						    verboseTest);
				   testEnd(elapsedTime.getElapsed());

			   testStart("Test Enumerate Instances Execution");
			           elapsedTime.reset();
				   TestEnumerateInstances(client, activeTest,
							  verboseTest);
				   testEnd(elapsedTime.getElapsed());

			  client.disconnect();
		  }
		  catch(Exception& e)
		  {
			   PEGASUS_STD(cerr) << "Error: " << e.getMessage() <<
			     PEGASUS_STD(endl);
			   exit(1);
		  }
		}
	}
    PEGASUS_STD(cout) << "+++++ "<< argv[0] << " Terminated Normally" << PEGASUS_STD(endl);
    return 0;
}

/*
    TODO:  1. put in the option manager		    DONE
           2. Make passive tests only option. DONE
		    3. Make test loop tool
		    4. Make display an option		  DONE
		    5. Make test multiple systems.
		    6. Get rid of diagnostics and clean display
		    7. Add correct successful at end
		    8. Make OO
		    9. Add associations test
		    10. Add cim references test.
		    11. Add test all namespaces test.

*/
