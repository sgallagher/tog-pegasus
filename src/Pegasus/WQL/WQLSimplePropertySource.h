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
	WQLOperand& value) const
    {
	return _map.lookup(propertyName, value);
    }

    Boolean addValue(
	const String& propertyName, 
	const WQLOperand& value)
    {
	return _map.insert(propertyName, value);
    }

private:

    typedef HashTable<
	String, WQLOperand, EqualFunc<String>, HashFunc<String> > Map;

    Map _map;
};

PEGASUS_NAMESPACE_END

#endif /* Pegasus_WQLSimplePropertySource_h */
