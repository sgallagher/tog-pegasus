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
//         Mike Day (mdday@us.ibm.com)
//         Jenny Yu (jenny_yu@hp.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#include <Pegasus/Common/Config.h>
#include <cassert>
#include <Pegasus/Common/Monitor.h>
#include <Pegasus/Common/TLS.h>
#include <Pegasus/Common/HTTPConnector.h>
#include <Pegasus/Client/CIMClient.h>
#include <Pegasus/Common/OptionManager.h>
#include <Pegasus/Common/FileSystem.h>
#include <Pegasus/Common/Stopwatch.h>
#ifndef PEGASUS_OS_ZOS
#include <slp/slp.h>
#endif

PEGASUS_USING_PEGASUS;
PEGASUS_USING_STD;


String NAMESPACE = "root/cimv2";

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
static void testStatus(const String& message)
{
    cout << message << endl;
}
/*****************************************************************
//    Testing Namespace operations
******************************************************************/

static void TestNameSpaceOperations(CIMClient& client, Boolean activeTest, 
		 		 Boolean verboseTest) 
{
    // Enumerate NameSpaces using the old technique
    String className = "__Namespace";

    // Call enumerate Instances CIM Method

    try 
    {
		 // cout << " Enumerate Namespaces " << className << endl;
		 Array<CIMReference> instanceNames = client.enumerateInstanceNames(
		     NAMESPACE, className);
    
		 cout << instanceNames.size() << " Namespaces" << endl;
		 // Convert from CIMReference to String form
    
		 if (verboseTest)
		 {
		     Array<String> tmpInstanceNames;
    
		     for (Uint32 i = 0; i < instanceNames.size(); i++)
		 		 cout << instanceNames[i].toString() << endl;
		     }
		 }
    
    
		 // ATTN Convert this to a test.
		 //tmpInstanceNames.append(instanceNames[i].toString());
    catch(Exception& e)
    {
		 cout << "Error NameSpace Enumeration:" << endl;
		 cout << e.getMessage() << endl;
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
		     NAMESPACE, className, deepInheritance);

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
		 NAMESPACE, "CIM_ComputerSystem", false, false, true);

    c.print();
}

static void TestClassOperations(CIMClient& client, Boolean ActiveTest, 
		 		 		     Boolean verboseTest) {
    
    // Name of Class to use in create/delete test
    String testClass = "PEG_TestSubClass";
    // NOTE: We should add test for CIM_ManagedElement first.

    //Test for class already existing
    Array<String> classNames = client.enumerateClassNames(
		 NAMESPACE, "CIM_ManagedElement", false);

    for (Uint32 i = 0; i < classNames.size(); i++)
    {
		 if (CIMName::equal(classNames[i], testClass))
		     client.deleteClass(NAMESPACE, testClass);
    }
    
    // CreateClass:

    CIMClass c1(testClass, "CIM_ManagedElement");
    c1.addQualifier(CIMQualifier("abstract", Boolean(true)));
    c1.addProperty(CIMProperty("count", Uint32(99)));
    c1.addProperty(CIMProperty("ratio", Real64(66.66)));
    c1.addProperty(CIMProperty("message", String("Hello World")));
    client.createClass(NAMESPACE, c1);

    // GetClass:

    CIMClass c2 = client.getClass(NAMESPACE, testClass, false);
    if (!c1.identical(c2))
		 cout << "Class SubClass Returned not equal to created" << endl;
    //assert(c1.identical(c2));

    // Modify the class:

    c2.removeProperty(c2.findProperty("message"));
    client.modifyClass(NAMESPACE, c2);

    // GetClass:

    CIMClass c3 = client.getClass(NAMESPACE, testClass, false);

    if (!c3.identical(c2))
		 cout << "Test Failed. Rtned class c3 not equal to c2" << endl;;

    // Determine if the new Class exists in Enumerate

    classNames = client.enumerateClassNames(
		 NAMESPACE, "CIM_ManagedElement", false);

    Boolean found = false;

    for (Uint32 i = 0; i < classNames.size(); i++)
    {
		 if (CIMName::equal(classNames[i], testClass))
		     found = true;
    }

    assert(found);

    // DeleteClass:

    client.deleteClass(NAMESPACE, testClass);

    // Get all the classes and compare enum names with enum classes

    classNames = client.enumerateClassNames(NAMESPACE, String(), false);

    Array<CIMClass> classDecls = client.enumerateClasses(
		 NAMESPACE, String(), false, false, true, true);

    assert(classDecls.size() == classNames.size());

    for (Uint32 i = 0; i < classNames.size(); i++)
    {
		 CIMClass tmp = client.getClass(
		     NAMESPACE, classNames[i], false, true, true);

		 assert(CIMName::equal(classDecls[i].getClassName(), classNames[i]));

		 assert(tmp.identical(classDecls[i]));
    }

}

