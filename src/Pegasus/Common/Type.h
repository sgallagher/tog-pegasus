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
// $Log: Type.h,v $
// Revision 1.1  2001/01/14 19:53:15  mike
// Initial revision
//
//
//END_HISTORY

////////////////////////////////////////////////////////////////////////////////
//
// Type.h
//
//	This file defines the Type enumeration which introduces 
//	symbolic constants for the CIM data types. The table below shows 
//	each CIM type, its symbolic constant, and its representation type.
//
//	    CIM Type	Constant	C++ Type
//	    -------------------------------------------------------
//	    boolean	Type::BOOLEAN		Boolean
//	    uint8	Type::UINT8		Uint8
//	    sint8	Type::SINT8		Sint8
//	    uint16	Type::UINT16		Uint16
//	    sint16	Type::SINT16		Sint16
//	    uint32	Type::UINT32		Uint32
//	    sint32	Type::SINT32		Sint32
//	    uint64	Type::UINT64		Sint64
//	    sint64	Type::SINT64		Sint64
//	    real32	Type::REAL32		Real32
//	    real64	Type::REAL64		Real64
//	    char16	Type::CHAR16		Char16
//	    string	Type::STRING		String
//	    datetime	Type::DATETIME		DateTime
//	    reference	Type::REFERENCE		Reference
//
////////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_Type_h
#define Pegasus_Type_h

#include <Pegasus/Common/Config.h>

PEGASUS_NAMESPACE_BEGIN

class PEGASUS_COMMON_LINKAGE Type
{
public:

    enum Tag
    {
	NONE,
	BOOLEAN,
	UINT8,
	SINT8,
	UINT16,
	SINT16,
	UINT32,
	SINT32,
	UINT64,
	SINT64,
	REAL32,
	REAL64,
	CHAR16,
	STRING,
	DATETIME,
	REFERENCE
    };

    Type() : _tag(NONE) { }

    Type(Tag tag) : _tag(tag) { }

    explicit Type(Uint32 tag) : _tag(Tag(tag)) { }

    Type(const Type& x) : _tag(x._tag) { }

    Type& operator=(Tag tag) { _tag = tag; return *this; }

    operator Tag() const { return _tag; }

    Boolean equal(const Type& x) const { return x._tag == _tag; }

private:

    Tag _tag;
};

inline Boolean operator==(Type x, Type y)
{
    return Type::Tag(x) == Type::Tag(y);
}

inline Boolean operator!=(Type x, Type y)
{
    return !operator==(x, y);
}

// ATTN: this doesn't seem necessary!

inline Boolean operator==(Type x, Type::Tag y)
{
    return x.equal(y);
}

inline Boolean operator==(Type::Tag x, Type y)
{
    return y.equal(x);
}

inline Boolean operator!=(Type x, Type::Tag y)
{
    return !operator==(x, y);
}

inline Boolean operator!=(Type::Tag x, Type y)
{
    return !operator==(x, y);
}

// Returns a string representation of the given type. The string
// is the same as the "CIM Type" column in the table above.

PEGASUS_COMMON_LINKAGE const char* TypeToString(Type type);

PEGASUS_NAMESPACE_END

#endif /* Pegasus_Type_h */
