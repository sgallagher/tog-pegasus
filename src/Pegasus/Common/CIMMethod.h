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

#ifndef Pegasus_Method_h
#define Pegasus_Method_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/CIMMethodRep.h>

PEGASUS_NAMESPACE_BEGIN

class CIMConstMethod;

/** Class CIMMethod - This class defines the operations associated with
    manipulation of the Pegasus implementation of the CIM CIMMethod. Within
    this class, methods are provides for creation, deletion, and manipulation
    of method declarations.

    // ATTN: remove the classOrigin and propagated parameters.
*/
class PEGASUS_COMMON_LINKAGE CIMMethod
{
public:

    /** Creates and instantiates a CIM method. */
    CIMMethod() : _rep(0)
    {

    }

    /** Creates and instantiates a CIM method from another method instance
	@return pointer to the new method instance
    */
    CIMMethod(const CIMMethod& x)
    {
	Inc(_rep = x._rep);
    }

    /** Assignment operator */
    CIMMethod& operator=(const CIMMethod& x)
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
    CIMMethod(
	const String& name,
	CIMType type,
	const String& classOrigin = String(),
	Boolean propagated = false)
    {
	_rep = new CIMMethodRep(name, type, classOrigin, propagated);
    }

    /** Desctructor. */
    ~CIMMethod()
    {
	Dec(_rep);
    }

    /** CIMMethod getName - Gets the name of the method
	@return String with the name of the method
    */
    const String& getName() const
    {
	_checkRep();
	return _rep->getName();
    }

    /** CIMMethod setName - Set the method name
	@param name
	@exception IllegalName if name argument not legal CIM identifier.
    */
    void setName(const String& name)
    {
	_checkRep();
	_rep->setName(name);
    }

    /** CIMMethod getType - gets the method type
	@return The CIM method type for this method.
    */
    CIMType getType() const
    {
	_checkRep();
	return _rep->getType();
    }

    /** CIMMethod setType - Sets the method type to the specified CIM method
	type as defined in CIMType /Ref{TYPE}
    */
    void setType(CIMType type)
    {
	_checkRep();
	_rep->setType(type);
    }

    /** CIMMethod getClassOrigin - Returns the class in which this method
	was defined.
	@return ATTN:
    */
    const String& getClassOrigin() const
    {
	_checkRep();
	return _rep->getClassOrigin();
    }

    /** CIMMethod setClassOrigin - ATTN: */
    void setClassOrigin(const String& classOrigin)
    {
	_checkRep();
	_rep->setClassOrigin(classOrigin);
    }

    /** method getPropagated - ATTN: */
    Boolean getPropagated() const
    {
	_checkRep();
	return _rep->getPropagated();
    }

    /** method setPropagated - ATTN: */
    void setPropagated(Boolean propagated)
    {
	_checkRep();
	_rep->setPropagated(propagated);
    }

    /** CIMMethod addQualifier - Adds a Qualifier to the method object.
	@param CIMQualifier to be added
	@return Throws AlreadyExists excetpion if the qualifier already exists
	in the method
	@exception AlreadyExists exception
    */
    CIMMethod& addQualifier(const CIMQualifier& x)
    {
	_checkRep();
	_rep->addQualifier(x);
	return *this;
    }

    /** CIMMethod findQualifier - returns the position of the qualifier with
	the given name.
	@param name Name of qualifier to be found.
	@return index of the parameter if found; otherwise PEG_NOT_FOUND.
    */
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

    /** existsQualifier - returns the position of the qualifier with
	the given name.
	@param name Name of qualifier to be found.
	@return index of the parameter if found; otherwise PEG_NOT_FOUND.
    */
    Boolean existsQualifier(const String& name)
    {
	_checkRep();
	return _rep->existsQualifier(name);
    }

    Boolean existsQualifier(const String& name) const
    {
	_checkRep();
	return _rep->existsQualifier(name);
    }


    /** CIMMethod getQualifier - Gets the CIMQualifier defined by the index
	input as a parameter.
	@param Index of the qualifier requested.
	@return CIMQualifier object or exception
	@exception OutOfBounds exception if the index is outside the range of
	parameters available from the CIMMethod.
    */
    CIMQualifier getQualifier(Uint32 pos)
    {
	_checkRep();
	return _rep->getQualifier(pos);
    }

    CIMConstQualifier getQualifier(Uint32 pos) const
    {
	_checkRep();
	return _rep->getQualifier(pos);
    }

    /** removeQualifier - Removes the CIMQualifier defined by the
	position input as a parameter.
	@param Position of the qualifier requested.
	@return CIMQualifier object or exception
	@exception OutOfBounds exception if the index is outside the range of
	parameters available from the CIMMethod.
    */
    void removeQualifier(Uint32 pos)
    {
	_checkRep();
	_rep->removeQualifier(pos);
    }


    /** CIMMethod getQualifierCount - Returns the number of Qualifiers attached
	to this method.
	@return integer representing number of Qualifiers.
    */
    Uint32 getQualifierCount() const
    {
	_checkRep();
	return _rep->getQualifierCount();
    }

