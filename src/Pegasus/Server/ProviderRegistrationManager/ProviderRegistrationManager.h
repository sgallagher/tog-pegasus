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
// Modified By: Carol Ann Krug Graves, Hewlett-Packard Company
//                  (carolann_graves@hp.com)
//
//%////////////////////////////////////////////////////////////////////////////


#ifndef Pegasus_ProviderRegistrationManager_h
#define Pegasus_ProviderRegistrationManager_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Server/ProviderRegistrationManager/Linkage.h>
#include <Pegasus/Common/String.h>
#include <Pegasus/Common/MessageQueueService.h>
#include <Pegasus/Repository/CIMRepository.h>
#include <Pegasus/Provider/CIMInstanceProvider.h>
#include <Pegasus/Common/ModuleController.h>
#include <Pegasus/Common/CIMMessage.h>


PEGASUS_NAMESPACE_BEGIN

struct RegistrationTable;

struct ClassNamespaceTable;
 
class PEGASUS_PRM_LINKAGE ProviderRegistrationManager
{
public:
	//ProviderRegistrationManager(ServiceCIMOMHandle & cimom);
	ProviderRegistrationManager(CIMRepository* repository);

	virtual ~ProviderRegistrationManager(void);

	Boolean lookupInstanceProvider(
		const CIMNamespaceName & nameSpace, 
		const CIMName & className,
	        CIMInstance & provider, 
		CIMInstance & providerModule,
                Boolean is_assoc = false); 

	Boolean lookupMethodProvider(
		const CIMNamespaceName & nameSpace, 
		const CIMName & className, 
	        const CIMName & method, 
	        CIMInstance & provider, 
		CIMInstance & providerModule); 

	Boolean lookupAssociationProvider(
		const CIMNamespaceName & nameSpace, 
		const CIMName & className,
                const CIMName & assocClassName,
                const CIMName & resultClassName,
                Array<CIMInstance>& provider, 
                Array<CIMInstance>& providerModule);

	Boolean getIndicationProviders(
		const CIMNamespaceName & nameSpace, 
		const CIMName & className,
		const CIMPropertyList & requiredProperties,
		Array<CIMInstance> & provider,
		Array<CIMInstance> & providerModule); 

	CIMInstance getInstance(const CIMObjectPath & ref);

	Array<CIMInstance> enumerateInstances(const CIMObjectPath & ref);

	Array<CIMObjectPath> enumerateInstanceNames(const CIMObjectPath & ref);

	void modifyInstance(const CIMObjectPath & ref, 
			    const CIMInstance & instance,
			    const Boolean includeQualifiers,
			    const Array<CIMName> & propertyList);

	CIMObjectPath createInstance(const CIMObjectPath & ref, 
		const CIMInstance & instance);

	void deleteInstance(const CIMObjectPath & ref);

	Array<Uint16> getProviderModuleStatus(const String & providerModuleName);

	Boolean setProviderModuleStatus(const String & providerModuleName, Array<Uint16> status);

   	enum Operation {OP_CREATE = 1, OP_DELETE = 2, OP_MODIFY = 3};

protected:

	CIMRepository * _repository;

	/**
	HashTable to store instances of PG_ProviderModule, instances of
	PG_Provider, and instances of PG_ProviderCapability 
	*/
	RegistrationTable* _registrationTable;

	String _generateKey(const String & name, 
		const String & provider);

	String _generateKey(const CIMNamespaceName & namespaceName, 
		const CIMName & className,
		const String & providerType);

	String _generateKey(const CIMNamespaceName & namespaceName, 
		const CIMName & className,
		const String & supportedMethod,
		const String & providerType);

	MessageQueueService * _getIndicationService();

	void _sendMessageToSubscription(
		CIMNotifyProviderRegistrationRequestMessage * notify);

private:

	void _initialRegistrationTable();

	void _addInstancesToTable(const String & key,
		const Array<CIMInstance> & instances); 

	void _addInitialInstancesToTable(const String & key,
		const Array<CIMInstance> & instances); 

	void _addClassNamespaceInfoToTable(const String & instanceID,
		const String & namespaceName, const String & className, 
		const Array<String> & supportedMethods);

	void _getInstances(const String & providerName,
		const String & moduleName,
		CIMInstance & providerInstance,
		CIMInstance & moduleInstance);

	void _getPropertyNames(const CIMInstance & instance,
		CIMPropertyList & propertyNames);

	void _sendDeleteNotifyMessage(const CIMInstance & instance);

	void _sendModifyNotifyMessage(const CIMInstance & instance,
				      const CIMInstance & origInstance);

	CIMObjectPath _createInstance(const CIMObjectPath & ref, 
		const CIMInstance & instance, Operation flag);

	void _deleteInstance(const CIMObjectPath & ref, Operation flag);

};

PEGASUS_NAMESPACE_END

#endif
