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

#if !defined(Pegasus_CIMNameInline_cxx)
# if !defined(PEGASUS_INTERNALONLY) || defined(PEGASUS_DISABLE_INTERNAL_INLINES)
#   define Pegasus_CIMNameInline_h
# endif
#endif

#ifndef Pegasus_CIMNameInline_h
#define Pegasus_CIMNameInline_h

#include <Pegasus/Common/CIMName.h>

#ifdef Pegasus_CIMNameInline_cxx
# define PEGASUS_CIMNAME_INLINE
#else
# define PEGASUS_CIMNAME_INLINE inline
#endif

PEGASUS_NAMESPACE_BEGIN

PEGASUS_CIMNAME_INLINE CIMName::CIMName()
{
}

PEGASUS_CIMNAME_INLINE CIMName& CIMName::operator=(const CIMName& name)
{
    cimName = name.cimName;
    return *this;
}

PEGASUS_CIMNAME_INLINE const String& CIMName::getString() const
{
    return cimName;
}

PEGASUS_CIMNAME_INLINE Boolean CIMName::isNull() const
{
    return cimName.size() == 0;
}

PEGASUS_CIMNAME_INLINE void CIMName::clear()
{
    cimName.clear();
}

PEGASUS_CIMNAME_INLINE Boolean CIMName::equal(const CIMName& name) const
{
    return String::equalNoCase(cimName, name.cimName);
}

PEGASUS_CIMNAME_INLINE Boolean CIMName::equal(const char* name) const
{
    return String::equalNoCase(cimName, name);
}

PEGASUS_CIMNAME_INLINE Boolean operator==(
    const CIMName& name1, const CIMName& name2)
{
    return name1.equal(name2);
}

PEGASUS_CIMNAME_INLINE Boolean operator==(
    const CIMName& name1, const char* name2)
{
    return name1.equal(name2);
}

PEGASUS_CIMNAME_INLINE Boolean operator==(
    const char* name1, const CIMName& name2)
{
    return name2.equal(name1);
}

PEGASUS_CIMNAME_INLINE Boolean operator!=(
    const CIMName& name1, const CIMName& name2)
{
    return !name1.equal(name2);
}

PEGASUS_CIMNAME_INLINE Boolean operator!=(
    const CIMName& name1, const char* name2)
{
    return !name1.equal(name2);
}

PEGASUS_CIMNAME_INLINE Boolean operator!=(
    const char* name1, const CIMName& name2)
{
    return !name2.equal(name1);
}

PEGASUS_CIMNAME_INLINE CIMNamespaceName::CIMNamespaceName()
{
}

PEGASUS_CIMNAME_INLINE const String& CIMNamespaceName::getString() const
{
    return cimNamespaceName;
}

PEGASUS_CIMNAME_INLINE Boolean CIMNamespaceName::isNull() const
{
    return cimNamespaceName.size() == 0;
}

PEGASUS_CIMNAME_INLINE void CIMNamespaceName::clear()
{
    cimNamespaceName.clear();
}

PEGASUS_CIMNAME_INLINE Boolean CIMNamespaceName::equal(
    const CIMNamespaceName& name) const
{
    return String::equalNoCase(cimNamespaceName, name.cimNamespaceName);
}

PEGASUS_CIMNAME_INLINE Boolean CIMNamespaceName::equal(const char* name) const
{
    return String::equalNoCase(cimNamespaceName, name);
}

//
// This function performs a compile-time cast from String to CIMName. It should
// only be used where the String is already known to contain a valid CIM name,
// thereby avoiding the overhead of checking every character of the String.
// This cast is possible because CIMName has a single String member. Note that
// that sizeof(CIMName) == sizeof(String) and that the classes are identical
// in their representation, differing only by interface. When compiled for
// debug, this function checks that str refers to a valid CIM name.
//
inline const CIMName& CIMNameCast(const String& str)
{
#if defined(PEGASUS_DEBUG)

    if (str.size() && !CIMName::legal(str))
    {
        throw InvalidNameException(str);
    }

#endif

    return *(reinterpret_cast<const CIMName*>(&str));
}

inline const CIMNamespaceName& CIMNamespaceNameCast(const String& str)
{
#if defined(PEGASUS_DEBUG)

    if (str.size() && !CIMNamespaceName::legal(str))
    {
        throw InvalidNamespaceNameException(str);
    }

#endif

    return *(reinterpret_cast<const CIMNamespaceName*>(&str));
}

/** Checks whether a given character string consists of ASCII only and
    legal characters for a CIMName (i.e. letter, numbers and underscore)
    The first character has to be a letter or underscore
    @param str character string to be checked
    @return  0 in case non-legal ASCII character was found
            >0 length of the character string str
*/
PEGASUS_COMMON_LINKAGE Uint32 CIMNameLegalASCII(const char* str);

PEGASUS_NAMESPACE_END

#endif /* Pegasus_CIMNameInline_h */
