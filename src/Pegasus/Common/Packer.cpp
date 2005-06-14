//%2005////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001, 2002 BMC Software; Hewlett-Packard Development
// Company, L.P.; IBM Corp.; The Open Group; Tivoli Systems.
// Copyright (c) 2003 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation, The Open Group.
// Copyright (c) 2004 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation; VERITAS Software Corporation; The Open Group.
// Copyright (c) 2005 Hewlett-Packard Development Company, L.P.; IBM Corp.;
// EMC Corporation; VERITAS Software Corporation; The Open Group.
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
// Author: Michael E. Brasher
//
//%/////////////////////////////////////////////////////////////////////////////

#include <string.h>
#include "Packer.h"

PEGASUS_NAMESPACE_BEGIN

static bool _constains16BitChars(const String& x)
{
    const Char16* p = x.getChar16Data();
    Uint32 n = x.size();

    while (n--)
    {
	if (Uint16(*p++) > 0xFF)
	    return true;
    }

    return false;
}

void Packer::packString(Array<char>& out, const String& x)
{
    Uint32 n = x.size();

    if (_constains16BitChars(x))
    {
	packUint8(out, 16);
	packSize(out, n);
	packChar16(out, x.getChar16Data(), n);
    }
    else
    {
	packUint8(out, 8);
	packSize(out, n);
	const Char16* data = x.getChar16Data();

	for (Uint32 i = 0; i < n; i++)
	    Packer::packUint8(out, (Uint8)data[i]);
    }
}

void Packer::packSize(Array<char>& out, Uint32 x)
{
    // Top two bits indicate the number of bytes used to
    // pack this size:
    //
    //     00 : 1 byte
    //     01 : 2 bytes
    //     10 : 4 bytes
    //     11 : illegal

    if (x > 16383)
    {
	// Use four bytes for size (tag == '1')
	packUint32(out, 0x80000000 | x);
    }
    else if (x > 63)
    {
	// Use two bytes for size.
	packUint16(out, 0x4000 | Uint16(x));
    }
    else /* x > 1073741823 */
    {
	// Use one byte for size.
	packUint8(out, 0x00 | Uint8(x));
    }
}

void Packer::unpackSize(const Array<char>& in, Uint32& pos, Uint32& x)
{
    // Top two bits form a tag that indicates the number of bytes used to
    // pack this size:
    //
    //     00 : 1 byte
    //     01 : 2 bytes
    //     10 : 4 bytes
    //     11 : illegal

    // Unpack first byte.

    Uint8 byte;
    Packer::unpackUint8(in, pos, byte);
    Uint8 tag = byte & 0xC0;

    if (tag == 0x80)
    {
	// Four-byte size:
	Uint8 b0 = tag ^ byte;
	Uint8 b1;
	Uint8 b2;
	Uint8 b3;

	Packer::unpackUint8(in, pos, b1);
	Packer::unpackUint8(in, pos, b2);
	Packer::unpackUint8(in, pos, b3);
	Uint32 tmp = (Uint32(b0) << 24) | 
	    (Uint32(b1) << 16) | 
	    (Uint32(b2) <<  8) | 
	    (Uint32(b3));

	x = tmp;
    }
    else if (tag == 0x40)
    {
	// Two-byte size:
	x = (tag ^ byte) << 8;
	Packer::unpackUint8(in, pos, byte);
	x |= byte;
    }
    else if (tag == 0x00)
    {
	// One-byte size:
	x = byte;
    }
    else
    {
	PACKER_ASSERT(0);
    }

    PACKER_ASSERT(pos <= in.size());
}

template<class T>
void _pack_array(Array<char>& out, const T* x, Uint32 n)
{
    Uint32 bytes = n * sizeof(T);
    out.reserveCapacity(out.size() + bytes);

    if (Packer::isLittleEndian())
    {
	for (size_t i = 0; i < n; i++)
	{
	    T tmp = Packer::swap(x[i]);
	    out.append((char*)&tmp, sizeof(tmp));
	}
    }
    else
	out.append((char*)x, bytes);
}

void Packer::packBoolean(Array<char>& out, const Boolean* x, Uint32 n)
{
    out.reserveCapacity(out.size() + n);

    for (size_t i = 0; i < n; i++)
    {
	Uint8 tmp = Uint8(x[i]); 
	out.append((char*)&tmp, sizeof(tmp));
    }
}

