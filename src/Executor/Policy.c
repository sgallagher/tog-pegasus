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
#include <unistd.h>
#include <ctype.h>
#include <fcntl.h>
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
** ARG()
**
**     Expands function arguments to "name, value" for use in formatted
**     output statements.
**
**     For example, this,
**
**         printf("%s=\"%s\"", ARG(count));
**
**     is expanded to this:
**
**         printf("%s=\"%s\"", "count", count);
**
**==============================================================================
*/

#define ARG(X) #X, X

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
    },
    { 
        EXECUTOR_RENAME_FILE_MESSAGE,
        "${currentConfigFilePath}",
        "${currentConfigFilePath}.bak",
    },
    { 
        EXECUTOR_REMOVE_FILE_MESSAGE,
        "${currentConfigFilePath}",
        NULL,
    },
    { 
        EXECUTOR_REMOVE_FILE_MESSAGE,
        "${currentConfigFilePath}.bak",
        NULL,
    },
    /* cimserver.passwd policies */
    {
        EXECUTOR_OPEN_FILE_MESSAGE,
        "${passwordFilePath}",
        "w",
    },
    {
        EXECUTOR_RENAME_FILE_MESSAGE,
        "${passwordFilePath}.bak",
        "${passwordFilePath}",
    },
    {
        EXECUTOR_RENAME_FILE_MESSAGE,
        "${passwordFilePath}",
        "${passwordFilePath}.bak",
    },
    {
        EXECUTOR_REMOVE_FILE_MESSAGE,
        "${passwordFilePath}.bak",
        NULL,
    },
    {
        EXECUTOR_REMOVE_FILE_MESSAGE,
        "${passwordFilePath}",
        NULL,
    },
    /* cimserver.trc policies */
    {
        EXECUTOR_OPEN_FILE_MESSAGE,
        "${traceFilePath}/cimserver.trc*",
        "a",
    },
    /* SSL key file policies. */
    {
        EXECUTOR_OPEN_FILE_MESSAGE,
        "${sslKeyFilePath}",
        "r",
    },
    /* SSL trust store policies. */
    {
        EXECUTOR_OPEN_FILE_MESSAGE,
        "${sslTrustStore}/*",
        "w",
    },
    {
        EXECUTOR_REMOVE_FILE_MESSAGE,
        "${sslTrustStore}/*",
        NULL,
    },
    /* CRL store policies. */
    {
        EXECUTOR_OPEN_FILE_MESSAGE,
        "${crlStore}/*",
        "w",
    },
    {
        EXECUTOR_REMOVE_FILE_MESSAGE,
        "${crlStore}/*",
        NULL,
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
        Log(LL_TRACE, "CheckOpenFilePolicy(%s=\"%s\", %s='%c') passed", 
            ARG(path), ARG(mode));
        return 0;
    }

    Log(LL_SEVERE, "CheckOpenFilePolicy(%s=\"%s\", %s='%c') failed", 
        ARG(path), ARG(mode));

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
        Log(LL_TRACE, "CheckRemoveFilePolicy(%s=\"%s\") passed", ARG(path));
        return 0;
    }

    Log(LL_SEVERE, "CheckRemoveFilePolicy(%s=\"%s\") failed", ARG(path));

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
        Log(LL_TRACE, "CheckRenameFilePolicy(%s=\"%s\", %s=\"%s\") passed",
            ARG(oldPath), ARG(newPath));
        return 0;
    }

    Log(LL_SEVERE, "CheckRenameFilePolicy(%s=\"%s\", %s=\"%s\") failed",
        ARG(oldPath), ARG(newPath));

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

int CheckStartProviderAgentPolicy(
    const char* providerModule, 
    const char* providerUser,
    const char* requestorUser)
{
    const char func[] = "CheckStartProviderAgentPolicy";

    /* Define ${requestorUser} since policy rule might use the macro. */

    DefineMacro("requestorUser", requestorUser);

    if (CheckPolicy(_dynamicPolicyTable, _dynamicPolicyTableSize,
        EXECUTOR_START_PROVIDER_AGENT_MESSAGE, 
        providerModule, providerUser) == 0)
    {
        Log(LL_TRACE, "%s(%s=\"%s\", %s=\"%s\", %s=\"%s\") passed", 
            func, ARG(providerModule), ARG(providerUser), ARG(requestorUser));
        UndefineMacro("requestorUser");
        return 0;
    }

    Log(LL_SEVERE, "%s(%s=\"%s\", %s=\"%s\", %s=\"%s\") failed", 
        func, ARG(providerModule), ARG(providerUser), ARG(requestorUser));

#if defined(EXIT_ON_POLICY_FAILURE)
    Fatal(FL, "exited due to policy failure");
#endif

    UndefineMacro("requestorUser");
    return -1;
}

/*
**==============================================================================
**
** ClearDynamicPolicy()
**
**     Clear the dynamic table structures.
**
**==============================================================================
*/

void ClearDynamicPolicy()
{
    size_t i;

    for (i = 0; i < _dynamicPolicyTableSize; i++)
    {
        struct Policy* policy = &_dynamicPolicyTable[i];

        if (policy->arg1)
            free((char*)policy->arg1);

        if (policy->arg2)
            free((char*)policy->arg2);
    }

    free(_dynamicPolicyTable);
    _dynamicPolicyTable = NULL;
    _dynamicPolicyTableSize = 0;
}

/*
**==============================================================================
**
** _LockFile()
**
**     Obtain an exclusive lock on the given file.
**
**==============================================================================
*/

static int _LockFile(FILE* fp)
{
    static struct flock lock;
    lock.l_type = F_RDLCK; 
    lock.l_whence = SEEK_SET; 
    lock.l_start = 0; 
    lock.l_len = 0; 
    return fcntl(fileno(fp), F_SETLKW, &lock);
}

/*
**==============================================================================
**
** _UnlockFile()
**
**     Release the lock on the given file.
**
**==============================================================================
*/

int _UnlockFile(FILE* fp)
{
    static struct flock lock;
    lock.l_type = F_UNLCK; 
    lock.l_whence = SEEK_SET; 
    lock.l_start = 0; 
    lock.l_len = 0; 
    return fcntl(fileno(fp), F_SETLKW, &lock);
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

    /* Obtain a lock on the file (wait until available). */

    if (_LockFile(is) != 0)
    {
        Fatal(FL, "failed to obtain a lock on policy configuration file: %s",
            path);
    }

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

    /* Release the lock on the file. */

    if (_UnlockFile(is) != 0)
    {
        Fatal(FL, "failed to obtain a lock on policy configuration file: %s",
            path);
    }

    /* Close the file. */

    fclose(is);
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
