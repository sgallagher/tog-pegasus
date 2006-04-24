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
#include <Pegasus/Common/Exception.h>
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
  
    Exception identifier indicating "Syntax Error" with client option.
  
 */

const Uint32 StressTestControllerException::INVALID_OPTION = 1;

/**
  
    Exception identifier indicating "Syntax Error" with client option operator.
  
 */
const Uint32 StressTestControllerException::INVALID_OPERATOR = 2;

/**
  
    Exception identifier indicating "Missing closing square brace".
  
 */
const Uint32 StressTestControllerException::MISSING_BRACE = 3;

/**
  
    Exception identifier indicating "Missing value for client option".
  
 */
const Uint32 StressTestControllerException::MISSING_VALUE = 4;

/**
  
    Maximum valid exception identifier.  This value must be updated when
    a new exception identifier and message are added.
  
 */
const Uint32 StressTestControllerException::MAX_ID = 
    StressTestControllerException::MISSING_VALUE;

/**

    Exception message strings.  The exception identifier is used as an
    index into this array to retrieve the appropriate exception message
    string.  When a new identifier is added, this array must be updated
    appropriately.

 */
const char*  StressTestControllerException::_messageStrings [] =
{
    "Error in TestStressTestController command: ",
    "Syntax Error Client option name: ",
    "Syntax Error Client option operator: ",
    "Missing closing square brace: ",
    "Missing value for client option: "
};

/**
  
    Constructs a StressTestControllerException with a message corresponding to
    the specified exception ID.
  
    @param  ID                the integer exception identifier
  
 */
StressTestControllerException::StressTestControllerException (
    Uint32 ID) : CommandException 
                     (_messageStrings [(ID > MAX_ID) ? DEFAULT_ID : ID])
{
}

/**
  
    Constructs a StressTestControllerException with the specified message.
  
    @param  exceptionMessage  a string containing the exception message
  
 */
StressTestControllerException::StressTestControllerException (
    const String& exceptionMessage) : CommandException (exceptionMessage)
{
}

PEGASUS_NAMESPACE_END
