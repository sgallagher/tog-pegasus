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
#include "MetaTypes.h"

PEGASUS_NAMESPACE_BEGIN

static const size_t _MAX_NAMESPACES = 32;
static const MetaNameSpace* _nameSpaces[_MAX_NAMESPACES];
static size_t _nameSpacesSize = 0;

static const size_t _MAX_FEATURES = 1024;
static const size_t _MAX_QUALIFIERS = 1024;

//==============================================================================
//
// Local definitions:
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

static const MetaClass* _findClass(
    const MetaNameSpace* ns, 
    const char* name)
{
    for (size_t i = 0; ns->classes[i]; i++)
    {
        const MetaClass* mc = ns->classes[i];

        if (_eqi(mc->name, name))
            return mc;
    }

    // Not found!
    return 0;
}

static const MetaQualifierDecl* _findQualifierDecl(
    const MetaNameSpace* ns, 
    const char* name)
{
    for (size_t i = 0; ns->classes[i]; i++)
    {
        const MetaQualifierDecl* mqd = ns->qualifiers[i];

        if (_eqi(mqd->name, name))
            return mqd;
    }

    // Not found!
    return 0;
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
    printf("===== MetaRepository::getClass()\n");

    // Lookup namespace:

    const MetaNameSpace* ns = _findNameSpace(*Str(nameSpace));

    if (!ns)
        _throw(CIM_ERR_INVALID_NAMESPACE, "%s", *Str(nameSpace));

    // Lookup class:

    const MetaClass* mc = _findClass(ns, *Str(className));

    if (!mc)
        _throw(CIM_ERR_NOT_FOUND, "unknown class: %s", *Str(className));

    // Build property list:

    char** pl = _makePropertyList(propertyList);

    // Make class:

    CIMClass cc;

    if (MakeClass(ns, mc, localOnly, includeQualifiers, 
        includeQualifiers, pl, cc) != 0)
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
    printf("===== MetaRepository::enumerateClasses()\n");

    // Lookup namespace:

    const MetaNameSpace* ns = _findNameSpace(*Str(nameSpace));

    if (!ns)
        _throw(CIM_ERR_INVALID_NAMESPACE, "%s", *Str(nameSpace));

    // Lookup class:

    const MetaClass* super = 0;
    
    if (!className.isNull())
    {
        super = _findClass(ns, *Str(className));

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

            if (MakeClass(ns, mc, localOnly, includeQualifiers, 
                includeQualifiers, 0, cc) != 0)
            {
                _throw(CIM_ERR_FAILED, "conversion failed: %s", mc->name);
            }
            else
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
    printf("===== MetaRepository::enumerateClassNames()\n");

    // Lookup namespace:

    const MetaNameSpace* ns = _findNameSpace(*Str(nameSpace));

    if (!ns)
        _throw(CIM_ERR_INVALID_NAMESPACE, "%s", *Str(nameSpace));

    // Lookup class:

    const MetaClass* super = 0;
    
    if (!className.isNull())
    {
        super = _findClass(ns, *Str(className));

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
    printf("===== MetaRepository::getSubClassNames()\n");

    subClassNames = MetaRepository::enumerateClassNames(
        nameSpace, className, deepInheritance);
}

void MetaRepository::getSuperClassNames(
    const CIMNamespaceName& nameSpace,
    const CIMName& className,
    Array<CIMName>& superClassNames)
{
    printf("===== MetaRepository::getSuperClassNames()\n");

    superClassNames.clear();

    // Lookup namespace:

    const MetaNameSpace* ns = _findNameSpace(*Str(nameSpace));

    if (!ns)
        _throw(CIM_ERR_INVALID_NAMESPACE, "%s", *Str(nameSpace));

    // Lookup class:

    const MetaClass* mc = _findClass(ns, *Str(className));
    
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
    printf("===== MetaRepository::createNameSpace()\n");

    _throw(CIM_ERR_NOT_SUPPORTED, "createNameSpace()");
}

void MetaRepository::modifyNameSpace(
    const CIMNamespaceName& nameSpace,
    const NameSpaceAttributes& attributes)
{
    printf("===== MetaRepository::modifyNameSpace()\n");

    _throw(CIM_ERR_NOT_SUPPORTED, "modifyNameSpace()");
}

Array<CIMNamespaceName> MetaRepository::enumerateNameSpaces()
{
    printf("===== MetaRepository::enumerateNameSpaces()\n");

    Array<CIMNamespaceName> nameSpaces;

    for (size_t i = 0; i < _nameSpacesSize; i++)
        nameSpaces.append(_nameSpaces[i]->name);

    return Array<CIMNamespaceName>();
}

void MetaRepository::deleteNameSpace(
    const CIMNamespaceName& nameSpace)
{
    printf("===== MetaRepository::deleteNameSpace()\n");

    _throw(CIM_ERR_NOT_SUPPORTED, "deleteNameSpace()");
}

Boolean MetaRepository::getNameSpaceAttributes(
    const CIMNamespaceName& nameSpace,
    NameSpaceAttributes& attributes)
{
    printf("===== MetaRepository::getNameSpaceAttributes()\n");

    _throw(CIM_ERR_NOT_SUPPORTED, "getNameSpaceAttributes()");

    return false;
}

Boolean MetaRepository::isRemoteNameSpace(
    const CIMNamespaceName& nameSpace,
    String& remoteInfo)
{
    printf("===== MetaRepository::isRemoteNameSpace()\n");

    return false;
}

CIMQualifierDecl MetaRepository::getQualifier(
    const CIMNamespaceName& nameSpace,
    const CIMName& qualifierName)
{
    printf("===== MetaRepository::getQualifier()\n");

    // Lookup namespace:

    const MetaNameSpace* ns = _findNameSpace(*Str(nameSpace));

    if (!ns)
        _throw(CIM_ERR_INVALID_NAMESPACE, "%s", *Str(nameSpace));

    // Lookup qualifier:

    const MetaQualifierDecl* mqd = _findQualifierDecl(ns, *Str(qualifierName));
    
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
    printf("===== MetaRepository::setQualifier()\n");

    _throw(CIM_ERR_NOT_SUPPORTED, "setQualifier()");

}

void MetaRepository::deleteQualifier(
    const CIMNamespaceName& nameSpace,
    const CIMName& qualifierName)
{
    printf("===== MetaRepository::deleteQualifier()\n");

    _throw(CIM_ERR_NOT_SUPPORTED, "deleteQualifier()");
}

Array<CIMQualifierDecl> MetaRepository::enumerateQualifiers(
    const CIMNamespaceName& nameSpace)
{
    printf("===== MetaRepository::enumerateQualifiers()\n");

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

PEGASUS_NAMESPACE_END
