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
// Author: Bob Blair (bblair@bmc.com)
//
// Modified By: Seema Gupta (gseema@in.ibm.com)
//              Alagaraja Ramasubramanian, IBM (alags_raj@in.ibm.com) - PEP-167
//              Amit K Arora, IBM (amitarora@in.ibm.com) - Bug#2333
//              Josephine Eskaline Joyce, IBM (jojustin@in.ibm.com) - Bug#2756, Bug#3032
//
//%/////////////////////////////////////////////////////////////////////////////


// A main for the cimmof_parser.  It can be embedded elsewhere, too. 

#include <Pegasus/Common/Config.h>
#include <iostream>
#include <Pegasus/Compiler/mofCompilerOptions.h>
#include "cmdlineExceptions.h"
#include "cmdline.h" 
#include <Pegasus/Compiler/cimmofParser.h>
#include <Pegasus/Compiler/parserExceptions.h>

#ifdef PEGASUS_OS_OS400
#include <qycmutiltyUtility.H>
#include <qycmutilu2.H>
#include "vfyptrs.cinc"
#include <stdio.h>
#include "OS400ConvertChar.h"
#define DISABLE_CIMMOFL_WARNING
#endif

PEGASUS_USING_STD;

#ifdef PEGASUS_HAVE_NAMESPACES
using namespace ParserExceptions;
#endif

// This is used by the parsing routines to control flow
// through include files
static mofCompilerOptions cmdline;


extern "C++" int processCmdLine(int, char **, mofCompilerOptions &, ostream &);

extern "C++" ostream& help(ostream& os, int progtype);

#ifndef DISABLE_CIMMOFL_WARNING 
extern "C++" ostream& cimmofl_warning(ostream& os);
#endif

#ifdef PEGASUS_OS_OS400
// Escape message generator for os400, reports errors that cimmofl encounters.
void os400_return_msg(int ret, String msg_, Boolean qsh);
#endif

#define NAMESPACE_ROOT "root/cimv2"

static const char MSG_PATH [] 				= "pegasus/pegasusServer";

int
main(int argc, char ** argv) {
  int ret = 0;
  String msg_;
  MessageLoaderParms parms;

  MessageLoader::_useProcessLocale = true; //l10n set message loading to use process locale
  MessageLoader::setPegasusMsgHomeRelative(argv[0]);

#ifdef PEGASUS_OS_OS400

  VFYPTRS_INCDCL;               // VFYPTRS local variables

  // verify pointers
  #pragma exception_handler (qsyvp_excp_hndlr,qsyvp_excp_comm_area,\
    0,_C2_MH_ESCAPE)
    for( int arg_index = 1; arg_index < argc; arg_index++ ){
	VFYPTRS(VERIFY_SPP_NULL(argv[arg_index]));
    }
  #pragma disable_handler

    // Convert the args to ASCII
    for(Uint32 i = 0;i< argc;++i)
    {
	EtoA(argv[i]);
    }

    // Set the stderr stream to buffered with 32k.
    // Allows utf-8 to be sent to stderr (P9A66750).
    setvbuf(stderr, new char[32768], _IOLBF, 32768);

    // check what environment we are running in, native or qsh
    Boolean qsh = false;
    if( getenv(
#pragma convert(37)
	       "SHLVL"
#pragma convert(0)
	       ) == NULL ){  // native mode
	// Check to ensure the user is authorized to use the command,
	// suppress diagnostic message, send escape message
	if(FALSE == ycmCheckCmdAuthorities(1)){
	  ycmSend_Message_Escape(CPFDF80_RC,
				 NULL,
				 NULL,
#pragma convert(37)
				 "*CTLBDY   ",
#pragma convert(0)
				 1);
	  return CPFDF80_RC;
	}
    }
    else{ // qsh mode
	// Check to ensure the user is authorized to use the command
	// ycmCheckCmdAuthorities() will send a diagnostic message to qsh
        qsh = true;
	if(FALSE == ycmCheckCmdAuthorities()){
	  return CPFDF80_RC;
	}
    }

