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
#if !defined(PEGASUS_OS_ZOS) && ! defined(PEGASUS_OS_HPUX)
#include <slp/slp.h>
#endif

PEGASUS_USING_PEGASUS;
PEGASUS_USING_STD;

String globalNamespace = "root/cimv2";
static const char __NAMESPACE_NAMESPACE [] = "root";

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
    String className = "__Namespace";

    Array<CIMObjectPath> instanceNames;

    // Call enumerate Instances CIM Method
    try 
    {
		 // cout << " Enumerate Namespaces " << className << endl;
		 instanceNames = client.enumerateInstanceNames(
		     __NAMESPACE_NAMESPACE, className);
    
		 cout << instanceNames.size() << " Namespaces" << endl;
		 // Convert from CIMObjectPath to String form
    
		 if (verboseTest)
		 {
		     // Array<String> tmpInstanceNames;
    
		     for (Uint32 i = 0; i < instanceNames.size(); i++)
		 		 cout << instanceNames[i].toString() << endl;
		     }
		 }
    
    
		 // ATTN Convert this to a test.
		 //tmpInstanceNames.append(instanceNames[i].toString());
    catch(CIMClientException& e)
    {
	 PEGASUS_STD(cerr) << "CIMClient Exception NameSpace Enumeration: "
		    << e.getMessage() << PEGASUS_STD(endl);
	 return;
    }
    catch(Exception& e)
    {
	PEGASUS_STD(cerr) << "Exception Namespace Enumeration: " << e.getMessage() 
	<< PEGASUS_STD(endl);
        return;
    }
    // If conducting active test, try to create and delete a namespace.
    if(activeTest)
    {
	if(verboseTest)
	    cout << "Conducting Create / Delete namespace test " << endl;

	// Build the instance name for __namespace
	String testNamespaceName = "karl/junk";
	String instanceName = className;
	instanceName.append( ".Name=\"");
	instanceName.append(testNamespaceName);
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
	    newInstance.addProperty(CIMProperty("name", testNamespaceName));
	    newInstanceName = client.createInstance(__NAMESPACE_NAMESPACE, newInstance);
	}
	catch(CIMClientCIMException& e)
	{
	     if (e.getCode() == CIM_ERR_ALREADY_EXISTS)
             {
	           newInstanceName = CIMObjectPath(instanceName);
	     }
	     else
             {
	           PEGASUS_STD(cerr) << "CIMClientException NameSpace Creation: "
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
	catch(CIMClientException& e)
	{
	     PEGASUS_STD(cerr) << "CIMClientException NameSpace getInstance: "
			<< e.getMessage() << " Retrieving " << instanceName
		        << PEGASUS_STD(endl);
	     return;
	}
	catch(Exception& e)
	{
	    PEGASUS_STD(cerr) << "Exception NameSpace getInstance: " << e.getMessage() << PEGASUS_STD(endl);
	    return;
	}

	// Now delete it

	try
	{
	    CIMObjectPath myReference(instanceName);
	    cout << "Deleting namespace = " << instanceName << endl;
	    client.deleteInstance(__NAMESPACE_NAMESPACE, myReference);
	}
	catch(CIMClientException& e)
	{
	     PEGASUS_STD(cerr) << "CIMClientException NameSpace Deletion: "
			<< e.getMessage() << " Deleting " << instanceName
		        << PEGASUS_STD(endl);
	     return;
	}
	catch(Exception& e)
	{
	    PEGASUS_STD(cerr) << "Exception NameSpace Deletion: " << e.getMessage() << PEGASUS_STD(endl);
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
		 String className = "";
		 Array<String> classNames = client.enumerateClassNames(
		     globalNamespace, className, deepInheritance);

		 if (verboseTest)
		 {
		     for (Uint32 i = 0, n = classNames.size(); i < n; i++)
		 		 cout << classNames[i] << endl;
		 }
    
		 cout << classNames.size() << " ClassNames" << endl;
    }
    catch(CIMClientException& e)
    {
		 cout << "Error NameSpace Enumeration:" << endl;
		 cout << e.getMessage() << endl;
    }

}
		 		 		 		    
static void TestGetClass(CIMClient& client, Boolean activeTest, 
		     Boolean verboseTest) 
{
    CIMClass c = client.getClass(
		 globalNamespace, "CIM_ComputerSystem", false, false, true);

    XmlWriter::printClassElement(c);
}

static void TestClassOperations(CIMClient& client, Boolean ActiveTest, 
		 		 		     Boolean verboseTest) {
    
    // Name of Class to use in create/delete test
    String testClass = "PEG_TestSubClass";
    // NOTE: We should add test for CIM_ManagedElement first.

    //Test for class already existing
    Array<String> classNames = client.enumerateClassNames(
		 globalNamespace, "CIM_ManagedElement", false);

    for (Uint32 i = 0; i < classNames.size(); i++)
    {
		 if (CIMName::equal(classNames[i], testClass))
		     client.deleteClass(globalNamespace, testClass);
    }
    
    // CreateClass:

    CIMClass c1(testClass, "CIM_ManagedElement");
    c1.addProperty(CIMProperty("count", Uint32(99)));
    c1.addProperty(CIMProperty("ratio", Real64(66.66)));
    c1.addProperty(CIMProperty("message", String("Hello World")));
    client.createClass(globalNamespace, c1);

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

    c2.removeProperty(c2.findProperty("message"));
    client.modifyClass(globalNamespace, c2);

    // GetClass:

    CIMClass c3 = client.getClass(globalNamespace, testClass, true);

    if (!c3.identical(c2))
    {
	cout << "Test Failed. Rtned class c3 not equal to c2" << endl;
    }
    assert(c3.identical(c2));

    // Determine if the new Class exists in Enumerate

    classNames = client.enumerateClassNames(
		 globalNamespace, "CIM_ManagedElement", false);

    Boolean found = false;

    for (Uint32 i = 0; i < classNames.size(); i++)
    {
		 if (CIMName::equal(classNames[i], testClass))
		     found = true;
    }

    assert(found);

    // DeleteClass:

    client.deleteClass(globalNamespace, testClass);

    // Get all the classes and compare enum names with enum classes

    classNames = client.enumerateClassNames(globalNamespace, String(), false);

    Array<CIMClass> classDecls = client.enumerateClasses(
		 globalNamespace, String(), false, false, true, true);

    assert(classDecls.size() == classNames.size());

    for (Uint32 i = 0; i < classNames.size(); i++)
    {
		 CIMClass tmp = client.getClass(
		     globalNamespace, classNames[i], false, true, true);

		 assert(CIMName::equal(classDecls[i].getClassName(), classNames[i]));

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
    
		 CIMQualifierDecl qd1("qd1", false, CIMScope::CLASS, CIMFlavor::TOSUBCLASS);
		 client.setQualifier(globalNamespace, qd1);
    
		 CIMQualifierDecl qd2("qd2", "Hello", CIMScope::PROPERTY | CIMScope::CLASS, 
		     CIMFlavor::OVERRIDABLE);
		 client.setQualifier(globalNamespace, qd2);
    
		 // Get them and compare:
    
		 CIMQualifierDecl tmp1 = client.getQualifier(globalNamespace, "qd1");
		 assert(tmp1.identical(qd1));
    
		 CIMQualifierDecl tmp2 = client.getQualifier(globalNamespace, "qd2");
		 assert(tmp2.identical(qd2));
    
		 // Enumerate the qualifiers:
    
		 Array<CIMQualifierDecl> qualifierDecls = client.enumerateQualifiers(globalNamespace);
    
		 for (Uint32 i = 0; i < qualifierDecls.size(); i++)
		 {
		     CIMQualifierDecl tmp = qualifierDecls[i];
    
		     if (CIMName::equal(tmp.getName(), "qd1"))
		 		 assert(tmp1.identical(tmp));
    
		     if (CIMName::equal(tmp.getName(), "qd2"))
		 		 assert(tmp2.identical(tmp));
		 }
    
		 // Delete the qualifiers:
    
		 client.deleteQualifier(globalNamespace, "qd1");
		 client.deleteQualifier(globalNamespace, "qd2");

    }

}

static void TestInstanceGetOperations(CIMClient& client, Boolean activeTest,
		 		 		 		    Boolean verboseTest)
{
    // Get all instances
    // Get all classes

    //Array<String> classNames = client.enumerateClassNames(
    //    globalNamespace, "CIM_ManagedElement", false);
    
    Array<String> classNames = client.enumerateClassNames(
		 globalNamespace, String(), true);

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
           catch(CIMClientCIMException& e)
           {
                 if (e.getCode() == CIM_ERR_NOT_SUPPORTED)
                 {
                    numberOfNotSupportedClassesFound++;
                 }
                 else
                 {
                    cerr << "CIMClientCIMException : " << classNames[i] << endl;
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
        const String& nameSpace,
        const String& className) = 0;

    virtual Array<CIMInstance> enumerateInstances(
        const String& nameSpace,
        const String& className,
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
    String className = "PEG_TESTLocalClass";

    // Delete the class if it already exists:
    try
    {
		 client.deleteClass(globalNamespace, className);
    }
    catch (CIMClientException&)
    {
		 // Ignore delete class!
    }

    // Create a new class:

    CIMClass cimClass(className);
    cimClass
		 .addProperty(CIMProperty("last", String())
		     .addQualifier(CIMQualifier("key", true)))
		 .addProperty(CIMProperty("first", String())
		     .addQualifier(CIMQualifier("key", true)))
		 .addProperty(CIMProperty("age", Uint32(0))
		     .addQualifier(CIMQualifier("key", true)))
		 .addProperty(CIMProperty("nick", String())
		      .addQualifier(CIMQualifier("key", false)));
    client.createClass(globalNamespace, cimClass);

    // Create an instance of that class:
    cout << "Create one Instance of class " << className << endl;

    CIMInstance cimInstance(className);
    cimInstance.addProperty(CIMProperty("last", "Smith"));
    cimInstance.addProperty(CIMProperty("first", "John"));
    cimInstance.addProperty(CIMProperty("age", Uint32(1010)));
    cimInstance.addProperty(CIMProperty("nick", "Duke"));
    CIMObjectPath instanceName = cimInstance.getInstanceName(cimClass);
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
    Uint32 origTimeout = client.getTimeOut();
    client.setTimeOut( TEST_TIMEOUT );
    Uint32 newTimeout = client.getTimeOut();
    assert( newTimeout == TEST_TIMEOUT );
    client.setTimeOut( origTimeout );

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
    CIMProperty nickProperty = CIMProperty( TESTPROPVALNAME, "Duke" );
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
        cimInstance.addProperty(CIMProperty("last", "Smith"));
        cimInstance.addProperty(CIMProperty("first", "John"));
        cimInstance.addProperty(CIMProperty("age", Uint32(i)));
        instanceNames.append( cimInstance.getInstanceName(cimClass) );
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
    {
        return;   
    }
    //Indication
    CIMClass cimClass = client.getClass(globalNamespace, "TestSoftwarePkg", false);
    CIMInstance cimInstance("TestSoftwarePkg");
    cimInstance.addProperty(CIMProperty("PkgName", "WBEM"));
    cimInstance.addProperty(CIMProperty("PkgIndex", Uint32(101)));
    cimInstance.addProperty(CIMProperty("trapOid", "1.3.6.1.4.1.11.2.3.1.7.0.4"));
    cimInstance.addProperty(CIMProperty("computerName", "NU744781"));
    CIMObjectPath instanceName = cimInstance.getInstanceName(cimClass);
    instanceName.setNameSpace(globalNamespace);
    client.createInstance(globalNamespace, cimInstance);
    try
    {
        
	Array<CIMParamValue> inParams;
	Array<CIMParamValue> outParams;
	inParams.append(CIMParamValue("param1", CIMValue("Hewlett-Packard")));
	inParams.append(CIMParamValue("param2", CIMValue("California")));
        Uint32 testRepeat = 100;
	for (Uint32 i = 0; i < testRepeat; i++)        // repeat the test x time
        {
            CIMValue retValue = client.invokeMethod(
                globalNamespace, 
                instanceName, 
                "ChangeName", 
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

    catch(CIMClientException& e)
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
  const String NAMESPACE  = "root/SampleProvider";
  const String classname  = "Sample_MethodProviderClass";
  const String methodName = "SayHello";
  const String OUTSTRING = "Yoda";
  const String GOODREPLY = "Hello, " + OUTSTRING + "!";
  const String GOODPARAM = "From Neverland";
  const CIMObjectPath instanceName = 
                            "Sample_MethodProviderClass.Identifier=1";

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
	      PEGASUS_STD(cerr) << "Error: bad reply \"" <<
		retValue.toString() << "\"" << PEGASUS_STD(endl);
	      return;
	    }
	  else
	    {
	      if( outParams.size() > 0 )
		{
		  String outReply = String::EMPTY;
		  CIMValue paramVal = outParams[0].getValue();
		  paramVal.get( outReply );
		  if( outReply != GOODPARAM )
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
  catch(CIMClientException& e)
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

  const String NAMESPACE = "root/SampleProvider";
  const String INSTANCE0 = "instance 0Sample_InstanceProviderClass";
  const String INSTANCE1 = "instance 1Sample_InstanceProviderClass";
  const String INSTANCE2 = "instance 2Sample_InstanceProviderClass";
  const String CLASSNAME = "Sample_InstanceProviderClass";

  try
    {
      const String classname = CLASSNAME;
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
	  
	  ASSERTTEMP( cimNInstances.size() == 3);
          numberInstances =  cimNInstances.size();
	  for (Uint32 i = 0; i < cimNInstances.size(); i++)
	    {
	      String instanceRef = cimNInstances[i].getPath ().toString();
	      
	      //ATTN P2 WO 4 April 2002
	      // Test for INSTANCE0..2 when getInstanceName returns
	      // the full reference string
	      
	      if( !(String::equal(  instanceRef, CLASSNAME ) ) )
		{
		  PEGASUS_STD(cerr) << "Error: EnumInstances failed" <<
		    PEGASUS_STD(endl);
		  return;
		}
	    }
	}
      cout << "Enumerate " << numberInstances << " instances " << testRepeat << " times" << endl;
    }
  catch(CIMClientException& e)
    {
      PEGASUS_STD(cerr) << "Error: " << e.getMessage() << PEGASUS_STD(endl);
      return;
    }
}

static Boolean verifyCertificate(CertificateInfo &certInfo)
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

		 {"version", "false", false, Option::BOOLEAN, 0, 0, "v",
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
    catch (CIMClientException& e)
    {
		 cerr << argv[0] << ": " << e.getMessage() << endl;
		 exit(1);
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

    String localNameSpace;
    om.lookupValue("namespace", localNameSpace);
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
		char* repeatsStr = repeats.allocateCString();
		repeatTestCount = atol(repeatsStr);
		delete [] repeatsStr;
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

#if !defined(PEGASUS_OS_ZOS) && ! defined(PEGASUS_OS_HPUX)
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
		     CIMClient client(60 * 1000);
		     cout << "Client created" << endl;
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
                            SSLContext * sslcontext = 
                                new SSLContext(certpath, verifyCertificate, randFile, true);

                            cout << "connecting to " << connectionList[i] << " using SSL" << endl;
		            client.connect(connectionList[i], sslcontext, userName, password);
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
						   client.connect(connectionList[i], userName, password);
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
		  catch(CIMClientException& e)
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
