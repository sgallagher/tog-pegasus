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
// Modified By:
//
//%/////////////////////////////////////////////////////////////////////////////

/*
 CIMType.h - This file defines the CIMType enumeration which introduces
symbolic constants for the CIM data types.
*/

#ifndef Pegasus_Type_h
#define Pegasus_Type_h

#include <Pegasus/Common/Config.h>

PEGASUS_NAMESPACE_BEGIN

/**
    The CIMType Class defines the CIMType enumeration which introduces
    symbolic constants for the CIM data types.

    The table below shows each CIM type, its symbolic constant, and its
    representation type.

    <pre>
	    CIM CIMType	Constant	        C++ CIMType
	    -------------------------------------------------------
	    boolean	CIMType::BOOLEAN	Boolean
	    uint8	CIMType::UINT8		Uint8
	    sint8	CIMType::SINT8		Sint8
	    uint16	CIMType::UINT16		Uint16
	    sint16	CIMType::SINT16		Sint16
	    uint32	CIMType::UINT32		Uint32
	    sint32	CIMType::SINT32		Sint32
	    uint64	CIMType::UINT64		Sint64
	    sint64	CIMType::SINT64		Sint64
	    real32	CIMType::REAL32		Real32
	    real64	CIMType::REAL64		Real64
	    char16	CIMType::CHAR16		Char16
	    string	CIMType::STRING		String
	    datetime	CIMType::DATETIME	CIMDateTime
	    reference	CIMType::REFERENCE	CIMReference

    </pre>
*/
class PEGASUS_COMMON_LINKAGE CIMType
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

    CIMType() : _tag(NONE) { }

    CIMType(Tag tag) : _tag(tag) { }

    PEGASUS_EXPLICIT CIMType(Uint32 tag) : _tag(Tag(tag)) { }

    CIMType(const CIMType& x) : _tag(x._tag) { }

    CIMType& operator=(Tag tag) { _tag = tag; return *this; }

    operator Tag() const { return _tag; }

    Boolean equal(const CIMType& x) const { return x._tag == _tag; }

private:

    Tag _tag;
};

inline Boolean operator==(CIMType x, CIMType y)
{
    return CIMType::Tag(x) == CIMType::Tag(y);
}

inline Boolean operator!=(CIMType x, CIMType y)
{
    return !operator==(x, y);
}

// ATTN: this doesn't seem necessary!

inline Boolean operator==(CIMType x, CIMType::Tag y)
{
    return x.equal(y);
}

inline Boolean operator==(CIMType::Tag x, CIMType y)
{
    return y.equal(x);
}

inline Boolean operator!=(CIMType x, CIMType::Tag y)
{
    return !operator==(x, y);
}

inline Boolean operator!=(CIMType::Tag x, CIMType y)
{
    return !operator==(x, y);
}

inline Boolean operator==(CIMType::Tag x, CIMType::Tag y)
{
	return x == y;
}

// Returns a string representation of the given type. The string
// is the same as the "CIM CIMType" column in the table above.

PEGASUS_COMMON_LINKAGE const char* TypeToString(CIMType type);

PEGASUS_NAMESPACE_END

#endif /* Pegasus_Type_h */