#endif

  try {
    ret = processCmdLine(argc, argv, cmdline, cerr);
  } catch (ArgumentErrorsException &e) {
   String msg(e.getMessage());

   cerr << argv[0] << ": " << msg ;

        if (msg.find(String("Unknown flag")) != PEG_NOT_FOUND)
         {
            MessageLoaderParms parms;
            parms.msg_id = "Compiler.cimmofMessages.ERR_OPTION_NOT_SUPPORTED";
            parms.default_msg = "Invalid option. Use '--help' to obtain command syntax.";
            parms.msg_src_path = MSG_PATH;
            cerr << argv[0] << ": " << MessageLoader::getMessage(parms) << endl;
         }
        else
         {
            MessageLoaderParms parms;
            parms.msg_id = "Compiler.cimmofMessages.ERR_USAGE";
            parms.default_msg = "Incorrect usage. Use '--help' to obtain command syntax.";
            parms.msg_src_path = MSG_PATH;
            cerr << argv[0] << ": " << MessageLoader::getMessage(parms) << endl;
         }

    ret =  PEGASUS_CIMMOF_CIM_EXCEPTION;
  } catch (CmdlineNoRepository &e) {
    cerr << e.getMessage() << endl;
    ret = PEGASUS_CIMMOF_CMDLINE_NOREPOSITORY;
  } catch (CIMException &e) {
  	//l10n
    //cerr << "Unexpected condition: " << e.getMessage() << endl;
    parms.msg_id = "Compiler.cmdline.cimmof.main.UNEXPECTED_CONDITION";
    parms.default_msg = "Unexpected condition: ";
    cerr << MessageLoader::getMessage(parms) << e.getMessage() << endl;
    ret = PEGASUS_CIMMOF_UNEXPECTED_CONDITION;
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

#ifdef PEGASUS_OS_OS400
    os400_return_msg(ret, msg_, qsh);
#endif

    return ret;
  }

#ifndef DISABLE_CIMMOFL_WARNING 
  if (cmdline.is_local())
  {
        cimmofl_warning(cerr);
  }
#endif

  const Array<String>& filespecs = cmdline.get_filespec_list();

  // For most options, a real repository is required.  If we can't
  // create one and we need to, bail. 
  cimmofParser *p = cimmofParser::Instance(); 
  p->setCompilerOptions(&cmdline);
  if ( p->setRepository() ) {	
    p->setDefaultNamespacePath(NAMESPACE_ROOT);
  } else {
  	//l10n
    //cerr << "Failed to set DefaultNamespacePath." << endl;
    parms.msg_id = "Compiler.cmdline.cimmof.main.FAILED_TO_SET";
    parms.default_msg = "Failed to set DefaultNamespacePath.";
    cerr << MessageLoader::getMessage(parms) << endl;
    // ATTN: P3 BB 2001 Did not set namespace.  We may need to log an error here.
	ret = PEGASUS_CIMMOF_NO_DEFAULTNAMESPACEPATH;
#ifdef PEGASUS_OS_OS400
    os400_return_msg(ret, msg_, qsh);
#endif
    return ret;
  }
  if (filespecs.size())    // user specified command line args
    for (unsigned int i = 0; i < filespecs.size(); i++) {
      if (p->setInputBufferFromName((const String &)filespecs[i]) == 0) {
        try {
          ret = p->parse();
        } catch(ParserLexException &e) {
          //l10n
          parms.msg_id = "Compiler.cmdline.cimmof.main.LEXER_ERROR";
          parms.default_msg = "Lexer error: ";
          //msg_ = String("Lexer error: ").append(e.getMessage());
          msg_ = MessageLoader::getMessage(parms).append(e.getMessage());
          ret = PEGASUS_CIMMOF_PARSER_LEXER_ERROR ;
        } catch(Exception &e) {
          //l10n
          parms.msg_id = "Compiler.cmdline.cimmof.main.PARSING_ERROR";
          parms.default_msg = "Parsing error: ";
          //msg_ = String("Parsing error: ").append(e.getMessage());
          msg_ = MessageLoader::getMessage(parms).append(e.getMessage());
          ret = PEGASUS_CIMMOF_PARSING_ERROR;
        }
      } else {
        //l10n
        // ATTN: l10n TODO - this path was not localized by the msg freeze for R2.3.  So, use an
        // internal exception msg for now.  But, need to replace this with
        // a new cimmof msg in release 2.4.
        parms.msg_id = "Common.InternalException.CANNOT_OPEN_FILE";
        parms.default_msg = "Can't open file $0";
        parms.arg0 = filespecs[i];
        //msg_ = String("Can't open file ").append(filespecs[i]);
        msg_ = MessageLoader::getMessage(parms);
        ret = PEGASUS_CIMMOF_BAD_FILENAME;
      }
    }
  else {
    try {
    int ret =  p->parse();
    } catch(ParserLexException &e) {
    	//l10n
    	parms.msg_id = "Compiler.cmdline.cimmof.main.LEXER_ERROR";
        parms.default_msg = "Lexer error: ";
	    //msg_ = String("Lexer error: ").append(e.getMessage());
	    msg_ = MessageLoader::getMessage(parms).append(e.getMessage());
        ret = PEGASUS_CIMMOF_PARSER_LEXER_ERROR ;
    } catch(Exception &e) {
    	//l10n
    	parms.msg_id = "Compiler.cmdline.cimmof.main.GENERAL_EXCEPTION";
    	parms.default_msg = "Compiler general exception: ";
	    //msg_ = String("Compiler general exception: ").append(e.getMessage());
	    msg_ = MessageLoader::getMessage(parms).append(e.getMessage());
        ret = PEGASUS_CIMMOF_COMPILER_GENERAL_EXCEPTION;
    }
  }
  cerr << msg_ << endl;

