#ifndef _Pegasus_CIMNameInline_h
#define _Pegasus_CIMNameInline_h

#include <Pegasus/Common/Atomic.h>

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
    return (cimName.size() == 0);
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

PEGASUS_CIMNAME_INLINE CIMName::CIMName(
    const String& name, UncheckedTag) : cimName(name)
{
}

PEGASUS_CIMNAME_INLINE CIMName::CIMName(
    const char* name, UncheckedTag) : cimName(name)
{
}

PEGASUS_CIMNAME_INLINE void CIMName::assignUnchecked(const String& name)
{
    cimName = name;
}

PEGASUS_CIMNAME_INLINE void CIMName::assignUnchecked(const char* name)
{
    cimName = name;
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

PEGASUS_CIMNAME_INLINE Boolean operator==(const char* name1, const CIMName& name2)
{
    return name2.equal(name1);
}

PEGASUS_CIMNAME_INLINE Boolean operator!=(const CIMName& name1, const CIMName& name2)
{
    return !name1.equal(name2);
}

PEGASUS_CIMNAME_INLINE Boolean operator!=(const CIMName& name1, const char* name2)
{
    return !name1.equal(name2);
}

PEGASUS_CIMNAME_INLINE Boolean operator!=(const char* name1, const CIMName& name2)
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
    return (cimNamespaceName.size() == 0);
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

PEGASUS_CIMNAME_INLINE Boolean operator==(
    const CIMNamespaceName& name1, const CIMNamespaceName& name2)
{
    return name1.equal(name2);
}

PEGASUS_CIMNAME_INLINE Boolean operator==(
    const CIMNamespaceName& name1, const char* name2)
{
    return name1.equal(name2);
}

PEGASUS_CIMNAME_INLINE Boolean operator==(
    const char* name1, const CIMNamespaceName& name2)
{
    return name2.equal(name1);
}

PEGASUS_CIMNAME_INLINE Boolean operator!=(
    const CIMNamespaceName& name1, const CIMNamespaceName& name2)
{
    return !name1.equal(name2);
}

PEGASUS_CIMNAME_INLINE Boolean operator!=(
    const CIMNamespaceName& name1, const char* name2)
{
    return !name1.equal(name2);
}

PEGASUS_CIMNAME_INLINE Boolean operator!=(
    const char* name1, const CIMNamespaceName& name2)
{
    return !name2.equal(name1);
}

PEGASUS_NAMESPACE_END

#endif /* _Pegasus_CIMNameInline_h */
