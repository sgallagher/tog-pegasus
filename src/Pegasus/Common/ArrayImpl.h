//%/////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001, 2002 BMC Software, Hewlett-Packard Company, IBM,
// The Open Group, Tivoli Systems
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
// Author: Mike Brasher (mbrasher@bmc.com)
//
// Modified By: Roger Kumpf, Hewlett-Packard Company (roger_kumpf@hp.com)
//
//%/////////////////////////////////////////////////////////////////////////////

//#if defined(PEGASUS_EXPLICIT_INSTANTIATION) || !defined(PEGASUS_ARRAY_T)
// Only include if not included as general template or if explicit instantiation
#if !defined(Pegasus_ArrayImpl_h) || defined(PEGASUS_ARRAY_T)
#if !defined(PEGASUS_ARRAY_T)
#define Pegasus_ArrayImpl_h
#endif

PEGASUS_NAMESPACE_END

#include <Pegasus/Common/Memory.h>
#include <Pegasus/Common/ArrayRep.h>
#include <Pegasus/Common/Exception.h>

#ifdef PEGASUS_HAS_EBCDIC
#include <unistd.h>
#endif

PEGASUS_NAMESPACE_BEGIN

#ifndef PEGASUS_ARRAY_T
template<class PEGASUS_ARRAY_T>
#endif
Array<PEGASUS_ARRAY_T>::Array()
{
    _rep = ArrayRep<PEGASUS_ARRAY_T>::create(0);
}

#ifndef PEGASUS_ARRAY_T
template<class PEGASUS_ARRAY_T>
#endif
Array<PEGASUS_ARRAY_T>::Array(const Array<PEGASUS_ARRAY_T>& x)
{
    _rep = static_cast<ArrayRep<PEGASUS_ARRAY_T>*>(x._rep)->clone();
}

#ifndef PEGASUS_ARRAY_T
template<class PEGASUS_ARRAY_T>
#endif
Array<PEGASUS_ARRAY_T>::Array(Uint32 size)
{
    _rep = ArrayRep<PEGASUS_ARRAY_T>::create(size);
    InitializeRaw(static_cast<ArrayRep<PEGASUS_ARRAY_T>*>(_rep)->data(), size);
}

#ifndef PEGASUS_ARRAY_T
template<class PEGASUS_ARRAY_T>
#endif
Array<PEGASUS_ARRAY_T>::Array(Uint32 size, const PEGASUS_ARRAY_T& x)
{
    _rep = ArrayRep<PEGASUS_ARRAY_T>::create(size);

    PEGASUS_ARRAY_T* data = static_cast<ArrayRep<PEGASUS_ARRAY_T>*>(_rep)->data();

    while (size--)
        new(data++) PEGASUS_ARRAY_T(x);
}

#ifndef PEGASUS_ARRAY_T
template<class PEGASUS_ARRAY_T>
#endif
Array<PEGASUS_ARRAY_T>::Array(const PEGASUS_ARRAY_T* items, Uint32 size)
{
    _rep = ArrayRep<PEGASUS_ARRAY_T>::create(size);
    CopyToRaw(static_cast<ArrayRep<PEGASUS_ARRAY_T>*>(_rep)->data(), items, size);
}

#ifndef PEGASUS_ARRAY_T
template<class PEGASUS_ARRAY_T>
#endif
Array<PEGASUS_ARRAY_T>::~Array()
{
    ArrayRep<PEGASUS_ARRAY_T>::destroy(static_cast<ArrayRep<PEGASUS_ARRAY_T>*>(_rep));
}

#ifndef PEGASUS_ARRAY_T
template<class PEGASUS_ARRAY_T>
#endif
Array<PEGASUS_ARRAY_T>& Array<PEGASUS_ARRAY_T>::operator=(
    const Array<PEGASUS_ARRAY_T>& x)
{
    if (static_cast<ArrayRep<PEGASUS_ARRAY_T>*>(x._rep) !=
        static_cast<ArrayRep<PEGASUS_ARRAY_T>*>(_rep))
    {
        ArrayRep<PEGASUS_ARRAY_T>::destroy(static_cast<ArrayRep<PEGASUS_ARRAY_T>*>(_rep));
        _rep = static_cast<ArrayRep<PEGASUS_ARRAY_T>*>(x._rep)->clone();
    }
    return *this;
}

#ifndef PEGASUS_ARRAY_T
template<class PEGASUS_ARRAY_T>
#endif
void Array<PEGASUS_ARRAY_T>::clear()
{
    ArrayRep<PEGASUS_ARRAY_T>::destroy(static_cast<ArrayRep<PEGASUS_ARRAY_T>*>(_rep));
    _rep = ArrayRep<PEGASUS_ARRAY_T>::create(0);
}

