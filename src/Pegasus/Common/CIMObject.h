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

#ifndef Pegasus_Object_h
#define Pegasus_Object_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/CIMObjectRep.h>

PEGASUS_NAMESPACE_BEGIN

class CIMClass;
class CIMConstClass;
class CIMInstance;
class CIMConstInstance;

////////////////////////////////////////////////////////////////////////////////
//
// CIMObject
//
////////////////////////////////////////////////////////////////////////////////

class CIMConstObject;
class CIMObject;

/** This class either refers to a CIMInstance or a CIMClass.

    The CIMObjectRep data member points to either a CIMInstanceRep or
    CIMClassRep.
*/
class PEGASUS_COMMON_LINKAGE CIMObject
{
public:

    /** Constructor.
    */
    CIMObject() : _rep(0)
    {

    }

    /** Copy constructor.
    */
    CIMObject(const CIMObject& x)
    {
	Inc(_rep = x._rep);
    }

    /** Construction from CIMClass.
    */
    CIMObject(const CIMClass& x);

    /** Construction from CIMInstance.
    */
    CIMObject(const CIMInstance& x);

    /** Assignment operator.
    */
    CIMObject& operator=(const CIMObject& x)
    {
	if (x._rep != _rep)
	{
	    Dec(_rep);
	    Inc(_rep = x._rep);
	}
	return *this;
    }

    /** Assignment operator.
    */
    CIMObject& operator=(const CIMClass& x);

    /** Assignment operator.
    */
    CIMObject& operator=(const CIMInstance& x);

    /** Destructor.
    */
    ~CIMObject()
    {
	Dec(_rep);
    }

	/** Accessor.
	*/
	const CIMReference & getPath(void) const
	{
	_checkRep();
	return _rep->getPath();
	}
	
    /**	Accessor.
    */
    const String& getClassName() const
    {
	_checkRep();
	return _rep->getClassName();
    }

    /**	addQualifier - Adds the CIMQualifier object to the instance.
	Thows an exception of the CIMQualifier already exists in the instance
	@param CIMQualifier object to add to instance
	@return ATTN:
	@exception Throws AlreadyExists.
    */
    CIMObject& addQualifier(const CIMQualifier& qualifier)
    {
	_checkRep();
	_rep->addQualifier(qualifier);
	return *this;
    }

    /**	findQualifier - Searches the instance for the qualifier object
        defined by the input parameter.
	@param String defining the qualifier object to be found.
	@return - Position of the qualifier to be used in subsequent
	operations or PEG_NOT_FOUND if the qualifier is not found.
    */
    Uint32 findQualifier(const String& name)
    {
	_checkRep();
	return _rep->findQualifier(name);
    }

    Uint32 findQualifier(const String& name) const
    {
	_checkRep();
	return _rep->findQualifier(name);
    }

    /**	existsQualifier - Searches the instance for the qualifier object
        defined by the input parameter.
	@param String defining the qualifier object to be found.
	@return - Returns True if  the qualifier object exists or false
	if the qualifier is not found.
    */
    Boolean existsQualifier(const String& name)
    {
	_checkRep();
	return _rep->existsQualifier(name);
    }

    Boolean existsQualifier(const String& name) const
    {
	_checkRep();
	return _rep->existsQualifier(name);
    }

    /**	getQualifier - Retrieves the qualifier object defined by the
	index input parameter.  @ index for the qualifier object.
	The index to qualifier objects is zero-origin and continuous
	so that incrementing loops can be used to get all qualifier
	objects in a CIMInstnace.
	@return: Returns qualifier object defined by index.
	@exception Throws the OutOfBounds exception if the index
	is out of bounds
    */
    CIMQualifier getQualifier(Uint32 pos)
    {
	_checkRep();
	return _rep->getQualifier(pos);
    }

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
    CIMConstQualifier getQualifier(Uint32 pos) const
    {
	_checkRep();
	return _rep->getQualifier(pos);
    }

    /**	getQualifierCount - Gets the numbercount of CIMQualifierobjects
	defined for this CIMObject.
	@return	Count of the number of CIMQalifier objects in the
	CIMObject.
	@exception Throws the OutOfBounds exception if the index
	is out of bounds
    */
    Uint32 getQualifierCount() const
    {
	_checkRep();
	return _rep->getQualifierCount();
    }

