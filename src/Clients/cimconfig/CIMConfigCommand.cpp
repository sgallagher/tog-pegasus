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
// Author: Nag Boranna, Hewlett-Packard Company (nagaraja_boranna@hp.com)
//
// Modified By: Yi Zhou, Hewlett-Packard Company (yi_zhou@hp.com)
//              Roger Kumpf, Hewlett-Packard Company (roger_kumpf@hp.com)
//
//%/////////////////////////////////////////////////////////////////////////////


#include <Pegasus/Common/Config.h>
#include <Pegasus/getoopt/getoopt.h>
#include <Pegasus/Common/System.h>
#include <Pegasus/Common/FileSystem.h>
#include <Pegasus/Common/CIMProperty.h>
#include <Pegasus/Common/CIMObjectPath.h>
#include <Pegasus/Common/CIMStatusCode.h>
#include <Pegasus/Common/Exception.h>
#include <Pegasus/Common/PegasusVersion.h>

#include <Pegasus/Client/CIMClient.h>
#include <Pegasus/Config/ConfigFileHandler.h>
#include "CIMConfigCommand.h"

PEGASUS_NAMESPACE_BEGIN

/**
    The command name.
*/
static const char COMMAND_NAME []              = "cimconfig";

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
    This constant represents a property get operation
*/
static const Uint32 OPERATION_TYPE_GET            = 1;

/**
    This constant represents a property set operation
*/
static const Uint32 OPERATION_TYPE_SET            = 2;

/**
    This constant represents a property unset operation
*/
static const Uint32 OPERATION_TYPE_UNSET          = 3;

/**
    This constant represents a property list operation
*/
static const Uint32 OPERATION_TYPE_LIST           = 4;


/**
    The constant representing the default namespace
*/
const String INTERNAL_NAMESPACE                = "root/PG_Internal";
 
/**
    The constants representing the string literals.
*/
static const char PROPERTY_NAME []             = "PropertyName";

static const char DEFAULT_VALUE []             = "DefaultValue";

static const char CURRENT_VALUE []             = "CurrentValue";

static const char PLANNED_VALUE []             = "PlannedValue";

static const char DYNAMIC_PROPERTY []          = "DynamicProperty";

/**
    The constants representing the messages.
*/
static const char CIMOM_NOT_RUNNING []         = 
                        "CIM server may not be running.";

static const char FILE_NOT_EXIST []            = 
                        "Configuration files does not exist";

static const char FILE_NOT_READABLE []         = 
                        "Configuration file not readable.";

static const char FAILED_TO_GET_PROPERTY []    = 
                        "Failed to get the config property.";

static const char FAILED_TO_SET_PROPERTY []    = 
                        "Failed to set the config property.";

static const char FAILED_TO_UNSET_PROPERTY []  = 
                        "Failed to unset the config property.";

static const char FAILED_TO_LIST_PROPERTIES [] = 
                        "Failed to list the config properties. ";

static const char CURRENT_VALUE_OF_PROPERTY [] =
                        "Current value for the property '";

static const char PLANNED_VALUE_OF_PROPERTY [] =
                        "Planned value for the property '";

static const char DEFAULT_VALUE_OF_PROPERTY [] =
                        "Default value for the property '";

static const char IS_SET_TO []                 = "' is set to ";

static const char IS_NOT_SET []                = "' is not set." ;

static const char IN_CIMSERVER []              = " in CIMServer.";

static const char IN_CONFIG_FILE []            = " in configuration file.";

static const char IS_UNSET_IN_FILE []          =
                        "' is unset in the configuration file.";

static const char UPDATED_IN_FILE []           =
                        "' updated in configuration file.";

static const char CONFIG_SCHEMA_NOT_LOADED []  =
    "Please make sure that the config schema is loaded in the CIM server.";

static const char PROPERTY_NOT_FOUND []        =
                        "Specified property name was not found.";

static const char INVALID_PROPERTY_VALUE []    =
                        "Specified property value is not valid.";

