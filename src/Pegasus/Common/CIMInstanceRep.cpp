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
// $Log: CIMInstanceRep.cpp,v $
// Revision 1.7  2001/04/25 07:44:35  mike
// Documented the heck out of the CIMReference class.
//
// Revision 1.6  2001/04/08 01:13:21  mike
// Changed "ConstCIM" to "CIMConst"
//
// Revision 1.4  2001/02/20 07:25:57  mike
// Added basic create-instance in repository and in client.
//
// Revision 1.3  2001/02/20 05:16:57  mike
// Implemented CIMInstance::getInstanceName()
//
// Revision 1.2  2001/02/19 01:47:16  mike
// Renamed names of the form CIMConst to CIMConst.
//
// Revision 1.1  2001/02/18 18:39:06  mike
// new
//
// Revision 1.2  2001/02/18 03:56:00  mike
// Changed more class names (e.g., ConstClassDecl -> CIMConstClass)
//
// Revision 1.1  2001/02/16 02:06:06  mike
// Renamed many classes and headers.
//
// Revision 1.5  2001/01/25 02:12:05  mike
// Added meta-qualifiers to LoadRepository program.
//
// Revision 1.4  2001/01/23 01:25:35  mike
// Reworked resolve scheme.
//
// Revision 1.3  2001/01/22 00:45:47  mike
// more work on resolve scheme
//
// Revision 1.2  2001/01/15 04:31:44  mike
// worked on resolve scheme
//
// Revision 1.1.1.1  2001/01/14 19:52:39  mike
// Pegasus import
//
//
//END_HISTORY

#include "CIMInstance.h"
#include "DeclContext.h"
#include "Indentor.h"
#include "CIMName.h"
#include "XmlWriter.h"

PEGASUS_NAMESPACE_BEGIN

using namespace std;

CIMInstanceRep::CIMInstanceRep(const String& className)
    : _className(className), _resolved(false)
{
    if (!CIMName::legal(className))
	throw IllegalName();
}

CIMInstanceRep::~CIMInstanceRep()
{

}

void CIMInstanceRep::addProperty(const CIMProperty& x)
{
    if (!x)
	throw UnitializedHandle();

    // Reject duplicate property names:

    if (findProperty(x.getName()) != Uint32(-1))
	throw AlreadyExists();

    // Note: class origin is resolved later:

    // Append property:

    _properties.append(x);
}

Uint32 CIMInstanceRep::findProperty(const String& name)
{
    for (Uint32 i = 0, n = _properties.getSize(); i < n; i++)
    {
	if (CIMName::equal(_properties[i].getName(), name))
	    return i;
    }

    return Uint32(-1);
}

CIMProperty CIMInstanceRep::getProperty(Uint32 pos)
{
    if (pos >= _properties.getSize())
	throw OutOfBounds();

    return _properties[pos];
}

Uint32 CIMInstanceRep::getPropertyCount() const
{
    return _properties.getSize();
}

