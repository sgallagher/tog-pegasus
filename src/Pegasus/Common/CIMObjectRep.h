//%/////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001 The Open group, BMC Software, Tivoli Systems, IBM
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
// Modified By:
//
//%/////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_CIMObjectRep_h
#define Pegasus_CIMObjectRep_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/String.h>
#include <Pegasus/Common/CIMProperty.h>
#include <Pegasus/Common/CIMQualifier.h>
#include <Pegasus/Common/CIMQualifierList.h>
#include <Pegasus/Common/Array.h>

PEGASUS_NAMESPACE_BEGIN

/** This class defines the internal representation of the CIMObject class.

    This base class has two implementations: CIMClassRep CIMInstanceRep. The
    CIMObjectRep pointer member of CIMObject points to one of these.

    This class contains what is common to CIMClass and CIMInstance.
*/
class PEGASUS_COMMON_LINKAGE CIMObjectRep : public Sharable
{
public:

    CIMObjectRep(const CIMReference& className);

    virtual ~CIMObjectRep();

    const String& getClassName() const
    {
	return _reference.getClassName();
    }

    const Boolean equalClassName(const String& classname) const
    {
	return (String::equalNoCase(classname, _reference.getClassName()));
    }

    const CIMReference& getPath() const
    {
	return _reference;
    }

    void addQualifier(const CIMQualifier& qualifier)
    {
	_qualifiers.add(qualifier);
    }

    Uint32 findQualifier(const String& name) const
    {
	return _qualifiers.find(name);
    }

    Boolean existsQualifier(const String& name) const
    {
	return ((_qualifiers.find(name) != PEG_NOT_FOUND) ? true : false);
    }

    CIMQualifier getQualifier(Uint32 pos)
    {
	return _qualifiers.getQualifier(pos);
    }

    CIMConstQualifier getQualifier(Uint32 pos) const
    {
	return _qualifiers.getQualifier(pos);
    }

    Boolean isTrueQualifer(String& name) const
    {
	return _qualifiers.isTrue(name);
    }

    Uint32 getQualifierCount() const
    {
	return _qualifiers.getCount();
    }

    void removeQualifier(Uint32 pos)
    {
	_qualifiers.removeQualifier(pos);
    }

    virtual void addProperty(const CIMProperty& x);

    Uint32 findProperty(const String& name) const;

    Boolean existsProperty(const String& name) const;

    CIMProperty getProperty(Uint32 pos);

    CIMConstProperty getProperty(Uint32 pos) const
    {
	return ((CIMObjectRep*)this)->getProperty(pos);
    }

    void removeProperty(Uint32 pos);

    Uint32 getPropertyCount() const;

    virtual Boolean identical(const CIMObjectRep* x) const;

    virtual void toXml(Array<Sint8>& out) const = 0;

    virtual void print(PEGASUS_STD(ostream)& os = PEGASUS_STD(cout)) const = 0;

    virtual CIMObjectRep* clone() const = 0;

protected:

    CIMObjectRep();

    CIMObjectRep(const CIMObjectRep& x);

    CIMReference _reference;
    CIMQualifierList _qualifiers;
    Array<CIMProperty> _properties;
    Boolean _resolved;

private:

    // This method is declared and made private so that the compiler does
    // not implicitly define a default copy constructor.
    CIMObjectRep& operator=(const CIMObjectRep& x)
    {
	PEGASUS_ASSERT(0);
	return *this;
    }

    friend class CIMObject;
};

PEGASUS_NAMESPACE_END

#endif /* Pegasus_CIMObjectRep_h */
