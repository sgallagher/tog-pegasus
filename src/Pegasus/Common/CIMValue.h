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
// Modified By: Karl schopmeyer (k.schopmeyer@opengroup.org)
//              Roger Kumpf, Hewlett-Packard Company (roger_kumpf@hp.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_Value_h
#define Pegasus_Value_h

#include <Pegasus/Common/String.h>
#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/CIMType.h>
#include <Pegasus/Common/CIMReference.h>
#include <Pegasus/Common/CIMDateTime.h>
#include <Pegasus/Common/Exception.h>
#include <Pegasus/Common/Array.h>

PEGASUS_NAMESPACE_BEGIN

class CIMValueRep;

/**
    The CIMValue class represents a value of any of the CIM data types
    (see CIMType.h for a list of valid CIM data types). This class
    encapsulates a union which holds the current value. The class also
    has a type field indicating the type of that value.
*/
class PEGASUS_COMMON_LINKAGE CIMValue
{
public:
    /** Constructor with no value. This constructor creates an NULL CIMValue
        object set to null and with type CIMType:none and !arraytype.
    */
    CIMValue();

    /** Constructor with only the Type. This constructor creates a NULL CIMValue
        object with the array indicator set as shown and with the
    */
    CIMValue(CIMType type, Boolean isArray, Uint32 arraySize = 0);

    /** Constructor with the Value constructor and a value.  This constructs a
        CIMValue object with the type defined by the value constructor and the value
        installed
        <pre>
        ATTN: Add example
        </pre> 
    */
    CIMValue(Boolean x);

    /// Constructor
    CIMValue(Uint8 x);

    /// Constructor
    CIMValue(Sint8 x);

    /// Constructor
    CIMValue(Uint16 x);

    /// Constructor
    CIMValue(Sint16 x);

    /// Constructor
    CIMValue(Uint32 x);

    /// Constructor
    CIMValue(Sint32 x);

    /// Constructor
    CIMValue(Uint64 x);

    /// Constructor
    CIMValue(Sint64 x);

    /// Constructor
    CIMValue(Real32 x);

    /// Constructor
    CIMValue(Real64 x);

    /// Constructor
    CIMValue(const Char16& x);

    /// Constructor
    CIMValue(const String& x);

    /// Constructor
    CIMValue(const char* x);

    /// Constructor
    CIMValue(const CIMDateTime& x);

    /// Constructor
    CIMValue(const CIMReference& x);

    /// Constructor
    CIMValue(const Array<Boolean>& x);

    /// Constructor
    CIMValue(const Array<Uint8>& x);

    /// Constructor
    CIMValue(const Array<Sint8>& x);

    /// Constructor
    CIMValue(const Array<Uint16>& x);

    /// Constructor
    CIMValue(const Array<Sint16>& x);

    /// Constructor
    CIMValue(const Array<Uint32>& x);

    /// Constructor
    CIMValue(const Array<Sint32>& x);

    /// Constructor
    CIMValue(const Array<Uint64>& x);

    /// Constructor
    CIMValue(const Array<Sint64>& x);

    /// Constructor
    CIMValue(const Array<Real32>& x);

    /// Constructor
    CIMValue(const Array<Real64>& x);

    /// Constructor
    CIMValue(const Array<Char16>& x);

    /// Constructor
    CIMValue(const Array<String>& x);

    /// Constructor
    CIMValue(const Array<CIMDateTime>& x);

    /// Constructor
    CIMValue(const Array<CIMReference>& x);

    /// Constructor
    CIMValue(const CIMValue& x);

    /// Destructor
    ~CIMValue();

    /// Operator =
    CIMValue& operator=(const CIMValue& x);

    /** assign - The method to assign one CIMValue object to another CIMValue
    object.
    */
    void assign(const CIMValue& x);

    /** clear - Clears the attributes and value of the CIMValue object.
    <pre>
    ATTNDOC:
    </pre>
    */
    void clear();

    /** typeCompatible - Compares the types of two CIMvalues. This
        compares the type field and the array indicators.
        @return true if both are of the same type and both are either arrays
        or not. Else returns false.
        <pre>
            CIMValue a(Boolean(true);
            CIMValue b = a;
            if b.typeCompatible(a)
                ...
        </pre>
    */
    Boolean typeCompatible(const CIMValue& x) const;

    /** isArray - Determines if the value is an array
        @return TRUE if the value is an array
    */
    Boolean isArray() const;

    /** Returns whether the CIMvalue object is Null. 
        Null is the specific condition where no value has
        yet been set into the value.
        If a CIMValue object is Null, any get on that object
        will create an exception.
        @return Returns True if the CIMValue object is Null
    */
    Boolean isNull() const;

    /** getArraySize = Returns the size of an Array CIMValue
        @return The number of entries in the array
    */
    Uint32 getArraySize() const;

    /** getType - Gets the CIMType attribute for the CIMValue.
        The CIMType is defined in ATTN:
        @return Returns the CIMType value
    */
    CIMType getType() const;

    /** setNullValue - Sets the CIMType, the Array indicator and if it is
        the arraytype, the Arraysize of a CIMValue and sets the isNull
        Attribute.  This function is used to set up CIMValues an NULL but
        with valid CIMType and array characteristics (ex. when update from
        XML)
        @param type - The CIMType for this CIMValue
        @param isArray - Boolean indicating whether this is an array CIMValue
        @param arraySize (optional) Uint32 parameter indicating the array
        size
        @return void
        <pre>
            CIMValue value;                   // Create a CIMValue object
            value.setNullValue(CIMType::BOOLEAN, false);  // Set it       
        </pre>
    */
    void setNullValue(CIMType type, Boolean isArray, Uint32 arraySize = 0);

