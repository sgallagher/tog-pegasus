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
#include "SourceRepository.h"
#include <Pegasus/Common/System.h>

PEGASUS_NAMESPACE_BEGIN

static const size_t _MAX_NAMESPACES = 32;
static const SourceNameSpace* _nameSpaces[_MAX_NAMESPACES];
static size_t _nameSpacesSize = 0;

//==============================================================================
//
// Local definitions:
//
//==============================================================================

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

PEGASUS_FORMAT(2, 3)
static void _throw(CIMStatusCode code, const char* format, ...)
{
    char buffer[4096];

    va_list ap;
    va_start(ap, format);
    vsprintf(buffer, format, ap);
    va_end(ap);
    throw CIMException(code, format);
}

static const SourceNameSpace* _findNameSpace(const char* name)
{
    for (size_t i = 0; i < _nameSpacesSize; i++)
    {
        if (System::strcasecmp(_nameSpaces[i]->name, name))
            return _nameSpaces[i];
    }

    // Not found!
    return 0;
}

static bool _isSubClass(const SourceClass* super, const SourceClass* sub)
{
    if (!super)
        return true;

    for (SourceClass* p = sub->super; p; p = p->super)
    {
        if (p == super)
            return true;
    }

    return false;
}

static inline bool _isDirectSubClass(
    const SourceClass* super, 
    const SourceClass* sub)
{
    return sub->super == super;
}

static const SourceClass* _findClass(
    const SourceNameSpace* ns, 
    const char* name)
{
    for (size_t i = 0; ns->classes[i]; i++)
    {
        const SourceClass* sc = ns->classes[i];

        if (System::strcasecmp(sc->name, name) == 0)
            return sc;
    }

    // Not found!
    return 0;
}

static const CIMValue _TRUE(Boolean(true));
static const CIMValue _FALSE(Boolean(false));

static inline void _readBoolean(const char*& value, Boolean& x)
{
    unsigned const char* p = (unsigned const char*)value;
    x = Boolean(p[0]);
    value++;
}

static inline void _readUint8(const char*& value, Uint8& x)
{
    unsigned const char* p = (unsigned const char*)value;
    x = Uint8(p[0]);
    value += sizeof(x);
}

static inline void _readSint8(const char*& value, Sint8& x)
{
    _readUint8(value, *((Uint8*)&x));
}

static inline void _readUint16(const char*& value, Uint16& x)
{
    unsigned const char* p = (unsigned const char*)value;
    Uint16 x0 = Uint16(p[0]) << 8;
    Uint16 x1 = Uint16(p[1]) << 0;
    x = Uint16(x0 | x1);
    value += sizeof(x);
}

static inline void _readSint16(const char*& value, Sint16& x)
{
    _readUint16(value, *((Uint16*)&x));
    value += sizeof(x);
}

static inline void _readUint32(const char*& value, Uint32& x)
{
    unsigned const char* p = (unsigned const char*)value;
    Uint32 x0 = Uint32(p[0]) << 24;
    Uint32 x1 = Uint32(p[1]) << 16;
    Uint32 x2 = Uint32(p[0]) <<  8;
    Uint32 x3 = Uint32(p[1]) <<  0;
    x = Uint32(x0 | x1 | x2 | x3);
    value += sizeof(x);
}

static inline void _readSint32(const char*& value, Sint32& x)
{
    _readUint32(value, *((Uint32*)&x));
    value += sizeof(x);
}

static inline void _readUint64(const char*& value, Uint64& x)
{
    unsigned const char* p = (unsigned const char*)value;
    Uint64 x0 = Uint64(p[0]) << 56;
    Uint64 x1 = Uint64(p[1]) << 48;
    Uint64 x2 = Uint64(p[2]) << 40;
    Uint64 x3 = Uint64(p[3]) << 32;
    Uint64 x4 = Uint64(p[4]) << 24;
    Uint64 x5 = Uint64(p[5]) << 16;
    Uint64 x6 = Uint64(p[6]) <<  8;
    Uint64 x7 = Uint64(p[7]) <<  0;
    x = Uint64(x0 | x1 | x2 | x3 | x4 | x5 | x6 | x7);
    value += sizeof(x);
}

