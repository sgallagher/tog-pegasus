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

#ifndef Pegasus_QualifierDecl_h
#define Pegasus_QualifierDecl_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/String.h>
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
        @param flavor Optional defintion of the flavor for the qualifier.  
        CIMFlavor::DEFAULTS is used if no value supplied.  This is how we
        install the flavor defaults defined in the CIM specificaiton
        @param arraySize Optional integer defining the arraysize if the
        qualifier is an array type with fixed value array. The default is
        zero indicating that the qualifier declaration is not a fixed size
        array.
        @exception IllegalName if name argument not legal CIM identifier.
    */
    CIMQualifierDecl(
        const String& name, 
        const CIMValue& value, 
        Uint32 scope,
        Uint32 flavor = CIMFlavor::DEFAULTS,
        Uint32 arraySize = 0);

    /// Destructor
    ~CIMQualifierDecl();

    /// Operator
    CIMQualifierDecl& operator=(const CIMQualifierDecl& x);

    /** CIMMethod ATTN:
    */
    const String& getName() const;

    /** setName - Puts the name into a CIMQualifierdecl
        @param Name String containing name to be put on this qualifier.
        This must be a legal qualifier name.
        @exception - Throws IllegalName if name argument not a legal
        CIM Qualifier name
    */
    void setName(const String& name);

    /** getType - gets the Qualifier Declaration type which is the
        value type (boolean, etc. for this qualifier.
        ATTN: P3 Documentation clean up
        @return Returns the type as CIMType
    */
    CIMType getType() const;

    /** isArray - test if this qualifier declaration is an array type.
        @return Boolean true if this is array type.
        ATTN: P1 KS 04/17/02 Confirm this correspond to array type or fixed array?
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
    Uint32 getScope() const;

    /** getFlavor - Gets the Flavor definition from the qualifer declaration
        constant
        @return - a Uint32 integer containing the flavor flags.  This can be
        tested against the flavor constants defined in CIMFlavor.
        @SeeAlso 
    */
    Uint32 getFlavor() const;

    /** isFlavor - Boolean function that determines if particular flavor
        flags are set in the flavor variable of a qualifier.
        @param flavor - The flavor bits to test.
        Return True if the defined flavor is set.
        <pre>
        if (q.isFlavor(CIMType::TOSUBCLASS)
            do something based on TOSUBCLASS being true
        </pre>
    */
    Boolean isFlavor(Uint32 flavor) const;

    /** CIMMethod
    */
    Uint32 getArraySize() const;

#ifdef PEGASUS_INTERNALONLY
    /** CIMMethod
    */
    Boolean isNull() const;
#endif
    
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
        @exception IllegalName if name argument not legal CIM identifier.
    */
    CIMConstQualifierDecl(
        const String& name, 
        const CIMValue& value, 
        Uint32 scope,
        Uint32 flavor = CIMFlavor::DEFAULTS,
        Uint32 arraySize = 0);

    ~CIMConstQualifierDecl();

    ///
    CIMConstQualifierDecl& operator=(const CIMConstQualifierDecl& x);

    ///
    CIMConstQualifierDecl& operator=(const CIMQualifierDecl& x);

    ///
    const String& getName() const;

    ///
    CIMType getType() const;

    ///
    Boolean isArray() const;

    ///
    const CIMValue& getValue() const;

    ///
    Uint32 getScope() const;

    ///
    const Uint32 getFlavor() const;

    ///
    Boolean isFlavor(Uint32 flavor) const;

    ///
    Uint32 getArraySize() const;

#ifdef PEGASUS_INTERNALONLY
    ///
    Boolean isNull() const;
#endif

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
# include "ArrayInter.h"
#undef PEGASUS_ARRAY_T

PEGASUS_NAMESPACE_END

#endif /* Pegasus_QualifierDecl_h */
