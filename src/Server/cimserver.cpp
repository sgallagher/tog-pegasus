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
// Modified By: Mike Day (mdday@us.ibm.com) 
//
// Modified By:	Karl Schopmeyer (k.schopmeyer@opengroup.org)
//
// Modified By: Nag Boranna (nagaraja_boranna@hp.com)
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
#include <Pegasus/Server/CIMServer.h>
#include <Pegasus/Common/PegasusVersion.h>
#include <Pegasus/Protocol/Handler.h>
#include <Pegasus/Common/Logger.h>
#include <Pegasus/Common/System.h>
#include <Pegasus/Common/Tracer.h>
#include <Pegasus/Config/ConfigManager.h>
#include <slp/slp.h>



#if defined(PEGASUS_OS_TYPE_WINDOWS)
# include "cimserver_windows.cpp"
#elif defined(PEGASUS_OS_TYPE_UNIX)
# include "cimserver_unix.cpp"
#else
# error "Unsupported platform"
#endif

PEGASUS_USING_PEGASUS;
PEGASUS_USING_STD;

/**
    The command name.
*/
static const char COMMAND_NAME []    = "cimserver";

/**
    The constant defining usage string.
*/
static const char USAGE []           = "Usage: ";


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
    and sets up the options from that table using the config manager.
*/
void GetOptions(
    ConfigManager* cm,
    int& argc,
    char** argv,
    const String& pegasusHome)
{
    String currentFile = pegasusHome + "/" + CURRENT_CONFIG_FILE;
    String plannedFile = pegasusHome + "/" + PLANNED_CONFIG_FILE;

    try
    {
        cm->mergeConfigFiles(currentFile, plannedFile);

        cm->mergeCommandLine(argc, argv);
    }
    catch (NoSuchFile nsf)
    {
        throw nsf;
    }
    catch (FileNotReadable fnr)
    {
        throw fnr;
    }
    catch (CannotRenameFile ftrf)
    {
        throw ftrf;
    }
    catch (ConfigFileSyntaxError cfse)
    {
        throw cfse;
    }

}

/* PrintHelp - This is temporary until we expand the options manager to allow
   options help to be defined with the OptionRow entries and presented from
   those entries.
*/
void PrintHelp(const char* arg0)
{
    /**
        Build the usage string for the config command.
    */
    String usage = String (USAGE);
    usage.append (COMMAND_NAME);
    usage.append (" [ [ options ] | [ configProperty=value, ... ] ]\n");
    usage.append ("  options\n");
    usage.append ("    -v    - prints out the version number\n");
    usage.append ("    -h    - prints this help message\n");
    usage.append ("    -t    - turns tracing on\n");
    usage.append ("    -l    - turns logging on\n");
    usage.append ("  configProperty\n");
    usage.append ("    port=nnnn  - specifies port number to listen on\n");

    cout << endl;
    cout << PEGASUS_NAME << PEGASUS_VERSION << endl;
    cout << endl;
    cout << usage << endl;
}

