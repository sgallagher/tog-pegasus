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

#include <iostream>
#include "WQLSelectStatement.h"

PEGASUS_USING_STD;

PEGASUS_NAMESPACE_BEGIN

WQLSelectStatement::WQLSelectStatement()
{
    //
    // Reserve space for a where clause with up to sixteen terms.
    //

    _operations.reserve(16);
    _operands.reserve(16);
}

WQLSelectStatement::~WQLSelectStatement()
{

}

void WQLSelectStatement::clear()
{
    _className.clear();
    _propertyNames.clear();
    _operations.clear();
    _operands.clear();
}

Boolean WQLSelectStatement::evaluateWhereClause(
    const CIMInstance& instance) const
{
    return false;
}

void WQLSelectStatement::print() const
{
    //
    // Print the header:
    //
    
    cout << "WQLSelectStatement" << endl;
    cout << "{" << endl;

    //
    // Print the class name:
    //

    cout << "    _className: \"" << _className << '"' << endl;

    // 
    // Print the property:
    //

    for (Uint32 i = 0; i < _propertyNames.size(); i++)
    {
	if (i == 0)
	    cout << endl;

	cout << "    _propertyNames[" << i << "]: ";
	cout << '"' << _propertyNames[i] << '"' << endl;
    }

    //
    // Print the operations:
    //

    for (Uint32 i = 0; i < _operations.size(); i++)
    {
	if (i == 0)
	    cout << endl;

	cout << "    _operations[" << i << "]: ";
	cout << '"' << WQLOperationToString(_operations[i]) << '"' << endl;
    }

    //
    // Print the operands:
    //

    for (Uint32 i = 0; i < _operands.size(); i++)
    {
	if (i == 0)
	    cout << endl;

	cout << "    _operands[" << i << "]: ";
	cout << '"' << _operands[i].toString() << '"' << endl;
    }

    //
    // Print the trailer:
    //

    cout << "}" << endl;
}

PEGASUS_NAMESPACE_END
