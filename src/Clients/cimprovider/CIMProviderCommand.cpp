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
// Author: Yi Zhou, Hewlett Packard Company (yi_zhou@hp.com)
//
// Modified By:  Carol Ann Krug Graves, Hewlett-Packard Company
//               (carolann_graves@hp.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#include <Pegasus/Common/Config.h>

#include <iostream>

#include <Clients/cliutils/Command.h>
#include <Clients/cliutils/CommandException.h>

#include <Pegasus/getoopt/getoopt.h>

#include <Pegasus/Client/CIMClient.h>
#include <Pegasus/Common/CIMProperty.h>
#include <Pegasus/Common/CIMObjectPath.h>
#include <Pegasus/Common/CIMStatusCode.h>
#include <Pegasus/Common/Exception.h>
#include <Pegasus/Common/String.h>
#include <Pegasus/Common/System.h>
#include <Pegasus/Common/Constants.h>
#include <Pegasus/Common/PegasusVersion.h>


#ifdef PEGASUS_OS_OS400
#include "qycmutiltyUtility.H"
#include "vfyptrs.cinc"
#include <stdio.h>
#endif

PEGASUS_USING_STD;

PEGASUS_NAMESPACE_BEGIN

//l10n
/**
 * The CLI message resource name
 */

static const char MSG_PATH [] 				= "pegasus/pegasusCLI";

//l10n end

/**
    The command name.
*/
static const char COMMAND_NAME []              = "cimprovider";

/**
    The name of the Name property for PG_ProviderModule class 
*/
static const CIMName _PROPERTY_PROVIDERMODULE_NAME = CIMName ("Name");

/**
   The name of the operational status property
*/
static const CIMName _PROPERTY_OPERATIONALSTATUS  = 
    CIMName ("OperationalStatus");

/**
   The name of the provider module name  property for PG_Provider class
*/
static const CIMName _PROPERTY_PROVIDERMODULENAME = 
    CIMName ("ProviderModuleName");

/**
   The name of the Name property for PG_Provider class
*/
static const CIMName _PROPERTY_PROVIDER_NAME = CIMName ("Name");

/**
    The usage string for this command.  This string is displayed
    when an error occurs in parsing or validating the command line.
*/
static const char USAGE []                     = "usage: ";

/**
    This constant represents the getoopt argument designator
*/
static const char GETOPT_ARGUMENT_DESIGNATOR   = ':';

/*
    These constants represent the operation modes supported by the CLI.
    Any new operation should be added here.
*/

/**
    This constant signifies that an operation option has not been recorded
*/
static const Uint32 OPERATION_TYPE_UNINITIALIZED  = 0;

/**
    This constant represents a disable provider operation
*/
static const Uint32 OPERATION_TYPE_DISABLE            = 1;

/**
    This constant represents a remove provider operation
*/
static const Uint32 OPERATION_TYPE_REMOVE         = 2;

/**
    This constant represents a enable provider operation
*/
static const Uint32 OPERATION_TYPE_ENABLE         = 3;

/**
    This constant represents a list operation
*/
static const Uint32 OPERATION_TYPE_LIST           = 4;

/**
    The constants representing the messages.
*/

static const char NOT_PRIVILEGED_USER []         = 
                     "Error, you must have superuser privilege to run cimprovider."; 
                     
static const char NOT_PRIVILEGED_USER_KEY [] = 
		"Clients.cimprovider.CIMProviderCommand.NOT_PRIVILEGED_USER";
		                    
static const char CIMOM_NOT_RUNNING []         = 
                        "The CIM server may not be running.";

static const char CIMOM_NOT_RUNNING_KEY [] =
		"Clients.cimprovider.CIMProviderCommand.CIMOM_NOT_RUNNING";

static const char DELETE_PROVIDER_FAILURE []    = 
                        "Failed to delete...";

static const char DELETE_PROVIDER_FAILURE_KEY []    =
		"Clients.cimprovider.CIMProviderCommand.DELETE_PROVIDER_FAILURE";


static const char START_PROVIDER_FAILURE []    = 
                        "Failed to enable provider module.";

static const char START_PROVIDER_FAILURE_KEY []    =
		"Clients.cimprovider.CIMProviderCommand.START_PROVIDER_FAILURE";

static const char STOP_PROVIDER_FAILURE []  = 
                        "Failed to disable provider module.";

static const char STOP_PROVIDER_FAILURE_KEY []  = 
		"Clients.cimprovider.CIMProviderCommand.STOP_PROVIDER_FAILURE";

static const char LIST_PROVIDERS_FAILURE [] = 
                        "Failed to list... ";

static const char LIST_PROVIDERS_FAILURE_KEY [] =
		"Clients.cimprovider.CIMProviderCommand.LIST_PROVIDERS_FAILURE";

static const char DELETE_PROVIDER_MODULE_SUCCESS []    = 
                        "Provider module deleted successfully.";

static const char DELETE_PROVIDER_MODULE_SUCCESS_KEY []    = 
		"Clients.cimprovider.CIMProviderCommand.DELETE_PROVIDER_SUCCESS";		

static const char DELETE_PROVIDER_SUCCESS []    = 
                        "Provider deleted successfully.";

static const char DELETE_PROVIDER_SUCCESS_KEY []    = 
		"Clients.cimprovider.CIMProviderCommand.DELETE_PROVIDER_SUCCESS";

static const char START_PROVIDER_SUCCESS []  = 
                        "Provider module enabled successfully.";

static const char START_PROVIDER_SUCCESS_KEY []  = 
		"Clients.cimprovider.CIMProviderCommand.START_PROVIDER_SUCCESS";

static const char STOP_PROVIDER_SUCCESS []  = 
                        "Provider module disabled successfully.";

static const char STOP_PROVIDER_SUCCESS_KEY []  =
		"Clients.cimprovider.CIMProviderCommand.STOP_PROVIDER_SUCCESS";

static const char PROVIDER_ALREADY_STOPPED []  = 
                        "Provider module already disabled.";

static const char PROVIDER_ALREADY_STOPPED_KEY []  =
		"Clients.cimprovider.CIMProviderCommand.PROVIDER_ALREADY_STOPPED";

static const char PROVIDER_ALREADY_STARTED []  = 
                        "Provider module already enabled.";

static const char PROVIDER_ALREADY_STARTED_KEY []  = 
		"Clients.cimprovider.CIMProviderCommand.PROVIDER_ALREADY_STARTED";

static const char CANNOT_START_PROVIDER []  = 
                        "Provider module can not be enabled since it is disabling.";
                        
static const char CANNOT_START_PROVIDER_KEY []  =
		"Clients.cimprovider.CIMProviderCommand.CANNOT_START_PROVIDER";

static const char PROVIDER_NOT_REGISTERED[] =
		"Specified provider was not registered.";

static const char PROVIDER_NOT_REGISTERED_KEY [] =
		"Clients.cimprovider.CIMProviderCommand.PROVIDER_NOT_REGISTERED";

static const char DELETEING_PROVIDER_MODULE[] =
			"Deleting provider module...";

