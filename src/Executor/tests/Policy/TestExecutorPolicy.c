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
//==============================================================================
//
//%/////////////////////////////////////////////////////////////////////////////
*/

#include <Executor/Policy.h>
#include <Executor/Macro.h>
#include <stdio.h>
#include <assert.h>

static struct Policy _testPolicyTable[] =
{
    {
        EXECUTOR_PING_MESSAGE,
        NULL,
        NULL
    },
    {
        EXECUTOR_RENAME_FILE_MESSAGE,
        "${file1}",
        "${file2}"
    },
    {
        EXECUTOR_RENAME_FILE_MESSAGE,
        "file1",
        "${file2}"
    },
    {
        EXECUTOR_RENAME_FILE_MESSAGE,
        "file1",
        "file2"
    }
};

static const size_t _testPolicyTableSize =
    sizeof(_testPolicyTable) / sizeof(_testPolicyTable[0]);

void testCheckPolicy()
{
    /* Test non-existent policy */
    assert(CheckPolicy(
        _testPolicyTable,
        _testPolicyTableSize,
        EXECUTOR_REAP_PROVIDER_AGENT,
        NULL,
        NULL) != 0);

    /* Test policy with no arguments */
    assert(CheckPolicy(
        _testPolicyTable,
        _testPolicyTableSize,
        EXECUTOR_PING_MESSAGE,
        NULL,
        NULL) == 0);

    /* Test policies with invalid macro expansion in first argument and
     * non-match in first argument
     */
    assert(CheckPolicy(
        _testPolicyTable,
        _testPolicyTableSize,
        EXECUTOR_RENAME_FILE_MESSAGE,
        "MyFile",
        "file2") != 0);

    /* Test policies with invalid macro expansion in second argument and
     * non-match in second argument
     */
    assert(CheckPolicy(
        _testPolicyTable,
        _testPolicyTableSize,
        EXECUTOR_RENAME_FILE_MESSAGE,
        "file1",
        "MyFile") != 0);

    /* Test policy with successful match in both arguments */
    assert(CheckPolicy(
        _testPolicyTable,
        _testPolicyTableSize,
        EXECUTOR_RENAME_FILE_MESSAGE,
        "file1",
        "file2") == 0);
}

void testFilePolicies()
{
    const char* currentConfigFile = "MyConfigFile";
    const char* currentConfigFileBak = "MyConfigFile.bak";
    const char* noAccessFile = "NoAccessFile";

    /* Define a macro used in the static policy table */
    DefineMacro("currentConfigFilePath", currentConfigFile);

    assert(CheckOpenFilePolicy(currentConfigFile, 'w') == 0);
    assert(CheckOpenFilePolicy(noAccessFile, 'w') != 0);

    assert(CheckRemoveFilePolicy(currentConfigFile) == 0);
    assert(CheckRemoveFilePolicy(noAccessFile) != 0);

    assert(CheckRenameFilePolicy(currentConfigFile, currentConfigFileBak) == 0);
    assert(CheckRenameFilePolicy(currentConfigFile, noAccessFile) != 0);
}

int main()
{
    testCheckPolicy();
    testFilePolicies();

    printf("+++++ passed all tests\n");

    return 0;
}
