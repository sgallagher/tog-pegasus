//%/////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001, 2002 BMC Software, Hewlett-Packard Company, IBM,
// The Open Group, Tivoli Systems
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
//             	Roger Kumpf, Hewlett-Packard Company (roger_kumpf@hp.com)
//              Carol Ann Krug Graves, Hewlett-Packard Company
//                (carolann_graves@hp.com)
//
//%/////////////////////////////////////////////////////////////////////////////

/*
 CIMQualifier.h - Defines the CIM qualifiers class.

*/

#ifndef Pegasus_Qualifier_h
#define Pegasus_Qualifier_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/Linkage.h>
#include <Pegasus/Common/CIMName.h>
#include <Pegasus/Common/CIMFlavor.h>
#include <Pegasus/Common/CIMType.h>
#include <Pegasus/Common/CIMValue.h>

PEGASUS_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////
//
// CIMQualifier
//
////////////////////////////////////////////////////////////////////////////////

class CIMConstQualifier;
class CIMClassRep;
class Resolver;
#if defined(PEGASUS_PLATFORM_AIX_RS_IBMCXX) || defined(PEGASUS_PLATFORM_ZOS_ZSERIES_IBM)
class CIMQualifierRep;
#endif

/** Class CIMQualifier - This class defines the Pegasus implementation of the 
    CIM Qualifier \Ref{QUALIFIER}.  It is almost identical to
    CIMQualifierDecl except that it has no scope member. \Ref{CIMQualifierDecl}
    This includes functions to create qualifiers and manipulate/test
    the individual components of the CIMQualifier.
    CIMQualifiers are accumulated into lists for use in CIMClasses and CIM
    Properties using the CIMQualifierList Class and its
    functions. \Ref{CIMQualifierList}
*/
class PEGASUS_COMMON_LINKAGE CIMQualifier
{
public:

    /** Constructor instantiates a CIM qualifier with empty name value 
	fields.Constructor 
	@return instantiated empty qualifier object
    */
    CIMQualifier();

    /** Constructor - instantiates a CIM qualifier object from another 
        qualifier object. 
        @param CIM CIMQualifier object
        @return - Instantiated qualifier object
    */
    CIMQualifier(const CIMQualifier& x);

    /** Constructor - Instantiates a CIM qualifier object with the parameters 
        defined on input.
        @param CIMName representing CIMName for the new qualifier
        @param value
        @param flavor - Flavor defined for this qualifier definition. Default
        for this parameter is CIMFlavor::NONE.
        @param propoagated - Boolean defining whether this is a propagated
        qualifier.  This is an optional parameter with default = false
        @return -Returns the instantiated qualifier object or throws an
        exception if the name argument is illegal
    
        @exception Throws IllegalName if name argument not legal CIM 
        identifier.
    */
    CIMQualifier(
	const CIMName& name, 
	const CIMValue& value, 
	Uint32 flavor = CIMFlavor::NONE,
	Boolean propagated = false);

    /// destructor
    ~CIMQualifier();

    /// operator
    CIMQualifier& operator=(const CIMQualifier& x);

    /**	getName - Returns the name field from the qualifier
        @return CIMName containing the qualifier name.
    */
    const CIMName& getName() const;

    /**	setName - Sets the qualifier name field in the qualifier object.
	@param name - CIMName containing the name for the qualifier
	@exception Throws IllegalName if name argument not legal CIM identifier.
    */
    void setName(const CIMName& name);

    /** getType - Gets the type field from the qualifier object.
        @return CIMType containing the type for this qualifier /Ref{CIMType}.
    */
    CIMType getType() const;

    /**	isArray - Returns true if the qualifier is an array
	@return Boolean true if array qualifier.
    */
    Boolean isArray() const;

    /**	getValue - Gets the value component of the qualifier object
	@return CIMValue containing the value component
    */
    const CIMValue& getValue() const;

