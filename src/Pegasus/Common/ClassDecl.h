//BEGIN_LICENSE
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
//END_LICENSE
//BEGIN_HISTORY
//
// Author:
//
// $Log: ClassDecl.h,v $
// Revision 1.5  2001/01/30 23:39:00  karl
// Add doc++ Documentation to header files
//
// Revision 1.4  2001/01/28 18:48:07  mike
// fixed typo in comment
//
// Revision 1.3  2001/01/28 18:46:50  mike
// more docs
//
// Revision 1.2  2001/01/15 04:31:43  mike
// worked on resolve scheme
//
// Revision 1.1.1.1  2001/01/14 19:50:37  mike
// Pegasus import
//
//
//END_HISTORY

#ifndef Pegasus_ClassDecl_h
#define Pegasus_ClassDecl_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/ClassDeclRep.h>

PEGASUS_NAMESPACE_BEGIN

class ConstClassDecl;

/** The ClassDecl class is used to represent CIM classes in Pegasus.  In CIM, 
a class object may be a class or an associator.  A CIM class must contain a 
name and may contain methods, properties, and qualifiers.  It is a template 
for creating a CIM instance.  A CIM class represents a collection of CIM 
instances, all of which support a common type (for example, a set of 
properties, methods, and associations).  
*/
class PEGASUS_COMMON_LINKAGE ClassDecl
{
public:
    /** Constructor - Creates and instantiates a new object reprenting a CIM
    class. If you use this constructor, use setName to define a name for the 
    class
    */
    ClassDecl() : _rep(0)
    {

    }
    /// Constructor - Creates a class from a previous class
    ClassDecl(const ClassDecl& x)
    {
	Inc(_rep = x._rep);
    }
    /// Operator = ATTN:
    ClassDecl& operator=(const ClassDecl& x)
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
    @param className - String representing the name of the class being created
    @param superClassName - String representing the name of the SuperClass
    ATTN: Define what makes up legal name.
    @return Throws IllegalName if className argument not legal CIM identifier.
    */
    ClassDecl(
	const String& className, 
	const String& superClassName = String())
    {
	_rep = new ClassDeclRep(className, superClassName);
    }
    /// Destructor
    ~ClassDecl()
    {
	Dec(_rep);
    }
    /** Method isAssociation - Identifies whether or not this CIM class is an 
    association.  An association is a relationship between two (or more) classes 
    or instances of two classes.  The properties of an association class include 
    pointers, or references, to the two (or more) instances.  All CIM classes can 
    be included in one or more associations.  
    ATTN: Move the association definition elsewhere
    @return  Boolean True if this CIM class belongs to an association; 
    otherwise, false. 
    */ 
    Boolean isAssociation() const
    {
	_checkRep();
	return _rep->isAssociation();
    }
    ///	 Method isAbstract
    Boolean isAbstract() const
    {
	_checkRep();
	return _rep->isAbstract();
    }
    /** Method Gets the name of the class
    ATTN: COMMENT. Why not just get name so we have common method for all.
    */
    const String& getClassName() const 
    { 
	_checkRep();
	return _rep->getClassName(); 
    }
    /** Method getSuperClassName - Gets the name of the Parent
    @return String with parent class name.
    */
    const String& getSuperClassName() const 
    { 
	_checkRep();
	return _rep->getSuperClassName(); 
    }
    /**	Method setSuperClassName - Sets the name of the parent class from
    the input parameter. \REF{CLASSNAME}.
    ATTN: Define legal classnames
    @param - String defining parent name.
    @return Throws IllegalName if superClassName argument not legal CIM 
    identifier. */
    void setSuperClassName(const String& superClassName)
    {
	_checkRep();
	_rep->setSuperClassName(superClassName);
    }
    /** Method addQualifier - Adds the specified qualifier to the class
    and increments the qualifier count. It is illegal to add the same
    qualifier more than one time.
    @param - Qualifier object representing the qualifier to be added
    ATTN: Pointer to qualifier object.
    @return Throws AlreadyExists.
    */
    ClassDecl& addQualifier(const Qualifier& qualifier)
    {
	_checkRep();
	_rep->addQualifier(qualifier);
	return *this;
    }
    /**	Method findQualifier - Finds a qualifier with the specified input name
    if it exists in the class
    @param name Name of the qualifier to be found
    @return Position of the qualifier in the Class
    ATTN: Clarify the return.  What if not found, etc.
    */
    Uint32 findQualifier(const String& name)
    {
	_checkRep();
	return _rep->findQualifier(name);
    }
    /** Method FindQualifier - ATTN:
    @param name of the qualifier to be found
    @return ATTN: Define this
    */
    Uint32 findQualifier(const String& name) const
    {
	_checkRep();
	return _rep->findQualifier(name);
    }
    /**	 Method getQualifier - Gets the Qualifier object defined
    by the input parameter
    @param pos defines the position of the qualifier in the class from the
    findQualifier method
    @return Qualifier object representing the qualifier found.
    ATTN: what is error return here?
    */
    Qualifier getQualifier(Uint32 pos)
    {
	_checkRep();
	return _rep->getQualifier(pos);
    }
    /// Method getQualifier - ATTN:
    ConstQualifier getQualifier(Uint32 pos) const
    {
	_checkRep();
	return _rep->getQualifier(pos);
    }
    /** Method getQualifierCount - Returns the number of qualifiers
    in the class.
    @return ATTN:
    */
    Uint32 getQualifierCount() const
    {
	_checkRep();
	return _rep->getQualifierCount();
    }
    /**	Method addProperty - Adds the specified property object to the
    properties in the CIM class
    
    */
    ClassDecl& addProperty(const Property& x)
    {
	_checkRep();
	_rep->addProperty(x);
	return *this;
    }
    /** Method removeProperty - Removes the property represented
    by the position input parameter from the class
    @param position parameter for the property to be removed from the 
    findPropety method
    @return ATTN:
    */
    void removeProperty(Uint32 pos)
    {
	_checkRep();
	_rep->removeProperty(pos);
    }
    /** Method findProperty - Finds the property object with the
    name defined by the input parameter in the class.
    @param String parameter with the property name.
    @return position representing the property object found.
    ATTN:   Clarify case of not found
    */
    Uint32 findProperty(const String& name)
    {
	_checkRep();
	return _rep->findProperty(name);
    }
    /// Method findProperty
    Uint32 findProperty(const String& name) const
    {
	_checkRep();
	return _rep->findProperty(name);
    }
    /** Method getProperty - Returns a property representing the property 
    defined by the input parameter
    @param position for this property
    ATTN: Should we not use something like handle for position???
    @return Property object
    ATTN: what is error return?
    */
    Property getProperty(Uint32 pos)
    {
	_checkRep();
	return _rep->getProperty(pos);
    }
    /// Method getProperty - ATTN
    ConstProperty getProperty(Uint32 pos) const
    {
	_checkRep();
	return _rep->getProperty(pos);
    }
    /** Method getProperty -   Gets the count of the number of properties
    defined in the class.
    @return count of number of proerties in the class
    */
    Uint32 getPropertyCount() const
    {
	_checkRep();
	return _rep->getPropertyCount();
    }
    /** Method addMethod - Adds the method object defined by the input 
    parameter to the class and increments the count of the number of methods in 
    the class
    @param - method object representing the method to be added 
    /REF{METHODOBJECT}
    */
    ClassDecl& addMethod(const Method& x)
    {
	_checkRep();
	_rep->addMethod(x);
	return *this;
    }
    /** Method findMethod - Located the method object defined by the
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
    /// Method findMethod - ATTN:
    Uint32 findMethod(const String& name) const
    {
	_checkRep();
	return _rep->findMethod(name);
    }
    /** Method getMethod - Gets the method object defined by the
    input parameter.
    @param   ATTN:
    @ method object representing the method defined 
    ATTN: Error???
    */
    Method getMethod(Uint32 pos)
    {
	_checkRep();
	return _rep->getMethod(pos);
    }
    /// Method getMethod - ATTN:
    ConstMethod getMethod(Uint32 pos) const
    {
	_checkRep();
	return _rep->getMethod(pos);
    }
    /** Method getMethodCount - Count of the number of methods in the class
    @return integer representing the number of methods in the class
    */
    Uint32 getMethodCount() const
    {
	_checkRep();
	return _rep->getMethodCount();
    }

    
    /** Method Resolve -  Resolve the class: inherit any properties and 
    qualifiers. Make sure the superClass really exists and is consistent with
    this class. Also set the propagated flag class-origin for each
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
    /// Method toXML 
    void toXml(Array<Sint8>& out) const
    {
	_checkRep();
	_rep->toXml(out);
    }
    /// Method print 
    void print() const
    {
	_checkRep();
	_rep->print();
    }
    /** Method identical -  Compares with another class
    ATTN: Clarify exactly what identical means
    @parm Class object for the class to be compared
    @return True if the classes are identical
    */
    Boolean identical(const ConstClassDecl& x) const;
    /// Method clone - ATTN:
    ClassDecl clone() const
    {
	return ClassDecl(_rep->clone());
    }

private:

