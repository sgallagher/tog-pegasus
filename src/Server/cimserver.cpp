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
// Modified By: Jenny Yu (jenny_yu@hp.com)
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
// To shutdown pegasus, use the -s option:
// 
// cimserver -s [-f] [-T timeout_value]
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


#include <Pegasus/Common/Config.h>
#include <iostream>
#include <cassert>
#include <cstdlib>
#include <Pegasus/Common/FileSystem.h>
#include <Pegasus/Common/Monitor.h>
#include <Pegasus/Server/CIMServer.h>
#include <Pegasus/Common/PegasusVersion.h>
#include <Pegasus/Protocol/Handler.h>
#include <Pegasus/Common/Logger.h>
#include <Pegasus/Common/System.h>
#include <Pegasus/Common/Tracer.h>
#include <Pegasus/Config/ConfigManager.h>
#include <Pegasus/Client/CIMClient.h>
#include <Pegasus/Common/HTTPConnector.h>
#include <Pegasus/Server/ShutdownService.h>
#ifndef PEGASUS_OS_ZOS
#include <slp/slp.h>
#endif


#if defined(PEGASUS_OS_TYPE_WINDOWS)
# include "cimserver_windows.cpp"
#elif defined(PEGASUS_OS_TYPE_UNIX)
# include "cimserver_unix.cpp"
#else
# error "Unsupported platform"
#endif

PEGASUS_USING_PEGASUS;
PEGASUS_USING_STD;

//
//  The command name.
//
static const char COMMAND_NAME []    = "cimserver";

//
//  The constant defining usage string.
//
static const char USAGE []           = "Usage: ";

/**
Constants representing the command line options.
*/
static const char OPTION_VERSION     = 'v';

static const char OPTION_HELP        = 'h';

static const char OPTION_HOME        = 'D';

static const char OPTION_SHUTDOWN    = 's';

static const char OPTION_FORCE       = 'f';

static const char OPTION_TIMEOUT     = 'T';

static const String NAMESPACE = "root/cimv2";
static const String CLASSNAME_SHUTDOWNSERVICE = "PG_ShutdownService";
static const String PROPERTY_TIMEOUT = "operationTimeout";

ConfigManager*    configManager;

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

