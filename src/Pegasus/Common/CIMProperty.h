//%/////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001 The Open group, BMC Software, Tivoli Systems, IBM
//									 ropogated
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
// Modified By: Roger Kumpf (roger_kumpf@hp.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_Property_h
#define Pegasus_Property_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/String.h>
#include <Pegasus/Common/CIMValue.h>
#include <Pegasus/Common/CIMQualifier.h>

PEGASUS_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////
//
// CIMProperty
//
////////////////////////////////////////////////////////////////////////////////

class CIMConstProperty;
class CIMPropertyRep;
#ifdef PEGASUS_INTERNALONLY
class CIMClassRep;
class CIMInstanceRep;
class DeclContext;
#endif

/** CIMProperty Class - This C++ class implements the CIM Property 
Object. It defines a single CIM Property and allows the manipulation of that 
property. A CIM property is a value used to characterize an instance of a 
class.
ATTN : P3 KS 03/02/02Documentation This is a very poor definition for property class.
ATTN: Define the property concept in more detail and ref property.
*/
class PEGASUS_COMMON_LINKAGE CIMProperty
{
public:

    /** CIMProperty constructor. */
    CIMProperty();

    /** CIMProperty constructor. Constructs this property form another 
		CIMProperty object
	*/
    CIMProperty(const CIMProperty& x);

    /** CIMProperty Constructor for CIMProperty that adds a number of 
        parameters to the constructed CIMProperty object.
        @param name String Name for the property 
		@param value CIMValue defines the value for the property
		@param arraySize (optional) - Size of array if fixed array size.
		@param referenceClassName (optional) String parameter that defines the 
        reference class name for the property. This parameter is required if
		the property is type CIMReference. /Ref{referenceClassName}
        @param classOrigin (optional) String parameter to define the class 
		origin of the property /Ref{ClassOrigin} 
        @param propagated (optional) If true defines the property as 
        propagated  /Ref{propagated properties}
        @return	The constructed property object
		@exception Throws "IllegalName" if name argumentis not legal a CIM 
		name /Ref{CIMName}.
    */
    CIMProperty(
	const String& name,
	const CIMValue& value,
	Uint32 arraySize = 0,
	const String& referenceClassName = String::EMPTY,
	const String& classOrigin = String::EMPTY,
	Boolean propagated = false);

    /** ~CIMProperty(). */
    ~CIMProperty();

    /// Operator =
    CIMProperty& operator=(const CIMProperty& x);

    /** getName - Gets the name of the property.
        @return String containing the property name.
        <pre>
        CIMProperty p1("count", Uint32(231));
    	assert(p1.getName() == Uint32(231));
		</pre>
    */
    const String& getName() const;

    /** setName - Set the property name. Throws IllegalName if name 
        argument not legal CIM identifier.
		@param - Name to set
		@exception Throws "IllegalName" exception is name
		ATTN: P3 please hide this. The only way a name should be
		set is through a constructor.
    */
    void setName(const String& name);

    /** Get the value of the property. */
    const CIMValue& getValue() const;

    /** Get the type of the property. */
    CIMType getType() const;

    /** Check if the property is an array type. */
    Boolean isArray() const;

    /** setValue Sets the Value in the Property object from the input 
        parameter.
		@param value CIMValue containing the value to be put into the 
        property. /Ref{CIMValue}
    */
    void setValue(const CIMValue& value);

    /** getArraySize gets the arraysize parameter from the property
	@return Uint32 with the arraysize.
	*/
    Uint32 getArraySize() const;

    /** getReferenceClassName - gets the referenceClassName.
	@return String contianing the referenceClassName if this is a
	reference property or empty if another CIMType.
    */
    const String& getReferenceClassName() const;

    /** getClassOrigin - Gets the classOrigin field from the property
		object. This will be a string with the name of the originating
		class for the property or empty if this is the originating class
		@return String with classOrigin name.
	*/
    const String& getClassOrigin() const;

    /**setClassOrigin - Sets the Class Origin attribute
	@param classOrigin String containing the classOrigin
    */
    void setClassOrigin(const String& classOrigin);

    /** getPropagated - Tests if this property is propogated.
	@return - Returns true if the class is propogated.
    */
    Boolean getPropagated() const;

    /** setProgagated - Sets the propagated attribute true or fals.
		@param Boolean true or false representing propagated state to be set.
	*/
    void setPropagated(Boolean propagated);

    /** addQualifier adds a qualifier object to the property and
		increases the qualifier count
		@param x CIMQualifier object to be added
		@return Returns this properpty as a convience in adding multiple entities
		@exceptionThrows AlreadyExists.
    */
    CIMProperty& addQualifier(const CIMQualifier& x);

    /**findQualifier - Finds the qualifier object defined
	by the name parameter if it is attached to this 
	CIMProperty
	@param name String parameter defining name of Qualifier
	object.
	@return Position of the qualifier object or -1 if not 
	found
    */
    Uint32 findQualifier(const String& name) const;

    /** existsQualifier - Determines if the qualifier object 
        defined by the name parameter is attached to this 
		CIMProperty.
		@param name String parameter defining name of Qualifier
		object.
		@return Returns true if the qualifier is found, else 
		returns false. 
    */
    Boolean existsQualifier(const String& name) const;

    /** getQualifier - gets the Qualifier object specified by the
	pos parameter.
	@param pos Position parameter for the Qualifier object to be
	retrieved
	@return returns a CIMQualifier object.
	@exception Throws OutOfBounds if pos is outside range
	of Qualifiers in this property object.
    */
    CIMQualifier getQualifier(Uint32 pos);