    /**	setValue - Puts a CIMValue object into a CIMQualifier object
	@param value - The CIMValue object to install
    */
    void setValue(const CIMValue& value);

    /** setFlavor - Sets the bits defined on input into the Flavor variable
        for the Qualifier Object.
        @param flavor - Uint32 defines the flavor bits to be set.
    */
    void setFlavor(Uint32 flavor);

    /** unsetFlavor - Resets the bits defined for the flavor 
        for the Qualifier Object with the input.
        @param flavor - Uint32 defines the flavor bits to be set.
    */
    void unsetFlavor(Uint32 flavor);

    /**	getFlavor - Gets the Flavor field from a Qualifier
	@return - Uint32 with the Flavor flags that can be tested
	against the CIMFlavor constants.
    */
    Uint32 getFlavor() const;

    /**	isFlavor - Boolean function that determines if particular flavor
	flags are set in the flavor variable of a qualifier.
	@param flavor - The flavor bits to test.
	Return True if the defined flavor is set.
	<pre>
	if (q.isFlavor(CIMType::TOSUBCLASS)
		do something based on TOSUBCLASS being true
	</pre>
    */
    Boolean isFlavor(Uint32 flavor) const;

    /**	getPropagated returns the propagated indicator
	@return Uint32 - TBD
    */
    const Uint32 getPropagated() const;

    /**	setPropagated - Sets the Propagated flag for the object.
    */
    void setPropagated(Boolean propagated);

#ifdef PEGASUS_INTERNALONLY
    /**	CIMMethod
    */
    Boolean isNull() const;
#endif

    /**	identical - compares two CIMQualifier objects.
        @return - True if the objects are identical.
    */
    Boolean identical(const CIMConstQualifier& x) const;

    /**	clone Creates an exact copy of the qualifier and returns the
	new object.
	@return CIMQualifier New Qualifier object.
    */
    CIMQualifier clone() const;

private:

    CIMQualifier(CIMQualifierRep* rep);

    void _checkRep() const;

    CIMQualifierRep* _rep;

    friend class CIMConstQualifier;
    friend class CIMClassRep;
    friend class Resolver;
    friend class XmlWriter;
    friend class MofWriter;
};

////////////////////////////////////////////////////////////////////////////////
//
// CIMConstQualifier
//
////////////////////////////////////////////////////////////////////////////////

class PEGASUS_COMMON_LINKAGE CIMConstQualifier
{
public:

    CIMConstQualifier();

    CIMConstQualifier(const CIMConstQualifier& x);

    CIMConstQualifier(const CIMQualifier& x);

    // Throws IllegalName if name argument not legal CIM identifier.

    CIMConstQualifier(
	const CIMName& name, 
	const CIMValue& value, 
	Uint32 flavor = CIMFlavor::NONE,
	Boolean propagated = false);

    ~CIMConstQualifier();

    CIMConstQualifier& operator=(const CIMConstQualifier& x);

    CIMConstQualifier& operator=(const CIMQualifier& x);

    const CIMName& getName() const;

    CIMType getType() const;

    Boolean isArray() const;

    const CIMValue& getValue() const;

    const Uint32 getFlavor() const;

    Boolean isFlavor(Uint32 flavor) const;
	
    Boolean isFlavorToSubclass() const;

    Boolean isFlavorToInstance() const;

    Boolean isFlavorOverridable() const;

    const Uint32 getPropagated() const;

#ifdef PEGASUS_INTERNALONLY
    Boolean isNull() const;
#endif

    Boolean identical(const CIMConstQualifier& x) const;

    CIMQualifier clone() const;

private:

    void _checkRep() const;

    CIMQualifierRep* _rep;

    friend class CIMQualifier;
    friend class XmlWriter;
    friend class MofWriter;
};

#define PEGASUS_ARRAY_T CIMQualifier
# include <Pegasus/Common/ArrayInter.h>
#undef PEGASUS_ARRAY_T

PEGASUS_NAMESPACE_END

#endif /* Pegasus_Qualifier_h */
