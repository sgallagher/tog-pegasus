//%2005////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001, 2002 BMC Software; Hewlett-Packard Development
// Company, L.P.; IBM Corp.; The Open Group; Tivoli Systems.
// Copyright (c) 2003 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation, The Open Group.
// Copyright (c) 2004 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation; VERITAS Software Corporation; The Open Group.
// Copyright (c) 2005 Hewlett-Packard Development Company, L.P.; IBM Corp.;
// EMC Corporation; VERITAS Software Corporation; The Open Group.
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
//              Carol Ann Krug Graves, Hewlett-Packard Company
//                  (carolann_graves@hp.com)
//              Terry Martin, Hewlett-Packard Company (terry.martin@hp.com)
//              Alagaraja Ramasubramanian, IBM (alags_raj@in.ibm.com) - PEP-167
//              Amit K Arora, IBM (amitarora@in.ibm.com) - Bug#2333,#2351
//              Josephine Eskaline Joyce, IBM (jojustin@in.ibm.com) - Bug#1794
//              Josephine Eskaline Joyce, IBM (jojustin@in.ibm.com) - PEP#101, Bug#2756, Bug#3032
//
//%/////////////////////////////////////////////////////////////////////////////


#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/Constants.h>
#include <Pegasus/Common/System.h>
#include <Pegasus/Common/FileSystem.h>
#include <Pegasus/Common/CIMProperty.h>
#include <Pegasus/Common/CIMObjectPath.h>
#include <Pegasus/Common/CIMStatusCode.h>
#include <Pegasus/Common/Exception.h>
#include <Pegasus/Common/PegasusVersion.h>
#include <Pegasus/Common/XmlWriter.h>
#include <Pegasus/getoopt/getoopt.h>

#include <Pegasus/Client/CIMClient.h>
#include <Pegasus/Config/ConfigFileHandler.h>
#include "CIMConfigCommand.h"

#ifdef PEGASUS_OS_OS400
#include "qycmutiltyUtility.H"
#include "qycmutilu2.H"
#include "OS400ConvertChar.h"
#include "vfyptrs.cinc"
#include <stdio.h>
#endif

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
static const char COMMAND_NAME []              = "cimconfig";

/**
    The usage string for this command.  This string is displayed
    when an error occurs in parsing or validating the command line.
*/
static const char USAGE []                     = "Usage: ";

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
    This constant represents a help operation
*/
static const Uint32 OPERATION_TYPE_HELP           = 5;

/**
    This constant represents a version display operation
*/
static const Uint32 OPERATION_TYPE_VERSION        = 6;

/**
    The constants representing the string literals.
*/
static const CIMName PROPERTY_NAME              = CIMName ("PropertyName");

static const CIMName DEFAULT_VALUE              = CIMName ("DefaultValue");

static const CIMName CURRENT_VALUE              = CIMName ("CurrentValue");

static const CIMName PLANNED_VALUE              = CIMName ("PlannedValue");

static const CIMName DYNAMIC_PROPERTY           = CIMName ("DynamicProperty");

/**
    The constants representing the messages.
*/
static const char CIMOM_NOT_RUNNING []         = 
                        "CIM server may not be running.";

static const char FILE_NOT_EXIST []            = 
                        "Configuration files does not exist.";

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

// no longer used
// static const char CURRENT_VALUE_OF_PROPERTY [] =
                        // "Current value for the property '";

// static const char PLANNED_VALUE_OF_PROPERTY [] =
                        // "Planned value for the property '";

// static const char DEFAULT_VALUE_OF_PROPERTY [] =
                        // "Default value for the property '";

static const char IS_SET_TO []                 = "' is set to ";

static const char IS_NOT_SET []                = "' is not set." ;

static const char IN_CIMSERVER []              = " in CIMServer.";

static const char IN_CONFIG_FILE []            = " in configuration file.";

static const char IS_UNSET_IN_FILE []          =
                        "' is unset in the configuration file.";

static const char UPDATED_IN_FILE []           =
                        "' updated in configuration file.";

static const char CONFIG_SCHEMA_NOT_LOADED []  =
    "Please restore the internal repository on the CIM Server.";

static const char PROPERTY_NOT_FOUND []        =
                        "Specified property name was not found.";

static const char INVALID_PROPERTY_VALUE []    =
                        "Specified property value is not valid.";

static const char PROPERTY_NOT_MODIFIED []     =
                        "Specified property can not be modified.";

//l10n default messages and resource keys

static const char OPTION_INVALID_CIM_RUNNING [] = 
	"Option -$0 is not valid for this command when CIM server is not running.";

static const char OPTION_INVALID_CIM_RUNNING_KEY [] = 
	"Clients.CIMConfig.CIMConfigCommand.OPTION_INVALID_CIM_RUNNING";

static const char CIMOM_NOT_RUNNING_KEY [] = 
	"Clients.CIMConfig.CIMConfigCommand.CIMOM_NOT_RUNNING";

static const char FILE_NOT_EXIST_KEY [] = 
	"Clients.CIMConfig.CIMConfigCommand.FILE_NOT_EXIST";

static const char FILE_NOT_READABLE_KEY [] = 
	"Clients.CIMConfig.CIMConfigCommand.FILE_NOT_READABLE";

static const char FAILED_TO_GET_PROPERTY_KEY [] = 
	"Clients.CIMConfig.CIMConfigCommand.FAILED_TO_GET_PROPERTY";

static const char FAILED_TO_SET_PROPERTY_KEY [] = 
	"Clients.CIMConfig.CIMConfigCommand.FAILED_TO_SET_PROPERTY";

static const char FAILED_TO_UNSET_PROPERTY_KEY [] = 
	"Clients.CIMConfig.CIMConfigCommand.FAILED_TO_UNSET_PROPERTY";

static const char FAILED_TO_LIST_PROPERTIES_KEY [] = 
	"Clients.CIMConfig.CIMConfigCommand.FAILED_TO_LIST_PROPERTIES";

static const char CONFIG_SCHEMA_NOT_LOADED_KEY [] = 
	"Clients.CIMConfig.CIMConfigCommand.CONFIG_SCHEMA_NOT_LOADED";

static const char PROPERTY_NOT_FOUND_KEY [] = 
	"Clients.CIMConfig.CIMConfigCommand.PROPERTY_NOT_FOUND";

static const char INVALID_PROPERTY_VALUE_KEY [] = 
	"Clients.CIMConfig.CIMConfigCommand.INVALID_PROPERTY_VALUE";

static const char PROPERTY_NOT_MODIFIED_KEY [] = 
	"Clients.CIMConfig.CIMConfigCommand.PROPERTY_NOT_MODIFIED";
                        
//static const char VALUE_OF_PROPERTY_IS [] = 
	//"$0 value for the property $1 is set to \"$2\" in CIMServer.";

//static const char VALUE_OF_PROPERTY_IS_KEY [] = 
	//"Clients.CIMConfig.CIMConfigCommand.VALUE_OF_PROPERTY_IS";

static const char PLANNED_VALUE_OF_PROPERTY_IS [] = "Planned value for the property $0 is set to \"$1\" in CIMServer.";
static const char PLANNED_VALUE_OF_PROPERTY_IS_KEY [] = "Clients.CIMConfig.CIMConfigCommand.PLANNED_VALUE_OF_PROPERTY_IS";

