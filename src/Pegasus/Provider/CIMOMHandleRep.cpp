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
// Author: Roger Kumpf, Hewlett-Packard Company (roger_kumpf@hp.com)
//
// Modified By:
//
//%/////////////////////////////////////////////////////////////////////////////

#include <string.h>    // for memcpy()
#include <Pegasus/Common/Tracer.h>

#include "CIMOMHandleRep.h"

PEGASUS_NAMESPACE_BEGIN

CIMOMHandleRep::CIMOMHandleRep()
    : _pendingOperations(0),
      _providerUnloadProtect(0)
{
    gettimeofday(&_idleTime, NULL);
}

CIMOMHandleRep::~CIMOMHandleRep()
{
}

void CIMOMHandleRep::disallowProviderUnload()
{
    PEG_METHOD_ENTER(TRC_CIMOM_HANDLE,
        "CIMOMHandleRep::disallowProviderUnload");

    try
    {
        AutoMutex lock(_providerUnloadProtectMutex);
        _providerUnloadProtect++;
    }
    catch (...)
    {
        // There's not much a provider could do with this exception.  Since
        // this is just a hint, our best bet is to just ignore it.
        PEG_TRACE_STRING(TRC_CIMOM_HANDLE, Tracer::LEVEL3,
            "Caught unexpected exception");
    }

    PEG_METHOD_EXIT();
}

void CIMOMHandleRep::allowProviderUnload()
{
    PEG_METHOD_ENTER(TRC_CIMOM_HANDLE,
        "CIMOMHandleRep::allowProviderUnload");

    try
    {
        AutoMutex lock(_providerUnloadProtectMutex);
        if (_providerUnloadProtect > 0)
        {
            _providerUnloadProtect--;
        }
    }
    catch (...)
    {
        // There's not much a provider could do with this exception.  Since
        // this is just a hint, our best bet is to just ignore it.
        PEG_TRACE_STRING(TRC_CIMOM_HANDLE, Tracer::LEVEL3,
            "Caught unexpected exception");
    }

    PEG_METHOD_EXIT();
}

void CIMOMHandleRep::get_idle_timer(struct timeval *tv)
{
    if (tv == 0)
    {
        return;
    }
   
    try 
    {
        AutoMutex lock(_idleTimeMutex);
        memcpy(tv, &_idleTime, sizeof(struct timeval));
    }
    catch (...)
    {
        gettimeofday(tv, NULL);
    }
}

void CIMOMHandleRep::update_idle_timer()
{
    try
    {
        AutoMutex lock(_idleTimeMutex);
        gettimeofday(&_idleTime, NULL);
    }
    catch (...)
    {
    }
}

Boolean CIMOMHandleRep::pending_operation()
{
    if (_pendingOperations.value())
    {
        return true;
    }
    return false;
}

Boolean CIMOMHandleRep::unload_ok()
{
    if (_pendingOperations.value())
    {
        return false;
    }

    Boolean unloadable = true;

    AutoMutex lock(_providerUnloadProtectMutex);
    if (_providerUnloadProtect > 0)
    {
        unloadable = false;
    }

    return (unloadable);
}

PEGASUS_NAMESPACE_END
