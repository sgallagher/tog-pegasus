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
// $Log: CIMProvider.cpp,v $
// Revision 1.3  2001/03/05 19:54:49  mike
// Fixed earlier boo boo (renamed CimException to CIMException).
//
// Revision 1.2  2001/02/18 19:02:17  mike
// Fixed CIM debacle
//
// Revision 1.1  2001/02/16 02:08:26  mike
// Renamed several classes
//
// Revision 1.4  2001/02/13 16:35:32  mike
// Fixed compiler error with createInstance()
//
// Revision 1.3  2001/01/31 08:20:51  mike
// Added dispatcher framework.
// Added enumerateInstanceNames.
//
// Revision 1.2  2001/01/29 02:24:15  mike
// Added support for GetInstance.
//
// Revision 1.1  2001/01/20 22:29:24  karl
// new provider interface class
//
//
//END_HISTORY

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
    throw CIMException(CIMException::NOT_SUPPORTED);
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
    throw CIMException(CIMException::NOT_SUPPORTED);
    return CIMInstance();
}

void CIMProvider::deleteClass(
    const String& nameSpace,
    const String& className)
{
    throw CIMException(CIMException::NOT_SUPPORTED);
}

void CIMProvider::deleteInstance(
    const String& nameSpace,
    const CIMReference& instanceName) 
{ 
    throw CIMException(CIMException::NOT_SUPPORTED);
}

void CIMProvider::createClass(
    const String& nameSpace,
    CIMClass& newClass) 
{
    throw CIMException(CIMException::NOT_SUPPORTED);
}

void CIMProvider::createInstance(
    const String& nameSpace,
    CIMInstance& newInstance) 
{ 
    throw CIMException(CIMException::NOT_SUPPORTED);
}

void CIMProvider::modifyClass(
    const String& nameSpace,
    CIMClass& modifiedClass) 
{
    throw CIMException(CIMException::NOT_SUPPORTED);
}

void CIMProvider::modifyInstance(
    const String& nameSpace,
    const CIMInstance& modifiedInstance) 
{
    throw CIMException(CIMException::NOT_SUPPORTED);
}

Array<CIMClass> CIMProvider::enumerateClasses(
    const String& nameSpace,
    const String& className,
    Boolean deepInheritance,
    Boolean localOnly,
    Boolean includeQualifiers,
    Boolean includeClassOrigin)
{
    throw CIMException(CIMException::NOT_SUPPORTED);
    return Array<CIMClass>();
}

Array<String> CIMProvider::enumerateClassNames(
    const String& nameSpace,
    const String& className,
    Boolean deepInheritance)
{
    throw CIMException(CIMException::NOT_SUPPORTED);
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
    throw CIMException(CIMException::NOT_SUPPORTED);
    return Array<CIMInstance>();
}

Array<CIMReference> CIMProvider::enumerateInstanceNames(
    const String& nameSpace,
    const String& className) 
{ 
    throw CIMException(CIMException::NOT_SUPPORTED);
    return Array<CIMReference>();
}

Array<CIMInstance> CIMProvider::execQuery(
    const String& queryLanguage,
    const String& query) 
{ 
    throw CIMException(CIMException::NOT_SUPPORTED);
    return Array<CIMInstance>();
}

Array<CIMInstance> CIMProvider::associators(
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
    throw CIMException(CIMException::NOT_SUPPORTED);
    return Array<CIMInstance>();
}

Array<CIMReference> CIMProvider::associatorNames(
    const String& nameSpace,
    const CIMReference& objectName,
    const String& assocClass,
    const String& resultClass,
    const String& role,
    const String& resultRole)
{ 
    throw CIMException(CIMException::NOT_SUPPORTED);
    return Array<CIMReference>();
}

Array<CIMInstance> CIMProvider::references(
    const String& nameSpace,
    const CIMReference& objectName,
    const String& resultClass,
    const String& role,
    Boolean includeQualifiers,
    Boolean includeClassOrigin,
    const Array<String>& propertyList)
{
    throw CIMException(CIMException::NOT_SUPPORTED);
    return Array<CIMInstance>();
}

Array<CIMReference> CIMProvider::referenceNames(
    const String& nameSpace,
    const CIMReference& objectName,
    const String& resultClass,
    const String& role)
{ 
    throw CIMException(CIMException::NOT_SUPPORTED);
    return Array<CIMReference>();
}

CIMValue CIMProvider::getProperty(
    const String& nameSpace,
    const CIMReference& instanceName,
    const String& propertyName) 
{ 
    throw CIMException(CIMException::NOT_SUPPORTED);
    return CIMValue();
}

void CIMProvider::setProperty(
    const String& nameSpace,
    const CIMReference& instanceName,
    const String& propertyName,
    const CIMValue& newValue)
{ 
    throw CIMException(CIMException::NOT_SUPPORTED);
}

CIMQualifierDecl CIMProvider::getQualifier(
    const String& nameSpace,
    const String& qualifierName) 
{
    throw CIMException(CIMException::NOT_SUPPORTED);
    return CIMQualifierDecl();
}

void CIMProvider::setQualifier(
    const String& nameSpace,
    const CIMQualifierDecl& qualifierDecl) 
{
    throw CIMException(CIMException::NOT_SUPPORTED);
}

void CIMProvider::deleteQualifier(
    const String& nameSpace,
    const String& qualifierName) 
{
    throw CIMException(CIMException::NOT_SUPPORTED);
}

Array<CIMQualifierDecl> CIMProvider::enumerateQualifiers(
    const String& nameSpace)
{
    throw CIMException(CIMException::NOT_SUPPORTED);
    return Array<CIMQualifierDecl>();
}

CIMValue CIMProvider::invokeMethod(
    const String& nameSpace,
    const CIMReference& instanceName,
    const String& methodName,
    const Array<CIMValue>& inParameters,
    Array<CIMValue>& outParameters) 
{
    throw CIMException(CIMException::NOT_SUPPORTED);
    return CIMValue();
}

void CIMProvider::initialize(CIMRepository& repository)
{

}

PEGASUS_NAMESPACE_END
