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

#include "ProviderStatus.h"

PEGASUS_NAMESPACE_BEGIN

ProviderStatus::ProviderStatus()
    : _cimom_handle(0),
      _module(0),
      _isInitialized(false),
      _noUnload(0),
      _currentOperations(0),
      _indicationsEnabled(false),
      _currentSubscriptions(0)
{
}

ProviderStatus::~ProviderStatus()
{
}

Boolean ProviderStatus::isInitialized()
{
    AutoMutex lock(_statusMutex);
    return _isInitialized;
}

void ProviderStatus::setInitialized(Boolean initialized)
{
    AutoMutex lock(_statusMutex);
    _isInitialized = initialized;
}

ProviderModule* ProviderStatus::getModule() const
{
    return _module;
}

void ProviderStatus::setModule(ProviderModule* module)
{
    _module = module;
}

void ProviderStatus::setCIMOMHandle(CIMOMHandle* cimomHandle)
{
    _cimom_handle = cimomHandle;
}

void ProviderStatus::reset()
{
    _cimom_handle = 0;
    _module = 0;
    _noUnload = 0;
    _isInitialized = false;
}

void ProviderStatus::get_idle_timer(struct timeval* t)
{
    if (t && _cimom_handle)
    {
        _cimom_handle->get_idle_timer(t);
    }
}

void ProviderStatus::update_idle_timer()
{
    if (_cimom_handle)
    {
        _cimom_handle->update_idle_timer();
    }
}

Boolean ProviderStatus::pending_operation()
{
    if (_cimom_handle)
    {
        return _cimom_handle->pending_operation();
    }

    return false;
}


Boolean ProviderStatus::unload_ok()
{
    if (!_isInitialized || _noUnload.get())
    {
        return false;
    }

    if (_cimom_handle)
    {
        return _cimom_handle->unload_ok();
    }

    return true;
}

void ProviderStatus::protect()
{
    _noUnload++;
}

void ProviderStatus::unprotect()
{
    _noUnload--;
}

Boolean ProviderStatus::testIfZeroAndIncrementSubscriptions()
{
    AutoMutex lock(_currentSubscriptionsMutex);
    return (_currentSubscriptions++ == 0);
}

Boolean ProviderStatus::decrementSubscriptionsAndTestIfZero()
{
    AutoMutex lock(_currentSubscriptionsMutex);
    return (--_currentSubscriptions == 0);
}

Boolean ProviderStatus::testSubscriptions()
{
    AutoMutex lock(_currentSubscriptionsMutex);
    return (_currentSubscriptions > 0);
}

void ProviderStatus::resetSubscriptions()
{
    AutoMutex lock(_currentSubscriptionsMutex);
    _currentSubscriptions = 0;
}

void ProviderStatus::setProviderInstance(const CIMInstance& instance)
{
    _providerInstance = instance;
}

CIMInstance ProviderStatus::getProviderInstance()
{
    return _providerInstance;
}

PEGASUS_NAMESPACE_END
