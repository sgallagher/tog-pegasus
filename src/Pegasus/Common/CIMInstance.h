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
#include <Pegasus/Common/Linkage.h>
#include <Pegasus/Common/CIMName.h>
#include <Pegasus/Common/CIMObject.h>

PEGASUS_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////
//
// CIMInstance
//
////////////////////////////////////////////////////////////////////////////////

class CIMConstInstance;
class CIMInstanceRep;
class Resolver;

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
	@param x Instance object from which the new instance is created.
    */
    CIMInstance(const CIMInstance& x);

    /**	Constructor - Creates an Instance object from the given CIMObject
	@param x CIMObject from which to create the CIMInstance
	@exception DynamicCastFailedException If a CIMInstance can not be
        created from the given CIMObject
    */
    PEGASUS_EXPLICIT CIMInstance(const CIMObject& x);

    /**	Constructor - Creates an Instance object with the classname
	from the input parameters
	@param - CIMName className to be used with new instance object
	@return The new instance object
    */
    CIMInstance(const CIMName& className);

    /** Constructor. */
    CIMInstance& operator=(const CIMInstance& x);

    /** Destructor. */
    virtual ~CIMInstance();

    /**	getClassName - 	Returns the class name of the instance
	@return CIMName with the class name.
    */
    const CIMName& getClassName() const;

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
	@exception AlreadyExistsException.
    */
    CIMInstance& addQualifier(const CIMQualifier& qualifier);

    /**	findQualifier - Searches the instance for the qualifier object
        defined by the input parameter.
	@param CIMName defining the qualifier object to be found.
	@return - Position of the qualifier to be used in subsequent
	operations or PEG_NOT_FOUND if the qualifier is not found.
    */
    Uint32 findQualifier(const CIMName& name) const;

    /**	getQualifier - Retrieves the qualifier object defined by the
	index input parameter.  @ index for the qualifier object.
	The index to qualifier objects is zero-origin and continuous
	so that incrementing loops can be used to get all qualifier
	objects in a CIMInstnace.
	@return: Returns qualifier object defined by index.
	@exception Throws the IndexOutOfBoundsException exception if the index
	is out of bounds
    */
    CIMQualifier getQualifier(Uint32 index);

    /** getQualifier - Retrieves the qualifier object defined by the
	index input parameter.  @ index for the qualifier object.
	The index to qualifier objects is zero-origin and continuous
	so that incrementing loops can be used to get all qualifier
	objects in a CIMInstnace.
	@return: Returns qualifier object defined by index.
	@exception Throws the IndexOutOfBoundsException exception if the index
	is out of bounds
    */
    CIMConstQualifier getQualifier(Uint32 index) const;

    /**	getQualifierCount - Gets the numbercount of CIMQualifierobjects
	defined for this CIMInstance.
	@return	Count of the number of CIMQalifier objects in the
	CIMInstance.
	@exception Throws the IndexOutOfBoundsException exception if the index
	is out of bounds
    */
    Uint32 getQualifierCount() const;

    /**	addProperty - Adds a property object defined by the input
	parameter to the CIMInstance
	@param Property Object to be added.  See the CIM Property
	class for definition of the property object
	@return the resulting CIMInstance.
	@exception AlreadyExistsException if the property already exists.
    */
    CIMInstance& addProperty(const CIMProperty& x);

    /**	findProperty - Searches the CIMProperty objects installed in the
	CIMInstance for property objects with the name defined by the
	input.
	@param CIMName with the name of the property object to be found
	@return Position in the CIM Instance to the property object if found or
	PEG_NOT_FOUND if no property object found with the name defined by the
	input.
    */
    Uint32 findProperty(const CIMName& name) const;

    /**	getProperty - Gets the CIMProperty object in the CIMInstance defined
	by the input index parameter.
	@param Index to the property object in the CIMInstance.
    	The index to qualifier objects is zero-origin and continuous
	so that incrementing loops can be used to get all qualifier
	objects in a CIMInstnace.
	@return CIMProperty object corresponding to the index.
	@exception IndexOutOfBoundsException if index is outside the range of
        properties in this instance
    */
    CIMProperty getProperty(Uint32 index);

    /**	getProperty - Gets the CIMproperty object in the CIMInstance defined
	by the input index parameter.
	@param Index to the property object in the CIMInstance.
    	The index to qualifier objects is zero-origin and continuous
	so that incrementing loops can be used to get all qualifier
	objects in a CIMInstnace.
	@return CIMProperty object corresponding to the index.
	@exception IndexOutOfBoundsException if index is outside the range of
        properties in this instance
    */
    CIMConstProperty getProperty(Uint32 index) const;

    /** removeProperty - Removes the property represented
	by the index input parameter from the instance.
	@param index Index to the property to be removed from the
	instance.  Normally this is obtained by getProperty();
	@exception IndexOutOfBoundsException if index is outside the range of
        properties in this instance
    */
    void removeProperty(Uint32 index);

    /**	getPropertyCount - Gets the numbercount of CIMProperty
	objects defined for this CIMInstance.
	@return	Count of the number of CIMProperty objects in the
	CIMInstance. Zero indicates that no CIMProperty objects
	are contained in the CIMInstance
	@exception Throws the IndexOutOfBoundsException exception if the index
	is out of bounds

    */
    Uint32 getPropertyCount() const;

    /** buildPath - Build the CIM object path for this instance. The class
	argument is used to determine which fields are keys. The instance
	name has this form:

	<PRE>
	    ClassName.key1=value1,...,keyN=valueN
	</PRE>

	The object path is in standard form (the class name and key name
	is all lowercase; the keys-value pairs appear in sorted order by
	key name).

        Note that the path attribute of the CIMInstanceRep object is not 
        modified.
    */
    CIMObjectPath buildPath(const CIMConstClass& cimClass) const;

    /**	CIMMethod

    */
    CIMInstance clone() const;

    /**	identical - Compares the CIMInstance with another CIMInstance
	defined by the input parameter for equality of all components.
	@param CIMInstance to be compared
	@return Boolean true if they are identical
    */
    Boolean identical(const CIMConstInstance& x) const;

    /**
        Determines if the object has not been initialized.

        @return  True if the object has not been initialized,
                 False otherwise
     */
    Boolean isUninitialized() const;

