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

#ifndef Pegasus_SchemaTypes_h
#define Pegasus_SchemaTypes_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/CIMType.h>
#include "Linkage.h"

#define SCHEMA_FLAG_PROPERTY           (1 << 0)
#define SCHEMA_FLAG_REFERENCE          (1 << 1)
#define SCHEMA_FLAG_METHOD             (1 << 2)
#define SCHEMA_FLAG_CLASS              (1 << 3)
#define SCHEMA_FLAG_ABSTRACT           (1 << 4)
#define SCHEMA_FLAG_AGGREGATE          (1 << 5)
#define SCHEMA_FLAG_AGGREGATION        (1 << 6)
#define SCHEMA_FLAG_ASSOCIATION        (1 << 7)
#define SCHEMA_FLAG_COMPOSITION        (1 << 8)
#define SCHEMA_FLAG_COUNTER            (1 << 9)
#define SCHEMA_FLAG_DELETE             (1 << 10)
#define SCHEMA_FLAG_DN                 (1 << 11)
#define SCHEMA_FLAG_EMBEDDEDOBJECT     (1 << 12)
#define SCHEMA_FLAG_EXCEPTION          (1 << 13)
#define SCHEMA_FLAG_EXPENSIVE          (1 << 14)
#define SCHEMA_FLAG_EXPERIMENTAL       (1 << 15)
#define SCHEMA_FLAG_GAUGE              (1 << 16)
#define SCHEMA_FLAG_IFDELETED          (1 << 17)
#define SCHEMA_FLAG_IN                 (1 << 18)
#define SCHEMA_FLAG_INDICATION         (1 << 19)
#define SCHEMA_FLAG_INVISIBLE          (1 << 20)
#define SCHEMA_FLAG_KEY                (1 << 21)
#define SCHEMA_FLAG_LARGE              (1 << 22)
#define SCHEMA_FLAG_OCTETSTRING        (1 << 23)
#define SCHEMA_FLAG_OUT                (1 << 24)
#define SCHEMA_FLAG_READ               (1 << 25)
#define SCHEMA_FLAG_REQUIRED           (1 << 26)
#define SCHEMA_FLAG_STATIC             (1 << 27)
#define SCHEMA_FLAG_TERMINAL           (1 << 28)
#define SCHEMA_FLAG_WEAK               (1 << 29)
#define SCHEMA_FLAG_WRITE              (1 << 30)
#define SCHEMA_FLAG_EMBEDDEDINSTANCE   (1 << 31)

#define SCHEMA_SCOPE_SCHEMA            (1 << 0)
#define SCHEMA_SCOPE_CLASS             (1 << 1)
#define SCHEMA_SCOPE_ASSOCIATION       (1 << 2)
#define SCHEMA_SCOPE_INDICATION        (1 << 3)
#define SCHEMA_SCOPE_PROPERTY          (1 << 4)
#define SCHEMA_SCOPE_REFERENCE         (1 << 5)
#define SCHEMA_SCOPE_METHOD            (1 << 6)
#define SCHEMA_SCOPE_PARAMETER         (1 << 7)
#define SCHEMA_SCOPE_ANY               (1|2|4|8|16|32|64|128)

#define SCHEMA_FLAVOR_OVERRIDABLE      (1 << 0)
#define SCHEMA_FLAVOR_TOSUBCLASS       (1 << 1)
#define SCHEMA_FLAVOR_TOINSTANCE       (1 << 2)
#define SCHEMA_FLAVOR_TRANSLATABLE     (1 << 3)
#define SCHEMA_FLAVOR_DISABLEOVERRIDE  (1 << 4)
#define SCHEMA_FLAVOR_RESTRICTED       (1 << 5)

#define SCHEMA_MAX_FEATURES 256

PEGASUS_NAMESPACE_BEGIN

struct SchemaFeature
{
    Uint32 flags;
    char* name;
    const char** qualifiers;
};

struct SchemaProperty /* extends SchemaFeature */
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

struct SchemaReference /* extends SchemaFeature */
{
    // Inherited fields:
    Uint32 flags;
    char* name;
    const char** qualifiers;

    // Local fields:
    Sint16 subscript;
    struct SchemaClass* ref;
};

struct SchemaMethod /* extends SchemaFeature */
{
    // Inherited fields:
    Uint32 flags;
    char* name;
    const char** qualifiers;

    // Local fields:
    Uint16 type;
    SchemaFeature** parameters;
};

struct SchemaClass
{
    Uint32 flags;
    char* name;
    const char** qualifiers;
    struct SchemaClass* super;
    SchemaFeature** features;
};

struct SchemaQualifierDecl
{
    char* name;
    Uint16 type;
    Sint16 subscript;
    Uint16 scope;
    Uint16 flavor;
    const char* value;
};

struct SchemaNameSpace
{
    char* name;
    SchemaQualifierDecl** qualifiers;
    SchemaClass** classes;
};

const SchemaQualifierDecl* FindQualifierDecl(
    const SchemaNameSpace* ns, 
    const char* name);

const SchemaClass* FindClass(const SchemaNameSpace* ns, const char* name);

bool IsSubClass(const SchemaClass* super, const SchemaClass* sub);

inline bool IsA(const SchemaClass* super, const SchemaClass* sub)
{
    return sub == super || IsSubClass(super, sub);
}

struct SchemaFeatureInfo
{
    const SchemaFeature* mf;
    const SchemaClass* mc;
};

int MergeFeatures(
    const SchemaClass* mc,
    bool localOnly,
    Uint32 flags,
    SchemaFeatureInfo features[SCHEMA_MAX_FEATURES],
    size_t& numFeatures);

int MakeClass(
    const char* hostName,
    const SchemaNameSpace* ns,
    const SchemaClass* mc,
    Boolean localOnly,
    Boolean includeQualifiers,
    Boolean includeClassOrigin,
    const char* const* propertyList,
    class CIMClass& cc);

int MakeQualifierDecl(
    const SchemaNameSpace* ns,
    const SchemaQualifierDecl* mqd,
    class CIMQualifierDecl& cqd);

const SchemaFeature* FindFeature(
    const SchemaClass* mc, 
    const char* name,
    Uint32 flags = 
        (SCHEMA_FLAG_PROPERTY|SCHEMA_FLAG_REFERENCE|SCHEMA_FLAG_METHOD));

PEGASUS_NAMESPACE_END

#endif /* Pegasus_SchemaTypes_h */
