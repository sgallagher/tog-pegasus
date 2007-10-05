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

#ifndef Pegasus_HandlerTable_h
#define Pegasus_HandlerTable_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/ArrayInternal.h>
#include <Pegasus/Common/InternalException.h>
#include <Pegasus/Common/DynamicLibrary.h>
#include <Pegasus/Common/ReadWriteSem.h>
#include <Pegasus/Handler/CIMHandler.h>

#include <Pegasus/HandlerService/Linkage.h>

PEGASUS_NAMESPACE_BEGIN

// The handler table maintains a list of handlers which have been
// dynamically loaded. It maintains a mapping between string
// handler identifiers and handlers. Indication Processor will use the
// handler table to find handler for the purposes of request dispatching.

class PEGASUS_HANDLER_SERVICE_LINKAGE HandlerTable
{
public:

    HandlerTable();

    CIMHandler* getHandler(
        const String& handlerId,
        CIMRepository* repository);

    ~HandlerTable();

private:

    class HandlerEntry
    {
    public:
        HandlerEntry(const String& id, const String& fileName)
            : handlerId(id), handlerLibrary(fileName)
        {
        }

        // NOTE: The compiler default implementations of the copy constructor
        // and assignment operator are used for this class.

        String handlerId;
        DynamicLibrary handlerLibrary;
        CIMHandler* handler;

    private:
        HandlerEntry();
    };

    Array<HandlerEntry> _handlers;

    CIMHandler* _lookupHandler(const String& handlerId);

    CIMHandler* _loadHandler(const String& handlerId);

    ReadWriteSem _handlerTableLock;
};

PEGASUS_NAMESPACE_END

#endif /* Pegasus_HandlerTable_h */
