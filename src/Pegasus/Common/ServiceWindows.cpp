//%/////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001 The Open group, BMC Software, Tivoli Systems, IBM
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
// Author: Mike Brasher (mbrasher@bmc.com)
//
// Modified By:
//
//%/////////////////////////////////////////////////////////////////////////////

#include <windows.h>
#include <cstdio>
#include <string>
#include <fstream>
#include <cstdlib>
#include "Service.h"

PEGASUS_NAMESPACE_BEGIN

using namespace std;

struct ClientData
{
    int argc;
    char** argv;
};

static const char* _serviceName = "";
static HANDLE _terminateEvent = NULL;
static SERVICE_STATUS_HANDLE _serviceStatusHandle = NULL;
static HANDLE _threadHandle = 0;
static BOOL _serviceRunning = FALSE;
static BOOL _servicePaused = FALSE;
static SERVICE_TABLE_ENTRY _serviceTable[2];
static ServiceHandler* _serviceHandler = 0;
static ClientData _clientData;

static void _stopService() 
{
    _serviceRunning = FALSE;
    SetEvent(_terminateEvent);
}

static BOOL _sendStatusToSCM(
    DWORD dwCurrentState,
    DWORD dwWin32ExitCode, 
    DWORD dwServiceSpecificExitCode,
    DWORD dwCheckPoint,
    DWORD dwWaitHint)
{
    SERVICE_STATUS serviceStatus;

    serviceStatus.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
    serviceStatus.dwCurrentState = dwCurrentState;

    if (dwCurrentState == SERVICE_START_PENDING)
	serviceStatus.dwControlsAccepted = 0;
    else
    {
	serviceStatus.dwControlsAccepted = 
	    SERVICE_ACCEPT_STOP |
	    SERVICE_ACCEPT_PAUSE_CONTINUE |
	    SERVICE_ACCEPT_SHUTDOWN;
    }

    if (dwServiceSpecificExitCode == 0)
	serviceStatus.dwWin32ExitCode = dwWin32ExitCode;
    else
	serviceStatus.dwWin32ExitCode = ERROR_SERVICE_SPECIFIC_ERROR;

    serviceStatus.dwServiceSpecificExitCode = dwServiceSpecificExitCode;
    serviceStatus.dwCheckPoint = dwCheckPoint;
    serviceStatus.dwWaitHint = dwWaitHint;

    BOOL success = SetServiceStatus(_serviceStatusHandle, &serviceStatus);

    if (!success)
	_stopService();

    return success;
}

static void _resumeService() 
{
    _servicePaused = FALSE;
    ResumeThread(_threadHandle);
}

static void _pauseService() 
{
    _servicePaused = TRUE;
    SuspendThread(_threadHandle);
}

static void _terminate(DWORD error)
{
    if (_terminateEvent)
	CloseHandle(_terminateEvent);

    if (_serviceStatusHandle)
	_sendStatusToSCM(SERVICE_STOPPED, error, 0, 0, 0);

    if (_threadHandle)
	CloseHandle(_threadHandle);
}

static DWORD _serviceThread(LPDWORD param)
{
    ClientData* clientData = (ClientData*)param;
    int status = _serviceHandler->main(clientData->argc, clientData->argv);
    return status;
}

static void _serviceCtrlHandler(DWORD controlCode) 
{
    DWORD  currentState = 0;

    switch(controlCode)
    {
	case SERVICE_CONTROL_STOP:
	{
	    SERVICE_STOP_PENDING;
	    _sendStatusToSCM( SERVICE_STOP_PENDING, NO_ERROR, 0, 1, 5000);
	    _stopService();
	    _serviceHandler->stop();
	    return;
	}

	case SERVICE_CONTROL_PAUSE:
	{
	    if (_serviceRunning && !_servicePaused)
	    {
		_sendStatusToSCM(SERVICE_PAUSE_PENDING, NO_ERROR, 0, 1, 1000);
		_pauseService();
		currentState = SERVICE_PAUSED;
		_serviceHandler->pause();
	    }
	    break;
	}

	case SERVICE_CONTROL_CONTINUE:
	{
	    if (_serviceRunning && _servicePaused)
	    {
		_sendStatusToSCM( 
		    SERVICE_CONTINUE_PENDING, NO_ERROR, 0, 1, 1000);
		_resumeService();
		currentState = SERVICE_RUNNING;
		_serviceHandler->resume();
	    }
	    break;
	}

	case SERVICE_CONTROL_INTERROGATE:
	case SERVICE_CONTROL_SHUTDOWN:
	default:
	    break;
    }

    _sendStatusToSCM(currentState, NO_ERROR, 0, 0, 0);
}

static void _serviceMain(DWORD argc, LPTSTR *argv) 
{
    // Register the service control handler:

    _serviceStatusHandle = RegisterServiceCtrlHandler(
	_serviceName, (LPHANDLER_FUNCTION)_serviceCtrlHandler);

    if (!_serviceStatusHandle)
    {
	_terminate(GetLastError()); 
	return;
    }

    if (!_sendStatusToSCM(SERVICE_START_PENDING, NO_ERROR, 0, 1, 5000))
    {
	_terminate(GetLastError()); 
	return;
    }

    if (!(_terminateEvent = CreateEvent (0, TRUE, FALSE, 0)))
    {
	_terminate(GetLastError());
	return;
    }

    if (!_sendStatusToSCM(SERVICE_START_PENDING, NO_ERROR, 0, 2, 1000))
    {
	_terminate(GetLastError()); 
	return;
    }

    if (!_sendStatusToSCM(SERVICE_START_PENDING, NO_ERROR, 0, 3, 5000))
    {
	_terminate(GetLastError()); 
	return;
    }

    // Create the service thread:

    _clientData.argc = argc;
    _clientData.argv = argv;
    DWORD id;

    _threadHandle = CreateThread(
	0, 0, (LPTHREAD_START_ROUTINE)_serviceThread, &_clientData, 0, &id); 

    if (_threadHandle)
	_serviceRunning = TRUE;
    else
    {
	_terminate(GetLastError());
	return;
    }

    // Send status to SCM:

    if (!_sendStatusToSCM( SERVICE_RUNNING, NO_ERROR, 0, 0, 0))
    {
	_terminate(GetLastError()); 
	return;
    }

    // Wait for termination!

    WaitForSingleObject(_terminateEvent, INFINITE);
    _terminate(0);
}

bool Service::run(
    int argc,
    char** argv,
    const char* serviceName, 
    ServiceHandler* serviceHandler,
    bool detach)
{
    if (!detach)
    {
        serviceHandler->main(argc, argv);
        return false;
    }

    _serviceHandler = serviceHandler;

    _serviceName = serviceName;
    _serviceTable[0].lpServiceName = (char*)serviceName;
    _serviceTable[0].lpServiceProc = (LPSERVICE_MAIN_FUNCTION)_serviceMain;
    _serviceTable[1].lpServiceName = NULL;
    _serviceTable[1].lpServiceProc = NULL;

    return StartServiceCtrlDispatcher(_serviceTable) ? true : false;
}

PEGASUS_NAMESPACE_END
