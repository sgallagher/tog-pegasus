//%/////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001 BMC Software, Hewlett-Packard Company, IBM, 
// The Open Group, Tivoli Systems
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

#ifndef Pegasus_WQLSimplePropertySource_h
#define Pegasus_WQLSimplePropertySource_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/WQL/WQLPropertySource.h>
#include <Pegasus/Common/HashTable.h>

PEGASUS_NAMESPACE_BEGIN

template<class T>
struct _WQLHashEntry
{
    T value;
    Boolean isNull;
};

class PEGASUS_WQL_LINKAGE WQLSimplePropertySource : public WQLPropertySource
{
public:

    WQLSimplePropertySource()
    {
    }

    ~WQLSimplePropertySource()
    {
    }

    virtual Boolean getValue(
	const String& propertyName, 
	Sint32& value, 
	Boolean& isNull) const
    {
	_WQLHashEntry<Sint32> entry;

	if (_intMap.lookup(propertyName, entry))
	{
	    value = entry.value;
	    isNull = entry.isNull;
	    return true;
	}

	return false;
    }

    virtual Boolean getValue(
	const String& propertyName, 
	Real64& value, 
	Boolean& isNull) const
    {
	_WQLHashEntry<Real64> entry;

	if (_realMap.lookup(propertyName, entry))
	{
	    value = entry.value;
	    isNull = entry.isNull;
	    return true;
	}

	return false;
    }

    virtual Boolean getValue(
	const String& propertyName, 
	Boolean& value, 
	Boolean& isNull) const
    {
	_WQLHashEntry<Boolean> entry;

	if (_boolMap.lookup(propertyName, entry))
	{
	    value = entry.value;
	    isNull = entry.isNull;
	    return true;
	}

	return false;
    }

    virtual Boolean getValue(
	const String& propertyName, 
	String& value, 
	Boolean& isNull) const
    {
	_WQLHashEntry<String> entry;

	if (_strMap.lookup(propertyName, entry))
	{
	    value = entry.value;
	    isNull = entry.isNull;
	    return true;
	}

	return false;
    }

    Boolean addValue(
	const String& propertyName, 
	Sint32 value, 
	Boolean isNull)
    {
	_WQLHashEntry<Sint32> entry;
	entry.value = value;
	entry.isNull = isNull;
	return _intMap.insert(propertyName, entry);
    }

    Boolean addValue(
	const String& propertyName, 
	Real64 value, 
	Boolean isNull)
    {
	_WQLHashEntry<Real64> entry;
	entry.value = value;
	entry.isNull = isNull;
	return _realMap.insert(propertyName, entry);
    }

    Boolean addValue(
	const String& propertyName, 
	Boolean value, 
	Boolean isNull)
    {
	_WQLHashEntry<Boolean> entry;
	entry.value = value;
	entry.isNull = isNull;
	return _boolMap.insert(propertyName, entry);
    }

    Boolean addValue(
	const String& propertyName, 
	const String& value, 
	Boolean isNull)
    {
	_WQLHashEntry<String> entry;
	entry.value = value;
	entry.isNull = isNull;
	return _strMap.insert(propertyName, entry);
    }

private:

    typedef HashTable< 
	String, _WQLHashEntry<Sint32>, EqualFunc<String>, HashFunc<String> > 
	IntMap;

    typedef HashTable<
	String, _WQLHashEntry<Real64>, EqualFunc<String>, HashFunc<String> > 
	RealMap;

    typedef HashTable<
	String, _WQLHashEntry<Boolean>, EqualFunc<String>, HashFunc<String> > 
	BoolMap;

    typedef HashTable<
	String, _WQLHashEntry<String>, EqualFunc<String>, HashFunc<String> > 
	StrMap;

    IntMap _intMap;
    RealMap _realMap;
    BoolMap _boolMap;
    StrMap _strMap;
};

PEGASUS_NAMESPACE_END

#endif /* Pegasus_WQLSimplePropertySource_h */
