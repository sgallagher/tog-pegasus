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

#ifndef _Executor_Defines_h
#define _Executor_Defines_h

#include <Pegasus/Common/Constants.h>

/*#define TRACE printf("TRACE: %s(%d)\n", __FILE__, __LINE__)*/

/*
**==============================================================================
**
** EXECUTOR_RESTART()
**
**     Macro used to repeatedly restart (retry) a system call as long as the
**     errno is EINTR.
**
**==============================================================================
*/

#define EXECUTOR_RESTART(F, X) while (((X = (F)) == -1) && (errno == EINTR))

/*
**==============================================================================
**
** EXECUTOR_BUFFER_SIZE
**
**     General purpose buffer size (large enough for any file path or user
**     name).
**
**==============================================================================
*/

#define EXECUTOR_BUFFER_SIZE 4096

/*
**==============================================================================
**
** FL
**
**     Shorthand macro for passing __FILE__ and __LINE__ arguments to a
**     function.
**
**==============================================================================
*/

#define FL __FILE__, __LINE__

/*
**==============================================================================
**
** CIMSERVERMAIN
**
**     The name of the main CIM server program.
**
**==============================================================================
*/

#define CIMSERVERMAIN "cimservermain"

/*
**==============================================================================
**
** CIMSHUTDOWN
**
**     The name of the main CIM shutdown program.
**
**==============================================================================
*/

#define CIMSHUTDOWN "cimshutdown"

/*
**==============================================================================
**
** CIMPROVAGT
**
**     The name of the provider agent program.
**
**==============================================================================
*/

#define CIMPROVAGT "cimprovagt"

/*
**==============================================================================
**
** EXECUTOR_FINGERPRINT
**
**     The executor passes this "fingerprint" as the first command line 
**     argument when executing internal Pegasus programs such as cimservermain
**     and cimshutdown. This is to prevent inadvertant execution of these
**     programs by end users. Note that this is not a security measure. It is
**     only a way to make it inconvenient to execute internal programs.
**
**==============================================================================
*/

#define EXECUTOR_FINGERPRINT "E97B2271E0E94DA8A2533FF9A9AA9443"

#endif /* _Executor_Defines_h */
