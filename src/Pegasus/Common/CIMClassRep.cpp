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
// $Log: CIMClassRep.cpp,v $
// Revision 1.3  2001/02/20 05:16:57  mike
// Implemented CIMInstance::getInstanceName()
//
// Revision 1.2  2001/02/19 01:47:16  mike
// Renamed names of the form CIMConst to ConstCIM.
//
// Revision 1.1  2001/02/18 18:39:05  mike
// new
//
// Revision 1.2  2001/02/18 03:56:00  mike
// Changed more class names (e.g., ConstClassDecl -> ConstCIMClass)
//
// Revision 1.1  2001/02/16 02:06:06  mike
// Renamed many classes and headers.
//
// Revision 1.6  2001/01/28 04:11:03  mike
// fixed qualifier resolution
//
// Revision 1.5  2001/01/26 23:26:53  mike
// reworked CGI inteface
//
// Revision 1.4  2001/01/25 02:12:05  mike
// Added meta-qualifiers to LoadRepository program.
//
// Revision 1.3  2001/01/23 01:25:34  mike
// Reworked resolve scheme.
//
// Revision 1.2  2001/01/15 04:31:43  mike
// worked on resolve scheme
//
// Revision 1.1.1.1  2001/01/14 19:50:39  mike
// Pegasus import
//
//
//END_HISTORY

#include "CIMClass.h"
#include "DeclContext.h"
#include "Indentor.h"
#include "CIMName.h"
#include "CIMQualifierNames.h"
#include "XmlWriter.h"

PEGASUS_NAMESPACE_BEGIN

CIMClassRep::CIMClassRep(
    const String& className, 
    const String& superClassName)
    : _className(className), _superClassName(superClassName), _resolved(false)
{
    if (!CIMName::legal(className))
	throw IllegalName();

    if (superClassName.getLength() && !CIMName::legal(superClassName))
	throw IllegalName();
}

CIMClassRep::~CIMClassRep()
{

}

Boolean CIMClassRep::isAssociation() const
{
    Uint32 pos = findQualifier(CIMQualifierNames::ASSOCIATION);
    
    if (pos == Uint32(-1))
	return false;

    Boolean flag;

    const CIMValue& value = getQualifier(pos).getValue();

    if (value.getType() != CIMType::BOOLEAN)
	return false;

    value.get(flag);
    return flag;
}

Boolean CIMClassRep::isAbstract() const
{
    Uint32 pos = findQualifier(CIMQualifierNames::ABSTRACT);
    
    if (pos == Uint32(-1))
	return false;

    Boolean flag;
    const CIMValue& value = getQualifier(pos).getValue();

    if (value.getType() != CIMType::BOOLEAN)
	return false;

    value.get(flag);
    return flag;
}

void CIMClassRep::setSuperClassName(const String& superClassName) 
{
    if (!CIMName::legal(superClassName))
	throw IllegalName();

    _superClassName = superClassName; 
}

void CIMClassRep::addProperty(const CIMProperty& x)
{
    if (!x)
	throw UnitializedHandle();

    // Reject addition of duplicate property name:

    if (findProperty(x.getName()) != Uint32(-1))
	throw AlreadyExists();

    // Reject addition of references to non-associations:

    if (!isAssociation() && x.getValue().getType() == CIMType::REFERENCE)
	throw AddedReferenceToClass(_className);

    // Set the class origin:
    // ATTN: put this check in other places:

    if (x.getClassOrigin().getLength() == 0)
	CIMProperty(x).setClassOrigin(_className);

    // Add the property:

    _properties.append(x);
}

void CIMClassRep::removeProperty(Uint32 pos)
{
    if (pos >= _properties.getSize())
	throw OutOfBounds();
}

Uint32 CIMClassRep::findProperty(const String& name)
{
    for (Uint32 i = 0, n = _properties.getSize(); i < n; i++)
    {
	if (CIMName::equal(_properties[i].getName(), name))
	    return i;
    }

    return Uint32(-1);
}

CIMProperty CIMClassRep::getProperty(Uint32 pos)
{
    if (pos >= _properties.getSize())
	throw OutOfBounds();

    return _properties[pos];
}

Uint32 CIMClassRep::getPropertyCount() const
{
    return _properties.getSize();
}

void CIMClassRep::addMethod(const CIMMethod& x)
{
    if (!x)
	throw UnitializedHandle();

    // Reject duplicate method names:

    if (findMethod(x.getName()) != Uint32(-1))
	throw AlreadyExists();

    // Add the method:

    _methods.append(x);
}

