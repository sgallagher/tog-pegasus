//BEGIN_LICENSE
//
// Copyright (c) 2000 The Open Group, BMC Software, Tivoli Systems, IBM
//
// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation
// the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following conditions:
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
// THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.
//
//END_LICENSE
//BEGIN_HISTORY
//
// Author:
//
// $Log: Provider.cpp,v $
// Revision 1.2  2001/01/29 02:24:15  mike
// Added support for GetInstance.
//
// Revision 1.1  2001/01/20 22:29:24  karl
// new provider interface class
//
//
//END_HISTORY

#include "Provider.h"

PEGASUS_NAMESPACE_BEGIN

Provider::Provider()
{

}

Provider::~Provider()
{

}

ClassDecl Provider::getClass(
    const String& nameSpace,
    const String& className,
    Boolean localOnly,
    Boolean includeQualifiers,
    Boolean includeClassOrigin,
    const Array<String>& propertyList)
{
    throw CimException(CimException::NOT_SUPPORTED);
    return ClassDecl();
}

InstanceDecl Provider::getInstance(
    const String& nameSpace,
    const Reference& instanceName,
    Boolean localOnly,
    Boolean includeQualifiers,
    Boolean includeClassOrigin,
    const Array<String>& propertyList)
{
    throw CimException(CimException::NOT_SUPPORTED);
    return InstanceDecl();
}

void Provider::deleteClass(
    const String& nameSpace,
    const String& className)
{
    throw CimException(CimException::NOT_SUPPORTED);
}

void Provider::deleteInstance(
    const String& nameSpace,
    const Reference& instanceName) 
{ 
    throw CimException(CimException::NOT_SUPPORTED);
}

void Provider::createClass(
    const String& nameSpace,
    ClassDecl& newClass) 
{
    throw CimException(CimException::NOT_SUPPORTED);
}

void Provider::createInstance(
    const String& nameSpace,
    const InstanceDecl& newInstance) 
{ 
    throw CimException(CimException::NOT_SUPPORTED);
}

void Provider::modifyClass(
    const String& nameSpace,
    ClassDecl& modifiedClass) 
{
    throw CimException(CimException::NOT_SUPPORTED);
}

void Provider::modifyInstance(
    const String& nameSpace,
    const InstanceDecl& modifiedInstance) 
{
    throw CimException(CimException::NOT_SUPPORTED);
}

Array<ClassDecl> Provider::enumerateClasses(
    const String& nameSpace,
    const String& className,
    Boolean deepInheritance,
    Boolean localOnly,
    Boolean includeQualifiers,
    Boolean includeClassOrigin)
{
    throw CimException(CimException::NOT_SUPPORTED);
    return Array<ClassDecl>();
}

Array<String> Provider::enumerateClassNames(
    const String& nameSpace,
    const String& className,
    Boolean deepInheritance)
{
    throw CimException(CimException::NOT_SUPPORTED);
    return Array<String>();
}

Array<InstanceDecl> Provider::enumerateInstances(
    const String& nameSpace,
    const String& className,
    Boolean deepInheritance,
    Boolean localOnly,
    Boolean includeQualifiers,
    Boolean includeClassOrigin,
    const Array<String>& propertyList)
{
    throw CimException(CimException::NOT_SUPPORTED);
    return Array<InstanceDecl>();
}

Array<String> Provider::enumerateInstanceNames(
    const String& nameSpace,
    const String& className) 
{ 
    throw CimException(CimException::NOT_SUPPORTED);
    return Array<String>();
}

Array<InstanceDecl> Provider::execQuery(
    const String& queryLanguage,
    const String& query) 
{ 
    throw CimException(CimException::NOT_SUPPORTED);
    return Array<InstanceDecl>();
}

Array<InstanceDecl> Provider::associators(
    const String& nameSpace,
    const Reference& objectName,
    const String& assocClass,
    const String& resultClass,
    const String& role,
    const String& resultRole,
    Boolean includeQualifiers,
    Boolean includeClassOrigin,
    const Array<String>& propertyList)
{
    throw CimException(CimException::NOT_SUPPORTED);
    return Array<InstanceDecl>();
}

Array<Reference> Provider::associatorNames(
    const String& nameSpace,
    const Reference& objectName,
    const String& assocClass,
    const String& resultClass,
    const String& role,
    const String& resultRole)
{ 
    throw CimException(CimException::NOT_SUPPORTED);
    return Array<Reference>();
}

Array<InstanceDecl> Provider::references(
    const String& nameSpace,
    const Reference& objectName,
    const String& resultClass,
    const String& role,
    Boolean includeQualifiers,
    Boolean includeClassOrigin,
    const Array<String>& propertyList)
{
    throw CimException(CimException::NOT_SUPPORTED);
    return Array<InstanceDecl>();
}

Array<Reference> Provider::referenceNames(
    const String& nameSpace,
    const Reference& objectName,
    const String& resultClass,
    const String& role)
{ 
    throw CimException(CimException::NOT_SUPPORTED);
    return Array<Reference>();
}

Value Provider::getProperty(
    const String& nameSpace,
    const Reference& instanceName,
    const String& propertyName) 
{ 
    throw CimException(CimException::NOT_SUPPORTED);
    return Value();
}

void Provider::setProperty(
    const String& nameSpace,
    const Reference& instanceName,
    const String& propertyName,
    const Value& newValue)
{ 
    throw CimException(CimException::NOT_SUPPORTED);
}

QualifierDecl Provider::getQualifier(
    const String& nameSpace,
    const String& qualifierName) 
{
    throw CimException(CimException::NOT_SUPPORTED);
    return QualifierDecl();
}

void Provider::setQualifier(
    const String& nameSpace,
    const QualifierDecl& qualifierDecl) 
{
    throw CimException(CimException::NOT_SUPPORTED);
}

void Provider::deleteQualifier(
    const String& nameSpace,
    const String& qualifierName) 
{
    throw CimException(CimException::NOT_SUPPORTED);
}

Array<QualifierDecl> Provider::enumerateQualifiers(
    const String& nameSpace)
{
    throw CimException(CimException::NOT_SUPPORTED);
    return Array<QualifierDecl>();
}

Value Provider::invokeMethod(
    const String& nameSpace,
    const Reference& instanceName,
    const String& methodName,
    const Array<Value>& inParameters,
    Array<Value>& outParameters) 
{
    throw CimException(CimException::NOT_SUPPORTED);
    return Value();
}

void Provider::initialize(Repository& repository)
{

}

PEGASUS_NAMESPACE_END
