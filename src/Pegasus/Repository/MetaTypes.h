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

#ifndef Pegasus_MetaTypes_h
#define Pegasus_MetaTypes_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/CIMType.h>
#include "Linkage.h"

#define META_FLAG_PROPERTY           (1 << 0)
#define META_FLAG_REFERENCE          (1 << 1)
#define META_FLAG_METHOD             (1 << 2)
#define META_FLAG_CLASS              (1 << 3)
#define META_FLAG_ABSTRACT           (1 << 4)
#define META_FLAG_AGGREGATE          (1 << 5)
#define META_FLAG_AGGREGATION        (1 << 6)
#define META_FLAG_ASSOCIATION        (1 << 7)
#define META_FLAG_COMPOSITION        (1 << 8)
#define META_FLAG_COUNTER            (1 << 9)
#define META_FLAG_DELETE             (1 << 10)
#define META_FLAG_DN                 (1 << 11)
#define META_FLAG_EMBEDDEDOBJECT     (1 << 12)
#define META_FLAG_EXCEPTION          (1 << 13)
#define META_FLAG_EXPENSIVE          (1 << 14)
#define META_FLAG_EXPERIMENTAL       (1 << 15)
#define META_FLAG_GAUGE              (1 << 16)
#define META_FLAG_IFDELETED          (1 << 17)
#define META_FLAG_IN                 (1 << 18)
#define META_FLAG_INDICATION         (1 << 19)
#define META_FLAG_INVISIBLE          (1 << 20)
#define META_FLAG_KEY                (1 << 21)
#define META_FLAG_LARGE              (1 << 22)
#define META_FLAG_OCTETSTRING        (1 << 23)
#define META_FLAG_OUT                (1 << 24)
#define META_FLAG_READ               (1 << 25)
#define META_FLAG_REQUIRED           (1 << 26)
#define META_FLAG_STATIC             (1 << 27)
#define META_FLAG_TERMINAL           (1 << 28)
#define META_FLAG_WEAK               (1 << 29)
#define META_FLAG_WRITE              (1 << 30)
#define META_FLAG_EMBEDDEDINSTANCE   (1 << 31)

#define META_SCOPE_SCHEMA            (1 << 0)
#define META_SCOPE_CLASS             (1 << 1)
#define META_SCOPE_ASSOCIATION       (1 << 2)
#define META_SCOPE_INDICATION        (1 << 3)
#define META_SCOPE_PROPERTY          (1 << 4)
#define META_SCOPE_REFERENCE         (1 << 5)
#define META_SCOPE_METHOD            (1 << 6)
#define META_SCOPE_PARAMETER         (1 << 7)
#define META_SCOPE_ANY               (1|2|4|8|16|32|64|128)

#define META_FLAVOR_OVERRIDABLE      (1 << 0)
#define META_FLAVOR_TOSUBCLASS       (1 << 1)
#define META_FLAVOR_TOINSTANCE       (1 << 2)
#define META_FLAVOR_TRANSLATABLE     (1 << 3)
#define META_FLAVOR_DISABLEOVERRIDE  (1 << 4)
#define META_FLAVOR_RESTRICTED       (1 << 5)

#define META_MAX_FEATURES 1024

PEGASUS_NAMESPACE_BEGIN

struct MetaFeature
{
    Uint32 flags;
    char* name;
    const char** qualifiers;
};

struct MetaProperty /* extends MetaFeature */
{
    // Inherited fields:
    Uint32 flags;
    char* name;
    const char** qualifiers;

    // Local fields:
    Uint16 type;
    Sint16 subscript;
    const char* value;
};

struct MetaReference /* extends MetaFeature */
{
    // Inherited fields:
    Uint32 flags;
    char* name;
    const char** qualifiers;

    // Local fields:
    Sint16 subscript;
    struct MetaClass* ref;
};

struct MetaMethod /* extends MetaFeature */
{
    // Inherited fields:
    Uint32 flags;
    char* name;
    const char** qualifiers;

    // Local fields:
    Uint16 type;
    MetaFeature** parameters;
};

struct MetaClass
{
    Uint32 flags;
    char* name;
    const char** qualifiers;
    struct MetaClass* super;
    MetaFeature** features;
};

struct MetaQualifierDecl
{
    char* name;
    Uint16 type;
    Sint16 subscript;
    Uint16 scope;
    Uint16 flavor;
    const char* value;
};

struct MetaNameSpace
{
    char* name;
    MetaQualifierDecl** qualifiers;
    MetaClass** classes;
};

const MetaQualifierDecl* FindQualifierDecl(
    const MetaNameSpace* ns, 
    const char* name);

const MetaClass* FindClass(const MetaNameSpace* ns, const char* name);

bool IsSubClass(const MetaClass* super, const MetaClass* sub);

inline bool IsA(const MetaClass* super, const MetaClass* sub)
{
    return sub == super || IsSubClass(super, sub);
}

struct MetaFeatureInfo
{
    const MetaFeature* mf;
    const MetaClass* mc;
};

int MergeFeatures(
    const MetaClass* mc,
    bool localOnly,
    Uint32 flags,
    MetaFeatureInfo features[META_MAX_FEATURES],
    size_t& numFeatures);

int MakeClass(
    const char* hostName,
    const MetaNameSpace* ns,
    const MetaClass* mc,
    Boolean localOnly,
    Boolean includeQualifiers,
    Boolean includeClassOrigin,
    const char* const* propertyList,
    class CIMClass& cc);

int MakeQualifierDecl(
    const MetaNameSpace* ns,
    const MetaQualifierDecl* mqd,
    class CIMQualifierDecl& cqd);

PEGASUS_NAMESPACE_END

#endif /* Pegasus_MetaTypes_h */
