//%2005////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001, 2002 BMC Software; Hewlett-Packard Development
// Company, L.P.; IBM Corp.; The Open Group; Tivoli Systems.
// Copyright (c) 2003 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation, The Open Group.
// Copyright (c) 2004 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation; VERITAS Software Corporation; The Open Group.
// Copyright (c) 2005 Hewlett-Packard Development Company, L.P.; IBM Corp.;
// EMC Corporation; VERITAS Software Corporation; The Open Group.
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
//              Sushma Fernandes, Hewlett-Packard Company 
//              (sushma_fernandes@hp.com)
//              Carol Ann Krug Graves, Hewlett-Packard Company
//                  (carolann_graves@hp.com)
//              David Dillard, VERITAS Software Corp.
//                  (david.dillard@veritas.com)
//              Yi Zhou, Hewlett-Packard Company (yi.zhou@hp.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#include "CIMInstanceRep.h"
#include "CIMInstance.h"
#include "CIMClassRep.h"
#include "CIMScope.h"
#include "DeclContext.h"
#include "Resolver.h"
#include "Indentor.h"
#include "CIMName.h"
#include "Constants.h"
#include "XmlWriter.h"
#include "MofWriter.h"

PEGASUS_USING_STD;

PEGASUS_NAMESPACE_BEGIN

CIMInstanceRep::CIMInstanceRep(const CIMObjectPath& reference)
    : CIMObjectRep(reference)
{

}

CIMInstanceRep::~CIMInstanceRep()
{

}

