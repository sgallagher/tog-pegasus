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
// Modified By: Carol Ann Krug Graves, Hewlett-Packard Company
//                  (carolann_graves@hp.com)
//              Adriann Schuur (schuur@de.ibm.com) PEP 164
//              Dave Sudlik, IBM (dsudlik@us.ibm.com)
//              David Dillard, VERITAS Software Corp.
//                  (david.dillard@veritas.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_CIMObjectRep_h
#define Pegasus_CIMObjectRep_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/Linkage.h>
#include <Pegasus/Common/String.h>
#include <Pegasus/Common/Sharable.h>
#include <Pegasus/Common/CIMName.h>
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

    CIMObjectRep(const CIMObjectPath& className);

    virtual ~CIMObjectRep();

    const CIMName& getClassName() const
    {
	return _reference.getClassName();
    }

    const CIMObjectPath& getPath() const
    {
	return _reference;
    }

    /**
      Sets the object path for the object
      @param  path  CIMObjectPath containing the object path
     */
    void setPath (const CIMObjectPath & path);

    void addQualifier(const CIMQualifier& qualifier)
    {
	_qualifiers.add(qualifier);
    }

    Uint32 findQualifier(const CIMName& name) const
    {
	return _qualifiers.find(name);
    }

    CIMQualifier getQualifier(Uint32 index)
    {
	return _qualifiers.getQualifier(index);
    }

    CIMConstQualifier getQualifier(Uint32 index) const
    {
	return _qualifiers.getQualifier(index);
    }

    Boolean isTrueQualifer(CIMName& name) const
    {
	return _qualifiers.isTrue(name);
    }

    Uint32 getQualifierCount() const
    {
	return _qualifiers.getCount();
    }

    void removeQualifier(Uint32 index)
    {
	_qualifiers.removeQualifier(index);
    }

    virtual void addProperty(const CIMProperty& x);

    Uint32 findProperty(const CIMName& name) const;

    CIMProperty getProperty(Uint32 index);

    CIMConstProperty getProperty(Uint32 index) const
    {
	return ((CIMObjectRep*)this)->getProperty(index);
    }

    void removeProperty(Uint32 index);

    Uint32 getPropertyCount() const;

    virtual Boolean identical(const CIMObjectRep* x) const;

    virtual void toXml(Array<char>& out) const = 0;

    virtual void toMof(Array<char>& out) const = 0;

    virtual CIMObjectRep* clone() const = 0;

protected:

    CIMObjectRep();

    CIMObjectRep(const CIMObjectRep& x);

    CIMObjectPath _reference;
    CIMQualifierList _qualifiers;
    Array<CIMProperty> _properties;
    Boolean _resolved;

private:

    // This method is declared and made private so that the compiler does
    // not implicitly define a default copy constructor.
    CIMObjectRep& operator=(const CIMObjectRep& x)
    {
	//PEGASUS_ASSERT(0);
	return *this;
    }

    friend class CIMObject;
    friend class BinaryStreamer;
};

PEGASUS_NAMESPACE_END

#endif /* Pegasus_CIMObjectRep_h */
