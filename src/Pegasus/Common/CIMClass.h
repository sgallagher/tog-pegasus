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

#ifndef Pegasus_CIMClass_h
#define Pegasus_CIMClass_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/Linkage.h>
#include <Pegasus/Common/CIMName.h>
#include <Pegasus/Common/CIMObject.h>
#include <Pegasus/Common/CIMMethod.h>

PEGASUS_NAMESPACE_BEGIN

class CIMConstClass;
class CIMClassRep;
class Resolver;

// REVIEW: redocument.

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
	@exception "unitialized handle" if this
	unitialized handle is used
    */
    CIMClass();

    /** Constructor - Creates a class from a previous class
    */
    CIMClass(const CIMClass& x);

    /**	Constructor - Creates a Class object from the given CIMObject
	@param x CIMObject from which to create the CIMClass
	@exception DynamicCastFailedException If a CIMClass can not be
        created from the given CIMObject
    */
    PEGASUS_EXPLICIT CIMClass(const CIMObject& x);

    /**	Constructor - Creates a Class from inputs of a classname and
	SuperClassName
	@param className CIMName representing name of the class being created
	@param superClassName CIMName representing name of the SuperClass
	<pre>
	    CIMClass NewClass("MyClass", "YourClass");
	</pre>

    */
    CIMClass(
	const CIMName& className,
	const CIMName& superClassName = CIMName());

    /** Assignment operator.
    */
    CIMClass& operator=(const CIMClass& x);

    /// Destructor
    ~CIMClass();

    /** isAssociation - Identifies whether or not this CIM class
	is an association. An association is a relationship between two
	(or more) classes or instances of two classes.  The properties of an
	association class include pointers, or references, to the two (or
	more) instances. All CIM classes can be included in one or more
	associations.
	@return  Boolean True if this CIM class belongs to an association;
	otherwise, false.
    */
    Boolean isAssociation() const;

    /** isAbstract Test if the CIMClass is abstract.
	@return - True if the CIMClass Object is abstract
	SeeAlso: Abstract
    */
    Boolean isAbstract() const;

    // ATTN: COMMENT. Why not just get name so we have common method for all.
    /** getClassName Gets the name of the class
	@return Returns CIMName with the class name.
    */
    const CIMName& getClassName() const;
        
    const CIMObjectPath& getPath() const;

    /**
      Sets the object path for the class
      @param  path  CIMObjectPath containing the object path
     */
    void setPath (const CIMObjectPath & path);

    /** getSuperClassName - Gets the name of the Parent
	@return CIMName with parent class name.
    */
    const CIMName& getSuperClassName() const;

    /**	setSuperClassName - Sets the name of the parent class from
	the input parameter. \REF{CLASSNAME}.
	@param CIMName defining parent name.
    */
    void setSuperClassName(const CIMName& superClassName);

    /** addQualifier - Adds the specified qualifier to the class
	and increments the qualifier count. It is illegal to add the same
	qualifier more than one time.
	@param qualifier CIMQualifier object representing the qualifier to be
	added
	@return Returns handle of the class object
	@exception AlreadyExistsException.
    */
    CIMClass& addQualifier(const CIMQualifier& qualifier);

    /**	findQualifier - Searches for a qualifier with the specified `
        input name if it exists in the class
	@param name CIMName of the qualifier
	to be found @return Position of the qualifier in the Class.
	@return Returns index of the qualifier found or PEG_NOT_FOUND
	if not found.
    */
    Uint32 findQualifier(const CIMName& name) const;

    /**	getQualifier - Gets the CIMQualifier object defined
	by the input parameter
	@param index defines the index of the qualifier in the class from the
	findQualifier method
	@return CIMQualifier object representing the qualifier found. On error,
	    CIMQualifier handle will be null.
    */
    CIMQualifier getQualifier(Uint32 index);

    /** getQualifier - Gets the qualifier defined by the input parameter
	from the qualifier list for this CIMClass.
    */
    CIMConstQualifier getQualifier(Uint32 index) const;

    /** removeQualifier - Removes the qualifier defined by the
    index parameter.
    @param Defines the index of the qualifier to be removed.
    @return There is no return.
    @exception IndexOutOfBoundsException if the index is outside
    the range of existing qualifier objects for this class
    */
    void removeQualifier(Uint32 index);

    /** getQualifierCount - Returns the number of qualifiers
	in the class.
	@return the number of qualifiers on the class definition (not
	those on properties or methods)
    */
    Uint32 getQualifierCount() const;

    /**	addProperty - Adds the specified property object to the
	properties in the CIM class
    */
    CIMClass& addProperty(const CIMProperty& x);

    /** findProperty - Finds the property object with the
	name defined by the input parameter in the class.
	@param CIMName parameter with the property name.
	@return index representing the property object found or
	PEG_NOT_FOUND if the property is not found.
    */
    Uint32 findProperty(const CIMName& name) const;

    /** getProperty - Returns a property representing the property
	defined by the input parameter
	@param index Index for this property
	@return CIMProperty object
        @exception IndexOutOfBoundsException if the index is outside
        the range of properties in this class
    */
    CIMProperty getProperty(Uint32 index);

    /**getProperty Gets a property object from the CIMClass
    	@param index The index of the property object to get.
    	@return Returns handle of the property object requested
        @exception IndexOutOfBoundsException if the index is outside
        the range of properties in this class
    */
    CIMConstProperty getProperty(Uint32 index) const;

    /** removeProperty - Remove the specified property from the class
	@param index Index of the property to be removed
        @exception IndexOutOfBoundsException if the index is outside
        the range of properties in this class
    */
    void removeProperty(Uint32 index);

    /** getPropertyCount -   Gets the count of the number of properties
	defined in the class.
	@return count of number of properties in the class
    */
    Uint32 getPropertyCount() const;

    /** addMethod - Adds the method object defined by the input
	parameter to the class and increments the count of the number of
	methods in the class
	@param method object representing the method to be added
	@return Returns the CIMClass object to which the method was added.
	@exception AlreadyExistsException if the method already exists
	@exception UninitializedObjectException if the object is not initialized
    */
    CIMClass& addMethod(const CIMMethod& x);

    /** findMethod - Locate the method object defined by the
	name input
	@param CIMName representing the name of the method to be found
	@return Position of the method object in the class to be used in
	subsequent getmethod, etc. operations
    */
    Uint32 findMethod(const CIMName& name) const;

    /** getMethod - Gets the method object defined by the
	input parameter.
	@param index Index to the method object to get
	@return Returns handle of the method requested
        @exception IndexOutOfBoundsException if the index is outside
        the range of methods in this class
    */
    CIMMethod getMethod(Uint32 index);

    /** getMethod Gets the method object defined by the input
    parameter. This is the const version.
    */

    CIMConstMethod getMethod(Uint32 index) const;

    /** removeMethod - Removes the method defined by the
    index parameter.
    @param Defines the index of the method to be removed.
    @return There is no return.
    @exception Throw OutOfBound exception if the index is outside
    the range of existing method objects for this class
    */
    void removeMethod(Uint32 index);

    /** getMethodCount - Count of the number of methods in the class
	@return integer representing the number of methods in the class object.
    */
    Uint32 getMethodCount() const;

    /** Get names of all keys of this class. */
    void getKeyNames(Array<CIMName>& keyNames) const;

    Boolean hasKeys() const;

    /** Makes a deep copy (clone) of the given object. */
    CIMClass clone() const;

    // ATTN: Clarify exactly what identical means
    /** identical -  Compares with another class
	@param Class object for the class to be compared
	@return True if the classes are identical
    */
    Boolean identical(const CIMConstClass& x) const;

    /**
        Determines if the object has not been initialized.

        @return  True if the object has not been initialized,
                 False otherwise
     */
    Boolean isUninitialized() const;

