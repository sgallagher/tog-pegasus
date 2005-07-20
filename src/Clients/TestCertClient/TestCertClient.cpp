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
// Author: Heather Sterling (hsterl@us.ibm.com)
//
//
//%/////////////////////////////////////////////////////////////////////////////

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/Constants.h>
#include <Pegasus/Common/PegasusVersion.h>
#include <cassert>
#include <Pegasus/Common/TLS.h>
#include <Pegasus/Client/CIMClient.h>
#include <Pegasus/Common/CIMName.h>
#include <Pegasus/Common/Exception.h>

PEGASUS_USING_PEGASUS;
PEGASUS_USING_STD;

/** This client is primarily used to test the SSL client verification function.  The client is used by the poststarttests
 *  of the ssltrustmgr CLI.  This client was introduced because existing test clients are heavy on function and thus take a long
 *  time to return.  We don't care about the response, only whether the request was authenticated.
 */ 

int main(int argc, char** argv)
{
    if (argc < 3)
    {
        PEGASUS_STD(cout) << "Wrong number of arguments" << PEGASUS_STD(endl);
        exit(1);
    }

    String certpath = argv[1];
    String keypath = argv[2];
	String randFile = String::EMPTY;

	if (argc == 4)
	{
		randFile = argv[3];
	}

    try
    {
        SSLContext ctx(String::EMPTY, certpath, keypath, 0, randFile);
    
        PEGASUS_STD(cout)<< "TestCertClient::Connecting to 127.0.0.1:5989" << PEGASUS_STD(endl);
    
        CIMClient client;
        client.connect("127.0.0.1", 5989, ctx, String::EMPTY, String::EMPTY);
    
        //perform a quick CIM operation call -- it doesn't matter what this is, as long as it's guaranteed to be in the repository
        CIMClass c = client.getClass("root/cimv2", "CIM_ComputerSystem", false, false, true);
    
        PEGASUS_STD(cout) << "Result: " << c.getClassName().getString() << PEGASUS_STD(endl);

    } catch (Exception& ex)
    {
        PEGASUS_STD(cout) << "Exception: " << ex.getMessage() << PEGASUS_STD(endl);
        exit(1);

    } catch (...)
    {
        PEGASUS_STD(cout) << "Unknown exception" << PEGASUS_STD(endl);
        exit(1);
    }

    PEGASUS_STD(cout) << "+++++ "<< "TestCertClient" << " Terminated Normally" << PEGASUS_STD(endl);
    exit(0);

    return 0;
}

