//BEGIN_LICENSE
//
// Copyright (c) 2000 The Open Group, BMC Software, Tivoli Systems, IBM
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
// Author:
//
// $Log: Array.h,v $
// Revision 1.1  2001/01/14 19:50:34  mike
// Initial revision
//
//
//END_HISTORY

///////////////////////////////////////////////////////////////////////////////
//
// Array.h
//
//	Simple array template implementation.
//
////////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_Array_h
#define Pegasus_Array_h

#include <new>
#include <cstring>
#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/Type.h>
#include <Pegasus/Common/Char16.h>
#include <Pegasus/Common/Memory.h>

PEGASUS_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////
//
// ArrayRep<T>
//
// 	The ArrayRep object represents the array size, capacity, reference
//	count and elements in one contiguous chunk of memory. The elements 
//	follow immediately after the end of the ArrayRep structure in memory. 
//	The union is used to force 64 bit alignment of these elements.
//
////////////////////////////////////////////////////////////////////////////////

template<class T>
struct ArrayRep
{
    Uint32 size;
    Uint32 capacity;

    // This union forces the first element (which follows this structure
    // in memory) to be aligned on a 64 bit boundary. It is a requirement
    // that even an array of characters be aligned for any purpose (as malloc()
    // does). That way, arrays of characters can be used for alignement
    // sensitive data.

    union
    {
	Uint32 ref;
	Uint64 alignment;
    };

    T* data() { return (T*)(this + 1); }

    const T* data() const { return (const T*)(this + 1); }

    ArrayRep* clone() const;

    // Create and initialize a ArrayRep instance. Set the reference count to
    // one so the caller need not bother incrementing it. Note that the
    // memory for the elements is unitialized and must be initialized by
    // the caller.

    static ArrayRep* create(Uint32 size);

    static void inc(const ArrayRep* rep_);

    static void dec(const ArrayRep* rep_);

    static ArrayRep* getNullRep();
};

template<class T>
ArrayRep<T>* ArrayRep<T>::clone() const
{
    ArrayRep<T>* rep = ArrayRep<T>::create(capacity);
    rep->size = size;
    CopyToRaw(rep->data(), data(), size);
    return rep;
}

template<class T>
ArrayRep<T>* ArrayRep<T>::create(Uint32 size)
{
    // Calcultate capacity (size rounded to the next power of two).

    Uint32 capacity = 8;

    while (capacity < size)
	capacity <<= 1;

    // Create object:

    ArrayRep* rep = 
	(ArrayRep*)operator new(sizeof(ArrayRep) + sizeof(T) * capacity);

    rep->size = size;
    rep->capacity = capacity;
    rep->ref = 1;

    return rep;
}

template<class T>
void ArrayRep<T>::inc(const ArrayRep<T>* rep)
{
    if (rep)
	((ArrayRep*)rep)->ref++;
}

template<class T>
void ArrayRep<T>::dec(const ArrayRep<T>* rep_)
{
    ArrayRep* rep = (ArrayRep*)rep_;

    if (rep && --rep->ref == 0)
    {
	Destroy(rep->data(), rep->size);
	operator delete(rep);
    }
}

template<class T>
ArrayRep<T>* ArrayRep<T>::getNullRep()
{
    static ArrayRep<T>* nullRep = 0;

    if (!nullRep)
    {
	nullRep = ArrayRep::create(0);

	// Increment reference count here so that it will be one
	// greater so that it will never be deleted.
	ArrayRep<T>::inc(nullRep);
    }

    // Increment reference count on behalf of caller.
    ArrayRep<T>::inc(nullRep);
    return nullRep;
}

////////////////////////////////////////////////////////////////////////////////
//
// Array<T>
//
////////////////////////////////////////////////////////////////////////////////

class Value;

PEGASUS_COMMON_LINKAGE void ThrowOutOfBounds();

class Value;

template<class T>
class Array
{
public:

    Array();

    Array(const Array<T>& x);

    Array(Uint32 size);

