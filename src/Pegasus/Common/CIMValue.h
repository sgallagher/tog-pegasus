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
// $Log: CIMValue.h,v $
// Revision 1.2  2001/02/20 05:16:57  mike
// Implemented CIMInstance::getInstanceName()
//
// Revision 1.1  2001/02/18 18:39:06  mike
// new
//
// Revision 1.2  2001/02/18 02:49:00  mike
// Removed ugly workarounds for MSVC++ 5.0 (using SP3 now)
//
// Revision 1.1  2001/02/16 02:07:06  mike
// Renamed many classes and headers (using new CIM prefixes).
//
// Revision 1.3  2001/01/30 08:00:43  karl
// DOC++ Documentation update for header files
//
// Revision 1.2  2001/01/29 08:03:12  karl
// DOC++ Documentation
//
// Revision 1.1.1.1  2001/01/14 19:53:21  mike
// Pegasus import
//
//
//END_HISTORY

#ifndef Pegasus_Value_h
#define Pegasus_Value_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/CIMType.h>
#include <Pegasus/Common/String.h>
#include <Pegasus/Common/CIMDateTime.h>
#include <Pegasus/Common/Union.h>
#include <Pegasus/Common/Array.h>

PEGASUS_NAMESPACE_BEGIN

/**
    The CIMValue class represents a value of any of the CIM data types
    (see CIMType.h for a list of valid CIM data types). This class
    encapsulates a union which holds the current value. The class also
    has a type field indicating the type of that value.
*/
class PEGASUS_COMMON_LINKAGE CIMValue
{
public:
    ///	Constructor
    CIMValue();

    ///	Constructor
    CIMValue(Boolean x) { _init(); set(x); }

    ///	Constructor
    CIMValue(Uint8 x) { _init(); set(x); }

    ///	Constructor
    CIMValue(Sint8 x) { _init(); set(x); }

    ///	Constructor
    CIMValue(Uint16 x) { _init(); set(x); }

    ///	Constructor
    CIMValue(Sint16 x) { _init(); set(x); }

    ///	Constructor
    CIMValue(Uint32 x) { _init(); set(x); }

    ///	Constructor
    CIMValue(Sint32 x) { _init(); set(x); }

    ///	Constructor
    CIMValue(Uint64 x) { _init(); set(x); }

    ///	Constructor
    CIMValue(Sint64 x) { _init(); set(x); }

    ///	Constructor
    CIMValue(Real32 x) { _init(); set(x); }

    ///	Constructor
    CIMValue(Real64 x) { _init(); set(x); }

    ///	Constructor
    CIMValue(const Char16& x) { _init(); set(x); }

    ///	Constructor
    CIMValue(const String& x) { _init(); set(x); }

    ///	Constructor
    CIMValue(const char* x) { _init(); set(x); }

    ///	Constructor
    CIMValue(const CIMDateTime& x) { _init(); set(x); }

    ///	Constructor
    CIMValue(const CIMReference& x) { _init(); set(x); }

    ///	Constructor
    CIMValue(const Array<Boolean>& x) { _init(); set(x); }

    ///	Constructor
    CIMValue(const Array<Uint8>& x) { _init(); set(x); }

    ///	Constructor
    CIMValue(const Array<Sint8>& x) { _init(); set(x); }

    ///	Constructor
    CIMValue(const Array<Uint16>& x) { _init(); set(x); }

    ///	Constructor
    CIMValue(const Array<Sint16>& x) { _init(); set(x); }

    ///	Constructor
    CIMValue(const Array<Uint32>& x) { _init(); set(x); }

    ///	Constructor
    CIMValue(const Array<Sint32>& x) { _init(); set(x); }

    ///	Constructor
    CIMValue(const Array<Uint64>& x) { _init(); set(x); }

    ///	Constructor
    CIMValue(const Array<Sint64>& x) { _init(); set(x); }

    ///	Constructor
    CIMValue(const Array<Real32>& x) { _init(); set(x); }

    ///	Constructor
    CIMValue(const Array<Real64>& x) { _init(); set(x); }

    ///	Constructor
    CIMValue(const Array<Char16>& x) { _init(); set(x); }

    ///	Constructor
    CIMValue(const Array<String>& x) { _init(); set(x); }

    ///	Constructor
    CIMValue(const Array<CIMDateTime>& x) { _init(); set(x); }

