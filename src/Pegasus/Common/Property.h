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
// $Log: Property.h,v $
// Revision 1.1  2001/01/14 19:53:04  mike
// Initial revision
//
//
//END_HISTORY

#ifndef Pegasus_Property_h
#define Pegasus_Property_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/PropertyRep.h>

PEGASUS_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////
//
// Property
//
////////////////////////////////////////////////////////////////////////////////

class ConstProperty;
class InstanceDeclRep;

class PEGASUS_COMMON_LINKAGE Property
{
public:

    Property() : _rep(0)
    {

    }

    Property(const Property& x)
    {
	Inc(_rep = x._rep);
    }

    // Throws IllegalName if name argument not legal CIM identifier.

    Property(
	const String& name, 
	const Value& value,
	Uint32 arraySize = 0,
	const String& referenceClassName = String::EMPTY,
	const String& classOrigin = String(),
	Boolean propagated = false)
    {
	_rep = new PropertyRep(name, value,
	    arraySize, referenceClassName, classOrigin, propagated);
    }

    ~Property()
    {
	Dec(_rep);
    }

    Property& operator=(const Property& x)
    {
	if (x._rep != _rep)
	{
	    Dec(_rep);
	    Inc(_rep = x._rep);
	}
	return *this;
    }

    const String& getName() const 
    { 
	_checkRep();
	return _rep->getName(); 
    }

    // Throws IllegalName if name argument not legal CIM identifier.

    void setName(const String& name)
    {
	_checkRep();
	_rep->setName(name);
    }

    const Value& getValue() const 
    { 
	_checkRep();
	return _rep->getValue();
    }

    void setValue(const Value& value) 
    {
	_checkRep();
	_rep->setValue(value);
    }

    Uint32 getArraySize() const
    {
	_checkRep();
	return _rep->getArraySize();
    }

    const String& getReferenceClassName() const 
    {
	_checkRep();
	return _rep->getReferenceClassName(); 
    }

    const String& getClassOrigin() const 
    { 
	_checkRep();
	return _rep->getClassOrigin();
    }

    void setClassOrigin(const String& classOrigin)
    {
	_checkRep();
	_rep->setClassOrigin(classOrigin);
    }

    Boolean getPropagated() const 
    { 
	_checkRep();
	return _rep->getPropagated();
    }

    void setPropagated(Boolean propagated) 
    { 
	_checkRep();
	_rep->setPropagated(propagated);
    }

    // Throws AlreadyExists.

    Property& addQualifier(const Qualifier& x)
    {
	_checkRep();
	_rep->addQualifier(x);
	return *this;
    }

    Uint32 findQualifier(const String& name)
    {
	_checkRep();
	return _rep->findQualifier(name);
    }

    Uint32 findQualifier(const String& name) const
    {
	_checkRep();
	return _rep->findQualifier(name);
    }

    Qualifier getQualifier(Uint32 pos)
    {
	_checkRep();
	return _rep->getQualifier(pos);
    }

    ConstQualifier getQualifier(Uint32 pos) const
    {
	_checkRep();
	return _rep->getQualifier(pos);
    }

    Uint32 getQualifierCount() const
    {
	_checkRep();
	return _rep->getQualifierCount();
    }

    void resolve(
	DeclContext* declContext, 
	const String& nameSpace,
	Boolean isInstancePart,
	const ConstProperty& property)
    {
	_checkRep();
	_rep->resolve(declContext, nameSpace, isInstancePart, property);
    }

    void resolve(
	DeclContext* declContext, 
	const String& nameSpace,
	Boolean isInstancePart)
    {
	_checkRep();
	_rep->resolve(declContext, nameSpace, isInstancePart);
    }

    operator int() const { return _rep != 0; }

    void toXml(Array<Sint8>& out) const
    {
	_checkRep();
	_rep->toXml(out);
    }

    void print() const
    {
	_checkRep();
	_rep->print();
    }

    Boolean identical(const ConstProperty& x) const;

    Property clone() const 
    { 
	return Property(_rep->clone()); 
    }

private:

    Property(PropertyRep* rep) : _rep(rep) 
    { 
    }

    // This constructor allows the ClassDeclRep friend class to cast
    // away constness.

    explicit Property(const ConstProperty& x);

    void _checkRep() const
    {
	if (!_rep)
	    throw UnitializedHandle();
    }

    PropertyRep* _rep;
    friend class ConstProperty;
    friend class ClassDeclRep;
    friend class InstanceDeclRep;
};

////////////////////////////////////////////////////////////////////////////////
//
// ConstProperty
//
////////////////////////////////////////////////////////////////////////////////

class PEGASUS_COMMON_LINKAGE ConstProperty
{
public:

    ConstProperty() : _rep(0)
    {

    }

    ConstProperty(const ConstProperty& x)
    {
	Inc(_rep = x._rep);
    }

    ConstProperty(const Property& x)
    {
	Inc(_rep = x._rep);
    }

    // Throws IllegalName if name argument not legal CIM identifier.

    ConstProperty(
	const String& name, 
	const Value& value,
	Uint32 arraySize = 0,
	const String& referenceClassName = String::EMPTY,
	const String& classOrigin = String(),
	Boolean propagated = false)
    {
	_rep = new PropertyRep(name, value,
	    arraySize, referenceClassName, classOrigin, propagated);
    }

    ~ConstProperty()
    {
	Dec(_rep);
    }

    ConstProperty& operator=(const ConstProperty& x)
    {
	if (x._rep != _rep)
	{
	    Dec(_rep);
	    Inc(_rep = x._rep);
	}
	return *this;
    }

    ConstProperty& operator=(const Property& x)
    {
	if (x._rep != _rep)
	{
	    Dec(_rep);
	    Inc(_rep = x._rep);
	}
	return *this;
    }

    const String& getName() const 
    { 
	_checkRep();
	return _rep->getName(); 
    }

    const Value& getValue() const 
    {
	_checkRep();
	return _rep->getValue();
    }

    Uint32 getArraySize() const
    {
	_checkRep();
	return _rep->getArraySize();
    }

    const String& getReferenceClassName() const 
    {
	_checkRep();
	return _rep->getReferenceClassName(); 
    }

    const String& getClassOrigin() const 
    {
	_checkRep();
	return _rep->getClassOrigin();
    }

    Boolean getPropagated() const 
    { 
	_checkRep();
	return _rep->getPropagated();
    }

    Uint32 findQualifier(const String& name) const
    {
	_checkRep();
	return _rep->findQualifier(name);
    }

    ConstQualifier getQualifier(Uint32 pos) const
    {
	_checkRep();
	return _rep->getQualifier(pos);
    }

    Uint32 getQualifierCount() const
    {
	_checkRep();
	return _rep->getQualifierCount();
    }

    operator int() const { return _rep != 0; }

    void toXml(Array<Sint8>& out) const
    {
	_checkRep();
	_rep->toXml(out);
    }

    void print() const
    {
	_checkRep();
	_rep->print();
    }

    Boolean identical(const ConstProperty& x) const
    {
	x._checkRep();
	_checkRep();
	return _rep->identical(x._rep);
    }

    ConstProperty clone() const 
    {
	return ConstProperty(_rep->clone()); 
    }

private:

    ConstProperty(PropertyRep* rep) : _rep(rep) 
    {
    }

    void _checkRep() const
    {
	if (!_rep)
	    throw UnitializedHandle();
    }

    PropertyRep* _rep;

    friend class Property;
    friend class PropertyRep;
};

PEGASUS_NAMESPACE_END

#endif /* Pegasus_Property_h */
