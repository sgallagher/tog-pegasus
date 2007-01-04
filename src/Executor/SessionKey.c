/*
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
//%/////////////////////////////////////////////////////////////////////////////
*/

#include <string.h>
#include "SessionKey.h"
#include "Random.h"
#include "Fatal.h"
#include "User.h"

typedef struct SessionKeyEntryStruct
{
    SessionKey key;
    int uid;
    void* data;
    void (*destructor)(void*);
    struct SessionKeyEntryStruct* next;
}
SessionKeyEntry;

static SessionKeyEntry* _head = 0;

/*
**==============================================================================
**
** _lookup()
**
**     Get the SessionKeyEntry with the given key value.
**
**==============================================================================
*/

static SessionKeyEntry* _lookup(const SessionKey* key)
{
    SessionKeyEntry* p;

    for (p = _head; p; p = p->next)
    {
        if (memcmp(p->key.data, key->data, sizeof(SessionKey)) == 0)
            return p;
    }

    /* Not found! */
    return 0;
}

/*
**==============================================================================
**
** NewSessionKey()
**
**     Create a new session key entry. Return the session key. Place the
**     entry into a linked list.
**
**==============================================================================
*/

SessionKey NewSessionKey(
    int uid,
    void* data, 
    void (*destructor)(void*))
{
    size_t i;
    SessionKeyEntry* entry;

    /* Loop until a unique key can be generated. */

    const size_t MAX_RETRIES = 16;
    int okay = 0;
    SessionKey key;

    for (i = 0; i < MAX_RETRIES; i++)
    {
        unsigned char data[16];
        FillRandomBytes(data, sizeof(data));
        RandBytesToHexASCII(data, sizeof(data), key.data);

        if (!_lookup(&key))
        {
            okay = 1;
            break;
        }
    }

    if (!okay)
        Fatal(FL, "failed to generate a unique session key");

    /* Create entry. */

    entry = (SessionKeyEntry*)malloc(sizeof(SessionKeyEntry));
    entry->key = key;
    entry->data = data;
    entry->uid = uid;
    entry->destructor = destructor;

    /* Prepend entry to list. */

    entry->next = _head;
    _head = entry;

    /* Return key part. */

    return entry->key;
}

/*
**==============================================================================
**
** DeleteSessionKey()
**
**     Find and delete the given session key entry. Call the destructor if
**     any.
**
**==============================================================================
*/

int DeleteSessionKey(const SessionKey* key)
{
    SessionKeyEntry* prev = 0;
    SessionKeyEntry* p;

    /* Remove entry with this key value from the list. */

    for (p = _head; p; p = p->next)
    {
        if (memcmp(p->key.data, key->data, sizeof(SessionKey)) == 0)
        {
            if (prev)
                prev->next = p->next;
            else
                _head = p->next;

            /* Invoke destructor if any. */

            if (p->destructor)
                (*p->destructor)(p->data);

            return 0;
        }

        prev = p;
    }

    /* Not found. */
    return -1;
}

/*
**==============================================================================
**
** GetSessionKeyData()
**
**     Get the data field for the given SessionKey.
**
**==============================================================================
*/

int GetSessionKeyData(const SessionKey* key, void** data)
{
    SessionKeyEntry* p;

    if (data)
        *data = 0;

    p = _lookup(key);

    if (!p)
        return -1;

    if (data)
        *data = p->data;

    return 0;
}

/*
**==============================================================================
**
** GetSessionKeyUid()
**
**     Get the UID for the session key.
**
**==============================================================================
*/

int GetSessionKeyUid(const SessionKey* key, int* uid)
{
    SessionKeyEntry* p;

    *uid = -1;

    p = _lookup(key);

    if (!p)
        return -1;

    *uid = p->uid;
    return 0;
}

/*
**==============================================================================
**
** GetSessionKeyUsername()
**
**     Get the username from the SessionKey.
**
**==============================================================================
*/

int GetSessionKeyUsername(
    const SessionKey* key,
    char username[EXECUTOR_BUFFER_SIZE])
{
    int uid;

    /* Lookup uid */

    if (GetSessionKeyUid(key, &uid) != 0)
    {
        username[0] = '\0';
        return -1;
    }

    /* Lookup username */

    return GetUserName(uid, username);
}