    /** CIMMethod addParameter - Adds the parameter defined by the input
	to the CIMMethod
    */
    CIMMethod& addParameter(const CIMParameter& x)
    {
	_checkRep();
	_rep->addParameter(x);
	return *this;
    }

    /** CIMMethod findParameter - Finds the parameter whose name is given
	by the name parameter.
	@param name Name of parameter to be found.
	@return index of the parameter if found; otherwise PEG_NOT_FOUND.
    */
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

    /** CIMMethod getParameter - ATTN: */
    CIMParameter getParameter(Uint32 pos)
    {
	_checkRep();
	return _rep->getParameter(pos);
    }

    /** CIMMethod getParameter - Gets the parameter defined by the index
	input as a parameter.
	@param index for the parameter to be returned.
	@return CIMParameter requested.
	@Exception OutOfBounds exception is thrown if the index is outside the
	range of available parameters
    */
    CIMConstParameter getParameter(Uint32 pos) const
    {
	_checkRep();
	return _rep->getParameter(pos);
    }

    /** CIMMethod getParameterCount - Gets the count of the numbeer of
	Parameters attached to the CIMMethod.
	@retrun - count of the number of parameters attached to the CIMMethod.
    */
    Uint32 getParameterCount() const
    {
	_checkRep();
	return _rep->getParameterCount();
    }

    /** method resolve - ATTN: */
    void resolve(
	DeclContext* declContext,
	const String& nameSpace,
	const CIMConstMethod& method)
    {
	_checkRep();
	_rep->resolve(declContext, nameSpace, method);
    }

    /** CIMMethod resolve */
    void resolve(
	DeclContext* declContext,
	const String& nameSpace)
    {
	_checkRep();
	_rep->resolve(declContext, nameSpace);
    }

    /** Returns zero if CIMMethod refers to a null pointer */
    operator int() const
    {
	return _rep != 0;
    }

    /** method toXML - placing XML encoding of this object into out arguemnt. */
    void toXml(Array<Sint8>& out) const
    {
	_checkRep();
	_rep->toXml(out);
    }

    /** method print - prints this method (in CIM encoded form). */
    void print(PEGASUS_STD(ostream) &o=PEGASUS_STD(cout)) const
    {
	_checkRep();
	_rep->print(o);
    }

    /** CIMMethod identical - Returns true if this method is identical to the
	one given by the argument x.
    */
    Boolean identical(const CIMConstMethod& x) const;

    /** CIMMethod clone - makes a distinct replica of this method */
    CIMMethod clone() const
    {
	return CIMMethod(_rep->clone());
    }

private:

    CIMMethod(CIMMethodRep* rep) : _rep(rep)
    {
    }

    PEGASUS_EXPLICIT CIMMethod(const CIMConstMethod& x);

    void _checkRep() const
    {
	if (!_rep)
	    ThrowUnitializedHandle();
    }

    CIMMethodRep* _rep;
    friend class CIMConstMethod;
    friend class CIMClassRep;
};

class PEGASUS_COMMON_LINKAGE CIMConstMethod
{
public:

    CIMConstMethod() : _rep(0)
    {

    }

    CIMConstMethod(const CIMConstMethod& x)
    {
	Inc(_rep = x._rep);
    }

    CIMConstMethod(const CIMMethod& x)
    {
	Inc(_rep = x._rep);
    }

    CIMConstMethod& operator=(const CIMConstMethod& x)
    {
	if (x._rep != _rep)
	{
	    Dec(_rep);
	    Inc(_rep = x._rep);
	}
	return *this;
    }

    CIMConstMethod& operator=(const CIMMethod& x)
    {
	if (x._rep != _rep)
	{
	    Dec(_rep);
	    Inc(_rep = x._rep);
	}
	return *this;
    }

    // Throws IllegalName if name argument not legal CIM identifier.

    CIMConstMethod(
	const String& name,
	CIMType type,
	const String& classOrigin = String(),
	Boolean propagated = false)
    {
	_rep = new CIMMethodRep(name, type, classOrigin, propagated);
    }

    ~CIMConstMethod()
    {
	Dec(_rep);
    }

    const String& getName() const
    {
	_checkRep();
	return _rep->getName();
    }

    CIMType getType() const
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

    CIMConstParameter getParameter(Uint32 pos) const
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

    void print(PEGASUS_STD(ostream) &o=PEGASUS_STD(cout)) const
    {
	_checkRep();
	_rep->print(o);
    }

    Boolean identical(const CIMConstMethod& x) const
    {
	x._checkRep();
	_checkRep();
	return _rep->identical(x._rep);
    }

    CIMMethod clone() const
    {
	return CIMMethod(_rep->clone());
    }

private:

    void _checkRep() const
    {
	if (!_rep)
	    ThrowUnitializedHandle();
    }

    CIMMethodRep* _rep;

    friend class CIMMethod;
    friend class CIMMethodRep;
};

#define PEGASUS_ARRAY_T CIMMethod
# include "ArrayInter.h"
#undef PEGASUS_ARRAY_T

PEGASUS_NAMESPACE_END

#endif /* Pegasus_Method_h */
