//%//-*-c++-*-/////////////////////////////////////////////////////////////////
// Copyright (c) 2000, 2001 The Open group, BMC Software, Tivoli Systems, IBM
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
// Author: Markus Mueller (markus_mueller@de.ibm.com
//
// Modified By: 
//
//%/////////////////////////////////////////////////////////////////////////////

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/Signal.h>
#include <Pegasus/Common/IPC.h>
#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/Exception.h>
#include <cstdio>
#include <unistd.h>

void sig_act(int s_n, siginfo_t * s_info, void * sig)
{
    void * retval = NULL;

    printf("Received a segmentation fault\n");
    printf(" in address %p\n", s_info->si_addr);
    printf(" pid %d\n", getpid());
    
    // in general it is dangerous to call pthread call
    // from within a signal handler, because
    // they are not signal safe

    pthread_exit(retval);
}

void * segmentation_faulter(void * parm)
{
    int * dataspace;

    printf("my pid is %d\n", getpid());
    dataspace = (int *) sbrk(0);
    dataspace++;
    *dataspace = 16;
    return NULL;
}


PEGASUS_NAMESPACE_BEGIN

SignalHandler::SignalHandler() : reg_mutex()
{
   for(Uint32 i=0;i < 32;i++)
   {
       reg_handler[i].active = 0;
       reg_handler[i].sh = NULL;
       memset(&reg_handler[i].oldsa,0,sizeof(struct sigaction));
   }
}

SignalHandler::~SignalHandler()
{
   deactivateAll();
}

void SignalHandler::registerHandler(Uint32 signum, signal_handler _sighandler)
{
    reg_mutex.lock(pegasus_thread_self());
    deactivate_i(signum);
    reg_handler[signum].sh = _sighandler;
    reg_mutex.unlock();
}

void SignalHandler::activate(Uint32 signum)
{
    reg_mutex.lock(pegasus_thread_self());
    if (reg_handler[signum].active) return; // throw exception

    struct sigaction * sig_acts = new struct sigaction;

    sig_acts->sa_sigaction = reg_handler[signum].sh;
    sigfillset(&(sig_acts->sa_mask));
#if defined(PEGASUS_PLATFORM_AIX_RS_IBMCXX) || defined(PEGASUS_PLATFORM_ZOS_ZSERIES_IBM)
    sig_acts->sa_flags = SA_SIGINFO | SA_RESETHAND;
#else
    sig_acts->sa_flags = SA_SIGINFO | SA_ONESHOT;
#if !defined(PEGASUS_PLATFORM_LINUX_IA64_GNU)
    sig_acts->sa_restorer = NULL;
#endif
#endif

    sigaction(signum, sig_acts, &reg_handler[signum].oldsa);

    reg_handler[signum].active = -1;
    reg_mutex.unlock();

    delete sig_acts;
}

void SignalHandler::deactivate(Uint32 signum)
{
    reg_mutex.lock(pegasus_thread_self());
    deactivate_i(signum);
    reg_mutex.unlock();
}

void SignalHandler::deactivate_i(Uint32 signum)
{
    if (reg_handler[signum].active)
    {
        reg_handler[signum].active = 0;
        sigaction(signum, &reg_handler[signum].oldsa, NULL);
    }
}

void SignalHandler::deactivateAll()
{
    reg_mutex.lock(pegasus_thread_self());
    for (Uint32 i=0; i < 32; i++)
        if (reg_handler[i].active) deactivate_i(i);
    reg_mutex.unlock();
}

void SignalHandler::ignore(Uint32 signum)
{
    ::sigignore(signum);
}


// export the global signal handling object

SignalHandler _globalSignalHandler;
SignalHandler * _globalSignalHandlerPtr = &_globalSignalHandler;
SignalHandler * getSigHandle() { return _globalSignalHandlerPtr; }


PEGASUS_NAMESPACE_END

