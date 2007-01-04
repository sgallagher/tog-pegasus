#include <stdlib.h>
#include <signal.h>
#include "Exit.h"
#include "Log.h"
#include "Globals.h"

//==============================================================================
//
// Exit()
//
//     The executor exit routine, which kills the cimservermain process.
//
//==============================================================================

void Exit(int status)
{
    Log(LL_INFORMATION, "exit(%d)", status);

    // Kill cimservermain.

    if (globalChildPid > 0)
        kill(globalChildPid, SIGTERM);

    exit(status);
}
