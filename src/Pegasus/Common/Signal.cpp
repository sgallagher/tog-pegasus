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
// Author: Markus Mueller (markus_mueller@de.ibm.com)
//
// Modified By: Roger Kumpf, Hewlett-Packard Company (roger_kumpf@hp.com)
//              Amit K Arora, IBM (amita@in.ibm.com) for Bug#1090
//              David Dillard, VERITAS Software Corp (david.dillard@veritas.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#include <cstdio>
#include <cstring>
#ifdef PEGASUS_PLATFORM_SOLARIS_SPARC_CC
#include <iostream.h>
#endif

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/Signal.h>
#include <Pegasus/Common/Exception.h>

PEGASUS_USING_PEGASUS;

void sig_act(int s_n, PEGASUS_SIGINFO_T * s_info, void * sig)
{
    PEGASUS_THREAD_RETURN retval = 0;

    if (s_n == PEGASUS_SIGABRT)
    {
        printf("Received an abort signal\n");
#if defined(PEGASUS_HAS_SIGNALS) && !defined(PEGASUS_OS_OS400)
        printf(" in address %p\n", s_info->si_addr);
#endif

        // In general it is dangerous to call pthread from within a
        // signal handler, because they are not signal safe
        exit_thread(retval);
    }
}


PEGASUS_NAMESPACE_BEGIN

#ifdef PEGASUS_HAS_SIGNALS

SignalHandler::SignalHandler()
{
    for(unsigned i=0;i <= PEGASUS_NSIG;i++)
    {
       register_handler &rh = reg_handler[i];
       rh.signum = i;
       rh.active = 0;
       rh.sh = NULL;
       memset(&rh.oldsa,0,sizeof(struct sigaction));
    }
}

SignalHandler::~SignalHandler()
{
    deactivateAll();
}

void SignalHandler::verifySignum(unsigned signum)
{
    if ( signum > PEGASUS_NSIG )
    {
        throw IndexOutOfBoundsException();
    }
}

SignalHandler::register_handler& 
SignalHandler::getHandler(unsigned signum)
{
    verifySignum(signum);
    return(reg_handler[signum]);
}

void SignalHandler::registerHandler(unsigned signum, signal_handler _sighandler)
{
    register_handler &rh = getHandler(signum);
    AutoMutex autoMut(reg_mutex);
    deactivate_i(rh);
    rh.sh = _sighandler;
}

void SignalHandler::activate(unsigned signum)
{
    register_handler &rh = getHandler(signum);
    AutoMutex autoMut(reg_mutex);
    if (rh.active)
    {
        return; // throw exception
    }

    struct sigaction sig_acts;

    sig_acts.sa_sigaction = rh.sh;
    sigfillset(&(sig_acts.sa_mask));
    sig_acts.sa_flags = SA_SIGINFO;

    sigaction(signum, &sig_acts, &rh.oldsa);

    rh.active = -1;
}

void SignalHandler::deactivate(unsigned signum)
{
    register_handler &rh = getHandler(signum);
    AutoMutex autoMut(reg_mutex);
    deactivate_i(rh);
}

void SignalHandler::deactivate_i(register_handler &rh)
{
    if (rh.active)
    {
        rh.active = 0;
        sigaction(rh.signum, &rh.oldsa, NULL);
    }
}

void SignalHandler::deactivateAll()
{
    AutoMutex autoMut(reg_mutex);
    for (unsigned i=0; i <= PEGASUS_NSIG; i++)
    {
        register_handler &rh = reg_handler[i];
        if (rh.active)
        {
            deactivate_i(rh);
        }
    }
}

void SignalHandler::ignore(unsigned signum)
{

    verifySignum(signum);

#if !defined(PEGASUS_PLATFORM_OS400_ISERIES_IBM) && !defined(PEGASUS_PLATFORM_DARWIN_PPC_GNU)
    sigignore(signum);
#else
    struct sigaction sig_acts;

    sig_acts.sa_handler = SIG_IGN;
    sigfillset(&(sig_acts.sa_mask));
    sig_acts.sa_flags = 0;

    sigaction(signum, &sig_acts, NULL);
#endif
}

#else // PEGASUS_HAS_SIGNALS

SignalHandler::SignalHandler() { }

SignalHandler::~SignalHandler() { }

void SignalHandler::registerHandler(unsigned signum, signal_handler _sighandler)
{ }

void SignalHandler::activate(unsigned signum) { }

void SignalHandler::deactivate(unsigned signum) { }

void SignalHandler::deactivateAll() { }

void SignalHandler::ignore(unsigned signum) { }

#endif // PEGASUS_HAS_SIGNALS


// export the global signal handling object

SignalHandler _globalSignalHandler;
SignalHandler * _globalSignalHandlerPtr = &_globalSignalHandler;
SignalHandler * getSigHandle() { return _globalSignalHandlerPtr; }


PEGASUS_NAMESPACE_END
