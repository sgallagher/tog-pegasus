//%/////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001 The Open group, BMC Software, Tivoli Systems, IBM
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to 
// deal in the Software without restriction, including without limitation the 
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or 
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// THE ABOVE COPYRIGHT NOTICE AND THIS PERMISSION NOTICE SHALL BE INCLUDED IN 
// ALL COPIES OR SUBSTANTIAL PORTIONS OF THE SOFTWARE. THE SOFTWARE IS PROVIDED
// "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT
// LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR 
// PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT 
// HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN 
// ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
// WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
//
//==============================================================================
//
// Author: Mike Brasher (mbrasher@bmc.com)
//
// Modified By:
//
//%/////////////////////////////////////////////////////////////////////////////

/*
 CIMQualifier.h - Defines the CIM qualifiers class.
	This class represents a CIM qualifiers. It is almost identical to
	CIMQualifierDecl except that it has no scope member.

*/

#ifndef Pegasus_Qualifier_h
#define Pegasus_Qualifier_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/CIMQualifierRep.h>

PEGASUS_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////
//
// CIMQualifier
//
////////////////////////////////////////////////////////////////////////////////

class CIMConstQualifier;
class CIMClassRep;

/** Class CIMQualifier - This class defines the Pegasus implementation of the 
    CIM CIMQualifier \Ref{QUALIFIER}.
*/
class PEGASUS_COMMON_LINKAGE CIMQualifier
{
public:

    /** Constructor instantiates a CIM qualifier with empty name value 
	fields.Constructor 
	@return instantiated empty qualifier object
    */
    CIMQualifier() : _rep(0)
    {

    }

    /** Constructor - instantiates a CIM qualifier object from another 
    qualifier object. 
    @param CIM CIMQualifier object
    ATTN: What is differenc from clone?
    @return - Instantiated qualifier object
 */
    CIMQualifier(const CIMQualifier& x) 
    {
	Inc(_rep = x._rep); 
    }
    /** Constructor - Instantiates a CIM qualifier object with the parameters 
    defined on input.
    @param String representing CIMName for the new qualifier
    @param value
    @param flavor - ATTN:
    @param propoagated - ATTN:
    @return -Returns the instantiated qualifier object or throws an exception 
    if the name argument is illegal
    
    @exception Throws IllegalName if name argument not legal CIM 
    identifier.
     */
    CIMQualifier(
	const String& name, 
	const CIMValue& value, 
	Uint32 flavor = CIMFlavor::DEFAULTS,
	Boolean propagated = false)
    {
	_rep = new CIMQualifierRep(name, value, flavor, propagated);
    }
    /// destructor
    ~CIMQualifier()
    {
	Dec(_rep);
    }
    /// operator
    CIMQualifier& operator=(const CIMQualifier& x)
    {
	if (x._rep != _rep)
	{
	    Dec(_rep);
	    Inc(_rep = x._rep);
	}

	return *this;
    }

    /**	CIMMethod
    
    */
    const String& getName() const 
    { 
	_checkRep();
	return _rep->getName(); 
    }

    /**	CIMMethod
     Throws IllegalName if name argument not legal CIM  identifier.
    */
    void setName(const String& name) 
    { 
	_checkRep();
	_rep->setName(name); 
    }

    /**	CIMMethod
    
    */
    CIMType getType() const 
    { 
	_checkRep();
	return _rep->getType(); 
    }

    /**	CIMMethod
    
    */
    Boolean isArray() const 
    {
	_checkRep();
	return _rep->isArray();
    }

    /**	CIMMethod
    
    */
    const CIMValue& getValue() const 
    { 
	_checkRep();
	return _rep->getValue(); 
    }

    /**	CIMMethod
    
    */
    void setValue(const CIMValue& value) 
    { 
	_checkRep();
	_rep->setValue(value); 
    }

    /**	CIMMethod
    
    */
    Uint32 getFlavor() const 
    {
	_checkRep();
	return _rep->getFlavor();
    }

    /**	CIMMethod

   */
    const Uint32 getPropagated() const 
    {
	_checkRep();
	return _rep->getPropagated(); 
    }

    /**	CIMMethod
    
    */
    void setPropagated(Boolean propagated) 
    {
	_checkRep();
	_rep->setPropagated(propagated); 
    }

    /**	CIMMethod
    
    */
    operator int() const { return _rep != 0; }

    /**	CIMMethod
    
    */
    void toXml(Array<Sint8>& out) const
    {
	_checkRep();
	_rep->toXml(out);
    }

    /**	CIMMethod

   */
    void print(PEGASUS_STD(ostream) &o=PEGASUS_STD(cout)) const
    {
	_checkRep();
	_rep->print(o);
    }

    /**	CIMMethod

   */
    Boolean identical(const CIMConstQualifier& x) const;

    /**	CIMMethod
    
    */
    CIMQualifier clone() const
    {
	return CIMQualifier(_rep->clone());
    }

private:

    CIMQualifier(CIMQualifierRep* rep) : _rep(rep)
    {
    }

    void _checkRep() const
    {
	if (!_rep)
	    ThrowUnitializedHandle();
    }

    CIMQualifierRep* _rep;
    friend class CIMConstQualifier;
    friend class CIMClassRep;
};

////////////////////////////////////////////////////////////////////////////////
//
// CIMConstQualifier
//
////////////////////////////////////////////////////////////////////////////////

class PEGASUS_COMMON_LINKAGE CIMConstQualifier
{
public:

    CIMConstQualifier() : _rep(0)
    {

    }

    CIMConstQualifier(const CIMConstQualifier& x) 
    {
	Inc(_rep = x._rep); 
    }

    CIMConstQualifier(const CIMQualifier& x) 
    {
	Inc(_rep = x._rep); 
    }

    // Throws IllegalName if name argument not legal CIM identifier.

    CIMConstQualifier(
	const String& name, 
	const CIMValue& value, 
	Uint32 flavor = CIMFlavor::DEFAULTS,
	Boolean propagated = false)
    {
	_rep = new CIMQualifierRep(name, value, flavor, propagated);
    }

    ~CIMConstQualifier()
    {
	Dec(_rep);
    }

    CIMConstQualifier& operator=(const CIMConstQualifier& x)
    {
	if (x._rep != _rep)
	{
	    Dec(_rep);
	    Inc(_rep = x._rep);
	}

	return *this;
    }

    CIMConstQualifier& operator=(const CIMQualifier& x)
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

    CIMType getType() const 
    { 
	_checkRep();
	return _rep->getType(); 
    }

    Boolean isArray() const 
    {
	_checkRep();
	return _rep->isArray();
    }

    const CIMValue& getValue() const 
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

    void print(PEGASUS_STD(ostream) &o=PEGASUS_STD(cout)) const
    {
	_checkRep();
	_rep->print(o);
    }

    Boolean identical(const CIMConstQualifier& x) const
    {
	x._checkRep();
	_checkRep();
	return _rep->identical(x._rep);
    }

    CIMQualifier clone() const
    {
	return CIMQualifier(_rep->clone());
    }

private:

    void _checkRep() const
    {
	if (!_rep)
	    ThrowUnitializedHandle();
    }

    CIMQualifierRep* _rep;
    friend class CIMQualifier;
};

PEGASUS_MEMORY_FUNCTIONS(CIMQualifier)
PEGASUS_MEMORY_FUNCTIONS(CIMQualifier*)

#define PEGASUS_ARRAY_T CIMQualifier
# include "ArrayInter.h"
#undef PEGASUS_ARRAY_T

PEGASUS_NAMESPACE_END

#endif /* Pegasus_Qualifier_h */