static const char PROPERTY_NOT_MODIFIED []     =
                        "Specified property can not be modified.";
/**
    The constant representing the config setting class name
*/
static const char PG_CONFIG_CLASS []          = "PG_ConfigSetting";

/**
    The option character used to specify get config property.
*/
static const char   OPTION_GET                 = 'g';

/**
    The option character used to specify set config property.
*/
static const char   OPTION_SET                 = 's';

/**
    The option character used to specify unset config property.
*/
static const char   OPTION_UNSET               = 'u';

/**
    The option character used to specify listing of config properties.
*/
static const char   OPTION_LIST                = 'l';

/**
    The option character used to specify the current config value.
*/
static const char   OPTION_CURRENT_VALUE       = 'c';

/**
    The option character used to specify the planned config value.
*/
static const char   OPTION_PLANNED_VALUE       = 'p';

/**
    The option character used to specify the default config value.
*/
static const char   OPTION_DEFAULT_VALUE       = 'd';



/**
    Constructs a CIMConfigCommand and initializes instance variables.
*/
CIMConfigCommand::CIMConfigCommand ()
{
    /**
        Initialize the instance variables.
    */
    _operationType       = OPERATION_TYPE_UNINITIALIZED;
    _propertyName        = String::EMPTY;
    _propertyValue       = String::EMPTY;
    _currentValueSet     = false;
    _plannedValueSet     = false;
    _defaultValueSet     = false;
    _hostName            = String::EMPTY;

    /**
        Build the usage string for the config command.  
    */
    String usage;
    usage.reserve(200);
    usage.append(USAGE);
    usage.append(COMMAND_NAME);

    usage.append(" -").append(OPTION_GET).append(" name");
    usage.append(" [ -").append(OPTION_CURRENT_VALUE); 
    usage.append(" ] [ -").append(OPTION_DEFAULT_VALUE);
    usage.append(" ] [ -").append(OPTION_PLANNED_VALUE).append(" ]\n");

    usage.append("                 -").append(OPTION_SET).append(" name=value");
    usage.append(" [ -").append(OPTION_CURRENT_VALUE);
    usage.append(" ] [ -").append(OPTION_PLANNED_VALUE).append(" ]\n");

    usage.append("                 -").append(OPTION_UNSET).append(" name");
    usage.append(" [ -").append(OPTION_CURRENT_VALUE);
    usage.append(" ] [ -").append(OPTION_PLANNED_VALUE).append(" ]\n");

    usage.append("                 -").append(OPTION_LIST);
    usage.append(" [ -").append(OPTION_CURRENT_VALUE);
    usage.append(" | -").append(OPTION_PLANNED_VALUE).append(" ]\n");

    setUsage (usage);
}


