//%2003////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001, 2002  BMC Software, Hewlett-Packard Development
// Company, L. P., IBM Corp., The Open Group, Tivoli Systems.
// Copyright (c) 2003 BMC Software; Hewlett-Packard Development Company, L. P.;
// IBM Corp.; EMC Corporation, The Open Group.
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
// Modified By: Sushma Fernandes (sushma_fernandes@hp.com)
//              Carol Ann Krug Graves, Hewlett-Packard Company
//                (carolann_graves@hp.com)
//		Yi Zhou, Hewlett-Packard Company (yi_zhou@hp.com)
//
// Modified By: Dave Rosckes (rosckes@us.ibm.com)
//
// Modified By: Humberto Rivero (hurivero@us.ibm.com)
//
// Modified By: Steve Hills (steve.hills@ncr.com)
//
//%/////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////
//
// Notes on deamon operation (Unix) and service operation (Win 32):
//
// To run pegasus as a daemon on Unix platforms: 
//
// cimserver
//
// To NOT run pegasus as a daemon on Unix platforms, set the daemon config
// property to false:
//
// cimserver daemon=false
//
// The daemon config property has no effect on windows operation. 
//
// To shutdown pegasus, use the -s option:
// 
// cimserver -s 
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
// or
// cimserver -start
//
// To STOP the Pegasus service, 
//
// net stop cimserver
// or
// cimserver -stop
//
// Alternatively, you can use the windows service manager. Pegasus shows up 
// in the service database as "Pegasus CIM Object Manager"
//
// Mike Day, mdday@us.ibm.com
// 
//////////////////////////////////////////////////////////////////////


#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/Constants.h>
#include <iostream>
#include <cassert>
#include <cstdlib>
#include <fstream>
#include <Pegasus/Common/FileSystem.h>
#include <Pegasus/Common/Monitor.h>
#include <Pegasus/Server/CIMServer.h>
#include <Pegasus/Common/PegasusVersion.h>
#include <Pegasus/Common/Logger.h>
#include <Pegasus/Common/System.h>
#include <Pegasus/Common/Tracer.h>
#include <Pegasus/Config/ConfigManager.h>
#include <Pegasus/Client/CIMClient.h>
#include <Pegasus/Server/ShutdownService.h>
#include <Pegasus/Common/Destroyer.h>

PEGASUS_USING_PEGASUS;
PEGASUS_USING_STD;

int cimserver_run( int argc, char** argv, Boolean shutdownOption );

#if defined(PEGASUS_OS_TYPE_WINDOWS)
# include "cimserver_windows.cpp"
#elif defined(PEGASUS_OS_TYPE_UNIX)
# if defined(PEGASUS_OS_OS400)
#  include "vfyptrs.cinc"
#  include "OS400ConvertChar.h"
#  include "cimserver_os400.cpp"
# else
#  include "cimserver_unix.cpp"
#endif
#else
# error "Unsupported platform"
#endif

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

#if defined(PEGASUS_OS_HPUX)
static const char OPTION_BINDVERBOSE = 'X';
#endif

static const String PROPERTY_TIMEOUT = "shutdownTimeout";

ConfigManager*    configManager;

/** Helper for platform specific handling. So platform specific code
    can use our single instance of CIMServer.
*/
class CimserverHolder
{
public:
	CimserverHolder( CIMServer* s )
	{
		cimserver_set( s );
	}
	virtual ~CimserverHolder()
	{
		cimserver_set( 0 );
	}
};

