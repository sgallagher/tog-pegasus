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

#ifndef Pegasus_CIMClass_h
#define Pegasus_CIMClass_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/CIMObject.h>
#include <Pegasus/Common/CIMMethod.h>

PEGASUS_NAMESPACE_BEGIN

class CIMConstClass;
class CIMClassRep;
class DeclContext;

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
	@exception Throws an exception "unitialized handle" if this
	unitialized handle is used
	/REF(HPEGASUS_HANDLES)
    */
    CIMClass();

    /** Constructor - Creates a class from a previous class
    */
    CIMClass(const CIMClass& x);

    PEGASUS_EXPLICIT CIMClass(const CIMObject& x)
        throw(DynamicCastFailed);

    /**	Constructor - Creates a Class from inputs of a classname and
	SuperClassName
	@param className CIMReference representing name of the class being created
	@param superClassName String representing name of the SuperClass
	ATTN: Define what makes up legal name.
	@return Throws IllegalName if className argument illegal CIM identifier.
	<pre>
	    CIMClass NewCass("MyClass", "YourClass");
	</pre>

    */
    CIMClass(
	const CIMReference& reference,
	const String& superClassName = String::EMPTY);

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

    /** getClassName Gets the name of the class
	ATTN: COMMENT. Why not just get name so we have common method for all.
	@return Returns string with the class name.
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

    /** getSuperClassName - Gets the name of the Parent
	@return String with parent class name.
    */
    const String& getSuperClassName() const;

    /**	setSuperClassName - Sets the name of the parent class from
	the input parameter. \REF{CLASSNAME}. ATTN: Define legal classnames
	@param String defining parent name.
	@return Throws IllegalName if superClassName argument not legal CIM
	identifier
	@exception throws IllegalName if the name is not correct. See
	\URL[ClassNames]{DefinitionofTerms.html#CLASSNAME}
    */
    void setSuperClassName(const String& superClassName);

    /** addQualifier - Adds the specified qualifier to the class
	and increments the qualifier count. It is illegal to add the same
	qualifier more than one time.
	@param qualifier CIMQualifier object representing the qualifier to be
	added
	@return Returns handle of the class object
	@exception Throws AlreadyExists.
    */
    CIMClass& addQualifier(const CIMQualifier& qualifier);

    /**	findQualifier - Searches for a qualifier with the specified `
        input name if it exists in the class
	@param name CIMName of the qualifier
	to be found @return Position of the qualifier in the Class.
	@return Returns index of the qualifier found or PEG_NOT_FOUND
	if not found.
    */
    Uint32 findQualifier(const String& name) const;

    /** existsQualifier - Returns true if the qualifier with the
    specified name exists in the class
    @param name String name of the qualifier object being tested.
    @return True if the qualifier exits.  Otherwise false is returned.
    */
    Boolean existsQualifier(const String& name) const;

    /** isTrueQualifier - Determines if the qualifier defined by
	the input parameter exists for the class, is Boolean, and
	has a value of true.
	This function is useful to quickly determine the state of a
	qualifier.
	@param String containing the qualifier  name.
	@return Boolean True if the qualifier exists, 
    */
    Boolean isTrueQualifier(const String& name) const;

