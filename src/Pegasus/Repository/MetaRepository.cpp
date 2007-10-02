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

#define TEST_META_REPOSITORY

#if defined(TEST_META_REPOSITORY)
# include "root_cimv2_namespace.h"
# include "root_PG_Internal_namespace.h"
# include "root_PG_InterOp_namespace.h"
#endif 

PEGASUS_NAMESPACE_BEGIN

static const size_t _MAX_NAMESPACES = 32;
static const MetaNameSpace* _nameSpaces[_MAX_NAMESPACES];
static size_t _nameSpacesSize = 0;

static const size_t _MAX_FEATURES = 1024;
static const size_t _MAX_QUALIFIERS = 1024;

#if defined(TEST_META_REPOSITORY)
static void _init()
{
    if (_nameSpacesSize == 0)
    {
        MetaRepository::addNameSpace(&root_PG_InterOp_namespace);
        MetaRepository::addNameSpace(&root_cimv2_namespace);
        MetaRepository::addNameSpace(&root_PG_Internal_namespace);
    }
}
#endif

static bool _eqi(const char* s1, const char* s2)
{
    return System::strcasecmp(s1, s2) == 0;
}

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
    throw CIMException(code, buffer);
}

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
                printf("T[%u]\n", __LINE__);
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
                printf("T[%u]\n", __LINE__);
                return -1;
        }
    }

    // Unreachable!
    printf("T[%u]\n", __LINE__);
    return -1;
}

struct FeatureInfo
{
    const MetaFeature* mf;
    const MetaClass* mc;
};

static int _mergeFeatures(
    const MetaClass* mc,
    bool localOnly,
    FeatureInfo features[_MAX_FEATURES],
    size_t& numFeatures)
{
    if (!localOnly && mc->super)
    {
        if (_mergeFeatures(mc->super, localOnly, features, numFeatures) != 0)
        {
            printf("T[%u]\n", __LINE__);
            return -1;
        }
    }

    // Process all features of this class:

    for (size_t i = 0; mc->features[i]; i++)
    {
        const MetaFeature* mf = mc->features[i];

        // Override feature if defined by ancestor class:

        bool found = false;

        for (size_t j = 0; j < numFeatures; j++)
        {
            const MetaFeature* tmp = features[j].mf;

            if (_eqi(mf->name, tmp->name))
            {
                features[j].mf = mf;
                features[j].mc = mc;
                found = true;
                break;
            }
        }

        // Add new feature if not not defined by ancestor class:

        if (!found)
        {
            if (numFeatures == _MAX_FEATURES)
            {
                printf("T[%u]\n", __LINE__);
                return -1;
            }

            features[numFeatures].mf = mf;
            features[numFeatures].mc = mc;
            numFeatures++;
        }
    }

    return 0;
}

struct QualifierInfo
{
    const char* qualifier;
    const MetaClass* mc;
};

static const MetaFeature* _findFeature(
    const MetaClass* mc, 
    const char* name)
{
    for (size_t i = 0; mc->features[i]; i++)
    {
        const MetaFeature* mf = mc->features[i];

        if (_eqi(mf->name, name))
            return mf;
    }

    // Not found!
    return 0;
}

static const MetaFeature* _findParameter(
    const MetaMethod* mm, 
    const char* name)
{
    for (size_t i = 0; mm->parameters[i]; i++)
    {
        const MetaFeature* mf = mm->parameters[i];

        if (_eqi(mm->name, name))
            return mf;
    }

    // Not found!
    return 0;
}

