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
// $Log: Memory.h,v $
// Revision 1.1  2001/01/14 19:52:41  mike
// Initial revision
//
//
//END_HISTORY

////////////////////////////////////////////////////////////////////////////////
//
// Memory.h
//
//	This file contains assorted memory-oriented routines:
//
//	    Zeros(): fills memory with zeros.
//	    Destroy(): destructs multiple objects in contiguous memory.
//	    CopyToRaw(): copies multiple objects to raw memory.
//	    InitializeRaw(): default constructs mulitple object over raw memory.
//
//	Each of these is a template but specializations are provide for
//	efficiency (which in some cases removes uncessary loops).
//
////////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_Memory_h
#define Pegasus_Memory_h

#include <cstring>
#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/Type.h>
#include <Pegasus/Common/Char16.h>

PEGASUS_NAMESPACE_BEGIN

template<class T>
inline void Zeros(T* items, Uint32 size)
{
    memset(items, 0, sizeof(T) * size);
}

template<class T>
inline void Destroy(T* items, Uint32 size)
{
    while (size--)
	items++->~T();
}

inline void Destroy(Boolean* items, Uint32 size) { }
inline void Destroy(Uint8* items, Uint32 size) { }
inline void Destroy(Sint8* items, Uint32 size) { }
inline void Destroy(Uint16* items, Uint32 size) { }
inline void Destroy(Sint16* items, Uint32 size) { }
inline void Destroy(Uint32* items, Uint32 size) { }
inline void Destroy(Sint32* items, Uint32 size) { }
inline void Destroy(Uint64* items, Uint32 size) { }
inline void Destroy(Sint64* items, Uint32 size) { }
inline void Destroy(Real32* items, Uint32 size) { }
inline void Destroy(Real64* items, Uint32 size) { }
inline void Destroy(Char16* items, Uint32 size) { }

template<class T, class U>
inline void CopyToRaw(T* to, const U* from, Uint32 size)
{
    while (size--)
	new(to++) T(*from++);
}

inline void CopyToRaw(Boolean* to, const Boolean* from, Uint32 size)
{
    memcpy(to, from, sizeof(Boolean) * size);
}

inline void CopyToRaw(Uint8* to, const Uint8* from, Uint32 size)
{
    memcpy(to, from, sizeof(Uint8) * size);
}

inline void CopyToRaw(Sint8* to, const Sint8* from, Uint32 size)
{
    memcpy(to, from, sizeof(Sint8) * size);
}

inline void CopyToRaw(Uint16* to, const Uint16* from, Uint32 size)
{
    memcpy(to, from, sizeof(Uint16) * size);
}

inline void CopyToRaw(Sint16* to, const Sint16* from, Uint32 size)
{
    memcpy(to, from, sizeof(Sint16) * size);
}

inline void CopyToRaw(Uint32* to, const Uint32* from, Uint32 size)
{
    memcpy(to, from, sizeof(Uint32) * size);
}

inline void CopyToRaw(Sint32* to, const Sint32* from, Uint32 size)
{
    memcpy(to, from, sizeof(Sint32) * size);
}

inline void CopyToRaw(Uint64* to, const Uint64* from, Uint32 size)
{
    memcpy(to, from, sizeof(Uint64) * size);
}

inline void CopyToRaw(Sint64* to, const Sint64* from, Uint32 size)
{
    memcpy(to, from, sizeof(Sint64) * size);
}

inline void CopyToRaw(Real32* to, const Real32* from, Uint32 size)
{
    memcpy(to, from, sizeof(Real32) * size);
}

inline void CopyToRaw(Real64* to, const Real64* from, Uint32 size)
{
    memcpy(to, from, sizeof(Real64) * size);
}

inline void CopyToRaw(Char16* to, const Char16* from, Uint32 size)
{
    memcpy(to, from, sizeof(Char16) * size);
}

template<class T>
inline void InitializeRaw(T* items, Uint32 size)
{
    while (size--)
	items++->~T();
}

inline void InitializeRaw(Boolean* items, Uint32 size) { Zeros(items, size); }
inline void InitializeRaw(Uint8* items, Uint32 size) { Zeros(items, size); }
inline void InitializeRaw(Sint8* items, Uint32 size) { Zeros(items, size); }
inline void InitializeRaw(Uint16* items, Uint32 size) { Zeros(items, size); }
inline void InitializeRaw(Sint16* items, Uint32 size) { Zeros(items, size); }
inline void InitializeRaw(Uint32* items, Uint32 size) { Zeros(items, size); }
inline void InitializeRaw(Sint32* items, Uint32 size) { Zeros(items, size); }
inline void InitializeRaw(Uint64* items, Uint32 size) { Zeros(items, size); }
inline void InitializeRaw(Sint64* items, Uint32 size) { Zeros(items, size); }
inline void InitializeRaw(Real32* items, Uint32 size) { Zeros(items, size); }
inline void InitializeRaw(Real64* items, Uint32 size) { Zeros(items, size); }
inline void InitializeRaw(Char16* items, Uint32 size) { Zeros(items, size); }

PEGASUS_NAMESPACE_END

#endif /* Pegasus_Memory_h */