void Packer::packUint8(Array<char>& out, const Uint8* x, Uint32 n)
{
    out.append((char*)x, n);
}

void Packer::packUint16(Array<char>& out, const Uint16* x, Uint32 n)
{
    _pack_array(out, x, n);
}

void Packer::packUint32(Array<char>& out, const Uint32* x, Uint32 n)
{
    _pack_array(out, x, n);
}

void Packer::packUint64(Array<char>& out, const Uint64* x, Uint32 n)
{
    _pack_array(out, x, n);
}

void Packer::packString(Array<char>& out, const String* x, Uint32 n)
{
    for (size_t i = 0; i < n; i++)
	packString(out, x[i]);
}

void Packer::unpackUint16(
    const Array<char>& in, Uint32& pos, Uint16& x)
{
    memcpy(&x, &in[pos], sizeof(x));
    pos += sizeof(x);

    if (isLittleEndian())
	x = Packer::swapUint16(x);
}

void Packer::unpackUint32(
    const Array<char>& in, Uint32& pos, Uint32& x)
{
    memcpy(&x, &in[pos], sizeof(x));
    pos += sizeof(x);

    if (isLittleEndian())
	x = Packer::swapUint32(x);

    PACKER_ASSERT(pos <= in.size());
}

void Packer::unpackUint64(
    const Array<char>& in, Uint32& pos, Uint64& x)
{
    memcpy(&x, &in[pos], sizeof(x));
    pos += sizeof(x);

    if (isLittleEndian())
	x = Packer::swapUint64(x);

    PACKER_ASSERT(pos <= in.size());
}

void Packer::unpackString(const Array<char>& in, Uint32& pos, String& x)
{
    // Determine whether packed as 8-bit or 16-bit.

    Uint8 bits;
    unpackUint8(in, pos, bits);

    PACKER_ASSERT(bits == 16 || bits == 8);

    // Unpack array size.

    Uint32 n;
    unpackSize(in, pos, n);

    if (bits == 16)
    {
	x.clear();
	x.reserveCapacity(n);

	for (size_t i = 0; i < n; i++)
	{
	    Char16 tmp;
	    unpackChar16(in , pos, tmp);
	    x.append(tmp);
	}
    }
    else
    {
	x.clear();
	x.reserveCapacity(n);

	for (size_t i = 0; i < n; i++)
	{
	    Uint8 tmp;
	    unpackUint8(in , pos, tmp);
	    x.append(tmp);
	}
    }

    PACKER_ASSERT(pos <= in.size());
}

void Packer::unpackBoolean(
    Array<char>& in, Uint32& pos, Boolean* x, Uint32 n)
{
    for (size_t i = 0; i < n; i++)
	unpackBoolean(in, pos, x[i]);

    PACKER_ASSERT(pos <= in.size());
}

void Packer::unpackUint8(
    Array<char>& in, Uint32& pos, Uint8* x, Uint32 n)
{
    for (size_t i = 0; i < n; i++)
	unpackUint8(in, pos, x[i]);

    PACKER_ASSERT(pos <= in.size());
}

void Packer::unpackUint16(
    Array<char>& in, Uint32& pos, Uint16* x, Uint32 n)
{
    for (size_t i = 0; i < n; i++)
	unpackUint16(in, pos, x[i]);

    PACKER_ASSERT(pos <= in.size());
}

void Packer::unpackUint32(
    Array<char>& in, Uint32& pos, Uint32* x, Uint32 n)
{
    for (size_t i = 0; i < n; i++)
	unpackUint32(in, pos, x[i]);

    PACKER_ASSERT(pos <= in.size());
}

void Packer::unpackUint64(
    Array<char>& in, Uint32& pos, Uint64* x, Uint32 n)
{
    for (size_t i = 0; i < n; i++)
	unpackUint64(in, pos, x[i]);

    PACKER_ASSERT(pos <= in.size());
}

void Packer::unpackString(
    Array<char>& in, Uint32& pos, String* x, Uint32 n)
{
    for (size_t i = 0; i < n; i++)
	unpackString(in, pos, x[i]);

    PACKER_ASSERT(pos <= in.size());
}

PEGASUS_NAMESPACE_END