static int _mergeQualifiers(
    const MetaNameSpace* ns,
    const MetaClass* mc,
    const char* featureName,
    const char* parameterName,
    bool depth,
    QualifierInfo qualifiers[_MAX_QUALIFIERS],
    size_t& numQualifiers)
{
    // Merge super-class qualifiers:

    if (mc->super)
    {
        _mergeQualifiers(ns, mc->super, featureName, parameterName, depth + 1,
            qualifiers, numQualifiers);
    }

    const char** quals = 0;

    // Find qualifiers of the given object:

    if (!featureName && !parameterName)
    {
        // Case 1: get class qualifiers:
        quals = mc->qualifiers;
    }
    else if (featureName && !parameterName)
    {
        // Case 2: get feature qualifiers:

        const MetaFeature* mf = _findFeature(mc, featureName);

        if (mf)
            quals = mf->qualifiers;
    }
    else if (featureName && parameterName)
    {
        // Case 3: get parameter qualifiers:

        const MetaFeature* mf = _findFeature(mc, featureName);

        if (mf && (mf->flags & META_FLAG_METHOD))
        {
            const MetaMethod* mm = (const MetaMethod*)mf;
            const MetaFeature* p = _findParameter(mm, parameterName);

            if (p)
                quals = p->qualifiers;
        }
    }

    // Merge quals into the qualifiers array:

    if (!quals)
        return 0;

    for (size_t i = 0; quals[i]; i++)
    {
        const char* qi = quals[i];

        // Override existing qualifier if any:

        bool found = false;

        for (size_t j = 0; j < numQualifiers; j++)
        {
            const char* qj = qualifiers[j].qualifier;

            if (qi[0] == qj[0])
            {
                qualifiers[j].qualifier = qi;
                qualifiers[j].mc = mc;
                found = true;
                break;
            }
        }

        // Inject this qualifier not found:

        if (!found)
        {
            MetaQualifierDecl* qd = ns->qualifiers[qi[0]];

            if (depth == 0 || !(qd->flavor & META_FLAVOR_RESTRICTED))
            {
                if (numQualifiers == _MAX_QUALIFIERS)
                {
                    printf("T[%u]\n", __LINE__);
                    return -1;
                }

                qualifiers[numQualifiers].qualifier = qi;
                qualifiers[numQualifiers].mc = mc;
                numQualifiers++;
            }
        }
    }

    return 0;
}

template<class C>
static int _addQualifiers(
    const MetaNameSpace* ns,
    const MetaClass* mc,
    const char* featureName,
    const char* parameterName,
    C& c)
{
    QualifierInfo qualifiers[_MAX_QUALIFIERS];
    size_t numQualifiers = 0;

    if (_mergeQualifiers(
        ns, mc, featureName, parameterName, 0, qualifiers, numQualifiers) != 0)
    {
        printf("T[%u]\n", __LINE__);
        return -1;
    }

    // Add qualifiers to container:

    for (size_t i = 0; i < numQualifiers; i++)
    {
        const char* q = qualifiers[i].qualifier;

        // Get qualifier id:

        Uint8 qid = Uint8(q[0]);

        // Get qualifier declaration:

        MetaQualifierDecl* qd = ns->qualifiers[qid];

        // Make CIMValue:

        CIMValue cv;

        if (_makeValue(cv, qd->type, qd->subscript, q + 1) != 0)
        {
            printf("T[%u]\n", __LINE__);
            return -1;
        }

        // Add qualifier:

        c.addQualifier(CIMQualifier(qd->name, cv));
    }

    return 0;
}

static int _addProperty(
    const MetaNameSpace* ns,
    const MetaClass* mc,
    const MetaProperty* mp,
    const char* classOrigin,
    bool propagated,
    bool includeQualifiers,
    bool includeClassOrigin,
    CIMClass& cc)
{
    // Make CIMvalue:

    CIMValue cv;

    if (_makeValue(cv, mp->type, mp->subscript, mp->value) != 0)
    {
        printf("T[%u]\n", __LINE__);
        return -1;
    }

    // Create property:

    CIMProperty cp(mp->name, cv);

    if (includeClassOrigin)
        cp.setClassOrigin(classOrigin);

    cp.setPropagated(propagated);

    // Add qualifiers:

    if (includeQualifiers)
    {
        if (_addQualifiers(ns, mc, mp->name, 0, cp) != 0)
        {
            printf("T[%u]\n", __LINE__);
            return -1;
        }
    }

    // Add to class:

    cc.addProperty(cp);
    return 0;
}

