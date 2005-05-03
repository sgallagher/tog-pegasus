//%2005////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001, 2002 BMC Software; Hewlett-Packard Development
// Company, L.P.; IBM Corp.; The Open Group; Tivoli Systems.
// Copyright (c) 2003 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation, The Open Group.
// Copyright (c) 2004 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation; VERITAS Software Corporation; The Open Group.
// Copyright (c) 2005 Hewlett-Packard Development Company, L.P.; IBM Corp.;
// EMC Corporation; VERITAS Software Corporation; The Open Group.
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
// Author: Mike Brasher (mbrasher@bmc.com)
//
// Modified By: Roger Kumpf, Hewlett Packard Company (roger_kumpf@hp.com)
//              Carol Ann Krug Graves, Hewlett-Packard Company
//                  (carolann_graves@hp.com)
//              David Dillard, VERITAS Software Corp.
//                  (david.dillard@veritas.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#include <cctype>
#include "CIMName.h"
#include "CommonUTF.h"

PEGASUS_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////
//
// CIMName
//
////////////////////////////////////////////////////////////////////////////////

#define PEGASUS_ARRAY_T CIMName
# include "ArrayImpl.h"
#undef PEGASUS_ARRAY_T

CIMName::CIMName()
    : cimName(String::EMPTY)
{
}

CIMName::CIMName(const String& name)
    : cimName(name)
{
    if (!legal(name))
    {
        throw InvalidNameException(name);
    }
}

CIMName::CIMName(const char* name)
    : cimName(name)
{
    if (!legal(name))
    {
        throw InvalidNameException(name);
    }
}

CIMName& CIMName::operator=(const CIMName& name)
{
    cimName=name.cimName;
    return *this;
}

CIMName& CIMName::operator=(const String& name)
{
    if (!legal(name))
    {
        throw InvalidNameException(name);
    }
    cimName=name;
    return *this;
}

const String& CIMName::getString() const
{
    return cimName;
}

Boolean CIMName::isNull() const
{
    return (cimName.size() == 0);
}

void CIMName::clear()
{
    cimName.clear();
}

Boolean CIMName::legal(const String& name)
{
    Uint32 length = name.size();
    Uint16 chkchr;

    if (length == 0)
        return false;

    chkchr = name[0];

    // First character must be alphabetic or '_' if ASCII
    if(!(
         (chkchr == 0x005F) ||
         ((chkchr >= 0x0041) && (chkchr <= 0x005A)) ||
         ((chkchr >= 0x0061) && (chkchr <= 0x007A)) ||
         ((chkchr >= 0x0080) && (chkchr <= 0xFFEF))))
    {
        return false;
    }

    // Remaining characters must be alphanumeric or '_' if ASCII
    for(Uint32 i = 1; i < length; ++i)
    {
        chkchr = name[i];
        if(!(
             (chkchr == 0x005F) ||
             ((chkchr >= 0x0041) && (chkchr <= 0x005A)) ||
             ((chkchr >= 0x0061) && (chkchr <= 0x007A)) ||
             ((chkchr >= 0x0080) && (chkchr <= 0xFFEF)) ||
             ((chkchr >= 0x0030) && (chkchr <= 0x0039))))
        {
            return false;
        }
    }

    return true;
}

Boolean CIMName::equal(const CIMName& name) const
{
    return String::equalNoCase(cimName, name.cimName);
}

Boolean operator==(const CIMName& name1, const CIMName& name2)
{
    return name1.equal(name2);
}

Boolean operator!=(const CIMName& name1, const CIMName& name2)
{
    return !name1.equal(name2);
}


////////////////////////////////////////////////////////////////////////////////
//
// CIMNamespaceName
//
////////////////////////////////////////////////////////////////////////////////

#define PEGASUS_ARRAY_T CIMNamespaceName
# include "ArrayImpl.h"
#undef PEGASUS_ARRAY_T

CIMNamespaceName::CIMNamespaceName()
    : cimNamespaceName(String::EMPTY)
{
}

CIMNamespaceName::CIMNamespaceName(const String& name)
{
    *this = name;
}

CIMNamespaceName::CIMNamespaceName(const char* name)
{
    *this = String(name);
}

CIMNamespaceName& CIMNamespaceName::operator=(const CIMNamespaceName& name)
{
    cimNamespaceName=name.cimNamespaceName;
    return *this;
}

CIMNamespaceName& CIMNamespaceName::operator=(const String& name)
{
    if (!legal(name))
    {
        throw InvalidNamespaceNameException(name);
    }

    if (name[0] == '/')
    {
        // Strip off the meaningless leading '/'
        cimNamespaceName=name.subString(1);
    }
    else
    {
        cimNamespaceName=name;
    }

    return *this;
}

const String& CIMNamespaceName::getString() const
{
    return cimNamespaceName;
}

Boolean CIMNamespaceName::isNull() const
{
    return (cimNamespaceName.size() == 0);
}

void CIMNamespaceName::clear()
{
    cimNamespaceName.clear();
}

Boolean CIMNamespaceName::legal(const String& name)
{
    Uint32 length = name.size();
    Uint32 index = 0;

    // Skip a leading '/' because the CIM specification is ambiguous
    if (name[0] == '/')
    {
        index++;
    }

    Boolean moreElements = true;

    // Check each namespace element (delimited by '/' characters)
    while (moreElements)
    {
        moreElements = false;

        if (index == length)
        {
            return false;
        }

        Uint16 chkchr = name[index++];

        // First character must be alphabetic or '_' if ASCII
        if(!(
             (chkchr == 0x005F) ||
             ((chkchr >= 0x0041) && (chkchr <= 0x005A)) ||
             ((chkchr >= 0x0061) && (chkchr <= 0x007A)) ||
             ((chkchr >= 0x0080) && (chkchr <= 0xFFEF))))
        {
            return false;
        }

        // Remaining characters must be alphanumeric or '_' if ASCII
        while (index < length)
        {
            chkchr = name[index++];

            // A '/' indicates another namespace element follows
            if (chkchr == '/')
            {
                moreElements = true;
                break;
            }

            if(!(
                 (chkchr == 0x005F) ||
                 ((chkchr >= 0x0041) && (chkchr <= 0x005A)) ||
                 ((chkchr >= 0x0061) && (chkchr <= 0x007A)) ||
                 ((chkchr >= 0x0080) && (chkchr <= 0xFFEF)) ||
                 ((chkchr >= 0x0030) && (chkchr <= 0x0039))))
            {
                return false;
            }
        }
    }

    return true;
}

Boolean CIMNamespaceName::equal(const CIMNamespaceName& name) const
{
    return String::equalNoCase(cimNamespaceName, name.cimNamespaceName);
}

Boolean operator==(const CIMNamespaceName& name1, const CIMNamespaceName& name2)
{
    return name1.equal(name2);
}

Boolean operator!=(const CIMNamespaceName& name1, const CIMNamespaceName& name2)
{
    return !name1.equal(name2);
}

PEGASUS_NAMESPACE_END