static const char CURRENT_VALUE_OF_PROPERTY_IS [] = "Current value for the property $0 is set to \"$1\" in CIMServer.";
static const char CURRENT_VALUE_OF_PROPERTY_IS_KEY [] = "Clients.CIMConfig.CIMConfigCommand.CURRENT_VALUE_OF_PROPERTY_IS";

static const char DEFAULT_VALUE_OF_PROPERTY_IS [] = "Default value for the property $0 is set to \"$1\" in CIMServer.";
static const char DEFAULT_VALUE_OF_PROPERTY_IS_KEY [] = "Clients.CIMConfig.CIMConfigCommand.DEFAULT_VALUE_OF_PROPERTY_IS";


//static const char VALUE_OF_PROPERTY_CANNOT_BE_SET [] = 
	//"$0 value for the property '$1' can not be set because the CIM server is not running.";

//static const char VALUE_OF_PROPERTY_CANNOT_BE_SET_KEY [] = 
	//"Clients.CIMConfig.CIMConfigCommand.VALUE_OF_PROPERTY_CANNOT_BE_SET";

static const char PLANNED_VALUE_OF_PROPERTY_CANNOT_BE_SET [] = "Planned value for the property '$0' can not be set because the CIM server is not running.";
static const char PLANNED_VALUE_OF_PROPERTY_CANNOT_BE_SET_KEY [] = "Clients.CIMConfig.CIMConfigCommand.PLANNED_VALUE_OF_PROPERTY_CANNOT_BE_SET";

static const char CURRENT_VALUE_OF_PROPERTY_CANNOT_BE_SET [] = "Current value for the property '$0' can not be set because the CIM server is not running.";
static const char CURRENT_VALUE_OF_PROPERTY_CANNOT_BE_SET_KEY [] = "Clients.CIMConfig.CIMConfigCommand.CURRENT_VALUE_OF_PROPERTY_CANNOT_BE_SET";

static const char DEFAULT_VALUE_OF_PROPERTY_CANNOT_BE_SET [] = "Default value for the property '$0' can not be set because the CIM server is not running.";
static const char DEFAULT_VALUE_OF_PROPERTY_CANNOT_BE_SET_KEY [] = "Clients.CIMConfig.CIMConfigCommand.DEFAULT_VALUE_OF_PROPERTY_CANNOT_BE_SET";

//static const char FAILED_UPDATE_OF_VALUE_IN_FILE [] = 
	//"Failed to update the $0 value of the Property '$1' in configuration file.";

//static const char FAILED_UPDATE_OF_VALUE_IN_FILE_KEY [] = 
	//"Clients.CIMConfig.CIMConfigCommand.FAILED_UPDATE_OF_VALUE_IN_FILE";
	
static const char FAILED_UPDATE_OF_PLANNED_VALUE_IN_FILE [] = "Failed to update the planned value of the Property '$0' in configuration file.";
static const char FAILED_UPDATE_OF_PLANNED_VALUE_IN_FILE_KEY [] = "Clients.CIMConfig.CIMConfigCommand.FAILED_UPDATE_OF_PLANNED_VALUE_IN_FILE";

static const char FAILED_UPDATE_OF_CURRENT_VALUE_IN_FILE [] = "Failed to update the current value of the Property '$0' in configuration file.";
static const char FAILED_UPDATE_OF_CURRENT_VALUE_IN_FILE_KEY [] = "Clients.CIMConfig.CIMConfigCommand.FAILED_UPDATE_OF_CURRENT_VALUE_IN_FILE";

static const char FAILED_UPDATE_OF_DEFAULT_VALUE_IN_FILE [] = "Failed to update the default value of the Property '$0' in configuration file.";
static const char FAILED_UPDATE_OF_DEFAULT_VALUE_IN_FILE_KEY [] = "Clients.CIMConfig.CIMConfigCommand.FAILED_UPDATE_OF_DEFAULT_VALUE_IN_FILE";


// 485
static const char PROPERTY_UPDATED_IN_FILE [] = 
	"Property '$0' updated in configuration file.";

static const char PROPERTY_UPDATED_IN_FILE_KEY [] = 
	"Clients.CIMConfig.CIMConfigCommand.PROPERTY_UPDATED_IN_FILE";

//static const char VALUE_IS [] = 
	//"$0 value: $1";
	
static const char PLANNED_VALUE_IS [] = "Planned value: $0";
static const char PLANNED_VALUE_IS_KEY [] =	"Clients.CIMConfig.CIMConfigCommand.PLANNED_VALUE_IS";

static const char CURRENT_VALUE_IS [] = "Current value: $0";
static const char CURRENT_VALUE_IS_KEY [] =	"Clients.CIMConfig.CIMConfigCommand.CURRENT_VALUE_IS";


static const char DEFAULT_VALUE_IS [] = "Default value: $0";
static const char DEFAULT_VALUE_IS_KEY [] =	"Clients.CIMConfig.CIMConfigCommand.DEFAULT_VALUE_IS";


//static const char VALUE_IS_KEY [] = 
	//"Clients.CIMConfig.CIMConfigCommand.VALUE_IS";


//static const char CANNOT_DETERMINE_VALUE_CIM_RUNNING [] = 
	//"$0 value can not be determined because the CIM server is not running.";

//static const char CANNOT_DETERMINE_VALUE_CIM_RUNNING_KEY [] = 
	//"Clients.CIMConfig.CIMConfigCommand.CANNOT_DETERMINE_VALUE_CIM_RUNNING";
  
static const char CANNOT_DETERMINE_PLANNED_VALUE_CIM_RUNNING [] = "Planned value can not be determined because the CIM server is not running.";
static const char CANNOT_DETERMINE_PLANNED_VALUE_CIM_RUNNING_KEY [] = "Clients.CIMConfig.CIMConfigCommand.CANNOT_DETERMINE_PLANNED_VALUE_CIM_RUNNING";
  
static const char CANNOT_DETERMINE_CURRENT_VALUE_CIM_RUNNING [] = "Current value can not be determined because the CIM server is not running.";
static const char CANNOT_DETERMINE_CURRENT_VALUE_CIM_RUNNING_KEY [] = "Clients.CIMConfig.CIMConfigCommand.CANNOT_DETERMINE_CURRENT_VALUE_CIM_RUNNING";

static const char CANNOT_DETERMINE_DEFAULT_VALUE_CIM_RUNNING [] = "Default value can not be determined because the CIM server is not running.";
static const char CANNOT_DETERMINE_DEFAULT_VALUE_CIM_RUNNING_KEY [] = "Clients.CIMConfig.CIMConfigCommand.CANNOT_DETERMINE_DEFAULT_VALUE_CIM_RUNNING";

                    								  
static const char PROPERTY_VALUE_ALREADY_SET_TO [] = 
	"The property '$0' value is already set to '$1'.";

static const char PROPERTY_VALUE_ALREADY_SET_TO_KEY [] = 
	"Clients.CIMConfig.CIMConfigCommand.PROPERTY_VALUE_ALREADY_SET_TO";

//static const char PROPERTY_SET_TO_DEFAULT [] = 
	//"$0 value for the property '$1' is set to default value in CIMServer.";
                            
//static const char PROPERTY_SET_TO_DEFAULT_KEY [] = 
	//"Clients.CIMConfig.CIMConfigCommand.PROPERTY_SET_TO_DEFAULT";