#ifndef PEGASUS_ARRAY_T
template<class PEGASUS_ARRAY_T>
#endif
void Array<PEGASUS_ARRAY_T>::reserve(Uint32 capacity)
{
    if (capacity > static_cast<ArrayRep<PEGASUS_ARRAY_T>*>(_rep)->capacity)
    {
        Uint32 size = this->size();
        ArrayRep<PEGASUS_ARRAY_T>* rep = ArrayRep<PEGASUS_ARRAY_T>::create(capacity);
        rep->size = size;
        CopyToRaw(rep->data(), static_cast<ArrayRep<PEGASUS_ARRAY_T>*>(_rep)->data(), size);
        ArrayRep<PEGASUS_ARRAY_T>::destroy(static_cast<ArrayRep<PEGASUS_ARRAY_T>*>(_rep));
        _rep = rep;
    }
}

#ifndef PEGASUS_ARRAY_T
template<class PEGASUS_ARRAY_T>
#endif
void Array<PEGASUS_ARRAY_T>::grow(Uint32 size, const PEGASUS_ARRAY_T& x)
{
    Uint32 oldSize = static_cast<ArrayRep<PEGASUS_ARRAY_T>*>(_rep)->size;
    reserve(oldSize + size);

    PEGASUS_ARRAY_T* p = static_cast<ArrayRep<PEGASUS_ARRAY_T>*>(_rep)->data() + oldSize;
    Uint32 n = size;

    while (n--)
        new(p++) PEGASUS_ARRAY_T(x);

    static_cast<ArrayRep<PEGASUS_ARRAY_T>*>(_rep)->size += size;
}

#ifndef PEGASUS_ARRAY_T
template<class PEGASUS_ARRAY_T>
#endif
void Array<PEGASUS_ARRAY_T>::swap(Array<PEGASUS_ARRAY_T>& x)
{
    ArrayRep<PEGASUS_ARRAY_T>* tmp = static_cast<ArrayRep<PEGASUS_ARRAY_T>*>(_rep);
    _rep = static_cast<ArrayRep<PEGASUS_ARRAY_T>*>(x._rep);
    x._rep = tmp;
}

#ifndef PEGASUS_ARRAY_T
template<class PEGASUS_ARRAY_T>
#endif
Uint32 Array<PEGASUS_ARRAY_T>::size() const
{
    return static_cast<ArrayRep<PEGASUS_ARRAY_T>*>(_rep)->size;
}

#ifndef PEGASUS_ARRAY_T
template<class PEGASUS_ARRAY_T>
#endif
Uint32 Array<PEGASUS_ARRAY_T>::getCapacity() const
{
    return static_cast<ArrayRep<PEGASUS_ARRAY_T>*>(_rep)->capacity;
}

#ifndef PEGASUS_ARRAY_T
template<class PEGASUS_ARRAY_T>
#endif
const PEGASUS_ARRAY_T* Array<PEGASUS_ARRAY_T>::getData() const
{
    return static_cast<ArrayRep<PEGASUS_ARRAY_T>*>(_rep)->data();
}

#ifndef PEGASUS_ARRAY_T
template<class PEGASUS_ARRAY_T>
#endif
PEGASUS_ARRAY_T& Array<PEGASUS_ARRAY_T>::operator[](Uint32 pos)
{
    if (pos >= size())
        throw OutOfBounds();

    return static_cast<ArrayRep<PEGASUS_ARRAY_T>*>(_rep)->data()[pos];
}

#ifndef PEGASUS_ARRAY_T
template<class PEGASUS_ARRAY_T>
#endif
const PEGASUS_ARRAY_T& Array<PEGASUS_ARRAY_T>::operator[](Uint32 pos) const
{
    if (pos >= size())
        throw OutOfBounds();

    return static_cast<ArrayRep<PEGASUS_ARRAY_T>*>(_rep)->data()[pos];
}

#ifndef PEGASUS_ARRAY_T
template<class PEGASUS_ARRAY_T>
#endif
void Array<PEGASUS_ARRAY_T>::append(const PEGASUS_ARRAY_T& x)
{
    reserve(size() + 1);
    new (_data() + size()) PEGASUS_ARRAY_T(x);
    static_cast<ArrayRep<PEGASUS_ARRAY_T>*>(_rep)->size++;
}

#ifndef PEGASUS_ARRAY_T
template<class PEGASUS_ARRAY_T>
#endif
void Array<PEGASUS_ARRAY_T>::append(const PEGASUS_ARRAY_T* x, Uint32 size)
{
    reserve(this->size() + size);
    CopyToRaw(_data() + this->size(), x, size);
    static_cast<ArrayRep<PEGASUS_ARRAY_T>*>(_rep)->size += size;
}

#ifndef PEGASUS_ARRAY_T
template<class PEGASUS_ARRAY_T>
#endif
void Array<PEGASUS_ARRAY_T>::appendArray(const Array<PEGASUS_ARRAY_T>& x)
{
    append(x.getData(), x.size());
}

#ifndef PEGASUS_ARRAY_T
template<class PEGASUS_ARRAY_T>
#endif
void Array<PEGASUS_ARRAY_T>::prepend(const PEGASUS_ARRAY_T& x)
{
    prepend(&x, 1);
}