static const char DELETEING_PROVIDER_MODULE_KEY [] =
		"Clients.cimprovider.CIMProviderCommand.DELETEING_PROVIDER_MODULE";

static const char DELETEING_PROVIDER[] =
			"Deleting provider...";

static const char DELETEING_PROVIDER_KEY[] =
		"Clients.cimprovider.CIMProviderCommand.DELETEING_PROVIDER";

static const char STARTING_PROVIDER_MODULE[] =
			"Enabling provider module...";
			
static const char STARTING_PROVIDER_MODULE_KEY[] =
		"Clients.cimprovider.CIMProviderCommand.STARTING_PROVIDER_MODULE";

static const char STOPING_PROVIDER_MODULE[] =
			"Disabling provider module...";

static const char STOPING_PROVIDER_MODULE_KEY[] =
		"Clients.cimprovider.CIMProviderCommand.STOPING_PROVIDER_MODULE";

static const char NO_MODULE_REGISTERED[] =
			"No modules registered for listing.";

static const char NO_MODULE_REGISTERED_KEY[] =
		"Clients.cimprovider.CIMProviderCommand.NO_MODULE_REGISTERED";

static const char ERR_OPTION_NOT_SUPPORTED[] =
			"-p option was not supported.";

static const char ERR_OPTION_NOT_SUPPORTED_KEY[] =
		"Clients.cimprovider.CIMProviderCommand.ERR_OPTION_NOT_SUPPORTED";

static const char ERR_MODULE_NOT_REGISTERED[] =
			"Specified provider module was not registered.";

static const char ERR_MODULE_NOT_REGISTERED_KEY[] =
		"Clients.cimprovider.CIMProviderCommand.ERR_MODULE_NOT_REGISTERED";

static const char ERR_PROVIDER_NOT_REGISTERED[] =
			"Specified provider was not registered.";

static const char ERR_PROVIDER_NOT_REGISTERED_KEY[] =
		"Clients.cimprovider.CIMProviderCommand.ERR_PROVIDER_NOT_REGISTERED";

static const char PG_PROVIDER_SCHEMA_NOT_LOADED []  =
    "Please make sure that the registration schema is loaded on the CIMOM.";

static const char PG_PROVIDER_SCHEMA_NOT_LOADED_KEY []  =
		"Clients.cimprovider.CIMProviderCommand.PG_PROVIDER_SCHEMA_NOT_LOADED";

static const char REQUIRED_ARGS_MISSING []        =
			"Required arguments missing.";

static const char REQUIRED_ARGS_MISSING_KEY []        =
		"Clients.cimprovider.CIMProviderCommand.REQUIRED_ARGS_MISSING";

static const char INVALID_ARGS []        =
                        "Invalid arguments.";

static const char INVALID_ARGS_KEY []        =
		"Clients.cimprovider.CIMProviderCommand.INVALID_ARGS";
		
static const char UNEXPECTED_OPTION [] = "Unexpected Option.";

static const char UNEXPECTED_OPTION_KEY [] = 
	"Clients.cimprovider.CIMProviderCommand.UNEXPECTED_OPTION";

/**
    The option character used to specify disable a provider module.
*/
static const char   OPTION_DISABLE                 = 'd';

/**
    The option character used to specify remove a provider or module.
*/
static const char   OPTION_REMOVE              = 'r';

/**
    The option character used to specify enable a provider module.
*/
static const char   OPTION_ENABLE           = 'e';

/**
    The option character used to specify a module.
*/
static const char   OPTION_MODULE           = 'm';

/**
    The option character used to specify a provider.
*/
static const char   OPTION_PROVIDER           = 'p';

/**
    The option character used to specify listing of providers or provider modules.
*/
static const char   OPTION_LIST                = 'l';

/**
    The option character used to specify get module status.
*/
static const char   OPTION_STATUS                = 's';

#ifdef PEGASUS_OS_OS400
/**
    The option character used to specify no output to stdout or stderr.
*/
     static const char OPTION_QUIET_VALUE      = 'q';
#endif


/**
    The name of the Method that implements stop provider or module
*/
static const CIMName   STOP_METHOD             = CIMName ("stop");

/**
    The name of the Method that implements start provider or module
*/
static const CIMName   START_METHOD            = CIMName ("start");

/**
This is a CLI used to update providers of the CIM Server.  This command supports 
operations to stop, start, list and delete provider or module.  

@author Yi Zhou, Hewlett-Packard Company
*/

class CIMProviderCommand : public Command 
{

public:

    /**    
        Constructs a CIMProviderCommand and initializes instance variables.
    */
    CIMProviderCommand ();

    //
    // Overrides the virtual function setCommand from Command class
    // This is defined as an empty function. 
    //
    void setCommand (
		      Uint32                   argc, 
		      char*                    argv [])
		      throw (CommandFormatException)
    {
        // Empty function 
    }

    /**
    Parses the command line, validates the options, and sets instance 
    variables based on the option arguments. This implementation of
    setCommand includes the parameters for output and error stream. 

    @param  ostream    The stream to which command output is written.
    @param  ostream    The stream to which command errors are written.
    @param  args       The string array containing the command line arguments
    @param  argc       The int containing the arguments count

    @throws  CommandFormatException  if an error is encountered in parsing
                                     the command line
    */
    void setCommand (
                      ostream&                outPrintWriter, 
                      ostream&                errPrintWriter,
		      Uint32                  argc, 
		      char*                   argv []);

    /**
    Executes the command and writes the results to the output streams.

    @param ostream    The stream to which command output is written.
    @param ostream    The stream to which command errors are written.
    @return  0        if the command is successful
             1        if an error occurs in executing the command
    */
    Uint32 execute ( 
		      ostream&                outPrintWriter,
                      ostream&                errPrintWriter);


private:

    //
    // Delete a provider or module
    //
    // @param ostream        The stream to which command output is written.
    // @param ostream        The stream to which command errors are written.
    // 
    // @exception CIMException  if failed to delete provider module
    //
    void _deleteProvider
        (
        ostream&    		outPrintWriter, 
        ostream&    		errPrintWriter
        ); 

    //
    // Start a provider or module 
    //
    // @param ostream          The stream to which command output is written.
    // @param ostream          The stream to which command errors are written.
    // 
    // @exception CIMException  if failed to start provider module
    //
    void _StartProvider
        (
        ostream&                 outPrintWriter,
        ostream&                 errPrintWriter
        );

    //
    // Stop a provider or module 
    //  
    // @param ostream          The stream to which command output is written.
    // @param ostream          The stream to which command errors are written.
    //
    // @exception CIMException  if failed to stop provider
    //
    void _StopProvider
        (
        ostream&		outPrintWriter, 
        ostream&		errPrintWriter
        ); 

    //
    // List all the registered providers or modules.       
    // 
    // @param ostream        The stream to which command output is written.
    // @param ostream        The stream to which command errors are written.
    //
    void _ListProviders
    (
	ostream&                outPrintWriter,
	ostream&                errPrintWriter
    );

