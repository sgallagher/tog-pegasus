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
// $Log: ClassDecl.h,v $
// Revision 1.1.1.1  2001/01/14 19:50:37  mike
// Pegasus import
//
//
//END_HISTORY

#ifndef Pegasus_ClassDecl_h
#define Pegasus_ClassDecl_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/ClassDeclRep.h>

PEGASUS_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////
//
// ClassDecl
//
////////////////////////////////////////////////////////////////////////////////

class ConstClassDecl;

class PEGASUS_COMMON_LINKAGE ClassDecl
{
public:

    ClassDecl() : _rep(0)
    {

    }

    ClassDecl(const ClassDecl& x)
    {
	Inc(_rep = x._rep);
    }

    ClassDecl& operator=(const ClassDecl& x)
    {
	if (x._rep != _rep)
	{
	    Dec(_rep);
	    Inc(_rep = x._rep);
	}
	return *this;
    }

    // Throws IllegalName if className argument not legal CIM identifier.

    ClassDecl(
	const String& className, 
	const String& superClassName = String())
    {
	_rep = new ClassDeclRep(className, superClassName);
    }

    ~ClassDecl()
    {
	Dec(_rep);
    }

    Boolean isAssociation() const
    {
	_checkRep();
	return _rep->isAssociation();
    }

    Boolean isAbstract() const
    {
	_checkRep();
	return _rep->isAbstract();
    }

    const String& getClassName() const 
    { 
	_checkRep();
	return _rep->getClassName(); 
    }

    const String& getSuperClassName() const 
    { 
	_checkRep();
	return _rep->getSuperClassName(); 
    }

    // Throws IllegalName if superClassName argument not legal CIM identifier.

    void setSuperClassName(const String& superClassName)
    {
	_checkRep();
	_rep->setSuperClassName(superClassName);
    }

    // Throws AlreadyExists.

    ClassDecl& addQualifier(const Qualifier& qualifier)
    {
	_checkRep();
	_rep->addQualifier(qualifier);
	return *this;
    }

    Uint32 findQualifier(const String& name)
    {
	_checkRep();
	return _rep->findQualifier(name);
    }

    Uint32 findQualifier(const String& name) const
    {
	_checkRep();
	return _rep->findQualifier(name);
    }

    Qualifier getQualifier(Uint32 pos)
    {
	_checkRep();
	return _rep->getQualifier(pos);
    }

    ConstQualifier getQualifier(Uint32 pos) const
    {
	_checkRep();
	return _rep->getQualifier(pos);
    }

    Uint32 getQualifierCount() const
    {
	_checkRep();
	return _rep->getQualifierCount();
    }

    ClassDecl& addProperty(const Property& x)
    {
	_checkRep();
	_rep->addProperty(x);
	return *this;
    }

    void removeProperty(Uint32 pos)
    {
	_checkRep();
	_rep->removeProperty(pos);
    }

    Uint32 findProperty(const String& name)
    {
	_checkRep();
	return _rep->findProperty(name);
    }

    Uint32 findProperty(const String& name) const
    {
	_checkRep();
	return _rep->findProperty(name);
    }

    Property getProperty(Uint32 pos)
    {
	_checkRep();
	return _rep->getProperty(pos);
    }

    ConstProperty getProperty(Uint32 pos) const
    {
	_checkRep();
	return _rep->getProperty(pos);
    }

    Uint32 getPropertyCount() const
    {
	_checkRep();
	return _rep->getPropertyCount();
    }

    ClassDecl& addMethod(const Method& x)
    {
	_checkRep();
	_rep->addMethod(x);
	return *this;
    }

    Uint32 findMethod(const String& name)
    {
	_checkRep();
	return _rep->findMethod(name);
    }

    Uint32 findMethod(const String& name) const
    {
	_checkRep();
	return _rep->findMethod(name);
    }

    Method getMethod(Uint32 pos)
    {
	_checkRep();
	return _rep->getMethod(pos);
    }

    ConstMethod getMethod(Uint32 pos) const
    {
	_checkRep();
	return _rep->getMethod(pos);
    }

    Uint32 getMethodCount() const
    {
	_checkRep();
	return _rep->getMethodCount();
    }

    // Resolve the class: inherit any properties and qualifiers.
    // make sure the superClass really exists and is consistent with
    // this class.