static inline void _readSint64(const char*& value, Sint64& x)
{
    _readUint64(value, *((Uint64*)&x));
    value += sizeof(x);
}

static inline void _readReal32(const char*& value, Real32& x)
{
    _readUint32(value, *((Uint32*)&x));
    value += sizeof(x);
}

static inline void _readReal64(const char*& value, Real64& x)
{
    _readUint64(value, *((Uint64*)&x));
    value += sizeof(x);
}

static inline void _readChar16(const char*& value, Char16& x)
{
    _readUint16(value, *((Uint16*)&x));
    value += sizeof(x);
}

static inline void _readString(const char*& value, String& x)
{
    size_t n = strlen(value);
    x.assign(value, n);
    value += n + 1;
}

static inline void _readDateTime(const char*& value, CIMDateTime& x)
{
    size_t n = strlen(value);
    x.set(value);
    value += n + 1;
}

static int _makeValue(
    CIMValue& cv, 
    Uint16 type, 
    Sint16 subscript, 
    const char* value)
{
    // If null value:

    if (value == 0)
    {
        if (subscript == -1)
            cv.setNullValue(CIMType(type), false);
        else
            cv.setNullValue(CIMType(type), true, subscript);

        return 0;
    }

    // If scalar, else array:

    if (subscript == -1)
    {
        switch (CIMType(type))
        {
            case CIMTYPE_BOOLEAN:
            {
                Boolean x;
                _readBoolean(value, x);
                cv.set(x);
                return 0;
            }
            case CIMTYPE_UINT8:
            {
                Uint8 x;
                _readUint8(value, x);
                cv.set(x);
                return 0;
            }
            case CIMTYPE_SINT8:
            {
                Sint8 x;
                _readSint8(value, x);
                cv.set(x);
                return 0;
            }
            case CIMTYPE_UINT16:
            {
                Uint16 x;
                _readUint16(value, x);
                cv.set(x);
                return 0;
            }
            case CIMTYPE_SINT16:
            {
                Sint16 x;
                _readSint16(value, x);
                cv.set(x);
                return 0;
            }
            case CIMTYPE_UINT32:
            {
                Uint32 x;
                _readUint32(value, x);
                cv.set(x);
                return 0;
            }
            case CIMTYPE_SINT32:
            {
                Sint32 x;
                _readSint32(value, x);
                cv.set(x);
                return 0;
            }
            case CIMTYPE_UINT64:
            {
                Uint64 x;
                _readUint64(value, x);
                cv.set(x);
                return 0;
            }
            case CIMTYPE_SINT64:
            {
                Sint64 x;
                _readSint64(value, x);
                cv.set(x);
                return 0;
            }
            case CIMTYPE_REAL32:
            {
                Real32 x;
                _readReal32(value, x);
                cv.set(x);
                return 0;
            }
            case CIMTYPE_REAL64:
            {
                Real64 x;
                _readReal64(value, x);
                cv.set(x);
                return 0;
            }
            case CIMTYPE_CHAR16:
            {
                Char16 x;
                _readChar16(value, x);
                cv.set(x);
                return 0;
            }
            case CIMTYPE_STRING:
            {
                String x;
                _readString(value, x);
                cv.set(x);
                return 0;
            }
            case CIMTYPE_DATETIME:
            {
                CIMDateTime x;
                _readDateTime(value, x);
                cv.set(x);
                return 0;
            }

            default:
                return -1;
        }
    }
    else
    {
        // Read array size:

        Uint16 size;
        _readUint16(value, size);

        // Read array elements:

        switch (CIMType(type))
        {
            case CIMTYPE_BOOLEAN:
            {
                Array<Boolean> a;

                for (Uint16 i = 0; i < size; i++)
                {
                    Boolean x;
                    _readBoolean(value, x);
                    a.append(x);
                }

                cv.set(a);
                return 0;
            }
            case CIMTYPE_UINT8:
            {
                Array<Uint8> a;

                for (Uint16 i = 0; i < size; i++)
                {
                    Uint8 x;
                    _readUint8(value, x);
                    a.append(x);
                }

                cv.set(a);
                return 0;
            }
            case CIMTYPE_SINT8:
            {
                Array<Sint8> a;

                for (Uint16 i = 0; i < size; i++)
                {
                    Sint8 x;
                    _readSint8(value, x);
                    a.append(x);
                }

                cv.set(a);
                return 0;
            }
            case CIMTYPE_UINT16:
            {
                Array<Uint16> a;

                for (Uint16 i = 0; i < size; i++)
                {
                    Uint16 x;
                    _readUint16(value, x);
                    a.append(x);
                }

                cv.set(a);
                return 0;
            }
            case CIMTYPE_SINT16:
            {
                Array<Sint16> a;

                for (Uint16 i = 0; i < size; i++)
                {
                    Sint16 x;
                    _readSint16(value, x);
                    a.append(x);
                }

                cv.set(a);
                return 0;
            }
            case CIMTYPE_UINT32:
            {
                Array<Uint32> a;

                for (Uint16 i = 0; i < size; i++)
                {
                    Uint32 x;
                    _readUint32(value, x);
                    a.append(x);
                }

                cv.set(a);
                return 0;
            }
            case CIMTYPE_SINT32:
            {
                Array<Sint32> a;

                for (Uint16 i = 0; i < size; i++)
                {
                    Sint32 x;
                    _readSint32(value, x);
                    a.append(x);
                }

                cv.set(a);
                return 0;
            }
            case CIMTYPE_UINT64:
            {
                Array<Uint64> a;

                for (Uint16 i = 0; i < size; i++)
                {
                    Uint64 x;
                    _readUint64(value, x);
                    a.append(x);
                }

                cv.set(a);
                return 0;
            }
            case CIMTYPE_SINT64:
            {
                Array<Sint64> a;

                for (Uint16 i = 0; i < size; i++)
                {
                    Sint64 x;
                    _readSint64(value, x);
                    a.append(x);
                }

                cv.set(a);
                return 0;
            }
            case CIMTYPE_REAL32:
            {
                Array<Real32> a;

                for (Uint16 i = 0; i < size; i++)
                {
                    Real32 x;
                    _readReal32(value, x);
                    a.append(x);
                }

                cv.set(a);
                return 0;
            }
            case CIMTYPE_REAL64:
            {
                Array<Real64> a;

                for (Uint16 i = 0; i < size; i++)
                {
                    Real64 x;
                    _readReal64(value, x);
                    a.append(x);
                }

                cv.set(a);
                return 0;
            }
            case CIMTYPE_CHAR16:
            {
                Array<Char16> a;

                for (Uint16 i = 0; i < size; i++)
                {
                    Char16 x;
                    _readChar16(value, x);
                    a.append(x);
                }

                cv.set(a);
                return 0;
            }
            case CIMTYPE_STRING:
            {
                Array<String> a;

                for (Uint16 i = 0; i < size; i++)
                {
                    String x;
                    _readString(value, x);
                    a.append(x);
                }

                cv.set(a);
                return 0;
            }
            case CIMTYPE_DATETIME:
            {
                Array<CIMDateTime> a;

                for (Uint16 i = 0; i < size; i++)
                {
                    CIMDateTime x;
                    _readDateTime(value, x);
                    a.append(x);
                }

                cv.set(a);
                return 0;
            }

            default:
                return -1;
        }
    }

    // Unreachable!
    return -1;
}

