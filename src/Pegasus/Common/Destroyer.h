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
// $Log: Destroyer.h,v $
// Revision 1.1  2001/01/14 19:51:32  mike
// Initial revision
//
//
//END_HISTORY

////////////////////////////////////////////////////////////////////////////////
//
// Destroyer.h
//
//	This class provides a convenient way of disposing of a heap object.
//	It automatically deletes the enclosed pointer on destruction. For
//	example:
//
//	    {
//		A* a = new A;
//		Destroyer<A> dummy = a;
//
//	    }
//
//	When the destroyer object destructs, it frees the instance of A.
//	This is particularly useful when a function has multiple returns.
//
//	There are two classes here: Destroyer<> and ArrayDestroyer<>. The
//	ArrayDestroyer<> class is used when a pointer must be deleted using 
//	the array form as shown below:
//
//	    delete [] ptr;
//
////////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_Destroyer_h
#define Pegasus_Destroyer_h

#include <Pegasus/Common/Config.h>

PEGASUS_NAMESPACE_BEGIN

template<class T>
class Destroyer
{
public:

    Destroyer(T* ptr) : _ptr(ptr) { }

    ~Destroyer() { delete _ptr; }

    T* getPointer() { return _ptr; }

private:

    Destroyer(const Destroyer&) { }

    Destroyer& operator=(const Destroyer&) { return *this; }

    T* _ptr;
};

template<class T>
class ArrayDestroyer
{
public:

    ArrayDestroyer(T* ptr) : _ptr(ptr) { }

    ~ArrayDestroyer() { delete [] _ptr; }

    T* getPointer() { return _ptr; }

private:

    ArrayDestroyer(const ArrayDestroyer&) { }

    ArrayDestroyer& operator=(const ArrayDestroyer&) { return *this; }

    T* _ptr;
};

PEGASUS_NAMESPACE_END

#endif /* Pegasus_Destroyer_h */
