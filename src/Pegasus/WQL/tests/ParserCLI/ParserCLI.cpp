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
//              Karl Schopmeyer
//              Jim Wunderlich (Jim_Wunderlich@prodigy.net)
//
//%/////////////////////////////////////////////////////////////////////////////

#include <cassert>
#include <iostream>
#include <Pegasus/Common/Exception.h>
#include <Pegasus/WQL/WQLParser.h>
#include <Pegasus/WQL/WQLSimplePropertySource.h>

PEGASUS_USING_PEGASUS;
PEGASUS_USING_STD;

Boolean containsProperty(const CIMName& name, const Array<CIMName>& props)
{
  for (Uint32 i = 0; i < props.size(); i++)
  {
    if (props[i] == name)
    {
      return true;
    }
  }

  return false;
}


int main(int argc, char** argv)
{
    Boolean verbose = (getenv ("PEGASUS_TEST_VERBOSE")) ? true : false;

    //
    // Check arguments:
    //

    if (argc < 2)
    {
	cerr << "Usage: " << argv[0] << " <returnOptions> wql-text..." << endl;
    cerr << "return Options are keywords parseError, noEvaluate or evalulateError" << endl;
    cerr << "They tell the call to reverse the error on the parse or evalutate tests" << endl;
	return(1);
    }
    Boolean evaluateErrorTest = false;
    Boolean parseErrorTest = false;
    Boolean ignoreEvaluation = false;

    if (!strcmp(argv[1],"evaluateError"))
    {
        if (verbose)
            {cout << "Negative  evaluate test" << endl;}
        evaluateErrorTest = true;
    }

    if (!strcmp(argv[1], "parseError"))
    {
        if (verbose)
            {cout << "Negative  parse test" << endl;}
        parseErrorTest = true;
    }


    if (!strcmp(argv[1], "noEvaluate"))
    {
        if (verbose)
            {cout << "Negative  parse test" << endl;}
        ignoreEvaluation = true;
    }
    // 
    // Append all arguments together to from a single string:
    //

    Array<char> text;
    int startArray = 1;
    if (parseErrorTest || evaluateErrorTest || ignoreEvaluation)
        startArray++;

    for (int i = startArray; i < argc; i++)
    {
        text.append(argv[i], strlen(argv[i]));

    	if (i + 1 !=  argc)
    	    text.append("\n", 1);
    }

    text.append('\0');

    if (verbose)
        cout << text.getData() << endl;

    // 
    // Parse the text:
    //

    WQLSelectStatement statement;

    try
    {
        WQLParser::parse(text, statement);
    }
    catch (Exception& e)
    {
        if (parseErrorTest)
        {
            cout << argv[0] <<" +++++ passed all tests" << endl;
            return 0;
        }
        else
        {
            cerr << "Parse Exception: " << e.getMessage() << endl;
            return(1);
        }
    }

    if (verbose)
    {
        statement.print();
    }

    //
    // Create a property source:
    //

    WQLSimplePropertySource source;
    assert(source.addValue("v", WQLOperand()));
    assert(source.addValue("w", WQLOperand(true, WQL_BOOLEAN_VALUE_TAG)));
    assert(source.addValue("x", WQLOperand(10, WQL_INTEGER_VALUE_TAG)));
    assert(source.addValue("y", WQLOperand(10.10, WQL_DOUBLE_VALUE_TAG)));
    assert(source.addValue("z", WQLOperand("Ten", WQL_STRING_VALUE_TAG)));

    assert(source.addValue("SourceInstance", WQLOperand("SourceInstance", WQL_STRING_VALUE_TAG)));
    assert(source.addValue("PreviousInstance", WQLOperand("PreviousInstance", WQL_STRING_VALUE_TAG)));
    assert(source.addValue("OperationalStatus", WQLOperand("OperationalStatus", WQL_STRING_VALUE_TAG)));

    //
    // Create an instance
    //

    CIMInstance inst("ClassName");
    inst.addProperty(CIMProperty(CIMName("v"), CIMValue())); 
    inst.addProperty(CIMProperty(CIMName("w"), CIMValue(true)));
    inst.addProperty(CIMProperty(CIMName("x"), CIMValue(10)));
    inst.addProperty(CIMProperty(CIMName("y"), CIMValue(10.10)));
    inst.addProperty(CIMProperty(CIMName("z"), CIMValue(String("Ten"))));
    inst.addProperty(CIMProperty(CIMName("SourceInstance"), CIMValue(String("SourceInstance"))));
    inst.addProperty(CIMProperty(CIMName("PreviousInstance"), CIMValue(String("PreviousInstance"))));
    inst.addProperty(CIMProperty(CIMName("OperationalStatus"), CIMValue(String("OperationalStatus"))));

    //
    // Create an empty instance
    //
    CIMInstance emptyInst("ClassName");

    //
    // Create an empty object
    //
    CIMObject emptyObj = emptyInst;

    //
    // Apply projection (both overloads) with populated instance or object.
    // Tests removal of extra properties.
    //

    try
    {
        Array<CIMName> selectList = statement.getSelectPropertyList().getPropertyNameArray();

        CIMInstance projInst = inst.clone();
        statement.applyProjection(projInst, false);  // do not allow missing props
        if (statement.getAllProperties())
        {
            assert(projInst.getPropertyCount() == inst.getPropertyCount());
        }
        else
        {
            assert(projInst.getPropertyCount() == statement.getSelectPropertyNameCount());
            for (Uint32 i = 0; i < projInst.getPropertyCount(); i++)
            {
                assert(containsProperty(projInst.getProperty(i).getName(), selectList));
            }
        }
        
        CIMObject projObj = inst.clone();
        statement.applyProjection(projObj, false);  // do not allow missing props
        if (statement.getAllProperties())
        {
            assert(projObj.getPropertyCount() == inst.getPropertyCount());
        }
        else
        {
            assert(projObj.getPropertyCount() == statement.getSelectPropertyNameCount());
            for (Uint32 i = 0; i < projObj.getPropertyCount(); i++)
            {
                assert(containsProperty(projObj.getProperty(i).getName(), selectList));
            }
        }
    }
    catch (Exception& e)
    {
        cerr << "ApplyProjection Exception: " << e.getMessage() << endl;
        return(1);
    }

    //
    // Apply projection (both overloads) with empty instance or object.
    // Tests checking for missing properties (allowMissing parameter).
    //
    if (!statement.getAllProperties())
    {
        Boolean gotExc = false;
        try
        {
            statement.applyProjection(emptyInst, false);  // do not allow missing props
            assert(false);
        }
        catch (QueryRuntimePropertyException&)
        {
            gotExc = true;  // this is expected due to missing props
        }
        catch (Exception& e)
        {
            cerr << "ApplyProjection Exception with empty instance, do not allow missing props: " 
                 << e.getMessage() << endl;
            return(1);
        }
        assert(gotExc);

        try
        {
            statement.applyProjection(emptyInst, true);  // allow missing props
        }    
        catch (Exception& e)
        {
            cerr << "ApplyProjection Exception with empty instance, allow missing props: " 
                 << e.getMessage() << endl;
            return(1);
        }

        gotExc = false;
        try
        {
            statement.applyProjection(emptyObj, false);  // do not allow missing props
            assert(false);
        }
        catch (QueryRuntimePropertyException&)
        {
            gotExc = true;  // this is expected due to missing props
        }
        catch (Exception& e)
        {
            cerr << "ApplyProjection Exception with empty object, do not allow missing props: " 
                 << e.getMessage() << endl;
            return(1);
        }
        assert(gotExc);

        try
        {
            statement.applyProjection(emptyObj, true);  // allow missing props
        }
        catch (Exception& e)
        {
            cerr << "ApplyProjection Exception with empty object, allow missing props: " 
                 << e.getMessage() << endl;
            return(1);
        }
    }
    
    //
    // Evaluate the where clause:
    //
    if (!ignoreEvaluation)
    {
        try
        {
            Boolean returnValue;
            returnValue = (statement.evaluateWhereClause(&source) == true);
            assert(returnValue || evaluateErrorTest);
        }
        catch (Exception& e)
        {
            if (evaluateErrorTest)
            {
                cerr << "EvaluateWhereClause Exception: " << e.getMessage() << endl;
                exit(1);
            }
            else
            {
                cout << argv[0] <<" +++++ passed all tests" << endl;
                return 0;
            }
        }
    }

    cout << argv[0] <<" +++++ passed all tests" << endl;
    return 0;
}
