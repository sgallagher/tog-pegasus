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
//
//%/////////////////////////////////////////////////////////////////////////////

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/ExceptionRep.h>
#include "StressTestControllerException.h"

PEGASUS_NAMESPACE_BEGIN


/**
  
    Default exception identifier.  This identifier is used if the specified
    identifier is out of range.  This identifier corresponds to a default
    (generic) message in the array of exception message strings.
  
 */
const Uint32 StressTestControllerException::DEFAULT_ID = 0;

/**
  
    Minimum valid exception identifier.
  
 */
const Uint32 StressTestControllerException::MIN_ID = DEFAULT_ID;

/**
  
   Exception identifier indicating a connection failure.
  
 */

const Uint32 StressTestControllerException::CONNECT_FAIL = 1;

/**
  
    Exception identifier indicating timed out waiting for response.
  
 */
const Uint32 StressTestControllerException::TIMED_OUT = 2;

/**
  
    Exception identifier indicating invalid input.
  
 */
const Uint32 StressTestControllerException::INVALID_INPUT = 3;

/**
  
    Maximum valid exception identifier.  This value must be updated when
    a new exception identifier and message are added.
  
 */
const Uint32 StressTestControllerException::MAX_ID = StressTestControllerException::INVALID_INPUT;

/**

    Exception message strings.  The exception identifier is used as an
    index into this array to retrieve the appropriate exception message
    string.  When a new identifier is added, this array must be updated
    appropriately.

 */
const char*  StressTestControllerException::_messageStrings [] =
{
    "Error in stressTestController command ",
    "Failed to connect to CIM server: ",
    "Timed out waiting for response ",
    "Invalid input"
};

/**
  
    Constructs a StressTestControllerException with a message corresponding to the
    specified exception ID.
  
    @param  ID                the integer exception identifier
  
 */
StressTestControllerException::StressTestControllerException (Uint32 ID) : CommandException 
    (_messageStrings [(ID > MAX_ID) ? DEFAULT_ID : ID])
{
}

/**
  
    Constructs a StressTestControllerException with a message corresponding to the
    specified ID, appended with the specified String.
  
    @param  ID                the integer exception identifier
    @param  appendString      the string to append to the exception message
  
 */
StressTestControllerException::StressTestControllerException (Uint32 ID, const String& appendString) : 
    CommandException (_messageStrings 
        [(ID > MAX_ID) ? DEFAULT_ID : ID])
{
    _rep->message.append (appendString);
}

/**
  
    Constructs a StressTestControllerException with the specified message.
  
    @param  exceptionMessage  a string containing the exception message
  
 */
StressTestControllerException::StressTestControllerException (const String& exceptionMessage) : 
    CommandException (exceptionMessage)
{
}

String ConfigFileSyntaxError::_formatMessage(
    const String& file, Uint32 line)
{
    char buffer[32];
    sprintf(buffer, "%d", line);

//l10n
    //String result = "Syntax error in configuration file: ";
    MessageLoaderParms parms("Config.ConfigExceptions.CONFIG_FILE_SYNTAX_ERR","Syntax error in configuration file: ");
    String result = MessageLoader::getMessage(parms);
//l10n end
    result.append(file);
    result.append("( Line number# ");
    result.append(buffer);
    result.append(")");
    return result;
}

PEGASUS_NAMESPACE_END
