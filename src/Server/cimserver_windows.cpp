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
// Author: Mike Day (mdday@us.ibm.com)
//
// Modified By:
//
//%/////////////////////////////////////////////////////////////////////////////

#include <windows.h>
#include <process.h>    /* _beginthread, _endthread */
#include <tchar.h>
#include <direct.h>


PEGASUS_USING_PEGASUS;
PEGASUS_USING_STD;

static DWORD dieNow = 0;
String *runPath;
CIMServer *server_windows;
static SERVICE_STATUS pegasus_status;
static SERVICE_STATUS_HANDLE pegasus_status_handle;

VOID WINAPI  cimserver_windows_main(int argc, char **argv) ;
VOID WINAPI cimserver_service_start(DWORD, LPTSTR *);
VOID WINAPI cimserver_service_ctrl_handler(DWORD ); 
DWORD cimserver_initialization(DWORD, LPTSTR *, DWORD *) ;

void GetOptions(
    OptionManager& om,
    int& argc,
    char** argv,
    const String& pegasusHome) ;


void cim_server_service(int argc, char **argv ) { cimserver_windows_main(argc, argv); exit(0); }
int cimserver_fork( ) { return(0); }

static void __cdecl cimserver_windows_thread(void *parm) 
{

    // Get options (from command line and from configuration file); this
    // removes corresponding options and their arguments fromt he command
    // line.

    OptionManager om;
    int dummy = 0;
    String pegasusHome;
    try
    {
	GetOptions(om, dummy, NULL, pegasusHome);
	// om.print();
    }
    catch (Exception&)
    {
      exit(1);
    }

    // Check the trace options and set global variable
    Boolean pegasusIOTrace = false;
    if (om.valueEquals("trace", "true"))
    {
         Handler::setMessageTrace(true);
	 pegasusIOTrace = true;
    }

    Boolean pegasusIOLog = false;
    if (om.valueEquals("logtrace", "true"))
    {
	Handler::setMessageLogTrace(true);
	 pegasusIOLog = true;
    }
    
    // Grab the port otpion:

    String portOption;
    om.lookupValue("port", portOption);

    char* address = portOption.allocateCString();

    // Set up the Logger
    Logger::setHomeDirectory("./logs");

    // Put server start message to the logger
    Logger::put(Logger::STANDARD_LOG, "CIMServer", Logger::INFORMATION,
	"Start $0 %1 port $2 $3 ", 88, PEGASUS_NAME, PEGASUS_VERSION,
		address, (pegasusIOTrace ? " Tracing": " "));
     // try loop to bind the address, and run the server
    try
    {
	Selector selector;
        
       	CIMServer server(&selector, *runPath);
	server_windows = &server;
	server_windows->bind(address);
	delete [] address;
	server_windows->runForever();
    }
    catch(Exception& e)
    {
	PEGASUS_STD(cerr) << "Error: " << e.getMessage() << PEGASUS_STD(endl);
    }

    _endthreadex(NULL);
}


/////////////////////////////////////////////////////////////////
//  Windows NT Service Control Code 
/////////////////////////////////////////////////////////////////




VOID WINAPI  cimserver_windows_main(int argc, char **argv) 
{
  int ccode;
  SERVICE_TABLE_ENTRY dispatch_table[] = 
  {
    {"cimserver", cimserver_service_start},
    {NULL, NULL}
  };

   /* let everyone know we are running (or trying to run) as an NT service */
  if(!(ccode =  StartServiceCtrlDispatcher(dispatch_table))) 
    {
      ccode = GetLastError();
      // Put server start message to the logger
      Logger::put(Logger::STANDARD_LOG, "CIMServer_Windows", Logger::INFORMATION,
		  "Started as a Windows Service");
    }
  return;
}

/////////////////////////////////////////////////////////////////
//
// called by the NT service control manager to start the SLP service
//
/////////////////////////////////////////////////////////////////

