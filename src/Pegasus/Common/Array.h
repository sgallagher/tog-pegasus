//%/////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001, 2002 BMC Software, Hewlett-Packard Company, IBM,
// The Open Group, Tivoli Systems
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

#ifndef Pegasus_Array_h
#define Pegasus_Array_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/Char16.h>
#include <Pegasus/Common/String.h>

PEGASUS_NAMESPACE_BEGIN

#include <Pegasus/Common/ArrayInter.h>
#ifdef PEGASUS_INTERNALONLY
#include <Pegasus/Common/ArrayImpl.h>
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

#define PEGASUS_ARRAY_T String
#include <Pegasus/Common/ArrayInter.h>
#undef PEGASUS_ARRAY_T


template<class PEGASUS_ARRAY_T>
Boolean operator==(
    const Array<PEGASUS_ARRAY_T>& x,
    const Array<PEGASUS_ARRAY_T>& y)
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
Boolean Contains(const Array<PEGASUS_ARRAY_T>& a, const PEGASUS_ARRAY_T& x)
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

#if 0
// Determine need for these functions
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

PEGASUS_NAMESPACE_END

#endif /* Pegasus_Array_h */
