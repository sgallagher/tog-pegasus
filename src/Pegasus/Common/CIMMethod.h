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

/** The CIMMethod class is used to represent CIM methods in Pegasus. A Pegasus CIMMethod
    consists of the following entities:
    <ul>
        <li>Name of the method, a CIMName.  The name must be a legal name for a 
        CIMProperty or Method \Ref{CIMName}.
    
        <li>CIM type of the return value of the method, a \Ref{CIMType}. This is any
        of the predefined CIM types (for example: Boolean).
    
        <li>Optional qualifiers (see \Ref{CIMQualifier}) for the method.  A method 
        can contain zero or more CIMQualifiers and methods are provided to 
        manipulate the set of qualifiers attached to a CIMMethod object.
    
        <li>Optional parameters (see \Ref{CIMParameter} for the method which are 
        the parameters to be placed on a CIM Method operation.  A CIMMethod can 
        contain zero or more CIMParameters and methods are provided in CIMMethod 
        to manipulate the set of CIMParameters attached to a CIMMethod object.  
    </ul>
    In addition a CIMMethod contains the following internal attributes:
    <ul>
        <li><b>propagated</b> - An attribute defining whether this CIMMethod is 
        propagated from a superclass.  Note that this is normally set as part of 
        completing the definition of objects (resolving) when they are placed in a 
        repository and is NOT automatically set when creating a local object.  It 
        is part of the context of the object within the repository.  It can only 
        logically be set in context of the superclass of which this CIMMethod is 
        defined.  
        <li><b>ClassOrigin</b> - An attribute defining the superclass in which this 
        CIMMethod was originally defined.  This is normally set within the context
        of the CIM Server context of other CIM objects (for example,  a 
        repository).  This attribute is available from objects retrieved from the 
        CIM Server, for example and provides information on the defintion of this method in the class hiearchy 
        (this object or a superclass or instance of a superclass)was originally 
        defined.  Together the propagated and ClassOrigin attributes can be used 
        to determine if methods originated with the current object or were 
        inherited from higher levels in the hiearchy.  
    </ul>
    CIMMethods are generally in the context of a CIMClass.
    
    CIMMethod is a shared class that uses shared representations, meaning that multiple
    CIMMethods objects may refer to the came copy of data. Assignment and copy
    operators create new references to the same data, not distinct copies. A distinct copy
    may be created using teh clone method.
    {@link Shared Classes}
    @see CIMConstMethod
    @see CIMParameters
    @see CIMQualifiers
    @see CIMType
    @see CIMClass
*/
class PEGASUS_COMMON_LINKAGE CIMMethod
{
public:

    /** Creates a new unitialized CIMMethod object. 
        The only thing that can be done with this object is to copy another object 
        into it.  Other methods such as setName, etc.  will fail with an 
        UnitializedObjectException.  The object has the state unitialized which 
        can be tested with the isUnitialized() method.  
        @see isUnitialized()
        @see UnitializedObjectException
    */
    CIMMethod();

    /** Creates a new CIMMethod object from another CIMMethod object.
        The new CIMMethodObject references the same copy of data as the
        specified object; no copy is made.
        @param x CIMMethod object from which to create the new CIMMethod object.
        <pre>
            CIMMethod m1(CIMName ("getHostName"), CIMTYPE_STRING);
            const CIMMethod cm1(m1);
        </pre>
        {@link Shared Classes}
    */
    CIMMethod(const CIMMethod& x);

    /** Creates a CIMMethod object with the specified name and other input attributes.
        @param name CIMName defining the name for the method.
        
        @param type CIMType defining method return  data type. 
            
        @param classOrigin (optional) CIMName representing the class origin. Note
            that this should normally not be used.  If not provided set to
            CIMName() (Null name).
        @param propagated Optional flag indicating whether the definition of the 
            CIM Method is local to the CIM Class (respectively, Instance) in which 
            it appears, or was propagated without modification from the a 
            Superclass. Default is false. Note that this attribute is normally
            not set by CIM Clients but is used internally within the CIM
            Server.
        <p><b>Example:</b>
        <pre>
            CIMMethod m1(CIMName ("getHostName"), CIMTYPE_STRING);
        </pre>
    */
    CIMMethod(
        const CIMName& name,
        CIMType type,
        const CIMName& classOrigin = CIMName(),
        Boolean propagated = false);

