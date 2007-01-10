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

#include "Policy.h"
#include "Defines.h"
#include "Macro.h"
#include "Path.h"
#include "Fatal.h"
#include "Log.h"
#include "Match.h"
#include "Messages.h"
#include "Globals.h"
#include "Strlcat.h"
#include "Strlcpy.h"

/*
**==============================================================================
**
** Policy
**
**     This structure defines a policy rule.
**
**==============================================================================
*/

struct Policy
{
    enum ExecutorMessageCode messageCode;
    const char* arg1;
    const char* arg2;
};

/*
**==============================================================================
**
** _policyTable[]
**
**     This array defines the static policy table for the executor.
**
**==============================================================================
*/

static struct Policy _policyTable[] =
{
    /* cimserver_current.conf policies */
    {
        EXECUTOR_OPEN_FILE_MESSAGE,
        "${CIMSERVER_CURRENT_CONF}",
        "w"
    },
    { 
        EXECUTOR_RENAME_FILE_MESSAGE,
        "${CIMSERVER_CURRENT_CONF}",
        "${CIMSERVER_CURRENT_CONF_BAK}",
    },
    { 
        EXECUTOR_REMOVE_FILE_MESSAGE,
        "${CIMSERVER_CURRENT_CONF}",
        NULL,
    },
    { 
        EXECUTOR_REMOVE_FILE_MESSAGE,
        "${CIMSERVER_CURRENT_CONF_BAK}",
        NULL,
    },
    /* cimserver.passwd policies */
    {
        EXECUTOR_OPEN_FILE_MESSAGE,
        "${CIMSERVER_PASSWD}",
        "w"
    },
    {
        EXECUTOR_RENAME_FILE_MESSAGE,
        "${CIMSERVER_PASSWD_BAK}",
        "${CIMSERVER_PASSWD}",
    },
    {
        EXECUTOR_RENAME_FILE_MESSAGE,
        "${CIMSERVER_PASSWD}",
        "${CIMSERVER_PASSWD_BAK}",
    },
    {
        EXECUTOR_REMOVE_FILE_MESSAGE,
        "${CIMSERVER_PASSWD_BAK}",
        NULL,
    },
    {
        EXECUTOR_REMOVE_FILE_MESSAGE,
        "${CIMSERVER_PASSWD}",
        NULL,
    },
    /* cimserver.trc policies */
    {
        EXECUTOR_OPEN_FILE_MESSAGE,
        "${TRACE_FILE_PATH}/cimserver.trc*",
        "a",
    },
    /* SSL key file policies. */
    {
        EXECUTOR_OPEN_FILE_MESSAGE,
        "${SSL_KEY_FILE_PATH}",
        "r",
    },
    /* SSL trust store policies. */
    {
        EXECUTOR_OPEN_FILE_MESSAGE,
        "${SSL_TRUST_STORE}/*",
        "w",
    },
    {
        EXECUTOR_REMOVE_FILE_MESSAGE,
        "${SSL_TRUST_STORE}/*",
        NULL,
    },
    /* CRL store policies. */
    {
        EXECUTOR_OPEN_FILE_MESSAGE,
        "${CRL_STORE}/*",
        "w",
    },
    {
        EXECUTOR_REMOVE_FILE_MESSAGE,
        "${CRL_STORE}/*",
        NULL,
    },
};

static const size_t _policyTableSize = 
    sizeof(_policyTable) / sizeof(_policyTable[0]);

/*
**==============================================================================
**
** CheckPolicy()
**
**==============================================================================
*/

static int CheckPolicy(
    enum ExecutorMessageCode messageCode,
    const char* arg1,
    const char* arg2)
{
    size_t i;

    for (i = 0; i < _policyTableSize; i++)
    {
        const struct Policy* p;

        p = &_policyTable[i];

        /* Check message code */

        if (p->messageCode != messageCode)
            continue;

        /* Check arg1. */

        if (p->arg1)
        {
            char pat[EXECUTOR_BUFFER_SIZE];

            if (ExpandMacros(p->arg1, pat) != 0 || Match(pat, arg1) != 0)
            {
                continue;
            }
        }

        /* Check arg2. */

        if (p->arg2)
        {
            char pat[EXECUTOR_BUFFER_SIZE];

            if (ExpandMacros(p->arg2, pat) != 0 || Match(pat, arg2) != 0)
                continue;
        }

        // Found a matching policy!
        return 0;
    }

    // Failed to find any matching policy.

    return -1;
}

