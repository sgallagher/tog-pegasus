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

#ifndef Pegasus_BasicProviderManagerRouter_h
#define Pegasus_BasicProviderManagerRouter_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/ArrayInternal.h>
#include <Pegasus/Common/CIMMessage.h>
#include <Pegasus/Common/OperationContextInternal.h>
#include <Pegasus/Common/ReadWriteSem.h>

#include <Pegasus/ProviderManager2/ProviderManager.h>
#include <Pegasus/ProviderManagerService/ProviderManagerRouter.h>
#include <Pegasus/ProviderManagerService/Linkage.h>

PEGASUS_NAMESPACE_BEGIN

class ProviderManagerContainer;

class PEGASUS_PMS_LINKAGE BasicProviderManagerRouter
    : public ProviderManagerRouter
{
public:
    BasicProviderManagerRouter(
        PEGASUS_INDICATION_CALLBACK_T indicationCallback,
        PEGASUS_RESPONSE_CHUNK_CALLBACK_T responseChunkCallback,
        ProviderManager* (*createDefaultProviderManagerCallback)());

    virtual ~BasicProviderManagerRouter();

    virtual Message* processMessage(Message* message);

    /**
        Indicates whether any of the active ProviderManagers managed by this
        ProviderManagerRouter has an active (e.g., loaded/initialized)
        provider.  If no providers are active, this ProviderManagerRouter
        may be destructed without harm.
     */
    Boolean hasActiveProviders();

    virtual void unloadIdleProviders();

private:
    BasicProviderManagerRouter();
    BasicProviderManagerRouter(const BasicProviderManagerRouter&);
    BasicProviderManagerRouter& operator=(const BasicProviderManagerRouter&);

   // providerModuleName is used to report the error when ProviderManager 
   // can not be found for the interfaceType.
    ProviderManager* _getProviderManager(const String& interfaceType,
        const String& providerModuleName,
        const String& providerManagerPath);
    ProviderManager* _lookupProviderManager(const String& interfaceType);

    Array<ProviderManagerContainer*> _providerManagerTable;
    ReadWriteSem _providerManagerTableLock;

    static PEGASUS_INDICATION_CALLBACK_T _indicationCallback;
    static PEGASUS_RESPONSE_CHUNK_CALLBACK_T _responseChunkCallback;
    static ProviderManager* (*_createDefaultProviderManagerCallback)();
};

PEGASUS_NAMESPACE_END

#endif
