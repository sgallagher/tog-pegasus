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

#include <cstdlib>
// #include <dlfcn.h>
#include <Pegasus/Common/Destroyer.h>
#include <Pegasus/Common/System.h>
#include "ConsumerTable.h"

PEGASUS_USING_STD;
PEGASUS_NAMESPACE_BEGIN

ConsumerTable::ConsumerTable()
{

}

CIMIndicationConsumer* ConsumerTable::lookupConsumer(const String& consumerId)
{
    for (Uint32 i = 0, n = _consumers.size(); i < n; i++)
	if (String::equal(_consumers[i].consumerId, consumerId))
	    return _consumers[i].consumer;

    return 0;
}

typedef CIMIndicationConsumer* (*CreateIndicationConsumerFunc)();

CIMIndicationConsumer* ConsumerTable::loadConsumer(const String& consumerId)
{
    // Load the dynamic library:

#ifdef PEGASUS_OS_TYPE_WINDOWS
    ArrayDestroyer<char> libraryName = consumerId.allocateCString();
#else
    String unixLibName = getenv("PEGASUS_HOME");
    unixLibName += "/lib/lib";
    unixLibName += consumerId;
    unixLibName += ".so";
    ArrayDestroyer<char> libraryName = unixLibName.allocateCString();
#endif

    DynamicLibraryHandle libraryHandle = 
	System::loadDynamicLibrary(libraryName.getPointer());

    if (!libraryHandle) {
#ifdef PEGASUS_OS_TYPE_WINDOWS
	throw DynamicLoadFailed(libraryName.getPointer());
#else
        unixLibName = System::dynamicLoadError();
        ArrayDestroyer<char> errorMsg = unixLibName.allocateCString();
	throw DynamicLoadFailed(errorMsg.getPointer());
#endif
    }

    // Lookup the create consumer symbol:

    String tmp = "PegasusCreateIndicationConsumer_";
    tmp.append(consumerId);
    ArrayDestroyer<char> functionName = tmp.allocateCString();

    CreateIndicationConsumerFunc func = 
	(CreateIndicationConsumerFunc)System::loadDynamicSymbol(
	libraryHandle, functionName.getPointer());

    if (!func)
	throw DynamicLookupFailed(functionName.getPointer());

    // Create the consumer:

    CIMIndicationConsumer* consumer = func();

    if (!consumer)
	throw CreateIndicationConsumerReturnedNull(
	    libraryName.getPointer(), 
	    functionName.getPointer());

    if (consumer)
    {
	Entry entry;
	entry.consumerId = consumerId;
	entry.consumer = consumer;
	_consumers.append(entry);
    }

    return consumer;
}

PEGASUS_NAMESPACE_END
