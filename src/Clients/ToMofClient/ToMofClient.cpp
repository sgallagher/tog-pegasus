
//%////////////////////////////////////////////////////////////////////////////
//
//
// Copyright (c) 2000, 2001 BMC Software, Hewlett-Packard Company, IBM,
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
// Author: Karl Schopmeyer (k.schopmeyer@opengroup.org)
//
// Modified By:
//
//%/////////////////////////////////////////////////////////////////////////////

#include <cassert>
#include <Pegasus/Common/Selector.h>
#include <Pegasus/Client/CIMClient.h>
#include <Pegasus/Common/OptionManager.h>
#include <Pegasus/Common/FileSystem.h>
#include <Pegasus/Common/Stopwatch.h>
#include <slp/slp.h>

PEGASUS_USING_PEGASUS;
PEGASUS_USING_STD;

String nameSpace = "root/cimv2";

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

//------------------------------------------------------------------------------
//
// _indent()
//
//------------------------------------------------------------------------------

static void _indent(PEGASUS_STD(ostream)& os, Uint32 level, Uint32 indentChars)
{
    Uint32 n = level * indentChars;

    for (Uint32 i = 0; i < n; i++)
	os << ' ';
}
void mofFormat(
    PEGASUS_STD(ostream)& os, 
    const char* text, 
    Uint32 indentChars)
{
    char* tmp = strcpy(new char[strlen(text) + 1], text);
    //const char* tmp = x.getData();
    Uint32 count = 0;
    Uint32 indent = 0;
    Boolean quoteState = false;
    char c;
    while (c = *tmp++)
    {
	count++;
	switch (c)
	{
	    case '\n':
		os << Sint8(c);
		count = 0;
		indent = 0;
		break;

	    case '\"':	 // quote 
		os <<Sint8(c);
		quoteState = !quoteState;
		break;

	    case ' ':
		os <<Sint8(c);
		if (count > 70)
		{
		    if (quoteState)
			os <<"\"\n    \"";
		    else
			os <<"\n    ";
		    count = 0 - indent;
		}
		break;

	    default:
		os <<Sint8(c);
	}

    }
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
		 
		 {"namespace", "root/cimv2", false, Option::STRING, 0, 0, "-n",
		 		 	"Specifies namespace to use for test" 
},

		 {"version", "false", false, Option::BOOLEAN, 0, 0, "v",
		 		 		 "Displays TestClient Version "},

		 {"verbose", "false", false, Option::BOOLEAN, 0, 0, "verbose",
		 		 		 "Displays Pegasus Version "},

		 {"help", "false", false, Option::BOOLEAN, 0, 0, "h",
		 		     "Prints help message with command line options "},

		 {"debug", "false", false, Option::BOOLEAN, 0, 0, "d", 
		              "Not Used "},
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
    if (argc != 2)
	ErrorExit("One Argument (classname) required");

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

    Array<String> connectionList;
    connectionList.append("localhost:5988");

    Selector selector;
    CIMClient client(&selector, 60 * 1000);

    char * connection = connectionList[0].allocateCString();
    cout << "connecting to " << connection << endl;
    client.connect(connection);


    Boolean singleClass = true;
    
    if (singleClass)
    {
	try
	{
	    String className;
	    className = argv[1];
	    Boolean localOnly = true;
	    Boolean includeQualifiers = true;
	    Boolean includeClassOrigin = true;

	    CIMClass cimClass = client.getClass(nameSpace, className,
		localOnly, includeQualifiers, includeClassOrigin);

   	    cout << endl;

	    Array<Sint8> x;
	    cimClass.toMof(x);
	    x.append('\0');

	    mofFormat(cout, x.getData(), 4);
	    //os << tmp.getData() << PEGASUS_STD(endl);

	    //cimClass.printMof(); 
	    cout << endl;
	}
	 catch(Exception& e)
	{
	    ErrorExit(e.getMessage());
	}
    }
    else
    {
	try
	{
	    Boolean deepInheritance = true;
	    Boolean localOnly = false;
	    Boolean includeQualifiers = false;
	    Boolean includeClassOrigin = true;

	    String className = "";
	    Array<CIMClass> classArray = client.enumerateClasses(
					    nameSpace,
					    className,
					    deepInheritance,
					    localOnly,
					    includeQualifiers,
					    includeClassOrigin);

	    for (Uint32 i = 0, n = classArray.size(); i < n; i++)
	    {
		cout << endl;
		Array<Sint8> x;
		classArray[i].toMof(x);
		x.append('\0');

		mofFormat(cout, x.getData(), 4);
		cout << endl;

	    }
	}
	catch(Exception& e)
	{
	     cout << "Error Class Enumeration:" << endl;
	     cout << e.getMessage() << endl;
	}
    }  
    return 0; 
} 
//PEGASUS_NAMESPACE_END


