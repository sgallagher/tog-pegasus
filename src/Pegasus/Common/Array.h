//%/////////////////////////////////////////////////////////////////////////////
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
//==============================================================================
//
// Author: Mike Brasher (mbrasher@bmc.com)
//
// Modified By:
//
//%/////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_Array_h
#define Pegasus_Array_h

#include <new>
#include <cstring>
#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/CIMType.h>
#include <Pegasus/Common/Char16.h>
#include <Pegasus/Common/Memory.h>

PEGASUS_NAMESPACE_BEGIN

/*  ArrayRep<T>
    The ArrayRep object represents the array size, capacity, reference
    count and elements in one contiguous chunk of memory. The elements
    follow immediately after the end of the ArrayRep structure in memory.
    The union is used to force 64 bit alignment of these elements. This is
    a private class and should not be accessed directly by the user.
*/
template<class T>
struct ArrayRep
{
    Uint32 size;
    Uint32 capacity;

    /* This union forces the first element (which follows this structure
	in memory) to be aligned on a 64 bit boundary. It is a requirement
	that even an array of characters be aligned for any purpose (as malloc()
	does). That way, arrays of characters can be used for alignement
	sensitive data.
    */
    union
    {
	Uint32 ref;
	Uint64 alignment;
    };

    // Obtains a pointer to the first element in the array.
    T* data() { return (T*)(this + 1); }

    // Same as method above but returns a constant pointer.
    const T* data() const { return (const T*)(this + 1); }

    /* Creates a clone of the current object and sets the reference
	count to one.
    */
    ArrayRep<T>* clone() const;

    /* Create and initialize a ArrayRep instance. Set the reference count
	to one so the caller need not bother incrementing it. Note that the
	memory for the elements is unitialized and must be initialized by
	the caller.
    */

    static ArrayRep<T>* create(Uint32 size);

    // Increments the reference count of this object.
    static void inc(const ArrayRep<T>* rep_);


    /* Decrements the reference count of this object. If the reference count
	falls to zero, the object is disposed of.
    */
    static void dec(const ArrayRep<T>* rep_);

    /* Gets a pointer to a single instance which is created for each class
	to represent an empty array (zero-size).
    */
    static ArrayRep<T>* getNullRep();
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

    ArrayRep<T>* rep =
	(ArrayRep<T>*)operator new(sizeof(ArrayRep<T>) + sizeof(T) * capacity);

    rep->size = size;
    rep->capacity = capacity;
    rep->ref = 1;

    return rep;
}

template<class T>
void ArrayRep<T>::inc(const ArrayRep<T>* rep)
{
    if (rep)
	((ArrayRep<T>*)rep)->ref++;
}

template<class T>
void ArrayRep<T>::dec(const ArrayRep<T>* rep_)
{
    ArrayRep<T>* rep = (ArrayRep<T>*)rep_;

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
	nullRep = ArrayRep<T>::create(0);

	// Increment reference count here so that it will be one
	// greater so that it will never be deleted.
	ArrayRep<T>::inc(nullRep);
    }

    // Increment reference count on behalf of caller.
    ArrayRep<T>::inc(nullRep);
    return nullRep;
}

class CIMValue;

PEGASUS_COMMON_LINKAGE void ThrowOutOfBounds();

class CIMValue;

/** Array Class.
    This clas is used to represent arrays of intrinsic data types in CIM. And
    it is also used by the implementation to represent arrays of other kinds of
    objects (e.g., it is used to implement the String class). However, the user
    will only use it directly to manipulate arrays of CIM data types.
*/
template<class T>
class Array
{
public:

    /// Default constructor.
    Array();

    /// Copy Constructor.
    Array(const Array<T>& x);

    /** Constructs an array with size elements. The elements are
	initialized with their copy constructor.
	@param size defines the number of elements
    */
    Array(Uint32 size);

    /** Constructs an array with size elements. The elements are
    	initialized with x.
    */
    Array(Uint32 size, const T& x);

    /** Constructs an array with size elements. The values come from
	the items pointer.
    */
    Array(const T* items, Uint32 size);

    Array(ArrayRep<T>* rep)
    {
	Rep::inc(_rep = rep);
    }

    /// Destructs the objects, freeing any resources.
    ~Array();

    /// Assignment operator.
    Array<T>& operator=(const Array<T>& x);

    /** Clears the contents of the array. After calling this, size()
	returns zero.
    */
    void clear();

    /** Reserves memory for capacity elements. Notice that this does not
	change the size of the array (size() returns what it did before).
	If the capacity of the array is already greater or equal to the
	capacity argument, this method has no effect. After calling reserve(),
	getCapacity() returns a value which is greater or equal to the
	capacity argument.
	@param capacity defines the size that is to be reserved
    */
    void reserve(Uint32 capacity)
    {
	if (capacity > _rep->capacity)
	    _reserveAux(capacity);
    }

    /** Make the size of the array grow by size elements. Thenew size will 
	be size() + size. The new elements (there are size of them) are
	initialized with x.
	@param size defines the number of elements by which the array is to
	grow.
    */
    void grow(Uint32 size, const T& x);

    /// Swaps the contents of two arrays.
    void swap(Array<T>& x);

    /** Returns the number of elements in the array.
    @return The number of elements in the array.
    */
    Uint32 size() const { return _rep->size; }

    /// Returns the capacity of the array.
    Uint32 getCapacity() const { return _rep->capacity; }

    /// Returns a pointer to the first element of the array.
    const T* getData() const
    {
	return _rep->data();
    }

