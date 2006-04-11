//%2006////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001, 2002 BMC Software; Hewlett-Packard Development
// Company, L.P.; IBM Corp.; The Open Group; Tivoli Systems.
// Copyright (c) 2003 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation, The Open Group.
// Copyright (c) 2004 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation; VERITAS Software Corporation; The Open Group.
// Copyright (c) 2005 Hewlett-Packard Development Company, L.P.; IBM Corp.;
// EMC Corporation; VERITAS Software Corporation; The Open Group.
// Copyright (c) 2006 Hewlett-Packard Development Company, L.P.; IBM Corp.;
// EMC Corporation; Symantec Corporation; The Open Group.
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
// Author: John Alex, IBM         (johnalex@us.ibm.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_StressTestControllerException_h
#define Pegasus_StressTestControllerException_h

#include <Clients/cliutils/CommandException.h>

PEGASUS_NAMESPACE_BEGIN

/** 
  
    StressTestControllerException signals that an exception has occurred in executing the 
    osinfo command.  
  
    @author  Hewlett-Packard Company
  
 */
class StressTestControllerException : public CommandException 
{
public:
    /**
      
        Constructs an StressTestControllerException with a message corresponding to the
        specified exception ID.
      
        @param  ID                the integer exception identifier
      
     */
    StressTestControllerException (Uint32 ID);

    /**
      
        Constructs an StressTestControllerException with a message corresponding to the
        specified ID, appended with the specified String.
      
        @param  ID                the integer exception identifier
        @param  appendString      the string to append to the exception message
      
     */
    StressTestControllerException (Uint32 ID, const String& appendString);

    /**
      
        Constructs an StressTestControllerException with the specified message.
      
        @param  exceptionMessage  a string containing the exception message
      
     */
    StressTestControllerException (const String& exceptionMessage);

    //
    //  Exception identifiers
    //
    //  These are used to index into the messageStrings array to retrieve the 
    //  appropriate exception message string.  When a new identifier is added, 
    //  the messageStrings array and MAX_ID must be updated appropriately.
    //
    
    /**
      
        Default exception identifier.  This identifier is used if the specified
        identifier is out of range.  This identifier corresponds to a default
        (generic) message in the array of exception message strings.
      
     */
    static const Uint32 DEFAULT_ID;
    
    /**
      
        Minimum valid exception identifier.
      
     */
    static const Uint32 MIN_ID;

    /**
      
        Exception identifier indicating a connection failure.
      
     */
    
    static const Uint32 CONNECT_FAIL;
    
    /**
      
        Exception identifier indicating timed out waiting for response.
      
     */
    static const Uint32 TIMED_OUT;
    
    /**
      
        Exception identifier indicating invalid input.
      
     */
    static const Uint32 INVALID_INPUT;
    
    /**

        Maximum valid exception identifier.  This value must be updated when
        a new exception identifier and message are added.
      
     */
    static const Uint32 MAX_ID;

private:
    /**
      
        Exception message strings.  The exception identifier is used as an
        index into this array to retrieve the appropriate exception message 
        string.  When a new identifier is added, this array must be updated
        appropriately.
      
     */
    static const char*  _messageStrings [];
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
InvalidPropertyValue Exception class
*/
class InvalidPropertyValue : public Exception
{
public:
    InvalidPropertyValue(const String& name, const String& value)
        : Exception(MessageLoaderParms("Config.ConfigExceptions.INVALID_PROPERTY_VALUE",
                                                        "Invalid property value: $0=$1",
                                                        name,
                                                        value )) { }
};

/**
InvalidClientPropertyValue Exception class
*/
class InvalidClientPropertyValue : public Exception
{
public:
    InvalidClientPropertyValue(const String& name, const String& value)
        : Exception(MessageLoaderParms("Config.ConfigExceptions.INVALID_PROPERTY_VALUE",
                                                        "Invalid client property value: $0=$1",
                                                        name,
                                                        value )) { }
};

/**
DuplicateOption Exception class
*/
class DuplicateOption : public Exception
{
public:
    DuplicateOption(const String& name)
        : Exception(MessageLoaderParms("Config.ConfigExceptions.DUPLICATE_OPTION",
                                                               name)) { }
};

PEGASUS_NAMESPACE_END

#endif  /* Pegasus_StressTestControllerException_h */
