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
// $Log: CIMInstance.h,v $
// Revision 1.8  2001/04/08 01:13:21  mike
// Changed "ConstCIM" to "CIMConst"
//
// Revision 1.6  2001/02/27 09:32:35  karl
// Document cleanup
//
// Revision 1.5  2001/02/20 14:05:24  karl
// Comments for Document
//
// Revision 1.4  2001/02/20 07:25:57  mike
// Added basic create-instance in repository and in client.
//
// Revision 1.3  2001/02/20 05:16:57  mike
// Implemented CIMInstance::getInstanceName()
//
// Revision 1.2  2001/02/19 01:47:16  mike
// Renamed names of the form CIMConst to CIMConst.
//
// Revision 1.1  2001/02/18 18:39:06  mike
// new
//
// Revision 1.2  2001/02/18 03:56:00  mike
// Changed more class names (e.g., ConstClassDecl -> CIMConstClass)
//
// Revision 1.1  2001/02/16 02:06:06  mike
// Renamed many classes and headers.
//
// Revision 1.4  2001/02/13 07:00:18  mike
// Added partial createInstance() method to repository.
//
// Revision 1.3  2001/01/30 23:39:00  karl
// Add doc++ Documentation to header files
//
// Revision 1.2  2001/01/15 04:31:44  mike
// worked on resolve scheme
//
// Revision 1.1.1.1  2001/01/14 19:52:38  mike
// Pegasus import
//
//
//END_HISTORY

/*

CIMInstance.h File defines the Class used to create, instantiate, and modify 
CIM Instances 

*/
#ifndef Pegasus_InstanceDecl_h
#define Pegasus_InstanceDecl_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/CIMInstanceRep.h>

PEGASUS_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////
//
// CIMInstance
//
////////////////////////////////////////////////////////////////////////////////

class CIMConstInstance;

/** Class CIMInstance	- The CIMInstance class represents the instance of 
    a CIM class in Pegasus. It is used manipulate instances and the 
    characteristics of instances
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
	@example
	ATTN:
    */
    CIMInstance(const CIMInstance& x)
    {
	Inc(_rep = x._rep);
    }
    /// Constructor - ATTN
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
    /// Destructor
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
        defined by the inputparameter.
	@param String defining the qualifier to be found
	@return - Index of the qualifier to be used in subsequent 
	operations or -1 if the qualifier is not found. 
    */ 
    Uint32 findQualifier(const String& name)
    {
	_checkRep();
	return _rep->findQualifier(name);
    }

    /**	findQualifier - Searches the instance for the qualifier object
        defined by the input parameter.
	@param String defining the qualifier to be found
	@return - Index of the qualifier to be used in subsequent 
	operations or -1 if the qualifier is not found. 
    */ 
    Uint32 findQualifier(const String& name) const
    {
	_checkRep();
	return _rep->findQualifier(name);
    }

    /**	getQualifier - Retrieves the qualifier object defined by the
	index input parameter.  @ index for the qualifier object.
	The index to qualifier objects is zero-origin and continuous
	so that incrementing loops can be used to get all qualifier
	objects in a CIMInstnace.  
	@return: Returns qualifier object defined by index.
	@exception Throws the OutOfBounds exception if the index
	is out of bounds  
    ATTN: What is effect of out of range index???
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
	@return Index in the CIM Instance to the property object if found or 
	-1 if no property object found with the name defined by the input. 
    */
    Uint32 findProperty(const String& name)
    {
	_checkRep();
	return _rep->findProperty(name);
    }

    /**	findProperty - Searches the property objects installed in the 
	CIMInstance for property objects with the name defined by the
	input
	@param String with the name of the property object to be found
	@return Index in the CIM Instance to the property object if found or 
	-1 if no property object found with the name defined by the input. 
    */  
    Uint32 findProperty(const String& name) const
    {
	_checkRep();
	return _rep->findProperty(name);
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

    /**	operator int() - ATTN:
    
    */
    operator int() const { return _rep != 0; }

    /**	resolve - ATTN:
    
    */
    void resolve(DeclContext* declContext, const String& nameSpace)
    {
	_checkRep();
	_rep->resolve(declContext, nameSpace);
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

    /**	CIMMethod
    
    */
    void print(std::ostream &o=std::cout) const
    {
	_checkRep();
	_rep->print(o);
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
	return CIMInstance(_rep->clone());
    }

    /** getInstnaceName - Get the instance name of this instance. The class 
	argument is used to determine which fields are keys. The instance
	name has this from:

	<PRE> 
	    ClassName.key1=value1,...,keyN=valueN
	</PRE>

	The instance name is in standard form (the class name and key name
	is all lowercase; the keys-value pairs appear in sorted order by
	key name).
    */
    String getInstanceName(const CIMConstClass& cimClass) const
    {
	_checkRep();
	return _rep->getInstanceName(cimClass);
    }

private:

    CIMInstance(CIMInstanceRep* rep) : _rep(rep)
    {
    }

    void _checkRep() const
    {
	if (!_rep)
	    throw UnitializedHandle();
    }

    CIMInstanceRep* _rep;
    friend class CIMConstInstance;
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

    void print(std::ostream &o=std::cout) const
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
	return CIMInstance(_rep->clone());
    }

    String getInstanceName(const CIMConstClass& cimClass) const
    {
	_checkRep();
	return _rep->getInstanceName(cimClass);
    }

private:

    void _checkRep() const
    {
	if (!_rep)
	    throw UnitializedHandle();
    }

    CIMInstanceRep* _rep;
    friend class CIMInstance;
};

PEGASUS_NAMESPACE_END

#endif /* Pegasus_InstanceDecl_h */
