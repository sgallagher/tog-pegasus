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
// $Log: MethodRep.h,v $
// Revision 1.1  2001/01/14 19:52:58  mike
// Initial revision
//
//
//END_HISTORY

#ifndef Pegasus_MethodRep_h
#define Pegasus_MethodRep_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/Exception.h>
#include <Pegasus/Common/String.h>
#include <Pegasus/Common/Qualifier.h>
#include <Pegasus/Common/QualifierList.h>
#include <Pegasus/Common/Parameter.h>
#include <Pegasus/Common/Sharable.h>
#include <Pegasus/Common/Pair.h>

PEGASUS_NAMESPACE_BEGIN

class Method;
class ConstMethod;
class DeclContext;

class PEGASUS_COMMON_LINKAGE MethodRep : public Sharable
{
public:

    MethodRep(
	const String& name, 
	Type type,
	const String& classOrigin,
	Boolean propagated);

    ~MethodRep();

    virtual const String& getName() const 
    { 
	return _name; 
    }

    void setName(const String& name);

    Type getType() const 
    {
	return _type; 
    }

    void setType(Type type)
    {
	_type = type; 

	if (type == Type::NONE)
	    throw NullType();
    }

    const String& getClassOrigin() const
    {
	return _classOrigin;
    }

    void setClassOrigin(const String& classOrigin);

    Boolean getPropagated() const 
    { 
	return _propagated; 
    }

    void setPropagated(Boolean propagated) 
    { 
	_propagated = propagated; 
    }

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

    void addParameter(const Parameter& x);

    Uint32 findParameter(const String& name);

    Uint32 findParameter(const String& name) const
    {
	return ((MethodRep*)this)->findParameter(name);
    }

    Parameter getParameter(Uint32 pos);

    ConstParameter getParameter(Uint32 pos) const
    {
	return ((MethodRep*)this)->getParameter(pos);
    }

    Uint32 getParameterCount() const;

    void resolve(
	DeclContext* declContext, 
	const String& nameSpace,
	const ConstMethod& method);

    void resolve(
	DeclContext* declContext,
	const String& nameSpace);

    void toXml(Array<Sint8>& out) const;

    virtual void print() const;

    Boolean identical(const MethodRep* x) const;

    MethodRep* clone() const
    {
	return new MethodRep(*this);
    }

private:

    MethodRep();

    MethodRep(const MethodRep& x);

    MethodRep& operator=(const MethodRep& x);

    String _name;
    Type _type;
    String _classOrigin;
    Boolean _propagated;
    QualifierList _qualifiers;
    Array<Parameter> _parameters;

    friend class ClassDeclRep;
};

PEGASUS_NAMESPACE_END

#endif /* Pegasus_MethodRep_h */