/**	getQualifier - Gets the CIMQualifier object defined
	by the input parameter
	@param pos defines the position of the qualifier in the class from the
	findQualifier method
	@return CIMQualifier object representing the qualifier found. On error,
	    CIMQualifier handle will be null.
    */
    CIMQualifier getQualifier(Uint32 pos);

    /** getQualifier - Gets the qualifier defined by the input parameter
		from the qualifier list for this CIMClass.
	*/
    CIMConstQualifier getQualifier(Uint32 pos) const;

    /** removeQualifier - Removes the qualifier defined by the
    index parameter.
    @param Defines the index of the qualifier to be removed.
    @return There is no return.
    @exception Throw OutOfBound exception if the index is outside
    the range of existing qualifier objects for this class
    */
    void removeQualifier(Uint32 pos);

    /** getQualifierCount - Returns the number of qualifiers
	in the class.
	@return ATTN:
    */
    Uint32 getQualifierCount() const;

    /**	addProperty - Adds the specified property object to the
	properties in the CIM class
    */
    CIMClass& addProperty(const CIMProperty& x);

    /** findProperty - Finds the property object with the
	name defined by the input parameter in the class.
	@param String parameter with the property name.
	@return position representing the property object found or
	PEG_NOT_FOUND if the property is not found.
    */
    Uint32 findProperty(const String& name) const;

    /** existsProperty - Determines if a property object with the
	name defined by the input parameter exists in the class.
	@parm String parameter with the property name.
	@return True if the property object exists.
    */
    Boolean existsProperty(const String& name) const;

    /** getProperty - Returns a property representing the property
	defined by the input parameter
	@param position for this property
	ATTN: Should we not use something like handle for position???
	@return CIMProperty object
	ATTN: what is error return?
    */
    CIMProperty getProperty(Uint32 pos);

    /**getProperty Gets a property object from the CIMClass
    	@param pos The index of the property object to get.
    	@return Returns handle of the property object requested
    	@exception Throws OutofBounds if the size field is greather than the
    	bunber of properties in the class.
    */
    CIMConstProperty getProperty(Uint32 pos) const;

    /** removeProperty - Removes the property represented
	by the position input parameter from the class
	@param pos Index to the property to be removed from the
	findPropety method
	@exception Throws OutofBounds if index is not a property object
    */
    void removeProperty(Uint32 pos);

    /** getPropertyCount -   Gets the count of the number of properties
	defined in the class.
	@return count of number of proerties in the class
    */
    Uint32 getPropertyCount() const;

    /** addMethod - Adds the method object defined by the input
	parameter to the class and increments the count of the number of
	methods in the class
	@param method object representing the method to be added
	@return Returns the CIMClass object to which the method was added.
	@exception Throws AlreadyExists if the method already exists and throws
	UninitializedHandle if the handle is not initialized
    */
    CIMClass& addMethod(const CIMMethod& x);

    /** findMethod - Locate the method object defined by the
	name input
	@param String representing the name of the method to be found
	@return Position of the method object in the class to be used in
	subsequent getmethod, etc. operations
    */
    Uint32 findMethod(const String& name) const;

     /** existsMethod - Determine if the method object defined by the
	name input exists
	@param String representing the name of the method to be found
	@return True if the method exists
    */
    Boolean existsMethod(const String& name) const;

    /** getMethod - Gets the method object defined by the
	input parameter.
	@param pos Index to the method object to get
	@return Returns handle of the method requested
	@exception Throws OutofBounds if the index represented by pos is greater
	than the number of methods defined in the class object
    */
    CIMMethod getMethod(Uint32 pos);

    /** getMethod Gets the method object defined by the input
    parameter. This is the const version.
    */

    CIMConstMethod getMethod(Uint32 pos) const;

    /** removeMethod - Removes the method defined by the
    index parameter.
    @param Defines the index of the method to be removed.
    @return There is no return.
    @exception Throw OutOfBound exception if the index is outside
    the range of existing method objects for this class
    */
    void removeMethod(Uint32 pos);

    /** getMethodCount - Count of the number of methods in the class
	@return integer representing the number of methods in the class object.
    */
    Uint32 getMethodCount() const;

    /** Get names of all keys of this class. */
    void getKeyNames(Array<String>& keyNames) const;

    Boolean hasKeys() const;

    /** Makes a deep copy (clone) of the given object. */
    CIMClass clone() const;

    /** identical -  Compares with another class
	ATTN: Clarify exactly what identical means
	@param Class object for the class to be compared
	@return True if the classes are identical
    */
    Boolean identical(const CIMConstClass& x) const;

    /** toXML  - prepares an XML representation of the CIMClass object
    	in the provided Sint8 variable.
	@param out Sint8 array for the XML representation
    */
    void toXml(Array<Sint8>& out) const;

    /** toMof  - prepares a MOF representation of the CIMClass object
    	in the provided Sint8 variable.
	@param out Sint8 array for the XML representation
    */
    void toMof(Array<Sint8>& out) const;

    /** print -  Prints the toXML output to cout
    */
    void print(PEGASUS_STD(ostream)& o=PEGASUS_STD(cout)) const;

