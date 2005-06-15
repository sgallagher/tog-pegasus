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
// Modified By:  Jenny Yu, Hewlett-Packard Company (jenny_yu@hp.com)
//       Yi Zhou, Hewlett-Packard Company (yi_zhou@hp.com)
//       Marek Szermutzky, IBM (ddt6szer@de.ibm.com)
//%/////////////////////////////////////////////////////////////////////////////
#include <sys/types.h>
#include <sys/stat.h>
#if defined(PEGASUS_OS_HPUX)
#include <sys/pstat.h>
#endif
#include <fcntl.h>
#include <unistd.h>
#include <Pegasus/Common/Signal.h>
#if defined(PEGASUS_PLATFORM_ZOS_ZSERIES_IBM)
#include <sys/ps.h>
#endif
#define MAX_WAIT_TIME 240
#if defined(PEGASUS_OS_AIX)
extern "C" {
#include <procinfo.h>
extern int getprocs(struct procsinfo *, int, struct fdsinfo *, int,pid_t *,int);
#define PROCSIZE sizeof(struct procsinfo)
}
#endif

PEGASUS_USING_PEGASUS;
PEGASUS_USING_STD;

Boolean handleSigUsr1 = false;
Boolean graveError = false;

pid_t server_pid;
Uint32 parentPid = 0;

void sigUsr1Handler(int s_n, PEGASUS_SIGINFO_T * s_info, void * sig)
{
    handleSigUsr1 = true;
}

void sigTermHandler(int s_n, PEGASUS_SIGINFO_T * s_info, void * sig)
{
    graveError= handleSigUsr1=true;
} 


//constructor
ServerProcess::ServerProcess() {}

//destructor
ServerProcess::~ServerProcess() {}

// no-ops
void ServerProcess::cimserver_set_process(void* p) {}
void ServerProcess::cimserver_exitRC(int rc) {}
int ServerProcess::cimserver_initialize(void) { return 1; }
int ServerProcess::cimserver_wait(void) { return 1; }
String ServerProcess::getHome(void) { return String::EMPTY; }

// daemon_init , RW Stevens, "Advance UNIX Programming"

int ServerProcess::cimserver_fork(void) 
{ 
    getSigHandle()->registerHandler(PEGASUS_SIGUSR1, sigUsr1Handler);
    getSigHandle()->activate(PEGASUS_SIGUSR1);
    getSigHandle()->registerHandler(SIGTERM, sigTermHandler);
    getSigHandle()->activate(SIGTERM);
 
  pid_t pid;
  if( (pid = fork() ) < 0) 
  {
      getSigHandle()->deactivate(PEGASUS_SIGUSR1);
      getSigHandle()->deactivate(SIGTERM);
      return(-1);
  }
  else if (pid != 0)
  {
      //
      // parent wait for child
      // if there is a problem with signal, parent process terminates
      // when waitTime expires
      //
      Uint32 waitTime = MAX_WAIT_TIME;

      while(!handleSigUsr1 && waitTime > 0)
      {
        sleep(1);
        waitTime--;
      }
      if( !handleSigUsr1 )
        {
        MessageLoaderParms parms("src.Service.ServerProcessUnix.CIMSERVER_START_TIMEOUT",
          "The cimserver command timed out waiting for the CIM server to start.");
        PEGASUS_STD(cerr) << MessageLoader::getMessage(parms) << PEGASUS_STD(endl);
      }
      exit(graveError);
  }
  
  setsid();
  umask(0);

  // get the pid of the cimserver process
  server_pid = getpid();
  return(0);
}

long ServerProcess::get_server_pid()
{
    return server_pid;
}

void ServerProcess::set_parent_pid(int pid)
{
    parentPid = pid;
}


#if defined(PEGASUS_PLATFORM_LINUX_GENERIC_GNU) || defined(PEGASUS_PLATFORM_SOLARIS_SPARC_CC)

//===========================================================================
//  NAME          : verify_process_name
//  DESCRIPTION   : Opens the 'stat' file in the /proc/<pid> directory to 
//                  verify that the process name is that of the cimserver.
//===========================================================================
int verify_process_name(char *directory, const char* server_process_name) 
{
    static char filename[80];
    static char buffer[512];
    int fd, bytesRead;

    // generate the name of the stat file in the process's /proc directory,
    // and open it
    sprintf(filename, "%s/%s", directory, "stat");
    if ( (fd = open(filename, O_RDONLY, 0)) == -1 ) 
    {
        return -1;
    }

    // read the contents
    if ( (bytesRead = read( fd, buffer, (sizeof buffer) - 1 )) <= 0 ) 
    {
        close(fd);
        return -1;
    }

    // null terminate the file contents
    buffer[bytesRead] = 0;

    close(fd);

    // the process name is the second element of the file contents and
    // is surrounded by parentheses. 
    //
    // find the positions of the parentheses in the file contents
    char * open_paren;
    char * close_paren;
    
    open_paren = strchr (buffer, '(');
    close_paren = strchr (buffer, ')');
    if (open_paren == NULL || close_paren == NULL || close_paren < open_paren)
    {
        return -1;
    }

    // allocate memory for the result
    char * process_name;
    process_name = (char*) malloc(close_paren - open_paren - 1);

    // copy the process name into the result  
    strncpy (process_name, open_paren + 1, close_paren - open_paren -1);

    // strncpy doesn't NULL-terminate the result, so do it here
    process_name[close_paren - open_paren -1] = '\0';

    if (strcmp(process_name, server_process_name) != 0)
    {
        return -1;
    }

    return 0;
}

