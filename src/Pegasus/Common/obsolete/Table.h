
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
