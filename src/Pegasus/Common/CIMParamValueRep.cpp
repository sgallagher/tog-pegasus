//%/////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001 BMC Software, Hewlett-Packard Company, IBM,
// The Open Group, Tivoli Systems
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
// Author: Nitin Upasani, Hewlett-Packard Company (Nitin_Upasani@hp.com)
//
// Modified By: Roger Kumpf, Hewlett-Packard Company (roger_kumpf@hp.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#include <Pegasus/Common/Config.h>
#include <cstdio>
#include "XmlWriter.h"
#include "CIMParamValueRep.h"

PEGASUS_NAMESPACE_BEGIN

CIMParamValueRep::CIMParamValueRep(
    String parameterName,
    CIMValue value,
    Boolean isTyped)
    : _parameterName(parameterName), _value(value), _isTyped(isTyped)
{
}

CIMParamValueRep::~CIMParamValueRep()
{

}

//------------------------------------------------------------------------------
//
//     <!ELEMENT PARAMVALUE (VALUE|VALUE.REFERENCE|VALUE.ARRAY|VALUE.REFARRAY)?>
//     <!ATTLIST PARAMVALUE
//         %CIMName;
//         %ParamType;>
//
//------------------------------------------------------------------------------
void CIMParamValueRep::toXml(Array<Sint8>& out) const
{
    out << "<PARAMVALUE NAME=\"" << _parameterName << "\"";

    CIMType type = _value.getType();

    if (_isTyped && (type != CIMType::NONE))
    {
        out << " PARAMTYPE=\"" << type.toString() << "\"";
    }

    out << ">\n";
    XmlWriter::appendValueElement(out, _value);

    out << "</PARAMVALUE>\n";
}

void CIMParamValueRep::print(PEGASUS_STD(ostream) &os) const 
{
    Array<Sint8> tmp;
    toXml(tmp);
    tmp.append('\0');
    os << tmp.getData() << PEGASUS_STD(endl);
}

Boolean CIMParamValueRep::identical(const CIMParamValueRep* x) const
{
    if (_parameterName != x->_parameterName)
	return false;

    if (_value != x->_value)
	return false;

    if (_isTyped != x->_isTyped)
	return false;

    return true;
}

CIMParamValueRep::CIMParamValueRep()
{

}

CIMParamValueRep::CIMParamValueRep(const CIMParamValueRep& x) :
    Sharable(),
    _parameterName(x._parameterName),
    _value(x._value),
    _isTyped(x._isTyped)
{
}

void CIMParamValueRep::setParameterName(String& parameterName)
{ 
    _parameterName = parameterName;
}

void CIMParamValueRep::setValue(CIMValue& value)
{ 
    _value = value;
}

void CIMParamValueRep::setIsTyped(Boolean isTyped)
{ 
    _isTyped = isTyped;
}

PEGASUS_NAMESPACE_END
