//%/////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000 The Open Group, BMC Software, Tivoli Systems, IBM
//
// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation
// the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following conditions:
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
// THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.
//
//==============================================================================
//
// Author: Mike Brasher (mbrasher@bmc.com)
//
// Modified By:
//
//%/////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_CIMClass_h
#define Pegasus_CIMClass_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/CIMClassRep.h>

PEGASUS_NAMESPACE_BEGIN

class CIMConstClass;

/** The CIMClass class is used to represent CIM classes in Pegasus.  In CIM,
    a class object may be a class or an associator.  A CIM class must contain a
    name and may contain methods, properties, and qualifiers.  It is a template
    for creating a CIM instance.  A CIM class represents a collection of CIM
    instances, all of which support a common type (for example, a set of
    properties, methods, and associations).
*/

class PEGASUS_COMMON_LINKAGE CIMClass
{
public:

    /** Constructor - Creates an uninitiated a new CIM object
	reprenting a CIM class. The class object created by this
	constructor can only be used in an operation such as the
	copy constructor.  It cannot be used to create a class by
	appending names, properties, etc. since it is unitiated.

	Use one of the other constructors to create an initiated new CIM class
	object.
	@exception Throws an exception "unitialized handle" if this
	unitialized handle is used
	/REF(HPEGASUS_HANDLES)
    */
    CIMClass() : _rep(0)
    {

    }

    /** Constructor - Creates a class from a previous class
    */
    CIMClass(const CIMClass& x)
    {
	Inc(_rep = x._rep);
    }

    /// Operator = Assigns the CIM Class constructor.
    CIMClass& operator=(const CIMClass& x)
    {
	if (x._rep != _rep)
	{
	    Dec(_rep);
	    Inc(_rep = x._rep);
	}
	return *this;
    }

    /**	 Constructor - Creates a Class from inputs of a classname and
	SuperClassName
	@param className String representing name of the class being created
	@param superClassName String representing name of the SuperClass
	ATTN: Define what makes up legal name.
	@return Throws IllegalName if className argument illegal CIM identifier.
	<pre>
	    CIMClass NewCass("MyClass", "YourClass");
	</pre>

    */
    CIMClass(
	const String& className,
	const String& superClassName = String())
    {
	_rep = new CIMClassRep(className, superClassName);
    }

    /// Destructor
    ~CIMClass()
    {
	Dec(_rep);
    }

    /** isAssociation - Identifies whether or not this CIM class
	is an association. An association is a relationship between two
	(or more) classes or instances of two classes.  The properties of an
	association class include pointers, or references, to the two (or
	more) instances. All CIM classes can be included in one or more
	associations.
	ATTN: Move the association definition elsewhere
	@return  Boolean True if this CIM class belongs to an association;
	otherwise, false.
    */
    Boolean isAssociation() const
    {
	_checkRep();
	return _rep->isAssociation();
    }

    ///	 CIMMethod isAbstract
    Boolean isAbstract() const
    {
	_checkRep();
	return _rep->isAbstract();
    }

    /** getClassName Gets the name of the class
	ATTN: COMMENT. Why not just get name so we have common method for all.
	@return Returns string with the class name.
    */
    const String& getClassName() const
    {
	_checkRep();
	return _rep->getClassName();
    }

    /** CIMMethod getSuperClassName - Gets the name of the Parent
	@return String with parent class name.
    */
    const String& getSuperClassName() const
    {
	_checkRep();
	return _rep->getSuperClassName();
    }

    /**	CIMMethod setSuperClassName - Sets the name of the parent class from
	the input parameter. \REF{CLASSNAME}. ATTN: Define legal classnames
	@param String defining parent name.
	@return Throws IllegalName if superClassName argument not legal CIM
	identifier
	@exception throws IllegalName if the name is not correct. See
	\URL[ClassNames]{DefinitionofTerms.html#CLASSNAME}
    */
    void setSuperClassName(const String& superClassName)
    {
	_checkRep();
	_rep->setSuperClassName(superClassName);
    }

    /** addQualifier - Adds the specified qualifier to the class
	and increments the qualifier count. It is illegal to add the same
	qualifier more than one time.
	@param qualifier CIMQualifier object representing the qualifier to be
	added
	@return Returns handle of the class object
	@exception Throws AlreadyExists.
    */
    CIMClass& addQualifier(const CIMQualifier& qualifier)
    {
	_checkRep();
	_rep->addQualifier(qualifier);
	return *this;
    }

    /**	findQualifier - Searches for a qualifier with the specified `
        input name if it exists in the class
	@param name CIMName of the qualifier
	to be found @return Position of the qualifier in the Class.
	@return Returns index of the qualifier found or PEG_NOT_FOUND
	if not found.
    */
    Uint32 findQualifier(const String& name)
    {
	_checkRep();
	return _rep->findQualifier(name);
    }
    ///
    Uint32 findQualifier(const String& name) const
    {
	_checkRep();
	return _rep->findQualifier(name);
    }
    /** existsQualifier - Returns true if the qualifier with the
    specified name exists in the class
    @param name String name of the qualifier object being tested.
    @return True if the qualifier exits.  Otherwise false is returned.
    */
    Boolean existsQualifier(const String& name)
    {
	_checkRep();
	return _rep->existsQualifier(name);
    }
    ///
    Boolean existsQualifier(const String& name) const
    {
	_checkRep();
	return _rep->existsQualifier(name);
    }

