//BEGIN_LICENSE
//
// Copyright (c) 2000 The Open Group, BMC Software
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
// Author: Mike Brasher
//
// $Log: Representation.h,v $
// Revision 1.1  2001/01/14 19:53:42  mike
// Initial revision
//
//
//END_HISTORY

////////////////////////////////////////////////////////////////////////////////
//
// Representation.h
//
//	This class is used to transform data representations between native
//	and universal format. For integers, universal format is big endian.
//	The universal format for floats and double is big endian IEEE.
//
////////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_Representation_h
#define Pegasus_Representation_h

#include <Pegasus/Common/Config.h>

PEGASUS_NAMESPACE_BEGIN

class Representation
{
public:

    static void toUniversal(Uint8 &x) { }

    static void toUniversal(Sint8 &x) { }

    static void toUniversal(Uint16 &x) { transform(x); }

    static void toUniversal(Sint16 &x) { transform((Uint16&)x); }

    static void toUniversal(Uint32 &x) { transform(x); }

    static void toUniversal(Sint32 &x) { transform((Uint32&)x); }

    static void toUniversal(Uint64 &x) { transform(x); }

    static void toUniversal(Sint64 &x) { transform((Uint64&)x); }

    static void toUniversal(Real32 &x) { transform((Uint32&)x); }

    static void toUniversal(Real64 &x) { transform((Uint64&)x); }

    static void toNative(Uint8 &x) { }

    static void toNative(Sint8 &x) { }

    static void toNative(Uint16 &x) { transform(x); }

    static void toNative(Sint16 &x) { transform((Uint16&)x); }

    static void toNative(Uint32 &x) { transform(x); }

    static void toNative(Sint32 &x) { transform((Uint32&)x); }

    static void toNative(Uint64 &x) { transform(x); }

    static void toNative(Sint64 &x) { transform((Uint64&)x); }

    static void toNative(Real32 &x) { transform((Uint32&)x); }

    static void toNative(Real64 &x) { transform((Uint64&)x); }

private:

    static void _swap(Uint8& x, Uint8& y)
    {
	Uint8 t = x;
	x = y;
	y = t;
    }

    static void transform(Uint16& x) 
    {
#if PEGASUS_LITTLE_ENDIAN
	union
	{
	    Uint16 x;
	    Uint8 bytes[2];
	} u;
	u.x = x;
	_swap(u.bytes[0], u.bytes[1]);
	x = u.x;
#endif
    }

    static void transform(Uint32& x) 
    {
#if PEGASUS_LITTLE_ENDIAN
	union
	{
	    Uint32 x;
	    Uint8 bytes[4];
	} u;
	u.x = x;
	_swap(u.bytes[0], u.bytes[3]);
	_swap(u.bytes[1], u.bytes[2]);
	x = u.x;
#endif
    }

    static void transform(Uint64& x) 
    {
#if PEGASUS_LITTLE_ENDIAN
	union
	{
	    Uint64 x;
	    Uint8 bytes[8];
	} u;
	u.x = x;
	_swap(u.bytes[0], u.bytes[7]);
	_swap(u.bytes[1], u.bytes[6]);
	_swap(u.bytes[2], u.bytes[5]);
	_swap(u.bytes[3], u.bytes[4]);
	x = u.x;
#endif
    }
};

PEGASUS_NAMESPACE_END

#endif /* Pegasus_Representation_h */
