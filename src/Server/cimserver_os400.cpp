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
// Author: Diane Olson (dianeols@us.ibm.com) 
//
// Modified By: Yi Zhou, Hewlett-Packard Company (yi_zhou@hp.com)
//
// Modified By: Dave Rosckes (rosckes@us.ibm.com)
//
// Modified By: Bert Rivero (hurivero@us.ibm.com) 
//
//%/////////////////////////////////////////////////////////////////////////////

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/String.h>
#include <Pegasus/Common/Logger.h>
#include "OS400ConvertChar.h"
#include <Pegasus/Common/MessageLoader.h> //l10n
#include <except.h>

#include <stdio.h>
#include "qycmutiltyUtility.H"
#include "qycmjobjobJob.H"
#include "qycmmsgclsMessage.H"
#include "licall.h"        // EPTCALL macros.

// Needed for QWTCHGJB API
#include <qwtchgjb.cleinc>
#include "qushdler.H"

//Needed for SQL APIs
#include "sqlcli.h"

// Structure need for the CHGJOB(QWTCHGJB) API
typedef struct jobChangeInfo
{
   Qus_Job_Change_Information_t  fieldNum;
   Qus_JOBC0100_t                format;
   char                          data[31];
} jobChangeInfo_t;

// Errorcode for system API calls
Qus_EC_t errorCode;

PEGASUS_USING_PEGASUS;
PEGASUS_USING_STD;

// Stub out all these functions.  Not used on the iSeries.
void cim_server_service(int argc, char **argv ) { return; }  

// notify parent process to terminate so user knows that cimserver
// is ready to serve CIM requests. If this plateform needs to implement
// this functionality, please see sample implementation in cimserver_unix.cpp.
void notify_parent(int id)
{
}

///////////////////////////////////////////////////////////////////////
// cimserver_fork()
//
// The iSeries qycmctlcimCimomServer.C (QYCMCTLCIM program) code has
// already checked that the CIMOM server is not already running prior
// to calling the CIMOM server (QYCMCIMOM) and telling it to start itself.
// Therefore, no check is made in this method to ensure the CIMOM server
// is not already running.
//
// Note: This code was written because fork( ) is not supported on OS/400.
///////////////////////////////////////////////////////////////////////
int cimserver_fork(void) 
{
#pragma convert(37)
  char rc5[3] = "05"; // rc5 means the CIMOM Server failed to start
  char cppServ[10] = "QYCMCIMOM";
  ycmJob cppJob(YCMJOB_SRVNAME_10, YCMJOB_SRVUSER_10); 

  // Submit the server job (QYCMCIMOM).  The job is submitted with the
  // daemon=false parameter to avoid an infinite loop of jobs being
  // submitted by this code!
  if (YCMJOB_SUBMIT_FAILED == cppJob.submit(YCMJOB_SRVR_PGM,  
					    YCMJOB_QSYS_LIB,
					    "daemon=false",
					    YCMJOB_JOBD,
					    YCMJOB_QSYS_LIB,
					    YCMJOB_CCSID_37,
					    YCMJOB_THREAD_YES))
  {  // QYCMCIMOM Server Failed on Submit Job
#pragma convert(0)

      //l10n
      //Logger::put(Logger::ERROR_LOG, System::CIMSERVER, Logger::SEVERE,
		 //"cimserver_os400::cimserver_fork() - SBMJOB failed to start the QYCMCIMOM server program!!");
	Logger::put_l(Logger::ERROR_LOG, System::CIMSERVER, Logger::SEVERE,
				"src.Server.cimserver_os400.FAILED_TO_START_SERVER",
		 		"$0 failed to start the $1 server program!!",
		 		"cimserver_os400::cimserver_fork() - SBMJOB",
		 		"QYCMCIMOM");


     char chData[sizeof(rc5)+sizeof(cppServ)];
     strcpy((char *)&chData,rc5);
     strcat(chData,cppServ);

#pragma convert(37)
     ycmMessage message("CPDDF80",
                        chData,
			strlen(chData),
                        "cimserver_os400::cimserver_fork()",
                        ycmCTLCIMID,
			utf8);
     message.joblogIt(UserError,ycmMessage::Diagnostic);
#pragma convert(0)

     // save the job log
     system ("QSYS/CHGJOB JOB(*) LOG(4 00 *SECLVL)");


     return(-1);	// -1 indicates to QYCMCIMOM that the server failed to start
  }

  // The QYCMCIMOM job was submitted with daemon=false.  This job can now exit.
  // This is similiar to what the unix version of this code does - the parent exits
  // after the fork( )
  return(0);
}