static const char PLANNED_PROPERTY_SET_TO_DEFAULT [] = "Planned value for the property '$0' is set to default value in CIMServer.";
static const char PLANNED_PROPERTY_SET_TO_DEFAULT_KEY [] = "Clients.CIMConfig.CIMConfigCommand.PLANNED_PROPERTY_SET_TO_DEFAULT";

static const char CURRENT_PROPERTY_SET_TO_DEFAULT [] = "Current value for the property '$0' is set to default value in CIMServer.";
static const char CURRENT_PROPERTY_SET_TO_DEFAULT_KEY [] = "Clients.CIMConfig.CIMConfigCommand.CURRENT_PROPERTY_SET_TO_DEFAULT";

static const char PROPERTY_UNSET_IN_FILE [] = 
	"Property '$0' is unset in the configuration file.";

static const char PROPERTY_UNSET_IN_FILE_KEY [] = 
	"Clients.CIMConfig.CIMConfigCommand.PROPERTY_UNSET_IN_FILE";

//static const char PROPERTY_CANNOT_BE_UNSET_CIM_NOT_RUNNING [] = 
	//"$0 value for the property '$1' can not be unset because the CIM server is not running.";

//static const char PROPERTY_CANNOT_BE_UNSET_CIM_NOT_RUNNING_KEY [] = 
	//"Clients.CIMConfig.CIMConfigCommand.PROPERTY_CANNOT_BE_UNSET_CIM_NOT_RUNNING";

static const char PLANNED_PROPERTY_CANNOT_BE_UNSET_CIM_NOT_RUNNING [] = "Planned value for the property '$0' can not be unset because the CIM server is not running.";
static const char PLANNED_PROPERTY_CANNOT_BE_UNSET_CIM_NOT_RUNNING_KEY [] = "Clients.CIMConfig.CIMConfigCommand.PLANNED_PROPERTY_CANNOT_BE_UNSET_CIM_NOT_RUNNING";

static const char CURRENT_PROPERTY_CANNOT_BE_UNSET_CIM_NOT_RUNNING [] = "Current value for the property '$0' can not be unset because the CIM server is not running.";
static const char CURRENT_PROPERTY_CANNOT_BE_UNSET_CIM_NOT_RUNNING_KEY [] = "Clients.CIMConfig.CIMConfigCommand.CURRENT_PROPERTY_CANNOT_BE_UNSET_CIM_NOT_RUNNING";

static const char DEFAULT_PROPERTY_CANNOT_BE_UNSET_CIM_NOT_RUNNING [] = "Default value for the property '$0' can not be unset because the CIM server is not running.";
static const char DEFAULT_PROPERTY_CANNOT_BE_UNSET_CIM_NOT_RUNNING_KEY [] = "Clients.CIMConfig.CIMConfigCommand.DEFAULT_PROPERTY_CANNOT_BE_UNSET_CIM_NOT_RUNNING";

static const char PROPERTY_VALUE_ALREADY_UNSET [] = 
	"The property '$0' value is already unset.";

static const char PROPERTY_VALUE_ALREADY_UNSET_KEY [] = 
	"Clients.CIMConfig.CIMConfigCommand.PROPERTY_VALUE_ALREADY_UNSET";

//static const char VALUES_CANNNOT_BE_LISTED_CIM_NOT_RUNNING [] = 
	//"$0 value of properties can not be listed because the CIM server is not running.";

//static const char VALUES_CANNNOT_BE_LISTED_CIM_NOT_RUNNING_KEY [] = 
	//"Clients.CIMConfig.CIMConfigCommand.VALUES_CANNNOT_BE_LISTED_CIM_NOT_RUNNING";

static const char PLANNED_VALUES_CANNOT_BE_LISTED_CIM_NOT_RUNNING [] = "Planned value of properties can not be listed because the CIM server is not running.";
static const char PLANNED_VALUES_CANNOT_BE_LISTED_CIM_NOT_RUNNING_KEY [] = "Clients.CIMConfig.CIMConfigCommand.PLANNED_VALUES_CANNOT_BE_LISTED_CIM_NOT_RUNNING";

static const char CURRENT_VALUES_CANNOT_BE_LISTED_CIM_NOT_RUNNING [] = "Current value of properties can not be listed because the CIM server is not running.";
static const char CURRENT_VALUES_CANNOT_BE_LISTED_CIM_NOT_RUNNING_KEY [] = "Clients.CIMConfig.CIMConfigCommand.CURRENT_VALUES_CANNOT_BE_LISTED_CIM_NOT_RUNNING";

static const char DEFAULT_VALUES_CANNOT_BE_LISTED_CIM_NOT_RUNNING [] = "Default value of properties can not be listed because the CIM server is not running.";
static const char DEFAULT_VALUES_CANNOT_BE_LISTED_CIM_NOT_RUNNING_KEY [] = "Clients.CIMConfig.CIMConfigCommand.DEFAULT_VALUES_CANNOT_BE_LISTED_CIM_NOT_RUNNING";

static const char NO_PROPERTIES_FOUND_IN_FILE [] = 
	"No configuration properties found in the configuration file.";

static const char NO_PROPERTIES_FOUND_IN_FILE_KEY [] = 
	"Clients.CIMConfig.CIMConfigCommand.NO_PROPERTIES_FOUND_IN_FILE";

static const char REQUIRED_ARGS_MISSING []        =
                        "Required arguments missing.";

static const char REQUIRED_ARGS_MISSING_KEY []        = "Clients.cimuser.CIMUserCommand.REQUIRED_ARGS_MISSING";

static const char ERR_OPTION_NOT_SUPPORTED [] =
                        "Invalid option. Use '--help' to obtain command syntax.";

static const char ERR_OPTION_NOT_SUPPORTED_KEY [] = "Clients.CIMConfig.CIMConfigCommand.ERR_OPTION_NOT_SUPPORTED";

static const char ERR_USAGE [] =
                        "Incorrect usage. Use '--help' to obtain command syntax.";

static const char ERR_USAGE_KEY [] = "Clients.CIMConfig.CIMConfigCommand.ERR_USAGE";


//l10n end default messages and keys

/**
    The option character used to specify get config property.
*/
static const char   OPTION_GET                 = 'g';

/**
    The option character used to specify set config property.
*/
static const char   OPTION_SET                 = 's';

#ifdef PEGASUS_OS_OS400
/**
    The option character used to specify no output to stdout or stderr.
*/
     static const char OPTION_QUIET_VALUE      = 'q';
#endif

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
    The option character used to display help info.
*/
static const char   OPTION_HELP                = 'h';

/**
    The option character used to display version info.
*/
static const char   OPTION_VERSION             = 'v';

static const char   LONG_HELP []  = "help";

static const char   LONG_VERSION []  = "version";

