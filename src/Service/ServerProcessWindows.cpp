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
// Author: Mike Day (mdday@us.ibm.com)
//
// Modified By: Mary Hinton (m.hinton@verizon.net)
//              Sushma Fernandes (sushma_fernandes@hp.com)
//              Yi Zhou, Hewlett-Packard Company (yi_zhou@hp.com)
//              Tony Fiorentino (fiorentino_tony@emc.com)
//              Roger Kumpf, Hewlett-Packard Company (roger_kumpf@hp.com)
//              Steve Hills (steve.hills@ncr.com)
//              Josephine Eskaline Joyce, IBM (jojustin@in.ibm.com) - Bug#2032, 2031
//
//%/////////////////////////////////////////////////////////////////////////////

#include <windows.h>
#include <process.h>    /* _beginthread, _endthread */
#include <tchar.h>
#include <direct.h>
#include <Pegasus/Common/MessageLoader.h> //l10n
#include <Pegasus/Common/Thread.h>  // l10n
#include <Pegasus/Server/CIMServer.h>

#include "Service.cpp"

PEGASUS_USING_PEGASUS;
PEGASUS_USING_STD;

static const char* _ALREADY_RUNNING_NAME = 0;

//-------------------------------------------------------------------------
// GLOBALS
//-------------------------------------------------------------------------
static Mutex _cimserverLock;
static ServerProcess* _server_proc = 0;
static bool _shutdown = false;
static Service pegasus_service;
static HANDLE pegasus_service_event = NULL;
static LPCSTR g_cimservice_key  = TEXT("SYSTEM\\CurrentControlSet\\Services\\%s");
static LPCSTR g_cimservice_home = TEXT("home");
static int g_argc = 0;
static char **g_argv = 0;

//  Constants representing the command line options.
static const char OPTION_INSTALL[] = "install";
static const char OPTION_REMOVE[]  = "remove";
static const char OPTION_START[]   = "start";
static const char OPTION_STOP[]    = "stop";

//-------------------------------------------------------------------------
// PROTOTYPES
//-------------------------------------------------------------------------
int cimserver_windows_main(int flag, int argc, char **argv);
static bool _getRegInfo(const char *lpchKeyword, char *lpchRetValue);
static bool _setRegInfo(const char *lpchKeyword, const char *lpchValue);

//-------------------------------------------------------------------------
// NO-OPs for windows platform
//-------------------------------------------------------------------------
int ServerProcess::cimserver_fork(void) { return(0); }
void ServerProcess::notify_parent(int id) { return; }
long ServerProcess::get_server_pid(void) { return 0; }
void ServerProcess::set_parent_pid(int pid) {}
int ServerProcess::get_proc(int pid) { return 0; }
int ServerProcess::cimserver_kill(int id) { return(0); }
void cimserver_exitRC(int rc) {}
int ServerProcess::cimserver_initialize(void) { return 0; }
int ServerProcess::cimserver_wait(void) { return 0; }


//-------------------------------------------------------------------------
// Helper for platform specific handling
//-------------------------------------------------------------------------

ServerProcess::ServerProcess()
{
    //be sure to call cimserver_set_process right after instantiating this in order for everything to work
}

ServerProcess::~ServerProcess()
{
}

void ServerProcess::cimserver_set_process(void* p)
{
    AutoMutex am( _cimserverLock );
    _server_proc = static_cast<ServerProcess *>(p);
    if(_server_proc && _shutdown)
        _server_proc->cimserver_stop();

    pegasus_service = Service(getProcessName());
    _ALREADY_RUNNING_NAME = getProcessName();
}

void signal_shutdown()
{
    AutoMutex am( _cimserverLock );
    _shutdown = true;
    if( _server_proc )
        _server_proc->cimserver_stop(); 
}

//-------------------------------------------------------------------------
// Run main server asynchronously
//-------------------------------------------------------------------------
static unsigned __stdcall cimserver_windows_thread( void* parm )
{
    int argc = 0;
    int rc = _server_proc->cimserver_run( g_argc, g_argv, false );
    SetEvent(pegasus_service_event);
    _endthreadex( rc );
    return rc;
}

