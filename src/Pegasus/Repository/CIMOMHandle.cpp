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
// Author: Mike Brasher (mbrasher@bmc.com)
//
// Modified By:
//
//%/////////////////////////////////////////////////////////////////////////////

#include <Pegasus/Common/XmlWriter.h>
#include <Pegasus/Common/CIMMessage.h>
#include <Pegasus/Common/Destroyer.h>
#include "CIMOMHandle.h"

PEGASUS_NAMESPACE_BEGIN

CIMOMHandle::CIMOMHandle(
    MessageQueue* outputQueue,
    CIMRepository* repository) 
    : 
    _outputQueue(outputQueue),
    _repository(repository)
{
    _inputQueue = new MessageQueue;
}

CIMOMHandle::~CIMOMHandle()
{
    delete _inputQueue;
}

CIMClass CIMOMHandle::getClass(
    const String& nameSpace,
    const String& className,
    Boolean localOnly,
    Boolean includeQualifiers,
    Boolean includeClassOrigin,
    const Array<String>& propertyList)
{
    Message* request = new CIMGetClassRequestMessage(
	XmlWriter::getNextMessageId(),
        nameSpace,
        className,
	localOnly,
	includeQualifiers,
	includeClassOrigin,
	propertyList,
        _inputQueue->getQueueId());

    _outputQueue->enqueue(request);

    Message* message = _inputQueue->dequeue();

    CIMGetClassResponseMessage* response = (CIMGetClassResponseMessage*)message;

    _checkError(response);

    return response->cimClass;
}

CIMInstance CIMOMHandle::getInstance(
    const String& nameSpace,
    const CIMReference& instanceName,
    Boolean localOnly,
    Boolean includeQualifiers,
    Boolean includeClassOrigin,
    const Array<String>& propertyList)
{
    Message* request = new CIMGetInstanceRequestMessage(
	XmlWriter::getNextMessageId(),
        nameSpace,
        instanceName,
	localOnly,
	includeQualifiers,
	includeClassOrigin,
	propertyList,
        _inputQueue->getQueueId());

    _outputQueue->enqueue(request);

    Message* message = _inputQueue->dequeue();

    CIMGetInstanceResponseMessage* response 
	= (CIMGetInstanceResponseMessage*)message;

    _checkError(response);

    return response->cimInstance;
}


void CIMOMHandle::deleteClass(
    const String& nameSpace,
    const String& className)
{
    Message* request = new CIMDeleteClassRequestMessage(
	XmlWriter::getNextMessageId(),
        nameSpace,
        className,
        _inputQueue->getQueueId());

    _outputQueue->enqueue(request);

    Message* message = _inputQueue->dequeue();

    CIMDeleteClassResponseMessage* response 
	= (CIMDeleteClassResponseMessage*)message;

    _checkError(response);
}


void CIMOMHandle::deleteInstance(
    const String& nameSpace,
    const CIMReference& instanceName)
{
    Message* request = new CIMDeleteInstanceRequestMessage(
	XmlWriter::getNextMessageId(),
        nameSpace,
        instanceName,
        _inputQueue->getQueueId());

    _outputQueue->enqueue(request);

    Message* message = _inputQueue->dequeue();

    CIMDeleteInstanceResponseMessage* response 
	= (CIMDeleteInstanceResponseMessage*)message;

    _checkError(response);
}


void CIMOMHandle::createClass(
    const String& nameSpace,
    const CIMClass& newClass)
{
    Message* request = new CIMCreateClassRequestMessage(
	XmlWriter::getNextMessageId(),
        nameSpace,
        newClass,
        _inputQueue->getQueueId());

    _outputQueue->enqueue(request);

    Message* message = _inputQueue->dequeue();

    CIMCreateClassResponseMessage* response 
	= (CIMCreateClassResponseMessage*)message;

    _checkError(response);
}

void CIMOMHandle::createInstance(
    const String& nameSpace,
    const CIMInstance& newInstance)
{
    Message* request = new CIMCreateInstanceRequestMessage(
	XmlWriter::getNextMessageId(),
        nameSpace,
        newInstance,
        _inputQueue->getQueueId());

    _outputQueue->enqueue(request);

    Message* message = _inputQueue->dequeue();

    CIMCreateInstanceResponseMessage* response 
	= (CIMCreateInstanceResponseMessage*)message;

    _checkError(response);
}