    //
    // Get module status
    //
    // @param ostream          The stream to which command output is written.
    // @param ostream          The stream to which command errors are written.
    //
    // @exception CIMException  if failed to get module status
    //
    void _GetStatus
        (
        ostream&                outPrintWriter,
        ostream&                errPrintWriter
        );

    // Get namedInstance for the provider module
    CIMInstance _getModuleInstance();

    // Get namedInstance for the provider
    CIMInstance _getProviderInstance();

    // Print out registered modules and status
    void _printList(Array<String> & moduleNames, Array<CIMInstance> & instances,
		    ostream& outPrintWriter, ostream& errPrintWriter);

    //
    // The CIM Client reference
    //
    CIMClient*    _client;

    //
    // The host name. 
    //
    String        _hostName;

    //
    // The name of the provider module.
    //
    String        _moduleName;

    //
    // The name of the provider.
    //
    String        _providerName;

    //
    // The type of operation specified on the command line. 
    //
    Uint32        _operationType;

    //
    // The flag to indicate whether the provider module is set or not
    //
    Boolean	_moduleSet;

    //
    // The flag to indicate whether the provider is set or not
    //
    Boolean	_providerSet;

    //
    // The flag to indicate whether the status is set or not
    //
    Boolean	_statusSet;

#ifdef PEGASUS_OS_OS400
    //
    // The flag to indicate whether standard output and standard error are suppressed
    //
    Boolean	_defaultQuietSet;
#endif
};

/**
    Constructs a CIMProviderCommand and initializes instance variables.
*/
CIMProviderCommand::CIMProviderCommand ()
{
    /**
        Initialize the instance variables.
    */
    _operationType      = OPERATION_TYPE_UNINITIALIZED;
    _hostName           = String::EMPTY;
    _moduleName		= String::EMPTY;
    _providerName	= String::EMPTY;
    _moduleSet 		= false;
    _providerSet	= false;
    _statusSet		= false;
#ifdef PEGASUS_OS_OS400
     _defaultQuietSet    = false;
#endif

    /**
        Build the usage string for the config command.  
    */
    String usage;
    usage.reserveCapacity(200);
    //l10n
    //localize usage keyword:
    //MessageLoaderParms parms("Clients.CLI.USAGE_STRING","usage: ");
    //String USAGE_L = MessageLoader::getMessage(parms);
    //usage.append(USAGE_L);
    usage.append(USAGE);
    usage.append(COMMAND_NAME);

#ifdef PEGASUS_OS_OS400
    usage.append(" -").append(OPTION_DISABLE);
    usage.append(" -").append(OPTION_MODULE).append(" module ");
    usage.append("[ -").append(OPTION_QUIET_VALUE).append(" ]\n");

    usage.append("                   -").append(OPTION_ENABLE);
    usage.append(" -").append(OPTION_MODULE).append(" module ");
    usage.append("[ -").append(OPTION_QUIET_VALUE).append(" ]\n");

    usage.append("                   -").append(OPTION_REMOVE);
    usage.append(" -").append(OPTION_MODULE).append(" module");
    usage.append(" [ -").append(OPTION_PROVIDER).append(" provider ] ");
    usage.append("[ -").append(OPTION_QUIET_VALUE).append(" ]\n");

    usage.append("                   -").append(OPTION_LIST);
    usage.append(" [ -").append(OPTION_STATUS);
    usage.append(" | -").append(OPTION_MODULE).append(" module ] \n");
#else
    usage.append(" -").append(OPTION_DISABLE);
    usage.append(" -").append(OPTION_MODULE).append(" module \n");

    usage.append("                   -").append(OPTION_ENABLE);
    usage.append(" -").append(OPTION_MODULE).append(" module \n");

    usage.append("                   -").append(OPTION_REMOVE);
    usage.append(" -").append(OPTION_MODULE).append(" module");
    usage.append(" [ -").append(OPTION_PROVIDER).append(" provider ] \n");

    usage.append("                   -").append(OPTION_LIST);
    usage.append(" [ -").append(OPTION_STATUS);
    usage.append(" | -").append(OPTION_MODULE).append(" module ] \n");
#endif

    setUsage (usage);
}


