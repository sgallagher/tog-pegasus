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

#ifndef Pegasus_ArrayRep_h
#define Pegasus_ArrayRep_h

#ifdef PEGASUS_OS_OS400
#include <stdlib.h>
#endif

#include <new>
#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/Memory.h>

PEGASUS_NAMESPACE_BEGIN

/*  ArrayRep<T>
    The ArrayRep object represents the array size, capacity,
    and elements in one contiguous chunk of memory. The elements
    follow immediately after the end of the ArrayRep structure in memory.
    The union is used to force 64 bit alignment of these elements. This is
    a private class and should not be accessed directly by the user.
*/
template<class T>
#ifdef PEGASUS_OS_OS400
/* For OS/400, this forces the alignment mentioned above. */
__align(16) 
#endif
struct ArrayRep
{
    Uint32 size;

    /* This union forces the first element (which follows this structure
        in memory) to be aligned on a 64 bit boundary. It is a requirement
        that even an array of characters be aligned for any purpose (as malloc()
        does). That way, arrays of characters can be used for alignement
        sensitive data.
    */
    union
    {
        Uint32 capacity;
        Uint64 alignment;
    };

    // Obtains a pointer to the first element in the array.
    T* data() { return (T*)(void*)(this + 1); }

    // Same as method above but returns a constant pointer.
    const T* data() const { return (const T*)(void*)(this + 1); }

    /* Creates a clone of the current */
    ArrayRep<T>* clone() const;

    /* Create and initialize a ArrayRep instance. Note that the
        memory for the elements is unitialized and must be initialized by
        the caller.
    */
    static ArrayRep<T>* PEGASUS_STATIC_CDECL create(Uint32 size);

    /* Disposes of the object.
    */
    static void PEGASUS_STATIC_CDECL destroy(ArrayRep<T>* rep);
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
ArrayRep<T>* PEGASUS_STATIC_CDECL ArrayRep<T>::create(Uint32 size)
{
    // Calculate capacity (size rounded to the next power of two).

    Uint32 initialCapacity = 8;

    while ((initialCapacity != 0) && (initialCapacity < size))
    {
        initialCapacity <<= 1;
    }

    // Test for Uint32 overflow in the capacity
    if (initialCapacity == 0)
    {
        initialCapacity = size;
    }

    // Test for Uint32 overflow in the memory allocation size
    if (initialCapacity > (Uint32(0xffffffff)-sizeof(ArrayRep<T>))/sizeof(T))
    {
        return 0;
    }

    // Create object:


    ArrayRep<T>* rep = (ArrayRep<T>*)operator new(
        sizeof(ArrayRep<T>) + sizeof(T) * initialCapacity);

    rep->size = size;
    rep->capacity = initialCapacity;

    return rep;
}

template<class T>
void PEGASUS_STATIC_CDECL ArrayRep<T>::destroy(ArrayRep<T>* rep)
{
    if (rep)
    {
        Destroy(rep->data(), rep->size);

        operator delete(rep);

        return;
    }
}

PEGASUS_NAMESPACE_END

#endif /* Pegasus_ArrayRep_h */
