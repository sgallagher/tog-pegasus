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
// $Log: Value.h,v $
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

/*

 Value.h

The Value class represents a value of any of the CIM data types
(see Type.h for a list of valid CIM data types). This class
encapsulates a union which holds the current value. The class also
has a type field indicating the type of that value.

*/

#ifndef Pegasus_Value_h
#define Pegasus_Value_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/Type.h>
#include <Pegasus/Common/String.h>
#include <Pegasus/Common/DateTime.h>
#include <Pegasus/Common/Union.h>
#include <Pegasus/Common/Array.h>

PEGASUS_NAMESPACE_BEGIN

// ATTN: more MSVC 5.0 hacks.
class Value;
PEGASUS_COMMON_LINKAGE Boolean operator==(const Value& x, const Value& y);
/**
The Value class represents a value of any of the CIM data types
(see Type.h for a list of valid CIM data types). This class
encapsulates a union which holds the current value. The class also
has a type field indicating the type of that value.
*/
class PEGASUS_COMMON_LINKAGE Value
{
public:
    ///	Constructor
    Value();
    ///	Constructor
    Value(Boolean x) { _init(); set(x); }
    ///	Constructor
    Value(Uint8 x) { _init(); set(x); }
    ///	Constructor
    Value(Sint8 x) { _init(); set(x); }
    ///	Constructor
    Value(Uint16 x) { _init(); set(x); }
    ///	Constructor
    Value(Sint16 x) { _init(); set(x); }
    ///	Constructor
    Value(Uint32 x) { _init(); set(x); }
    ///	Constructor
    Value(Sint32 x) { _init(); set(x); }
    ///	Constructor
    Value(Uint64 x) { _init(); set(x); }
    ///	Constructor
    Value(Sint64 x) { _init(); set(x); }
    ///	Constructor
    Value(Real32 x) { _init(); set(x); }
    ///	Constructor
    Value(Real64 x) { _init(); set(x); }
    ///	Constructor
    Value(const Char16& x) { _init(); set(x); }
    ///	Constructor
    Value(const String& x) { _init(); set(x); }
    ///	Constructor
    Value(const char* x) { _init(); set(x); }
    ///	Constructor
    Value(const DateTime& x) { _init(); set(x); }
    ///	Constructor
    Value(const Reference& x) { _init(); set(x); }
    ///	Constructor
    Value(const Array<Boolean>& x) { _init(); set(x); }
    ///	Constructor
    Value(const Array<Uint8>& x) { _init(); set(x); }
    ///	Constructor
    Value(const Array<Sint8>& x) { _init(); set(x); }
    ///	Constructor
    Value(const Array<Uint16>& x) { _init(); set(x); }
    ///	Constructor
    Value(const Array<Sint16>& x) { _init(); set(x); }
    ///	Constructor
    Value(const Array<Uint32>& x) { _init(); set(x); }
    ///	Constructor
    Value(const Array<Sint32>& x) { _init(); set(x); }
    ///	Constructor
    Value(const Array<Uint64>& x) { _init(); set(x); }
    ///	Constructor
    Value(const Array<Sint64>& x) { _init(); set(x); }
    ///	Constructor
    Value(const Array<Real32>& x) { _init(); set(x); }
    ///	Constructor
    Value(const Array<Real64>& x) { _init(); set(x); }
    ///	Constructor
    Value(const Array<Char16>& x) { _init(); set(x); }
    ///	Constructor
    Value(const Array<String>& x) { _init(); set(x); }
    ///	Constructor
    Value(const Array<DateTime>& x) { _init(); set(x); }
    ///	Constructor
    Value(const Value& x);
    ///	Destrustructor
    ~Value();
    /// Operator =
    Value& operator=(const Value& x) 
    { 
	assign(x); return *this; 
    }
    /// Method assign
    void assign(const Value& x);
    /// Method clear
    void clear();
    /** Method typeCompatible - Compares the types of
    two values.
    @return TRUE if ATTN
    */
    Boolean typeCompatible(const Value& x) const
    {
	return _type == x._type && _isArray == x._isArray;
    }
    /** Method isArray - Determines if the value is an array
    @return TRUE if the value is an array
    */
    Boolean isArray() const { return _isArray; }
    /** Method getArraySize
    @return The number of entries in the array
    */
    Uint32 getArraySize() const;

    Type getType() const 
    { 
	return Type(_type); 
    }
    /// method setNullvalue - ATTN:
    void setNullValue(Type type, Boolean isArray, Uint32 arraySize = 0);
    /// method set - ATTN:
    void set(Boolean x);
    /// Method Set
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

    void set(const DateTime& x);

    void set(const Reference& x);

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

    void set(const Array<DateTime>& x);
    /// Method get - ATTN
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

    void get(DateTime& x) const;

    void get(Reference& x) const;

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

    void get(Array<DateTime>& x) const; 
    /// Method toXML  - ATTN
    void toXml(Array<Sint8>& out) const;
    /// Method print - ATTN
    void print() const;
    /// Method toString	- ATTN
    String toString() const;

private:

    void _init();

    Type _type;
    Boolean _isArray;
    Union _u;

    friend class MethodRep;
    friend class ParameterRep;
    friend class PropertyRep;
    friend class QualifierRep;
    friend class QualifierDeclRep;
    PEGASUS_COMMON_LINKAGE friend Boolean operator==(
	const Value& x, 
	const Value& y);
};

PEGASUS_COMMON_LINKAGE Boolean operator==(const Value& x, const Value& y);

inline Boolean operator!=(const Value& x, const Value& y)
{
    return !operator==(x, y);
}

PEGASUS_NAMESPACE_END

#endif /* Pegasus_Value_h */
