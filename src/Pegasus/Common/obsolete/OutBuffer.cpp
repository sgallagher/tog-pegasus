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
// $Log: OutBuffer.cpp,v $
// Revision 1.1.1.1  2001/01/14 19:53:39  mike
// Pegasus import
//
//
//END_HISTORY

#include "OutBuffer.h"

PEGASUS_NAMESPACE_BEGIN

OutBuffer::OutBuffer()
{

}

OutBuffer::~OutBuffer()
{

}

void OutBuffer::pack(const Char16& x)
{
    Uint16 code = x;
    align(sizeof(code));
    Representation::toUniversal(code);
    _data.append((const Uint8*)&code, sizeof(code));
}

void OutBuffer::pack(const String& x)
{
    // Pack the size (include the null terminator in the size):

    Uint32 size = x.getLength() + 1;
    pack(size);
    align(sizeof(Char16));

    // Pack each element:

    for (Uint32 i = 0; i < size; i++)
    {
	Uint16 code = x[i];
	_data.append((const Uint8*)&code, sizeof(Uint16));
    }
}

void OutBuffer::pack(const DateTime& x)
{
    // Include the null terminator:

    _data.append((const Uint8*)x.getString(), DateTime::FORMAT_LENGTH + 1);
}

void OutBuffer::pack(const Reference& x)
{
    String tmp = x.getReference();
    pack(tmp);
}

void OutBuffer::pack(const Array<Boolean>& x) 
{
    _packArray(x);
}

void OutBuffer::pack(const Array<Uint8>& x) 
{
    _packArray(x);
}

void OutBuffer::pack(const Array<Sint8>& x) 
{
    _packArray(x);
}

void OutBuffer::pack(const Array<Uint16>& x) 
{
    _packArray(x);
}

void OutBuffer::pack(const Array<Sint16>& x) 
{
    _packArray(x);
}

void OutBuffer::pack(const Array<Uint32>& x) 
{
    _packArray(x);
}

void OutBuffer::pack(const Array<Sint32>& x) 
{
    _packArray(x);
}

void OutBuffer::pack(const Array<Uint64>& x) 
{
    _packArray(x);
}

void OutBuffer::pack(const Array<Sint64>& x) 
{
    _packArray(x);
}

void OutBuffer::pack(const Array<Real32>& x) 
{
    _packArray(x);
}

void OutBuffer::pack(const Array<Real64>& x) 
{
    _packArray(x);
}

void OutBuffer::pack(const Array<Char16>& x)
{
    _packArray(x);
}

void OutBuffer::pack(const Array<String>& x)
{
    _packArray(x);
}

void OutBuffer::pack(const Array<DateTime>& x)
{
    _packArray(x);
}

PEGASUS_NAMESPACE_END
