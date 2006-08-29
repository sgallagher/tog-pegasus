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

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/PegasusAssert.h>
#include <Pegasus/HandlerService/HandlerTable.h>

PEGASUS_USING_PEGASUS;
PEGASUS_USING_STD;

int main(int argc, char** argv)
{
    const char* pegasusHome = getenv("PEGASUS_HOME");
    if (!pegasusHome)
    {
        cerr << argv[0] << ": Test skipped because PEGASUS_HOME is not defined"
             << endl;
        return 1;
    }
    ConfigManager::setPegasusHome(pegasusHome);

    try
    {
        HandlerTable handlerTable;
        String handlerId;
        CIMHandler* handler;
        CIMRepository* repository = 0;

        //
        //  Test getHandler - get a handler that is not yet loaded
        //
        handlerId = "CIMxmlIndicationHandler";
        handler = handlerTable.getHandler(handlerId, repository);
        PEGASUS_TEST_ASSERT(handler != 0);

        //
        //  Test getHandler - get a handler that is already loaded
        //
        handlerId = "CIMxmlIndicationHandler";
        handler = handlerTable.getHandler(handlerId, repository);
        PEGASUS_TEST_ASSERT(handler != 0);

        //
        //  Test getHandler - try to get a nonexistent handler
        //  Specified handler library does not exist
        //
        Boolean dynamicLoadFailedCaught = false;
        try
        {
            handlerId = "NonexistentHandler";
            handler = handlerTable.getHandler(handlerId, repository);
        }
        catch(const DynamicLoadFailed&)
        {
            dynamicLoadFailedCaught = true;                      
        }
        PEGASUS_TEST_ASSERT(dynamicLoadFailedCaught);

        // this test currently is broken on z/OS due to a problem
        // with the dlsym call
        // TODO: when problem fixed, reenable the test
#ifndef PEGASUS_OS_ZOS
        //
        //  Test getHandler - try to get a nonexistent handler
        //  handlerId specifies a valid library name but library is missing
        //  the PegasusCreateHandler function
        //
        Boolean dynamicLookupFailedCaught = false;
        try
        {
            handlerId = "MissingEntryPointHandler";
            handler = handlerTable.getHandler(handlerId, repository);
        }
        catch(const DynamicLookupFailed&)
        {
            dynamicLookupFailedCaught = true;                      
        }
        PEGASUS_TEST_ASSERT(dynamicLookupFailedCaught);
#endif
    }
    catch(const Exception& e)
    {
        cerr << argv[0] << ": Exception " << e.getMessage() << endl;
        return 1;
    }
    catch(...)
    {
        cerr << argv[0] << ": " << "Unknown error" << endl;
        return 1;
    }

    cout << "+++++ passed all tests" << endl;
    return 0;
}
