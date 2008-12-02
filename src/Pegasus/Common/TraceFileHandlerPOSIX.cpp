//%LICENSE////////////////////////////////////////////////////////////////
//
// Licensed to The Open Group (TOG) under one or more contributor license
// agreements.  Refer to the OpenPegasusNOTICE.txt file distributed with
// this work for additional information regarding copyright ownership.
// Each contributor licenses this file to you under the OpenPegasus Open
// Source License; you may not use this file except in compliance with the
// License.
//
// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation
// the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included
// in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
// OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
// IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
// CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
// TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
// SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
//
//////////////////////////////////////////////////////////////////////////
//
//%/////////////////////////////////////////////////////////////////////////////

#if defined(PEGASUS_OS_VMS)
# include <fcntl.h>
#endif
#include <Pegasus/Common/Logger.h>
#include <Pegasus/Common/System.h>
#include <Pegasus/Common/TraceFileHandler.h>
#include <Pegasus/Common/Mutex.h>

PEGASUS_USING_STD;

PEGASUS_NAMESPACE_BEGIN

static Mutex writeMutex;
PEGASUS_FORK_SAFE_MUTEX(writeMutex)

///////////////////////////////////////////////////////////////////////////////
//  Writes message to file. Locks the file before writing to it
//  Implementation of this function is platform specific
///////////////////////////////////////////////////////////////////////////////

#if defined(PEGASUS_OS_VMS)

void TraceFileHandler::prepareFileHandle(void)
{
    Sint32 retCode;

    if (_configHasChanged)
    {
        _reConfigure();
    }

    // Check if the file has been deleted, if so re-open the file and
    // continue
    if (!System::exists(_fileName))
    {
        if (_fileHandle == 0)
        {
            _fileHandle = fopen(_fileName, "a+", "shr=get,put,upd");
            // _fileHandle = fopen(_fileName,"a","shr=get");
        }
        else
        {
            _fileHandle =
                freopen(_fileName, "a+", _fileHandle, "shr=get,put,upd");
            // _fileHandle = freopen(_fileName,"a",_fileHandle,"shr=get");
        }
        if (!_fileHandle)
        {
            // Unable to re-open file, log a message

            Logger::put_l(
                Logger::ERROR_LOG, System::CIMSERVER, Logger::WARNING,
                MessageLoaderParms(
                    "Common.TraceFileHandlerUnix.FAILED_TO_OPEN_FILE",
                    "Failed to open File $0",
                    _fileName));
            return;
        }

        // Set permissions on the trace file to 0400

        if (!FileSystem::changeFilePermissions(
                String(_fileName), (S_IRUSR | S_IWUSR)))
        {
            Logger::put_l(
                Logger::ERROR_LOG, System::CIMSERVER, Logger::WARNING,
                MessageLoaderParms(
                    "Common.TraceFileHandlerUnix."
                        "FAILED_TO_SET_FILE_PERMISSIONS",
                    "Failed to set permissions on file $0",
                    _fileName));
            return;
        }
    }
    // Seek to the end of File
    retCode = fseek(_fileHandle, 0, SEEK_END);
}

void TraceFileHandler::handleMessage(
    const char *message,
    Uint32 msgLen,
    const char *fmt, va_list argList)
{
    Sint32 retCode;
    Sint32 fileDesc;

    // Do not add Trace calls in the Critical section
    // ---- BEGIN CRITICAL SECTION

    prepareFileHandle();
    
    // Write the message to the file

    retCode = fprintf(_fileHandle, "%s", message);
    retCode = vfprintf(_fileHandle, fmt, argList);
    retCode = fprintf(_fileHandle, "\n");
    retCode = fflush(_fileHandle);
    fileDesc = fileno(_fileHandle);
    retCode = fsync(fileDesc);
    _wroteToLog = false;
    // retCode = fclose(_fileHandle);
    // _fileHandle = 0;

    // ---- END CRITICAL SECTION

    return;
}

