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

#ifndef Pegasus_ParameterRep_h
#define Pegasus_ParameterRep_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/Exception.h>
#include <Pegasus/Common/String.h>
#include <Pegasus/Common/Sharable.h>
#include <Pegasus/Common/CIMQualifier.h>
#include <Pegasus/Common/CIMQualifierList.h>

PEGASUS_NAMESPACE_BEGIN

class DeclContext;
class CIMConstParameter;
class CIMParameter;

class PEGASUS_COMMON_LINKAGE CIMParameterRep : public Sharable
{
public:

    CIMParameterRep(
	const String& name, 
	CIMType type,
	Boolean isArray,
	Uint32 arraySize,
	const String& referenceClassName);

    ~CIMParameterRep();

    const String& getName() const 
    { 
	return _name; 
    }

    void setName(const String& name);

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

    const CIMType getType() const 
    { 
	return _type; 
    }

    void setType(CIMType type);

    void addQualifier(const CIMQualifier& qualifier)
    {
	_qualifiers.add(qualifier);
    }

    Uint32 findQualifier(const String& name) const
    {
	return _qualifiers.find(name);
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

    void print(PEGASUS_STD(ostream) &o=PEGASUS_STD(cout)) const;

    Boolean identical(const CIMParameterRep* x) const;

    CIMParameterRep* clone() const
    {
	return new CIMParameterRep(*this);
    }

private:

    CIMParameterRep();

    CIMParameterRep(const CIMParameterRep& x);

    CIMParameterRep& operator=(const CIMParameterRep& x);

    String _name;
    CIMType _type;
    Boolean _isArray;
    Uint32 _arraySize;
    String _referenceClassName;
    CIMQualifierList _qualifiers;
};

PEGASUS_NAMESPACE_END

#endif /* Pegasus_ParameterRep_h */
