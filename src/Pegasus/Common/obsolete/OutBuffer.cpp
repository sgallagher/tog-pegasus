
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