#ifndef PEGASUS_ARRAY_T
template<class PEGASUS_ARRAY_T>
#endif
void Array<PEGASUS_ARRAY_T>::prepend(const PEGASUS_ARRAY_T* x, Uint32 size)
{
    reserve(this->size() + size);
    memmove(_data() + size, _data(), sizeof(PEGASUS_ARRAY_T) * this->size());
    CopyToRaw(_data(), x, size);
    static_cast<ArrayRep<PEGASUS_ARRAY_T>*>(_rep)->size += size;
}

#ifndef PEGASUS_ARRAY_T
template<class PEGASUS_ARRAY_T>
#endif
void Array<PEGASUS_ARRAY_T>::insert(Uint32 pos, const PEGASUS_ARRAY_T& x)
{
    insert(pos, &x, 1);
}

#ifndef PEGASUS_ARRAY_T
template<class PEGASUS_ARRAY_T>
#endif
void Array<PEGASUS_ARRAY_T>::insert(Uint32 pos, const PEGASUS_ARRAY_T* x, Uint32 size)
{
    if (pos > this->size())
        throw OutOfBounds();

    reserve(this->size() + size);

    Uint32 n = this->size() - pos;

    if (n)
        memmove(
            _data() + pos + size, _data() + pos, sizeof(PEGASUS_ARRAY_T) * n);

    CopyToRaw(_data() + pos, x, size);
    static_cast<ArrayRep<PEGASUS_ARRAY_T>*>(_rep)->size += size;
}

#ifndef PEGASUS_ARRAY_T
template<class PEGASUS_ARRAY_T>
#endif
void Array<PEGASUS_ARRAY_T>::remove(Uint32 pos)
{
    remove(pos, 1);
}

#ifndef PEGASUS_ARRAY_T
template<class PEGASUS_ARRAY_T>
#endif
void Array<PEGASUS_ARRAY_T>::remove(Uint32 pos, Uint32 size)
{
    if (pos + size - 1 > this->size())
        throw OutOfBounds();

    Destroy(_data() + pos, size);

    Uint32 rem = this->size() - (pos + size);

    if (rem)
        memmove(
            _data() + pos, _data() + pos + size, sizeof(PEGASUS_ARRAY_T) * rem);

    static_cast<ArrayRep<PEGASUS_ARRAY_T>*>(_rep)->size -= size;
}

#ifdef PEGASUS_HAS_EBCDIC

#ifndef PEGASUS_ARRAY_T
template<class PEGASUS_ARRAY_T>
#endif
void Array<PEGASUS_ARRAY_T>::etoa()
{
#if PEGASUS_ARRAY_T == Sint8
    __etoa_l((char *)_data(),static_cast<ArrayRep<PEGASUS_ARRAY_T>*>(_rep)->size);
#endif
}

#ifndef PEGASUS_ARRAY_T
template<class PEGASUS_ARRAY_T>
#endif
void Array<PEGASUS_ARRAY_T>::atoe()
{
#if PEGASUS_ARRAY_T == Sint8
    __atoe_l((char *)_data(),static_cast<ArrayRep<PEGASUS_ARRAY_T>*>(_rep)->size);
#endif
}

#endif

#ifndef PEGASUS_ARRAY_T
template<class PEGASUS_ARRAY_T>
#endif
PEGASUS_ARRAY_T* Array<PEGASUS_ARRAY_T>::begin()
{
    return static_cast<ArrayRep<PEGASUS_ARRAY_T>*>(_rep)->data();
}

#ifndef PEGASUS_ARRAY_T
template<class PEGASUS_ARRAY_T>
#endif
PEGASUS_ARRAY_T* Array<PEGASUS_ARRAY_T>::end()
{
    return static_cast<ArrayRep<PEGASUS_ARRAY_T>*>(_rep)->data() + size();
}

#ifndef PEGASUS_ARRAY_T
template<class PEGASUS_ARRAY_T>
#endif
const PEGASUS_ARRAY_T* Array<PEGASUS_ARRAY_T>::begin() const
{
    return getData();
}

#ifndef PEGASUS_ARRAY_T
template<class PEGASUS_ARRAY_T>
#endif
const PEGASUS_ARRAY_T* Array<PEGASUS_ARRAY_T>::end() const
{
    return getData() + size();
}

#ifndef PEGASUS_ARRAY_T
template<class PEGASUS_ARRAY_T>
#endif
PEGASUS_ARRAY_T* Array<PEGASUS_ARRAY_T>::_data() const
{
    return static_cast<ArrayRep<PEGASUS_ARRAY_T>*>(_rep)->data();
}

#endif //!defined(Pegasus_ArrayImpl_h) || !defined(PEGASUS_ARRAY_T)
//#endif /*defined(PEGASUS_EXPLICIT_INSTANTIATION) || !defined(PEGASUS_ARRAY_T)*/
