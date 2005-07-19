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
// Author: Mike Brasher (mbrasher@bmc.com)
//
// Modified By: Sean Keenan (sean.keenan@hp.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#include "System.h"

#include <dlfcn.h>
#include <unistd.h>
#include <dirent.h>
#include <pwd.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <cstdio>
#include <time.h>
#include <lib$routines.h>
#include <sys/time.h>
#include <netdb.h>
#include <prvdef.h>
#include <descrip.h>
#include <iodef.h>
#include <stsdef.h>
#include <ssdef.h>
#include <ttdef.h>
#include <tt2def.h>
#include <starlet.h>
#include <libdef.h>
#include <cxx_exception.h>

#include <Pegasus/Common/Tracer.h>
#include <Pegasus/Common/InternalException.h>
#include <Pegasus/Config/ConfigManager.h>

PEGASUS_NAMESPACE_BEGIN

#define MAX_PASS_LEN 32
#define CR 0x0d
#define LF 0x0a

inline void sleep_wrapper(Uint32 seconds)
{
  sleep(seconds);
}

void System::getCurrentTime(Uint32 & seconds, Uint32 & milliseconds)
{
  timeval tv;
  gettimeofday(&tv, 0);
  seconds = Uint32(tv.tv_sec);
  milliseconds = Uint32(tv.tv_usec) / 1000;
}

String System::getCurrentASCIITime()
{
  char str[50];
  time_t rawTime;

  time(&rawTime);
  strftime(str, 40, "%m/%d/%Y-%T", localtime(&rawTime));
  String time = str;
  return time;
}

void System::sleep(Uint32 seconds)
{
  sleep_wrapper(seconds);
}

Boolean System::exists(const char *path)
{
  return access(path, F_OK) == 0;
}

Boolean System::canRead(const char *path)
{
  return access(path, R_OK) == 0;
}

Boolean System::canWrite(const char *path)
{
  return access(path, W_OK) == 0;
}

Boolean System::getCurrentDirectory(char *path, Uint32 size)
{
  return getcwd(path, size) != NULL;
}

Boolean System::isDirectory(const char *path)
{
  struct stat st;

  if (stat(path, &st) != 0)
  {
    return false;
  }
  return S_ISDIR(st.st_mode);
}

Boolean System::changeDirectory(const char *path)
{
  return chdir(path) == 0;
}

Boolean System::makeDirectory(const char *path)
{
  return mkdir(path, 0777) == 0;
}

Boolean System::getFileSize(const char *path, Uint32 & size)
{
  struct stat st;

  if (stat(path, &st) != 0)
  {
    return false;
  }
  size = st.st_size;
  return true;
}

Boolean System::removeDirectory(const char *path)
{
  return rmdir(path) == 0;
}

Boolean System::removeFile(const char *path)
{
  return unlink(path) == 0;
}

Boolean System::renameFile(const char *oldPath, const char *newPath)
{
  if (rename(oldPath, newPath) != 0)
  {
    return false;
  }
  return true;
}

DynamicLibraryHandle System::loadDynamicLibrary(const char *fileName)
{
  PEG_METHOD_ENTER(TRC_OS_ABSTRACTION, "System::loadDynamicLibrary()");

  Tracer:: trace(TRC_OS_ABSTRACTION, Tracer::LEVEL2,
	"Attempting to load library %s - 1", fileName);

  PEG_METHOD_EXIT();
  return DynamicLibraryHandle(dlopen(fileName, RTLD_NOW));
}

void System::unloadDynamicLibrary(DynamicLibraryHandle libraryHandle)
{
  // ATTN: Should this method indicate success/failure?

  dlclose(libraryHandle);
}

String System::dynamicLoadError()
{
  // ATTN: Is this safe in a multi-threaded process?  Should this string
  // be returned from loadDynamicLibrary?

  String dlerr = dlerror();
  return dlerr;
}

DynamicSymbolHandle System::loadDynamicSymbol(
				      DynamicLibraryHandle libraryHandle,
					       const char *symbolName)
{
  char* Errorout;
  void* Dsh;

  if ((Dsh = dlsym(libraryHandle, (char*)symbolName)) == 0)
  {
    Errorout = dlerror();
  }
  return (DynamicSymbolHandle)Dsh;
}

String System::getHostName()
{
  static char hostname[PEGASUS_MAXHOSTNAMELEN];

  if (!*hostname)
  {
    gethostname(hostname, sizeof (hostname));
  }
  return hostname;
}