#ifdef PEGASUS_INTERNALONLY
    /** Resolve -  Resolve the class: inherit any properties and
	qualifiers. Make sure the superClass really exists and is consistent
	with this class. Also set the propagated flag class-origin for each
	class feature.
	ATTN: explain why this here
    */
    void resolve(
	DeclContext* declContext,
	const String& nameSpace);

    /// operator - ATTN:
    operator int() const;

    /** printMof -  Prints the toMof output to cout
    */
    void printMof(PEGASUS_STD(ostream)& o=PEGASUS_STD(cout)) const;
#endif

private:

    CIMClassRep* _rep;

#ifdef PEGASUS_INTERNALONLY
    CIMClass(CIMClassRep* rep);

    void _checkRep() const;

    friend class CIMConstClass;
    friend class CIMObject;
    friend class CIMConstObject;
#endif
};

#define PEGASUS_ARRAY_T CIMClass
# include "ArrayInter.h"
#undef PEGASUS_ARRAY_T

/** CIMConstClass - ATTN: define this.

*/
class PEGASUS_COMMON_LINKAGE CIMConstClass
{
public:

    CIMConstClass();

    CIMConstClass(const CIMConstClass& x);

    CIMConstClass(const CIMClass& x);

    PEGASUS_EXPLICIT CIMConstClass(const CIMObject& x)
        throw(DynamicCastFailed);

    PEGASUS_EXPLICIT CIMConstClass(const CIMConstObject& x)
        throw(DynamicCastFailed);

    // Throws IllegalName if className argument not legal CIM identifier.
    CIMConstClass(
	const CIMReference& reference,
	const String& superClassName = String::EMPTY);

    CIMConstClass& operator=(const CIMConstClass& x);

    CIMConstClass& operator=(const CIMClass& x);

    ~CIMConstClass();

    Boolean isAssociation() const;

    Boolean isAbstract() const;

    const String& getClassName() const;

    const Boolean equalClassName(const String& classname) const;

    const CIMReference& getPath() const;

    const String& getSuperClassName() const;

    Uint32 findQualifier(const String& name) const;

    CIMConstQualifier getQualifier(Uint32 pos) const;

    Boolean isTrueQualifier(const String& name) const;

    Uint32 getQualifierCount() const;

    Uint32 findProperty(const String& name) const;

    CIMConstProperty getProperty(Uint32 pos) const;

    Uint32 getPropertyCount() const;

    Uint32 findMethod(const String& name) const;

    CIMConstMethod getMethod(Uint32 pos) const;

    Uint32 getMethodCount() const;

    void getKeyNames(Array<String>& keyNames) const;

    Boolean hasKeys() const;

    CIMClass clone() const;

    Boolean identical(const CIMConstClass& x) const;

#ifdef PEGASUS_INTERNALONLY
    operator int() const;

    void toXml(Array<Sint8>& out) const;

    void print(PEGASUS_STD(ostream)& o=PEGASUS_STD(cout)) const;
#endif

private:

    CIMClassRep* _rep;

#ifdef PEGASUS_INTERNALONLY
    void _checkRep() const;

    friend class CIMClassRep;
    friend class CIMClass;
    friend class CIMInstanceRep;
    friend class CIMObject;
    friend class CIMConstObject;
#endif
};

PEGASUS_NAMESPACE_END

#endif /* Pegasus_CIMClass_h */
