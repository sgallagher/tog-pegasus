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

#ifndef Pegasus_QualifierDeclRep_h
#define Pegasus_QualifierDeclRep_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/String.h>
#include <Pegasus/Common/CIMValue.h>
#include <Pegasus/Common/Sharable.h>
#include <Pegasus/Common/Array.h>
#include <Pegasus/Common/Exception.h>
#include <Pegasus/Common/CIMFlavor.h>
#include <Pegasus/Common/CIMScope.h>

PEGASUS_NAMESPACE_BEGIN

class CIMConstQualifierDecl;
class CIMQualifierDecl;

class PEGASUS_COMMON_LINKAGE CIMQualifierDeclRep : public Sharable
{
public:

    CIMQualifierDeclRep(
	const String& name, 
	const CIMValue& value, 
	Uint32 scope,
	Uint32 flavor,
	Uint32 arraySize);

    virtual ~CIMQualifierDeclRep();

    const String& getName() const 
    { 
	return _name; 
    }

    void setName(const String& name);

    CIMType getType() const 
    {
	return _value.getType(); 
    }

    Boolean isArray() const 
    {
	return _value.isArray(); 
    }

    const CIMValue& getValue() const 
    { 
	return _value; 
    }

    void setValue(const CIMValue& value);

    Uint32 getScope() const 
    {
	return _scope; 
    }

    Uint32 getFlavor() const 
    {
	return _flavor; 
    }

    Uint32 getArraySize() const 
    {
	return _arraySize; 
    }

    void toXml(Array<Sint8>& out) const;

    void toMof(Array<Sint8>& out) const;

    void print(PEGASUS_STD(ostream) &o=PEGASUS_STD(cout)) const;

    void printMof(PEGASUS_STD(ostream) &o=PEGASUS_STD(cout)) const;

    Boolean identical(const CIMQualifierDeclRep* x) const;

    CIMQualifierDeclRep* clone() const
    {
	return new CIMQualifierDeclRep(*this);
    }

private:

    CIMQualifierDeclRep();

    CIMQualifierDeclRep(const CIMQualifierDeclRep& x);

    // This method is declared and made private so that the compiler does
    // not implicitly define a default copy constructor.
    CIMQualifierDeclRep& operator=(const CIMQualifierDeclRep& x)
    {
        PEGASUS_ASSERT(0);
        return *this;
    }

    String _name;
    CIMValue _value;
    Uint32 _scope;
    Uint32 _flavor;
    Uint32 _arraySize;
};

PEGASUS_NAMESPACE_END

#endif /* Pegasus_QualifierDeclRep_h */