/**
    Parses the command line, validates the options, and sets instance 
    variables based on the option arguments.
*/
void CIMProviderCommand::setCommand (
                        ostream& outPrintWriter, 
                        ostream& errPrintWriter,
			Uint32   argc, 
			char*    argv []) 
{
    Uint32            i                = 0;
    Uint32            c                = 0;
    String            badOptionString  = String ();
    String            optString        = String ();

    //
    //  Construct optString
    //
#ifdef PEGASUS_OS_OS400
    optString.append(OPTION_DISABLE);
    optString.append(OPTION_MODULE);
    optString.append(getoopt::GETOPT_ARGUMENT_DESIGNATOR);
    optString.append(OPTION_QUIET_VALUE);
    optString.append(OPTION_ENABLE);
    optString.append(OPTION_MODULE);
    optString.append(getoopt::GETOPT_ARGUMENT_DESIGNATOR);
    optString.append(OPTION_QUIET_VALUE);
    optString.append(OPTION_REMOVE);
    optString.append(OPTION_MODULE);
    optString.append(getoopt::GETOPT_ARGUMENT_DESIGNATOR);
    optString.append(OPTION_PROVIDER);
    optString.append(getoopt::GETOPT_ARGUMENT_DESIGNATOR);
    optString.append(OPTION_QUIET_VALUE);
    optString.append(OPTION_LIST);
    optString.append(OPTION_STATUS);
    optString.append(OPTION_MODULE);
    optString.append(getoopt::GETOPT_ARGUMENT_DESIGNATOR);
#else
    optString.append(OPTION_DISABLE);
    optString.append(OPTION_MODULE);
    optString.append(getoopt::GETOPT_ARGUMENT_DESIGNATOR);
    optString.append(OPTION_ENABLE);
    optString.append(OPTION_MODULE);
    optString.append(getoopt::GETOPT_ARGUMENT_DESIGNATOR);
    optString.append(OPTION_REMOVE);
    optString.append(OPTION_MODULE);
    optString.append(getoopt::GETOPT_ARGUMENT_DESIGNATOR);
    optString.append(OPTION_PROVIDER);
    optString.append(getoopt::GETOPT_ARGUMENT_DESIGNATOR);
    optString.append(OPTION_LIST);
    optString.append(OPTION_STATUS);
    optString.append(OPTION_MODULE);
    optString.append(getoopt::GETOPT_ARGUMENT_DESIGNATOR);
#endif

    //
    //  Initialize and parse options
    //
    getoopt options ("");
    options.addFlagspec(optString);

    options.parse (argc, argv);

    if (options.hasErrors ())
    {
        CommandFormatException e (options.getErrorStrings () [0]);
        throw e;
    }
    _operationType = OPERATION_TYPE_UNINITIALIZED;


    //
    //  Get options and arguments from the command line
    //
    for (i =  options.first (); i <  options.last (); i++)
    {
        if (options [i].getType () == Optarg::LONGFLAG)
        {
            //
            //  This path should not be hit
            //  The cimprovider command has no LONGFLAG options
            //
            c = options [i].getopt () [0];

            UnexpectedOptionException e (c);
            throw e;
        } 
        else if (options [i].getType () == Optarg::REGULAR)
        {
            //
            //  The cimprovider command has no non-option argument options
            //
            UnexpectedArgumentException e (options [i].Value ()); 
            throw e;
        } 
        else /* if (options [i].getType () == Optarg::FLAG) */
        {

            c = options [i].getopt () [0];

            switch (c) 
            {
                case OPTION_DISABLE: 
                {
                    if (_operationType != OPERATION_TYPE_UNINITIALIZED)
                    {
                        //
                        // More than one operation option was found
                        //
                        UnexpectedOptionException e (OPTION_DISABLE);
                        throw e;
                    }

                    if (options.isSet (OPTION_DISABLE) > 1)
                    {
                        //
                        // More than one disable provider option was found
                        //
                        DuplicateOptionException e (OPTION_DISABLE); 
                        throw e;
                    }

                    _operationType = OPERATION_TYPE_DISABLE;

                    break;
                }
                case OPTION_REMOVE:
                {
                    if (_operationType != OPERATION_TYPE_UNINITIALIZED)
                    {
                        //
                        // More than one operation option was found
                        //
                        UnexpectedOptionException e (OPTION_REMOVE);
                        throw e;
                    }

                    if (options.isSet (OPTION_REMOVE) > 1)
                    {
                        //
                        // More than one remove provider option was found
                        //
                        DuplicateOptionException e (OPTION_REMOVE);
                        throw e;
                    }

                    _operationType = OPERATION_TYPE_REMOVE;

                    break;
                }
                case OPTION_ENABLE:
                {
                    if (_operationType != OPERATION_TYPE_UNINITIALIZED)
                    {
                        //
                        // More than one operation option was found
                        //
                        UnexpectedOptionException e (OPTION_ENABLE);
                        throw e;
                    }

                    if (options.isSet (OPTION_ENABLE) > 1)
                    {
                        //
                        // More than one stop option was found
                        //
                        DuplicateOptionException e (OPTION_ENABLE);
                        throw e;
                    }

                    _operationType = OPERATION_TYPE_ENABLE;

                    break;
                }

                case OPTION_LIST: 
                {
                    if (_operationType != OPERATION_TYPE_UNINITIALIZED)
                    {
                        //
                        // More than one operation option was found
                        //
                        UnexpectedOptionException e (OPTION_LIST);
                        throw e;
                    }

                    if (options.isSet (OPTION_LIST) > 1)
                    {
                        //
                        // More than one list option was found
                        //
                        DuplicateOptionException e (OPTION_LIST); 
                        throw e;
                    }
                    _operationType = OPERATION_TYPE_LIST;
                    break;
                }

                case OPTION_MODULE:
                {
                    if (options.isSet (OPTION_MODULE) > 1)
                    {
                        //
                        // More than one module option was found
                        //
                        DuplicateOptionException e (OPTION_MODULE);
                        throw e;
                    }

                    _moduleName = options [i].Value ();
		    _moduleSet = true;

                    break;
                }

                case OPTION_PROVIDER:
                {
                    if (options.isSet (OPTION_PROVIDER) > 1)
                    {
                        //
                        // More than one provider option was found
                        //
                        DuplicateOptionException e (OPTION_PROVIDER);
                        throw e;
                    }

                    _providerName = options [i].Value ();
		    _providerSet = true;

		    break;
		}

                case OPTION_STATUS:
                {
                    if (options.isSet (OPTION_STATUS) > 1)
                    {
                        //
                        // More than one status option was found
                        //
                        DuplicateOptionException e (OPTION_STATUS);
                        throw e;
                    }

		    _statusSet = true;

		    break;
		}

#ifdef PEGASUS_OS_OS400
 		case OPTION_QUIET_VALUE:
 	        {
 			_defaultQuietSet = true;
 			break;
 	        }
 #endif

                default:
		{ 
		    // 
		    // Should never get here
		    //
		    break;
                }
            }
        }
    }

    // 
    // Some more validations
    //
    if ( _operationType == OPERATION_TYPE_UNINITIALIZED )
    {
        //
        // No operation type was specified 
        // Show the usage 
        //
        CommandFormatException e ( REQUIRED_ARGS_MISSING );
        throw e;
    }
	
    if ( _operationType == OPERATION_TYPE_DISABLE )
    {
	if ( _providerSet )
	{
		//l10n						
	    //CommandFormatException e("Unexpected Option.");
	    
	    CommandFormatException e(localizeMessage(MSG_PATH,
	    										 UNEXPECTED_OPTION_KEY,
	    										 UNEXPECTED_OPTION));
	    throw e;
	}

	if ( !_moduleSet )
	{
	    MissingOptionException e (OPTION_MODULE);
	    throw e;
	}
    }

    if ( _operationType == OPERATION_TYPE_ENABLE )
    {
	if ( _providerSet )
	{
		//l10n
	    //CommandFormatException e("Unexpected Option.");
	    CommandFormatException e(localizeMessage(MSG_PATH,
	    										 UNEXPECTED_OPTION_KEY,
	    										 UNEXPECTED_OPTION));
	    throw e;
	}

	if ( !_moduleSet )
	{
	    MissingOptionException e (OPTION_MODULE);
	    throw e;
	}
    }

    if ( _operationType == OPERATION_TYPE_REMOVE && !_moduleSet )
    {
	MissingOptionException e (OPTION_MODULE);
	throw e;
    }

    if ( _operationType == OPERATION_TYPE_LIST && _providerSet )
    {
    	//l10n
	//CommandFormatException e("Unexpected Option.");
	CommandFormatException e(localizeMessage(MSG_PATH,
	    										 UNEXPECTED_OPTION_KEY,
	    										 UNEXPECTED_OPTION));
	throw e;
    }

    if ( _operationType == OPERATION_TYPE_LIST && _statusSet && _moduleSet)
    {
    	//l10n
	//CommandFormatException e("Unexpected Option.");
	CommandFormatException e(localizeMessage(MSG_PATH,
	    										 UNEXPECTED_OPTION_KEY,
	    										 UNEXPECTED_OPTION));
	throw e;
    }

#ifdef PEGASUS_OS_OS400
     if ( _operationType == OPERATION_TYPE_LIST && _defaultQuietSet )
     {
 	//
        // An invalid option was encountered
        //
        InvalidOptionException e (OPTION_QUIET_VALUE);
        throw e;
    }
#endif

}

