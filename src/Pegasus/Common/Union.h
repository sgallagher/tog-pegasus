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
// $Log: Union.h,v $
// Revision 1.2  2001/01/30 08:00:43  karl
// DOC++ Documentation update for header files
//
// Revision 1.1.1.1  2001/01/14 19:53:16  mike
// Pegasus import
//
//
//END_HISTORY

////////////////////////////////////////////////////////////////////////////////
//
// Union
//
//	This union is used to represent the values of properties, qualifiers,
//	method return values, and method arguments. All of the types
//	defined in Type.h are represented by a Union. The 
//	Union is used as the the basis for the Value implementation.
//
////////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_Union_h
#define Pegasus_Union_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/Type.h>
#include <Pegasus/Common/String.h>
#include <Pegasus/Common/DateTime.h>
#include <Pegasus/Common/Reference.h>
#include <Pegasus/Common/Array.h>

PEGASUS_NAMESPACE_BEGIN
/** union Union
    This union is used to represent the values of properties, qualifiers,
    method return values, and method arguments. All of the types
    defined in Type.h are represented by a Union. The 
    Union is used as the the basis for the Value implementation.
*/
union Union
{
    Uint8 _booleanValue;
    Uint8 _uint8Value;
    Sint8 _sint8Value;
    Uint16 _uint16Value;
    Sint16 _sint16Value;
    Uint32 _uint32Value;
    Sint32 _sint32Value;
    Uint64 _uint64Value;
    Sint64 _sint64Value;
    Real32 _real32Value;
    Real64 _real64Value;
    Uint16 _char16Value;
    String* _stringValue;
    DateTime* _dateTimeValue;
    Reference* _referenceValue;

    ArrayRep<Boolean>* _booleanArray;
    ArrayRep<Uint8>* _uint8Array;
    ArrayRep<Sint8>* _sint8Array;
    ArrayRep<Uint16>* _uint16Array;
    ArrayRep<Sint16>* _sint16Array;
    ArrayRep<Uint32>* _uint32Array;
    ArrayRep<Sint32>* _sint32Array;
    ArrayRep<Uint64>* _uint64Array;
    ArrayRep<Sint64>* _sint64Array;
    ArrayRep<Real32>* _real32Array;
    ArrayRep<Real64>* _real64Array;
    ArrayRep<Char16>* _char16Array;
    ArrayRep<String>* _stringArray;
    ArrayRep<DateTime>* _dateTimeArray;

    void* _voidPtr;
};

PEGASUS_NAMESPACE_END

#endif /* Pegasus_Union_h */
