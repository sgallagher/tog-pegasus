//%/////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001, 2002 BMC Software, Hewlett-Packard Company, IBM,
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
// Author: Mike Brasher (mbrasher@bmc.com)
//
// Modified By: Carol Ann Krug Graves, Hewlett-Packard Company
//                  (carolann_graves@hp.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_Common_RepositoryBase_h
#define Pegasus_Common_RepositoryBase_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/IPC.h>
#include <Pegasus/Common/CIMClass.h>
#include <Pegasus/Common/CIMObject.h>
#include <Pegasus/Common/CIMInstance.h>
#include <Pegasus/Common/CIMPropertyList.h>
#include <Pegasus/Common/CIMQualifierDecl.h>

PEGASUS_NAMESPACE_BEGIN

class RepositoryDeclContext;

/** This class declares the interface for a CIM repository implementation.

    CIMRepositoryBase is a pure virtual base class (all the method signatures
    must 
*/
class CIMRepositoryBase
{
public:

    virtual ~CIMRepositoryBase() { }

    virtual void read_lock(void) throw(IPCException) = 0;

    virtual void read_unlock(void) = 0;
    
    virtual void write_lock(void) throw(IPCException) = 0;

    virtual void write_unlock(void) = 0;
    
    virtual CIMClass getClass(
        const String& nameSpace,
        const String& className,
        Boolean localOnly = true,
        Boolean includeQualifiers = true,
        Boolean includeClassOrigin = false,
        const CIMPropertyList& propertyList = CIMPropertyList()) = 0;

    virtual CIMInstance getInstance(
        const String& nameSpace,
        const CIMObjectPath& instanceName,
        Boolean localOnly = true,
        Boolean includeQualifiers = false,
        Boolean includeClassOrigin = false,
        const CIMPropertyList& propertyList = CIMPropertyList()) = 0;

    virtual void deleteClass(
        const String& nameSpace,
        const String& className) = 0;

    virtual void deleteInstance(
        const String& nameSpace,
        const CIMObjectPath& instanceName) = 0;

    virtual void createClass(
        const String& nameSpace,
        const CIMClass& newClass) = 0;

    virtual CIMObjectPath createInstance(
        const String& nameSpace,
        const CIMInstance& newInstance) = 0;

    virtual void modifyClass(
        const String& nameSpace,
        const CIMClass& modifiedClass) = 0;

    virtual void modifyInstance(
        const String& nameSpace,
        const CIMInstance& modifiedInstance,
        Boolean includeQualifiers = true,
        const CIMPropertyList& propertyList = CIMPropertyList()) = 0;

    virtual Array<CIMClass> enumerateClasses(
        const String& nameSpace,
        const String& className = String::EMPTY,
        Boolean deepInheritance = false,
        Boolean localOnly = true,
        Boolean includeQualifiers = true,
        Boolean includeClassOrigin = false) = 0;

    virtual Array<String> enumerateClassNames(
        const String& nameSpace,
        const String& className = String::EMPTY,
        Boolean deepInheritance = false) = 0;

    virtual Array<CIMInstance> enumerateInstances(
        const String& nameSpace,
        const String& className,
        Boolean deepInheritance = true,
        Boolean localOnly = true,
        Boolean includeQualifiers = false,
        Boolean includeClassOrigin = false,
        const CIMPropertyList& propertyList = CIMPropertyList()) = 0;

    virtual Array<CIMObjectPath> enumerateInstanceNames(
        const String& nameSpace,
        const String& className) = 0;

    virtual Array<CIMInstance> execQuery(
        const String& queryLanguage,
        const String& query)  = 0;

    virtual Array<CIMObject> associators(
        const String& nameSpace,
        const CIMObjectPath& objectName,
        const String& assocClass = String::EMPTY,
        const String& resultClass = String::EMPTY,
        const String& role = String::EMPTY,
        const String& resultRole = String::EMPTY,
        Boolean includeQualifiers = false,
        Boolean includeClassOrigin = false,
        const CIMPropertyList& propertyList = CIMPropertyList()) = 0;

    virtual Array<CIMObjectPath> associatorNames(
        const String& nameSpace,
        const CIMObjectPath& objectName,
        const String& assocClass = String::EMPTY,
        const String& resultClass = String::EMPTY,
        const String& role = String::EMPTY,
        const String& resultRole = String::EMPTY) = 0;

    virtual Array<CIMObject> references(
        const String& nameSpace,
        const CIMObjectPath& objectName,
        const String& resultClass = String::EMPTY,
        const String& role = String::EMPTY,
        Boolean includeQualifiers = false,
        Boolean includeClassOrigin = false,
        const CIMPropertyList& propertyList = CIMPropertyList()) = 0;

    virtual Array<CIMObjectPath> referenceNames(
        const String& nameSpace,
        const CIMObjectPath& objectName,
        const String& resultClass = String::EMPTY,
        const String& role = String::EMPTY) = 0;

    virtual CIMValue getProperty(
        const String& nameSpace,
        const CIMObjectPath& instanceName,
        const String& propertyName) = 0;

    virtual void setProperty(
        const String& nameSpace,
        const CIMObjectPath& instanceName,
        const String& propertyName,
        const CIMValue& newValue = CIMValue()) = 0;

    virtual CIMQualifierDecl getQualifier(
        const String& nameSpace,
        const String& qualifierName) = 0;

    virtual void setQualifier(
        const String& nameSpace,
        const CIMQualifierDecl& qualifierDecl) = 0;

    virtual void deleteQualifier(
        const String& nameSpace,
        const String& qualifierName) = 0;

    virtual Array<CIMQualifierDecl> enumerateQualifiers(
        const String& nameSpace) = 0;

    virtual void createNameSpace(const String& nameSpace) = 0;

    virtual Array<String> enumerateNameSpaces() const = 0;

    virtual void deleteNameSpace(const String& nameSpace) = 0;

    virtual void getSubClassNames(
        const String& nameSpaceName,
        const String& className,
        Boolean deepInheritance,
        Array<String>& subClassNames) const = 0;

    virtual void getSuperClassNames(
        const String& nameSpaceName,
        const String& className,
        Array<String>& subClassNames) const = 0;
};

PEGASUS_NAMESPACE_END

#endif /* Pegasus_Common_RepositoryBase_h */
