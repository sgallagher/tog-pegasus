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
// Modified By: Carol Ann Krug Graves, Hewlett-Packard Company
//                (carolann_graves@hp.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#include <cassert>
#include <iostream>
#include <Pegasus/Common/Exception.h>
#include <Pegasus/WQL/WQLParser.h>
#include <Pegasus/WQL/WQLSimplePropertySource.h>

PEGASUS_USING_PEGASUS;
PEGASUS_USING_STD;

Boolean verbose;

void test01()
{
    //
    // Create a property source (a place for the evaluate to get the
    // values of properties from):
    //

    WQLSimplePropertySource source;
    assert(source.addValue("x", WQLOperand(10, WQL_INTEGER_VALUE_TAG)));
    assert(source.addValue("y", WQLOperand(20, WQL_INTEGER_VALUE_TAG)));
    assert(source.addValue("z", WQLOperand(1.5, WQL_DOUBLE_VALUE_TAG)));

    //
    // Define query:
    //
    
    const char TEXT[] = 
	"SELECT x,y,z\n"
	"FROM MyClass\n"
	"WHERE x > 5 AND y < 25 AND z > 1.2";

    //
    //  Will test WQLParser::parse(const Array<Sint8>&, WQLSelectStatement&)
    //  and WQLParser::parse(const char*, WQLSelectStatement&) forms
    //
    Array<char> text;
    text.append(TEXT, sizeof(TEXT));
    if (verbose)
    {
        cout << text.getData() << endl;
    }

    // 
    // Parse the text:
    //

    WQLSelectStatement statement;

    try
    {
	WQLParser::parse(text, statement);
        if (verbose)
        {
	    statement.print();
        }

        //
        //  Test WQLSelectStatement functions
        //
        assert (statement.getClassName().equal ("MyClass"));
        assert (!statement.getAllProperties());
        assert (statement.getSelectPropertyNameCount() == 3);
        CIMName propName = statement.getSelectPropertyName (0);
        assert ((propName.equal ("x")) || (propName.equal ("y")) || 
                (propName.equal ("z")));
        CIMPropertyList propList = statement.getSelectPropertyList();
        assert (!propList.isNull());
        assert (propList.size() == 3);
        assert ((propList[0].equal ("x")) || (propList[0].equal ("y")) || 
                (propList[0].equal ("z")));
        assert (statement.hasWhereClause());
        assert (statement.getWherePropertyNameCount() == 3);
        CIMName wherePropName = statement.getWherePropertyName (0);
        assert ((wherePropName.equal ("x")) || (wherePropName.equal ("y")) || 
                (wherePropName.equal ("z")));
        CIMPropertyList wherePropList = statement.getWherePropertyList();
        assert (!wherePropList.isNull());
        assert (wherePropList.size() == 3);
        assert ((wherePropList[0].equal ("x")) || 
                (wherePropList[0].equal ("y")) || 
                (wherePropList[0].equal ("z")));
        assert (statement.evaluateWhereClause(&source));
    }
    catch (Exception& e)
    {
	cerr << "Exception: " << e.getMessage() << endl;
	exit(1);
    }
}

void test02()
{
    //
    // Create a property source (a place for the evaluate to get the
    // values of properties from):
    //

    WQLSimplePropertySource source;
    assert(source.addValue("a", WQLOperand(5, WQL_INTEGER_VALUE_TAG)));
    assert(source.addValue("b", WQLOperand(25, WQL_INTEGER_VALUE_TAG)));
    assert(source.addValue("c", WQLOperand(0.9, WQL_DOUBLE_VALUE_TAG)));
    assert(source.addValue("d", WQLOperand("Test", WQL_STRING_VALUE_TAG)));

    //
    // Define query:
    //
    
    const char TEXT[] = 
	"SELECT a,c,d\n"
	"FROM YourClass\n"
	"WHERE a > 5 AND b < 25 AND c > 1.2 AND d = \"Pass\"";

    //
    //  Will test WQLParser::parse(const String&, WQLSelectStatement&)
    //  and WQLParser::parse(const char*, WQLSelectStatement&) forms
    //
    String text (TEXT);
    if (verbose)
    {
        cout << text << endl;
    }

    // 
    // Parse the text:
    //

    WQLSelectStatement statement;

    try
    {
	WQLParser::parse(text, statement);
        if (verbose)
        {
	    statement.print();
        }

        //
        //  Test WQLSelectStatement functions
        //
        assert (statement.getClassName().equal ("YourClass"));
        assert (!statement.getAllProperties());
        assert (statement.getSelectPropertyNameCount() == 3);
        CIMName propName = statement.getSelectPropertyName (2);
        assert ((propName.equal ("a")) || (propName.equal ("c")) || 
                (propName.equal ("d")));
        CIMPropertyList propList = statement.getSelectPropertyList();
        assert (!propList.isNull());
        assert (propList.size() == 3);
        assert ((propList[2].equal ("a")) || (propList[2].equal ("c")) || 
                (propList[2].equal ("d")));
        assert (statement.hasWhereClause());
        assert (statement.getWherePropertyNameCount() == 4);
        CIMName wherePropName = statement.getWherePropertyName (3);
        assert ((wherePropName.equal ("a")) || (wherePropName.equal ("b")) || 
                (wherePropName.equal ("c")) || (wherePropName.equal ("d")));
        CIMPropertyList wherePropList = statement.getWherePropertyList();
        assert (!wherePropList.isNull());
        assert (wherePropList.size() == 4);
        assert ((wherePropList[3].equal ("a")) || 
                (wherePropList[3].equal ("b")) || 
                (wherePropList[3].equal ("c")) || 
                (wherePropList[3].equal ("d")));
        assert (!statement.evaluateWhereClause(&source));
    }
    catch (Exception& e)
    {
	cerr << "Exception: " << e.getMessage() << endl;
	exit(1);
    }
}

void test03()
{
    //
    // Define query:
    //
    
    const char TEXT[] = 
	"SELECT *\n"
	"FROM AnotherClass\n";

    //
    //  Will test WQLParser::parse(const String&, WQLSelectStatement&)
    //  and WQLParser::parse(const char*, WQLSelectStatement&) forms
    //
    String text (TEXT);
    if (verbose)
    {
        cout << text << endl;
    }

    // 
    // Parse the text:
    //

    WQLSelectStatement statement;

    try
    {
	WQLParser::parse(text, statement);
        if (verbose)
        {
	    statement.print();
        }

        //
        //  Test WQLSelectStatement functions
        //
        assert (statement.getClassName().equal ("AnotherClass"));
        assert (statement.getAllProperties());
        CIMPropertyList propList = statement.getSelectPropertyList();
        assert (propList.isNull());
        assert (!statement.hasWhereClause());
        assert (statement.getWherePropertyNameCount() == 0);
        CIMPropertyList wherePropList = statement.getWherePropertyList();
        assert (!wherePropList.isNull());
        assert (wherePropList.size() == 0);
    }
    catch (Exception& e)
    {
	cerr << "Exception: " << e.getMessage() << endl;
	exit(1);
    }
}

int main(int argc, char** argv)
{
    verbose = (getenv ("PEGASUS_TEST_VERBOSE")) ? true : false;
    test01();
    test02();
    test03();

    cout << "+++++ passed all tests" << endl;
    return 0;
}
