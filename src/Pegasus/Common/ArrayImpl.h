//%/////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001 The Open group, BMC Software, Tivoli Systems, IBM
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

#if defined(PEGASUS_EXPLICIT_INSTANTIATION) || !defined(PEGASUS_ARRAY_T)

#ifndef PEGASUS_ARRAY_T
template<class PEGASUS_ARRAY_T>
#else
PEGASUS_TEMPLATE_SPECIALIZATION
#endif
Array<PEGASUS_ARRAY_T>::Array()
{
    _rep = Rep::create(0);
}

#ifndef PEGASUS_ARRAY_T
template<class PEGASUS_ARRAY_T>
#else
PEGASUS_TEMPLATE_SPECIALIZATION
#endif
Array<PEGASUS_ARRAY_T>::Array(const Array<PEGASUS_ARRAY_T>& x)
{
    _rep = x._rep->clone();
}

#ifndef PEGASUS_ARRAY_T
template<class PEGASUS_ARRAY_T>
#else
PEGASUS_TEMPLATE_SPECIALIZATION
#endif
Array<PEGASUS_ARRAY_T>::Array(Uint32 size)
{
    _rep = Rep::create(size);
#if defined(PEGASUS_PLATFORM_HPUX_PARISC_ACC) || defined(PEGASUS_PLATFORM_LINUX_GENERIC_GNU)
    InitializeRaw<PEGASUS_ARRAY_T>(_rep->data(), size);
#else
    InitializeRaw(_rep->data(), size);
#endif
}

#ifndef PEGASUS_ARRAY_T
template<class PEGASUS_ARRAY_T>
#else
PEGASUS_TEMPLATE_SPECIALIZATION
#endif
Array<PEGASUS_ARRAY_T>::Array(Uint32 size, const PEGASUS_ARRAY_T& x)
{
    _rep = Rep::create(size);

    PEGASUS_ARRAY_T* data = _rep->data();

    while (size--)
	new(data++) PEGASUS_ARRAY_T(x);
}

#ifndef PEGASUS_ARRAY_T
template<class PEGASUS_ARRAY_T>
#else
PEGASUS_TEMPLATE_SPECIALIZATION
#endif
Array<PEGASUS_ARRAY_T>::Array(const PEGASUS_ARRAY_T* items, Uint32 size)
{
    _rep = Rep::create(size);
#if defined(PEGASUS_PLATFORM_HPUX_PARISC_ACC) || defined(PEGASUS_PLATFORM_LINUX_GENERIC_GNU)
    CopyToRaw<PEGASUS_ARRAY_T>(_rep->data(), items, size);
#else
    CopyToRaw(_rep->data(), items, size);
#endif
}

#ifndef PEGASUS_ARRAY_T
template<class PEGASUS_ARRAY_T>
#else
PEGASUS_TEMPLATE_SPECIALIZATION
#endif
Array<PEGASUS_ARRAY_T>::Array(ArrayRep<PEGASUS_ARRAY_T>* rep)
{
    _rep = rep->clone();
}

#ifndef PEGASUS_ARRAY_T
template<class PEGASUS_ARRAY_T>
#else
PEGASUS_TEMPLATE_SPECIALIZATION
#endif
Array<PEGASUS_ARRAY_T>::~Array()
{
    Rep::destroy(_rep);
}

#ifndef PEGASUS_ARRAY_T
template<class PEGASUS_ARRAY_T>
#else
PEGASUS_TEMPLATE_SPECIALIZATION
#endif
Array<PEGASUS_ARRAY_T>& Array<PEGASUS_ARRAY_T>::operator=(
    const Array<PEGASUS_ARRAY_T>& x)
{
    if (x._rep != _rep)
    {
	Rep::destroy(_rep);
	_rep = x._rep->clone();
    }
    return *this;
}

#ifndef PEGASUS_ARRAY_T
template<class PEGASUS_ARRAY_T>
#else
PEGASUS_TEMPLATE_SPECIALIZATION
#endif
void Array<PEGASUS_ARRAY_T>::clear()
{
    Rep::destroy(_rep);
    _rep = Rep::create(0);
}

