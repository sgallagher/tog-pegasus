//%/////////////////////////////////////////////////////////////////////////////
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
//==============================================================================
//
// Author: Mike Brasher (mbrasher@bmc.com)
//
// Modified By:
//
//%/////////////////////////////////////////////////////////////////////////////

#include <cstring>
#include "HashTable.h"

PEGASUS_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////
//
// Hash()
//
////////////////////////////////////////////////////////////////////////////////

Uint32 Hash(const String& str)
{
    Uint32 h = 0;

    for (Uint32 i = 0, n = str.getLength(); i < n; i++)
        h = 5 * h + str[i];

    return h;
}

////////////////////////////////////////////////////////////////////////////////
//
// HashTableBase::BucketBase
//
////////////////////////////////////////////////////////////////////////////////

BucketBase::~BucketBase()
{

}

////////////////////////////////////////////////////////////////////////////////
//
// HashTableIteratorBase
//
////////////////////////////////////////////////////////////////////////////////

HashTableIteratorBase HashTableIteratorBase::operator++(int)
{
    HashTableIteratorBase tmp = *this;
    operator++();
    return tmp;
}

HashTableIteratorBase& HashTableIteratorBase::operator++()
{
    // At the end?

    if (!_bucket)
        return *this;

    // More buckets this chain?

    if ((_bucket = _bucket->next))
        return *this;

    // Find the next non-empty chain:

    _bucket = 0;

    while (_first != _last)
    {
        if (*_first)
	{
	    _bucket = *_first++;
	    break;
	}

	_first++;
    }

    return *this;
}

HashTableIteratorBase::HashTableIteratorBase(
    BucketBase** first, 
    BucketBase** last) : _first(first), _last(last)
{
    while (_first != last)
    {
        if (*_first)
	{
	    _bucket = *_first++;
	    break;
	}

	_first++;
    }
}

////////////////////////////////////////////////////////////////////////////////
//
// HashTableBase
//
////////////////////////////////////////////////////////////////////////////////

HashTableBase::HashTableBase(Uint32 numChains) : _size(0), _numChains(numChains)
{
    if (numChains < 8)
	numChains = 8;

    _chains = new BucketBase*[_numChains];
    memset(_chains, 0, sizeof(BucketBase*) * _numChains);
}

HashTableBase::~HashTableBase()
{
    clear();
}

void HashTableBase::clear()
{
    for (Uint32 i = 0; i < _numChains; i++)
    {
	for (BucketBase* bucket = _chains[i]; bucket; )
	{
	    BucketBase* next = bucket->next;
	    delete bucket;
	    bucket = next;
	}
    }

    _size = 0;
    memset(_chains, 0, sizeof(BucketBase*) * _numChains);
}

Boolean HashTableBase::insert(
    Uint32 hashCode, 
    BucketBase* bucket, 
    const void* key)
{
    // Check for duplicate entry with same key:

    Uint32 i = hashCode % _numChains;
    BucketBase* last = 0;

    for (BucketBase* bucket = _chains[i]; bucket; bucket = bucket->next)
    {
	if (bucket->equal(key))
	{
	    delete bucket;
	    return false;
	}

	last = bucket;
    }

    // Insert bucket:

    bucket->next = 0;

    if (last)
	last->next = bucket;
    else
	_chains[i] = bucket;

    _size++;
    return true;
}

const BucketBase* HashTableBase::lookup(
    Uint32 hashCode, 
    const void* key)
{
    Uint32 i = hashCode % _numChains;

    for (BucketBase* bucket = _chains[i]; bucket; bucket = bucket->next)
    {
	if (bucket->equal(key))
	    return bucket;
    }

    // Not found!
    return 0;
}

Boolean HashTableBase::remove(Uint32 hashCode, const void* key)
{
    for (Uint32 i = 0; i < _numChains; i++)
    {
	BucketBase* prev = 0;

	for (BucketBase* bucket = _chains[i]; bucket; bucket = bucket->next)
	{
	    if (bucket->equal(key))
	    {
		if (prev)
		    prev->next = bucket->next;
		else
		    _chains[i] = bucket->next;

		delete bucket;
		_size--;
		return true;
	    }
	    prev = bucket;
	}
    }

    return false;
}

PEGASUS_NAMESPACE_END
