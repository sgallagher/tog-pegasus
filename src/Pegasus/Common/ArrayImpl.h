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
// Modified By:
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
    _rep = Rep::getNullRep();
}

#ifndef PEGASUS_ARRAY_T
template<class PEGASUS_ARRAY_T>
#else
PEGASUS_TEMPLATE_SPECIALIZATION
#endif
Array<PEGASUS_ARRAY_T>::Array(const Array<PEGASUS_ARRAY_T>& x)
{
    Rep::inc(_rep = x._rep);
}

#ifndef PEGASUS_ARRAY_T
template<class PEGASUS_ARRAY_T>
#else
PEGASUS_TEMPLATE_SPECIALIZATION
#endif
Array<PEGASUS_ARRAY_T>::Array(Uint32 size)
{
    _rep = Rep::create(size);
    InitializeRaw(_rep->data(), size);
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
    CopyToRaw(_rep->data(), items, size);
}

#ifndef PEGASUS_ARRAY_T
template<class PEGASUS_ARRAY_T>
#else
PEGASUS_TEMPLATE_SPECIALIZATION
#endif
Array<PEGASUS_ARRAY_T>::~Array()
{
    Rep::dec(_rep);
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
	Rep::dec(_rep);
	Rep::inc(_rep = x._rep);
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
    Rep::dec(_rep);
    _rep = Rep::getNullRep();
}

#ifndef PEGASUS_ARRAY_T
template<class PEGASUS_ARRAY_T>
#else
PEGASUS_TEMPLATE_SPECIALIZATION
#endif
void Array<PEGASUS_ARRAY_T>::_reserveAux(Uint32 capacity)
{
    Uint32 size = this->size();
    Rep* rep = Rep::create(capacity);
    rep->size = size;
    CopyToRaw(rep->data(), _rep->data(), size);
    Rep::dec(_rep);
    _rep = rep;
}

#ifndef PEGASUS_ARRAY_T
template<class PEGASUS_ARRAY_T>
#else
PEGASUS_TEMPLATE_SPECIALIZATION
#endif
void Array<PEGASUS_ARRAY_T>::_copyOnWriteAux()
{
    if (_rep->ref != 1)
    {
	Rep* rep = _rep->clone();
	Rep::dec(_rep);
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
    _copyOnWrite();

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
void Array<PEGASUS_ARRAY_T>::append(const PEGASUS_ARRAY_T& x)
{
    reserve(size() + 1);
    _copyOnWrite();
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
    _copyOnWrite();
    CopyToRaw(_data() + this->size(), x, size);
    _rep->size += size;
}

#ifndef PEGASUS_ARRAY_T
template<class PEGASUS_ARRAY_T>
#else
PEGASUS_TEMPLATE_SPECIALIZATION
#endif
void Array<PEGASUS_ARRAY_T>::prepend(const PEGASUS_ARRAY_T& x)
{
    reserve(size() + 1);
    _copyOnWrite();
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
    _copyOnWrite();
    memmove(_data() + size, _data(), sizeof(PEGASUS_ARRAY_T) * this->size());
    CopyToRaw(_data(), x, size);
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
    _copyOnWrite();

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
    _copyOnWrite();

    Uint32 n = this->size() - pos;

    if (n)
	memmove(
	    _data() + pos + size, _data() + pos, sizeof(PEGASUS_ARRAY_T) * n);

    CopyToRaw(_data() + pos, x, size);
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

    _copyOnWrite();

    Destroy(_data() + pos);

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

    _copyOnWrite();

    Destroy(_data() + pos, size);

    Uint32 rem = this->size() - (pos + size);

    if (rem)
	memmove(
	    _data() + pos, _data() + pos + size, sizeof(PEGASUS_ARRAY_T) * rem);

    _rep->size -= size;
}

#endif /*defined(PEGASUS_EXPLICIT_INSTANTIATION) || !defined(PEGASUS_ARRAY_T)*/
