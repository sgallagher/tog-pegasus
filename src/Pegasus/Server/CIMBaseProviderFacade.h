//%/////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001 The Open group, BMC Software, Tivoli Systems, IBM
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
//
// Modified By: Roger Kumpf, Hewlett-Packard Company (roger_kumpf@hp.com)
//              Nitin Upasani, Hewlett-Packard Company (Nitin_Upasani@hp.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_CIMBaseProviderFacade_h
#define Pegasus_CIMBaseProviderFacade_h

#include <Pegasus/Common/Config.h>

#include <Pegasus/Provider/CIMBaseProvider.h>
#include <Pegasus/Provider/CIMInstanceProvider.h>
#include <Pegasus/Provider/CIMClassProvider.h>
#include <Pegasus/Provider/CIMAssociationProvider.h>
#include <Pegasus/Provider/CIMPropertyProvider.h>
#include <Pegasus/Provider/CIMMethodProvider.h>
#include <Pegasus/Provider/CIMQueryProvider.h>
#include <Pegasus/Provider/CIMIndicationProvider.h>
#include <Pegasus/Provider/CIMIndicationConsumer.h>

PEGASUS_NAMESPACE_BEGIN

class PEGASUS_SERVER_LINKAGE CIMBaseProviderFacade :
	public CIMInstanceProvider,
	public CIMClassProvider,
	public CIMAssociationProvider,
	public CIMPropertyProvider,
	public CIMMethodProvider,
	public CIMQueryProvider,
	public CIMIndicationProvider,
	public CIMIndicationConsumer
{
public:
	CIMBaseProviderFacade(CIMBaseProvider * provider);
	virtual ~CIMBaseProviderFacade(void);

	// CIMBaseProvider interface
	virtual void initialize(CIMOMHandle & cimom);
	virtual void terminate(void);

	// CIMInstanceProvider interface
	virtual void getInstance(
		const OperationContext & context,
		const CIMReference & instanceReference,
		const Uint32 flags,
		const Array<String> & propertyList,
		ResponseHandler<CIMInstance> & handler);

	virtual void enumerateInstances(
		const OperationContext & context,
		const CIMReference & classReference,
		const Uint32 flags,
		const Array<String> & propertyList,
		ResponseHandler<CIMInstance> & handler);

	virtual void enumerateInstanceNames(
		const OperationContext & context,
		const CIMReference & classReference,
		ResponseHandler<CIMReference> & handler);

	virtual void modifyInstance(
		const OperationContext & context,
		const CIMReference & instanceReference,
		const CIMInstance & instanceObject,
		const Uint32 flags,
		const Array<String> & propertyList,
		ResponseHandler<CIMInstance> & handler);

	virtual void createInstance(
		const OperationContext & context,
		const CIMReference & instanceReference,
		const CIMInstance & instanceObject,
		ResponseHandler<CIMReference> & handler);

	virtual void deleteInstance(
		const OperationContext & context,
		const CIMReference & instanceReference,
		ResponseHandler<CIMInstance> & handler);

	// CIMClassProvider interface
	virtual void getClass(
		const OperationContext & context,
		const CIMReference & classReference,
		const Uint32 flags,
		const Array<String> & propertyList,
		ResponseHandler<CIMClass> & handler);

	virtual void enumerateClasses(
		const OperationContext & context,
		const CIMReference & classReference,
		const Uint32 flags,
		ResponseHandler<CIMClass> & handler);

	virtual void enumerateClassNames(
		const OperationContext & context,
		const CIMReference & classReference,
		const Uint32 flags,
		ResponseHandler<CIMReference> & handler);

	virtual void modifyClass(
		const OperationContext & context,
		const CIMReference & classReference,
		const CIMClass & classObject,
		ResponseHandler<CIMClass> & handler);

	virtual void createClass(
		const OperationContext & context,
		const CIMReference & classReference,
		const CIMClass & classObject,
		ResponseHandler<CIMClass> & handler);

	virtual void deleteClass(
		const OperationContext & context,
		const CIMReference & classReference,
		ResponseHandler<CIMClass> & handler);

	// CIMAssociationProvider interface
	virtual void associators(
		const OperationContext & context,
		const CIMReference & objectName,
		const String & associationClass,
		const String & resultClass,
		const String & role,
		const String & resultRole,
		const Uint32 flags,
		const Array<String> & propertyList,
		ResponseHandler<CIMObject> & handler);

	virtual void associatorNames(
		const OperationContext & context,
		const CIMReference & objectName,
		const String & associationClass,
		const String & resultClass,
		const String & role,
		const String & resultRole,
		ResponseHandler<CIMReference> & handler);

	virtual void references(
		const OperationContext & context,
		const CIMReference & objectName,
		const String & resultClass,
		const String & role,
		const Uint32 flags,
		const Array<String> & propertyList,
		ResponseHandler<CIMObject> & handler);

	virtual void referenceNames(
		const OperationContext & context,
		const CIMReference & objectName,
		const String & resultClass,
		const String & role,
		ResponseHandler<CIMReference> & handler);

	// CIMPropertyProvider interface
	virtual void getProperty(
		const OperationContext & context,
		const CIMReference & instanceReference,
		const String & propertyName,
		ResponseHandler<CIMValue> & handler);

	virtual void setProperty(
		const OperationContext & context,
		const CIMReference & instanceReference,
		const String & propertyName,
		const CIMValue & newValue,
		ResponseHandler<CIMValue> & handler);

	// CIMMethodProviderFacade
	virtual void invokeMethod(
		const OperationContext & context,
		const CIMReference & objectReference,
		const String & methodName,
		const Array<CIMParamValue> & inParameters,
		Array<CIMParamValue> & outParameters,
		ResponseHandler<CIMValue> & handler);

	// CIMQueryProvider interface
	virtual void executeQuery(
		const OperationContext & context,
		const String & queryLanguage,
		const String & query,
		ResponseHandler<CIMObject> & handler);

	// CIMIndicationProvider interface
	virtual void provideIndication(
		const OperationContext & context,
		const CIMReference & classReference,
		const CIMDateTime & minimumInterval,
		const CIMDateTime & maximumInterval,
		const Array<String> & propertyList,
		ResponseHandler<CIMInstance> & handler);
	
	virtual void updateIndication(
		const OperationContext & context,
		const CIMReference & classReference,
		const CIMDateTime & minimumInterval,
		const CIMDateTime & maximumInterval,
		const Array<String> & propertyList,
		ResponseHandler<CIMInstance> & handler);

	virtual void cancelIndication(
		const OperationContext & context,
		const CIMReference & classReference,
		ResponseHandler<CIMInstance> & handler);

	virtual void checkIndication(
		const OperationContext & context,
		const CIMReference & classReference,
		const Array<String> & propertyList,
		ResponseHandler<CIMInstance> & handler);

	virtual void enableIndication(
		const OperationContext & context,
		const String & nameSpace,
		const Array<String> & classNames,
		const String & providerName,
		const Array<String> & propertyList,
		const Uint16 repeatNotificationPolicy,
		const String & otherRepeatNotificationPolicy,
		const CIMDateTime & repeatNotificationInterval,
		const CIMDateTime & repeatNotificationGap,
		const Uint16 repeatNotificationCount,
		const String & condition,
		const String & queryLanguage,
		const CIMInstance & subscription,
		ResponseHandler<CIMInstance> & handler);

	virtual void disableIndication(
		const OperationContext & context,
		const String & nameSpace,
		const Array<String> & classNames,
		const String & providerName,
		const CIMInstance & subscription,
		ResponseHandler<CIMInstance> & handler);

	virtual void modifyIndication(
		const OperationContext & context,
		const String & nameSpace,
		const Array<String> & classNames,
		const String & providerName,
		const Array<String> & propertyList,
		const Uint16 repeatNotificationPolicy,
		const String & otherRepeatNotificationPolicy,
		const CIMDateTime & repeatNotificationInterval,
		const CIMDateTime & repeatNotificationGap,
		const Uint16 repeatNotificationCount,
		const String & condition,
		const String & queryLanguage,
		const CIMInstance & subscription,
		ResponseHandler<CIMInstance> & handler);

	// CIMIndicationConsumer interface
	virtual void handleIndication(
		const OperationContext & context,
		const CIMInstance & indication,
		ResponseHandler<CIMInstance> & handler);

	virtual void handleIndication(
                const OperationContext & context,
                const String & url,
                const CIMInstance& indicationInstance) { }

protected:
	CIMBaseProvider * _provider;

};

PEGASUS_NAMESPACE_END

#endif