static int _addReference(
    const MetaNameSpace* ns,
    const MetaClass* mc,
    const MetaReference* mr,
    const char* classOrigin,
    bool propagated,
    bool includeQualifiers,
    bool includeClassOrigin,
    CIMClass& cc)
{
    // Set isArray and arraySize:

    Boolean isArray;
    Uint32 arraySize;
    
    if (mr->subscript == -1)
    {
        isArray = false;
        arraySize = 0;
    }
    else
    {
        isArray = true;
        arraySize = mr->subscript;
    }

    // Set referenceClassName:

    CIMName rcn = mr->ref->name;

    // Create value:

    CIMValue cv(CIMTYPE_REFERENCE, isArray, arraySize);

    // Create property:

    CIMProperty cp(mr->name, cv, arraySize, rcn);

    if (includeClassOrigin)
        cp.setClassOrigin(classOrigin);

    cp.setPropagated(propagated);

    // Add qualifiers:

    if (includeQualifiers)
    {
        if (_addQualifiers(ns, mc, mr->name, 0, cp) != 0)
        {
            printf("T[%u]\n", __LINE__);
            return -1;
        }
    }

    // Add to class:

    cc.addProperty(cp);
    return 0;
}

static int _addPropertyParameter(
    const MetaNameSpace* ns,
    const MetaClass* mc,
    const MetaMethod* mm,
    const MetaProperty* mp,
    bool includeQualifiers,
    CIMMethod& cm)
{
    // Create property:

    bool isArray;
    Uint32 arraySize;

    if (mp->subscript == -1)
    {
        isArray = false;
        arraySize = 0;
    }
    else 
    {
        isArray = true;
        arraySize = Uint32(mp->subscript);
    }

    CIMParameter cp(mp->name, CIMType(mp->type), isArray, arraySize);

    // Add qualifiers:

    if (includeQualifiers)
    {
        if (_addQualifiers(ns, mc, mm->name, mp->name, cm) != 0)
        {
            printf("T[%u]\n", __LINE__);
            return -1;
        }
    }

    // Add to method:

    cm.addParameter(cp);
    return 0;
}

static int _addReferenceParameter(
    const MetaNameSpace* ns,
    const MetaClass* mc,
    const MetaMethod* mm,
    const MetaReference* mr,
    bool includeQualifiers,
    CIMMethod& cm)
{
    // Create property:

    bool isArray;
    Uint32 arraySize;

    if (mr->subscript == -1)
    {
        isArray = false;
        arraySize = 0;
    }
    else 
    {
        isArray = true;
        arraySize = Uint32(mr->subscript);
    }

    CIMName rcn = mr->ref->name;
    CIMParameter cp(mr->name, CIMTYPE_REFERENCE, isArray, arraySize, rcn);

    // Add qualifiers:

    if (includeQualifiers)
    {
        if (_addQualifiers(ns, mc, mm->name, mr->name, cm) != 0)
        {
            printf("T[%u]\n", __LINE__);
            return -1;
        }
    }

    // Add to method:

    cm.addParameter(cp);
    return 0;
}

static int _addMethod(
    const MetaNameSpace* ns,
    const MetaClass* mc,
    const MetaMethod* mm,
    const char* classOrigin,
    bool propagated,
    bool includeQualifiers,
    bool includeClassOrigin,
    CIMClass& cc)
{
    // Create method:

    CIMMethod cm(mm->name, CIMType(mm->type));

    if (includeClassOrigin)
        cm.setClassOrigin(classOrigin);

    cm.setPropagated(propagated);

    // Add parameters:

    for (size_t i = 0; mm->parameters[i]; i++)
    {
        MetaFeature* mf = mm->parameters[i];

        if (mf->flags & META_FLAG_PROPERTY)
        {
            MetaProperty* mp = (MetaProperty*)mf;
            _addPropertyParameter(ns, mc, mm, mp, includeQualifiers, cm);
        }
        else if (mf->flags & META_FLAG_REFERENCE)
        {
            MetaReference* mr = (MetaReference*)mf;
            _addReferenceParameter(ns, mc, mm, mr, includeQualifiers, cm);
        }
    }

    // Add qualifiers:

    if (includeQualifiers)
    {
        if (_addQualifiers(ns, mc, mm->name, 0, cm) != 0)
        {
            printf("T[%u]\n", __LINE__);
            return -1;
        }
    }

    // Add to class:

    cc.addMethod(cm);
    return 0;
}