#ifndef PEGASUS_ARRAY_T
template<class PEGASUS_ARRAY_T>
#else
PEGASUS_TEMPLATE_SPECIALIZATION
#endif
void Array<PEGASUS_ARRAY_T>::reserve(Uint32 capacity)
{
    if (capacity > _rep->capacity)
    {
        Uint32 size = this->size();
        Rep* rep = Rep::create(capacity);
        rep->size = size;
#if defined(PEGASUS_PLATFORM_HPUX_PARISC_ACC) || defined(PEGASUS_PLATFORM_LINUX_GENERIC_GNU)
        CopyToRaw<PEGASUS_ARRAY_T>(rep->data(), _rep->data(), size);
#else
        CopyToRaw(rep->data(), _rep->data(), size);
#endif
        Rep::destroy(_rep);
        _rep = rep;
    }
}

#ifndef PEGASUS_ARRAY_T
template<class PEGASUS_ARRAY_T>
#else
PEGASUS_TEMPLATE_SPECIALIZATION
#endif
void Array<PEGASUS_ARRAY_T>::grow(Uint32 size, const PEGASUS_ARRAY_T& x)
{
    Uint32 oldSize = _rep->size;
    reserve(oldSize + size);

    PEGASUS_ARRAY_T* p = _rep->data() + oldSize;
    Uint32 n = size;

    while (n--)
	new(p++) PEGASUS_ARRAY_T(x);

    _rep->size += size;
}

#ifndef PEGASUS_ARRAY_T
template<class PEGASUS_ARRAY_T>
#else
PEGASUS_TEMPLATE_SPECIALIZATION
#endif
void Array<PEGASUS_ARRAY_T>::swap(Array<PEGASUS_ARRAY_T>& x)
{
    Rep* tmp = _rep;
    _rep = x._rep;
    x._rep = tmp;
}

#ifndef PEGASUS_ARRAY_T
template<class PEGASUS_ARRAY_T>
#else
PEGASUS_TEMPLATE_SPECIALIZATION
#endif
Uint32 Array<PEGASUS_ARRAY_T>::size() const
{
    return _rep->size;
}

#ifndef PEGASUS_ARRAY_T
template<class PEGASUS_ARRAY_T>
#else
PEGASUS_TEMPLATE_SPECIALIZATION
#endif
Uint32 Array<PEGASUS_ARRAY_T>::getCapacity() const
{
    return _rep->capacity;
}

#ifndef PEGASUS_ARRAY_T
template<class PEGASUS_ARRAY_T>
#else
PEGASUS_TEMPLATE_SPECIALIZATION
#endif
const PEGASUS_ARRAY_T* Array<PEGASUS_ARRAY_T>::getData() const
{
    return _rep->data();
}

#ifndef PEGASUS_ARRAY_T
template<class PEGASUS_ARRAY_T>
#else
PEGASUS_TEMPLATE_SPECIALIZATION
#endif
void Array<PEGASUS_ARRAY_T>::append(const PEGASUS_ARRAY_T& x)
{
    reserve(size() + 1);
    new (_data() + size()) PEGASUS_ARRAY_T(x);
    _rep->size++;
}

#ifndef PEGASUS_ARRAY_T
template<class PEGASUS_ARRAY_T>
#else
PEGASUS_TEMPLATE_SPECIALIZATION
#endif
void Array<PEGASUS_ARRAY_T>::append(const PEGASUS_ARRAY_T* x, Uint32 size)
{
    reserve(this->size() + size);
#if defined(PEGASUS_PLATFORM_HPUX_PARISC_ACC) || defined(PEGASUS_PLATFORM_LINUX_GENERIC_GNU)
    CopyToRaw<PEGASUS_ARRAY_T>(_data() + this->size(), x, size);
#else
    CopyToRaw(_data() + this->size(), x, size);
#endif
    _rep->size += size;
}

#ifndef PEGASUS_ARRAY_T
template<class PEGASUS_ARRAY_T>
#else
PEGASUS_TEMPLATE_SPECIALIZATION
#endif
void Array<PEGASUS_ARRAY_T>::appendArray(const Array<PEGASUS_ARRAY_T>& x)
{
    append(x.getData(), x.size());
}