/**
    Constructs a CIMConfigCommand and initializes instance variables.
*/
CIMConfigCommand::CIMConfigCommand ()
{
    /**
        Initialize the instance variables.
    */
    _operationType       = OPERATION_TYPE_UNINITIALIZED;
    _propertyName        = CIMName ();
    _propertyValue       = String::EMPTY;
    _currentValueSet     = false;
    _plannedValueSet     = false;
    _defaultValueSet     = false;
#ifdef PEGASUS_OS_OS400
     _defaultQuietSet     = false;
#endif
    _hostName            = String::EMPTY;

    /**
        Build the usage string for the config command.  
    */
    usage.reserveCapacity(200);
    usage.append(USAGE);
    
    usage.append(COMMAND_NAME);
#ifdef PEGASUS_OS_OS400
    usage.append(" -").append(OPTION_GET).append(" name");
    usage.append(" [ -").append(OPTION_CURRENT_VALUE); 
    usage.append(" ] [ -").append(OPTION_DEFAULT_VALUE);
    usage.append(" ] [ -").append(OPTION_PLANNED_VALUE);
    usage.append(" ] [ -").append(OPTION_QUIET_VALUE).append(" ]\n");

    usage.append("                 -").append(OPTION_SET).append(" name=value");
    usage.append(" [ -").append(OPTION_CURRENT_VALUE);
    usage.append(" ] [ -").append(OPTION_PLANNED_VALUE);
    usage.append(" ] [ -").append(OPTION_QUIET_VALUE).append(" ]\n");

    usage.append("                 -").append(OPTION_UNSET).append(" name");
    usage.append(" [ -").append(OPTION_CURRENT_VALUE);
    usage.append(" ] [ -").append(OPTION_PLANNED_VALUE);
    usage.append(" ] [ -").append(OPTION_QUIET_VALUE).append(" ]\n");

    usage.append("                 -").append(OPTION_LIST);
    usage.append(" [ -").append(OPTION_CURRENT_VALUE);
    usage.append(" | -").append(OPTION_PLANNED_VALUE).append(" ]\n");
#else  
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
#endif


    usage.append("                 -").append(OPTION_HELP).append("\n");
    usage.append("                 --").append(LONG_HELP).append("\n");
    usage.append("                 --").append(LONG_VERSION).append("\n");

    usage.append("Options : \n");
    usage.append("    -c         - Use current configuration\n");
    usage.append("    -d         - Use default configuration\n");
    usage.append("    -g         - Get the value of specified configuration property\n");
    usage.append("    -h, --help - Display this help message\n");
    usage.append("    -l         - Display all the configuration properties\n");
    usage.append("    -p         - Configuration used on next CIM Server start\n");
#ifdef PEGASUS_OS_OS400
    usage.append("    -q         - Specify quiet mode, avoiding output to stdout or stderr\n");
#endif
    usage.append("    -s         - Add or Update configuration property value\n");
    usage.append("    -u         - Reset configuration property to its default value\n");
    usage.append("    --version  - Display CIM Server version number\n");

    usage.append("\nUsage note: The cimconfig command can be used to update the next planned\n"); 
    usage.append( "configuration without having the CIM Server running. All other options \n");
    usage.append( "of the cimconfig command require that the CIM Server is running.");
	
//l10n localize usage
#ifdef PEGASUS_HAS_ICU
	
	#ifdef PEGASUS_OS_OS400
		
		MessageLoaderParms menuparms("Clients.CIMConfig.CIMConfigCommand.MENU.PEGASUS_OS_OS400",usage);
		menuparms.msg_src_path = MSG_PATH;
		usage = MessageLoader::getMessage(menuparms);
	
	#else
		
		MessageLoaderParms menuparms("Clients.CIMConfig.CIMConfigCommand.MENU.STANDARD",usage);
		menuparms.msg_src_path = MSG_PATH;
		usage = MessageLoader::getMessage(menuparms);
		
	#endif

#endif
    setUsage (usage);
}

/**
    Parses the command line, validates the options, and sets instance 
    variables based on the option arguments.
*/
void CIMConfigCommand::setCommand (Uint32 argc, char* argv [])
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
#ifdef PEGASUS_OS_OS400
    optString.append(OPTION_QUIET_VALUE);
