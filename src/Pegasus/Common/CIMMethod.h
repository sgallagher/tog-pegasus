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
// $Log: CIMMethod.h,v $
// Revision 1.4  2001/04/08 01:13:21  mike
// Changed "ConstCIM" to "CIMConst"
//
// Revision 1.2  2001/02/19 01:47:16  mike
// Renamed names of the form CIMConst to CIMConst.
//
// Revision 1.1  2001/02/18 18:39:06  mike
// new
//
// Revision 1.2  2001/02/18 03:56:01  mike
// Changed more class names (e.g., ConstClassDecl -> CIMConstClass)
//
// Revision 1.1  2001/02/16 02:07:06  mike
// Renamed many classes and headers (using new CIM prefixes).
//
// Revision 1.5  2001/02/16 00:34:17  mike
// added ATTN
//
// Revision 1.4  2001/02/06 17:04:03  karl
// add documentation
//
// Revision 1.3  2001/01/28 19:07:07  karl
// add DOC++ comments
//
// Revision 1.2  2001/01/15 04:31:44  mike
// worked on resolve scheme
//
// Revision 1.1.1.1  2001/01/14 19:52:57  mike
// Pegasus import
//
//
//END_HISTORY

/*
 CIMMethod.h - This header file defines the method class.
*/

#ifndef Pegasus_Method_h
#define Pegasus_Method_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/CIMMethodRep.h>

PEGASUS_NAMESPACE_BEGIN


class CIMConstMethod;

/** Class CIMMethod - This class defines the operations associated with
manipulation of the Pegasus implementation of the CIM CIMMethod.  Within this
class, methods are provides for creation, deletion, and manipulation of method
declarations.

*/

class PEGASUS_COMMON_LINKAGE CIMMethod
{
public:
    /// Creates and instantiates a CIM method.
    CIMMethod() : _rep(0)
    {

    }
    /** Creates and instantiates a CIM method from another method instance
    @return pointer to the new method instance
    */
    CIMMethod(const CIMMethod& x)
    {
	Inc(_rep = x._rep);
    }
    ///
    CIMMethod& operator=(const CIMMethod& x)
    {
	if (x._rep != _rep)
	{
	    Dec(_rep);
	    Inc(_rep = x._rep);
	}
	return *this;
    }

    // ATTN: remove the classOrigin and propagated parameters.

    /**	 Creates a CIM method with the specified name, type, and classOrigin
    @param name for the method
    @param type ATTN
    @param classOrigin
    @param propagated
    @return  Throws IllegalName if name argument not legal CIM identifier.
    */
    CIMMethod(
	const String& name,
	CIMType type,
	const String& classOrigin = String(),
	Boolean propagated = false)
    {
	_rep = new CIMMethodRep(name, type, classOrigin, propagated);
    }

    /// CIMMethod desctructor
    ~CIMMethod()
    {
	Dec(_rep);
    }
    /** CIMMethod getName - Gets the name of the method
    @return String with the name of the method
    */
    const String& getName() const
    {
	_checkRep();
	return _rep->getName();
    }

    /** CIMMethod setName - Set the method name
    @parm name ATTN (make reference to name defintion here)
    @return Throws IllegalName if name argument not legal CIM identifier.
    */
    void setName(const String& name)
    {
	_checkRep();
	_rep->setName(name);
    }
    /** CIMMethod getType - gets the method type
    @return The CIM method type for this method.
    */

