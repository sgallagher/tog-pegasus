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
// $Log: Method.h,v $
// Revision 1.3  2001/01/28 19:07:07  karl
// add DOC++ comments
//
// Revision 1.2  2001/01/15 04:31:44  mike
// worked on resolve scheme
//
// Revision 1.1.1.1  2001/01/14 19:52:57  mike
// Pegasus import
//
//
//END_HISTORY

/*
 Method.h - This header file defines the method class.
*/

#ifndef Pegasus_Method_h
#define Pegasus_Method_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/MethodRep.h>

PEGASUS_NAMESPACE_BEGIN


class ConstMethod;

/** Class Method - Defines a CIM method
*/

class PEGASUS_COMMON_LINKAGE Method
{
public:
    /// Creates and instantiates a CIM method.
    Method() : _rep(0)
    {

    }
    /** Creates and instantiates a CIM method from another method instance
    @return pointer to the new method 
    */
    Method(const Method& x)
    {
	Inc(_rep = x._rep);
    }
    ///
    Method& operator=(const Method& x)
    {
	if (x._rep != _rep)
	{
	    Dec(_rep);
	    Inc(_rep = x._rep);
	}
	return *this;
    }
    /**	 Creates a CIM method with the specified name, type, and classOrigin
    @param name for the method
    @param type ATTN
    @param classOrigin
    @param propagated
    @return  Throws IllegalName if name argument not legal CIM identifier. 
    */
    Method(
	const String& name, 
	Type type,
	const String& classOrigin = String(),
	Boolean propagated = false)
    {
	_rep = new MethodRep(name, type, classOrigin, propagated);
    }
    /// Method desctructor
    ~Method()
    {
	Dec(_rep);
    }
    /** method getName - gets the name of the method
    @return - ATTN
    */
    const String& getName() const 
    { 
	_checkRep();
	return _rep->getName(); 
    }

    /** Method setName - Set the method name
    @parm name ATTN (make reference to name defintion here)
    @return Throws IllegalName if name argument not legal CIM identifier.
    */
    void setName(const String& name)
    {
	_checkRep();
	_rep->setName(name);
    }
    /** method getType - gets the method type 
    @return The CIM method type for this method.
    */

    Type getType() const 
    {
	_checkRep();
	return _rep->getType();
    }
    /** method setType - Sets the method type to the specified CIM method 
    type as defined in Type /Ref{TYPE}
    */
    void setType(Type type)
    { 
	_checkRep();
	_rep->setType(type);
    }
    /** method getClassOrigin - Returns the class in which this method
     was defined.
    @return ATTN:
    */
    const String& getClassOrigin() const
    {
	_checkRep();
	return _rep->getClassOrigin();
    }
    /** method setClassOrigin - ATTN:
    */
    void setClassOrigin(const String& classOrigin)
    {
	_checkRep();
	_rep->setClassOrigin(classOrigin);
    }
    /// method getPropagated - ATTN:
    Boolean getPropagated() const 
    { 
	_checkRep();
	return _rep->getPropagated();
    }
    /// method setPropagated - ATTN:
    void setPropagated(Boolean propagated) 
    { 
	_checkRep();
	_rep->setPropagated(propagated);
    }
    /** Method addQualifier - 
    @parm Qualifier to add
    @return Throws AlreadyExists excetpion if the qualifier already exists in 
    the method
    */
    Method& addQualifier(const Qualifier& x)
    {
	_checkRep();
	_rep->addQualifier(x);
	return *this;
    }
    /// mehtod findQualifier - TBD
    Uint32 findQualifier(const String& name)
    {
	_checkRep();
	return _rep->findQualifier(name);
    }
    /// Method findQualifier - ATTN
    Uint32 findQualifier(const String& name) const
    {
	_checkRep();
	return _rep->findQualifier(name);
    }
    /// method getQualifier - ATTN:
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
	/// method getQualifierCount - ATTN:
    Uint32 getQualifierCount() const
    {
	_checkRep();
	return _rep->getQualifierCount();
    }
	/// method addParameter - ATTN:
    Method& addParameter(const Parameter& x)
    {
	_checkRep();
	_rep->addParameter(x);
	return *this;
    }
	/// method findParameter - ATTN:
    Uint32 findParameter(const String& name)
    {
	_checkRep();
	return _rep->findParameter(name);
    }

    Uint32 findParameter(const String& name) const
    {
	_checkRep();
	return _rep->findParameter(name);
    }