    Array(Uint32 size, const T& x);

    Array(const T* items, Uint32 size);

    Array(ArrayRep<T>* rep)
    {
	Rep::inc(_rep = rep);
    }

    ~Array();

    Array<T>& operator=(const Array<T>& x);

    void clear();

    void reserve(Uint32 capacity)
    {
	if (capacity > _rep->capacity)
	    _reserveAux(capacity);
    }

    void grow(Uint32 size, const T& x);

    void swap(Array<T>& x);

    Uint32 getSize() const { return _rep->size; }

    const T* getData() const { return _rep->data(); }

    T& operator[](Uint32 pos);

    const T& operator[](Uint32 pos) const;

    void append(const T& x);

    void append(const T* x, Uint32 size);

    void appendArray(const Array<T>& x)
    {
	append(x.getData(), x.getSize());
    }

    void prepend(const T& x);

    void prepend(const T* x, Uint32 size);

    void insert(Uint32 pos, const T& x);

    void insert(Uint32 pos, const T* x, Uint32 size);

    void remove(Uint32 pos);

    void remove(Uint32 pos, Uint32 size);

private:

    void set(ArrayRep<T>* rep)
    {
	if (_rep != rep)
	{
	    Rep::dec(_rep);
	    Rep::inc(_rep = rep);
	}
    }

    void _reserveAux(Uint32 capacity);

    T* _data() const { return _rep->data(); }

    typedef ArrayRep<T> Rep;

    void _copyOnWriteAux();

    void _copyOnWrite()
    {
	if (_rep->ref != 1)
	    _copyOnWriteAux();
    }

    Rep* _rep;

    friend Value;
};

template<class T>
Array<T>::Array()
{
    _rep = Rep::getNullRep();
}

template<class T>
Array<T>::Array(const Array<T>& x)
{
    Rep::inc(_rep = x._rep);
}

template<class T>
Array<T>::Array(Uint32 size)
{
    _rep = Rep::create(size);
    InitializeRaw(_rep->data(), size);
}

template<class T>
Array<T>::Array(Uint32 size, const T& x)
{
    _rep = Rep::create(size);

    T* data = _rep->data();
    
    while (size--)
	new(data++) T(x);
}

template<class T>
Array<T>::Array(const T* items, Uint32 size)
{
    _rep = Rep::create(size);
    CopyToRaw(_rep->data(), items, size);
}

template<class T>
Array<T>::~Array()
{
    Rep::dec(_rep);
}

template<class T>
Array<T>& Array<T>::operator=(const Array<T>& x)
{
    if (x._rep != _rep)
    {
	Rep::dec(_rep);
	Rep::inc(_rep = x._rep);
    }
    return *this;
}

template<class T>
void Array<T>::clear()
{
    Rep::dec(_rep);
    _rep = Rep::getNullRep();
}

template<class T>
void Array<T>::_reserveAux(Uint32 capacity)
{
    Uint32 size = getSize();
    Rep* rep = Rep::create(capacity);
    rep->size = size;
    CopyToRaw(rep->data(), _rep->data(), size);
    Rep::dec(_rep);
    _rep = rep;
}

template<class T>
void Array<T>::grow(Uint32 size, const T& x)
{
    Uint32 oldSize = _rep->size;
    reserve(oldSize + size);
    _copyOnWrite();

    T* p = _rep->data() + oldSize;
    Uint32 n = size;

    while (n--)
	new(p) T(x);

    _rep->size += size;
}

template<class T>
void Array<T>::swap(Array<T>& x)
{
    Rep* tmp = _rep;
    _rep = x._rep;
    x._rep = tmp;
}

template<class T>
inline T& Array<T>::operator[](Uint32 pos) 
{
    if (pos >= getSize())
	ThrowOutOfBounds();

    _copyOnWrite();

    return _rep->data()[pos];
}

template<class T>
inline const T& Array<T>::operator[](Uint32 pos) const
{
    if (pos >= getSize())
	ThrowOutOfBounds();

    return _rep->data()[pos];
}

