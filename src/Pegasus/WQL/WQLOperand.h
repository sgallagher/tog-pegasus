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

#ifndef Pegasus_WQLOperand_h
#define Pegasus_WQLOperand_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/String.h>
#include <Pegasus/WQL/Linkage.h>

PEGASUS_NAMESPACE_BEGIN

enum WQLOperandIntegerValueTag { WQL_OPERAND_INTEGER_VALUE };
enum WQLOperandDoubleValueTag { WQL_OPERAND_DOUBLE_VALUE };
enum WQLOperandBooleanValueTag { WQL_OPERAND_BOOLEAN_VALUE };
enum WQLOperandStringValueTag { WQL_OPERAND_STRING_VALUE };
enum WQLOperandPropertyNameTag { WQL_OPERAND_PROPERTY_NAME };

class PEGASUS_WQL_LINKAGE WQLOperand
{
public:

    enum Type
    {
	UNITIALIZED,
	INTEGER_VALUE,
	DOUBLE_VALUE,
	BOOLEAN_VALUE,
	STRING_VALUE,
	PROPERTY_NAME
    };

    WQLOperand();

    WQLOperand(const WQLOperand& x);

    ~WQLOperand();

    WQLOperand& operator=(const WQLOperand& x);

    void clear();

    void assign(const WQLOperand& x);

    Type getType() const { return _type; }

    void setIntegerValue(Sint32 x)
    {
	clear();
	_integerValue = x;
	_type = INTEGER_VALUE;
    }

    void setDoubleValue(Real64 x)
    {
	clear();
	_doubleValue = x;
	_type = DOUBLE_VALUE;
    }

    void setBooleanValue(Boolean x)
    {
	clear();
	_booleanValue = x;
	_type = BOOLEAN_VALUE;
    }

    void setStringValue(const String& x)
    {
	clear();
	new(_stringValue) String(x);
	_type = STRING_VALUE;
    }

    void setPropertyName(const String& x)
    {
	clear();
	new(_propertyName) String(x);
	_type = PROPERTY_NAME;
    }

private:

    union
    {
	Sint32 _integerValue;
	Real64 _doubleValue;
	Boolean _booleanValue;
	char _stringValue[sizeof(String)];
	char _propertyName[sizeof(String)];
    };

    Type _type;
};

PEGASUS_NAMESPACE_END

#endif /* Pegasus_WQLOperand_h */
