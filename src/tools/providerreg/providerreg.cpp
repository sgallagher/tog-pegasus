 //%/////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000 The Open Group, BMC Software, Tivoli Systems, IBM
//
// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation
// the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following conditions:
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
// THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.
//
//==============================================================================
//
// Author: Karl Schopmeyer (k.schopmeyer@opengroup.org)
//
// Modified By:
//
//%/////////////////////////////////////////////////////////////////////////////

#include <cassert>
#include <fstream>
#include <fstream>
#include <iostream>
#include <iomanip>

#include <Pegasus/Repository/CIMRepository.h>
//#include <Pegasus/Client/CIMClient.h>
//#include <Pegasus/Common/Selector.h>
#include <Pegasus/Common/OptionManager.h>
#include <Pegasus/Common/FileSystem.h>


using namespace Pegasus;
using namespace std;


/** RegisterProvider - Sets Registration for Provider
This executable sets the appropriate registration parameters
to register a provider or unregister a provider.
It can be used to register a provider against a single class and
define it as a property, method, or instance provider.

It can be used either to register or unregister the provider.

The parameters of the call are

Options
    -U - If set, tries to unregister the provider (default is register)
    -D - Simply show the current state of registration
    -M - Method provider registraion
    -P - Property provider registration
    -V - View all current provider registrations
    
Class - CIMReference with the Namespace and classname to register.

Method or property name.

Registration type is defined through the CIMReference.
If the reference is a class name only, the registration is on the
class.  If it includes a method name, it is on the method, etc.
*/

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

void GetOptions(
    OptionManager& om,
    int& argc, 
    char** argv, 
    const String& pegasusHome)
{
    static struct OptionRow options[] =
    {
	{"unregister", "false", false, Option::BOOLEAN, 0, 0, "u"},
	{"display", "false", false, Option::BOOLEAN, 0, 0, "d"},
	{"method", "false", false, Option::BOOLEAN, 0, 0, "m"},
	{"property", "false", false, Option::BOOLEAN, 0, 0, "p"},
	{"version", "false", false, Option::BOOLEAN, 0, 0, "v"},
	{"help", "false", false, Option::BOOLEAN, 0, 0, "h"}
    };
    const Uint32 NUM_OPTIONS = sizeof(options) / sizeof(options[0]);

    om.registerOptions(options, NUM_OPTIONS);

    String configFile = pegasusHome + "/cimserver.conf";

    if (FileSystem::exists(configFile))
	om.mergeFile(configFile);

    om.mergeCommandLine(argc, argv);

    om.checkRequiredOptions();
}

void PrintHelp(const char* arg0)
{
    Uint32 PROGRAM_VERSION = 1;
    cout << '\n';
    cout << "Provider Registration  Version " << PROGRAM_VERSION << endl;
    cout << '\n';
    cout << "Usage: " << arg0 << " [options] [namespace] [class]"; 
    cout << " [providerID]\n" << '\n';
    cout << " Registers ProviderIDs for classes in repository\n\n";
    cout << "    -h prints this help message\n";
    cout << "    -u unregister the class defined from the provider\n";
    cout << "    -m method registration (Not Enabled)\n";
    cout << "    -p property registration provider (Not Enabled)\n";
    cout << "    -v print out the program version number\n";
    cout << " 	Parameters required are\n";
    cout << "         Namespace ClassName  ProviderID\n";
    cout << "\n providerreg root/cimv2 process MyProvider\n";
    cout << "            registers class process to provider MyProvider \n";
    cout << "\n providerreg -u root/cimv2 process\n";
    cout << "            unregisters class process\n";
    cout << "   NOTE: unregistration does not require providerID param\n";
    cout << endl;
}



// Functions to do
void registerInstanceProvider(
	    CIMRepository& r, 
	    Boolean registering,
	    String& nameSpace,
	    String& className, 
	    String& providerName)
{
      CIMClass c1;
    
    // Create the new qualifier
    try
    {
      
	c1 = r.getClass(
	    nameSpace, className, false, false, true);
	/*
	virtual CIMClass getClass(
	const String& nameSpace,
	const String& className,
	Boolean localOnly = true,
	Boolean includeQualifiers = true,
	Boolean includeClassOrigin = false,
	const Array<String>& propertyList = EmptyStringArray());
	*/
        // CIMClass c2 = client.getClass(NAMESPACE, "SubClass", false);
	/*
	CIMConstClass cc1;
	cc1 = r.getClass(NAMESPACE, "MyClass");
	*/

    }
    catch (Exception& e)
    {
	// Catch when class does not exist.
	cout << "Class does not exist " << e.getMessage() << endl;
    }

    if (registering)
    {
	//test if already registered

	cout << "registering " << className << " for " 
		<< providerName << endl;
	 if (c1.existsQualifier("Provider" ))
	    {
		// NOTE that we should modify   
		cout << "Provider exists" << endl;
		Uint32 pos;
		pos = c1.findQualifier("Provider");
		if (pos != PEG_NOT_FOUND)
		    c1.removeQualifier(pos);
	    }
	c1.addQualifier(CIMQualifier("Provider", providerName));
	cout << "Provider " << providerName << " changed." << endl;
	r.modifyClass(nameSpace, c1);

}
    else  // Here we deregister
    {
	// test not registered
	cout << "unregistering "<< endl;
	Uint32 qualifierIndex;
	qualifierIndex = c1.findQualifier("Provider");
	if (qualifierIndex != PEG_NOT_FOUND)
	{
	    c1.removeQualifier(qualifierIndex);
	}
	else
	{
	    // Does not exist.  Issue message
	    cout << "Not Registered" << endl;
	}

    }
}

