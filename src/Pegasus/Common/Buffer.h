//%2006////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001, 2002 BMC Software; Hewlett-Packard Development
// Company, L.P.; IBM Corp.; The Open Group; Tivoli Systems.
// Copyright (c) 2003 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation, The Open Group.
// Copyright (c) 2004 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation; VERITAS Software Corporation; The Open Group.
// Copyright (c) 2005 Hewlett-Packard Development Company, L.P.; IBM Corp.;
// EMC Corporation; VERITAS Software Corporation; The Open Group.
// Copyright (c) 2006 Hewlett-Packard Development Company, L.P.; IBM Corp.;
// EMC Corporation; Symantec Corporation; The Open Group.
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
// Author: Michael E. Brasher (mike-brasher@austin.rr.com -- Inova Europe)
//
//%/////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_Buffer_h
#define Pegasus_Buffer_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/Linkage.h>
#include <cstring>

PEGASUS_NAMESPACE_BEGIN

struct BufferRep
{
    size_t size;
    size_t cap;
    char data[1];
};

class PEGASUS_COMMON_LINKAGE Buffer
{
public:

    Buffer();

    Buffer(const Buffer& x);

    Buffer(const char* data, size_t size);

    ~Buffer();

    Buffer& operator=(const Buffer& x);

    void swap(Buffer& x);

    size_t size() const;

    size_t capacity() const;

    /**
        Returns a pointer to a character buffer with the Buffer contents.
        The character buffer is null-terminated even if the Buffer contents
        do not include a null termination character.
     */
    const char* getData() const;

    char get(size_t i) const;

    void set(size_t i, char x);

    const char& operator[](size_t i) const;

    void reserveCapacity(size_t cap);

    void grow(size_t size, char x = '\0');

    void append(char x);

    void append(const char* data, size_t size);

    void append(char c1, char c2, char c3, char c4);

    void append(
	char c1, char c2, char c3, char c4, char c5, char c6, char c7, char c8);

    void insert(size_t pos, const char* data, size_t size);

    void remove(size_t pos, size_t size);

    void remove(size_t pos);

    void clear();

private:

    void _reserve_aux(size_t cap);

    void _append_char_aux();

    BufferRep* _rep;
    static BufferRep _empty_rep;
};

inline Buffer::Buffer() : _rep(&_empty_rep)
{
}

inline Buffer::~Buffer()
{
    if (_rep->cap != 0)
	free(_rep);
}

inline void Buffer::swap(Buffer& x)
{
    BufferRep* tmp = _rep;
    _rep = x._rep;
    x._rep = tmp;
}

inline size_t Buffer::size() const
{
    return _rep->size;
}

inline size_t Buffer::capacity() const
{
    return _rep->cap;
}

inline const char* Buffer::getData() const
{
    if (_rep->cap == 0)
    {
        const_cast<Buffer*>(this)->_append_char_aux();
    }

    _rep->data[_rep->size] = '\0';

    return _rep->data;
}

inline char Buffer::get(size_t i) const
{
    return _rep->data[i];
}

inline void Buffer::set(size_t i, char x)
{
    _rep->data[i] = x;
}

inline const char& Buffer::operator[](size_t i) const
{
    return _rep->data[i];
}

inline void Buffer::reserveCapacity(size_t cap)
{
    if (cap > _rep->cap)
	_reserve_aux(cap);
}

inline void Buffer::grow(size_t size, char x)
{
    size_t cap = _rep->size + size;

    if (cap > _rep->cap)
	_reserve_aux(cap);

    memset(_rep->data + _rep->size, x, size);
    _rep->size += size;
}

inline void Buffer::append(char x)
{
    if (_rep->size == _rep->cap)
	_append_char_aux();

    _rep->data[_rep->size++] = x;
}

inline void Buffer::append(const char* data, size_t size)
{
    size_t cap = _rep->size + size;

    if (cap > _rep->cap)
	_reserve_aux(cap);

    memcpy(_rep->data + _rep->size, data, size);
    _rep->size += size;
}

inline void Buffer::clear()
{
    if (_rep->cap != 0)
	_rep->size = 0;
}

inline void Buffer::remove(size_t pos)
{
    remove(pos, 1);
}

inline void Buffer::append(char c1, char c2, char c3, char c4)
{
    size_t cap = _rep->size + 4;

    if (cap > _rep->cap)
	_reserve_aux(cap);

    char* p = _rep->data + _rep->size;
    p[0] = c1;
    p[1] = c2;
    p[2] = c3;
    p[3] = c4;
    _rep->size += 4;
}

inline void Buffer::append(
    char c1, char c2, char c3, char c4, char c5, char c6, char c7, char c8)
{
    size_t cap = _rep->size + 8;

    if (cap > _rep->cap)
	_reserve_aux(cap);

    char* p = _rep->data + _rep->size;
    p[0] = c1;
    p[1] = c2;
    p[2] = c3;
    p[3] = c4;
    p[4] = c5;
    p[5] = c6;
    p[6] = c7;
    p[7] = c8;
    _rep->size += 8;
}

inline bool operator==(const Buffer& x, const Buffer& y)
{
    return memcmp(x.getData(), y.getData(), x.size()) == 0;
}

PEGASUS_NAMESPACE_END

#endif /* Pegasus_Buffer_h */
