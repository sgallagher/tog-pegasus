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
//              Carol Ann Krug Graves, Hewlett-Packard Company
//                  (carolann_graves@hp.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_Object_h
#define Pegasus_Object_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/Linkage.h>
#include <Pegasus/Common/String.h>
#include <Pegasus/Common/CIMName.h>
#include <Pegasus/Common/Array.h>
#include <Pegasus/Common/CIMProperty.h>
#include <Pegasus/Common/CIMQualifier.h>

PEGASUS_NAMESPACE_BEGIN

class CIMConstObject;
class CIMObjectRep;
class CIMClass;
class CIMConstClass;
class CIMInstance;
class CIMConstInstance;

////////////////////////////////////////////////////////////////////////////////
//
// CIMObject
//
////////////////////////////////////////////////////////////////////////////////

/** This class either refers to a CIMInstance or a CIMClass.

    The CIMObjectRep data member points to either a CIMInstanceRep or
    CIMClassRep.
*/
class PEGASUS_COMMON_LINKAGE CIMObject
{
public:

    /** Constructor.
    */
    CIMObject();

    /** Copy constructor.
    */
    CIMObject(const CIMObject& x);

    /** Construction from CIMClass.
    */
    CIMObject(const CIMClass& x);

    /** Construction from CIMInstance.
    */
    CIMObject(const CIMInstance& x);

    /** Assignment operator.
    */
    CIMObject& operator=(const CIMObject& x);

    /** Destructor.
    */
    ~CIMObject();

    /**	Accessor for ClassName component of the object.
	@return - Returns the ClassName of the object in
	a CIMName parameter.
	<pre>
	    CIMName className;
	    CIMClass myclass("myclass", "superclass");
	    className = myclass.getClassName;
	</pre>
    */
    const CIMName& getClassName() const;

    const CIMObjectPath& getPath() const;

    /**
      Sets the object path for the object
      @param  path  CIMObjectPath containing the object path
     */
    void setPath (const CIMObjectPath & path);

    /**	addQualifier - Adds the CIMQualifier object to the instance.
	Thows an exception of the CIMQualifier already exists in the instance
	@param CIMQualifier object to add to instance
	@return the resulting object.
	@exception Throws AlreadyExists.
    */
    CIMObject& addQualifier(const CIMQualifier& qualifier);

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

    void removeQualifier(Uint32 pos);
	
    /** getQualifierCount - Gets the number of CIMQualifier objects
	defined for this CIMObject.
	@return	Count of the number of CIMQualifier objects in the
	CIMObject.
	@exception Throws the OutOfBounds exception if the index
	is out of bounds
    */
    Uint32 getQualifierCount() const;

    /**	addProperty - Adds a property object defined by the input
	parameter to the CIMObject
	@param Property Object to be added.  See the CIM Property
	class for definition of the property object
	@return the resulting object.
	@exception Throws the exception AlreadyExists if the property
	already exists.
    */
    CIMObject& addProperty(const CIMProperty& x);

    /**	findProperty - Searches the CIMProperty objects installed in the
	CIMObject for property objects with the name defined by the
	input.
	@param CIMName with the name of the property object to be found
	@return Position in the CIM object to the property object if found or
	PEG_NOT_FOUND if no property object found with the name defined by the
	input.
    */
    Uint32 findProperty(const CIMName& name) const;

    /**	getProperty - Gets the CIMProperty object in the CIMObject defined
	by the input index parameter.
	@param Index to the property object in the CIMObject.
    	The index to qualifier objects is zero-origin and continuous
	so that incrementing loops can be used to get all qualifier
	objects in a CIMObject.
	@return CIMProperty object corresponding to the index.
	@exception Throws the OutOfBounds exception if the index
	is out of bounds
    */
    CIMProperty getProperty(Uint32 pos);

