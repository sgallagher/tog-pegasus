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

#include "CIMInstance.h"
#include "DeclContext.h"
#include "Indentor.h"
#include "CIMName.h"
#include "XmlWriter.h"

PEGASUS_NAMESPACE_BEGIN

CIMInstanceRep::CIMInstanceRep(const String& className) : 
    CIMObjectRep(className)
{

}

CIMInstanceRep::~CIMInstanceRep()
{

}

void CIMInstanceRep::resolve(
    DeclContext* context,
    const String& nameSpace,
    CIMConstClass& cimClassOut)
{
    // ATTN: Verify that references are initialized.

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
	context->lookupClass(nameSpace, _className);

    if (!cimClass)
	throw PEGASUS_CIM_EXCEPTION(CIM_ERR_INVALID_CLASS, _className);

    cimClassOut = cimClass;

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

    for (Uint32 i = 0, n = _properties.size(); i < n; i++)
    {
	CIMProperty& property = _properties[i];

	Uint32 pos = cimClass.findProperty(property.getName());

	if (pos == PEG_NOT_FOUND)
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

	for (Uint32 j = m, n = _properties.size(); j < n; j++)
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

CIMInstanceRep::CIMInstanceRep(const CIMInstanceRep& x) : CIMObjectRep(x)
{

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

    for (Uint32 i = 0, n = _properties.size(); i < n; i++)
	_properties[i].toXml(out);

    // Class closing element:

    out << "</INSTANCE>\n";
}

void CIMInstanceRep::print(PEGASUS_STD(ostream) &os) const
{
    Array<Sint8> tmp;
    toXml(tmp);
    tmp.append('\0');
    os << tmp.getData() << PEGASUS_STD(endl);
}

CIMReference CIMInstanceRep::getInstanceName(
    const CIMConstClass& cimClass) const
{
    //--------------------------------------------------------------------------
    // Get class name:
    //--------------------------------------------------------------------------

    String className = getClassName();

    //--------------------------------------------------------------------------
    // Get key names:
    //--------------------------------------------------------------------------

    Array<String> keyNames;
    cimClass.getKeyNames(keyNames);

    if (keyNames.size() == 0)
	return CIMReference();

    //--------------------------------------------------------------------------
    // Get type and value for each key (building up key bindings):
    //--------------------------------------------------------------------------

    KeyBindingArray keyBindings;

    for (Uint32 i = 0, n = keyNames.size(); i < n; i++)
    {
	const String& keyName = keyNames[i];

	Uint32 pos = findProperty(keyName);
	PEGASUS_ASSERT(pos != PEG_NOT_FOUND);

	CIMConstProperty tmp = getProperty(pos);

	if (CIMName::equal(tmp.getName(), keyName))
	{
	    const CIMValue& value = tmp.getValue();

	    // ATTN-A: for now just assert:
	    if (value.isArray())
		PEGASUS_ASSERT(false);

	    CIMType type = value.getType();
	    String valueStr;

	    KeyBinding::Type kbType = KeyBinding::STRING;

	    switch (type)
	    {
		case CIMType::BOOLEAN:
		    kbType = KeyBinding::BOOLEAN;
		    valueStr = value.toString();
		    break;

		case CIMType::UINT8:
		case CIMType::SINT8:
		case CIMType::UINT16:
		case CIMType::SINT16:
		case CIMType::UINT32:
		case CIMType::SINT32:
		case CIMType::UINT64:
		case CIMType::SINT64:
		case CIMType::CHAR16:
		    kbType = KeyBinding::NUMERIC;
		    valueStr = value.toString();
		    break;

		case CIMType::STRING:
		case CIMType::DATETIME:
		    kbType = KeyBinding::STRING;
		    valueStr = value.toString();
		    break;

		case CIMType::REFERENCE:
		    kbType = KeyBinding::REFERENCE;
		    valueStr = value.toString();
		    break;

		case CIMType::REAL32:
		case CIMType::REAL64:
		    PEGASUS_ASSERT(false);
	    }

	    keyBindings.append(KeyBinding(keyName, valueStr, kbType));
	}
    }

    return CIMReference(String(), String(), className, keyBindings);
}

PEGASUS_NAMESPACE_END