    /**	addProperty - Adds a property object defined by the input
	parameter to the CIMObject
	@param Property Object to be added.  See the CIM Property
	class for definition of the property object
	@return ATTN:
	@exception Throws the exception AlreadyExists if the property
	already exists.
    */
    CIMObject& addProperty(const CIMProperty& x)
    {
	_checkRep();
	_rep->addProperty(x);
	return *this;
    }

    /**	findProperty - Searches the CIMProperty objects installed in the
	CIMObject for property objects with the name defined by the
	input.
	@param String with the name of the property object to be found
	@return Position in the CIM object to the property object if found or
	PEG_NOT_FOUND if no property object found with the name defined by the
	input.
    */
    Uint32 findProperty(const String& name)
    {
	_checkRep();
	return _rep->findProperty(name);
    }

    Uint32 findProperty(const String& name) const
    {
	_checkRep();
	return _rep->findProperty(name);
    }

    /** existsPropery - Determines if a property object with the
	name defined by the input parameter exists in the class.
	@parm String parameter with the property name.
	@return True if the property object exists.
    */
    Boolean existsProperty(const String& name)
    {
	_checkRep();
	return _rep->existsProperty(name);
    }

    Boolean existsProperty(const String& name) const
    {
       _checkRep();
       return _rep->existsProperty(name);
    }

    /**	getProperty - Gets the CIMproperty object in the CIMObject defined
	by the input index parameter.
	@param Index to the property object in the CIMObject.
    	The index to qualifier objects is zero-origin and continuous
	so that incrementing loops can be used to get all qualifier
	objects in a CIMObject.
	@return CIMProperty object corresponding to the index.
	@exception Throws the OutOfBounds exception if the index
	is out of bounds

	ATTN: What is the effect of out of range?
    */
    CIMProperty getProperty(Uint32 pos)
    {
	_checkRep();
	return _rep->getProperty(pos);
    }

    /**	getProperty - Gets the CIMproperty object in the CIMObject defined
	by the input index parameter.
	@param Index to the property object in the CIMObject.
    	The index to qualifier objects is zero-origin and continuous
	so that incrementing loops can be used to get all qualifier
	objects in a CIMInstnace.
	@return CIMProperty object corresponding to the index.
	@exception Throws the OutOfBounds exception if the index
	is out of bounds

	ATTN: What is the effect of out of range?
    */
    CIMConstProperty getProperty(Uint32 pos) const
    {
	_checkRep();
	return _rep->getProperty(pos);
    }

    /** removeProperty - Removes the property represented
	by the position input parameter from the instance.
	@param pos Index to the property to be removed from the
	instance.  Normally this is obtained by getProperty();
	@exception Throws OutofBounds if index is not a property object
    */
    void removeProperty(Uint32 pos)
    {
	_checkRep();
	_rep->removeProperty(pos);
    }

    /**	getPropertyCount - Gets the numbercount of CIMProperty
	objects defined for this CIMObject.
	@return	Count of the number of CIMProperty objects in the
	CIMObject. Zero indicates that no CIMProperty objects
	are contained in the CIMObject
	@exception Throws the OutOfBounds exception if the index
	is out of bounds

    */
    Uint32 getPropertyCount() const
    {
	_checkRep();
	return _rep->getPropertyCount();
    }

    /**	operator int() - ATTN: */
    operator int() const { return _rep != 0; }

    /**	Returns true if the two classes are structurally identical.
    */
    Boolean identical(const CIMConstObject& x) const;

    /** Convert object to XML format.
    */
    void toXml(Array<Sint8>& out) const
    {
	_checkRep();
	_rep->toXml(out);
    }

    /**	Clones the given object.
    */
    CIMObject clone() const
    {
	_checkRep();
	return CIMObject(_rep->clone());
    }

private:

    CIMObject(CIMObjectRep* rep) : _rep(rep)
    {

    }

    void _checkRep() const
    {
	if (!_rep)
	    ThrowUnitializedHandle();
    }

	CIMObjectRep* _rep;

