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
// $Log: InBuffer.h,v $
// Revision 1.1.1.1  2001/01/14 19:53:38  mike
// Pegasus import
//
//
//END_HISTORY

////////////////////////////////////////////////////////////////////////////////
//
// InBuffer.h
//
////////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_InBuffer_h
#define Pegasus_InBuffer_h

#include <cassert>
#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/Array.h>
#include <Pegasus/Common/Representation.h>
#include <Pegasus/Common/String.h>
#include <Pegasus/Common/Exception.h>
#include <Pegasus/Common/DateTime.h>
#include <Pegasus/Common/Reference.h>

PEGASUS_NAMESPACE_BEGIN

class InBuffer
{
private:

    template<class T>
    void _unpack(T& x)
    {
	align(sizeof(x));

	if (getRemaining() < sizeof(x))
	    throw ExhaustedInBuffer();

	x = *((T*)&_data[_pos]);
	_pos += sizeof(x);
	Representation::toNative(x);

	if (_pos > _data.getSize())
	    throw ExhaustedInBuffer();
    }

    template<class T>
    void _unpackArray(Array<T>& x)
    {
	x.clear();

	Uint32 size = 0;
	_unpack(size);

	while (size--)
	{
	    T tmp;
	    unpack(tmp);
	    x.append(tmp);
	}
    }

    static Uint32 _round(Uint32 x, Uint32 n)
    {
	return ((x + (n - 1)) / n) * n;
    }

public:

    InBuffer();

    InBuffer(const Array<Uint8>& data);

    ~InBuffer();

    void set(const Array<Uint8>& data);

    const Array<Uint8>& getData() const 
    {
	return _data;
    }

    const Uint32 getPos() const
    {
	return _pos;
    }

    const Uint32 getRemaining() const
    {
	return _data.getSize() - _pos;
    }

    void align(Uint32 n)
    {
	Uint32 pos = _round(_pos, n);

	if (pos > _data.getSize())
	    throw ExhaustedInBuffer();

	_pos = pos;
    }

    void unpack(Boolean& x);

    void unpack(Uint8& x);

    void unpack(Sint8& x);

    void unpack(Uint16& x);

    void unpack(Sint16& x);

    void unpack(Uint32& x);

    void unpack(Sint32& x);

    void unpack(Uint64& x);

    void unpack(Sint64& x);

    void unpack(Real32& x);

    void unpack(Real64& x);

    void unpack(Char16& x);

    void unpack(String& x);

    void unpack(DateTime& x);

    void unpack(Reference& x);

    void unpack(Array<Boolean>& x);

    void unpack(Array<Uint8>& x);

    void unpack(Array<Sint8>& x);

    void unpack(Array<Uint16>& x);

    void unpack(Array<Sint16>& x);

    void unpack(Array<Uint32>& x);

    void unpack(Array<Sint32>& x);

    void unpack(Array<Uint64>& x);

    void unpack(Array<Sint64>& x);

    void unpack(Array<Real32>& x);

    void unpack(Array<Real64>& x);

    void unpack(Array<Char16>& x);

    void unpack(Array<String>& x);

    void unpack(Array<DateTime>& x);

private:


    Array<Uint8> _data;
    Uint32 _pos;
};

inline InBuffer& operator>>(InBuffer& in, Boolean& x)
{
    in.unpack(x);
    return in;
}

inline InBuffer& operator>>(InBuffer& in, Uint8& x)
{
    in.unpack(x);
    return in;
}

inline InBuffer& operator>>(InBuffer& in, Sint8& x)
{
    in.unpack(x);
    return in;
}

inline InBuffer& operator>>(InBuffer& in, Uint16& x)
{
    in.unpack(x);
    return in;
}

inline InBuffer& operator>>(InBuffer& in, Sint16& x)
{
    in.unpack(x);
    return in;
}

inline InBuffer& operator>>(InBuffer& in, Uint32& x)
{
    in.unpack(x);
    return in;
}

inline InBuffer& operator>>(InBuffer& in, Sint32& x)
{
    in.unpack(x);
    return in;
}

inline InBuffer& operator>>(InBuffer& in, Uint64& x)
{
    in.unpack(x);
    return in;
}

inline InBuffer& operator>>(InBuffer& in, Sint64& x)
{
    in.unpack(x);
    return in;
}

inline InBuffer& operator>>(InBuffer& in, Real32& x)
{
    in.unpack(x);
    return in;
}

inline InBuffer& operator>>(InBuffer& in, Real64& x)
{
    in.unpack(x);
    return in;
}

inline InBuffer& operator>>(InBuffer& in, Char16& x)
{
    in.unpack(x);
    return in;
}

inline InBuffer& operator>>(InBuffer& in, String& x)
{
    in.unpack(x);
    return in;
}

inline InBuffer& operator>>(InBuffer& in, DateTime& x)
{
    in.unpack(x);
    return in;
}

inline InBuffer& operator>>(InBuffer& in, Reference& x)
{
    in.unpack(x);
    return in;
}

inline InBuffer& operator>>(InBuffer& in, Array<Boolean>& x)
{
    in.unpack(x);
    return in;
}

inline InBuffer& operator>>(InBuffer& in, Array<Uint8>& x)
{
    in.unpack(x);
    return in;
}

inline InBuffer& operator>>(InBuffer& in, Array<Sint8>& x)
{
    in.unpack(x);
    return in;
}

inline InBuffer& operator>>(InBuffer& in, Array<Uint16>& x)
{
    in.unpack(x);
    return in;
}

inline InBuffer& operator>>(InBuffer& in, Array<Sint16>& x)
{
    in.unpack(x);
    return in;
}

inline InBuffer& operator>>(InBuffer& in, Array<Uint32>& x)
{
    in.unpack(x);
    return in;
}

inline InBuffer& operator>>(InBuffer& in, Array<Sint32>& x)
{
    in.unpack(x);
    return in;
}

inline InBuffer& operator>>(InBuffer& in, Array<Uint64>& x)
{
    in.unpack(x);
    return in;
}

inline InBuffer& operator>>(InBuffer& in, Array<Sint64>& x)
{
    in.unpack(x);
    return in;
}

inline InBuffer& operator>>(InBuffer& in, Array<Real32>& x)
{
    in.unpack(x);
    return in;
}

inline InBuffer& operator>>(InBuffer& in, Array<Real64>& x)
{
    in.unpack(x);
    return in;
}

inline InBuffer& operator>>(InBuffer& in, Array<Char16>& x)
{
    in.unpack(x);
    return in;
}

inline InBuffer& operator>>(InBuffer& in, Array<String>& x)
{
    in.unpack(x);
    return in;
}

inline InBuffer& operator>>(InBuffer& in, Array<DateTime>& x)
{
    in.unpack(x);
    return in;
}

PEGASUS_NAMESPACE_END

#endif /* Pegasus_InBuffer_h */
