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
// Author: Sushma Fernandes, Hewlett-Packard Company (sushma_fernandes@hp.com)
//
// Modified By:
//
//%/////////////////////////////////////////////////////////////////////////////

#include <iostream>
#include <Pegasus/Common/Logger.h>
#include <Pegasus/Common/TraceFileHandler.h>

PEGASUS_USING_STD;

PEGASUS_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////
//   Writes message to file. 
//   Implementation of this function is platform specific
// 
//   Note: The current implementation writes the message to the defined file.
//         Will have to be enhanced to support synchronous write operations to
//         the same file.
////////////////////////////////////////////////////////////////////////////////
void TraceFileHandler::handleMessage(
    const char* message,
    const char* fmt,
    va_list argList) 
{
    Uint32 retCode;

    if (_fileHandle)
    {
        //Move to the End of File
        fseek(_fileHandle,0,SEEK_SET);

        // Write message to file
        fprintf(_fileHandle,"%s", message);
        vfprintf(_fileHandle,fmt,argList);
        retCode = fprintf(_fileHandle,"\n");

        if (retCode < 0)
        {
	    // Unable to write message to file
	    // Log message 
	    Logger::put(Logger::DEBUG_LOG,"Tracer",Logger::WARNING,
	        "Unable to write trace message to File $0",_fileName);
        }
	else
	{
            fflush(_fileHandle);
        }
    }
    else
    {
	// Invalid file handle
	// Log message 
	Logger::put(Logger::DEBUG_LOG,"Tracer",Logger::WARNING,
            "Invalid file handle for file $0",_fileName);
    }
} 

PEGASUS_NAMESPACE_END
