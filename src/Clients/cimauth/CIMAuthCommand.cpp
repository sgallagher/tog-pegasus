//%/////////////////////////////////////////////////////////////////////////////
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
// Author: Nag Boranna, Hewlett Packard Company (nagaraja_boranna@hp.com)
//
//
//%/////////////////////////////////////////////////////////////////////////////

#include <iostream>

#include <Clients/cliutils/Command.h>
#include <Clients/cliutils/CommandException.h>

#include <Pegasus/getoopt/getoopt.h>

#include <Pegasus/Client/CIMClient.h>
#include <Pegasus/Common/CIMProperty.h>
#include <Pegasus/Common/CIMReference.h>
#include <Pegasus/Common/CIMStatusCode.h>
#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/Exception.h>
#include <Pegasus/Common/FileSystem.h>
#include <Pegasus/Common/HTTPConnector.h>
#include <Pegasus/Common/Monitor.h>
#include <Pegasus/Common/String.h>
#include <Pegasus/Common/System.h>
#include <Pegasus/Config/ConfigFileHandler.h>

PEGASUS_USING_STD;

PEGASUS_NAMESPACE_BEGIN

/**
    The command name.
*/
static const char COMMAND_NAME []              = "cimauth";

/**
    This constant represents the name of the User name property in the schema
*/
static const char USER_NAME []                 = "Username";

/**
    This constant represents the name of the Namespace property in the schema
*/
static const char NAMESPACE []                 = "Namespace";

/**
    This constant represents the name of the authorizations property in the schema
*/
static const char AUTHORIZATION []            = "Authorization";

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
static const int OPERATION_TYPE_UNINITIALIZED  = 0;

/**
    This constant represents a add user operation
*/
static const int OPERATION_TYPE_ADD            = 1;

/**
    This constant represents a add user operation
*/
static const int OPERATION_TYPE_MODIFY         = 2;

/**
    This constant represents a remove user operation
*/
static const int OPERATION_TYPE_REMOVE         = 3;

/**
    This constant represents a list operation
*/
static const int OPERATION_TYPE_LIST           = 4;

/**
    The constant representing the default namespace
*/
const String ROOT_NAMESPACE                         = "root/cimv2";
 
/**
    The constant representing the User class 
*/
const String PG_AUTH_CLASS                     = "PG_Authorization";
 
/**
    The default port string
*/
static const char DEFAULT_PORT_STR []          = "5988";

/**
    The host name used by the command line.
*/
static const char HOST_NAME []                 = "localhost";

/**
    The constant representing the string "port".
*/
static const char PORT []                      = "port";


/**
    The constants representing the messages.
*/

static const char CIMOM_NOT_RUNNING []         = 
                        "CIMOM may not be running.";

static const char FILE_NOT_READABLE []         = 
                        "Unable to read the config file.";

static const char ADD_AUTH_FAILURE []    = 
                        "Failed to add authorizations.";

static const char MODIFY_AUTH_FAILURE []  = 
                        "Failed to modify authorizations.";

static const char REMOVE_AUTH_FAILURE []    = 
                        "Failed to remove authorizations.";

static const char LIST_AUTH_FAILURE [] = 
                        "Failed to list the users. ";

static const char ADD_AUTH_SUCCESS []    = 
                        "Authorizations added successfully.";

static const char MODIFY_AUTH_SUCCESS []  = 
                        "Authorizations modified successfully.";

static const char REMOVE_AUTH_SUCCESS[]  = 
                        "Authorizations removed successfully.";

static const char NO_AUTHS_FOUND[] =
			"No user authorizations found for listing.";

static const char AUTH_SCHEMA_NOT_LOADED []  =
    "Please make sure that the authorization schema is loaded on the CIMOM.";

static const char  REQUIRED_ARGS_MISSING []        =
                        "Required arguments missing.";

static const char AUTH_ALREADY_EXISTS []        =
                        "Specified user authorizations already exists.";

static const char AUTH_NOT_FOUND []        =
                        "Specified user authorizations were not found.";

static const char USERNAME_REQUIRED []        =
                        "User name is required.";

static const char NAMESPACE_REQUIRED []       =
                        "Namespace is required.";

static const char AUTHFLAG_REQUIRED []       =
                        "Read or Write authorization flag is required.";


