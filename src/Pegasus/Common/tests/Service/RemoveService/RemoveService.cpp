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
#include <windows.h>
#include <windows.h>
#include <iostream>

using namespace std;

void error(char* message)
{
    LPVOID lpMsgBuf;

    FormatMessage(
	FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
	NULL,
	GetLastError(),
	MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
	(LPTSTR)&lpMsgBuf,
	0,
	NULL);

    cout << "Error: " << (char*)lpMsgBuf << endl;
    // cout << message << endl;
    
    LocalFree(lpMsgBuf);
    exit(1);
}

int main(int argc, char** argv)
{
    if (argc != 2)
    {
	cerr << "Usage: " << argv[0] << " service-name" << endl;
	exit(1);
    }

    const char* serviceName = argv[1];

    // Connect to the SCM:

    SC_HANDLE scm = OpenSCManager(0, 0, SC_MANAGER_CREATE_SERVICE);

    // Open the service:

    SC_HANDLE service 
	= OpenService(scm, serviceName, SERVICE_ALL_ACCESS | DELETE);

    if (!service)
	error("OpenService() failed");
	
    // Stop the service:

    SERVICE_STATUS status;

    if (!QueryServiceStatus(service, &status))
	error("QueryServiceStatus() failed");

    if (status.dwCurrentState != SERVICE_STOPPED)
    {
	// Stop the service:

	if (!ControlService(service, SERVICE_CONTROL_STOP, &status))
	    error("ControlService() failed");

	// Give it some time to stop:

	Sleep(1000);
    }

    // Delete the service:
	
    if (!DeleteService(service))
	error("DeleteService() failed");

    // Close handles:

    CloseServiceHandle(service);
    CloseServiceHandle(scm);

    return 0;
}