    /** Destructor for the CIMMethod. The shared data copy remains valid until
        all referring objects are destructed
        {@link Shared Classes}
    */
    ~CIMMethod();

    /** The assignment operator assigns one CIM method to another.  
        After the assignment, both CIMMethod objects refer to the same
        data copy; a distinct copy is not created.
        
        <p><b>Example:</b>
        <pre>
            CIMMethod m1(CIMName ("getHostName"), CIMTYPE_STRING);
            CIMMethod m2;
            m2 = m1;
        </pre>
    */
    CIMMethod& operator=(const CIMMethod& x);

    /** Gets the name of the method.
        @return CIMName with the name of the method.
        <p><b>Example:</b>
        <pre>
            CIMMethod m1(CIMName ("getHostName"), CIMTYPE_STRING);
            assert(m1.getName() == CIMName ("getHostName"));
        </pre>
    */
    const CIMName& getName() const;

    /** Sets the method name.
        @param name CIMName for the method name. Replaces any
            previously defined name for this method object.
        <p><b>Example:</b>
        <pre>
            CIMMethod m2(CIMName ("test"), CIMTYPE_STRING);
            m2.setName(CIMName ("getVersion"));
        </pre>
    */
    void setName(const CIMName& name);

    /** Gets the method return data type.
        @return CIMType containing the method return data type for this 
        method.
        @exception Throws UnitializedObjectException if object is not
        initialized.
        <p><b>Example:</b>
        <pre>
            CIMMethod m1(CIMName ("getHostName"), CIMTYPE_STRING);
            assert(m1.getType() == CIMTYPE_STRING);
        </pre>
    */
    CIMType getType() const;

    /** Sets the method return data type to the specified CIMtype. 
        This is the type of the CIMValue
        that is returned on a CIM method invocation
        @param type CIMType to be set into the CIMMethod object.
        @exception Throws UnitializedObjectException if object is not
        initialized.
        <p><b>Example:</b>
        <pre>
            CIMMethod m1();
            m1.setName(CIMName ("getVersion"));
            assert(m1.setType(CIMTYPE_STRING));
        </pre>
    */
    void setType(CIMType type);

    /** Gets the class in which this method was defined. This information
        is normally available with methods that are part of classes and
        instances returned from a CIM Server.
        @return CIMName containing the classOrigin field. 
    */
    const CIMName& getClassOrigin() const;

    /** Sets the ClassOrigin attribute with the classname defined on 
        the input parameter. Normally this method is used internally
        as part of the use objects containing methods (classes
        and instances) in the context of the CIM Server. 
        @param classOrigin CIMName parameter defining the name
        of the class origin.
        @exception Throws UnitializedObjectException if object is not
        initialized.
    */
    void setClassOrigin(const CIMName& classOrigin);

    /** Tests the propagated attribute of the object.  The propagated attribute
        indicates if this method was propagated from a higher level
        class.  Normally this attribute is set as part of putting
        classes into the repository (resolving the class).  It is 
        available on methods in classes read from the repository and
        on instances that are read from the instance repository.
        @return True if method is propagated; otherwise,false.
    */
    Boolean getPropagated() const;

    /** Sets the Propagated qualifier. Normally this is used by the functions
        that resolve classes and instances as part of the installation into
        a repository.
        @param propagated Flag indicating method is propagated from superclass propagation.
        True means that the method was propagated from superclass.
        @exception Throws UnitializedObjectException if object is not
        initialized.
    */
    void setPropagated(Boolean propagated);

    /** Adds the specified qualifier to the CIMmethod object. 
        @param x CIMQualifier object representing the qualifier
        to be added.
        @return The CIMMethod object after adding the specified qualifier.
        @exception Throws AlreadyExistsException if the qualifier already exists.
        <p><b>Example:</b>
        <pre>
            CIMMethod m1(CIMName ("getHostName"), CIMTYPE_STRING);
            m1.addQualifier(CIMQualifier(CIMName ("stuff"), true));
        </pre>
    */
    CIMMethod& addQualifier(const CIMQualifier& x);

    /** Searches for a qualifier with the specified input name.
        @param name CIMName of the qualifier to be found.
        @return Zero origin index of the qualifier found or PEG_NOT_FOUND
        if not found.
        @exception Throws UnitializedObjectException if object is not
        initialized.
        <p><b>Example:</b>
        <pre>
            CIMMethod m1(CIMName ("getHostName"), CIMTYPE_STRING);
            m1.addQualifier(CIMQualifier(CIMName ("stuff"), true));
            assert(m1.findQualifier(CIMName ("stuff")) != PEG_NOT_FOUND);
        </pre>
    */
    Uint32 findQualifier(const CIMName& name) const;

