//%/////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001, 2002 BMC Software, Hewlett-Packard Company, IBM,
// The Open Group, Tivoli Systems
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
// Author: Bob Blair (bblair@bmc.com)
//
// Modified By:
//
//%/////////////////////////////////////////////////////////////////////////////


// A main for the cimmof_parser.  It can be embedded elsewhere, too. 

#include <Pegasus/Common/Config.h>
#include <iostream>
#include <Pegasus/Compiler/mofCompilerOptions.h>
#include "cmdlineExceptions.h"
#include <Pegasus/Compiler/cimmofParser.h>
#include <Pegasus/Compiler/parserExceptions.h>

#include <Pegasus/Common/PegasusVersion.h>

#ifdef PEGASUS_OS_OS400
#include <qycmutiltyUtility.H>
#include "vfyptrs.cinc"
#include <stdio.h>
#endif

PEGASUS_USING_STD;

#ifdef PEGASUS_HAVE_NAMESPACES
using namespace ParserExceptions;
#endif

// This is used by the parsing routines to control flow
// through include files
static mofCompilerOptions cmdline;


extern "C++" int processCmdLine(int, char **, mofCompilerOptions &, ostream &);

extern "C++" ostream& help(ostream& os);

#define NAMESPACE_ROOT "root/cimv2"

int
main(int argc, char ** argv) {
  int ret = 0;
  String msg_;
  MessageLoaderParms parms;

#ifdef PEGASUS_OS_OS400

  VFYPTRS_INCDCL;               // VFYPTRS local variables

  // verify pointers
  #pragma exception_handler (qsyvp_excp_hndlr,qsyvp_excp_comm_area,\
    0,_C2_MH_ESCAPE)
    for( int arg_index = 1; arg_index < argc; arg_index++ ){
	VFYPTRS(VERIFY_SPP_NULL(argv[arg_index]));
    }
  #pragma disable_handler

  // check what environment we are running in, native or qsh
    if( getenv("SHLVL") == NULL ){  // native mode
	// Check to ensure the user is authorized to use the command,
	// suppress diagnostic message, send escape message
	if(FALSE == ycmCheckCmdAuthorities(1)){
	  ycmSend_Message_Escape(CPFDF80_RC);
	  return CPFDF80_RC;
	}
    }
    else{ // qsh mode
	// Check to ensure the user is authorized to use the command
	// ycmCheckCmdAuthorities() will send a diagnostic message to qsh
	if(FALSE == ycmCheckCmdAuthorities()){
	  return CPFDF80_RC;
	}
    }

#endif

  try {
    ret = processCmdLine(argc, argv, cmdline, cerr);
  } catch (ArgumentErrorsException &e) {
    cerr << e.getMessage() << endl;
    help(cerr);
    ret =  -2;
  } catch (CmdlineNoRepository &e) {
    cerr << e.getMessage() << endl;
    ret = -3;
  } catch (CIMException &e) {
  	//l10n
    //cerr << "Unexpected condition: " << e.getMessage() << endl;
    parms.msg_id = "Compiler.cmdline.cimmof.main.UNEXPECTED_CONDITION";
    parms.default_msg = "Unexpected condition: ";
    cerr << MessageLoader::getMessage(parms) << e.getMessage() << endl;
    ret = -4;
  }
  if (ret) {
    if (ret > 0) {
    	//l10n
      //cerr << "Unexpected result from processing command line: " << ret <<endl;
      //cerr << "Compilation terminating." << endl;
      parms.msg_id = "Compiler.cmdline.cimmof.main.UNEXPECTED_RESULT";
      parms.default_msg = "Unexpected result from processing command line: $0";
      parms.arg0 = ret;
      cerr << MessageLoader::getMessage(parms) << endl;
      parms.msg_id = "Compiler.cmdline.cimmof.main.COMPILE_TERMINATING";
      parms.default_msg = "Compilation terminating.";
      cerr << MessageLoader::getMessage(parms) << endl;
    }
    return ret;
  }
  const Array<String>& filespecs = cmdline.get_filespec_list();

#ifdef PEGASUS_OS_OS400
  // If quiet mode is chosen then shut down stdout and stderr.
  // This is used during product installation and PTF application.
  // We must be absolutely quiet to avoid a terminal being
  // activated in native mode.
  if (cmdline.quiet())
  {
      // Redirect to /dev/null.
      // Works for both qshell and native modes.
      freopen("/dev/null","w",stdout);
      freopen("/dev/null","w",stderr);
  } 	
