//%2003////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001, 2002  BMC Software, Hewlett-Packard Development
// Company, L. P., IBM Corp., The Open Group, Tivoli Systems.
// Copyright (c) 2003 BMC Software; Hewlett-Packard Development Company, L. P.;
// IBM Corp.; EMC Corporation, The Open Group.
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

#include "InBuffer.h"

PEGASUS_NAMESPACE_BEGIN

InBuffer::InBuffer() : _pos(0)
{

}

InBuffer::InBuffer(const Array<Uint8>& data) : _data(data), _pos(0)
{

}

InBuffer::~InBuffer()
{

}

void InBuffer::set(const Array<Uint8>& data)
{
    _pos = 0;
    _data = data;
}

void InBuffer::unpack(Boolean& x)
{
    Uint8 tmp;
    _unpack(tmp);
    assert(tmp == 1 || tmp == 0);
    x = tmp ? true : false;
}

void InBuffer::unpack(Uint8& x)
{
    _unpack(x);
}

void InBuffer::unpack(Sint8& x)
{
    _unpack(x);
}

void InBuffer::unpack(Uint16& x)
{
    _unpack(x);
}

void InBuffer::unpack(Sint16& x)
{
    _unpack(x);
}

void InBuffer::unpack(Uint32& x)
{
    _unpack(x);
}

void InBuffer::unpack(Sint32& x)
{
    _unpack(x);
}

void InBuffer::unpack(Uint64& x)
{
    _unpack(x);
}

void InBuffer::unpack(Sint64& x)
{
    _unpack(x);
}

void InBuffer::unpack(Real32& x)
{
    _unpack(x);
}

void InBuffer::unpack(Real64& x)
{
    _unpack(x);
}

void InBuffer::unpack(Char16& x)
{
    Uint16 tmp;
    _unpack(tmp);
    x = tmp;
}

void InBuffer::unpack(String& x)
{
    // Get the size (including the null-terminator):

    Uint32 size;
    _unpack(size);
    assert(size > 0);

    // Get a character at a time:

    x.clear();
    x.reserve(size - 1);

    for (Uint32 i = 0; i < size - 1; i++)
    {
	Uint16 tmp;
	_unpack(tmp);
	x.append(tmp);
    }

    // Discard the null terminator:

    Uint16 nullTerminator;
    _unpack(nullTerminator);
}

void InBuffer::unpack(DateTime& x)
{
    const Uint32 SIZE = DateTime::FORMAT_LENGTH + 1;

    if (_pos + SIZE > _data.getSize())
	throw ExhaustedInBuffer();

    x.set((const char*)&_data[_pos]);
    _pos += SIZE;
}

void InBuffer::unpack(Reference& x)
{
    String tmp;
    unpack(tmp);
    x.setFromReference(tmp);
}

void InBuffer::unpack(Array<Boolean>& x)
{
    _unpackArray(x);
}

void InBuffer::unpack(Array<Uint8>& x)
{
    _unpackArray(x);
}

void InBuffer::unpack(Array<Sint8>& x)
{
    _unpackArray(x);
}

void InBuffer::unpack(Array<Uint16>& x)
{
    _unpackArray(x);
}

void InBuffer::unpack(Array<Sint16>& x)
{
    _unpackArray(x);
}

void InBuffer::unpack(Array<Uint32>& x)
{
    _unpackArray(x);
}

void InBuffer::unpack(Array<Sint32>& x)
{
    _unpackArray(x);
}

void InBuffer::unpack(Array<Uint64>& x)
{
    _unpackArray(x);
}

void InBuffer::unpack(Array<Sint64>& x)
{
    _unpackArray(x);
}

void InBuffer::unpack(Array<Real32>& x)
{
    _unpackArray(x);
}

void InBuffer::unpack(Array<Real64>& x)
{
    _unpackArray(x);
}

void InBuffer::unpack(Array<Char16>& x)
{
    _unpackArray(x);
}

void InBuffer::unpack(Array<String>& x)
{
    _unpackArray(x);
}

void InBuffer::unpack(Array<DateTime>& x)
{
    _unpackArray(x);
}

PEGASUS_NAMESPACE_END
