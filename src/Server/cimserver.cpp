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
// The above copyright notice and this permission notice shall be included in 
// all copies or substantial portions of the Software.
//
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
// Author: Mike Brasher (mbrasher@bmc.com)
//
// Modified By: Mike Day (mdday@us.ibm.com) 
// =======
// Modified By:	Karl Schopmeyer (k.schopmeyer@opengroup.org)
//
//%/////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////
//
// Notes on deamon operation (Unix) and service operation (Win 32):
//
// To run pegasus as a daemon on Unix platforms, use the -d option:
//
// cimserver -d
//
// The -d option has no effect on windows operation. 
//
// To run pegasus as an NT service, there are FOUR  different possibilities:
//
// To INSTALL the Pegasus service, 
//
// cimserver -install
//
// To REMOVE the Pegasus service, 
//
// cimserver -remove
//
// To START the Pegasus service, 
//
// net start cimserver
//
// To STOP the Pegasus service, 
//
// net stop cimserver
//
// Alternatively, you can use the windows service manager. Pegasus shows up 
// in the service database as "Pegasus CIM Object Manager"
//
// Mike Day, mdday@us.ibm.com
// 
//////////////////////////////////////////////////////////////////////


#include <iostream>
#include <cstdlib>
#include <Pegasus/Common/FileSystem.h>
#include <Pegasus/Common/Selector.h>
#include <Pegasus/Common/OptionManager.h>
#include <Pegasus/Server/CIMServer.h>
#include <Pegasus/Common/PegasusVersion.h>
#include <Pegasus/Protocol/Handler.h>
#include <Pegasus/Common/Logger.h>
#include <Pegasus/Common/System.h>



#if defined(PEGASUS_OS_TYPE_WINDOWS)
# include "cimserver_windows.cpp"
#elif defined(PEGASUS_OS_TYPE_UNIX)
# include "cimserver_unix.cpp"
#else
# error "Unsupported platform"
#endif

PEGASUS_USING_PEGASUS;
PEGASUS_USING_STD;

void GetEnvironmentVariables(
    const char* arg0,
    String& pegasusHome)
{
    // Get environment variables:

    const char* tmp = getenv("PEGASUS_HOME");

    if (!tmp)
    {
	cerr << arg0 << ": PEGASUS_HOME environment variable undefined" << endl;
	exit(1);
    }

    pegasusHome = tmp;
    FileSystem::translateSlashes(pegasusHome);
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
	{"port", "5988", false, Option::WHOLE_NUMBER, 0, 0, "port",
			"specifies port number to listen on" },
	{"trace", "false", false, Option::BOOLEAN, 0, 0, "t", 
			"turns on trace of Client IO to console "},
	{"logtrace", "false", false, Option::BOOLEAN, 0, 0, "l",
			"Turns on trace of Client IO to trace log "},
	{"options", "false", false, Option::BOOLEAN, 0, 0, "options",
			" Displays the settings of the Options "},
	{"severity", "ALL", false, Option::STRING, 0, 0, "s",

		    "Sets the severity level that will be logged "},
	{"logs", "ALL", false, Option::STRING, 0, 0, "X", 
			"Not Used "},
	{"daemon", "false", false, Option::BOOLEAN, 0, 0, "d", 
			"Detach Pegasus from the console and run it in the background "},
	{"logdir", "./logs", false, Option::STRING, 0, 0, "logdir", 
			"Directory for log files"},
	{"cleanlogs", "false", false, Option::BOOLEAN, 0, 0, "clean", 
			"Clears the log files at startup"},
	{"version", "false", false, Option::BOOLEAN, 0, 0, "v",
			"Displays Pegasus Version "},
	{"help", "false", false, Option::BOOLEAN, 0, 0, "h",
		    "Prints help message with command line options "},
	{"install", "false", false, Option::BOOLEAN, 0, 0, "install",
		    "Installs Pegasus as a Windows NT Service "},
	{"remove", "false", false, Option::BOOLEAN, 0, 0, "remove",
		    "Removes Pegasus as a Windows NT Service "},
	{"debug", "false", false, Option::BOOLEAN, 0, 0, "d", 
	                "Not Used "},
	{"slp", "true", false, Option::BOOLEAN, 0, 0, "slp", 
			"Register Pegasus as a Service with SLP"}
    };
    const Uint32 NUM_OPTIONS = sizeof(optionsTable) / sizeof(optionsTable[0]);

    om.registerOptions(optionsTable, NUM_OPTIONS);

    String configFile = pegasusHome + "/testclient.conf";

    cout << "Config file from " << configFile << endl;

    if (FileSystem::exists(configFile))
	om.mergeFile(configFile);
    if(argc && argv != NULL)
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
    cout << PEGASUS_NAME << PEGASUS_VERSION << endl;
    cout << '\n';
    cout << "Usage: " << arg0 << endl;
    cout << endl;
}