//-------------------------------------------------------------------------
//  Windows NT Service Control Code 
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
// START/STOP handler 
//-------------------------------------------------------------------------
int cimserver_windows_main(int flag, int argc, char *argv[])
{
    switch( flag )
    {
    case Service::STARTUP_FLAG:
    {
        //
        // Start up main run in a separate thread and wait for it to finish.
        //

        unsigned threadid = 0;
        g_argc = argc;
        g_argv = argv;
        HANDLE hThread = (HANDLE)_beginthreadex( NULL, 0, cimserver_windows_thread, NULL, 0, &threadid );
        if( hThread == NULL )
            return 1;

        WaitForSingleObject( pegasus_service_event, INFINITE );

        //
        // Shutdown the cimserver.
        //

        signal_shutdown();

        //
        // Make sure we upate the SCM that our stop is pending.
        // Wait for the main run thread to exit.
        //

        DWORD dwCheckPoint = 1; // service code should have already started at 0

        while( WaitForSingleObject( hThread, 3000 ) == WAIT_TIMEOUT )
        {
            pegasus_service.report_status( 
                SERVICE_STOP_PENDING, NO_ERROR, dwCheckPoint++, 5000 );
        }

        CloseHandle( hThread );

        break;
    }
    case Service::SHUTDOWN_FLAG:
        SetEvent(pegasus_service_event);
        break;

    default:
        break;
    }

    return 0;
}

//-------------------------------------------------------------------------
// IS RUNNING?
//-------------------------------------------------------------------------

class AlreadyRunning
{
public:
    AlreadyRunning(): alreadyRunning( true ), event( NULL )
    {
    }
    ~AlreadyRunning()
    {
        if( event != NULL )
            CloseHandle( event );
    }

    void Init()
    {
        if( event == NULL )
        {
            event = CreateEvent( NULL, TRUE, TRUE, _ALREADY_RUNNING_NAME );
            if( event != NULL && GetLastError() != ERROR_ALREADY_EXISTS )
                alreadyRunning = false;
        }
    }

    bool IsAlreadyRunning()
    {
        return alreadyRunning;
    }

    bool alreadyRunning;
    HANDLE event;
};

AlreadyRunning _alreadyRunning;


Boolean ServerProcess::isCIMServerRunning(void)
{
    //Service::State state;
    //pegasus_service.GetState(&state);
    //return (state == Service::SERVICE_STATE_RUNNING) ? true : false;

    // We do it this way so this will work when run as a 
    // console process and a Windows service.
    AlreadyRunning ar;
    ar.Init();
    return ar.IsAlreadyRunning();
}

//-------------------------------------------------------------------------
// INSTALL
//-------------------------------------------------------------------------
bool cimserver_install_nt_service(char *service_name)
{
  Service::ReturnCode status = Service::SERVICE_RETURN_SUCCESS;
  char filename[_MAX_PATH] = {0};
  char displayname[_MAX_PATH] = {0};
  char descriptionname[_MAX_PATH] = {0};

  // If service name is specified, override default
  if (service_name == NULL)
    {
      strcpy(displayname, _server_proc->getExtendedName());
    }
  else
    {
      pegasus_service.SetServiceName(service_name);
      sprintf(displayname, "%s - %s", _server_proc->getExtendedName(), service_name);
    }

  strcpy(descriptionname, _server_proc->getDescription());

  if(0 != GetModuleFileName(NULL, filename, sizeof(filename)))
  {
     status = pegasus_service.Install(displayname, descriptionname, filename);

     // Upon success, set home in registry
     if (status == Service::SERVICE_RETURN_SUCCESS)
     {
      char pegasus_homepath[_MAX_PATH];
      System::extract_file_path(filename, pegasus_homepath);
      pegasus_homepath[strlen(pegasus_homepath)-1] = '\0';
      strcpy(filename, pegasus_homepath);
      System::extract_file_path(filename, pegasus_homepath);
      pegasus_homepath[strlen(pegasus_homepath)-1] = '\0';
      _setRegInfo(g_cimservice_home, pegasus_homepath);
     }
  }
  else
  {
    status = (Service::ReturnCode) GetLastError();
  }
  return (status == Service::SERVICE_RETURN_SUCCESS) ? true : false;
}

//-------------------------------------------------------------------------
// REMOVE
//-------------------------------------------------------------------------
bool cimserver_remove_nt_service(char *service_name) 
{
  Service::ReturnCode status = Service::SERVICE_RETURN_SUCCESS;

  // If service name is specified, override default
  if (service_name != NULL)
    {
      pegasus_service.SetServiceName(service_name);
    }

  status = pegasus_service.Remove();

  return (status == Service::SERVICE_RETURN_SUCCESS) ? true : false;
}

//-------------------------------------------------------------------------
// START
//-------------------------------------------------------------------------
bool cimserver_start_nt_service(char *service_name, int num_args, char **service_args) 
{
  Service::ReturnCode status = Service::SERVICE_RETURN_SUCCESS;

  // If service name is specified, override default
  if (service_name != NULL)
    {
      pegasus_service.SetServiceName(service_name);
    }

  if(num_args > 0 && service_args != NULL)
  {
      pegasus_service.SetServiceArgs(num_args, service_args);
  }

  status = pegasus_service.Start(5);

  return (status == Service::SERVICE_RETURN_SUCCESS) ? true : false;
}

