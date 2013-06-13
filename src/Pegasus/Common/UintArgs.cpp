//%LICENSE////////////////////////////////////////////////////////////////
//
// Licensed to The Open Group (TOG) under one or more contributor license
// agreements.  Refer to the OpenPegasusNOTICE.txt file distributed with
// this work for additional information regarding copyright ownership.
// Each contributor licenses this file to you under the OpenPegasus Open
// Source License; you may not use this file except in compliance with the
// License.
//
// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation
// the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included
// in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
// OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
// IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
// CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
// TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
// SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
//
//////////////////////////////////////////////////////////////////////////
//
//%////////////////////////////////////////////////////////////////////////////

#include <Pegasus/Common/UintArgs.h>
#include <Pegasus/Common/StringConversion.h>

PEGASUS_NAMESPACE_BEGIN

//
// Uint32Arg Integer Class used for parameters that
// require Uint32 on input or output. Provides for NULL as well as
// all Uint32 values
//
Uint32Arg::Uint32Arg() : _value(0), _null(true)
{
}

Uint32Arg::Uint32Arg(const Uint32Arg& x) :
    _value(x._value), _null(x._null)
{
}

Uint32Arg::Uint32Arg(Uint32 x) : _value(x), _null(false)
{
}

Uint32Arg::~Uint32Arg()
{
}

Uint32Arg& Uint32Arg::operator=(const Uint32Arg& x)
{
    if (&x != this)
    {
        _value = x._value;
        _null = x._null;
    }

    return *this;
}


const Uint32& Uint32Arg::getValue() const
{
    return _value;
}

void Uint32Arg::setValue(Uint32 x)
{
    _value = x;
    _null = false;
}

Boolean Uint32Arg::isNull() const
{
    return _null;
}

void Uint32Arg::setNullValue()
{
    _value = 0;
    _null = true;
}

String Uint32Arg::toString()
{
    String s;
    if (_null)
    {
        s = "NULL";
    }
    else
    {
        char buffer[22];
        Uint32 size;
        const char* rtn = Uint32ToString(buffer, _value, size);
        s = rtn;
    }
    return s;
}
Boolean Uint32Arg::equal(const Uint32Arg& x) const
{
    if ((_null != x._null))
    {
        return false;
    }
    return _null? true : (_value == x._value);
}

Boolean operator==(const Uint32Arg& x, const Uint32Arg& y)
{
    return x.equal(y);
}
// Uint64 Class Used for handling of Uint64
// parameters on Client input and output
//

Uint64Arg::Uint64Arg() : _value(0), _null(true)
{
}

Uint64Arg::Uint64Arg(const Uint64Arg& x) :
    _value(x._value), _null(x._null)
{
}

Uint64Arg::Uint64Arg(Uint64 x) : _value(x), _null(false)
{
}

Uint64Arg::~Uint64Arg()
{
}

Uint64Arg& Uint64Arg::operator=(const Uint64Arg& x)
{
    if (&x != this)
    {
        _value = x._value;
        _null = x._null;
    }

    return *this;
}


const Uint64& Uint64Arg::getValue() const
{
    return _value;
}

void Uint64Arg::setValue(Uint64 x)
{
    _value = x;
    _null = false;
}

Boolean Uint64Arg::isNull() const
{
    return _null;
}

void Uint64Arg::setNullValue()
{
    _value = 0;
    _null = true;
}
String Uint64Arg::toString()
{
    String s;
    if (_null)
    {
        s = "NULL";
    }
    else
    {
        char buffer[22];
        Uint32 size;
        const char* rtn = Uint64ToString(buffer, _value, size);
        s = rtn;
    }
    return s;
}

Boolean Uint64Arg::equal(const Uint64Arg& x) const
{
    if ((_null != x._null))
    {
        return false;
    }
    return _null? true : (_value == x._value);
}
Boolean operator==(const Uint64Arg& x, const Uint64Arg& y)
{
    return x.equal(y);
}
PEGASUS_NAMESPACE_END