private:

    CIMClassRep* _rep;

    CIMClass(CIMClassRep* rep);

    void _checkRep() const;

    friend class CIMConstClass;
    friend class CIMObject;
    friend class CIMConstObject;
    friend class Resolver;
    friend class XmlWriter;
    friend class MofWriter;
};

#define PEGASUS_ARRAY_T CIMClass
#include <Pegasus/Common/ArrayInter.h>
#undef PEGASUS_ARRAY_T

// ATTN document this
/** CIMConstClass
*/
class PEGASUS_COMMON_LINKAGE CIMConstClass
{
public:

    CIMConstClass();

    CIMConstClass(const CIMConstClass& x);

    CIMConstClass(const CIMClass& x);

    PEGASUS_EXPLICIT CIMConstClass(const CIMObject& x);

    PEGASUS_EXPLICIT CIMConstClass(const CIMConstObject& x);

    CIMConstClass(
	const CIMName& className,
	const CIMName& superClassName = CIMName());

    CIMConstClass& operator=(const CIMConstClass& x);

    CIMConstClass& operator=(const CIMClass& x);

    ~CIMConstClass();

    Boolean isAssociation() const;

    Boolean isAbstract() const;

    const CIMName& getClassName() const;

    const CIMObjectPath& getPath() const;

    const CIMName& getSuperClassName() const;

    Uint32 findQualifier(const CIMName& name) const;

    CIMConstQualifier getQualifier(Uint32 index) const;

    Uint32 getQualifierCount() const;

    Uint32 findProperty(const CIMName& name) const;

    CIMConstProperty getProperty(Uint32 index) const;

    Uint32 getPropertyCount() const;

    Uint32 findMethod(const CIMName& name) const;

    CIMConstMethod getMethod(Uint32 index) const;

    Uint32 getMethodCount() const;

    void getKeyNames(Array<CIMName>& keyNames) const;

    Boolean hasKeys() const;

    CIMClass clone() const;

    Boolean identical(const CIMConstClass& x) const;

    Boolean isUninitialized() const;

private:

    CIMClassRep* _rep;

    void _checkRep() const;

    friend class CIMClassRep;
    friend class CIMClass;
    friend class CIMInstanceRep;
    friend class CIMObject;
    friend class CIMConstObject;
    friend class XmlWriter;
    friend class MofWriter;
};

PEGASUS_NAMESPACE_END

#endif /* Pegasus_CIMClass_h */