Uint32 CIMClassRep::findMethod(const String& name)
{
    for (Uint32 i = 0, n = _methods.getSize(); i < n; i++)
    {
	if (CIMName::equal(_methods[i].getName(), name))
	    return i;
    }

    return Uint32(-1);
}

CIMMethod CIMClassRep::getMethod(Uint32 pos)
{
    if (pos >= _methods.getSize())
	throw OutOfBounds();

    return _methods[pos];
}

Uint32 CIMClassRep::getMethodCount() const
{
    return _methods.getSize();
}

void CIMClassRep::resolve(
    DeclContext* context,
    const String& nameSpace)
{
#if 0
    if (_resolved)
	throw ClassAlreadyResolved(_className);
#endif

    if (!context)
	throw NullPointer();

    if (_superClassName.getLength())
    {
	//----------------------------------------------------------------------
	// First check to see if the super-class really exists:
	//----------------------------------------------------------------------

	ConstCIMClass superClass 
	    = context->lookupClassDecl(nameSpace, _superClassName);

	if (!superClass)
	    throw NoSuchSuperClass(_superClassName);

#if 0
	if (!superClass._rep->_resolved)
	    throw ClassNotResolved(_superClassName);
#endif

	//----------------------------------------------------------------------
	// Iterate all the properties of *this* class. Resolve each one and
	// set the class-origin:
	//----------------------------------------------------------------------

	for (Uint32 i = 0, n = _properties.getSize(); i < n; i++)
	{
	    CIMProperty& property = _properties[i];
	    Uint32 pos = superClass.findProperty(property.getName());

	    if (pos == Uint32(-1))
	    {
		property.resolve(context, nameSpace, false);
	    }
	    else
	    {
		ConstCIMProperty superClassProperty = superClass.getProperty(pos);
		property.resolve(context, nameSpace, false, superClassProperty);
	    }
	}

	//----------------------------------------------------------------------
	// Now prepend all properties inherited from the super-class (that 
	// are not overriden by this sub-class).
	//----------------------------------------------------------------------

	// Iterate super-class properties:

	for (Uint32 i = 0, m = 0, n = superClass.getPropertyCount(); i < n; i++)
	{
	    ConstCIMProperty superClassProperty = superClass.getProperty(i);

	    // Find the property in *this* class; if not found, then clone and
	    // insert it (setting the propagated flag). Otherwise, change 
	    // the class-origin and propagated flag accordingly.

	    Uint32 pos = Uint32(-1);

	    for (Uint32 j = m, n = _properties.getSize(); j < n; j++)
	    {
		if (CIMName::equal(
		    _properties[j].getName(), 
		    superClassProperty.getName()))
		{
		    pos = j;
		    break;
		}
	    }

	    // If property exists in super class but not in this one, then
	    // clone and insert it. Otherwise, the properties class
	    // origin was set above.

	    if (pos == Uint32(-1))
	    {
		CIMProperty property = superClassProperty.clone();
		property.setPropagated(true);
		_properties.insert(m++, property);
	    }
	}

	//----------------------------------------------------------------------
	// Iterate all the methods of *this* class. Resolve each one and
	// set the class-origin:
	//----------------------------------------------------------------------

	for (Uint32 i = 0, n = _methods.getSize(); i < n; i++)
	{
	    CIMMethod& method = _methods[i];
	    Uint32 pos = superClass.findMethod(method.getName());

	    if (pos == Uint32(-1))
	    {
		method.resolve(context, nameSpace);
	    }
	    else
	    {
		CIMConstMethod superClassMethod = superClass.getMethod(pos);
		method.resolve(context, nameSpace, superClassMethod);
	    }
	}

	//----------------------------------------------------------------------
	// Now prepend all methods inherited from the super-class (that 
	// are not overriden by this sub-class).
	//----------------------------------------------------------------------

	for (Uint32 i = 0, m = 0, n = superClass.getMethodCount(); i < n; i++)
	{
	    CIMConstMethod superClassMethod = superClass.getMethod(i);

	    // Find the method in *this* class; if not found, then clone and
	    // insert it (setting the propagated flag). Otherwise, change 
	    // the class-origin and propagated flag accordingly.

	    Uint32 pos = Uint32(-1);

	    for (Uint32 j = m, n = _methods.getSize(); j < n; j++)
	    {
		if (CIMName::equal(
		    _methods[j].getName(), 
		    superClassMethod.getName()))
		{
		    pos = j;
		    break;
		}
	    }

	    // If method exists in super class but not in this one, then
	    // clone and insert it. Otherwise, the method's class origin
	    // has already been set above.

	    if (pos == Uint32(-1))
	    {
		CIMMethod method = superClassMethod.clone();
		method.setPropagated(true);
		_methods.insert(m++, method);
	    }
	}

	//----------------------------------------------------------------------
	// Validate the qualifiers of this class:
	//----------------------------------------------------------------------

	_qualifiers.resolve(
	    context,
	    nameSpace,
	    isAssociation() ? CIMScope::ASSOCIATION : CIMScope::CLASS,
	    false,
	    superClass._rep->_qualifiers);
    }
    else
    {
	//----------------------------------------------------------------------
	// Resolve each property:
	//----------------------------------------------------------------------

	for (Uint32 i = 0, n = _properties.getSize(); i < n; i++)
	    _properties[i].resolve(context, nameSpace, false);

	//----------------------------------------------------------------------
	// Resolve each method:
	//----------------------------------------------------------------------

	for (Uint32 i = 0, n = _methods.getSize(); i < n; i++)
	    _methods[i].resolve(context, nameSpace);

	//----------------------------------------------------------------------
	// Resolve the qualifiers:
	//----------------------------------------------------------------------

	CIMQualifierList dummy;

	_qualifiers.resolve(
	    context,
	    nameSpace,
	    isAssociation() ? CIMScope::ASSOCIATION : CIMScope::CLASS,
	    false,
	    dummy);
    }

    // _resolved = true;
}