/** GetOptions function - This function defines the Options Table
    and sets up the options from that table using the config manager.
*/
void GetOptions(
    ConfigManager* cm,
    int& argc,
    char** argv)
{
    try
    {
        cm->mergeConfigFiles();

        cm->mergeCommandLine(argc, argv);
    }
    catch (NoSuchFile&)
    {
        throw;
    }
    catch (FileNotReadable&)
    {
        throw;
    }
    catch (CannotRenameFile&)
    {
        throw;
    }
    catch (ConfigFileSyntaxError&)
    {
        throw;
    }
    catch(UnrecognizedConfigProperty&)
    {
        throw;
    }
    catch(InvalidPropertyValue&)
    {
        throw;
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
    /* l10n
    String usage = String (USAGE);
    usage.append (COMMAND_NAME);
    usage.append (" [ [ options ] | [ configProperty=value, ... ] ]\n");
    usage.append ("  options\n");
    usage.append ("    -v              - displays CIM Server version number\n");
    usage.append ("    -h              - prints this help message\n");
    usage.append ("    -s              - shuts down CIM Server\n");
#if !defined(PEGASUS_OS_HPUX) && !defined(PEGASUS_PLATFORM_LINUX_IA64_GNU)
    usage.append ("    -D [home]       - sets pegasus home directory\n");
#endif
#if defined(PEGASUS_OS_TYPE_WINDOWS)
    usage.append ("    -install [name] - installs pegasus as a Windows NT Service\n");
    usage.append ("                      [name] is optional and overrides the\n");
    usage.append ("                      default CIM Server Service Name\n");
    usage.append ("    -remove [name]  - removes pegasus as a Windows NT Service\n");
    usage.append ("                      [name] is optional and overrides the\n");
    usage.append ("                      default CIM Server Service Name\n");
    usage.append ("    -start [name]   - starts pegasus as a Windows NT Service\n");
    usage.append ("                      [name] is optional and overrides the\n");
    usage.append ("                      default CIM Server Service Name\n");
    usage.append ("    -stop [name]    - stops pegasus as a Windows NT Service\n");
    usage.append ("                      [name] is optional and overrides the\n");
    usage.append ("                      default CIM Server Service Name\n\n");
#endif
    usage.append ("  configProperty=value\n");
    usage.append ("                    - sets CIM Server configuration property\n");

    cout << endl;
#if defined(PEGASUS_OS_HPUX) || defined(PEGASUS_PLATFORM_LINUX_IA64_GNU)
    cout << PLATFORM_PRODUCT_NAME << " " << PLATFORM_PRODUCT_VERSION << endl;
#else
    cout << PEGASUS_NAME << PEGASUS_VERSION << endl;
#endif
    cout << endl;
    cout << usage << endl;
    */
    
    String usage = String (USAGE);
    usage.append (COMMAND_NAME);
    usage.append (" [ [ options ] | [ configProperty=value, ... ] ]\n");
    usage.append ("  options\n");
    usage.append ("    -v              - displays CIM Server version number\n");
    usage.append ("    -h              - prints this help message\n");
    usage.append ("    -s              - shuts down CIM Server\n");
#if !defined(PEGASUS_OS_HPUX) && !defined(PEGASUS_PLATFORM_LINUX_IA64_GNU)
    //usage.append ("    -D [home]       - sets pegasus home directory\n");
    usage.append("$0");
#endif
#if defined(PEGASUS_OS_TYPE_WINDOWS)
    usage.append ("    -install [name] - installs pegasus as a Windows NT Service\n");
    usage.append ("                      [name] is optional and overrides the\n");
    usage.append ("                      default CIM Server Service Name\n");
    usage.append ("    -remove [name]  - removes pegasus as a Windows NT Service\n");
    usage.append ("                      [name] is optional and overrides the\n");
    usage.append ("                      default CIM Server Service Name\n");
    usage.append ("    -start [name]   - starts pegasus as a Windows NT Service\n");
    usage.append ("                      [name] is optional and overrides the\n");
    usage.append ("                      default CIM Server Service Name\n");
    usage.append ("    -stop [name]    - stops pegasus as a Windows NT Service\n");
    usage.append ("                      [name] is optional and overrides the\n");
    usage.append ("                      default CIM Server Service Name\n\n");
#endif
    usage.append ("  configProperty=value\n");
    usage.append ("                    - sets CIM Server configuration property\n");

    cout << endl;
#if defined(PEGASUS_OS_HPUX) || defined(PEGASUS_PLATFORM_LINUX_IA64_GNU)
    cout << PLATFORM_PRODUCT_NAME << " " << PLATFORM_PRODUCT_VERSION << endl;
#else
    cout << PEGASUS_NAME << PEGASUS_VERSION << endl;
#endif
    cout << endl;
    
#if defined(PEGASUS_OS_TYPE_WINDOWS)
    MessageLoaderParms parms("src.Server.cimserver.MENU.WINDOWS", usage);
#elif defined(PEGASUS_OS_HPUX) || defined(PEGASUS_PLATFORM_LINUX_IA64_GNU)
	MessageLoaderParms parms("src.Server.cimserver.MENU.HPUXLINUXIA64GNU", usage);
#else
	MessageLoaderParms parms("src.Server.cimserver.MENU.STANDARD", usage);
#endif
    //cout << usage << endl;
    cout << MessageLoader::getMessage(parms) << endl;
}

// l10n
//
// Dummy function for the Thread object associated with the initial thread.
// Since the initial thread is used to process CIM requests, this is
// needed to localize the exceptions thrown during CIM request processing.
// Note: This function should never be called! 
// 
PEGASUS_THREAD_RETURN PEGASUS_THREAD_CDECL dummyThreadFunc(void *parm)
{
   return((PEGASUS_THREAD_RETURN)0);	
}

void shutdownCIMOM(Uint32 timeoutValue)
{
    //
    // Create CIMClient object
    //
    CIMClient client;

    //
    // Get local host name
    //
    String hostStr = System::getHostName();

    //
    // open connection to CIMOM 
    //
    try
    {
        client.connectLocal();

        //
        // set client timeout to 2 seconds
        //
        client.setTimeout(2000);
    }
    catch(Exception&)
    {
#ifdef PEGASUS_OS_OS400
	//l10n
	//Logger::put(Logger::ERROR_LOG, System::CIMSERVER, Logger::SEVERE,
		    //"Unable to connect to CIM Server.  CIM Server may not be running." );
	Logger::put_l(Logger::ERROR_LOG, System::CIMSERVER, Logger::SEVERE,
			"src.Server.cimserver.UNABLE_CONNECT_SERVER_MAY_NOT_BE_RUNNING",
		    "Unable to connect to CIM Server.  CIM Server may not be running." );
	// The server job may still be active but not responding.
	// Kill the job if it exists.
	if(cimserver_kill() == -1)
	   cimserver_exitRC(2);
	cimserver_exitRC(1);
#else
        //l10n
        //PEGASUS_STD(cerr) << "Unable to connect to CIM Server." << PEGASUS_STD(endl);
        //PEGASUS_STD(cerr) << "CIM Server may not be running." << PEGASUS_STD(endl);
        MessageLoaderParms parms("src.Server.cimserver.UNABLE_CONNECT_SERVER_MAY_NOT_BE_RUNNING",
                                                         "Unable to connect to CIM Server.\nCIM Server may not be running.\n");
        PEGASUS_STD(cerr) << MessageLoader::getMessage(parms);
        exit(1);
#endif
    }

    try
    {
        //
        // construct CIMObjectPath
        //
        String referenceStr = "//";
        referenceStr.append(hostStr);
        referenceStr.append("/");  
        referenceStr.append(PEGASUS_NAMESPACENAME_SHUTDOWN.getString());
        referenceStr.append(":");
        referenceStr.append(PEGASUS_CLASSNAME_SHUTDOWN.getString());
        CIMObjectPath reference(referenceStr);

        //
        // issue the invokeMethod request on the shutdown method
        //
        Array<CIMParamValue> inParams;
        Array<CIMParamValue> outParams;

        // set force option to true for now
        inParams.append(CIMParamValue("force",
            CIMValue(Boolean(true))));

        inParams.append(CIMParamValue("timeout",
            CIMValue(Uint32(timeoutValue))));

        CIMValue retValue = client.invokeMethod(
            PEGASUS_NAMESPACENAME_SHUTDOWN,
            reference,
            "shutdown",
            inParams,
            outParams);
    }
    catch(CIMException& e)
    {
#ifdef PEGASUS_OS_OS400

	if (e.getCode() == CIM_ERR_INVALID_NAMESPACE)
	{
	    //l10n
	    //Logger::put(Logger::ERROR_LOG, System::CIMSERVER, Logger::SEVERE,
		    //"Failed to shutdown server: $0", "The repository may be empty.");
		Logger::put_l(Logger::ERROR_LOG, System::CIMSERVER, Logger::SEVERE,
			"src.Server.cimserver.SHUTDOWN_FAILED_REPOSITORY_EMPTY",
		    "Error in server shutdown: The repository may be empty.");
	}
	else
	{
	    //l10n
	    //Logger::put(Logger::ERROR_LOG, System::CIMSERVER, Logger::SEVERE,
			//"Failed to shutdown server: $0", e.getMessage());
		Logger::put_l(Logger::ERROR_LOG, System::CIMSERVER, Logger::SEVERE,
			"src.Server.cimserver.SHUTDOWN_FAILED",
			"Error in server shutdown: $0", e.getMessage());
	}
	// Kill the server job.
	if(cimserver_kill() == -1)
	   cimserver_exitRC(2);
#else
        //l10n - TODO
        MessageLoaderParms parms("src.Server.cimserver.SHUTDOWN_FAILED",
                                 "Error in server shutdown: ");
        PEGASUS_STD(cerr) << MessageLoader::getMessage(parms);
        if (e.getCode() == CIM_ERR_INVALID_NAMESPACE)
        {
            //
            // Repository may be empty.  
            //
            //l10n - TODO
            Logger::put_l(Logger::ERROR_LOG, System::CIMSERVER, Logger::SEVERE,
                "src.Server.cimserver.SHUTDOWN_FAILED_REPOSITORY_EMPTY",
                "Error in server shutdown: The repository may be empty.");
            MessageLoaderParms parms("src.Server.cimserver.REPOSITORY_EMPTY",
                                     "The repository may be empty.");
            PEGASUS_STD(cerr) << MessageLoader::getMessage(parms) << PEGASUS_STD(endl);
        }
        else
        {
            //l10n - TODO
            Logger::put_l(Logger::ERROR_LOG, System::CIMSERVER, Logger::SEVERE,
                "src.Server.cimserver.SHUTDOWN_FAILED",
                "Error in server shutdown: $0", e.getMessage());
            PEGASUS_STD(cerr) << e.getMessage() << PEGASUS_STD(endl);
        }

	// Kill the cimserver process 
	if (cimserver_kill() == 0)
        {
            //l10n - TODO
            Logger::put_l(Logger::ERROR_LOG, System::CIMSERVER, Logger::SEVERE,
                "src.Server.cimserver.SERVER_FORCED_SHUTDOWN",
			"Forced shutdown initiated.");
            MessageLoaderParms parms("src.Server.cimserver.SERVER_FORCED_SHUTDOWN",
                                     "Forced shutdown initiated.");
            PEGASUS_STD(cerr) << MessageLoader::getMessage(parms) << PEGASUS_STD(endl);
        }
        exit(1);
#endif
    }
    catch(Exception&)
    {
        //
        // This may mean that the CIM Server has terminated, causing this
        // client to get a "Empty HTTP response message" exception.  It may
        // also mean that the CIM Server is taking longer than 2 seconds 
        // (client timeout value) to terminate, causing this client to 
        // timeout with a "connection timeout" exception.
        //
        // Check to see if CIM Server is still running.  If CIM Server
        // is still running and the shutdown timeout has not expired,
        // loop and wait one second until either the CIM Server is
        // terminated or timeout expires.  If timeout expires and
        // the CIM Server is still running, kill the CIMServer process.
        // 
        Uint32 maxWaitTime = timeoutValue - 2;
        Boolean running = isCIMServerRunning();
        while ( running && maxWaitTime > 0 )
        {
            System::sleep(1);
            running = isCIMServerRunning();
            maxWaitTime = maxWaitTime - 1;
        }

        if (running)
        {
	   int kill_rc = cimserver_kill();
	   
#ifdef PEGASUS_OS_OS400
	    if(kill_rc == -1)
		cimserver_exitRC(2);
	    cimserver_exitRC(1);
#endif

#if defined(PEGASUS_OS_HPUX) || defined(PEGASUS_PLATFORM_LINUX_GENERIC_GNU) || defined(PEGASUS_PLATFORM_ZOS_ZSERIES_IBM)
	    if (kill_rc != -1)
            {
                //l10n - TODO
                Logger::put_l(Logger::ERROR_LOG, System::CIMSERVER, Logger::SEVERE,
                    "src.Server.cimserver.TIMEOUT_EXPIRED_SERVER_KILLED",
                    "Shutdown timeout expired.  Forced shutdown initiated.");
                MessageLoaderParms parms("src.Server.cimserver.TIMEOUT_EXPIRED_SERVER_KILLED",
                    "Shutdown timeout expired.  Forced shutdown initiated.");
                cout << MessageLoader::getMessage(parms) << endl;
                exit(0);
            }
#endif
        }
    }

    return;
}


/////////////////////////////////////////////////////////////////////////
//  MAIN
//////////////////////////////////////////////////////////////////////////
int main(int argc, char** argv)
{
    String pegasusHome  = String::EMPTY;
    Boolean shutdownOption = false;

//l10n
// Set Message loading to process locale
MessageLoader::_useProcessLocale = true; 
//l10n

#ifdef PEGASUS_OS_OS400

  VFYPTRS_INCDCL;               // VFYPTRS local variables

  // verify pointers
  #pragma exception_handler (qsyvp_excp_hndlr,qsyvp_excp_comm_area,\
    0,_C2_MH_ESCAPE)
    for( int arg_index = 1; arg_index < argc; arg_index++ ){
	VFYPTRS(VERIFY_SPP_NULL(argv[arg_index]));
    }
  #pragma disable_handler

    // Convert the args to ASCII
    for(Uint32 i = 0;i< argc;++i)
    {
	EtoA(argv[i]);
    }

    // Initialize Pegasus home to the shipped OS/400 directory.
    pegasusHome = OS400_DEFAULT_PEGASUS_HOME;
#endif

#ifndef PEGASUS_OS_TYPE_WINDOWS
    //
    // Get environment variables:
    //
#ifdef PEGASUS_OS_OS400
#pragma convert(37)
    const char* tmp = getenv("PEGASUS_HOME");
#pragma convert(0)
    char home[256] = {0};
    if (tmp && strlen(tmp) < 256)
    {
	strcpy(home, tmp);
	EtoA(home);
	pegasusHome = home;
    }
#else
    const char* tmp = getenv("PEGASUS_HOME");

    if (tmp)
    {
        pegasusHome = tmp;
    }
#endif

    FileSystem::translateSlashes(pegasusHome);
#else

  // windows only
  setHome(pegasusHome);
#endif

        // Get help, version, and shutdown options

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
                if (*option == OPTION_VERSION &&
                    strlen(option) == 1)
                {
#if defined(PEGASUS_OS_HPUX) || defined(PEGASUS_PLATFORM_LINUX_IA64_GNU)
                    cout << PLATFORM_PRODUCT_VERSION << endl;
#else
                    cout << PEGASUS_VERSION << endl;
#endif
                    exit(0);
                }
                //
                // Check to see if user asked for help (-h option):
                //
                else if (*option == OPTION_HELP &&
                        (strlen(option) == 1))
                {
                    PrintHelp(argv[0]);
                    exit(0);
                }
#if !defined(PEGASUS_OS_HPUX) && !defined(PEGASUS_PLATFORM_LINUX_IA64_GNU)
                else if (*option == OPTION_HOME &&
                        (strlen(option) == 1))
                {
                    if (i + 1 < argc)
                    {
                        pegasusHome.assign(argv[i + 1]);
                    }
                    else
                    {
                        //l10n
                        //cout << "Missing argument for option -" << option << endl;
                        String opt(option);
                        MessageLoaderParms parms("src.Server.cimserver.MISSING_ARGUMENT",
                						 "Missing argument for option -$0",
                						 opt);
                		cout << MessageLoader::getMessage(parms) << endl;
                        exit(0);
                    }

                    memmove(&argv[i], &argv[i + 2], (argc-i-1) * sizeof(char*));
                    argc -= 2;
                }
#endif
#if defined(PEGASUS_OS_HPUX)
                //
                // Check to see if user asked for the version (-X option):
                //
                if (*option == OPTION_BINDVERBOSE &&
                        (strlen(option) == 1))
                {
		    System::bindVerbose = true;
                    //l10n
                    //cout << "Unsupported debug option, BIND_VERBOSE, enabled." 
                         //<< endl;
                    MessageLoaderParms parms("src.Server.cimserver.UNSUPPORTED_DEBUG_OPTION",
                						 "Unsupported debug option, BIND_VERBOSE, enabled.");
                	cout << MessageLoader::getMessage(parms) << endl;
                    // remove the option from the command line
                    memmove(&argv[i], &argv[i + 1], (argc-i) * sizeof(char*));
                    argc--;   
                }
#endif
                //
                // Check to see if user asked for shutdown (-s option):
                //
                else if (*option == OPTION_SHUTDOWN &&
                        (strlen(option) == 1))
                {
                    //
                    // check to see if user is root
                    //
#ifndef PEGASUS_OS_OS400
                    if (!System::isPrivilegedUser(System::getEffectiveUserName()))
                    {
                        //l10n
                        //cout << "You must have superuser privilege to run ";
                        //cout << "cimserver." << endl;
                        MessageLoaderParms parms("src.Server.cimserver.SUPERVISOR_PRIVILEGE_TO_RUN_SERVER",
                        						 "You must have superuser privilege to run cimserver.");
                       
                        cout << MessageLoader::getMessage(parms) << endl;
                        exit(0);
                    }
#endif

                    //
                    // Check to see if shutdown has already been specified:
                    //
                    if (shutdownOption)
                    {
                        //l10n
                        //cout << "Duplicate shutdown option specified." << endl;
                        MessageLoaderParms parms("src.Server.cimserver.DUPLICATE_SHUTDOWN_OPTION",
                        						 "Duplicate shutdown option specified.");
                       
                        cout << MessageLoader::getMessage(parms) << endl;
                        exit(0);
                    }

                    shutdownOption = true;
 
                    // remove the option from the command line
                    memmove(&argv[i], &argv[i + 1], (argc-i) * sizeof(char*));
                    argc--;   
                }
                else
                    i++;
            }
            else
                i++;
        }

    //
    // Set the value for pegasusHome property
    //
    ConfigManager::setPegasusHome(pegasusHome);

    //
    // Do the plaform specific run
    //

    return platform_run( argc, argv, shutdownOption );
}


