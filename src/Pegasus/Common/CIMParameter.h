//%2005////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001, 2002 BMC Software; Hewlett-Packard Development
// Company, L.P.; IBM Corp.; The Open Group; Tivoli Systems.
// Copyright (c) 2003 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation, The Open Group.
// Copyright (c) 2004 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation; VERITAS Software Corporation; The Open Group.
// Copyright (c) 2005 Hewlett-Packard Development Company, L.P.; IBM Corp.;
// EMC Corporation; VERITAS Software Corporation; The Open Group.
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
//%/////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_Parameter_h
#define Pegasus_Parameter_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/CIMName.h>
#include <Pegasus/Common/CIMQualifier.h>
#include <Pegasus/Common/CIMType.h>
#include <Pegasus/Common/Linkage.h>

PEGASUS_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////
//
// CIMParameter
//
////////////////////////////////////////////////////////////////////////////////

class Resolver;
class CIMConstParameter;
class CIMParameterRep;

/** This class provides the interface to construct and manage CIM Parameters.
    CIM Parameters are the parameters attached to CIMMethods.
    A CIM Parameter consists of:
    <UL>
    <LI> <B>qualifiers</B> - zero or more qualifiers.
    <LI> <B>name</B> - The name of the parameter which must be a valid CIM name.
    <LI> <B>type</B> - The type for the parameter, one of the inherent CIM types
    <LI> <B>value</B> - Which is dependent on the type. The value can be
        <UL>
        <LI> a reference type. In this class the value is a Class reference. This must be
        a single value
        <LI> an array (fixed or variable number of elements) or a single value
        with any CIMType other than reference.  The input parameters allow specifying
        these conditions.
        </UL>
    </UL>

    ATTN: Define the form of this objec, the rep and what it means.
*/
class PEGASUS_COMMON_LINKAGE CIMParameter
{
public:

    /// Construct a NULL CIMParameter object.
    CIMParameter();

    /** Construct a CIMParameter from another CIMParameter
        @param CIMParameter from which the new object is to be constructed
    */
    CIMParameter(const CIMParameter& x);

    /** Constructs a CIMParameter object with properties. The Properties
        Must include name and type and may include the indicator whether
        this is an array or not, the size of the array and a reference
        class name.
        @param name Name of the parameter, a legal CIMName.
        @param type CIMType defining the CIM Type for this parameter
        @param isArray Boolean indicating whether this parameter defines an
        array.
        @param arraySize Size of the array if this is to be a fixed size
        array parameter. The default is zero which indicates a variable size array.
        @param referenceClassName Optional property but required for reference
        type parameters.  This defines the class for the reference.
        @exception TypeMismatchException Thrown if reference type and referenceClassname
        is Null.
        @exception TypeMismatchException Thrown if arraysize != zero and isArray true.
    */
    CIMParameter(
        const CIMName& name,
        CIMType type,
        Boolean isArray = false,
        Uint32 arraySize = 0,
        const CIMName& referenceClassName = CIMName());

    /** Destroys the object.
    */
    ~CIMParameter();

    /** Assignment operator. Assigns one CIMParameter to
        another CIMParameter
    */
    CIMParameter& operator=(const CIMParameter& x);

    /** Get the name from the CIMParameter object.
        @return CIMName containing the name from the object.
    */
    const CIMName& getName() const;

    /** Set the name field in the object with a valid CIMName
        @param name CIMName to set into the name field.
    */
    void setName(const CIMName& name);

    /** Test for Array type for this parameter object.
        @return true if the value for this parameter is defined
        as an array (is array = true).
    */
    Boolean isArray() const;

    /** Get the array size for the parameter.
        @return Uint32 array size.
    */
    Uint32 getArraySize() const;

    ///
    const CIMName& getReferenceClassName() const;

    /** Get the type (CIMTYPE) defined for this parameter.
        If the parameter is not initialized the type returned is
        TBD.
        @return the type for this parameter defined as a CIMTYPE
        object.
    */
    CIMType getType() const ;

    /** Add a single qualifier object to the CIMParameter.
        @param x CIMQualifier object to be added.
        @exception AlreadyExistsException if a qualifier with the
        same name already exists for this CIMParameter.
    */
    CIMParameter& addQualifier(const CIMQualifier& x);

