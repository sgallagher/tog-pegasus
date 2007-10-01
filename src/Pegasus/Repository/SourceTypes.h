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

#ifndef Pegasus_SourceTypes_h
#define Pegasus_SourceTypes_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/CIMType.h>
#include "Linkage.h"

#define PEGASUS_FLAG_PROPERTY           (1 << 0)
#define PEGASUS_FLAG_REFERENCE          (1 << 1)
#define PEGASUS_FLAG_METHOD             (1 << 2)
#define PEGASUS_FLAG_CLASS              (1 << 3)
#define PEGASUS_FLAG_ABSTRACT           (1 << 4)
#define PEGASUS_FLAG_AGGREGATE          (1 << 5)
#define PEGASUS_FLAG_AGGREGATION        (1 << 6)
#define PEGASUS_FLAG_ASSOCIATION        (1 << 7)
#define PEGASUS_FLAG_COMPOSITION        (1 << 8)
#define PEGASUS_FLAG_COUNTER            (1 << 9)
#define PEGASUS_FLAG_DELETE             (1 << 10)
#define PEGASUS_FLAG_DN                 (1 << 11)
#define PEGASUS_FLAG_EMBEDDEDOBJECT     (1 << 12)
#define PEGASUS_FLAG_EXCEPTION          (1 << 13)
#define PEGASUS_FLAG_EXPENSIVE          (1 << 14)
#define PEGASUS_FLAG_EXPERIMENTAL       (1 << 15)
#define PEGASUS_FLAG_GAUGE              (1 << 16)
#define PEGASUS_FLAG_IFDELETED          (1 << 17)
#define PEGASUS_FLAG_IN                 (1 << 18)
#define PEGASUS_FLAG_INDICATION         (1 << 19)
#define PEGASUS_FLAG_INVISIBLE          (1 << 20)
#define PEGASUS_FLAG_KEY                (1 << 21)
#define PEGASUS_FLAG_LARGE              (1 << 22)
#define PEGASUS_FLAG_OCTETSTRING        (1 << 23)
#define PEGASUS_FLAG_OUT                (1 << 24)
#define PEGASUS_FLAG_READ               (1 << 25)
#define PEGASUS_FLAG_REQUIRED           (1 << 26)
#define PEGASUS_FLAG_STATIC             (1 << 27)
#define PEGASUS_FLAG_TERMINAL           (1 << 28)
#define PEGASUS_FLAG_WEAK               (1 << 29)
#define PEGASUS_FLAG_WRITE              (1 << 30)
#define PEGASUS_FLAG_EMBEDDEDINSTANCE   (1 << 31)

#define PEGASUS_SCOPE_SCHEMA            (1 << 0)
#define PEGASUS_SCOPE_CLASS             (1 << 1)
#define PEGASUS_SCOPE_ASSOCIATION       (1 << 2)
#define PEGASUS_SCOPE_INDICATION        (1 << 3)
#define PEGASUS_SCOPE_PROPERTY          (1 << 4)
#define PEGASUS_SCOPE_REFERENCE         (1 << 5)
#define PEGASUS_SCOPE_METHOD            (1 << 6)
#define PEGASUS_SCOPE_PARAMETER         (1 << 7)
#define PEGASUS_SCOPE_ANY               (1|2|4|8|16|32|64|128)

#define PEGASUS_FLAVOR_OVERRIDABLE      (1 << 0)
#define PEGASUS_FLAVOR_TOSUBCLASS       (1 << 1)
#define PEGASUS_FLAVOR_TOINSTANCE       (1 << 2)
#define PEGASUS_FLAVOR_TRANSLATABLE     (1 << 3)
#define PEGASUS_FLAVOR_DISABLEOVERRIDE  (1 << 4)
#define PEGASUS_FLAVOR_RESTRICTED       (1 << 5)

PEGASUS_NAMESPACE_BEGIN

struct SourceFeature
{
    Uint32 flags;
    char* name;
    const char** qualifiers;
};

struct SourceProperty /* extends SourceFeature */
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

struct SourceReference /* extends SourceFeature */
{
    // Inherited fields:
    Uint32 flags;
    char* name;
    const char** qualifiers;

    // Local fields:
    Sint16 subscript;
    struct SourceClass* ref;
};

struct SourceMethod /* extends SourceFeature */
{
    // Inherited fields:
    Uint32 flags;
    char* name;
    const char** qualifiers;

    // Local fields:
    Uint16 type;
    SourceFeature** parameters;
};

struct SourceClass
{
    Uint32 flags;
    char* name;
    const char** qualifiers;
    struct SourceClass* super;
    SourceFeature** features;
};

struct SourceQualifierDecl
{
    char* name;
    Uint16 type;
    Sint16 subscript;
    Uint16 scope;
    Uint16 flavor;
    const char* value;
};

struct SourceNameSpace
{
    char* name;
    SourceQualifierDecl** qualifiers;
    SourceClass** classes;
};

PEGASUS_NAMESPACE_END

#endif /* Pegasus_SourceTypes_h */