/**
    Executes the command and writes the results to the PrintWriters.
*/
Uint32 CIMProviderCommand::execute (
    ostream& outPrintWriter, 
    ostream& errPrintWriter)
{
    if ( _operationType == OPERATION_TYPE_UNINITIALIZED )
    {
        //
        // The command was not initialized
        //
        return 1;
    }

#ifdef PEGASUS_OS_OS400
    // disable standard out and standard error
    if( _defaultQuietSet && (_operationType != OPERATION_TYPE_LIST) ){
	freopen("/dev/null","w",stdout);
        freopen("/dev/null","w",stderr);
    }
#endif

    // 
    // Get local host name
    //
    _hostName.assign(System::getHostName());

    try
    {
        //
        // Open connection with CIMSever
        //
        _client = new CIMClient;

        _client->connectLocal();

    }
    catch(Exception& e)
    {
    	//l10n
        //outPrintWriter << CIMOM_NOT_RUNNING << endl;
        outPrintWriter << localizeMessage(MSG_PATH,
        								  CIMOM_NOT_RUNNING_KEY,
        								  CIMOM_NOT_RUNNING) << endl;
	return 1;
    }


    //
    // Perform the requested operation
    //
    switch ( _operationType )
    {
        case OPERATION_TYPE_REMOVE:
            try
            {
	        _deleteProvider( outPrintWriter, errPrintWriter );
            }
            catch (CIMException& e)
            {
                CIMStatusCode code = e.getCode();

                if (code == CIM_ERR_FAILED)
                {
                	//l10n
		    //outPrintWriter << DELETE_PROVIDER_FAILURE << endl;
		    outPrintWriter << localizeMessage(MSG_PATH,
        								  DELETE_PROVIDER_FAILURE_KEY,
        								  DELETE_PROVIDER_FAILURE) << endl;
                    errPrintWriter << e.getMessage() << endl;
                }
                else if (code == CIM_ERR_INVALID_CLASS)
                {
                	//l10n
		    //outPrintWriter << DELETE_PROVIDER_FAILURE << endl;
		    //outPrintWriter << PG_PROVIDER_SCHEMA_NOT_LOADED << endl;
		    outPrintWriter << localizeMessage(MSG_PATH,
        								  DELETE_PROVIDER_FAILURE_KEY,
        								  DELETE_PROVIDER_FAILURE) << endl;
        	outPrintWriter << localizeMessage(MSG_PATH,
        								  PG_PROVIDER_SCHEMA_NOT_LOADED_KEY,
        								  PG_PROVIDER_SCHEMA_NOT_LOADED) << endl;							  
                }
                else
                {
		    errPrintWriter << e.getMessage() << endl;
                }
                return ( RC_ERROR );
            }
            catch (Exception& e)
            {
                errPrintWriter << e.getMessage() << endl;
                return ( RC_ERROR );
            }

            break; 

            case OPERATION_TYPE_ENABLE:
            try
            {
                _StartProvider( outPrintWriter, errPrintWriter );
            }
            catch (CIMException& e)
            {
                CIMStatusCode code = e.getCode();
                if (code == CIM_ERR_FAILED)
                {
                	//l10n
                    //outPrintWriter << START_PROVIDER_FAILURE << endl;
                    outPrintWriter << localizeMessage(MSG_PATH,
        								  START_PROVIDER_FAILURE_KEY,
        								  START_PROVIDER_FAILURE) << endl;
                    errPrintWriter << e.getMessage() << endl;
                }
                else if (code == CIM_ERR_INVALID_CLASS)
                {
                	//l10n
                    //outPrintWriter << START_PROVIDER_FAILURE << endl;
                    //outPrintWriter << PG_PROVIDER_SCHEMA_NOT_LOADED << endl;
                    outPrintWriter << localizeMessage(MSG_PATH,
        								  START_PROVIDER_FAILURE_KEY,
        								  START_PROVIDER_FAILURE) << endl;
        			outPrintWriter << localizeMessage(MSG_PATH,
        								  PG_PROVIDER_SCHEMA_NOT_LOADED_KEY,
        								  PG_PROVIDER_SCHEMA_NOT_LOADED) << endl;					  
                }
                else
                {
                    errPrintWriter << e.getMessage() << endl;
                }
                return ( RC_ERROR );
            }
            catch (Exception& e)
            {
                errPrintWriter << e.getMessage() << endl;
                return ( RC_ERROR );
            }

            break;

        case OPERATION_TYPE_DISABLE:
            try
            {
                _StopProvider( outPrintWriter, errPrintWriter );
            }
            catch (CIMException& e)
            {
                CIMStatusCode code = e.getCode();
                if (code == CIM_ERR_FAILED)
                {
                	//l10n
                    //outPrintWriter << STOP_PROVIDER_FAILURE << endl;
                    outPrintWriter << localizeMessage(MSG_PATH,
        								  STOP_PROVIDER_FAILURE_KEY,
        								  STOP_PROVIDER_FAILURE) << endl;
                    errPrintWriter << e.getMessage() << endl;
                }
                else if (code == CIM_ERR_INVALID_CLASS)
                {
                	//l10n
                    //outPrintWriter << STOP_PROVIDER_FAILURE << endl;
                    //outPrintWriter << PG_PROVIDER_SCHEMA_NOT_LOADED << endl;
                    outPrintWriter << localizeMessage(MSG_PATH,
        								  STOP_PROVIDER_FAILURE_KEY,
        								  STOP_PROVIDER_FAILURE) << endl;
        			outPrintWriter << localizeMessage(MSG_PATH,
        								  PG_PROVIDER_SCHEMA_NOT_LOADED,
        								  PG_PROVIDER_SCHEMA_NOT_LOADED) << endl;					  
                }
                else
                {
                    errPrintWriter << e.getMessage() << endl;
                }
                return ( RC_ERROR );
            }
            catch (Exception& e)
            {
                errPrintWriter << e.getMessage() << endl;
                return ( RC_ERROR );
            }
            break;

        case OPERATION_TYPE_LIST:
            try
            {
                 _ListProviders(outPrintWriter, errPrintWriter);

                break;
            }
            catch (CIMException& e)
            {
                CIMStatusCode code = e.getCode();
                if (code == CIM_ERR_FAILED)
                {
                	//l10n
                    //outPrintWriter << LIST_PROVIDERS_FAILURE << endl;
                    outPrintWriter << localizeMessage(MSG_PATH,
        								  LIST_PROVIDERS_FAILURE_KEY,
        								  LIST_PROVIDERS_FAILURE) << endl;
                    errPrintWriter << e.getMessage() << endl;
                }
                else if (code == CIM_ERR_INVALID_CLASS)
                {
                	//l10n
                    //outPrintWriter << LIST_PROVIDERS_FAILURE << endl;
                    //outPrintWriter << PG_PROVIDER_SCHEMA_NOT_LOADED << endl;
                    outPrintWriter << localizeMessage(MSG_PATH,
        								  LIST_PROVIDERS_FAILURE_KEY,
        								  LIST_PROVIDERS_FAILURE) << endl;
        			outPrintWriter << localizeMessage(MSG_PATH,
        								  PG_PROVIDER_SCHEMA_NOT_LOADED_KEY,
        								  PG_PROVIDER_SCHEMA_NOT_LOADED) << endl;					  
        								  
                }
                else
                {
                    errPrintWriter << e.getMessage() << endl;
                }
                return ( RC_ERROR );
            }
            catch (Exception& e)
            {
                errPrintWriter << e.getMessage() << endl;
                return ( RC_ERROR );
            }

        default:
            //
            // Should never get here
            //
            break;
    }

    return (RC_SUCCESS);
}

