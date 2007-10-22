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

#include <cstdarg>
#include <Pegasus/Common/Resolver.h>
#include "MemoryResidentRepository.h"
#include "RepositoryDeclContext.h"
#include "MetaRepository.h"
#include "Filtering.h"
#include "Serialization.h"

PEGASUS_NAMESPACE_BEGIN

#define PEGASUS_ARRAY_T NamespaceInstancePair
# include <Pegasus/Common/ArrayImpl.h>
#undef PEGASUS_ARRAY_T

//==============================================================================
//
// Local routines:
//
//==============================================================================

PEGASUS_FORMAT(2, 3)
static void _throw(CIMStatusCode code, const char* format, ...)
{
    char buffer[4096];

    va_list ap;
    va_start(ap, format);
    vsprintf(buffer, format, ap);
    va_end(ap);
    throw CIMException(code, buffer);
}

class Str
{
public:
    Str(const String& s) : _cstr(s.getCString()) { }
    Str(const CIMName& n) : _cstr(n.getString().getCString()) { }
    Str(const CIMNamespaceName& n) : _cstr(n.getString().getCString()) { }
    Str(const Exception& e) : _cstr(e.getMessage().getCString()) { }
    Str(const CIMDateTime& x) : _cstr(x.toString().getCString()) { }
    Str(const CIMObjectPath& x) : _cstr(x.toString().getCString()) { }
    const char* operator*() const { return (const char*)_cstr; }
    operator const char*() const { return (const char*)_cstr; }
private:
    CString _cstr;
};

static bool _contains(const CIMPropertyList& propertyList, const CIMName& name)
{
    for (Uint32 i = 0; i < propertyList.size(); i++)
    {
        if (propertyList[i] == name)
            return true;
    }

    return false;
}

static void _applyModifiedInstance(
    const MetaClass* mc,
    const CIMInstance& modifiedInstance_,
    const CIMPropertyList& propertyList,
    CIMInstance& resultInstance)
{
    CIMInstance& modifiedInstance = *((CIMInstance*)&modifiedInstance_);

    for (Uint32 i = 0; i < modifiedInstance.getPropertyCount(); i++)
    {
        CIMProperty cp = modifiedInstance.getProperty(i);
        Uint32 pos = resultInstance.findProperty(cp.getName());

        if (propertyList.isNull() || _contains(propertyList, cp.getName()))
        {
            // Reject attempts to add properties not in class:

            const MetaFeature* mf = FindFeature(
                mc, *Str(cp.getName()), META_FLAG_PROPERTY|META_FLAG_REFERENCE);

            if (!mf)
            {
                _throw(CIM_ERR_NOT_FOUND, 
                    "modifyInstance() failed: unknown property: %s",
                    *Str(cp.getName()));
            }

            // Reject attempts to modify key properties:

            if (mf->flags & META_FLAG_KEY)
            {
                _throw(CIM_ERR_FAILED,
                    "modifyInstance() failed to modify key property: %s",
                    *Str(cp.getName()));
            }

            // Add or replace property in result instance:

            if (pos != PEG_NOT_FOUND)
                resultInstance.removeProperty(pos);

            resultInstance.addProperty(cp);
        }
    }
}

static void _print(const CIMInstance& ci)
{
    CIMObject co(ci);

    std::cout << co.toString() << std::endl;
}

//==============================================================================
//
// class MemoryResidentRepository:
//
//==============================================================================

static void (*_saveCallback)(const Buffer& buffer, void* data);
static void* _saveData;
static void (*_loadCallback)(Buffer& buffer, void* data);
static void* _loadData;

MemoryResidentRepository::MemoryResidentRepository(
    const String& repositoryRoot, 
    Uint32 repositoryMode) 
    : 
    Repository(repositoryRoot, repositoryMode)
{
    // Load users data if any:
    _processLoadHandler();
}

MemoryResidentRepository::~MemoryResidentRepository()
{
}

CIMClass MemoryResidentRepository::getClass(
    bool lock,
    const CIMNamespaceName& nameSpace,
    const CIMName& className,
    Boolean localOnly,
    Boolean includeQualifiers,
    Boolean includeClassOrigin,
    const CIMPropertyList& propertyList)
{
    return MetaRepository::getClass(
        nameSpace, 
        className, 
        localOnly,
        includeQualifiers,
        includeClassOrigin,
        propertyList);
}

