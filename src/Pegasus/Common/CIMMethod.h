//%2003////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001, 2002  BMC Software, Hewlett-Packard Development
// Company, L. P., IBM Corp., The Open Group, Tivoli Systems.
// Copyright (c) 2003 BMC Software; Hewlett-Packard Development Company, L. P.;
// IBM Corp.; EMC Corporation, The Open Group.
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

/** The CIMMethod class is used to represent CIM methods in Pegasus. A CIMMethod
    consists of the following entities
    <ul>
        <li>Name of the method, a CIMName
        <li>CIMType of the method, a CIMType
        <li>Optional Qualifiers for the method
        <li>Optional Parameters for the method
    </ul>
    In addition, internally, there are the following additional attributes
    that are part of a CIMMethod.
    <ul>
        <li>propagated - attributed defining whether this CIMMethod is 
        propagated from a superclass.  Note that this is normally set
        as part of resolving objects when they are placed in a repository
        and is NOT automatically set when creating a local object.  It
        can only logically be set in context of the superclass of which
        this CIMMethod is defined.
        <li>ClassOrigin - attribute defining the superclass in which this
        CIMMethod was originally defined.  This is normally se as part
        of resolving Class and instances.  It is available from objects
        retrieved from the repository.
    </ul>
    CIMMethod is a shared class and uses a representation object and reference
    counting to control the lifecycle of the objects.
    @see CIMConstMethod
*/
class PEGASUS_COMMON_LINKAGE CIMMethod
{
public:

    /** Creates a new default CIMMethod object. The object created is
        empty.  Other methods such as setName() or assign must be
        used to provide attributes for the object
        @see CIMConstMethod()
    */
    CIMMethod();

    /** Creates a new CIMMethod object from another method instance. This method
        assigns the new object to the representation in the parameter and increments the
        representation count.  It does NOT create a new independent object.
        @param x - CIMMethod object from which to create CIMMethod object.
        <pre>
            CIMMethod m1(CIMName ("getHostName"), CIMTYPE_STRING);
            const CIMMethod cm1(m1);
        </pre>
    */
    CIMMethod(const CIMMethod& x);

    /** Creates a CIMMethod object with the specified name and other input parameters.
        @param name CIMName defining the name for the method.
        @param type CIMType defining data type of method to create. See 
            
        @param classOrigin (optional) CIMName representing the class origin. Note
            that this should normally not be used.  If not provided set to
            CIMName() (Null name).
        @param propagated Optional flag indicating whether the definition of the 
            CIM Method is local to the CIM Class (respectively, Instance) in which 
            it appears, or was propagated without modification from the underlying 
            Subclass (respectively, Class). Default is false.
        <pre>
            CIMMethod m1(CIMName ("getHostName"), CIMTYPE_STRING)
        </pre>
    */
    CIMMethod(
        const CIMName& name,
        CIMType type,
        const CIMName& classOrigin = CIMName(),
        Boolean propagated = false);

    /** Destructor for the CIMMethod. Since this is a shared class, the destructor
        is automatically executed when the representation reference count becomes
        zero
    */
    ~CIMMethod();

    /** Assignment operator. Assigns one CIM method to another.  This method performs
        the assignment by incrementing the reference count for the representation of
        the CIMMethod, not by creating a deep copy of the object
    */
    CIMMethod& operator=(const CIMMethod& x);

    /** Gets the name of the method.
        @return CIMName with the name of the method.
        <pre>
            CIMMethod m1(CIMName ("getHostName"), CIMTYPE_STRING)
            assert(m1.getName() == CIMName ("getHostName"));
        </pre>
    */
    const CIMName& getName() const;

    /** Sets the method name.
        @param name - CIMName for the method name. Replaces any
            previously defined name for this method object.
        <pre>
            CIMMethod m2(CIMName ("test"), CIMTYPE_STRING);
            m2.setName(CIMName ("getVersion"));
        </pre>
    */
    void setName(const CIMName& name);

    /** Gets the method type.
        @return The CIMType containing the method type for this method.
        <pre>
            CIMMethod m1(CIMName ("getHostName"), CIMTYPE_STRING)
            assert(m1.getType() == CIMTYPE_STRING);
        </pre>
    */
    CIMType getType() const;

    /** Sets the method type to the specified CIM method type 
        as defined in CIMType.
        @param type CIMType to be set into the method object.
        <pre>
            CIMMethod m1();
            m1.setName(CIMName ("getVersion"));
            assert(m1.setType(CIMTYPE_STRING));
        </pre>
    */
    void setType(CIMType type);

    /** Gets the class in which this method was defined. This information
        is available after the class containing the method has been
        resolved and is part of the class repository.
        @return CIMName containing the classOrigin field. 
    */
    const CIMName& getClassOrigin() const;

    /** Sets the ClassOrigin attribute with the classname defined on 
        the input parameter. Normally this function is used internally
        as part of resolving the objects containing methods (classes
        and instances). 
    @param classOrigin - CIMName parameter defining the name
    of the class origin.
    */
    void setClassOrigin(const CIMName& classOrigin);