//-------------------------------------------------------------------------
// STOP
//-------------------------------------------------------------------------
bool cimserver_stop_nt_service(char *service_name) 
{
  Service::ReturnCode status = Service::SERVICE_RETURN_SUCCESS;

  // If service name is specified, override default
  if (service_name != NULL)
    {
      pegasus_service.SetServiceName(service_name);
    }

  status = pegasus_service.Stop(5);

  return (status == Service::SERVICE_RETURN_SUCCESS) ? true : false;
}

//-------------------------------------------------------------------------
// HELPER Utilities
//-------------------------------------------------------------------------
static bool _getRegInfo(const char *lpchKeyword, char *lpchRetValue)
{
  HKEY   hKey;
  DWORD  dw                   = _MAX_PATH;
  char   subKey[_MAX_PATH]    = {0};
  
  sprintf(subKey, g_cimservice_key, pegasus_service.GetServiceName());

  if ((RegOpenKeyEx(HKEY_LOCAL_MACHINE,
                    subKey, 
                    0,
                    KEY_READ, 
                    &hKey)) != ERROR_SUCCESS)
    {
      return false;
    }

  if ((RegQueryValueEx(hKey, 
                       lpchKeyword, 
                       NULL, 
                       NULL, 
                       (LPBYTE)lpchRetValue,
                       &dw)) != ERROR_SUCCESS)
    {
      RegCloseKey(hKey);
      return false;
    }

  RegCloseKey(hKey);

  return true;
}

static bool _setRegInfo(const char *lpchKeyword, const char *lpchValue)
{
  HKEY   hKey;
  DWORD  dw                   = _MAX_PATH;
  char   home_key[_MAX_PATH]  = {0};
  char   subKey[_MAX_PATH]    = {0};

  if (lpchKeyword == NULL || lpchValue == NULL)
    return false;

  sprintf(subKey, g_cimservice_key, pegasus_service.GetServiceName());

  if ((RegCreateKeyEx (HKEY_LOCAL_MACHINE,
                      subKey,
                      0,
                      NULL,
                      0,
                      KEY_ALL_ACCESS,
                      NULL,
                      &hKey,
                      NULL) != ERROR_SUCCESS))
    {
      return false;
    }

  if ((RegSetValueEx(hKey, 
                     lpchKeyword, 
                     0, 
                     REG_SZ, 
                     (CONST BYTE *)lpchValue,
                     (DWORD)(strlen(lpchValue)+1))) != ERROR_SUCCESS)
    {
      RegCloseKey(hKey);
      return false;
    }

  RegCloseKey(hKey);

  return true;
}

//void ServerProcess::setHome(const String& home)
String ServerProcess::getHome(void)
{
    String home = String::EMPTY;

  // Determine the absolute path to the running program
  char exe_pathname[_MAX_PATH] = {0};
  char home_pathname[_MAX_PATH] = {0};
  if(0 != GetModuleFileName(NULL, exe_pathname, sizeof(exe_pathname)))
  {

    // Pegasus home search rules:
    // - look in registry (if set)
    // - if not found, look in PEGASUS_HOME (if set)
    // - if not found, use exe directory minus one level

    bool found_reg = _getRegInfo("home", home_pathname);
    if (found_reg == true)
      {
        // Make sure home matches
        String current_home(home_pathname);
        String current_exe(exe_pathname);
        current_home.toLower();
        current_exe.toLower();

        Uint32 pos = current_exe.find(current_home);
        if (pos != PEG_NOT_FOUND)
          {
            home = home_pathname;
          }
        else
          {
            found_reg = false;
          }
      }
    if (found_reg == false)
      {
        const char* tmp = getenv("PEGASUS_HOME");
        if (tmp)
          {
            home = tmp;
          }
        else
          {
            // ASSUMPTION: At a minimum, the cimserver program is running
            // from a "bin" directory
            home = FileSystem::extractFilePath(exe_pathname);
            home.remove(home.size()-1, 1);
            home = FileSystem::extractFilePath(home);
            home.remove(home.size()-1, 1);
          }
      }
  }
    return home;
}

//
// Our console control handler
//

static BOOL WINAPI ControlHandler( DWORD dwCtrlType )
{
    switch( dwCtrlType )
    {
    case CTRL_BREAK_EVENT:  // use Ctrl+C or Ctrl+Break to simulate
    case CTRL_C_EVENT:      // SERVICE_CONTROL_STOP in debug mode
    {
        signal_shutdown();
        return TRUE;
    }
    }
    return FALSE;
}

//
// Platform specific run
//

