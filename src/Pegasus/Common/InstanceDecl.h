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
// $Log: InstanceDecl.h,v $
// Revision 1.3  2001/01/30 23:39:00  karl
// Add doc++ Documentation to header files
//
// Revision 1.2  2001/01/15 04:31:44  mike
// worked on resolve scheme
//
// Revision 1.1.1.1  2001/01/14 19:52:38  mike
// Pegasus import
//
//
//END_HISTORY

/*

InstanceDecl.h File defines the Class used to create, instantiate, and modify 
CIM Instances 

*/
#ifndef Pegasus_InstanceDecl_h
#define Pegasus_InstanceDecl_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/InstanceDeclRep.h>

PEGASUS_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////
//
// InstanceDecl
//
////////////////////////////////////////////////////////////////////////////////

class ConstInstanceDecl;
/** Class InstanceDecl	- The InstanceDecl class represents the instance of 
a CIM class in Pegasus. It is used manipulate instances and the 
characteristics of instances

*/ 
class PEGASUS_COMMON_LINKAGE InstanceDecl
{
public:
    /** Constructor - Create a CIM Instance object.
    @return  Instance created
    */
    InstanceDecl() : _rep(0)
    {

    }
    /** Constructor - Create a CIM Instance object from another Instance.
    @param Instance object from which the new instance is created.
    @return New instance
    @example
	ATTN:
    */
    InstanceDecl(const InstanceDecl& x)
    {
	Inc(_rep = x._rep);
    }
    /// Constructor - ATTN
    InstanceDecl& operator=(const InstanceDecl& x)
    {
	if (x._rep != _rep)
	{
	    Dec(_rep);
	    Inc(_rep = x._rep);
	}
	return *this;
    }
    /**	Constructor - Creates an Instance object with the classname
    from the inpur parameters
    @param - String className to be used with new instance object
    
    @return The new instance object
    
    Throws IllegalName if className argument not legal CIM identifier.
    ATTN: Clarify the defintion
    */
    InstanceDecl(const String& className)
    {
	_rep = new InstanceDeclRep(className);
    }
    /// Destructor
    ~InstanceDecl()
    {
	Dec(_rep);
    }
    /**	Method
    
    */
    const String& getClassName() const 
    { 
	_checkRep();
	return _rep->getClassName(); 
    }
    /**	Method addQualifier - Adds teh Qualifier object to the instance.
    Thows an exception of the Qualifier already exists in the instance
    @param Qualifier object to add to instance
    @return ATTN:
    @exception Throws AlreadyExists.
    */
    InstanceDecl& addQualifier(const Qualifier& qualifier)
    {
	_checkRep();
	_rep->addQualifier(qualifier);
	return *this;
    }
    /**	Method
    
    */

    Uint32 findQualifier(const String& name)
    {
	_checkRep();
	return _rep->findQualifier(name);
    }
    /**	Method
    
    */

    Uint32 findQualifier(const String& name) const
    {
	_checkRep();
	return _rep->findQualifier(name);
    }
    /**	Method
    
    */

    Qualifier getQualifier(Uint32 pos)
    {
	_checkRep();
	return _rep->getQualifier(pos);
    }
    /**	Method
    
    */

    ConstQualifier getQualifier(Uint32 pos) const
    {
	_checkRep();
	return _rep->getQualifier(pos);
    }
    /**	Method
    
    */

    Uint32 getQualifierCount() const
    {
	_checkRep();
	return _rep->getQualifierCount();
    }

    /**	Method
    
    */
    InstanceDecl& addProperty(const Property& x)
    {
	_checkRep();
	_rep->addProperty(x);
	return *this;
    }

    /**	Method
    
    */
    Uint32 findProperty(const String& name)
    {
	_checkRep();
	return _rep->findProperty(name);
    }

    /**	Method
    
    */
    Uint32 findProperty(const String& name) const
    {
	_checkRep();
	return _rep->findProperty(name);
    }

    /**	Method
    
    */
    Property getProperty(Uint32 pos)
    {
	_checkRep();
	return _rep->getProperty(pos);
    }

    /**	Method
    
    */
    ConstProperty getProperty(Uint32 pos) const
    {
	_checkRep();
	return _rep->getProperty(pos);
    }

    /**	Method
    
    */
    Uint32 getPropertyCount() const
    {
	_checkRep();
	return _rep->getPropertyCount();
    }

    /**	Method
    
    */
    operator int() const { return _rep != 0; }

    /**	Method
    
    */
    void resolve(DeclContext* declContext, const String& nameSpace)
    {
	_checkRep();
	_rep->resolve(declContext, nameSpace);
    }

    /**	Method

   */
    void toXml(Array<Sint8>& out) const
    {
	_checkRep();
	_rep->toXml(out);
    }

    /**	Method
    
    */
    void print() const
    {
	_checkRep();
	_rep->print();
    }

    /**	Method
    
    */
    Boolean identical(const ConstInstanceDecl& x) const;

    /**	Method
    
    */
    InstanceDecl clone() const
    {
	return InstanceDecl(_rep->clone());
    }

private:

    InstanceDecl(InstanceDeclRep* rep) : _rep(rep)
    {
    }

    void _checkRep() const
    {
	if (!_rep)
	    throw UnitializedHandle();
    }

    InstanceDeclRep* _rep;
    friend class ConstInstanceDecl;
};

////////////////////////////////////////////////////////////////////////////////
//
// ConstInstanceDecl
//
////////////////////////////////////////////////////////////////////////////////

class PEGASUS_COMMON_LINKAGE ConstInstanceDecl
{
public:

    ConstInstanceDecl() : _rep(0)
    {

    }

    ConstInstanceDecl(const ConstInstanceDecl& x)
    {
	Inc(_rep = x._rep);
    }

    ConstInstanceDecl(const InstanceDecl& x)
    {
	Inc(_rep = x._rep);
    }

    ConstInstanceDecl& operator=(const ConstInstanceDecl& x)
    {
	if (x._rep != _rep)
	{
	    Dec(_rep);
	    Inc(_rep = x._rep);
	}
	return *this;
    }

    ConstInstanceDecl& operator=(const InstanceDecl& x)
    {
	if (x._rep != _rep)
	{
	    Dec(_rep);
	    Inc(_rep = x._rep);
	}
	return *this;
    }

    // Throws IllegalName if className argument not legal CIM identifier.

    ConstInstanceDecl(const String& className)
    {
	_rep = new InstanceDeclRep(className);
    }

    ~ConstInstanceDecl()
    {
	Dec(_rep);
    }

    const String& getClassName() const 
    { 
	_checkRep();
	return _rep->getClassName(); 
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

    Boolean identical(const ConstInstanceDecl& x) const
    {
	x._checkRep();
	_checkRep();
	return _rep->identical(x._rep);
    }

    InstanceDecl clone() const
    {
	return InstanceDecl(_rep->clone());
    }

private:

    void _checkRep() const
    {
	if (!_rep)
	    throw UnitializedHandle();
    }

    InstanceDeclRep* _rep;
    friend class InstanceDecl;
};

PEGASUS_NAMESPACE_END

#endif /* Pegasus_InstanceDecl_h */