    /** Gets the CIMQualifier defined by the input parameter.
        @param index Zero origin index of the qualifier requested.
        @return CIMQualifier object representing the qualifier found.
        @exception Throws IndexOutOfBoundsException exception if the index is
        outside the range of parameters available from the CIMMethod.
        <p><b>Example:</b>
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
        @param index Index of the qualifier requested.
        @return CIMConstQualifier object representing the qualifier found.
        @exception Throws IndexOutOfBoundsException exception if the index is
        outside the range of parameters available from the CIMMethod.
        <p><b>Example:</b>
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
        @param index Index of the qualifier to remove.
        @exception Throws IndexOutOfBoundsException exception if the index is
            outside the range of parameters available from the CIMMethod.
        <p><b>Example:</b>
        <pre>
            // remove all qualifiers from a class
        	Uint32 count = 0;
        	while((count = cimClass.getQualifierCount()) > 0)
        		cimClass.removeQualifier(count - 1);
        </pre>
    */
    void removeQualifier(Uint32 index);

    /** Returns the number of Qualifiers attached to this CIMMethod object.
        @return The number of qualifiers attached to the CIM Method.
        <p><b>Example:</b>
        <pre>
            CIMMethod m1(CIMName ("getHostName"), CIMTYPE_STRING);
            m1.addQualifier(CIMQualifier(CIMName ("stuff"), true));
            m1.addQualifier(CIMQualifier(CIMName ("stuff2"), true));
            assert(m1.getQualifierCount() == 2);
        </pre>
    */
    Uint32 getQualifierCount() const;

    /** Adds the parameter defined by the input to the CIMMethod.
        @param x CIMParameter to be added to the CIM Method.
        @return CIMMethod object after the specified parameter is added.
        <p><b>Example:</b>
        @exception Throws UnitializedObjectException if object is not
        initialized.
        <pre>
            CIMMethod m1(CIMName ("getHostName"), CIMTYPE_STRING);
            m1.addParameter(CIMParameter(CIMName ("ipaddress"), CIMTYPE_STRING));
        </pre>
    */
    CIMMethod& addParameter(const CIMParameter& x);

    /** Finds the parameter with the specified name.
        @param name CIMName of parameter to be found.
        @return Index of the parameter object found or PEG_NOT_FOUND 
        if the property is not found.
        <p><b>Example:</b>
        <pre>
            Uint32 posParameter;
            posParameter = m1.findParameter(CIMName ("ipaddress"));
            if (posParameter != PEG_NOT_FOUND)
                ...
        </pre>
    */
    Uint32 findParameter(const CIMName& name) const;

    /** Gets the parameter defined by the specified index.
        @param index Index for the parameter to be returned.
        @return CIMParameter object requested.
        @exception Throws IndexOutOfBoundsException if the index is outside 
            the range of available parameters.
        <p><b>Example:</b>
        <pre>
            CIMParameter cp = m1.getParameter(m1.findParameter(CIMName ("ipaddress")));
        </pre>
    */
    CIMParameter getParameter(Uint32 index);

    /** Gets the parameter defined for the specified index.
        @param index Index for the parameter to be returned.
        @return CIMConstParameter object requested.
        @exception Throws IndexOutOfBoundsException if the index is outside 
            the range of available parameters
    */
    CIMConstParameter getParameter(Uint32 index) const;

    /** Removes the CIMParameter defined by the specified index.
        @param index Index of the parameter to be removed.
        @exception Throws IndexOutOfBoundsException if the index is outside the
            range of parameters available from the CIMMethod.
    */
    void removeParameter (Uint32 index);

    /** Gets the count of Parameters defined in the CIMMethod.
        @return Count of the number of parameters attached to the CIMMethod.
    */
    Uint32 getParameterCount() const;

    /** Determines if the object has not been initialized.
        @return  True if the object has not been initialized;
                 otherwise false.
        <p><b>Example:</b>
        <pre>
            CIMMethod m1;
            assert(m1.isUnitialized());
        </pre>
     */
    Boolean isUninitialized() const;