    ///	Constructor
    CIMValue(const CIMValue& x);

    ///	Destrustructor
    ~CIMValue();

    /// Operator =
    CIMValue& operator=(const CIMValue& x) 
    { 
	assign(x); return *this; 
    }

    /// CIMMethod assign
    void assign(const CIMValue& x);

    /// CIMMethod clear
    void clear();

    /** CIMMethod typeCompatible - Compares the types of two values.
	@return true if compatible.
    */
    Boolean typeCompatible(const CIMValue& x) const
    {
	return _type == x._type && _isArray == x._isArray;
    }

    /** CIMMethod isArray - Determines if the value is an array
	@return TRUE if the value is an array
    */
    Boolean isArray() const { return _isArray; }

    /** CIMMethod getArraySize
	@return The number of entries in the array
    */
    Uint32 getArraySize() const;

    CIMType getType() const 
    { 
	return CIMType(_type); 
    }

    /// method setNullvalue - ATTN:
    void setNullValue(CIMType type, Boolean isArray, Uint32 arraySize = 0);

    /// method set - ATTN:
    void set(Boolean x);

    /// CIMMethod Set
    void set(Uint8 x);

    void set(Sint8 x);

    void set(Uint16 x);

    void set(Sint16 x);

    void set(Uint32 x);

    void set(Sint32 x);

    void set(Uint64 x);

    void set(Sint64 x);

    void set(Real32 x);

    void set(Real64 x);

    void set(const Char16& x);

    void set(const String& x);

    void set(const char* x);

    void set(const CIMDateTime& x);

    void set(const CIMReference& x);

    void set(const Array<Boolean>& x);

    void set(const Array<Uint8>& x);

    void set(const Array<Sint8>& x);

    void set(const Array<Uint16>& x);

    void set(const Array<Sint16>& x);

    void set(const Array<Uint32>& x);

    void set(const Array<Sint32>& x);

    void set(const Array<Uint64>& x);

    void set(const Array<Sint64>& x);

    void set(const Array<Real32>& x);

    void set(const Array<Real64>& x);

    void set(const Array<Char16>& x);

    void set(const Array<String>& x);

    void set(const Array<CIMDateTime>& x);

    /// CIMMethod get - ATTN
    void get(Boolean& x) const;

    void get(Uint8& x) const;

    void get(Sint8& x) const;

    void get(Uint16& x) const;

    void get(Sint16& x) const;

    void get(Uint32& x) const;

    void get(Sint32& x) const;

    void get(Uint64& x) const;

    void get(Sint64& x) const;

    void get(Real32& x) const;

    void get(Real64& x) const;

    void get(Char16& x) const;

    void get(String& x) const;

    void get(CIMDateTime& x) const;

    void get(CIMReference& x) const;

    void get(Array<Boolean>& x) const;

    void get(Array<Uint8>& x) const;

    void get(Array<Sint8>& x) const;

    void get(Array<Uint16>& x) const;

    void get(Array<Sint16>& x) const;

    void get(Array<Uint32>& x) const;

    void get(Array<Sint32>& x) const;

    void get(Array<Uint64>& x) const;

    void get(Array<Sint64>& x) const;

    void get(Array<Real32>& x) const;

    void get(Array<Real64>& x) const;

    void get(Array<Char16>& x) const;

    void get(Array<String>& x) const;

    void get(Array<CIMDateTime>& x) const; 

    /// CIMMethod toXML  - ATTN
    void toXml(Array<Sint8>& out) const;

    /// CIMMethod print - ATTN
    void print() const;

    /// CIMMethod toString	- ATTN
    String toString() const;

private:

    void _init();

    CIMType _type;
    Boolean _isArray;
    Union _u;

    friend class CIMMethodRep;
    friend class CIMParameterRep;
    friend class CIMPropertyRep;
    friend class CIMQualifierRep;
    friend class CIMQualifierDeclRep;
    PEGASUS_COMMON_LINKAGE friend Boolean operator==(
	const CIMValue& x, 
	const CIMValue& y);
};

PEGASUS_COMMON_LINKAGE Boolean operator==(const CIMValue& x, const CIMValue& y);

inline Boolean operator!=(const CIMValue& x, const CIMValue& y)
{
    return !operator==(x, y);
}

PEGASUS_NAMESPACE_END

#endif /* Pegasus_Value_h */