#ifndef PEGASUS_ARRAY_T
template<class PEGASUS_ARRAY_T>
#else
PEGASUS_TEMPLATE_SPECIALIZATION
#endif
void Array<PEGASUS_ARRAY_T>::prepend(const PEGASUS_ARRAY_T& x)
{
    reserve(size() + 1);
    memmove(_data() + 1, _data(), sizeof(PEGASUS_ARRAY_T) * size());
    new(_data()) PEGASUS_ARRAY_T(x);
    _rep->size++;
}

#ifndef PEGASUS_ARRAY_T
template<class PEGASUS_ARRAY_T>
#else
PEGASUS_TEMPLATE_SPECIALIZATION
#endif
void Array<PEGASUS_ARRAY_T>::prepend(const PEGASUS_ARRAY_T* x, Uint32 size)
{
    reserve(this->size() + size);
    memmove(_data() + size, _data(), sizeof(PEGASUS_ARRAY_T) * this->size());
#if defined(PEGASUS_PLATFORM_HPUX_PARISC_ACC) || defined(PEGASUS_PLATFORM_LINUX_GENERIC_GNU)
    CopyToRaw<PEGASUS_ARRAY_T>(_data(), x, size);
#else
    CopyToRaw(_data(), x, size);
#endif
    _rep->size += size;
}

#ifndef PEGASUS_ARRAY_T
template<class PEGASUS_ARRAY_T>
#else
PEGASUS_TEMPLATE_SPECIALIZATION
#endif
void Array<PEGASUS_ARRAY_T>::insert(Uint32 pos, const PEGASUS_ARRAY_T& x)
{
    if (pos > size())
	ThrowOutOfBounds();

    reserve(size() + 1);

    Uint32 n = size() - pos;

    if (n)
	memmove(_data() + pos + 1, _data() + pos, sizeof(PEGASUS_ARRAY_T) * n);

    new(_data() + pos) PEGASUS_ARRAY_T(x);
    _rep->size++;
}

#ifndef PEGASUS_ARRAY_T
template<class PEGASUS_ARRAY_T>
#else
PEGASUS_TEMPLATE_SPECIALIZATION
#endif
void Array<PEGASUS_ARRAY_T>::insert(Uint32 pos, const PEGASUS_ARRAY_T* x, Uint32 size)
{
    if (pos + size > this->size())
	ThrowOutOfBounds();

    reserve(this->size() + size);

    Uint32 n = this->size() - pos;

    if (n)
	memmove(
	    _data() + pos + size, _data() + pos, sizeof(PEGASUS_ARRAY_T) * n);

#if defined(PEGASUS_PLATFORM_HPUX_PARISC_ACC) || defined(PEGASUS_PLATFORM_LINUX_GENERIC_GNU)
    CopyToRaw<PEGASUS_ARRAY_T>(_data() + pos, x, size);
#else
    CopyToRaw(_data() + pos, x, size);
#endif
    _rep->size += size;
}

#ifndef PEGASUS_ARRAY_T
template<class PEGASUS_ARRAY_T>
#else
PEGASUS_TEMPLATE_SPECIALIZATION
#endif
void Array<PEGASUS_ARRAY_T>::remove(Uint32 pos)
{
    if (pos >= this->size())
	ThrowOutOfBounds();

#if defined(PEGASUS_PLATFORM_HPUX_PARISC_ACC) || defined(PEGASUS_PLATFORM_LINUX_GENERIC_GNU)
    Destroy<PEGASUS_ARRAY_T>(_data() + pos);
#else
    Destroy(_data() + pos);
#endif

    Uint32 rem = this->size() - pos - 1;

    if (rem)
	memmove(_data() + pos, _data() + pos + 1, sizeof(PEGASUS_ARRAY_T) * rem);

    _rep->size--;
}

#ifndef PEGASUS_ARRAY_T
template<class PEGASUS_ARRAY_T>
#else
PEGASUS_TEMPLATE_SPECIALIZATION
#endif
void Array<PEGASUS_ARRAY_T>::remove(Uint32 pos, Uint32 size)
{
    if (pos + size > this->size())
	ThrowOutOfBounds();

#if defined(PEGASUS_PLATFORM_HPUX_PARISC_ACC) || defined(PEGASUS_PLATFORM_LINUX_GENERIC_GNU)
    Destroy<PEGASUS_ARRAY_T>(_data() + pos, size);
#else
    Destroy(_data() + pos, size);
#endif

    Uint32 rem = this->size() - (pos + size);

    if (rem)
	memmove(
	    _data() + pos, _data() + pos + size, sizeof(PEGASUS_ARRAY_T) * rem);

    _rep->size -= size;
}

