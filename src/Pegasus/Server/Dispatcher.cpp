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
// Revision 1.5  2001/02/19 01:47:17  mike
// Renamed names of the form CIMConst to ConstCIM.
//
// Revision 1.4  2001/02/18 19:02:18  mike
// Fixed CIM debacle
//
// Revision 1.3  2001/02/16 02:06:09  mike
// Renamed many classes and headers.
//
// Revision 1.2  2001/02/13 16:35:32  mike
// Fixed compiler error with createInstance()
//
// Revision 1.1  2001/01/31 08:22:28  mike
// added dispatcher
//
//
//END_HISTORY

#include "Dispatcher.h"
#include <Pegasus/Repository/CIMRepository.h>
#include "ProviderTable.h"

PEGASUS_NAMESPACE_BEGIN

Dispatcher::Dispatcher(CIMRepository* repository)
    : _repository(repository)
{

}

Dispatcher::~Dispatcher()
{

}

CIMClass Dispatcher::getClass(
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

CIMInstance Dispatcher::getInstance(
    const String& nameSpace,
    const CIMReference& instanceName,
    Boolean localOnly,
    Boolean includeQualifiers,
    Boolean includeClassOrigin,
    const Array<String>& propertyList)
{
    String className = instanceName.getClassName();

    CIMProvider* provider = _lookupProviderForClass(nameSpace, className);

    CIMInstance cimInstance = provider->getInstance(nameSpace, instanceName, 
	localOnly, includeQualifiers, includeClassOrigin);

    // ATTN: Need code here to fill out the class?

    return cimInstance;
}

void Dispatcher::deleteClass(
    const String& nameSpace,
    const String& className)
{
    _repository->deleteClass(nameSpace, className);
}

void Dispatcher::deleteInstance(
    const String& nameSpace,
    const CIMReference& instanceName) 
{ 
    throw CimException(CimException::NOT_SUPPORTED);
}

void Dispatcher::createClass(
    const String& nameSpace,
    CIMClass& newClass) 
{
    _repository->createClass(nameSpace, newClass);
}

void Dispatcher::createInstance(
    const String& nameSpace,
    CIMInstance& newInstance) 
{ 
    throw CimException(CimException::NOT_SUPPORTED);
}

void Dispatcher::modifyClass(
    const String& nameSpace,
    CIMClass& modifiedClass) 
{
    _repository->modifyClass(nameSpace, modifiedClass);
}

void Dispatcher::modifyInstance(
    const String& nameSpace,
    const CIMInstance& modifiedInstance) 
{
    throw CimException(CimException::NOT_SUPPORTED);
}

Array<CIMClass> Dispatcher::enumerateClasses(
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

Array<CIMInstance> Dispatcher::enumerateInstances(
    const String& nameSpace,
    const String& className,
    Boolean deepInheritance,
    Boolean localOnly,
    Boolean includeQualifiers,
    Boolean includeClassOrigin,
    const Array<String>& propertyList)
{
    std::cout << "Dispatcher::enumerateInstances()" << std::endl;
    return Array<CIMInstance>();
}

Array<CIMReference> Dispatcher::enumerateInstanceNames(
    const String& nameSpace,
    const String& className) 
{
    CIMProvider* provider = _lookupProviderForClass(nameSpace, className);

    return provider->enumerateInstanceNames(nameSpace, className);
}

Array<CIMInstance> Dispatcher::execQuery(
    const String& queryLanguage,
    const String& query) 
{ 
    throw CimException(CimException::NOT_SUPPORTED);
    return Array<CIMInstance>();
}

Array<CIMInstance> Dispatcher::associators(
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
    throw CimException(CimException::NOT_SUPPORTED);
    return Array<CIMInstance>();
}

Array<CIMReference> Dispatcher::associatorNames(
    const String& nameSpace,
    const CIMReference& objectName,
    const String& assocClass,
    const String& resultClass,
    const String& role,
    const String& resultRole)
{ 
    throw CimException(CimException::NOT_SUPPORTED);
    return Array<CIMReference>();
}

Array<CIMInstance> Dispatcher::references(
    const String& nameSpace,
    const CIMReference& objectName,
    const String& resultClass,
    const String& role,
    Boolean includeQualifiers,
    Boolean includeClassOrigin,
    const Array<String>& propertyList)
{
    throw CimException(CimException::NOT_SUPPORTED);
    return Array<CIMInstance>();
}

Array<CIMReference> Dispatcher::referenceNames(
    const String& nameSpace,
    const CIMReference& objectName,
    const String& resultClass,
    const String& role)
{ 
    throw CimException(CimException::NOT_SUPPORTED);
    return Array<CIMReference>();
}

CIMValue Dispatcher::getProperty(
    const String& nameSpace,
    const CIMReference& instanceName,
    const String& propertyName) 
{ 
    throw CimException(CimException::NOT_SUPPORTED);
    return CIMValue();
}

void Dispatcher::setProperty(
    const String& nameSpace,
    const CIMReference& instanceName,
    const String& propertyName,
    const CIMValue& newValue)
{ 
    throw CimException(CimException::NOT_SUPPORTED);
}

CIMQualifierDecl Dispatcher::getQualifier(
    const String& nameSpace,
    const String& qualifierName) 
{
    return _repository->getQualifier(nameSpace, qualifierName);
}

void Dispatcher::setQualifier(
    const String& nameSpace,
    const CIMQualifierDecl& qualifierDecl) 
{
    _repository->setQualifier(nameSpace, qualifierDecl);
}

void Dispatcher::deleteQualifier(
    const String& nameSpace,
    const String& qualifierName) 
{
    _repository->deleteQualifier(nameSpace, qualifierName);
}

Array<CIMQualifierDecl> Dispatcher::enumerateQualifiers(
    const String& nameSpace)
{
    return _repository->enumerateQualifiers(nameSpace);
}

CIMValue Dispatcher::invokeMethod(
    const String& nameSpace,
    const CIMReference& instanceName,
    const String& methodName,
    const Array<CIMValue>& inParameters,
    Array<CIMValue>& outParameters) 
{
    throw CimException(CimException::NOT_SUPPORTED);
    return CIMValue();
}

CIMProvider* Dispatcher::_lookupProviderForClass(
    const String& nameSpace,
    const String& className)
{
    //----------------------------------------------------------------------
    // Look up the class:
    //----------------------------------------------------------------------

    CIMClass cimClass = _repository->getClass(nameSpace, className);

    if (!cimClass)
	throw CimException(CimException::INVALID_CLASS);

    // cimClass.print();

    //----------------------------------------------------------------------
    // Get the provider qualifier:
    //----------------------------------------------------------------------

    Uint32 pos = cimClass.findQualifier("provider");

    if (pos == Uint32(-1))
	throw CimException(CimException::FAILED);

    CIMQualifier q = cimClass.getQualifier(pos);
    String providerId;
    q.getValue().get(providerId);

    //----------------------------------------------------------------------
    // Get the provider (initialize it if not already initialize)
    // ATTN: move this block so that it can be shared.
    //----------------------------------------------------------------------

    CIMProvider* provider = _providerTable.lookupProvider(providerId);

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