CIMInstance MemoryResidentRepository::getInstance(
    bool lock,
    const CIMNamespaceName& nameSpace,
    const CIMObjectPath& instanceName,
    Boolean localOnly,
    Boolean includeQualifiers,
    Boolean includeClassOrigin,
    const CIMPropertyList& propertyList)
{
    Uint32 pos = _findInstance(nameSpace, instanceName);

    if (pos == PEG_NOT_FOUND)
        _throw(CIM_ERR_NOT_FOUND, "%s", *Str(instanceName));

    CIMInstance cimInstance = _rep[pos].second.clone();

    Filtering::filterInstance(
        cimInstance,
        localOnly,
        includeQualifiers,
        includeClassOrigin,
        propertyList);

    return cimInstance;
}

void MemoryResidentRepository::deleteClass(
    bool lock,
    const CIMNamespaceName& nameSpace,
    const CIMName& className)
{
    MetaRepository::deleteClass(
        nameSpace,
        className);
}

void MemoryResidentRepository::deleteInstance(
    bool lock,
    const CIMNamespaceName& nameSpace,
    const CIMObjectPath& instanceName)
{
    Uint32 pos = _findInstance(nameSpace, instanceName);

    if (pos == PEG_NOT_FOUND)
        _throw(CIM_ERR_NOT_FOUND, "%s", *Str(instanceName));

    _rep.remove(pos);
    _processSaveHandler();
}

void MemoryResidentRepository::createClass(
    bool lock,
    const CIMNamespaceName& nameSpace,
    const CIMClass& newClass,
    const ContentLanguageList& contentLangs)
{
    MetaRepository::createClass(
        nameSpace,
        newClass);
}

CIMObjectPath MemoryResidentRepository::createInstance(
    bool lock,
    const CIMNamespaceName& nameSpace,
    const CIMInstance& newInstance,
    const ContentLanguageList& contentLangs)
{
    // Resolve the instance first:

    CIMInstance ci(newInstance.clone());
    CIMConstClass cc;
    RepositoryDeclContext context(this);
    Resolver::resolveInstance(ci, &context, nameSpace, cc, false);
    CIMObjectPath cop = ci.buildPath(cc);

    ci.setPath(cop);

    // Reject if an instance with this name already exists:

    if (_findInstance(nameSpace, cop) != PEG_NOT_FOUND)
        _throw(CIM_ERR_ALREADY_EXISTS, "%s", *Str(cop));

    // Add instance to array:

    _rep.append(NamespaceInstancePair(nameSpace, ci));
    _processSaveHandler();

    return cop;
}

void MemoryResidentRepository::modifyClass(
    bool lock,
    const CIMNamespaceName& nameSpace,
    const CIMClass& modifiedClass,
    const ContentLanguageList& contentLangs)
{
    MetaRepository::modifyClass(
        nameSpace,
        modifiedClass);
}

void MemoryResidentRepository::modifyInstance(
    bool lock,
    const CIMNamespaceName& nameSpace,
    const CIMInstance& modifiedInstance,
    Boolean includeQualifiers,
    const CIMPropertyList& propertyList,
    const ContentLanguageList& contentLangs)
{
    const CIMObjectPath& cop = modifiedInstance.getPath();
    CIMName className = cop.getClassName();

    // Get the meta class for this instance.

    const MetaClass* mc = MetaRepository::findMetaClass(
        *Str(nameSpace), *Str(className));

    if (!mc)
    {
        _throw(CIM_ERR_FAILED, 
            "modifyInstance() failed: unknown class: %s:%s",
            *Str(nameSpace), *Str(className));
    }

    // Get original instance to be modified:

    Uint32 pos = _findInstance(nameSpace, cop);

    if (pos == PEG_NOT_FOUND)
    {
        _throw(CIM_ERR_NOT_FOUND, "modified() failed: unknown instance: %s",
            *Str(cop.toString()));
    }

    CIMInstance resultInstance = _rep[pos].second.clone();

    // Apply features of modifiedInstance to result instance.

    _applyModifiedInstance(mc, modifiedInstance, propertyList, resultInstance);

    // Resolve the instance.

    CIMConstClass cc;
    RepositoryDeclContext context(this);
    Resolver::resolveInstance(resultInstance, &context, nameSpace, cc, false);

    // Replace original instance.

    _rep[pos].second = resultInstance;
    _processSaveHandler();
}

