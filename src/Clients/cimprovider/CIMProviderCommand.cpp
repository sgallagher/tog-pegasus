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
#endif

PEGASUS_USING_STD;

PEGASUS_NAMESPACE_BEGIN

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
static const char CIMOM_NOT_RUNNING []         = 
                        "The CIM server may not be running.";

static const char DELETE_PROVIDER_FAILURE []    = 
                        "Failed to delete...";

static const char START_PROVIDER_FAILURE []    = 
                        "Failed to enable provider module.";

static const char STOP_PROVIDER_FAILURE []  = 
                        "Failed to disable provider module.";

static const char LIST_PROVIDERS_FAILURE [] = 
                        "Failed to list... ";

static const char DELETE_PROVIDER_MODULE_SUCCESS []    = 
                        "Provider module deleted successfully.";

static const char DELETE_PROVIDER_SUCCESS []    = 
                        "Provider deleted successfully.";

static const char START_PROVIDER_SUCCESS []  = 
                        "Provider module enabled successfully.";

static const char STOP_PROVIDER_SUCCESS []  = 
                        "Provider module disabled successfully.";

static const char PROVIDER_ALREADY_STOPPED []  = 
                        "Provider module already disabled.";

static const char PROVIDER_ALREADY_STARTED []  = 
                        "Provider module already enabled.";

static const char CANNOT_START_PROVIDER []  = 
                        "Provider module can not be enabled since it is disabling.";

static const char PROVIDER_NOT_REGISTERED[] =
		"Specified provider was not registered.";

static const char DELETEING_PROVIDER_MODULE[] =
			"Deleting provider module...";

static const char DELETEING_PROVIDER[] =
			"Deleting provider...";

static const char STARTING_PROVIDER_MODULE[] =
			"Enabling provider module...";

static const char STOPING_PROVIDER_MODULE[] =
			"Disabling provider module...";

static const char NO_MODULE_REGISTERED[] =
			"No modules registered for listing.";

static const char ERR_OPTION_NOT_SUPPORTED[] =
			"-p option was not supported.";

static const char ERR_MODULE_NOT_REGISTERED[] =
			"Specified provider module was not registered.";

static const char ERR_PROVIDER_NOT_REGISTERED[] =
			"Specified provider was not registered.";

static const char PG_PROVIDER_SCHEMA_NOT_LOADED []  =
    "Please make sure that the registration schema is loaded on the CIMOM.";

static const char REQUIRED_ARGS_MISSING []        =
			"Required arguments missing.";

