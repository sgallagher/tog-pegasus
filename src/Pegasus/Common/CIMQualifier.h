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
// Modified By:	Karl Schopmeyer (k.schopmeyer@opengroup.org)
//
//%/////////////////////////////////////////////////////////////////////////////

/*
 CIMQualifier.h - Defines the CIM qualifiers class.

*/

#ifndef Pegasus_Qualifier_h
#define Pegasus_Qualifier_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/CIMQualifierRep.h>
#include <Pegasus/Common/CIMFlavor.h>

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
	This class represents a CIM qualifiers. It is almost identical to
	CIMQualifierDecl except that it has no scope member. \Ref{CIMQualifierDecl}
    This includes functions to create qualifiers and manipulate/test
    the individual components of the CIMQualifier.
    CIMQualifiers are accumulated into lists for use in CIMClasses and CIM Properties
    using the CIMQualifierList Class and its functions. \Ref{CIMQualifierList}
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
    @param flavor - Flavor defined for this qualifier definition. Default for this
	parameter is CIMFlavor::NONE.
    @param propoagated - Boolean defining whether this is a propagated qualifier.
	This is an optional parameter with default = false
    @return -Returns the instantiated qualifier object or throws an exception 
    if the name argument is illegal
    
    @exception Throws IllegalName if name argument not legal CIM 
    identifier.
     */
    CIMQualifier(
	const String& name, 
	const CIMValue& value, 
	Uint32 flavor = CIMFlavor::NONE,
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

    /**	getName - Returns the name field from the qualifier
    @return String containing the qualifier name.
    
    */
    const String& getName() const 
    { 
	_checkRep();
	return _rep->getName(); 
    }

    /**	setName - Sets the qualifier name field in the qualifier object.
	@param name - String containing the name for the qualifier
	@exception Throws IllegalName if name argument not legal CIM  identifier.
    */
    void setName(const String& name) 
    { 
	_checkRep();
	_rep->setName(name); 
    }

    /** getType - Gets the type field from the qualifier object.
    @return CIMType containing the type for this qualifier /Ref{CIMType}.
    
    */
    CIMType getType() const 
    { 
	_checkRep();
	return _rep->getType(); 
    }

    /**	isArray - Returns true if the qualifier is an array
	@return Boolean true if array qualifier.
    */
    Boolean isArray() const 
    {
	_checkRep();
	return _rep->isArray();
    }

    /**	getValue - Gets the value component of the qualifier object
	@return CIMValue containing the value component
    
    */
    const CIMValue& getValue() const 
    { 
	_checkRep();
	return _rep->getValue(); 
    }

    /**	setValue - Puts a CIMValue object into a CIMQualifier object
	@param value - The CIMValue object to install
    
    */
    void setValue(const CIMValue& value) 
    { 
	_checkRep();
	_rep->setValue(value); 
    }

    /* setFlavor - Sets the bits defined on input into the Flavor variable
	    for the Qualifier Object.
	    @param flavor - Uint32 defines the flavor bits to be set.
    */
    void setFlavor(Uint32 flavor) 
    {
		_checkRep();
		_rep->setFlavor(flavor);
    }
    /* unsetFlavor - Resets the bits defined for the flavor 
	    for the Qualifier Object with the input.
	    @param flavor - Uint32 defines the flavor bits to be set.
    */
    void unsetFlavor(Uint32 flavor) 
    {
		_checkRep();
		_rep->unsetFlavor(flavor);
    }

    /**	getFlavor - Gets the Flavor field from a Qualifier
	@return - Uint32 with the Flavor flags that can be tested
	against the CIMFlavor constants.
    */
    Uint32 getFlavor() const 
    {
	_checkRep();
	return _rep->getFlavor();
    }
    /**	isFlavor - Boolean function that determines if particular flavor
	flags are set in the flavor variable of a qualifier.
	@param flavor - The flavor bits to test.
	Return True if the defined flavor is set.
	<pre>
	if (q.isFlavor(CIMType::TOSUBCLASS)
		do something based on TOSUBCLASS being true
	</pre>
		
    */
    Boolean isFlavor(Uint32 flavor) const
    {
	    return _rep->isFlavor(flavor);
    }
    /* resolveFlavor - Function used only in object creation to
	    resolve the combination of a qualifer flavor input and
	    the corresponding inherited flavor from declaration or
	    superclass and set the current qualifier to that
	    definition.	The functions changes the current flavor based
	    on the characteristics of the inheritance.
	    @param inheritedFlavor - The flavor inherited from higher level
	    @param inherited - True if inherited from definition. False if this
	    is definition that inherits from the declaration
    */
    void resolveFlavor(Uint32 inheritedFlavor, Boolean inherited)
    {
	    _checkRep();
	    _rep->resolveFlavor(inheritedFlavor, inherited);
    }
    /**	getPropagated returns the propagated indicator
	@return Uint32 - TBD

    */
    const Uint32 getPropagated() const 
    {
	_checkRep();
	return _rep->getPropagated(); 
    }

    /**	setPropagated - Sets the Propagated flag for the object.
    
    */
    void setPropagated(Boolean propagated) 
    {
	_checkRep();
	_rep->setPropagated(propagated); 
    }

    /**	CIMMethod
    
    */
    operator int() const { return _rep != 0; }

    /**	toXml- Converts the CIMQualifier object to XML.
    @param out The array where the XML output is to be stored.
    
    */
    void toXml(Array<Sint8>& out) const
    {
	_checkRep();
	_rep->toXml(out);
    }

    /**	print - Converts the CIMQualifier object to XML and 
        sends it to cout.
	@SeeAlso toXML

    */
    void print(PEGASUS_STD(ostream) &o=PEGASUS_STD(cout)) const
    {
	_checkRep();
	_rep->print(o);
    }

    /**	toMof- Converts the CIMQualifier object to MOF.
	@param out The Array where the MOF output is stored.
	Note that the result does not have a zero terminator.
    */
    void toMof(Array<Sint8>& out) const
    {
	_checkRep();
	_rep->toMof(out);
    }

    /**	printMOF - Converts the CIMQualifier object to XML and 
        sends it the stream defined.
	@param o Output stream for the MOF. The default is cout.
   */
    void printMof(PEGASUS_STD(ostream) &o=PEGASUS_STD(cout)) const
    {
	_checkRep();
	_rep->printMof(o);
    }


    /**	identical - compares two CIMQualifier objects.
        @return - True if the objects are identical.
   */
    Boolean identical(const CIMConstQualifier& x) const;

    /**	clone Creates an exact copy of the qualifier and returns the
	new object.
	@return CIMQualifier New Qualifier object.
    
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

	Boolean isFlavor(Uint32 flavor) const
	{
		return ((getFlavor() & flavor) !=0);
	}
	
	Boolean isFlavorToSubclass() const
	{
		return ((getFlavor() & CIMFlavor::TOSUBCLASS) != 0);
	}

	Boolean isFlavorToInstance() const
	{
		return ((getFlavor() & CIMFlavor::TOINSTANCE) != 0);
	}

	Boolean isFlavorOverridable() const
	{
		return ((getFlavor() & CIMFlavor::OVERRIDABLE) != 0);
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
