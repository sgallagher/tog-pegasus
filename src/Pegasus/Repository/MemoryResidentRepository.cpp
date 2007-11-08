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
#include <Pegasus/Common/Once.h>
#include <Pegasus/Common/System.h>
#include "MemoryResidentRepository.h"
#include "RepositoryDeclContext.h"
#include "Filtering.h"
#include "Serialization.h"
#include "SchemaTypes.h"

PEGASUS_NAMESPACE_BEGIN

typedef const SchemaClass* ConstSchemaClassPtr;
#define PEGASUS_ARRAY_T ConstSchemaClassPtr
# include <Pegasus/Common/ArrayInter.h>
# include <Pegasus/Common/ArrayImpl.h>
#undef PEGASUS_ARRAY_T

#define PEGASUS_ARRAY_T NamespaceInstancePair
# include <Pegasus/Common/ArrayImpl.h>
#undef PEGASUS_ARRAY_T

//==============================================================================
//
// Local definitions:
//
//==============================================================================

static size_t const _MAX_NAMESPACE_TABLE_SIZE = 64;
static const SchemaNameSpace* _nameSpaceTable[_MAX_NAMESPACE_TABLE_SIZE];
static size_t _nameSpaceTableSize = 0;

class ThrowContext
{
public:

    PEGASUS_FORMAT(3, 4)
    ThrowContext(CIMStatusCode code_, const char* format, ...) : code(code_)
    {
        char buffer[1024];
        va_list ap;
        va_start(ap, format);
        vsprintf(buffer, format, ap);
        va_end(ap);
        msg = buffer;
    }
    CIMStatusCode code;
    String msg;
};

