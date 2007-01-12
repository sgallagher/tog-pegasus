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

#include <string.h>
#include <ctype.h>
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

#define POLICY_FLAG_REQUESTOR 1

struct Policy
{
    enum ExecutorMessageCode messageCode;
    const char* arg1;
    const char* arg2;
    int flags;
};

/*
**==============================================================================
**
** _staticPolicyTable[]
**
**     This array defines the static policy table for the executor.
**
**==============================================================================
*/

static struct Policy _staticPolicyTable[] =
{
    /* cimserver_current.conf policies */
    {
        EXECUTOR_OPEN_FILE_MESSAGE,
        "${currentConfigFilePath}",
        "w",
        0
    },
    { 
        EXECUTOR_RENAME_FILE_MESSAGE,
        "${currentConfigFilePath}",
        "${currentConfigFilePath}.bak",
        0
    },
    { 
        EXECUTOR_REMOVE_FILE_MESSAGE,
        "${currentConfigFilePath}",
        NULL,
        0
    },
    { 
        EXECUTOR_REMOVE_FILE_MESSAGE,
        "${currentConfigFilePath}.bak",
        NULL,
        0
    },
    /* cimserver.passwd policies */
    {
        EXECUTOR_OPEN_FILE_MESSAGE,
        "${passwordFilePath}",
        "w",
        0
    },
    {
        EXECUTOR_RENAME_FILE_MESSAGE,
        "${passwordFilePath}.bak",
        "${passwordFilePath}",
        0
    },
    {
        EXECUTOR_RENAME_FILE_MESSAGE,
        "${passwordFilePath}",
        "${passwordFilePath}.bak",
        0
    },
    {
        EXECUTOR_REMOVE_FILE_MESSAGE,
        "${passwordFilePath}.bak",
        NULL,
        0
    },
    {
        EXECUTOR_REMOVE_FILE_MESSAGE,
        "${passwordFilePath}",
        NULL,
        0
    },
    /* cimserver.trc policies */
    {
        EXECUTOR_OPEN_FILE_MESSAGE,
        "${traceFilePath}/cimserver.trc*",
        "a",
        0
    },
    /* SSL key file policies. */
    {
        EXECUTOR_OPEN_FILE_MESSAGE,
        "${sslKeyFilePath}",
        "r",
        0
    },
    /* SSL trust store policies. */
    {
        EXECUTOR_OPEN_FILE_MESSAGE,
        "${sslTrustStore}/*",
        "w",
        0
    },
    {
        EXECUTOR_REMOVE_FILE_MESSAGE,
        "${sslTrustStore}/*",
        NULL,
        0
    },
    /* CRL store policies. */
    {
        EXECUTOR_OPEN_FILE_MESSAGE,
        "${crlStore}/*",
        "w",
        0
    },
    {
        EXECUTOR_REMOVE_FILE_MESSAGE,
        "${crlStore}/*",
        NULL,
        0
    },
};

static const size_t _staticPolicyTableSize = 
    sizeof(_staticPolicyTable) / sizeof(_staticPolicyTable[0]);

/*
**==============================================================================
**
** _dynamicPolicyTable[]
**
**     This array defines the dynamic policy table for the executor (this
**     includes the START_PROVIDER_AGENT policies.
**
**==============================================================================
*/
static struct Policy* _dynamicPolicyTable = 0;

static size_t _dynamicPolicyTableSize = 
    sizeof(_dynamicPolicyTable) / sizeof(_dynamicPolicyTable[0]);

/*
**==============================================================================
**
** CheckPolicy()
**
**==============================================================================
*/

