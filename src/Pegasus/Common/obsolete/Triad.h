//BEGIN_LICENSE
//
// Copyright (c) 2000 The Open Group, BMC Software
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
// Author: Mike Brasher
//
// $Log: Triad.h,v $
// Revision 1.1  2001/01/14 19:53:42  mike
// Initial revision
//
//
//END_HISTORY

////////////////////////////////////////////////////////////////////////////////
//
// Triad.h
//
//	These classes provide simple representation of a template structures
//	three elements. Constructors are provided for initializing the
//	elements.
//
////////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_Triad_h
#define Pegasus_Triad_h

#include <Pegasus/Common/Config.h>

PEGASUS_NAMESPACE_BEGIN

template<class T, class U, class V>
struct Triad
{
    T first;
    U second;
    V third;

    Triad() : first(), second(), third() 
    { 

    }

    Triad(const Triad<T, U, V>& x)
	: first(x.first), second(x.second), third(x.third)
    {

    }

    Triad(const T& first_, const U& second_, const V& third_) 
	: first(first_), second(second_), third(third_)
    {

    }

    Triad& operator=(const Triad& x)
    {
	first = x.first;
	second = x.second;
	third = x.third;
	return *this;
    }
};

PEGASUS_NAMESPACE_END

#endif /* Pegasus_Triad_h */