/*
**==============================================================================
**
** CheckOpenFilePolicy()
**
**==============================================================================
*/

int CheckOpenFilePolicy(const char* path, int mode)
{
    char arg2[2];

    arg2[0] = mode;
    arg2[1] = '\0';

    if (CheckPolicy(EXECUTOR_OPEN_FILE_MESSAGE, path, arg2) == 0)
    {
        Log(LL_TRACE, "CheckOpenFilePolicy(\"%s\", '%c') passed", path, mode);
        return 0;
    }

    Log(LL_SEVERE, "CheckOpenFilePolicy(\"%s\", '%c') failed", path, mode);

#if defined(EXIT_ON_POLICY_FAILURE)
    Fatal(FL, "exited due to policy failure");
#endif

    return -1;
}

/*
**==============================================================================
**
** CheckRemoveFilePolicy()
**
**==============================================================================
*/

int CheckRemoveFilePolicy(const char* path)
{
    if (CheckPolicy(EXECUTOR_REMOVE_FILE_MESSAGE, path, NULL) == 0)
    {
        Log(LL_TRACE, "CheckRemoveFilePolicy(\"%s\") passed", path);
        return 0;
    }

    Log(LL_SEVERE, "CheckRemoveFilePolicy(\"%s\") failed", path);

#if defined(EXIT_ON_POLICY_FAILURE)
    Fatal(FL, "exited due to policy failure");
#endif

    return -1;
}

/*
**==============================================================================
**
** CheckRenameFilePolicy()
**
**==============================================================================
*/

int CheckRenameFilePolicy(const char* oldPath, const char* newPath)
{
    if (CheckPolicy(EXECUTOR_RENAME_FILE_MESSAGE, oldPath, newPath) == 0)
    {
        Log(LL_TRACE, "CheckRenameFilePolicy(\"%s\", \"%s\") passed",
            oldPath, newPath);
        return 0;
    }

    Log(LL_SEVERE, "CheckRenameFilePolicy(\"%s\", \"%s\") failed",
        oldPath, newPath);

#if defined(EXIT_ON_POLICY_FAILURE)
    Fatal(FL, "exited due to policy failure");
#endif

    return -1;
}

/*
**==============================================================================
**
** DefinePolicyMacros()
**
**     Define macros used by the policies.
**
**==============================================================================
*/

void DefinePolicyMacros()
{
    /* Define ${CIMSERVER_CURRENT_CONF} */

    {
        char path[EXECUTOR_BUFFER_SIZE];

        if (GetHomedPath(PEGASUS_CURRENT_CONFIG_FILE_PATH, path) != 0)
        {
            Fatal(FL, "GetHomedPath() failed on \"%s\"", 
                PEGASUS_CURRENT_CONFIG_FILE_PATH);
        }

        DefineMacro("CIMSERVER_CURRENT_CONF", path);
    }

    /* Define ${CIMSERVER_CURRENT_CONF_BAK} */

    {
        char path[EXECUTOR_BUFFER_SIZE];

        if (GetHomedPath(PEGASUS_CURRENT_CONFIG_FILE_PATH, path) != 0)
        {
            Fatal(FL, "GetHomedPath() failed on \"%s\"", 
                PEGASUS_CURRENT_CONFIG_FILE_PATH);
        }

        Strlcat(path, ".bak", sizeof(path));
        DefineMacro("CIMSERVER_CURRENT_CONF_BAK", path);
    }

    /* Define ${CIMSERVER_PASSWD} */

    DefineMacro("CIMSERVER_PASSWD", globals.passwordFilePath);

    /* Define ${CIMSERVER_PASSWD_BAK} */

    {
        char path[EXECUTOR_BUFFER_SIZE];
        Strlcpy(path, globals.passwordFilePath, sizeof(path));
        Strlcat(path, ".bak", sizeof(path));

        DefineMacro("CIMSERVER_PASSWD_BAK", path);
    }

    /* Define ${TRACE_FILE_PATH} */

    DefineMacro("TRACE_FILE_PATH", globals.traceFilePath);

    /* Define ${SSL_KEY_FILE_PATH} */

    DefineMacro("SSL_KEY_FILE_PATH", globals.sslKeyFilePath);

    /* Define ${SSL_TRUST_STORE} */

    DefineMacro("SSL_TRUST_STORE", globals.sslTrustStore);

    /* Define ${SSL_TRUST_STORE} */

    DefineMacro("CRL_STORE", globals.crlStore);
}