static int CheckPolicy(
    const struct Policy* policyTable,
    size_t policyTableSize,
    enum ExecutorMessageCode messageCode,
    const char* arg1,
    const char* arg2)
{
    size_t i;

    for (i = 0; i < policyTableSize; i++)
    {
        const struct Policy* p;

        p = &policyTable[i];

        /* Check message code */

        if (p->messageCode != messageCode)
            continue;

        /* Check arg1. */

        if (p->arg1)
        {
            char pat[EXECUTOR_BUFFER_SIZE];

            if (ExpandMacros(p->arg1, pat) != 0 || Match(pat, arg1) != 0)
                continue;
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

    if (CheckPolicy(_staticPolicyTable, _staticPolicyTableSize, 
        EXECUTOR_OPEN_FILE_MESSAGE, path, arg2) == 0)
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
    if (CheckPolicy(_staticPolicyTable, _staticPolicyTableSize,
        EXECUTOR_REMOVE_FILE_MESSAGE, path, NULL) == 0)
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
    if (CheckPolicy(_staticPolicyTable, _staticPolicyTableSize, 
        EXECUTOR_RENAME_FILE_MESSAGE, oldPath, newPath) == 0)
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
** CheckStartProviderAgentPolicy()
**
**==============================================================================
*/

int CheckStartProviderAgentPolicy(const char* module, const char* user)
{
    if (CheckPolicy(_dynamicPolicyTable, _dynamicPolicyTableSize,
        EXECUTOR_START_PROVIDER_AGENT_MESSAGE, module, user) == 0)
    {
        Log(LL_TRACE, "CheckStartProviderAgentPolicy(\"%s\", \"%s\") passed", 
            module, user);
        return 0;
    }

    Log(LL_SEVERE, "CheckStartProviderAgentPolicy(\"%s\", \"%s\") failed", 
        module, user);

#if defined(EXIT_ON_POLICY_FAILURE)
    Fatal(FL, "exited due to policy failure");
#endif

    return -1;
}

/*
**==============================================================================
**
** LoadDynamicPolicy()
**
**     Load the dynamic policy file (cimserver_policy.conf). This file has
**     lines of the format.
**
**         <provider-module-name>:<username>
**
**     For example:
**
**         # This is the policy configuration file.
**         MyProviderModule:smith
**         YourProviderModule:jones
**
**==============================================================================
*/

void LoadDynamicPolicy()
{
    const char* path;
    FILE* is;
    char buffer[EXECUTOR_BUFFER_SIZE];
    int line;
    size_t r;

    /* Locate the policy configuration file. */

    if ((path = FindMacro("policyConfigFilePath")) == NULL)
        Fatal(FL, "failed form path of policy configuration file");

    /* Open the policy configuration file. */

    if ((is = fopen(path, "r")) == NULL)
        Fatal(FL, "failed to open the policy configuration file: %s", path);

    /* Process file line-by-line. */

    for (line = 1; fgets(buffer, sizeof(buffer), is) != NULL; line++)
    {
        char* p;
        struct Policy policy;

        /* Skip comment lines. */

        if (buffer[0] == '#')
            continue;

        /* Remove trailing whitespace. */

        r = strlen(buffer);

        while (r--)
        {
            if (isspace(buffer[r]))
                buffer[r] = '\0';
        }

        /* Skip empty lines. */

        if (buffer[0] == '\0')
            continue;

        /* Split line about the ':' character. */

        if ((p = strchr(buffer, ':')) == NULL)
        {
            Fatal(FL, "%s(%d): syntax error in policy configuration file; "
                "missing ':' separator character.",
                path, line);
        }

        *p = '\0';

        /* Create new policy object. */

        policy.messageCode = EXECUTOR_START_PROVIDER_AGENT_MESSAGE;
        policy.arg1 = strdup(buffer);
        policy.arg2 = strdup(p + 1);

        /* Append the policy to the _dynamicPolicyTable[]. */

        _dynamicPolicyTable = (struct Policy*)realloc(_dynamicPolicyTable, 
            (_dynamicPolicyTableSize + 1) * sizeof(struct Policy));

        _dynamicPolicyTable[_dynamicPolicyTableSize++] = policy;
    }

    /* Close the file. */

    fclose(is);

    /* Error out if no entries found (we need at least one entry). */

    if (_dynamicPolicyTableSize == 0)
    {
        Fatal(FL, "policy configuration file must have at least one entry: %s",
            path);
    }
}

/*
**==============================================================================
**
** _DumpPolicyHelper()
**
**     Dump the policy table given by *policyTable* and *policyTableSize*.
**     Expand any macros in the entries.
**
**==============================================================================
*/

static void _DumpPolicyHelper(
    const struct Policy* policyTable,
    size_t policyTableSize,
    int expandMacros)
{
    size_t i;

    for (i = 0; i < policyTableSize; i++)
    {
        const struct Policy* p = &policyTable[i];
        const char* codeStr = MessageCodeToString(p->messageCode);
        char arg1[EXECUTOR_BUFFER_SIZE];
        char arg2[EXECUTOR_BUFFER_SIZE];

        if (expandMacros)
        {
            ExpandMacros(p->arg1, arg1);

            if (p->arg2)
                ExpandMacros(p->arg2, arg2);
        }
        else
        {
            Strlcpy(arg1, p->arg1, sizeof(arg1));

            if (p->arg2)
                Strlcpy(arg2, p->arg2, sizeof(arg2));
        }

        if (p->arg2)
            printf("%s(\"%s\", \"%s\")\n", codeStr, arg1, arg2);
        else
            printf("%s(\"%s\")\n", codeStr, arg1);
    }
}

/*
**==============================================================================
**
** DumpPolicy()
**
**     Dump both the static and dynamic policy tables.
**
**==============================================================================
*/

void DumpPolicy(int expandMacros)
{
    printf("===== Policy:\n");

    _DumpPolicyHelper(
        _staticPolicyTable, _staticPolicyTableSize, expandMacros);

    _DumpPolicyHelper(
        _dynamicPolicyTable, _dynamicPolicyTableSize, expandMacros);

    putchar('\n');
}
