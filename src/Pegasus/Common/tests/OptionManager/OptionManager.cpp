//%2006////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001, 2002 BMC Software; Hewlett-Packard Development
// Company, L.P.; IBM Corp.; The Open Group; Tivoli Systems.
// Copyright (c) 2003 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation, The Open Group.
// Copyright (c) 2004 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation; VERITAS Software Corporation; The Open Group.
// Copyright (c) 2005 Hewlett-Packard Development Company, L.P.; IBM Corp.;
// EMC Corporation; VERITAS Software Corporation; The Open Group.
// Copyright (c) 2006 Hewlett-Packard Development Company, L.P.; IBM Corp.;
// EMC Corporation; Symantec Corporation; The Open Group.
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
// Author: Vageesh Umesh (vagumesh@in.ibm.com)
//
//%/////////////////////////////////////////////////////////////////////////////

/**
    This file has testcases that are added to cover the functions in
    Pegasus/Common/OptionManager.cpp.
*/

#include <Pegasus/Common/OptionManager.h>
#include <Pegasus/Common/CIMObject.h>
#include <Pegasus/Common/PegasusAssert.h>

PEGASUS_USING_PEGASUS;
PEGASUS_USING_STD;

void test01()
{
     OptionManager omobj;

     String optionName;
     String defaultValue;
     Option::Type INTEGER;
     Array<String> str;

    {
        Option oobj(optionName, defaultValue, true, INTEGER);

        /**
            Added to cover the function
            const Array<String>& Option::getDomain() const
        */
        str = oobj.getDomain();
        PEGASUS_TEST_ASSERT(str != NULL);

        /**
            Added to cover the function
            void Option::setDomain(const Array<String>& domain)
        */
        oobj.setDomain(str);
        PEGASUS_TEST_ASSERT(oobj.getDomain() != NULL);

        /**
            Added to cover the function
            Option& Option(const Option& x)
        */
        const Option oobj1 = oobj;
        Option oobj2(oobj1);
        PEGASUS_TEST_ASSERT(oobj2.getType() == oobj1.getType());

        /**
            Added to cover the function
            Option& Option::operator=(const Option& x)
        */
        Option oobj3(oobj2);
        oobj3 = oobj1;
        PEGASUS_TEST_ASSERT(oobj3.getType() == oobj1.getType());
   }

   {
        /**
            Added to cover the function
            void OptionManager::mergeFile(const String& fileName)
        */
        Boolean gotException = false;
        try
        {
            omobj.mergeFile("abc.cpp");
        }
        catch(NoSuchFile&)
        {
            gotException = true;
        }
        PEGASUS_TEST_ASSERT(gotException);
    }

    {
        /**
            Added to cover the function
            Boolean OptionManager::lookupValue(
            const String& name, String& value) const
        */
        Boolean ret_val = false;
        String value1;
        String &value = value1;
        ret_val = omobj.lookupValue("name", value);
        PEGASUS_TEST_ASSERT(ret_val == false);
    }

    {
        /**
            Added to cover the function
            Boolean OptionManager::lookupIntegerValue(
            const String& name, Uint32& value) const
        */
        Boolean ret_val = false;
        Uint32 value1 = 32;
        Uint32 &value = value1;
        ret_val = omobj.lookupIntegerValue("name", value);
        PEGASUS_TEST_ASSERT(ret_val == false);
    }

    {
        /**
            Added to cover the function
            Boolean OptionManager::valueEquals(
            const String& name, const String& value)
        */
        Boolean ret_val = false;
        String value1;
        String &value = value1;
        ret_val = omobj.valueEquals("name", value);
        PEGASUS_TEST_ASSERT(ret_val == false);
    }

    {
        /**
            Added to cover the function
            Boolean OptionManager::isTrue(const String& name) const
        */
        Boolean ret_val = false;
        String value1;
        const String &value = value1;
        ret_val = omobj.isTrue(value);
        PEGASUS_TEST_ASSERT(ret_val == false);
    }
}


int main(int argc, char** argv)
{
    try
    {
       test01();
    }
    catch (Exception& e)
    {
        PEGASUS_STD(cout) << "Exception: " << e.getMessage() << PEGASUS_STD(endl);
        exit(1);
    }

    cout << argv[0] << " +++++ passed all tests" << endl;
    return 0;
}
