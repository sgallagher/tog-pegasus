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
//%/////////////////////////////////////////////////////////////////////////////

#ifndef _Pegasus_StringRep_h
#define _Pegasus_StringRep_h

#include <Pegasus/Common/Atomic.h>

PEGASUS_NAMESPACE_BEGIN

struct StringRep
{
    StringRep();

    ~StringRep();

    static StringRep* alloc(size_t cap);

    static void free(StringRep* rep);

    static StringRep* create(const Uint16* data, size_t size);

    static StringRep* create(const char* data, size_t size);

    static StringRep* createASCII7(const char* data, size_t size);

    static StringRep* copy_on_write(StringRep* rep);

    static Uint32 length(const Uint16* str);

    static void ref(const StringRep* rep);

    static void unref(const StringRep* rep);

    static StringRep _empty_rep;

    size_t size;
    size_t cap;
    Atomic refs;
    Uint16 data[1];
};

inline void StringRep::free(StringRep* rep)
{
    Atomic_destroy(&rep->refs);
    ::operator delete(rep);
}

inline StringRep::StringRep() : size(0), cap(0)
{
    // Only called on _empty_rep.
    Atomic_create(&refs, 99);
    data[0] = 0;
}

inline StringRep::~StringRep()
{
    // Only called on _empty_rep.
    Atomic_destroy(&refs);
}

inline void StringRep::ref(const StringRep* rep)
{
    if (rep != &StringRep::_empty_rep)
	Atomic_inc(&((StringRep*)rep)->refs);
}

inline void StringRep::unref(const StringRep* rep)
{
    if (rep != &StringRep::_empty_rep && 
	Atomic_dec_and_test(&((StringRep*)rep)->refs))
	StringRep::free((StringRep*)rep);
}

PEGASUS_COMMON_LINKAGE void String_throw_out_of_bounds();

#ifdef PEGASUS_STRING_NO_THROW
# define _check_bounds(ARG1, ARG2) /* empty */
#else
inline void _check_bounds(size_t index, size_t size)
{
    if (index > size)
	String_throw_out_of_bounds();
}
#endif

PEGASUS_NAMESPACE_END

#endif /* _Pegasus_StringRep_h */