void registerMethodProvider(
                   CIMRepository& r, 
                   Boolean toRegister,
                   String& nameSpace,
                   String& className, 
                   String& providerName)

{
    CIMClass c = r.getClass(
	nameSpace, "CIM_ComputerSystem", false, false, true);

    c.print();
}

void registerPropertyProvider(
		    CIMRepository& r, 
		    Boolean toRegister,
		    String& nameSpace,
		    String& className, 
		    String& providerName)

{
    CIMClass c = r.getClass(
	nameSpace, "CIM_ComputerSystem", false, false, true);

    c.print();
}

void showOneProviderRegistration(
    CIMRepository& r,
    String& nameSpace,
    CIMClass& c1,
    String& providerName)
{
    try
    {
      if (c1.existsQualifier("provider"))
      {
	  Uint32 pos;
	  pos = c1.findQualifier("provider");

	  if (pos != PEG_NOT_FOUND)
	  {
	      // Display the qualifier.
	      CIMQualifier q1 = c1.getQualifier(pos);

	      String providerId;
	      q1.getValue().get(providerId);
	      String fill = "                       ";
	      String name = c1.getClassName();
	      cout << name
  		   << fill.subString(0, 20-name.size())
		   << providerId
	           << endl;
	  }
      }
    }
    catch (Exception& e)
    {
	// Catch when class does not exist.
	cout << "Class does not exist " << e.getMessage() << endl;
    }


}

void showProviderRegistrations (
		    CIMRepository& r,
		    String& nameSpace,
		    String& className,
		    String& providerName)
{
    
    cout << "Show Provider Registrations for Namespace " 
	 << nameSpace << endl;
    try
    {
	
	/* enumerateClasses
	virtual Array<CIMClass> enumerateClasses(
	    const String& nameSpace,
	    const String& className = String::EMPTY,
	    Boolean deepInheritance = false,
	    Boolean localOnly = true,
	    Boolean includeQualifiers  = true,
	    Boolean includeClassOrigin = false);
	*/
	cout << "Class               Provider" << endl;
	cout << "----------------------------" << endl;


	Uint32 providerCount = 0;
	Array<CIMClass> classList = r.enumerateClasses(
		nameSpace, className, true, false, true);


	for (Uint32 i = 0, n = classList.size(); i < n; i++)
	{
	    if (classList[i].existsQualifier("provider"))
	    {
		showOneProviderRegistration(
			    r,
			    nameSpace,
			    classList[i],
			    providerName);
		providerCount++;
	    }
	}
    	cout << endl << providerCount << " Classes found" << endl;

    }
    catch (Exception& e)
    {
	// Catch when class does not exist.
	cout << "Class does not exist " << e.getMessage() << endl;
    }


}

// Questions: if registered already for class, do we have to
// register for property or method?
//

 int main(int argc, char** argv)
{
    
    // Get the parameters from the command line
    // arg2 is objectpath
    // arg3 is provider
    // make - parameter to deregister -d
    if (argc < 2)
	ErrorExit("unexpected command line arguments");
    Boolean toRegister = true;
    String className;
    className = "Process";
    String providerName;
    providerName = "MyProvider";
    String nameSpace;
    nameSpace = "root/cimv2";
    String pegasusHome;

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

    String versionOption;
    if (om.lookupValue("version", versionOption) && versionOption == "true")
    {
	cerr << "Registration Version 1.0" << endl;
	exit(0);
    }

    // Check to see if user asked for help (-h otpion):

    String helpOption;

    if (om.lookupValue("help", helpOption) && helpOption == "true")
    {
	PrintHelp(argv[0]);
	exit(0);
    }
    // At this point, all options should have been extracted; print an
    // error if there are any remaining:

    //if (argc != 1)
    //{
    //    cerr << argv[0] << ": unrecognized options: ";
    //
    //    for (int i = 1; i < argc; i++)
    //        cerr << argv[i] << ' ';
    //    cout << endl;
    //    // exit(1);
    //}

    try
    {
	const char *tmp = getenv("PEGASUS_HOME");
	if (!tmp)
	    ErrorExit("PEGASUS_HOME environment variable missing");

        pegasusHome = tmp;
	FileSystem::translateSlashes(pegasusHome);
	
	String repositoryDir = pegasusHome;
	repositoryDir += "/repository";

	// Test for legal repository here.

	CIMRepository r(repositoryDir);

	// Process the display option
	if (om.valueEquals("display", "true")) 
	{
	    if (argc > 1)
		nameSpace = argv[1];
	    if (argc == 2)
		className = argv[2];
	    else className = "";
	    showProviderRegistrations (
		    r, nameSpace, className,providerName);
	}
	else
	{   
	    if (argc < 2)
		ErrorExit("Namespace and classname required");
	    nameSpace = argv[1];
	    className = argv[2];
	    if (om.lookupValue("unregister", helpOption) && helpOption == 
		    "false")
	    {
		// if registering, get the provider name parameter
		if (argc < 3)
		    ErrorExit("Provider name parameter required");
		else
		{
     		    providerName = argv[3];
		}
	    }
	    else  // set to unregister
		toRegister = false;

	    cout << (toRegister ? "Registering " :  "Unregistering ")
		 << "Class " << nameSpace << ":" <<className
	         << " for Provider " << providerName << endl;
	    
	    registerInstanceProvider(r, 
				 toRegister, 
				 nameSpace,  
				 className, 
				 providerName);
	}


    }
    catch(Exception& e)
    {
	std::cerr << "Error: " << e.getMessage() << std::endl;
	exit(1);
    }

    std::cout << "OK" << std::endl;

    return 0;
}

