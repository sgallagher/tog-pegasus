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

#include "CIMQualifier.h"
#include "Indentor.h"
#include "DeclContext.h"
#include "CIMName.h"
#include "Exception.h"
#include "XmlWriter.h"

PEGASUS_NAMESPACE_BEGIN
PEGASUS_USING_STD;
////////////////////////////////////////////////////////////////////////////////
//
// CIMQualifierRep
//
////////////////////////////////////////////////////////////////////////////////

CIMQualifierRep::CIMQualifierRep(
    const String& name, 
    const CIMValue& value, 
    Uint32 flavor,
    Boolean propagated)
    : 
    _name(name), 
    _value(value), 
    _flavor(flavor),
    _propagated(propagated)
{
    if (!CIMName::legal(name))
	throw IllegalName();

    if (_value.getType() == CIMType::NONE)
	throw NullType();
}

CIMQualifierRep::~CIMQualifierRep()
{

}

void CIMQualifierRep::setName(const String& name) 
{
    if (!CIMName::legal(name))
	throw IllegalName();

    _name = name; 
}

static const char* _toString(Boolean x)
{
    return x ? "true" : "false";
}

void CIMQualifierRep::toXml(Array<Sint8>& out) const
{
    out << "<QUALIFIER";
    out << " NAME=\"" << _name << "\"";
    out << " TYPE=\"" << TypeToString(_value.getType()) << "\"";

    if (_propagated != false)
	out << " PROPAGATED=\"" << _toString(_propagated) << "\"";

    FlavorToXml(out, _flavor);

    out << ">\n";
    
    /* While property Null XML is to be printed without the value tag
       the specification requires that Qualifer NULL values include the
       value tag
       KSTESTNULL
    */
    _value.toXml(out, true);

    out << "</QUALIFIER>\n";
}

void CIMQualifierRep::print(PEGASUS_STD(ostream) &os) const
{
    Array<Sint8> tmp;
    toXml(tmp);
    tmp.append('\0');
    os << tmp.getData() << PEGASUS_STD(endl);
}

/** toMof Generates MOF output for a qualifier.
    The BNF for this is:
    <pre>
    qualifier 	       = qualifierName [ qualifierParameter ] [ ":" 1*flavor]

    qualifierParameter = "(" constantValue ")" | arrayInitializer

    arrayInitializer   = "{" constantValue*( "," constantValue)"}"
    </pre>
*/
void CIMQualifierRep::toMof(Array<Sint8>& out) const
{
    // Output Qualifier name
    out << _name;

    /* If the qualifier is Boolean, we do not put out a value. This is
       the way MOF is shown.  Note that we should really be checking
       the qualifierdecl to compare with the default but the defaults
       on the current ones are all false so the existence of the qualifier
       implies true.
       Also if the value is Null, we do not put out a value because
       no value has been set.  Assumes that qualifiers are built
       with NULL set if no value has been placed in the qualifier.
    */
    Boolean hasValueField = false;
    if (!_value.isNull() || !(_value.getType() == CIMType::BOOLEAN) )
    {
	out << " (";
	hasValueField = true;
	_value.toMof(out);
    }

    // output the flavors
    // ATTN: KS P3 This is a poor test to see if flavor output exists
    String flavorString;
    flavorString = FlavorToMof(_flavor);
    if (flavorString.size())
    {
	if (hasValueField == false)
	    out << " (";
	out << " : ";
	out << flavorString;
	hasValueField = true;
	//FlavorToMof(out, _flavor);
    }

    if (hasValueField)
	out << ")"; 
}

void CIMQualifierRep::printMof(PEGASUS_STD(ostream) &os) const
{
    Array<Sint8> tmp;
    toMof(tmp);
    tmp.append('\0');
    os << tmp.getData() << PEGASUS_STD(endl);
}


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

CIMQualifierRep& CIMQualifierRep::operator=(const CIMQualifierRep& x) 
{ 
    return *this; 
}

Boolean CIMQualifierRep::identical(const CIMQualifierRep* x) const
{
    return
	this == x ||
	CIMName::equal(_name, x->_name) && 
	_value == x->_value && 
	_flavor == x->_flavor &&
	_propagated == x->_propagated;
}

void CIMQualifierRep::setValue(const CIMValue& value) 
{
    _value = value; 

    if (_value.getType() == CIMType::NONE)
	throw NullType();
}

PEGASUS_NAMESPACE_END