/**
    Delete the provider module or providers in a module
*/
void CIMProviderCommand::_deleteProvider
    (
    ostream&    outPrintWriter, 
    ostream&    errPrintWriter
    ) 
{
    
    try
    {
	if ( _moduleSet )
	{
	  if ( _providerSet )
	  {
	    // Delete provider which have specified module name and provider name

	    CIMInstance providerInstance = _getProviderInstance();

	    CIMObjectPath providerRef = providerInstance.getPath ();
	    providerRef.setNameSpace(PEGASUS_NAMESPACENAME_PROVIDERREG);
	    providerRef.setClassName(PEGASUS_CLASSNAME_PROVIDER);

	    CIMKeyBinding kb1(_PROPERTY_PROVIDERMODULENAME, _moduleName, CIMKeyBinding::STRING);
	    CIMKeyBinding kb2(_PROPERTY_PROVIDER_NAME, _providerName, CIMKeyBinding::STRING);
	    Array<CIMKeyBinding> keys;
	    keys.append(kb1);
	    keys.append(kb2);
	    providerRef.setKeyBindings(keys);
	
	//l10n
	    //outPrintWriter << DELETEING_PROVIDER << endl;
	    outPrintWriter << localizeMessage(MSG_PATH,
        								  DELETEING_PROVIDER_KEY,
        								  DELETEING_PROVIDER) << endl;	

	    _client->deleteInstance(
		PEGASUS_NAMESPACENAME_PROVIDERREG,
		providerRef);

//l10n
	    //outPrintWriter << DELETE_PROVIDER_SUCCESS << endl;
	    outPrintWriter << localizeMessage(MSG_PATH,
        								  DELETE_PROVIDER_SUCCESS_KEY,
        								  DELETE_PROVIDER_SUCCESS) << endl;
	  }
	  else
	  {
	    // Delete all the registered provider modules

	    CIMInstance moduleInstance = _getModuleInstance();

	    CIMObjectPath moduleRef = moduleInstance.getPath ();
	    moduleRef.setNameSpace(PEGASUS_NAMESPACENAME_PROVIDERREG);
	    moduleRef.setClassName(PEGASUS_CLASSNAME_PROVIDERMODULE);

	    CIMKeyBinding kb1(CIMName ("Name"), _moduleName, 
                CIMKeyBinding::STRING);
	    Array<CIMKeyBinding> keys;
	    keys.append(kb1);

	    moduleRef.setKeyBindings(keys);
//l10n
	    //outPrintWriter << DELETEING_PROVIDER_MODULE << endl;
		outPrintWriter << localizeMessage(MSG_PATH,
        								  DELETEING_PROVIDER_MODULE_KEY,
        								  DELETEING_PROVIDER_MODULE) << endl;


	    _client->deleteInstance(
		PEGASUS_NAMESPACENAME_PROVIDERREG,
		moduleRef);
//l10n
	    //outPrintWriter << DELETE_PROVIDER_MODULE_SUCCESS << endl;
	    outPrintWriter << localizeMessage(MSG_PATH,
        								  DELETE_PROVIDER_MODULE_SUCCESS_KEY,
        								  DELETE_PROVIDER_MODULE_SUCCESS) << endl;
	  }

        }

    }

    catch (Exception& e)
    {
        throw e;
    }
}

//
// Enable the provider module
//
void CIMProviderCommand::_StartProvider
    (
    ostream&    outPrintWriter, 
    ostream&    errPrintWriter
    ) 
{
    
    try
    {
	CIMObjectPath ref;
	//
	// enable module
	//
	if ( _moduleSet )
	{
 	    // get the module instance
	    CIMInstance moduleInstance = _getModuleInstance();

	    CIMObjectPath moduleRef = moduleInstance.getPath ();
	    moduleRef.setNameSpace(PEGASUS_NAMESPACENAME_PROVIDERREG);
	    moduleRef.setClassName(PEGASUS_CLASSNAME_PROVIDERMODULE);

	    CIMKeyBinding kb1(CIMName ("Name"), _moduleName, 
                CIMKeyBinding::STRING);
	    Array<CIMKeyBinding> keys;
	    keys.append(kb1);

	    moduleRef.setKeyBindings(keys);
	    
	    ref = moduleRef;
//l10n
	    //outPrintWriter << STARTING_PROVIDER_MODULE << endl;
	    outPrintWriter << localizeMessage(MSG_PATH,
        								  STARTING_PROVIDER_MODULE_KEY,
        								  STARTING_PROVIDER_MODULE) << endl;
	}

	Array<CIMParamValue> inParams;
	Array<CIMParamValue> outParams;

	CIMValue ret_value = _client->invokeMethod(
		PEGASUS_NAMESPACENAME_PROVIDERREG,
		ref,
		START_METHOD,
		inParams,
		outParams);

	Sint16 retValue;
	ret_value.get(retValue);
	if (retValue == 1)
	{
		//l10n
   	    //outPrintWriter << PROVIDER_ALREADY_STARTED << endl;
   	    outPrintWriter << localizeMessage(MSG_PATH,
        								  PROVIDER_ALREADY_STARTED_KEY,
        								  PROVIDER_ALREADY_STARTED) << endl;
	}
	else if (retValue == 2)
	{
		//l10n
   	    //outPrintWriter << CANNOT_START_PROVIDER << endl;
   	    outPrintWriter << localizeMessage(MSG_PATH,
        								  CANNOT_START_PROVIDER_KEY,
        								  CANNOT_START_PROVIDER) << endl;
	}
	else if (retValue == 0)
	{
		//l10n
   	    //outPrintWriter << START_PROVIDER_SUCCESS << endl;
   	    outPrintWriter << localizeMessage(MSG_PATH,
        								  START_PROVIDER_SUCCESS_KEY,
        								  START_PROVIDER_SUCCESS) << endl;
	}
	else
	{
		//l10n
	    //outPrintWriter << START_PROVIDER_FAILURE << endl;
	    outPrintWriter << localizeMessage(MSG_PATH,
        								  START_PROVIDER_FAILURE_KEY,
        								  START_PROVIDER_FAILURE) << endl;
	}
    }

    catch (Exception& e)
    {
        throw e;
    }

}

