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
// $Log: Formatter.h,v $
// Revision 1.1.1.1  2001/01/14 19:53:37  mike
// Pegasus import
//
//
//END_HISTORY

////////////////////////////////////////////////////////////////////////////////
//
// Formatter.h
//
////////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_Formatter_h
#define Pegasus_Formatter_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/Array.h>
#include <Pegasus/Common/String.h>

PEGASUS_NAMESPACE_BEGIN

class Inserter
{
public:

    virtual void insert(Array<Sint8>& out) const = 0;
};

class NullIns : public Inserter
{
public:

    virtual void insert(Array<Sint8>& out) const { }
};

class CStrIns : public Inserter
{
public:

    CStrIns(const char* str) : _str(str) { }

    virtual void insert(Array<Sint8>& out) const
    { 
	out.append(_str, strlen(_str));
    }

private:

    const char* _str;
};

class IntIns : public Inserter
{
public:

    IntIns(Uint32 x) : _x(x) { } 

    virtual void insert(Array<Sint8>& out) const;

private:

    Uint32 _x;
};

class ArrIns : public Inserter
{
public:

    ArrIns(const Array<Sint8>& arr) : _arr(arr) { }

    virtual ~ArrIns() { }

    virtual void insert(Array<Sint8>& out) const;

private:

    Array<Sint8> _arr;
};

class StringIns : public Inserter
{
public: 

    StringIns(const String& x) : _x(x) { }

    virtual ~StringIns() { }
        
    virtual void insert(Array<Sint8>& out) const;

private:

    String _x;
};

class Formatter
{
public:

    static void format(
	Array<Sint8>& out,
	const char* format,
	const Inserter& i0 = NullIns(),
	const Inserter& i1 = NullIns(),
	const Inserter& i2 = NullIns(),
	const Inserter& i3 = NullIns(),
	const Inserter& i4 = NullIns(),
	const Inserter& i5 = NullIns(),
	const Inserter& i6 = NullIns(),
	const Inserter& i7 = NullIns(),
	const Inserter& i8 = NullIns(),
	const Inserter& i9 = NullIns());

private:

    Formatter() { }

    static void _format(
	Array<Sint8>& out,
	const char* format,
	const Inserter* ins[],
	Uint32 insSize);
};

PEGASUS_NAMESPACE_END

#endif /* Pegasus_Formatter_h */
