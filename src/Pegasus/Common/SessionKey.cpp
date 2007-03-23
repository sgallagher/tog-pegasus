//%2006////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001, 2002 BMC Software; Hewlett-Packard Development
// Company, L.P.; IBM Corp.; The Open Group; Tivoli Systems.
// Copyright (c) 2003 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation, The Open Group.
// Copyright (c) 2004 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation; VERITAS Software Corporation; The Open Group.
// Copyright (c) 2005 Hewlett-Packard Development Company, L.P.; IBM Corp.;
// EMC Corporation; VERITAS Software Corporation; The Open Group.
// Copyright (c) 2006 Hewlett-Packard Development Company, L.P.; IBM Corp.;
// EMC Corporation; Symantec Corporation; The Open Group.
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

#include <new>
#include <cctype>
#include <cstring>
#include <cstdlib>
#include "SessionKey.h"

PEGASUS_NAMESPACE_BEGIN

//==============================================================================
//
// class SessionKey
//
//==============================================================================

SessionKey& SessionKey::operator=(const SessionKey& x)
{
    if (&x != this)
        memcpy(_data, x._data, sizeof(_data));

    return *this;
}

void SessionKey::clear()
{
    memset(_data, 'F', SESSION_KEY_LENGTH);
    _data[SESSION_KEY_LENGTH] = '\0';
}

bool SessionKey::null() const
{
    return memcmp(
        _data, "FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF", sizeof(_data)) == 0;
}

bool SessionKey::valid() const
{
    for (size_t i = 0; i < SESSION_KEY_LENGTH; i++)
    {
        if (!isxdigit(_data[i]))
            return false;
    }

    return _data[SESSION_KEY_LENGTH] == '\0';
}

//==============================================================================
//
// class SessionKeyMap
//
//==============================================================================

SessionKeyMap::SessionKeyMap() : _data(0), _size(0)
{
}

SessionKeyMap::SessionKeyMap(const SessionKeyMap& x)
{
    _copy(x);
}

SessionKeyMap::~SessionKeyMap()
{
    clear();
}

void SessionKeyMap::clear()
{
    for (Uint32 i = 0; i < _size; i++)
        _data[i].~Pair();

    free(_data);

    _data = 0;
    _size = 0;
}

SessionKeyMap& SessionKeyMap::operator=(const SessionKeyMap& x)
{
    if (&x != this)
    {
        clear();
        _copy(x);
    }

    return *this;
}

bool SessionKeyMap::insert(const String& userName, const SessionKey& sessionKey)
{
    for (Uint32 i = 0; i < _size; i++)
    {
        if (_data[i].userName == userName)
            return false;
    }

    _data = (Pair*)realloc(_data, sizeof(Pair) * (_size + 1));
    new(&_data[_size]) Pair(userName, sessionKey);
    _size++;

    return true;
}

bool SessionKeyMap::find(const String& userName, SessionKey& sessionKey)
{
    for (Uint32 i = 0; i < _size; i++)
    {
        if (_data[i].userName == userName)
        {
            sessionKey = _data[i].sessionKey;
            return true;
        }
    }

    return false;
}

bool SessionKeyMap::remove(const String& userName)
{
    for (Uint32 i = 0; i < _size; i++)
    {
        if (_data[i].userName == userName)
        {
            _data[i].~Pair();
            size_t r = _size - i - 1;

            if (r)
                memcpy(&_data[i], &_data[i+1], sizeof(Pair) * r);

            return true;
        }
    }

    return false;
}

void SessionKeyMap::_copy(const SessionKeyMap& x)
{
    if (x._size)
    {
        _data = (Pair*)malloc(sizeof(Pair) * x._size);
        _size = x._size;

        for (Uint32 i = 0; i < x._size; i++)
            new(&_data[i]) Pair(x._data[i]);
    }
    else
    {
        _data = 0;
        _size = 0;
    }
}

PEGASUS_NAMESPACE_END
