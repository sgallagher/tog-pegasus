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
// $Log: QualifierRep.h,v $
// Revision 1.2  2001/01/23 01:25:35  mike
// Reworked resolve scheme.
//
// Revision 1.1.1.1  2001/01/14 19:53:10  mike
// Pegasus import
//
//
//END_HISTORY

#ifndef Pegasus_QualifierRep_h
#define Pegasus_QualifierRep_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/String.h>
#include <Pegasus/Common/Value.h>
#include <Pegasus/Common/Sharable.h>
#include <Pegasus/Common/Array.h>
#include <Pegasus/Common/Pair.h>
#include <Pegasus/Common/Sharable.h>
#include <Pegasus/Common/Exception.h>
#include <Pegasus/Common/Flavor.h>

PEGASUS_NAMESPACE_BEGIN

class ConstQualifier;
class Qualifier;

class PEGASUS_COMMON_LINKAGE QualifierRep : public Sharable
{
public:

    QualifierRep(
	const String& name, 
	const Value& value, 
	Uint32 flavor,
	Boolean propagated);

    virtual ~QualifierRep();

    const String& getName() const 
    { 
	return _name; 
    }

    void setName(const String& name);

    Type getType() const 
    { 
	return _value.getType(); 
    }

    Boolean isArray() const 
    {
	return _value.isArray();
    }

    const Value& getValue() const 
    { 
	return _value; 
    }

    void setValue(const Value& value) 
    {
	_value = value; 

	if (_value.getType() == Type::NONE)
	    throw NullType();
    }

    Uint32 getFlavor() const 
    {
	return _flavor; 
    }

    Boolean getPropagated() const 
    { 
	return _propagated; 
    }

    void setPropagated(Boolean propagated) 
    {
	_propagated = propagated; 
    }

    void toXml(Array<Sint8>& out) const;

    void print() const;

    Boolean identical(const QualifierRep* x) const;

    QualifierRep* clone() const
    {
	return new QualifierRep(*this);
    }

private:

    QualifierRep();

    // Cloning constructor:

    QualifierRep(const QualifierRep& x);

    QualifierRep& operator=(const QualifierRep& x);

    String _name;
    Value _value;
    Uint32 _flavor;
    Boolean _propagated;
};

PEGASUS_NAMESPACE_END

#endif /* Pegasus_QualifierRep_h */
