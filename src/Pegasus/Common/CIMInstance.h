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

#ifndef Pegasus_Instance_h
#define Pegasus_Instance_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/CIMObject.h>
#include <Pegasus/Common/CIMInstanceRep.h>

PEGASUS_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////
//
// CIMInstance
//
////////////////////////////////////////////////////////////////////////////////

class CIMConstInstance;
class CIMObject;
class CIMConstObject;

/** This class represents the instance of a CIM class. It is used manipulate 
    instances and their parts.
*/
class PEGASUS_COMMON_LINKAGE CIMInstance
{
public:

    /** Constructor - Create a CIM Instance object.
	@return  Instance created
    */
    CIMInstance() : _rep(0)
    {

    }

    /** Constructor - Create a CIMInstance object from another Instance.
	@param Instance object from which the new instance is created.
	@return New instance
    */
    CIMInstance(const CIMInstance& x)
    {
	Inc(_rep = x._rep);
    }

    PEGASUS_EXPLICIT CIMInstance(const CIMObject& x);

    PEGASUS_EXPLICIT CIMInstance(const CIMObject& x, NoThrow&);

    /** Constructor. */
    CIMInstance& operator=(const CIMInstance& x)
    {
	if (x._rep != _rep)
	{
            Dec(_rep);
	    Inc(_rep = x._rep);
	}
	return *this;
    }

    /**	Constructor - Creates an Instance object with the classname
	from the input parameters
	@param - String className to be used with new instance object
	@return The new instance object
	@exception Throws IllegalName if className argument not legal CIM
	identifier. ATTN: Clarify the defintion	of legal CIM identifier.
    */
    CIMInstance(const String& className)
    {
	_rep = new CIMInstanceRep(className);
    }

    /** Destructor. */
    ~CIMInstance()
    {
        Dec(_rep);
    }

    /**	getClassName - 	Returns the class name of the instance
	@return String with the class name.
    */
    const String& getClassName() const
    {
	_checkRep();
	return _rep->getClassName();
    }

    /** equalClassName compares Name of the class with a String. This test performs
	a comparison of the classname component of the object
	with a String.	Note that this function was included specifically
	because the equality compare is not just a straight comparison
	because classnames are case independent.
	@param classname String containing the name for comparison
	@return True if it is the same class name (equalNoCase compare passes)
	or false if not.
    */
    const Boolean equalClassName(const String& classname) const
    {
	_checkRep();
	return _rep->equalClassName(classname);

    }

    const CIMReference& getPath() const
    {
	_checkRep();
	return _rep->getPath();
    }

    /**	addQualifier - Adds the CIMQualifier object to the instance.
	Thows an exception of the CIMQualifier already exists in the instance
	@param CIMQualifier object to add to instance
	@return ATTN:
	@exception Throws AlreadyExists.
    */
    CIMInstance& addQualifier(const CIMQualifier& qualifier)
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
	defined for this CIMInstance.
	@return	Count of the number of CIMQalifier objects in the
	CIMInstance.
	@exception Throws the OutOfBounds exception if the index
	is out of bounds
    */
    Uint32 getQualifierCount() const
    {
	_checkRep();
	return _rep->getQualifierCount();
    }

    /**	addProperty - Adds a property object defined by the input
	parameter to the CIMInstance
	@param Property Object to be added.  See the CIM Property
	class for definition of the property object
	@return ATTN:
	@exception Throws the exception AlreadyExists if the property
	already exists.
    */
    CIMInstance& addProperty(const CIMProperty& x)
    {
	_checkRep();
	_rep->addProperty(x);
	return *this;
    }

    /**	findProperty - Searches the CIMProperty objects installed in the
	CIMInstance for property objects with the name defined by the
	input.
	@param String with the name of the property object to be found
	@return Position in the CIM Instance to the property object if found or
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
    CIMProperty getProperty(Uint32 pos)
    {
	_checkRep();
	return _rep->getProperty(pos);
    }

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
	objects defined for this CIMInstance.
	@return	Count of the number of CIMProperty objects in the
	CIMInstance. Zero indicates that no CIMProperty objects
	are contained in the CIMInstance
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

    /**	resolve - ATTN: */
    void resolve(
	DeclContext* declContext, 
	const String& nameSpace,
	Boolean propagateQualifiers);

    void resolve(
	DeclContext* declContext,
	const String& nameSpace,
	CIMConstClass& cimClassOut,
	Boolean propagateQualifiers)
    {
	_checkRep();
	_rep->resolve(declContext, nameSpace, cimClassOut, propagateQualifiers);
    }