void CIMOMHandle::modifyClass(
    const String& nameSpace,
    const CIMClass& modifiedClass)
{
    Message* request = new CIMModifyClassRequestMessage(
	XmlWriter::getNextMessageId(),
        nameSpace,
        modifiedClass,
        _inputQueue->getQueueId());

    _outputQueue->enqueue(request);

    Message* message = _inputQueue->dequeue();

    CIMModifyClassResponseMessage* response 
	= (CIMModifyClassResponseMessage*)message;

    _checkError(response);
}


void CIMOMHandle::modifyInstance(
    const String& nameSpace,
    const CIMInstance& modifiedInstance)
{
    Message* request = new CIMModifyInstanceRequestMessage(
	XmlWriter::getNextMessageId(),
        nameSpace,
        modifiedInstance,
        _inputQueue->getQueueId());

    _outputQueue->enqueue(request);

    Message* message = _inputQueue->dequeue();

    CIMModifyInstanceResponseMessage* response 
	= (CIMModifyInstanceResponseMessage*)message;

    _checkError(response);
}


Array<CIMClass> CIMOMHandle::enumerateClasses(
    const String& nameSpace,
    const String& className,
    Boolean deepInheritance,
    Boolean localOnly,
    Boolean includeQualifiers,
    Boolean includeClassOrigin)
{
    Message* request = new CIMEnumerateClassesRequestMessage(
	XmlWriter::getNextMessageId(),
        nameSpace,
	className,
	deepInheritance,
	localOnly,
	includeQualifiers,
	includeClassOrigin,
        _inputQueue->getQueueId());

    _outputQueue->enqueue(request);

    Message* message = _inputQueue->dequeue();

    CIMEnumerateClassesResponseMessage* response 
	= (CIMEnumerateClassesResponseMessage*)message;

    _checkError(response);

    return response->cimClasses;
}

Array<String> CIMOMHandle::enumerateClassNames(
    const String& nameSpace,
    const String& className,
    Boolean deepInheritance)
{
    Message* request = new CIMEnumerateClassNamesRequestMessage(
	XmlWriter::getNextMessageId(),
        nameSpace,
	className,
	deepInheritance,
        _inputQueue->getQueueId());

    _outputQueue->enqueue(request);

    Message* message = _inputQueue->dequeue();

    CIMEnumerateClassNamesResponseMessage* response 
	= (CIMEnumerateClassNamesResponseMessage*)message;

    _checkError(response);

    return response->classNames;
}


Array<CIMInstance> CIMOMHandle::enumerateInstances(
    const String& nameSpace,
    const String& className,
    Boolean deepInheritance,
    Boolean localOnly,
    Boolean includeQualifiers,
    Boolean includeClassOrigin,
    const Array<String>& propertyList)
{
    Message* request = new CIMEnumerateInstancesRequestMessage(
	XmlWriter::getNextMessageId(),
        nameSpace,
	className,
	deepInheritance,
	localOnly,
	includeQualifiers,
	includeClassOrigin,
	propertyList,
        _inputQueue->getQueueId());

    _outputQueue->enqueue(request);

    Message* message = _inputQueue->dequeue();

    CIMEnumerateInstancesResponseMessage* response 
	= (CIMEnumerateInstancesResponseMessage*)message;

    _checkError(response);

    return response->cimInstances;
}


Array<CIMReference> CIMOMHandle::enumerateInstanceNames(
    const String& nameSpace,
    const String& className)
{
    Message* request = new CIMEnumerateInstanceNamesRequestMessage(
	XmlWriter::getNextMessageId(),
        nameSpace,
	className,
        _inputQueue->getQueueId());

    _outputQueue->enqueue(request);

    Message* message = _inputQueue->dequeue();

    CIMEnumerateInstanceNamesResponseMessage* response 
	= (CIMEnumerateInstanceNamesResponseMessage*)message;

    _checkError(response);

    return response->instanceNames;
}