    Parameter getParameter(Uint32 pos)
    {
	_checkRep();
	return _rep->getParameter(pos);
    }

    ConstParameter getParameter(Uint32 pos) const
    {
	_checkRep();
	return _rep->getParameter(pos);
    }

    Uint32 getParameterCount() const
    {
	_checkRep();
	return _rep->getParameterCount();
    }
    /// method resolve - ATTN:
    void resolve(
	DeclContext* declContext, 
	const String& nameSpace,
	const ConstMethod& method)
    {
	_checkRep();
	_rep->resolve(declContext, nameSpace, method);
    }
    /// Method resolve
    void resolve(
	DeclContext* declContext,
	const String& nameSpace)
    {
	_checkRep();
	_rep->resolve(declContext, nameSpace);
    }
    /// operator
    operator int() const { return _rep != 0; }
    /// method toXML - ATTN:
    void toXml(Array<Sint8>& out) const
    {
	_checkRep();
	_rep->toXml(out);
    }
    /// method print - ATTN:
    void print() const
    {
	_checkRep();
	_rep->print();
    }
    /// Method identical - ATTN
    Boolean identical(const ConstMethod& x) const;
    /// Method clone - ATTN
    Method clone() const
    {
	return Method(_rep->clone());
    }

private:

    Method(MethodRep* rep) : _rep(rep)
    {
    }

    explicit Method(const ConstMethod& x);

    void _checkRep() const
    {
	if (!_rep)
	    throw UnitializedHandle();
    }

    MethodRep* _rep;
    friend class ConstMethod;
    friend class ClassDeclRep;
};

////////////////////////////////////////////////////////////////////////////////
//
// ConstMethod
//
////////////////////////////////////////////////////////////////////////////////

class PEGASUS_COMMON_LINKAGE ConstMethod
{
public:

    ConstMethod() : _rep(0)
    {

    }

    ConstMethod(const ConstMethod& x)
    {
	Inc(_rep = x._rep);
    }

    ConstMethod(const Method& x)
    {
	Inc(_rep = x._rep);
    }

    ConstMethod& operator=(const ConstMethod& x)
    {
	if (x._rep != _rep)
	{
	    Dec(_rep);
	    Inc(_rep = x._rep);
	}
	return *this;
    }

    ConstMethod& operator=(const Method& x)
    {
	if (x._rep != _rep)
	{
	    Dec(_rep);
	    Inc(_rep = x._rep);
	}
	return *this;
    }

    // Throws IllegalName if name argument not legal CIM identifier.

    ConstMethod(
	const String& name, 
	Type type,
	const String& classOrigin = String(),
	Boolean propagated = false)
    {
	_rep = new MethodRep(name, type, classOrigin, propagated);
    }

    ~ConstMethod()
    {
	Dec(_rep);
    }

    const String& getName() const 
    { 
	_checkRep();
	return _rep->getName(); 
    }

    Type getType() const 
    { 
	_checkRep();
	return _rep->getType();
    }

    const String& getClassOrigin() const
    {
	_checkRep();
	return _rep->getClassOrigin();
    }

    Boolean getPropagated() const 
    { 
	_checkRep();
	return _rep->getPropagated();
    }

    Uint32 findQualifier(const String& name) const
    {
	_checkRep();
	return _rep->findQualifier(name);
    }

    Uint32 getQualifier(Uint32 pos) const
    {
	_checkRep();
	return _rep->getQualifier(pos);
    }

    Uint32 getQualifierCount() const
    {
	_checkRep();
	return _rep->getQualifierCount();
    }

    Uint32 findParameter(const String& name) const
    {
	_checkRep();
	return _rep->findParameter(name);
    }

    ConstParameter getParameter(Uint32 pos) const
    {
	_checkRep();
	return _rep->getParameter(pos);
    }

    Uint32 getParameterCount() const
    {
	_checkRep();
	return _rep->getParameterCount();
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

    Boolean identical(const ConstMethod& x) const
    {
	x._checkRep();
	_checkRep();
	return _rep->identical(x._rep);
    }

    Method clone() const
    {
	return Method(_rep->clone());
    }

private:

    void _checkRep() const
    {
	if (!_rep)
	    throw UnitializedHandle();
    }

    MethodRep* _rep;

    friend class Method;
    friend class MethodRep;
};

PEGASUS_NAMESPACE_END

#endif /* Pegasus_Method_h */