////////////////////////////////////////////////////
//  CancelHandler
////////////////////////////////////////////////////
void CancelHandler (_CNL_Hndlr_Parms_T *cancelParms)
{  // make sure a job log gets saved too
   system ("QSYS/CHGJOB JOB(*) LOG(4 00 *SECLVL)");
}


////////////////////////////////////////////////////
// iSeries-specific function to initialize the server.
// Does the following:
//    -- Sets the server type to QIBM_CIMOM
//       so that iNavigator can start/stop it.
//    -- Swaps the job user to QSYS. 
//    -- Changes the authority of QYCMJOBD
////////////////////////////////////////////////////
int cimserver_initialize(void)
{
    SQLHENV    henv; // SQL environment variable
    long       attr; // SQL attribute to be set

   // setup cancel handler to make sure job log gets saved if we exit abnormally
   // TODO:  this is currently commented out because it causes build errors -
   //        it compiles just fine though.  Hopefully this problem will be fixed
   //        (it's a known problem) and we can uncomment this #pragma. 
//   #pragma cancel_handler (CancelHandler, NULL)
    try {
   system ("QSYS/CHGJOB JOB(*) LOG(4 00 *SECLVL)");

#pragma convert(37)
   //////////////////////////////////////////
   // Change Job API call
   // Change the server type to QICM_CIMOM
   ////////////////////////////////////////// 
   jobChangeInfo_t chg = {1,
                          46,
                          1911,
                          'C',
                          0X40,0X40,0X40,30,
                          "QIBM_CIMOM                    ",
                         };

   // Initialize the error code structure to signal exceptions
   errorCode.Bytes_Provided = 0;

   // Call CHGJOB API with Server Type field
   EPTCALL(QWTCHGJB, ("*                         ",
            "                ",
            "JOBC0200",
            &chg,
            &errorCode), OFF, OFF);
#pragma convert(0)

   ////////////////////////////////////////////////////
   // Change authority to the qypsjobd job description
   //////////////////////////////////////////////////// 
   system("QSYS/GRTOBJAUT OBJ(QSYS/QYCMJOBD) OBJTYPE(*JOBD) USER(*PUBLIC) AUT(*EXCLUDE)");

   SQLAllocEnv(&henv);  // Allocating SQL environment variable
   attr = SQL_TRUE;     // Set SQL attribute to true
   
   // Set the SQL server mode to true.
   // This will allow multiple connections to the same data source.
   SQLSetEnvAttr(henv,SQL_ATTR_SERVER_MODE, &attr,0);
 }
  catch (...)
  {
      //l10n
      //Logger::put(Logger::ERROR_LOG, System::CIMSERVER, Logger::SEVERE,
		  //"cimerver_os400::cimserver_os400_setup() - caught unknown exception\n");
		Logger::put_l(Logger::ERROR_LOG, System::CIMSERVER, Logger::SEVERE,
			"src.Server.cimserver_os400.CAUGHT_UNKNOWN_EXCEPTION",
		  	"$0 caught unknown exception\n",
		  	"cimerver_os400::cimserver_os400_setup() -");

      return(-1);
  }

   // TODO:  this is currently commented out because it causes build errors -
   //        it compiles just fine though.  Hopefully this problem will be fixed
   //        (it's a known problem) and we can uncomment this #pragma. 
//  #pragma disable_handler

    return(0);
}

