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

#include <cstdio>
#include "CIMQualifierDecl.h"
#include "Indentor.h"
#include "DeclContext.h"
#include "CIMName.h"
#include "Exception.h"
#include "XmlWriter.h"

PEGASUS_NAMESPACE_BEGIN
PEGASUS_USING_STD;
////////////////////////////////////////////////////////////////////////////////
//
// CIMQualifierDeclRep
//
////////////////////////////////////////////////////////////////////////////////

CIMQualifierDeclRep::CIMQualifierDeclRep(
    const String& name,
    const CIMValue& value,
    Uint32 scope,
    Uint32 flavor,
    Uint32 arraySize)
    :
    _name(name),
    _value(value),
    _scope(scope),
    _flavor(flavor),
    _arraySize(arraySize)
{
    //cout << "KSTEST Qualifier Declflavor " << flavor << endl;
    if (!CIMName::legal(name))
	throw IllegalName();

    if (_value.getType() == CIMType::NONE)
	throw NullType();
}

CIMQualifierDeclRep::~CIMQualifierDeclRep()
{

}

void CIMQualifierDeclRep::setName(const String& name)
{
    if (!CIMName::legal(name))
	throw IllegalName();

    _name = name;
}

static const char* _toString(Boolean x)
{
    return x ? "true" : "false";
}

void CIMQualifierDeclRep::toXml(Array<Sint8>& out) const
{
    out << "<QUALIFIER.DECLARATION";
    out << " NAME=\"" << _name << "\"";
    out << " TYPE=\"" << TypeToString(_value.getType()) << "\"";

    if (_value.isArray())
    {
	out << " ISARRAY=\"true\"";

	if (_arraySize)
	{
	    char buffer[64];
	    sprintf(buffer, " ARRAYSIZE=\"%d\"", _arraySize);
	    out << buffer;
	}
    }

    FlavorToXml(out, _flavor);

    out << ">\n";

    ScopeToXml(out, _scope);
    /*KSTESTNULL - Put the XML out with no VALUE tags if Null*/
    _value.toXml(out, false);

    out << "</QUALIFIER.DECLARATION>\n";
}

/** toMof - Generate the MOF output for the Qualifier Declaration object.
    
    The BNF for this output is:
    <pre>
    qualifierDeclaration   = 	QUALIFIER qualifierName qualifierType scope
				[ defaultFlavor ] ";"

    qualifierName 	   = 	IDENTIFIER

    qualifierType 	   = 	":" dataType [ array ] [ defaultValue ]

    scope 		   = 	"," SCOPE "(" metaElement *( "," metaElement )
    ")"
    </pre>
*/
void CIMQualifierDeclRep::toMof(Array<Sint8>& out) const
{
    out << "\n";

    // output the "Qualifier" keyword and name
    out << "Qualfier " << _name;

    // output the qualifiertype
    out << " : " << TypeToString(_value.getType());

    // If array put the Array indicator "[]" and possible size after name.
    if (_value.isArray())
    {
	if (_arraySize)
	{
	    char buffer[32];
	    sprintf(buffer, "[%d]", _arraySize);
	    out << buffer;
	}
	else
	    out << "[]";
    }

    Boolean hasValueField = false;
    // KS think through the following test
    //if (!_value.isNull() || !(_value.getType() == CIMType::BOOLEAN) )
    //{
        // KS With CIM Qualifier, this should be =
	out << " = ";
	hasValueField = true;
	_value.toMof(out);
    //}

    // Output Scope Information
    String scopeString;
    scopeString = ScopeToMofString(_scope);
    //if (scopeString.size())
    //{
	out << ", Scope(" << scopeString << ")";
    //}
    // Output Flavor Information
    String flavorString;
    flavorString = FlavorToMof(_flavor);
    if (flavorString.size())
    {
    out << ", Flavor(" << flavorString << ")";
    }
    // End each qualifier declaration with newline
    out << ";\n";
}


void CIMQualifierDeclRep::print(PEGASUS_STD(ostream) &os) const
{
    Array<Sint8> tmp;
    toXml(tmp);
    tmp.append('\0');
    // ATTN:KS 7 Aug 2001 I think the endl should be removed here.
    os << tmp.getData() << PEGASUS_STD(endl);
}

CIMQualifierDeclRep::CIMQualifierDeclRep()
{

}

CIMQualifierDeclRep::CIMQualifierDeclRep(const CIMQualifierDeclRep& x) :
    Sharable(),
    _name(x._name),
    _value(x._value),
    _scope(x._scope),
    _flavor(x._flavor),
    _arraySize(x._arraySize)
{

}

CIMQualifierDeclRep& CIMQualifierDeclRep::operator=(const CIMQualifierDeclRep&
x)
{
    return *this;
}

Boolean CIMQualifierDeclRep::identical(const CIMQualifierDeclRep* x) const
{
    return
	this == x ||
	CIMName::equal(_name, x->_name) &&
	_value == x->_value &&
	_scope == x->_scope &&
	_flavor == x->_flavor &&
	_arraySize == x->_arraySize;
}

void CIMQualifierDeclRep::setValue(const CIMValue& value)
{
    _value = value;

    if (_value.getType() == CIMType::NONE)
	throw NullType();
}

PEGASUS_NAMESPACE_END

