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
#include <Pegasus/Common/Stack.h>
#include "WQLSelectStatement.h"

PEGASUS_USING_STD;

PEGASUS_NAMESPACE_BEGIN

template<class T>
inline static Boolean _Compare(const T& x, const T& y, WQLOperation op)
{
    switch (op)
    {
	case WQL_EQ: 
	    return x == y;

	case WQL_NE: 
	    return x != y;

	case WQL_LT: 
	    return x < y;
	case WQL_LE: 
	    return x <= y;

	case WQL_GT: 
	    return x > y;

	case WQL_GE: 
	    return x >= y;

	default:
	    PEGASUS_ASSERT(0);
    }

    return false;
}

static Boolean _Evaluate(
    const WQLOperand& lhs, 
    const WQLOperand& rhs, 
    WQLOperation op)
{
    switch (lhs.getType())
    {
	case WQLOperand::NULL_VALUE:
	{
	    // This cannot happen since expressions of the form
	    // OPERAND OPERATOR NULL are converted to unary form.
	    // For example: "count IS NULL" is treated as a unary
	    // operation in which IS_NULL is the unary operation
	    // and count is the the unary operand.

	    PEGASUS_ASSERT(0);
	    break;
	}

	case WQLOperand::INTEGER_VALUE:
	{
	    return _Compare(
		lhs.valueOf(WQLOperand::INTEGER_VALUE_TAG), 
		rhs.valueOf(WQLOperand::INTEGER_VALUE_TAG), 
		op);
	}

	case WQLOperand::DOUBLE_VALUE:
	{
	    return _Compare(
		lhs.valueOf(WQLOperand::DOUBLE_VALUE_TAG), 
		rhs.valueOf(WQLOperand::DOUBLE_VALUE_TAG), 
		op);
	}

	case WQLOperand::BOOLEAN_VALUE:
	{
	    return _Compare(
		lhs.valueOf(WQLOperand::BOOLEAN_VALUE_TAG), 
		rhs.valueOf(WQLOperand::BOOLEAN_VALUE_TAG), 
		op);
	}

	case WQLOperand::STRING_VALUE:
	{
	    return _Compare(
		lhs.valueOf(WQLOperand::STRING_VALUE_TAG), 
		rhs.valueOf(WQLOperand::STRING_VALUE_TAG), 
		op);
	}

	default:
	    PEGASUS_ASSERT(0);
    }

    return false;
}

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
    const WQLPropertySource* source) const
{
    WQLSelectStatement* that = (WQLSelectStatement*)this;
    Stack<Boolean> stack;
    stack.reserve(16);

    // 
    // Counter for operands:
    //

    Uint32 j = 0;

    //
    // Process each of the operations:
    //

    for (Uint32 i = 0, n = _operations.size(); i < n; i++)
    {
	WQLOperation op = _operations[i];

	switch (op)
	{
	    case WQL_OR:
	    {
		PEGASUS_ASSERT(stack.size() >= 2);

		Boolean op1 = stack.top();
		stack.pop();

		Boolean op2 = stack.top();

		stack.top() = op1 || op2;
		break;
	    }

	    case WQL_AND:
	    {
		PEGASUS_ASSERT(stack.size() >= 2);

		Boolean op1 = stack.top();
		stack.pop();

		Boolean op2 = stack.top();

		stack.top() = op1 && op2;
		break;
	    }

	    case WQL_NOT:
	    {
		PEGASUS_ASSERT(stack.size() >= 1);

		Boolean op = stack.top();
		stack.top() = !op;
		break;
	    }

	    case WQL_EQ:
	    case WQL_NE:
	    case WQL_LT:
	    case WQL_LE:
	    case WQL_GT:
	    case WQL_GE:
	    {
		PEGASUS_ASSERT(_operands.size() >= 2);

		//
		// Resolve the left-hand-side to a value (if not already
		// a value).
		//

		WQLOperand& lhs = that->_operands[j++];

		if (lhs.getType() == WQLOperand::PROPERTY_NAME)
		{
		    const String& propertyName = 
			lhs.valueOf(WQLOperand::PROPERTY_NAME_TAG);

		    if (!source->getValue(propertyName, lhs))
			throw NoSuchProperty(propertyName);
		}

		//
		// Resolve the right-hand-side to a value (if not already
		// a value).
		//

		WQLOperand& rhs = that->_operands[j++];

		if (rhs.getType() == WQLOperand::PROPERTY_NAME)
		{
		    const String& propertyName = 
			rhs.valueOf(WQLOperand::PROPERTY_NAME_TAG);

		    if (!source->getValue(propertyName, rhs))
			throw NoSuchProperty(propertyName);
		}

		//
		// Check for a type mismatch:
		//

		// PEGASUS_OUT(lhs.toString());
		// PEGASUS_OUT(rhs.toString());

		if (rhs.getType() != lhs.getType())
		    throw TypeMismatch();

		//
		// Now that the types are known to be alike, apply the
		// operation:
		//

		stack.push(_Evaluate(lhs, rhs, op));
		break;
	    }

	    case WQL_IS_TRUE:
	    case WQL_IS_NOT_FALSE:
	    {
		PEGASUS_ASSERT(stack.size() >= 1);
		break;
	    }

	    case WQL_IS_FALSE:
	    case WQL_IS_NOT_TRUE:
	    {
		PEGASUS_ASSERT(stack.size() >= 1);
		stack.top() = !stack.top();
		break;
	    }

	    case WQL_IS_NULL:
	    {
		PEGASUS_ASSERT(stack.size() >= 1);
		WQLOperand& x = that->_operands[j++];
		stack.push(x.getType() == WQLOperand::NULL_VALUE);
		break;
	    }

	    case WQL_IS_NOT_NULL:
	    {
		PEGASUS_ASSERT(stack.size() >= 1);
		WQLOperand& x = that->_operands[j++];
		stack.push(x.getType() != WQLOperand::NULL_VALUE);
		break;
	    }
	}
    }

    PEGASUS_ASSERT(stack.size() == 1);
    return stack.top();
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
