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
// $Log: OutBuffer.h,v $
// Revision 1.1  2001/01/14 19:53:40  mike
// Initial revision
//
//
//END_HISTORY

////////////////////////////////////////////////////////////////////////////////
//
// OutBuffer.h
//
////////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_OutBuffer_h
#define Pegasus_OutBuffer_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/Array.h>
#include <Pegasus/Common/Representation.h>
#include <Pegasus/Common/String.h>
#include <Pegasus/Common/DateTime.h>
#include <Pegasus/Common/Reference.h>

PEGASUS_NAMESPACE_BEGIN

class OutBuffer
{
private:

    // Compiler bug: this member template definition must precede the site 
    // of instantiation:

    template<class T>
    void _pack(T x)
    {
	align(sizeof(x));
	Representation::toUniversal(x);
	_data.append((const Uint8*)&x, sizeof(x));
    }

    template<class T>
    void _packArray(const Array<T>& x) 
    {
	Uint32 n = x.getSize();
	_pack(n);

	for (Uint32 i = 0; i < n; i++)
	    pack(x[i]);
    }

    static Uint32 _round(Uint32 x, Uint32 n)
    {
	return ((x + (n - 1)) / n) * n;
    }

public:

    OutBuffer();

    ~OutBuffer();

    const Array<Uint8>& getData() const 
    { 
	return _data;
    }

    Uint32 getSize() const
    {
	return _data.getSize();
    }

    void align(Uint32 n)
    {
	Uint32 size = _data.getSize();
	Uint32 diff = _round(size, n) - size;

	if (diff)
	    _data.grow(diff, 0xaa);
    }

    void pack(Boolean x) 
    {
	Uint8 flag = x ? 1 : 0;
	_data.append(flag);
    }

    void pack(Uint8 x) 
    { 
	_data.append((const Uint8*)&x, sizeof(x)); 
    }

    void pack(Sint8 x) 
    { 
	_data.append((const Uint8*)&x, sizeof(x)); 
    }

    void pack(Uint16 x) 
    { 
	_pack(x); 
    }

    void pack(Sint16 x)
    { 
	_pack(x); 
    }

    void pack(Uint32 x)
    { 
	_pack(x); 
    }

    void pack(Sint32 x)
    { 
	_pack(x); 
    }

    void pack(Uint64 x)
    { 
	_pack(x); 
    }

    void pack(Sint64 x)
    { 
	_pack(x); 
    }

    void pack(Real32 x)
    { 
	_pack(x); 
    }

    void pack(Real64 x)
    {
	_pack(x); 
    }

    void pack(const Char16& x);

    void pack(const String& x);

    void pack(const DateTime& x);

    void pack(const Reference& x);

    void pack(const Array<Boolean>& x);

    void pack(const Array<Uint8>& x);

    void pack(const Array<Sint8>& x);

    void pack(const Array<Uint16>& x);

    void pack(const Array<Sint16>& x);

    void pack(const Array<Uint32>& x);

    void pack(const Array<Sint32>& x);

    void pack(const Array<Uint64>& x);

    void pack(const Array<Sint64>& x);

    void pack(const Array<Real32>& x);

    void pack(const Array<Real64>& x);

    void pack(const Array<Char16>& x);

    void pack(const Array<String>& x);

    void pack(const Array<DateTime>& x);

private:

    Array<Uint8> _data;
};

inline OutBuffer& operator<<(OutBuffer& out, Boolean x)
{
    out.pack(x);
    return out;
}

inline OutBuffer& operator<<(OutBuffer& out, Uint8 x)
{
    out.pack(x);
    return out;
}

inline OutBuffer& operator<<(OutBuffer& out, Sint8 x)
{
    out.pack(x);
    return out;
}

inline OutBuffer& operator<<(OutBuffer& out, Uint16 x)
{
    out.pack(x);
    return out;
}

inline OutBuffer& operator<<(OutBuffer& out, Sint16 x)
{
    out.pack(x);
    return out;
}

inline OutBuffer& operator<<(OutBuffer& out, Uint32 x)
{
    out.pack(x);
    return out;
}

inline OutBuffer& operator<<(OutBuffer& out, Sint32 x)
{
    out.pack(x);
    return out;
}

inline OutBuffer& operator<<(OutBuffer& out, Uint64 x)
{
    out.pack(x);
    return out;
}

inline OutBuffer& operator<<(OutBuffer& out, Sint64 x)
{
    out.pack(x);
    return out;
}

inline OutBuffer& operator<<(OutBuffer& out, Real32 x)
{
    out.pack(x);
    return out;
}

inline OutBuffer& operator<<(OutBuffer& out, Real64 x)
{
    out.pack(x);
    return out;
}

inline OutBuffer& operator<<(OutBuffer& out, Char16 x)
{
    out.pack(x);
    return out;
}

inline OutBuffer& operator<<(OutBuffer& out, const String& x)
{
    out.pack(x);
    return out;
}

inline OutBuffer& operator<<(OutBuffer& out, const DateTime& x)
{
    out.pack(x);
    return out;
}

inline OutBuffer& operator<<(OutBuffer& out, const Reference& x)
{
    out.pack(x);
    return out;
}

inline OutBuffer& operator<<(OutBuffer& out, const Array<Boolean>& x)
{
    out.pack(x);
    return out;
}

inline OutBuffer& operator<<(OutBuffer& out, const Array<Uint8>& x)
{
    out.pack(x);
    return out;
}

inline OutBuffer& operator<<(OutBuffer& out, const Array<Sint8>& x)
{
    out.pack(x);
    return out;
}

inline OutBuffer& operator<<(OutBuffer& out, const Array<Uint16>& x)
{
    out.pack(x);
    return out;
}

inline OutBuffer& operator<<(OutBuffer& out, const Array<Sint16>& x)
{
    out.pack(x);
    return out;
}

inline OutBuffer& operator<<(OutBuffer& out, const Array<Uint32>& x)
{
    out.pack(x);
    return out;
}

inline OutBuffer& operator<<(OutBuffer& out, const Array<Sint32>& x)
{
    out.pack(x);
    return out;
}

inline OutBuffer& operator<<(OutBuffer& out, const Array<Uint64>& x)
{
    out.pack(x);
    return out;
}

inline OutBuffer& operator<<(OutBuffer& out, const Array<Sint64>& x)
{
    out.pack(x);
    return out;
}

inline OutBuffer& operator<<(OutBuffer& out, const Array<Real32>& x)
{
    out.pack(x);
    return out;
}

inline OutBuffer& operator<<(OutBuffer& out, const Array<Real64>& x)
{
    out.pack(x);
    return out;
}

inline OutBuffer& operator<<(OutBuffer& out, const Array<Char16>& x)
{
    out.pack(x);
    return out;
}

inline OutBuffer& operator<<(OutBuffer& out, const Array<String>& x)
{
    out.pack(x);
    return out;
}

inline OutBuffer& operator<<(OutBuffer& out, const Array<DateTime>& x)
{
    out.pack(x);
    return out;
}

PEGASUS_NAMESPACE_END

#endif /* Pegasus_OutBuffer_h */
