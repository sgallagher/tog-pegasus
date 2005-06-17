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

#ifndef _Pegasus_Common_Packer_h
#define _Pegasus_Common_Packer_h

#include <cassert>
#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/ArrayInternal.h>
#include <Pegasus/Common/String.h>

#define PACKER_ASSERT assert

PEGASUS_NAMESPACE_BEGIN

/** This class packs the basic CIM data types into an array of characters.
    The elements are packed in a binary big-endian format. This class also
    provides unpacking functions as well.

    <p>
    This class is for internal use only.
*/
class PEGASUS_COMMON_LINKAGE Packer
{
public:

    static void packBoolean(Array<char>& out, Boolean x);
    static void packUint8(Array<char>& out, Uint8 x);
    static void packUint16(Array<char>& out, Uint16 x);
    static void packUint32(Array<char>& out, Uint32 x);
    static void packUint64(Array<char>& out, Uint64 x);
    static void packReal32(Array<char>& out, Real32 x);
    static void packReal64(Array<char>& out, Real64 x);
    static void packChar16(Array<char>& out, Char16 x);
    static void packString(Array<char>& out, const String& x);
    static void packSize(Array<char>& out, Uint32 x);

    static void packBoolean(Array<char>& out, const Boolean* x, Uint32 n);
    static void packUint8(Array<char>& out, const Uint8* x, Uint32 n);
    static void packUint16(Array<char>& out, const Uint16* x, Uint32 n);
    static void packUint32(Array<char>& out, const Uint32* x, Uint32 n);
    static void packUint64(Array<char>& out, const Uint64* x, Uint32 n);
    static void packReal32(Array<char>& out, const Real32* x, Uint32 n);
    static void packReal64(Array<char>& out, const Real64* x, Uint32 n);
    static void packChar16(Array<char>& out, const Char16* x, Uint32 n);
    static void packString(Array<char>& out, const String* x, Uint32 n);

    static void unpackBoolean(const Array<char>& in, Uint32& pos, Boolean& x);
    static void unpackUint8(const Array<char>& in, Uint32& pos, Uint8& x);
    static void unpackUint16(const Array<char>& in, Uint32& pos, Uint16& x);
    static void unpackUint32(const Array<char>& in, Uint32& pos, Uint32& x);
    static void unpackUint64(const Array<char>& in, Uint32& pos, Uint64& x);
    static void unpackReal32(const Array<char>& in, Uint32& pos, Real32& x);
    static void unpackReal64(const Array<char>& in, Uint32& pos, Real64& x);
    static void unpackChar16(const Array<char>& in, Uint32& pos, Char16& x);
    static void unpackString(const Array<char>& in, Uint32& pos, String& x);
    static void unpackSize(const Array<char>& out, Uint32& pos, Uint32& x);

    static void unpackBoolean(
	Array<char>& in, Uint32& pos, Boolean* x, Uint32 n);
    static void unpackUint8(
	Array<char>& in, Uint32& pos, Uint8* x, Uint32 n);
    static void unpackUint16(
	Array<char>& in, Uint32& pos, Uint16* x, Uint32 n);
    static void unpackUint32(
	Array<char>& in, Uint32& pos, Uint32* x, Uint32 n);
    static void unpackUint64(
	Array<char>& in, Uint32& pos, Uint64* x, Uint32 n);
    static void unpackReal32(
	Array<char>& in, Uint32& pos, Real32* x, Uint32 n);
    static void unpackReal64(
	Array<char>& in, Uint32& pos, Real64* x, Uint32 n);
    static void unpackChar16(
	Array<char>& in, Uint32& pos, Char16* x, Uint32 n);
    static void unpackString(
	Array<char>& in, Uint32& pos, String* x, Uint32 n);

    static bool isLittleEndian();

    static Uint16 swapUint16(Uint16 x);
    static Uint32 swapUint32(Uint32 x);
    static Uint64 swapUint64(Uint64 x);

    static Uint16 swap(Uint16 x) { return swapUint16(x); }
    static Uint32 swap(Uint32 x) { return swapUint32(x); }
    static Uint64 swap(Uint64 x) { return swapUint64(x); }

    static void swap(Uint8& x, Uint8& y);
};

inline bool Packer::isLittleEndian()
{
#if defined(PEGASUS_PLATFORM_LINUX_IA64_GNU)
    return true;
#elif defined(PEGASUS_PLATFORM_LINUX_X86_GNU)
    return true;
#else
    // You can add your platform above to avoid this calculation.
    union U { Uint16 x; Uint8 a[2]; } u;
    u.x = 1;
    return u.a[0] == 1;
#endif
}

inline void Packer::packBoolean(Array<char>& out, Boolean x_)
{
    Uint8 x = Uint8(x_);
    out.append((char*)&x, sizeof(x));
}

