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

PEGASUS_NAMESPACE_BEGIN


/** 
MissingCommandLineOptionArgument Exception class 
*/
class MissingCommandLineOptionArgument : public Exception
{
public:
    MissingCommandLineOptionArgument(const String& optionName)
        : Exception("Missing command line option argument: " + optionName) { }
};

/** 
UnrecognizedCommandLineOption Exception class 
*/
class UnrecognizedCommandLineOption : public Exception
{
public:
    //UnrecognizedCommandLineOption(const String& optionName)
    UnrecognizedCommandLineOption()
        : Exception("Unrecognized command line option. ") { }
};


/** 
InvalidPropertyValue Exception class 
*/
class InvalidPropertyValue : public Exception
{
public:
    InvalidPropertyValue(const String& name, const String& value)
        : Exception("Invalid property value: " + name + "=" + value ) { }
};


/** 
DuplicateOption Exception class 
*/
class DuplicateOption : public Exception
{
public:
    DuplicateOption(const String& name)
        : Exception("Duplicate option: " + name) { }
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
    UnrecognizedConfigFileOption(const String& name)
        : Exception("Unrecognized config file option: " + name) { }
};


/** 
MissingRequiredOptionValue Exception class 
*/
class MissingRequiredOptionValue : public Exception
{
public:
    MissingRequiredOptionValue(const String& name)
        : Exception("Missing required option value: " + name) { }
};


/** 
UnrecognizedConfigProperty Exception class 
*/
class UnrecognizedConfigProperty : public Exception
{
public:
    UnrecognizedConfigProperty(const String& name)
        : Exception("Unrecognized config property: " + name) { }
};

/** 
NonDynamicConfigProperty Exception class 
*/
class NonDynamicConfigProperty : public Exception
{
public:
    NonDynamicConfigProperty(const String& name)
        : Exception("NonDynamic config property: " + name) { }
};

PEGASUS_NAMESPACE_END

#endif /* Pegasus_ConfigExceptions_h */