    /** Compares with a CIMConstMethod.
        @param x CIMConstMethod object for the method to be compared.
        @return True if this method is identical to the one specified.
        <p><b>Example:</b>
        <pre>
            CIMMethod m1(CIMName ("getHostName"), CIMTYPE_STRING);
            CIMConstMethod m2(CIMName ("test"), CIMTYPE_STRING);
            assert(!m1.identical(m2));
        </pre>
    */
    Boolean identical(const CIMConstMethod& x) const;

    /** Makes a clone (deep copy) of this method. This creates
        a new copy of all of the components of the method including
        parameters and qualifiers.
        @return Independent copy of the CIMMethod object.
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
    constructors, (for example: getter methods) and the destructor.
    
    REVIEWERS: ATTN: Complete the explanation of why.
    @see CIMMethod()
*/
class PEGASUS_COMMON_LINKAGE CIMConstMethod
{
public:

    /**  Creates a new empty CIMConstMethod object.
        @see  CIMMethod()
    */
    CIMConstMethod();

    /** Creates a new CIMConstMethod object from another CIMConstMethod object.
        The new CIMMethodObject references the same copy of data as teh specified object,
        no copy is made.
        @param x CIMMethod object from which to create the newCIMMethod object.
        <pre>
            CIMMethod m1(CIMName ("getHostName"), CIMTYPE_STRING);
            const CIMMethod cm1(m1);
        </pre>
        {@link Shared Classes}
    */
    CIMConstMethod(const CIMConstMethod& x);

    /** Creates a new CIMConstMethod object from an
        existing CIMMethod object.  Creates a pointer
        to the existing representation.
        @return CIMConstMethod object reference.
        @see CIMMethod()
    */
    CIMConstMethod(const CIMMethod& x);

    /** Creates a CIMConstMethod object with the specified name and other input attributes.
        @param name CIMName defining the name for the method.
        
        @param type CIMType defining method return  data type. 
            
        @param classOrigin (optional) CIMName representing the class origin. Note
            that this should normally not be used.  If not provided set to
            CIMName() (Null name).
        @param propagated Optional flag indicating whether the definition of the 
            CIM Method is local to the CIM Class (respectively, Instance) in which 
            it appears, or was propagated without modification from the a 
            Superclass. Default is false. Note that this attribute is normally
            not set by CIM Clients but is used internally within the CIM
            Server.
        <p><b>Example:</b>
        <pre>
            CIMConstMethod m1(CIMName ("getHostName"), CIMTYPE_STRING);
        </pre>
    */
    CIMConstMethod(
    const CIMName& name,
    CIMType type,
    const CIMName& classOrigin = CIMName(),
    Boolean propagated = false);

    /** Destructor for the CIMConstMethod. The shared data copy remains valid until
        all referring objects are destructed
        {@link Shared Classes}
    */
    ~CIMConstMethod();

    /** The assignment operator assigns one CIMConstMethod object to another.  
        After the assignment, both CIMMethod objects refer to the same
        data copy; a distinct copy is not created.
        
        <p><b>Example:</b>
        <pre>
            CIMConstMethod m1(CIMName ("getHostName"), CIMTYPE_STRING);
            CIMConstMethod m2;
            m2 = m1;
        </pre>
    */
    CIMConstMethod& operator=(const CIMConstMethod& x);

    /** The assignment operator assigns a CIMMethod object to a
        CIMConstMethod.  
        After the assignment, both objects refer to the same
        data copy; a distinct copy is not created.
        
        <p><b>Example:</b>
        <pre>
            CIMMethod m1(CIMName ("getHostName"), CIMTYPE_STRING);
            CIMConstMethod m2;
            m2 = m1;
        </pre>
    */
    CIMConstMethod& operator=(const CIMMethod& x);

    /** Gets the name of the method.
        @return CIMName with the name of the method.
        <p><b>Example:</b>
        <pre>
            CIMConstMethod m1(CIMName ("getHostName"), CIMTYPE_STRING);
            assert(m1.getName() == CIMName ("getHostName"));
        </pre>
    */
    const CIMName& getName() const;

    /** Gets the method return data type.
        @return CIMType containing the method return data type for this 
        method.  
        <p><b>Example:</b>
        <pre>
            CIMConstMethod m1(CIMName ("getHostName"), CIMTYPE_STRING);
            assert(m1.getType() == CIMTYPE_STRING);
        </pre>
    */
    CIMType getType() const;