inline void Packer::packUint8(Array<char>& out, Uint8 x)
{
    out.append((char*)&x, sizeof(x));
}

inline void Packer::packUint16(Array<char>& out, Uint16 x)
{
    if (isLittleEndian())
	x = Packer::swapUint16(x);

    out.append((char*)&x, sizeof(x));
}

inline void Packer::packUint32(Array<char>& out, Uint32 x)
{
    if (isLittleEndian())
	x = Packer::swapUint32(x);

    out.append((char*)&x, sizeof(x));
}

inline void Packer::packUint64(Array<char>& out, Uint64 x)
{
    if (isLittleEndian())
	x = Packer::swapUint64(x);

    out.append((char*)&x, sizeof(x));
}

inline void Packer::packReal32(Array<char>& out, Real32 x)
{
    packUint32(out, *((Uint32*)&x));
}

inline void Packer::packReal64(Array<char>& out, Real64 x)
{
    packUint64(out, *((Uint64*)&x));
}

inline void Packer::packChar16(Array<char>& out, Char16 x)
{
    packUint16(out, Uint16(x));
}

inline void Packer::packReal32(Array<char>& out, const Real32* x, Uint32 n)
{
    Packer::packUint32(out, (const Uint32*)x, n);
}

inline void Packer::packReal64(Array<char>& out, const Real64* x, Uint32 n)
{
    Packer::packUint64(out, (const Uint64*)x, n);
}

inline void Packer::packChar16(Array<char>& out, const Char16* x, Uint32 n)
{
    Packer::packUint16(out, (const Uint16*)x, n);
}

inline void Packer::unpackBoolean(
    const Array<char>& in, Uint32& pos, Boolean& x)
{
    x = in[pos++] ? true : false;
    PACKER_ASSERT(pos <= in.size());
}

inline void Packer::unpackUint8(
    const Array<char>& in, Uint32& pos, Uint8& x)
{
    x = Uint8(in[pos++]);
    PACKER_ASSERT(pos <= in.size());
}

inline void Packer::unpackReal32(const Array<char>& in, Uint32& pos, Real32& x)
{
    Packer::unpackUint32(in, pos, *((Uint32*)&x));
    PACKER_ASSERT(pos <= in.size());
}

inline void Packer::unpackReal64(const Array<char>& in, Uint32& pos, Real64& x)
{
    Packer::unpackUint64(in, pos, *((Uint64*)&x));
    PACKER_ASSERT(pos <= in.size());
}

inline void Packer::unpackChar16(const Array<char>& in, Uint32& pos, Char16& x)
{
    Packer::unpackUint16(in, pos, *((Uint16*)&x));
    PACKER_ASSERT(pos <= in.size());
}

inline void Packer::unpackReal32(
    Array<char>& in, Uint32& pos, Real32* x, Uint32 n)
{
    unpackUint32(in, pos, (Uint32*)x, n);
    PACKER_ASSERT(pos <= in.size());
}

inline void Packer::unpackReal64(
    Array<char>& in, Uint32& pos, Real64* x, Uint32 n)
{
    unpackUint64(in, pos, (Uint64*)x, n);
    PACKER_ASSERT(pos <= in.size());
}

inline void Packer::unpackChar16(
    Array<char>& in, Uint32& pos, Char16* x, Uint32 n)
{
    unpackUint16(in, pos, (Uint16*)x, n);
    PACKER_ASSERT(pos <= in.size());
}

inline Uint16 Packer::swapUint16(Uint16 x)
{
    return (Uint16)(
	(((Uint16)(x) & 0x00ffU) << 8) | 
	(((Uint16)(x) & 0xff00U) >> 8));
}

inline Uint32 Packer::swapUint32(Uint32 x)
{
    return (Uint32)(
	(((Uint32)(x) & 0x000000ffUL) << 24) |
	(((Uint32)(x) & 0x0000ff00UL) <<  8) |
	(((Uint32)(x) & 0x00ff0000UL) >>  8) |
	(((Uint32)(x) & 0xff000000UL) >> 24));
}

inline void Packer::swap(Uint8& x, Uint8& y)
{
    Uint8 t = x;
    x = y;
    y = t;
}

inline Uint64 Packer::swapUint64(Uint64 x)
{
    union
    {
	Uint64 x;
	Uint8 bytes[8];
    }
    u;

    u.x = x;
    Packer::swap(u.bytes[0], u.bytes[7]);
    Packer::swap(u.bytes[1], u.bytes[6]);
    Packer::swap(u.bytes[2], u.bytes[5]);
    Packer::swap(u.bytes[3], u.bytes[4]);
    return u.x;
}

PEGASUS_NAMESPACE_END

#endif /* _Pegasus_Common_Packer_h */
