//%////////////////////////////////////////////////////////////////////////////
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
//=============================================================================
//
// Author: Yi Zhou, Hewlett-Packard Company (yi_zhou@hp.com)
//
// Modified By:
//
//%////////////////////////////////////////////////////////////////////////////


#ifndef Pegasus_ProviderRegistrationManager_h
#define Pegasus_ProviderRegistrationManager_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/String.h>
#include <Pegasus/Common/MessageQueueService.h>
#include <Pegasus/Server/ServiceCIMOMHandle.h>
#include <Pegasus/Repository/CIMRepository.h>
#include <Pegasus/Provider/CIMInstanceProvider.h>


PEGASUS_NAMESPACE_BEGIN

struct RegistrationTable;

struct ClassNamespaceTable;
 
class PEGASUS_SERVER_LINKAGE ProviderRegistrationManager
{
public:
	//ProviderRegistrationManager(ServiceCIMOMHandle & cimom);
	ProviderRegistrationManager(CIMRepository* repository);

	virtual ~ProviderRegistrationManager(void);

	Boolean lookupInstanceProvider(
		const String & nameSpace, 
		const String & className,
	        CIMInstance & provider, 
		CIMInstance & providerModule); 

	Boolean lookupMethodProvider(
		const String & nameSpace, 
		const String & className, 
	        const String & method, 
	        CIMInstance & provider, 
		CIMInstance & providerModule); 

	Boolean lookupAssociationProvider(
		const String & nameSpace, 
		const String & className,
		String& providerName, 
		String& location, 
		Uint16& status);

	Boolean getIndicationProviders(
		const String & nameSpace, 
		const String & className,
		const Array<String>& requiredProperties,
		Array<CIMInstance> & provider,
		Array<CIMInstance> & providerModule); 

	CIMInstance getInstance(const CIMReference & ref);

	Array<CIMNamedInstance> enumerateInstances(const CIMReference & ref);

	Array<CIMReference> enumerateInstanceNames(const CIMReference & ref);

//	void modifyInstance(const CIMReference & ref, const CIMInstance & instance);

	CIMReference createInstance(const CIMReference & ref, 
		const CIMInstance & instance);

//	void initialRegistrationTable();

	void deleteInstance(const CIMReference & ref);

	Array<Uint16> getProviderModuleStatus(const String & providerModuleName);

	Boolean setProviderModuleStatus(const String & providerModuleName, Array<Uint16> status);

protected:

//	ServiceCIMOMHandle _cimom;

	CIMRepository * _repository;

	/**
	HashTable to store instances of PG_ProviderModule, instances of
	PG_Provider, and instances of PG_ProviderCapability 
	*/
	RegistrationTable* _registrationTable;

	String _generateKey(const String & namespaceName, 
		const String & className,
		const String & providerType);

	String _generateKey(const String & namespaceName, 
		const String & className,
		const String & supportedMethod,
		const String & providerType);

/*
	Array<ProviderModule> _providers;
	Array<ProviderStatusEntry> _providerST;
*/

private:

	void _initialRegistrationTable();

	void _addInstancesToTable(const String & key,
		const Array<CIMInstance> & instances); 

	void _addClassNamespaceInfoToTable(const String & instanceID,
		const String & namespaceName, const String & className, 
		const Array<String> & supportedMethods);

//	Uint32 _stopProvider(Uint32 providerIndex);

};

PEGASUS_NAMESPACE_END

#endif
