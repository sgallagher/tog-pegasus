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
// Modified By:
//
//%/////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_ParamValueRep_h
#define Pegasus_ParamValueRep_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/Exception.h>
#include <Pegasus/Common/String.h>
#include <Pegasus/Common/Sharable.h>
#include <Pegasus/Common/CIMQualifier.h>
#include <Pegasus/Common/CIMQualifierList.h>
#include <Pegasus/Common/CIMParameter.h>

PEGASUS_NAMESPACE_BEGIN

class DeclContext;
class CIMConstParameter;

// REVIEW: This class should be renamed to CIMParameterValueRep to be consistent
// REVIEW: with the name of the CIMParameterRep class.

// REVIEW: This class contains a CIMParameter but then repeats many of the
// REVIEW: members that CIMParameter contains. The standard does not provide
// REVIEW: a way of passing the type of the CIMParameter. However, CIMParameter
// REVIEW: has a type tag. There is no way to get this information from the wire
// REVIEW: and therefore this class is unworkable.
// REVIEW: In any case, CIMParamValueRep should not repeat the _isArray,
// REVIEW: _arraySize, _referenceClassName, and _qualifiers fields.

class PEGASUS_COMMON_LINKAGE CIMParamValueRep : public Sharable
{
public:

    CIMParamValueRep(
	CIMParameter parameter,
	CIMValue value,
	Boolean isArray,
	Uint32 arraySize,
	const String& referenceClassName);

    ~CIMParamValueRep();

    Boolean isArray() const
    {
	return _isArray;
    }

    Uint32 getAraySize() const
    {
	return _arraySize;
    }

    const String& getReferenceClassName() const 
    {
	return _referenceClassName; 
    }

    const CIMParameter getParameter() const 
    { 
	return _parameter; 
    }

    const CIMValue getValue() const 
    { 
	return _value; 
    }

    void setParameter(CIMParameter parameter);

    void setValue(CIMValue value);

    void addQualifier(const CIMQualifier& qualifier)
    {
	_qualifiers.add(qualifier);
    }

    CIMQualifier getQualifier(Uint32 pos)
    {
	return _qualifiers.getQualifier(pos);
    }

    CIMConstQualifier getQualifier(Uint32 pos) const
    {
	return _qualifiers.getQualifier(pos);
    }

    Uint32 getQualifierCount() const
    {
	return _qualifiers.getCount();
    }

    void resolve(DeclContext* declContext, const String& nameSpace);

    void toXml(Array<Sint8>& out) const;

    void toMof(Array<Sint8>& out) const;

    void print(PEGASUS_STD(ostream) &o=PEGASUS_STD(cout)) const;

    Boolean identical(const CIMParamValueRep* x) const;

    CIMParamValueRep* clone() const
    {
	return new CIMParamValueRep(*this);
    }

private:

    CIMParamValueRep();

    CIMParamValueRep(const CIMParamValueRep& x);

    CIMParamValueRep& operator=(const CIMParamValueRep& x);

    CIMParameter _parameter;
    CIMValue _value;
    Boolean _isArray;
    Uint32 _arraySize;
    String _referenceClassName;
    CIMQualifierList _qualifiers;
};

PEGASUS_NAMESPACE_END

#endif /* Pegasus_ParamValueRep_h */
