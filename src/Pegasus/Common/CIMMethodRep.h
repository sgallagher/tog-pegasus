//%/////////////////////////////////////////////////////////////////////////////
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
//==============================================================================
//
// Author: Mike Brasher (mbrasher@bmc.com)
//
// Modified By:
//
//%/////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_MethodRep_h
#define Pegasus_MethodRep_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/Exception.h>
#include <Pegasus/Common/String.h>
#include <Pegasus/Common/CIMQualifier.h>
#include <Pegasus/Common/CIMQualifierList.h>
#include <Pegasus/Common/CIMParameter.h>
#include <Pegasus/Common/Sharable.h>
#include <Pegasus/Common/Pair.h>

PEGASUS_NAMESPACE_BEGIN

class CIMMethod;
class CIMConstMethod;
class DeclContext;

class PEGASUS_COMMON_LINKAGE CIMMethodRep : public Sharable
{
public:

    CIMMethodRep(
	const String& name, 
	CIMType type,
	const String& classOrigin,
	Boolean propagated);

    ~CIMMethodRep();

    virtual const String& getName() const 
    { 
	return _name; 
    }

    void setName(const String& name);

    CIMType getType() const 
    {
	return _type; 
    }

    void setType(CIMType type)
    {
	_type = type; 

	if (type == CIMType::NONE)
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

    void addParameter(const CIMParameter& x);

    Uint32 findParameter(const String& name);

    Uint32 findParameter(const String& name) const
    {
	return ((CIMMethodRep*)this)->findParameter(name);
    }

    CIMParameter getParameter(Uint32 pos);

    CIMConstParameter getParameter(Uint32 pos) const
    {
	return ((CIMMethodRep*)this)->getParameter(pos);
    }

    Uint32 getParameterCount() const;

    void resolve(
	DeclContext* declContext, 
	const String& nameSpace,
	const CIMConstMethod& method);

    void resolve(
	DeclContext* declContext,
	const String& nameSpace);

    void toXml(Array<Sint8>& out) const;

    virtual void print(std::ostream &o=std::cout) const;

    Boolean identical(const CIMMethodRep* x) const;

    CIMMethodRep* clone() const
    {
	return new CIMMethodRep(*this);
    }

private:

    CIMMethodRep();

    CIMMethodRep(const CIMMethodRep& x);

    CIMMethodRep& operator=(const CIMMethodRep& x);

    String _name;
    CIMType _type;
    String _classOrigin;
    Boolean _propagated;
    CIMQualifierList _qualifiers;
    Array<CIMParameter> _parameters;

    friend class CIMClassRep;
};

PEGASUS_NAMESPACE_END

#endif /* Pegasus_MethodRep_h */
