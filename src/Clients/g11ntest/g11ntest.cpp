//%/////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001, 2002, 2003 BMC Software, Hewlett-Packard Company, IBM,
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
// Author: Chuck Carmack (carmack@us.ibm.com)
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

#include <Pegasus/Common/AcceptLanguages.h>
#include <Pegasus/Common/ContentLanguages.h>

PEGASUS_USING_PEGASUS;
PEGASUS_USING_STD;

#define TESTMACRO(EXCTYPE) throw new EXCTYPE;

//#define MYASSERT ASSERTTEMP
#define MYASSERT PEGASUS_ASSERT

static char* programVersion =  "1.0";

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

// l10n start

/*
   Tests the globalization support of the LocalizedProvider
   for the instance operations
*/

static void TestLocalizedInstances( CIMClient& client,
								    Boolean verboseTest )
{

  const CIMNamespaceName NAMESPACE = CIMNamespaceName ("root/SampleProvider");
  const CIMName CLASSNAME = CIMName ("Sample_LocalizedProviderClass");
  const String INSTANCE0 = "instance 0Sample_LocalizedProviderClass";
  const String INSTANCE1 = "instance 1Sample_LocalizedProviderClass";
  const String RBPROP = "ResourceBundleString";
  const String ROUNDTRIPPROP = "RoundTripString";
  const String CLPROP = "ContentLanguageString";
  const String IDPROP = "Identifier";

  ContentLanguages CL_DE("de");
  String expectedDEString = "ResourceBundleString DE";
  ContentLanguages CL_FR("fr");
  String expectedFRString = "ResourceBundleString FR";		
  ContentLanguages CL_Dft = ContentLanguages::EMPTY;
  String expectedDftString = "ResourceBundleString DEFAULT";

  ContentLanguages CL_ES("es");  
  
// l10n TODO - put back UTF16 when that is ready
/*
Char16 utf16Chars[] = 
			{0x6A19, 
			0x6E96,
			0x842C, 
			0x570B,
			0x78BC,
			0xdbc0,
			0xdc01,
			0x00};
*/
Char16 utf16Chars[] = 
			{'a', 
			'b',
			0x00};	
						
  String expectedUTF16String(utf16Chars);			

  try
    {
      const CIMName className = CLASSNAME;
      Boolean deepInheritance = true;
      Boolean localOnly = true;
      Boolean includeQualifiers = false;
      Boolean includeClassOrigin = false;
      Uint32 testRepeat = 1;
      Uint32 numberInstances;

	  if (verboseTest)
	  	cout << "Getting class: " << CLASSNAME << endl;	

      CIMClass sampleClass = client.getClass(
			NAMESPACE,
			CLASSNAME,
			localOnly,
			includeQualifiers,
			includeClassOrigin);
	
	  // Note:  When Pegasus has external message files enabled,
	  // then the provider has message files for fr, de,
	  // and es, but no root message file.  The missing root
	  // message file will cause us to test the default message
	  // strings compiled into the code.	
 
      for (Uint32 i = 0; i < testRepeat; i++)        // repeat the test x time
	  {
	  	//
	  	//  TEST 1 - Enumerate Instances.  de is supported by
	  	//  the provider, and is the most preferred by us.
	  	//
	  	
		AcceptLanguages acceptLangs1;
		acceptLangs1.add(AcceptLanguageElement("fr", 0.5));
		acceptLangs1.add(AcceptLanguageElement("de", 0.8)); 
		acceptLangs1.add(AcceptLanguageElement("es", 0.4));    	  	
		client.setRequestAcceptLanguages(acceptLangs1);
        	
  		cout << "INSTANCE TEST 1: Enumerate Instances with AcceptLanguages = " << acceptLangs1 << endl;	        	
        	
   		Array<CIMInstance> cimNInstances =
	    client.enumerateInstances(NAMESPACE,  className, deepInheritance,
				      localOnly,  includeQualifiers,
				      includeClassOrigin );
				      
		cout << "Found " << cimNInstances.size() << " Instances of " << className << endl;
	  	MYASSERT(cimNInstances.size() == 2);

#ifdef PEGASUS_HAS_MESSAGES	
		cout << "Checking expected response ContentLanguages: " << CL_DE << endl;
		MYASSERT(CL_DE == client.getResponseContentLanguages());
#else
		cout << "Checking expected response ContentLanguages: " << CL_Dft << endl;
		MYASSERT(CL_Dft == client.getResponseContentLanguages());
#endif		

		cout << "Checking the translated string in each instance" << endl;

  		for (Uint32 i = 0; i < cimNInstances.size(); i++)
	    {
			String enumString;
	        cimNInstances[i].getProperty (
	        	cimNInstances[i].findProperty(RBPROP)).
    	        getValue().
        	    get(enumString);

#ifdef PEGASUS_HAS_MESSAGES        	    
			MYASSERT(expectedDEString == enumString);  
#else
			MYASSERT(expectedDftString == enumString);
#endif			      	        			
	    }
	    
	  	//
	  	//  TEST 2 - Get Instance, check default language returned.
	  	//  None of our preferred languages is supported by the 
	  	//  provider, and there is no root message file.  Expect
	  	//  the compiled-in default strings from the provider.
	  	//	    
	    
		AcceptLanguages acceptLangs2;
		acceptLangs2.add(AcceptLanguageElement("x-martian",0.8));
		acceptLangs2.add(AcceptLanguageElement("x-pig-latin", 0.1)); 
		acceptLangs2.add(AcceptLanguageElement("x-men", 0.4));    	  	
		client.setRequestAcceptLanguages(acceptLangs2);	    

  		cout << "INSTANCE TEST 2: Get Instance with AcceptLanguages = " << acceptLangs2 << endl;	        		    
		
		CIMInstance instance2 = client.getInstance(
				NAMESPACE,
				cimNInstances[1].buildPath(sampleClass),
				localOnly,
				includeQualifiers,
				includeClassOrigin);

		String returnedString;
		instance2.getProperty (
				instance2.findProperty(RBPROP)).
    	        getValue().
        	    get(returnedString);

		cout << "Checking for default language returned" << endl;

		MYASSERT(expectedDftString == returnedString); 
		
	  	//
	  	//  TEST 3 - Get Instance, check fr language returned.
	  	//  fr is the lowest preferred language from us, but
	  	//  is the only one in the list supported by the provider.
	  	//	   		
	  	
		AcceptLanguages acceptLangs3;
		acceptLangs3.add(AcceptLanguageElement("x-martian", 0.8));
		acceptLangs3.add(AcceptLanguageElement("fr", 0.1)); 
		acceptLangs3.add(AcceptLanguageElement("x-men", 0.4));    	  	
		client.setRequestAcceptLanguages(acceptLangs3);	  		
		
  		cout << "INSTANCE TEST 3: Get Instance with AcceptLanguages = " << acceptLangs3 << endl;	        		    		
		
		CIMInstance instance3 = client.getInstance(
				NAMESPACE,
				cimNInstances[0].buildPath(sampleClass),
				localOnly,
				includeQualifiers,
				includeClassOrigin);

		instance3.getProperty (
				instance3.findProperty(RBPROP)).
    	        getValue().
        	    get(returnedString);		

#ifdef PEGASUS_HAS_MESSAGES		
		cout << "Checking for fr returned" << endl;	
		MYASSERT(CL_FR == client.getResponseContentLanguages());
		MYASSERT(expectedFRString == returnedString); 		
#else
		cout << "Checking for default language returned" << endl;
		MYASSERT(CL_Dft == client.getResponseContentLanguages());
		MYASSERT(expectedDftString == returnedString); 		
#endif			 
	
	  	//
	  	//  TEST 4 - Round trip Test. 
	  	//  Create Instance followed by Get Instance.
	  	//
	  	//  Create with an fr string and expect that
	  	//  the fr string is preserved on get instance.
	  	//  (fr is one of the supported languages of the provider)
	  	//
	  	//  Create with a UTF-16 string and expect that the
	  	//  UTF-16 string is preserved on get instance
	  	//	  	
	
 		String oui = "Oui";
    	CIMInstance frInstance(CLASSNAME);
	    frInstance.addProperty(CIMProperty(
	    							CIMName(CLPROP),
		    						oui));
	    frInstance.addProperty(CIMProperty(
	    							CIMName(ROUNDTRIPPROP),
		    						String(utf16Chars)));
	    frInstance.addProperty(CIMProperty(
	    							CIMName(IDPROP),
		    						Uint8(2)));			    								    						

	    CIMObjectPath frInstanceName = frInstance.buildPath(sampleClass);
	    
	    client.setRequestContentLanguages(CL_FR);
	    
 		cout << "INSTANCE TEST 4: Create Instance with ContentLanguages = " << CL_FR
 		     << "  and UTF-16 string." << endl;	        		    			    
	    
	    client.createInstance(NAMESPACE, frInstance);
	
		AcceptLanguages acceptLangs4;
		acceptLangs4.add(AcceptLanguageElement("x-martian", 0.8));
		acceptLangs4.add(AcceptLanguageElement("fr", 0.1)); 
		acceptLangs4.add(AcceptLanguageElement("x-men", 0.4));    	  	
		client.setRequestAcceptLanguages(acceptLangs4);		

 		cout << "Getting the instance with AcceptLanguages = " << acceptLangs4
 		     << endl;	   	
    						
		CIMInstance instance4 = client.getInstance(
				NAMESPACE,
				frInstanceName,
				localOnly,
				includeQualifiers,
				includeClassOrigin);	    						

		instance4.getProperty (
				instance4.findProperty(ROUNDTRIPPROP)).
    	        getValue().
        	    get(returnedString);

 		cout << "Checking the UTF-16 string was preserved" << endl;	 

		MYASSERT(expectedUTF16String == returnedString); 
		
		instance4.getProperty (
				instance4.findProperty(CLPROP)).
    	        getValue().
        	    get(returnedString);

		cout << "Checking for fr returned" << endl;
		MYASSERT(oui == returnedString); 
		MYASSERT(CL_FR == client.getResponseContentLanguages());

		// Delete the instance for the next pass
		cout << "Deleting the instance" << endl;
		client.deleteInstance(
					NAMESPACE,
					frInstanceName);		
	        
	  	//
	  	//  TEST 5 - Round trip Test. 
	  	//  Modify Instance followed by Get Instance.
	  	//
	  	//  Modify instance2 from the enumerate above with an x-homer
	  	//  string and expect that the x-homer string is preserved on get
	  	//  instance.
	  	//  (x-homer is not one of the supported languages of the provider
	  	//   but still should be returned when asked for)
	  	//
	  	//  Modify instance2 with a UTF-16 string and expect that the
	  	//  UTF-16 string is preserved on get instance
	  	//	  	        
	        	
	    String doh = "doh!";
	    ContentLanguages CL_HOMER("x-homer");    	
		instance2.removeProperty (instance2.findProperty(CLPROP));
	    instance2.addProperty(CIMProperty(
	    							CIMName(CLPROP),
		    						doh));
		instance2.removeProperty (instance2.findProperty(ROUNDTRIPPROP));		    						
	    instance2.addProperty(CIMProperty(
	    							CIMName(ROUNDTRIPPROP),
		    						String(utf16Chars)));			    								
		
	    client.setRequestContentLanguages(CL_HOMER);
	    
 		cout << "INSTANCE TEST 5: Modify Instance with ContentLanguages = " << 
 				CL_HOMER << "  and UTF-16 string." << endl;	   	    
	    
	    client.modifyInstance(NAMESPACE,
	    						instance2,
	    						includeQualifiers);
	    			
	    AcceptLanguages acceptLangs5;
		acceptLangs5.add(AcceptLanguageElement("x-homer", 0.8));	    			
	    client.setRequestAcceptLanguages(acceptLangs5);
	    
 		cout << "Getting the instance with AcceptLanguages = " << acceptLangs5
 		     << endl;	 	    
	    	
		CIMInstance instance5 = client.getInstance(
				NAMESPACE,
				instance2.buildPath(sampleClass),
				localOnly,
				includeQualifiers,
				includeClassOrigin);
					 
		instance5.getProperty (
				instance5.findProperty(ROUNDTRIPPROP)).
    	        getValue().
        	    get(returnedString);
        	       		
		cout << "Checking the UTF-16 string was preserved" << endl;	         	       		
        	       						
		MYASSERT(expectedUTF16String == returnedString);
		
		instance5.getProperty (
				instance5.findProperty(CLPROP)).
    	        getValue().
        	    get(returnedString);
        	
		cout << "Checking for x-homer returned" << endl;	         	    
		MYASSERT(returnedString == doh); 
		MYASSERT(CL_HOMER == client.getResponseContentLanguages());				
		
	  	//
	  	//  TEST 6 - Exception test. 
	  	//  Expect an exception to be returned from the server
	  	//  with a localized message in es locale.
	  	//
	  	//  Note: the provider will throw a not-supported exception
	  	//  on all delete instance requests.
	  	//
	  	
		AcceptLanguages acceptLangs6;
		acceptLangs6.add(AcceptLanguageElement("es", 1.0));
		acceptLangs6.add(AcceptLanguageElement("fr", 0.9)); 
		client.setRequestAcceptLanguages(acceptLangs6);			  	
	  	
 		cout << "INSTANCE TEST 6: Delete Instance with AcceptLanguages = " << 
 				acceptLangs6 << endl;	   	  	
	  	
	  	// Try to delete instance2, expect an exception
	  	Boolean gotException = false;
	  	try 
	  	{
			client.deleteInstance(
					NAMESPACE,
					instance2.buildPath(sampleClass));	 	  	
	  	} catch (CIMException & ce)
	  	{
	  		String message = ce.getMessage();	
	  		
#ifdef PEGASUS_HAS_MESSAGES		
			cout << "Checking for es returned" << endl;	
			MYASSERT(CL_ES == client.getResponseContentLanguages());
	  		Uint32 n = message.find("ES");
	  		MYASSERT(n != PEG_NOT_FOUND);			
#else
			cout << "Checking for default language returned" << endl;
			MYASSERT(CL_Dft == client.getResponseContentLanguages());
	  		Uint32 n = message.find("default");
	  		MYASSERT(n != PEG_NOT_FOUND);				
#endif		  		
	  		gotException = true;	
	  	}		
  		
		MYASSERT(gotException == true);		
	  }
    }
	catch(Exception& e)
    {
      PEGASUS_STD(cerr) << "Error: " << e.getMessage() << PEGASUS_STD(endl);
      return;
    }
}
// l10n end


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

		 {"repeat", "1", false, Option::WHOLE_NUMBER, 0, 0, "r",
		 		       "Specifies a Repeat Count Entire test repeated this many times" },

		 {"version", "false", false, Option::BOOLEAN, 0, 0, "version",
		 		 		 "Displays TestClient Version "},

		 {"verbose", "false", false, Option::BOOLEAN, 0, 0, "verbose",
		 		 		 "If set, outputs extra information "},

		 {"help", "false", false, Option::BOOLEAN, 0, 0, "h",
		 		     "Prints help message with command line options "},
		 		     
		 {"debug", "false", false, Option::BOOLEAN, 0, 0, "d",
		              "Not Used "},

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
    if (om.valueEquals("version","true"))
    {
        cout << argv[0] << " version " <<programVersion <<  endl;
        //cout << argv[0] << endl;
        exit(0);
    }

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
	
    if(verboseTest)
		cout << "Test repeat count " << repeatTestCount << endl;

    Boolean localConnection = (om.valueEquals("local", "true"))? true: false;
    cout << "localConnection " << (localConnection ? "true" : "false") << endl;

    Array<String> connectionList;
    if (argc > 1 && !localConnection)
		 for (Sint32 i = 1; i < argc; i++)
		     connectionList.append(argv[i]);

	 if(argc < 2)
      connectionList.append("localhost:5988");

    // Expand host to add port if not defined

	// Show the connectionlist
    cout << "Connection List size " << connectionList.size() << endl;
    for (Uint32 i = 0; i < connectionList.size(); i++)
	cout << "Connection " << i << " address " << connectionList[i] << endl;

#ifdef PEGASUS_HAS_MESSAGES	
	cout << "NOTE: Pegasus supports ICU resource bundles.  Expecting strings loaded from resource bundles." << endl;  
#else
	cout << "NOTE: Pegasus does not support ICU resource bundles.  Expecting default strings." << endl;
#endif

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
			 cout << "Client Connected" << endl;

			 testStart("Test Instance Operations");
			 elapsedTime.reset();
			 TestLocalizedInstances(client, verboseTest);
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


