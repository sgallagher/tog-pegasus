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

#ifndef _Pegasus_Common_SessionKey_h
#define _Pegasus_Common_SessionKey_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/String.h>
#include "Linkage.h"

PEGASUS_NAMESPACE_BEGIN

/** This structure represents the session key assigned by the executor during 
    authentication. The key is a zero-terminated ASCII string, consisting of
    the characters 'A' through 'Z' and '0' through '9'. For example:

        62DC257868A5453C874B4C6873F050DD

    When running without privilege separation or without authentication,
    the SessionKey is inapplicable. By default, the session key is null
    (i.e., FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF).
*/
class PEGASUS_COMMON_LINKAGE SessionKey
{
public:

    enum { SESSION_KEY_LENGTH = 32 };

    SessionKey()
    {
        clear();
    }

    SessionKey(const SessionKey& x)
    {
        memcpy(_data, x._data, sizeof(_data));
    }

    SessionKey& operator=(const SessionKey& x);

    void clear();

    const char* data() const 
    { 
        return _data; 
    }

    size_t size() const 
    {
        return sizeof(_data);
    }

    bool null() const;

    bool valid() const;

private:

    char _data[SESSION_KEY_LENGTH+1];
};

/** The SessionKeyMap maintains a mapping between userNames and sessionKeys
    It provides methods for adding, finding, and removing userName-sessionKey
    pairs.
*/
class PEGASUS_COMMON_LINKAGE SessionKeyMap
{
public:

    SessionKeyMap();

    SessionKeyMap(const SessionKeyMap& x);

    ~SessionKeyMap();

    void clear();

    SessionKeyMap& operator=(const SessionKeyMap& x);

    bool insert(const String& userName, const SessionKey& sessionKey);

    bool find(const String& userName, SessionKey& sessionKey);

    bool remove(const String& userName);

private:

    void _copy(const SessionKeyMap& x);

    struct Pair
    {
        const String userName;
        SessionKey sessionKey;

        Pair(const String& u, const SessionKey& s) : userName(u), sessionKey(s)
        {
        }
    };

    Pair* _data;
    Uint32 _size;
};

PEGASUS_NAMESPACE_END

#endif /* _Pegasus_Common_SessionKey_h */