void CIMInstanceRep::resolve(
    DeclContext* context,
    const CIMNamespaceName& nameSpace,
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

    if (cimClass.isUninitialized())
	throw PEGASUS_CIM_EXCEPTION(CIM_ERR_INVALID_CLASS, 
            _reference.getClassName().getString ());

    cimClassOut = cimClass;

#if 0
    if (!cimClass._rep->_resolved)
	throw ClassNotResolved(_reference.getClassName());
#endif

    //----------------------------------------------------------------------
    // Disallow instantiation of abstract classes.
    //----------------------------------------------------------------------

    if (cimClass.isAbstract())
	throw InstantiatedAbstractClass(_reference.getClassName().getString ());

    //----------------------------------------------------------------------
    // Validate and propagate qualifiers.
    //----------------------------------------------------------------------
    _qualifiers.resolve(
	context,
	nameSpace,
        (cimClass.isAssociation()) ? CIMScope::ASSOCIATION : CIMScope::CLASS,
	false,
	cimClass._rep->_qualifiers,
	propagateQualifiers);

    //----------------------------------------------------------------------
    // First iterate the properties of this instance and verify that
    // each one is defined in the class and then resolve each one.
    //----------------------------------------------------------------------

    CIMName className = cimClass.getClassName();

    for (Uint32 i = 0, n = _properties.size(); i < n; i++)
    {
	CIMProperty& property = _properties[i];

	Uint32 index = cimClass.findProperty(property.getName());

	if (index == PEG_NOT_FOUND)
        {
            //
            //  Allow addition of Creator property to Indication Subscription,
            //  Filter and Handler instances
            //
// l10n add language property support           
            if (!(((className.equal 
                    (CIMName (PEGASUS_CLASSNAME_INDSUBSCRIPTION))) ||
                (className.equal 
                    (CIMName (PEGASUS_CLASSNAME_FORMATTEDINDSUBSCRIPTION))) ||
                (className.equal 
                    (CIMName (PEGASUS_CLASSNAME_INDHANDLER_CIMXML))) ||
								(className.equal 
                    (CIMName (PEGASUS_CLASSNAME_LSTNRDST_CIMXML))) ||
                (className.equal 
                    (CIMName (PEGASUS_CLASSNAME_INDHANDLER_SNMP))) ||
#ifdef  PEGASUS_ENABLE_SYSTEM_LOG_HANDLER
                (className.equal 
                    (CIMName (PEGASUS_CLASSNAME_LSTNRDST_SYSTEM_LOG))) ||
#endif
#ifdef  PEGASUS_ENABLE_EMAIL_HANDLER
                (className.equal 
                    (CIMName (PEGASUS_CLASSNAME_LSTNRDST_EMAIL))) ||
#endif
                (className.equal (CIMName (PEGASUS_CLASSNAME_INDFILTER)))) &&
                ((property.getName ().equal 
                    (CIMName (PEGASUS_PROPERTYNAME_INDSUB_CREATOR))) ||
                (property.getName ().equal 
                    (CIMName (PEGASUS_PROPERTYNAME_INDSUB_ACCEPTLANGS))) ||
                (property.getName ().equal 
                    (CIMName (PEGASUS_PROPERTYNAME_INDSUB_CONTENTLANGS))))))
            {
	        throw NoSuchProperty(property.getName().getString ());
            }
        }
        else
        {
	    // resolve the property
	    Resolver::resolveProperty (property, context, nameSpace, true, 
                cimClass.getProperty (index), propagateQualifiers);
        // Ensure that the PROPAGATED attribute is false
        property.setPropagated(false);
        }
    }

    //----------------------------------------------------------------------
    // Inject all properties from the class that are not included in the
    // instance. Copy over the class-origin and set the propagated flag
    // to true. NOTE: The propagated flag indicates that the property
    // was not part of the property set input with the create and
    // was inherited from the default in the class (see cimxml spec sect 3.1.5) 
    //----------------------------------------------------------------------

    for (Uint32 i = 0, m = 0, n = cimClass.getPropertyCount(); i < n; i++)
    {
    	CIMConstProperty property = cimClass.getProperty(i);
    	const CIMName& name = property.getName();
    
    	// See if this instance already contains a property with this name:
    
    	Boolean found = false;
    
    	for (Uint32 j = m, n = _properties.size(); j < n; j++)
    	{
    	    if (name.equal(_properties[j].getName()))
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

void CIMInstanceRep::toXml(Array<char>& out) const
{
    // Class opening element:

    out << "<INSTANCE ";
    out << " CLASSNAME=\"" << _reference.getClassName() << "\" ";
    out << ">\n";

    // Qualifiers:

    _qualifiers.toXml(out);

    // Parameters:

    for (Uint32 i = 0, n = _properties.size(); i < n; i++)
	XmlWriter::appendPropertyElement(out, _properties[i]);

    // Class closing element:

    out << "</INSTANCE>\n";
}

void CIMInstanceRep::toMof(Array<char>& out) const
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
    	// Generate MOF if this property not propagated
    	// Note that the test is required only because
    	// there is an error in getclass that does not
    	// test the localOnly flag.
    	if (!_properties[i].getPropagated())
    	    MofWriter::appendPropertyElement(out, _properties[i]);
    }

    // Class closing element:
    out << "\n};\n";
}

CIMObjectPath CIMInstanceRep::buildPath(
    const CIMConstClass& cimClass) const
{
    //--------------------------------------------------------------------------
    // Get class name:
    //--------------------------------------------------------------------------

    CIMName className = getClassName();

    //--------------------------------------------------------------------------
    // Get key names:
    //--------------------------------------------------------------------------

    Array<CIMName> keyNames;
    cimClass.getKeyNames(keyNames);

    if (keyNames.size() == 0)
	return CIMObjectPath("", CIMNamespaceName(), className);

    //--------------------------------------------------------------------------
    // Get type and value for each key (building up key bindings):
    //--------------------------------------------------------------------------

    Array<CIMKeyBinding> keyBindings;

    for (Uint32 i = 0, n = keyNames.size(); i < n; i++)
    {
	const CIMName& keyName = keyNames[i];

	Uint32 index = findProperty(keyName);
        if (index == PEG_NOT_FOUND)
        {
            throw NoSuchProperty(keyName.getString());
        }

	CIMConstProperty tmp = getProperty(index);

	if (keyName.equal(tmp.getName()))
	{
	    keyBindings.append(CIMKeyBinding(keyName, tmp.getValue()));
	}
    }

    return CIMObjectPath(String(), CIMNamespaceName(), className, keyBindings);
}

// KS Mar 05 - The following removal functions are very inefficient and should
// be optimized to avoid the multiple memory moves.  Actually, the remove
// qualifiers should be added as a function and optimized that once.
void CIMInstanceRep::filter(Boolean includeQualifiers, Boolean includeClassOrigin,
                        const CIMPropertyList& propertyList)
{
    // Filter any qualifiers from this instance.
    if (!includeQualifiers && _qualifiers.getCount() > 0)
    {
        while( _qualifiers.getCount() )
        {
            _qualifiers.removeQualifier(0);
        }
    }

    // For each property, remove if not in propertylist
    for (Uint32 i = 0 ; i < _properties.size(); i++)
    {
        CIMConstProperty p = getProperty(i);
        CIMName name = p.getName();
        Array<CIMName> pl = propertyList.getPropertyNameArray();
        if (propertyList.isNull() || Contains(pl, name))
        {
            // test ClassOrigin and possibly remove
            if (!includeClassOrigin)
            {
                _properties[i].setClassOrigin(CIMName());
            }
            // remove qualifiers if required.
            if (!includeQualifiers && _properties[i].getQualifierCount() > 0)
            {
                while( _properties[i].getQualifierCount() > 0 )
                {
                    _properties[i].removeQualifier(0);
                }
            }
        }
        else
        {
            _properties.remove(i--);
        }
    }
    return;
}

PEGASUS_NAMESPACE_END