String System::getFullyQualifiedHostName()
{
  char hostName[PEGASUS_MAXHOSTNAMELEN];
  struct hostent *he;
  String fqName;

  if (gethostname(hostName, PEGASUS_MAXHOSTNAMELEN) != 0)
  {
  return String::EMPTY;
  }

  if (he = gethostbyname(hostName))
  {
    strcpy(hostName, he->h_name);
  }

  fqName.assign(hostName);

  return fqName;
}

String System::getSystemCreationClassName()
{
  return "CIM_ComputerSystem";
}

Uint32 System::lookupPort(
			    const char *serviceName,
			    Uint32 defaultPort)
{
  Uint32 localPort;

  struct servent *serv;

  //
  // Get wbem-local port from /etc/services
  //
  if ((serv = getservbyname(serviceName, TCP)) != NULL)
  {
    localPort = htons((uint16_t) serv->s_port);
  }
  else
  {
    localPort = defaultPort;
  }

  return localPort;
}

String System::getPassword(const char *prompt)
{
  struct
  {
    short int numbuf;
    char frst_char;
    char rsv1;
    long rsv2;
  }
  tahead;

  typedef struct
  {				// I/O status block     
    short i_cond;		// Condition value      
    short i_xfer;		// Transfer count     
    long i_info;		// Device information     
  }
  iosb;

  typedef struct
  {				// Terminal characteristics   
    char t_class;		// Terminal class     
    char t_type;		// Terminal type      
    short t_width;		// Terminal width in characters   
    long t_mandl;		// Terminal's mode and length   
    long t_extend;		// Extended terminal characteristics  
  }
  termb;

  termb otermb;
  termb ntermb;

  static long ichan;		// Gets channel number for TT:  

  register int errorcode;
  int kbdflgs;			// saved keyboard fd flags  
  int kbdpoll;			// in O_NDELAY mode         
  int kbdqp = false;		// there is a char in kbdq  
  int psize;			// size of the prompt 

  static char buf[MAX_PASS_LEN];
  char kbdq;			// char we've already read  

  iosb iostatus;

  static long termset[2] =
  {0, 0};			// No terminator                

  $DESCRIPTOR(inpdev, "TT");	// Terminal to use for input    

  //
  // Get a channel for the terminal
  //

  buf[0] = 0;

  errorcode = sys$assign(&inpdev,	// Device name 
			  &ichan,	// Channel assigned 
			  0,		// request KERNEL mode access 
			  0);		// No mailbox assigned 

  if (errorcode != SS$_NORMAL)
  {
    return buf;
  }

  //
  // Read current terminal settings
  //

  errorcode = sys$qiow(0,	// Wait on event flag zero  
			ichan,	// Channel to input terminal  
			IO$_SENSEMODE,	// Function - Sense Mode 
			&iostatus,	// Status after operation 
			0, 0,	// No AST service   
			&otermb,	// [P1] Address of Char Buffer 
			sizeof (otermb),	// [P2] Size of Char Buffer 
			0, 0, 0, 0);	// [P3] - [P6] 

  if (errorcode != SS$_NORMAL)
  {
    return buf;
  }

  //
  // setup new settings   
  //

  ntermb = otermb;

  //
  // turn on passthru and nobroadcast 
  //

  ntermb.t_extend |= TT2$M_PASTHRU;
  ntermb.t_mandl |= TT$M_NOBRDCST;

  //
  // Write out new terminal settings 
  //

  errorcode = sys$qiow(0,	// Wait on event flag zero  
			ichan,	// Channel to input terminal  
			IO$_SETMODE,	// Function - Set Mode 
			&iostatus,	// Status after operation 
			0, 0,	// No AST service   
			&ntermb,	// [P1] Address of Char Buffer 
			sizeof (ntermb),	// [P2] Size of Char Buffer 
			0, 0, 0, 0);	// [P3] - [P6] 

  if (errorcode != SS$_NORMAL)
  {
    return buf;
  }

  //
  // Write a prompt, read characters from the terminal, performing no editing
  //  and doing no echo at all.
  //

  psize = strlen(prompt);

  errorcode = sys$qiow(0,		// Event flag 
			ichan,		// Input channel 
			IO$_READPROMPT | IO$M_NOECHO | IO$M_NOFILTR | IO$M_TRMNOECHO,
					// Read with prompt, no echo, no translate, no termination character echo
			&iostatus,	// I/O status block 
			NULL,		// AST block (none) 
			0,		// AST parameter 
			&buf,		// P1 - input buffer 
			MAX_PASS_LEN,	// P2 - buffer length 
			0,		// P3 - ignored (timeout) 
			0,		// P4 - ignored (terminator char set) 
			prompt,		// P5 - prompt buffer 
			psize);		// P6 - prompt size 

  if (errorcode != SS$_NORMAL)
  {
    return buf;
  }

  //
  // Write out old terminal settings 
  //

  errorcode = sys$qiow(0,	// Wait on event flag zero  
			ichan,	// Channel to input terminal  
			IO$_SETMODE,	// Function - Set Mode 
			&iostatus,	// Status after operation 
			0, 0,	// No AST service   
			&otermb,	// [P1] Address of Char Buffer 
			sizeof (otermb),	// [P2] Size of Char Buffer 
			0, 0, 0, 0);	// [P3] - [P6] 

  if (errorcode != SS$_NORMAL)
  {
    return buf;
  }

  //
  // Start new line
  //

  fputc (CR, stdout);
  fputc (LF, stdout);

  // 
  // Remove the termination character
  // 

  psize = strlen(buf);
  buf[psize-1] = 0;
  return buf;
}

