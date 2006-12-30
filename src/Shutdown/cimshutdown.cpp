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

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/Constants.h>
#include <Pegasus/Common/System.h>
#include <Pegasus/Client/CIMClient.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/signal.h>
#include <unistd.h>

#define PEGASUS_BUFFER_SIZE 1024

#ifdef PEGASUS_ENABLE_PRIVILEGE_SEPARATION
# define PEGASUS_PROCESS_NAME "cimservermain"
#else
# define PEGASUS_PROCESS_NAME "cimserver"
#endif

PEGASUS_USING_PEGASUS;
PEGASUS_USING_STD;

const char* arg0;

//==============================================================================
//
// _getProcessName()
//
//==============================================================================

#if defined(PEGASUS_OS_HPUX)

static int _getProcessName(int pid, char name[PEGASUS_BUFFER_SIZE])
{
    struct pst_status psts;

    if (pstat_getproc(&psts, sizeof(psts), 0, pid) == -1)
        return -1;

    name[0] = '\0';
    strncat(name, pstru.pst_ucomm, PEGASUS_BUFFER_SIZE);

    return 0;
}

#elif defined(PEGASUS_PLATFORM_LINUX_GENERIC_GNU)

static int _getProcessName(int pid, char name[PEGASUS_BUFFER_SIZE])
{
    // Read the process name from the file.

    static char buffer[1024];
    sprintf(buffer, "/proc/%d/stat", pid);
    FILE* is = fopen(buffer, "r");


    if (!is)
        return -1;

    // Read the first line of the file.

    if (fgets(buffer, sizeof(buffer), is) == NULL)
    {
        fclose(is);
        return -1;
    }

    fclose(is);

    // Extract the PID enclosed in parentheses.

    const char* start = strchr(buffer, '(');

    if (!start)
        return -1;

    start++;

    const char* end = strchr(start, ')');

    if (!end)
        return -1;

    if (start == end)
        return -1;

    name[0] = '\0';
    strncat(name, start, end - start);

    return 0;
}

#else
# error "not implemented on this platform."
#endif /* PEGASUS_PLATFORM_LINUX_GENERIC_GNU */

//==============================================================================
//
// _readPidFile()
//
//==============================================================================

static int _readPidFile(const char* path, int& pid)
{
    FILE* is = fopen(path, "r");

    if (!is) 
        return -1;

    pid = 0;

    fscanf(is, "%d\n", &pid);
    fclose(is);

    if (pid == 0)
        return -1;

    return 0;
}

//==============================================================================
//
// _killCimServer()
//
//     Kill the CIM server. Obtain the PID of the server from the PID file.
//
//==============================================================================

int _killCimServer()
{
    // Read the PID from the PID file.

    int pid;

    if (_readPidFile(PEGASUS_CIMSERVER_START_FILE, pid) != 0)
        return -1;

    // Kill the cimserver process:

    char name[PEGASUS_BUFFER_SIZE];

    if (_getProcessName(pid, name) != 0)
        return -1;

    if (strcmp(name, PEGASUS_PROCESS_NAME) != 0)
        return -1;

    kill(pid, SIGKILL);
  
    return 0;
}

//==============================================================================
//
// _killCimServer()
//
//     Kill the CIM server. Obtain the PID of the server from the PID file.
//
//==============================================================================

static void _waitForTerminationOrTimeout(Uint32 timeout)
{
    bool running = true;

    for (; running && timeout; timeout--)
    {
        // If PID file disappeared, process no longer running.

        int pid;

        if (_readPidFile(PEGASUS_CIMSERVER_START_FILE, pid) != 0)
        {
            running = false;
            break;
        }

        // If process name changed, process no longer running.

        char name[PEGASUS_BUFFER_SIZE];

        if (_getProcessName(pid, name) != 0 ||
            strcmp(name, PEGASUS_PROCESS_NAME) != 0)
        {
            running = false;
            break;
        }

        sleep(1);
    }
    while (0);

    if (running)
    {
        if (_killCimServer() == 0)
        {
            cerr << "Timeout expired forced shutdown initiated" << endl;
        }
    }
}

//==============================================================================
//
// _shutdown()
//
//==============================================================================

static void _shutdown(Uint32 timeout)
{
    // Connect locally to CIM server.

    CIMClient client;

    try
    {
        client.connectLocal();
        client.setTimeout(2000);
    }
    catch(Exception& e)
    {
        cerr << "Unable to connect to CIM Server." << endl;
        cerr << "CIM Server may not be running." << endl;
        exit(0);
    }

    // Invoke extrinsic PG_ShutdownService.shutdown() method.

    try
    {
        String referenceStr = "//";
        referenceStr.append(System::getHostName());
        referenceStr.append("/");
        referenceStr.append(PEGASUS_NAMESPACENAME_SHUTDOWN.getString());
        referenceStr.append(":");
        referenceStr.append(PEGASUS_CLASSNAME_SHUTDOWN.getString());
        CIMObjectPath reference(referenceStr);

        Array<CIMParamValue> inParams;
        inParams.append(CIMParamValue("force", CIMValue(Boolean(true))));

        inParams.append(CIMParamValue("timeout", CIMValue(Uint32(timeout))));
        Array<CIMParamValue> outParams;

        CIMValue retValue = client.invokeMethod(
            PEGASUS_NAMESPACENAME_SHUTDOWN,
            reference,
            "shutdown",
            inParams,
            outParams);
    }
    catch(CIMException& e)
    {
        cerr << arg0 << "Error in server shutdown: ";

        if (e.getCode() == CIM_ERR_INVALID_NAMESPACE)
            cerr << "The repository may be empty." << endl;
        else
            cerr << e.getMessage() << endl;

        if (_killCimServer() != 0)
            cerr << arg0 << ": failed to kill CIM server" << endl;
    }
    catch(Exception& e)
    {
        _waitForTerminationOrTimeout(timeout - 2);
    }

    _waitForTerminationOrTimeout(timeout);

    cerr << "CIM Server stopped." << endl;
}

int main(int argc, char** argv)
{
    arg0 = argv[0];

    // Seconds.
    _shutdown(5);
}