#define Throw(ARGS) \
    do \
    { \
        ThrowContext c ARGS; \
        throw CIMException(c.code, c.msg); \
    } \
    while (0)

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
    const SchemaClass* sc,
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

            const SchemaFeature* sf = FindFeature(sc, 
                *Str(cp.getName()), SCHEMA_FLAG_PROPERTY|SCHEMA_FLAG_REFERENCE);

            if (!sf)
            {
                Throw((CIM_ERR_NOT_FOUND, 
                    "modifyInstance() failed: unknown property: %s",
                    *Str(cp.getName())));
            }

            // Reject attempts to modify key properties:

            if (sf->flags & SCHEMA_FLAG_KEY)
            {
                Throw((CIM_ERR_FAILED,
                    "modifyInstance() failed to modify key property: %s",
                    *Str(cp.getName())));
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

static Once _once = PEGASUS_ONCE_INITIALIZER;
static const char* _hostName = 0;

static void _initHostName()
{
    String hn = System::getHostName();
    _hostName = strdup(*Str(hn));
}

static inline const char* _getHostName()
{
    once(&_once, _initHostName);
    return _hostName;
}

static bool _eqi(const char* s1, const char* s2)
{
    return System::strcasecmp(s1, s2) == 0;
}

static const SchemaNameSpace* _findNameSpace(const char* name)
{
    for (size_t i = 0; i < _nameSpaceTableSize; i++)
    {
        const SchemaNameSpace* ns = _nameSpaceTable[i];

        if (_eqi(ns->name, name))
            return ns;
    }

    // Not found!
    return 0;
}

static bool _isSubClass(const SchemaClass* super, const SchemaClass* sub)
{
    if (!super)
        return true;

    for (SchemaClass* p = sub->super; p; p = p->super)
    {
        if (p == super)
            return true;
    }

    return false;
}

static inline bool _isDirectSubClass(
    const SchemaClass* super, 
    const SchemaClass* sub)
{
    return sub->super == super;
}

static char** _makePropertyList(const CIMPropertyList& propertyList)
{
    if (propertyList.isNull())
        return 0;

    size_t size = propertyList.size();
    char** pl = (char**)malloc(sizeof(char*) * (size + 1));

    for (size_t i = 0; i < size; i++)
        pl[i] = strdup(*Str(propertyList[i]));

    pl[size] = 0;

    return pl;
}

static void _freePropertyList(char** pl)
{
    if (!pl)
        return;

    for (size_t i = 0; pl[i]; i++)
    {
        free(pl[i]);
    }

    free(pl);
}

static void _printPropertyList(const char* const* pl)
{
    if (!pl)
        return;

    for (size_t i = 0; pl[i]; i++)
        printf("pl[%s]\n", pl[i]);
}

static bool _contains(const Array<const SchemaClass*>& x, const SchemaClass* sc)
{
    Uint32 n = x.size();
    const SchemaClass* const* p = x.getData();

    while (n--)
    {
        if (*p++ == sc)
            return true;
    }

    return false;
}

static void _associators(
    const SchemaNameSpace* ns,
    const CIMName& className,
    const CIMName& assocClass,
    const CIMName& resultClass,
    const String& role,
    const String& resultRole,
    Array<const SchemaClass*>& result)
{
    // Lookup source class:

    const SchemaClass* sc = FindClass(ns, *Str(className));
    
    if (!sc)
        Throw((CIM_ERR_NOT_FOUND, "unknown class: %s", *Str(className)));


    // Lookup result class (if any).

    const SchemaClass* rmc = 0;

    if (!resultClass.isNull())
    {
        rmc = FindClass(ns, *Str(resultClass));

        if (!rmc)
            Throw((CIM_ERR_NOT_FOUND, "unknown class: %s", *Str(resultClass)));
    }

    // Convert these to UTF8 now to avoid doing so in loop below.

    Str ac(assocClass);
    Str r(role);
    Str rr(resultRole);

    // Process association classes:

    for (size_t i = 0; ns->classes[i]; i++)
    {
        SchemaClass* amc = ns->classes[i];

        // Skip non-association classes:

        if (!(amc->flags & SCHEMA_FLAG_ASSOCIATION))
            continue;

        // Filter by assocClass parameter:

        if (!assocClass.isNull() && !_eqi(ac, amc->name))
            continue;

        // Process reference properties:

        SchemaFeatureInfo features[SCHEMA_MAX_FEATURES];
        size_t size = 0;
        MergeFeatures(amc, false, SCHEMA_FLAG_REFERENCE, features, size);

        for (size_t j = 0; j < size; j++)
        {
            const SchemaFeature* sf = features[j].sf;

            // Skip non references:

            if (!(sf->flags & SCHEMA_FLAG_REFERENCE))
                continue;

            const SchemaReference* sr = (const SchemaReference*)sf;

            // Filter by role parameter.

            if (role.size() && !_eqi(r, sf->name))
                continue;

            // Filter by source class:

            if (!IsA(sr->ref, sc))
                continue;

            // Process result reference:

            for (size_t k = 0; k < size; k++)
            {
                const SchemaFeature* rmf = features[k].sf;

                // Skip the feature under consideration:

                if (rmf == sf)
                    continue;

                // Skip non references:

                if (!(rmf->flags & SCHEMA_FLAG_REFERENCE))
                    continue;

                const SchemaReference* rmr = (const SchemaReference*)rmf;

                // Filter by resultRole parameter.

                if (resultRole.size() && !_eqi(rr, rmf->name))
                    continue;

                // Skip references not of the result class kind:

                if (rmc && !IsA(rmr->ref, rmc))
                    continue;

                // ATTN: should we include entire class hierarchy under
                // result class?

                // If reached, then save this one.

                if (!_contains(result, rmr->ref))
                    result.append(rmr->ref);
            }
        }
    }
}

static void _references(
    const SchemaNameSpace* ns,
    const CIMName& className,
    const CIMName& resultClass,
    const String& role,
    Array<const SchemaClass*>& result)
{
    // Lookup source class:

    const SchemaClass* sc = FindClass(ns, *Str(className));
    
    if (!sc)
        Throw((CIM_ERR_NOT_FOUND, "unknown class: %s", *Str(className)));

    // Lookup result class (if any).

    const SchemaClass* rmc = 0;

    if (!resultClass.isNull())
    {
        rmc = FindClass(ns, *Str(resultClass));

        if (!rmc)
            Throw((CIM_ERR_NOT_FOUND, "unknown class: %s", *Str(resultClass)));
    }

    // Convert these to UTF8 now to avoid doing so in loop below.

    Str r(role);

    // Process association classes:

    for (size_t i = 0; ns->classes[i]; i++)
    {
        SchemaClass* amc = ns->classes[i];

        // Skip non-association classes:

        if (!(amc->flags & SCHEMA_FLAG_ASSOCIATION))
            continue;

        // Filter by result class:

        if (rmc && !IsA(rmc, amc))
            continue;

        // Process reference properties:

        SchemaFeatureInfo features[SCHEMA_MAX_FEATURES];
        size_t size = 0;
        MergeFeatures(amc, false, SCHEMA_FLAG_REFERENCE, features, size);

        for (size_t j = 0; j < size; j++)
        {
            const SchemaFeature* sf = features[j].sf;

            // Skip non references:

            if (!(sf->flags & SCHEMA_FLAG_REFERENCE))
                continue;

            const SchemaReference* sr = (const SchemaReference*)sf;

            // Filter by role parameter.

            if (role.size() && !_eqi(r, sf->name))
                continue;

            // Filter by source class:

            if (!IsA(sr->ref, sc))
                continue;

            // Add this one to the output:

            if (!_contains(result, amc))
                result.append((SchemaClass*)amc);
        }
    }
}

static const SchemaClass* _findSchemaClass(
    const char* nameSpace,
    const char* className)
{
    // Lookup namespace:

    const SchemaNameSpace* ns = _findNameSpace(nameSpace);

    if (!ns)
        return 0;

    return FindClass(ns, className);
}

static Array<CIMName> _enumerateClassNames(
    const CIMNamespaceName& nameSpace,
    const CIMName& className,
    Boolean deepInheritance)
{
    // Lookup namespace:

    const SchemaNameSpace* ns = _findNameSpace(*Str(nameSpace));

    if (!ns)
        Throw((CIM_ERR_INVALID_NAMESPACE, "%s", *Str(nameSpace)));

    // Lookup class:

    const SchemaClass* super = 0;
    
    if (!className.isNull())
    {
        super = FindClass(ns, *Str(className));

        if (!super)
            Throw((CIM_ERR_NOT_FOUND, "unknown class: %s", *Str(className)));
    }

    // Iterate all classes looking for matches:

    Array<CIMName> result;

    for (size_t i = 0; ns->classes[i]; i++)
    {
        SchemaClass* sc = ns->classes[i];

        if (deepInheritance)
        {
            if (_isSubClass(super, sc))
                result.append(sc->name);
        }
        else
        {
            if (_isDirectSubClass(super, sc))
                result.append(sc->name);
        }
    }

    return result;
}

static void _getSubClassNames(
    const CIMNamespaceName& nameSpace,
    const CIMName& className,
    Boolean deepInheritance,
    Array<CIMName>& subClassNames)
{
    subClassNames = _enumerateClassNames(
        nameSpace, className, deepInheritance);
}

static Array<CIMObject> _associatorClasses(
    const CIMNamespaceName& nameSpace,
    const CIMName& className,
    const CIMName& assocClass,
    const CIMName& resultClass,
    const String& role,
    const String& resultRole,
    Boolean includeQualifiers,
    Boolean includeClassOrigin,
    const CIMPropertyList& propertyList)
{
    // Lookup namespace:

    const SchemaNameSpace* ns = _findNameSpace(*Str(nameSpace));

    if (!ns)
        Throw((CIM_ERR_INVALID_NAMESPACE, "%s", *Str(nameSpace)));

    // Get associator schema-classes:

    Array<const SchemaClass*> mcs;
    _associators(ns, className, assocClass, resultClass, role, resultRole, mcs);

    // Convert schema-classes to classes.

    Array<CIMObject> result;

    char** pl = _makePropertyList(propertyList);

    for (Uint32 i = 0; i < mcs.size(); i++)
    {
        const SchemaClass* sc = mcs[i];
        CIMClass cc;

        if (MakeClass(_getHostName(), ns, sc, false, includeQualifiers, 
            includeClassOrigin, pl, cc) != 0)
        {
            _freePropertyList(pl);
            Throw((CIM_ERR_FAILED, "conversion failed: %s", sc->name));
        }

        result.append(cc);
    }

    _freePropertyList(pl);
    return result;
}

static Array<CIMObjectPath> _associatorClassPaths(
    const CIMNamespaceName& nameSpace,
    const CIMName& className,
    const CIMName& assocClass,
    const CIMName& resultClass,
    const String& role,
    const String& resultRole)
{
    // Lookup namespace:

    const SchemaNameSpace* ns = _findNameSpace(*Str(nameSpace));

    if (!ns)
        Throw((CIM_ERR_INVALID_NAMESPACE, "%s", *Str(nameSpace)));

    // Get associator schema-classes:

    Array<const SchemaClass*> mcs;
    _associators(ns, className, assocClass, resultClass, role, resultRole, mcs);

    // Convert schema-classes to object names:

    Array<CIMObjectPath> result;

    for (Uint32 i = 0; i < mcs.size(); i++)
        result.append(CIMObjectPath(_getHostName(), nameSpace, mcs[i]->name));

    return result;
}

static Array<CIMObject> _referenceClasses(
    const CIMNamespaceName& nameSpace,
    const CIMName& className,
    const CIMName& resultClass,
    const String& role,
    Boolean includeQualifiers,
    Boolean includeClassOrigin,
    const CIMPropertyList& propertyList)
{
    // Lookup namespace:

    const SchemaNameSpace* ns = _findNameSpace(*Str(nameSpace));

    if (!ns)
        Throw((CIM_ERR_INVALID_NAMESPACE, "%s", *Str(nameSpace)));

    // Get reference schema-classes:

    Array<const SchemaClass*> mcs;
    _references(ns, className, resultClass, role, mcs);

    // Convert schema-classes to classes.

    Array<CIMObject> result;

    char** pl = _makePropertyList(propertyList);

    for (Uint32 i = 0; i < mcs.size(); i++)
    {
        const SchemaClass* sc = mcs[i];
        CIMClass cc;

        if (MakeClass(_getHostName(), ns, sc, false, includeQualifiers, 
            includeClassOrigin, pl, cc) != 0)
        {
            _freePropertyList(pl);
            Throw((CIM_ERR_FAILED, "conversion failed: %s", sc->name));
        }

        result.append(cc);
    }

    _freePropertyList(pl);
    return result;
}

static Array<CIMObjectPath> _referenceClassPaths(
    const CIMNamespaceName& nameSpace,
    const CIMName& className,
    const CIMName& resultClass,
    const String& role)
{
    // Lookup namespace:

    const SchemaNameSpace* ns = _findNameSpace(*Str(nameSpace));

    if (!ns)
        Throw((CIM_ERR_INVALID_NAMESPACE, "%s", *Str(nameSpace)));

    // Get reference schema-classes:

    Array<const SchemaClass*> mcs;
    _references(ns, className, resultClass, role, mcs);

    // Convert schema-classes to object paths.

    Array<CIMObjectPath> result;

    for (Uint32 i = 0; i < mcs.size(); i++)
        result.append(CIMObjectPath(_getHostName(), nameSpace, mcs[i]->name));

    return result;
}

static CIMQualifierDecl _getQualifier(
    const CIMNamespaceName& nameSpace,
    const CIMName& qualifierName)
{
    // Lookup namespace:

    const SchemaNameSpace* ns = _findNameSpace(*Str(nameSpace));

    if (!ns)
        Throw((CIM_ERR_INVALID_NAMESPACE, "%s", *Str(nameSpace)));

    // Lookup qualifier:

    const SchemaQualifierDecl* mqd = FindQualifierDecl(ns, *Str(qualifierName));
    
    if (!mqd)
        Throw((CIM_ERR_NOT_FOUND, 
            "unknown qualifier: %s", *Str(qualifierName)));

    // Make the qualifier declaration:

    CIMQualifierDecl cqd;

    if (MakeQualifierDecl(ns, mqd, cqd) != 0)
    {
        Throw((CIM_ERR_FAILED, "conversion failed: %s", mqd->name));
    }

    return cqd;
}

static Array<CIMQualifierDecl> _enumerateQualifiers(
    const CIMNamespaceName& nameSpace)
{
    // Lookup namespace:

    const SchemaNameSpace* ns = _findNameSpace(*Str(nameSpace));

    if (!ns)
        Throw((CIM_ERR_INVALID_NAMESPACE, "%s", *Str(nameSpace)));

    // Build the array of qualifier declarations:

    Array<CIMQualifierDecl> result;

    for (size_t i = 0; ns->qualifiers[i]; i++)
    {
        const SchemaQualifierDecl* mqd = ns->qualifiers[i];
        CIMQualifierDecl cqd;

        if (MakeQualifierDecl(ns, mqd, cqd) != 0)
        {
            Throw((CIM_ERR_FAILED, "conversion failed: %s", mqd->name));
        }

        result.append(cqd);
    }

    return result;

}

static Array<CIMNamespaceName> _enumerateNameSpaces()
{
    Array<CIMNamespaceName> result;

    for (size_t i = 0; i < _nameSpaceTableSize; i++)
    {
        const SchemaNameSpace* ns = _nameSpaceTable[i];
        result.append(ns->name);
    }

    return result;
}

static void _getSuperClassNames(
    const CIMNamespaceName& nameSpace,
    const CIMName& className,
    Array<CIMName>& superClassNames)
{
    superClassNames.clear();

    // Lookup namespace:

    const SchemaNameSpace* ns = _findNameSpace(*Str(nameSpace));

    if (!ns)
        Throw((CIM_ERR_INVALID_NAMESPACE, "%s", *Str(nameSpace)));

    // Lookup class:

    const SchemaClass* sc = FindClass(ns, *Str(className));
    
    if (!sc)
        Throw((CIM_ERR_NOT_FOUND, "unknown class: %s", *Str(className)));

    // Append superclass names:

    for (const SchemaClass* p = sc->super; p; p = p->super)
        superClassNames.append(p->name);
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

MemoryResidentRepository::MemoryResidentRepository()
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
    // Lookup namespace:

    const SchemaNameSpace* ns = _findNameSpace(*Str(nameSpace));

    if (!ns)
        Throw((CIM_ERR_INVALID_NAMESPACE, "%s", *Str(nameSpace)));

    // Lookup class:

    const SchemaClass* sc = FindClass(ns, *Str(className));

    if (!sc)
    {
        Throw((CIM_ERR_NOT_FOUND, "unknown class: %s", *Str(className)));
    }

    // Build property list:

    char** pl = _makePropertyList(propertyList);

    // Make class:

    CIMClass cc;

    if (MakeClass(_getHostName(), ns, sc, localOnly, includeQualifiers, 
        includeClassOrigin, pl, cc) != 0)
    {
        _freePropertyList(pl);
        Throw((CIM_ERR_FAILED, "conversion failed: %s", sc->name));
    }

    _freePropertyList(pl);
    return cc;
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
        Throw((CIM_ERR_NOT_FOUND, "%s", *Str(instanceName)));

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
    Throw((CIM_ERR_NOT_SUPPORTED, "deleteClass()"));
}

void MemoryResidentRepository::deleteInstance(
    bool lock,
    const CIMNamespaceName& nameSpace,
    const CIMObjectPath& instanceName)
{
    Uint32 pos = _findInstance(nameSpace, instanceName);

    if (pos == PEG_NOT_FOUND)
        Throw((CIM_ERR_NOT_FOUND, "%s", *Str(instanceName)));

    _rep.remove(pos);
    _processSaveHandler();
}

void MemoryResidentRepository::createClass(
    bool lock,
    const CIMNamespaceName& nameSpace,
    const CIMClass& newClass,
    const ContentLanguageList& contentLangs)
{
    Throw((CIM_ERR_NOT_SUPPORTED, "createClass()"));
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
        Throw((CIM_ERR_ALREADY_EXISTS, "%s", *Str(cop)));

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
    Throw((CIM_ERR_NOT_SUPPORTED, "modifyClass()"));
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

    // Get the schema-class for this instance.

    const SchemaClass* sc = _findSchemaClass(*Str(nameSpace), *Str(className));

    if (!sc)
    {
        Throw((CIM_ERR_FAILED, 
            "modifyInstance() failed: unknown class: %s:%s",
            *Str(nameSpace), *Str(className)));
    }

    // Get original instance to be modified:

    Uint32 pos = _findInstance(nameSpace, cop);

    if (pos == PEG_NOT_FOUND)
    {
        Throw((CIM_ERR_NOT_FOUND, 
            "modifyInstance() failed: unknown instance: %s",
            *Str(cop.toString())));
    }

    CIMInstance resultInstance = _rep[pos].second.clone();

    // Apply features of modifiedInstance to result instance.

    _applyModifiedInstance(sc, modifiedInstance, propertyList, resultInstance);

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
    // Lookup namespace:

    const SchemaNameSpace* ns = _findNameSpace(*Str(nameSpace));

    if (!ns)
        Throw((CIM_ERR_INVALID_NAMESPACE, "%s", *Str(nameSpace)));

    // Lookup class:

    const SchemaClass* super = 0;
    
    if (!className.isNull())
    {
        super = FindClass(ns, *Str(className));

        if (!super)
            Throw((CIM_ERR_NOT_FOUND, "unknown class: %s", *Str(className)));
    }

    // Iterate all classes looking for matches:

    Array<CIMClass> result;

    for (size_t i = 0; ns->classes[i]; i++)
    {
        SchemaClass* sc = ns->classes[i];

        bool flag = false;

        if (deepInheritance)
        {
            if (_isSubClass(super, sc))
                flag = true;
        }
        else
        {
            if (_isDirectSubClass(super, sc))
                flag = true;
        }

        if (flag)
        {
            CIMClass cc;

            if (MakeClass(_getHostName(), ns, sc, localOnly, includeQualifiers, 
                includeClassOrigin, 0, cc) != 0)
            {
                Throw((CIM_ERR_FAILED, "conversion failed: %s", sc->name));
            }

            result.append(cc);
        }
    }

    return result;
}

Array<CIMName> MemoryResidentRepository::enumerateClassNames(
    bool lock,
    const CIMNamespaceName& nameSpace,
    const CIMName& className,
    Boolean deepInheritance)
{
    return _enumerateClassNames(nameSpace, className, deepInheritance);
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
    _getSubClassNames(nameSpace, className, true, classNames);

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
    _getSubClassNames(nameSpace, className, true, classNames);

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
    Throw((CIM_ERR_NOT_SUPPORTED, "execQuery()"));
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
        return _associatorClasses(
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
        Throw((CIM_ERR_NOT_SUPPORTED, "associators()"));
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
        return _associatorClassPaths(
            nameSpace,
            objectName.getClassName(),
            assocClass,
            resultClass,
            role,
            resultRole);
    }
    else
    {
        Throw((CIM_ERR_NOT_SUPPORTED, "associatorNames()"));
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
        return _referenceClasses(
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
        Throw((CIM_ERR_NOT_SUPPORTED, "references()"));
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
        return _referenceClassPaths(
            nameSpace,
            objectName.getClassName(),
            resultClass,
            role);
    }
    else
    {
        Throw((CIM_ERR_NOT_SUPPORTED, "referenceNames()"));
        return Array<CIMObjectPath>();
    }
}

CIMValue MemoryResidentRepository::getProperty(
    bool lock,
    const CIMNamespaceName& nameSpace,
    const CIMObjectPath& instanceName,
    const CIMName& propertyName)
{
    Throw((CIM_ERR_NOT_SUPPORTED, "getProperty()"));
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
    Throw((CIM_ERR_NOT_SUPPORTED, "setProperty()"));
}

CIMQualifierDecl MemoryResidentRepository::getQualifier(
    bool lock,
    const CIMNamespaceName& nameSpace,
    const CIMName& qualifierName)
{
    return _getQualifier(nameSpace, qualifierName);
}

void MemoryResidentRepository::setQualifier(
    bool lock,
    const CIMNamespaceName& nameSpace,
    const CIMQualifierDecl& qualifierDecl,
    const ContentLanguageList& contentLangs)
{
    Throw((CIM_ERR_NOT_SUPPORTED, "setQualifier()"));
}

void MemoryResidentRepository::deleteQualifier(
    bool lock,
    const CIMNamespaceName& nameSpace,
    const CIMName& qualifierName)
{
    Throw((CIM_ERR_NOT_SUPPORTED, "deleteQualifier()"));
}

Array<CIMQualifierDecl> MemoryResidentRepository::enumerateQualifiers(
    bool lock,
    const CIMNamespaceName& nameSpace)
{
    return _enumerateQualifiers(nameSpace);
}

void MemoryResidentRepository::createNameSpace(
    bool lock,
    const CIMNamespaceName& nameSpace,
    const NameSpaceAttributes& attributes)
{
    Throw((CIM_ERR_NOT_SUPPORTED, "createNameSpace()"));
}

void MemoryResidentRepository::modifyNameSpace(
    bool lock,
    const CIMNamespaceName& nameSpace,
    const NameSpaceAttributes& attributes)
{
    Throw((CIM_ERR_NOT_SUPPORTED, "modifyNameSpace()"));
}

Array<CIMNamespaceName> MemoryResidentRepository::enumerateNameSpaces(
    bool lock) const
{
    return _enumerateNameSpaces();
}

void MemoryResidentRepository::deleteNameSpace(
    bool lock,
    const CIMNamespaceName& nameSpace)
{
    Throw((CIM_ERR_NOT_SUPPORTED, "deleteNameSpace()"));
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
    Throw((CIM_ERR_NOT_SUPPORTED, "setDeclContext()"));
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
    _getSubClassNames(nameSpace, className, deepInheritance, subClassNames);
}

void MemoryResidentRepository::getSuperClassNames(
    bool lock,
    const CIMNamespaceName& nameSpace,
    const CIMName& className,
    Array<CIMName>& superClassNames) const
{
    _getSuperClassNames(nameSpace, className, superClassNames);
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

Boolean MemoryResidentRepository::addNameSpace(const SchemaNameSpace* nameSpace)
{
    if (!nameSpace)
        return false;

    if (_nameSpaceTableSize == _MAX_NAMESPACE_TABLE_SIZE)
        return false;

    if (_findNameSpace(nameSpace->name))
        return false;

    _nameSpaceTable[_nameSpaceTableSize++] = nameSpace;

    return true;
}

PEGASUS_NAMESPACE_END