//
// Disable the provider module 
//
void CIMProviderCommand::_StopProvider
    (
    ostream&    outPrintWriter, 
    ostream&    errPrintWriter
    ) 
{
    try
    {
	CIMObjectPath ref;
	//
	// disable the module
	//
	if ( _moduleSet )
	{
 	    // get the module instance
	    CIMInstance moduleInstance = _getModuleInstance();

	    CIMObjectPath moduleRef = moduleInstance.getPath ();
	    moduleRef.setNameSpace(PEGASUS_NAMESPACENAME_PROVIDERREG);
	    moduleRef.setClassName(PEGASUS_CLASSNAME_PROVIDERMODULE);

	    CIMKeyBinding kb1(CIMName ("Name"), _moduleName, 
                CIMKeyBinding::STRING);
	    Array<CIMKeyBinding> keys;
	    keys.append(kb1);

	    moduleRef.setKeyBindings(keys);
	    
	    ref = moduleRef;
//l10n
	    //outPrintWriter << STOPING_PROVIDER_MODULE << endl;
	    outPrintWriter << localizeMessage(MSG_PATH,
        								  STOPING_PROVIDER_MODULE_KEY,
        								  STOPING_PROVIDER_MODULE) << endl;
	}
	Array<CIMParamValue> inParams;
	Array<CIMParamValue> outParams;

	CIMValue ret_value = _client->invokeMethod(
		PEGASUS_NAMESPACENAME_PROVIDERREG,
		ref,
		STOP_METHOD,
		inParams,
		outParams);

	Sint16 retValue;
	ret_value.get(retValue);
	if (retValue == 1)
	{
		//l10n
   	    //outPrintWriter << PROVIDER_ALREADY_STOPPED << endl;
   	    outPrintWriter << localizeMessage(MSG_PATH,
        								  PROVIDER_ALREADY_STOPPED_KEY,
        								  PROVIDER_ALREADY_STOPPED) << endl;
	}
	else if (retValue == 0)
	{
		//l10n
   	    //outPrintWriter << STOP_PROVIDER_SUCCESS << endl;
   	    outPrintWriter << localizeMessage(MSG_PATH,
        								  PROVIDER_ALREADY_STOPPED_KEY,
        								  PROVIDER_ALREADY_STOPPED) << endl;
	}
	else
	{
		//l10n
	    //outPrintWriter << STOP_PROVIDER_FAILURE << endl;
	    outPrintWriter << localizeMessage(MSG_PATH,
        								  PROVIDER_ALREADY_STOPPED_KEY,
        								  PROVIDER_ALREADY_STOPPED) << endl;
	}
    }

    catch (Exception& e)
    {
        throw e;
    }

}


/**
    get a list of all registered provider modules or their status or providers.
 */
void CIMProviderCommand::_ListProviders
    ( 
    ostream&    outPrintWriter,
    ostream&    errPrintWriter
    )
{

    try
    {
    	Array<CIMInstance> moduleInstances;
    	Array<CIMObjectPath> instanceNames;
    	String moduleName;
    	String providerName;
	Array<String> moduleNames;
	Array<CIMInstance> instances;
	Boolean moduleExist = false;

	if ( _moduleSet )
	{
	    // get registered providers which are in the specified module
	    instanceNames = _client->enumerateInstanceNames(
				PEGASUS_NAMESPACENAME_PROVIDERREG, 
				PEGASUS_CLASSNAME_PROVIDER);

	    if ( instanceNames.size() == 0 )
	    {
	    	//l10n
 	  	//cerr << ERR_PROVIDER_NOT_REGISTERED << endl;
 	  	cerr << localizeMessage(MSG_PATH,
        								  ERR_PROVIDER_NOT_REGISTERED_KEY,
        								  ERR_PROVIDER_NOT_REGISTERED) << endl;
		exit(-1);	
	    }
	    else
	    {
	        // List all the registered providers which have specified module
	        for (Uint32 i = 0; i < instanceNames.size(); i++)
	        {
		    Array<CIMKeyBinding> keys = instanceNames[i].getKeyBindings();
		    for(Uint32 j=0; j < keys.size(); j++)
            	    {
                	if(keys[j].getName().equal (_PROPERTY_PROVIDERMODULENAME))
                	{
                    	    moduleName = keys[j].getValue();
                	}
                	if(keys[j].getName().equal (_PROPERTY_PROVIDER_NAME))
                	{
                    	    providerName = keys[j].getValue();
                	}
             	    }
		    if (String::equalNoCase(moduleName, _moduleName))
		    {
			moduleExist = true;
		    	outPrintWriter << providerName << endl;;
		    }
	        }  
		
		if (!moduleExist)
		{
			//l10n
		    //cerr << ERR_PROVIDER_NOT_REGISTERED << endl;
		    cerr << localizeMessage(MSG_PATH,
        								  ERR_PROVIDER_NOT_REGISTERED_KEY,
        								  ERR_PROVIDER_NOT_REGISTERED) << endl;
		    exit(-1);
		}
	    }
	}
	else
	{
	    // Get all registered provider modules
    	    moduleInstances = _client->enumerateInstances(
				PEGASUS_NAMESPACENAME_PROVIDERREG, 
				PEGASUS_CLASSNAME_PROVIDERMODULE);
	    if ( moduleInstances.size() == 0 )
	    {
	    	//l10n
 	  	//cerr << ERR_MODULE_NOT_REGISTERED << endl;
 	  	cerr << localizeMessage(MSG_PATH,
        								  ERR_MODULE_NOT_REGISTERED_KEY,
        								  ERR_MODULE_NOT_REGISTERED) << endl;
		exit(-1);	
	    }
	    else
	    {
	        // List all the registered provider modules 
	        for (Uint32 i = 0; i < moduleInstances.size(); i++)
	        {
		    CIMInstance& instance = moduleInstances[i];
		    instance.getProperty(
			instance.findProperty(_PROPERTY_PROVIDERMODULE_NAME)).getValue().get(moduleName);
		    moduleNames.append(moduleName);
		    instances.append(instance);
	        }  

		_printList(moduleNames, instances, outPrintWriter, errPrintWriter);
	    }
	}
    }
    catch (Exception& e)
    {
        throw e;
    }
}

// Get namedInstance for a provider module
CIMInstance CIMProviderCommand::_getModuleInstance()
{

    CIMKeyBinding kb(_PROPERTY_PROVIDERMODULE_NAME,
                     _moduleName, CIMKeyBinding::STRING);

    Array<CIMKeyBinding> kbArray;
    kbArray.append(kb);

    CIMObjectPath moduleRef("", PEGASUS_NAMESPACENAME_PROVIDERREG,
                            PEGASUS_CLASSNAME_PROVIDERMODULE,
                            kbArray);

    try
    {
	    CIMInstance instance = _client->getInstance(
		PEGASUS_NAMESPACENAME_PROVIDERREG, moduleRef);
	    return (instance);
    }
    catch (CIMException& exception)
    {
        // Provider module was not registered yet
        //l10n
 	//cerr << ERR_MODULE_NOT_REGISTERED << endl;
 	cerr << localizeMessage(MSG_PATH,
        								  ERR_MODULE_NOT_REGISTERED_KEY,
        								  ERR_MODULE_NOT_REGISTERED) << endl;
	exit(-1);	
    }
    catch (Exception& exception)
    {
        // Provider module was not registered yet
        //l10n
 	//cerr << ERR_MODULE_NOT_REGISTERED << endl;
 	cerr << localizeMessage(MSG_PATH,
        								  ERR_MODULE_NOT_REGISTERED_KEY,
        								  ERR_MODULE_NOT_REGISTERED) << endl;
	exit(-1);	
    }
    
    // Keep the compiler happy
    return CIMInstance();
}

