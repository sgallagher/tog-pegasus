//%/////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001, 2002 BMC Software, Hewlett-Packard Company, IBM,
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
// Author: Yi Zhou, Hewlett-Packard Company (yi_zhou@hp.com)
//
// Modified By: Roger Kumpf, Hewlett-Packard Company (roger_kumpf@hp.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#include <Pegasus/Common/Config.h>
#include "Linkage.h"

#include <Pegasus/Provider/CIMInstanceProvider.h>
#include <Pegasus/Provider/CIMMethodProvider.h>
#include <Pegasus/Common/ModuleController.h>

#include <Pegasus/Server/ProviderRegistrationManager/ProviderRegistrationManager.h>

PEGASUS_NAMESPACE_BEGIN

class PEGASUS_PROVREGPROVIDER_LINKAGE ProviderRegistrationProvider :
    public CIMInstanceProvider,
    public CIMMethodProvider
{
public:

      class callback_data
      {
         public:

            Message *reply;
            Semaphore client_sem;
            ProviderRegistrationProvider & cimom_handle;

            callback_data(ProviderRegistrationProvider *handle)
               : reply(0), client_sem(0), cimom_handle(*handle)
            {
            }
            ~callback_data()
            {
               delete reply;
            }

            Message *get_reply(void)
            {
               Message *ret = reply;
               reply = NULL;
               return ret;
            }

         private:
            callback_data(void);
      };


    ProviderRegistrationProvider(
        ProviderRegistrationManager* providerRegistrationManager);
    virtual ~ProviderRegistrationProvider(void);

ProviderRegistrationProvider & operator=(const ProviderRegistrationProvider & handle);

    // CIMBaseProvider interface
    virtual void initialize(CIMOMHandle& cimom);
    virtual void terminate(void);

    // CIMInstanceProvider interface
    virtual void getInstance(
        const OperationContext & context,
        const CIMObjectPath & instanceReference,
        const Uint32 flags,
        const CIMPropertyList & propertyList,
        ResponseHandler<CIMInstance> & handler);

    virtual void enumerateInstances(
        const OperationContext & context,
        const CIMObjectPath & classReference,
        const Uint32 flags,
        const CIMPropertyList & propertyList,
        ResponseHandler<CIMInstance> & handler);

    virtual void enumerateInstanceNames(
        const OperationContext & context,
        const CIMObjectPath & classReference,
        ResponseHandler<CIMObjectPath> & handler);

    virtual void modifyInstance(
        const OperationContext & context,
        const CIMObjectPath & instanceReference,
        const CIMInstance & instanceObject,
        const Uint32 flags,
        const CIMPropertyList & propertyList,
        ResponseHandler<void> & handler);

    virtual void createInstance(
        const OperationContext & context,
        const CIMObjectPath & instanceReference,
        const CIMInstance & instanceObject,
        ResponseHandler<CIMObjectPath> & handler);

    virtual void deleteInstance(
        const OperationContext & context,
        const CIMObjectPath & instanceReference,
        ResponseHandler<void> & handler);

    // CIMMethodProvider interface
    virtual void invokeMethod(
        const OperationContext & context,
        const CIMObjectPath & objectReference,
        const String & methodName,
        const Array<CIMParamValue> & inParameters,
        Array<CIMParamValue> & outParameters,
        ResponseHandler<CIMValue> & handler);

protected:

    pegasus_internal_identity _id;
    ModuleController * _controller;
    ModuleController::client_handle *_client_handle;

    ProviderRegistrationManager* _providerRegistrationManager;

    MessageQueueService * _getProviderManagerService();
    Array<Uint16> _sendDisableMessageToProviderManager(
	CIMDisableModuleRequestMessage * notify_req);
    Array<Uint16> _sendEnableMessageToProviderManager(
	CIMEnableModuleRequestMessage * notify_req);
    MessageQueueService * _getIndicationService();
    void _sendTerminationMessageToSubscription(
	const CIMObjectPath & ref, const String & moduleName);
};

PEGASUS_NAMESPACE_END