template<class C>
static void _addQualifiers(
    const SourceNameSpace* ns,
    C& c, 
    Uint32 flags,
    const char** qualifiers)
{
    // ATTN: handle qualifier propagation:

    // Add the boolean qualifiers:

    if (flags & PEGASUS_FLAG_ABSTRACT)
        c.addQualifier(CIMQualifier("Abstract", _TRUE));
    if (flags & PEGASUS_FLAG_AGGREGATE)
        c.addQualifier(CIMQualifier("Aggregate", _TRUE));
    if (flags & PEGASUS_FLAG_AGGREGATION)
        c.addQualifier(CIMQualifier("Aggregation", _TRUE));
    if (flags & PEGASUS_FLAG_ASSOCIATION)
        c.addQualifier(CIMQualifier("Association", _TRUE));
    if (flags & PEGASUS_FLAG_COMPOSITION)
        c.addQualifier(CIMQualifier("Composition", _TRUE));
    if (flags & PEGASUS_FLAG_COUNTER)
        c.addQualifier(CIMQualifier("Counter", _TRUE));
    if (flags & PEGASUS_FLAG_DELETE)
        c.addQualifier(CIMQualifier("Delete", _TRUE));
    if (flags & PEGASUS_FLAG_DN)
        c.addQualifier(CIMQualifier("DN", _TRUE));
    if (flags & PEGASUS_FLAG_EMBEDDEDOBJECT)
        c.addQualifier(CIMQualifier("EmbeddedObject", _TRUE));
    if (flags & PEGASUS_FLAG_EXCEPTION)
        c.addQualifier(CIMQualifier("Exception", _TRUE));
    if (flags & PEGASUS_FLAG_EXPENSIVE)
        c.addQualifier(CIMQualifier("Expensive", _TRUE));
    if (flags & PEGASUS_FLAG_EXPERIMENTAL)
        c.addQualifier(CIMQualifier("Experimental", _TRUE));
    if (flags & PEGASUS_FLAG_GAUGE)
        c.addQualifier(CIMQualifier("Gauge", _TRUE));
    if (flags & PEGASUS_FLAG_IFDELETED)
        c.addQualifier(CIMQualifier("IfDeleted", _TRUE));
    if (flags & PEGASUS_FLAG_IN)
        c.addQualifier(CIMQualifier("In", _TRUE));
    if (flags & PEGASUS_FLAG_INDICATION)
        c.addQualifier(CIMQualifier("Indication", _TRUE));
    if (flags & PEGASUS_FLAG_INVISIBLE)
        c.addQualifier(CIMQualifier("Invisible", _TRUE));
    if (flags & PEGASUS_FLAG_KEY)
        c.addQualifier(CIMQualifier("Key", _TRUE));
    if (flags & PEGASUS_FLAG_LARGE)
        c.addQualifier(CIMQualifier("Large", _TRUE));
    if (flags & PEGASUS_FLAG_OCTETSTRING)
        c.addQualifier(CIMQualifier("OctetString", _TRUE));
    if (flags & PEGASUS_FLAG_OUT)
        c.addQualifier(CIMQualifier("Out", _TRUE));
    if (flags & PEGASUS_FLAG_READ)
        c.addQualifier(CIMQualifier("Read", _TRUE));
    if (flags & PEGASUS_FLAG_REQUIRED)
        c.addQualifier(CIMQualifier("Required", _TRUE));
    if (flags & PEGASUS_FLAG_STATIC)
        c.addQualifier(CIMQualifier("Static", _TRUE));
    if (flags & PEGASUS_FLAG_TERMINAL)
        c.addQualifier(CIMQualifier("Terminal", _TRUE));
    if (flags & PEGASUS_FLAG_WEAK)
        c.addQualifier(CIMQualifier("Weak", _TRUE));
    if (flags & PEGASUS_FLAG_WRITE)
        c.addQualifier(CIMQualifier("Write", _TRUE));
    if (flags & PEGASUS_FLAG_EMBEDDEDINSTANCE)
        c.addQualifier(CIMQualifier("EmbeddedInstance", _TRUE));

    // Add non-boolean qualifiers:

    for (const char** p = qualifiers; *p; p++)
    {
        const char* q = *p;

        // Read qualifier id:

        Uint8 qid;
        _readUint8(q, qid);

        // Get qualifier declaration:

        SourceQualifierDecl* qd = ns->qualifiers[qid];

        // Make CIMValue:

        CIMValue cv;
        int status = _makeValue(cv, qd->type, qd->subscript, q);
        assert(status == 0);

        // Add qualifier:

        c.addQualifier(CIMQualifier(qd->name, cv));
    }
}