static bool _hasProperty(const char* const* propertyList, const char* name)
{
    for (size_t i = 0; propertyList[i]; i++)
    {
        if (_eqi(propertyList[i], name))
            return true;
    }

    return false;
}

static int _addFeatures(
    const MetaNameSpace* ns,
    const MetaClass* mc,
    bool localOnly,
    bool includeQualifiers,
    bool includeClassOrigin,
    const char* const* propertyList,
    CIMClass& cc)
{
    // Merge features from all inheritance levels into a single array:

    FeatureInfo features[_MAX_FEATURES];
    size_t numFeatures = 0;

    if (_mergeFeatures(mc, localOnly, features, numFeatures) != 0)
    {
        printf("T[%u]\n", __LINE__);
        return -1;
    }

    // For each feature:

    for (size_t i = 0; i < numFeatures; i++)
    {
        const FeatureInfo& fi = features[i];

        // Set propagated flag:

        bool propagated = fi.mc != mc;

        // Set classOrigin:

        const char* classOrigin = fi.mc->name;

        // Skip feature not in property list:

        const MetaFeature* mf = fi.mf;

        if (propertyList && !_hasProperty(propertyList, mf->name))
            continue;

        // Add the feature:

        if (mf->flags & META_FLAG_PROPERTY)
        {
            MetaProperty* mp = (MetaProperty*)mf;

            if (_addProperty(ns, mc, mp, classOrigin, propagated, 
                includeQualifiers, includeClassOrigin, cc) != 0)
            {
                printf("T[%u]\n", __LINE__);
                return -1;
            }
        }
        else if (mf->flags & META_FLAG_REFERENCE)
        {
            MetaReference* mr = (MetaReference*)mf;

            if (_addReference(ns, mc, mr, classOrigin, propagated, 
                includeQualifiers, includeClassOrigin, cc) != 0)
            {
                printf("T[%u]\n", __LINE__);
                return -1;
            }
        }
        else if (mf->flags & META_FLAG_METHOD)
        {
            MetaMethod* mm = (MetaMethod*)mf;

            if (_addMethod(ns, mc, mm, classOrigin, propagated, 
                includeQualifiers, includeClassOrigin, cc) != 0)
            {
                printf("T[%u]\n", __LINE__);
                return -1;
            }
        }
    }

    return 0;
}

static int _makeClass(
    const MetaNameSpace* ns,
    const MetaClass* mc,
    Boolean localOnly,
    Boolean includeQualifiers,
    Boolean includeClassOrigin,
    const char* const* propertyList,
    CIMClass& cc)
{
    try
    {
        // Create class:
        {
            CIMName scn;

            if (mc->super)
                scn = mc->super->name;

            cc = CIMClass(mc->name, scn);
        }

        // Add qualifiers:

        if (includeQualifiers)
        {
            if (_addQualifiers(ns, mc, 0, 0, cc) != 0)
            {
                printf("T[%u]\n", __LINE__);
                return -1;
            }
        }

        // Features:

        if (_addFeatures(ns, mc, localOnly, includeQualifiers, 
            includeClassOrigin, propertyList, cc) != 0)
        {
            printf("T[%u]\n", __LINE__);
            return -1;
        }
    }
    catch (Exception& e)
    {
        printf("EXCEPTION[%s]\n", *Str(e));
        return -1;
    }
    catch (...)
    {
        printf("T[%u]\n", __LINE__);
        return -1;
    }

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

    // ATTN-MEB: propertyList ignored!

#if defined(TEST_META_REPOSITORY)
    _init();
#endif

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

    if (_makeClass(ns, mc, localOnly, includeQualifiers, 
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

#if defined(TEST_META_REPOSITORY)
    _init();
#endif

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

            if (_makeClass(ns, mc, localOnly, includeQualifiers, 
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

#if defined(TEST_META_REPOSITORY)
    _init();
#endif

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

PEGASUS_NAMESPACE_END