Array<CIMClass> MemoryResidentRepository::enumerateClasses(
    bool lock,
    const CIMNamespaceName& nameSpace,
    const CIMName& className,
    Boolean deepInheritance,
    Boolean localOnly,
    Boolean includeQualifiers,
    Boolean includeClassOrigin)
{
    return MetaRepository::enumerateClasses(
        nameSpace,
        className,
        deepInheritance,
        localOnly,
        includeQualifiers,
        includeClassOrigin);
}

Array<CIMName> MemoryResidentRepository::enumerateClassNames(
    bool lock,
    const CIMNamespaceName& nameSpace,
    const CIMName& className,
    Boolean deepInheritance)
{
    return MetaRepository::enumerateClassNames(
        nameSpace,
        className,
        deepInheritance);
}

Array<CIMInstance> MemoryResidentRepository::enumerateInstancesForSubtree(
    bool lock,
    const CIMNamespaceName& nameSpace,
    const CIMName& className,
    Boolean deepInheritance,
    Boolean localOnly,
    Boolean includeQualifiers,
    Boolean includeClassOrigin,
    const CIMPropertyList& propertyList)
{
    // Form array of classnames for this class and descendent classes:

    Array<CIMName> classNames;
    classNames.append(className);
    MetaRepository::getSubClassNames(nameSpace, className, true, classNames);

    // Get all instances for this class and all descendent classes

    Array<CIMInstance> result;

    for (Uint32 i = 0; i < classNames.size(); i++)
    {
        Array<CIMInstance> instances = enumerateInstancesForClass(false, 
            nameSpace, classNames[i], false, includeQualifiers, 
            includeClassOrigin, propertyList);

        for (Uint32 i = 0 ; i < instances.size(); i++)
        {
            Filtering::filterInstance(
                instances[i],
                localOnly,
                includeQualifiers,
                includeClassOrigin,
                propertyList);
        }

        result.appendArray(instances);
    }

    return result;
}

Array<CIMInstance> MemoryResidentRepository::enumerateInstancesForClass(
    bool lock,
    const CIMNamespaceName& nameSpace,
    const CIMName& className,
    Boolean localOnly,
    Boolean includeQualifiers,
    Boolean includeClassOrigin,
    const CIMPropertyList& propertyList)
{
    Array<CIMInstance> result;

    for (Uint32 i = 0; i < _rep.size(); i++)
    {
        if (_rep[i].first != nameSpace)
            continue;

        CIMInstance& ci = _rep[i].second;

        if (ci.getPath().getClassName() == className)
        {
            CIMInstance tmp = ci.clone();

            Filtering::filterInstance(
                tmp,
                localOnly,
                includeQualifiers,
                includeClassOrigin,
                propertyList);

            result.append(tmp);
        }
    }

    return result;
}

Array<CIMObjectPath> MemoryResidentRepository::enumerateInstanceNamesForSubtree(
    bool lock,
    const CIMNamespaceName& nameSpace,
    const CIMName& className)
{
    // Form array of classnames for this class and descendent classes:

    Array<CIMName> classNames;
    classNames.append(className);
    MetaRepository::getSubClassNames(nameSpace, className, true, classNames);

    // Get all instances for this class and all descendent classes

    Array<CIMObjectPath> result;

    for (Uint32 i = 0; i < classNames.size(); i++)
    {
        Array<CIMObjectPath> paths = enumerateInstanceNamesForClass(
            false, nameSpace, classNames[i]);

        result.appendArray(paths);
    }

    return result;
}

Array<CIMObjectPath> MemoryResidentRepository::enumerateInstanceNamesForClass(
    bool lock,
    const CIMNamespaceName& nameSpace,
    const CIMName& className)
{
    Array<CIMObjectPath> result;

    for (Uint32 i = 0; i < _rep.size(); i++)
    {
        if (_rep[i].first != nameSpace)
            continue;

        CIMInstance& ci = _rep[i].second;

        if (ci.getPath().getClassName() == className)
            result.append(ci.getPath());
    }

    return result;
}

Array<CIMInstance> MemoryResidentRepository::execQuery(
    bool lock,
    const String& queryLanguage,
    const String& query)
{
    _throw(CIM_ERR_NOT_SUPPORTED, "execQuery()");
    return Array<CIMInstance>();
}