static void TestQualifierOperations(CIMClient& client, Boolean activeTest, 
		 		 		 Boolean verboseTest) {
    
    Array<CIMQualifierDecl> qualifierDecls = client.enumerateQualifiers(NAMESPACE);

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
		 client.setQualifier(NAMESPACE, qd1);
    
		 CIMQualifierDecl qd2("qd2", "Hello", CIMScope::PROPERTY | CIMScope::CLASS, 
		     CIMFlavor::OVERRIDABLE);
		 client.setQualifier(NAMESPACE, qd2);
    
		 // Get them and compare:
    
		 CIMQualifierDecl tmp1 = client.getQualifier(NAMESPACE, "qd1");
		 assert(tmp1.identical(qd1));
    
		 CIMQualifierDecl tmp2 = client.getQualifier(NAMESPACE, "qd2");
		 assert(tmp2.identical(qd2));
    
		 // Enumerate the qualifiers:
    
		 Array<CIMQualifierDecl> qualifierDecls = client.enumerateQualifiers(NAMESPACE);
    
		 for (Uint32 i = 0; i < qualifierDecls.size(); i++)
		 {
		     CIMQualifierDecl tmp = qualifierDecls[i];
    
		     if (CIMName::equal(tmp.getName(), "qd1"))
		 		 assert(tmp1.identical(tmp));
    
		     if (CIMName::equal(tmp.getName(), "qd2"))
		 		 assert(tmp2.identical(tmp));
		 }
    
		 // Delete the qualifiers:
    
		 client.deleteQualifier(NAMESPACE, "qd1");
		 client.deleteQualifier(NAMESPACE, "qd2");

    }

}

