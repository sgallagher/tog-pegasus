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
// $Log: Operations.h,v $
// Revision 1.1  2001/01/14 19:53:00  mike
// Initial revision
//
//
//END_HISTORY

////////////////////////////////////////////////////////////////////////////////
//
// Operations.h
//
//	This interface contains a method for each operations defined in 
// 	the "Specification for CDIM Operations over HTTP".
//
////////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_Operations_h
#define Pegasus_Operations_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/ClassDecl.h>
#include <Pegasus/Common/InstanceDecl.h>
#include <Pegasus/Common/QualifierDecl.h>

PEGASUS_NAMESPACE_BEGIN

class PEGASUS_COMMON_LINKAGE Operations
{
public:

    Operations();
    
    virtual ~Operations();

    // ATTN: MSVC++ 5.0 you know what!
    static Array<String> _getStringArray()
    {
	return Array<String>();
    }

    virtual ClassDecl getClass(
	const String& nameSpace,
	const String& className,
	Boolean localOnly = true,
	Boolean includeQualifiers = true,
	Boolean includeClassOrigin = false,
	const Array<String>& propertyList = _getStringArray()) = 0;

    virtual InstanceDecl getInstance(
	const String& nameSpace,
	const Reference& instanceName,
	Boolean localOnly = true,
	Boolean includeQualifiers = false,
	Boolean includeClassOrigin = false,
	const Array<String>& propertyList = _getStringArray()) = 0;

    virtual void deleteClass(
	const String& nameSpace,
	const String& className) = 0;

    virtual void deleteInstance(
	const String& nameSpace,
	const Reference& instanceName) = 0;

    virtual void createClass(
	const String& nameSpace,
	ClassDecl& newClass) = 0;

    virtual void createInstance(
	const String& nameSpace,
	const InstanceDecl& newInstance) = 0;

    virtual void modifyClass(
	const String& nameSpace,
	ClassDecl& modifiedClass) = 0;

    virtual void modifyInstance(
	const String& nameSpace,
	const InstanceDecl& modifiedInstance) = 0;

    virtual Array<ClassDecl> enumerateClasses(
	const String& nameSpace,
	const String& className = String::EMPTY,
	Boolean deepInheritance = false,
	Boolean localOnly = true,
	Boolean includeQualifiers  = true,
	Boolean includeClassOrigin = false) = 0;

    virtual Array<String> enumerateClassNames(
	const String& nameSpace,
	const String& className = String::EMPTY,
	Boolean deepInheritance = false) = 0;

    virtual Array<InstanceDecl> enumerateInstances(
	const String& nameSpace,
	const String& className,
	Boolean deepInheritance = true,
	Boolean localOnly = true,
	Boolean includeQualifiers = false,
	Boolean includeClassOrigin = false,
	const Array<String>& propertyList = _getStringArray()) = 0;

    virtual Array<String> enumerateInstanceNames(
	const String& nameSpace,
	const String& className) = 0;

    virtual Array<InstanceDecl> execQuery(
	const String& queryLanguage,
	const String& query) = 0;

    virtual Array<InstanceDecl> associators(
	const String& nameSpace,
	const Reference& objectName,
	const String& assocClass = String::EMPTY,
	const String& resultClass = String::EMPTY,
	const String& role = String::EMPTY,
	const String& resultRole = String::EMPTY,
	Boolean includeQualifiers = false,
	Boolean includeClassOrigin = false,
	const Array<String>& propertyList= _getStringArray()) = 0;

    virtual Array<Reference> associatorNames(
	const String& nameSpace,
	const Reference& objectName,
	const String& assocClass = String::EMPTY,
	const String& resultClass = String::EMPTY,
	const String& role = String::EMPTY,
	const String& resultRole = String::EMPTY) = 0;

    virtual Array<InstanceDecl> references(
	const String& nameSpace,
	const Reference& objectName,
	const String& resultClass = String::EMPTY,
	const String& role = String::EMPTY,
	Boolean includeQualifiers = false,
	Boolean includeClassOrigin = false,
	const Array<String>& propertyList= _getStringArray()) = 0;

    virtual Array<Reference> referenceNames(
	const String& nameSpace,
	const Reference& objectName,
	const String& resultClass = String::EMPTY,
	const String& role = String::EMPTY) = 0;

    virtual Value getProperty(
	const String& nameSpace,
	const Reference& instanceName,
	const String& propertyName) = 0;

    virtual void setProperty(
	const String& nameSpace,
	const Reference& instanceName,
	const String& propertyName,
	const Value& newValue = Value()) = 0;

    virtual QualifierDecl getQualifier(
	const String& nameSpace,
	const String& qualifierName) = 0;

    virtual void setQualifier(
	const String& nameSpace,
	const QualifierDecl& qualifierDecl) = 0;

    virtual void deleteQualifier(
	const String& nameSpace,
	const String& qualifierName) = 0;

    virtual Array<QualifierDecl> enumerateQualifiers(
	const String& nameSpace) = 0;

    virtual Value invokeMethod(
	const String& nameSpace,
	const Reference& instanceName,
	const String& methodName,
	const Array<Value>& inParameters,
	Array<Value>& outParameters) = 0;
};

PEGASUS_NAMESPACE_END

#endif /* Pegasus_Operations_h */