static void _addProperty(
    const SourceNameSpace* ns,
    CIMClass& cc, 
    const SourceProperty* sp)
{
    // Make CIMvalue:

    CIMValue cv;
    int status = _makeValue(cv, sp->type, sp->subscript, sp->value);
    assert(status == 0);

    // Create property:

    CIMProperty cp(sp->name, cv);

    // Add qualifiers:

    _addQualifiers(ns, cp, sp->flags, sp->qualifiers);

    // Add to class:

    cc.addProperty(cp);
}

static void _addReference(
    const SourceNameSpace* ns,
    CIMClass& cc, 
    const SourceReference* sr)
{
    // Create reference property:

    CIMValue cv;
    
    if (sr->subscript == -1)
        cv.setNullValue(CIMTYPE_REFERENCE, false, 0);
    else
        cv.setNullValue(CIMTYPE_REFERENCE, true, sr->subscript);

    CIMProperty cp(sr->name, cv);

    // Add qualifiers:

    _addQualifiers(ns, cp, sr->flags, sr->qualifiers);

    // Add to class:

    cc.addProperty(cp);
}

static void _addPropertyParameter(
    const SourceNameSpace* ns,
    CIMMethod& cm, 
    const SourceProperty* sp)
{
    // Create property:

    bool isArray;
    Uint32 arraySize;

    if (sp->subscript == -1)
    {
        isArray = false;
        arraySize = 0;
    }
    else 
    {
        isArray = true;
        arraySize = Uint32(sp->subscript);
    }

    CIMParameter cp(sp->name, CIMType(sp->type), isArray, arraySize);

    // Add qualifiers:

    _addQualifiers(ns, cp, sp->flags, sp->qualifiers);

    // Add to method:

    cm.addParameter(cp);
}

