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


#include <fcntl.h>
#include <Pegasus/Common/Logger.h>
#include <Pegasus/Common/System.h>
#include <Pegasus/Common/TraceFileHandler.h>

PEGASUS_USING_STD;

PEGASUS_NAMESPACE_BEGIN

/////////////////////////////////////////////////////////////////////////////// 
//  Writes message to file. Locks the file before writing to it
//  Implementation of this function is platform specific
///////////////////////////////////////////////////////////////////////////////
void TraceFileHandler::handleMessage(
    const char* message,
    const char* fmt,
    va_list argList) 
{
    struct flock lock;
    Sint32 retCode;

    lock.l_type = F_WRLCK;
    lock.l_whence = SEEK_SET;
    lock.l_start = 0;
    lock.l_len = 1;

    // Try to lock the file. If the lock is busy, wait for it to be free.
    // Do not add Trace calls in the Critical section
    // ---- BEGIN CRITICAL SECTION

    // Check if the file has been deleted, if so re-open the file and continue
    if (!System::exists(_fileName))
    {
	freopen(_fileName,"a+",_fileHandle);
        if (!_fileHandle)
        {
            // Unable to re-open file, log a message
            Logger::put(Logger::DEBUG_LOG,"Tracer",Logger::WARNING,
                "Failed to open File $0",_fileName);
            return; 
        } 	
    }
    retCode = fcntl(fileno(_fileHandle), F_SETLKW, &lock);

    if (retCode != -1)
    {
	// Got the Lock on the File. Seek to the end of File
	fseek(_fileHandle,0,SEEK_END);

	// Write the message to the file
        fprintf (_fileHandle, "%s",message);
	retCode = vfprintf(_fileHandle,fmt,argList);
	fprintf(_fileHandle,"\n");
	fflush(_fileHandle);
	
	// Free the Lock on the File
        lock.l_type = F_UNLCK;
        retCode = fcntl(fileno(_fileHandle), F_SETLK, &lock);

	if (retCode == -1)
	{
	    // Failed to release lock on file. Log message and return
	    Logger::put(Logger::DEBUG_LOG,"Tracer",Logger::WARNING,
		"Failed to release write lock on File $0",_fileName);
        }
	_wroteToLog = false;
    }
    else
    {
	// Unable to Lock the file for writing
	// Log message and return
	// Check if message is already logged, so as to not to flood log file
	if (!_wroteToLog)
	{
	    Logger::put(Logger::DEBUG_LOG,"Tracer",Logger::WARNING,
		"Failed to obtain write lock on File $0",_fileName);
            _wroteToLog = true;
	}
    }
    // ---- END CRITICAL SECTION

    return;
} 

PEGASUS_NAMESPACE_END
