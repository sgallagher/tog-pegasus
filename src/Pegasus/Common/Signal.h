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
// 	      : Yi Zhou, Hewlett-Packard Company (yi_zhou@hp.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_Signal_h
#define Pegasus_Signal_h

#include <Pegasus/Common/Linkage.h>
#include <Pegasus/Common/IPC.h>

// // Ensure Unix 98
// #ifdef PEGASUS_PLATFORM_LINUX_IX86_GNU
//    #ifndef _GNU_SOURCE
//       #define _GNU_SOURCE
//    #endif
// #else
//    #ifdef _XOPEN_SOURCE
//       #undef _XOPEN_SOURCE
//    #endif
//    #define _XOPEN_SOURCE 600
// #endif

#ifdef PEGASUS_HAS_SIGNALS

# include <signal.h>
typedef siginfo_t PEGASUS_SIGINFO_T;
# define PEGASUS_SIGHUP   SIGHUP
# define PEGASUS_SIGABRT  SIGABRT
# define PEGASUS_SIGPIPE  SIGPIPE
# define PEGASUS_SIGTERM  SIGTERM
# define PEGASUS_SIGUSR1  SIGUSR1
# define PEGASUS_SIGCHLD  SIGCHLD

#else // PEGASUS_HAS_SIGNALS

typedef void PEGASUS_SIGINFO_T;
# define PEGASUS_SIGHUP   1
# define PEGASUS_SIGABRT  11
# define PEGASUS_SIGPIPE  13
# define PEGASUS_SIGTERM  15
# define PEGASUS_SIGUSR1  16
# define PEGASUS_SIGCHLD  18

#endif // PEGASUS_HAS_SIGNALS


#if defined(PEGASUS_PLATFORM_ZOS_ZSERIES_IBM) || (PEGASUS_OS_SOLARIS)
extern "C" {
#endif

typedef void (* signal_handler)(int, PEGASUS_SIGINFO_T *, void *);

#if defined(PEGASUS_PLATFORM_ZOS_ZSERIES_IBM) || (PEGASUS_OS_SOLARIS)
}
#endif

// Sample signal handler for SIGABRT that stops the failing thread normally
void sig_act(int s_n, PEGASUS_SIGINFO_T * s_info, void * sig);

PEGASUS_NAMESPACE_BEGIN

class PEGASUS_COMMON_LINKAGE SignalHandler
{
public:
    SignalHandler();

    ~SignalHandler(); // deactivate all registered handlers

    // these functions should throw exceptions

    void registerHandler(unsigned signum, signal_handler _sighandler);

    void activate(unsigned signum);

    //void activateAll();

    void deactivate(unsigned signum);

    void deactivateAll();

    static void ignore(unsigned signum);

private:

#ifdef PEGASUS_HAS_SIGNALS
    enum
    {
        PEGASUS_NSIG = 32
    };

    static void verifySignum(unsigned signum);

    typedef struct {
        int signum;
        int active;
        signal_handler sh;
        struct sigaction oldsa;
    } register_handler;

    register_handler reg_handler[PEGASUS_NSIG + 1];
    Mutex reg_mutex;

    void deactivate_i(register_handler &rh);

    register_handler &getHandler(unsigned sigum);
#endif

};

PEGASUS_COMMON_LINKAGE SignalHandler * getSigHandle();

PEGASUS_NAMESPACE_END

#endif // Pegasus_Signal_h