    /** set - Sets the type, Array attribute and puts the value provided
        into the value of the target CIMValue. This function sets the
        CIMValue to nonNull also. The result is a complete CIMValue object
        All of the CIMTypes defined in ATTN: and the Array types defined in
        ATTN: may be set.
        @param x Typed value (ex. Boolean(true).
        @return void
        <pre>
            CIMValue x;
            x.set(Uint16(9));
        </pre>
    */    
    void set(Boolean x);

    /// Set
    void set(Uint8 x);
    ///
    void set(Sint8 x);
    ///
    void set(Uint16 x);
    ///
    void set(Sint16 x);
    ///
    void set(Uint32 x);
    ///
    void set(Sint32 x);
    ///
    void set(Uint64 x);
    ///
    void set(Sint64 x);
    ///
    void set(Real32 x);
    ///
    void set(Real64 x);
    ///
    void set(const Char16& x);
    ///
    void set(const String& x);
    ///
    void set(const char* x);
    ///
    void set(const CIMDateTime& x);
    ///
    void set(const CIMReference& x);
    ///
    void set(const Array<Boolean>& x);
    ///
    void set(const Array<Uint8>& x);
    ///
    void set(const Array<Sint8>& x);
    ///
    void set(const Array<Uint16>& x);
    ///
    void set(const Array<Sint16>& x);
    ///
    void set(const Array<Uint32>& x);
    ///
    void set(const Array<Sint32>& x);
    ///
    void set(const Array<Uint64>& x);
    ///
    void set(const Array<Sint64>& x);
    ///
    void set(const Array<Real32>& x);
    ///
    void set(const Array<Real64>& x);
    ///
    void set(const Array<Char16>& x);
    ///
    void set(const Array<String>& x);
    ///
    void set(const Array<CIMDateTime>& x);
    ///
    void set(const Array<CIMReference>& x);

    /** get - Gets the value of a CIMValue
        @param Variable in which we return the value:
        @return void
                <pre>
                Uint32 v;
                CIMValue value(CIMValue::UINT32, UINT32(99));
                value.get(v);
                </pre>
    */
    void get(Boolean& x) const;
    ///
    void get(Uint8& x) const;
    ///
    void get(Sint8& x) const;
    ///
    void get(Uint16& x) const;
    ///
    void get(Sint16& x) const;
    ///
    void get(Uint32& x) const;
    ///
    void get(Sint32& x) const;
    ///
    void get(Uint64& x) const;
    ///
    void get(Sint64& x) const;
    ///
    void get(Real32& x) const;
    ///
    void get(Real64& x) const;
    ///
    void get(Char16& x) const;
    ///
    void get(String& x) const;
    ///
    void get(CIMDateTime& x) const;
    ///
    void get(CIMReference& x) const;
    ///
    void get(Array<Boolean>& x) const;
    ///
    void get(Array<Uint8>& x) const;
    //////
    void get(Array<Sint8>& x) const;
    ///
    void get(Array<Uint16>& x) const;
    ///
    void get(Array<Sint16>& x) const;
    ///
    void get(Array<Uint32>& x) const;
    ///
    void get(Array<Sint32>& x) const;
    ///
    void get(Array<Uint64>& x) const;
    ///
    void get(Array<Sint64>& x) const;
    ///
    void get(Array<Real32>& x) const;
    ///
    void get(Array<Real64>& x) const;
    ///
    void get(Array<Char16>& x) const;
    ///
    void get(Array<String>& x) const;
    ///
    void get(Array<CIMDateTime>& x) const; 
    ///
    void get(Array<CIMReference>& x) const; 

    /** Makes a deep copy (clone) of the given object. */
    CIMValue clone() const;

    /** toString - Converts the CIMvalue to a string.  Should only be
            used for output purposes.  To get an actual String value, use
            get(String &).
        @return - String output for CIMValue.
        @exception - Throws exception CIMValueInvalidType if the CIMValue
            has an invalid type. Normally this is a Pegasus internal error.
        <PRE>
        Example:
            String test;
            CIMValue value(Boolean(true));
            test = value.toString();  // puts "TRUE" into test
        </PRE>
        */
    String toString() const;

private:

    CIMValueRep* _rep;

    friend class CIMMethodRep;
    friend class CIMParameterRep;
    friend class CIMPropertyRep;
    friend class CIMQualifierRep;
    friend class CIMQualifierDeclRep;
    PEGASUS_COMMON_LINKAGE friend Boolean operator==(
        const CIMValue& x, 
        const CIMValue& y);
};

/** operator == compares two CIMValue objects for equality.
        @param x - First CIMvalue to compare
        @parm y - Second CIMValue to compare
        @return True if they are identical in type, attribute and value.
*/
PEGASUS_COMMON_LINKAGE Boolean operator==(const CIMValue& x, const CIMValue& y);

/** operator != compares two CIMValue objects for nonequality
*/
PEGASUS_COMMON_LINKAGE Boolean operator!=(const CIMValue& x, const CIMValue& y);

#define PEGASUS_ARRAY_T CIMValue
# include "ArrayInter.h"
#undef PEGASUS_ARRAY_T

PEGASUS_NAMESPACE_END

#endif /* Pegasus_Value_h */