private:

    CIMInstanceRep* _rep;

    CIMInstance(CIMInstanceRep* rep);

    void _checkRep() const;

    friend class CIMConstInstance;
    friend class CIMObject;
    friend class CIMConstObject;
    friend class Resolver;
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

    PEGASUS_EXPLICIT CIMConstInstance(const CIMObject& x);

    PEGASUS_EXPLICIT CIMConstInstance(const CIMConstObject& x);

    CIMConstInstance(const CIMName& className);

    CIMConstInstance& operator=(const CIMConstInstance& x);

    CIMConstInstance& operator=(const CIMInstance& x);

    ~CIMConstInstance();

    const CIMName& getClassName() const;

    const CIMObjectPath& getPath() const;

    Uint32 findQualifier(const CIMName& name) const;

    CIMConstQualifier getQualifier(Uint32 index) const;

    Uint32 getQualifierCount() const;

    Uint32 findProperty(const CIMName& name) const;

    CIMConstProperty getProperty(Uint32 index) const;

    Uint32 getPropertyCount() const;

    CIMObjectPath buildPath(const CIMConstClass& cimClass) const;

    CIMInstance clone() const;

    Boolean identical(const CIMConstInstance& x) const;

    Boolean isUninitialized() const;

private:

    CIMInstanceRep* _rep;

    void _checkRep() const;

    friend class CIMInstance;
    friend class CIMObject;
    friend class CIMConstObject;
    friend class XmlWriter;
    friend class MofWriter;
};

#define PEGASUS_ARRAY_T CIMInstance
# include <Pegasus/Common/ArrayInter.h>
#undef PEGASUS_ARRAY_T

PEGASUS_NAMESPACE_END

#endif /* Pegasus_Instance_h */