static const char INVALID_ARGS []        =
                        "Invalid arguments.";

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

    /**
        Build the usage string for the config command.  
    */
    String usage;
    usage.reserveCapacity(200);
    usage.append(USAGE);
    usage.append(COMMAND_NAME);

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
	    CommandFormatException e("Unexpected Option.");
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
	    CommandFormatException e("Unexpected Option.");
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
	CommandFormatException e("Unexpected Option.");
	throw e;
    }

    if ( _operationType == OPERATION_TYPE_LIST && _statusSet && _moduleSet)
    {
	CommandFormatException e("Unexpected Option.");
	throw e;
    }
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
        outPrintWriter << CIMOM_NOT_RUNNING << endl;
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
		    outPrintWriter << DELETE_PROVIDER_FAILURE << endl;
                    errPrintWriter << e.getMessage() << endl;
                }
                else if (code == CIM_ERR_INVALID_CLASS)
                {
		    outPrintWriter << DELETE_PROVIDER_FAILURE << endl;
		    outPrintWriter << PG_PROVIDER_SCHEMA_NOT_LOADED << endl;
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
                    outPrintWriter << START_PROVIDER_FAILURE << endl;
                    errPrintWriter << e.getMessage() << endl;
                }
                else if (code == CIM_ERR_INVALID_CLASS)
                {
                    outPrintWriter << START_PROVIDER_FAILURE << endl;
                    outPrintWriter << PG_PROVIDER_SCHEMA_NOT_LOADED << endl;
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
                    outPrintWriter << STOP_PROVIDER_FAILURE << endl;
                    errPrintWriter << e.getMessage() << endl;
                }
                else if (code == CIM_ERR_INVALID_CLASS)
                {
                    outPrintWriter << STOP_PROVIDER_FAILURE << endl;
                    outPrintWriter << PG_PROVIDER_SCHEMA_NOT_LOADED << endl;
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
                    outPrintWriter << LIST_PROVIDERS_FAILURE << endl;
                    errPrintWriter << e.getMessage() << endl;
                }
                else if (code == CIM_ERR_INVALID_CLASS)
                {
                    outPrintWriter << LIST_PROVIDERS_FAILURE << endl;
                    outPrintWriter << PG_PROVIDER_SCHEMA_NOT_LOADED << endl;
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
	
	    outPrintWriter << DELETEING_PROVIDER << endl;

	    _client->deleteInstance(
		PEGASUS_NAMESPACENAME_PROVIDERREG,
		providerRef);

	    outPrintWriter << DELETE_PROVIDER_SUCCESS << endl;
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

	    outPrintWriter << DELETEING_PROVIDER_MODULE << endl;

	    _client->deleteInstance(
		PEGASUS_NAMESPACENAME_PROVIDERREG,
		moduleRef);

	    outPrintWriter << DELETE_PROVIDER_MODULE_SUCCESS << endl;
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

	    outPrintWriter << STARTING_PROVIDER_MODULE << endl;
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
   	    outPrintWriter << PROVIDER_ALREADY_STARTED << endl;
	}
	else if (retValue == 2)
	{
   	    outPrintWriter << CANNOT_START_PROVIDER << endl;
	}
	else if (retValue == 0)
	{
   	    outPrintWriter << START_PROVIDER_SUCCESS << endl;
	}
	else
	{
	    outPrintWriter << START_PROVIDER_FAILURE << endl;
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

	    outPrintWriter << STOPING_PROVIDER_MODULE << endl;
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
   	    outPrintWriter << PROVIDER_ALREADY_STOPPED << endl;
	}
	else if (retValue == 0)
	{
   	    outPrintWriter << STOP_PROVIDER_SUCCESS << endl;
	}
	else
	{
	    outPrintWriter << STOP_PROVIDER_FAILURE << endl;
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
    	Array<CIMInstance> providerInstances;
    	String moduleName;
    	String providerName;
	Array<String> moduleNames;
	Array<CIMInstance> instances;
	Boolean moduleExist = false;

	if ( _moduleSet )
	{
	    // get registered providers which are in the specified module
	    providerInstances = _client->enumerateInstances(
				PEGASUS_NAMESPACENAME_PROVIDERREG, 
				PEGASUS_CLASSNAME_PROVIDER);

	    if ( providerInstances.size() == 0 )
	    {
 	  	cerr << ERR_PROVIDER_NOT_REGISTERED << endl;
		exit(-1);	
	    }
	    else
	    {
	        // List all the registered providers which have specified module
	        for (Uint32 i = 0; i < providerInstances.size(); i++)
	        {
		    CIMInstance& instance = providerInstances[i];
		    instance.getProperty(
			instance.findProperty(_PROPERTY_PROVIDERMODULENAME)).getValue().get(moduleName);
		    instance.getProperty(
			instance.findProperty(_PROPERTY_PROVIDER_NAME)).getValue().get(providerName);
		    if (String::equalNoCase(moduleName, _moduleName))
		    {
			moduleExist = true;
		    	outPrintWriter << providerName << endl;;
		    }
	        }  
		
		if (!moduleExist)
		{
		    cerr << ERR_PROVIDER_NOT_REGISTERED << endl;
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
 	  	cerr << ERR_MODULE_NOT_REGISTERED << endl;
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

    Array<CIMInstance> namedInstances;
    String moduleName;

    // Get all registered provider modules
    namedInstances = _client->enumerateInstances(PEGASUS_NAMESPACENAME_PROVIDERREG, PEGASUS_CLASSNAME_PROVIDERMODULE);
    if ( namedInstances.size() == 0 )
    {
 	cerr << ERR_MODULE_NOT_REGISTERED << endl;
	exit(-1);	
    }


    for (Uint32 i = 0; i < namedInstances.size(); i++)
    {
	CIMInstance& instance = namedInstances[i];
	instance.getProperty(
	    instance.findProperty(_PROPERTY_PROVIDERMODULE_NAME)).getValue().get(moduleName);
	
	// If the provider is registered
	if (String::equalNoCase(_moduleName, moduleName))
	{
	    return (namedInstances[i]);
	}
    }

    // Provider module was not registered yet
    cerr << ERR_MODULE_NOT_REGISTERED << endl;
    exit(-1); 
    
    // Keep the compiler happy
    return CIMInstance();
}

// Get namedInstance for a provider
CIMInstance CIMProviderCommand::_getProviderInstance()
{

    Array<CIMInstance> namedInstances;
    String moduleName;
    String providerName;

    // Get all registered providers
    namedInstances = _client->enumerateInstances(PEGASUS_NAMESPACENAME_PROVIDERREG, PEGASUS_CLASSNAME_PROVIDER);

    if ( namedInstances.size() == 0 )
    {
 	cerr << ERR_PROVIDER_NOT_REGISTERED << endl;
	exit(-1);	
    }


    for (Uint32 i = 0; i < namedInstances.size(); i++)
    {
	CIMInstance& instance = namedInstances[i];
	instance.getProperty(
	    instance.findProperty(_PROPERTY_PROVIDERMODULENAME)).getValue().get(moduleName);
	instance.getProperty(
	    instance.findProperty(_PROPERTY_PROVIDER_NAME)).getValue().get(providerName);
	
	// If the provider is registered
	if (String::equalNoCase(_moduleName, moduleName) &&
	    String::equalNoCase(_providerName, providerName))
	{
	    return (namedInstances[i]);
	}
    }

    // Provider was not registered yet
    cerr << ERR_PROVIDER_NOT_REGISTERED << endl;
    exit(-1); 
    
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
    if(FALSE == ycmCheckCmdAuthorities())
    { 
	return 9;
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

	retCode = 0;
    return (retCode);
}
