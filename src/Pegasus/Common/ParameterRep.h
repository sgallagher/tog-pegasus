//BEGIN_LICENSE
//
// Copyright (c) 2000 The Open Group, BMC Software, Tivoli Systems, IBM
//
// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation
// the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following conditions:
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
// THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.
//
//END_LICENSE
//BEGIN_HISTORY
//
// Author:
//
// $Log: ParameterRep.h,v $
// Revision 1.1.1.1  2001/01/14 19:53:03  mike
// Pegasus import
//
//
//END_HISTORY

#ifndef Pegasus_ParameterRep_h
#define Pegasus_ParameterRep_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/Exception.h>
#include <Pegasus/Common/String.h>
#include <Pegasus/Common/Sharable.h>
#include <Pegasus/Common/Qualifier.h>
#include <Pegasus/Common/QualifierList.h>

PEGASUS_NAMESPACE_BEGIN

class DeclContext;
class ConstParameter;
class Parameter;

class PEGASUS_COMMON_LINKAGE ParameterRep : public Sharable
{
public:

    ParameterRep(
	const String& name, 
	Type type,
	Boolean isArray,
	Uint32 arraySize,
	const String& referenceClassName);

    ~ParameterRep();

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

    const Type getType() const 
    { 
	return _type; 
    }

    void setType(Type type);

    void addQualifier(const Qualifier& qualifier)
    {
	_qualifiers.add(qualifier);
    }

    Uint32 findQualifier(const String& name) const
    {
	return _qualifiers.find(name);
    }

    Qualifier getQualifier(Uint32 pos)
    {
	return _qualifiers.getQualifier(pos);
    }

    ConstQualifier getQualifier(Uint32 pos) const
    {
	return _qualifiers.getQualifier(pos);
    }

    Uint32 getQualifierCount() const
    {
	return _qualifiers.getCount();
    }

    void resolve(DeclContext* declContext, const String& nameSpace);

    void toXml(Array<Sint8>& out) const;

    void print() const;

    Boolean identical(const ParameterRep* x) const;

    ParameterRep* clone() const
    {
	return new ParameterRep(*this);
    }

private:

    ParameterRep();

    ParameterRep(const ParameterRep& x);

    ParameterRep& operator=(const ParameterRep& x);

    String _name;
    Type _type;
    Boolean _isArray;
    Uint32 _arraySize;
    String _referenceClassName;
    QualifierList _qualifiers;
};

PEGASUS_NAMESPACE_END

#endif /* Pegasus_ParameterRep_h */