    /** Find a qualifier by name.  Finds a single qualifier
        based on the name input as parameter and returns an
        index to the name.
        @param name CIMName with the name of the qualifier to be found
        @return Uint32 with either the index (zero origin) of
        the parameter that was to be found or the value
        PEG_NOT_FOUND if no parameter is found with the
        defined name.
    */
    Uint32 findQualifier(const CIMName& name) const;

    /** Get qualifier at index defined by input.  Gets the
        qualifier in the array of qualifiers for this parameter
        defined by the index provided on input.
        @param index Specifies the position in the qualifier array
        of the qualifier to be retrieved
        @return CIMQualifier object containing the qualifer defined
        by the index
        @exception IndexOutOfBoundsException thrown if index outside
        the array of qualifiers.
    */
    CIMQualifier getQualifier(Uint32 index);

    /** Removes the CIMQualifier defined by the input parameter.
        @param index Index of the qualifier to be removed.
        @exception IndexOutOfBoundsException if the index is outside
        the range of qualifiers available for the CIMParameter.
        @exception IndexOutOfBoundsException thrown if index outside
        the array of qualifiers.
    */
    void removeQualifier (Uint32 index);

    /** Get qualifier at index defined by input.  Gets the
        qualifier in the array of qualifiers for this parameter
        defined by the index provided on input.
        @param index Specifies the position in the qualifier array
        of the qualifier to be retrieved
        @return CIMQualifier object containing the qualifer defined
        by the index
        @exception IndexOutOfBoundsException thrown if index outside
        the array of qualifiers.
    */
    CIMConstQualifier getQualifier(Uint32 index) const;

    /** Gets the count of qualifiers attached to this CIMParameter.
        @return count of number of qualifiers that have been added
        to this CIMparameter.
        <pre>
             // loop to access all qualifiers in a CIMparameter
             CIMParameter parm;
             ....               // build the parameter
            for (Uint32 i = 0 ; i < parm.getQualifierCount() ; i++
                ....
        </pre>
    */
    Uint32 getQualifierCount() const;

    /** Determines if the object has not been initialized. A CIM parameter
        is intialized only when the name and type fields have been set either
        on construction or through the set functions.
        @return  true if the object has not been initialized,
                 false otherwise.
     */
    Boolean isUninitialized() const;

    ///
    Boolean identical(const CIMConstParameter& x) const;

    /** Creates a deep copy, i.e. a clone, of the associated object.
        @return The deep copy of the associated object.
    */
    CIMParameter clone() const;

private:

    CIMParameter(CIMParameterRep* rep);

    void _checkRep() const;

    CIMParameterRep* _rep;

    friend class CIMConstParameter;
    friend class Resolver;
    friend class XmlWriter;
    friend class MofWriter;
    friend class BinaryStreamer;
};

////////////////////////////////////////////////////////////////////////////////
//
// CIMConstParameter
//
////////////////////////////////////////////////////////////////////////////////

///
class PEGASUS_COMMON_LINKAGE CIMConstParameter
{
public:

    ///
    CIMConstParameter();

    ///
    CIMConstParameter(const CIMConstParameter& x);

    ///
    CIMConstParameter(const CIMParameter& x);

    ///
    CIMConstParameter(
        const CIMName& name,
        CIMType type,
        Boolean isArray = false,
        Uint32 arraySize = 0,
        const CIMName& referenceClassName = CIMName());

    ///
    ~CIMConstParameter();

    ///
    CIMConstParameter& operator=(const CIMConstParameter& x);

    ///
    CIMConstParameter& operator=(const CIMParameter& x);

    ///
    const CIMName& getName() const;

    ///
    Boolean isArray() const;

    ///
    Uint32 getArraySize() const;

    ///
    const CIMName& getReferenceClassName() const;

    ///
    CIMType getType() const;

    ///
    Uint32 findQualifier(const CIMName& name) const;

    ///
    CIMConstQualifier getQualifier(Uint32 index) const;

    ///
    Uint32 getQualifierCount() const;

    ///
    Boolean isUninitialized() const;

    ///
    Boolean identical(const CIMConstParameter& x) const;

    ///
    CIMParameter clone() const;

private:

    void _checkRep() const;

    CIMParameterRep* _rep;
    friend class CIMParameter;
    friend class XmlWriter;
    friend class MofWriter;
};

#define PEGASUS_ARRAY_T CIMParameter
# include <Pegasus/Common/ArrayInter.h>
#undef PEGASUS_ARRAY_T

PEGASUS_NAMESPACE_END

#endif /* Pegasus_Parameter_h */