/**
    Parses the command line, validates the options, and sets instance 
    variables based on the option arguments.
*/
void CIMConfigCommand::setCommand (Uint32 argc, char* argv []) 
    throw (CommandFormatException)
{
    unsigned int      i                = 0;
    Uint32            c                = 0;
    String            property         = String ();
    String            badOptionString  = String ();
    String            optString        = String ();
    Uint32            equalsIndex      = 0;

    //
    //  Construct optString
    //
    optString.append(OPTION_GET);
    optString.append(GETOPT_ARGUMENT_DESIGNATOR);

    optString.append(OPTION_SET);
    optString.append(GETOPT_ARGUMENT_DESIGNATOR);

    optString.append(OPTION_UNSET);
    optString.append(GETOPT_ARGUMENT_DESIGNATOR);

    optString.append(OPTION_LIST);
    optString.append(OPTION_CURRENT_VALUE);
    optString.append(OPTION_PLANNED_VALUE);
    optString.append(OPTION_DEFAULT_VALUE);

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
            //  The cimconfig command has no LONGFLAG options
            //
            c = options [i].getopt () [0];

            UnexpectedOptionException e (c);
            throw e;
        } 
        else if (options [i].getType () == Optarg::REGULAR)
        {
            //
            //  The cimconfig command has no non-option argument options
            //
            UnexpectedArgumentException e (options [i].Value ()); 
            throw e;
        } 
        else /* if (options [i].getType () == Optarg::FLAG) */
        {

            c = options [i].getopt () [0];

            switch (c) 
            {
                case OPTION_GET: 
                {
                    if (_operationType != OPERATION_TYPE_UNINITIALIZED)
                    {
                        //
                        // More than one operation option was found
                        //
                        UnexpectedOptionException e (OPTION_GET);
                        throw e;
                    }

                    if (options.isSet (OPTION_GET) > 1)
                    {
                        //
                        // More than one get option was found
                        //
                        DuplicateOptionException e (OPTION_GET); 
                        throw e;
                    }

                    _propertyName = options [i].Value ();

                    _operationType = OPERATION_TYPE_GET;

                    break;
                }

                case OPTION_SET: 
                {
                    if (_operationType != OPERATION_TYPE_UNINITIALIZED)
                    {
                        //
                        // More than one operation option was found
                        //
                        UnexpectedOptionException e (OPTION_SET);
                        throw e;
                    }

                    if (options.isSet (OPTION_SET) > 1)
                    {
                        //
                        // More than one set option was found
                        //
                        DuplicateOptionException e (OPTION_SET); 
                        throw e;
                    }

                    _operationType = OPERATION_TYPE_SET;

                    property = options [i].Value ();

                    equalsIndex = property.find ('=');

                    if ( equalsIndex == PEG_NOT_FOUND )
                    {
                        //
                        // The property value was not specified
                        //
                        InvalidOptionArgumentException e (property,
                            OPTION_SET);
                        throw e;
                    }

                    _propertyName  = property.subString( 0, equalsIndex );

                    _propertyValue = property.subString( equalsIndex + 1 );

                    break;
                }

                case OPTION_UNSET: 
                {
                    if (_operationType != OPERATION_TYPE_UNINITIALIZED)
                    {
                        //
                        // More than one operation option was found
                        //
                        UnexpectedOptionException e (OPTION_UNSET);
                        throw e;
                    }

                    if (options.isSet (OPTION_UNSET) > 1)
                    {
                        //
                        // More than one unset option was found
                        //
                        DuplicateOptionException e (OPTION_UNSET); 
                        throw e;
                    }

                    _propertyName = options [i].Value ();

                    _operationType = OPERATION_TYPE_UNSET;

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

                case OPTION_CURRENT_VALUE: 
                {
                    if (options.isSet (OPTION_CURRENT_VALUE) > 1)
                    {
                        //
                        // More than one current value option was found
                        //
                        DuplicateOptionException e (OPTION_CURRENT_VALUE); 
                        throw e;
                    }

                    _currentValueSet = true;
                    break;
                }

                case OPTION_PLANNED_VALUE: 
                {
                    if (options.isSet (OPTION_PLANNED_VALUE) > 1)
                    {
                        //
                        // More than one planned value option was found
                        //
                        DuplicateOptionException e (OPTION_PLANNED_VALUE); 
                        throw e;
                    }

                    _plannedValueSet = true;
                    break;
                }

                case OPTION_DEFAULT_VALUE: 
                {
                    if (options.isSet (OPTION_DEFAULT_VALUE) > 1)
                    {
                        //
                        // More than one default value option was found
                        //
                        DuplicateOptionException e (OPTION_DEFAULT_VALUE); 
                        throw e;
                    }

                    _defaultValueSet = true;
                    break;
                }

                default:
                    //
                    // Should never get here
                    //
                    break;
            }
        }
    }

    if ( _operationType == OPERATION_TYPE_UNINITIALIZED )
    {
        //
        // No operation type was specified; throw exception
        // so that usage can be displayed.
        //
        CommandFormatException e ("");
        throw e;
    }

    if ( ( _operationType != OPERATION_TYPE_GET ) && ( _defaultValueSet ) )
    {
        //
        // An invalid option was encountered
        //
        InvalidOptionException e (OPTION_DEFAULT_VALUE);
        throw e;
    }

    if (_operationType == OPERATION_TYPE_LIST)
    {
        if ( _currentValueSet && _plannedValueSet )
        {
            //
            // An invalid option was encountered
            //
            InvalidOptionException e (OPTION_CURRENT_VALUE);
            throw e;
        }
    }
    else
    {
        //
        // if no options specified for get, set or unset operations
        // then set option as _currentValueSet
        //
        if ( !_currentValueSet && !_plannedValueSet && !_defaultValueSet )
        {
            _currentValueSet = true;
        }
    }

}

