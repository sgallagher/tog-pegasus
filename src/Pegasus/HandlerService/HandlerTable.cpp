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
// Author: Nitin Upasani, Hewlett-Packard Company (Nitin_Upasani@hp.com)
//
// Modified By: Sushma Fernandes, 
//                 Hewlett-Packard Company (sushma_fernandes@hp.com)
//            : Yi Zhou Hewlett-Packard Company (yi_zhou@hp.com)
//		Sean Keenan (sean.keenan@hp.com)
//              Josephine Eskaline Joyce, IBM (jojustin@in.ibm.com) for PEP # 101
//
//%/////////////////////////////////////////////////////////////////////////////

#include <Pegasus/Common/Config.h>
#include <cstdlib>
//#include <dlfcn.h>
#include <Pegasus/Common/System.h>
#include <Pegasus/Common/FileSystem.h>
#include "HandlerTable.h"

PEGASUS_USING_STD;
PEGASUS_NAMESPACE_BEGIN

HandlerTable::HandlerTable()
{

}

CIMHandler* HandlerTable::getHandler(
    const String& handlerId,
    CIMRepository* repository)
{
    CIMHandler * handler;
    {
        ReadLock lock(_handlerTableLock);
        handler = _lookupHandler(handlerId);
	if (handler)
        {
            return (handler);
        }
    }

    {
        WriteLock lock(_handlerTableLock);
	handler = _lookupHandler(handlerId);
        // Note: Lock handler table until handler initialize is done.
	// This is ok for handler since the initialization is simple.
	if (!handler)
        {
            handler = _loadHandler(handlerId);
            handler->initialize(repository);
        }

        return (handler);
    }
}

CIMHandler* HandlerTable::_lookupHandler(const String& handlerId)
{
    for (Uint32 i = 0, n = _handlers.size(); i < n; i++)
	if (String::equal(_handlers[i].handlerId, handlerId))
	    return _handlers[i].handler;

    return 0;
}

typedef CIMHandler* (*CreateHandlerFunc)();

CIMHandler* HandlerTable::_loadHandler(const String& handlerId)
{
#if defined (PEGASUS_OS_VMS)
    String fileName = FileSystem::buildLibraryFileName(handlerId);
#elif defined(PEGASUS_OS_OS400)
    Uint32 lastSlash = handlerId.reverseFind('/');
    if (lastSlash == PEG_NOT_FOUND)
      throw DynamicLoadFailed(handlerId);
    String fileName = handlerId.subString(0, lastSlash);
    String os400HandlerId = handlerId.subString(lastSlash + 1);
#else
    String fileName = ConfigManager::getHomedPath((PEGASUS_DEST_LIB_DIR) +
        String("/") + FileSystem::buildLibraryFileName(handlerId));
#endif

    DynamicLibraryHandle libraryHandle = 
	System::loadDynamicLibrary(fileName.getCString());

    if (!libraryHandle) {
#if defined(PEGASUS_OS_TYPE_WINDOWS) || defined(PEGASUS_OS_VMS)
	throw DynamicLoadFailed(fileName);
#else
        String errorMsg = System::dynamicLoadError();
	throw DynamicLoadFailed(errorMsg);
#endif
    }

    // Lookup the create handler symbol:

    String functionName = "PegasusCreateHandler_";
#ifndef PEGASUS_OS_OS400
    functionName.append(handlerId);
#else
    functionName.append(os400HandlerId);
#endif

    CreateHandlerFunc func = (CreateHandlerFunc)System::loadDynamicSymbol(
	libraryHandle, functionName.getCString());


    if (!func)
    {
	throw DynamicLookupFailed(functionName);
    }

    // Create the handler:

    CIMHandler* handler = func();

    if (!handler)
    {
	throw CreateHandlerReturnedNull(
	    fileName, 
	    functionName);
    }
    else
    {
	Entry entry;
	entry.handlerId = handlerId;
	entry.handler = handler;
	_handlers.append(entry);
    }

    return handler;
}

PEGASUS_NAMESPACE_END