#endif
    optString.append(OPTION_HELP);

    //
    //  Initialize and parse options
    //
    getoopt options ("");
    options.addFlagspec(optString);
    //PEP#167 - adding long flag for options : 'help' and 'version'
    options.addLongFlagspec(LONG_HELP,getoopt::NOARG);
    options.addLongFlagspec(LONG_VERSION,getoopt::NOARG);

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
        if (options[i].getType () == Optarg::LONGFLAG)
        {
            if (options[i].getopt () == LONG_HELP)
            {
                if (_operationType != OPERATION_TYPE_UNINITIALIZED)
                {
                    String param = String (LONG_HELP);
                    //
                    // More than one operation option was found
                    //
                    UnexpectedOptionException e (param);
                    throw e;
                }

               _operationType = OPERATION_TYPE_HELP;
            }
            else if (options[i].getopt () == LONG_VERSION)
            {
                if (_operationType != OPERATION_TYPE_UNINITIALIZED)
                {
                    String param = String (LONG_VERSION);
                    //
                    // More than one operation option was found
                    //
                    UnexpectedOptionException e (param);
                    throw e;
                }

               _operationType = OPERATION_TYPE_VERSION;
            }
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

                    try
                    {
                        _propertyName = options [i].Value ();
                    }
                    catch (const InvalidNameException&)
                    {
                        throw InvalidOptionArgumentException(
                            options[i].Value(), OPTION_GET);
                    }

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

                    try
                    {
                        _propertyName = CIMName (property.subString
                            (0, equalsIndex));
                    }
                    catch (const InvalidNameException&)
                    {
                        throw InvalidOptionArgumentException(
                            property, OPTION_SET);
                    }

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

                    try
                    {
                        _propertyName = options [i].Value ();
                    }
                    catch (const InvalidNameException&)
                    {
                        throw InvalidOptionArgumentException(
                            options[i].Value(), OPTION_UNSET);
                    }

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

                //PEP#167 - 2 new cases added below for HELP and VERSION
                case OPTION_HELP: 
                {
                    if (_operationType != OPERATION_TYPE_UNINITIALIZED)
                    {
                        //
                        // More than one operation option was found
                        //
                        UnexpectedOptionException e (OPTION_HELP);
                        throw e;
                    }

                    if (options.isSet (OPTION_HELP) > 1)
                    {
                        //
                        // More than one list option was found
                        //
                        DuplicateOptionException e (OPTION_HELP); 
                        throw e;
                    }
                    _operationType = OPERATION_TYPE_HELP;
                    break;
                }
                case OPTION_VERSION: 
                {
                    if (_operationType != OPERATION_TYPE_UNINITIALIZED)
                    {
                        //
                        // More than one operation option was found
                        //
                        UnexpectedOptionException e (OPTION_VERSION);
                        throw e;
                    }

                    if (options.isSet (OPTION_VERSION) > 1)
                    {
                        //
                        // More than one list option was found
                        //
                        DuplicateOptionException e (OPTION_VERSION); 
                        throw e;
                    }
                    _operationType = OPERATION_TYPE_VERSION;
                    break;
                }

#ifdef PEGASUS_OS_OS400
                 // check for quiet option before processing the rest of the options
		case OPTION_QUIET_VALUE:
		{
			_defaultQuietSet = true;
			break;
	        }     
#endif

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
        //l10n change was missing and added while implementing PEP#167 changes
        CommandFormatException e (localizeMessage(MSG_PATH,REQUIRED_ARGS_MISSING_KEY, REQUIRED_ARGS_MISSING));
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
#ifdef PEGASUS_OS_OS400
	if( _defaultQuietSet ){
	    //
            // An invalid option was encountered
            //
            InvalidOptionException e (OPTION_QUIET_VALUE);
            throw e;
	}
#endif
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

#ifdef PEGASUS_OS_OS400
    // disable standard output and standard error
    if( _defaultQuietSet && (_operationType != OPERATION_TYPE_LIST) ){
        freopen("/dev/null","w",stdout);
        freopen("/dev/null","w",stderr);
        // Set the stderr stream to buffered with 32k.  
        // Allows utf-8 to be sent to stderr (P9A66750)
        setvbuf(stderr, new char[32768], _IOLBF, 32768);
    }
#endif

    if ( _operationType == OPERATION_TYPE_UNINITIALIZED )
    {
        //
        // The command was not initialized
        //
        return ( RC_ERROR );
    }
    //PEP#167 - Added Options HELP and VERSION
    //PEP#167 - CIMServer need not be running for these to work
    else if (_operationType == OPERATION_TYPE_HELP)
    {
        cerr << usage << endl;
        return (RC_SUCCESS);
    }
    else if(_operationType == OPERATION_TYPE_VERSION)
    {
        cerr << "Version " << PEGASUS_PRODUCT_VERSION << endl;
        return (RC_SUCCESS);
    }

    //
    // Get environment variables
    //
#ifdef PEGASUS_PLATFORM_OS400_ISERIES_IBM
#pragma convert(37)
    const char* env = getenv("PEGASUS_HOME");
#pragma convert(0)
    // Set pegasusHome to the env var,if it is set.  Otherwise,
    // use the OS/400 default path.
    if (env != NULL)
    {
	char home[256] = {0};
	if (strlen(env) < 256)
	{
	    strcpy(home, env);
	    EtoA(home);
	}
    }
    else
        pegasusHome = OS400_DEFAULT_PEGASUS_HOME;

    String currentFile = FileSystem::getAbsolutePath(pegasusHome.getCString(), CURRENT_CONFIG_FILE);
    String plannedFile = FileSystem::getAbsolutePath(pegasusHome.getCString(), PLANNED_CONFIG_FILE);
#else
    const char* env = getenv("PEGASUS_HOME");

    String currentFile = FileSystem::getAbsolutePath(env, CURRENT_CONFIG_FILE);
    String plannedFile = FileSystem::getAbsolutePath(env, PLANNED_CONFIG_FILE);
#endif

    try
    {
        //
        // Open default config files and load current config properties
        //
        _configFileHandler = new ConfigFileHandler(currentFile, plannedFile, true);
    }
    catch (const NoSuchFile&)
    {
    }
    catch (const FileNotReadable& fnr)
    {
    	//l10n
        //errPrintWriter << FILE_NOT_READABLE << fnr.getMessage() << endl;
        errPrintWriter << localizeMessage(MSG_PATH, FILE_NOT_READABLE_KEY, FILE_NOT_READABLE) << fnr.getMessage() << endl;
        return ( RC_ERROR );
    }
    catch (const ConfigFileSyntaxError& cfse)
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
        // Construct the CIMClient and set to request server messages
        // in the default language of this client process.
        _client.reset(new CIMClient);
        _client->setRequestDefaultLanguages(); //l10n
    }
    catch (const Exception & e)
    {
        errPrintWriter << e.getMessage() << endl;
        return ( RC_ERROR );        
    }

    try
    {
        //
        // Open connection with CIMSever
        //
        _client->connectLocal();

        connected = true;
    }
    catch(const Exception&)
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
                    	//l10n
                        //errPrintWriter << "Option -" << OPTION_DEFAULT_VALUE <<
                           //" is not valid for this command when" <<
                           //" CIM server is not running." << endl;
                        errPrintWriter << localizeMessage(MSG_PATH, 
                        						OPTION_INVALID_CIM_RUNNING_KEY, 
                        						OPTION_INVALID_CIM_RUNNING, 
                        						String(&OPTION_DEFAULT_VALUE,1)) << endl;  
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
            catch (const CIMException& e)
            {
                CIMStatusCode code = e.getCode();

                if (code == CIM_ERR_NOT_FOUND || 
                    code == CIM_ERR_FAILED)
                {
                	//l10n
                    //outPrintWriter << PROPERTY_NOT_FOUND << endl;
                    outPrintWriter << localizeMessage(MSG_PATH,
                    							PROPERTY_NOT_FOUND_KEY,
                    							PROPERTY_NOT_FOUND)
                    							 << endl;

                    errPrintWriter << e.getMessage() << endl;
                }
                else if (code == CIM_ERR_INVALID_CLASS)
                {
                	//l10n
                    //outPrintWriter << FAILED_TO_GET_PROPERTY << endl <<
                        //CONFIG_SCHEMA_NOT_LOADED << endl;
                    outPrintWriter << localizeMessage(MSG_PATH,
                    							FAILED_TO_GET_PROPERTY_KEY,
                    							FAILED_TO_GET_PROPERTY)
                    							 << endl << 
                    				  localizeMessage(MSG_PATH,
                    				  			CONFIG_SCHEMA_NOT_LOADED_KEY,
                    				  			CONFIG_SCHEMA_NOT_LOADED) << endl;
                }
                else
                {
                	//l10n
                    //errPrintWriter << FAILED_TO_GET_PROPERTY <<
                        //e.getMessage() << endl;
                    outPrintWriter << localizeMessage(MSG_PATH,FAILED_TO_GET_PROPERTY_KEY,
                    							      FAILED_TO_GET_PROPERTY) 
                    			   << e.getMessage() << endl;
                    			
                }
                return ( RC_ERROR );
            }
            catch (const Exception& e)
            {
            	//l10n
                //outPrintWriter << FAILED_TO_GET_PROPERTY << endl <<
                    //e.getMessage() << endl;
                outPrintWriter << localizeMessage(MSG_PATH,FAILED_TO_GET_PROPERTY_KEY,
                    							      FAILED_TO_GET_PROPERTY) << endl
                    			   << e.getMessage() << endl;
                return ( RC_ERROR );
            }

            if( _currentValueSet || ( !_plannedValueSet && !_defaultValueSet ) )
            {
                if (gotCurrentValue)
                {
                	//l10n
                    //outPrintWriter << "Current value: " << currentValue << endl;
                    outPrintWriter << localizeMessage(MSG_PATH,
                    							      CURRENT_VALUE_IS_KEY,
                    								  CURRENT_VALUE_IS,
                    								  currentValue) << endl;
                }
                else
                {
                	//l10n
                    //outPrintWriter << "Current value can not be determined " <<
                        //"because the CIM server is not running." << endl;
                    outPrintWriter << localizeMessage(MSG_PATH,
                    								  CANNOT_DETERMINE_CURRENT_VALUE_CIM_RUNNING_KEY,
                    								  CANNOT_DETERMINE_CURRENT_VALUE_CIM_RUNNING) << endl;
                }
            }

            if( _plannedValueSet )
            {
                if (gotPlannedValue)
                {
                	//l10n
                    //outPrintWriter << "Planned value: " << plannedValue << endl;
                    outPrintWriter << localizeMessage(MSG_PATH,
                    								  PLANNED_VALUE_IS_KEY,
                    								  PLANNED_VALUE_IS, 
                    								  plannedValue) << endl;
                }
                else
                {
                	//l10n
                    //outPrintWriter << "Planned value can not be determined "
                        //"because the CIM server is not running." << endl;
                    outPrintWriter << localizeMessage(MSG_PATH,
                    								  CANNOT_DETERMINE_PLANNED_VALUE_CIM_RUNNING_KEY,
                    								  CANNOT_DETERMINE_PLANNED_VALUE_CIM_RUNNING) << endl;
                }
            }

            if( _defaultValueSet )
            {
            	//l10n
                //outPrintWriter << "Default value: " << defaultValue << endl;
                outPrintWriter << localizeMessage(MSG_PATH,
                								  DEFAULT_VALUE_IS_KEY,
                								  DEFAULT_VALUE_IS, 
                    							  defaultValue) << endl;
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
                    	//l10n
                        //outPrintWriter << CURRENT_VALUE_OF_PROPERTY
                            //<< _propertyName << IS_SET_TO << "\"" 
                            //<< _propertyValue << "\"" << IN_CIMSERVER << endl;
                            
                       outPrintWriter << localizeMessage(MSG_PATH,
                       									 CURRENT_VALUE_OF_PROPERTY_IS_KEY,
                       									 CURRENT_VALUE_OF_PROPERTY_IS,
                       									 _propertyName.getString(),
                       									 _propertyValue) << endl;
                    }

                    if ( _plannedValueSet )
                    {
                    	//l10n
                        //outPrintWriter << PLANNED_VALUE_OF_PROPERTY
                            //<< _propertyName << IS_SET_TO << "\"" <<_propertyValue
                            //<< "\"" << IN_CIMSERVER << endl;
                        outPrintWriter << localizeMessage(MSG_PATH,
                        								  PLANNED_VALUE_OF_PROPERTY_IS_KEY,
                       									  PLANNED_VALUE_OF_PROPERTY_IS,
                       									  _propertyName.getString(),
                       									  _propertyValue) << endl;
                    }
                }
                else
                {
                    if (_currentValueSet)
                    {
                    	//l10n
                        //outPrintWriter << CURRENT_VALUE_OF_PROPERTY <<
                            //_propertyName <<"' can not be set " <<
                            //"because the CIM server is not running." << endl;
                        outPrintWriter << localizeMessage(MSG_PATH,
                        								  CURRENT_VALUE_OF_PROPERTY_CANNOT_BE_SET_KEY,
                        								  CURRENT_VALUE_OF_PROPERTY_CANNOT_BE_SET,
                        								  _propertyName.getString()) << endl;

                        return ( RC_ERROR );
                    }
                    else if (_plannedValueSet)
                    {
                        if ( !_configFileHandler->updatePlannedValue( 
                            _propertyName, _propertyValue, false ) )
                        {
                        	//l10n
                            //outPrintWriter << "Failed to update the planned" 
                                //<< " value of the Property '" << _propertyName 
                                //<< "'" << IN_CONFIG_FILE << endl;
                            outPrintWriter << localizeMessage(MSG_PATH,
                            								  FAILED_UPDATE_OF_PLANNED_VALUE_IN_FILE_KEY,
                            								  FAILED_UPDATE_OF_PLANNED_VALUE_IN_FILE,
                            								  _propertyName.getString()) << endl;
                            return ( RC_ERROR );
                        }
                    }
					//l10n
                    //outPrintWriter << "Property '" << _propertyName <<
                        //UPDATED_IN_FILE << endl;
                    outPrintWriter << localizeMessage(MSG_PATH,
                    								  PROPERTY_UPDATED_IN_FILE_KEY,
                    								  PROPERTY_UPDATED_IN_FILE,
                    								  _propertyName.getString()) << endl;
                   
                }
            }
            catch (const CIMException& e)
            {
                CIMStatusCode code = e.getCode();

                if (code == CIM_ERR_TYPE_MISMATCH)
                { 
                	//l10n
                    //outPrintWriter << INVALID_PROPERTY_VALUE << endl;
                    outPrintWriter << localizeMessage(MSG_PATH,
                    								  INVALID_PROPERTY_VALUE_KEY,
                    								  INVALID_PROPERTY_VALUE) << endl;

                    errPrintWriter << e.getMessage() << endl;
                }
                else if (code == CIM_ERR_NOT_FOUND)
                {
                	//l10n
                    //outPrintWriter << PROPERTY_NOT_FOUND << endl;
					outPrintWriter << localizeMessage(MSG_PATH,
                    								  PROPERTY_NOT_FOUND_KEY,
                    								  PROPERTY_NOT_FOUND) << endl;
                    								  
                    errPrintWriter << e.getMessage() << endl;
                }
                else if (code == CIM_ERR_NOT_SUPPORTED)
                {
                	//l10n
                    //outPrintWriter << PROPERTY_NOT_MODIFIED << endl;
                    outPrintWriter << localizeMessage(MSG_PATH,
                    								  PROPERTY_NOT_MODIFIED_KEY,
                    								  PROPERTY_NOT_MODIFIED) << endl;

                    errPrintWriter << e.getMessage() << endl;
                }
                else if (code == CIM_ERR_FAILED)
                {
                	//l10n
                    //outPrintWriter << FAILED_TO_SET_PROPERTY << 
                        //e.getMessage() << endl;
                    outPrintWriter << localizeMessage(MSG_PATH,
                    								  FAILED_TO_SET_PROPERTY_KEY,
                    								  FAILED_TO_SET_PROPERTY) 
                    								  << e.getMessage() << endl;
                }
                else if (code == CIM_ERR_ALREADY_EXISTS)
                {
                	//l10n
                    //outPrintWriter << "The property '" << _propertyName <<
                        //"' value is already set to '" << _propertyValue <<
                        //"'." << endl;
                    outPrintWriter << localizeMessage(MSG_PATH,
                    								  PROPERTY_VALUE_ALREADY_SET_TO_KEY,
                    								  PROPERTY_VALUE_ALREADY_SET_TO,
                    								  _propertyName.getString(),
                    								  _propertyValue) << endl;

                    errPrintWriter << e.getMessage() << endl;
                }
                else if (code == CIM_ERR_INVALID_CLASS)
                {
                	//l10n
                    //outPrintWriter << FAILED_TO_SET_PROPERTY << endl << 
                        //CONFIG_SCHEMA_NOT_LOADED << endl;
                    outPrintWriter << localizeMessage(MSG_PATH,
                    								  FAILED_TO_SET_PROPERTY_KEY,
                    								  FAILED_TO_SET_PROPERTY)
                    								  << endl <<
                    				  localizeMessage(MSG_PATH,
                    				   				  CONFIG_SCHEMA_NOT_LOADED_KEY,
                    				   				  CONFIG_SCHEMA_NOT_LOADED)
                    				   				  << endl;
                }
                else
                {
                    errPrintWriter << e.getMessage() << endl;
                }
                return ( RC_ERROR );
            }
            catch (const Exception& e)
            {
            	//l10n
                //outPrintWriter << FAILED_TO_SET_PROPERTY << endl << 
                    //e.getMessage() << endl;
                outPrintWriter << localizeMessage(MSG_PATH,
                								  FAILED_TO_SET_PROPERTY_KEY,
                								  FAILED_TO_SET_PROPERTY) 
                								  << endl 
                								  << e.getMessage() << endl;
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
                    	//l10n
                        //outPrintWriter << CURRENT_VALUE_OF_PROPERTY
                            //<< _propertyName <<"' is set to default value"
                            //<< IN_CIMSERVER << endl;
                        outPrintWriter << localizeMessage(MSG_PATH,
                        							      CURRENT_PROPERTY_SET_TO_DEFAULT_KEY,
                        							      CURRENT_PROPERTY_SET_TO_DEFAULT,
                        							      _propertyName.getString()) 
                        							      << endl;
                            
                    }

                    if ( _plannedValueSet )
                    {
                    	//l10n
                        //outPrintWriter << "Property '" << _propertyName <<
                       	    //IS_UNSET_IN_FILE << endl;
                       	outPrintWriter << localizeMessage(MSG_PATH,
                       									  PROPERTY_UNSET_IN_FILE_KEY,
                       									  PROPERTY_UNSET_IN_FILE,
                       									  _propertyName.getString())
                       									  << endl;
                    }
                }
                else
                {
                    if (_currentValueSet)
                    {
                    	//l10n
                        //outPrintWriter << CURRENT_VALUE_OF_PROPERTY <<
                            //_propertyName <<"' can not be unset " <<
                            //"because the CIM server is not running." << endl;
                        outPrintWriter << localizeMessage(MSG_PATH,
                        								  CURRENT_PROPERTY_CANNOT_BE_UNSET_CIM_NOT_RUNNING_KEY,
                        								  CURRENT_PROPERTY_CANNOT_BE_UNSET_CIM_NOT_RUNNING,
                        								  _propertyName.getString())
                        								  << endl;
                        

                        return ( RC_ERROR );
                    }

                    if ( !_configFileHandler->updatePlannedValue( 
                        _propertyName, _propertyValue, true ) )
                    {
                        return ( RC_ERROR );
                    }
			
					//l10n
                    //outPrintWriter << "Property '" << _propertyName <<
                        //IS_UNSET_IN_FILE << endl;
                    outPrintWriter << localizeMessage(MSG_PATH,
                       								  PROPERTY_UNSET_IN_FILE_KEY,
                       								  PROPERTY_UNSET_IN_FILE,
                       								  _propertyName.getString())
                       								  << endl;
                }

            }
            catch (const CIMException& e)
            {
                CIMStatusCode code = e.getCode();

                if (code == CIM_ERR_TYPE_MISMATCH)
                {
                	//l10n
                    //outPrintWriter << INVALID_PROPERTY_VALUE << endl;
					outPrintWriter << localizeMessage(MSG_PATH,
													  INVALID_PROPERTY_VALUE_KEY,
													  INVALID_PROPERTY_VALUE)
													  << endl;

                    errPrintWriter << e.getMessage() << endl;
                }
                else if (code == CIM_ERR_NOT_FOUND)
                {
                	//l10n
                    //outPrintWriter << PROPERTY_NOT_FOUND << endl;
					outPrintWriter << localizeMessage(MSG_PATH,
													  PROPERTY_NOT_FOUND_KEY,
													  PROPERTY_NOT_FOUND)
													  << endl;

                    errPrintWriter << e.getMessage() << endl;
                }
                else if (code == CIM_ERR_NOT_SUPPORTED)
                {
                	//l10n
                    //outPrintWriter << PROPERTY_NOT_MODIFIED << endl;
                    outPrintWriter << localizeMessage(MSG_PATH,
													  PROPERTY_NOT_MODIFIED_KEY,
													  PROPERTY_NOT_MODIFIED)
													  << endl;

                    errPrintWriter << e.getMessage() << endl;
                }
                else if (code == CIM_ERR_FAILED)
                {
                	//l10n
                    //outPrintWriter << FAILED_TO_UNSET_PROPERTY <<
                        //e.getMessage() << endl;
                    outPrintWriter << localizeMessage(MSG_PATH,
													  FAILED_TO_UNSET_PROPERTY_KEY,
													  FAILED_TO_UNSET_PROPERTY)
													  << e.getMessage() << endl;
                }
                else if (code == CIM_ERR_ALREADY_EXISTS)
                {
                	//l10n
                    //outPrintWriter << "The property '" << _propertyName <<
                        //"' value is already unset." << endl;
                    outPrintWriter << localizeMessage(MSG_PATH,
                    								  PROPERTY_VALUE_ALREADY_UNSET_KEY,
                    								  PROPERTY_VALUE_ALREADY_UNSET,
                    								  _propertyName.getString())
                    								  << endl;

                    errPrintWriter << e.getMessage() << endl;
                }
                else if (code == CIM_ERR_INVALID_CLASS)
                {
                	//l10n
                    //outPrintWriter << FAILED_TO_UNSET_PROPERTY << endl <<
                        //CONFIG_SCHEMA_NOT_LOADED << endl;
                    outPrintWriter << localizeMessage(MSG_PATH,
                    								  FAILED_TO_UNSET_PROPERTY_KEY,
                    								  FAILED_TO_UNSET_PROPERTY)
                    								  << endl <<
                        		      localizeMessage(MSG_PATH,
                        		      				  CONFIG_SCHEMA_NOT_LOADED_KEY,
                        		      				  CONFIG_SCHEMA_NOT_LOADED)
                        		      				  << endl;
                }
                else
                {
                    errPrintWriter << e.getMessage() << endl;
                }
                return ( RC_ERROR );
            }
            catch (const Exception& e)
            {
            	//l10n
                //outPrintWriter << FAILED_TO_UNSET_PROPERTY << endl <<
                    //e.getMessage() << endl;
                outPrintWriter << localizeMessage(MSG_PATH,
                    							  FAILED_TO_UNSET_PROPERTY_KEY,
                    							  FAILED_TO_UNSET_PROPERTY)
                    							  << endl << e.getMessage() << endl;
                return ( RC_ERROR );
            }
            break;

        case OPERATION_TYPE_LIST:
            //
            // send request to CIMOM if running, else send to config file
            //
            try
            {
                Array<CIMName> propertyNames;
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
                    	//l10n
                        //outPrintWriter << "Current value of properties can not be " <<
                            //"listed because the CIM server is not running." << endl;
                        outPrintWriter << localizeMessage(MSG_PATH,
                        								  CURRENT_VALUES_CANNOT_BE_LISTED_CIM_NOT_RUNNING_KEY,
                        								  CURRENT_VALUES_CANNOT_BE_LISTED_CIM_NOT_RUNNING)
                        								  << endl;
                        break;
                    }
                }

                Uint32 valuesSize = propertyValues.size();
                Uint32 namesSize  = propertyNames.size();

                if (namesSize == 0)
                {
                	//l10n
                    //outPrintWriter << "No configuration properties found"
                        //<< " in the configuration file." << endl;
                    outPrintWriter << localizeMessage(MSG_PATH,
                    								  NO_PROPERTIES_FOUND_IN_FILE_KEY,
                    								  NO_PROPERTIES_FOUND_IN_FILE)
                    								  << endl;
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
            catch (const CIMException& e)
            {
                CIMStatusCode code = e.getCode();

                if (code == CIM_ERR_NOT_FOUND || code == CIM_ERR_INVALID_CLASS)
                {
                	//l10n
                    //outPrintWriter << FAILED_TO_LIST_PROPERTIES << endl <<
                        //CONFIG_SCHEMA_NOT_LOADED << endl;
                    outPrintWriter << localizeMessage(MSG_PATH,
                    								  FAILED_TO_LIST_PROPERTIES_KEY,
                    								  FAILED_TO_LIST_PROPERTIES)
                    								  << endl <<
                        			  localizeMessage(MSG_PATH,
                        			  				  CONFIG_SCHEMA_NOT_LOADED_KEY,
                        			  				  CONFIG_SCHEMA_NOT_LOADED)
                        			  				  << endl;
                }
                else if (code == CIM_ERR_FAILED)
                {
                	//l10n
                    //outPrintWriter << FAILED_TO_LIST_PROPERTIES <<
                        //e.getMessage() << endl;
                    outPrintWriter << localizeMessage(MSG_PATH,
                    								  FAILED_TO_LIST_PROPERTIES_KEY,
                    								  FAILED_TO_LIST_PROPERTIES)
                    								  << e.getMessage() << endl;
                }
                else
                {
                    errPrintWriter << e.getMessage() << endl;
                }

                return ( RC_ERROR );
            }
            catch (const Exception& e)
            {
            	//l10n
                //outPrintWriter << FAILED_TO_LIST_PROPERTIES <<  endl <<
                    //e.getMessage() << endl;
                outPrintWriter << localizeMessage(MSG_PATH,
                    								  FAILED_TO_LIST_PROPERTIES_KEY,
                    								  FAILED_TO_LIST_PROPERTIES)
                    								  << endl << e.getMessage() << endl;
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
    const CIMName&    propName,
    Array <String>&    propValues
    ) 
{
    CIMProperty prop;

        Array<CIMKeyBinding> kbArray;
        CIMKeyBinding        kb;

        kb.setName(PROPERTY_NAME);
        kb.setValue(propName.getString());
        kb.setType(CIMKeyBinding::STRING);

        kbArray.append(kb);

        CIMObjectPath reference(
            _hostName, PEGASUS_NAMESPACENAME_CONFIG,
            PEGASUS_CLASSNAME_CONFIGSETTING, kbArray);

        CIMInstance cimInstance =
            _client->getInstance(PEGASUS_NAMESPACENAME_CONFIG, reference);

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

/**
    Send an updated property value to the CIM Server.
 */
void CIMConfigCommand::_updatePropertyInCIMServer
    (
    ostream&    outPrintWriter, 
    ostream&    errPrintWriter,
    const CIMName&   propName,
    const String&   propValue,
    Boolean     isUnsetOperation
    ) 
{

        Array<CIMKeyBinding> kbArray;
        CIMKeyBinding        kb;

        kb.setName(PROPERTY_NAME);
        kb.setValue(propName.getString());
        kb.setType(CIMKeyBinding::STRING);

        kbArray.append(kb);

        CIMObjectPath reference(
            _hostName, PEGASUS_NAMESPACENAME_CONFIG,
            PEGASUS_CLASSNAME_CONFIGSETTING, kbArray);

        CIMInstance modifiedInst = CIMInstance(PEGASUS_CLASSNAME_CONFIGSETTING);
        Array<CIMName> propertyList;

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

        CIMInstance namedInstance (modifiedInst);
        namedInstance.setPath (reference);
        _client->modifyInstance(
            PEGASUS_NAMESPACENAME_CONFIG,
            namedInstance,
            false,
            CIMPropertyList(propertyList));
}


/**
    get a list of all property names and value from the CIM Server.
 */
void CIMConfigCommand::_listAllPropertiesInCIMServer
    ( 
    ostream&    outPrintWriter,
    ostream&    errPrintWriter,
    Array <CIMName>&   propNames,
    Array <String>&   propValues
    )
{
    Array<CIMInstance> configNamedInstances;

        if ( _currentValueSet ||  _plannedValueSet )
        {
            //
            // get all the instances of class PG_ConfigSetting
            //
            configNamedInstances =
                _client->enumerateInstances(
                    PEGASUS_NAMESPACENAME_CONFIG,
                    PEGASUS_CLASSNAME_CONFIGSETTING);

            //
            // copy all the property names and values
            //
            for (Uint32 i = 0; i < configNamedInstances.size(); i++)
            {
                CIMInstance& configInstance =
                    configNamedInstances[i];

                Uint32 pos = configInstance.findProperty
                    (CIMName ("PropertyName"));
                CIMProperty prop = (CIMProperty)configInstance.getProperty(pos);
                propNames.append(prop.getValue().toString());

                if (_currentValueSet)
                {
                    //
                    // get current value
                    //
                    pos = configInstance.findProperty(CIMName ("CurrentValue"));
                    prop = (CIMProperty)configInstance.getProperty(pos);
                    propValues.append(prop.getValue().toString());
                }
                else if (_plannedValueSet)
                {
                    //
                    // get planned value
                    //
                    pos = configInstance.findProperty(CIMName ("PlannedValue"));
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
                _client->enumerateInstanceNames(
                    PEGASUS_NAMESPACENAME_CONFIG,
                    PEGASUS_CLASSNAME_CONFIGSETTING);

            //
            // copy all the property names
            //
            for (Uint32 i = 0; i < instanceNames.size(); i++)
            {
                Array<CIMKeyBinding> kbArray = instanceNames[i].getKeyBindings();
  
                if (kbArray.size() > 0)
                {
                    propNames.append(kbArray[0].getValue());
                }
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
    AutoPtr<CIMConfigCommand> command;
    Uint32               returnCode;
    
	MessageLoader::_useProcessLocale = true; //l10n set message loading to process locale
    MessageLoader::setPegasusMsgHomeRelative(argv[0]);
	
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

    // Set the stderr stream to buffered with 32k.
    // Allows utf-8 to be sent to stderr (P9A66750)
    setvbuf(stderr, new char[32768], _IOLBF, 32768);

  // check what environment we are running in, native or qsh
    if( getenv(
#pragma convert(37)
               "SHLVL"
#pragma convert(0)
               ) == NULL )
    {  // native mode
      // Check to ensure the user is authorized to use the command,
      // suppress diagnostic message
      if(FALSE == ycmCheckCmdAuthorities(1)){
        exit(CPFDF80_RC);
      }
    }
    else
    { // qsh mode
      // Check to ensure the user is authorized to use the command
      // ycmCheckCmdAuthorities() will send a diagnostic message to qsh
      if(FALSE == ycmCheckCmdAuthorities()){
        exit(CPFDF80_RC);
      }
    }
#endif

    command.reset(new CIMConfigCommand ());

    try 
    {
        command->setCommand (argc, argv);
    } 
    catch (const CommandFormatException& cfe) 
    {
        String msg(cfe.getMessage());
        cerr << COMMAND_NAME << ": " << msg <<  endl;

        if (msg.find(String("Unknown flag")) != PEG_NOT_FOUND)
         {
           MessageLoaderParms parms(ERR_OPTION_NOT_SUPPORTED_KEY,ERR_OPTION_NOT_SUPPORTED);
              parms.msg_src_path = MSG_PATH;
           cerr << COMMAND_NAME <<
             ": " << MessageLoader::getMessage(parms) << endl;
         }
        else
         {
           MessageLoaderParms parms(ERR_USAGE_KEY,ERR_USAGE);
              parms.msg_src_path = MSG_PATH;
           cerr << COMMAND_NAME <<
             ": " << MessageLoader::getMessage(parms) << endl;
         }

        exit (Command::RC_ERROR);
    }

    returnCode = command->execute (cout, cerr);
    command.reset();

    exit (returnCode);
    return 0;
}