//
// The main, common, running code
//
// NOTE: Do NOT call exit().  Use return(), otherwise some platforms 
// will fail to shutdown properly/cleanly.
//
// TODO: Current change minimal for platform "service" shutdown bug fixes.  
// Perhpas further extract out common stuff and put into main(), put 
// daemon stuff into platform specific platform_run(), etc.  
// Note: make sure to not put error handling stuff that platform 
// specific runs may need to deal with bettter (instead of exit(), etc).
//

int cimserver_run( int argc, char** argv, Boolean shutdownOption )
{
    String logsDirectory = String::EMPTY;
    Boolean daemonOption = false;

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
        GetOptions(configManager, argc, argv);
    }
    catch (Exception& e)
    {
#ifdef PEGASUS_OS_OS400
	Logger::put(Logger::ERROR_LOG, System::CIMSERVER, Logger::SEVERE,
			"$0: $1",argv[0] ,e.getMessage());
#else
        cerr << argv[0] << ": " << e.getMessage() << endl;
#endif
        return(1);
    }

// l10n
	// Set the home directory, msg sub-dir, into the MessageLoader.
	// This will be the default directory where the resource bundles 
	// are found.
	String messagesDir = String::EMPTY;
#ifdef PEGASUS_PLATFORM_OS400_ISERIES_IBM
	messagesDir = OS400_DEFAULT_MESSAGE_SOURCE;