VOID WINAPI cimserver_service_start(DWORD argc, LPTSTR *argv) 
{

  DWORD status;
  DWORD specificError;
  pegasus_status.dwServiceType = SERVICE_WIN32;
  pegasus_status.dwCurrentState = SERVICE_START_PENDING;
  pegasus_status.dwControlsAccepted 
      = SERVICE_ACCEPT_STOP | SERVICE_ACCEPT_SHUTDOWN ;
  pegasus_status.dwWin32ExitCode = 0;
  pegasus_status.dwServiceSpecificExitCode = 0;
  pegasus_status.dwCheckPoint = 0;
  pegasus_status.dwWaitHint = 0;

  pegasus_status_handle = RegisterServiceCtrlHandler("cimserver", cimserver_service_ctrl_handler);
  if( pegasus_status_handle == (SERVICE_STATUS_HANDLE)0) 
    {
      Logger::put(Logger::STANDARD_LOG, "CIMServer_Windows", Logger::INFORMATION,
		  "Error installing service handler");
      return;
    }

  // mdday -- I need to replace this hack with registry code

  // this is an ugly hack because we should really be getting this data 
  // out of the registry. We are essentially forcing pegasus to be run 
  // from its build tree. i.e.: 
  // PEGASUS_HOME = binary_exe_path minus  "\bin\cimserver.exe"

  // so if my build environment is in "c:\my-programs\pegasus\ 
  // I will install the service binary path as "c:\my-programs\pegasus\bin\cimserver.exe"
  // Therefore I will derive PEGASUS_HOME as "c:\my-programs\pegasus"
  
  // If I do something wierd and run pegasus from "c:\winnt" then this hack will break 
  // the service will think its running but the CIMServer object will never have been instantiated. 
 
  SC_HANDLE service_handle, sc_manager;
  if(NULL != (sc_manager = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS))) 
    {
      if(NULL != (service_handle = OpenService(sc_manager, 
					       "cimserver",
					       SERVICE_ALL_ACCESS)))

	{
	  DWORD bytes_needed = 0;
	  QUERY_SERVICE_CONFIG *svc_config = NULL;
	  
	  QueryServiceConfig(service_handle, svc_config, sizeof(svc_config), &bytes_needed);
	  if(bytes_needed > 0) 
	    {
	      if(NULL != ( svc_config = (QUERY_SERVICE_CONFIG *) malloc(bytes_needed))) 
		{
		  if(TRUE == QueryServiceConfig(service_handle, svc_config, bytes_needed, &bytes_needed)) 
		    {
		      Uint32 position;
		      runPath = new String(svc_config->lpBinaryPathName);
		      if(PEG_NOT_FOUND != (position = runPath->reverseFind('\\'))) 
			{
			  Uint32 len = runPath->size();
			  runPath->remove(position, len - position);
			  position = runPath->reverseFind('\\');
			  len = runPath->size();
			  runPath->remove(position, len - position);
			}
		    }
		  free(svc_config);
		}
	    }
	  CloseServiceHandle(service_handle);
	}
      CloseServiceHandle(sc_manager);
    }

  status = cimserver_initialization(argc, argv, &specificError);
  if(status < 0) 
    {
      pegasus_status.dwCurrentState = SERVICE_STOPPED;
      pegasus_status.dwCheckPoint = 0;
      pegasus_status.dwWaitHint = 0;
      pegasus_status.dwWin32ExitCode = status;
      pegasus_status.dwServiceSpecificExitCode = specificError;
      SetServiceStatus(pegasus_status_handle, &pegasus_status);
      Logger::put(Logger::STANDARD_LOG, "CIMServer_Windows", Logger::INFORMATION,
		  "Error starting Cim Server");
    return;
    }

  pegasus_status.dwCurrentState = SERVICE_RUNNING;
  pegasus_status.dwCheckPoint = 0;
  pegasus_status.dwWaitHint = 0;

  if(!SetServiceStatus(pegasus_status_handle, &pegasus_status)) 
    {
      if(server_windows != NULL)
	server_windows->killServer();
    }

  return;
}

VOID WINAPI cimserver_service_ctrl_handler(DWORD opcode) 
{

  switch(opcode) {
  case SERVICE_CONTROL_STOP:
  case SERVICE_CONTROL_SHUTDOWN:
    if(server_windows != NULL)
      server_windows->killServer();
    pegasus_status.dwCurrentState = SERVICE_STOPPED;
    pegasus_status.dwCheckPoint = 0;
    pegasus_status.dwWaitHint = 0;
    pegasus_status.dwWin32ExitCode = 0;
    SetServiceStatus(pegasus_status_handle, &pegasus_status);
    return;
    break;
    default:
      break;
  }
  SetServiceStatus(pegasus_status_handle, &pegasus_status);
  return;
}

DWORD cimserver_initialization(DWORD argc, LPTSTR *argv, DWORD *specificError) 
{
  
  return( _beginthread(cimserver_windows_thread, 0, NULL ));
}
 

Uint32 cimserver_install_nt_service(String &pegasusHome ) 
{
  SC_HANDLE service_handle, sc_manager;
  Uint32 ccode = 0;
  pegasusHome += "\\bin\\cimserver.exe";
  LPCSTR path_name = pegasusHome.allocateCString() ;
  if(NULL != (sc_manager = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS))) 
    {
      if(NULL != (service_handle = CreateService(sc_manager, 
						 "cimserver",
						 "Pegasus CIM Object Manager",
						 SERVICE_ALL_ACCESS,
						 SERVICE_WIN32_OWN_PROCESS,
						 SERVICE_DEMAND_START,
						 SERVICE_ERROR_NORMAL, 
						 path_name,
						 NULL, NULL, NULL, NULL, NULL))) 
	{
	  ccode = (Uint32)service_handle;
	}
      CloseServiceHandle(service_handle);
    }
  
  return(ccode);
}

Uint32 cimserver_remove_nt_service(void) 
{

  SC_HANDLE service_handle, sc_manager;
  int ccode = 0;
  if(NULL != (sc_manager = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS))) 
    {
      if(NULL != (service_handle = OpenService(sc_manager, "cimserver", DELETE))) 
	{
	  DeleteService(service_handle);
	  CloseServiceHandle(service_handle);
	  ccode = 1;
	} 
      CloseServiceHandle(sc_manager);
    } 
  return(ccode);
}