Array<CIMObject> MemoryResidentRepository::associators(
    bool lock,
    const CIMNamespaceName& nameSpace,
    const CIMObjectPath& objectName,
    const CIMName& assocClass,
    const CIMName& resultClass,
    const String& role,
    const String& resultRole,
    Boolean includeQualifiers,
    Boolean includeClassOrigin,
    const CIMPropertyList& propertyList)
{
    if (objectName.getKeyBindings().size() == 0)
    {
        return MetaRepository::associatorClasses(
            nameSpace,
            objectName.getClassName(),
            assocClass,
            resultClass,
            role,
            resultRole,
            includeQualifiers,
            includeClassOrigin,
            propertyList);
    }
    else
    {
        _throw(CIM_ERR_NOT_SUPPORTED, "associators()");
        return Array<CIMObject>();
    }
}

Array<CIMObjectPath> MemoryResidentRepository::associatorNames(
    bool lock,
    const CIMNamespaceName& nameSpace,
    const CIMObjectPath& objectName,
    const CIMName& assocClass,
    const CIMName& resultClass,
    const String& role,
    const String& resultRole)
{
    if (objectName.getKeyBindings().size() == 0)
    {
        return MetaRepository::associatorClassPaths(
            nameSpace,
            objectName.getClassName(),
            assocClass,
            resultClass,
            role,
            resultRole);
    }
    else
    {
        _throw(CIM_ERR_NOT_SUPPORTED, "associatorNames()");
        return Array<CIMObjectPath>();
    }
}

Array<CIMObject> MemoryResidentRepository::references(
    bool lock,
    const CIMNamespaceName& nameSpace,
    const CIMObjectPath& objectName,
    const CIMName& resultClass,
    const String& role,
    Boolean includeQualifiers,
    Boolean includeClassOrigin,
    const CIMPropertyList& propertyList)
{
    if (objectName.getKeyBindings().size() == 0)
    {
        return MetaRepository::referenceClasses(
            nameSpace,
            objectName.getClassName(),
            resultClass,
            role,
            includeQualifiers,
            includeClassOrigin,
            propertyList);
    }
    else
    {
        _throw(CIM_ERR_NOT_SUPPORTED, "references()");
        return Array<CIMObject>();
    }
}

Array<CIMObjectPath> MemoryResidentRepository::referenceNames(
    bool lock,
    const CIMNamespaceName& nameSpace,
    const CIMObjectPath& objectName,
    const CIMName& resultClass,
    const String& role)
{
    if (objectName.getKeyBindings().size() == 0)
    {
        return MetaRepository::referenceClassPaths(
            nameSpace,
            objectName.getClassName(),
            resultClass,
            role);
    }
    else
    {
        _throw(CIM_ERR_NOT_SUPPORTED, "referenceNames()");
        return Array<CIMObjectPath>();
    }
}

CIMValue MemoryResidentRepository::getProperty(
    bool lock,
    const CIMNamespaceName& nameSpace,
    const CIMObjectPath& instanceName,
    const CIMName& propertyName)
{
    _throw(CIM_ERR_NOT_SUPPORTED, "getProperty()");
    return CIMValue();
}

void MemoryResidentRepository::setProperty(
    bool lock,
    const CIMNamespaceName& nameSpace,
    const CIMObjectPath& instanceName,
    const CIMName& propertyName,
    const CIMValue& newValue,
    const ContentLanguageList& contentLangs)
{
    _throw(CIM_ERR_NOT_SUPPORTED, "setProperty()");
}

CIMQualifierDecl MemoryResidentRepository::getQualifier(
    bool lock,
    const CIMNamespaceName& nameSpace,
    const CIMName& qualifierName)
{
    return MetaRepository::getQualifier(nameSpace, qualifierName);
}

void MemoryResidentRepository::setQualifier(
    bool lock,
    const CIMNamespaceName& nameSpace,
    const CIMQualifierDecl& qualifierDecl,
    const ContentLanguageList& contentLangs)
{
    MetaRepository::setQualifier(nameSpace, qualifierDecl);
}

void MemoryResidentRepository::deleteQualifier(
    bool lock,
    const CIMNamespaceName& nameSpace,
    const CIMName& qualifierName)
{
    MetaRepository::deleteQualifier(nameSpace, qualifierName);
}