#else
	messagesDir = ConfigManager::getHomedPath("msg");
#endif
	MessageLoader::setPegasusMsgHome(messagesDir);		

    Boolean enableHttpConnection = String::equal(
        configManager->getCurrentValue("enableHttpConnection"), "true");
    Boolean enableHttpsConnection = String::equal(
        configManager->getCurrentValue("enableHttpsConnection"), "true");

    // Make sure at least one connection is enabled
#ifndef PEGASUS_LOCAL_DOMAIN_SOCKET
    if (!enableHttpConnection && !enableHttpsConnection)
    {
        //l10n
        //Logger::put(Logger::STANDARD_LOG, System::CIMSERVER, Logger::WARNING,
            //"Neither HTTP nor HTTPS connection is enabled.  "
            //"CIMServer will not be started.");
        Logger::put_l(Logger::STANDARD_LOG, System::CIMSERVER, Logger::WARNING,
        	"src.Server.cimserver.HTTP_NOT_ENABLED_SERVER_NOT_STARTING",
            "Neither HTTP nor HTTPS connection is enabled.  CIMServer will not be started.");
        //cerr << "Neither HTTP nor HTTPS connection is enabled.  "
            //"CIMServer will not be started." << endl;
        MessageLoaderParms parms("src.Server.cimserver.HTTP_NOT_ENABLED_SERVER_NOT_STARTING",
        						 "Neither HTTP nor HTTPS connection is enabled.  CIMServer will not be started.");
        cerr << MessageLoader::getMessage(parms) << endl;
        return(1);
    }
