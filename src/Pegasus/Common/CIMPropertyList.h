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

#ifndef Pegasus_CIMPropertyList_h
#define Pegasus_CIMPropertyList_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/Array.h>
#include <Pegasus/Common/CIMProperty.h>

PEGASUS_NAMESPACE_BEGIN


/** This class is for representing property lists in the CIM interface.

    This class comprises an array of properties and a flag indicating whether
    the list is null. There are three possibilties which must be represented;
    the property list is:

    <ul>
	<li>Non-empty (and non-null)</li>
	<li>Empty (and non-null)</li>
	<li>Null</li>
    </ul>

    The isNull member is used to indicate whether the parameter list is
    null (or not).

    To create a null property list use the default constructor. Otherwise 
    use the constructor which takes a property array (pass an empty property
    array to get an empty property list object).

    Members are provided for accessing elements of the the internal property
    list. There are none for modifying elements (the entire array must be
    formed and passed to the constructor or replaced by calling set()).
*/
class CIMPropertyList
{
public:

    /** Default constructor (sets isNull to true).
    */
    CIMPropertyList();

    /** Copy constructor.
    */
    CIMPropertyList(const CIMPropertyList& x);

    /** Constructor. Initializes properties (sets isNull to false).
    */
    CIMPropertyList(const Array<CIMProperty>& properties);

    /** Modifier for properties (sets isNull to false).
    */
    void set(const Array<CIMProperty>& properties);

    /** Assignment operator.
    */
    CIMPropertyList& operator=(const CIMPropertyList& x);

    /** Clears the properties array (sets isNull to true).
    */
    void clear();

    /** Returns true if the property list is null.
    */
    Boolean isNull() const { return _isNull; }

    /** Get the number of properties in the list.
    */
    Uint32 getNumProperties() const { return _properties.size(); }

    /** Returns true if the property list is empty.
    */
    Boolean isEmpty() const { return getNumProperties() == 0; }

    /** Get the property at the given position.
    */
    CIMProperty getProperty(Uint32 pos)
    {
	return _properties[pos];
    }

    /** Get the (const) property at the given position.
    */
    CIMConstProperty getProperty(Uint32 pos) const
    {
	return _properties[pos];
    }

private:

    Array<CIMProperty> _properties;
    Boolean _isNull;
};


PEGASUS_NAMESPACE_END

#endif /* Pegasus_CIMPropertyList_h */
