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
//%/////////////////////////////////////////////////////////////////////////////

/**
    This file has testcases that are added to cover the functions in
    Pegasus/ProviderManager2/ProviderName.cpp.
*/

#include <Pegasus/ProviderManager2/ProviderName.h>
#include <Pegasus/Common/PegasusAssert.h>

PEGASUS_USING_PEGASUS;
PEGASUS_USING_STD;

static char * verbose;

void test01()
{
    /**
        Added to cover the Function
        ProviderName::ProviderName(
        const String& moduleName,
        const String & logicalName,
        const String & physicalName)
    */
    ProviderName pnobj(
        "dummymodulename",
        "dummylogicalname",
        "dummyphysicalname");

    /**
        Added to cover the Function
        String ProviderName::getModuleName(void) const
    */
    String get_module_name = pnobj.getModuleName();
    PEGASUS_TEST_ASSERT(get_module_name == "dummymodulename");

    /**
        Added to cover the Function
        String ProviderName::getLogicalName(void) const
    */
    String get_logical_name = pnobj.getLogicalName();
    PEGASUS_TEST_ASSERT(get_logical_name == "dummylogicalname");

    /**
        Added to cover the Functions
        void ProviderName::setLocation(const String &location)
                        AND
        String ProviderName::getLocation(void) const
    */
    pnobj.setLocation("//localhost/root/dummy");
    String get_loc = pnobj.getLocation();
    PEGASUS_TEST_ASSERT(get_loc == "//localhost/root/dummy");
}

int main(int argc, char** argv)
{
    try
    {
        test01();
    }
    catch (Exception& e)
    {
        PEGASUS_STD(cout) << "Exception: " << e.getMessage() <<
            PEGASUS_STD(endl);
        exit(1);
    }

    PEGASUS_STD(cout) << argv[0] << " +++++ passed all tests" <<
        PEGASUS_STD(endl);
    return 0;
}
