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
// Modified By: Roger Kumpf, Hewlett-Packard Company (roger_kumpf@hp.com)
// 				Karl Schopmeyer, (k.schopmeyer@opengroup.org)
//              Carol Ann Krug Graves, Hewlett-Packard Company 
//                  (carolann_graves@hp.com)
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

    //  ATTN: Clarify the defintion of legal CIM identifier.
    /**	Constructor - Creates an Instance object with the classname
	from the input parameters
	@param - String className to be used with new instance object
	@return The new instance object
	@exception Throws IllegalName if className argument not legal CIM
	identifier.
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

    const CIMObjectPath& getPath() const;

    /**
      Sets the object path for the instance
      @param  path  CIMObjectPath containing the object path
     */
    void setPath (const CIMObjectPath & path);

    /**	addQualifier - Adds the CIMQualifier object to the instance.
	Thows an exception of the CIMQualifier already exists in the instance
	@param CIMQualifier object to add to instance
	@return the resulting CIMInstance
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
	@return the resulting CIMInstance.
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

    /**	getProperty - Gets the CIMproperty object in the CIMInstance defined
	by the input index parameter.
	@param Index to the property object in the CIMInstance.
    	The index to qualifier objects is zero-origin and continuous
	so that incrementing loops can be used to get all qualifier
	objects in a CIMInstnace.
	@return CIMProperty object corresponding to the index.
	@exception Throws the OutOfBounds exception if the index
	is out of bounds
    */
    CIMProperty getProperty(Uint32 pos) throw(OutOfBounds);

    /**	getProperty - Gets the CIMproperty object in the CIMInstance defined
	by the input index parameter.
	@param Index to the property object in the CIMInstance.
    	The index to qualifier objects is zero-origin and continuous
	so that incrementing loops can be used to get all qualifier
	objects in a CIMInstnace.
	@return CIMProperty object corresponding to the index.
	@exception Throws the OutOfBounds exception if the index
	is out of bounds
    */
    CIMConstProperty getProperty(Uint32 pos) const throw(OutOfBounds);

    /** removeProperty - Removes the property represented
	by the position input parameter from the instance.
	@param pos Index to the property to be removed from the
	instance.  Normally this is obtained by getProperty();
	@exception Throws OutofBounds if index is not a property object
    */
    void removeProperty(Uint32 pos) throw(OutOfBounds);

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
    CIMObjectPath getInstanceName(const CIMConstClass& cimClass) const;

    /**	CIMMethod

    */
    CIMInstance clone() const;

    /**	identical - Compares the CIMInstance with another CIMInstance
	defined by the input parameter for equality of all components.
	@param CIMInstance to be compared
	@return Boolean true if they are identical
    */
    Boolean identical(const CIMConstInstance& x) const;

#ifdef PEGASUS_INTERNALONLY
    /**	isNull() */
    Boolean isNull() const;

    /**	resolve */
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

    CIMInstance(CIMInstanceRep* rep);

    void _checkRep() const;

    friend class CIMConstInstance;
    friend class CIMObject;
    friend class CIMConstObject;
    friend class XmlWriter;
    friend class MofWriter;
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

    const CIMObjectPath& getPath() const;

    Uint32 findQualifier(const String& name) const;

    CIMConstQualifier getQualifier(Uint32 pos) const;

    Uint32 getQualifierCount() const;

    Uint32 findProperty(const String& name) const;

    CIMConstProperty getProperty(Uint32 pos) const;

    Uint32 getPropertyCount() const;

    CIMObjectPath getInstanceName(const CIMConstClass& cimClass) const;

    CIMInstance clone() const;

    Boolean identical(const CIMConstInstance& x) const;

#ifdef PEGASUS_INTERNALONLY
    Boolean isNull() const;

    String toString() const;
#endif

private:

    CIMInstanceRep* _rep;

    void _checkRep() const;

    friend class CIMInstance;
    friend class CIMObject;
    friend class CIMConstObject;
    friend class XmlWriter;
    friend class MofWriter;
};

PEGASUS_COMMON_LINKAGE Boolean operator==(
    const CIMInstance& x,
    const CIMInstance& y);

#define PEGASUS_ARRAY_T CIMInstance
# include <Pegasus/Common/ArrayInter.h>
#undef PEGASUS_ARRAY_T

PEGASUS_NAMESPACE_END

#endif /* Pegasus_Instance_h */