    /**	getProperty - Gets the CIMproperty object in the CIMObject defined
	by the input index parameter.
	@param Index to the property object in the CIMObject.
    	The index to qualifier objects is zero-origin and continuous
	so that incrementing loops can be used to get all qualifier
	objects in a CIMInstnace.
	@return CIMProperty object corresponding to the index.
	@exception Throws the OutOfBounds exception if the index
	is out of bounds
    */
    CIMConstProperty getProperty(Uint32 pos) const;

    /** removeProperty - Removes the property represented
	by the position input parameter from the instance.
	@param pos Index to the property to be removed from the
	instance.  Normally this is obtained by getProperty();
	@exception Throws OutofBounds if index is not a property object
    */
    void removeProperty(Uint32 pos);

    /**	getPropertyCount - Gets the number of CIMProperty
	objects defined for this CIMObject.
	@return	Count of the number of CIMProperty objects in the
	CIMObject. Zero indicates that no CIMProperty objects
	are contained in the CIMObject
	@exception Throws the OutOfBounds exception if the index
	is out of bounds

    */
    Uint32 getPropertyCount() const;

    /**	Clones the given object.
    */
    CIMObject clone() const;

    /**	Returns true if the two classes are structurally identical.
    */
    Boolean identical(const CIMConstObject& x) const;

    /**
        Determines if the object has not been initialized.

        @return  True if the object has not been initialized,
                 False otherwise
     */
    Boolean isUninitialized() const;

    /**
        Determines if the object represents a CIMClass.

        @return  True if the object represents a CIMClass,
                 False otherwise
     */
    Boolean isClass () const;

    /**
        Determines if the object represents a CIMInstance.

        @return  True if the object represents a CIMInstance,
                 False otherwise
     */
    Boolean isInstance () const;

private:

    CIMObjectRep* _rep;

#ifdef PEGASUS_INTERNALONLY
    CIMObject(CIMObjectRep* rep);

    void _checkRep() const;

    friend class CIMConstObject;
    friend class CIMClass;
    friend class CIMConstClass;
    friend class CIMInstance;
    friend class CIMConstInstance;
#endif
};

#define PEGASUS_ARRAY_T CIMObject
# include <Pegasus/Common/ArrayInter.h>
#undef PEGASUS_ARRAY_T

////////////////////////////////////////////////////////////////////////////////
//
// CIMConstObject
//
////////////////////////////////////////////////////////////////////////////////

class PEGASUS_COMMON_LINKAGE CIMConstObject
{
public:

    CIMConstObject();

    CIMConstObject(const CIMConstObject& x);

    CIMConstObject(const CIMObject& x);

    /** Construction from CIMClass.
    */
    CIMConstObject(const CIMClass& x);

    /** Construction from CIMInstance.
    */
    CIMConstObject(const CIMInstance& x);

    /** Construction from CIMClass.
    */
    CIMConstObject(const CIMConstClass& x);

    /** Construction from CIMInstance.
    */
    CIMConstObject(const CIMConstInstance& x);

    CIMConstObject& operator=(const CIMConstObject& x);

    ~CIMConstObject();

    const CIMName& getClassName() const;

    const CIMObjectPath& getPath() const;

    Uint32 findQualifier(const CIMName& name) const;

    CIMConstQualifier getQualifier(Uint32 pos) const;

    Uint32 getQualifierCount() const;

    Uint32 findProperty(const CIMName& name) const;

    CIMConstProperty getProperty(Uint32 pos) const;

    Uint32 getPropertyCount() const;

    CIMObject clone() const;

    Boolean identical(const CIMConstObject& x) const;

    Boolean isUninitialized() const;

private:

    CIMObjectRep* _rep;

#ifdef PEGASUS_INTERNALONLY
    void _checkRep() const;

    friend class CIMObject;
    friend class CIMClass;
    friend class CIMConstClass;
    friend class CIMInstance;
    friend class CIMConstInstance;
#endif
};

PEGASUS_NAMESPACE_END

#endif /* Pegasus_Object_h */
