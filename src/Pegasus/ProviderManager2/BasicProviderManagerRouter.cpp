//%2003////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001, 2002  BMC Software, Hewlett-Packard Development
// Company, L. P., IBM Corp., The Open Group, Tivoli Systems.
// Copyright (c) 2003 BMC Software; Hewlett-Packard Development Company, L. P.;
// IBM Corp.; EMC Corporation, The Open Group.
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
// Author: Chip Vincent (cvincent@us.ibm.com)
//         Roger Kumpf, Hewlett-Packard Company (roger_kumpf@hp.com)
//
// Modified By:
//
//%/////////////////////////////////////////////////////////////////////////////

#include "BasicProviderManagerRouter.h"

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/CIMMessage.h>
#include <Pegasus/Common/Tracer.h>
#include <Pegasus/ProviderManager2/OperationResponseHandler.h>
#include <Pegasus/ProviderManager2/ProviderManagerModule.h>
#include <Pegasus/ProviderManager2/ProviderManager.h>

// ProviderManager library names.  Should these be defined elsewhere?
#if defined(PEGASUS_OS_OS400)
# define LIBRARY_NAME_DEFAULTPM "QSYS/QYCMDFTPVM"
# define LIBRARY_NAME_CMPIPM    "QSYS/QYCMCMPIPM"
# define LIBRARY_NAME_JMPIPM    "QSYS/QYCMJMPIPM"
#else
# define LIBRARY_NAME_DEFAULTPM "DefaultProviderManager"
# define LIBRARY_NAME_CMPIPM    "CMPIProviderManager"
# define LIBRARY_NAME_JMPIPM    "JMPIProviderManager"
#endif

PEGASUS_NAMESPACE_BEGIN

// BEGIN TEMP SECTION
class ProviderManagerContainer
{
public:
    ProviderManagerContainer()
    : _manager(0)
    {
    }

    ProviderManagerContainer(const ProviderManagerContainer & container)
    : _manager(0)
    {
        *this = container;
    }

    ProviderManagerContainer(
        const String& physicalName,
        const String& logicalName,
        const String& interfaceName,
        PEGASUS_INDICATION_CALLBACK indicationCallback)
    : _manager(0)
    {
        _physicalName = ProviderManager::_resolvePhysicalName(physicalName);

        _logicalName = logicalName;
        _interfaceName = interfaceName;

        _module = ProviderManagerModule(_physicalName);
        _module.load();

        _manager = _module.getProviderManager(_logicalName);
        PEGASUS_ASSERT(_manager != 0);

        _manager->setIndicationCallback(indicationCallback);
    }

    ~ProviderManagerContainer()
    {
        _module.unload();
    }

    ProviderManagerContainer& operator=(
        const ProviderManagerContainer & container)
    {
        if (this == &container)
        {
            return *this;
        }

        _logicalName = container._logicalName;
        _physicalName = container._physicalName;
        _interfaceName = container._interfaceName;

        _module = container._module;
        _manager = container._manager;

        return *this;
    }

    ProviderManager* getProviderManager()
    {
        return _manager;
    }

    const String& getPhysicalName() const
    {
        return _physicalName;
    }

    const String& getLogicalName() const
    {
        return _logicalName;
    }

    const String& getInterfaceName() const
    {
        return _interfaceName;
    }

private:
    String _physicalName;
    String _logicalName;
    String _interfaceName;

    ProviderManagerModule _module;
    ProviderManager* _manager;
};
// END TEMP SECTION


PEGASUS_INDICATION_CALLBACK BasicProviderManagerRouter::_indicationCallback = 0;

// Private, unimplemented constructor
BasicProviderManagerRouter::BasicProviderManagerRouter()
{
}

BasicProviderManagerRouter::BasicProviderManagerRouter(
    PEGASUS_INDICATION_CALLBACK indicationCallback)
{
    PEG_METHOD_ENTER(TRC_PROVIDERMANAGER,
        "BasicProviderManagerRouter::BasicProviderManagerRouter");

    _indicationCallback = indicationCallback;

    PEG_METHOD_EXIT();
}

BasicProviderManagerRouter::~BasicProviderManagerRouter()
{
    PEG_METHOD_ENTER(TRC_PROVIDERMANAGER,
        "BasicProviderManagerRouter::~BasicProviderManagerRouter");
    PEG_METHOD_EXIT();
}