/** 
    Print message to the given stream
*/

//void CIMConfigCommand::_printErrorMessage(
//    CIMStatusCode code, 
//    const String&,
//    ostream& errPrintWriter)
//{
//
//}

/**
    Executes the command and writes the results to the PrintWriters.
*/
Uint32 CIMConfigCommand::execute (
    ostream& outPrintWriter, 
    ostream& errPrintWriter)
{
    Boolean   connected     = false;
    String    defaultValue  = String::EMPTY;
    String    currentValue  = String::EMPTY;
    String    plannedValue  = String::EMPTY;
    String    pegasusHome   = String::EMPTY;
    Boolean   gotCurrentValue = false;
    Boolean   gotPlannedValue = false;


    if ( _operationType == OPERATION_TYPE_UNINITIALIZED )
    {
        //
        // The command was not initialized
        //
        return ( RC_ERROR );
    }

    //
    // Get environment variables
    //

    String currentFile;
    String plannedFile;

    const char* env = getenv("PEGASUS_HOME");

    if (!env || env == "")
    {
        cerr << "PEGASUS_HOME environment variable undefined," << endl;
        cerr << "using the configuration files from the current directory." << endl;

        currentFile.append(CURRENT_CONFIG_FILE);
        plannedFile.append(PLANNED_CONFIG_FILE);
    }
    else
    {
        pegasusHome = env;
        FileSystem::translateSlashes(pegasusHome);

        currentFile.append(pegasusHome + "/" + CURRENT_CONFIG_FILE);
        plannedFile.append(pegasusHome + "/" + PLANNED_CONFIG_FILE);
    }

    try
    {
        //
        // Open default config files and load current config properties
        //
        _configFileHandler = new ConfigFileHandler(currentFile, plannedFile, true);
    }
    catch (NoSuchFile& nsf)
    {
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
        _client = new CIMClient;

        _client->connectLocal();

        connected = true;
    }
    catch(CIMClientException& e)
    {
        //
        // Failed to connect, so process the request offline.
        // When CIMOM is running the config command updates changes to 
        // config properties in the planned config file, so load only
        // planned config properties. 
        //
        _configFileHandler->loadPlannedConfigProperties();
        connected = false;
    }

    //
    // Perform the requested operation
    //
    switch ( _operationType )
    {
        case OPERATION_TYPE_GET:
            try
            {
                if (connected)
                {
                    Array<String> propertyValues;

                    _getPropertiesFromCIMServer( outPrintWriter, 
                        errPrintWriter, _propertyName, propertyValues);

                    defaultValue = propertyValues[1];
                    currentValue = propertyValues[2];
                    gotCurrentValue = true;
                    plannedValue = propertyValues[3];
                    gotPlannedValue = true;
                }
                else
                {
                    if (_defaultValueSet)
                    {
                        errPrintWriter << "Option -" << OPTION_DEFAULT_VALUE <<
                           " is not valid for this command when" <<
                           " CIM server is not running." << endl;
                        return ( RC_ERROR );
                    }
                    else 
                    {
                        gotCurrentValue = _configFileHandler->getCurrentValue ( 
                            _propertyName, currentValue );
                        gotPlannedValue = _configFileHandler->getPlannedValue ( 
                            _propertyName, plannedValue );
                    }
                }
            }
            catch (CIMClientCIMException& e)
            {
                CIMStatusCode code = e.getCode();

                if (code == CIM_ERR_NOT_FOUND || 
                    code == CIM_ERR_FAILED)
                {
                    outPrintWriter << PROPERTY_NOT_FOUND << endl;

                    errPrintWriter << e.getMessage() << endl;
                }
                else if (code == CIM_ERR_INVALID_CLASS)
                {
                    outPrintWriter << FAILED_TO_GET_PROPERTY << endl <<
                        CONFIG_SCHEMA_NOT_LOADED << endl;
                }
                else
                {
                    errPrintWriter << FAILED_TO_GET_PROPERTY <<
                        e.getMessage() << endl;
                }
                return ( RC_ERROR );
            }
            catch (CIMClientException& e)
            {
                outPrintWriter << FAILED_TO_GET_PROPERTY << endl <<
                    e.getMessage() << endl;
                return ( RC_ERROR );
            }

            if( _currentValueSet || ( !_plannedValueSet && !_defaultValueSet ) )
            {
                if (gotCurrentValue)
                {
                    outPrintWriter << "Current value: " << currentValue << endl;
                }
                else
                {
                    outPrintWriter << "Current value can not be determined " <<
                        "because the CIM server is not running." << endl;
                }
            }

            if( _plannedValueSet )
            {
                if (gotPlannedValue)
                {
                    outPrintWriter << "Planned value: " << plannedValue << endl;
                }
                else
                {
                    outPrintWriter << "Planned value can not be determined "
                        "because the CIM server is not running." << endl;
                }
            }

            if( _defaultValueSet )
            {
                outPrintWriter << "Default value: " << defaultValue << endl;
            }
            break; 

        case OPERATION_TYPE_SET:
            //
            // send changes to CIMOM if running, else send to config file
            //
            try
            {
                if (connected)
                {
                    _updatePropertyInCIMServer( outPrintWriter, 
                        errPrintWriter, _propertyName, _propertyValue, false);

                    if ( _currentValueSet )
                    {
                        outPrintWriter << CURRENT_VALUE_OF_PROPERTY
                            << _propertyName << IS_SET_TO << "\"" 
                            << _propertyValue << "\"" << IN_CIMSERVER << endl;
                    }

                    if ( _plannedValueSet )
                    {
                        outPrintWriter << PLANNED_VALUE_OF_PROPERTY
                            << _propertyName << IS_SET_TO << "\"" <<_propertyValue
                            << "\"" << IN_CIMSERVER << endl;
                    }
                }
                else
                {
                    if (_currentValueSet)
                    {
                        outPrintWriter << CURRENT_VALUE_OF_PROPERTY <<
                            _propertyName <<"' can not be set " <<
                            "because the CIM server is not running." << endl;

                        return ( RC_ERROR );
                    }
                    else if (_plannedValueSet)
                    {
                        if ( !_configFileHandler->updatePlannedValue( 
                            _propertyName, _propertyValue, false ) )
                        {
                            outPrintWriter << "Failed to update the planned" 
                                << " value of the Property '" << _propertyName 
                                << "'" << IN_CONFIG_FILE << endl;
                            return ( RC_ERROR );
                        }
                    }

                    outPrintWriter << "Property '" << _propertyName <<
                        UPDATED_IN_FILE << endl;
                }
            }
            catch (CIMClientCIMException& e)
            {
                CIMStatusCode code = e.getCode();

                if (code == CIM_ERR_TYPE_MISMATCH)
                { 
                    outPrintWriter << INVALID_PROPERTY_VALUE << endl;

                    errPrintWriter << e.getMessage() << endl;
                }
                else if (code == CIM_ERR_NOT_FOUND)
                {
                    outPrintWriter << PROPERTY_NOT_FOUND << endl;

                    errPrintWriter << e.getMessage() << endl;
                }
                else if (code == CIM_ERR_NOT_SUPPORTED)
                {
                    outPrintWriter << PROPERTY_NOT_MODIFIED << endl;

                    errPrintWriter << e.getMessage() << endl;
                }
                else if (code == CIM_ERR_FAILED)
                {
                    outPrintWriter << FAILED_TO_SET_PROPERTY << 
                        e.getMessage() << endl;
                }
                else if (code == CIM_ERR_ALREADY_EXISTS)
                {
                    outPrintWriter << "The property '" << _propertyName <<
                        "' value is already set to '" << _propertyValue <<
                        "'." << endl;

                    errPrintWriter << e.getMessage() << endl;
                }
                else if (code == CIM_ERR_INVALID_CLASS)
                {
                    outPrintWriter << FAILED_TO_SET_PROPERTY << endl << 
                        CONFIG_SCHEMA_NOT_LOADED << endl;
                }
                else
                {
                    errPrintWriter << e.getMessage() << endl;
                }
                return ( RC_ERROR );
            }
            catch (CIMClientException& e)
            {
                outPrintWriter << FAILED_TO_SET_PROPERTY << endl << 
                    e.getMessage() << endl;
                return ( RC_ERROR );
            }
            break;

        case OPERATION_TYPE_UNSET:
            //
            // send changes to CIMOM if running, else send to config file
            //
            try
            {
                if (connected)
                {
                    _propertyValue = String::EMPTY;

                    _updatePropertyInCIMServer( outPrintWriter, 
                        errPrintWriter, _propertyName, _propertyValue, true);

                    if ( _currentValueSet )
                    {
                        outPrintWriter << CURRENT_VALUE_OF_PROPERTY
                            << _propertyName <<"' is set to default value"
                            << IN_CIMSERVER << endl;
                    }

                    if ( _plannedValueSet )
                    {
                        outPrintWriter << "Property '" << _propertyName <<
                       	    IS_UNSET_IN_FILE << endl;
                    }
                }
                else
                {
                    if (_currentValueSet)
                    {
                        outPrintWriter << CURRENT_VALUE_OF_PROPERTY <<
                            _propertyName <<"' can not be unset " <<
                            "because the CIM server is not running." << endl;

                        return ( RC_ERROR );
                    }

                    if ( !_configFileHandler->updatePlannedValue( 
                        _propertyName, _propertyValue, true ) )
                    {
                        return ( RC_ERROR );
                    }

                    outPrintWriter << "Property '" << _propertyName <<
                        IS_UNSET_IN_FILE << endl;
                }

            }
            catch (CIMClientCIMException& e)
            {
                CIMStatusCode code = e.getCode();

                if (code == CIM_ERR_TYPE_MISMATCH)
                {
                    outPrintWriter << INVALID_PROPERTY_VALUE << endl;

                    errPrintWriter << e.getMessage() << endl;
                }
                else if (code == CIM_ERR_NOT_FOUND)
                {
                    outPrintWriter << PROPERTY_NOT_FOUND << endl;

                    errPrintWriter << e.getMessage() << endl;
                }
                else if (code == CIM_ERR_NOT_SUPPORTED)
                {
                    outPrintWriter << PROPERTY_NOT_MODIFIED << endl;

                    errPrintWriter << e.getMessage() << endl;
                }
                else if (code == CIM_ERR_FAILED)
                {
                    outPrintWriter << FAILED_TO_UNSET_PROPERTY <<
                        e.getMessage() << endl;
                }
                else if (code == CIM_ERR_ALREADY_EXISTS)
                {
                    outPrintWriter << "The property '" << _propertyName <<
                        "' value is already unset." << endl;

                    errPrintWriter << e.getMessage() << endl;
                }
                else if (code == CIM_ERR_INVALID_CLASS)
                {
                    outPrintWriter << FAILED_TO_UNSET_PROPERTY << endl <<
                        CONFIG_SCHEMA_NOT_LOADED << endl;
                }
                else
                {
                    errPrintWriter << e.getMessage() << endl;
                }
                return ( RC_ERROR );
            }
            catch (CIMClientException& e)
            {
                outPrintWriter << FAILED_TO_UNSET_PROPERTY << endl <<
                    e.getMessage() << endl;
                return ( RC_ERROR );
            }
            break;

        case OPERATION_TYPE_LIST:
            //
            // send request to CIMOM if running, else send to config file
            //
            try
            {
                Array<String> propertyNames;
                Array<String> propertyValues;

                if (connected)
                {
                    _listAllPropertiesInCIMServer(outPrintWriter, 
                         errPrintWriter, propertyNames, propertyValues);

                }
                else
                {
                    if (_plannedValueSet)
                    {
                        _configFileHandler->getAllPlannedProperties(
                            propertyNames, propertyValues);
                    }
                    else
                    {
                        outPrintWriter << "Current value of properties can not be " <<
                            "listed because the CIM server is not running." << endl;
                        break;
                    }
                }

                Uint32 valuesSize = propertyValues.size();
                Uint32 namesSize  = propertyNames.size();

                if (namesSize == 0)
                {
                    outPrintWriter << "No configuration properties found"
                        << " in the configuration file." << endl;
                    break;
                }

                for ( Uint32 i = 0; i < namesSize; i++ )
                {
                    outPrintWriter << propertyNames[i];
                    if ( ( _currentValueSet || _plannedValueSet ) &&
                         ( valuesSize == namesSize) )
                    {
                        outPrintWriter << "=" << propertyValues[i];
                    }
                    outPrintWriter << endl;
                }
                break;
            }
            catch (CIMClientCIMException& e)
            {
                CIMStatusCode code = e.getCode();

                if (code == CIM_ERR_NOT_FOUND || code == CIM_ERR_INVALID_CLASS)
                {
                    outPrintWriter << FAILED_TO_LIST_PROPERTIES << endl <<
                        CONFIG_SCHEMA_NOT_LOADED << endl;
                }
                else if (code == CIM_ERR_FAILED)
                {
                    outPrintWriter << FAILED_TO_LIST_PROPERTIES <<
                        e.getMessage() << endl;
                }
                else
                {
                    errPrintWriter << e.getMessage() << endl;
                }

                return ( RC_ERROR );
            }
            catch (CIMClientException& e)
            {
                outPrintWriter << FAILED_TO_LIST_PROPERTIES <<  endl <<
                    e.getMessage() << endl;
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
    Get property values for the specified property from the CIM Server.
*/
void CIMConfigCommand::_getPropertiesFromCIMServer
    (
    ostream&    outPrintWriter, 
    ostream&    errPrintWriter,
    const String&    propName,
    Array <String>&    propValues
    ) 
{
    CIMProperty prop;

    try
    {
        Array<KeyBinding> kbArray;
        KeyBinding        kb;

        kb.setName(PROPERTY_NAME);
        kb.setValue(propName);
        kb.setType(KeyBinding::STRING);

        kbArray.append(kb);

        CIMObjectPath reference(
            _hostName, INTERNAL_NAMESPACE, PG_CONFIG_CLASS, kbArray);

        CIMInstance cimInstance =
            _client->getInstance(INTERNAL_NAMESPACE, reference);

        Uint32 pos = cimInstance.findProperty(PROPERTY_NAME);
        prop = (CIMProperty)cimInstance.getProperty(pos);
        propValues.append(prop.getValue().toString());

        pos = cimInstance.findProperty(DEFAULT_VALUE);
        prop = (CIMProperty)cimInstance.getProperty(pos);
        propValues.append(prop.getValue().toString());

        pos = cimInstance.findProperty(CURRENT_VALUE);
        prop = (CIMProperty)cimInstance.getProperty(pos);
        propValues.append(prop.getValue().toString());

        pos = cimInstance.findProperty(PLANNED_VALUE);
        prop = (CIMProperty)cimInstance.getProperty(pos);
        propValues.append(prop.getValue().toString());

        pos = cimInstance.findProperty(DYNAMIC_PROPERTY);
        prop = (CIMProperty)cimInstance.getProperty(pos);
        propValues.append(prop.getValue().toString());
    }
    catch (CIMClientException& e)
    {
        throw e;
    }
}

/**
    Send an updated property value to the CIM Server.
 */
void CIMConfigCommand::_updatePropertyInCIMServer
    (
    ostream&    outPrintWriter, 
    ostream&    errPrintWriter,
    const String&   propName,
    const String&   propValue,
    Boolean     isUnsetOperation
    ) 
{

    try
    {
        Array<KeyBinding> kbArray;
        KeyBinding        kb;

        kb.setName(PROPERTY_NAME);
        kb.setValue(propName);
        kb.setType(KeyBinding::STRING);

        kbArray.append(kb);

        CIMObjectPath reference(
            _hostName, INTERNAL_NAMESPACE, PG_CONFIG_CLASS, kbArray);

        CIMInstance modifiedInst = CIMInstance(PG_CONFIG_CLASS);
        Array<String> propertyList;

        if ( _currentValueSet )
        {
            if (!isUnsetOperation)
            {
                CIMProperty prop =
                    CIMProperty(CURRENT_VALUE, CIMValue(propValue));
                modifiedInst.addProperty(prop);
            }
            propertyList.append(CURRENT_VALUE);
        }

        if ( _plannedValueSet )
        {
            if (!isUnsetOperation)
            {
                CIMProperty prop =
                    CIMProperty(PLANNED_VALUE, CIMValue(propValue));
                modifiedInst.addProperty(prop);
            }
            propertyList.append(PLANNED_VALUE);
        }

        CIMNamedInstance namedInstance(reference, modifiedInst);
        _client->modifyInstance(
            INTERNAL_NAMESPACE,
            namedInstance,
            false,
            CIMPropertyList(propertyList));
    }
    catch (CIMClientException& e)
    {
        throw e;
    }
}


/**
    get a list of all property names and value from the CIM Server.
 */
void CIMConfigCommand::_listAllPropertiesInCIMServer
    ( 
    ostream&    outPrintWriter,
    ostream&    errPrintWriter,
    Array <String>&   propNames,
    Array <String>&   propValues
    )
{
    Array<CIMNamedInstance> configNamedInstances;

    try
    {
        if ( _currentValueSet ||  _plannedValueSet )
        {
            //
            // get all the instances of class PG_ConfigSetting
            //
            configNamedInstances =
                _client->enumerateInstances(INTERNAL_NAMESPACE, PG_CONFIG_CLASS);

            //
            // copy all the property names and values
            //
            for (Uint32 i = 0; i < configNamedInstances.size(); i++)
            {
                CIMInstance& configInstance =
                    configNamedInstances[i].getInstance();

                Uint32 pos = configInstance.findProperty("PropertyName");
                CIMProperty prop = (CIMProperty)configInstance.getProperty(pos);
                propNames.append(prop.getValue().toString());

                if (_currentValueSet)
                {
                    //
                    // get current value
                    //
                    pos = configInstance.findProperty("CurrentValue");
                    prop = (CIMProperty)configInstance.getProperty(pos);
                    propValues.append(prop.getValue().toString());
                }
                else if (_plannedValueSet)
                {
                    //
                    // get planned value
                    //
                    pos = configInstance.findProperty("PlannedValue");
                    prop = (CIMProperty)configInstance.getProperty(pos);
                    propValues.append(prop.getValue().toString());
                }
            }
        }
        else 
        {
            //
            // call enumerateInstanceNames
            //
            Array<CIMObjectPath> instanceNames =
                _client->enumerateInstanceNames(INTERNAL_NAMESPACE, PG_CONFIG_CLASS);

            //
            // copy all the property names
            //
            for (Uint32 i = 0; i < instanceNames.size(); i++)
            {
                Array<KeyBinding> kbArray = instanceNames[i].getKeyBindings();
  
                if (kbArray.size() > 0)
                {
                    propNames.append(kbArray[0].getValue());
                }
            }
 
        }
    }
    catch (CIMClientException& e)
    {
        throw e;
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
    CIMConfigCommand*    command;
    Uint32               returnCode;

    command  = new CIMConfigCommand ();


    try 
    {
        command->setCommand (argc, argv);
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

    returnCode = command->execute (cout, cerr);

    exit (returnCode);
    return 0;
}
