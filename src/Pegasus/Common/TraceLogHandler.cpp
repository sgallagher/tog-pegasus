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
//%/////////////////////////////////////////////////////////////////////////////

#ifdef PEGASUS_PLATFORM_ZOS_ZSERIES_IBM
#define _ISOC99_SOURCE
#include <stdio.h>
#include <stdarg.h>
#endif

#include <Pegasus/Common/FileSystem.h>
#include <Pegasus/Common/TraceLogHandler.h>

PEGASUS_USING_STD;

PEGASUS_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////
//  Constructs TraceLogHandler
////////////////////////////////////////////////////////////////////////////////
TraceLogHandler::TraceLogHandler()
{
}

////////////////////////////////////////////////////////////////////////////////
//  Destructs TraceLogHandler
////////////////////////////////////////////////////////////////////////////////
TraceLogHandler::~TraceLogHandler()
{
}

////////////////////////////////////////////////////////////////////////////////
//  Sends a trace message with format string to the Logger
////////////////////////////////////////////////////////////////////////////////
void TraceLogHandler::handleMessage(
    const char *message,
    const char *fmt, va_list argList)
{
    if (Logger::wouldLog(Logger::TRACE))
    {
        char buffer[4096];
        
        vsnprintf(buffer, 4095, fmt, argList);
        
        String completeMsg(buffer);
        completeMsg.append(message);
        
        Logger::trace( Logger::TRACE_LOG, 
                       System::CIMSERVER, 
                       Logger::TRACE,
                       completeMsg );
    }    
}

////////////////////////////////////////////////////////////////////////////////
//  Sends a simple trace message to the Logger
////////////////////////////////////////////////////////////////////////////////
void TraceLogHandler::handleMessage(const char *message)
{
    if (Logger::wouldLog(Logger::TRACE))
    {
        Logger::trace( Logger::TRACE_LOG, System::CIMSERVER, Logger::TRACE,
                       String(message) );
    }    
}
PEGASUS_NAMESPACE_END
