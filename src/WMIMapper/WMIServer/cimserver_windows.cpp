//%2003////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001, 2002  BMC Software, Hewlett-Packard Development
// Company, L. P., IBM Corp., The Open Group, Tivoli Systems.
// Copyright (c) 2003 BMC Software; Hewlett-Packard Development Company, L. P.;
// IBM Corp.; EMC Corporation, The Open Group.
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
//              Jair Santos, Hewlett-Packard Company (jair.santos@hp.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#include <windows.h>
#include <process.h>    /* _beginthread, _endthread */
#include <tchar.h>
#include <direct.h>

#include "service.cpp"

PEGASUS_USING_PEGASUS;
PEGASUS_USING_STD;

//-------------------------------------------------------------------------
// DEFINES
//-------------------------------------------------------------------------
#define PEGASUS_SERVICE_NAME "wmiserver"
#define PEGASUS_DISPLAY_NAME "Pegasus WMI Mapper"
#define PEGASUS_DESCRIPTION  "Pegasus WBEM to WMI Mapper Manager Service"

//-------------------------------------------------------------------------
// GLOBALS
//-------------------------------------------------------------------------
CIMServer *server_windows;
static Service pegasus_service(PEGASUS_SERVICE_NAME);
static HANDLE pegasus_service_event;
static LPCSTR g_cimservice_key  = TEXT("SYSTEM\\CurrentControlSet\\Services\\%s");
static LPCSTR g_cimservice_home = TEXT("home");

//-------------------------------------------------------------------------
// PROTOTYPES
//-------------------------------------------------------------------------
int cimserver_windows_main(int flag, int argc, char **argv);
extern void GetOptions(ConfigManager *cm,
                int &argc,
                char **argv,
                const String &pegasusHome);
static bool _getRegInfo(const char *lpchKeyword, char *lpchRetValue);
static bool _setRegInfo(const char *lpchKeyword, const char *lpchValue);
void setHome(String & home);

//-------------------------------------------------------------------------
// NO-OPs for windows platform
//-------------------------------------------------------------------------
int cimserver_fork(void) { return(0); }
int cimserver_kill(void) { return(0); }
void notify_parent(int id) { return;    }

//-------------------------------------------------------------------------
// START MONITOR Asynchronously
//-------------------------------------------------------------------------
static void __cdecl cimserver_windows_thread(void *parm) 
{

  // Get options (from command line and from configuration file); this
  // removes corresponding options and their arguments fromt he command
  // line.

  String pegasusHome;

  // Windows way to set home
  setHome(pegasusHome);

  ConfigManager::setPegasusHome(pegasusHome);

  ConfigManager* configManager = ConfigManager::getInstance();
  int dummy = 0;

  try
    {
      GetOptions(configManager, dummy, NULL, pegasusHome);
    }
  catch (Exception&)
    {
      exit(1);
    }

  Boolean enableHttpConnection = String::equal(
    configManager->getCurrentValue("enableHttpConnection"), "true");
  Boolean enableHttpsConnection = String::equal(
    configManager->getCurrentValue("enableHttpsConnection"), "true");

  if (!enableHttpConnection && !enableHttpsConnection)
  {
    Logger::put(Logger::STANDARD_LOG, "CIMServer", Logger::WARNING,
      "Neither HTTP nor HTTPS connection is enabled.  "
      "CIMServer will not be started.");
    cerr << "Neither HTTP nor HTTPS connection is enabled.  "
      "CIMServer will not be started." << endl;
    exit(1);
  }

  // Get the connection port configurations

  Uint32 portNumberHttps;
  Uint32 portNumberHttp;

  if (enableHttpsConnection)
  {
    String httpsPort = configManager->getCurrentValue("httpsPort");
    CString portString = httpsPort.getCString();
    char* end = 0;
    Uint32 port = strtol(portString, &end, 10);
    assert(end != 0 && *end == '\0');

    //
    // Look up the WBEM-HTTPS port number
    //
    portNumberHttps = System::lookupPort(WBEM_HTTPS_SERVICE_NAME, port);
  }

  if (enableHttpConnection)
  {
    String httpPort = configManager->getCurrentValue("httpPort");
    CString portString = httpPort.getCString();
    char* end = 0;
    Uint32 port = strtol(portString, &end, 10);
    assert(end != 0 && *end == '\0');

    //
    // Look up the WBEM-HTTP port number
    //
    portNumberHttp = System::lookupPort(WBEM_HTTP_SERVICE_NAME, port);
  }

  // Set up the Logger
  String logsDirectory = String::EMPTY;
  logsDirectory = configManager->getCurrentValue("logdir");
  logsDirectory = ConfigManager::getHomedPath(configManager->getCurrentValue("logdir"));

  Logger::setHomeDirectory(logsDirectory);

  // Put server start message to the logger
  Logger::put(Logger::STANDARD_LOG, PEGASUS_SERVICE_NAME, Logger::INFORMATION,
              "Started $0 version $1.", PEGASUS_NAME, PEGASUS_VERSION);

   // try loop to bind the address, and run the server
  try
  {
    Monitor monitor(true);
    
    CIMServer server(&monitor);
    server_windows = &server;

    if (enableHttpConnection)
    {
      server_windows->addAcceptor(false, portNumberHttp, false);
      Logger::put(Logger::STANDARD_LOG, "CIMServer", Logger::INFORMATION,
                  "Listening on HTTP port $0.", portNumberHttp);
    }
    if (enableHttpsConnection)
    {
      server_windows->addAcceptor(false, portNumberHttps, true);
      Logger::put(Logger::STANDARD_LOG, "CIMServer", Logger::INFORMATION,
                  "Listening on HTTPS port $0.", portNumberHttps);
    }

    server_windows->bind();

    while(!server_windows->terminated())
    {
        server_windows->runForever();
    }
  }
  catch(Exception& e)
  {
    PEGASUS_STD(cerr) << "Error: " << e.getMessage() << PEGASUS_STD(endl);
  }

  _endthreadex(NULL);
}