    /** Returns the element at the index given by the pos argument.
	@return A reference to the elementdefined by index so that it may be
	modified.
    */
    T& operator[](Uint32 pos);

    /** Same as the above method except that this is the version called
	on const arrays. The return value cannot be modified since it
	is constant.
    */
    const T& operator[](Uint32 pos) const;

    /** Appends an element to the end of the array. This increases the size
	of the array by one.
	@param Element to append.
    */
    void append(const T& x);

    /// Appends size elements at x to the end of this array.
    void append(const T* x, Uint32 size);

    /** Appends one array to another. The size of this array grows by the
	size of the other.
	@param Array to append.
    */
    void appendArray(const Array<T>& x)
    {
	append(x.getData(), x.size());
    }

    /** Appends one element to the beginning of the array. This increases
	the size by one.
	@param Element to prepend.
    */
    void prepend(const T& x);

    /** Appends size elements to the array starting at the memory address
	given by x. The array grows by size elements.
    */
    void prepend(const T* x, Uint32 size);

    /** Inserts the element at the given index in the array. Subsequent
	elements are moved down. The size of the array grows by one.
    */
    void insert(Uint32 pos, const T& x);

    /** Inserts size elements at x into the array at the given position.
	Subsequent elements are moved down. The size of the array grows
	by size elements.
    */
    void insert(Uint32 pos, const T* x, Uint32 size);

    /** Removes the element at the given position from the array. The
	size of the array shrinks by one.
    */
    void remove(Uint32 pos);

    /** Removes size elements starting at the given position. The size of
	the array shrinks by size elements.
    */
    void remove(Uint32 pos, Uint32 size);

    typedef T* iterator;

    typedef const T* const_iterator;

    iterator begin() { return _rep->data(); }

    iterator end() { return _rep->data() + size(); }

    const_iterator begin() const { return getData(); }

    const_iterator end() const { return getData() + size(); }

    Boolean operator==(const Array<T>& x) const;

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

    friend CIMValue;
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
    Uint32 size = this->size();
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
	new(p++) T(x);

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
    if (pos >= size())
	ThrowOutOfBounds();

    _copyOnWrite();

    return _rep->data()[pos];
}

template<class T>
inline const T& Array<T>::operator[](Uint32 pos) const
{
    if (pos >= size())
	ThrowOutOfBounds();

    return _rep->data()[pos];
}

template<class T>
void Array<T>::append(const T& x)
{
    reserve(size() + 1);
    _copyOnWrite();
    new (_data() + size()) T(x);
    _rep->size++;
}

template<class T>
void Array<T>::append(const T* x, Uint32 size)
{
    reserve(this->size() + size);
    _copyOnWrite();
    CopyToRaw(_data() + this->size(), x, size);
    _rep->size += size;
}

template<class T>
void Array<T>::prepend(const T& x)
{
    reserve(size() + 1);
    _copyOnWrite();
    memmove(_data() + 1, _data(), sizeof(T) * size());
    new(_data()) T(x);
    _rep->size++;
}

template<class T>
void Array<T>::prepend(const T* x, Uint32 size)
{
    reserve(this->size() + size);
    _copyOnWrite();
    memmove(_data() + size, _data(), sizeof(T) * this->size());
    CopyToRaw(_data(), x, size);
    _rep->size += size;
}

template<class T>
void Array<T>::insert(Uint32 pos, const T& x)
{
    if (pos > size())
	ThrowOutOfBounds();

    reserve(size() + 1);
    _copyOnWrite();

    Uint32 n = size() - pos;

    if (n)
	memmove(_data() + pos + 1, _data() + pos, sizeof(T) * n);

    new(_data() + pos) T(x);
    _rep->size++;
}

template<class T>
void Array<T>::insert(Uint32 pos, const T* x, Uint32 size)
{
    if (pos + size > this->size())
	ThrowOutOfBounds();

    reserve(this->size() + size);
    _copyOnWrite();

    Uint32 n = this->size() - pos;

    if (n)
	memmove(_data() + pos + size, _data() + pos, sizeof(T) * n);

    CopyToRaw(_data() + pos, x, size);
    _rep->size += size;
}

template<class T>
void Array<T>::remove(Uint32 pos)
{
    if (pos >= this->size())
	ThrowOutOfBounds();

    _copyOnWrite();

    Destroy(_data() + pos);

    Uint32 rem = this->size() - pos - 1;

    if (rem)
	memmove(_data() + pos, _data() + pos + 1, sizeof(T) * rem);

    _rep->size--;
}

template<class T>
void Array<T>::remove(Uint32 pos, Uint32 size)
{
    if (pos + size > this->size())
	ThrowOutOfBounds();

    _copyOnWrite();

    Destroy(_data() + pos, size);

    Uint32 rem = this->size() - (pos + size);

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
Boolean Equal(const Array<T>& x, const Array<T>& y)
{
    if (x.size() != y.size())
	return false;

    for (Uint32 i = 0, n = x.size(); i < n; i++)
    {
	if (!(x[i] == y[i]))
	    return false;
    }

    return true;
}

template<class T>
inline Boolean Array<T>::operator==(const Array<T>& x) const
{
    // ATTN: had to implement as a member. Ran into bug on AIX
    // where the non-member form was not found during compile.
    return Equal(*this, x);
}

template<class T>
void BubbleSort(Array<T>& x) 
{
    Uint32 n = x.size();

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
    for (Uint32 i = 0, n = x.size(); i < n; i++)
	cout << x[i] << endl;
}

PEGASUS_NAMESPACE_END

#endif /* Pegasus_Array_h */