void TraceFileHandler::handleMessage(const char *message, Uint32 msgLen)
{
    Sint32 retCode;
    Sint32 fileDesc;

    // Do not add Trace calls in the Critical section
    // ---- BEGIN CRITICAL SECTION

    prepareFileHandle();
    
    // Write the message to the file

    retCode = fprintf(_fileHandle, "%s\n", message);
    retCode = fflush(_fileHandle);
    fileDesc = fileno(_fileHandle);
    retCode = fsync(fileDesc);
    _wroteToLog = false;
    // retCode = fclose(_fileHandle);
    // _fileHandle = 0;

    // ---- END CRITICAL SECTION

    return;
}


#else /* PEGASUS_OS_VMS */

void TraceFileHandler::prepareFileHandle(void)
{    
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

    // Got the Lock on the File. Seek to the end of File
    fseek(_fileHandle, 0, SEEK_END);
# ifdef PEGASUS_PLATFORM_LINUX_GENERIC_GNU
    long pos = ftell(_fileHandle);
    // Check if the file size is approaching 2GB - which is the
    // maximum size a file on 32 bit Linux can grow (ofcourse if
    // not using large-files option). If this is not checked, the
    // cimserver may get a SIGXFSZ signal and shutdown. See Bug#1527.
    if (pos >= 0x7ff00000)
    {
        // If the file size is almost 2 GB in size, close this trace
        // file and open a new trace file which would have _fileCount
        // as the suffix. So, if "cimserver.trc" is the trace file that
        // approaches 2GB, the next file which gets created would be
        // named "cimserver.trc.1" and so on ...
        fclose(_fileHandle);
        sprintf(_fileName, "%s.%u", _baseFileName, ++_fileCount);
        _fileHandle = fopen(_fileName, "a+");
        if (!_fileHandle)
        {
            // Unable to open file, log a message
            if (!_wroteToLog)
            {
                Logger::put_l(
                    Logger::ERROR_LOG, System::CIMSERVER, Logger::WARNING,
                    MessageLoaderParms(
                        "Common.TraceFileHandler.FAILED_TO_OPEN_FILE",
                        "Failed to open File $0",
                        _fileName));
                _wroteToLog = true;
            }
            return;
        }
    }
# endif
}

void TraceFileHandler::handleMessage(
    const char *message,
    Uint32 msgLen,
    const char *fmt, va_list argList)
{
    if (_configHasChanged)
    {
        _reConfigure();
    }

    if (!_fileHandle)
    {
        // The trace file is not open, which means an earlier fopen() was
        // unsuccessful.  Stop now to avoid logging duplicate error messages.
        return;
    }

    // Do not add Trace calls in the Critical section
    // ---- BEGIN CRITICAL SECTION
    AutoMutex writeLock(writeMutex);

    prepareFileHandle();
    // Write the message to the file
    fprintf(_fileHandle, "%s", message);
    vfprintf(_fileHandle, fmt, argList);
    fprintf(_fileHandle, "\n");
    fflush(_fileHandle);
    // ---- END CRITICAL SECTION
}

void TraceFileHandler::handleMessage(const char *message, Uint32 msgLen)
{
    if (_configHasChanged)
    {
        _reConfigure();
    }

    if (!_fileHandle)
    {
        // The trace file is not open, which means an earlier fopen() was
        // unsuccessful.  Stop now to avoid logging duplicate error messages.
        return;
    }

    // Do not add Trace calls in the Critical section
    // ---- BEGIN CRITICAL SECTION
    AutoMutex writeLock(writeMutex);

    prepareFileHandle();
    // Write the message to the file
    fprintf(_fileHandle, "%s\n", message);
    fflush(_fileHandle);
    // ---- END CRITICAL SECTION
}


#endif /* !PEGASUS_OS_VMS */

PEGASUS_NAMESPACE_END
