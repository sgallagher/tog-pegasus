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
// Author: Mike Brasher (mbrasher@bmc.com)
//
// Modified By:
//
//%/////////////////////////////////////////////////////////////////////////////

#include "WQLOperand.h"

PEGASUS_NAMESPACE_BEGIN

WQLOperand::WQLOperand() : _type(UNITIALIZED) 
{ 

}

WQLOperand::WQLOperand(const WQLOperand& x)
{
    assign(x);
}

WQLOperand::~WQLOperand()
{
    clear();
}

WQLOperand& WQLOperand::operator=(const WQLOperand& x)
{
    clear();
    assign(x);
    return *this;
}

void WQLOperand::clear()
{
    if (_type == PROPERTY_NAME)
	((String*)_propertyName)->~String();
    else if (_type == STRING_VALUE)
	((String*)_stringValue)->~String();

    _type = UNITIALIZED;
}

void WQLOperand::assign(const WQLOperand& x)
{
    switch (_type = x._type)
    {
	case PROPERTY_NAME:
	    new(_propertyName) String(*((String*)x._propertyName));
	    break;

	case STRING_VALUE:
	    new(_stringValue) String(*((String*)x._stringValue));
	    break;

	case INTEGER_VALUE:
	    _integerValue = x._integerValue;
	    break;

	case DOUBLE_VALUE:
	    _doubleValue = x._doubleValue;
	    break;

	case BOOLEAN_VALUE:
	    _booleanValue = x._booleanValue;
	    break;
    }
}

PEGASUS_NAMESPACE_END
