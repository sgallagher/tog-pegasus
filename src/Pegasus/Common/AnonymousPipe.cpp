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
// 		Sean Keenan (sean.keenan@hp.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#include <Pegasus/Common/CIMMessageSerializer.h>
#include <Pegasus/Common/CIMMessageDeserializer.h>
#include <Pegasus/Common/MessageLoader.h>
#include <Pegasus/Common/Exception.h>
#include <Pegasus/Common/Tracer.h>

#include "AnonymousPipe.h"


#if defined (PEGASUS_OS_TYPE_WINDOWS)
# include "AnonymousPipeWindows.cpp"
#elif defined (PEGASUS_OS_TYPE_UNIX)
# include "AnonymousPipeUnix.cpp"
#elif defined (PEGASUS_OS_VMS)
# include "AnonymousPipeVms.cpp"
#else
# error "Unsupported platform"
#endif

PEGASUS_NAMESPACE_BEGIN

AnonymousPipe::Status AnonymousPipe::writeMessage (CIMMessage * message)
{
    PEG_METHOD_ENTER (TRC_OS_ABSTRACTION, "AnonymousPipe::writeMessage");

    //
    // Serialize the request
    //
    Array<char> messageBuffer;
    messageBuffer.reserveCapacity (4096);
    try
    {
        CIMMessageSerializer::serialize (messageBuffer, message);
    }
    catch (Exception & e)
    {
        PEG_TRACE_STRING (TRC_OS_ABSTRACTION, Tracer::LEVEL2,
            "Failed to serialize message: " + e.getMessage ());
        PEG_METHOD_EXIT ();
        throw;
    }

    //
    // Write the serialized message to the pipe
    //
    Status writeStatus;
    try
    {
        Uint32 messageLength = messageBuffer.size ();
        const char * messageData = messageBuffer.getData ();

        writeStatus = writeBuffer ((const char *) &messageLength, 
            sizeof (Uint32));

        if (writeStatus == STATUS_SUCCESS)
        {
            writeStatus = writeBuffer (messageBuffer.getData (), 
                messageLength);
        }
    }
    catch (...)
    {
        PEG_METHOD_EXIT ();
        throw;
    }

    PEG_METHOD_EXIT ();
    return writeStatus;
}

AnonymousPipe::Status AnonymousPipe::readMessage (CIMMessage * & message)
{
    PEG_METHOD_ENTER (TRC_OS_ABSTRACTION, "AnonymousPipe::readMessage");

    message = 0;

    //
    //  Read the message length
    //
    Uint32 messageLength;
    Status readStatus = readBuffer ((char *) &messageLength, sizeof (Uint32));

    if (readStatus != STATUS_SUCCESS)
    {
        PEG_METHOD_EXIT ();
        return readStatus;
    }

    if (messageLength == 0)
    {
        //
        //  Null message
        //
        PEG_METHOD_EXIT ();
        return STATUS_SUCCESS;
    }

    //
    //  Read the message data
    //
    AutoArrayPtr <char> messageBuffer (new char [messageLength + 1]);

    //
    //  We know a message is coming
    //  Keep reading even if interrupted
    //
    do
    {
        readStatus = readBuffer (messageBuffer.get (), messageLength);
    } while (readStatus == STATUS_INTERRUPT);

    if (readStatus != STATUS_SUCCESS)
    {
        PEG_METHOD_EXIT ();
        return readStatus;
    }

    try
    {
        //
        //  De-serialize the message
        //
        message = CIMMessageDeserializer::deserialize (messageBuffer.get ());
    }
    catch (Exception & e)
    {
        //
        //  De-serialization failed
        //
        PEG_TRACE_STRING (TRC_OS_ABSTRACTION, Tracer::LEVEL2,
            "Failed to de-serialize message: " + e.getMessage ());
        PEG_METHOD_EXIT ();
        throw;
    }

    PEG_METHOD_EXIT ();
    return readStatus;
}

PEGASUS_NAMESPACE_END