    /** Tests the propagated qualifier.  The propagated attribute
        indicates if this method was propagated from a higher level
        class.  Normally this attribute is set as part of putting
        classes into the repository (resolving the class).  It is 
        available on methods in classes read from the repository and
        on instances that are read from the instance repository.
        @return true if method is propagated, false otherwise.
    */
    Boolean getPropagated() const;

    /** Sets the Propagaged Qualifier. Normally this is used by the functions
        that resolve classes and instances as part of the installation into
        a repository.
        @param propagated Flag indicating propagation. True means that
        the method was propagated from a superclass.
    */
    void setPropagated(Boolean propagated);

    /** Adds the specified qualifier to the method and increments the
        qualifier count. 
        @param x - CIMQualifier object representing the qualifier
        to be added.
        @return the CIMMethod object after adding the specified qualifier.
        @exception AlreadyExistsException if the qualifier already exists.
        <pre>
            CIMMethod m1(CIMName ("getHostName"), CIMTYPE_STRING);
            m1.addQualifier(CIMQualifier(CIMName ("stuff"), true));
        </pre>
    */
    CIMMethod& addQualifier(const CIMQualifier& x);

    /** Searches for a qualifier with the specified input name.
        @param name - CIMName of the qualifier to be found.
        @return Index of the qualifier found or PEG_NOT_FOUND
        if not found.
        <pre>
            CIMMethod m1(CIMName ("getHostName"), CIMTYPE_STRING);
            m1.addQualifier(CIMQualifier(CIMName ("stuff"), true));
            assert(m1.findQualifier(CIMName ("stuff")) != PEG_NOT_FOUND);
        </pre>
    */
    Uint32 findQualifier(const CIMName& name) const;

    /** Gets the CIMQualifier defined by the input parameter.
        @param index - Index of the qualifier requested.
        @return CIMQualifier object representing the qualifier found.
        @exception IndexOutOfBoundsException exception if the index is
        outside the range of parameters available from the CIMMethod.
        <pre>
            CIMMethod m1(CIMName ("getHostName"), CIMTYPE_STRING);
            m1.addQualifier(CIMQualifier(CIMName ("stuff"), true));
            Uint32 posQualifier;
            posQualifier = m1.findQualifier(CIMName ("stuff"));
            CIMQualifier q = m1.getQualifier(posQualifier);
        </pre>
    */
    CIMQualifier getQualifier(Uint32 index);

    /** Gets the CIMQualifier defined by the input parameter.
        @param index - Index of the qualifier requested.
        @return CIMConstQualifier object representing the qualifier found.
        @exception IndexOutOfBoundsException exception if the index is
        outside the range of parameters available from the CIMMethod.
        <pre>
            CIMMethod m1(CIMName ("getHostName"), CIMTYPE_STRING);
            m1.addQualifier(CIMQualifier(CIMName ("stuff"), true));
            
            Uint32 posQualifier;
            posQualifier = m1.findQualifier(CIMName ("stuff"));
            CIMQualifier q = m1.getQualifier(posQualifier);
        </pre>
    */
    CIMConstQualifier getQualifier(Uint32 index) const;

    /** Removes the specified CIMQualifier from this method.
        @param index - Index of the qualifier to remove.
        @exception IndexOutOfBoundsException exception if the index is
            outside the range of parameters available from the CIMMethod.
    */
    void removeQualifier(Uint32 index);

    /** Returns the number of Qualifiers attached to this CIMMethod object.
        @return the number of qualifiers in the CIM Method.
        <pre>
            CIMMethod m1(CIMName ("getHostName"), CIMTYPE_STRING);
            m1.addQualifier(CIMQualifier(CIMName ("stuff"), true));
            m1.addQualifier(CIMQualifier(CIMName ("stuff2"), true));
            assert(m1.getQualifierCount() == 2);
        </pre>
    */
    Uint32 getQualifierCount() const;

    /** Adds the parameter defined by the input to the CIMMethod.
        @param x - CIMParameter to be added to the CIM Method.
        @return CIMMethod object after the specified parameter is added.
        <pre>
            CIMMethod m1(CIMName ("getHostName"), CIMTYPE_STRING);
            m1.addParameter(CIMParameter(CIMName ("ipaddress"), CIMTYPE_STRING));
        </pre>
    */
    CIMMethod& addParameter(const CIMParameter& x);

    /** Finds the parameter with the specified name.
        @param name - Name of parameter to be found.
        @return Index of the parameter object found or PEG_NOT_FOUND 
        if the property is not found.
        <pre>
            Uint32 posParameter;
            posParameter = m1.findParameter(CIMName ("ipaddress"));
            if (posParameter != PEG_NOT_FOUND)
                ...
        </pre>
    */
    Uint32 findParameter(const CIMName& name) const;

    /** Gets the parameter defined by the specified index.
        @param index - Index for the parameter to be returned.
        @return CIMParameter object requested.
        @exception IndexOutOfBoundsException if the index is outside 
            the range of available parameters.
        <pre>
            CIMParameter cp = m1.getParameter(m1.findParameter(CIMName ("ipaddress")));
        </pre>
    */
    CIMParameter getParameter(Uint32 index);

