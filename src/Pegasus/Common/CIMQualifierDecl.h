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

#ifndef Pegasus_QualifierDecl_h
#define Pegasus_QualifierDecl_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/CIMQualifierDeclRep.h>

PEGASUS_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////
//
// CIMQualifierDecl
//
////////////////////////////////////////////////////////////////////////////////

class CIMConstQualifierDecl;
class CIMClassRep;
/** Class CIMQualifierDecl
  This class defines the CIM Qualifier Declarations. Note that the Declarations
  are not the same as CIM Qualiifers as seen on Classes, properties, etc.
  These are the original declarations of qualifiers (i.e. input from the compiler
  qualifierDeclarations.
*/
class PEGASUS_COMMON_LINKAGE CIMQualifierDecl
{
public:
    /// Constructor - 
    CIMQualifierDecl() : _rep(0)
    {

    }
    /// Constructor - Creates a CIMQualifierDecl from another CIMQualifierDecl

    CIMQualifierDecl(const CIMQualifierDecl& x) 
    {
	Inc(_rep = x._rep); 
    }
    /** Constructor for CIMQualifierDecl. Constructs a single CIMQualifierDecl
		object.
		@param name	 Name of the Qualifier being declared
		@param value CIMValue for the qualifier
		@param scope scope of the qualifier
		@param flavor Optional defintion of the flavor for the qualifier.  
		CIMFlavor::DEFAULTS is used if no value supplied.  This is how we install
		the flavor defaults defined in the CIM specificaiton
        @param arraySize Optional integer defining the arraysize if the qualifier is an
		array type with fixed value array. The default is zero indicating that
		the qualifier declaration is not a fixed size array.
		@exceptions Throws IllegalName if name argument not legal CIM identifier.
    */
    
    CIMQualifierDecl(
	const String& name, 
	const CIMValue& value, 
	Uint32 scope,
	Uint32 flavor = CIMFlavor::DEFAULTS,
	Uint32 arraySize = 0)
    {
	_rep = new CIMQualifierDeclRep(name, value, scope, flavor, arraySize);
    }
    /// Destructor
    ~CIMQualifierDecl()
    {
	Dec(_rep);
    }
    /// Operator
    CIMQualifierDecl& operator=(const CIMQualifierDecl& x)
    {
	if (x._rep != _rep)
	{
	    Dec(_rep);
	    Inc(_rep = x._rep);
	}

	return *this;
    }
    /** CIMMethod ATTN:

    
    */
    const String& getName() const 
    { 
	_checkRep();
	return _rep->getName(); 
    }

    /** setName - Puts the name into a CIMQualifierdecl
    @param Name String containing name to be put on this qualifier.
    This must be a legal qualifier name.
    @exception - Throws IllegalName if name argument not a legal
    CIM Qualifier name
    */
    void setName(const String& name) 
    { 
	_checkRep();
	_rep->setName(name); 
    }
    /** getType - gets the Qualifier Declaration type which is the
        value type (boolean, etc. for this qualifier.
        ATTN: P3 Documentation clean up
        @return Returns the type as CIMType
    */
    CIMType getType() const 
    { 
	_checkRep();
	return _rep->getType(); 
    }
    /** isArray - test if this qualifier declaration is an array type.
    @return Boolean true if this is array type.
    ATTN: P1 KS 04/17/02 Confirm this correspond to array type or fixed array?
    */
    Boolean isArray() const 
    {
	_checkRep();
	return _rep->isArray();
    }
    /** CIMMethod
    
    */
    const CIMValue& getValue() const 
    { 
	_checkRep();
	return _rep->getValue(); 
    }
    /** CIMMethod
    
    */
    void setValue(const CIMValue& value) 
    { 
	_checkRep();
	_rep->setValue(value); 
    }
    /** CIMMethod
    
    */
    Uint32 getScope() const 
    {
	_checkRep();
	return _rep->getScope();
    }
    /** getFlavor - Gets the Flavor definition from the qualifer declaration
		constant
		@return - a Uint32 integer containing the flavor flags.  This can be
		tested against the flavor constants defined in CIMFlavor.
		@SeeAlso 

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
		return ((getFlavor() & flavor) !=0);
	}

    /** CIMMethod

    */

    Uint32 getArraySize() const 
    {
	_checkRep();
	return _rep->getArraySize();
    }
    /** CIMMethod
    
    */

    operator int() const { return _rep != 0; }
    
    /** toXml  Generates XML output for the Qualifier Declaration object.
    
    */
    void toXml(Array<Sint8>& out) const
    {
	_checkRep();
	_rep->toXml(out);
    }

    /** toMof  Generates MOF output for the Qualifier Declaration object.
    
    */
    void toMof(Array<Sint8>& out) const
    {
	_checkRep();
	_rep->toMof(out);
    }

