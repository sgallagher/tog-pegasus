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
// Modified By: Roger Kumpf, Hewlett-Packard Company (roger_kumpf@hp.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#include "CIMInstanceRep.h"
#include "CIMInstance.h"
#include "CIMClassRep.h"
#include "CIMScope.h"
#include "DeclContext.h"
#include "Indentor.h"
#include "CIMName.h"
#include "XmlWriter.h"
#include "Constants.h"

PEGASUS_USING_STD;

PEGASUS_NAMESPACE_BEGIN

CIMInstanceRep::CIMInstanceRep(const CIMReference& reference)
    : CIMObjectRep(reference)
{

}

CIMInstanceRep::~CIMInstanceRep()
{

}

void CIMInstanceRep::resolve(
    DeclContext* context,
    const String& nameSpace,
    CIMConstClass& cimClassOut,
    Boolean propagateQualifiers)
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
	context->lookupClass(nameSpace, _reference.getClassName());

    if (cimClass.isNull())
	throw PEGASUS_CIM_EXCEPTION(CIM_ERR_INVALID_CLASS, _reference.getClassName());

    cimClassOut = cimClass;

#if 0
    if (!cimClass._rep->_resolved)
	throw ClassNotResolved(_reference.getClassName());
#endif

    //----------------------------------------------------------------------
    // Disallow instantiation of abstract classes.
    //----------------------------------------------------------------------

    if (cimClass.isAbstract())
	throw InstantiatedAbstractClass(_reference.getClassName());

    //----------------------------------------------------------------------
    // Validate and propagate qualifiers.
    //----------------------------------------------------------------------
    _qualifiers.resolve(
	context,
	nameSpace,
	CIMScope::CLASS,
	false,
	cimClass._rep->_qualifiers,
	propagateQualifiers);

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
        {
            //
            //  Allow addition of Creator property to Indication Subscription,
            //  Filter and Handler instances
            //
            if (!(((classOrigin == PEGASUS_CLASSNAME_INDSUBSCRIPTION) ||
                (classOrigin == PEGASUS_CLASSNAME_INDHANDLER_CIMXML) ||
                (classOrigin == PEGASUS_CLASSNAME_INDHANDLER_SNMP) ||
                (classOrigin == PEGASUS_CLASSNAME_INDFILTER)) &&
                (property.getName () == PEGASUS_PROPERTYNAME_INDSUB_CREATOR)))
            {
	        throw NoSuchProperty(property.getName());
            }
        }
        else
        {
	    // resolve the property
	    property.resolve (context, nameSpace, true, 
                cimClass.getProperty (pos), propagateQualifiers);
            property.setClassOrigin(classOrigin);
        }
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
			CIMProperty p = property.clone(propagateQualifiers);
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
    out << " CLASSNAME=\"" << _reference.getClassName() << "\" ";
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

void CIMInstanceRep::toMof(Array<Sint8>& out) const
{
    // Get and format the class qualifiers
    out << "\n//Instance of Class " << _reference.getClassName();
    if (_qualifiers.getCount())
	out << "\n";
    _qualifiers.toMof(out);

    // Separate qualifiers from Class Name
    out << "\n";

    // output class statement
    out << "instance of class " << _reference.getClassName();

    out << "\n{";

    // format the Properties:
    for (Uint32 i = 0, n = _properties.size(); i < n; i++)
    {
	// Generate MOF if this property not propogated
	// Note that the test is required only because
	// there is an error in getclass that does not
	// test the localOnly flag.
	if (!_properties[i].getPropagated())
	    _properties[i].toMof(out);
    }

    // Class closing element:
    out << "\n};\n";

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

String CIMInstanceRep::toString() const
{
    String objectName;
    CIMReference object = getPath();

    // Get the host:

    if (object.getHost().size() && object.getNameSpace().size())
    {
        objectName = "//";
        objectName += object.getHost();
        objectName += "/";

        objectName += object.getNameSpace();
        objectName += ":";
    }

    // Get the class name:

    const String& className = getClassName();
    objectName.append(className);

    //if (isClassName())
    //    return objectName;

    objectName.append('.');

    // Append each key-value pair:

    //const Array<KeyBinding>& keyBindings = getKeyBindings();
    CIMConstProperty prop;

    for (Uint32 i = 0, n = getPropertyCount(); i < n; i++)
    {
        prop = getProperty(i);
        objectName.append(prop.getName());
        objectName.append('=');

        //const String& value = _escapeSpecialCharacters(
        //    prop.getValue());
        objectName.append(prop.getValue().toString());

        //CIMValue type = prop.getType();

        //if (type == KeyBinding::REFERENCE)
        //    objectName.append('R');

        //if (type == KeyBinding::STRING || type == KeyBinding::REFERENCE)
        //    objectName.append('"');

        //objectName.append(value);

        //if (type == KeyBinding::STRING || type == KeyBinding::REFERENCE)
        //    objectName.append('"');

        if (i + 1 != n)
            objectName.append(',');
    }

    return objectName;
}

PEGASUS_NAMESPACE_END