#endif

    try
    {
        //
        // Check to see if we should Pegasus as a daemon
        //

        if (String::equal(configManager->getCurrentValue("daemon"), "true"))
        {
            daemonOption = true;
        }
	
        // Get the log file directory definition.
        // We put String into Cstring because
        // Directory functions only handle Cstring.
        // ATTN-KS: create String based directory functions.

        logsDirectory = configManager->getCurrentValue("logdir");
        logsDirectory = 
	    ConfigManager::getHomedPath(configManager->getCurrentValue("logdir"));

        // Set up the Logger. This does not open the logs
        // Might be more logical to clean before set.
        // ATTN: Need tool to completely disable logging.

#if !defined(PEGASUS_OS_HPUX) && !defined(PEGASUS_PLATFORM_LINUX_IA64_GNU) && \
!defined(PEGASUS_OS_OS400)
        Logger::setHomeDirectory(logsDirectory);
#endif

        //
        // Check to see if we need to shutdown CIMOM 
        //
        if (shutdownOption)
        {
            String configTimeout = 
                configManager->getCurrentValue("shutdownTimeout");
            Uint32 timeoutValue = strtol(configTimeout.getCString(), (char **)0, 10);
            
            shutdownCIMOM(timeoutValue);

#ifdef PEGASUS_OS_OS400
	    //l10n
	    //Logger::put(Logger::ERROR_LOG, System::CIMSERVER, Logger::INFORMATION,
			//"CIM Server stopped.");  
		Logger::put_l(Logger::ERROR_LOG, System::CIMSERVER, Logger::INFORMATION,
			"src.Server.cimserver.SERVER_STOPPED",
			"CIM Server stopped.");
            cimserver_exitRC(0);
#else
			//l10n
            //cout << "CIM Server stopped." << endl;
            MessageLoaderParms parms("src.Server.cimserver.SERVER_STOPPED",
            						 "CIM Server stopped.");

            cout << MessageLoader::getMessage(parms) << endl;
            return(0);
#endif
        }

        // Leave this in until people get familiar with the logs.
#if !defined(PEGASUS_OS_HPUX) && !defined(PEGASUS_PLATFORM_LINUX_IA64_GNU) && \
!defined(PEGASUS_OS_OS400)
        //l10n
        //cout << "Logs Directory = " << logsDirectory << endl;
        MessageLoaderParms parms("src.Server.cimserver.LOGS_DIRECTORY",
            				     "Logs Directory = ");
        cout << MessageLoader::getMessage(parms) << logsDirectory << endl;
#endif


    }
    catch (UnrecognizedConfigProperty& e)
    {

#ifdef PEGASUS_OS_OS400
	//l10n
	//Logger::put(Logger::ERROR_LOG, System::CIMSERVER, Logger::SEVERE,
		    //"Error: $0",e.getMessage());  
	Logger::put_l(Logger::ERROR_LOG, System::CIMSERVER, Logger::SEVERE,
			"src.Server.cimserver.ERROR",
		    "Error: $0",e.getMessage()); 
#else
	//l10n
	//cout << "Error: " << e.getMessage() << endl;
	MessageLoaderParms parms("src.Server.cimserver.ERROR",
							 "Error: $0",
							 e.getMessage());
	cout << MessageLoader::getMessage(parms) << endl;
#endif
    }

    Uint32 portNumberHttps;
    Uint32 portNumberHttp;

    if (enableHttpsConnection)
    {
        String httpsPort = configManager->getCurrentValue("httpsPort");
        CString portString = httpsPort.getCString();
        char* end = 0;
        Uint32 port = strtol(portString, &end, 10);
        assert(end != 0 && *end == '\0');

        //
        // Look up the WBEM-HTTPS port number
        //
        portNumberHttps = System::lookupPort(WBEM_HTTPS_SERVICE_NAME, port);
    }

    if (enableHttpConnection)
    {
        String httpPort = configManager->getCurrentValue("httpPort");
        CString portString = httpPort.getCString();
        char* end = 0;
        Uint32 port = strtol(portString, &end, 10);
        assert(end != 0 && *end == '\0');

        //
        // Look up the WBEM-HTTP port number
        //
        portNumberHttp = System::lookupPort(WBEM_HTTP_SERVICE_NAME, port);
    }

    // Put out startup up message.
