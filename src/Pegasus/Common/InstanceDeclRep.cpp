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
// $Log: InstanceDeclRep.cpp,v $
// Revision 1.1.1.1  2001/01/14 19:52:39  mike
// Pegasus import
//
//
//END_HISTORY

#include "InstanceDecl.h"
#include "DeclContext.h"
#include "Indentor.h"
#include "Name.h"
#include "XmlWriter.h"

PEGASUS_NAMESPACE_BEGIN

InstanceDeclRep::InstanceDeclRep(const String& className)
    : _className(className), _resolved(false)
{
    if (!Name::legal(className))
	throw IllegalName();
}

InstanceDeclRep::~InstanceDeclRep()
{

}

void InstanceDeclRep::addProperty(const Property& x)
{
    if (!x)
	throw UnitializedHandle();

    if (findProperty(x.getName()) != Uint32(-1))
	throw AlreadyExists();

    _properties.append(x);
}

Uint32 InstanceDeclRep::findProperty(const String& name)
{
    for (Uint32 i = 0, n = _properties.getSize(); i < n; i++)
    {
	if (Name::equal(_properties[i].getName(), name))
	    return i;
    }

    return Uint32(-1);
}

Property InstanceDeclRep::getProperty(Uint32 pos)
{
    if (pos >= _properties.getSize())
	throw OutOfBounds();

    return _properties[pos];
}

Uint32 InstanceDeclRep::getPropertyCount() const
{
    return _properties.getSize();
}

// ATTN-A: rework resolve strategy using clone methodology!

void InstanceDeclRep::resolve(
    DeclContext* declContext, 
    const String& nameSpace)
{
    if (_resolved)
	throw InstanceAlreadyResolved();

    if (!declContext)
	throw NullPointer();

    //----------------------------------------------------------------------
    // First obtain the class:
    //----------------------------------------------------------------------

    ConstClassDecl classDecl = 
	declContext->lookupClassDecl(nameSpace, _className);

    if (!classDecl)
	throw NoSuchSuperClass(_className);

    if (!classDecl._rep->_resolved)
	throw ClassNotResolved(_className);

    //----------------------------------------------------------------------
    // Disallow instantiation of abstract classes.
    //----------------------------------------------------------------------

    if (classDecl.isAbstract())
	throw InstantiatedAbstractClass();

    //----------------------------------------------------------------------
    // Validate the qualifiers of this class:
    //----------------------------------------------------------------------

    _qualifiers.resolve(
	declContext,
	nameSpace,
	Scope::CLASS,
	false,
	classDecl._rep->_qualifiers);

    //----------------------------------------------------------------------
    // First iterate the properties of this instance and verify that
    // each one is defined in the class and also check that the types
    // are consistent. Set the class-origin as well.
    //----------------------------------------------------------------------

    for (Uint32 i = 0, n = _properties.getSize(); i < n; i++)
    {
	Property property = _properties[i];

	Uint32 pos = classDecl.findProperty(property.getName());

	if (pos == Uint32(-1))
	    throw NoSuchProperty(property.getName());

	ConstProperty classProperty = classDecl.getProperty(pos);

	property.resolve(declContext, nameSpace, true, classProperty);
    }

    //----------------------------------------------------------------------
    // Inject all properties from the class that are not included in the
    // instance. Copy over the class-origin and set the propagated flag
    // to true.
    //----------------------------------------------------------------------

    Uint32 start = 0;

    for (Uint32 i = 0, n = classDecl.getPropertyCount(); i < n; i++)
    {
	ConstProperty property = classDecl.getProperty(i);
	const String& classOrigin = property.getClassOrigin();
	const String& name = property.getName();

	// See if this instance already contains a property with this name:

	Boolean found = false;

	for (Uint32 j = start, n = _properties.getSize(); j < n; j++)
	{
	    if (_properties[j].getName() == name)
	    {
		found = true;
		break;
	    }
	}

	if (!found)
	{
	    _properties.insert(start++, Property(property));
	}
    }

    _resolved = true;
}

InstanceDeclRep::InstanceDeclRep()
{

}

InstanceDeclRep::InstanceDeclRep(const InstanceDeclRep& x) : 
    Sharable(),
    _className(x._className),
    _resolved(x._resolved)
{
    x._qualifiers.cloneTo(_qualifiers);

    _properties.reserve(x._properties.getSize());

    for (Uint32 i = 0, n = x._properties.getSize(); i < n; i++)
	_properties.append(x._properties[i].clone());
}

InstanceDeclRep& InstanceDeclRep::operator=(const InstanceDeclRep& x) 
{ 
    return *this; 
}

Boolean InstanceDeclRep::identical(const InstanceDeclRep* x) const
{
    if (_className != x->_className)
	return false;

    if (!_qualifiers.identical(x->_qualifiers))
	return false;

    // Compare properties:

    {
	const Array<Property>& tmp1 = _properties;
	const Array<Property>& tmp2 = x->_properties;

	if (tmp1.getSize() != tmp2.getSize())
	    return false;

	for (Uint32 i = 0, n = tmp1.getSize(); i < n; i++)
	{
	    if (!tmp1[i].identical(tmp2[i]))
		return false;
	}
    }

    if (_resolved != x->_resolved)
	return false;

    return true;
}

void InstanceDeclRep::toXml(Array<Sint8>& out) const
{
    // Class opening element:

    out << "<INSTANCE ";
    out << " CLASSNAME=\"" << _className << "\" ";
    out << ">\n";

    // Qualifiers:

    _qualifiers.toXml(out);

    // Parameters:

    for (Uint32 i = 0, n = _properties.getSize(); i < n; i++)
	_properties[i].toXml(out);

    // Class closing element:

    out << "</INSTANCE>\n";
}

void InstanceDeclRep::print() const
{
    Array<Sint8> tmp;
    toXml(tmp);
    tmp.append('\0');
    std::cout << tmp.getData() << std::endl;
}

PEGASUS_NAMESPACE_END