static void _addReferenceParameter(
    const SourceNameSpace* ns,
    CIMMethod& cm, 
    const SourceReference* sp)
{
    // Create property:

    bool isArray;
    Uint32 arraySize;

    if (sp->subscript == -1)
    {
        isArray = false;
        arraySize = 0;
    }
    else 
    {
        isArray = true;
        arraySize = Uint32(sp->subscript);
    }

    assert(sp->ref != 0);
    CIMName rcn = sp->ref->name;

    CIMParameter cp(sp->name, CIMTYPE_REFERENCE, isArray, arraySize, rcn);

    // Add qualifiers:

    _addQualifiers(ns, cp, sp->flags, sp->qualifiers);

    // Add to method:

    cm.addParameter(cp);
}

static void _addMethod(
    const SourceNameSpace* ns,
    CIMClass& cc, 
    const SourceMethod* sm)
{
    // Create method:

    CIMMethod cm(sm->name, CIMType(sm->type));

    // Add parameters:

    for (SourceFeature** p = sm->parameters; *p; p++)
    {
        SourceFeature* sf = *p;

        if (sf->flags & PEGASUS_FLAG_PROPERTY)
        {
            SourceProperty* sp = (SourceProperty*)sf;
            _addPropertyParameter(ns, cm, sp);
        }
        else if (sf->flags & PEGASUS_FLAG_REFERENCE)
        {
            SourceReference* sr = (SourceReference*)sf;
            _addReferenceParameter(ns, cm, sr);
        }
    }

    // Add qualifiers:

    _addQualifiers(ns, cm, sm->flags, sm->qualifiers);

    // Add to class:

    cc.addMethod(cm);
}