#endif

  // For most options, a real repository is required.  If we can't
  // create one and we need to, bail. 
  cimmofParser *p = cimmofParser::Instance(); 
  p->setCompilerOptions(&cmdline);
  if ( p->setRepository() ) {	
    p->setDefaultNamespacePath(NAMESPACE_ROOT);
  } else {
  	//l10n
    //cerr << "Failed to set DefaultNamespacePath." << endl;
    String s1 = "DefaultNamespacePath";
    parms.msg_id = "Compiler.cmdline.cimmof.main.FAILED_TO_SET";
    parms.default_msg = "Failed to set $0.";
    parms.arg0 = s1;
    cerr << MessageLoader::getMessage(parms) << endl;
    // ATTN: P3 BB 2001 Did not set namespace.  We may need to log an error here.
    return ret;
  }
  if (filespecs.size())    // user specified command line args
    for (unsigned int i = 0; i < filespecs.size(); i++) {
      if (p->setInputBufferFromName((const String &)filespecs[i]) == 0) {
	try {
	  ret = p->parse();
	} catch(ParserLexException &e) {
		//l10n
		String s1 = "Lexer";
		parms.msg_id = "Compiler.cmdline.cimmof.main.ERROR";
		parms.default_msg = "$0 error: ";
		parms.arg0 = s1;
	    //msg_ = String("Lexer error: ").append(e.getMessage());
	    msg_ = MessageLoader::getMessage(parms).append(e.getMessage());
	    ret = -5;
	} catch(Exception &e) {
		//l10n
		String s1 = "Parsing";
		parms.msg_id = "Compiler.cmdline.cimmof.main.ERROR";
		parms.default_msg = "$0 error: ";
		parms.arg0 = s1;
	    //msg_ = String("Parsing error: ").append(e.getMessage());
	    msg_ = MessageLoader::getMessage(parms).append(e.getMessage());
	    ret = -6;
	}
      } else {
	  msg_ = String("Can't open file ").append(filespecs[i]);
          ret = -7;
      }
    }
  else {
    try {
    int ret =  p->parse();
    } catch(ParserLexException &e) {
    	//l10n
    	String s1 = "Lexer";
    	parms.msg_id = "Compiler.cmdline.cimmof.main.ERROR";
		parms.default_msg = "$0 error: ";
		parms.arg0 = s1;
	    //msg_ = String("Lexer error: ").append(e.getMessage());
	    msg_ = MessageLoader::getMessage(parms).append(e.getMessage());
        ret = -5;
    } catch(Exception &e) {
    	//l10n
    	parms.msg_id = "Compiler.cmdline.cimmof.main.GENERAL_EXCEPTION";
    	parms.default_msg = "Compiler general exception: ";
	    //msg_ = String("Compiler general exception: ").append(e.getMessage());
	    msg_ = MessageLoader::getMessage(parms).append(e.getMessage());
        ret = -8;
    }
  }
  cerr << msg_ << endl;

#ifdef PEGASUS_OS_OS400
  // Send good completion message to stdout if the compile worked.
  // Callers of QYCMMOFL *PGM from the native command line will want to see this.
  if (ret == 0)
  		//l10n
      //cout << "Compile completed successfully." << endl;
      parms.msg_id = "Compiler.cmdline.cimmof.main.COMPILE_SUCCESSFUL";
      parms.default_msg = "Compile completed successfully.";
      cout << MessageLoader::getMessage(parms) << endl;

  // Send a CPDDF83 and CPFDF81 OS/400 program message if an error occurred
  // This will allow PTFs to monitor for this message.
  if (ret != 0)
  {
      message_t    message_;	// Message information
      cmd_msg_t    cmdMSG;
      memset((char *)&cmdMSG, ' ', sizeof(cmd_msg_t) ); // init to blanks
      memcpy(cmdMSG.commandName, "QYCMMOFL", 7);
      memcpy(cmdMSG.message, msg_.getCString(), sizeof(msg_.getCString()) );
      memcpy(message_.MsgId, "CPDDF83", 7);
      memcpy(message_.MsgData, (char *)&cmdMSG, sizeof(cmd_msg_t));
      message_.MsgLen = sizeof(cmd_msg_t);
      memcpy(message_.MsgType, "*DIAG     ", 10); 
      ycmSend_Message(&message_);

      ycmSend_Message_Escape(CPFDF81_RC, "03", "QYCMMOFL");
  }
#endif

  return ret;
}