Array<CIMInstance> CIMOMHandle::execQuery(
    const String& queryLanguage,
    const String& query)
{
    Message* request = new CIMExecQueryRequestMessage(
	XmlWriter::getNextMessageId(),
        queryLanguage,
	query,
        _inputQueue->getQueueId());

    _outputQueue->enqueue(request);

    Message* message = _inputQueue->dequeue();

    CIMExecQueryResponseMessage* response 
	= (CIMExecQueryResponseMessage*)message;

    _checkError(response);

    return response->cimInstances;
}

Array<CIMObjectWithPath> CIMOMHandle::associators(
    const String& nameSpace,
    const CIMReference& objectName,
    const String& assocClass,
    const String& resultClass,
    const String& role,
    const String& resultRole,
    Boolean includeQualifiers,
    Boolean includeClassOrigin,
    const Array<String>& propertyList)
{
    Message* request = new CIMAssociatorsRequestMessage(
	XmlWriter::getNextMessageId(),
        nameSpace,
	objectName,
	assocClass,
	resultClass,
	role,
	resultRole,
	includeQualifiers,
	includeClassOrigin,
	propertyList,
        _inputQueue->getQueueId());

    _outputQueue->enqueue(request);

    Message* message = _inputQueue->dequeue();

    CIMAssociatorsResponseMessage* response 
	= (CIMAssociatorsResponseMessage*)message;

    _checkError(response);

    return response->cimObjects;
}

Array<CIMReference> CIMOMHandle::associatorNames(
    const String& nameSpace,
    const CIMReference& objectName,
    const String& assocClass,
    const String& resultClass,
    const String& role,
    const String& resultRole)
{
    Message* request = new CIMAssociatorNamesRequestMessage(
	XmlWriter::getNextMessageId(),
        nameSpace,
	objectName,
	assocClass,
	resultClass,
	role,
	resultRole,
        _inputQueue->getQueueId());

    _outputQueue->enqueue(request);

    Message* message = _inputQueue->dequeue();

    CIMAssociatorNamesResponseMessage* response 
	= (CIMAssociatorNamesResponseMessage*)message;

    _checkError(response);

    return response->objectNames;
}

Array<CIMObjectWithPath> CIMOMHandle::references(
    const String& nameSpace,
    const CIMReference& objectName,
    const String& resultClass,
    const String& role,
    Boolean includeQualifiers,
    Boolean includeClassOrigin,
    const Array<String>& propertyList)
{
    Message* request = new CIMReferencesRequestMessage(
	XmlWriter::getNextMessageId(),
        nameSpace,
	objectName,
	resultClass,
	role,
	includeQualifiers,
	includeClassOrigin,
	propertyList,
        _inputQueue->getQueueId());

    _outputQueue->enqueue(request);

    Message* message = _inputQueue->dequeue();

    CIMReferencesResponseMessage* response 
	= (CIMReferencesResponseMessage*)message;

    _checkError(response);

    return response->cimObjects;
}

Array<CIMReference> CIMOMHandle::referenceNames(
    const String& nameSpace,
    const CIMReference& objectName,
    const String& resultClass,
    const String& role)
{
    Message* request = new CIMReferenceNamesRequestMessage(
	XmlWriter::getNextMessageId(),
        nameSpace,
	objectName,
	resultClass,
	role,
        _inputQueue->getQueueId());

    _outputQueue->enqueue(request);

    Message* message = _inputQueue->dequeue();

    CIMReferenceNamesResponseMessage* response 
	= (CIMReferenceNamesResponseMessage*)message;

    _checkError(response);

    return response->objectNames;
}

CIMValue CIMOMHandle::getProperty(
    const String& nameSpace,
    const CIMReference& instanceName,
    const String& propertyName)
{
    Message* request = new CIMGetPropertyRequestMessage(
	XmlWriter::getNextMessageId(),
        nameSpace,
	instanceName,
	propertyName,
        _inputQueue->getQueueId());

    _outputQueue->enqueue(request);

    Message* message = _inputQueue->dequeue();

    CIMGetPropertyResponseMessage* response 
	= (CIMGetPropertyResponseMessage*)message;

    _checkError(response);

    return response->value;
}