/////////////////////////////////////////////////////////////////////////
//  MAIN
//////////////////////////////////////////////////////////////////////////
int main(int argc, char** argv)
{

    String pegasusHome  = String::EMPTY;
    Boolean pegasusIOTrace = false;
    Boolean pegasusIOLog = false;
    String portOption = String::EMPTY;
    String logsDirectory = String::EMPTY;
    Boolean useSLP = false;
    Boolean daemonOption = false;

    // on Windows NT if there are no command-line options, run as a service

    if (argc == 1 )
      cim_server_service(argc, argv) ;
  
    // Get environment variables

    if (pegasusHome.size() == 0)
    for (int i=0; i < argc; i++) {
        if (!strcmp(argv[i],"-D")) {
            i++;
            if (i < argc) pegasusHome = argv[i];
            break;
        }
    }
    if (pegasusHome.size() == 0)
        GetEnvironmentVariables(argv[0], pegasusHome);

    // Get options (from command line and from configuration file); this
    // removes corresponding options and their arguments from the command
    // line.

    //
    // Get an instance of the Config Manager.
    //
    ConfigManager* configManager = ConfigManager::getInstance();

    //
    // Get options (from command line and from configuration file); this
    // removes corresponding options and their arguments fromt he command
    // line.
    //
    try
    {
        GetOptions(configManager, argc, argv, pegasusHome);
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

    try
    {
        //
        // Check to see if user asked for the version (-v option):
        //

        if (configManager->isVersionFlagSet())
        {
            cout << PEGASUS_VERSION << endl;
            exit(0);
        }

        //
        // Check to see if user asked for help (-h option):
        //

        if (configManager->isHelpFlagSet())
        {
            PrintHelp(argv[0]);
            exit(0);
        }

        //
        // Check to see if we should (can) install as a NT service
        //

        if (configManager->isInstallFlagSet())
        {
            if( 0 != cimserver_install_nt_service( pegasusHome ))
            {
                cout << "\nPegasus installed as NT Service";
                exit(0);
            }
        }

        //
        // Check to see if we should (can) remove Pegasus as an NT service
        //

        if (configManager->isRemoveFlagSet())
        {
            if( 0 != cimserver_remove_nt_service() )
            {
                cout << "\nPegasus removed as NT Service";
                exit(0);
            }
        }

        //
        // Check to see if we should Pegasus as a daemon
        //

        if (configManager->isDaemonFlagSet())
        {
            daemonOption = true;
        }

        //
        // Grab the port option:
        //

        portOption = configManager->getCurrentValue("port");

        //
        // Check the trace options and set global variable
        //

        if (configManager->isTraceFlagSet())
        {
            Handler::setMessageTrace(true);
            pegasusIOTrace = true;
            cout << "Trace Set" << endl;
        }
        //
        // Check the log trace options and set global variable
        //

        if (configManager->isLogTraceFlagSet())
        {
            Handler::setMessageLogTrace(true);
            pegasusIOLog = true;
        }

        // Get the log file directory definition.
        // We put String into Cstring because
        // Directory functions only handle Cstring.
        // ATTN-KS: create String based directory functions.

        logsDirectory = configManager->getCurrentValue("logdir");

        // Set up the Logger. This does not open the logs
        // Might be more logical to clean before set.
        // ATTN: Need tool to completely disable logging.
        Logger::setHomeDirectory(logsDirectory);

        if (configManager->isCleanLogsFlagSet())
        {
            Logger::clean(logsDirectory);;
        }

        // Leave this in until people get familiar with the logs.
        cout << "Logs Directory = " << logsDirectory << endl;

        if (configManager->isSlpFlagSet())
        {
            useSLP =  true;
        }
    }
    catch (UnrecognizedConfigProperty e)
    {
        cout << "Error: " << e.getMessage() << endl;
    }

    char* address = portOption.allocateCString();

    // Put out startup up message.
    cout << PEGASUS_NAME << PEGASUS_VERSION <<
	 " on port " << address << endl;
    cout << "Built " << __DATE__ << " " << __TIME__ << endl;
    cout <<"Started..."
	 << (pegasusIOTrace ? " Tracing to Display ": " ") 
         << (pegasusIOLog ? " Tracing to Log ": " ")
	 << (useSLP ? " SLP reg. " : " No SLP ")
	<< endl;

    // Put server start message to the logger
    Logger::put(Logger::STANDARD_LOG, "CIMServer", Logger::INFORMATION,
	"Start $0 $1 port $2 $3 $4",
		PEGASUS_NAME, 
		PEGASUS_VERSION,
		address,
		(pegasusIOTrace ? " Tracing": " "),
		(useSLP ? " SLP on " : " SLP off "));

    // do we need to run as a daemon ?
    if (daemonOption)
    {
        if(-1 == cimserver_fork())
          exit(-1);
    }

    // try loop to bind the address, and run the server
    try
    {
      	slp_client *discovery = new slp_client() ;;
        String serviceURL;
	serviceURL.assign("service:cim.pegasus://");
	String host_name = slp_get_host_name();
	serviceURL += host_name;
	serviceURL += ":";
	serviceURL += address;
	char *url = serviceURL.allocateCString();
	//	free(host_name);

	Selector selector;
	CIMServer server(&selector, pegasusHome);

	// bind throws an exception of the bind fails
	cout << "Binding to " << address << endl;
	server.bind(address);
	delete [] address;

	time_t last = 0;
	while( 1 )
	{
	  if(useSLP  ) 
	  {
	    if(  (time(NULL) - last ) > 60 ) 
	    {
	      if( discovery != NULL && url != NULL )
		discovery->srv_reg_all(url,  
				       "(namespace=root/cimv20)",
				       "service:cim.pegasus", 
				       "DEFAULT", 
				       70) ;
	      time(&last);
	    }
	  
	    discovery->service_listener();
	  }
	  server.runForever();
	}

	// This statement is unrechable!
	//
	// Logger::put(Logger::STANDARD_LOG, "CIMServer", Logger::INFORMATION,
	//   "Normal Termination");
    }
    catch(Exception& e)
    {
	Logger::put(Logger::STANDARD_LOG, "CIMServer", Logger::INFORMATION,
	    "Abnormal Termination $0", e.getMessage());
	
	PEGASUS_STD(cerr) << "Error: " << e.getMessage() << PEGASUS_STD(endl);
    }

    return 0;
}
