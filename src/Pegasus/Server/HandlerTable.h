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
// Author: Nitin Upasani, Hewlett-Packard Company (Nitin_Upasani@hp.com)
//
// Modified By:
//
//%/////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_HandlerTable_h
#define Pegasus_HandlerTable_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/Array.h>
#include <Pegasus/Common/Exception.h>
#include <Pegasus/Handler/CIMHandler.h>

PEGASUS_NAMESPACE_BEGIN

class CreateHandlerReturnedNull : public Exception
{
public:

    CreateHandlerReturnedNull(
	const String& libName, 
	const String& funcName)
	: Exception(funcName + " returned null in library " + libName) { }
};
 
// The handler table maintains a list of handlers which have been 
// dynamically loaded. It maintains a mapping between string 
// handler identifiers and handlers. Indication Processor will use the 
// handler table to find handler for the purposes of request dispatching.

class PEGASUS_SERVER_LINKAGE HandlerTable
{
public:

    HandlerTable();

    CIMHandler* lookupHandler(const String& handlerId);

    CIMHandler* loadHandler(const String& handlerId);

private:

    struct Entry
    {
	String handlerId;
	CIMHandler* handler;
    };

    Array<Entry> _handlers;

public:

    friend int operator==(const Entry& x, const Entry& y)
    {
	return 0;
    }
};

PEGASUS_NAMESPACE_END

#endif /* Pegasus_HandlerTable_h */