void CIMOMHandle::setProperty(
    const String& nameSpace,
    const CIMReference& instanceName,
    const String& propertyName,
    const CIMValue& newValue)
{
    Message* request = new CIMSetPropertyRequestMessage(
	XmlWriter::getNextMessageId(),
        nameSpace,
	instanceName,
	propertyName,
	newValue,
        _inputQueue->getQueueId());

    _outputQueue->enqueue(request);

    Message* message = _inputQueue->dequeue();

    CIMSetPropertyResponseMessage* response 
	= (CIMSetPropertyResponseMessage*)message;

    _checkError(response);
}

CIMQualifierDecl CIMOMHandle::getQualifier(
    const String& nameSpace,
    const String& qualifierName)
{
    Message* request = new CIMGetQualifierRequestMessage(
	XmlWriter::getNextMessageId(),
        nameSpace,
	qualifierName,
        _inputQueue->getQueueId());

    _outputQueue->enqueue(request);

    Message* message = _inputQueue->dequeue();

    CIMGetQualifierResponseMessage* response 
	= (CIMGetQualifierResponseMessage*)message;

    _checkError(response);

    return response->cimQualifierDecl;
}

void CIMOMHandle::setQualifier(
    const String& nameSpace,
    const CIMQualifierDecl& qualifierDeclaration)
{
    Message* request = new CIMSetQualifierRequestMessage(
	XmlWriter::getNextMessageId(),
        nameSpace,
	qualifierDeclaration,
        _inputQueue->getQueueId());

    _outputQueue->enqueue(request);

    Message* message = _inputQueue->dequeue();

    CIMSetQualifierResponseMessage* response 
	= (CIMSetQualifierResponseMessage*)message;

    _checkError(response);
}

void CIMOMHandle::deleteQualifier(
    const String& nameSpace,
    const String& qualifierName)
{
    Message* request = new CIMDeleteQualifierRequestMessage(
	XmlWriter::getNextMessageId(),
        nameSpace,
	qualifierName,
        _inputQueue->getQueueId());

    _outputQueue->enqueue(request);

    Message* message = _inputQueue->dequeue();

    CIMDeleteQualifierResponseMessage* response 
	= (CIMDeleteQualifierResponseMessage*)message;

    _checkError(response);
}


Array<CIMQualifierDecl> CIMOMHandle::enumerateQualifiers(
    const String& nameSpace)
{
    Message* request = new CIMEnumerateQualifiersRequestMessage(
	XmlWriter::getNextMessageId(),
        nameSpace,
        _inputQueue->getQueueId());

    _outputQueue->enqueue(request);

    Message* message = _inputQueue->dequeue();

    CIMEnumerateQualifiersResponseMessage* response 
	= (CIMEnumerateQualifiersResponseMessage*)message;

    _checkError(response);

    return response->qualifierDeclarations;
}

CIMValue CIMOMHandle::invokeMethod(
    const String& nameSpace,
    const CIMReference& instanceName,
    const String& methodName,
    const Array<CIMValue>& inParameters,
    Array<CIMValue>& outParameters)
{
    Message* request = new CIMInvokeMethodRequestMessage(
	XmlWriter::getNextMessageId(),
        nameSpace,
	instanceName,
	methodName,
	inParameters,
        _inputQueue->getQueueId());

    _outputQueue->enqueue(request);

    Message* message = _inputQueue->dequeue();

    CIMInvokeMethodResponseMessage* response 
	= (CIMInvokeMethodResponseMessage*)message;

    _checkError(response);

    outParameters = response->outParameters;
    return response->value;
}

void CIMOMHandle::_checkError(CIMResponseMessage* response)
{
    if (!response)
    {
	throw CIMException(
	    CIM_ERR_FAILED, __FILE__, __LINE__, "queue underflow");
	}

    if (response->errorCode != CIM_ERR_SUCCESS)
    {
	throw CIMException(response->errorCode, __FILE__, __LINE__, 
	    response->errorDescription);
    }
}

PEGASUS_NAMESPACE_END