    void resolve(
	DeclContext* declContext,
	const String& nameSpace)
    {
	_checkRep();
	_rep->resolve(declContext, nameSpace);
    }

    operator int() const { return _rep != 0; }

    void toXml(Array<Sint8>& out) const
    {
	_checkRep();
	_rep->toXml(out);
    }

    void print() const
    {
	_checkRep();
	_rep->print();
    }

    Boolean identical(const ConstClassDecl& x) const;

    ClassDecl clone() const
    {
	return ClassDecl(_rep->clone());
    }

private:

    ClassDecl(ClassDeclRep* rep) : _rep(rep)
    {
    }

    void _checkRep() const
    {
	if (!_rep)
	    throw UnitializedHandle();
    }

    ClassDeclRep* _rep;
    friend class ConstClassDecl;
};

////////////////////////////////////////////////////////////////////////////////
//
// ConstClassDecl
//
////////////////////////////////////////////////////////////////////////////////

class PEGASUS_COMMON_LINKAGE ConstClassDecl
{
public:

    ConstClassDecl() : _rep(0)
    {

    }

    ConstClassDecl(const ConstClassDecl& x)
    {
	Inc(_rep = x._rep);
    }

    ConstClassDecl(const ClassDecl& x)
    {
	Inc(_rep = x._rep);
    }

    ConstClassDecl& operator=(const ConstClassDecl& x)
    {
	if (x._rep != _rep)
	{
	    Dec(_rep);
	    Inc(_rep = x._rep);
	}
	return *this;
    }

    ConstClassDecl& operator=(const ClassDecl& x)
    {
	if (x._rep != _rep)
	{
	    Dec(_rep);
	    Inc(_rep = x._rep);
	}
	return *this;
    }

    // Throws IllegalName if className argument not legal CIM identifier.

    ConstClassDecl(
	const String& className, 
	const String& superClassName = String())
    {
	_rep = new ClassDeclRep(className, superClassName);
    }

    ~ConstClassDecl()
    {
	Dec(_rep);
    }

    Boolean isAssociation() const
    {
	_checkRep();
	return _rep->isAssociation();
    }

    Boolean isAbstract() const
    {
	_checkRep();
	return _rep->isAbstract();
    }

    const String& getClassName() const 
    { 
	_checkRep();
	return _rep->getClassName(); 
    }

    const String& getSuperClassName() const 
    { 
	_checkRep();
	return _rep->getSuperClassName(); 
    }

    Uint32 findQualifier(const String& name) const
    {
	_checkRep();
	return _rep->findQualifier(name);
    }

    ConstQualifier getQualifier(Uint32 pos) const
    {
	_checkRep();
	return _rep->getQualifier(pos);
    }

    Uint32 getQualifierCount() const
    {
	_checkRep();
	return _rep->getQualifierCount();
    }

    Uint32 findProperty(const String& name) const
    {
	_checkRep();
	return _rep->findProperty(name);
    }

    ConstProperty getProperty(Uint32 pos) const
    {
	_checkRep();
	return _rep->getProperty(pos);
    }

    Uint32 getPropertyCount() const
    {
	_checkRep();
	return _rep->getPropertyCount();
    }

    Uint32 findMethod(const String& name) const
    {
	_checkRep();
	return _rep->findMethod(name);
    }

    ConstMethod getMethod(Uint32 pos) const
    {
	_checkRep();
	return _rep->getMethod(pos);
    }

    Uint32 getMethodCount() const
    {
	_checkRep();
	return _rep->getMethodCount();
    }

    operator int() const { return _rep != 0; }

    void toXml(Array<Sint8>& out) const
    {
	_checkRep();
	_rep->toXml(out);
    }

    void print() const
    {
	_checkRep();
	_rep->print();
    }

    Boolean identical(const ConstClassDecl& x) const
    {
	x._checkRep();
	_checkRep();
	return _rep->identical(x._rep);
    }

    ConstClassDecl clone() const
    {
	return ClassDecl(_rep->clone());
    }

private:

    ConstClassDecl(ClassDeclRep* rep) : _rep(rep)
    {
    }

    void _checkRep() const
    {
	if (!_rep)
	    throw UnitializedHandle();
    }

    ClassDeclRep* _rep;

    friend class ClassDeclRep;
    friend class ClassDecl;
    friend class InstanceDeclRep;
};

PEGASUS_NAMESPACE_END

#endif /* Pegasus_ClassDecl_h */
