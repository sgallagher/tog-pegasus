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
// Author: Carol Ann Krug Graves, Hewlett-Packard Company
//             (carolann_graves@hp.com)
//
// Modified By: David Dillard, VERITAS Software Corp.
//                  (david.dillard@veritas.com)
//
//%/////////////////////////////////////////////////////////////////////////////


#include "AnonymousPipe.h"
#include <Pegasus/Common/Signal.h>
#include <windows.h>
#include <stdio.h>


PEGASUS_NAMESPACE_BEGIN

AnonymousPipe::AnonymousPipe ()
{
    PEG_METHOD_ENTER (TRC_OS_ABSTRACTION, "AnonymousPipe::AnonymousPipe ()");

    AnonymousPipeHandle thePipe [2];

    SECURITY_ATTRIBUTES saAttr;
    saAttr.nLength = sizeof (SECURITY_ATTRIBUTES);
    saAttr.bInheritHandle = TRUE;
    saAttr.lpSecurityDescriptor = NULL;

    if (!CreatePipe (&thePipe [0], &thePipe [1], &saAttr, 0))
    {
        Tracer::trace (TRC_OS_ABSTRACTION, Tracer::LEVEL2,
            "Failed to create pipe.  Error code: %d", GetLastError ());
        PEG_METHOD_EXIT ();

        MessageLoaderParms mlp ("Common.AnonymousPipe.CREATE_PIPE_FAILED",
            "Failed to create pipe.");
        throw Exception (mlp);
    }

    _readHandle = thePipe [0];
    _writeHandle = thePipe [1];
    _readOpen = true;
    _writeOpen = true;

    PEG_METHOD_EXIT ();
}

AnonymousPipe::AnonymousPipe (
    const char * readHandle,
    const char * writeHandle)
{
    PEG_METHOD_ENTER (TRC_OS_ABSTRACTION, 
        "AnonymousPipe::AnonymousPipe (const char *, const char *)");

    _readHandle = 0;
    _writeHandle = 0;
    _readOpen = false;
    _writeOpen = false;

    if (readHandle != NULL)
    {
        if (sscanf (readHandle, "%p", &_readHandle) != 1)
        {
            Tracer::trace (TRC_OS_ABSTRACTION, Tracer::LEVEL2,
                "Failed to create pipe: invalid read handle %s", readHandle);
            PEG_METHOD_EXIT ();

            MessageLoaderParms mlp ("Common.AnonymousPipe.CREATE_PIPE_FAILED",
                "Failed to create pipe.");
            throw Exception (mlp);
        }
        _readOpen = true;
    }

    if (writeHandle != NULL)
    {
        if (sscanf (writeHandle, "%p", &_writeHandle) != 1)
        {
            Tracer::trace (TRC_OS_ABSTRACTION, Tracer::LEVEL2,
                "Failed to create pipe: invalid write handle %s", writeHandle);
            PEG_METHOD_EXIT ();

            MessageLoaderParms mlp ("Common.AnonymousPipe.CREATE_PIPE_FAILED",
                "Failed to create pipe.");
            throw Exception (mlp);
        }
        _writeOpen = true;
    }

    PEG_METHOD_EXIT ();
}

AnonymousPipe::~AnonymousPipe ()
{
    PEG_METHOD_ENTER (TRC_OS_ABSTRACTION, "AnonymousPipe::~AnonymousPipe");

    if (_readOpen)
    {
        closeReadHandle ();
    }

    if (_writeOpen)
    {
        closeWriteHandle ();
    }

    PEG_METHOD_EXIT ();
}

AnonymousPipe::Status AnonymousPipe::writeBuffer (
    const void * buffer,
    Uint32 bytesToWrite)
{
    //
    //  Treat invalid handle as connection closed
    //
    if (!_writeOpen)
    {
        Tracer::trace (TRC_OS_ABSTRACTION, Tracer::LEVEL2,
            "Attempted to write to pipe whose write handle is not open");
        return STATUS_CLOSED;
    }

    //
    //  Ignore SIGPIPE signals
    //
    SignalHandler::ignore (PEGASUS_SIGPIPE);

    const char * writeBuffer = reinterpret_cast<const char *>(buffer);
    DWORD expectedBytes = bytesToWrite;
    do
    {
        BOOL returnValue;
        DWORD bytesWritten = 0;
        returnValue = WriteFile (_writeHandle, writeBuffer, expectedBytes,
            &bytesWritten, NULL);

        if (!returnValue)
        {
            Tracer::trace (TRC_OS_ABSTRACTION, Tracer::LEVEL2,
                "Failed to write buffer to pipe.  Error code: %d", 
                GetLastError ());
            return STATUS_ERROR;
        }

        if (bytesWritten < 0)
        {
            Tracer::trace (TRC_OS_ABSTRACTION, Tracer::LEVEL2,
                "Failed to write buffer to pipe.  Error code: %d", 
                GetLastError ());

            if ((GetLastError () == ERROR_PIPE_NOT_CONNECTED) ||
                (GetLastError () == ERROR_BROKEN_PIPE))
            {
                return STATUS_CLOSED;
            }
            else
            {
                return STATUS_ERROR;
            }
        }

        expectedBytes -= bytesWritten;
        writeBuffer += bytesWritten;
    } while (expectedBytes > 0);

    return STATUS_SUCCESS;
}