/**
    The constant representing the user provider class name
*/
static const char PG_USER_MGR_PROV_CLASS []       = "PG_UserManager";

/**
    The option character used to specify add user.
*/
static const char   OPTION_ADD                 = 'a';

/**
    The option character used to specify modify user.
*/
static const char   OPTION_MODIFY              = 'm';

/**
    The option character used to specify user name.
*/
static const char   OPTION_USER_NAME           = 'u';

/**
    The option character used to specify password.
*/
static const char   OPTION_NAMESPACE           = 'n';

/**
    The option character used to specify read permision.
*/
static const char   OPTION_READ                = 'R';

/**
    The option character used to specify write permision.
*/
static const char   OPTION_WRITE               = 'W';

/**
    The option character used to specify remove user.
*/
static const char   OPTION_REMOVE              = 'r';

/**
    The option character used to specify listing of users.
*/
static const char   OPTION_LIST                = 'l';


// Contains the address for connecting to CIMOM
char*     address     = 0;


/**
This is a CLI used to manage users of the CIM Server.  This command supports 
operations to add, modify, list and remove users.  

@author Sushma Fernandes, Hewlett-Packard Company
*/

class CIMAuthCommand : public Command 
{

public:

    /**    
        Constructs a CIMAuthCommand and initializes instance variables.
    */
    CIMAuthCommand ();

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
    // Add a new user to the CIM Server
    //
    // @param ostream        The stream to which command output is written.
    // @param ostream        The stream to which command errors are written.
    // 
    // @exception CIMException  if failed to add user
    //
    void _AddAuthorization
        (
        ostream&    		outPrintWriter, 
        ostream&    		errPrintWriter
        ); 

    //
    // Modify an existing user's password.
    //
    // @param ostream          The stream to which command output is written.
    // @param ostream          The stream to which command errors are written.
    // 
    // @exception CIMException  if failed to modify password
    //
    void _ModifyAuthorization
        (
        ostream&                 outPrintWriter,
        ostream&                 errPrintWriter
        );

    //
    // Remove an existing user from the CIM Server 
    //  
    // @param ostream          The stream to which command output is written.
    // @param ostream          The stream to which command errors are written.
    //
    // @exception CIMException  if failed to remove user
    //
    void _RemoveAuthorization
        (
        ostream&		outPrintWriter, 
        ostream&		errPrintWriter
        ); 

    //
    // List all users.       
    // 
    // @param ostream        The stream to which command output is written.
    // @param ostream        The stream to which command errors are written.
    //
    void _ListAuthorization
    (
	ostream&                outPrintWriter,
	ostream&                errPrintWriter
    );

    //
    //    Configuration File handler
    //    This handler is used to obtain the port number of the CIMOM
    //
    ConfigFileHandler* _configFileHandler;

    //
    // The CIM Client reference
    //
    CIMClient*    _client;

    //
    // The host name. 
    //
    String        _hostName;

    //
    // The name of the user.
    //
    String        _userName;

    //
    // The namespace. 
    //
    String        _namespace;

    //
    // The authorization capability. 
    //
    String        _authorizations;

    //
    // The type of operation specified on the command line. 
    //
    Uint32        _operationType;

    //
    // Flags for command options
    //
    Boolean       _userNameSet;
    Boolean       _namespaceSet;
    Boolean       _readFlagSet;
    Boolean       _writeFlagSet;

};

/**
    Constructs a CIMAuthCommand and initializes instance variables.
*/
CIMAuthCommand::CIMAuthCommand ()
{
    /**
        Initialize the instance variables.
    */
    _operationType       = OPERATION_TYPE_UNINITIALIZED;
    _userName            = String::EMPTY;
    _namespace           = String::EMPTY;
    _authorizations      = String::EMPTY;
    _hostName            = String::EMPTY;
    _namespaceSet        = false;
    _userNameSet         = false;
    _readFlagSet         = false;
    _writeFlagSet        = false;

    /**
        Build the usage string for the config command.  
    */
    String usage = String (USAGE);
    usage.append (COMMAND_NAME);
    usage.append (Cat(" -", OPTION_ADD));
    usage.append (Cat(" -", OPTION_USER_NAME, " username")); 
    usage.append (Cat(" -", OPTION_NAMESPACE, " namespace"));
    usage.append (Cat(" [ -", OPTION_READ, " ]")); 
    usage.append (Cat(" [ -", OPTION_WRITE, " ] \n")); 

    usage.append (Cat("               -", OPTION_MODIFY));
    usage.append (Cat(" -", OPTION_USER_NAME, " username")); 
    usage.append (Cat(" -", OPTION_NAMESPACE, " namespace"));
    usage.append (Cat(" [ -", OPTION_READ, " ]")); 
    usage.append (Cat(" [ -", OPTION_WRITE, " ] \n")); 

    usage.append (Cat("               -", OPTION_REMOVE));
    usage.append (Cat(" -", OPTION_USER_NAME, " username")); 
    usage.append (Cat(" [ -", OPTION_NAMESPACE, " namespace ]\n"));

    usage.append (Cat("               -", OPTION_LIST, " \n"));

    setUsage (usage);
}