void SetEnvironmentVariables(
    const char* arg0)
{
    cout << "PEGASUS_HOME is now " << arg0 << endl;

    String str = "PEGASUS_HOME=";
    str += arg0;
    char* tmp = str.allocateCString();
    putenv(tmp);

    // Note: don't delete tmp! putenv() uses it.
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
    catch(UnrecognizedConfigProperty ucp)
    {
        throw ucp;
    }
    catch(InvalidPropertyValue ipv)
    {
        throw ipv;
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
    usage.append ("    -v          - displays pegasus version number\n");
    usage.append ("    -h          - prints this help message\n");
    usage.append ("    -D [home]   - sets pegasus home directory\n");
    usage.append ("    -t          - turns tracing on\n");
    usage.append ("    -t          - turns on trace of client IO to console\n");
    usage.append ("    -l          - turns on trace of client IO to trace file\n");
    usage.append ("    -d          - runs pegasus as a daemon\n");
    usage.append ("    -s [-f] [-T timeout] \n");
    usage.append ("                - shuts down pegasus\n");
    usage.append ("    -cleanlogs  - clears the log files at startup\n");
    usage.append ("    -install    - installs pegasus as a Windows NT Service\n");
    usage.append ("    -remove     - removes pegasus as a Windows NT Service\n");
    usage.append ("    -slp        - registers pegasus as a service with SLP\n\n");
    usage.append ("    -SSL        - uses SSL\n\n");

    usage.append ("  configProperty=value\n");
    usage.append ("    port=nnnn            - sets port number to listen on\n");
    usage.append ("    home=/pegasus/bin    - sets pegasus home directory\n");
    usage.append ("    logdir=/pegasus/logs - directory for log files\n");

    cout << endl;
    cout << PEGASUS_NAME << PEGASUS_VERSION << endl;
    cout << endl;
    cout << usage << endl;
}

void shutdownCIMOM(Boolean forceOption, Uint32 timeoutValue)
{
    //
    // Create CIMClient object
    //
    Monitor* monitor = new Monitor;
    HTTPConnector* httpConnector = new HTTPConnector(monitor);
    CIMClient client(monitor, httpConnector);

    //
    // Get the port number
    //
    String portNumberStr = configManager->getCurrentValue("port");

    String hostStr = System::getHostName();
    hostStr.append(":");
    hostStr.append(portNumberStr);

    // Put server shutdown message to the logger
    Logger::put(Logger::STANDARD_LOG, "CIMServer", Logger::INFORMATION,
	"Shutdown $0 on port $1.", PEGASUS_NAME, portNumberStr);

    //
    // open connection to CIMOM 
    //
    try
    {
        client.connect(hostStr.allocateCString());
    }
    catch(Exception& e)
    {
        Logger::put(Logger::STANDARD_LOG, "CIMServer", Logger::INFORMATION,
            "Failed to connect to $0 $1.", PEGASUS_NAME, e.getMessage());

        PEGASUS_STD(cerr) << "Failed to connect to server: " << e.getMessage() << PEGASUS_STD(endl);
        exit(1);
    }

    try
    {
        //
        // construct CIMReference 
        //
        String referenceStr = "//";
        referenceStr.append(hostStr);
        referenceStr.append("/root/cimv2:PG_ShutdownService");
        CIMReference reference(referenceStr);

        //
        // issue the invokeMethod request on the shutdown method
        //
        Array<CIMParamValue> inParams;
        Array<CIMParamValue> outParams;

        if (forceOption)
        {
            inParams.append(CIMParamValue(
                CIMParameter("force", CIMType::STRING),
                CIMValue("TRUE")));
        }
        else
        {
            inParams.append(CIMParamValue(
                CIMParameter("force", CIMType::STRING),
                CIMValue("FALSE")));
        }

        inParams.append(CIMParamValue(
            CIMParameter("timeout", CIMType::UINT32),
            CIMValue(timeoutValue)));

        CIMValue retValue = client.invokeMethod(
            NAMESPACE,
            reference,
            "shutdown",
            inParams,
            outParams);

        // Put server shutdown message to the logger
        Logger::put(Logger::STANDARD_LOG, "CIMServer", Logger::INFORMATION,
	    "$0 terminated on port $1.", PEGASUS_NAME, portNumberStr);

    }
    catch(Exception& e)
    {
        PEGASUS_STD(cerr) << "Failed to shutdown server: " << e.getMessage() << PEGASUS_STD(endl);
        exit(1);
    }

    return;
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
    Boolean useSSL = false;
    Boolean daemonOption = false;
    Boolean shutdownOption = false;
    Boolean forceOption = false;
    Boolean timeoutOption = false;
    String  timeoutStr  = String::EMPTY;
    long timeoutValue  = 0;

    // on Windows NT if there are no command-line options, run as a service

    if (argc == 1 )
    {
      cim_server_service(argc, argv);
    }
    else
    {
        // Get help, version and home options

        for (int i = 1; i < argc; )
        {
            const char* arg = argv[i];

            // Check for -option
            if (*arg == '-')
            {
                // Get the option
                const char* option = arg + 1;

                //
                // Check to see if user asked for the version (-v option):
                //
                if (*option == OPTION_VERSION)
                {
                    cout << PEGASUS_VERSION << endl;
                    exit(0);
                }
                //
                // Check to see if user asked for help (-h option):
                //
                else if (*option == OPTION_HELP)
                {
                    PrintHelp(argv[0]);
                    exit(0);
                }
                else if (*option == OPTION_HOME)
                {
                    if (i + 1 < argc) 
                    {
                        pegasusHome.assign(argv[i + 1]);
        	        SetEnvironmentVariables(argv[i + 1]);
                    }
                    else
                    {
                        cout << "Missing argument for option -" << option << endl;
                        exit(0);
                    }

                    memmove(&argv[i], &argv[i + 2], (argc-i-1) * sizeof(char*));
                    argc -= 2;
                }
                //
                // Check to see if user asked for shutdown (-s option):
                //
                else if (*option == OPTION_SHUTDOWN)
                {
                    //
                    // Check to see if shutdown has already been specified:
                    //
                    if (shutdownOption)
                    {
                        cout << "Duplicate shutdown option specified." << endl;
                        exit(0);
                    }
                    shutdownOption = true;
 
                    // remove the option from the command line
                    memmove(&argv[i], &argv[i + 1], (argc-i) * sizeof(char*));
                    argc--;   
                }
                else if (*option == OPTION_FORCE)
                {
                    //
                    // Check to see if shutdown has been specified:
                    //
                    if (!shutdownOption)
                    {
                        cout << "Invalid option -" << option << endl;
                        exit(0);
                    }

                    //
                    // Check to see if force has already been specified:
                    //
                    if (forceOption)
                    {
                        cout << "Duplicate force option specified." << endl;
                        exit(0);
                    }

                    forceOption = true;
 
                    // remove the option from the command line
                    memmove(&argv[i], &argv[i + 1], (argc-i) * sizeof(char*));
                    argc--;   
                }
                else if (*option == OPTION_TIMEOUT)
                {
                    //
                    // Check to see if shutdown has been specified:
                    //
                    if (!shutdownOption)
                    {
                        cout << "Invalid option -" << option << endl;
                        exit(0);
                    }

                    if (timeoutOption)
                    {
                        cout << "Duplicate timeout option specified." << endl;
                        exit(0);
                    }

                    timeoutOption = true;

                    if (i + 1 < argc)
                    {
                        // get timeout value
                        timeoutStr.assign(argv[i + 1]);

                        // validate timeout value string
                        char* tmp = timeoutStr.allocateCString();
                        char* end = 0;
                        timeoutValue  = strtol(tmp, &end, 10);
                      
                        if (!end || *end != '\0')
                        {
                            cout << "invalid timeout value specified: ";
                            cout << timeoutStr << endl;
                            delete [] tmp;
                            exit(0);
                        }
                    }
                    else
                    {
                        cout << "Missing argument for option -";
                        cout << option << endl;
                        exit(0);
                    }

                    // remove the option from the command line
                    memmove(&argv[i], &argv[i + 2], (argc-i-1) * sizeof(char*));
                    argc -= 2;
                }
                else
                    i++;
            }
            else
                i++;
        }
    }

    if (pegasusHome.size() == 0)
        GetEnvironmentVariables(argv[0], pegasusHome);

    //
    // Get an instance of the Config Manager.
    //
    configManager = ConfigManager::getInstance();

    //
    // Get options (from command line and from configuration file); this
    // removes corresponding options and their arguments from the command
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


    try
    {
        //
        // Check to see if we should (can) install as a NT service
        //

        if (String::equal(configManager->getCurrentValue("install"), "true"))
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

        if (String::equal(configManager->getCurrentValue("remove"), "true"))
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

        if (String::equal(configManager->getCurrentValue("daemon"), "true"))
        {
            daemonOption = true;
        }

        //
        // Check the log trace options and set global variable
        //

        if (String::equal(configManager->getCurrentValue("logtrace"), "true"))
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

        //
        // Check to see if we need to shutdown CIMOM 
        //
        if (shutdownOption)
        {
            //
            // if timeout was specified, validate the timeout value 
            //
            if (timeoutOption)
            {
                Boolean valid = configManager->validatePropertyValue(
                                             PROPERTY_TIMEOUT,
                                             timeoutStr);
                if (!valid)
                {
                    cout << "Invalid timeout value specified: " << timeoutValue;
                    cout << endl;
                    exit(1);
                }
            }

            shutdownCIMOM(forceOption, timeoutValue);
            cout << "Pegasus CIM Server terminated." << endl;
            exit(0);
        }

        //
        // Grab the port option:
        //

        portOption = configManager->getCurrentValue("port");

        //
        // Check the trace options and set global variable
        //

        if (String::equal(configManager->getCurrentValue("trace"), "true"))
        {
            Handler::setMessageTrace(true);
            pegasusIOTrace = true;
            cout << "Trace Set" << endl;
        }

        // Leave this in until people get familiar with the logs.
        cout << "Logs Directory = " << logsDirectory << endl;

        if (String::equal(configManager->getCurrentValue("cleanlogs"), "true"))
        {
            Logger::clean(logsDirectory);;
        }

        if (String::equal(configManager->getCurrentValue("slp"), "true"))
        {
            useSLP =  true;
        }

        if (String::equal(configManager->getCurrentValue("SSL"), "true"))
        {
            useSSL =  true;
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
         << (useSSL ? " Use SSL " : " No SSL ")
	<< endl;

    // Put server start message to the logger
    Logger::put(Logger::STANDARD_LOG, "CIMServer", Logger::INFORMATION,
	"Start $0 $1 port $2 $3 $4 $5",
		PEGASUS_NAME, 
		PEGASUS_VERSION,
		address,
		(pegasusIOTrace ? " Tracing": " "),
		(useSLP ? " SLP on " : " SLP off "),
                (useSSL ? " Use SSL " : " No SSL "));

    // do we need to run as a daemon ?
    if (daemonOption)
    {
        if(-1 == cimserver_fork())
          exit(-1);
    }

    // try loop to bind the address, and run the server
    try
    {
#ifndef PEGASUS_OS_ZOS
      	slp_client *discovery = new slp_client() ;;
        String serviceURL;
	serviceURL.assign("service:cim.pegasus://");
	String host_name = slp_get_host_name();
	serviceURL += host_name;
	serviceURL += ":";
	serviceURL += address;
	char *url = serviceURL.allocateCString();
	//	free(host_name);
#endif

	Monitor monitor;
	CIMServer server(&monitor, pegasusHome, useSSL);
		
	// bind throws an exception of the bind fails
	cout << "Binding to " << address << endl;

	char* end = 0;
	long portNumber = strtol(address, &end, 10);
	assert(end != 0 && *end == '\0');
	server.bind(portNumber);

	delete [] address;

	time_t last = 0;

        //
        // Loop to call CIMServer's runForever() method until CIMServer
        // has been shutdown
        //
	while( !server.terminated() )
	{
#ifndef PEGASUS_OS_ZOS
	  if(useSLP  ) 
	  {
	    if(  (time(NULL) - last ) > 60 ) 
	    {
	      if( discovery != NULL && url != NULL )
		discovery->srv_reg_all(url,  
				       "(namespace=root/cimv2)",
				       "service:cim.pegasus", 
				       "DEFAULT", 
				       70) ;
	      time(&last);
	    }
	  
	    discovery->service_listener();
	  }
#endif
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
