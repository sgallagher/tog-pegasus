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
// $Log: Provider.h,v $
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
//
//END_HISTORY

#ifndef PegasusProvider_Provider_h
#define PegasusProvider_Provider_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/Operations.h>

PEGASUS_NAMESPACE_BEGIN

class Repository;

class PEGASUS_PROVIDER_LINKAGE Provider : public Operations
{
public:

    Provider();
    
    virtual ~Provider();

    virtual ClassDecl getClass(
	const String& nameSpace,
	const String& className,
	Boolean localOnly = true,
	Boolean includeQualifiers = true,
	Boolean includeClassOrigin = false,
	const Array<String>& propertyList = _getStringArray());

    virtual InstanceDecl getInstance(
	const String& nameSpace,
	const Reference& instanceName,
	Boolean localOnly = true,
	Boolean includeQualifiers = false,
	Boolean includeClassOrigin = false,
	const Array<String>& propertyList = _getStringArray());

    virtual void deleteClass(
	const String& nameSpace,
	const String& className);

    virtual void deleteInstance(
	const String& nameSpace,
	const Reference& instanceName);

    virtual void createClass(
	const String& nameSpace,
	ClassDecl& newClass);

    virtual void createInstance(
	const String& nameSpace,
	const InstanceDecl& newInstance) ;

    virtual void modifyClass(
	const String& nameSpace,
	ClassDecl& modifiedClass);

    virtual void modifyInstance(
	const String& nameSpace,
	const InstanceDecl& modifiedInstance);

    virtual Array<ClassDecl> enumerateClasses(
	const String& nameSpace,
	const String& className = String::EMPTY,
	Boolean deepInheritance = false,
	Boolean localOnly = true,
	Boolean includeQualifiers  = true,
	Boolean includeClassOrigin = false);

    virtual Array<String> enumerateClassNames(
	const String& nameSpace,
	const String& className = String::EMPTY,
	Boolean deepInheritance = false);

    virtual Array<InstanceDecl> enumerateInstances(
	const String& nameSpace,
	const String& className,
	Boolean deepInheritance = true,
	Boolean localOnly = true,
	Boolean includeQualifiers = false,
	Boolean includeClassOrigin = false,
	const Array<String>& propertyList = _getStringArray());

    virtual Array<Reference> enumerateInstanceNames(
	const String& nameSpace,
	const String& className);

    virtual Array<InstanceDecl> execQuery(
	const String& queryLanguage,
	const String& query) ;

    virtual Array<InstanceDecl> associators(
	const String& nameSpace,
	const Reference& objectName,
	const String& assocClass = String::EMPTY,
	const String& resultClass = String::EMPTY,
	const String& role = String::EMPTY,
	const String& resultRole = String::EMPTY,
	Boolean includeQualifiers = false,
	Boolean includeClassOrigin = false,
	const Array<String>& propertyList = _getStringArray());

    virtual Array<Reference> associatorNames(
	const String& nameSpace,
	const Reference& objectName,
	const String& assocClass = String::EMPTY,
	const String& resultClass = String::EMPTY,
	const String& role = String::EMPTY,
	const String& resultRole = String::EMPTY);

    virtual Array<InstanceDecl> references(
	const String& nameSpace,
	const Reference& objectName,
	const String& resultClass = String::EMPTY,
	const String& role = String::EMPTY,
	Boolean includeQualifiers = false,
	Boolean includeClassOrigin = false,
	const Array<String>& propertyList = _getStringArray());

    virtual Array<Reference> referenceNames(
	const String& nameSpace,
	const Reference& objectName,
	const String& resultClass = String::EMPTY,
	const String& role = String::EMPTY);

    virtual Value getProperty(
	const String& nameSpace,
	const Reference& instanceName,
	const String& propertyName);

    virtual void setProperty(
	const String& nameSpace,
	const Reference& instanceName,
	const String& propertyName,
	const Value& newValue = Value());

    virtual QualifierDecl getQualifier(
	const String& nameSpace,
	const String& qualifierName);

    virtual void setQualifier(
	const String& nameSpace,
	const QualifierDecl& qualifierDecl);

    virtual void deleteQualifier(
	const String& nameSpace,
	const String& qualifierName);

    virtual Array<QualifierDecl> enumerateQualifiers(
	const String& nameSpace);

    virtual Value invokeMethod(
	const String& nameSpace,
	const Reference& instanceName,
	const String& methodName,
	const Array<Value>& inParameters,
	Array<Value>& outParameters);

    /** 
	This method is invoked to initialize the provider. Eventually we
	will pass a loca interface to the CIMOM, but for now we are just
	passing the repository.
    */

    virtual void initialize(Repository& repository);
};

PEGASUS_NAMESPACE_END

#endif /* PegasusProvider_Provider_h */
