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
//
//%/////////////////////////////////////////////////////////////////////////////


#ifndef Pegasus_TraceFileHandler_h
#define Pegasus_TraceFileHandler_h

#include <cstdarg>
#include <cstdio>
#include <Pegasus/Common/String.h>
#include <Pegasus/Common/Linkage.h>

PEGASUS_NAMESPACE_BEGIN

/** TraceFileHandler implements logging of messages to file 
 */

class PEGASUS_COMMON_LINKAGE TraceFileHandler 
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

    /** Writes message to file.
        Implementation of this function is platform specific
        @param    message  message to be written
        @param    fmt      printf style format string
        @param    argList  variable argument list
     */
    void handleMessage(const char* message,const char* fmt,va_list argList);

    /** Sets the filename to the given filename and opens the file in append
        mode
        @param    fileName full path of the file 
        @return   0        if the function is successful in opening the file
                  1        if an error occurs while opening the file
     */
    Uint32 setFileName(const char* fileName);

    /** Validates the File Path for the trace File
        @param    filePath full path of the file 
        @return   1        if the file path is valid
                  0        if the file path is invalid
     */
    Boolean isValidFilePath(const char* filePath);

    TraceFileHandler();

    ~TraceFileHandler();

};

PEGASUS_NAMESPACE_END

#endif /* Pegasus_TraceFileHandler_h */