Array<CIMQualifierDecl> MemoryResidentRepository::enumerateQualifiers(
    bool lock,
    const CIMNamespaceName& nameSpace)
{
    return MetaRepository::enumerateQualifiers(nameSpace);
}

void MemoryResidentRepository::createNameSpace(
    bool lock,
    const CIMNamespaceName& nameSpace,
    const NameSpaceAttributes& attributes)
{
    MetaRepository::createNameSpace(nameSpace, attributes);
}

void MemoryResidentRepository::modifyNameSpace(
    bool lock,
    const CIMNamespaceName& nameSpace,
    const NameSpaceAttributes& attributes)
{
    MetaRepository::createNameSpace(nameSpace, attributes);
}

Array<CIMNamespaceName> MemoryResidentRepository::enumerateNameSpaces(
    bool lock) const
{
    return MetaRepository::enumerateNameSpaces();
}

void MemoryResidentRepository::deleteNameSpace(
    bool lock,
    const CIMNamespaceName& nameSpace)
{
    MetaRepository::deleteNameSpace(nameSpace);
}

Boolean MemoryResidentRepository::getNameSpaceAttributes(
    bool lock,
    const CIMNamespaceName& nameSpace,
    NameSpaceAttributes& attributes)
{
    attributes.clear();
    return false;
}

void MemoryResidentRepository::setDeclContext(
    bool lock,
    RepositoryDeclContext* context)
{
    _throw(CIM_ERR_NOT_SUPPORTED, "setDeclContext()");
}

Boolean MemoryResidentRepository::isDefaultInstanceProvider(
    bool lock)
{
    return true;
}

void MemoryResidentRepository::getSubClassNames(
    bool lock,
    const CIMNamespaceName& nameSpace,
    const CIMName& className,
    Boolean deepInheritance,
    Array<CIMName>& subClassNames) const
{
    MetaRepository::getSubClassNames(
        nameSpace,
        className,
        deepInheritance,
        subClassNames);
}

void MemoryResidentRepository::getSuperClassNames(
    bool lock,
    const CIMNamespaceName& nameSpace,
    const CIMName& className,
    Array<CIMName>& superClassNames) const
{
    MetaRepository::getSuperClassNames(
        nameSpace,
        className,
        superClassNames);
}

Boolean MemoryResidentRepository::isRemoteNameSpace(
    bool lock,
    const CIMNamespaceName& nameSpace,
    String& remoteInfo)
{
    return false;
}

#ifdef PEGASUS_DEBUG
void MemoryResidentRepository::DisplayCacheStatistics(
    bool lock)
{
}
#endif

Uint32 MemoryResidentRepository::_findInstance(
    const CIMNamespaceName& nameSpace,
    const CIMObjectPath& instanceName)
{
    for (Uint32 i = 0; i < _rep.size(); i++)
    {
        if (_rep[i].first == nameSpace &&
            _rep[i].second.getPath() == instanceName)
        {
            return i;
        }
    }

    return PEG_NOT_FOUND;
}

void MemoryResidentRepository::installSaveCallback(
    void (*handler)(const Buffer& buffer, void* data),
    void * data)
{
    _saveCallback = handler;
    _saveData = data;
}

void MemoryResidentRepository::installLoadCallback(
    void (*handler)(Buffer& buffer, void* data),
    void * data)
{
    _loadCallback = handler;
    _loadData = data;
}

void MemoryResidentRepository::_processSaveHandler()
{
    if (!_saveCallback)
        return;

    Buffer out;

    for (Uint32 i = 0; i < _rep.size(); i++)
    {
        SerializeNameSpace(out, _rep[i].first);
        SerializeInstance(out, _rep[i].second);
    }

    (*_saveCallback)(out, _saveData);
}

void MemoryResidentRepository::_processLoadHandler()
{
    if (!_loadCallback)
        return;

    Buffer in;
    (*_loadCallback)(in, _loadData);
    size_t pos = 0;

    while (pos != in.size())
    {
        CIMNamespaceName nameSpace;

        if (DeserializeNameSpace(in, pos, nameSpace) != 0)
            return;

        CIMInstance cimInstance;

        if (DeserializeInstance(in, pos, cimInstance) != 0)
            return;

        _rep.append(NamespaceInstancePair(nameSpace, cimInstance));
    }
}

PEGASUS_NAMESPACE_END