Message* BasicProviderManagerRouter::processMessage(Message * message)
{
    PEG_METHOD_ENTER(TRC_PROVIDERMANAGER,
        "BasicProviderManagerRouter::processMessage");

    CIMRequestMessage* request = dynamic_cast<CIMRequestMessage *>(message);
    PEGASUS_ASSERT(request != 0);

    Message* response = 0;

    //
    // Retrieve the ProviderManager routing information
    //

    CIMInstance providerModule;

    if ((dynamic_cast<CIMOperationRequestMessage*>(request) != 0) ||
        (request->getType() == CIM_EXPORT_INDICATION_REQUEST_MESSAGE) ||
	(request->getType() == CIM_INITIALIZE_PROVIDER_REQUEST_MESSAGE))
    {
        // Provider information is in OperationContext
        ProviderIdContainer pidc = (ProviderIdContainer)
            request->operationContext.get(ProviderIdContainer::NAME);
        providerModule = pidc.getModule();
    }
    else if (dynamic_cast<CIMIndicationRequestMessage*>(request) != 0)
    {
        // Provider information is in CIMIndicationRequestMessage
        CIMIndicationRequestMessage* indReq =
            dynamic_cast<CIMIndicationRequestMessage*>(request);
        providerModule = indReq->providerModule;
    }
    else if (request->getType() == CIM_ENABLE_MODULE_REQUEST_MESSAGE)
    {
        // Provider information is in CIMEnableModuleRequestMessage
        CIMEnableModuleRequestMessage* emReq =
            dynamic_cast<CIMEnableModuleRequestMessage*>(request);
        providerModule = emReq->providerModule;
    }
    else if (request->getType() == CIM_DISABLE_MODULE_REQUEST_MESSAGE)
    {
        // Provider information is in CIMDisableModuleRequestMessage
        CIMDisableModuleRequestMessage* dmReq =
            dynamic_cast<CIMDisableModuleRequestMessage*>(request);
        providerModule = dmReq->providerModule;
    }
    else if (request->getType() == CIM_STOP_ALL_PROVIDERS_REQUEST_MESSAGE)
    {
        // This operation is not provider-specific
    }
    else
    {
        // Error: Unrecognized message type.
        PEGASUS_ASSERT(0);
        CIMResponseMessage* resp = new CIMResponseMessage(
            0, request->messageId, CIMException(),
            request->queueIds.copyAndPop());
        resp->synch_response(request);
        OperationResponseHandler handler(request, resp);
        handler.setStatus(CIM_ERR_FAILED, "Unknown message type.");
        response = resp;
    }

    //
    // Forward the request to the appropriate ProviderManager(s)
    //

    if (request->getType() == CIM_STOP_ALL_PROVIDERS_REQUEST_MESSAGE)
    {
        // Send CIMStopAllProvidersRequestMessage to all ProviderManagers
        for (Uint32 i = 0, n = _providerManagerTable.size(); i < n; i++)
        {
            ProviderManagerContainer* pmc=_providerManagerTable[i];
            Message* resp = pmc->getProviderManager()->processMessage(request);
            if (resp)
            {
                delete resp;
            }
        }

        response = new CIMStopAllProvidersResponseMessage(
            request->messageId,
            CIMException(),
            request->queueIds.copyAndPop());
    }
    else
    {
        // Retrieve the provider interface type
        String interfaceType;
        CIMValue itValue = providerModule.getProperty(
            providerModule.findProperty("InterfaceType")).getValue();
        itValue.get(interfaceType);

        // Look up the appropriate ProviderManager by InterfaceType
        ProviderManager* pm = _lookupProviderManager(interfaceType);
        response = pm->processMessage(request);
    }

    // preserve message key
    response->setKey(request->getKey());

    // set HTTP method in response from request
    response->setHttpMethod(request->getHttpMethod());

    PEG_METHOD_EXIT();
    return response;
}

// ATTN: May need to add interfaceVersion parameter to further constrain lookup
ProviderManager* BasicProviderManagerRouter::_lookupProviderManager(
    const String& interfaceType)
{
    PEG_METHOD_ENTER(TRC_PROVIDERMANAGER,
        "BasicProviderManagerRouter::_lookupProviderManager");

    //
    // Search for this InterfaceType in the table of loaded ProviderManagers
    //
    {
        ReadLock tableLock(_providerManagerTableLock);

        // find provider manager for specified provider interface type
        for(Uint32 i = 0, n = _providerManagerTable.size(); i < n; i++)
        {
            if (interfaceType == _providerManagerTable[i]->getInterfaceName())
            {
                ProviderManagerContainer* pmc=_providerManagerTable[i];
                PEG_METHOD_EXIT();
                return pmc->getProviderManager();
            }
        }
    }

    //
    // Load the ProviderManager for this InterfaceType and add it to the table
    //
    {
        WriteLock tableLock(_providerManagerTableLock);

        // ATTN: this section is a temporary solution to populate the list of
        // enabled provider managers for a given distribution.  It includes
        // another temporary solution for converting a generic file name into
        // a file name useable by each platform.

#if defined(ENABLE_DEFAULT_PROVIDER_MANAGER)
        if (interfaceType == "C++Default")
        {
            ProviderManagerContainer* pmc = new ProviderManagerContainer(
                LIBRARY_NAME_DEFAULTPM, "DEFAULT", "C++Default",
                _indicationCallback);
            _providerManagerTable.append(pmc);
            return pmc->getProviderManager();
        }
#endif

#if defined(ENABLE_CMPI_PROVIDER_MANAGER)
        if (interfaceType == "CMPI")
        {
            ProviderManagerContainer* pmc = new ProviderManagerContainer(
                LIBRARY_NAME_CMPIPM, "CMPI", "CMPI", _indicationCallback);
            _providerManagerTable.append(pmc);
            return pmc->getProviderManager();
        }
#endif

#if defined(ENABLE_JMPI_PROVIDER_MANAGER)
        if (interfaceType == "JMPI")
        {
            ProviderManagerContainer* pmc = new ProviderManagerContainer(
                LIBRARY_NAME_JMPIPM, "JMPI", "JMPI", _indicationCallback);
            _providerManagerTable.append(pmc);
            return pmc->getProviderManager();
        }
#endif
        // END TEMP SECTION
    }

    // Error: ProviderManager not found for the specified interface type
    PEGASUS_ASSERT(0);
    PEG_METHOD_EXIT();
    return 0;
}

void BasicProviderManagerRouter::unload_idle_providers(void)
{
    PEG_METHOD_ENTER(TRC_PROVIDERMANAGER,
        "BasicProviderManagerRouter::unload_idle_providers");

    for(Uint32 i = 0, n = _providerManagerTable.size(); i < n; i++)
    {
        ProviderManagerContainer* pmc = _providerManagerTable[i];
        pmc->getProviderManager()->unload_idle_providers();
    }

    PEG_METHOD_EXIT();
}

PEGASUS_NAMESPACE_END