String System::getEffectiveUserName()
{
  String userName = String::EMPTY;
  struct passwd *pwd = NULL;

  //
  //  get the currently logged in user's UID.
  //
  pwd = getpwuid(geteuid());
  if (pwd == NULL)
  {
  Tracer:: trace(TRC_OS_ABSTRACTION, Tracer::LEVEL4,
	"getpwuid failure, user may have been removed just after login");
  }
  else
  {
    //
    //  get the user name
    //
    userName.assign(pwd->pw_name);
  }
  return (userName);
}

String System::encryptPassword(const char *password, const char *salt)
{
  char pbBuffer[MAX_PASS_LEN] = {0};
  int dwByteCount;
  char pcSalt[3] = {0};

  strncpy(pcSalt, salt, 2);
  dwByteCount = strlen(password);
  memcpy(pbBuffer, password, dwByteCount);
  for (int i=0; (i<dwByteCount) || (i>=MAX_PASS_LEN); i++)
  {
    (i%2 == 0) ? pbBuffer[i] ^= pcSalt[1] : pbBuffer[i] ^= pcSalt[0];
  }

  return String(pcSalt) + String((char *)pbBuffer);
}

Boolean System::isSystemUser(const char *userName)
{
  //
  //  get the password entry for the user
  //
  struct passwd *result;

  result = getpwnam(userName);

  if (result == NULL)
  {
    return false;
  }
  return true;
}

Boolean System::isPrivilegedUser(const String & userName)
{
  //
  // Check if the given user is a privileged user
  //
  int retStat;

  unsigned long int prvPrv = 0;

  retStat = sys$setprv(0, 0, 0, &prvPrv);
  if (!$VMS_STATUS_SUCCESS(retStat))
  {
    return false;
  }
  if ((PRV$M_SETPRV && prvPrv) == 1)
  {
    return true;
  }
  else
  {
    return false;
  }
}

String System::getPrivilegedUserName()
{
  static String userName = String::EMPTY;

if (userName == String::EMPTY)
  {
    struct passwd *pwd = NULL;
    //
    //  get the privileged user's UID.
    //

    pwd = getpwuid(geteuid());
    if (pwd != NULL)
    {
      //
      //  get the user name
      //
      userName.assign(pwd->pw_name);
    }
    else
    {
    Tracer:: trace(TRC_OS_ABSTRACTION, Tracer::LEVEL4,
	    "Could not find entry.");
      PEGASUS_ASSERT(0);
    }
  }
  return (userName);
}

#ifdef PEGASUS_ENABLE_USERGROUP_AUTHORIZATION

