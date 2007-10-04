//%2006////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001, 2002 BMC Software; Hewlett-Packard Development
// Company, L.P.; IBM Corp.; The Open Group; Tivoli Systems.
// Copyright (c) 2003 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation, The Open Group.
// Copyright (c) 2004 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation; VERITAS Software Corporation; The Open Group.
// Copyright (c) 2005 Hewlett-Packard Development Company, L.P.; IBM Corp.;
// EMC Corporation; VERITAS Software Corporation; The Open Group.
// Copyright (c) 2006 Hewlett-Packard Development Company, L.P.; IBM Corp.;
// EMC Corporation; Symantec Corporation; The Open Group.
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
//%/////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_MetaRepository_h
#define Pegasus_MetaRepository_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/CIMClass.h>
#include <Pegasus/Common/CIMQualifierDecl.h>
#include <Pegasus/Common/HashTable.h>
#include "Linkage.h"
#include "MetaTypes.h"

PEGASUS_NAMESPACE_BEGIN

class PEGASUS_REPOSITORY_LINKAGE MetaRepository
{
public:

    typedef HashTable <String, String, EqualNoCaseFunc, HashLowerCaseFunc>
        NameSpaceAttributes;

    // Class operations:

    static CIMClass getClass(
        const CIMNamespaceName& nameSpace,
        const CIMName& className,
        Boolean localOnly,
        Boolean includeQualifiers,
        Boolean includeClassOrigin,
        const CIMPropertyList& propertyList);

    static Array<CIMClass> enumerateClasses(
        const CIMNamespaceName& nameSpace,
        const CIMName& className,
        Boolean deepInheritance,
        Boolean localOnly,
        Boolean includeQualifiers,
        Boolean includeClassOrigin);

    static Array<CIMName> enumerateClassNames(
        const CIMNamespaceName& nameSpace,
        const CIMName& className,
        Boolean deepInheritance);

    static void deleteClass(
        const CIMNamespaceName& nameSpace,
        const CIMName& className);

    static void createClass(
        const CIMNamespaceName& nameSpace,
        const CIMClass& newClass);

    static void modifyClass(
        const CIMNamespaceName& nameSpace,
        const CIMClass& modifiedClass);

    static void getSubClassNames(
        const CIMNamespaceName& nameSpace,
        const CIMName& className,
        Boolean deepInheritance,
        Array<CIMName>& subClassNames);

    static void getSuperClassNames(
        const CIMNamespaceName& nameSpace,
        const CIMName& className,
        Array<CIMName>& superClassNames);

    // Namespace operations:

    static bool addNameSpace(
        const MetaNameSpace* nameSpace);

    static void createNameSpace(
        const CIMNamespaceName& nameSpace,
        const NameSpaceAttributes& attributes);

    static void modifyNameSpace(
        const CIMNamespaceName& nameSpace,
        const NameSpaceAttributes& attributes);

    static Array<CIMNamespaceName> enumerateNameSpaces();

    static void deleteNameSpace(
        const CIMNamespaceName& nameSpace);

    static Boolean getNameSpaceAttributes(
        const CIMNamespaceName& nameSpace,
        NameSpaceAttributes& attributes);

    static Boolean isRemoteNameSpace(
        const CIMNamespaceName& nameSpace,
        String& remoteInfo);

    // Qualifier operations:

    static CIMQualifierDecl getQualifier(
        const CIMNamespaceName& nameSpace,
        const CIMName& qualifierName);

    static void setQualifier(
        const CIMNamespaceName& nameSpace,
        const CIMQualifierDecl& qualifierDecl);

    static void deleteQualifier(
        const CIMNamespaceName& nameSpace,
        const CIMName& qualifierName);

    static Array<CIMQualifierDecl> enumerateQualifiers(
        const CIMNamespaceName& nameSpace);

    static Array<CIMObject> associatorClasses(
        const CIMNamespaceName& nameSpace,
        const CIMName& className,
        const CIMName& assocClass,
        const CIMName& resultClass,
        const String& role,
        const String& resultRole,
        Boolean includeQualifiers,
        Boolean includeClassOrigin,
        const CIMPropertyList& propertyList);

    static Array<CIMObjectPath> associatorClassPaths(
        const CIMNamespaceName& nameSpace,
        const CIMName& className,
        const CIMName& assocClass,
        const CIMName& resultClass,
        const String& role,
        const String& resultRole);

    static Array<CIMObject> referenceClasses(
        const CIMNamespaceName& nameSpace,
        const CIMName& className,
        const CIMName& resultClass,
        const String& role,
        Boolean includeQualifiers,
        Boolean includeClassOrigin,
        const CIMPropertyList& propertyList);

    static Array<CIMObjectPath> referenceClassPaths(
        const CIMNamespaceName& nameSpace,
        const CIMName& className,
        const CIMName& resultClass,
        const String& role);

    static const MetaClass* findMetaClass(
        const char* nameSpace,
        const char* className);

private:

    MetaRepository();

    ~MetaRepository();
};

PEGASUS_NAMESPACE_END

#endif /* Pegasus_MetaRepository_h */