template<class T>
void Array<T>::append(const T& x)
{
    reserve(getSize() + 1);
    _copyOnWrite();
    new (_data() + getSize()) T(x);
    _rep->size++;
}

template<class T>
void Array<T>::append(const T* x, Uint32 size)
{
    reserve(getSize() + size);
    _copyOnWrite();

    // ATTN: MSVC++ 5.0 you know what!
    CopyToRaw(_data() + getSize(), x, size);
    _rep->size += size;
}

template<class T>
void Array<T>::prepend(const T& x)
{
    reserve(getSize() + 1);
    _copyOnWrite();
    memmove(_data() + 1, _data(), sizeof(T) * getSize());
    new(_data()) T(x);
    _rep->size++;
}

template<class T>
void Array<T>::prepend(const T* x, Uint32 size)
{
    reserve(getSize() + size);
    _copyOnWrite();
    memmove(_data() + size, _data(), sizeof(T) * getSize());
    CopyToRaw(_data(), x, size);
    _rep->size += size;
}

template<class T>
void Array<T>::insert(Uint32 pos, const T& x)
{
    if (pos > getSize())
	ThrowOutOfBounds();

    reserve(getSize() + 1);
    _copyOnWrite();

    Uint32 n = getSize() - pos;

    if (n)
	memmove(_data() + pos + 1, _data() + pos, sizeof(T) * n);

    new(_data() + pos) T(x);
    _rep->size++;
}

template<class T>
void Array<T>::insert(Uint32 pos, const T* x, Uint32 size)
{
    if (pos + size > getSize())
	ThrowOutOfBounds();

    reserve(getSize() + size);
    _copyOnWrite();

    Uint32 n = getSize() - pos;

    if (n)
	memmove(_data() + pos + size, _data() + pos, sizeof(T) * n);

    CopyToRaw(_data() + pos, x, size);
    _rep->size += size;
}

template<class T>
void Array<T>::remove(Uint32 pos)
{
    if (pos >= getSize())
	ThrowOutOfBounds();

    _copyOnWrite();

    (_data() + pos)->~T();

    Uint32 rem = getSize() - pos - 1;

    if (rem)
	memmove(_data() + pos, _data() + pos + 1, sizeof(T) * rem);

    _rep->size--;
}

template<class T>
void Array<T>::remove(Uint32 pos, Uint32 size)
{
    if (pos + size > getSize())
	ThrowOutOfBounds();

    _copyOnWrite();

    Destroy(_data() + pos, size);

    Uint32 rem = getSize() - (pos + size);

    if (rem)
	memmove(_data() + pos, _data() + pos + size, sizeof(T) * rem);

    _rep->size -= size;
}

template<class T>
void Array<T>::_copyOnWriteAux()
{
    if (_rep->ref != 1)
    {
	Rep* rep = _rep->clone();
	Rep::dec(_rep);
	_rep = rep;
    }
}

template<class T>
Boolean operator==(const Array<T>& x, const Array<T>& y)
{
    if (x.getSize() != y.getSize())
	return false;

    for (Uint32 i = 0, n = x.getSize(); i < n; i++)
    {
	if (!(x[i] == y[i]))
	    return false;
    }

    return true;
}

template<class T>
void BubbleSort(Array<T>& x)
{
    Uint32 n = x.getSize();

    if (n < 2)
	return;

    for (Uint32 i = 0; i < n - 1; i++)
    {
	for (Uint32 j = 0; j < n - 1; j++)
	{
	    if (x[j] > x[j+1])
	    {
		T t = x[j];
		x[j] = x[j+1];
		x[j+1] = t;
	    }
	}
    }
}

template<class T>
void Print(Array<T>& x)
{
    for (Uint32 i = 0, n = x.getSize(); i < n; i++)
	cout << x[i] << endl;
}

PEGASUS_NAMESPACE_END

#endif /* Pegasus_Array_h */
