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
        Added to cover the Function ProviderName::ProviderName(void);
    */
    ProviderName pn;

    /**
        Added to cover the Function
        ProviderName::ProviderName(
        const CIMNamespaceName & nameSpace,const CIMName & className,
        const Uint32 capabilities,const CIMName & method)
    */
    CIMNamespaceName nsn("/root/cimv");
    CIMName classname("MyClass");
    CIMName method("MyMethod");
    ProviderName pn1(nsn,classname,1,method);

    /**
        Added to cover the Function
        ProviderName::ProviderName(
        const String & logicalName,
        const String & physicalName,
        const String & interfaceName,
        const Uint32 capabilities,
        const CIMName & method)
    */
    ProviderName pnobj("dummylogicalname", "dummyphysicalname", "dummyinterfacename", 1, method);

    /**
        Added to cover the Function
        String ProviderName::getLogicalName(void) const
    */
    String get_logical_name = pnobj.getLogicalName();
    PEGASUS_TEST_ASSERT(get_logical_name == "dummylogicalname");

    /**
        Added to cover the Function
        ProviderName::ProviderName(
        const CIMObjectPath & path,const Uint32 capabilities,const CIMName & method)
    */
    CIMObjectPath cobjpath("//localhost/root/cimv2:MyClass");
    ProviderName pn2(cobjpath,1,method);

    /**
        Added to cover the Function
        void ProviderName::setPhysicalName(const String & physicalName)
    */
    pn.setPhysicalName("PhysicalName1");
    String set_phy_name = pn.getPhysicalName();
    PEGASUS_TEST_ASSERT(set_phy_name == "PhysicalName1");

    /**
        Added to cover the Function
        String ProviderName::getInterfaceName(void) const
    */
    String get_inf_name = pn1.getInterfaceName();
    PEGASUS_TEST_ASSERT(get_inf_name == "");

    /**
        Added to cover the Functions
        void ProviderName::setLocation(const String &location)
                        AND
        String ProviderName::getLocation(void) const
    */
    pn2.setLocation("//localhost/root/dummy");
    String get_loc = pn2.getLocation();
    PEGASUS_TEST_ASSERT(get_loc == "//localhost/root/dummy");

    /**
        Added to cover the Function
        Uint32 ProviderName::getCapabilitiesMask(void) const
    */
    Uint32 get_cap = pn2.getCapabilitiesMask();
    PEGASUS_TEST_ASSERT(get_cap == 1);
    Uint32 get_cap1 = pn.getCapabilitiesMask();
    PEGASUS_TEST_ASSERT(get_cap1 == 0);

    /**
        Added to cover the Function
        CIMName ProviderName::getMethodName(void) const
    */
    CIMName get_met_name = pn1.getMethodName();
    PEGASUS_TEST_ASSERT(get_met_name.getString() == "MyMethod");

    /**
        Added to cover the Function
        CIMName ProviderName::getClassName() const
    */
    CIMName get_cl_name = pn1.getClassName();
    PEGASUS_TEST_ASSERT(get_cl_name.getString() == "MyClass");

    /**
        Added to cover the Function
        CIMNamespaceName ProviderName::getNameSpace() const
    */
    CIMNamespaceName ret_cnsn = pn1.getNameSpace();
    PEGASUS_TEST_ASSERT(ret_cnsn.getString() == "root/cimv");
}

int main(int argc, char** argv)
{
    try
    {
        test01();
    }
    catch(Exception& e)
    {
        PEGASUS_STD (cout) << "Exception: " << e.getMessage() << PEGASUS_STD(endl);
        exit(1);
    }

    PEGASUS_STD (cout) << argv[0] << " +++++ passed all tests" << PEGASUS_STD(endl);
    return 0;
}