//////////////////////////////////////////////////////////////////////////
//  MAIN
//////////////////////////////////////////////////////////////////////////
int main(int argc, char** argv)
{
  
    // on Windows NT if there are no command-line options, run as a service
    if (argc == 1 )
      cim_server_service(argc, argv) ;
  
    // Get environment variables:

    String pegasusHome;

    GetEnvironmentVariables(argv[0], pegasusHome);

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

    // At this point, all options should have been extracted; print an
    // error if there are any remaining:

    if (argc != 1)
    {
	cerr << argv[0] << ": unrecognized options: ";

	for (int i = 1; i < argc; i++)
	    cerr << argv[i] << ' ';
	cout << endl;
	exit(1);
    }

    // Check to see if we should (can) install as a NT service

    String installOption;
    if(om.lookupValue("install", installOption) && installOption == "true")
      {
	if( 0 != cimserver_install_nt_service( pegasusHome ))
	  cout << "\nPegasus installed as NT Service";
	exit(0);
      }

    // Check to see if we should (can) remove Pegasus as an NT service

    String removeOption;
    if(om.lookupValue("remove", removeOption) && removeOption == "true")
      {
	if( 0 != cimserver_remove_nt_service() )
	  cout << "\nPegasus removed as NT Service";
	exit(0);
	  
      }

    // Check to see if user asked for the version (-v otpion):

    String versionOption;

    if (om.lookupValue("version", versionOption) && versionOption == "true")
    {
	cerr << PEGASUS_VERSION << endl;
	exit(0);
    }

    // Check to see if user asked for help (-h otpion):
    String helpOption;

    if (om.lookupValue("help", helpOption) && helpOption == "true")
    {
	PrintHelp(argv[0]);
	om.printHelp();
	exit(0);
    }

    // Check the trace options and set global variable
    Boolean pegasusIOTrace = false;
    if (om.valueEquals("trace", "true"))
    {
	Handler::setMessageTrace(true);
	pegasusIOTrace = true;
    }

    Boolean pegasusIOLog = false;
    if (om.valueEquals("logtrace", "true"))
    {
	Handler::setMessageLogTrace(true);
	pegasusIOLog = true;
    }
    
    // Grab the port otpion:

    String portOption;
    om.lookupValue("port", portOption);

    // Get the log file directory definition.
    // We put String into Cstring because
    // Directory functions only handle Cstring.
    // ATTN-KS: create String based directory functions.
    String logsDirectory;
    om.lookupValue("logdir", logsDirectory);

    // Set up the Logger. This does not open the logs
    // Might be more logical to clean before set.
    // ATTN: Need tool to completely disable logging.
    Logger::setHomeDirectory(logsDirectory);
    
    if (om.valueEquals("cleanlogs", "true"))
    {
	Logger::clean(logsDirectory);;
    }

    // Leave this in until people get familiar with the logs.
    cout << "Logs Directory = " << logsDirectory << endl;


    char* address = portOption.allocateCString();

    // Put out startup up message.
    cout << PEGASUS_NAME << PEGASUS_VERSION <<
	 " on port " << address << endl;
    cout << "Built " << __DATE__ << " " << __TIME__ << endl;
    cout <<"Started..."
	 << (pegasusIOTrace ? " Tracing to Display ": " ") 
         << (pegasusIOLog ? " Tracing to Log ": " ")
	<< endl;

    // Option to Display the options table.  Primarily
    // a diagnostic tool.
    if (om.valueEquals("options", "true"))
	om.print();

    // Put server start message to the logger
    Logger::put(Logger::STANDARD_LOG, "CIMServer", Logger::INFORMATION,
	"Start $0 $1 port $2 $3 ",
		PEGASUS_NAME, 
		PEGASUS_VERSION,
		address,
		(pegasusIOTrace ? " Tracing": " "));

    Boolean useSLP;
    if(om.valueEquals("slp", "true")) 
      useSLP = true;

    // do we need to run as a daemon ?
    String daemonOption;
    if(om.lookupValue("daemon", daemonOption) && daemonOption == "true") 
      {
	if(-1 == cimserver_fork())
	  exit(-1);
      }


    // try loop to bind the address, and run the server
    try
    {
	Selector selector;
	CIMServer server(&selector, pegasusHome);
	server.setSLP(useSLP );

	// bind throws an exception of the bind fails
	server.bind(address);
	delete [] address;
	server.runForever();

	Logger::put(Logger::STANDARD_LOG, "CIMServer", Logger::INFORMATION,
	    "Normal Termination");

    }
    catch(Exception& e)
    {
	Logger::put(Logger::STANDARD_LOG, "CIMServer", Logger::INFORMATION,
	    "Abnormal Termination $0", e.getMessage());
	
	PEGASUS_STD(cerr) << "Error: " << e.getMessage() << PEGASUS_STD(endl);
    }

    return 0;
}
