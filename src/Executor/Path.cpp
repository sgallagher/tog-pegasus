#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include "Path.h"
#include "Strlcpy.h"
#include "Strlcat.h"
#include "Config.h"
#include "Defines.h"

//==============================================================================
//
// GetHomedPath()
//
//     Get the absolute path of the given named file or directory. If already
//     absolute it just returns. Otherwise, it prepends the PEGASUS_HOME
//     environment variable.
//
//==============================================================================

int GetHomedPath(
    const char* name,
    char path[EXECUTOR_BUFFER_SIZE])
{
    // If absolute, then use the name as is.

    if (name[0] == '/')
    {
        Strlcpy(path, name, EXECUTOR_BUFFER_SIZE);
        return 0;
    }

    // Use PEGASUS_HOME to form path.

    /* Flawfinder: ignore */
    const char* home = getenv("PEGASUS_HOME");

    if (!home)
        return -1;

    Strlcpy(path, home, EXECUTOR_BUFFER_SIZE);
    Strlcat(path, "/", EXECUTOR_BUFFER_SIZE);
    Strlcat(path, name, EXECUTOR_BUFFER_SIZE);

    return 0;
}
//==============================================================================
//
// GetPegasusInternalBinDir()
//
//     Get the Pegasus "lbin" directory. This is the directory that contains
//     internal Pegasus programs. Note that administrative tools are contained
//     in the "sbin" directory.
//
//==============================================================================

int GetPegasusInternalBinDir(char path[EXECUTOR_BUFFER_SIZE])
{
    // Make a copy of PEGASUS_PROVIDER_AGENT_PROC_NAME:

    char buffer[EXECUTOR_BUFFER_SIZE];
    Strlcpy(buffer, PEGASUS_PROVIDER_AGENT_PROC_NAME, sizeof(buffer));

    // Remove "cimprovagt" suffix.

    char* p = strrchr(buffer, '/');

    if (!p)
        p = buffer;

    *p = '\0';

    // If buffer path absolute, use this.

    if (buffer[0] == '/')
    {
        Strlcat(path, buffer, EXECUTOR_BUFFER_SIZE);
    }
    else
    {
        // Prefix with PEGASUS_HOME environment variable.

        /* Flawfinder: ignore */
        const char* home = getenv("PEGASUS_HOME");

        if (!home)
            return -1;

        Strlcpy(path, home, EXECUTOR_BUFFER_SIZE);
        Strlcat(path, "/", EXECUTOR_BUFFER_SIZE);
        Strlcat(path, buffer, EXECUTOR_BUFFER_SIZE);
    }

    // Fail if no such directory.

    struct stat st;

    if (stat(path, &st) != 0)
        return -1;

    if (!S_ISDIR(st.st_mode))
        return -1;

    return 0;
}

//==============================================================================
//
// GetInternalPegasusProgramPath()
//
//     Get the full path name of the given program.
//
//==============================================================================

int GetInternalPegasusProgramPath(
    const char* program,
    char path[EXECUTOR_BUFFER_SIZE])
{
    if (GetPegasusInternalBinDir(path) != 0)
        return -1;

    Strlcat(path, "/", EXECUTOR_BUFFER_SIZE);
    Strlcat(path, program, EXECUTOR_BUFFER_SIZE);

    return 0;
}

//==============================================================================
//
// LocateRepositoryDirectory()
//
//==============================================================================

int LocateRepositoryDirectory(
    int argc, 
    char** argv, 
    char path[EXECUTOR_BUFFER_SIZE])
{
    if (GetConfigParam(argc, argv, "repositoryDir", path) == 0)
        return 0;

    if (GetHomedPath(PEGASUS_REPOSITORY_DIR, path) == 0)
        return 0;

    // Not found!
    return -1;
}
