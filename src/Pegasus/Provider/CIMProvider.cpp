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

#include "CIMProvider.h"

PEGASUS_NAMESPACE_BEGIN

CIMProvider::CIMProvider()
{

}

CIMProvider::~CIMProvider()
{

}

CIMClass CIMProvider::getClass(
    const String& nameSpace,
    const String& className,
    Boolean localOnly,
    Boolean includeQualifiers,
    Boolean includeClassOrigin,
    const Array<String>& propertyList)
{
    throw CIMException(CIM_ERR_NOT_SUPPORTED);
    return CIMClass();
}

CIMInstance CIMProvider::getInstance(
    const String& nameSpace,
    const CIMReference& instanceName,
    Boolean localOnly,
    Boolean includeQualifiers,
    Boolean includeClassOrigin,
    const Array<String>& propertyList)
{
    throw CIMException(CIM_ERR_NOT_SUPPORTED);
    return CIMInstance();
}

void CIMProvider::deleteClass(
    const String& nameSpace,
    const String& className)
{
    throw CIMException(CIM_ERR_NOT_SUPPORTED);
}

void CIMProvider::deleteInstance(
    const String& nameSpace,
    const CIMReference& instanceName)
{
    throw CIMException(CIM_ERR_NOT_SUPPORTED);
}

void CIMProvider::createClass(
    const String& nameSpace,
    const CIMClass& newClass)
{
    throw CIMException(CIM_ERR_NOT_SUPPORTED);
}

void CIMProvider::createInstance(
    const String& nameSpace,
    const CIMInstance& newInstance)
{
    throw CIMException(CIM_ERR_NOT_SUPPORTED);
}

void CIMProvider::modifyClass(
    const String& nameSpace,
    const CIMClass& modifiedClass)
{
    throw CIMException(CIM_ERR_NOT_SUPPORTED);
}

void CIMProvider::modifyInstance(
    const String& nameSpace,
    const CIMInstance& modifiedInstance)
{
    throw CIMException(CIM_ERR_NOT_SUPPORTED);
}

Array<CIMClass> CIMProvider::enumerateClasses(
    const String& nameSpace,
    const String& className,
    Boolean deepInheritance,
    Boolean localOnly,
    Boolean includeQualifiers,
    Boolean includeClassOrigin)
{
    throw CIMException(CIM_ERR_NOT_SUPPORTED);
    return Array<CIMClass>();
}

Array<String> CIMProvider::enumerateClassNames(
    const String& nameSpace,
    const String& className,
    Boolean deepInheritance)
{
    throw CIMException(CIM_ERR_NOT_SUPPORTED);
    return Array<String>();
}

Array<CIMInstance> CIMProvider::enumerateInstances(
    const String& nameSpace,
    const String& className,
    Boolean deepInheritance,
    Boolean localOnly,
    Boolean includeQualifiers,
    Boolean includeClassOrigin,
    const Array<String>& propertyList)
{
    throw CIMException(CIM_ERR_NOT_SUPPORTED);
    return Array<CIMInstance>();
}

Array<CIMReference> CIMProvider::enumerateInstanceNames(
    const String& nameSpace,
    const String& className)
{
    throw CIMException(CIM_ERR_NOT_SUPPORTED);
    return Array<CIMReference>();
}

Array<CIMInstance> CIMProvider::execQuery(
    const String& queryLanguage,
    const String& query)
{
    throw CIMException(CIM_ERR_NOT_SUPPORTED);
    return Array<CIMInstance>();
}

Array<CIMObjectWithPath> CIMProvider::associators(
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
    throw CIMException(CIM_ERR_NOT_SUPPORTED);
    return Array<CIMObjectWithPath>();
}

Array<CIMReference> CIMProvider::associatorNames(
    const String& nameSpace,
    const CIMReference& objectName,
    const String& assocClass,
    const String& resultClass,
    const String& role,
    const String& resultRole)
{
    throw CIMException(CIM_ERR_NOT_SUPPORTED);
    return Array<CIMReference>();
}

Array<CIMObjectWithPath> CIMProvider::references(
    const String& nameSpace,
    const CIMReference& objectName,
    const String& resultClass,
    const String& role,
    Boolean includeQualifiers,
    Boolean includeClassOrigin,
    const Array<String>& propertyList)
{
    throw CIMException(CIM_ERR_NOT_SUPPORTED);
    return Array<CIMObjectWithPath>();
}

Array<CIMReference> CIMProvider::referenceNames(
    const String& nameSpace,
    const CIMReference& objectName,
    const String& resultClass,
    const String& role)
{
    throw CIMException(CIM_ERR_NOT_SUPPORTED);
    return Array<CIMReference>();
}

CIMValue CIMProvider::getProperty(
    const String& nameSpace,
    const CIMReference& instanceName,
    const String& propertyName)
{
    throw CIMException(CIM_ERR_NOT_SUPPORTED);
    return CIMValue();
}

void CIMProvider::setProperty(
    const String& nameSpace,
    const CIMReference& instanceName,
    const String& propertyName,
    const CIMValue& newValue)
{
    throw CIMException(CIM_ERR_NOT_SUPPORTED);
}

CIMQualifierDecl CIMProvider::getQualifier(
    const String& nameSpace,
    const String& qualifierName)
{
    throw CIMException(CIM_ERR_NOT_SUPPORTED);
    return CIMQualifierDecl();
}

void CIMProvider::setQualifier(
    const String& nameSpace,
    const CIMQualifierDecl& qualifierDecl)
{
    throw CIMException(CIM_ERR_NOT_SUPPORTED);
}

void CIMProvider::deleteQualifier(
    const String& nameSpace,
    const String& qualifierName)
{
    throw CIMException(CIM_ERR_NOT_SUPPORTED);
}

Array<CIMQualifierDecl> CIMProvider::enumerateQualifiers(
    const String& nameSpace)
{
    throw CIMException(CIM_ERR_NOT_SUPPORTED);
    return Array<CIMQualifierDecl>();
}

CIMValue CIMProvider::invokeMethod(
    const String& nameSpace,
    const CIMReference& instanceName,
    const String& methodName,
    const Array<CIMValue>& inParameters,
    Array<CIMValue>& outParameters)
{
    throw CIMException(CIM_ERR_NOT_SUPPORTED);
    return CIMValue();
}

void CIMProvider::initialize(CIMOMHandle& cimom)
{
}

void CIMProvider::terminate(void)
{
}

PEGASUS_NAMESPACE_END