#ifdef PEGASUS_OS_OS400
  os400_return_msg(ret, msg_, qsh);
#endif

  return ret;
}

// os400_return_msg:  creates an escape message for the os400, reports any 
//                    severe errors that cimmofl encounters.
// @ret:  the (unix) return status.
// @msg_:  The general error message. 
// @qsh:  shell status (QSHELL or Native mode).
#ifdef PEGASUS_OS_OS400
void
os400_return_msg(int ret, String msg_, Boolean qsh)
{
  // Send good completion message to stdout if the compile worked.
  // Callers of QYCMMOFL *PGM from the native command line will want to see this.
  // Note: in PTF mode the quiet option should be used.
  if (ret == 0 && !cmdline.quiet())  
  {
      //l10n
      //cout << "Compile completed successfully." << endl;
      MessageLoaderParms parms;
      parms.msg_id = "Compiler.cmdline.cimmof.main.COMPILE_SUCCESSFUL";
      parms.default_msg = "Compile completed successfully.";
      cout << MessageLoader::getMessage(parms) << endl;
  }

  // Send a CPDDF83 and CPFDF81 OS/400 program message if an error occurred,
  // and we are running in native mode.
  // This will allow PTFs to monitor for this message.
  if (ret != 0 && !qsh)
  {
      message_t    message_;	// Message information
      cmd_msg_t    cmdMSG;
      memset((char *)&cmdMSG, ' ', sizeof(cmd_msg_t) ); // init to blanks
      memcpy(cmdMSG.commandName, "QYCMMOFL", 8);  // must be in utf-8
      CString utf8 = msg_.getCString();
      if (strlen((const char *)utf8) <= 200)            // max repl data is 200 chars  
	  memcpy(cmdMSG.message, utf8, strlen((const char *)utf8));
      else
	  memcpy(cmdMSG.message, utf8, 200);
#pragma convert(37)
      memcpy(message_.MsgId, "CPDDF83", 7);
      memcpy(message_.MsgData, (char *)&cmdMSG, sizeof(cmd_msg_t));
      message_.MsgLen = sizeof(cmd_msg_t);
      memcpy(message_.MsgType, "*DIAG     ", 10); 
      ycmSend_Message(&message_,
		      "*CTLBDY   ",
		      1,
		      true);                     // repl data is utf-8 

      ycmSend_Message_Escape(CPFDF81_RC,
			     "03",               // must be in ccsid 37
			     "QYCMMOFL",         // must be in ccsid 37
			     "*CTLBDY   ",
			     1);
#pragma convert(0)
  }
}
#endif
