//%/////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000 The Open Group, BMC Software, Tivoli Systems, IBM
//
// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation
// the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following conditions:
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
// THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.
//
//==============================================================================
//
// Author: Mike Brasher (mbrasher@bmc.com)
//
// Modified By:
//
//%/////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_InstanceDeclRep_h
#define Pegasus_InstanceDeclRep_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/Exception.h>
#include <Pegasus/Common/String.h>
#include <Pegasus/Common/CIMQualifier.h>
#include <Pegasus/Common/CIMQualifierList.h>
#include <Pegasus/Common/CIMProperty.h>

PEGASUS_NAMESPACE_BEGIN

class DeclContext;
class CIMInstance;
class CIMConstInstance;
class CIMClass;
class CIMConstClass;

class PEGASUS_COMMON_LINKAGE CIMInstanceRep : public Sharable
{
public:
    
    CIMInstanceRep(const String& className);

    ~CIMInstanceRep();

    const String& getClassName() const { return _className; }

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
	return ((_qualifiers.find(name) != -1) ? true : false);
    }


    CIMQualifier getQualifier(Uint32 pos)
    {
	return _qualifiers.getQualifier(pos);
    }

    CIMConstQualifier getQualifier(Uint32 pos) const
    {
	return _qualifiers.getQualifier(pos);
    }

    Uint32 getQualifierCount() const
    {
	return _qualifiers.getCount();
    }

    void removeQualifier(Uint32 pos)
    {
	_qualifiers.removeQualifier(pos);
    }

    void addProperty(const CIMProperty& x);

    Uint32 findProperty(const String& name);

    Uint32 findProperty(const String& name) const
    {
	return ((CIMInstanceRep*)this)->findProperty(name);
    }

    Boolean existsProperty(const String& name);

    Boolean existsProperty(const String& name) const
    {
	return ((CIMInstanceRep*)this)->existsProperty(name);
    }

    CIMProperty getProperty(Uint32 pos);

    CIMConstProperty getProperty(Uint32 pos) const
    {
	return ((CIMInstanceRep*)this)->getProperty(pos);
    }

    void removeProperty(Uint32 pos);

    Uint32 getPropertyCount() const;

    void resolve(
	DeclContext* declContext, 
	const String& nameSpace,
	CIMConstClass& cimClassOut);

    void toXml(Array<Sint8>& out) const;

    void print(PEGASUS_STD(ostream) &o=PEGASUS_STD(cout)) const;

    Boolean identical(const CIMInstanceRep* x) const;

    CIMInstanceRep* clone() const
    {
	return new CIMInstanceRep(*this);
    }

    CIMReference getInstanceName(const CIMConstClass& cimClass) const;

private:

    CIMInstanceRep();

    CIMInstanceRep(const CIMInstanceRep& x);

    CIMInstanceRep& operator=(const CIMInstanceRep& x);

    String _className;
    CIMQualifierList _qualifiers;
    Array<CIMProperty> _properties;
    Boolean _resolved;

    friend class CIMInstance;
};

PEGASUS_NAMESPACE_END

#endif /* Pegasus_InstanceDeclRep_h */
