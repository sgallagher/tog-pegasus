//%LICENSE////////////////////////////////////////////////////////////////
//
// Licensed to The Open Group (TOG) under one or more contributor license
// agreements.  Refer to the OpenPegasusNOTICE.txt file distributed with
// this work for additional information regarding copyright ownership.
// Each contributor licenses this file to you under the OpenPegasus Open
// Source License; you may not use this file except in compliance with the
// License.
//
// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation
// the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included
// in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
// OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
// IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
// CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
// TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
// SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
//
//////////////////////////////////////////////////////////////////////////
//
//%/////////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include "ConfigExceptions.h"

PEGASUS_NAMESPACE_BEGIN

/**
    MissingCommandLineOptionArgument Exception class
*/
MissingCommandLineOptionArgument::MissingCommandLineOptionArgument(
    const String& optionName):Exception(
        MessageLoaderParms(
            "Config.ConfigExceptions.MISSING_CMDLINE_OPTION",
            "Missing command line option argument: $0",
            optionName))
{
}

/**
    UnrecognizedCommandLineOption Exception class
*/
UnrecognizedCommandLineOption::UnrecognizedCommandLineOption()
    :Exception(
        MessageLoaderParms(
            "Config.ConfigExceptions.UNRECOGNIZED_CMDLINE_OPTION",
            "Unrecognized command line option. "))
{
}


/**
    InvalidPropertyValue Exception class
*/
InvalidPropertyValue::InvalidPropertyValue(
    const String& name,
    const String& value)
        :Exception(
            MessageLoaderParms(
                "Config.ConfigExceptions.INVALID_PROPERTY_VALUE",
                "Invalid property value: $0=$1",
                name,
                value))
{
}

InvalidPropertyValue::InvalidPropertyValue(const MessageLoaderParms& theMessage)
         :Exception(theMessage)
{
}


/**
    InvalidDirectoryPropertyValue Exception class
*/
InvalidDirectoryPropertyValue::InvalidDirectoryPropertyValue(
    const String& name,
    const String& value)
        :InvalidPropertyValue(
            MessageLoaderParms(
                "Config.ConfigExceptions.INVALID_DIRECTORY_PROPERTY_VALUE",
                "For property $0 specified value $1 is not a directory or "
                    "the directory is not writeable.",
                name,
                value))
{
}

/**
    InvalidListenAddressPropertyValue Exception class
*/
InvalidListenAddressPropertyValue::InvalidListenAddressPropertyValue(
    const String& name,
    const String& value)
        :InvalidPropertyValue(
            MessageLoaderParms(
                "Config.ConfigExceptions.INVALID_LISTENADDRESS_PROPERTY_VALUE",
                "For property $0 specified value $1 is not a valid interface "
                    "address.",
                name,
                value))
{
}

/**
    DuplicateOption Exception class
*/
DuplicateOption::DuplicateOption(const String& name)
    :Exception(
        MessageLoaderParms(
            "Config.ConfigExceptions.DUPLICATE_OPTION",
            "Duplicate option: $0",
            name))
{
}


ConfigFileSyntaxError::ConfigFileSyntaxError(const String& file, Uint32 line)
    :Exception(_formatMessage(file, line))
{
}


String ConfigFileSyntaxError::_formatMessage(
    const String& file,
    Uint32 line)
{
    char buffer[32];
    sprintf(buffer, "%u", line);

    MessageLoaderParms parms(
        "Config.ConfigExceptions.CONFIG_FILE_SYNTAX_ERR",
        "Syntax error in configuration file: ");
    String result = MessageLoader::getMessage(parms);
    result.append(file);
    result.append("(");
    result.append(buffer);
    result.append(")");
    return result;
}


/**
    UnrecognizedConfigFileOption Exception class
*/
UnrecognizedConfigFileOption::UnrecognizedConfigFileOption(const String& name)
    :Exception(
        MessageLoaderParms(
            "Config.ConfigExceptions.UNRECOGNIZED_CONFIG_FILE_OPTION",
            "Unrecognized config file option: $0",
            name))
{
}


/**
    MissingRequiredOptionValue Exception class
*/
MissingRequiredOptionValue::MissingRequiredOptionValue(const String& name)
    :Exception(
        MessageLoaderParms(
            "Config.ConfigExceptions.MISSING_REQUIRED_OPTION",
            "Missing required option value: $0",
            name))
{
}


/**
    UnrecognizedConfigProperty Exception class
*/
UnrecognizedConfigProperty::UnrecognizedConfigProperty(const String& name)
    :Exception(
        MessageLoaderParms(
            "Config.ConfigExceptions.UNRECOGNIZED_CONFIG_PROPERTY",
            "Unrecognized config property: $0",
            name))
{
}

/**
    NonDynamicConfigProperty Exception class
*/
NonDynamicConfigProperty::NonDynamicConfigProperty(const String& name)
    :Exception(
        MessageLoaderParms(
            "Config.ConfigExceptions.NONDYNAMIC_CONFIG_PROPERTY",
            "NonDynamic config property: $0",
            name))
{
}

/**
    FailedSaveProperties Exception class
*/
FailedSaveProperties::FailedSaveProperties(const String& reason)
    :Exception(
        MessageLoaderParms(
            "Config.ConfigExceptions.FAILED_SAVE_PROPERTIES",
            "Failed to save configuration properties to file: $0. "
                "Configuration property not set.",
            reason))
{
}

PEGASUS_NAMESPACE_END
