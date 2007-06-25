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

#include <Pegasus/Common/PegasusAssert.h>
#include <cstdlib>
#include <iostream>
#include <Pegasus/Common/HostAddress.h>

PEGASUS_USING_PEGASUS;
PEGASUS_USING_STD;

void testHostAddress()
{
    
    // Check for valid hostnames or addresses
    HostAddress hostaddr("1.222.33.44");
    PEGASUS_TEST_ASSERT(hostaddr.isValid());
    PEGASUS_TEST_ASSERT(!HostAddress::isValidHostName(hostaddr.getHost()));
    PEGASUS_TEST_ASSERT(HostAddress::isValidIPV4Address(hostaddr.getHost()));
    PEGASUS_TEST_ASSERT(!HostAddress::isValidIPV6Address(hostaddr.getHost()));
    PEGASUS_TEST_ASSERT(hostaddr.getHost() == String("1.222.33.44"));
    PEGASUS_TEST_ASSERT(hostaddr.getAddressType() == HostAddress::AT_IPV4);

    hostaddr.setHostAddress("1.2.3.4");
    HostAddress hostaddr2(hostaddr);
    PEGASUS_TEST_ASSERT(hostaddr2.isValid());
    PEGASUS_TEST_ASSERT(!HostAddress::isValidHostName(hostaddr2.getHost()));
    PEGASUS_TEST_ASSERT(HostAddress::isValidIPV4Address(hostaddr2.getHost()));
    PEGASUS_TEST_ASSERT(!HostAddress::isValidIPV6Address(hostaddr2.getHost()));
    PEGASUS_TEST_ASSERT(hostaddr2.getHost() == String("1.2.3.4"));
    PEGASUS_TEST_ASSERT(hostaddr2.getAddressType() == HostAddress::AT_IPV4);


    hostaddr.setHostAddress("fe00:ef::1");
    PEGASUS_TEST_ASSERT(hostaddr.isValid());
    PEGASUS_TEST_ASSERT(!HostAddress::isValidHostName(hostaddr.getHost()));
    PEGASUS_TEST_ASSERT(!HostAddress::isValidIPV4Address(hostaddr.getHost()));
    PEGASUS_TEST_ASSERT(HostAddress::isValidIPV6Address(hostaddr.getHost()));
    PEGASUS_TEST_ASSERT(hostaddr.getHost() == String("fe00:ef::1"));
    PEGASUS_TEST_ASSERT(hostaddr.getAddressType() == HostAddress::AT_IPV6);


    hostaddr.setHostAddress("fe00:1::");
    PEGASUS_TEST_ASSERT(hostaddr.isValid());
    PEGASUS_TEST_ASSERT(!HostAddress::isValidHostName(hostaddr.getHost()));
    PEGASUS_TEST_ASSERT(!HostAddress::isValidIPV4Address(hostaddr.getHost()));
    PEGASUS_TEST_ASSERT(HostAddress::isValidIPV6Address(hostaddr.getHost()));
    PEGASUS_TEST_ASSERT(hostaddr.getHost() == String("fe00:1::"));
    PEGASUS_TEST_ASSERT(hostaddr.getAddressType() == HostAddress::AT_IPV6);

    hostaddr.setHostAddress("::1");
    PEGASUS_TEST_ASSERT(hostaddr.isValid());
    PEGASUS_TEST_ASSERT(!HostAddress::isValidHostName(hostaddr.getHost()));
    PEGASUS_TEST_ASSERT(!HostAddress::isValidIPV4Address(hostaddr.getHost()));
    PEGASUS_TEST_ASSERT(HostAddress::isValidIPV6Address(hostaddr.getHost()));
    PEGASUS_TEST_ASSERT(hostaddr.getHost() == String("::1"));
    PEGASUS_TEST_ASSERT(hostaddr.getAddressType() == HostAddress::AT_IPV6);

    hostaddr.setHostAddress("fe00:1231:23fe:3456:acde:ebcd:12fe:ecd4");
    PEGASUS_TEST_ASSERT(hostaddr.isValid());
    PEGASUS_TEST_ASSERT(!HostAddress::isValidHostName(hostaddr.getHost()));
    PEGASUS_TEST_ASSERT(!HostAddress::isValidIPV4Address(hostaddr.getHost()));
    PEGASUS_TEST_ASSERT(HostAddress::isValidIPV6Address(hostaddr.getHost()));
    PEGASUS_TEST_ASSERT(hostaddr.getHost() == 
        String("fe00:1231:23fe:3456:acde:ebcd:12fe:ecd4"));
    PEGASUS_TEST_ASSERT(hostaddr.getAddressType() == HostAddress::AT_IPV6);

    hostaddr.setHostAddress("fe00:1231:23fe:3456::ebcd:12fe:ecd4");
    PEGASUS_TEST_ASSERT(hostaddr.isValid());
    PEGASUS_TEST_ASSERT(!HostAddress::isValidHostName(hostaddr.getHost()));
    PEGASUS_TEST_ASSERT(!HostAddress::isValidIPV4Address(hostaddr.getHost()));
    PEGASUS_TEST_ASSERT(HostAddress::isValidIPV6Address(hostaddr.getHost()));
    PEGASUS_TEST_ASSERT(hostaddr.getHost() ==
        String("fe00:1231:23fe:3456::ebcd:12fe:ecd4"));
    PEGASUS_TEST_ASSERT(hostaddr.getAddressType() == HostAddress::AT_IPV6);

    hostaddr.setHostAddress("123.34.abcd.xyz.com");
    PEGASUS_TEST_ASSERT(hostaddr.isValid());
    PEGASUS_TEST_ASSERT(HostAddress::isValidHostName(hostaddr.getHost()));
    PEGASUS_TEST_ASSERT(!HostAddress::isValidIPV4Address(hostaddr.getHost()));
    PEGASUS_TEST_ASSERT(!HostAddress::isValidIPV6Address(hostaddr.getHost()));
    PEGASUS_TEST_ASSERT(hostaddr.getHost() == String("123.34.abcd.xyz.com"));
    PEGASUS_TEST_ASSERT(hostaddr.getAddressType() == HostAddress::AT_HOSTNAME);


    // Invalid hostnames or addresses 
    hostaddr.setHostAddress("[1.2.3.4]:");
    PEGASUS_TEST_ASSERT(!hostaddr.isValid());
    PEGASUS_TEST_ASSERT(!HostAddress::isValidHostName(hostaddr.getHost()));
    PEGASUS_TEST_ASSERT(!HostAddress::isValidIPV4Address(hostaddr.getHost()));
    PEGASUS_TEST_ASSERT(!HostAddress::isValidIPV6Address(hostaddr.getHost()));

    hostaddr.setHostAddress("[1.2.3.4]:1234");
    PEGASUS_TEST_ASSERT(!hostaddr.isValid());
    PEGASUS_TEST_ASSERT(!HostAddress::isValidHostName(hostaddr.getHost()));
    PEGASUS_TEST_ASSERT(!HostAddress::isValidIPV4Address(hostaddr.getHost()));
    PEGASUS_TEST_ASSERT(!HostAddress::isValidIPV6Address(hostaddr.getHost()));

    hostaddr.setHostAddress("[1.2.3.4]");
    PEGASUS_TEST_ASSERT(!hostaddr.isValid());
    PEGASUS_TEST_ASSERT(!HostAddress::isValidHostName(hostaddr.getHost()));
    PEGASUS_TEST_ASSERT(!HostAddress::isValidIPV4Address(hostaddr.getHost()));
    PEGASUS_TEST_ASSERT(!HostAddress::isValidIPV6Address(hostaddr.getHost()));

    hostaddr.setHostAddress("[fe00:ef::1]");  // must not be enclosed in brackets.
    PEGASUS_TEST_ASSERT(!hostaddr.isValid());
    PEGASUS_TEST_ASSERT(!HostAddress::isValidHostName(hostaddr.getHost()));
    PEGASUS_TEST_ASSERT(!HostAddress::isValidIPV4Address(hostaddr.getHost()));
    PEGASUS_TEST_ASSERT(!HostAddress::isValidIPV6Address(hostaddr.getHost()));

    hostaddr.setHostAddress("[ffff1:2:3:4]");
    PEGASUS_TEST_ASSERT(!hostaddr.isValid());
    PEGASUS_TEST_ASSERT(!HostAddress::isValidHostName(hostaddr.getHost()));
    PEGASUS_TEST_ASSERT(!HostAddress::isValidIPV4Address(hostaddr.getHost()));
    PEGASUS_TEST_ASSERT(!HostAddress::isValidIPV6Address(hostaddr.getHost()));

    hostaddr.setHostAddress("[123]:123345");
    PEGASUS_TEST_ASSERT(!hostaddr.isValid());
    PEGASUS_TEST_ASSERT(!HostAddress::isValidHostName(hostaddr.getHost()));
    PEGASUS_TEST_ASSERT(!HostAddress::isValidIPV4Address(hostaddr.getHost()));
    PEGASUS_TEST_ASSERT(!HostAddress::isValidIPV6Address(hostaddr.getHost()));

    hostaddr.setHostAddress("[]");
    PEGASUS_TEST_ASSERT(!hostaddr.isValid());
    PEGASUS_TEST_ASSERT(!HostAddress::isValidHostName(hostaddr.getHost()));
    PEGASUS_TEST_ASSERT(!HostAddress::isValidIPV4Address(hostaddr.getHost()));
    PEGASUS_TEST_ASSERT(!HostAddress::isValidIPV6Address(hostaddr.getHost()));

    hostaddr.setHostAddress("[::1]"); 
    PEGASUS_TEST_ASSERT(!hostaddr.isValid());
    PEGASUS_TEST_ASSERT(!HostAddress::isValidHostName(hostaddr.getHost()));
    PEGASUS_TEST_ASSERT(!HostAddress::isValidIPV4Address(hostaddr.getHost()));
    PEGASUS_TEST_ASSERT(!HostAddress::isValidIPV6Address(hostaddr.getHost()));

    hostaddr.setHostAddress("[1::24]:");
    PEGASUS_TEST_ASSERT(!hostaddr.isValid());
    PEGASUS_TEST_ASSERT(!HostAddress::isValidHostName(hostaddr.getHost()));
    PEGASUS_TEST_ASSERT(!HostAddress::isValidIPV4Address(hostaddr.getHost()));
    PEGASUS_TEST_ASSERT(!HostAddress::isValidIPV6Address(hostaddr.getHost()));

    hostaddr.setHostAddress("[xyz.com]");
    PEGASUS_TEST_ASSERT(!hostaddr.isValid());
    PEGASUS_TEST_ASSERT(!HostAddress::isValidHostName(hostaddr.getHost()));
    PEGASUS_TEST_ASSERT(!HostAddress::isValidIPV4Address(hostaddr.getHost()));
    PEGASUS_TEST_ASSERT(!HostAddress::isValidIPV6Address(hostaddr.getHost()));

    hostaddr.setHostAddress("xyz.com:123241");
    PEGASUS_TEST_ASSERT(!hostaddr.isValid());
    PEGASUS_TEST_ASSERT(!HostAddress::isValidHostName(hostaddr.getHost()));
    PEGASUS_TEST_ASSERT(!HostAddress::isValidIPV4Address(hostaddr.getHost()));
    PEGASUS_TEST_ASSERT(!HostAddress::isValidIPV6Address(hostaddr.getHost()));
}

int main(int argc, char** argv)
{
    testHostAddress();
    cout << argv[0] << " +++++ passed all tests" << endl;

    return 0;
}