    CIMType getType() const
    {
	_checkRep();
	return _rep->getType();
    }
    /** CIMMethod setType - Sets the method type to the specified CIM method
    type as defined in CIMType /Ref{TYPE}
    */
    void setType(CIMType type)
    {
	_checkRep();
	_rep->setType(type);
    }
    /** CIMMethod getClassOrigin - Returns the class in which this method
     was defined.
    @return ATTN:
    */
    const String& getClassOrigin() const
    {
	_checkRep();
	return _rep->getClassOrigin();
    }
    /** CIMMethod setClassOrigin - ATTN:
    */
    void setClassOrigin(const String& classOrigin)
    {
	_checkRep();
	_rep->setClassOrigin(classOrigin);
    }
    /// method getPropagated - ATTN:
    Boolean getPropagated() const
    {
	_checkRep();
	return _rep->getPropagated();
    }
    /// method setPropagated - ATTN:
    void setPropagated(Boolean propagated)
    {
	_checkRep();
	_rep->setPropagated(propagated);
    }
    /** CIMMethod addQualifier -
    @parm CIMQualifier to add
    @param CIMQualifier to be added
    @return Throws AlreadyExists excetpion if the qualifier already exists in
    the method
    @exception 	AlreadyExists exception of CIMQualifier already exists.
    */
    CIMMethod& addQualifier(const CIMQualifier& x)
    {
	_checkRep();
	_rep->addQualifier(x);
	return *this;
    }
    /** CIMMethod findQualifier - Finds the CIMQualifier named by the input string.
    @param String defining the name of the parameter to be found
    @return Index to the parameter found or -1 if not found
    The -1 must be converted to a Unit32 as follows:
    <PRE>
	Uint32 pos = myClass.findProperty("name");
	if (pos == Uint32(-1))
	{
	    // Not found!
	}
    </PRE>
    */
    Uint32 findQualifier(const String& name)
    {
	_checkRep();
	return _rep->findQualifier(name);
    }
    /** CIMMethod findQualifier - Find the qualifier with the name defined on
    input
    @param String with the name of the parameter to be found.
    @return - index to the qualifier found or -1 if no qualifier found.
    */
    Uint32 findQualifier(const String& name) const
    {
	_checkRep();
	return _rep->findQualifier(name);
    }
    /** CIMMethod getQualifier - Gets the CIMQualifier defined by the index input
    as a parameter.
    @param Index of the qualifier requested.
    @return CIMQualifier object or exception
    @exception OutOfBounds exception if the index is outside the range of
    parameters available from the CIMMethod.
    */
    CIMQualifier getQualifier(Uint32 pos)
    {
	_checkRep();
	return _rep->getQualifier(pos);
    }

    CIMConstQualifier getQualifier(Uint32 pos) const
    {
	_checkRep();
	return _rep->getQualifier(pos);
    }
    /** CIMMethod getQualifierCount - Returns count of the number of Qualifiers
    attached to the CIMMethod
    @return integer representing number of Qualifiers.
    */
    Uint32 getQualifierCount() const
    {
	_checkRep();
	return _rep->getQualifierCount();
    }
    /** CIMMethod addParameter - Adds the parameter defined by the input to the
    CIMMethod
    Param - ATTN:
    */
    CIMMethod& addParameter(const CIMParameter& x)
    {
	_checkRep();
	_rep->addParameter(x);
	return *this;
    }
    /// CIMMethod findParameter - ATTN:
    Uint32 findParameter(const String& name)
    {
	_checkRep();
	return _rep->findParameter(name);
    }

    /** CIMMethod findParameter - Finds the parameter defined by the name input
    and returns an index to the CIMParameter
    @param String defining the parameter to be found
    @return index to the parameter if found.  Returns -1 if parameter not
    found
    */
     Uint32 findParameter(const String& name) const
    {
	_checkRep();
	return _rep->findParameter(name);
    }
    /// CIMMethod getParameter - ATTN:
    CIMParameter getParameter(Uint32 pos)
    {
	_checkRep();
	return _rep->getParameter(pos);
    }

    /** CIMMethod getParameter - Gets the parameter defined by the index
    input as a parameter.
    @param index for the parameter to be returned.
    @return CIMParameter requested.
    @Exception OutOfBounds exception is thrown if the index is outside the
    range of available parameters
    */
    CIMConstParameter getParameter(Uint32 pos) const
    {
	_checkRep();
	return _rep->getParameter(pos);
    }