    /**	toXml - Creates an XML transformation of the CIMInstance
	compatiblewith the DMTF CIM Operations over HTTP defintions.
	@return
	ATTN: This is incorrect and needs to be corrected.
    */
    void toXml(Array<Sint8>& out) const
    {
	_checkRep();
	_rep->toXml(out);
    }

    /**	prints the class in XML format. */
    void print(PEGASUS_STD(ostream) &o=PEGASUS_STD(cout)) const
    {
	_checkRep();
	_rep->print(o);
    }

    /**	toMof - Creates an MOF transformation of the CIMInstance
	compatiblewith the DMTF specification.
	@return
    */
    void toMof(Array<Sint8>& out) const
    {
	_checkRep();
	_rep->toMof(out);
    }
    /**	identical - Compares the CIMInstance with another CIMInstance
	defined by the input parameter for equality of all components.
	@param CIMInstance to be compared
	@return Boolean true if they are identical
    */
    Boolean identical(const CIMConstInstance& x) const;

    /**	CIMMethod

    */
    CIMInstance clone() const
    {
	return CIMInstance((CIMInstanceRep*)(_rep->clone()));
    }

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
    CIMReference getInstanceName(const CIMConstClass& cimClass) const
    {
	_checkRep();
	return _rep->getInstanceName(cimClass);
    }

    String toString() const
    {
	_checkRep();
	return _rep->toString();
    }

private:

    CIMInstance(CIMInstanceRep* rep) : _rep(rep)
    {
    }

    void _checkRep() const
    {
	if (!_rep)
	    ThrowUnitializedHandle();
    }

    CIMInstanceRep* _rep;
    friend class CIMConstInstance;
    friend class CIMObject;
    friend class CIMConstObject;
};

////////////////////////////////////////////////////////////////////////////////
//
// CIMConstInstance
//
////////////////////////////////////////////////////////////////////////////////

class PEGASUS_COMMON_LINKAGE CIMConstInstance
{
public:

    CIMConstInstance() : _rep(0)
    {

    }

    CIMConstInstance(const CIMConstInstance& x)
    {
	Inc(_rep = x._rep);
    }

    CIMConstInstance(const CIMInstance& x)
    {
	Inc(_rep = x._rep);
    }

    PEGASUS_EXPLICIT CIMConstInstance(const CIMObject& x);

    PEGASUS_EXPLICIT CIMConstInstance(const CIMConstObject& x);

    PEGASUS_EXPLICIT CIMConstInstance(const CIMObject& x, NoThrow&);

    PEGASUS_EXPLICIT CIMConstInstance(const CIMConstObject& x, NoThrow&);

    CIMConstInstance& operator=(const CIMConstInstance& x)
    {
	if (x._rep != _rep)
	{
            Dec(_rep);
	    Inc(_rep = x._rep);
	}
	return *this;
    }

    CIMConstInstance& operator=(const CIMInstance& x)
    {
	if (x._rep != _rep)
	{
            Dec(_rep);
	    Inc(_rep = x._rep);
	}
	return *this;
    }

    // Throws IllegalName if className argument not legal CIM identifier.

    CIMConstInstance(const String& className)
    {
	_rep = new CIMInstanceRep(className);
    }

    ~CIMConstInstance()
    {
        Dec(_rep);
    }

    const String& getClassName() const
    {
	_checkRep();
	return _rep->getClassName();
    }

    const Boolean equalClassName(const String& classname) const
    {
	_checkRep();
	return _rep->equalClassName(classname);

    }

    const CIMReference& getPath() const
    {
	_checkRep();
	return _rep->getPath();
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

    Boolean identical(const CIMConstInstance& x) const
    {
	x._checkRep();
	_checkRep();
	return _rep->identical(x._rep);
    }

    CIMInstance clone() const
    {
	return CIMInstance((CIMInstanceRep*)(_rep->clone()));
    }

    CIMReference getInstanceName(const CIMConstClass& cimClass) const
    {
	_checkRep();
	return _rep->getInstanceName(cimClass);
    }

    String toString() const
    {
	_checkRep();
	return _rep->toString();
    }

private:

    void _checkRep() const
    {
	if (!_rep)
	    ThrowUnitializedHandle();
    }

    CIMInstanceRep* _rep;
    friend class CIMInstance;
    friend class CIMObject;
    friend class CIMConstObject;
};

#define PEGASUS_ARRAY_T CIMInstance
# include "ArrayInter.h"
#undef PEGASUS_ARRAY_T

PEGASUS_NAMESPACE_END

#endif /* Pegasus_Instance_h */