    /** Gets the class in which this method was defined. This information
        is normally available with methods that are part of classes and
        instances returned from a CIM Server.
        @return CIMName containing the classOrigin field. 
    */
    const CIMName& getClassOrigin() const;

    /** Tests the propagated attribute of the object.  The propagated attribute
        indicates if this method was propagated from a higher level
        class.  Normally this attribute is set as part of putting
        classes into the repository (resolving the class).  It is 
        available on methods in classes read from the repository and
        on instances that are read from the instance repository.
        @return True if method is propagated; otherwise,false.
    */
    Boolean getPropagated() const;

    /** Searches for a qualifier with the specified input name.
        @param name CIMName of the qualifier to be found.
        @return Index of the qualifier found or PEG_NOT_FOUND
        if not found.
        <p><b>Example:</b>
        <pre>
            CIMConstMethod m1(CIMName ("getHostName"), CIMTYPE_STRING);
            m1.addQualifier(CIMQualifier(CIMName ("stuff"), true));
            assert(m1.findQualifier(CIMName ("stuff")) != PEG_NOT_FOUND);
        </pre>
    */
    Uint32 findQualifier(const CIMName& name) const;

    /** Gets the CIMQualifier defined by the input parameter.
        @param index Index of the qualifier requested.
        @return CIMQualifier object representing the qualifier found.
        @exception Throws IndexOutOfBoundsException exception if the index is
        outside the range of parameters available from the CIMMethod.
        <p><b>Example:</b>
        <pre>
            CIMMethod m1(CIMName ("getHostName"), CIMTYPE_STRING);
            m1.addQualifier(CIMQualifier(CIMName ("stuff"), true));
            Uint32 posQualifier;
            posQualifier = m1.findQualifier(CIMName ("stuff"));
            CIMQualifier q = m1.getQualifier(posQualifier);
        </pre>
    */
    CIMConstQualifier getQualifier(Uint32 index) const;

    /** Returns the number of Qualifiers attached to this CIMMethod object.
        @return The number of qualifiers in the CIM Method.
        <p><b>Example:</b>
        <pre>
            CIMConstMethod m1(CIMName ("getHostName"), CIMTYPE_STRING);
            assert(m1.getQualifierCount() == 0);
        </pre>
    */
    Uint32 getQualifierCount() const;

    /** Finds the parameter with the specified name.
        @param name CIMName of parameter to be found.
        @return Index of the parameter object found or PEG_NOT_FOUND 
        if the property is not found.
        <p><b>Example:</b>
        <pre>
            Uint32 posParameter;
            posParameter = m1.findParameter(CIMName ("ipaddress"));
            if (posParameter != PEG_NOT_FOUND)
                ...
        </pre>
    */
    Uint32 findParameter(const CIMName& name) const;

    /** Gets the parameter defined by the specified index.
        @param index Index for the parameter to be returned.
        @return CIMParameter object requested.
        @exception Throws IndexOutOfBoundsException if the index is outside 
            the range of available parameters.
        <p><b>Example:</b>
        <pre>
            CIMConstParameter cp = m1.getParameter(m1.findParameter(CIMName ("ipaddress")));
        </pre>
    */
    CIMConstParameter getParameter(Uint32 index) const;

    /** Gets the count of Parameters defined in the CIMMethod.
        @return Count of the number of parameters attached to the CIMMethod.
    */
    Uint32 getParameterCount() const;

    /** Determines if the object has not been initialized.
        @return  True if the object has not been initialized;
                 otherwise false.
     */
    Boolean isUninitialized() const;

    /** Compares with a CIMConstMethod.
        @param x CIMConstMethod object for the method to be compared.
        @return True if this method is identical to the one specified.
        <p><b>Example:</b>
        <pre>
            CIMMethod m1(CIMName ("getHostName"), CIMTYPE_STRING);
            CIMConstMethod m2(CIMName ("test"), CIMTYPE_STRING);
            assert(!m1.identical(m2));
        </pre>
    */
    Boolean identical(const CIMConstMethod& x) const;

    /** Makes a clone (deep copy) of this CIMConstmethod. This creates
        a new copy of all of the components of the method including
        parameters and qualifiers.
        @return Independent copy of the CIMConstMethod object. Note that 
        it is returned as a CIMMethod, not const.
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
