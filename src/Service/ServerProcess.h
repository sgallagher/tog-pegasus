//%2005////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001, 2002 BMC Software; Hewlett-Packard Development
// Company, L.P.; IBM Corp.; The Open Group; Tivoli Systems.
// Copyright (c) 2003 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation, The Open Group.
// Copyright (c) 2004 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation; VERITAS Software Corporation; The Open Group.
// Copyright (c) 2005 Hewlett-Packard Development Company, L.P.; IBM Corp.;
// EMC Corporation; VERITAS Software Corporation; The Open Group.
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
// Author: Heather Sterling (hsterl@us.ibm.com) PEP#222
//
//%/////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_Server_Process_h
#define Pegasus_Server_Process_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/String.h>
#include "Linkage.h"

PEGASUS_NAMESPACE_BEGIN

/** This abstract class has virtual methods for information that varies across applications. The rest of the methods 
  * are called from the application (i.e. cimserver.cpp), and need to be defined by every operating system implementation. 
  * This version will not touch the method names, as it is fairly risky to do so now. However, the goal is to 
  * eventually standardize the interface so we pull out as much OS-specific function as possible from the main cimserver file.
  *
  * Not all operating systems need to fully implement all these methods.  Stick methods which do not apply to
  * your OS in a "No-ops" section at the top.
  * 
  * See PEP#222 for more information.
  */ 

class PEGASUS_SERVICE_LINKAGE ServerProcess
{
public:

    ServerProcess(void);

    virtual ~ServerProcess(void);

    virtual const char* getProductName() const = 0;

    virtual const char* getExtendedName() const = 0;

    virtual const char* getDescription() const = 0;

    virtual const char* getVersion() const = 0;
    
    virtual const char* getProcessName() const = 0;

    virtual const char* getPIDFileName() const = 0;

    virtual int cimserver_run(int argc, char** argv, bool shutdownOption) = 0;

    virtual void cimserver_stop(void) = 0;

    int platform_run( int argc, char** argv, bool shutdownOption);

    int cimserver_fork(void);

    void notify_parent(int id);

    long get_server_pid(void);

    void set_parent_pid(int pid);

    int get_proc(int pid);

    bool isCIMServerRunning(void);

    void cimserver_set_process(void* p);

    int cimserver_kill(int id);

    void cimserver_exitRC(int rc);

    int cimserver_initialize(void);

    int cimserver_wait(void);

    String getHome(void);
};

PEGASUS_NAMESPACE_END

#endif // Pegasus_Server_Process_h