void CIMInstanceRep::resolve(
    DeclContext* context, 
    const String& nameSpace)
{
#if 0
    if (_resolved)
	throw InstanceAlreadyResolved();
#endif

    if (!context)
	throw NullPointer();

    //----------------------------------------------------------------------
    // First obtain the class:
    //----------------------------------------------------------------------

    CIMConstClass cimClass = 
	context->lookupClassDecl(nameSpace, _className);

    if (!cimClass)
	throw NoSuchClass(_className);

#if 0
    if (!cimClass._rep->_resolved)
	throw ClassNotResolved(_className);
#endif

    //----------------------------------------------------------------------
    // Disallow instantiation of abstract classes.
    //----------------------------------------------------------------------

    if (cimClass.isAbstract())
	throw InstantiatedAbstractClass();

    //----------------------------------------------------------------------
    // Validate the qualifiers of this class:
    //----------------------------------------------------------------------

    _qualifiers.resolve(
	context,
	nameSpace,
	CIMScope::CLASS,
	false,
	cimClass._rep->_qualifiers);

    //----------------------------------------------------------------------
    // First iterate the properties of this instance and verify that
    // each one is defined in the class and then resolve each one.
    // Also set the class origin.
    //----------------------------------------------------------------------

    String classOrigin = cimClass.getClassName();

    for (Uint32 i = 0, n = _properties.getSize(); i < n; i++)
    {
	CIMProperty& property = _properties[i];

	Uint32 pos = cimClass.findProperty(property.getName());

	if (pos == Uint32(-1))
	    throw NoSuchProperty(property.getName());

	property.resolve(context, nameSpace, true, cimClass.getProperty(pos));
        property.setClassOrigin(classOrigin);
    }

    //----------------------------------------------------------------------
    // Inject all properties from the class that are not included in the
    // instance. Copy over the class-origin and set the propagated flag
    // to true.
    //----------------------------------------------------------------------

    for (Uint32 i = 0, m = 0, n = cimClass.getPropertyCount(); i < n; i++)
    {
	CIMConstProperty property = cimClass.getProperty(i);
	const String& name = property.getName();

	// See if this instance already contains a property with this name:

	Boolean found = false;

	for (Uint32 j = m, n = _properties.getSize(); j < n; j++)
	{
	    if (CIMName::equal(_properties[j].getName(), name))
	    {
		found = true;
		break;
	    }
	}

	if (!found)
	{
	    CIMProperty p = property.clone();
	    p.setPropagated(true);
	    _properties.insert(m++, p);
	}
    }

#if 0
    _resolved = true;
#endif
}

CIMInstanceRep::CIMInstanceRep()
{

}

CIMInstanceRep::CIMInstanceRep(const CIMInstanceRep& x) : 
    Sharable(),
    _className(x._className),
    _resolved(x._resolved)
{
    x._qualifiers.cloneTo(_qualifiers);

    _properties.reserve(x._properties.getSize());

    for (Uint32 i = 0, n = x._properties.getSize(); i < n; i++)
	_properties.append(x._properties[i].clone());
}

CIMInstanceRep& CIMInstanceRep::operator=(const CIMInstanceRep& x) 
{ 
    return *this; 
}

Boolean CIMInstanceRep::identical(const CIMInstanceRep* x) const
{
    if (_className != x->_className)
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

    if (_resolved != x->_resolved)
	return false;

    return true;
}

void CIMInstanceRep::toXml(Array<Sint8>& out) const
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

void CIMInstanceRep::print(std::ostream &os) const
{
    Array<Sint8> tmp;
    toXml(tmp);
    tmp.append('\0');
    os << tmp.getData() << std::endl;
}

String CIMInstanceRep::getInstanceName(const CIMConstClass& cimClass) const
{
    // ATTN-A: should we disallow keys on arrays and reals?

    // First append the class name:

    String instanceName = ToLower(getClassName());
    instanceName.append('.');

    // Form the key-value pairs:

    Array<String> keyNames;
    cimClass.getKeyNames(keyNames);

    if (keyNames.getSize() == 0)
	return String();

    // Sort the key names:

    BubbleSort(keyNames);

    // Append key value pairs to the instance-name:

    for (Uint32 i = 0, n = keyNames.getSize(); i < n; i++)
    {
	const String keyName = ToLower(keyNames[i]);

	Uint32 pos = findProperty(keyName);
	PEGASUS_ASSERT(pos != Uint32(-1));

	CIMConstProperty tmp = getProperty(pos);

	if (CIMName::equal(tmp.getName(), keyName))
	{
	    const CIMValue& value = tmp.getValue();

	    // ATTN-A: for now just assert:
	    if (value.isArray())
		PEGASUS_ASSERT(false);

	    CIMType type = value.getType();

	    // ATTN-A: throw an actual exception:
	    if (type == CIMType::REAL32 || type == CIMType::REAL64)
		PEGASUS_ASSERT(false);

	    instanceName.append(keyName);
	    instanceName.append('=');

	    String str = value.toString();

	    if (type == CIMType::STRING)
		instanceName.append('"');

	    instanceName.append(str);
	    
	    if (type == CIMType::STRING)
		instanceName.append('"');

	    if (i + 1 != n)
		instanceName.append(',');
	}
    }

    return instanceName;
}

PEGASUS_NAMESPACE_END