//-------------------------------------------------------------------------
//  Windows NT Service Control Code 
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
// SERVICE (no parameters)
//-------------------------------------------------------------------------
void cim_server_service(int argc, char **argv)
{
  Service::ReturnCode status = Service::SERVICE_RETURN_SUCCESS;
  char console_title[_MAX_PATH] = {0};

  // Check if running from a console window
  if (GetConsoleTitle(console_title, _MAX_PATH) > 0)
    return;

  pegasus_service_event = CreateEvent(NULL, FALSE, FALSE, NULL);

  // Run should exit the process if a service
  status = pegasus_service.Run(cimserver_windows_main);

  // If we made it here there was a problem starting this process as a service
  // Log the problem to the log file

  // TODO: log or echo something here
}

//-------------------------------------------------------------------------
// START/STOP handler 
//-------------------------------------------------------------------------
int cimserver_windows_main(int flag, int argc, char *argv[])
{
  switch (flag)
  {
    case Service::STARTUP_FLAG:
      if (_beginthread(cimserver_windows_thread, 0, NULL))
        WaitForSingleObject(pegasus_service_event, INFINITE);
      break;

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
Boolean isCIMServerRunning(void)
{
  Service::State state;
  pegasus_service.GetState(&state);

  return (state == Service::SERVICE_STATE_RUNNING) ? true : false;
}

//-------------------------------------------------------------------------
// INSTALL
//-------------------------------------------------------------------------
bool cimserver_install_nt_service(char *service_name)
{
  Service::ReturnCode status = Service::SERVICE_RETURN_SUCCESS;
  char filename[_MAX_PATH] = {0};
  char displayname[_MAX_PATH] = {0};

  // If service name is specified, override default
  if (service_name == NULL)
    {
      strcpy(displayname, PEGASUS_DISPLAY_NAME);
    }
  else
    {
      pegasus_service.SetServiceName(service_name);
      sprintf(displayname, "%s - %s", PEGASUS_DISPLAY_NAME, service_name);
    }

  GetModuleFileName(NULL, filename, sizeof(filename));
  status = pegasus_service.Install(displayname, PEGASUS_DESCRIPTION, filename);

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
bool cimserver_start_nt_service(char *service_name) 
{
  Service::ReturnCode status = Service::SERVICE_RETURN_SUCCESS;

  // If service name is specified, override default
  if (service_name != NULL)
    {
      pegasus_service.SetServiceName(service_name);
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

void setHome(String & home)
{
  // Determine the absolute path to the running program
  char exe_pathname[_MAX_PATH] = {0};
  char home_pathname[_MAX_PATH] = {0};
  GetModuleFileName(NULL, exe_pathname, sizeof(exe_pathname));

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

