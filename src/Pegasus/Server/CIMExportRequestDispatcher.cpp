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

#include "CIMExportRequestDispatcher.h"
#include <Pegasus/Repository/CIMRepository.h>
#include "HandlerTable.h"

PEGASUS_NAMESPACE_BEGIN

PEGASUS_USING_STD;

CIMExportRequestDispatcher::CIMExportRequestDispatcher(CIMRepository* repository)
    : _repository(repository)
{

}

CIMExportRequestDispatcher::~CIMExportRequestDispatcher()
{

}

void CIMExportRequestDispatcher::handleIndication(
    CIMInstance& indicationHandlerInstance,
    CIMInstance& indicationInstance,
    String nameSpace)
{
    String className = indicationHandlerInstance.getClassName();
    CIMHandler* handler = _lookupHandlerForClass(nameSpace, className);
    
    if (handler)
	handler->handleIndication(
	    indicationHandlerInstance,
	    indicationInstance,
	    nameSpace);
    else
	throw CIMException(CIM_ERR_FAILED);
}

CIMHandler* CIMExportRequestDispatcher::_lookupHandlerForClass(
    const String& nameSpace,
    const String& className)
{
    //----------------------------------------------------------------------
    // Look up the class:
    //----------------------------------------------------------------------

    CIMClass cimClass = _repository->getClass(nameSpace, className);

    if (!cimClass)
	throw CIMException(CIM_ERR_INVALID_CLASS);

    //----------------------------------------------------------------------
    // Get the handler qualifier:
    //----------------------------------------------------------------------

    Uint32 pos = cimClass.findQualifier("Handler");
    
    if (pos == PEG_NOT_FOUND)
	return 0;

    CIMQualifier q = cimClass.getQualifier(pos);
    String handlerId;

    q.getValue().get(handlerId);
    
    CIMHandler* handler = _handlerTable.lookupHandler(handlerId);

    if (!handler)
    {
	handler = _handlerTable.loadHandler(handlerId);

	if (!handler)
	    throw CIMException(CIM_ERR_FAILED);

	handler->initialize();
    }

    return handler;
}

PEGASUS_NAMESPACE_END