Boolean System::isGroupMember(const char *userName, const char *groupName)
{
  struct group grp;
  char *member;
  Boolean retVal = false;
  const unsigned int PWD_BUFF_SIZE = 1024;
  const unsigned int GRP_BUFF_SIZE = 1024;
  struct passwd pwd;
  struct passwd *result;
  struct group *grpresult;
  char pwdBuffer[PWD_BUFF_SIZE];
  char grpBuffer[GRP_BUFF_SIZE];

  //
  // Search Primary group information.
  //

  // Find the entry that matches "userName"

  if (getpwnam_r(userName, &pwd, pwdBuffer, PWD_BUFF_SIZE, &result) != 0)
  {
    String errorMsg = String("getpwnam_r failure : ") +
    String(strerror(errno));
  Tracer:: PEG_TRACE_STRING(TRC_OS_ABSTRACTION, Tracer::LEVEL2,
		     errorMsg);
  Logger:: put(Logger:: STANDARD_LOG, "CIMServer", Logger::WARNING,
	errorMsg);
    throw InternalSystemError();
  }

  if (result != NULL)
  {
    // User found, check for group information.
    gid_t group_id;
    group_id = pwd.pw_gid;

    // Get the group name using group_id and compare with group passed.
    if (getgrgid_r(group_id, &grp,
		   grpBuffer, GRP_BUFF_SIZE, &grpresult) != 0)
    {
      String errorMsg = String("getgrgid_r failure : ") +
      String(strerror(errno));
    Tracer:: PEG_TRACE_STRING(TRC_OS_ABSTRACTION, Tracer::LEVEL2,
		       errorMsg);
    Logger:: put(Logger:: STANDARD_LOG, "CIMServer", Logger::WARNING,
	  errorMsg);
      throw InternalSystemError();
    }

    // Compare the user's group name to groupName.
    if (strcmp(grp.gr_name, groupName) == 0)
    {
      // User is a member of the group.
      return true;
    }
  }

  //
  // Search supplemental groups.
  // Get a user group entry
  //
  if (getgrnam_r(groupName, &grp,
		 grpBuffer, GRP_BUFF_SIZE, &grpresult) != 0)

  {
    String errorMsg = String("getgrnam_r failure : ") +
    String(strerror(errno));
  Tracer:: PEG_TRACE_STRING(TRC_OS_ABSTRACTION, Tracer::LEVEL2,
		     errorMsg);
  Logger:: put(Logger:: STANDARD_LOG, "CIMServer", Logger::WARNING,
	errorMsg);
    throw InternalSystemError();
  }

  // Check if the requested group was found.
  if (grpresult == NULL)
  {
    return false;
  }

  Uint32 j = 0;

  //
  // Get all the members of the group
  //
  member = grp.gr_mem[j++];

  while (member)
  {
    //
    // Check if the user is a member of the group
    //
    if (strcmp(userName, member) == 0)
    {
      retVal = true;
      break;
    }
    member = grp.gr_mem[j++];
  }

  return retVal;
}

#endif

Boolean System::changeUserContext(const char *userName)
{
  const unsigned int PWD_BUFF_SIZE = 1024;
  struct passwd pwd;
  struct passwd *result;
  char pwdBuffer[PWD_BUFF_SIZE];

  int rc = getpwnam_r(userName, &pwd, pwdBuffer, PWD_BUFF_SIZE, &result);

  if (rc != 0)
  {
  PEG_TRACE_STRING(TRC_OS_ABSTRACTION, Tracer::LEVEL2,
		String("getpwnam_r failed: ") + String(strerror(errno)));
    return false;
  }

  if (result == 0)
  {
  Tracer:: PEG_TRACE_STRING(TRC_OS_ABSTRACTION, Tracer::LEVEL2,
		     "getpwnam_r failed.");
    return false;
  }

Tracer:: trace(TRC_OS_ABSTRACTION, Tracer::LEVEL4,
	"Changing user context to: uid = %d, gid = %d",
	(int) pwd.pw_uid, (int) pwd.pw_gid);

  if (setgid(pwd.pw_gid) != 0)
  {
  PEG_TRACE_STRING(TRC_OS_ABSTRACTION, Tracer::LEVEL2,
		     String("setgid failed: ") + String(strerror(errno)));
    return false;
  }

  if (setuid(pwd.pw_uid) != 0)
  {
  PEG_TRACE_STRING(TRC_OS_ABSTRACTION, Tracer::LEVEL2,
		     String("setuid failed: ") + String(strerror(errno)));
    return false;
  }

  return true;
}

Uint32 System::getPID()
{
  //
  // Get the Process ID
  //
  Uint32 pid = getpid();

  return pid;
}

Boolean System::truncateFile(
			      const char *path,
			      size_t newSize)
{
  return (truncate(path, newSize) == 0);
}

// Is absolute path?
Boolean System::is_absolute_path(const char *path)
{
  if (path == NULL)
    return false;

  if (path[0] == '/')
    return true;

  return false;
}

// Changes file permissions on the given file.
Boolean System::changeFilePermissions(const char *path, mode_t mode)
{
  Sint32 ret = 0;

  const char *tmp = path;
  ret = chmod(tmp, mode);
  return (ret != -1);
}

Boolean System::verifyFileOwnership(const char *path)
{
  struct stat st;

  if (lstat(path, &st) != 0)
  {
    return false;
  }

  return ((st.st_uid == geteuid()) &&    // Verify the file owner
          S_ISREG(st.st_mode) &&         // Verify it is a regular file
          (st.st_nlink == 1));           // Verify it is not a hard link
}

void System::syslog(const String & ident, Uint32 severity, const char *message)
{
  // Not implemented
}

// System ID constants for Logger::put and Logger::trace

const String System::CIMSERVER = "cimserver";	// Server system ID

PEGASUS_NAMESPACE_END
