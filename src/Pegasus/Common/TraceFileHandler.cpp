//%2003////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001, 2002  BMC Software, Hewlett-Packard Development
// Company, L. P., IBM Corp., The Open Group, Tivoli Systems.
// Copyright (c) 2003 BMC Software; Hewlett-Packard Development Company, L. P.;
// IBM Corp.; EMC Corporation, The Open Group.
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
// Modified By: Rudy Schuet (rudy.schuet@compaq.com) 11/12/01
//              added nsk platform support
//
//%/////////////////////////////////////////////////////////////////////////////

#include <fstream>
#include <iostream>
#include <Pegasus/Common/FileSystem.h>
#include <Pegasus/Common/TraceFileHandler.h>

#if defined(PEGASUS_OS_TYPE_WINDOWS)
# include <Pegasus/Common/TraceFileHandlerWindows.cpp>
#elif defined(PEGASUS_OS_TYPE_UNIX)
# include <Pegasus/Common/TraceFileHandlerUnix.cpp>
#elif defined(PEGASUS_OS_TYPE_NSK)
# include <Pegasus/Common/TraceFileHandlerNsk.cpp>
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
    _fileName = 0;
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
    if (_fileName)
    {
        delete []_fileName;
    }
}

////////////////////////////////////////////////////////////////////////////////
//  Sets the filename to the given filename and opens the file in append
//  mode
////////////////////////////////////////////////////////////////////////////////

Uint32 TraceFileHandler::setFileName(const char* fileName)
{
    if (!isValidFilePath(fileName))
    {
	return 1;
    }
    // Check if a file is already open, if so close it
    if (_fileHandle)
    {
        fclose(_fileHandle);
    }

    // Now open the file
    _fileHandle = fopen(fileName,"a+");
    if (!_fileHandle)
    {
        // Unable to open file, log a message
        //l10n
        //Logger::put(Logger::DEBUG_LOG,"Tracer",Logger::WARNING,
           //"Failed to open File $0",fileName);
        Logger::put_l(Logger::DEBUG_LOG,"Tracer",Logger::WARNING,
           "Common.TraceFileHandler.FAILED_TO_OPEN_FILE",
           "Failed to open File $0",fileName);
            return 1;
    }
    else
    {
        if (_fileName)
        {
            delete [] _fileName;
        }
        _fileName = new char [strlen(fileName)+1];
        strcpy (_fileName,fileName);
    }
    return 0;
}

Boolean TraceFileHandler::isValidFilePath(const char* filePath)
{
    String fileName = String(filePath);

    // Check if the file path is a directory
    FileSystem::translateSlashes(fileName);
    if (FileSystem::isDirectory(fileName))
    {
	return 0;
    }

    // Check if the file exists and is writable
    if (FileSystem::exists(fileName))
    {
        if (!FileSystem::canWrite(fileName))
        {
	    return 0;
        }
	else
	{
	    return 1;
        }
    }
    else
    {
        // Check if directory is writable
        Uint32 index = fileName.reverseFind('/');

        if (index != PEG_NOT_FOUND)
	{
            String dirName = fileName.subString(0,index);
            if (!FileSystem::isDirectory(dirName))
            {
	        return 0;
            }
            if (!FileSystem::canWrite(dirName) )
            {
		return 0;
            }
	    else
	    {
		return 1;
            }
        }
	else
        {
            String currentDir;

            // Check if there is permission to write in the
            // current working directory
            FileSystem::getCurrentDirectory(currentDir);

            if (!FileSystem::canWrite(currentDir))
            {
		return 0;
            }
	    else
	    {
		return 1;
            }
        }
    }
    return 1;
}
PEGASUS_NAMESPACE_END