    friend class CIMConstObject;
    friend class CIMClass;
    friend class CIMConstClass;
    friend class CIMInstance;
    friend class CIMConstInstance;
};

////////////////////////////////////////////////////////////////////////////////
//
// CIMConstObject
//
////////////////////////////////////////////////////////////////////////////////

class PEGASUS_COMMON_LINKAGE CIMConstObject
{
public:

    CIMConstObject() : _rep(0)
    {

    }

    CIMConstObject(const CIMConstObject& x)
    {
	Inc(_rep = x._rep);
    }

    CIMConstObject(const CIMObject& x)
    {
	Inc(_rep = x._rep);
    }

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

    CIMConstObject& operator=(const CIMConstObject& x)
    {
	if (x._rep != _rep)
	{
	    Dec(_rep);
	    Inc(_rep = x._rep);
	}
	return *this;
    }

    CIMConstObject& operator=(const CIMObject& x)
    {
	if (x._rep != _rep)
	{
	    Dec(_rep);
	    Inc(_rep = x._rep);
	}
	return *this;
    }

    CIMConstObject& operator=(const CIMClass& x);

    CIMConstObject& operator=(const CIMConstClass& x);

    CIMConstObject& operator=(const CIMInstance& x);

    CIMConstObject& operator=(const CIMConstInstance& x);

    ~CIMConstObject()
    {
	Dec(_rep);
    }

	const CIMReference & getPath(void) const
	{
	_checkRep();
	return _rep->getPath();
	}
	
    const String& getClassName() const
    {
	_checkRep();
	return _rep->getClassName();
    }

    Uint32 findQualifier(const String& name) const
    {
	_checkRep();
	return _rep->findQualifier(name);
    }

    CIMConstQualifier getQualifier(Uint32 pos) const
    {
	_checkRep();
	return _rep->getQualifier(pos);
    }

    Uint32 getQualifierCount() const
    {
	_checkRep();
	return _rep->getQualifierCount();
    }

    Uint32 findProperty(const String& name) const
    {
	_checkRep();
	return _rep->findProperty(name);
    }

    CIMConstProperty getProperty(Uint32 pos) const
    {
	_checkRep();
	return _rep->getProperty(pos);
    }

    Uint32 getPropertyCount() const
    {
	_checkRep();
	return _rep->getPropertyCount();
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

    Boolean identical(const CIMConstObject& x) const
    {
	x._checkRep();
	_checkRep();
	return _rep->identical(x._rep);
    }

    CIMObject clone() const
    {
	return CIMObject(_rep->clone());
    }

private:

    void _checkRep() const
    {
	if (!_rep)
	    ThrowUnitializedHandle();
    }

    CIMObjectRep* _rep;

    friend class CIMObject;
    friend class CIMClass;
    friend class CIMConstClass;
    friend class CIMInstance;
    friend class CIMConstInstance;
};

/** The CIMObjectWithPath encapsulates a CIMReference and CIMObject.
    Accessors are provided for getting the two parts. Constructors are
    provided for initializing it from a CIMObject.
*/
class PEGASUS_COMMON_LINKAGE CIMObjectWithPath
{
public:

    /**	Constructor
    */
    CIMObjectWithPath();

    /** constructor
    */
    CIMObjectWithPath(const CIMReference& reference, const CIMObject& object);

    /** Constructor - Constructs a CIMObjectWithPath Object from
        another CimObjectWithPath
        @param - ATTN
    */
    CIMObjectWithPath(const CIMObjectWithPath& x);

    ~CIMObjectWithPath();

    CIMObjectWithPath& operator=(const CIMObjectWithPath& x);

    /** set -
    */
    void set(const CIMReference& reference, const CIMObject& object);

    /**
    */
    const CIMReference& getReference() const { return _reference; }

    /**
    */
    const CIMObject& getObject() const { return _object; }

    /**
    */
    CIMReference& getReference() { return _reference; }

    /**
    */
    CIMObject& getObject() { return _object; }

    /**
    */
    void toXml(Array<Sint8>& out) const;

private:

    CIMReference _reference;
    CIMObject _object;
};

PEGASUS_NAMESPACE_END

#endif /* Pegasus_Object_h */
