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
// $Log: InstanceDeclRep.h,v $
// Revision 1.1  2001/01/14 19:52:40  mike
// Initial revision
//
//
//END_HISTORY

#ifndef Pegasus_InstanceDeclRep_h
#define Pegasus_InstanceDeclRep_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/Exception.h>
#include <Pegasus/Common/String.h>
#include <Pegasus/Common/Qualifier.h>
#include <Pegasus/Common/QualifierList.h>
#include <Pegasus/Common/Property.h>

PEGASUS_NAMESPACE_BEGIN

class DeclContext;
class InstanceDecl;
class ConstInstanceDecl;

class PEGASUS_COMMON_LINKAGE InstanceDeclRep : public Sharable
{
public:
    
    InstanceDeclRep(const String& className);

    ~InstanceDeclRep();

    const String& getClassName() const { return _className; }

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

    void addProperty(const Property& x);

    Uint32 findProperty(const String& name);

    Uint32 findProperty(const String& name) const
    {
	return ((InstanceDeclRep*)this)->findProperty(name);
    }

    Property getProperty(Uint32 pos);

    ConstProperty getProperty(Uint32 pos) const
    {
	return ((InstanceDeclRep*)this)->getProperty(pos);
    }

    Uint32 getPropertyCount() const;

    void resolve(DeclContext* declContext, const String& nameSpace);

    void toXml(Array<Sint8>& out) const;

    void print() const;

    Boolean identical(const InstanceDeclRep* x) const;

    InstanceDeclRep* clone() const
    {
	return new InstanceDeclRep(*this);
    }

private:

    InstanceDeclRep();

    InstanceDeclRep(const InstanceDeclRep& x);

    InstanceDeclRep& operator=(const InstanceDeclRep& x);

    String _className;
    QualifierList _qualifiers;
    Array<Property> _properties;
    Boolean _resolved;

    friend class InstanceDecl;
};

PEGASUS_NAMESPACE_END

#endif /* Pegasus_InstanceDeclRep_h */
