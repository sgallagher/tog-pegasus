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
// Modified By: Roger Kumpf, Hewlett-Packard Company (roger_kumpf@hp.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_Instance_h
#define Pegasus_Instance_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/CIMObject.h>

PEGASUS_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////
//
// CIMInstance
//
////////////////////////////////////////////////////////////////////////////////

class CIMConstInstance;
class CIMInstanceRep;

/** This class represents the instance of a CIM class. It is used manipulate
    instances and their parts.
*/
class PEGASUS_COMMON_LINKAGE CIMInstance
{
public:

    /** Constructor - Create a CIM Instance object.
	@return  Instance created
    */
    CIMInstance();

    /** Constructor - Create a CIMInstance object from another Instance.
	@param Instance object from which the new instance is created.
	@return New instance
    */
    CIMInstance(const CIMInstance& x);

    PEGASUS_EXPLICIT CIMInstance(const CIMObject& x) throw(DynamicCastFailed);

    /**	Constructor - Creates an Instance object with the classname
	from the input parameters
	@param - String className to be used with new instance object
	@return The new instance object
	@exception Throws IllegalName if className argument not legal CIM
	identifier. ATTN: Clarify the defintion	of legal CIM identifier.
    */
    CIMInstance(const String& className);

    /** Constructor. */
    CIMInstance& operator=(const CIMInstance& x);

    /** Destructor. */
    virtual ~CIMInstance();

    /**	getClassName - 	Returns the class name of the instance
	@return String with the class name.
    */
    const String& getClassName() const;

    /** equalClassName compares Name of the class with a String. This test
        performs a comparison of the classname component of the object
	with a String.	Note that this function was included specifically
	because the equality compare is not just a straight comparison
	because classnames are case independent.
	@param classname String containing the name for comparison
	@return True if it is the same class name (equalNoCase compare passes)
	or false if not.
    */
    const Boolean equalClassName(const String& classname) const;

    const CIMReference& getPath() const;

    /**	addQualifier - Adds the CIMQualifier object to the instance.
	Thows an exception of the CIMQualifier already exists in the instance
	@param CIMQualifier object to add to instance
	@return ATTN:
	@exception Throws AlreadyExists.
    */
    CIMInstance& addQualifier(const CIMQualifier& qualifier);

    /**	findQualifier - Searches the instance for the qualifier object
        defined by the input parameter.
	@param String defining the qualifier object to be found.
	@return - Position of the qualifier to be used in subsequent
	operations or PEG_NOT_FOUND if the qualifier is not found.
    */
    Uint32 findQualifier(const String& name) const;

    /**	existsQualifier - Searches the instance for the qualifier object
        defined by the input parameter.
	@param String defining the qualifier object to be found.
	@return - Returns True if  the qualifier object exists or false
	if the qualifier is not found.
    */
    Boolean existsQualifier(const String& name) const;

    /**	getQualifier - Retrieves the qualifier object defined by the
	index input parameter.  @ index for the qualifier object.
	The index to qualifier objects is zero-origin and continuous
	so that incrementing loops can be used to get all qualifier
	objects in a CIMInstnace.
	@return: Returns qualifier object defined by index.
	@exception Throws the OutOfBounds exception if the index
	is out of bounds
    */
    CIMQualifier getQualifier(Uint32 pos);

    /** getQualifier - Retrieves the qualifier object defined by the
	index input parameter.  @ index for the qualifier object.
	The index to qualifier objects is zero-origin and continuous
	so that incrementing loops can be used to get all qualifier
	objects in a CIMInstnace.
	@return: Returns qualifier object defined by index.
	@exception Throws the OutOfBounds exception if the index
	is out of bounds
	ATTN: What is effect of out of range index???
	ATTN: Is the above statement correct???
    */
    CIMConstQualifier getQualifier(Uint32 pos) const;

    /**	getQualifierCount - Gets the numbercount of CIMQualifierobjects
	defined for this CIMInstance.
	@return	Count of the number of CIMQalifier objects in the
	CIMInstance.
	@exception Throws the OutOfBounds exception if the index
	is out of bounds
    */
    Uint32 getQualifierCount() const;

    /**	addProperty - Adds a property object defined by the input
	parameter to the CIMInstance
	@param Property Object to be added.  See the CIM Property
	class for definition of the property object
	@return ATTN:
	@exception Throws the exception AlreadyExists if the property
	already exists.
    */
    CIMInstance& addProperty(const CIMProperty& x);

    /**	findProperty - Searches the CIMProperty objects installed in the
	CIMInstance for property objects with the name defined by the
	input.
	@param String with the name of the property object to be found
	@return Position in the CIM Instance to the property object if found or
	PEG_NOT_FOUND if no property object found with the name defined by the
	input.
    */
    Uint32 findProperty(const String& name) const;

    /** existsPropery - Determines if a property object with the
	name defined by the input parameter exists in the class.
	@parm String parameter with the property name.
	@return True if the property object exists.
    */
    Boolean existsProperty(const String& name) const;

    /**	getProperty - Gets the CIMproperty object in the CIMInstance defined
	by the input index parameter.
	@param Index to the property object in the CIMInstance.
    	The index to qualifier objects is zero-origin and continuous
	so that incrementing loops can be used to get all qualifier
	objects in a CIMInstnace.
	@return CIMProperty object corresponding to the index.
	@exception Throws the OutOfBounds exception if the index
	is out of bounds

	ATTN: What is the effect of out of range?
    */
    CIMProperty getProperty(Uint32 pos);

