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

#ifndef Pegasus_Array_h
#define Pegasus_Array_h

#include <Pegasus/Common/Config.h>
#include <new>
#include <cstring>
#ifdef PEGASUS_HAS_EBCDIC
#include <unistd.h>
#endif
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

    static ArrayRep<T>* PEGASUS_STATIC_CDECL create(Uint32 size);

    // Increments the reference count of this object.
    static void PEGASUS_STATIC_CDECL inc(const ArrayRep<T>* rep_);


    /* Decrements the reference count of this object. If the reference count
	falls to zero, the object is disposed of.
    */
    static void PEGASUS_STATIC_CDECL dec(const ArrayRep<T>* rep_);

    /* Gets a pointer to a single instance which is created for each class
	to represent an empty array (zero-size).
    */
    static ArrayRep<T>* PEGASUS_STATIC_CDECL getNullRep();
};

template<class T>
ArrayRep<T>* ArrayRep<T>::clone() const
{
    ArrayRep<T>* rep = ArrayRep<T>::create(capacity);
    rep->size = size;
#if defined(PEGASUS_PLATFORM_HPUX_PARISC_ACC) || defined(PEGASUS_PLATFORM_LINUX_GENERIC_GNU)
    CopyToRaw<T>(rep->data(), data(), size);
#else
    CopyToRaw(rep->data(), data(), size);
#endif
    return rep;
}

template<class T>
ArrayRep<T>* PEGASUS_STATIC_CDECL ArrayRep<T>::create(Uint32 size)
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
void PEGASUS_STATIC_CDECL ArrayRep<T>::inc(const ArrayRep<T>* rep)
{
    // REVIEW: Need locked increment here so we can shared among threads.

    if (rep)
	((ArrayRep<T>*)rep)->ref++;
}

template<class T>
void PEGASUS_STATIC_CDECL ArrayRep<T>::dec(const ArrayRep<T>* rep_)
{
    // REVIEW: Need locked decrement here so we can shared among threads.

    ArrayRep<T>* rep = (ArrayRep<T>*)rep_;

    if (rep && --rep->ref == 0)
    {
// ATTN-RK-P1-20020509: PLATFORM PORT: This change fixes memory leaks for me.
// Should you make these changes on your platform?  (See also ArrayImpl.h)
#if defined(PEGASUS_PLATFORM_HPUX_PARISC_ACC) || defined(PEGASUS_PLATFORM_LINUX_GENERIC_GNU)
	Destroy<T>(rep->data(), rep->size);
#else
	Destroy(rep->data(), rep->size);
#endif

	// ATTN: take this out later:
	//memset(rep->data(), 0xaa, rep->size * sizeof(T));

	operator delete(rep);
    }
}

template<class T>
ArrayRep<T>* PEGASUS_STATIC_CDECL ArrayRep<T>::getNullRep()
{
    static ArrayRep<T>* nullRep = 0;

    if (!nullRep)
    {
        // Create sets the reference count to 1 so that it will be one
        // greater than the callers ask for and so it will never be deleted.
	nullRep = ArrayRep<T>::create(0);
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

template<class T>
Boolean Contains(const Array<T>& a, const T& x)
{
    Uint32 n = a.size();

    for (Uint32 i = 0; i < n; i++)
    {
	if (a[i] == x)
	    return true;
    }

    return false;
}

#ifdef PEGASUS_INTERNALONLY
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
#endif

#if 0 // Determine need for these functions
template<class PEGASUS_ARRAY_T>
void Unique(Array<PEGASUS_ARRAY_T>& x) 
{
    Array<PEGASUS_ARRAY_T> result;

    for (Uint32 i = 0, n = x.size(); i < n; i++)
    {
	if (i == 0 || x[i] != x[i-1])
	    result.append(x[i]);
    }

    x.swap(result);
}

template<class PEGASUS_ARRAY_T>
void Print(Array<PEGASUS_ARRAY_T>& x)
{
    for (Uint32 i = 0, n = x.size(); i < n; i++)
	PEGASUS_STD(cout) << x[i] << PEGASUS_STD(endl);
}
#endif

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
