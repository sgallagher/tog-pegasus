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
// Author: Chip Vincent (cvincent@us.ibm.com)
//
// Modified By: Mike Brasher (mbrasher@bmc.com)
//              Nitin Upasani, Hewlett-Packard Company (Nitin_Upasani@hp.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_CIMOMHandle_h
#define Pegasus_CIMOMHandle_h

#include <fstream>
#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/MessageQueue.h>
#include <Pegasus/Common/CIMMessage.h>
#include <Pegasus/Common/CIMObject.h>
#include <Pegasus/Repository/CIMRepository.h>

PEGASUS_NAMESPACE_BEGIN

class PEGASUS_COMMON_LINKAGE CIMOMHandle
{
public:

	/** */
	CIMOMHandle(void);

	/** */
	CIMOMHandle(MessageQueue* outputQueue, CIMRepository * repository);

	/** */
	virtual ~CIMOMHandle(void);

	CIMOMHandle & operator=(const CIMOMHandle & handle);

	const CIMRepository * getRepository(void) const { return(_repository); }
	
	CIMRepository * getRepository(void) { return(_repository); }
	
	/** */
	virtual CIMClass getClass(
		const String& nameSpace,
		const String& className,
		Boolean localOnly = true,
		Boolean includeQualifiers = true,
		Boolean includeClassOrigin = false,
		const Array<String>& propertyList = EmptyStringArray());

	/** */
	virtual CIMInstance getInstance(
		const String& nameSpace,
		const CIMReference& instanceName,
		Boolean localOnly = true,
		Boolean includeQualifiers = false,
		Boolean includeClassOrigin = false,
		const Array<String>& propertyList = EmptyStringArray());

	/** */
	virtual void deleteClass(
		const String& nameSpace,
		const String& className);

	/** */
	virtual void deleteInstance(
		const String& nameSpace,
		const CIMReference& instanceName);

	/** */
	virtual void createClass(
		const String& nameSpace,
		const CIMClass& newClass);

	/** */
	virtual void createInstance(
		const String& nameSpace,
		const CIMInstance& newInstance);

	/** */
	virtual void modifyClass(
		const String& nameSpace,
		const CIMClass& modifiedClass);

	/** */
	virtual void modifyInstance(
		const String& nameSpace,
		const CIMInstance& modifiedInstance);

	/** */
	virtual Array<CIMClass> enumerateClasses(
		const String& nameSpace,
		const String& className = String::EMPTY,
		Boolean deepInheritance = false,
		Boolean localOnly = true,
		Boolean includeQualifiers  = true,
		Boolean includeClassOrigin = false);

	/** */
	virtual Array<String> enumerateClassNames(
		const String& nameSpace,
		const String& className = String::EMPTY,
		Boolean deepInheritance = false);

	/** */
	virtual Array<CIMInstance> enumerateInstances(
		const String& nameSpace,
		const String& className,
		Boolean deepInheritance = true,
		Boolean localOnly = true,
		Boolean includeQualifiers = false,
		Boolean includeClassOrigin = false,
		const Array<String>& propertyList = EmptyStringArray());

	/** */
	virtual Array<CIMReference> enumerateInstanceNames(
		const String& nameSpace,
		const String& className);

	/** */
	virtual Array<CIMInstance> execQuery(
		const String& queryLanguage,
		const String& query);

	/** */
	virtual Array<CIMObjectWithPath> associators(
		const String& nameSpace,
		const CIMReference& objectName,
		const String& assocClass = String::EMPTY,
		const String& resultClass = String::EMPTY,
		const String& role = String::EMPTY,
		const String& resultRole = String::EMPTY,
		Boolean includeQualifiers = false,
		Boolean includeClassOrigin = false,
		const Array<String>& propertyList = EmptyStringArray());

	/** */
	virtual Array<CIMReference> associatorNames(
		const String& nameSpace,
		const CIMReference& objectName,
		const String& assocClass = String::EMPTY,
		const String& resultClass = String::EMPTY,
		const String& role = String::EMPTY,
		const String& resultRole = String::EMPTY);
	/** */
	virtual Array<CIMObjectWithPath> references(
		const String& nameSpace,
		const CIMReference& objectName,
		const String& resultClass = String::EMPTY,
		const String& role = String::EMPTY,
		Boolean includeQualifiers = false,
		Boolean includeClassOrigin = false,
		const Array<String>& propertyList = EmptyStringArray());

	/** */
	virtual Array<CIMReference> referenceNames(
		const String& nameSpace,
		const CIMReference& objectName,
		const String& resultClass = String::EMPTY,
		const String& role = String::EMPTY);

	/** */
	virtual CIMValue getProperty(
		const String& nameSpace,
		const CIMReference& instanceName,
		const String& propertyName);

	/** */
	virtual void setProperty(
		const String& nameSpace,
		const CIMReference& instanceName,
		const String& propertyName,
		const CIMValue& newValue = CIMValue());

	/** */
	virtual CIMValue invokeMethod(
		const String& nameSpace,
		const CIMReference& instanceName,
		const String& methodName,
		const Array<CIMParamValue>& inParameters,
		Array<CIMParamValue>& outParameters);

protected:

	Message * _waitForResponse(
		const Uint32 messageType,
		const Uint32 messageKey,
		const Uint32 timeout = 0xffffffff);

	void _checkError(
		const CIMResponseMessage * responseMessage);

protected:	
	MessageQueue* _outputQueue;
	MessageQueue* _inputQueue;

	CIMRepository * _repository;
};

PEGASUS_NAMESPACE_END

#endif /* Pegasus_CIMOMHandle_h */
	