/**
    Parses the command line, validates the options, and sets instance 
    variables based on the option arguments.
*/
void CIMAuthCommand::setCommand (
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
    optString.append (OPTION_ADD); 
    optString.append (Cat(
		       OPTION_USER_NAME, getoopt::GETOPT_ARGUMENT_DESIGNATOR));
    optString.append (Cat(
		       OPTION_NAMESPACE, getoopt::GETOPT_ARGUMENT_DESIGNATOR));

    optString.append (OPTION_MODIFY); 
    optString.append (Cat(
		       OPTION_USER_NAME, getoopt::GETOPT_ARGUMENT_DESIGNATOR));
    optString.append (Cat(
		       OPTION_NAMESPACE, getoopt::GETOPT_ARGUMENT_DESIGNATOR));

    optString.append (OPTION_REMOVE); 
    optString.append (Cat(
		       OPTION_USER_NAME, getoopt::GETOPT_ARGUMENT_DESIGNATOR));
    optString.append (Cat(
		       OPTION_NAMESPACE, getoopt::GETOPT_ARGUMENT_DESIGNATOR));

    optString.append (OPTION_LIST); 
    optString.append (OPTION_READ);
    optString.append (OPTION_WRITE);

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
            //  The cimuser command has no LONGFLAG options
            //
            c = options [i].getopt () [0];

            UnexpectedOptionException e (c);
            throw e;
        } 
        else if (options [i].getType () == Optarg::REGULAR)
        {
            //
            //  The cimuser command has no non-option argument options
            //
            UnexpectedArgumentException e (options [i].Value ()); 
            throw e;
        } 
        else /* if (options [i].getType () == Optarg::FLAG) */
        {

            c = options [i].getopt () [0];

            switch (c) 
            {
                case OPTION_ADD: 
                {
                    if (_operationType != OPERATION_TYPE_UNINITIALIZED)
                    {
                        //
                        // More than one operation option was found
                        //
                        UnexpectedOptionException e (OPTION_ADD);
                        throw e;
                    }

                    if (options.isSet (OPTION_ADD) > 1)
                    {
                        //
                        // More than one add user option was found
                        //
                        DuplicateOptionException e (OPTION_ADD); 
                        throw e;
                    }

                    _operationType = OPERATION_TYPE_ADD;

                    break;
                }
                case OPTION_MODIFY:
                {
                    if (_operationType != OPERATION_TYPE_UNINITIALIZED)
                    {
                        //
                        // More than one operation option was found
                        //
                        UnexpectedOptionException e (OPTION_MODIFY);
                        throw e;
                    }

                    if (options.isSet (OPTION_MODIFY) > 1)
                    {
                        //
                        // More than one modify user option was found
                        //
                        DuplicateOptionException e (OPTION_MODIFY);
                        throw e;
                    }

                    _operationType = OPERATION_TYPE_MODIFY;

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
                        // More than one remove user option was found
                        //
                        DuplicateOptionException e (OPTION_REMOVE); 
                        throw e;
                    }

                    _operationType = OPERATION_TYPE_REMOVE;

                    break;
                }
                case OPTION_USER_NAME:
                {
                    if (options.isSet (OPTION_USER_NAME) > 1)
                    {
                        //
                        // More than one username option was found
                        //
                        DuplicateOptionException e (OPTION_USER_NAME);
                        throw e;
                    }

                    _userName = options [i].Value ();

                    _userNameSet = true; 

                    break;
                }
                case OPTION_NAMESPACE:
                {
                    if (options.isSet (OPTION_NAMESPACE) > 1)
                    {
                        //
                        // More than one password option was found
                        //
                        DuplicateOptionException e (OPTION_NAMESPACE);
                        throw e;
                    }

                    _namespace = options [i].Value ();

                    _namespaceSet = true; 

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
                case OPTION_READ: 
                {
                    if (_operationType != OPERATION_TYPE_ADD &&
                        _operationType != OPERATION_TYPE_MODIFY)
                    {
                        //
                        // More than one operation option was found
                        //
                        UnexpectedOptionException e (OPTION_READ);
                        throw e;
                    }

                    if (options.isSet (OPTION_READ) > 1)
                    {
                        //
                        // More than one read option was found
                        //
                        DuplicateOptionException e (OPTION_READ); 
                        throw e;
                    }
                    _authorizations.append("r");
                    _readFlagSet = true; 
                    break;
                }
                case OPTION_WRITE: 
                {
                    if (_operationType != OPERATION_TYPE_ADD &&
                        _operationType != OPERATION_TYPE_MODIFY)
                    {
                        //
                        // More than one operation option was found
                        //
                        UnexpectedOptionException e (OPTION_WRITE);
                        throw e;
                    }

                    if (options.isSet (OPTION_WRITE) > 1)
                    {
                        //
                        // More than one write option was found
                        //
                        DuplicateOptionException e (OPTION_WRITE); 
                        throw e;
                    }
                    _authorizations.append("w");
                    _writeFlagSet = true; 
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
    if ( _operationType == OPERATION_TYPE_UNINITIALIZED  && 
	 ( _userNameSet || _namespaceSet || _readFlagSet || _writeFlagSet ) )
    {
        CommandFormatException e ( REQUIRED_ARGS_MISSING );
        throw e;
    }
	
    if ( _operationType == OPERATION_TYPE_UNINITIALIZED )
    {
        //
        // No operation type was specified 
        // Show the usage 
        //
        CommandFormatException e ( REQUIRED_ARGS_MISSING );
        throw e;
    }

    if ( _operationType == OPERATION_TYPE_ADD ||
         _operationType == OPERATION_TYPE_MODIFY ) 
    {
        if ( !_userNameSet )
        {
            //
            // An invalid option was encountered
            //
            CommandFormatException e (USERNAME_REQUIRED);
            throw e;
        }
        if ( !_namespaceSet )
        {
            //
            // An invalid option was encountered
            //
            CommandFormatException e (NAMESPACE_REQUIRED);
            throw e;
        }
        if ( !_readFlagSet && !_writeFlagSet )
        {
            //
            // Authorization flags were not specified, 
            // set default to read only
            //
            _authorizations.append("r");
            _readFlagSet = true;
        }
    }
    else if ( _operationType == OPERATION_TYPE_REMOVE )
    {
        if ( !_userNameSet )
        {
            //
            // An invalid option was encountered
            //
            CommandFormatException e (USERNAME_REQUIRED);
            throw e;
        }
    }
}

/**
    Executes the command and writes the results to the PrintWriters.
*/
Uint32 CIMAuthCommand::execute (
    ostream& outPrintWriter, 
    ostream& errPrintWriter)
{
    Boolean   connected     = false;
    String    portNumberStr = String::EMPTY;
    String    addressStr    = String::EMPTY;
    String    pegasusHome   = String::EMPTY;

    if ( _operationType == OPERATION_TYPE_UNINITIALIZED )
    {
        //
        // The command was not initialized
        //
        return ( RC_ERROR );
    }

    //
    // Get environment variable
    //

    const char* env = getenv("PEGASUS_HOME");

    if (!env || env == "")
    {
        cerr << "PEGASUS_HOME environment variable undefined" << endl;
        exit(1);
    }

    pegasusHome = env;
    FileSystem::translateSlashes(pegasusHome);

    String currentFile = pegasusHome + "/" + CURRENT_CONFIG_FILE;
    String plannedFile = pegasusHome + "/" + PLANNED_CONFIG_FILE;

    try
    {
        //
        // Open default config files and load current config properties
        //
        _configFileHandler = new ConfigFileHandler(
                                   currentFile, plannedFile, true);

        _configFileHandler->loadCurrentConfigProperties();

	//
	// Get the port number of the CIMOM
	//
        portNumberStr = _configFileHandler->getCurrentValue(PORT);

        if (portNumberStr == String::EMPTY)
        {
            portNumberStr.append (DEFAULT_PORT_STR);
        }
    }
    catch (NoSuchFile& nsf)
    {
        portNumberStr = DEFAULT_PORT_STR;
    }
    catch (FileNotReadable& fnr)
    {
        errPrintWriter << FILE_NOT_READABLE << fnr.getMessage() << endl;
        return ( RC_ERROR );
    }
    catch (ConfigFileSyntaxError& cfse)
    {
        errPrintWriter << cfse.getMessage() << endl;
        return ( RC_ERROR );
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
        Monitor* monitor = new Monitor;
        HTTPConnector* connector = new HTTPConnector(monitor);
        CIMClient client(monitor, connector);

        addressStr.append(_hostName);
        addressStr.append(":");
        addressStr.append(portNumberStr);

        address = addressStr.allocateCString ();

        client.connectLocal(address);

        connected = true;
    }
    catch(Exception& e)
    {
        outPrintWriter << CIMOM_NOT_RUNNING << endl;
        connected = false;
    }

    //
    // Perform the requested operation
    //
    switch ( _operationType )
    {
        case OPERATION_TYPE_ADD:
            try
            {
                if (connected)
                {
		    _AddAuthorization( outPrintWriter, errPrintWriter );
                }
            }
            catch (CIMException& e)
            {
                CIMStatusCode code = e.getCode();

                if (code == CIM_ERR_FAILED)
                {
		    outPrintWriter << ADD_AUTH_FAILURE << endl;
                    errPrintWriter << e.getMessage() << endl;
                }
		else if (code == CIM_ERR_NOT_SUPPORTED)
		{
		    outPrintWriter << ADD_AUTH_FAILURE << endl;
		    errPrintWriter << e.getMessage()  << endl;
                }
		else if (code == CIM_ERR_ALREADY_EXISTS)
		{
		    outPrintWriter << ADD_AUTH_FAILURE << endl;
		    outPrintWriter << AUTH_ALREADY_EXISTS << endl;
		    errPrintWriter << e.getMessage()  << endl;
		}    
                else if (code == CIM_ERR_INVALID_CLASS)
                {
		    outPrintWriter << ADD_AUTH_FAILURE << endl;
		    outPrintWriter << AUTH_SCHEMA_NOT_LOADED << endl;
                }
                else
                {
		    errPrintWriter << e.getMessage() << endl;
                }
                return ( RC_ERROR );
            }

            break; 

            case OPERATION_TYPE_MODIFY:
            try
            {
                if (connected)
                {
                    _ModifyAuthorization( outPrintWriter, errPrintWriter );
                }
            }
            catch (CIMException& e)
            {
                CIMStatusCode code = e.getCode();
                if (code == CIM_ERR_FAILED)
                {
                    outPrintWriter << MODIFY_AUTH_FAILURE << endl;
                    errPrintWriter << e.getMessage() << endl;
                }
                else if (code == CIM_ERR_NOT_SUPPORTED)
                {
                    outPrintWriter << MODIFY_AUTH_FAILURE << endl;
                    errPrintWriter << e.getMessage()  << endl;
                }
                else if (code == CIM_ERR_NOT_FOUND)
                {
                    outPrintWriter << MODIFY_AUTH_FAILURE << endl;
		    outPrintWriter << AUTH_NOT_FOUND          << endl;
                    errPrintWriter << e.getMessage()  << endl;
                }
                else if (code == CIM_ERR_INVALID_CLASS)
                {
                    outPrintWriter << MODIFY_AUTH_FAILURE << endl;
                    outPrintWriter << AUTH_SCHEMA_NOT_LOADED << endl;
                }
                else
                {
                    errPrintWriter << e.getMessage() << endl;
                }
                return ( RC_ERROR );
            }

            break;

        case OPERATION_TYPE_REMOVE:
            try
            {
                if (connected)
                {
                    _RemoveAuthorization( outPrintWriter, errPrintWriter );
                }
            }
            catch (CIMException& e)
            {
                CIMStatusCode code = e.getCode();
                if (code == CIM_ERR_FAILED)
                {
                    outPrintWriter << REMOVE_AUTH_FAILURE << endl;
                    errPrintWriter << e.getMessage() << endl;
                }
                else if (code == CIM_ERR_NOT_SUPPORTED)
                {
                    outPrintWriter << REMOVE_AUTH_FAILURE << endl;
                    errPrintWriter << e.getMessage()  << endl;
                }
                else if (code == CIM_ERR_NOT_FOUND)
                {
                    outPrintWriter << REMOVE_AUTH_FAILURE << endl;
		    outPrintWriter << AUTH_NOT_FOUND          << endl;
                    errPrintWriter << e.getMessage()  << endl;
                }
                else if (code == CIM_ERR_INVALID_CLASS)
                {
                    outPrintWriter << REMOVE_AUTH_FAILURE << endl;
                    outPrintWriter << AUTH_SCHEMA_NOT_LOADED << endl;
                }
                else
                {
                    errPrintWriter << e.getMessage() << endl;
                }
                return ( RC_ERROR );
            }
            break;

        case OPERATION_TYPE_LIST:
            try
            {
                if (connected)
                {
                    _ListAuthorization( outPrintWriter, errPrintWriter );
                }

                break;
            }
            catch (CIMException& e)
            {
                CIMStatusCode code = e.getCode();
                if (code == CIM_ERR_FAILED)
                {
                    outPrintWriter << LIST_AUTH_FAILURE << endl;
                    errPrintWriter << e.getMessage() << endl;
                }
                else if (code == CIM_ERR_NOT_SUPPORTED)
                {
                    outPrintWriter << LIST_AUTH_FAILURE << endl;
                    errPrintWriter << e.getMessage()  << endl;
                }
                else if (code == CIM_ERR_ALREADY_EXISTS)
                {
                    outPrintWriter << LIST_AUTH_FAILURE << endl;
                    errPrintWriter << e.getMessage()  << endl;
                }
                else if (code == CIM_ERR_INVALID_CLASS)
                {
                    outPrintWriter << LIST_AUTH_FAILURE << endl;
                    outPrintWriter << AUTH_SCHEMA_NOT_LOADED << endl;
                }
                else
                {
                    errPrintWriter << e.getMessage() << endl;
                }
                return ( RC_ERROR );
            }
            catch (TimedOut& timeout)
            {
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
    Add the user to the CIM Server.
*/
void CIMAuthCommand::_AddAuthorization
    (
    ostream&    outPrintWriter, 
    ostream&    errPrintWriter
    ) 
{
    CIMProperty prop;
    
    try
    {
        //
        // Open connection with CIMSever
        //
        Monitor* monitor = new Monitor;
        HTTPConnector* connector = new HTTPConnector(monitor);
        CIMClient client(monitor, connector);

        client.connectLocal(address);

        CIMInstance newInstance( PG_AUTH_CLASS );
	newInstance.addProperty ( CIMProperty( USER_NAME, _userName ) );
	newInstance.addProperty ( CIMProperty( NAMESPACE , _namespace ) );
	newInstance.addProperty ( 
            CIMProperty( AUTHORIZATION, _authorizations ) );

	client.createInstance( ROOT_NAMESPACE, newInstance );
	outPrintWriter << ADD_AUTH_SUCCESS << endl;

    }
    catch (CIMException& e)
    {
        throw e;
    }
}

//
// Modify the password for a user
//
void CIMAuthCommand::_ModifyAuthorization
    (
    ostream&    outPrintWriter, 
    ostream&    errPrintWriter
    ) 
{
    try
    {
        Array<KeyBinding>      kbArray;
        KeyBinding             kb;

        //
        // Open connection with CIMSever
        //
        Monitor* monitor = new Monitor;
        HTTPConnector* connector = new HTTPConnector(monitor);
        CIMClient client(monitor, connector);

        client.connectLocal(address);

	//
	// Build the input params
	//
        kb.setName(USER_NAME);
        kb.setValue(_userName);
        kb.setType(KeyBinding::STRING);

        kbArray.append(kb);

        kb.setName(NAMESPACE);
        kb.setValue(_namespace);
        kb.setType(KeyBinding::STRING);

        kbArray.append(kb);

        CIMReference reference(
            _hostName, ROOT_NAMESPACE, PG_AUTH_CLASS, kbArray);

        CIMInstance modifiedInst( PG_AUTH_CLASS );
	modifiedInst.addProperty( CIMProperty( USER_NAME, _userName ) );
	modifiedInst.addProperty( CIMProperty( NAMESPACE , _namespace ) );
	modifiedInst.addProperty( 
            CIMProperty( AUTHORIZATION, _authorizations ) );

/****
//ATTN: Remove this if the above code works fine!

        CIMInstance modifiedInst = client.getInstance( ROOT_NAMESPACE, reference );

        Uint32 pos = modifiedInst.findProperty( AUTHORIZATION );
        CIMProperty prop = (CIMProperty)modifiedInst.getProperty( pos );
        modifiedInst.removeProperty( pos );
        prop.setValue( CIMValue( _authorizations ) );
        modifiedInst.addProperty( prop );
***/

        CIMNamedInstance namedInstance( reference, modifiedInst );
        client.modifyInstance( ROOT_NAMESPACE, namedInstance );
        outPrintWriter << MODIFY_AUTH_SUCCESS << endl;
    }
    catch (CIMException& e)
    {
        throw e;
    }

}

//
// Remove a user    
//
void CIMAuthCommand::_RemoveAuthorization
    (
    ostream&    outPrintWriter, 
    ostream&    errPrintWriter
    ) 
{
    try
    {
        Array<KeyBinding> kbArray;
        KeyBinding        kb;

        //
        // Open connection with CIMSever
        //
        Monitor* monitor = new Monitor;
        HTTPConnector* connector = new HTTPConnector(monitor);
        CIMClient client(monitor, connector);

        client.connectLocal(address);

        kb.setName(USER_NAME);
        kb.setValue(_userName);
        kb.setType(KeyBinding::STRING);

        kbArray.append(kb);

        //
        // Include namespace if specified
        //
        if ( _namespaceSet )
        {
            kb.setName(NAMESPACE);
            kb.setValue(_namespace);
            kb.setType(KeyBinding::STRING);

            kbArray.append(kb);
        }

        CIMReference reference(
            _hostName, ROOT_NAMESPACE, PG_AUTH_CLASS, kbArray);

        client.deleteInstance(ROOT_NAMESPACE, reference);

	outPrintWriter << REMOVE_AUTH_SUCCESS << endl;

    }
    catch (CIMException& e)
    {
	throw e;
    }
}


/**
    get a list of all user names from the CIM Server.
 */
void CIMAuthCommand::_ListAuthorization
    ( 
    ostream&    outPrintWriter,
    ostream&    errPrintWriter
    )
{
    Array<CIMNamedInstance> authNamedInstances;

    try
    {
        //
        // Open connection with CIMSever
        //
        Monitor* monitor = new Monitor;
        HTTPConnector* connector = new HTTPConnector(monitor);
        CIMClient client(monitor, connector);
        
        client.connectLocal(address);

        //
        // get all the instances of class PG_Authorization
        //
        authNamedInstances =
            client.enumerateInstances(ROOT_NAMESPACE, PG_AUTH_CLASS);

        //
        // display all the user names, namespaces, and authorizations
        //
        for (Uint32 i = 0; i < authNamedInstances.size(); i++)
        {
            CIMInstance& authInstance =
                authNamedInstances[i].getInstance();

            //
            // get user name
            //
            Uint32 pos = authInstance.findProperty(USER_NAME);
            CIMProperty prop = (CIMProperty)authInstance.getProperty(pos);
            String name = prop.getValue().toString();

            //
            // get namespace name
            //
            pos = authInstance.findProperty(NAMESPACE);
            prop = (CIMProperty)authInstance.getProperty(pos);
            String ns = prop.getValue().toString();

            //
            // get authorizations
            //
            pos = authInstance.findProperty(AUTHORIZATION);
            prop = (CIMProperty)authInstance.getProperty(pos);
            String auth = prop.getValue().toString();

            outPrintWriter << name << ", " << ns << ", \"" ; 
            outPrintWriter << auth << "\"" << endl;
        }
    }
    catch (CIMException& e)
    {
        throw e;
    }
    catch (TimedOut& timeout)
    {
        throw timeout;
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
    CIMAuthCommand*      command;
    Uint32               retCode;

    command  = new CIMAuthCommand ();

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
        exit (-1);
    }

    retCode = command->execute (cout, cerr);

    exit (retCode);
    return 0;
}