///////////////////////////////////////////////////////////////////////
// cimserver_kill()
//
// The iSeries qycmctlcimCimomServer.C (QYCMCTLCIM program) code has
// already checked that the CIMOM server is already running prior to
// calling the CIMOM server (QYCMCIMOM) and telling it to shutdown.
// However, a check is still made in this method because we have to
// find the job number in order to kill the job.
//
// For iSeries, this method is called regardless of whether we took
// errors trying to connect to the server - if the CIMOM server job
// is anywhere on the system, in any state, this method will find it
// and kill it dead!!
//
// NEVER call this method unless the server is unable to be shut down
// gracefully.
///////////////////////////////////////////////////////////////////////
int cimserver_kill(void)
{  // Need to kill the server
#pragma convert(37)
  char rc2[3] = "02"; // CIMOM server failed to end
  char cppServ[10] = "QYCMCIMOM";

  // Construct a ycmJob object
  ycmJob cppJob(YCMJOB_SRVNAME_10, YCMJOB_SRVUSER_10);
  // Find the QYCMCIMOM job
  char cppStatus  = cppJob.find(YCMJOB_ALL_NUMBERS);

  if (cppStatus == YCMJOB_FOUND)       // CIMOM Server is Running
  {
    if (cppJob.end((char *)cppJob.getNumber().c_str(), 'C', 30) == YCMJOB_END_FAILED)
    {

      char chData[sizeof(rc2)+sizeof(cppServ)];
      strcpy((char *)&chData,rc2);
      strcat(chData,cppServ);

      ycmMessage message("CPDDF81",
                         chData,
			 strlen(chData),
                         "cimserver_os400::cimserver_kill()",
                         ycmCTLCIMID,
			 utf8);
      message.joblogIt(UserError,ycmMessage::Diagnostic);

#pragma convert(0)

      //l10n
      //Logger::put(Logger::ERROR_LOG, System::CIMSERVER, Logger::SEVERE,
		  //"cimserver_os400::cimserver_kill - FAILED to end the QYCMCIMOM job!!");
	Logger::put_l(Logger::ERROR_LOG, System::CIMSERVER, Logger::SEVERE,
			"src.Server.cimserver_os400.FAILED_TO_END_JOB",
		  	"$0 FAILED to end the $1 job!!",
		  	"cimserver_os400::cimserver_kill -",
		  	"QYCMCIMOM");


      return -1; // Note: this return code is ignored by the CIMOM server.
    }
  }
  // The case of the job not found is already handled in QYCMCTLCIM program
    return(0);
}

////////////////////////////////////////////////////
//  Checks if the QYCMCIMOM server job is running.
////////////////////////////////////////////////////
Boolean isCIMServerRunning(void)
{

  // Construct a ycmJob object
  ycmJob cppJob(YCMJOB_SRVNAME_10, YCMJOB_SRVUSER_10);

  // Find the QYCMCIMOM job
  char cppStatus  = cppJob.find(YCMJOB_ALL_NUMBERS);

  if (cppStatus == YCMJOB_FOUND)       // CIMOM Server is Running
  {
      return true; 
  }

  return false;
}

////////////////////////////////////////////////////
//  Setup a fifo for process communication
////////////////////////////////////////////////////
int init_fifo(const char * fifo_name){
    int fifo = -1;
    struct stat FIFO_STAT;

    // Need to convert the fifo name to ebcdic because
    // that is what stat and open want.   
    char tmp[256];
    strcpy(tmp, fifo_name);
    AtoE(tmp);
    
    int stat_rc = stat( tmp, &FIFO_STAT );

    // check if the FIFO already exists
    if( S_ISFIFO( FIFO_STAT.st_mode ) ){
	// prep FIFO, on this end we only want to write to it,
	// set its I/O mode to not block on any reads
	fifo = open(tmp, O_RDWR | O_NONBLOCK);
    }
    return fifo;
}
 
 
////////////////////////////////////////////////////
//  Sends return code back to caller before exiting...
////////////////////////////////////////////////////
void cimserver_exitRC(int rc){
    int fifo = init_fifo(QYCMSSERV_FIFO);
    
    if( fifo != -1 ){
	char rc_tmp[3];
	memset(rc_tmp, 0, 3);
 	sprintf(rc_tmp,"%d",rc);
	AtoE(rc_tmp);  // qycmctlcim wants ebcdic
 	write(fifo,rc_tmp,strlen(rc_tmp));
    }
}

////////////////////////////////////////////////////
// Platform specific run
////////////////////////////////////////////////////
int platform_run( int argc, char** argv, Boolean shutdownOption )
{
	return cimserver_run( argc, argv, shutdownOption );
}

void cimserver_set( CIMServer* s )
{
}
