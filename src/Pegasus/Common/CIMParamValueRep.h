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

#ifndef Pegasus_ParamValueRep_h
#define Pegasus_ParamValueRep_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/Exception.h>
#include <Pegasus/Common/String.h>
#include <Pegasus/Common/Sharable.h>
#include <Pegasus/Common/CIMValue.h>

PEGASUS_NAMESPACE_BEGIN

class PEGASUS_COMMON_LINKAGE CIMParamValueRep : public Sharable
{
public:

    CIMParamValueRep(
	String parameterName,
	CIMValue value,
	Boolean isTyped=true);

    ~CIMParamValueRep();

    const String & getParameterName() const 
    { 
	return _parameterName; 
    }

    const CIMValue & getValue() const 
    { 
	return _value; 
    }

    Boolean isTyped() const 
    { 
	return _isTyped; 
    }

    void setParameterName(String& parameterName);

    void setValue(CIMValue& value);

    void setIsTyped(Boolean isTyped);

    void toXml(Array<Sint8>& out) const;

    void print(PEGASUS_STD(ostream) &o=PEGASUS_STD(cout)) const;

    Boolean identical(const CIMParamValueRep* x) const;

    CIMParamValueRep* clone() const
    {
	return new CIMParamValueRep(*this);
    }

private:

    CIMParamValueRep();

    CIMParamValueRep(const CIMParamValueRep& x);

    // This method is declared and made private so that the compiler does
    // not implicitly define a default copy constructor.
    CIMParamValueRep& operator=(const CIMParamValueRep& x)
    {
        PEGASUS_ASSERT(0);
        return *this;
    }

    String _parameterName;
    CIMValue _value;
    Boolean _isTyped;
};

PEGASUS_NAMESPACE_END

#endif /* Pegasus_ParamValueRep_h */