#if !defined(PEGASUS_OS_HPUX) && !defined(PEGASUS_PLATFORM_LINUX_IA64_GNU) && \
!defined(PEGASUS_OS_OS400)
    cout << PEGASUS_NAME << PEGASUS_VERSION << endl;
    //l10n
    //cout << "Built " << __DATE__ << " " << __TIME__ << endl;
    //cout <<"Starting..."
    MessageLoaderParms parms("src.Server.cimserver.STARTUP_MESSAGE",
    						 "Built $0 $1\nStarting...",
    						 __DATE__,
    						 __TIME__);
#endif

//l10n
// reset message loading to NON-process locale
MessageLoader::_useProcessLocale = false; 
//l10n

    // do we need to run as a daemon ?
    if (daemonOption)
    {
        if(-1 == cimserver_fork())
#ifndef PEGASUS_OS_OS400
	{
	    return(-1);
	}
#else
	{
            return(-1);
	}
	else
	{
	    return(0);
	}
#endif
	
    }

// l10n
    // Now we are after the fork...
    // Create a dummy Thread object that can be used to store the
    // AcceptLanguages object for CIM requests that are serviced
    // by this thread (initial thread of server).  Need to do this
    // because this thread is not in a ThreadPool, but is used
    // to service CIM requests.
    // The run function for the dummy Thread should never be called,
    Thread *dummyInitialThread = new Thread(dummyThreadFunc, NULL, false);
    Thread::setCurrent(dummyInitialThread); 
    AcceptLanguages default_al;
    try{
    	 default_al = AcceptLanguages::getDefaultAcceptLanguages();   
    	 Thread::setLanguages(new AcceptLanguages(default_al));
    }catch(InvalidAcceptLanguageHeader& e){
    	  Logger::put_l(Logger::ERROR_LOG, System::CIMSERVER, Logger::SEVERE,
				  "src.Server.cimserver.FAILED_TO_SET_PROCESS_LOCALE",
		    	  "Could not convert the system process locale into a valid AcceptLanguage format.");  
		  Logger::put(Logger::ERROR_LOG, System::CIMSERVER, Logger::SEVERE,
		    				 e.getMessage()); 
    }
    
    