//=============================================================================
// NAME           : get_proc
// DESCRIPTION    : get_proc() makes a stat() system call on the directory in
//                  /proc with a name that matches the pid of the cimserver.
//                  It returns 0 if it successfully located the process dir
//                  and verified that the process name matches that of the
//                  cimserver.  It returns -1 if it fails to open /proc, or
//                  the cimserver process does not exist.
//=============================================================================
int ServerProcess::get_proc(int pid)
{
  static char path[32];
  static struct stat stat_buff;

  sprintf(path, "/proc/%d", pid);
  if (stat(path, &stat_buff) == -1)          // process stopped running
  {
    return -1;
  }

 // get the process name to make sure it is the cimserver process
// ATTN: skip verify for Solaris
#if !defined(PEGASUS_PLATFORM_SOLARIS_SPARC_CC)
  if ((verify_process_name(path, getProcessName())) == -1)
  {
    return -1;
  }
#endif

  //
  // Check to see if this command process has the same pid as the cimserver
  // daemon process pid stored in the cimserver_start.conf file.  Since the
  // command has the same name as the cimserver daemon process, this could
  // happen after a system reboot.  If the pids are the same, cimserver 
  // isn't really running.
  //
  Uint32 mypid = System::getPID();
  if ((mypid == (unsigned)pid) || ((unsigned)parentPid == (unsigned)pid))
  {
      return -1;
  }
  return 0;
}
#endif

#if defined(PEGASUS_PLATFORM_ZOS_ZSERIES_IBM)
Boolean isProcRunning(pid_t pid)
{
    W_PSPROC buf;                                                              
    int token = 0;
    memset(&buf, 0x00, sizeof(buf));                                           
    buf.ps_conttyptr =(char *) malloc(buf.ps_conttylen =PS_CONTTYBLEN);        
    buf.ps_pathptr   =(char *) malloc(buf.ps_pathlen   =PS_PATHBLEN);          
    buf.ps_cmdptr    =(char *) malloc(buf.ps_cmdlen    =PS_CMDBLEN);

    token = w_getpsent(token, &buf, sizeof(buf));                              
    do {                                                                       
        token = w_getpsent(token, &buf, sizeof(buf));                          
        if (buf.ps_pid==pid) {
            free(buf.ps_conttyptr);                                                    
            free(buf.ps_pathptr);                                                      
            free(buf.ps_cmdptr);
            return true;
        }
    } while(token>0);

    free(buf.ps_conttyptr);                                                    
    free(buf.ps_pathptr);                                                      
    free(buf.ps_cmdptr);
    return false;
}
#endif

#if defined(PEGASUS_OS_AIX)

/////////////////////////////////////////////////////////////////////////////
// NAME           : processdata
// FUNCTION       : it calls subroutine getprocs() to get information
//                  about all processes.
/////////////////////////////////////////////////////////////////////////////
// ARGUMENTS:
//              cnt: the number of the processes in the returned table
// RETURN VALUES:
//              when it successfully gets the process table entries,
//              an array of procsinfo structures filled with process table
//              entries are returned. Otherewise, a null pointer is
//              returned.
//  
/////////////////////////////////////////////////////////////////////////////
struct procsinfo *processdata(int *cnt)
{
        struct procsinfo *proctable=NULL, *rtnp=NULL;
        int count=1048576, rtncnt, repeat=1, nextp=0;

        *cnt=0;
        while ( repeat && (rtncnt=getprocs(rtnp,PROCSIZE,0,0,&nextp, count))>0)
        {
                if (!rtnp)
                {
                        count=rtncnt;
                        proctable=(struct procsinfo *) malloc((size_t) \
                                        PROCSIZE*count);
                        if (!proctable)
                                return NULL;
                        rtnp=proctable;
                        nextp=0;
                } else
                {
                        *cnt+=rtncnt;
                        if (rtncnt>=count)
                        {
                                proctable=(struct procsinfo *) realloc(\
                                        (void*)proctable, (size_t)\
                                        (PROCSIZE*(*cnt+count)));
                                if (!proctable)
                                        return NULL;
                                rtnp=proctable+(*cnt);
                        } else
                                repeat=0;
                } // end of if(!rtnp)
        } //end of while
        return proctable;
}

