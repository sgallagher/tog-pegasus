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

    if (superClassName.size() && !CIMName::legal(superClassName))
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

    if (x.getClassOrigin().size() == 0)
	CIMProperty(x).setClassOrigin(_className);

    // Add the property:

    _properties.append(x);
}

void CIMClassRep::removeProperty(Uint32 pos)
{
    if (pos >= _properties.size())
	throw OutOfBounds();

    _properties.remove(pos);
}

Uint32 CIMClassRep::findProperty(const String& name)
{
    for (Uint32 i = 0, n = _properties.size(); i < n; i++)
    {
	if (CIMName::equal(_properties[i].getName(), name))
	    return i;
    }

    return Uint32(-1);
}

CIMProperty CIMClassRep::getProperty(Uint32 pos)
{
    if (pos >= _properties.size())
	throw OutOfBounds();

    return _properties[pos];
}

Uint32 CIMClassRep::getPropertyCount() const
{
    return _properties.size();
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
    for (Uint32 i = 0, n = _methods.size(); i < n; i++)
    {
	if (CIMName::equal(_methods[i].getName(), name))
	    return i;
    }

    return Uint32(-1);
}

CIMMethod CIMClassRep::getMethod(Uint32 pos)
{
    if (pos >= _methods.size())
	throw OutOfBounds();

    return _methods[pos];
}

Uint32 CIMClassRep::getMethodCount() const
{
    return _methods.size();
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

    if (_superClassName.size())
    {
	//----------------------------------------------------------------------
	// First check to see if the super-class really exists:
	//----------------------------------------------------------------------

	CIMConstClass superClass 
	    = context->lookupClass(nameSpace, _superClassName);

	if (!superClass)
	    throw PEGASUS_CIM_EXCEPTION(INVALID_SUPERCLASS, _superClassName);

#if 0
	if (!superClass._rep->_resolved)
	    throw ClassNotResolved(_superClassName);
#endif

	//----------------------------------------------------------------------
	// Iterate all the properties of *this* class. Resolve each one and
	// set the class-origin:
	//----------------------------------------------------------------------

	for (Uint32 i = 0, n = _properties.size(); i < n; i++)
	{
	    CIMProperty& property = _properties[i];
	    Uint32 pos = superClass.findProperty(property.getName());

	    if (pos == Uint32(-1))
	    {
		property.resolve(context, nameSpace, false);
	    }
	    else
	    {
		CIMConstProperty superClassProperty = superClass.getProperty(pos);
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
	    CIMConstProperty superClassProperty = superClass.getProperty(i);

	    // Find the property in *this* class; if not found, then clone and
	    // insert it (setting the propagated flag). Otherwise, change 
	    // the class-origin and propagated flag accordingly.

	    Uint32 pos = Uint32(-1);

	    for (Uint32 j = m, n = _properties.size(); j < n; j++)
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

	for (Uint32 i = 0, n = _methods.size(); i < n; i++)
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

	    for (Uint32 j = m, n = _methods.size(); j < n; j++)
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

	for (Uint32 i = 0, n = _properties.size(); i < n; i++)
	    _properties[i].resolve(context, nameSpace, false);

	//----------------------------------------------------------------------
	// Resolve each method:
	//----------------------------------------------------------------------

	for (Uint32 i = 0, n = _methods.size(); i < n; i++)
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

    if (_superClassName.size())
	out << " SUPERCLASS=\"" << _superClassName << "\" ";

    out << ">\n";

    // Qualifiers:

    _qualifiers.toXml(out);

    // Parameters:

    for (Uint32 i = 0, n = _properties.size(); i < n; i++)
	_properties[i].toXml(out);

    // Methods:

    for (Uint32 i = 0, n = _methods.size(); i < n; i++)
	_methods[i].toXml(out);

    // Class closing element:

    out << "</CLASS>\n";
}

void CIMClassRep::print(std::ostream &os) const
{
    Array<Sint8> tmp;
    toXml(tmp);
    tmp.append('\0');
    XmlWriter::indentedPrint(os, tmp.getData(), 4);
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

    _properties.reserve(x._properties.size());

    for (Uint32 i = 0, n = x._properties.size(); i < n; i++)
	_properties.append(x._properties[i].clone());

    _methods.reserve(x._methods.size());

    for (Uint32 i = 0, n = x._methods.size(); i < n; i++)
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

	if (tmp1.size() != tmp2.size())
	    return false;

	for (Uint32 i = 0, n = tmp1.size(); i < n; i++)
	{
	    if (!tmp1[i].identical(tmp2[i]))
		return false;
	}
    }

    // Compare methods:

    {
	const Array<CIMMethod>& tmp1 = _methods;
	const Array<CIMMethod>& tmp2 = x->_methods;

	if (tmp1.size() != tmp2.size())
	    return false;

	for (Uint32 i = 0, n = tmp1.size(); i < n; i++)
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
	CIMConstProperty property = getProperty(i);

	if (property.isKey())
	    keyNames.append(property.getName());
    }
}

PEGASUS_NAMESPACE_END
