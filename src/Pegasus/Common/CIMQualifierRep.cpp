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
// $Log: CIMQualifierRep.cpp,v $
// Revision 1.2  2001/02/26 04:33:28  mike
// Fixed many places where cim names were be compared with operator==(String,String).
// Changed all of these to use CIMName::equal()
//
// Revision 1.1  2001/02/18 18:39:06  mike
// new
//
// Revision 1.1  2001/02/16 02:07:06  mike
// Renamed many classes and headers (using new CIM prefixes).
//
// Revision 1.1.1.1  2001/01/14 19:53:10  mike
// Pegasus import
//
//
//END_HISTORY

#include "CIMQualifier.h"
#include "Indentor.h"
#include "DeclContext.h"
#include "CIMName.h"
#include "Exception.h"
#include "XmlWriter.h"

PEGASUS_NAMESPACE_BEGIN

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

    _value.toXml(out);

    out << "</QUALIFIER>\n";
}

void CIMQualifierRep::print() const
{
    Array<Sint8> tmp;
    toXml(tmp);
    tmp.append('\0');
    std::cout << tmp.getData() << std::endl;
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

PEGASUS_NAMESPACE_END