void CIMClassRep::toXml(Array<Sint8>& out) const
{
    // Class opening element:

    out << "<CLASS ";
    out << " NAME=\"" << _className << "\" ";

    if (_superClassName.getLength())
	out << " SUPERCLASS=\"" << _superClassName << "\" ";

    out << ">\n";

    // Qualifiers:

    _qualifiers.toXml(out);

    // Parameters:

    for (Uint32 i = 0, n = _properties.getSize(); i < n; i++)
	_properties[i].toXml(out);

    // Methods:

    for (Uint32 i = 0, n = _methods.getSize(); i < n; i++)
	_methods[i].toXml(out);

    // Class closing element:

    out << "</CLASS>\n";
}

void CIMClassRep::print() const
{
    Array<Sint8> tmp;
    toXml(tmp);
    tmp.append('\0');
    XmlWriter::indentedPrint(std::cout, tmp.getData(), 4);
    // cout << tmp.getData() << endl;
}

CIMClassRep::CIMClassRep()
{

}

CIMClassRep::CIMClassRep(const CIMClassRep& x) : 
    Sharable(),
    _className(x._className),
    _superClassName(x._superClassName),
    _resolved(x._resolved)
{
    x._qualifiers.cloneTo(_qualifiers);

    _properties.reserve(x._properties.getSize());

    for (Uint32 i = 0, n = x._properties.getSize(); i < n; i++)
	_properties.append(x._properties[i].clone());

    _methods.reserve(x._methods.getSize());

    for (Uint32 i = 0, n = x._methods.getSize(); i < n; i++)
	_methods.append(x._methods[i].clone());
}

CIMClassRep& CIMClassRep::operator=(const CIMClassRep& x) 
{ 
    return *this; 
}

Boolean CIMClassRep::identical(const CIMClassRep* x) const
{
    if (_className != x->_className)
	return false;

    if (_superClassName != x->_superClassName)
	return false;

    if (!_qualifiers.identical(x->_qualifiers))
	return false;

    // Compare properties:

    {
	const Array<CIMProperty>& tmp1 = _properties;
	const Array<CIMProperty>& tmp2 = x->_properties;

	if (tmp1.getSize() != tmp2.getSize())
	    return false;

	for (Uint32 i = 0, n = tmp1.getSize(); i < n; i++)
	{
	    if (!tmp1[i].identical(tmp2[i]))
		return false;
	}
    }

    // Compare methods:

    {
	const Array<CIMMethod>& tmp1 = _methods;
	const Array<CIMMethod>& tmp2 = x->_methods;

	if (tmp1.getSize() != tmp2.getSize())
	    return false;

	for (Uint32 i = 0, n = tmp1.getSize(); i < n; i++)
	{
	    if (!tmp1[i].identical(tmp2[i]))
		return false;

	    if (tmp1[i].getClassOrigin() != tmp2[i].getClassOrigin())
		return false;

	    if (tmp1[i].getPropagated() != tmp2[i].getPropagated())
		return false;
	}
    }

    if (_resolved != x->_resolved)
	return false;

    return true;
}

void CIMClassRep::getKeyNames(Array<String>& keyNames) const
{
    keyNames.clear();

    for (Uint32 i = 0, n = getPropertyCount(); i < n; i++)
    {
	ConstCIMProperty property = getProperty(i);

	if (property.isKey())
	    keyNames.append(property.getName());
    }
}

PEGASUS_NAMESPACE_END
