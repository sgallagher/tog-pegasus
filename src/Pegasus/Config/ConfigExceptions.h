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
// Author: Nag Boranna (nagaraja_boranna@hp.com)
//
// Modified By:
//
//%/////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
//  This file contains the exception classes used in the configuration
//  classes.
////////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_ConfigExceptions_h
#define Pegasus_ConfigExceptions_h

#include <Pegasus/Common/Exception.h>
#include <Pegasus/Config/Linkage.h>
#include <Pegasus/Common/MessageLoader.h> //l10n

PEGASUS_NAMESPACE_BEGIN


/** 
MissingCommandLineOptionArgument Exception class 
*/
class MissingCommandLineOptionArgument : public Exception
{
public:
//l10n
    //MissingCommandLineOptionArgument(const String& optionName)
        //: Exception("Missing command line option argument: " + optionName) { }
    MissingCommandLineOptionArgument(const String& optionName)
        : Exception(MessageLoaderParms("Config.ConfigExceptions.MISSING_CMDLINE_OPTION",
        				"Missing command line option argument: $0",
        				optionName)) { }
};

/** 
UnrecognizedCommandLineOption Exception class 
*/
class UnrecognizedCommandLineOption : public Exception
{
public:
    //UnrecognizedCommandLineOption(const String& optionName)
    //l10n
    //UnrecognizedCommandLineOption()
        //: Exception("Unrecognized command line option. ") { }
    UnrecognizedCommandLineOption()
        : Exception(MessageLoaderParms("Config.ConfigExceptions.UNRECOGNIZED_CMDLINE_OPTION",
        				"Unrecognized command line option. ")) { }
};


/** 
InvalidPropertyValue Exception class 
*/
class InvalidPropertyValue : public Exception
{
public:
//l10n
    //InvalidPropertyValue(const String& name, const String& value)
        //: Exception("Invalid property value: " + name + "=" + value ) { }
    InvalidPropertyValue(const String& name, const String& value)
        : Exception(MessageLoaderParms("Config.ConfigExceptions.INVALID_PROPERTY_VALUE",
        						"Invalid property value: $0=$1",
        						name,
        						value )) { }
};


/** 
DuplicateOption Exception class 
*/
class DuplicateOption : public Exception
{
public:
//l10n
    //DuplicateOption(const String& name)
        //: Exception("Duplicate option: " + name) { }
    DuplicateOption(const String& name)
        : Exception(MessageLoaderParms("Config.ConfigExceptions.DUPLICATE_OPTION",
        						       "Duplicate option: $0",
        						       name)) { }
};


/** 
ConfigFileSyntaxError Exception class 
*/
class ConfigFileSyntaxError : public Exception
{
public:
    ConfigFileSyntaxError(const String& file, Uint32 line)
        : Exception(_formatMessage(file, line)) { }

    static String _formatMessage(const String& file, Uint32 line);
};


/** 
UnrecognizedConfigFileOption Exception class 
*/
class UnrecognizedConfigFileOption : public Exception
{
public:
//l10n
    //UnrecognizedConfigFileOption(const String& name)
        //: Exception("Unrecognized config file option: " + name) { }
    UnrecognizedConfigFileOption(const String& name)
        : Exception(MessageLoaderParms("Config.ConfigExceptions.UNRECOGNIZED_CONFIG_FILE_OPTION",
        							   "Unrecognized config file option: $0",
        							   name)) { }
};


/** 
MissingRequiredOptionValue Exception class 
*/
class MissingRequiredOptionValue : public Exception
{
public:
//l10n
    //MissingRequiredOptionValue(const String& name)
        //: Exception("Missing required option value: " + name) { }
    MissingRequiredOptionValue(const String& name)
        : Exception(MessageLoaderParms("Config.ConfigExceptions.MISSING_REQUIRED_OPTION",
        							   "Missing required option value: $0",
        							   name)) { }
};


/** 
UnrecognizedConfigProperty Exception class 
*/
class UnrecognizedConfigProperty : public Exception
{
public:
//l10n
    //UnrecognizedConfigProperty(const String& name)
        //: Exception("Unrecognized config property: " + name) { }
    UnrecognizedConfigProperty(const String& name)
        : Exception(MessageLoaderParms("Config.ConfigExceptions.UNRECOGNIZED_CONFIG_PROPERTY",
        							   "Unrecognized config property: $0",
        							   name)) { }
};

/** 
NonDynamicConfigProperty Exception class 
*/
class NonDynamicConfigProperty : public Exception
{
public:
//l10n
    //NonDynamicConfigProperty(const String& name)
        //: Exception("NonDynamic config property: " + name) { }
    NonDynamicConfigProperty(const String& name)
        : Exception(MessageLoaderParms("Config.ConfigExceptions.NONDYNAMIC_CONFIG_PROPERTY",
        							   "NonDynamic config property: $0",
        							   name)) { }
};

PEGASUS_NAMESPACE_END

#endif /* Pegasus_ConfigExceptions_h */
