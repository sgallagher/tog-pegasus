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

#ifndef Pegasus_WQLPropertySource_h
#define Pegasus_WQLPropertySource_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/String.h>
#include <Pegasus/WQL/Linkage.h>

PEGASUS_NAMESPACE_BEGIN

/** This class must be implemented to provide a source of properties for the
    WQLSelectStatement::evaluateWhereClause() method which calls methods of
    this class to obtain real values for property names used in the where
    clause.

    The methods of this class all have the following form:

    <pre>
	virtual Boolean getValue(
	    const String& propertyName, 
	    T& value, 
	    Boolean& isNull) const = 0;
    </pre>

    Where "T" is the data type. True is returned if a property with that
    name is actually exists and the value refers to the value of that
    property. If the property is null, the the isNull flag is passed.
*/
class PEGASUS_WQL_LINKAGE WQLPropertySource
{
public:

    virtual Boolean getValue(
	const String& propertyName, 
	Sint32& value, 
	Boolean& isNull) const = 0;

    virtual Boolean getValue(
	const String& propertyName, 
	Real64& x, 
	Boolean& isNull) const = 0;

    virtual Boolean getValue(
	const String& propertyName, 
	Boolean& x, 
	Boolean& isNull) const = 0;

    virtual Boolean getValue(
	const String& propertyName, 
	String& x, 
	Boolean& isNull) const = 0;
};

PEGASUS_NAMESPACE_END

#endif /* Pegasus_WQLPropertySource_h */