int ServerProcess::platform_run( int argc, char** argv, Boolean shutdownOption )
{
    //
    // Check for my command line options
    //

    for( int i = 1; i < argc; )
    {
        const char* arg = argv[i];

        // Check for -option
        if (*arg == '-')
        {
            // Get the option
            const char* option = arg + 1;

            if (strcmp(option, OPTION_INSTALL) == 0)
            {
                //
                // Install as a NT service
                //
                char *opt_arg = NULL;
                if (i+1 < argc)
                {
                    opt_arg = argv[i+1];

                }
                if(cimserver_install_nt_service(opt_arg))
                {
                    //l10n
                    //cout << "\nPegasus installed as NT Service";
                    MessageLoaderParms parms(
                        "src.Server.cimserver.INSTALLED_NT_SERVICE",
                        "\nPegasus installed as a Windows service");
                    cout << MessageLoader::getMessage(parms) << endl;
                    exit(0);
                }
                else
                {
                    exit(0);
                }
            }
            else if (strcmp(option, OPTION_REMOVE) == 0)
            {
                //
                // Remove Pegasus as an NT service
                //
                char *opt_arg = NULL;
                if (i+1 < argc)
                {
                    opt_arg = argv[i+1];                    
                }
                if(cimserver_remove_nt_service(opt_arg))
                {
                    //l10n
                    //cout << "\nPegasus removed as NT Service";
                    MessageLoaderParms parms(
                        "src.Server.cimserver.REMOVED_NT_SERVICE",
                        "\nPegasus removed as a Windows service");
                    cout << MessageLoader::getMessage(parms) << endl;
                    exit(0);
                }
                else
                {
                    exit(0);
                }

            }
            else if (strcmp(option, OPTION_START) == 0)
            {
                //
                // Start as a NT service
                //
                char *opt_arg = NULL;
                int num_args = 0;
                if (i+1 < argc)
                {
                    opt_arg = argv[i+1];                    
                    num_args = argc - 3;
                }
                else
                {
                    num_args = argc - 2;
                }

                char **service_args = &argv[1];
                if(cimserver_start_nt_service(opt_arg, num_args, service_args))
                {
                    //l10n
                    //cout << "\nPegasus started as NT Service";
                    MessageLoaderParms parms(
                        "src.Server.cimserver.STARTED_NT_SERVICE",
                        "\nPegasus started as a Windows service");
                    cout << MessageLoader::getMessage(parms) << endl;
                    exit(0);
                }
                else
                {
                    exit(0);
                }
            }
            else if (strcmp(option, OPTION_STOP) == 0)
            {
                //
                // Stop as a NT service
                //
                char *opt_arg = NULL;
                if (i+1 < argc)
                {
                    opt_arg = argv[i+1];                    
                }
                if(cimserver_stop_nt_service(opt_arg))
                {
                    //l10n
                    //cout << "\nPegasus stopped as NT Service";
                    MessageLoaderParms parms(
                        "src.Server.cimserver.STOPPED_NT_SERVICE",
                        "\nPegasus stopped as a Windows service");
                    cout << MessageLoader::getMessage(parms) << endl;
                    exit(0);
                }
                else
                {
                    exit(0);
                }
            }
            else
                i++;
        }
        else
            i++;
    }

    //
    // Signal ourself as running
    //

    if( !shutdownOption )
        _alreadyRunning.Init();

    //
    // Check if already running
    //
    // Hmm, when starting as a service, should we do this here (before
    // starting the control dispatcher)?  If we do then the SCM reports
    // a dumb message to the user.  If we don't, and it in the serviceProc 
    // then the service will start up then die silently.
    //

    if( !shutdownOption && _alreadyRunning.IsAlreadyRunning() )
    {
        MessageLoaderParms parms(
            "src.Server.cimserver.UNABLE_TO_START_SERVER_ALREADY_RUNNING",
            "Unable to start CIMServer.\nCIMServer is already running." );
        Logger::put(
            Logger::ERROR_LOG, "CIMServer", Logger::SEVERE,
            MessageLoader::getMessage(parms) );
        PEGASUS_STD(cerr) << MessageLoader::getMessage(parms) << PEGASUS_STD(endl);
        return 1;
    }

    //
    // Check if running from a console window. If so then just run
    // as a console process.
    //

    char console_title[ _MAX_PATH ] = {0};
    if( GetConsoleTitle( console_title, _MAX_PATH ) > 0 )
    {
        SetConsoleCtrlHandler( ControlHandler, TRUE );

        return cimserver_run( argc, argv, shutdownOption );
    }

    //
    // Run as a service
    //

    pegasus_service_event = CreateEvent( NULL, FALSE, FALSE, NULL );

    Service::ReturnCode status;
    status = pegasus_service.Run( cimserver_windows_main );

    if( status != Service::SERVICE_RETURN_SUCCESS )
    {
        // todo: put into localized messages when messages unfreezes.
        Logger::put_l(
            Logger::ERROR_LOG, "CIMServer", Logger::SEVERE,
            "src.Server.cimserver_windows.LISTENING_ON_HTTP_PORT",
            "Error during service run: code = $0.", status );
        return 1;
    }

    return 0;
}
