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

#ifndef Pegasus_ParamValue_h
#define Pegasus_ParamValue_h

#include <Pegasus/Common/Config.h>
#ifdef PEGASUS_INTERNALONLY
#include <Pegasus/Common/CIMParamValueRep.h>
#endif

PEGASUS_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////
//
// CIMParamValue
//
////////////////////////////////////////////////////////////////////////////////

class PEGASUS_COMMON_LINKAGE CIMParamValue
{
public:

    CIMParamValue();

    CIMParamValue(const CIMParamValue& x);

    CIMParamValue& operator=(const CIMParamValue& x);

    CIMParamValue(
	String parameterName,
	CIMValue value,
	Boolean isTyped=true);

    ~CIMParamValue();

    String getParameterName() const;

    CIMValue getValue() const;

    Boolean isTyped() const;

    void setParameterName(String& parameterName);

    void setValue(CIMValue& value);

    void setIsTyped(Boolean isTyped=true);

    CIMParamValue clone() const;

#ifdef PEGASUS_INTERNALONLY
    operator int() const;

    void toXml(Array<Sint8>& out) const;

    void print(PEGASUS_STD(ostream) &o=PEGASUS_STD(cout)) const;

    Boolean identical(const CIMParamValue& x) const;

private:

    CIMParamValue(CIMParamValueRep* rep);

    void _checkRep() const;

    CIMParamValueRep* _rep;
#endif
};

#define PEGASUS_ARRAY_T CIMParamValue
# include "ArrayInter.h"
#undef PEGASUS_ARRAY_T

PEGASUS_NAMESPACE_END

#endif /* Pegasus_ParamValue_h */
