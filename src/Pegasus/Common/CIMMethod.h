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
//                (carolann_graves@hp.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_Method_h
#define Pegasus_Method_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/Linkage.h>
#include <Pegasus/Common/String.h>
#include <Pegasus/Common/CIMName.h>
#include <Pegasus/Common/CIMParameter.h>
#include <Pegasus/Common/CIMQualifier.h>
#include <Pegasus/Common/CIMType.h>

PEGASUS_NAMESPACE_BEGIN

class Resolver;
class CIMConstMethod;
class CIMMethodRep;

    // ATTN: remove the classOrigin and propagated parameters.
/** Class CIMMethod - This class defines the operations associated with
    manipulation of the Pegasus implementation of the CIM CIMMethod. Within
    this class, methods are provides for creation, deletion, and manipulation
    of method declarations.
*/
class PEGASUS_COMMON_LINKAGE CIMMethod
{
public:

    /** Creates and instantiates a CIM method. */
    CIMMethod();

    /** Creates and instantiates a CIM method from another method instance
	@return pointer to the new method instance
    */
    CIMMethod(const CIMMethod& x);

    /**	Creates a CIM method with the specified name, type, and classOrigin
	@param name for the method
	@param type CIM data type of method to create
	@param classOrigin
	@param propagated
    */
    CIMMethod(
	const CIMName& name,
	CIMType type,
	const CIMName& classOrigin = CIMName(),
	Boolean propagated = false);

    /** Desctructor. */
    ~CIMMethod();

    /** Assignment operator */
    CIMMethod& operator=(const CIMMethod& x);

    /** getName - Gets the name of the method
	@return CIMName with the name of the method
    */
    const CIMName& getName() const;

    /** setName - Set the method name
	@param name
    */
    void setName(const CIMName& name);

    /** getType - gets the method type
	@return The CIM method type for this method.
    */
    CIMType getType() const;

    /** setType - Sets the method type to the specified CIM method
	type as defined in CIMType /Ref{TYPE}
    */
    void setType(CIMType type);

    /** getClassOrigin - Returns the class in which this method
	was defined.
	@return CIMName containing the classOrigin field. 
    */
    const CIMName& getClassOrigin() const;

    /** setClassOrigin - Set the ClassOrigin attribute with
	the classname defined on input
	@param classOrigin - CIMName parameter defining the name
	of the class origin
    */
    void setClassOrigin(const CIMName& classOrigin);

    /** getPropagated - Tests the propagated qualifier
        @return - returns True if method is propagated
    */
    Boolean getPropagated() const;

    /** setPropagated - Sets the Propagaged Qualifier */
    void setPropagated(Boolean propagated);

    /** addQualifier - Adds a Qualifier to the method object.
	@param CIMQualifier to be added
	@return Throws AlreadyExists excetpion if the qualifier already exists
	in the method
	@exception AlreadyExists exception
    */
    CIMMethod& addQualifier(const CIMQualifier& x);

    /** findQualifier - returns the position of the qualifier with
	the given name.
	@param name Name of qualifier to be found.
	@return index of the parameter if found; otherwise PEG_NOT_FOUND.
    */
    Uint32 findQualifier(const CIMName& name) const;

    /** getQualifier - Gets the CIMQualifier defined by the index
	input as a parameter.
	@param Index of the qualifier requested.
	@return CIMQualifier object or exception
	@exception OutOfBounds exception if the index is outside the range of
	parameters available from the CIMMethod.
    */
    CIMQualifier getQualifier(Uint32 pos);

    CIMConstQualifier getQualifier(Uint32 pos) const;

    /** removeQualifier - Removes the CIMQualifier defined by the
	position input as a parameter.
	@param Position of the qualifier requested.
	@return CIMQualifier object or exception
	@exception OutOfBounds exception if the index is outside the range of
	parameters available from the CIMMethod.
    */
    void removeQualifier(Uint32 pos);

    /** getQualifierCount - Returns the number of Qualifiers attached
	to this CIMMethod object.
	@return integer representing number of Qualifiers.
    */
    Uint32 getQualifierCount() const;

    /** addParameter - Adds the parameter defined by the input
	to the CIMMethod
    */
    CIMMethod& addParameter(const CIMParameter& x);

    /** findParameter - Finds the parameter whose name is given
	by the name parameter.
	@param name Name of parameter to be found.
	@return index of the parameter if found; otherwise PEG_NOT_FOUND.
    */
    Uint32 findParameter(const CIMName& name) const;

    /** getParameter - Gets the parameter defined by the index
	input as a parameter.
	@param index for the parameter to be returned.
	@return CIMParameter requested.
	@Exception OutOfBounds exception is thrown if the index is outside the
	range of available parameters
    */
    CIMParameter getParameter(Uint32 pos);

    /** getParameter - const form */
    CIMConstParameter getParameter(Uint32 pos) const;

    /** removeParameter - Removes the CIMParameter defined by the
	specified index

	@param pos index of the parameter to be removed

	@exception OutOfBounds if the index is outside the range of
	                       parameters available from the CIMMethod
    */
    void removeParameter (Uint32 pos);

    /** getParameterCount - Gets the count of the numbeer of
	Parameters attached to the CIMMethod.
	@retrun - count of the number of parameters attached to the CIMMethod.
    */
    Uint32 getParameterCount() const;

    /**
        Determines if the object has not been initialized.

        @return  True if the object has not been initialized,
                 False otherwise
     */
    Boolean isUninitialized() const;

    /** identical - Returns true if this method is identical to the
	one given by the argument x.
    */
    Boolean identical(const CIMConstMethod& x) const;

    /** CIMMethod clone - makes a distinct replica of this method */
    CIMMethod clone() const;

private:

    CIMMethod(CIMMethodRep* rep);

    PEGASUS_EXPLICIT CIMMethod(const CIMConstMethod& x);

    void _checkRep() const;

    CIMMethodRep* _rep;
    friend class CIMConstMethod;
    friend class Resolver;
    friend class XmlWriter;
    friend class MofWriter;
};

class PEGASUS_COMMON_LINKAGE CIMConstMethod
{
public:

    CIMConstMethod();

    CIMConstMethod(const CIMConstMethod& x);

    CIMConstMethod(const CIMMethod& x);

    CIMConstMethod(
	const CIMName& name,
	CIMType type,
	const CIMName& classOrigin = CIMName(),
	Boolean propagated = false);

    ~CIMConstMethod();

    CIMConstMethod& operator=(const CIMConstMethod& x);

    CIMConstMethod& operator=(const CIMMethod& x);

    const CIMName& getName() const;

    CIMType getType() const;

    const CIMName& getClassOrigin() const;

    Boolean getPropagated() const;

    Uint32 findQualifier(const CIMName& name) const;

    CIMConstQualifier getQualifier(Uint32 pos) const;

    Uint32 getQualifierCount() const;

    Uint32 findParameter(const CIMName& name) const;

    CIMConstParameter getParameter(Uint32 pos) const;

    Uint32 getParameterCount() const;

    Boolean isUninitialized() const;

    Boolean identical(const CIMConstMethod& x) const;

    CIMMethod clone() const;

private:

    void _checkRep() const;

    CIMMethodRep* _rep;

    friend class CIMMethod;
    friend class CIMMethodRep;
    friend class XmlWriter;
    friend class MofWriter;
};

#define PEGASUS_ARRAY_T CIMMethod
# include <Pegasus/Common/ArrayInter.h>
#undef PEGASUS_ARRAY_T

PEGASUS_NAMESPACE_END

#endif /* Pegasus_Method_h */
