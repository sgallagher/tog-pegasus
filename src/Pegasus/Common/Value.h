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
// Revision 1.1  2001/01/14 19:53:21  mike
// Initial revision
//
//
//END_HISTORY

////////////////////////////////////////////////////////////////////////////////
//
// Value.h
//
//	The Value class represents a value of any of the CIM data types
//	(see Type.h for a list of valid CIM data types). This class
//	encapsulates a union which holds the current value. The class also
//	has a type field indicating the type of that value.
//
////////////////////////////////////////////////////////////////////////////////

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

class PEGASUS_COMMON_LINKAGE Value
{
public:

    Value();

    Value(Boolean x) { _init(); set(x); }

    Value(Uint8 x) { _init(); set(x); }

    Value(Sint8 x) { _init(); set(x); }

    Value(Uint16 x) { _init(); set(x); }

    Value(Sint16 x) { _init(); set(x); }

    Value(Uint32 x) { _init(); set(x); }

    Value(Sint32 x) { _init(); set(x); }

    Value(Uint64 x) { _init(); set(x); }

    Value(Sint64 x) { _init(); set(x); }

    Value(Real32 x) { _init(); set(x); }

    Value(Real64 x) { _init(); set(x); }

    Value(const Char16& x) { _init(); set(x); }

    Value(const String& x) { _init(); set(x); }

    Value(const char* x) { _init(); set(x); }

    Value(const DateTime& x) { _init(); set(x); }

    Value(const Reference& x) { _init(); set(x); }

    Value(const Array<Boolean>& x) { _init(); set(x); }

    Value(const Array<Uint8>& x) { _init(); set(x); }

    Value(const Array<Sint8>& x) { _init(); set(x); }

    Value(const Array<Uint16>& x) { _init(); set(x); }

    Value(const Array<Sint16>& x) { _init(); set(x); }

    Value(const Array<Uint32>& x) { _init(); set(x); }

    Value(const Array<Sint32>& x) { _init(); set(x); }

    Value(const Array<Uint64>& x) { _init(); set(x); }

    Value(const Array<Sint64>& x) { _init(); set(x); }

    Value(const Array<Real32>& x) { _init(); set(x); }

    Value(const Array<Real64>& x) { _init(); set(x); }

    Value(const Array<Char16>& x) { _init(); set(x); }

    Value(const Array<String>& x) { _init(); set(x); }

    Value(const Array<DateTime>& x) { _init(); set(x); }

    Value(const Value& x);

    ~Value();

    Value& operator=(const Value& x) 
    { 
	assign(x); return *this; 
    }

    void assign(const Value& x);

    void clear();

    Boolean typeCompatible(const Value& x) const
    {
	return _type == x._type && _isArray == x._isArray;
    }

    Boolean isArray() const { return _isArray; }

    Uint32 getArraySize() const;

    Type getType() const 
    { 
	return Type(_type); 
    }

    void setNullValue(Type type, Boolean isArray, Uint32 arraySize = 0);

    void set(Boolean x);

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

    void toXml(Array<Sint8>& out) const;

    void print() const;

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