    /**	getProperty - Gets the CIMproperty object in the CIMInstance defined
	by the input index parameter.
	@param Index to the property object in the CIMInstance.
    	The index to qualifier objects is zero-origin and continuous
	so that incrementing loops can be used to get all qualifier
	objects in a CIMInstnace.
	@return CIMProperty object corresponding to the index.
	@exception Throws the OutOfBounds exception if the index
	is out of bounds

	ATTN: What is the effect of out of range?
    */
    CIMConstProperty getProperty(Uint32 pos) const;

    /** removeProperty - Removes the property represented
	by the position input parameter from the instance.
	@param pos Index to the property to be removed from the
	instance.  Normally this is obtained by getProperty();
	@exception Throws OutofBounds if index is not a property object
    */
    void removeProperty(Uint32 pos);

    /**	getPropertyCount - Gets the numbercount of CIMProperty
	objects defined for this CIMInstance.
	@return	Count of the number of CIMProperty objects in the
	CIMInstance. Zero indicates that no CIMProperty objects
	are contained in the CIMInstance
	@exception Throws the OutOfBounds exception if the index
	is out of bounds

    */
    Uint32 getPropertyCount() const;

    /** getInstanceName - Get the instance name of this instance. The class
	argument is used to determine which fields are keys. The instance
	name has this from:

	<PRE>
	    ClassName.key1=value1,...,keyN=valueN
	</PRE>

	The instance name is in standard form (the class name and key name
	is all lowercase; the keys-value pairs appear in sorted order by
	key name).
    */
    CIMReference getInstanceName(const CIMConstClass& cimClass) const;

    /**	CIMMethod

    */
    CIMInstance clone() const;

    /**	identical - Compares the CIMInstance with another CIMInstance
	defined by the input parameter for equality of all components.
	@param CIMInstance to be compared
	@return Boolean true if they are identical
    */
    Boolean identical(const CIMConstInstance& x) const;

    /**	toXml - Creates an XML transformation of the CIMInstance
	compatible with the DMTF CIM Operations over HTTP defintions.
	ATTN: This is incorrect and needs to be corrected.
    */
    void toXml(Array<Sint8>& out) const;

    /**	toMof - Creates an MOF transformation of the CIMInstance
	compatible with the DMTF specification.
    */
    void toMof(Array<Sint8>& out) const;

    /**	prints the class in XML format. */
    void print(PEGASUS_STD(ostream)& o=PEGASUS_STD(cout)) const;

#ifdef PEGASUS_INTERNALONLY
    /**	isNull() - ATTN: */
    Boolean isNull() const;

    /**	resolve - ATTN: */
    void resolve(
	DeclContext* declContext,
	const String& nameSpace,
	Boolean propagateQualifiers);

    void resolve(
	DeclContext* declContext,
	const String& nameSpace,
	CIMConstClass& cimClassOut,
	Boolean propagateQualifiers);

    String toString() const;
#endif

private:

    CIMInstanceRep* _rep;

#ifdef PEGASUS_INTERNALONLY
    CIMInstance(CIMInstanceRep* rep);

    void _checkRep() const;

    friend class CIMConstInstance;
    friend class CIMObject;
    friend class CIMConstObject;
#endif
};

////////////////////////////////////////////////////////////////////////////////
//
// CIMConstInstance
//
////////////////////////////////////////////////////////////////////////////////

class PEGASUS_COMMON_LINKAGE CIMConstInstance
{
public:

    CIMConstInstance();

    CIMConstInstance(const CIMConstInstance& x);

    CIMConstInstance(const CIMInstance& x);

    PEGASUS_EXPLICIT CIMConstInstance(const CIMObject& x)
        throw(DynamicCastFailed);

    PEGASUS_EXPLICIT CIMConstInstance(const CIMConstObject& x)
        throw(DynamicCastFailed);

    // Throws IllegalName if className argument not legal CIM identifier.
    CIMConstInstance(const String& className);

    CIMConstInstance& operator=(const CIMConstInstance& x);

    CIMConstInstance& operator=(const CIMInstance& x);

    ~CIMConstInstance();

    const String& getClassName() const;

    const Boolean equalClassName(const String& classname) const;

    const CIMReference& getPath() const;

    Uint32 findQualifier(const String& name) const;

    CIMConstQualifier getQualifier(Uint32 pos) const;

    Uint32 getQualifierCount() const;

    Uint32 findProperty(const String& name) const;

    CIMConstProperty getProperty(Uint32 pos) const;

    Uint32 getPropertyCount() const;

    CIMReference getInstanceName(const CIMConstClass& cimClass) const;

    CIMInstance clone() const;

    Boolean identical(const CIMConstInstance& x) const;

#ifdef PEGASUS_INTERNALONLY
    Boolean isNull() const;

    void toXml(Array<Sint8>& out) const;

    void print(PEGASUS_STD(ostream)& o=PEGASUS_STD(cout)) const;

    String toString() const;
#endif

private:

    CIMInstanceRep* _rep;

#ifdef PEGASUS_INTERNALONLY
    void _checkRep() const;

    friend class CIMInstance;
    friend class CIMObject;
    friend class CIMConstObject;
#endif
};

PEGASUS_COMMON_LINKAGE Boolean operator==(
    const CIMInstance& x,
    const CIMInstance& y);

#define PEGASUS_ARRAY_T CIMInstance
# include "ArrayInter.h"
#undef PEGASUS_ARRAY_T

PEGASUS_NAMESPACE_END

#endif /* Pegasus_Instance_h */
