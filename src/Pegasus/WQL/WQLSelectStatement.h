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

#ifndef Pegasus_WQLSelectStatement_h
#define Pegasus_WQLSelectStatement_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/Array.h>
#include <Pegasus/Common/String.h>
#include <Pegasus/Common/CIMInstance.h>
#include <Pegasus/WQL/WQLOperation.h>
#include <Pegasus/WQL/WQLOperand.h>

PEGASUS_NAMESPACE_BEGIN

/** This class represents a compiled WBEMSQl1 select statement. 

    An instance of WQLSelectStatement is passed to WQLParser::parse() which
    parses and WBEMSQL1 SELECT statement and initializes the WQLSelectStatement
    instance. The WBEMSWL1 SELECT statement has the following form:

    <pre>
	SELECT <property>...
	FROM <class name>
	WHERE <where clause>
    </pre>

    The properties may be obtained with the getProperties() method.
    The class name may be obtained with getClassName(). The where clause
    may be evaluated by calling evaluateWhereClause() with the appropriate
    arguments. If hasWhereClause() returns false, then there is no where
    clause.

    The components of the where clause are stored in two arrays: one for
    operands and one for operators (these are placed in proper order by the
    YACC parser). Evaluation is performed using a Boolean stack. See the
    implementation of evaluateWhereClause() for details.
*/
class PEGASUS_WQL_LINKAGE WQLSelectStatement
{
public:

    /** Default constructor. 
    */
    WQLSelectStatement();

    /** Destructor.
    */
    ~WQLSelectStatement();

    /** Clears all data members of this object.
    */
    void clear();

    /** Accessor.
    */
    const String& getClassName() const
    {
	return _className;
    }

    /** Modifier. This method should not be called by the user (only by the
	parser).
    */
    void setClassName(const String& className)
    {
	_className = className;
    }

    /** Returns the number of property names which were indicated in the
	selection list.
    */
    Uint32 getPropertyNameCount() const
    {
	return _propertyNames.size();
    }

    /** Gets the i-th property name in the list.
    */
    const String& getPropertyName(Uint32 i)
    {
	return _propertyNames[i];
    }

    /** Appends a property name to the property name list. This user should
	not call this method; it should only be called by the parser.
    */
    void appendPropertyName(const String& x)
    {
	_propertyNames.append(x);
    }

    /** Appends an operation to the operation array. This method should only
	be called by the parser itself.
    */
    void appendOperation(WQLOperation x)
    {
	_operations.append(x);
    }

    /** Appends an operand to the operation array. This method should only
	be called by the parser itself.
    */
    void appendOperand(const WQLOperand& x)
    {
	_operands.append(x);
    }

    /** Returns true if this class has a where clause.
    */
    Boolean hasWhereClause() const
    {
	return _operations.size() != 0;
    }

    /** Evalautes the where clause using the CIMInstance as input.
    */
    Boolean evaluateWhereClause(const CIMInstance& instance) const;

    /** Prints out the members of this class.
    */
    void print() const;

private:

    //
    // The name of the target class. For example:
    //
    //     SELECT * 
    //     FROM TargetClass
    //     WHERE ...
    //

    String _className;

    //
    // The list of property names being selected. For example, see "firstName",
    // and "lastName" below.
    //
    //     SELECT firstName, lastName 
    //     FROM TargetClass
    //     WHERE ...
    //

    Array<String> _propertyNames;

    //
    // The list of operations encountered while parsing the WHERE clause.
    // Consider this query:
    //
    //     SELECT *
    //     FROM TargetClass
    //     WHERE count > 10 OR peak < 20 AND state = "OKAY"
    //
    // This would generate the following stream of WQLOperations:
    //
    //     WQL_GT
    //     WQL_LT
    //     WQL_EQ
    //     WQL_AND
    //     WQL_OR
    //

    Array<WQLOperation> _operations;

    // 
    // The list of operands encountered while parsing the WHERE clause. They
    // query just above would generate the following stream of operands:
    //
    //     count, 10, peak, 20, state, "OKAY"
    //

    Array<WQLOperand> _operands;
};

PEGASUS_NAMESPACE_END

#endif /* Pegasus_WQLSelectStatement_h */
