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
// Modified By: Carol Ann Krug Graves, Hewlett-Packard Company
//                  (carolann_graves@hp.com)
//              David Dillard, VERITAS Software Corp.
//                  (david.dillard@veritas.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#include "CIMQualifier.h"
#include "CIMQualifierRep.h"
#include "Indentor.h"
#include "CIMName.h"
#include "InternalException.h"
#include "XmlWriter.h"
#include "MofWriter.h"

PEGASUS_NAMESPACE_BEGIN
PEGASUS_USING_STD;

////////////////////////////////////////////////////////////////////////////////
//
// CIMQualifierRep
//
////////////////////////////////////////////////////////////////////////////////

CIMQualifierRep::CIMQualifierRep()
{
}

CIMQualifierRep::CIMQualifierRep(const CIMQualifierRep& x) :
    Sharable(),
    _name(x._name),
    _value(x._value),
    _flavor(x._flavor),
    _propagated(x._propagated)
{
}

CIMQualifierRep::CIMQualifierRep(
    const CIMName& name,
    const CIMValue& value,
    const CIMFlavor & flavor,
    Boolean propagated)
    :
    _name(name),
    _value(value),
    _flavor(flavor),
    _propagated(propagated)
{
    // ensure name is not null
    if(name.isNull())
    {
        throw UninitializedObjectException();
    }
}

CIMQualifierRep::~CIMQualifierRep()
{
}

void CIMQualifierRep::setName(const CIMName& name)
{
    // ensure name is not null
    if(name.isNull())
    {
        throw UninitializedObjectException();
    }

    _name = name;
}

void CIMQualifierRep::resolveFlavor (
    const CIMFlavor & inheritedFlavor,
    Boolean inherited)
{
    // ATTN: KS P3 Needs more tests and expansion so we treate first different
    // from inheritance

    // if the turnoff flags set, reset the flavor bits
    if (inheritedFlavor.hasFlavor (CIMFlavor::RESTRICTED))
    {
        _flavor.removeFlavor (CIMFlavor::TOSUBCLASS);
        _flavor.removeFlavor (CIMFlavor::TOINSTANCE);
    }
    if (inheritedFlavor.hasFlavor (CIMFlavor::DISABLEOVERRIDE))
    {
        _flavor.removeFlavor (CIMFlavor::ENABLEOVERRIDE);
    }

    _flavor.addFlavor (inheritedFlavor);
}

static const char* _toString(Boolean x)
{
    return x ? "true" : "false";
}

void CIMQualifierRep::toXml(Array<char>& out) const
{
    out << "<QUALIFIER";
    out << " NAME=\"" << _name << "\"";
    out << " TYPE=\"" << cimTypeToString (_value.getType ()) << "\"";

    if (_propagated != false)
	out << " PROPAGATED=\"" << _toString(_propagated) << "\"";

    XmlWriter::appendQualifierFlavorEntity(out, _flavor);

    out << ">\n";

    XmlWriter::appendValueElement(out, _value);

    out << "</QUALIFIER>\n";
}

/** toMof Generates MOF output for a qualifier.
    The BNF for this is:
    <pre>
    qualifier 	       = qualifierName [ qualifierParameter ] [ ":" 1*flavor]

    qualifierParameter = "(" constantValue ")" | arrayInitializer

    arrayInitializer   = "{" constantValue*( "," constantValue)"}"
    </pre>
*/
void CIMQualifierRep::toMof(Array<char>& out) const
{
    // Output Qualifier name
    out << _name;

    /* If the qualifier is Boolean, we do not put out a value. This is
       the way MOF is shown.  Note that we should really be checking
       the qualifierdecl to compare with the default.
       Also if the value is Null, we do not put out a value because
       no value has been set.  Assumes that qualifiers are built
       with NULL set if no value has been placed in the qualifier.
    */
    Boolean hasValueField = false;
    if (!_value.isNull())
    {
	   if (_value.getType() == CIMTYPE_BOOLEAN)
	   {
		    Boolean b;
			_value.get(b);
		    if(!b)
				out << " (false)";
	   }
	   else
	   {
		   out << " (";
		   hasValueField = true;
		   MofWriter::appendValueElement(out, _value);
		   out << ")";
	   }
    }

    // output the flavors
    String flavorString;
    flavorString = MofWriter::getQualifierFlavor(_flavor);
    if (flavorString.size())
    {
		out << " : ";
		out << flavorString;
    }
}


Boolean CIMQualifierRep::identical(const CIMQualifierRep* x) const
{
    return
	this == x ||
	_name.equal(x->_name) &&
	_value == x->_value &&
	(_flavor.equal (x->_flavor)) &&
	_propagated == x->_propagated;
}

void CIMQualifierRep::setValue(const CIMValue& value)
{
    _value = value;
}

PEGASUS_NAMESPACE_END