#ifdef PEGASUS_OS_OS400
    // Special server initialization code for OS/400.
    if (cimserver_initialize() != 0)
    {
	// do some logging here!
	//l10n
	//Logger::put(Logger::ERROR_LOG, System::CIMSERVER, Logger::SEVERE,
		    //"CIM Server failed to initialize"); 
	Logger::put_l(Logger::ERROR_LOG, System::CIMSERVER, Logger::SEVERE,
				  "src.Server.cimserver.SERVER_FAILED_TO_INITIALIZE",
		    	  "CIM Server failed to initialize");  
	return(-1);
    } 
#endif


#if defined(PEGASUS_OS_HPUX) || defined(PEGASUS_PLATFORM_LINUX_GENERIC_GNU) || defined(PEGASUS_PLATFORM_ZOS_ZSERIES_IBM)
    umask(S_IWGRP|S_IWOTH);

    //
    // check if CIMServer is already running
    // if CIMServer is already running, print message and 
    // notify parent process (if there is a parent process) to terminate
    //
    if(isCIMServerRunning())
    {
	//l10n
		//cout << "Unable to start CIMServer." << endl;
		//cout << "CIMServer is already running." << endl;
		MessageLoaderParms parms("src.Server.cimserver.UNABLE_TO_START_SERVER_ALREADY_RUNNING",
					 "Unable to start CIMServer.\nCIMServer is already running.");
	PEGASUS_STD(cerr) << MessageLoader::getMessage(parms) << PEGASUS_STD(endl);

	//
        // notify parent process (if there is a parent process) to terminate
        //
        if (daemonOption)
                notify_parent(1);

        return(1);
    }
     
