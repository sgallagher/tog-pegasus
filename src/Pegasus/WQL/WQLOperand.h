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

class PEGASUS_WQL_LINKAGE WQLOperand
{
public:

    enum Type
    {
	NULL_VALUE,
	INTEGER_VALUE,
	DOUBLE_VALUE,
	BOOLEAN_VALUE,
	STRING_VALUE,
	PROPERTY_NAME
    };

    enum IntegerValueTag { INTEGER_VALUE_TAG };
    enum DoubleValueTag { DOUBLE_VALUE_TAG };
    enum BooleanValueTag { BOOLEAN_VALUE_TAG };
    enum StringValueTag { STRING_VALUE_TAG };
    enum PropertyNameTag { PROPERTY_NAME_TAG };


    WQLOperand();

    WQLOperand(const WQLOperand& x);

    WQLOperand(Sint32 x, IntegerValueTag)
    {
	_integerValue = x;
	_type = INTEGER_VALUE;
    }

    WQLOperand(Real64 x, DoubleValueTag)
    {
	_doubleValue = x;
	_type = DOUBLE_VALUE;
    }

    WQLOperand(Boolean x, BooleanValueTag)
    {
	_booleanValue = x;
	_type = BOOLEAN_VALUE;
    }

    WQLOperand(const String& x, StringValueTag)
    {
	new(_stringValue) String(x);
	_type = STRING_VALUE;
    }

    WQLOperand(const String& x, PropertyNameTag)
    {
	new(_propertyName) String(x);
	_type = PROPERTY_NAME;
    }

    ~WQLOperand();

    WQLOperand& operator=(const WQLOperand& x);

    void clear();

    void assign(const WQLOperand& x);

    Type getType() const { return _type; }

    void set(Sint32 x, IntegerValueTag)
    {
	clear();
	_integerValue = x;
	_type = INTEGER_VALUE;
    }

    void set(Real64 x, DoubleValueTag)
    {
	clear();
	_doubleValue = x;
	_type = DOUBLE_VALUE;
    }

    void set(Boolean x, BooleanValueTag)
    {
	clear();
	_booleanValue = x;
	_type = BOOLEAN_VALUE;
    }

    void set(const String& x, StringValueTag)
    {
	clear();
	new(_stringValue) String(x);
	_type = STRING_VALUE;
    }

    void set(const String& x, PropertyNameTag)
    {
	clear();
	new(_propertyName) String(x);
	_type = PROPERTY_NAME;
    }

    String toString() const;

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
