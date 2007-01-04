#include <string.h>
#include "SessionKey.h"
#include "Random.h"
#include "Fatal.h"

struct SessionKeyEntry
{
    SessionKey key;
    void* data;
    void (*destructor)(void*);
    SessionKeyEntry* next;
};

static SessionKeyEntry* _head = 0;

//==============================================================================
//
// _lookup()
//
//     Get the SessionKeyEntry with the given key value.
//
//==============================================================================

static SessionKeyEntry* _lookup(const SessionKey* key)
{
    for (SessionKeyEntry* p = _head; p; p = p->next)
    {
        if (memcmp(p->key.data, key->data, sizeof(SessionKey)) == 0)
            return p;
    }

    // Not found!
    return 0;
}

//==============================================================================
//
// NewSessionKey()
//
//     Create a new session key entry. Return the session key. Place the
//     entry into a linked list.
//
//==============================================================================

SessionKey NewSessionKey(
    void* data, 
    void (*destructor)(void*))
{
    // Loop until a unique key can be generated.

    const size_t MAX_RETRIES = 16;
    bool okay = false;
    SessionKey key;

    for (size_t i = 0; i < MAX_RETRIES; i++)
    {
        unsigned char data[16];
        FillRandomBytes(data, sizeof(data));
        RandBytesToHexASCII(data, sizeof(data), key.data);

        if (!_lookup(&key))
        {
            okay = true;
            break;
        }
    }

    if (!okay)
        Fatal(FL, "failed to generate a unique session key");

    // Create entry.

    SessionKeyEntry* entry = (SessionKeyEntry*)malloc(sizeof(SessionKeyEntry));
    entry->key = key;
    entry->data = data;
    entry->destructor = destructor;

    // Prepend entry to list.

    entry->next = _head;
    _head = entry;

    // Return key part.

    return entry->key;
}

//==============================================================================
//
// DeleteSessionKey()
//
//     Find and delete the given session key entry. Call the destructor if
//     any.
//
//==============================================================================

int DeleteSessionKey(const SessionKey* key)
{
    // Remove entry with this key value from the list.

    SessionKeyEntry* prev = 0;

    for (SessionKeyEntry* p = _head; p; p->next)
    {
        if (memcmp(p->key.data, key->data, sizeof(SessionKey)) == 0)
        {
            if (prev)
                prev->next = p->next;
            else
                _head = p->next;

            // Invoke destructor if any.

            if (p->destructor)
                (*p->destructor)(p->data);

            return 0;
        }

        prev = p;
    }

    // Not found.
    return -1;
}

//==============================================================================
//
// GetSessionKeyData
//
//     Get the data field for the given SessionKey.
//
//==============================================================================

int GetSessionKeyData(const SessionKey* key, void** data)
{
    if (data)
        *data = 0;

    SessionKeyEntry* p = _lookup(key);

    if (!p)
        return -1;

    if (data)
        *data = p->data;

    return 0;
}