    /**	getQualifier - Gets the CIMQualifier object defined
	by the input parameter
	@param pos defines the position of the qualifier in the class from the
	findQualifier method
	@return CIMQualifier object representing the qualifier found.
	ATTN: what is error return here?
    */
    CIMQualifier getQualifier(Uint32 pos)
    {
	_checkRep();
	return _rep->getQualifier(pos);
    }

    /// getQualifier - ATTN:
    CIMConstQualifier getQualifier(Uint32 pos) const
    {
	_checkRep();
	return _rep->getQualifier(pos);
    }

    /** removeQualifier - Removes the qualifier defined by the
    index parameter.
    @param Defines the index of the qualifier to be removed.
    @return There is no return.
    @exception Throw OutOfBound exception if the index is outside
    the range of existing qualifier objects for this class
    */
    void removeQualifier(Uint32 pos) const
    {
	_checkRep();
	_rep->removeQualifier(pos);
    }

    /** getQualifierCount - Returns the number of qualifiers
	in the class.
	@return ATTN:
    */
    Uint32 getQualifierCount() const
    {
	_checkRep();
	return _rep->getQualifierCount();
    }

    /**	addProperty - Adds the specified property object to the
	properties in the CIM class
    */
    CIMClass& addProperty(const CIMProperty& x)
    {
	_checkRep();
	_rep->addProperty(x);
	return *this;
    }



    /** CIMMethod findProperty - Finds the property object with the
	name defined by the input parameter in the class.
	@param String parameter with the property name.
	@return position representing the property object found or
	PEG_NOT_FOUND if the property is not found.
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

    /** getProperty - Returns a property representing the property
	defined by the input parameter
	@param position for this property
	ATTN: Should we not use something like handle for position???
	@return CIMProperty object
	ATTN: what is error return?
    */
    CIMProperty getProperty(Uint32 pos)
    {
	_checkRep();
	return _rep->getProperty(pos);
    }

    /**getProperty Gets a property object from the CIMClass
    	@param pos The index of the property object to get.
    	@return Returns handle of the property object requested
    	@exception Throws OutofBounds if the size field is greather than the
    	bunber of properties in the class.
    */
    CIMConstProperty getProperty(Uint32 pos) const
    {
	_checkRep();
	return _rep->getProperty(pos);
    }

    /** removeProperty - Removes the property represented
	by the position input parameter from the class
	@param pos Index to the property to be removed from the
	findPropety method
	@exception Throws OutofBounds if index is not a property object
    */
    void removeProperty(Uint32 pos)
    {
	_checkRep();
	_rep->removeProperty(pos);
    }

    /** getPropertyCount -   Gets the count of the number of properties
	defined in the class.
	@return count of number of proerties in the class
    */
    Uint32 getPropertyCount() const
    {
	_checkRep();
	return _rep->getPropertyCount();
    }

    /** addMethod - Adds the method object defined by the input
	parameter to the class and increments the count of the number of
	methods in the class
	@param method object representing the method to be added
	@return Returns the CIMClass object to which the method was added.
	@exception Throws AlreadyExists if the method already exists and throws
	UnitializedHandle if the handle is not initialized
    */
    CIMClass& addMethod(const CIMMethod& x)
    {
	_checkRep();
	_rep->addMethod(x);
	return *this;
    }

    /** findMethod - Locate the method object defined by the
	name input
	@param String representing the name of the method to be found
	@return Position of the method object in the class to be used in
	subsequent getmethod, etc. operations
    */
    Uint32 findMethod(const String& name)
    {
	_checkRep();
	return _rep->findMethod(name);
    }

    Uint32 findMethod(const String& name) const
    {
	_checkRep();
	return _rep->findMethod(name);
    }

     /** existsMethod - Determine if the method object defined by the
	name input exists
	@param String representing the name of the method to be found
	@return True if the method exists
    */
    Boolean existsMethod(const String& name)
    {
	_checkRep();
	return _rep->existsMethod(name);
    }

    Boolean existsMethod(const String& name) const
    {
	_checkRep();
	return _rep->existsMethod(name);
    }


    /** getMethod - Gets the method object defined by the
	input parameter.
	@param pos Index to the CIMMethod object to get
	@return Returns handle of the CIMMethod requested
	@exception Throws OutofBounds if the index represented by pos is greater
	than the number of methods defined in the class object
    */
    CIMMethod getMethod(Uint32 pos)
    {
	_checkRep();
	return _rep->getMethod(pos);
    }

    /** getMethod Gets the method object defined by the input
    parameter. This is the const version.
    */

