//%2003////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001, 2002  BMC Software, Hewlett-Packard Development
// Company, L. P., IBM Corp., The Open Group, Tivoli Systems.
// Copyright (c) 2003 BMC Software; Hewlett-Packard Development Company, L. P.;
// IBM Corp.; EMC Corporation, The Open Group.
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

#ifndef Pegasus_Table_h
#define Pegasus_Table_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/Array.h>
#include <Pegasus/Common/Pair.h>

PEGASUS_NAMESPACE_BEGIN

/** This class maintains a group of key value pairs. The key may be used
    to obtain the value.
*/
template<class K, class V>
class Table
{
public:

    Table() { }

    ~Table() { }

    Uint32 getSize() const { return _items.getSize(); }

    Boolean contains(const K& key) const;

    Boolean find(const K& key, V& value) const;

    Boolean insert(const K& key, const V& value);

    Boolean remove(const K& key);

private:

    typedef Pair<K, V> KeyValuePair;
    Array<KeyValuePair> _items;
};

template<class K, class V>
Boolean Table<K,V>::contains(const K& key) const
{
    for (Uint32 i = 0, n = _items.getSize(); i < n; i++)
    {
	if (_items[i].first == key)
	    return true;
    }
    return false;
}

template<class K, class V>
Boolean Table<K,V>::find(const K& key, V& value) const
{
    for (Uint32 i = 0, n = _items.getSize(); i < n; i++)
    {
	if (_items[i].first == key)
	{
	    value = _items[i].second;
	    return true;
	}
    }
    return false;
}

template<class K, class V>
Boolean Table<K,V>::insert(const K& key, const V& value)
{
    if (contains(key))
	return false;

    _items.append(Pair<K,V>(key, value));
    return true;
}

template<class K, class V>
Boolean Table<K,V>::remove(const K& key)
{
    for (Uint32 i = 0, n = _items.getSize(); i < n; i++)
    {
	if (_items[i].first == key)
	{
	    _items.remove(i);
	    return true;
	}
    }
    return false;
}

PEGASUS_NAMESPACE_END

#endif /* Pegasus_Table_h */