    ClassDecl(ClassDeclRep* rep) : _rep(rep)
    {
    }

    void _checkRep() const
    {
	if (!_rep)
	    throw UnitializedHandle();
    }

    ClassDeclRep* _rep;
    friend class ConstClassDecl;
};

////////////////////////////////////////////////////////////////////////////////
//
// ConstClassDecl
//
////////////////////////////////////////////////////////////////////////////////

class PEGASUS_COMMON_LINKAGE ConstClassDecl
{
public:

    ConstClassDecl() : _rep(0)
    {

    }

    ConstClassDecl(const ConstClassDecl& x)
    {
	Inc(_rep = x._rep);
    }

    ConstClassDecl(const ClassDecl& x)
    {
	Inc(_rep = x._rep);
    }

    ConstClassDecl& operator=(const ConstClassDecl& x)
    {
	if (x._rep != _rep)
	{
	    Dec(_rep);
	    Inc(_rep = x._rep);
	}
	return *this;
    }

    ConstClassDecl& operator=(const ClassDecl& x)
    {
	if (x._rep != _rep)
	{
	    Dec(_rep);
	    Inc(_rep = x._rep);
	}
	return *this;
    }

    // Throws IllegalName if className argument not legal CIM identifier.

    ConstClassDecl(
	const String& className, 
	const String& superClassName = String())
    {
	_rep = new ClassDeclRep(className, superClassName);
    }

    ~ConstClassDecl()
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

    ConstQualifier getQualifier(Uint32 pos) const
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

    ConstProperty getProperty(Uint32 pos) const
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

    ConstMethod getMethod(Uint32 pos) const
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

    void print() const
    {
	_checkRep();
	_rep->print();
    }

    Boolean identical(const ConstClassDecl& x) const
    {
	x._checkRep();
	_checkRep();
	return _rep->identical(x._rep);
    }

    ClassDecl clone() const
    {
	return ClassDecl(_rep->clone());
    }

private:

    void _checkRep() const
    {
	if (!_rep)
	    throw UnitializedHandle();
    }

    ClassDeclRep* _rep;

    friend class ClassDeclRep;
    friend class ClassDecl;
    friend class InstanceDeclRep;
};

PEGASUS_NAMESPACE_END

#endif /* Pegasus_ClassDecl_h */
