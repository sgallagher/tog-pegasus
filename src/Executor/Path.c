/*
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
//%/////////////////////////////////////////////////////////////////////////////
*/
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

/*
**==============================================================================
**
** GetHomedPath()
**
**     Get the absolute path of the given named file or directory. If already
**     absolute it just returns. Otherwise, it prepends the PEGASUS_HOME
**     environment variable.
**
**==============================================================================
*/

int GetHomedPath(
    const char* name,
    char path[EXECUTOR_BUFFER_SIZE])
{
    const char* home;

    /* If absolute, then use the name as is. */

    if (name && name[0] == '/')
    {
        Strlcpy(path, name, EXECUTOR_BUFFER_SIZE);
        return 0;
    }

    /* Use PEGASUS_HOME to form path. */

    /* Flawfinder: ignore */
    if ((home = getenv("PEGASUS_HOME")) == NULL)
        return -1;

    Strlcpy(path, home, EXECUTOR_BUFFER_SIZE);

    if (name)
    {
        Strlcat(path, "/", EXECUTOR_BUFFER_SIZE);
        Strlcat(path, name, EXECUTOR_BUFFER_SIZE);
    }

    return 0;
}

/*
**==============================================================================
**
** GetPegasusInternalBinDir()
**
**     Get the Pegasus "lbin" directory. This is the directory that contains
**     internal Pegasus programs. Note that administrative tools are contained
**     in the "sbin" directory.
**
**==============================================================================
*/

int GetPegasusInternalBinDir(char path[EXECUTOR_BUFFER_SIZE])
{
    char* p;
    struct stat st;

    /* Make a copy of PEGASUS_PROVIDER_AGENT_PROC_NAME: */

    char buffer[EXECUTOR_BUFFER_SIZE];
    Strlcpy(buffer, PEGASUS_PROVIDER_AGENT_PROC_NAME, sizeof(buffer));

    /* Remove "cimprovagt" suffix. */

    p = strrchr(buffer, '/');

    if (!p)
        p = buffer;

    *p = '\0';

    /* If buffer path absolute, use this. */

    if (buffer[0] == '/')
    {
        Strlcat(path, buffer, EXECUTOR_BUFFER_SIZE);
    }
    else
    {
        /* Prefix with PEGASUS_HOME environment variable. */

        /* Flawfinder: ignore */
        const char* home = getenv("PEGASUS_HOME");

        if (!home)
            return -1;

        Strlcpy(path, home, EXECUTOR_BUFFER_SIZE);
        Strlcat(path, "/", EXECUTOR_BUFFER_SIZE);
        Strlcat(path, buffer, EXECUTOR_BUFFER_SIZE);
    }

    /* Fail if no such directory. */

    if (stat(path, &st) != 0)
        return -1;

    if (!S_ISDIR(st.st_mode))
        return -1;

    return 0;
}

/*
**==============================================================================
**
** GetInternalPegasusProgramPath()
**
**     Get the full path name of the given program.
**
**==============================================================================
*/

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