AnonymousPipe::Status AnonymousPipe::readBuffer (
    void * buffer,
    Uint32 bytesToRead)
{
    //
    //  Treat invalid handle as connection closed
    //
    if (!_readOpen)
    {
        Tracer::trace (TRC_OS_ABSTRACTION, Tracer::LEVEL2,
            "Attempted to read from pipe whose read handle is not open");
        return STATUS_CLOSED;
    }

    DWORD expectedBytes = bytesToRead;

    do
    {
        BOOL returnValue;
        DWORD bytesRead;
        returnValue = ReadFile (_readHandle, buffer, bytesToRead, &bytesRead,
            NULL);

        if (!returnValue)
        {
            Tracer::trace (TRC_OS_ABSTRACTION, Tracer::LEVEL2,
                "Failed to read buffer from pipe.  Error code: %d", 
                GetLastError ());
            if ((GetLastError () == ERROR_PIPE_NOT_CONNECTED) ||
                (GetLastError () == ERROR_BROKEN_PIPE))
            {
                return STATUS_CLOSED;
            }

            return STATUS_ERROR;
        }

        if (bytesRead == 0)
        {
            //
            //  Connection closed
            //
            Tracer::trace (TRC_OS_ABSTRACTION, Tracer::LEVEL2,
                "Failed to read buffer from pipe: connection closed");
            return STATUS_CLOSED;
        }

        if (bytesRead < 0)
        {
            Tracer::trace (TRC_OS_ABSTRACTION, Tracer::LEVEL2,
                "Failed to read buffer from pipe.  Error code: %d", 
                GetLastError ());

            //
            //  Error reading from pipe
            //
            return STATUS_ERROR;
        }

        buffer = reinterpret_cast<char *>(buffer) + bytesRead;
        bytesToRead -= bytesRead;
    } while (bytesToRead > 0);

    return STATUS_SUCCESS;
}

void AnonymousPipe::exportReadHandle (char * buffer) const
{
    PEG_METHOD_ENTER (TRC_OS_ABSTRACTION, "AnonymousPipe::exportReadHandle");

    sprintf (buffer, "%p", _readHandle);

    PEG_METHOD_EXIT ();
}

void AnonymousPipe::exportWriteHandle (char * buffer) const
{
    PEG_METHOD_ENTER (TRC_OS_ABSTRACTION, "AnonymousPipe::exportWriteHandle");

    sprintf (buffer, "%p", _writeHandle);

    PEG_METHOD_EXIT ();
}

void AnonymousPipe::closeReadHandle ()
{
    PEG_METHOD_ENTER (TRC_OS_ABSTRACTION, "AnonymousPipe::closeReadHandle");

    if (_readOpen)
    {
        if (!CloseHandle (_readHandle))
        {
            Tracer::trace (TRC_OS_ABSTRACTION, Tracer::LEVEL2,
                "Failed to close read handle.  Error code: %d", 
                GetLastError ());
        }
        else
        {
            _readOpen = false;
        }
    }
    else
    {
        Tracer::trace (TRC_OS_ABSTRACTION, Tracer::LEVEL2,
            "Attempted to close read handle that was not open");
    }

    PEG_METHOD_EXIT ();
}

void AnonymousPipe::closeWriteHandle ()
{
    PEG_METHOD_ENTER (TRC_OS_ABSTRACTION, "AnonymousPipe::closeWriteHandle");

    if (_writeOpen)
    {
        if (!CloseHandle (_writeHandle))
        {
            Tracer::trace (TRC_OS_ABSTRACTION, Tracer::LEVEL2,
                "Failed to close write handle.  Error code: %d", 
                GetLastError ());
        }
        else
        {
            _writeOpen = false;
        }
    }
    else
    {
        Tracer::trace (TRC_OS_ABSTRACTION, Tracer::LEVEL2,
            "Attempted to close write handle that was not open");
    }

    PEG_METHOD_EXIT ();
}

PEGASUS_NAMESPACE_END