#endif

    // try loop to bind the address, and run the server
    try
    {





#if defined(PEGASUS_MONITOR2) 
	monitor_2 monitor;
	CIMServer server(&monitor);
#else

	Monitor monitor(true);
	CIMServer server(&monitor);
#endif

	CimserverHolder cimserverHolder( &server );

        if (enableHttpConnection)
        {
            server.addAcceptor(false, portNumberHttp, false);
            //l10n
            //Logger::put(Logger::STANDARD_LOG, System::CIMSERVER, Logger::INFORMATION,
                        //"Listening on HTTP port $0.", portNumberHttp);
                        
            Logger::put_l(Logger::STANDARD_LOG, System::CIMSERVER, Logger::INFORMATION,
            				"src.Server.cimserver.LISTENING_ON_HTTP_PORT",
                        	"Listening on HTTP port $0.", portNumberHttp);
        }
        if (enableHttpsConnection)
        {
            server.addAcceptor(false, portNumberHttps, true);
            //l10n
            //Logger::put(Logger::STANDARD_LOG, System::CIMSERVER, Logger::INFORMATION,
                        //"Listening on HTTPS port $0.", portNumberHttps);
            Logger::put_l(Logger::STANDARD_LOG, System::CIMSERVER, Logger::INFORMATION,
            				"src.Server.cimserver.LISTENING_ON_HTTPS_PORT",
                        	"Listening on HTTPS port $0.", portNumberHttps);
        }
#ifdef PEGASUS_LOCAL_DOMAIN_SOCKET
        server.addAcceptor(true, 0, false);
        //l10n
        //Logger::put(Logger::STANDARD_LOG, System::CIMSERVER, Logger::INFORMATION,
                    //"Listening on local connection socket.");
        Logger::put_l(Logger::STANDARD_LOG, System::CIMSERVER, Logger::INFORMATION,
		      "src.Server.cimserver.LISTENING_ON_LOCAL",
		      "Listening on local connection socket.");
#endif

#if !defined(PEGASUS_OS_HPUX) && !defined(PEGASUS_PLATFORM_LINUX_IA64_GNU) && !defined(PEGASUS_OS_OS400)
        if (enableHttpConnection)
        {
            //l10n
            //cout << "Listening on HTTP port " << portNumberHttp << endl;
	  MessageLoaderParms parms("src.Server.cimserver.LISTENING_ON_HTTP_PORT",
				   "Listening on HTTP port $0.", portNumberHttp);
           	cout << MessageLoader::getMessage(parms) << endl;
        }
        if (enableHttpsConnection)
        {
            //l10n
            //cout << "Listening on HTTPS port " << portNumberHttps << endl;
            MessageLoaderParms parms("src.Server.cimserver.LISTENING_ON_HTTPS_PORT",
				     "Listening on HTTPS port $0.", portNumberHttps);
           	cout << MessageLoader::getMessage(parms) << endl;
        }
# ifdef PEGASUS_LOCAL_DOMAIN_SOCKET
        //l10n
        //cout << "Listening on local connection socket" << endl;
        MessageLoaderParms parms("src.Server.cimserver.LISTENING_ON_LOCAL",
				 "Listening on local connection socket.");
        cout << MessageLoader::getMessage(parms) << endl;
# endif
#endif

        // bind throws an exception if the bind fails
        server.bind();

	// notify parent process (if there is a parent process) to terminate 
        // so user knows that there is cimserver ready to serve CIM requests.
	if (daemonOption)
		notify_parent(0);

	time_t last = 0;

#if defined(PEGASUS_OS_HPUX) || defined(PEGASUS_PLATFORM_LINUX_GENERIC_GNU) || defined(PEGASUS_PLATFORM_ZOS_ZSERIES_IBM)
        //
        // create a file to indicate that the cimserver has started and
        // save the process id of the cimserver process in the file
        //
        // remove the old file if it exists
        System::removeFile(CIMSERVER_START_FILE);

        // open the file
        FILE *pid_file = fopen(CIMSERVER_START_FILE, "w");

        if (pid_file)
        {
            // save the pid in the file
            fprintf(pid_file, "%ld\n", (long)server_pid);
            fclose(pid_file);
        }
#endif

#if !defined(PEGASUS_OS_HPUX) && !defined(PEGASUS_PLATFORM_LINUX_IA64_GNU) && \
!defined(PEGASUS_OS_OS400)
	cout << "Started. " << endl;
#endif
	
        // Put server started message to the logger
#if defined(PEGASUS_OS_HPUX) || defined(PEGASUS_PLATFORM_LINUX_IA64_GNU)
        //l10n
        //Logger::put(Logger::STANDARD_LOG, System::CIMSERVER, Logger::INFORMATION,
                    //"Started $0 version $1.",
                    //PLATFORM_PRODUCT_NAME, PLATFORM_PRODUCT_VERSION);
        Logger::put_l(Logger::STANDARD_LOG, System::CIMSERVER, Logger::INFORMATION,
        			"src.Server.cimserver.STARTED_VERSION",
                    "Started $0 version $1.",
                    PLATFORM_PRODUCT_NAME, PLATFORM_PRODUCT_VERSION);
#else
		//l10n
        //Logger::put(Logger::STANDARD_LOG, System::CIMSERVER, Logger::INFORMATION,
                    //"Started $0 version $1.",
                    //PEGASUS_NAME, PEGASUS_VERSION);
        Logger::put_l(Logger::STANDARD_LOG, System::CIMSERVER, Logger::INFORMATION,
        			"src.Server.cimserver.STARTED_VERSION",
                    "Started $0 version $1.",
                    PEGASUS_NAME, PEGASUS_VERSION);
#endif

	
        //
        // Loop to call CIMServer's runForever() method until CIMServer
        // has been shutdown
        //
	while( !server.terminated() )
	{

	  server.runForever();

	}
	MessageQueueService::force_shutdown(true);
        //
        // normal termination
	//
        // Put server shutdown message to the logger
#if defined(PEGASUS_OS_HPUX) || defined(PEGASUS_PLATFORM_LINUX_IA64_GNU)
        //l10n
        //Logger::put(Logger::STANDARD_LOG, System::CIMSERVER, Logger::INFORMATION,
            //"$0 stopped.", PLATFORM_PRODUCT_NAME);
        Logger::put_l(Logger::STANDARD_LOG, System::CIMSERVER, Logger::INFORMATION,
        	"src.Server.cimserver.STOPPED",
            "$0 stopped.", PLATFORM_PRODUCT_NAME);
#else
		//l10n
        //Logger::put(Logger::STANDARD_LOG, System::CIMSERVER, Logger::INFORMATION,
            //"$0 stopped.", PEGASUS_NAME);
        Logger::put_l(Logger::STANDARD_LOG, System::CIMSERVER, Logger::INFORMATION,
        	"src.Server.cimserver.STOPPED",
            "$0 stopped.", PEGASUS_NAME);
#endif

#if defined(PEGASUS_OS_HPUX) || defined(PEGASUS_PLATFORM_LINUX_GENERIC_GNU) || defined(PEGASUS_PLATFORM_ZOS_ZSERIES_IBM)
        //
        // close the file created at startup time to indicate that the 
        // cimserver has terminated normally.
        //
        FileSystem::removeFile(CIMSERVER_START_FILE);
#endif
    }
    catch(Exception& e)
    {

	//l10n
	//Logger::put(Logger::STANDARD_LOG, System::CIMSERVER, Logger::WARNING,
		    //"Error: $0", e.getMessage()); 
	Logger::put_l(Logger::STANDARD_LOG, System::CIMSERVER, Logger::WARNING,
			"src.Server.cimserver.ERROR",
		    "Error: $0", e.getMessage());  

#ifndef PEGASUS_OS_OS400
	//l10n
	//PEGASUS_STD(cerr) << "Error: " << e.getMessage() << PEGASUS_STD(endl);
	MessageLoaderParms parms("src.Server.cimserver.ERROR",
							 "Error: $0", e.getMessage());
	PEGASUS_STD(cerr) << MessageLoader::getMessage(parms) << PEGASUS_STD(endl);

#endif

	//
        // notify parent process (if there is a parent process) to terminate
        //
        if (daemonOption)
                notify_parent(1);

        return 1;
    }

    return 0;
}
