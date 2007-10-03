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
#include <cassert>
#include "MetaRepository.h"
#include <Pegasus/Common/System.h>
#include <Pegasus/Common/Once.h>
#include "MetaTypes.h"

PEGASUS_NAMESPACE_BEGIN

typedef const MetaClass* ConstMetaClassPtr;
#define PEGASUS_ARRAY_T ConstMetaClassPtr
# include <Pegasus/Common/ArrayInter.h>
# include <Pegasus/Common/ArrayImpl.h>
#undef PEGASUS_ARRAY_T

static const size_t _MAX_NAMESPACES = 32;
static const MetaNameSpace* _nameSpaces[_MAX_NAMESPACES];
static size_t _nameSpacesSize = 0;

static const size_t _MAX_FEATURES = 1024;
static const size_t _MAX_QUALIFIERS = 1024;

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

static bool _eqi(const char* s1, const char* s2)
{
    return System::strcasecmp(s1, s2) == 0;
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

static const MetaNameSpace* _findNameSpace(const char* name)
{
    for (size_t i = 0; i < _nameSpacesSize; i++)
    {
        if (_eqi(_nameSpaces[i]->name, name))
            return _nameSpaces[i];
    }

    // Not found!
    return 0;
}

static bool _isSubClass(const MetaClass* super, const MetaClass* sub)
{
    if (!super)
        return true;

    for (MetaClass* p = sub->super; p; p = p->super)
    {
        if (p == super)
            return true;
    }

    return false;
}

static inline bool _isDirectSubClass(
    const MetaClass* super, 
    const MetaClass* sub)
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

static bool _contains(const Array<const MetaClass*>& x, const MetaClass* mc)
{
    Uint32 n = x.size();
    const MetaClass* const* p = x.getData();

    while (n--)
    {
        if (*p++ == mc)
            return true;
    }

    return false;
}

static void _associators(
    const MetaNameSpace* ns,
    const CIMName& className,
    const CIMName& assocClass,
    const CIMName& resultClass,
    const String& role,
    const String& resultRole,
    Array<const MetaClass*>& result)
{
    // Lookup source class:

    const MetaClass* mc = FindClass(ns, *Str(className));
    
    if (!mc)
        _throw(CIM_ERR_NOT_FOUND, "unknown class: %s", *Str(className));


    // Lookup result class (if any).

    const MetaClass* rmc = 0;

    if (!resultClass.isNull())
    {
        rmc = FindClass(ns, *Str(resultClass));

        if (!rmc)
            _throw(CIM_ERR_NOT_FOUND, "unknown class: %s", *Str(resultClass));
    }

    // Convert these to UTF8 now to avoid doing so in loop below.

    Str ac(assocClass);
    Str r(role);
    Str rr(resultRole);

    // Process association classes:

    for (size_t i = 0; ns->classes[i]; i++)
    {
        MetaClass* amc = ns->classes[i];

        // Skip non-association classes:

        if (!(amc->flags & META_FLAG_ASSOCIATION))
            continue;

        // Filter by assocClass parameter:

        if (!assocClass.isNull() && !_eqi(ac, amc->name))
            continue;

        // Process reference properties:

        MetaFeatureInfo features[META_MAX_FEATURES];
        size_t size = 0;
        MergeFeatures(amc, false, META_FLAG_REFERENCE, features, size);

        for (size_t j = 0; j < size; j++)
        {
            const MetaFeature* mf = features[j].mf;

            // Skip non references:

            if (!(mf->flags & META_FLAG_REFERENCE))
                continue;

            const MetaReference* mr = (const MetaReference*)mf;

            // Filter by role parameter.

            if (role.size() && !_eqi(r, mf->name))
                continue;

            // Filter by source class:

            if (!IsA(mr->ref, mc))
                continue;

            // Process result reference:

            for (size_t k = 0; k < size; k++)
            {
                const MetaFeature* rmf = features[k].mf;

                // Skip the feature under consideration:

                if (rmf == mf)
                    continue;

                // Skip non references:

                if (!(rmf->flags & META_FLAG_REFERENCE))
                    continue;

                const MetaReference* rmr = (const MetaReference*)rmf;

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
    const MetaNameSpace* ns,
    const CIMName& className,
    const CIMName& resultClass,
    const String& role,
    Array<const MetaClass*>& result)
{
    // Lookup source class:

    const MetaClass* mc = FindClass(ns, *Str(className));
    
    if (!mc)
        _throw(CIM_ERR_NOT_FOUND, "unknown class: %s", *Str(className));

    // Lookup result class (if any).

    const MetaClass* rmc = 0;

    if (!resultClass.isNull())
    {
        rmc = FindClass(ns, *Str(resultClass));

        if (!rmc)
            _throw(CIM_ERR_NOT_FOUND, "unknown class: %s", *Str(resultClass));
    }

    // Convert these to UTF8 now to avoid doing so in loop below.

    Str r(role);

    // Process association classes:

    for (size_t i = 0; ns->classes[i]; i++)
    {
        MetaClass* amc = ns->classes[i];

        // Skip non-association classes:

        if (!(amc->flags & META_FLAG_ASSOCIATION))
            continue;

        // Filter by result class:

        if (rmc && !IsA(rmc, amc))
            continue;

        // Process reference properties:

        MetaFeatureInfo features[META_MAX_FEATURES];
        size_t size = 0;
        MergeFeatures(amc, false, META_FLAG_REFERENCE, features, size);

        for (size_t j = 0; j < size; j++)
        {
            const MetaFeature* mf = features[j].mf;

            // Skip non references:

            if (!(mf->flags & META_FLAG_REFERENCE))
                continue;

            const MetaReference* mr = (const MetaReference*)mf;

            // Filter by role parameter.

            if (role.size() && !_eqi(r, mf->name))
                continue;

            // Filter by source class:

            if (!IsA(mr->ref, mc))
                continue;

            // Add this one to the output:

            if (!_contains(result, amc))
                result.append((MetaClass*)amc);
        }
    }
}

//==============================================================================
//
// _getHostName()
//
//==============================================================================

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

//==============================================================================
//
// class MetaRepository
//
//==============================================================================

MetaRepository::MetaRepository()
{
}

MetaRepository::~MetaRepository()
{
}

bool MetaRepository::addNameSpace(const MetaNameSpace* nameSpace)
{
    if (_nameSpacesSize == _MAX_NAMESPACES || !nameSpace)
        return false;

    for (size_t i = 0; i < _nameSpacesSize; i++)
    {
        if (_eqi(_nameSpaces[i]->name, nameSpace->name))
            return false;
    }

    _nameSpaces[_nameSpacesSize++] = nameSpace;
    return true;
}

CIMClass MetaRepository::getClass(
    const CIMNamespaceName& nameSpace,
    const CIMName& className,
    Boolean localOnly,
    Boolean includeQualifiers,
    Boolean includeClassOrigin,
    const CIMPropertyList& propertyList)
{
    // Lookup namespace:

    const MetaNameSpace* ns = _findNameSpace(*Str(nameSpace));

    if (!ns)
        _throw(CIM_ERR_INVALID_NAMESPACE, "%s", *Str(nameSpace));

    // Lookup class:

    const MetaClass* mc = FindClass(ns, *Str(className));

    if (!mc)
        _throw(CIM_ERR_NOT_FOUND, "unknown class: %s", *Str(className));

    // Build property list:

    char** pl = _makePropertyList(propertyList);

    // Make class:

    CIMClass cc;

    if (MakeClass(_getHostName(), ns, mc, localOnly, includeQualifiers, 
        includeClassOrigin, pl, cc) != 0)
    {
        _freePropertyList(pl);
        _throw(CIM_ERR_FAILED, "conversion failed: %s", mc->name);
    }

    _freePropertyList(pl);
    return cc;
}

Array<CIMClass> MetaRepository::enumerateClasses(
    const CIMNamespaceName& nameSpace,
    const CIMName& className,
    Boolean deepInheritance,
    Boolean localOnly,
    Boolean includeQualifiers,
    Boolean includeClassOrigin)
{
    // Lookup namespace:

    const MetaNameSpace* ns = _findNameSpace(*Str(nameSpace));

    if (!ns)
        _throw(CIM_ERR_INVALID_NAMESPACE, "%s", *Str(nameSpace));

    // Lookup class:

    const MetaClass* super = 0;
    
    if (!className.isNull())
    {
        super = FindClass(ns, *Str(className));

        if (!super)
            _throw(CIM_ERR_NOT_FOUND, "unknown class: %s", *Str(className));
    }

    // Iterate all classes looking for matches:

    Array<CIMClass> result;

    for (size_t i = 0; ns->classes[i]; i++)
    {
        MetaClass* mc = ns->classes[i];

        bool flag = false;

        if (deepInheritance)
        {
            if (_isSubClass(super, mc))
                flag = true;
        }
        else
        {
            if (_isDirectSubClass(super, mc))
                flag = true;
        }

        if (flag)
        {
            CIMClass cc;

            if (MakeClass(_getHostName(), ns, mc, localOnly, includeQualifiers, 
                includeClassOrigin, 0, cc) != 0)
            {
                _throw(CIM_ERR_FAILED, "conversion failed: %s", mc->name);
            }

            result.append(cc);
        }
    }

    return result;
}

Array<CIMName> MetaRepository::enumerateClassNames(
    const CIMNamespaceName& nameSpace,
    const CIMName& className,
    Boolean deepInheritance)
{
    // Lookup namespace:

    const MetaNameSpace* ns = _findNameSpace(*Str(nameSpace));

    if (!ns)
        _throw(CIM_ERR_INVALID_NAMESPACE, "%s", *Str(nameSpace));

    // Lookup class:

    const MetaClass* super = 0;
    
    if (!className.isNull())
    {
        super = FindClass(ns, *Str(className));

        if (!super)
            _throw(CIM_ERR_NOT_FOUND, "unknown class: %s", *Str(className));
    }

    // Iterate all classes looking for matches:

    Array<CIMName> result;

    for (size_t i = 0; ns->classes[i]; i++)
    {
        MetaClass* mc = ns->classes[i];

        if (deepInheritance)
        {
            if (_isSubClass(super, mc))
                result.append(mc->name);
        }
        else
        {
            if (_isDirectSubClass(super, mc))
                result.append(mc->name);
        }
    }

    return result;
}

void MetaRepository::deleteClass(
    const CIMNamespaceName& nameSpace,
    const CIMName& className)
{
    _throw(CIM_ERR_NOT_SUPPORTED, "deleteClass()");
}

void MetaRepository::createClass(
    const CIMNamespaceName& nameSpace,
    const CIMClass& newClass)
{
    _throw(CIM_ERR_NOT_SUPPORTED, "createClass()");
}

void MetaRepository::modifyClass(
    const CIMNamespaceName& nameSpace,
    const CIMClass& newClass)
{
    _throw(CIM_ERR_NOT_SUPPORTED, "modifyClass()");
}

void MetaRepository::getSubClassNames(
    const CIMNamespaceName& nameSpace,
    const CIMName& className,
    Boolean deepInheritance,
    Array<CIMName>& subClassNames)
{
    subClassNames = MetaRepository::enumerateClassNames(
        nameSpace, className, deepInheritance);
}

void MetaRepository::getSuperClassNames(
    const CIMNamespaceName& nameSpace,
    const CIMName& className,
    Array<CIMName>& superClassNames)
{
    superClassNames.clear();

    // Lookup namespace:

    const MetaNameSpace* ns = _findNameSpace(*Str(nameSpace));

    if (!ns)
        _throw(CIM_ERR_INVALID_NAMESPACE, "%s", *Str(nameSpace));

    // Lookup class:

    const MetaClass* mc = FindClass(ns, *Str(className));
    
    if (!mc)
        _throw(CIM_ERR_NOT_FOUND, "unknown class: %s", *Str(className));

    // Append superclass names:

    for (const MetaClass* p = mc->super; p; p = p->super)
        superClassNames.append(p->name);
}

void MetaRepository::createNameSpace(
    const CIMNamespaceName& nameSpace,
    const NameSpaceAttributes& attributes)
{
    _throw(CIM_ERR_NOT_SUPPORTED, "createNameSpace()");
}

void MetaRepository::modifyNameSpace(
    const CIMNamespaceName& nameSpace,
    const NameSpaceAttributes& attributes)
{
    _throw(CIM_ERR_NOT_SUPPORTED, "modifyNameSpace()");
}

Array<CIMNamespaceName> MetaRepository::enumerateNameSpaces()
{
    Array<CIMNamespaceName> nameSpaces;

    for (size_t i = 0; i < _nameSpacesSize; i++)
        nameSpaces.append(_nameSpaces[i]->name);

    return Array<CIMNamespaceName>();
}

void MetaRepository::deleteNameSpace(
    const CIMNamespaceName& nameSpace)
{
    _throw(CIM_ERR_NOT_SUPPORTED, "deleteNameSpace()");
}

Boolean MetaRepository::getNameSpaceAttributes(
    const CIMNamespaceName& nameSpace,
    NameSpaceAttributes& attributes)
{
    _throw(CIM_ERR_NOT_SUPPORTED, "getNameSpaceAttributes()");

    return false;
}

Boolean MetaRepository::isRemoteNameSpace(
    const CIMNamespaceName& nameSpace,
    String& remoteInfo)
{
    return false;
}

CIMQualifierDecl MetaRepository::getQualifier(
    const CIMNamespaceName& nameSpace,
    const CIMName& qualifierName)
{
    // Lookup namespace:

    const MetaNameSpace* ns = _findNameSpace(*Str(nameSpace));

    if (!ns)
        _throw(CIM_ERR_INVALID_NAMESPACE, "%s", *Str(nameSpace));

    // Lookup qualifier:

    const MetaQualifierDecl* mqd = FindQualifierDecl(ns, *Str(qualifierName));
    
    if (!mqd)
        _throw(CIM_ERR_NOT_FOUND, "unknown qualifier: %s", *Str(qualifierName));

    // Make the qualifier declaration:

    CIMQualifierDecl cqd;

    if (MakeQualifierDecl(ns, mqd, cqd) != 0)
    {
        _throw(CIM_ERR_FAILED, "conversion failed: %s", mqd->name);
    }

    return cqd;
}

void MetaRepository::setQualifier(
    const CIMNamespaceName& nameSpace,
    const CIMQualifierDecl& qualifierDecl)
{
    _throw(CIM_ERR_NOT_SUPPORTED, "setQualifier()");

}

void MetaRepository::deleteQualifier(
    const CIMNamespaceName& nameSpace,
    const CIMName& qualifierName)
{
    _throw(CIM_ERR_NOT_SUPPORTED, "deleteQualifier()");
}

Array<CIMQualifierDecl> MetaRepository::enumerateQualifiers(
    const CIMNamespaceName& nameSpace)
{
    // Lookup namespace:

    const MetaNameSpace* ns = _findNameSpace(*Str(nameSpace));

    if (!ns)
        _throw(CIM_ERR_INVALID_NAMESPACE, "%s", *Str(nameSpace));

    // Build the array of qualifier declarations:

    Array<CIMQualifierDecl> result;

    for (size_t i = 0; ns->qualifiers[i]; i++)
    {
        const MetaQualifierDecl* mqd = ns->qualifiers[i];
        CIMQualifierDecl cqd;

        if (MakeQualifierDecl(ns, mqd, cqd) != 0)
        {
            _throw(CIM_ERR_FAILED, "conversion failed: %s", mqd->name);
        }

        result.append(cqd);
    }

    return result;
}

Array<CIMObjectPath> MetaRepository::associatorClassPaths(
    const CIMNamespaceName& nameSpace,
    const CIMName& className,
    const CIMName& assocClass,
    const CIMName& resultClass,
    const String& role,
    const String& resultRole)
{
    // Lookup namespace:

    const MetaNameSpace* ns = _findNameSpace(*Str(nameSpace));

    if (!ns)
        _throw(CIM_ERR_INVALID_NAMESPACE, "%s", *Str(nameSpace));

    // Get associator meta-classes:

    Array<const MetaClass*> mcs;
    _associators(ns, className, assocClass, resultClass, role, resultRole, mcs);

    // Convert meta-classes to object names:

    Array<CIMObjectPath> result;

    for (Uint32 i = 0; i < mcs.size(); i++)
        result.append(CIMObjectPath(_getHostName(), nameSpace, mcs[i]->name));

    return result;
}

Array<CIMObject> MetaRepository::associatorClasses(
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

    const MetaNameSpace* ns = _findNameSpace(*Str(nameSpace));

    if (!ns)
        _throw(CIM_ERR_INVALID_NAMESPACE, "%s", *Str(nameSpace));

    // Get associator meta-classes:

    Array<const MetaClass*> mcs;
    _associators(ns, className, assocClass, resultClass, role, resultRole, mcs);

    // Convert meta-classes to classes.

    Array<CIMObject> result;

    char** pl = _makePropertyList(propertyList);

    for (Uint32 i = 0; i < mcs.size(); i++)
    {
        const MetaClass* mc = mcs[i];
        CIMClass cc;

        if (MakeClass(_getHostName(), ns, mc, false, includeQualifiers, 
            includeClassOrigin, pl, cc) != 0)
        {
            _freePropertyList(pl);
            _throw(CIM_ERR_FAILED, "conversion failed: %s", mc->name);
        }

        result.append(cc);
    }

    _freePropertyList(pl);
    return result;
}

Array<CIMObject> MetaRepository::referenceClasses(
    const CIMNamespaceName& nameSpace,
    const CIMName& className,
    const CIMName& resultClass,
    const String& role,
    Boolean includeQualifiers,
    Boolean includeClassOrigin,
    const CIMPropertyList& propertyList)
{
    // Lookup namespace:

    const MetaNameSpace* ns = _findNameSpace(*Str(nameSpace));

    if (!ns)
        _throw(CIM_ERR_INVALID_NAMESPACE, "%s", *Str(nameSpace));

    // Get reference meta-classes:

    Array<const MetaClass*> mcs;
    _references(ns, className, resultClass, role, mcs);

    // Convert meta-classes to classes.

    Array<CIMObject> result;

    char** pl = _makePropertyList(propertyList);

    for (Uint32 i = 0; i < mcs.size(); i++)
    {
        const MetaClass* mc = mcs[i];
        CIMClass cc;

        if (MakeClass(_getHostName(), ns, mc, false, includeQualifiers, 
            includeClassOrigin, pl, cc) != 0)
        {
            _freePropertyList(pl);
            _throw(CIM_ERR_FAILED, "conversion failed: %s", mc->name);
        }

        result.append(cc);
    }

    _freePropertyList(pl);
    return result;
}

Array<CIMObjectPath> MetaRepository::referenceClassPaths(
    const CIMNamespaceName& nameSpace,
    const CIMName& className,
    const CIMName& resultClass,
    const String& role)
{
    // Lookup namespace:

    const MetaNameSpace* ns = _findNameSpace(*Str(nameSpace));

    if (!ns)
        _throw(CIM_ERR_INVALID_NAMESPACE, "%s", *Str(nameSpace));

    // Get reference meta-classes:

    Array<const MetaClass*> mcs;
    _references(ns, className, resultClass, role, mcs);

    // Convert meta-classes to object paths.

    Array<CIMObjectPath> result;

    for (Uint32 i = 0; i < mcs.size(); i++)
        result.append(CIMObjectPath(_getHostName(), nameSpace, mcs[i]->name));

    return result;
}

PEGASUS_NAMESPACE_END
