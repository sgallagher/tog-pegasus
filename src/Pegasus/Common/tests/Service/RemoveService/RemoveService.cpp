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
