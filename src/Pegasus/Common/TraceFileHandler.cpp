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
// Author: Sushma Fernandes, Hewlett-Packard Company (sushma_fernandes@hp.com)
//
// Modified By:
//
//%/////////////////////////////////////////////////////////////////////////////

#include <fstream>
#include <iostream>
#include <Pegasus/Common/TraceFileHandler.h>

#if defined(PEGASUS_OS_TYPE_WINDOWS)
# include <Pegasus/Common/TraceFileHandlerWindows.cpp>
#elif defined(PEGASUS_OS_TYPE_UNIX)
# include <Pegasus/Common/TraceFileHandlerUnix.cpp>
#else
# error "Unsupported platform"
#endif


PEGASUS_USING_STD;

PEGASUS_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////
//  Constructs TraceFileHandler
////////////////////////////////////////////////////////////////////////////////

TraceFileHandler::TraceFileHandler () 
{
    _fileHandle = 0;
    _wroteToLog = false;
}

////////////////////////////////////////////////////////////////////////////////
//  Destructs TraceFileHandler
////////////////////////////////////////////////////////////////////////////////

TraceFileHandler::~TraceFileHandler () 
{
    // Close the File 
    if (_fileHandle)
    {
        fclose(_fileHandle);
    }
    delete []_fileName;
}

////////////////////////////////////////////////////////////////////////////////
//  Sets the filename to the given filename and opens the file in append
//  mode
////////////////////////////////////////////////////////////////////////////////

Uint32 TraceFileHandler::setFileName(const char* fileName)
{

    Uint32 retCode;

    // Check if a file is already open, if so close it
    if (_fileHandle)
    {
	fclose(_fileHandle);
    }

    // Check if the file can be opened
    _fileHandle = fopen(fileName,"a+");
    if (!_fileHandle)
    {
	// Unable to open file, log a message
	Logger::put(Logger::DEBUG_LOG,"Tracer",Logger::WARNING,
	    "Failed to open File $0",fileName);
	retCode = 1;
    }
    else
    {
	// File is valid, set the file name
	_fileName = new char [strlen(fileName)+1];
        strcpy (_fileName,fileName);
	retCode = 0;
    }
    return retCode;
}

PEGASUS_NAMESPACE_END