static void TestInstanceGetOperations(CIMClient& client, Boolean activeTest,
		 		 		 		    Boolean verboseTest)
{
    // Get all instances
    // Get all classes

    //Array<String> classNames = client.enumerateClassNames(
    //    NAMESPACE, "CIM_ManagedElement", false);
    
    Array<String> classNames = client.enumerateClassNames(
		 NAMESPACE, String(), true);

    cout <<  classNames.size() << " Classes found " << endl;

    Array<CIMReference> instanceNames;

    for (Uint32 i = 0; i < classNames.size(); i++)
    {
       if (classNames[i] == "PG_ShutdownService")
       {
           // Skip the PG_ObjectManager class.  It currently has no 
           // instance provider and no instances.  
       }
       else
       {
           instanceNames = client.enumerateInstanceNames(NAMESPACE,classNames[i]);
           if (instanceNames.size() > 0)
		   cout << "Class " << classNames[i] << " " 
 		        << instanceNames.size() << " Instances" << endl;
       }
    }
    /*
    virtual Array<CIMReference> enumerateInstanceNames(
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
    // Delete the class if it already exists:

    try
    {
		 client.deleteClass(NAMESPACE, "myclass");
    }
    catch (Exception&)
    {
		 // Ignore delete class!
    }

#if 0
    // Create a new class:

    CIMClass cimClass("myclass");
    cimClass
		 .addProperty(CIMProperty("last", String())
		     .addQualifier(CIMQualifier("key", true)))
		 .addProperty(CIMProperty("first", String())
		     .addQualifier(CIMQualifier("key", true)))
		 .addProperty(CIMProperty("age", Uint8(0))
		     .addQualifier(CIMQualifier("key", true)));
    client.createClass(NAMESPACE, cimClass);

    // Create an instance of that class:

    CIMInstance cimInstance("myclass");
    cimInstance.addProperty(CIMProperty("last", "Smith"));
    cimInstance.addProperty(CIMProperty("first", "John"));
    cimInstance.addProperty(CIMProperty("age", Uint8(101)));
    String instanceName = cimInstance.getInstanceName(cimClass);
    client.createInstance(NAMESPACE, cimInstance);

    // Get the instance and compare with created one:

    CIMReference ref;
    CIMReference::instanceNameToReference(instanceName, ref);
    CIMInstance tmp = client.getInstance(NAMESPACE, ref);

    // cimInstance.print();
    // tmp.print();
    // assert(cimInstance.identical(tmp));
#endif
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
    {
		 {"active", "false", false, Option::BOOLEAN, 0, 0, "a",
		 		 		 "If set allows test that modify the repository" },
		 
		 
		 {"repeat", "1", false, Option::WHOLE_NUMBER, 0, 0, "r",
		 		 		 "specifies port number to listen on" },
		 
		 {"namespace", "root/cimv2", false, Option::STRING, 0, 0, "-n",
		 		 		 "specifies namespace to use for test" },

		 {"version", "false", false, Option::BOOLEAN, 0, 0, "v",
		 		 		 "Displays TestClient Version "},

		 {"verbose", "false", false, Option::BOOLEAN, 0, 0, "verbose",
		 		 		 "Displays Pegasus Version "},

		 {"help", "false", false, Option::BOOLEAN, 0, 0, "h",
		 		     "Prints help message with command line options "},
		 {"debug", "false", false, Option::BOOLEAN, 0, 0, "d", 
		              "Not Used "},
		 {"slp", "false", false, Option::BOOLEAN, 0, 0, "slp", 
		 		 		 "use SLP to find cim servers to test"},
		 {"ssl", "false", false, Option::BOOLEAN, 0, 0, "ssl", 
		 		 		 "use SSL"}
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

/* PrintHelp - This is temporary until we expand the options manager to allow
   options help to be defined with the OptionRow entries and presented from
   those entries.
*/
void PrintHelp(const char* arg0)
{
    cout << '\n';
    cout << "TestClient" << endl;
    cout << '\n';
    cout << "Usage: " << arg0 << endl;
    cout << endl;
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
		 // om.print();
    }
    catch (Exception& e)
    {
		 cerr << argv[0] << ": " << e.getMessage() << endl;
		 exit(1);
    }


    String localNameSpace;
    if(om.lookupValue("namespace", localNameSpace))
      {
       NAMESPACE = localNameSpace;
       cout << "Namespace = " << localNameSpace << endl;

      }
    cout << "Namespace = " << localNameSpace << endl;
    
    cout << "Namespace = " << NAMESPACE << endl;

    // Check to see if user asked for help (-h otpion):
    String helpOption;

    if (om.valueEquals("verbose", "true"))
    {
		 PrintHelp(argv[0]);
		 om.printHelp();
		 exit(0);
    }

    Boolean verboseTest = (om.valueEquals("verbose", "true")) ? true :false;

    Boolean activeTest = false;
    if (om.valueEquals("active", "true"))
		 activeTest = true;
     
    // here we determine the list of systems to test.
    // All arguments remaining in argv go into list.
    // if SLP option set, SLP list goes into set.
    // if SLP false and no args, use default localhost:5988
    Boolean useSLP =  (om.valueEquals("slp", "true"))? true: false;
    cout << "SLP " << (useSLP ? "true" : "false") << endl;
    Array<String> connectionList;
    if (argc > 1)
		 for (Sint32 i = 1; i < argc; i++)
		     connectionList.append(argv[i]);

    // substitute the default only if noslp and no params
    if(useSLP == false && argc < 2)
      connectionList.append("localhost:5988");

#ifndef PEGASUS_OS_ZOS
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
    Boolean useSSL =  (om.valueEquals("ssl", "true"))? true: false;
    

    cout << "Connection List size " << connectionList.size() << endl;
    for (Uint32 i = 0; i < connectionList.size(); i++)
		 cout << "Connection " << i << " address " << connectionList[i] << 
		 		 		 		 		 		     endl; 

    
    for (Uint32 i = 0; i < connectionList.size(); i++)
    {

      try
      {
	   Stopwatch elapsedTime;
	   Monitor* monitor = new Monitor;
	   //HTTPConnector* connector = new HTTPConnector(monitor);

	   HTTPConnector* connector;
           if (useSSL)
           {
               String certpath("/home/markus/src/pegasus/server.pem");
               SSLContext * sslcontext = new SSLContext(certpath);
	       connector = new HTTPConnector(monitor, sslcontext);
           }
           else
	       connector = new HTTPConnector(monitor);

	   CIMClient client(monitor, connector, 60 * 1000);

	   char * connection = connectionList[i].allocateCString();
	   cout << "connecting to " << connection << endl;
	   client.connect(connection);
	   delete [] connection;

	   testStatus("Test NameSpace Operations");

	   TestNameSpaceOperations(client, activeTest, verboseTest);
	   cout << " in " << elapsedTime.getElapsed() << " Seconds\n";
		   
	   testStatus("Test Qualifier Operations");
	   elapsedTime.reset();
	   TestQualifierOperations(client,activeTest,verboseTest);
	   cout << " in " << elapsedTime.getElapsed() << " Seconds\n";

	   testStatus("Test EnumerateClassNames");
	   elapsedTime.reset();
	   TestEnumerateClassNames(client,activeTest,verboseTest);
	   cout << " in " << elapsedTime.getElapsed() << " Seconds\n";


	   testStatus("Test Class Operations");
	   elapsedTime.reset();
	   TestClassOperations(client,activeTest,verboseTest);
	   cout << " in " << elapsedTime.getElapsed() << " Seconds\n";
	   elapsedTime.printElapsed();

	   testStatus("Test Instance Get Operations");
	   elapsedTime.reset();
	   TestInstanceGetOperations(client,activeTest,verboseTest);
	   cout << " in " << elapsedTime.getElapsed() << " Seconds\n";

	   testStatus("Test Instance Modification Operations");
	   elapsedTime.reset();
	   TestInstanceModifyOperations(client, activeTest, verboseTest);
	   cout << " in " << elapsedTime.getElapsed() << " Seconds\n";

	   testStatus("Test Associations");
		   
      }
      catch(Exception& e)
		 {
		   PEGASUS_STD(cerr) << "Error: " << e.getMessage() << PEGASUS_STD(endl);
		   exit(1);
		 }
    }
      
    PEGASUS_STD(cout) << "+++++ passed all tests" << PEGASUS_STD(endl);
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
