//BEGIN_LICENSE
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
//END_LICENSE
//BEGIN_HISTORY
//
// Author:
//
// $Log: PropertyRep.h,v $
// Revision 1.1.1.1  2001/01/14 19:53:05  mike
// Pegasus import
//
//
//END_HISTORY

#ifndef Pegasus_PropertyRep_h
#define Pegasus_PropertyRep_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/Exception.h>
#include <Pegasus/Common/String.h>
#include <Pegasus/Common/Value.h>
#include <Pegasus/Common/Qualifier.h>
#include <Pegasus/Common/QualifierList.h>
#include <Pegasus/Common/Sharable.h>
#include <Pegasus/Common/Pair.h>

PEGASUS_NAMESPACE_BEGIN

class ClassDeclRep;
class Property;
class ConstProperty;
class DeclContext;

class PEGASUS_COMMON_LINKAGE PropertyRep : public Sharable
{
public:

    PropertyRep(
	const String& name, 
	const Value& value,
	Uint32 arraySize,
	const String& referenceClassName,
	const String& classOrigin,
	Boolean propagated);

    ~PropertyRep();

    const String& getName() const 
    { 
	return _name; 
    }

    void setName(const String& name);

    const Value& getValue() const 
    { 
	return _value; 
    }

    void setValue(const Value& value);

    Uint32 getArraySize() const
    {
	return _arraySize;
    }

    const String& getReferenceClassName() const 
    {
	return _referenceClassName; 
    }

    const String& getClassOrigin() const 
    { 
	return _classOrigin; 
    }

    void setClassOrigin(const String& classOrigin);

    Boolean getPropagated() const 
    { 
	return _propagated; 
    }

    void setPropagated(Boolean propagated) 
    { 
	_propagated = propagated; 
    }

    void addQualifier(const Qualifier& qualifier)
    {
	_qualifiers.add(qualifier);
    }

    Uint32 findQualifier(const String& name) const
    {
	return _qualifiers.find(name);
    }

    Qualifier getQualifier(Uint32 pos)
    {
	return _qualifiers.getQualifier(pos);
    }

    ConstQualifier getQualifier(Uint32 pos) const
    {
	return _qualifiers.getQualifier(pos);
    }

    Uint32 getQualifierCount() const
    {
	return _qualifiers.getCount();
    }

    void resolve(
	DeclContext* declContext, 
	const String& nameSpace,
	Boolean isInstancePart,
	const ConstProperty& property);

    void resolve(
	DeclContext* declContext, 
	const String& nameSpace,
	Boolean isInstancePart);

    void toXml(Array<Sint8>& out) const;

    void print() const;

    Boolean identical(const PropertyRep* x) const;

    PropertyRep* clone() const
    {
	return new PropertyRep(*this);
    }

private:

    PropertyRep();

    // Cloning constructor:

    PropertyRep(const PropertyRep& x);

    PropertyRep& operator=(const PropertyRep& x);

    String _name;
    Value _value;
    Uint32 _arraySize;
    String _referenceClassName;
    String _classOrigin;
    Boolean _propagated;
    QualifierList _qualifiers;

    friend class ClassDeclRep;
};

PEGASUS_NAMESPACE_END

#endif /* Pegasus_PropertyRep_h */
