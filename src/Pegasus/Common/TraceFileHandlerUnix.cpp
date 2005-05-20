//%2005////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001, 2002 BMC Software; Hewlett-Packard Development
// Company, L.P.; IBM Corp.; The Open Group; Tivoli Systems.
// Copyright (c) 2003 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation, The Open Group.
// Copyright (c) 2004 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation; VERITAS Software Corporation; The Open Group.
// Copyright (c) 2005 Hewlett-Packard Development Company, L.P.; IBM Corp.;
// EMC Corporation; VERITAS Software Corporation; The Open Group.
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
// Modified By: Amit K Arora, IBM (amita@in.ibm.com) for Bug#1527
//              Roger Kumpf, Hewlett-Packard Company (roger_kumpf@hp.com)
//              David Dillard, VERITAS Software Corp.
//                  (david.dillard@veritas.com)
//
//%/////////////////////////////////////////////////////////////////////////////


#include <Pegasus/Common/Logger.h>
#include <Pegasus/Common/System.h>
#include <Pegasus/Common/IPC.h>
#include <Pegasus/Common/TraceFileHandler.h>

PEGASUS_USING_STD;

PEGASUS_NAMESPACE_BEGIN

static Mutex writeMutex;

///////////////////////////////////////////////////////////////////////////////
//  Writes message to file. Locks the file before writing to it
//  Implementation of this function is platform specific
///////////////////////////////////////////////////////////////////////////////
void TraceFileHandler::handleMessage(
    const char* message,
    const char* fmt,
    va_list argList)
{
    if (!_fileHandle)
    {
        // The trace file is not open, which means an earlier fopen() was
        // unsuccessful.  Stop now to avoid logging duplicate error messages.
        return;
    }

    // Do not add Trace calls in the Critical section
    // ---- BEGIN CRITICAL SECTION

    // If the file has been deleted, re-open it and continue
    if (!System::exists(_fileName))
    {
        fclose(_fileHandle);
        _fileHandle = _openFile(_fileName);
        if (!_fileHandle)
        {
            return;
        }
    }

    AutoMutex writeLock(writeMutex);

    // Got the Lock on the File. Seek to the end of File
    fseek(_fileHandle,0,SEEK_END);
#ifdef PEGASUS_PLATFORM_LINUX_GENERIC_GNU
    long pos = ftell(_fileHandle);
    // Check if the file size is approaching 2GB - which is the
    // maximum size a file on 32 bit Linux can grow (ofcourse if
    // not using large-files option). If this is not checked, the
    // cimserver may get a SIGXFSZ signal and shutdown. See Bug#1527.
    if(pos >= 0x7ff00000)
    {
        // If the file size is almost 2 GB in size, close this trace
        // file and open a new trace file which would have _fileCount
        // as the suffix. So, if "cimserver.trc" is the trace file that
        // approaches 2GB, the next file which gets created would be
        // named "cimserver.trc.1" and so on ...
        fclose(_fileHandle);
        sprintf(_fileName,"%s.%u",_baseFileName,++_fileCount);
        _fileHandle = fopen(_fileName,"a+");
        if (!_fileHandle)
        {
            // Unable to open file, log a message
            if (!_wroteToLog)
            {
                Logger::put_l(Logger::DEBUG_LOG,"Tracer",Logger::WARNING,
                        "Common.TraceFileHandler.FAILED_TO_OPEN_FILE",
                        "Failed to open File $0",_fileName);
                _wroteToLog = true;
            }
            return;
        }
    }
#endif

    // Write the message to the file
    fprintf (_fileHandle, "%s",message);
    vfprintf(_fileHandle,fmt,argList);
    fprintf(_fileHandle,"\n");
    fflush(_fileHandle);
}

PEGASUS_NAMESPACE_END
