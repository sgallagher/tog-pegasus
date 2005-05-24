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

Uint32 HashFunc<String>::hash(const String& str)
{
    Uint32 h = 0;

    for (Uint32 i = 0, n = str.size(); i < n; i++)
        h = 5 * h + str[i];

    return h;
}

////////////////////////////////////////////////////////////////////////////////
//
// _HashTableRep::_BucketBase
//
////////////////////////////////////////////////////////////////////////////////

_BucketBase::~_BucketBase()
{

}

////////////////////////////////////////////////////////////////////////////////
//
// _HashTableIteratorBase
//
////////////////////////////////////////////////////////////////////////////////

_HashTableIteratorBase _HashTableIteratorBase::operator++(int)
{
    _HashTableIteratorBase tmp = *this;
    operator++();
    return tmp;
}

_HashTableIteratorBase& _HashTableIteratorBase::operator++()
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

_HashTableIteratorBase::_HashTableIteratorBase(
    _BucketBase** first, 
    _BucketBase** last) : _first(first), _last(last)
{
    _bucket = 0;

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
// _HashTableRep
//
////////////////////////////////////////////////////////////////////////////////

_HashTableRep::_HashTableRep(Uint32 numChains) : _size(0), _numChains(numChains)
{
    if (numChains < 8)
	numChains = 8;

    _chains = new _BucketBase*[_numChains];
    memset(_chains, 0, sizeof(_BucketBase*) * _numChains);
}

_HashTableRep::_HashTableRep(const _HashTableRep& x)
{
    _size = 0;
    _numChains = 0;
    _chains = 0;
    operator=(x);
}

_HashTableRep::~_HashTableRep()
{
    clear();
    if (_chains)
       delete [] _chains;
}

_HashTableRep& _HashTableRep::operator=(const _HashTableRep& x)
{
    if (this == &x)
	return *this;

    // Destroy the old representation:

    clear();

    if (_chains)
	delete [] _chains;

    // Create chain array:

    _chains = new _BucketBase*[_numChains = x._numChains];
    memset(_chains, 0, sizeof(_BucketBase*) * _numChains);
    _size = x._size;

    // Copy over the buckets:

    for (Uint32 i = 0; i < _numChains; i++)
    {
	if (x._chains[i])
	{
	    _chains[i] = x._chains[i]->clone();

	    _BucketBase* curr = _chains[i];
	    _BucketBase* next = x._chains[i]->next;

	    for (; next; next = next->next)
	    {
		curr->next = next->clone();
		curr = curr->next;
	    }
	}
    }

    return *this;
}

void _HashTableRep::clear()
{
    for (Uint32 i = 0; i < _numChains; i++)
    {
	for (_BucketBase* bucket = _chains[i]; bucket; )
	{
	    _BucketBase* next = bucket->next;
	    delete bucket;
	    bucket = next;
	}
    }

    _size = 0;

    if (_numChains)
	memset(_chains, 0, sizeof(_BucketBase*) * _numChains);
}

Boolean _HashTableRep::insert(
    Uint32 hashCode, 
    _BucketBase* bucket, 
    const void* key)
{
    // Check for duplicate entry with same key:

    Uint32 i = hashCode % _numChains;
    _BucketBase* last = 0;

    for (_BucketBase* b = _chains[i]; b; b = b->next)
    {
	if (b->equal(key))
	{
	    delete bucket;
	    return false;
	}

	last = b;
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

const _BucketBase* _HashTableRep::lookup(
    Uint32 hashCode, 
    const void* key) const
{
#ifdef PEGASUS_OS_VMS

// This is to prevent a crash when the hash
//  code hasn't been initialized!

    if (_numChains == 0)
    {
      return 0;
    }
#endif
    Uint32 i = hashCode % _numChains;

    for (_BucketBase* bucket = _chains[i]; bucket; bucket = bucket->next)
    {
	if (bucket->equal(key))
	    return bucket;
    }

    // Not found!
    return 0;
}

Boolean _HashTableRep::remove(Uint32 hashCode, const void* key)
{
    for (Uint32 i = 0; i < _numChains; i++)
    {
	_BucketBase* prev = 0;

	for (_BucketBase* bucket = _chains[i]; bucket; bucket = bucket->next)
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