/////////////////////////////////////////////////////////////////////////////
// NAME           : aixcimsrvrunning
// FUNCTION       : it figures out if the cimserver process is running
//                  by checking the process table entries in the array
//                  returned from processdata(). If the cimserver process is
//                  running and its pid is the same as the pid in the file
//                  getPIDFileName(), it will return 0, otherwise it will
//                  return -1.
/////////////////////////////////////////////////////////////////////////////
// ARGUMENTS:
//              pid: the process identifier saved in the file
//                   getPIDFileName()
// RETURN VALUES:
//              0: successful
//              -1: errors
//  
/////////////////////////////////////////////////////////////////////////////
       
int aixcimsrvrunning(pid_t pid, const char* processName)
{
        int i,count;
        struct procsinfo *proctable;

        proctable=processdata(&count);
        if (proctable==NULL)
                return -1;
        for (i=0;i<count;i++)
                if (!strcmp(proctable[i].pi_comm, processName) && \
                    proctable[i].pi_pid==pid)
                {
                        free(proctable);
                        return 0;
                }

        free(proctable);
        return -1;
}
#endif

Boolean ServerProcess::isCIMServerRunning(void)
{
  FILE *pid_file;
  pid_t pid = 0;

  // open the file containing the CIMServer process ID
  pid_file = fopen(getPIDFileName(), "r");
  if (!pid_file)
  {
      return false;
  }

  // get the pid from the file
  fscanf(pid_file, "%d\n", &pid);

  fclose(pid_file);

  if (pid == 0)
  {
     return false;
  }

  //
  // check to see if cimserver process is alive
  //
#if defined(PEGASUS_OS_HPUX)
  struct pst_status pstru;

  int ret_code;
  ret_code = pstat_getproc(&pstru, sizeof(struct pst_status), (size_t)0, pid);

  if ( (ret_code != -1 ) && (strcmp(pstru.pst_ucomm, getProcessName())) == 0)
  {
      //
      // Check to see if this command process has the same pid as the 
      // cimserver daemon process pid stored in the cimserver_start.conf 
      // file.  Since the command has the same name as the cimserver daemon
      // process, this could happen after a system reboot.  If the pids are
      // the same, cimserver isn't really running.
      //
      Uint32 mypid = System::getPID();
      if ((mypid != pid) && (parentPid != pid))
      {
          // cimserver is running
          return true;
      }
  }
#endif
#if defined(PEGASUS_PLATFORM_LINUX_GENERIC_GNU) || defined(PEGASUS_PLATFORM_SOLARIS_SPARC_CC)
  if (get_proc(pid) != -1 )
  {
      // cimserver is running
      return true;
  }
#endif
#if defined(PEGASUS_PLATFORM_ZOS_ZSERIES_IBM)
    return isProcRunning(pid);
#endif
#if defined(PEGASUS_OS_AIX)
    if (aixcimsrvrunning(pid, getProcessName())!=-1)
        return true;
#endif
  return false;
}

int ServerProcess::cimserver_kill(int id) 
{ 
  FILE *pid_file;
  pid_t pid = 0;
  
  // open the file containing the CIMServer process ID
  pid_file = fopen(getPIDFileName(), "r");
  if (!pid_file) 
  {
      return (-1);
  }

  // get the pid from the file
  fscanf(pid_file, "%d\n", &pid);

  fclose(pid_file);

  if (pid == 0)
  {
     System::removeFile(getPIDFileName());
     return (-1);
  }

  //
  // kill the process if it is still alive
  //
#if defined(PEGASUS_OS_HPUX)
  struct pst_status pstru;

  int ret_code;
  ret_code = pstat_getproc(&pstru, sizeof(struct pst_status), (size_t)0, pid);

  if ( (ret_code != -1 ) && (strcmp(pstru.pst_ucomm, getProcessName())) == 0)
  {
      // cimserver is running, kill the process
      kill(pid, SIGKILL);
  }
#endif
#if defined(PEGASUS_PLATFORM_LINUX_GENERIC_GNU) || defined(PEGASUS_PLATFORM_SOLARIS_SPARC_CC)
  if (get_proc(pid) != -1 )
  {
      kill(pid, SIGKILL);
  }
#endif
#if defined(PEGASUS_PLATFORM_ZOS_ZSERIES_IBM)
  if (isProcRunning(pid)) {
      kill(pid, SIGKILL);
  }
#endif
#if defined(PEGASUS_OS_AIX)
  if (!aixcimsrvrunning(pid, getProcessName()))
        kill(pid,SIGKILL);
#endif
  // remove the file
  System::removeFile(getPIDFileName());
  
  return(0);
}

// notify parent process to terminate so user knows that cimserver
// is ready to serve CIM requests.
void ServerProcess::notify_parent(int id)
{
  pid_t ppid = getppid();
  if (id)
   kill(ppid, SIGTERM);
  else
   kill(ppid, PEGASUS_SIGUSR1); 
}


// Platform specific run
int ServerProcess::platform_run( int argc, char** argv, Boolean shutdownOption )
{
    return cimserver_run( argc, argv, shutdownOption );
}