    /** CIMMethod getParameterCount - Gets the count of the numbeer of Parameters
    attached to the CIMMethod.
    @retrun - count of the number of parameters attached to the CIMMethod.
    */
    Uint32 getParameterCount() const
    {
	_checkRep();
	return _rep->getParameterCount();
    }
    /// method resolve - ATTN:
    void resolve(
	DeclContext* declContext,
	const String& nameSpace,
	const CIMConstMethod& method)
    {
	_checkRep();
	_rep->resolve(declContext, nameSpace, method);
    }
    /// CIMMethod resolve
    void resolve(
	DeclContext* declContext,
	const String& nameSpace)
    {
	_checkRep();
	_rep->resolve(declContext, nameSpace);
    }
    /// operator
    operator int() const { return _rep != 0; }
    /// method toXML - ATTN:
    void toXml(Array<Sint8>& out) const
    {
	_checkRep();
	_rep->toXml(out);
    }
    /// method print - ATTN:
    void print(std::ostream &o=std::cout) const
    {
	_checkRep();
	_rep->print(o);
    }
    /// CIMMethod identical - ATTN
    Boolean identical(const CIMConstMethod& x) const;
    /// CIMMethod clone - ATTN
    CIMMethod clone() const
    {
	return CIMMethod(_rep->clone());
    }

private:

    CIMMethod(CIMMethodRep* rep) : _rep(rep)
    {
    }

    explicit CIMMethod(const CIMConstMethod& x);

    void _checkRep() const
    {
	if (!_rep)
	    throw UnitializedHandle();
    }

    CIMMethodRep* _rep;
    friend class CIMConstMethod;
    friend class CIMClassRep;
};

////////////////////////////////////////////////////////////////////////////////
//
// CIMConstMethod
//
////////////////////////////////////////////////////////////////////////////////

class PEGASUS_COMMON_LINKAGE CIMConstMethod
{
public:

    CIMConstMethod() : _rep(0)
    {

    }

    CIMConstMethod(const CIMConstMethod& x)
    {
	Inc(_rep = x._rep);
    }

    CIMConstMethod(const CIMMethod& x)
    {
	Inc(_rep = x._rep);
    }

    CIMConstMethod& operator=(const CIMConstMethod& x)
    {
	if (x._rep != _rep)
	{
	    Dec(_rep);
	    Inc(_rep = x._rep);
	}
	return *this;
    }

    CIMConstMethod& operator=(const CIMMethod& x)
    {
	if (x._rep != _rep)
	{
	    Dec(_rep);
	    Inc(_rep = x._rep);
	}
	return *this;
    }

    // Throws IllegalName if name argument not legal CIM identifier.

    CIMConstMethod(
	const String& name,
	CIMType type,
	const String& classOrigin = String(),
	Boolean propagated = false)
    {
	_rep = new CIMMethodRep(name, type, classOrigin, propagated);
    }

    ~CIMConstMethod()
    {
	Dec(_rep);
    }

    const String& getName() const
    {
	_checkRep();
	return _rep->getName();
    }

    CIMType getType() const
    {
	_checkRep();
	return _rep->getType();
    }

    const String& getClassOrigin() const
    {
	_checkRep();
	return _rep->getClassOrigin();
    }

    Boolean getPropagated() const
    {
	_checkRep();
	return _rep->getPropagated();
    }

    Uint32 findQualifier(const String& name) const
    {
	_checkRep();
	return _rep->findQualifier(name);
    }

    Uint32 getQualifier(Uint32 pos) const
    {
	_checkRep();
	return _rep->getQualifier(pos);
    }

    Uint32 getQualifierCount() const
    {
	_checkRep();
	return _rep->getQualifierCount();
    }

    Uint32 findParameter(const String& name) const
    {
	_checkRep();
	return _rep->findParameter(name);
    }

    CIMConstParameter getParameter(Uint32 pos) const
    {
	_checkRep();
	return _rep->getParameter(pos);
    }

    Uint32 getParameterCount() const
    {
	_checkRep();
	return _rep->getParameterCount();
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

    Boolean identical(const CIMConstMethod& x) const
    {
	x._checkRep();
	_checkRep();
	return _rep->identical(x._rep);
    }

    CIMMethod clone() const
    {
	return CIMMethod(_rep->clone());
    }

private:

    void _checkRep() const
    {
	if (!_rep)
	    throw UnitializedHandle();
    }

    CIMMethodRep* _rep;

    friend class CIMMethod;
    friend class CIMMethodRep;
};

PEGASUS_NAMESPACE_END

#endif /* Pegasus_Method_h */
