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
//
//%/////////////////////////////////////////////////////////////////////////////

#include <cassert>
#include <Pegasus/Common/Selector.h>
#include <Pegasus/Client/CIMClient.h>
#include <Pegasus/Common/lslp-perl-lib.h>
#include <Pegasus/Common/OptionManager.h>
#include <Pegasus/Common/FileSystem.h>

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

/** GetOptions function - This function defines the Options Table
    and sets up the options from that table using the option manager.
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
	
	{"port", "5988", false, Option::WHOLE_NUMBER, 0, 0, "port",
			"specifies port number to listen on" },

	{"namespace", "root/cimv2", false, Option::STRING, 0, 0, "-n",
			"specifies namespace to use for test" },

	{"version", "false", false, Option::BOOLEAN, 0, 0, "v",
			"Displays Pegasus Version "},
	{"help", "false", false, Option::BOOLEAN, 0, 0, "h",
		    "Prints help message with command line options "},
	{"debug", "false", false, Option::BOOLEAN, 0, 0, "d", 
	             "Not Used "},
	{"slp", "true", false, Option::BOOLEAN, 0, 0, "d", 
			"use SLP to find cim servers to test"}
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

static void TestNameSpaceOperations(CIMClient& client)
{
    // Enumerate NameSpaces using the old technique
    String className = "__Namespace";

    // Call enumerate Instances CIM Method

    try 
    {
    cout << " Enumerate Namespaces " << className << endl;
    Array<CIMReference> instanceNames = client.enumerateInstanceNames(
	NAMESPACE, className);

    // Convert from CIMReference to String form
    Array<String> tmpInstanceNames;

    for (Uint32 i = 0; i < instanceNames.size(); i++)
	cout << instanceNames[i].toString() << endl;
    }


    // ATTN Convert this to a test.
	//tmpInstanceNames.append(instanceNames[i].toString());
    catch(Exception& e)
    {
	cout << "Error NameSpace Enumeration:" << endl;
	cout << e.getMessage() << endl;
    }


}

static void TestGetClass(CIMClient& client)
{
    CIMClass c = client.getClass(
	NAMESPACE, "CIM_ComputerSystem", false, false, true);

    c.print();
}

static void TestClassOperations(CIMClient& client)
{
    // CreateClass:

    // NOTE: We should add test for CIM_ManagedElement first.

    CIMClass c1("SubClass", "CIM_ManagedElement");
    c1.addQualifier(CIMQualifier("abstract", Boolean(true)));
    c1.addProperty(CIMProperty("count", Uint32(99)));
    c1.addProperty(CIMProperty("ratio", Real64(66.66)));
    c1.addProperty(CIMProperty("message", String("Hello World")));
    client.createClass(NAMESPACE, c1);

    // GetClass:

    CIMClass c2 = client.getClass(NAMESPACE, "SubClass", false);
    // assert(c1.identical(c2));

    // Modify the class:

    c2.removeProperty(c2.findProperty("message"));
    client.modifyClass(NAMESPACE, c2);

    // GetClass:

    CIMClass c3 = client.getClass(NAMESPACE, "SubClass", false);
    // assert(c3.identical(c2));

    // EnumerateClassNames:

    Array<String> classNames = client.enumerateClassNames(
	NAMESPACE, "CIM_ManagedElement", false);

    Boolean found = false;

    for (Uint32 i = 0; i < classNames.size(); i++)
    {
	if (CIMName::equal(classNames[i], "SubClass"))
	    found = true;
    }

    assert(found);

    // DeleteClass:

    client.deleteClass(NAMESPACE, "SubClass");

    // Get all the classes;


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

static void TestQualifierOperations(CIMClient& client)
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

static void TestInstanceOperations(CIMClient& client)
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
//    MAIN
///////////////////////////////////////////////////////////////

int main(int argc, char** argv)
{
  LSLP_LIB_SRVRPLY *replies = NULL, *thisReply = NULL;
  char* connection = "localhost:5988";
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
    
    Boolean useSLP;
    if(om.valueEquals("slp", "true")) 
      useSLP = true;

    if(useSLP == false && argc > 1)
      connection= argv[1];
    else if ( lslp_lib_init("pegasus_cim_client", 
			    "239.255.255.253",   
			    "0.0.0.0", 
			    427, NULL ) ) // try slp
      { 
	
	replies = (LSLP_LIB_SRVRPLY *)lslp_lib_converge_srv_req("pegasus_cim_client",
								"service:cim.pegasus",
								"(namespace=*)",
								"DEFAULT");
	if(replies != NULL) 
	  {
	    thisReply = replies->next;
	    if  (! thisReply->isHead ) 
	      {
		connection = (address_string = lslp_lib_get_addr_string_from_url(thisReply->url) );
		thisReply = thisReply->next;
	      }
	  }
      }
    
    do {

      if(connection == NULL)
	continue;

      try
	{
	  cout << "connecting to " << connection << endl;
	  Selector selector;
	  CIMClient client(&selector);
	  client.connect(connection);
	  testStatus("Test NameSpace Operations");
	  TestNameSpaceOperations(client);
	  
	  testStatus("Test Qualifier Operations");
	  TestQualifierOperations(client);
	  testStatus("Test Class Operations");
	  TestClassOperations(client);
	  testStatus("Test Instance Operations");
	  TestInstanceOperations(client);
	  
	}
      catch(Exception& e)
	{
	  PEGASUS_STD(cerr) << "Error: " << e.getMessage() << PEGASUS_STD(endl);
	  //	exit(1);
	}
      

      if( thisReply != NULL && ( ! thisReply->isHead ) )
	{
	  // reinitialize the connection string
	  connection = (address_string = lslp_lib_get_addr_string_from_url(thisReply->url) );
	  thisReply = thisReply->next;
	}
      else
	connection = NULL;
    
    } while ( connection != NULL ); 

    PEGASUS_STD(cout) << "+++++ passed all tests" << PEGASUS_STD(endl);
    lslp_lib_deinit("pegasus_cim_client");
    return 0;
}

/*
    TODO:  1. put in the option manager
           2. Make passive tests only option.
	   3. Make test loop tool
	   4. Make display an option
	   5. Make test multiple systems.
	   6.
*/
