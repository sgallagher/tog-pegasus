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
// Revision 1.2  2001/02/19 01:47:16  mike
// Renamed names of the form CIMConst to ConstCIM.
//
// Revision 1.1  2001/02/18 18:39:06  mike
// new
//
// Revision 1.2  2001/02/18 03:56:00  mike
// Changed more class names (e.g., ConstClassDecl -> ConstCIMClass)
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
    /** Constructor - Create a CIM Instance object from another Instance.
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
    from the inpur parameters
    @param - String className to be used with new instance object
    
    @return The new instance object
    
    Throws IllegalName if className argument not legal CIM identifier.
    ATTN: Clarify the defintion
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
    /**	CIMMethod
    
    */
    const String& getClassName() const 
    { 
	_checkRep();
	return _rep->getClassName(); 
    }
    /**	CIMMethod addQualifier - Adds teh CIMQualifier object to the instance.
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
    /**	CIMMethod
    
    */

    Uint32 findQualifier(const String& name)
    {
	_checkRep();
	return _rep->findQualifier(name);
    }
    /**	CIMMethod
    
    */

    Uint32 findQualifier(const String& name) const
    {
	_checkRep();
	return _rep->findQualifier(name);
    }
    /**	CIMMethod
    
    */

    CIMQualifier getQualifier(Uint32 pos)
    {
	_checkRep();
	return _rep->getQualifier(pos);
    }
    /**	CIMMethod
    
    */

    CIMConstQualifier getQualifier(Uint32 pos) const
    {
	_checkRep();
	return _rep->getQualifier(pos);
    }
    /**	CIMMethod
    
    */

    Uint32 getQualifierCount() const
    {
	_checkRep();
	return _rep->getQualifierCount();
    }

    /**	CIMMethod
    
    */
    CIMInstance& addProperty(const CIMProperty& x)
    {
	_checkRep();
	_rep->addProperty(x);
	return *this;
    }

    /**	CIMMethod
    
    */
    Uint32 findProperty(const String& name)
    {
	_checkRep();
	return _rep->findProperty(name);
    }

    /**	CIMMethod
    
    */
    Uint32 findProperty(const String& name) const
    {
	_checkRep();
	return _rep->findProperty(name);
    }

    /**	CIMMethod
    
    */
    CIMProperty getProperty(Uint32 pos)
    {
	_checkRep();
	return _rep->getProperty(pos);
    }

    /**	CIMMethod
    
    */
    CIMConstProperty getProperty(Uint32 pos) const
    {
	_checkRep();
	return _rep->getProperty(pos);
    }

    /**	CIMMethod
    
    */
    Uint32 getPropertyCount() const
    {
	_checkRep();
	return _rep->getPropertyCount();
    }

    /**	CIMMethod
    
    */
    operator int() const { return _rep != 0; }

    /**	CIMMethod
    
    */
    void resolve(DeclContext* declContext, const String& nameSpace)
    {
	_checkRep();
	_rep->resolve(declContext, nameSpace);
    }

    /**	CIMMethod

    */
    void toXml(Array<Sint8>& out) const
    {
	_checkRep();
	_rep->toXml(out);
    }

    /**	CIMMethod
    
    */
    void print() const
    {
	_checkRep();
	_rep->print();
    }

    /**	CIMMethod
    
    */
    Boolean identical(const CIMConstInstance& x) const;

    /**	CIMMethod
    
    */
    CIMInstance clone() const
    {
	return CIMInstance(_rep->clone());
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

    String getInstanceName(ConstCIMClass& cimClass) const
    {
	_checkRep();
	return _rep->getInstanceName(cimClass);
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

    void print() const
    {
	_checkRep();
	_rep->print();
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

    String getInstanceName(ConstCIMClass& cimClass) const
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