#ifdef PEGASUS_HAS_EBCDIC

#ifndef PEGASUS_ARRAY_T
template<class PEGASUS_ARRAY_T>
#else
PEGASUS_TEMPLATE_SPECIALIZATION
#endif
void Array<PEGASUS_ARRAY_T>::etoa()
{
#if PEGASUS_ARRAY_T == Sint8
    __etoa_l((char *)_data(),_rep->size);
#endif
}

#ifndef PEGASUS_ARRAY_T
template<class PEGASUS_ARRAY_T>
#else
PEGASUS_TEMPLATE_SPECIALIZATION
#endif
void Array<PEGASUS_ARRAY_T>::atoe()
{
#if PEGASUS_ARRAY_T == Sint8
    __atoe_l((char *)_data(),_rep->size);
#endif
}

#endif

#ifndef PEGASUS_ARRAY_T
template<class PEGASUS_ARRAY_T>
#else
PEGASUS_TEMPLATE_SPECIALIZATION
#endif
PEGASUS_ARRAY_T* Array<PEGASUS_ARRAY_T>::begin()
{
    return _rep->data();
}

#ifndef PEGASUS_ARRAY_T
template<class PEGASUS_ARRAY_T>
#else
PEGASUS_TEMPLATE_SPECIALIZATION
#endif
PEGASUS_ARRAY_T* Array<PEGASUS_ARRAY_T>::end()
{
    return _rep->data() + size();
}

#ifndef PEGASUS_ARRAY_T
template<class PEGASUS_ARRAY_T>
#else
PEGASUS_TEMPLATE_SPECIALIZATION
#endif
const PEGASUS_ARRAY_T* Array<PEGASUS_ARRAY_T>::begin() const
{
    return getData();
}

#ifndef PEGASUS_ARRAY_T
template<class PEGASUS_ARRAY_T>
#else
PEGASUS_TEMPLATE_SPECIALIZATION
#endif
const PEGASUS_ARRAY_T* Array<PEGASUS_ARRAY_T>::end() const
{
    return getData() + size();
}

#ifndef PEGASUS_ARRAY_T
template<class PEGASUS_ARRAY_T>
#else
PEGASUS_TEMPLATE_SPECIALIZATION
#endif
void Array<PEGASUS_ARRAY_T>::set(ArrayRep<PEGASUS_ARRAY_T>* rep)
{
    if (_rep != rep)
    {
        Rep::destroy(_rep);
	_rep = rep->clone();
    }
}

#ifndef PEGASUS_ARRAY_T
template<class PEGASUS_ARRAY_T>
#else
PEGASUS_TEMPLATE_SPECIALIZATION
#endif
PEGASUS_ARRAY_T* Array<PEGASUS_ARRAY_T>::_data() const
{
    return _rep->data();
}


#ifndef PEGASUS_ARRAY_T
template<class PEGASUS_ARRAY_T>
#else
PEGASUS_TEMPLATE_SPECIALIZATION
#endif
Boolean operator==(
    const Array<PEGASUS_ARRAY_T>& x,
    const Array<PEGASUS_ARRAY_T>& y)
{
    return Equal(x, y);
}

#ifndef PEGASUS_ARRAY_T
template<class PEGASUS_ARRAY_T>
#else
PEGASUS_TEMPLATE_SPECIALIZATION
#endif
PEGASUS_ARRAY_T& Array<PEGASUS_ARRAY_T>::operator[](Uint32 pos)
{
    if (pos >= size())
        ThrowOutOfBounds();

    return _rep->data()[pos];
}

#ifndef PEGASUS_ARRAY_T
template<class PEGASUS_ARRAY_T>
#else
PEGASUS_TEMPLATE_SPECIALIZATION
#endif
const PEGASUS_ARRAY_T& Array<PEGASUS_ARRAY_T>::operator[](
    Uint32 pos) const
{
    if (pos >= size())
        ThrowOutOfBounds();

    return _rep->data()[pos];
}

#endif /*defined(PEGASUS_EXPLICIT_INSTANTIATION) || !defined(PEGASUS_ARRAY_T)*/
