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
    T* data() { return (T*)(void*)(this + 1); }

    // Same as method above but returns a constant pointer.
    const T* data() const { return (const T*)(void*)(this + 1); }

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

#include <Pegasus/Common/ArrayInter.h>
#include <Pegasus/Common/ArrayImpl.h>

template<class PEGASUS_ARRAY_T>
Boolean Equal(const Array<PEGASUS_ARRAY_T>& x, const Array<PEGASUS_ARRAY_T>& y)
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

template<class PEGASUS_ARRAY_T>
void BubbleSort(Array<PEGASUS_ARRAY_T>& x) 
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
		PEGASUS_ARRAY_T t = x[j];
		x[j] = x[j+1];
		x[j+1] = t;
	    }
	}
    }
}

template<class PEGASUS_ARRAY_T>
void Print(Array<PEGASUS_ARRAY_T>& x)
{
    for (Uint32 i = 0, n = x.size(); i < n; i++)
	cout << x[i] << endl;
}

#define PEGASUS_ARRAY_T Boolean
#include <Pegasus/Common/ArrayInter.h>
#undef PEGASUS_ARRAY_T

#define PEGASUS_ARRAY_T Uint8
#include <Pegasus/Common/ArrayInter.h>
#undef PEGASUS_ARRAY_T

#define PEGASUS_ARRAY_T Sint8
#include <Pegasus/Common/ArrayInter.h>
#undef PEGASUS_ARRAY_T

#define PEGASUS_ARRAY_T Uint16
#include <Pegasus/Common/ArrayInter.h>
#undef PEGASUS_ARRAY_T

#define PEGASUS_ARRAY_T Sint16
#include <Pegasus/Common/ArrayInter.h>
#undef PEGASUS_ARRAY_T

#define PEGASUS_ARRAY_T Uint32
#include <Pegasus/Common/ArrayInter.h>
#undef PEGASUS_ARRAY_T

#define PEGASUS_ARRAY_T Sint32
#include <Pegasus/Common/ArrayInter.h>
#undef PEGASUS_ARRAY_T

#define PEGASUS_ARRAY_T Uint64
#include <Pegasus/Common/ArrayInter.h>
#undef PEGASUS_ARRAY_T

#define PEGASUS_ARRAY_T Sint64
#include <Pegasus/Common/ArrayInter.h>
#undef PEGASUS_ARRAY_T

#define PEGASUS_ARRAY_T Real32
#include <Pegasus/Common/ArrayInter.h>
#undef PEGASUS_ARRAY_T

#define PEGASUS_ARRAY_T Real64
#include <Pegasus/Common/ArrayInter.h>
#undef PEGASUS_ARRAY_T

#define PEGASUS_ARRAY_T Char16
#include <Pegasus/Common/ArrayInter.h>
#undef PEGASUS_ARRAY_T

typedef const char* ConstCharPtr;
#define PEGASUS_ARRAY_T ConstCharPtr
# include "ArrayInter.h"
#undef PEGASUS_ARRAY_T

typedef char* CharPtr;
#define PEGASUS_ARRAY_T CharPtr
# include "ArrayInter.h"
#undef PEGASUS_ARRAY_T

PEGASUS_NAMESPACE_END

#endif /* Pegasus_Array_h */
