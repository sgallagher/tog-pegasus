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
// $Log: Dispatcher.cpp,v $
// Revision 1.1  2001/01/31 08:22:28  mike
// added dispatcher
//
//
//END_HISTORY

#include "Dispatcher.h"
#include <Pegasus/Repository/Repository.h>
#include "ProviderTable.h"

PEGASUS_NAMESPACE_BEGIN

Dispatcher::Dispatcher(Repository* repository)
    : _repository(repository)
{

}

Dispatcher::~Dispatcher()
{

}

ClassDecl Dispatcher::getClass(
    const String& nameSpace,
    const String& className,
    Boolean localOnly,
    Boolean includeQualifiers,
    Boolean includeClassOrigin,
    const Array<String>& propertyList)
{
    return _repository->getClass(
	nameSpace,
	className,
	localOnly,
	includeQualifiers,
	includeClassOrigin,
	propertyList);
}

InstanceDecl Dispatcher::getInstance(
    const String& nameSpace,
    const Reference& instanceName,
    Boolean localOnly,
    Boolean includeQualifiers,
    Boolean includeClassOrigin,
    const Array<String>& propertyList)
{
    String className = instanceName.getClassName();

    Provider* provider = _lookupProviderForClass(nameSpace, className);

    InstanceDecl instanceDecl = provider->getInstance(nameSpace, instanceName, 
	localOnly, includeQualifiers, includeClassOrigin);

    // ATTN: Need code here to fill out the class?

    return instanceDecl;
}

void Dispatcher::deleteClass(
    const String& nameSpace,
    const String& className)
{
    _repository->deleteClass(nameSpace, className);
}

void Dispatcher::deleteInstance(
    const String& nameSpace,
    const Reference& instanceName) 
{ 
    throw CimException(CimException::NOT_SUPPORTED);
}

void Dispatcher::createClass(
    const String& nameSpace,
    ClassDecl& newClass) 
{
    _repository->createClass(nameSpace, newClass);
}

void Dispatcher::createInstance(
    const String& nameSpace,
    const InstanceDecl& newInstance) 
{ 
    throw CimException(CimException::NOT_SUPPORTED);
}

void Dispatcher::modifyClass(
    const String& nameSpace,
    ClassDecl& modifiedClass) 
{
    _repository->modifyClass(nameSpace, modifiedClass);
}

void Dispatcher::modifyInstance(
    const String& nameSpace,
    const InstanceDecl& modifiedInstance) 
{
    throw CimException(CimException::NOT_SUPPORTED);
}

Array<ClassDecl> Dispatcher::enumerateClasses(
    const String& nameSpace,
    const String& className,
    Boolean deepInheritance,
    Boolean localOnly,
    Boolean includeQualifiers,
    Boolean includeClassOrigin)
{
    return _repository->enumerateClasses(
	nameSpace,
	className,
	deepInheritance,
	localOnly,
	includeQualifiers,
	includeClassOrigin);
}

Array<String> Dispatcher::enumerateClassNames(
    const String& nameSpace,
    const String& className,
    Boolean deepInheritance)
{
    return _repository->enumerateClassNames(
	nameSpace,
	className,
	deepInheritance);
}

Array<InstanceDecl> Dispatcher::enumerateInstances(
    const String& nameSpace,
    const String& className,
    Boolean deepInheritance,
    Boolean localOnly,
    Boolean includeQualifiers,
    Boolean includeClassOrigin,
    const Array<String>& propertyList)
{
    std::cout << "Dispatcher::enumerateInstances()" << std::endl;
    return Array<InstanceDecl>();
}

Array<Reference> Dispatcher::enumerateInstanceNames(
    const String& nameSpace,
    const String& className) 
{
    Provider* provider = _lookupProviderForClass(nameSpace, className);

    return provider->enumerateInstanceNames(nameSpace, className);
}

Array<InstanceDecl> Dispatcher::execQuery(
    const String& queryLanguage,
    const String& query) 
{ 
    throw CimException(CimException::NOT_SUPPORTED);
    return Array<InstanceDecl>();
}

Array<InstanceDecl> Dispatcher::associators(
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

Array<Reference> Dispatcher::associatorNames(
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

Array<InstanceDecl> Dispatcher::references(
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

Array<Reference> Dispatcher::referenceNames(
    const String& nameSpace,
    const Reference& objectName,
    const String& resultClass,
    const String& role)
{ 
    throw CimException(CimException::NOT_SUPPORTED);
    return Array<Reference>();
}

Value Dispatcher::getProperty(
    const String& nameSpace,
    const Reference& instanceName,
    const String& propertyName) 
{ 
    throw CimException(CimException::NOT_SUPPORTED);
    return Value();
}

void Dispatcher::setProperty(
    const String& nameSpace,
    const Reference& instanceName,
    const String& propertyName,
    const Value& newValue)
{ 
    throw CimException(CimException::NOT_SUPPORTED);
}

QualifierDecl Dispatcher::getQualifier(
    const String& nameSpace,
    const String& qualifierName) 
{
    return _repository->getQualifier(nameSpace, qualifierName);
}

void Dispatcher::setQualifier(
    const String& nameSpace,
    const QualifierDecl& qualifierDecl) 
{
    _repository->setQualifier(nameSpace, qualifierDecl);
}

void Dispatcher::deleteQualifier(
    const String& nameSpace,
    const String& qualifierName) 
{
    _repository->deleteQualifier(nameSpace, qualifierName);
}

Array<QualifierDecl> Dispatcher::enumerateQualifiers(
    const String& nameSpace)
{
    return _repository->enumerateQualifiers(nameSpace);
}

Value Dispatcher::invokeMethod(
    const String& nameSpace,
    const Reference& instanceName,
    const String& methodName,
    const Array<Value>& inParameters,
    Array<Value>& outParameters) 
{
    throw CimException(CimException::NOT_SUPPORTED);
    return Value();
}

Provider* Dispatcher::_lookupProviderForClass(
    const String& nameSpace,
    const String& className)
{
    //----------------------------------------------------------------------
    // Look up the class:
    //----------------------------------------------------------------------

    ClassDecl classDecl = _repository->getClass(nameSpace, className);

    if (!classDecl)
	throw CimException(CimException::INVALID_CLASS);

    // classDecl.print();

    //----------------------------------------------------------------------
    // Get the provider qualifier:
    //----------------------------------------------------------------------

    Uint32 pos = classDecl.findQualifier("provider");

    if (pos == Uint32(-1))
	throw CimException(CimException::FAILED);

    Qualifier q = classDecl.getQualifier(pos);
    String providerId;
    q.getValue().get(providerId);

    //----------------------------------------------------------------------
    // Get the provider (initialize it if not already initialize)
    // ATTN: move this block so that it can be shared.
    //----------------------------------------------------------------------

    Provider* provider = _providerTable.lookupProvider(providerId);

    if (!provider)
    {
	provider = _providerTable.loadProvider(providerId);

	if (!provider)
	    throw CimException(CimException::FAILED);

	provider->initialize(*_repository);
    }

    return provider;
}

PEGASUS_NAMESPACE_END