    /** getQualifier returns the qualifier defined at the position
        input.
        @param pos Uint32 position for the qualifier. Can be obtained
        from functions such as findQualifier.
        @return Returns the qualifier object.
        @exception throws OutOfBounds if pos is outside the range
        of qualifiers that exist for the property.
    */
    CIMConstQualifier getQualifier(Uint32 pos) const;

    /** removeQualifier - Removes the CIMQualifier defined by the 
	position input as a parameter.
	@param Position of the qualifier requested.
	@return CIMQualifier object or exception
	@exception OutOfBounds exception if the index is outside the range of
	parameters available from the CIMMethod.
    */
    void removeQualifier(Uint32 pos);
    
    /** getQualifierCount - Returns count of the number
    	of qualifiers attached to the CIMProperty object.
	@return Count of the number of CIMQualifier objects attached
	to the CIMProperty object.
    */
    Uint32 getQualifierCount() const;

    /**identical - compares the CIMProperty object with
       another CIMProperty object defined by the input parameter.
       @param x CIMPropery object for comparison
       @return Returns true if the objects are identical
    */
    Boolean identical(const CIMConstProperty& x) const;

    /** isKey - Tests the CIMProperty to determine if any
        qualifiers is a key indicating that this is a key
	property
	@return Returns true if this is a key property.
    */
    Boolean isKey() const;

    /// clone - ATTN: P3 Documentation
    CIMProperty clone(Boolean propagateQualifiers) const;

#ifdef PEGASUS_INTERNALONLY
    /// CIMMethod resolve
    void resolve(
	DeclContext* declContext,
	const String& nameSpace,
	Boolean isInstancePart,
	const CIMConstProperty& property,
	Boolean propagateQualifiers);

    /** resolve - Resolves the property. Resolution is the process of
        intregating the property into the the context of a repository
        or other store.
        Note that this is an internal function and should not be made
        available to external users.
        ATTN: P3 03/02/02 KS Needs more documentation.
        @param declContext Defines the context in which the property is
        to be resolved.  This provides the basis for other functions to
        get information from the context to use to resolve the property.
        @parm nameSpace Namespace in which the property is to be placed
        @param isInstancePart - Is this instance or class resolution
        @param propagateQualifiers Boolean to determine if we propagate qualifiers
    */
    void resolve(
	DeclContext* declContext,
	const String& nameSpace,
	Boolean isInstancePart,
	Boolean propagateQualifiers);

    /// ATTN: documentation
    operator int() const;

    /** toXML  - Converts the object to XML and puts the
	resutl in the out parameter
	@param out Parameter for XML output
    */
    void toXml(Array<Sint8>& out) const;

    /** print - Converts the object to XML and output
    	it to cout
    */
    void print(PEGASUS_STD(ostream)& o = PEGASUS_STD(cout)) const;

    /** toMof  - Converts the object to Mof and puts the
	resutl in the out parameter
	@param out Parameter for Mof output
    */
    void toMof(Array<Sint8>& out) const;

private:

    CIMProperty(CIMPropertyRep* rep);

    // This constructor allows the CIMClassRep friend class to cast
    // away constness.

    PEGASUS_EXPLICIT CIMProperty(const CIMConstProperty& x);

    void _checkRep() const;

    friend class CIMConstProperty;
    friend class CIMClassRep;
    friend class CIMInstanceRep;
#endif

    CIMPropertyRep* _rep;
};

////////////////////////////////////////////////////////////////////////////////
//
// CIMConstProperty
//
////////////////////////////////////////////////////////////////////////////////

class PEGASUS_COMMON_LINKAGE CIMConstProperty
{
public:

    CIMConstProperty();

    CIMConstProperty(const CIMConstProperty& x);

    CIMConstProperty(const CIMProperty& x);

    // Throws IllegalName if name argument not legal CIM identifier.

    CIMConstProperty(
	const String& name,
	const CIMValue& value,
	Uint32 arraySize = 0,
	const String& referenceClassName = String::EMPTY,
	const String& classOrigin = String::EMPTY,
	Boolean propagated = false);

    ~CIMConstProperty();

    CIMConstProperty& operator=(const CIMConstProperty& x);

    CIMConstProperty& operator=(const CIMProperty& x);

    const String& getName() const;

    const CIMValue& getValue() const;

    CIMType getType() const;

    Boolean isArray() const;

    Uint32 getArraySize() const;

    const String& getReferenceClassName() const;

    const String& getClassOrigin() const;

    Boolean getPropagated() const;

    Uint32 findQualifier(const String& name) const;

    CIMConstQualifier getQualifier(Uint32 pos) const;

    Uint32 getQualifierCount() const;

    Boolean identical(const CIMConstProperty& x) const;

    Boolean isKey() const;

    CIMProperty clone(Boolean propagateQualifiers) const;

#ifdef PEGASUS_INTERNALONLY
    operator int() const;

    void toXml(Array<Sint8>& out) const;

    void print(PEGASUS_STD(ostream)& o = PEGASUS_STD(cout)) const;

    void toMof(Array<Sint8>& out) const;

private:

    void _checkRep() const;

    CIMPropertyRep* _rep;

    friend class CIMProperty;
    friend class CIMPropertyRep;
#endif
};

#define PEGASUS_ARRAY_T CIMProperty
# include "ArrayInter.h"
#undef PEGASUS_ARRAY_T

PEGASUS_NAMESPACE_END

#endif /* Pegasus_Property_h */
