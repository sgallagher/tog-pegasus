//%/////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001, 2002 BMC Software, Hewlett-Packard Company, IBM,
// The Open Group, Tivoli Systems
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


#ifndef Pegasus_SignalHandler_h
#define Pegasus_SignalHandler_h

// REVIEW: Where is this signal handling code used?

// REVIEW: Is there an equivalent implementation for Windows?


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

#include <signal.h>

#if defined(PEGASUS_PLATFORM_ZOS_ZSERIES_IBM)
extern "C" {
#endif

typedef void (* signal_handler)(int,siginfo_t *,void *);

typedef struct {
    int active;
    signal_handler sh;
    struct sigaction oldsa;
} register_handler;

#if defined(PEGASUS_PLATFORM_ZOS_ZSERIES_IBM)
}
#endif

#include <Pegasus/Common/Thread.h>
#include <Pegasus/Common/DQueue.h>
#include <Pegasus/Common/InternalException.h>
#include <Pegasus/Common/Linkage.h>

// used locally as standard response to stop the failing thread normally
void sig_act(int s_n, siginfo_t * s_info, void * sig);
// test routine, that just causes a segmentation fault
void * segmentation_faulter(void * parm);

PEGASUS_NAMESPACE_BEGIN

class PEGASUS_COMMON_LINKAGE SignalHandler
{
   public:
      SignalHandler();

      ~SignalHandler(); // deactivate all registered handlers

      // these functions should throw exceptions

      void registerHandler(Uint32 signum, signal_handler _sighandler);

      void activate(Uint32 signum);

      //void activateAll();

      void deactivate(Uint32 signum);

      void deactivateAll();

      static void ignore(Uint32 signum);

   private:

      register_handler reg_handler[32];
      Mutex reg_mutex;

      void deactivate_i(Uint32 signum);

};

SignalHandler * getSigHandle();

PEGASUS_NAMESPACE_END

#endif // Pegasus_SignalHandler_h
