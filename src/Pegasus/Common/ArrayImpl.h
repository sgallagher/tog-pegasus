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
// Author: Mike Brasher (mbrasher@bmc.com)
//
// Modified By: Roger Kumpf, Hewlett-Packard Company (roger_kumpf@hp.com)
//
//%/////////////////////////////////////////////////////////////////////////////

// Only include if not included as general template or if explicit instantiation
#if !defined(Pegasus_ArrayImpl_h) || defined(PEGASUS_ARRAY_T)
#if !defined(PEGASUS_ARRAY_T)
#define Pegasus_ArrayImpl_h
#endif

PEGASUS_NAMESPACE_END

#include <Pegasus/Common/Memory.h>
#include <Pegasus/Common/ArrayRep.h>
#include <Pegasus/Common/InternalException.h>

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

    if (_rep == 0)
    {
        throw NullPointer();
    }

    InitializeRaw(static_cast<ArrayRep<PEGASUS_ARRAY_T>*>(_rep)->data(), size);
}

#ifndef PEGASUS_ARRAY_T
template<class PEGASUS_ARRAY_T>
#endif
Array<PEGASUS_ARRAY_T>::Array(Uint32 size, const PEGASUS_ARRAY_T& x)
{
    _rep = ArrayRep<PEGASUS_ARRAY_T>::create(size);

    if (_rep == 0)
    {
        throw NullPointer();
    }

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

    if (_rep == 0)
    {
        throw NullPointer();
    }

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
void Array<PEGASUS_ARRAY_T>::reserveCapacity(Uint32 capacity)
{
    if (capacity > static_cast<ArrayRep<PEGASUS_ARRAY_T>*>(_rep)->capacity)
    {
        Uint32 size = this->size();
        ArrayRep<PEGASUS_ARRAY_T>* rep =
            ArrayRep<PEGASUS_ARRAY_T>::create(capacity);

        if (rep != 0)
        {
            rep->size = size;
            CopyToRaw(
                rep->data(),
                static_cast<ArrayRep<PEGASUS_ARRAY_T>*>(_rep)->data(),
                size);
            ArrayRep<PEGASUS_ARRAY_T>::destroy(
                static_cast<ArrayRep<PEGASUS_ARRAY_T>*>(_rep));
            _rep = rep;
        }
    }
}

#ifndef PEGASUS_ARRAY_T
template<class PEGASUS_ARRAY_T>
#endif
void Array<PEGASUS_ARRAY_T>::grow(Uint32 size, const PEGASUS_ARRAY_T& x)
{
    Uint32 oldSize = static_cast<ArrayRep<PEGASUS_ARRAY_T>*>(_rep)->size;
    reserveCapacity(oldSize + size);

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
PEGASUS_ARRAY_T& Array<PEGASUS_ARRAY_T>::operator[](Uint32 index)
{
    if (index >= size())
        throw IndexOutOfBoundsException();

    return static_cast<ArrayRep<PEGASUS_ARRAY_T>*>(_rep)->data()[index];
}

#ifndef PEGASUS_ARRAY_T
template<class PEGASUS_ARRAY_T>
#endif
const PEGASUS_ARRAY_T& Array<PEGASUS_ARRAY_T>::operator[](Uint32 index) const
{
    if (index >= size())
        throw IndexOutOfBoundsException();

    return static_cast<ArrayRep<PEGASUS_ARRAY_T>*>(_rep)->data()[index];
}

#ifndef PEGASUS_ARRAY_T
template<class PEGASUS_ARRAY_T>
#endif
void Array<PEGASUS_ARRAY_T>::append(const PEGASUS_ARRAY_T& x)
{
    reserveCapacity(size() + 1);
    new (_data() + size()) PEGASUS_ARRAY_T(x);
    static_cast<ArrayRep<PEGASUS_ARRAY_T>*>(_rep)->size++;
}

#ifndef PEGASUS_ARRAY_T
template<class PEGASUS_ARRAY_T>
#endif
void Array<PEGASUS_ARRAY_T>::append(const PEGASUS_ARRAY_T* x, Uint32 size)
{
    reserveCapacity(this->size() + size);
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
    reserveCapacity(this->size() + size);
    memmove(_data() + size, _data(), sizeof(PEGASUS_ARRAY_T) * this->size());
    CopyToRaw(_data(), x, size);
    static_cast<ArrayRep<PEGASUS_ARRAY_T>*>(_rep)->size += size;
}

#ifndef PEGASUS_ARRAY_T
template<class PEGASUS_ARRAY_T>
#endif
void Array<PEGASUS_ARRAY_T>::insert(Uint32 index, const PEGASUS_ARRAY_T& x)
{
    insert(index, &x, 1);
}

#ifndef PEGASUS_ARRAY_T
template<class PEGASUS_ARRAY_T>
#endif
void Array<PEGASUS_ARRAY_T>::insert(Uint32 index, const PEGASUS_ARRAY_T* x, Uint32 size)
{
    if (index > this->size())
        throw IndexOutOfBoundsException();

    reserveCapacity(this->size() + size);

    Uint32 n = this->size() - index;

    if (n)
        memmove(_data() + index + size,
                _data() + index,
                sizeof(PEGASUS_ARRAY_T) * n);

    CopyToRaw(_data() + index, x, size);
    static_cast<ArrayRep<PEGASUS_ARRAY_T>*>(_rep)->size += size;
}

#ifndef PEGASUS_ARRAY_T
template<class PEGASUS_ARRAY_T>
#endif
void Array<PEGASUS_ARRAY_T>::remove(Uint32 index)
{
    remove(index, 1);
}

#ifndef PEGASUS_ARRAY_T
template<class PEGASUS_ARRAY_T>
#endif
void Array<PEGASUS_ARRAY_T>::remove(Uint32 index, Uint32 size)
{
    if (index + size - 1 > this->size())
        throw IndexOutOfBoundsException();

    Destroy(_data() + index, size);

    Uint32 rem = this->size() - (index + size);

    if (rem)
        memmove(_data() + index,
                _data() + index + size,
                sizeof(PEGASUS_ARRAY_T) * rem);

    static_cast<ArrayRep<PEGASUS_ARRAY_T>*>(_rep)->size -= size;
}

#ifndef PEGASUS_ARRAY_T
template<class PEGASUS_ARRAY_T>
#endif
PEGASUS_ARRAY_T* Array<PEGASUS_ARRAY_T>::_data() const
{
    return static_cast<ArrayRep<PEGASUS_ARRAY_T>*>(_rep)->data();
}

#endif //!defined(Pegasus_ArrayImpl_h) || !defined(PEGASUS_ARRAY_T)
