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
// Author: Roger Kumpf, Hewlett-Packard Company (roger_kumpf@hp.com)
//         Jenny Yu, Hewlett-Packard Company (jenny_yu@hp.com)
//
// Modified By:
//
//%/////////////////////////////////////////////////////////////////////////////

#include <Pegasus/Common/String.h>
#include <Pegasus/Common/AnonymousPipe.h>
#include <Pegasus/Common/Tracer.h>
#include <Pegasus/Common/Logger.h>
#include <Pegasus/Common/System.h>
#include <Pegasus/Common/PegasusVersion.h>
#include <Pegasus/ProviderManager2/ProviderAgent/ProviderAgent.h>

PEGASUS_USING_STD;
PEGASUS_USING_PEGASUS;

void usage()
{
    cerr << "Usage: cimprovagt <input_pipe> <output_pipe> <id>" << endl;
}

//
// Dummy function for the Thread object associated with the initial thread.
// Since the initial thread is used to process CIM requests, this is
// needed to localize the exceptions thrown during CIM request processing.
// Note: This function should never be called! 
// 
PEGASUS_THREAD_RETURN PEGASUS_THREAD_CDECL dummyThreadFunc(void *parm)
{
    return(PEGASUS_THREAD_RETURN(0));
}

/////////////////////////////////////////////////////////////////////////
// MAIN
//////////////////////////////////////////////////////////////////////////

int main(int argc, char* argv[])
{
    //
    // Get the arguments from the command line
    // arg1 is the input pipe handle
    // arg2 is the output pipe handle
    // arg3 is the Provider Agent ID 
    //

    if (argc < 4)
    {
        usage();
        return 1;
    }

    try
    {
        AnonymousPipe pipeFromServer(argv[1], 0);
        AnonymousPipe pipeToServer(0, argv[2]);

        // Provider Agent ID argument is used for process identification
        String agentId = argv[3];
        Tracer::setModuleName(agentId + "." + System::getEffectiveUserName());

        // Set message loading not to use process locale
        MessageLoader::_useProcessLocale = false;

        // Create a dummy Thread object that can be used to store the
        // AcceptLanguages object for CIM requests that are serviced
        // by this thread (initial thread of cimprovagt).  Need to do this
        // because this thread is not in a ThreadPool, but is used
        // to service CIM requests.
        // The run function for the dummy Thread should never be called,
        Thread *dummyInitialThread = new Thread(dummyThreadFunc, NULL, false);
        Thread::setCurrent(dummyInitialThread); 
        AcceptLanguages default_al;
        try
        {
            default_al = AcceptLanguages::getDefaultAcceptLanguages();   
            Thread::setLanguages(new AcceptLanguages(default_al));
        }
        catch (InvalidAcceptLanguageHeader& e)
        {
            Logger::put_l(Logger::ERROR_LOG, System::CIMSERVER, Logger::SEVERE,
                "src.Server.cimserver.FAILED_TO_SET_PROCESS_LOCALE",
                "Could not convert the system process locale into a valid "
                    "AcceptLanguage format.");  
            Logger::put(Logger::ERROR_LOG, System::CIMSERVER, Logger::SEVERE,
                e.getMessage()); 
        }

        //
        // Instantiate and run the Provider Agent
        //
        ProviderAgent providerAgent(agentId, &pipeFromServer, &pipeToServer);
        providerAgent.run();
    }
    catch (Exception& e)
    {
        Logger::put_l(Logger::ERROR_LOG, System::CIMSERVER, Logger::SEVERE,
            "ProviderManager.ProviderAgent.cimprovagt.CIMPROVAGT_EXCEPTION",
            "cimprovagt \"$0\" error: $1", argv[3], e.getMessage());
        return 1;
    }
    catch (...)
    {
        Logger::put_l(Logger::ERROR_LOG, System::CIMSERVER, Logger::SEVERE,
            "ProviderManager.ProviderAgent.cimprovagt.CIMPROVAGT_ERROR",
            "cimprovagt \"$0\" error.  Exiting.", argv[3]);
        return 1;
    }

    return 0;
}
