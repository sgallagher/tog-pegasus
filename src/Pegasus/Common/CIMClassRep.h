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

#ifndef Pegasus_CIMClassRep_h
#define Pegasus_CIMClassRep_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/Exception.h>
#include <Pegasus/Common/String.h>
#include <Pegasus/Common/CIMQualifier.h>
#include <Pegasus/Common/CIMQualifierList.h>
#include <Pegasus/Common/CIMProperty.h>
#include <Pegasus/Common/CIMMethod.h>

PEGASUS_NAMESPACE_BEGIN

class DeclContext;
class CIMClass;
class CIMConstClass;
class CIMInstanceRep;

class PEGASUS_COMMON_LINKAGE CIMClassRep : public Sharable
{
public:

    CIMClassRep(
	const String& className,
	const String& superClassName);

    ~CIMClassRep();

    Boolean isAssociation() const;

    Boolean isAbstract() const;

    const String& getClassName() const { return _className; }

    const String& getSuperClassName() const { return _superClassName; }

    void setSuperClassName(const String& superClassName);

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
	return ((CIMClassRep*)this)->findProperty(name);
    }

    Boolean existsProperty(const String& name);

    Boolean existsProperty(const String& name) const
    {
	return ((CIMClassRep*)this)->existsProperty(name);
    }

    CIMProperty getProperty(Uint32 pos);

    CIMConstProperty getProperty(Uint32 pos) const
    {
	return ((CIMClassRep*)this)->getProperty(pos);
    }

    void removeProperty(Uint32 pos);


    Uint32 getPropertyCount() const;

    void addMethod(const CIMMethod& x);

    Uint32 findMethod(const String& name);

    Uint32 findMethod(const String& name) const
    {
	return ((CIMClassRep*)this)->findMethod(name);
    }

    Boolean existsMethod(const String& name);

    Boolean existsMethod(const String& name) const
    {
	return ((CIMClassRep*)this)->existsMethod(name);
    }


    CIMMethod getMethod(Uint32 pos);

    CIMConstMethod getMethod(Uint32 pos) const
    {
	return ((CIMClassRep*)this)->getMethod(pos);
    }

    void removeMethod(Uint32 pos);

    Uint32 getMethodCount() const;

    void resolve(
	DeclContext* context,
	const String& nameSpace);

    void toXml(Array<Sint8>& out) const;

    void print(PEGASUS_STD(ostream) &o=PEGASUS_STD(cout)) const;

    void toMof(Array<Sint8>& out) const;

    void printMof(PEGASUS_STD(ostream) &o=PEGASUS_STD(cout)) const;

    Boolean identical(const CIMClassRep* x) const;

    CIMClassRep* clone() const
    {
	return new CIMClassRep(*this);
    }

    void getKeyNames(Array<String>& keyNames) const;

    Boolean hasKeys() const;

private:

    CIMClassRep();

    CIMClassRep(const CIMClassRep& x);

    CIMClassRep& operator=(const CIMClassRep& x);

    String _className;
    String _superClassName;
    CIMQualifierList _qualifiers;
    Array<CIMProperty> _properties;
    Array<CIMMethod> _methods;
    Boolean _resolved;

    friend class CIMClass;
    friend class CIMInstanceRep;
};

PEGASUS_NAMESPACE_END

#endif /* Pegasus_CIMClassRep_h */