// Get namedInstance for a provider
CIMInstance CIMProviderCommand::_getProviderInstance()
{

    CIMKeyBinding kb(_PROPERTY_PROVIDERMODULENAME,
                     _moduleName, CIMKeyBinding::STRING);

    CIMKeyBinding kb2(_PROPERTY_PROVIDER_NAME,
                     _providerName, CIMKeyBinding::STRING);

    Array<CIMKeyBinding> kbArray;
    kbArray.append(kb);
    kbArray.append(kb2);

    CIMObjectPath providerRef("", PEGASUS_NAMESPACENAME_PROVIDERREG,
                            PEGASUS_CLASSNAME_PROVIDER,
                            kbArray);

    try
    {
	    CIMInstance instance = _client->getInstance(
		PEGASUS_NAMESPACENAME_PROVIDERREG, providerRef);
	    return (instance);
    }
    catch (CIMException& exception)
    {
        // Provider was not registered yet
        //l10n
        //cerr << ERR_PROVIDER_NOT_REGISTERED << endl;
        cerr << localizeMessage(MSG_PATH,
        								  ERR_PROVIDER_NOT_REGISTERED_KEY,
        								  ERR_PROVIDER_NOT_REGISTERED) << endl;
	exit(-1);	
    }
    catch (Exception& exception)
    {
        // Provider was not registered yet
        //l10n
        //cerr << ERR_PROVIDER_NOT_REGISTERED << endl;
        cerr << localizeMessage(MSG_PATH,
        								  ERR_PROVIDER_NOT_REGISTERED_KEY,
        								  ERR_PROVIDER_NOT_REGISTERED) << endl;
	exit(-1);	
    }
    
    // Keep the compiler happy
    return CIMInstance();
}

// Print out registered modules and status
void CIMProviderCommand::_printList(Array<String> & moduleNames, 
				    Array<CIMInstance> & instances,
				    ostream& outPrintWriter, 
				    ostream& errPrintWriter)
{
    Uint32 maxLength=0;
    Uint32 length=0;
    Array<Uint16> _status;
    String output;
    String statusValue;

    if (_statusSet)
    {
	// get max length of module name
	for (Uint32 i=0; i < moduleNames.size(); i++)
	{
	    if (maxLength < moduleNames[i].size())
	    {
		maxLength = moduleNames[i].size();
	    }
	}
	
	output = "MODULE";

	for (Uint32 i = 0; i < maxLength; i++)
	{
	    output.append(" ");
	}

	output.append("STATUS");
	outPrintWriter << output << endl;

	for (Uint32 i =0; i < instances.size(); i++)
	{
	    output = moduleNames[i]; 
	    length = maxLength +6 - moduleNames[i].size();

	    for (Uint32 j = 0; j < length; j++)
            {
            	output.append(" ");
            }

	    Uint32 pos = instances[i].findProperty(_PROPERTY_OPERATIONALSTATUS);
	    if (pos == PEG_NOT_FOUND)
	    {
		_status.append(0);		
	    }
	    else
	    {
		if( instances[i].getProperty(pos).getValue().isNull() ){
		    if( _status.size() == 0 )
				_status.append(0);
		}
		else
		    instances[i].getProperty(pos).getValue().get(_status);
	    }

	    for (Uint32 j=0; j < _status.size(); j++)
	    {
		switch ( _status[j])
		{
		    case 0:
		    {
			statusValue = "Unknown";
			break;
		    }

		    case 1:
		    {
			statusValue = "Other";
			break;
		    }

		    case 2:
		    {
			statusValue = "OK";
			break;
		    }

		    case 3:
		    {
			statusValue = "Degraded";
			break;
		    }

		    case 4:
		    {
			statusValue = "Stressed";
			break;
		    }

		    case 5:
		    {
			statusValue = "Predictive Failure";
			break;
		    }

		    case 6:
		    {
			statusValue = "Error";
			break;
		    }

		    case 7:
		    {
			statusValue = "Non-Recoverable Error";
			break;
		    }

		    case 8:
		    {
			statusValue = "Starting";
			break;
		    }

		    case 9:
		    {
			statusValue = "Stopping";
			break;
		    }

		    case 10:
		    {
			statusValue = "Stopped";
			break;
		    }

		    case 11:
		    {
			statusValue = "In Service";
			break;
		    }

		    case 12:
		    {
			statusValue = "No Contact";
			break;
		    }

		    case 13:
		    {
			statusValue = "Lost Communication";
			break;
		    }

		    default:
			statusValue = "Not Support";
			break;
		}
	        output.append(statusValue);
		output.append(" ");
	    }
	    outPrintWriter << output << endl;
	}
	
    }
    else
    {
	for (Uint32 i=0; i < moduleNames.size(); i++)
	{
	    outPrintWriter << moduleNames[i] << endl;
	}
    }
}

PEGASUS_NAMESPACE_END

//
// exclude main from the Pegasus Namespace
//
PEGASUS_USING_PEGASUS;

PEGASUS_USING_STD;

///////////////////////////////////////////////////////////////////////////////
/**
    Parses the command line, and execute the command.

    @param   args  the string array containing the command line arguments
*/
///////////////////////////////////////////////////////////////////////////////

int main (int argc, char* argv []) 
{
    CIMProviderCommand*      command;
    Uint32               retCode;
    

#ifdef PEGASUS_OS_OS400

  VFYPTRS_INCDCL;               // VFYPTRS local variables

  // verify pointers
  #pragma exception_handler (qsyvp_excp_hndlr,qsyvp_excp_comm_area,\
    0,_C2_MH_ESCAPE)
      for( int arg_index = 1; arg_index < argc; arg_index++ ){
	  VFYPTRS(VERIFY_SPP_NULL(argv[arg_index]));
      }
  #pragma disable_handler

    // check what environment we are running in, native or qsh
    if( getenv("SHLVL") == NULL ){  // native mode
	// Check to ensure the user is authorized to use the command,
	// suppress diagnostic message
	if(FALSE == ycmCheckCmdAuthorities(1)){
	  exit(CPFDF80_RC);
	}
    }
    else{ // qsh mode
	// Check to ensure the user is authorized to use the command
	// ycmCheckCmdAuthorities() will send a diagnostic message to qsh
	if(FALSE == ycmCheckCmdAuthorities()){
	  exit(CPFDF80_RC);
	}
    }

#endif

    command  = new CIMProviderCommand ();

    try 
    {
        command->setCommand ( cout, cerr, argc, argv);
    } 
    catch (CommandFormatException& cfe) 
    {
        if (!String::equal(cfe.getMessage (), ""))
        {
            cerr << COMMAND_NAME << ": " << cfe.getMessage () << endl;
        }
        cerr << command->getUsage () << endl;
        return 1;
    }

    retCode = command->execute (cout, cerr);

    exit(retCode);
    return (0);
}
