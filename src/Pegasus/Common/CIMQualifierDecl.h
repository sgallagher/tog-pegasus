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

#ifndef Pegasus_QualifierDecl_h
#define Pegasus_QualifierDecl_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/Linkage.h>
#include <Pegasus/Common/CIMName.h>
#include <Pegasus/Common/Array.h>
#include <Pegasus/Common/CIMQualifierDecl.h>
#include <Pegasus/Common/CIMFlavor.h>
#include <Pegasus/Common/CIMScope.h>
#include <Pegasus/Common/CIMType.h>
#include <Pegasus/Common/CIMValue.h>

PEGASUS_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////
//
// CIMQualifierDecl
//
////////////////////////////////////////////////////////////////////////////////

class CIMConstQualifierDecl;
class CIMClassRep;
#if defined(PEGASUS_PLATFORM_AIX_RS_IBMCXX) || defined(PEGASUS_PLATFORM_ZOS_ZSERIES_IBM) || defined(PEGASUS_PLATFORM_OS400_ISERIES_IBM)
class CIMQualifierDeclRep;
#endif
/** Class CIMQualifierDecl
  This class defines the CIM Qualifier Declarations. Note that the Declarations
  are not the same as CIM Qualiifers as seen on Classes, properties, etc.
  These are the original declarations of qualifiers (i.e. input from the compiler
  qualifierDeclarations.
*/
class PEGASUS_COMMON_LINKAGE CIMQualifierDecl
{
public:
    /// Constructor - 
    CIMQualifierDecl();

    /// Constructor - Creates a CIMQualifierDecl from another CIMQualifierDecl
    CIMQualifierDecl(const CIMQualifierDecl& x);

    /** Constructor for CIMQualifierDecl. Constructs a single CIMQualifierDecl
        object.
        @param name Name of the Qualifier being declared
        @param value CIMValue for the qualifier
        @param scope scope of the qualifier
        @param flavor Optional definition of the flavor for the qualifier.  
        CIMFlavor::DEFAULTS is used if no value supplied.  This is how we
        install the flavor defaults defined in the CIM specificaiton
        @param arraySize Optional integer defining the arraysize if the
        qualifier is an array type with fixed value array. The default is
        zero indicating that the qualifier declaration is not a fixed size
        array.
    */
    CIMQualifierDecl(
        const CIMName& name, 
        const CIMValue& value, 
        const CIMScope & scope,
        const CIMFlavor & flavor = CIMFlavor (CIMFlavor::DEFAULTS),
        Uint32 arraySize = 0);

    /// Destructor
    ~CIMQualifierDecl();

    /// Operator
    CIMQualifierDecl& operator=(const CIMQualifierDecl& x);

    /**
    Get the name of a CIMQualifierDecl object.
    */
    const CIMName& getName() const;

    /** setName - Puts the name into a CIMQualifierdecl
        @param Name CIMName containing name to be put on this qualifier.
    */
    void setName(const CIMName& name);

    //  ATTN: P3 Documentation clean up
    /** getType - gets the Qualifier Declaration type which is the
        value type (boolean, etc. for this qualifier.
        @return Returns the type as CIMType
    */
    CIMType getType() const;

    //  ATTN: P1 KS 04/17/02 Confirm this correspond to array type or fixed array?
    /** isArray - test if this qualifier declaration is an array type.
        @return Boolean true if this is array type.
    */
    Boolean isArray() const;

    /** CIMMethod
    */
    const CIMValue& getValue() const;

    /** CIMMethod
    */
    void setValue(const CIMValue& value);

    /** CIMMethod
    */
    const CIMScope & getScope() const;

    /** getFlavor - Gets the Flavor definition from the qualifier declaration
        constant
        @return - a CIMFlavor object containing the flavor flags.  The 
        CIMFlavor hasFlavor method can be used to test against the flavor 
        constants defined in CIMFlavor.
        @SeeAlso 
    */
    const CIMFlavor & getFlavor() const;

    /** CIMMethod
    */
    Uint32 getArraySize() const;

    /**
        Determines if the object has not been initialized.

        @return  True if the object has not been initialized,
                 False otherwise
     */
    Boolean isUninitialized() const;
    
    /** identical Compares two qualifier declarations
        @return Returns true if they are identical
    */ 
    Boolean identical(const CIMConstQualifierDecl& x) const;

    /** CIMMethod
    */
    CIMQualifierDecl clone() const;

private:

    CIMQualifierDecl(CIMQualifierDeclRep* rep);

    void _checkRep() const;

    CIMQualifierDeclRep* _rep;

    friend class CIMConstQualifierDecl;
    friend class CIMClassRep;
    friend class XmlWriter;
    friend class MofWriter;
};

////////////////////////////////////////////////////////////////////////////////
//
// CIMConstQualifierDecl
//
////////////////////////////////////////////////////////////////////////////////

class PEGASUS_COMMON_LINKAGE CIMConstQualifierDecl
{
public:
    ///
    CIMConstQualifierDecl();

    ///
    CIMConstQualifierDecl(const CIMConstQualifierDecl& x);

    ///
    CIMConstQualifierDecl(const CIMQualifierDecl& x);

    /** Constructor creates a CIMConstQualiferDecl.
        @param name Name of the qualifier declaration object
        @param value CIMValue to put into the declaration
        @param scope
        @param flavor Optional flavor to define for the declaration. Default
        if not specified is CIMFlavor::DEFAULTS
        @arraysize Optional integer with array size for fixed size arrays.  If
        not supplied, assumes 0.
    */
    CIMConstQualifierDecl(
        const CIMName& name, 
        const CIMValue& value, 
        const CIMScope & scope,
        const CIMFlavor & flavor = CIMFlavor (CIMFlavor::DEFAULTS),
        Uint32 arraySize = 0);

    ~CIMConstQualifierDecl();

    ///
    CIMConstQualifierDecl& operator=(const CIMConstQualifierDecl& x);

    ///
    CIMConstQualifierDecl& operator=(const CIMQualifierDecl& x);

    ///
    const CIMName& getName() const;

    ///
    CIMType getType() const;

    ///
    Boolean isArray() const;

    ///
    const CIMValue& getValue() const;

    ///
    const CIMScope & getScope() const;

    ///
    const CIMFlavor & getFlavor() const;

    ///
    Uint32 getArraySize() const;

    Boolean isUninitialized() const;

    ///
    Boolean identical(const CIMConstQualifierDecl& x) const;

    ///
    CIMQualifierDecl clone() const;

private:

    void _checkRep() const;

    CIMQualifierDeclRep* _rep;

    friend class CIMQualifierDecl;
    friend class XmlWriter;
    friend class MofWriter;
};

#define PEGASUS_ARRAY_T CIMQualifierDecl
# include <Pegasus/Common/ArrayInter.h>
#undef PEGASUS_ARRAY_T

PEGASUS_NAMESPACE_END

#endif /* Pegasus_QualifierDecl_h */
