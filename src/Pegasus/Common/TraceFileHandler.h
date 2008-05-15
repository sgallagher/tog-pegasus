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


#ifndef Pegasus_TraceFileHandler_h
#define Pegasus_TraceFileHandler_h

#include <cstdarg>
#include <cstdio>
#include <Pegasus/Common/String.h>
#include <Pegasus/Common/Linkage.h>
#include <Pegasus/Common/TraceHandler.h>

PEGASUS_NAMESPACE_BEGIN

/** TraceFileHandler implements logging of messages to file
 */

class PEGASUS_COMMON_LINKAGE TraceFileHandler: public TraceHandler
{
private:

    /** Open the specified file in append mode and ensure the file owner and
        permissions are appropriate.
        @param    fileName Full path of the file to open.
        @return   Handle to the opened file if successful, otherwise 0.
     */
    FILE* _openFile(const char* fileName);

    /* File path to write messages
     */
    char* _fileName;

#ifdef PEGASUS_PLATFORM_LINUX_GENERIC_GNU
    /* Base File path to write messages
     */
    char* _baseFileName;

    /* Count for the suffix of the trace file
     */
    Uint32 _fileCount;
#endif

    /* Handle to the File
     */
    FILE* _fileHandle;

    /* Flag to track writing of message to log
     */
    Boolean _wroteToLog;

public:

    /** Writes message with format string to the tracing facility
        @param    message  message to be written
        @param    fmt      printf style format string
        @param    argList  variable argument list
     */
    virtual void handleMessage(const char* message,
                               const char* fmt,
                               va_list argList);

    /** Writes simple message to the tracing facility.
        @param    message  message to be written
     */
    virtual void handleMessage(const char* message);


    /** Sets and prepares the destination (e.g. traceFileName) for the
        trace handler.
        @param    destination tracer destination, e.g. file
        @return   0           if the function is successful
                  1           if an error occurs
     */
    virtual Uint32 setMessageDestination(const char* traceFileName);

    /** Validates the File Path for the trace File
        @param    filePath full path of the file
        @return   1        if the file path is valid
                  0        if the file path is invalid
     */
    virtual Boolean isValidMessageDestination(const char* traceFileName);

    TraceFileHandler();

    virtual ~TraceFileHandler();

private:

    /** Prepares write of message to file.
        Implementation of this function is platform specific
     */
    void prepareFileHandle(void);

};

PEGASUS_NAMESPACE_END

#endif /* Pegasus_TraceFileHandler_h */
