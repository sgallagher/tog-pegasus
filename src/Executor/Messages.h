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

#ifndef _Executor_Messages_h
#define _Executor_Messages_h

/*
**==============================================================================
**
** Messages.h
**
**     This file defines messages exchanges over the socket between the
**     server and executor process.
**
**==============================================================================
*/

#include "Defines.h"

/*
**==============================================================================
**
** ExecutorMessageCode
**
**==============================================================================
*/

enum ExecutorMessageCode
{
    EXECUTOR_PING_MESSAGE = 1,
    EXECUTOR_OPEN_FILE_MESSAGE,
    EXECUTOR_START_PROVIDER_AGENT_MESSAGE,
    EXECUTOR_DAEMONIZE_EXECUTOR_MESSAGE,
    EXECUTOR_REMOVE_FILE_MESSAGE,
    EXECUTOR_RENAME_FILE_MESSAGE,
    EXECUTOR_WAIT_PID_MESSAGE,
    EXECUTOR_AUTHENTICATE_PASSWORD_MESSAGE,
    EXECUTOR_VALIDATE_USER_MESSAGE,
    EXECUTOR_CHALLENGE_LOCAL_MESSAGE,
    EXECUTOR_AUTHENTICATE_LOCAL_MESSAGE,
    EXECUTOR_NEW_SESSION_KEY_MESSAGE,
    EXECUTOR_DELETE_SESSION_KEY_MESSAGE
};

/*
**==============================================================================
**
** struct ExecutorRequestHeader
**
**==============================================================================
*/

struct ExecutorRequestHeader
{
    unsigned int code;
};

/*
**==============================================================================
**
** EXECUTOR_PING_MESSAGE
**
**==============================================================================
*/

#define EXECUTOR_PING_MAGIC 0x9E5EACB6

struct ExecutorPingResponse
{
    unsigned int magic;
};

/*
**==============================================================================
**
** EXECUTOR_OPEN_FILE_MESSAGE
**
**==============================================================================
*/

struct ExecutorOpenFileRequest
{
    char path[EXECUTOR_BUFFER_SIZE];
    /* ('r' = read, 'w' = write, 'a' = append) */
    int mode;
};

struct ExecutorOpenFileResponse
{
    int status;
};

/*
**==============================================================================
**
** EXECUTOR_REMOVE_FILE_MESSAGE
**
**==============================================================================
*/

struct ExecutorRemoveFileRequest
{
    char path[EXECUTOR_BUFFER_SIZE];
};

struct ExecutorRemoveFileResponse
{
    int status;
};

/*
**==============================================================================
**
** EXECUTOR_RENAME_FILE_MESSAGE
**
**==============================================================================
*/

struct ExecutorRenameFileRequest
{
    char oldPath[EXECUTOR_BUFFER_SIZE];
    char newPath[EXECUTOR_BUFFER_SIZE];
};

struct ExecutorRenameFileResponse
{
    int status;
};

/*
**==============================================================================
**
** EXECUTOR_START_PROVIDER_AGENT_MESSAGE
**
**==============================================================================
*/

struct ExecutorStartProviderAgentRequest
{
    char key[EXECUTOR_BUFFER_SIZE];
    char module[EXECUTOR_BUFFER_SIZE];
    int uid;
    int gid;
};

struct ExecutorStartProviderAgentResponse
{
    int status;
    int pid;
    char key[EXECUTOR_BUFFER_SIZE];
};

/*
**==============================================================================
**
** EXECUTOR_DAEMONIZE_EXECUTOR_MESSAGE
**
**==============================================================================
*/

struct ExecutorDaemonizeExecutorResponse
{
    int status;
};

/*
**==============================================================================
**
** EXECUTOR_WAIT_PID_MESSAGE
**
**==============================================================================
*/

struct ExecutorWaitPidRequest
{
    int pid;
};

struct ExecutorWaitPidResponse
{
    int status;
};

/*
**==============================================================================
**
** EXECUTOR_AUTHENTICATE_PASSWORD_MESSAGE
**
**==============================================================================
*/

struct ExecutorAuthenticatePasswordRequest
{
    char username[EXECUTOR_BUFFER_SIZE];
    char password[EXECUTOR_BUFFER_SIZE];
};

struct ExecutorAuthenticatePasswordResponse
{
    int status;
    char key[EXECUTOR_BUFFER_SIZE];
};

/*
**==============================================================================
**
** EXECUTOR_VALIDATE_USER_MESSAGE
**
**==============================================================================
*/

struct ExecutorValidateUserRequest
{
    char username[EXECUTOR_BUFFER_SIZE];
};

struct ExecutorValidateUserResponse
{
    int status;
};

/*
**==============================================================================
**
** EXECUTOR_CHALLENGE_LOCAL_MESSAGE
**
**==============================================================================
*/

struct ExecutorChallengeLocalRequest
{
    char user[EXECUTOR_BUFFER_SIZE];
};

struct ExecutorChallengeLocalResponse
{
    int status;
    char challenge[EXECUTOR_BUFFER_SIZE];
    char key[EXECUTOR_BUFFER_SIZE];
};

/*
**==============================================================================
**
** EXECUTOR_AUTHENTICATE_LOCAL_MESSAGE
**
**==============================================================================
*/

struct ExecutorAuthenticateLocalRequest
{
    char key[EXECUTOR_BUFFER_SIZE];
    char token[EXECUTOR_BUFFER_SIZE];
};

struct ExecutorAuthenticateLocalResponse
{
    int status;
};

/*
**==============================================================================
**
** EXECUTOR_NEW_SESSION_KEY_MESSAGE
**
**==============================================================================
*/

struct ExecutorNewSessionKeyRequest
{
    char username[EXECUTOR_BUFFER_SIZE];
};

struct ExecutorNewSessionKeyResponse
{
    int status;
    char key[EXECUTOR_BUFFER_SIZE];
};

/*
**==============================================================================
**
** EXECUTOR_DELETE_SESSION_KEY_MESSAGE
**
**==============================================================================
*/

struct ExecutorDeleteSessionKeyRequest
{
    char key[EXECUTOR_BUFFER_SIZE];
};

struct ExecutorDeleteSessionKeyResponse
{
    int status;
};

#endif /* _Executor_Messages_h */