    /** print Output the XML for the Qualifier Declaration object to stdout.
    
    */
    void print(PEGASUS_STD(ostream) &o=PEGASUS_STD(cout)) const
    {
	_checkRep();
	_rep->print(o);
    }
    /** identical Compares two qualifier declarations
    @return Returns true if they are identical
    
    */ 
    Boolean identical(const CIMConstQualifierDecl& x) const;
    /** CIMMethod
    
    */

    CIMQualifierDecl clone() const
    {
	return CIMQualifierDecl(_rep->clone());
    }

private:

    CIMQualifierDecl(CIMQualifierDeclRep* rep) : _rep(rep)
    {
    }

    void _checkRep() const
    {
	if (!_rep)
	    ThrowUnitializedHandle();
    }

    CIMQualifierDeclRep* _rep;
    friend class CIMConstQualifierDecl;
    friend class CIMClassRep;
};

////////////////////////////////////////////////////////////////////////////////
//
// CIMConstQualifierDecl
//
////////////////////////////////////////////////////////////////////////////////

class PEGASUS_COMMON_LINKAGE CIMConstQualifierDecl
{
public:
	///
    CIMConstQualifierDecl() : _rep(0)
    {

    }
	///
    CIMConstQualifierDecl(const CIMConstQualifierDecl& x) 
    {
	Inc(_rep = x._rep); 
    }
	///
    CIMConstQualifierDecl(const CIMQualifierDecl& x) 
    {
	Inc(_rep = x._rep); 
    }

	/** Constructor creates a CIMConstQualiferDecl - 
	@param name Name of the qualifier declaration object
	param @value - CIMValue to put into the declaration
	@param scope
	@param flavor - Optional flavor to define for the declaration. Default
	if not specified is CIMFlavor::DEFAULTS
	@arraysize - Optional integer with array size for fixed size arrays.  If
	not supplied, assumes 0.
    @exception Throws IllegalName if name argument not legal CIM identifier.
	*/
    CIMConstQualifierDecl(
	const String& name, 
	const CIMValue& value, 
	Uint32 scope,
	Uint32 flavor = CIMFlavor::DEFAULTS,
	Uint32 arraySize = 0)
    {
	_rep = new CIMQualifierDeclRep(name, value, scope, flavor, arraySize);
    }

    ~CIMConstQualifierDecl()
    {
	Dec(_rep);
    }
	///
    CIMConstQualifierDecl& operator=(const CIMConstQualifierDecl& x)
    {
	if (x._rep != _rep)
	{
	    Dec(_rep);
	    Inc(_rep = x._rep);
	}

	return *this;
    }
	///
    CIMConstQualifierDecl& operator=(const CIMQualifierDecl& x)
    {
	if (x._rep != _rep)
	{
	    Dec(_rep);
	    Inc(_rep = x._rep);
	}

	return *this;
    }
	///
    const String& getName() const 
    { 
	_checkRep();
	return _rep->getName(); 
    }
	///
    CIMType getType() const 
    { 
	_checkRep();
	return _rep->getType(); 
    }
	///
    Boolean isArray() const 
    {
	_checkRep();
	return _rep->isArray();
    }
	 ///
    const CIMValue& getValue() const 
    { 
	_checkRep();
	return _rep->getValue(); 
    }
	///
    Uint32 getScope() const 
    {
	_checkRep();
	return _rep->getScope();
    }
	///
    const Uint32 getFlavor() const 
    {
	_checkRep();
	return _rep->getFlavor();
    }
	///
	Boolean isFlavor(Uint32 flavor) const
	{
		return ((getFlavor() & flavor) !=0);
	}
	///
    Uint32 getArraySize() const 
    {
	_checkRep();
	return _rep->getArraySize();
    }
	///
    operator int() const { return _rep != 0; }
	///
    void toXml(Array<Sint8>& out) const
    {
	_checkRep();
	_rep->toXml(out);
    }
	///
    void print(PEGASUS_STD(ostream) &o=PEGASUS_STD(cout)) const
    {
	_checkRep();
	_rep->print(o);
    }
	///
    Boolean identical(const CIMConstQualifierDecl& x) const
    {
	x._checkRep();
	_checkRep();
	return _rep->identical(x._rep);
    }
	///
    CIMQualifierDecl clone() const
    {
	return CIMQualifierDecl(_rep->clone());
    }

private:

    void _checkRep() const
    {
	if (!_rep)
	    ThrowUnitializedHandle();
    }

    CIMQualifierDeclRep* _rep;
    friend class CIMQualifierDecl;
};

#define PEGASUS_ARRAY_T CIMQualifierDecl
# include "ArrayInter.h"
#undef PEGASUS_ARRAY_T

PEGASUS_NAMESPACE_END

#endif /* Pegasus_QualifierDecl_h */
