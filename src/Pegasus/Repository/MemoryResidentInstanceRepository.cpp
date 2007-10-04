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
#include "MemoryResidentInstanceRepository.h"
#include "MetaRepository.h"
#include "MetaRepositoryDeclContext.h"
#include "Filtering.h"

PEGASUS_NAMESPACE_BEGIN

#define PEGASUS_ARRAY_T NamespaceInstancePair
# include <Pegasus/Common/ArrayImpl.h>
#undef PEGASUS_ARRAY_T

#define TRACE printf("TRACE: %s(%d): %s\n", __FILE__, __LINE__, __FUNCTION__)

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
// class MemoryResidentInstanceRepository
//
//==============================================================================

MemoryResidentInstanceRepository::MemoryResidentInstanceRepository()
{
}

MemoryResidentInstanceRepository::~MemoryResidentInstanceRepository()
{
}

CIMInstance MemoryResidentInstanceRepository::getInstance(
    const CIMNamespaceName& nameSpace,
    const CIMObjectPath& instanceName,
    Boolean localOnly,
    Boolean includeQualifiers,
    Boolean includeClassOrigin,
    const CIMPropertyList& propertyList)
{
    TRACE;

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

void MemoryResidentInstanceRepository::deleteInstance(
    const CIMNamespaceName& nameSpace,
    const CIMObjectPath& instanceName)
{
    TRACE;

    Uint32 pos = _findInstance(nameSpace, instanceName);

    if (pos == PEG_NOT_FOUND)
        _throw(CIM_ERR_NOT_FOUND, "%s", *Str(instanceName));

    _rep.remove(pos);
}

CIMObjectPath MemoryResidentInstanceRepository::createInstance(
    const CIMNamespaceName& nameSpace,
    const CIMInstance& newInstance)
{
    TRACE;

    // Resolve the instance first:

    CIMInstance ci(newInstance.clone());
    CIMConstClass cc;
    MetaRepositoryDeclContext context;
    Resolver::resolveInstance(ci, &context, nameSpace, cc, false);
    CIMObjectPath cop = ci.buildPath(cc);

    ci.setPath(cop);

    // Reject if an instance with this name already exists:

    if (_findInstance(nameSpace, cop) != PEG_NOT_FOUND)
        _throw(CIM_ERR_ALREADY_EXISTS, "%s", *Str(cop));

    // Add instance to array:

    _rep.append(NamespaceInstancePair(nameSpace, ci));

    return cop;
}

void MemoryResidentInstanceRepository::modifyInstance(
    const CIMNamespaceName& nameSpace,
    const CIMInstance& modifiedInstance,
    Boolean includeQualifiers,
    const CIMPropertyList& propertyList)
{
    TRACE;

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
    MetaRepositoryDeclContext context;
    Resolver::resolveInstance(resultInstance, &context, nameSpace, cc, false);

    // Replace original instance.

    _rep[pos].second = resultInstance;
}

Array<CIMInstance> 
MemoryResidentInstanceRepository::enumerateInstancesForSubtree(
    const CIMNamespaceName& nameSpace,
    const CIMName& className,
    Boolean deepInheritance,
    Boolean localOnly,
    Boolean includeQualifiers,
    Boolean includeClassOrigin,
    const CIMPropertyList& propertyList)
{
    TRACE;

    // Form array of classnames for this class and descendent classes:

    Array<CIMName> classNames;
    classNames.append(className);
    MetaRepository::getSubClassNames(nameSpace, className, true, classNames);

    // Get all instances for this class and all descendent classes

    Array<CIMInstance> result;

    for (Uint32 i = 0; i < classNames.size(); i++)
    {
        Array<CIMInstance> instances = enumerateInstancesForClass(nameSpace, 
            classNames[i], false, includeQualifiers, includeClassOrigin, 
            propertyList);

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

Array<CIMInstance> MemoryResidentInstanceRepository::enumerateInstancesForClass(
    const CIMNamespaceName& nameSpace,
    const CIMName& className,
    Boolean localOnly,
    Boolean includeQualifiers,
    Boolean includeClassOrigin,
    const CIMPropertyList& propertyList)
{
    TRACE;

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

Array<CIMObjectPath> 
MemoryResidentInstanceRepository::enumerateInstanceNamesForSubtree(
    const CIMNamespaceName& nameSpace,
    const CIMName& className)
{
    TRACE;

    // Form array of classnames for this class and descendent classes:

    Array<CIMName> classNames;
    classNames.append(className);
    MetaRepository::getSubClassNames(nameSpace, className, true, classNames);

    // Get all instances for this class and all descendent classes

    Array<CIMObjectPath> result;

    for (Uint32 i = 0; i < classNames.size(); i++)
    {
        Array<CIMObjectPath> paths =
            enumerateInstanceNamesForClass(nameSpace, classNames[i]);

        result.appendArray(paths);
    }

    return result;
}

Array<CIMObjectPath> 
MemoryResidentInstanceRepository::enumerateInstanceNamesForClass(
    const CIMNamespaceName& nameSpace,
    const CIMName& className)
{
    TRACE;

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

Uint32 MemoryResidentInstanceRepository::_findInstance(
    const CIMNamespaceName& nameSpace,
    const CIMObjectPath& instanceName)
{
    TRACE;

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

// ATTN-MEB: Implement associator operations!

PEGASUS_NAMESPACE_END
