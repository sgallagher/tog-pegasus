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
// $Log: Qualifier.h,v $
// Revision 1.4  2001/01/30 23:39:00  karl
// Add doc++ Documentation to header files
//
// Revision 1.3  2001/01/23 01:25:35  mike
// Reworked resolve scheme.
//
// Revision 1.2  2001/01/15 04:31:44  mike
// worked on resolve scheme
//
// Revision 1.1.1.1  2001/01/14 19:53:06  mike
// Pegasus import
//
//
//END_HISTORY

/*
 Qualifier.h - Defines the CIM qualifiers class.
	This class represents a CIM qualifiers. It is almost identical to
	QualifierDecl except that it has no scope member.

*/

#ifndef Pegasus_Qualifier_h
#define Pegasus_Qualifier_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/QualifierRep.h>

PEGASUS_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////
//
// Qualifier
//
////////////////////////////////////////////////////////////////////////////////

class ConstQualifier;
class ClassDeclRep;
/** Class Qualifier - This class defines the Pegasus implementation of the CIM 
Qualifier \Ref{QUALIFIER}.
*/ 
class PEGASUS_COMMON_LINKAGE Qualifier
{
public:

    /** Constructor instantiates a CIM qualifier with empty name value 
    fields.Constructor 
    @return instantiated empty qualifier object
   */
    Qualifier() : _rep(0)
    {

    }
    /** Constructor - instantiates a CIM qualifier object from another 
    qualifier object. 
    @param CIM Qualifier object
    ATTN: What is differenc from clone?
    @return - Instantiated qualifier object
 */
    Qualifier(const Qualifier& x) 
    {
	Inc(_rep = x._rep); 
    }
    /** Constructor - Instantiates a CIM qualifier object with the parameters 
    defined on input.
    @param String representing Name for the new qualifier
    @param value
    @param flavor - ATTN:
    @param propoagated - ATTN:
    @return -Returns the instantiated qualifier object or throws an exception 
    if the name argument is illegal
    
    @exception Throws IllegalName if name argument not legal CIM 
    identifier.
     */
    Qualifier(
	const String& name, 
	const Value& value, 
	Uint32 flavor = Flavor::DEFAULTS,
	Boolean propagated = false)
    {
	_rep = new QualifierRep(name, value, flavor, propagated);
    }
    /// destructor
    ~Qualifier()
    {
	Dec(_rep);
    }
    /// operator
    Qualifier& operator=(const Qualifier& x)
    {
	if (x._rep != _rep)
	{
	    Dec(_rep);
	    Inc(_rep = x._rep);
	}

	return *this;
    }

    /**	Method
    
    */
    const String& getName() const 
    { 
	_checkRep();
	return _rep->getName(); 
    }

    /**	Method
     Throws IllegalName if name argument not legal CIM  identifier.
    */
    void setName(const String& name) 
    { 
	_checkRep();
	_rep->setName(name); 
    }

    /**	Method
    
    */
    Type getType() const 
    { 
	_checkRep();
	return _rep->getType(); 
    }

    /**	Method
    
    */
    Boolean isArray() const 
    {
	_checkRep();
	return _rep->isArray();
    }

    /**	Method
    
    */
    const Value& getValue() const 
    { 
	_checkRep();
	return _rep->getValue(); 
    }

    /**	Method
    
    */
    void setValue(const Value& value) 
    { 
	_checkRep();
	_rep->setValue(value); 
    }

    /**	Method
    
    */
    Uint32 getFlavor() const 
    {
	_checkRep();
	return _rep->getFlavor();
    }

    /**	Method

   */
    const Uint32 getPropagated() const 
    {
	_checkRep();
	return _rep->getPropagated(); 
    }

    /**	Method
    
    */
    void setPropagated(Boolean propagated) 
    {
	_checkRep();
	_rep->setPropagated(propagated); 
    }

    /**	Method
    
    */
    operator int() const { return _rep != 0; }

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
    Boolean identical(const ConstQualifier& x) const;

    /**	Method
    
    */
    Qualifier clone() const
    {
	return Qualifier(_rep->clone());
    }

private:

    Qualifier(QualifierRep* rep) : _rep(rep)
    {
    }

    void _checkRep() const
    {
	if (!_rep)
	    throw UnitializedHandle();
    }

    QualifierRep* _rep;
    friend class ConstQualifier;
    friend class ClassDeclRep;
};

////////////////////////////////////////////////////////////////////////////////
//
// ConstQualifier
//
////////////////////////////////////////////////////////////////////////////////

class PEGASUS_COMMON_LINKAGE ConstQualifier
{
public:

    ConstQualifier() : _rep(0)
    {

    }

    ConstQualifier(const ConstQualifier& x) 
    {
	Inc(_rep = x._rep); 
    }

    ConstQualifier(const Qualifier& x) 
    {
	Inc(_rep = x._rep); 
    }

    // Throws IllegalName if name argument not legal CIM identifier.

    ConstQualifier(
	const String& name, 
	const Value& value, 
	Uint32 flavor = Flavor::DEFAULTS,
	Boolean propagated = false)
    {
	_rep = new QualifierRep(name, value, flavor, propagated);
    }

    ~ConstQualifier()
    {
	Dec(_rep);
    }

    ConstQualifier& operator=(const ConstQualifier& x)
    {
	if (x._rep != _rep)
	{
	    Dec(_rep);
	    Inc(_rep = x._rep);
	}

	return *this;
    }

    ConstQualifier& operator=(const Qualifier& x)
    {
	if (x._rep != _rep)
	{
	    Dec(_rep);
	    Inc(_rep = x._rep);
	}

	return *this;
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

    Boolean isArray() const 
    {
	_checkRep();
	return _rep->isArray();
    }

    const Value& getValue() const 
    { 
	_checkRep();
	return _rep->getValue(); 
    }

    const Uint32 getFlavor() const 
    {
	_checkRep();
	return _rep->getFlavor();
    }

    const Uint32 getPropagated() const 
    { 
	_checkRep();
	return _rep->getPropagated(); 
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

    Boolean identical(const ConstQualifier& x) const
    {
	x._checkRep();
	_checkRep();
	return _rep->identical(x._rep);
    }

    Qualifier clone() const
    {
	return Qualifier(_rep->clone());
    }

private:

    void _checkRep() const
    {
	if (!_rep)
	    throw UnitializedHandle();
    }

    QualifierRep* _rep;
    friend class Qualifier;
};

PEGASUS_NAMESPACE_END

#endif /* Pegasus_Qualifier_h */