    CIMConstMethod getMethod(Uint32 pos) const
    {
	_checkRep();
	return _rep->getMethod(pos);
    }

    /** removeMethod - Removes the method defined by the
    index parameter.
    @param Defines the index of the method to be removed.
    @return There is no return.
    @exception Throw OutOfBound exception if the index is outside
    the range of existing method objects for this class
    */
    void removeMethod(Uint32 pos) const
    {
	_checkRep();
	_rep->removeMethod(pos);
    }

    /** CIMMethod getMethodCount - Count of the number of methods in the class
	@return integer representing the number of methods in the class object.
    */
    Uint32 getMethodCount() const
    {
	_checkRep();
	return _rep->getMethodCount();
    }

    /** Resolve -  Resolve the class: inherit any properties and
	qualifiers. Make sure the superClass really exists and is consistent
	with this class. Also set the propagated flag class-origin for each
	class feature.
	ATTN: explain why this here
    */
    void resolve(
	DeclContext* declContext,
	const String& nameSpace)
    {
	_checkRep();
	_rep->resolve(declContext, nameSpace);
    }

    /// operator - ATTN:
    operator int() const { return _rep != 0; }

    /// CIMMethod toXML
    void toXml(Array<Sint8>& out) const
    {
	_checkRep();
	_rep->toXml(out);
    }

    /// CIMMethod print
    void print(PEGASUS_STD(ostream) &o=PEGASUS_STD(cout)) const
    {
	_checkRep();
	_rep->print(o);
    }

    /** CIMMethod identical -  Compares with another class
	ATTN: Clarify exactly what identical means
	@param Class object for the class to be compared
	@return True if the classes are identical
    */
    Boolean identical(const CIMConstClass& x) const;

    /// CIMMethod clone - ATTN:
    CIMClass clone() const
    {
	return CIMClass(_rep->clone());
    }

    void getKeyNames(Array<String>& keyNames) const
    {
	_checkRep();
	_rep->getKeyNames(keyNames);
    }

private:

    CIMClass(CIMClassRep* rep) : _rep(rep)
    {
    }

    void _checkRep() const
    {
	if (!_rep)
	    ThrowUnitializedHandle();
    }

    CIMClassRep* _rep;
    friend class CIMConstClass;
};

#define PEGASUS_ARRAY_T CIMClass
# include "ArrayInter.h"
#undef PEGASUS_ARRAY_T

/** CIMConstClass - ATTN: define this.

*/
class PEGASUS_COMMON_LINKAGE CIMConstClass
{
public:

    CIMConstClass() : _rep(0)
    {

    }

    CIMConstClass(const CIMConstClass& x)
    {
	Inc(_rep = x._rep);
    }

    CIMConstClass(const CIMClass& x)
    {
	Inc(_rep = x._rep);
    }

    CIMConstClass& operator=(const CIMConstClass& x)
    {
	if (x._rep != _rep)
	{
	    Dec(_rep);
	    Inc(_rep = x._rep);
	}
	return *this;
    }

    CIMConstClass& operator=(const CIMClass& x)
    {
	if (x._rep != _rep)
	{
	    Dec(_rep);
	    Inc(_rep = x._rep);
	}
	return *this;
    }

    // Throws IllegalName if className argument not legal CIM identifier.

    CIMConstClass(
	const String& className,
	const String& superClassName = String())
    {
	_rep = new CIMClassRep(className, superClassName);
    }

    ~CIMConstClass()
    {
	Dec(_rep);
    }

    Boolean isAssociation() const
    {
	_checkRep();
	return _rep->isAssociation();
    }

    Boolean isAbstract() const
    {
	_checkRep();
	return _rep->isAbstract();
    }

    const String& getClassName() const
    {
	_checkRep();
	return _rep->getClassName();
    }

    const String& getSuperClassName() const
    {
	_checkRep();
	return _rep->getSuperClassName();
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

    Uint32 findMethod(const String& name) const
    {
	_checkRep();
	return _rep->findMethod(name);
    }

    CIMConstMethod getMethod(Uint32 pos) const
    {
	_checkRep();
	return _rep->getMethod(pos);
    }

    Uint32 getMethodCount() const
    {
	_checkRep();
	return _rep->getMethodCount();
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

    Boolean identical(const CIMConstClass& x) const
    {
	x._checkRep();
	_checkRep();
	return _rep->identical(x._rep);
    }

    CIMClass clone() const
    {
	return CIMClass(_rep->clone());
    }

    void getKeyNames(Array<String>& keyNames) const
    {
	_checkRep();
	_rep->getKeyNames(keyNames);
    }

private:

    void _checkRep() const
    {
	if (!_rep)
	    ThrowUnitializedHandle();
    }

    CIMClassRep* _rep;

    friend class CIMClassRep;
    friend class CIMClass;
    friend class CIMInstanceRep;
};

PEGASUS_NAMESPACE_END

#endif /* Pegasus_CIMClass_h */