    /** Gets the parameter defined for the specified index.
        @param index - Index for the parameter to be returned.
        @return CIMConstParameter object requested.
        @exception IndexOutOfBoundsException if the index is outside 
            the range of available parameters
    */
    CIMConstParameter getParameter(Uint32 index) const;

    /** Removes the CIMParameter defined by the specified index.
        @param index - Index of the parameter to be removed.
        @exception IndexOutOfBoundsException if the index is outside the
            range of parameters available from the CIMMethod.
    */
    void removeParameter (Uint32 index);

    /** Gets the count of Parameters defined in the CIMMethod.
        @return - count of the number of parameters attached to the CIMMethod.
    */
    Uint32 getParameterCount() const;

    /** Determines if the object has not been initialized.
        @return  true if the object has not been initialized,
                 false otherwise false.
     */
    Boolean isUninitialized() const;

    /** Compares with another CIMConstMethod.
        @param x - CIMConstMethod object for the method to be compared.
        @return true if this method is identical to the one specified.
        <pre>
            CIMMethod m1(CIMName ("getHostName"), CIMTYPE_STRING);
            CIMMethod m2(CIMName ("test"), CIMTYPE_STRING);
            assert(!m1.identical(m2));
        </pre>
    */
    Boolean identical(const CIMConstMethod& x) const;

    /** Makes a deep copy (clone) of this method.
        @return copy of the CIMMethod object.
    */
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

/** The CIMConstMethod class is used to represent CIM methods in the
    same manner as the CIMMethod class except that the const attribute
    is applied to the objects created. This class includes equivalents
    to the methods from CIMMethod that are usable in a const object including
    constructors, (i.e. getter methods) and the destructor.
    The const form of the object is used TBD
    ATTN: Complete the explanation of why.
    @see CIMMethod
*/
class PEGASUS_COMMON_LINKAGE CIMConstMethod
{
public:

    /**  Creates a new default CIMConstMethod object.
        @see  CIMMethod()
    */
    CIMConstMethod();

    /// @see CIMMethod()
    CIMConstMethod(const CIMConstMethod& x);

    /** Creates a new CIMConstMethod object from an
        existing CIMMethod object.  Creates a pointer
        to the existing representation.
        @return CIMConstMethod object reference.
        @see CIMMethod()
    */
    CIMConstMethod(const CIMMethod& x);

    ///
    CIMConstMethod(
    const CIMName& name,
    CIMType type,
    const CIMName& classOrigin = CIMName(),
    Boolean propagated = false);

    /// destructor.
    ~CIMConstMethod();

    /// assignment operator. 
    CIMConstMethod& operator=(const CIMConstMethod& x);

    /// assignment operator.
    CIMConstMethod& operator=(const CIMMethod& x);

    /** gets CIMMethod name. Operation is the same as
        CIMMethod getName().
        @see CIMMethod
        
    */
    const CIMName& getName() const;

    /** gets CIMMethod CIMType. Functions the same as
        CIMMethod getType();
        @see CIMMethod
        
    */
    CIMType getType() const;

    /**  gets ClassOrigin attribute. Functions the same
         as CIMMethod getClassOrigin()
         @see CIMMethod
        
    */
    const CIMName& getClassOrigin() const;

    /**  gets Propagated attribute. Functions the same
         as CIMMethod getPropagated()
         @see CIMMethod
    */
    Boolean getPropagated() const;

    /** finds qualifier based on name. Functions the
        same as the CIMMethod findQualifier() method.
        
        @see CIMMethod
    */
    Uint32 findQualifier(const CIMName& name) const;

    /** gets qualifier based on index. Functions the
        same as the CIMMethod getQualifier() method.
        @see CIMMethod
    */
    CIMConstQualifier getQualifier(Uint32 index) const;

    /** gets qualifier count based on name. Functions the
        same as the CIMMethod getQualifierCount() method.
        @see CIMMethod
    */
    Uint32 getQualifierCount() const;

    /** finds method parameter based on name. Functions the
        same as the CIMMethod findParameter() method.
        @see CIMMethod
    */
    Uint32 findParameter(const CIMName& name) const;

    /** gets method parameter based on index. Functions the
        same as the CIMMethod getParameter() method.
        @see CIMMethod
    */
    CIMConstParameter getParameter(Uint32 index) const;

    /** finds method parameter count based on name. Functions the
        same as the CIMMethod getParameterCount() method.
        @see CIMMethod
    */
    Uint32 getParameterCount() const;

    /** Determines if CIMMethod is unitinitialized. Functions the
        same as corresponding funtion in CIMMethod class.
        @see CIMMethod
    */
    Boolean isUninitialized() const;

    /** Determines if CIMMethod is identical to object define in parameter.
        Functions the same as corresponding funtion in CIMMethod class.
        @see CIMMethod
    */
    Boolean identical(const CIMConstMethod& x) const;

    /** clones a CIMMethod object by making a deep copy. Functions the
        same as corresponding funtion in CIMMethod class.
        @see CIMMethod
    */
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