static void _addFeatures(
    const SourceNameSpace* ns,
    CIMClass& cc, 
    const SourceClass* sc)
{
    // Add superclass properties first:

    // ATTN: handle feature overriding.
    // ATTN: set the feature propagated flag.
    // ATTN: set the class origin.

    if (sc->super)
        _addFeatures(ns, cc, sc->super);

    // For each feature:

    for (SourceFeature** p = sc->features; *p; p++)
    {
        SourceFeature* sf = *p;

        if (sf->flags & PEGASUS_FLAG_PROPERTY)
        {
            SourceProperty* sp = (SourceProperty*)sf;
            _addProperty(ns, cc, sp);
        }
        else if (sf->flags & PEGASUS_FLAG_REFERENCE)
        {
            SourceReference* sr = (SourceReference*)sf;
            _addReference(ns, cc, sr);
        }
        else if (sf->flags & PEGASUS_FLAG_METHOD)
        {
            SourceMethod* sm = (SourceMethod*)sf;
            _addMethod(ns, cc, sm);
        }
    }
}

static bool _makeCIMClass(
    const SourceNameSpace* ns,
    CIMClass& cc, 
    const SourceClass* sc)
{
    try
    {
        // Create class:

        CIMName scn;

        if (sc->super)
            scn = sc->super->name;

        cc = CIMClass(sc->name, scn);

        // Add qualifiers:

        _addQualifiers(ns, cc, sc->flags, sc->qualifiers);

        // Features:

        _addFeatures(ns, cc, sc);
    }
    catch (...)
    {
        return false;
    }

    return true;
}

//==============================================================================
//
// class SourceRepository
//
//==============================================================================

SourceRepository::SourceRepository()
{
}

SourceRepository::~SourceRepository()
{
}

bool SourceRepository::addNameSpace(const SourceNameSpace* nameSpace)
{
    if (_nameSpacesSize == _MAX_NAMESPACES || !nameSpace)
        return false;

    for (size_t i = 0; i < _nameSpacesSize; i++)
    {
        if (System::strcasecmp(_nameSpaces[i]->name, nameSpace->name) == 0)
            return false;
    }

    _nameSpaces[_nameSpacesSize++] = nameSpace;
    return true;
}

CIMClass SourceRepository::getClass(
    const CIMNamespaceName& nameSpace,
    const CIMName& className,
    Boolean localOnly,
    Boolean includeQualifiers,
    Boolean includeClassOrigin,
    const CIMPropertyList& propertyList)
{
    return CIMClass();
}

Array<CIMClass> enumerateClasses(
    const CIMNamespaceName& nameSpace,
    const CIMName& className,
    Boolean deepInheritance,
    Boolean localOnly,
    Boolean includeQualifiers,
    Boolean includeClassOrigin)
{
    return Array<CIMClass>();
}

Array<CIMName> enumerateClassNames(
    const CIMNamespaceName& nameSpace,
    const CIMName& className,
    Boolean deepInheritance)
{
    // Lookup namespace:

    const SourceNameSpace* ns = _findNameSpace(*Str(nameSpace));

    if (!ns)
        _throw(CIM_ERR_INVALID_NAMESPACE, "%s", *Str(nameSpace));

    // Lookup class:

    const SourceClass* super = 0;
    
    if (!nameSpace.isNull())
    {
        super = _findClass(ns, *Str(className));

        if (!super)
            _throw(CIM_ERR_NOT_FOUND, "unknown class: %s", *Str(className));
    }

    // Iterate all classes looking for matches:

    Array<CIMName> result;

    for (size_t i = 0; ns->classes[i]; i++)
    {
        SourceClass* sc = ns->classes[i];

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

void SourceRepository::deleteClass(
    const CIMNamespaceName& nameSpace,
    const CIMName& className)
{
    _throw(CIM_ERR_NOT_SUPPORTED, "deleteClass()");
}

void SourceRepository::createClass(
    const CIMNamespaceName& nameSpace,
    const CIMClass& newClass)
{
    _throw(CIM_ERR_NOT_SUPPORTED, "createClass()");
}

void SourceRepository::modifyClass(
    const CIMNamespaceName& nameSpace,
    const CIMClass& newClass)
{
    _throw(CIM_ERR_NOT_SUPPORTED, "modifyClass()");
}

PEGASUS_NAMESPACE_END
