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
// Modified By: Gerarda Marquez (gmarquez@us.ibm.com)
//              -- PEP 43
//
//%/////////////////////////////////////////////////////////////////////////////


//
//
// process the command line, getting the repository location and
// the include path list and the other options
//

#include <Pegasus/Common/Config.h>
#include <fstream>
#include <cstdlib>
#include <iostream>
#include <Pegasus/Common/String.h>
#include <Pegasus/Common/FileSystem.h>

PEGASUS_USING_STD;
PEGASUS_USING_PEGASUS;

#include <Pegasus/Compiler/mofCompilerOptions.h>
#include "cmdlineExceptions.h"
#include "cmdline.h"
#include "cmdlineExceptions.h"
#include <Pegasus/getoopt/getoopt.h>
#include <fstream>
#include <Pegasus/Common/String.h>
//#include <strstream>
//#include <sstream>
#ifdef PEGASUS_OS_OS400
#include <qycmutiltyUtility.H> 
#include <qycmutilu2.H>
#endif 


// COMPILER VERSION ++++++++++++++++++++++++++++++++++++++++++++++++++++
#define COMPILER_VERSION "1.2.00" /* as of April 28, 2003 */
// COMPILER_VERSION ++++++++++++++++++++++++++++++++++++++++++++++++++++
 
#define DEFAULT_SERVER_AND_PORT "localhost:5988"

ostream & 
help(ostream &os) {
	
//l10n menu
  String help = "MOF Compiler version ";
  help.append(COMPILER_VERSION).append("\n\n");
#ifdef PEGASUS_OS_HPUX
  help.append("Usage: cimmof [-h] [-w] [-uc] [-aE | -aV | -aEV] [-I path] [-n namespace] [file, ...]\n");
  help.append( "    -h           - show this help \n");
  help.append( "    -w           - suppress warnings \n");
  help.append( "    -I path      - specify an include path \n");
  help.append( "    -n namespace - override the default CIMRepository namespace \n");
  help.append( "  -uc   -- allow update of an existing class definition.\n");
  help.append( "  -aE   -- allow creation, either through addition or modification, of Experimental classes.\n");
  help.append( "  -aV   -- allow both Major and Down Revision Schema changes.\n");
  help.append( "  -aEV  -- allow both Experimental and Version Schema changes.\n");
#else
#ifndef PEGASUS_OS_OS400
  help.append( "Usage: cimmof [-h] [-E] [-w] [-uc] [-aE | -aV | -aEV] [-R repository] [-I path] [-n namespace] [--xml] [--trace] -ffile\n");
  help.append( "       cimmof [-h] [-E] [-w] [-uc] [-aE | -aV | -aEV] [-R repository] [-I path] [-n namespace] [--xml] [--trace] [mof_files...]\n");
#else
  help.append( "Usage: cimmof [-h] [-E] [-w] [-uc] [-aE | -aV | -aEV] [-R repository] [-I path] [-n namespace] [--xml] [--trace] [-q] -ffile\n");
  help.append( "       cimmof [-h] [-E] [-w] [-uc] [-aE | -aV | -aEV] [-R repository] [-I path] [-n namespace] [--xml] [--trace] [-q] mof_files...\n");
#endif
  help.append( "  -h, --help -- show this help.\n");
  help.append( "  -E -- syntax check only.\n");
  help.append( "  -w -- suppress warnings.\n");
  help.append( "  -uc   -- allow update of an existing class definition.\n");
  help.append( "  -aE   -- allow creation, either through addition or modification, of Experimental classes.\n");
  help.append( "  -aV   -- allow both Major and Down Revision Schema changes.\n");
  help.append( "  -aEV  -- allow both Experimental and Version Schema changes.\n");
#ifdef PEGASUS_OS_OS400
  help.append( "  -q -- suppress all messages except command line usage errors.\n");
#endif
  help.append( "  -Rrepository -- specify the repository path (cimmofl) or hostname:portnumber (cimmof)\n");
  help.append( "  --CIMRepository=repository -- specify repository path or hostname:portnumber.\n");
  help.append( "  -Ipath -- specify an include path.\n");
  help.append( "  -ffile -- specify file containing a list of MOFs to compile.\n");
  help.append( " --file=file -- specify file containing list of MOFs.\n");
  help.append( " -npath -- override the default CIMRepository namespace (root/cimv2).\n");
  help.append( " --namespace=path -- override default CIMRepository namespace (root/cimv2).\n");
  help.append( " --xml -- output XML only, to stdout.  Do not update repository.\n");
  help.append( " --trace or --trace=ttracefile -- trace to file (default to stdout).\n");
#endif

// now localize the menu based on platform, use help as the default menu which
// has been appropriately built above for the specific platform

MessageLoaderParms parms("Compiler.cmdline.cimmof.cmdline.MENU.STANDARD",help,COMPILER_VERSION); 

#ifdef PEGASUS_OS_HPUX
	parms = MessageLoaderParms("Compiler.cmdline.cimmof.cmdline.MENU.PEGASUS_OS_HPUX",help,COMPILER_VERSION);	
#endif
#ifdef PEGASUS_OS_OS400
	parms = MessageLoaderParms("Compiler.cmdline.cimmof.cmdline.MENU.PEGASUS_OS_OS400",help,COMPILER_VERSION);
#endif

  os << MessageLoader::getMessage(parms);
  return os;
}

// If the 'f' flag is encountered, it names a file that contains the
// names of files to be processed.  Open the file and stick them into
// the filelist.
static int
process_filelist(const String &filename, mofCompilerOptions &cmdlinedata)
{
  String line;

  ifstream ifs;
  Open(ifs, filename);

  while (ifs != 0) {
    GetLine(ifs, line);   
    cmdlinedata.add_filespecs(line);
  }
  return 0;
}

/* flag value, type, islong?, needsValue? */
static struct optspec optspecs[] = 
{
    {(char*)"", FILESPEC, false, getoopt::NOARG},
    {(char*)"h", HELPFLAG, false, getoopt::NOARG},
    {(char*)"help", HELPFLAG, true, getoopt::NOARG},
    {(char*)"n", NAMESPACE, false, getoopt::MUSTHAVEARG},
    {(char*)"namespace", NAMESPACE, true, getoopt::MUSTHAVEARG}, 
    {(char*)"I", INCLUDEPATH, false, getoopt::MUSTHAVEARG},
    {(char*)"Include", INCLUDEPATH, true, getoopt::MUSTHAVEARG},
    {(char*)"w", SUPPRESSFLAG, false, getoopt::NOARG},
    {(char*)"R", REPOSITORYNAME, false, getoopt::MUSTHAVEARG},
    {(char*)"CIMRepository", REPOSITORYNAME, true, getoopt::MUSTHAVEARG},
    {(char*)"u", UPDATEFLAG, false, getoopt::MUSTHAVEARG},
    {(char*)"a", ALLOWFLAG, false, getoopt::MUSTHAVEARG},
#ifndef PEGASUS_OS_HPUX
    {(char*)"f", FILELIST, false, getoopt::MUSTHAVEARG},
    {(char*)"filelist", FILELIST, true, getoopt::MUSTHAVEARG},
    {(char*)"E", SYNTAXFLAG, false, getoopt::NOARG}, 
    {(char*)"trace", TRACEFLAG, true, getoopt::OPTIONALARG},
    {(char*)"xml", XMLFLAG, true, getoopt::NOARG},
#endif
#ifdef PEGASUS_OS_OS400
    {(char*)"q", QUIETFLAG, false, getoopt::NOARG},
#endif
    {(char*)"", OPTEND, false, getoopt::NOARG}
};

static void
setCmdLineOpts(getoopt &cmdline) {
  for (unsigned int i = 0; ; i++) {
    const optspec &o = optspecs[i];
    if (o.catagory == OPTEND)
      break;
    if (o.flag == "")
      continue;
    if (o.islong)
      cmdline.addLongFlagspec(o.flag, (getoopt::argtype)o.needsvalue);
    else
      cmdline.addFlagspec(o.flag[0], o.needsvalue == getoopt::MUSTHAVEARG ?
			  true : false);
  }
}

static opttypes
catagorize(const Optarg &arg) {
  
  for (unsigned int i = 0; ; i++) {
    const optspec &o = optspecs[i];
    if (o.catagory == OPTEND)
      break;
    if (arg.getName() == o.flag)
      return o.catagory;
  }
  return OPTEND;
}

static void
applyDefaults(mofCompilerOptions &cmdlinedata) {
  if (cmdlinedata.is_local()) {
    char *peghome = getenv(PEGASUS_HOME);
    if (peghome) {
#if defined(PEGASUS_PLATFORM_OS400_ISERIES_IBM)
	char home[256] = {0};
	if (strlen(peghome) < 256)
	{
	    strcpy(home, peghome);
	    EtoA(home);
	}
	cmdlinedata.set_repository_name(home);
#else
      cmdlinedata.set_repository_name(peghome);
#endif
    } else {
#if defined(PEGASUS_PLATFORM_OS400_ISERIES_IBM)
      // Default to the shipped OS/400 CIM directory so that
      // the user doesn't need to set PEGASUS_HOME
      cmdlinedata.set_repository_name(OS400_DEFAULT_PEGASUS_HOME);
#endif
    }
  } else {
    cmdlinedata.set_repository_name(DEFAULT_SERVER_AND_PORT);
  }
  cmdlinedata.reset_syntax_only();
  cmdlinedata.reset_suppress_warnings();
  cmdlinedata.reset_suppress_all_messages();
  cmdlinedata.reset_trace();
  cmdlinedata.add_include_path(".");
  cmdlinedata.set_namespacePath(ROOTCIMV2);
  cmdlinedata.set_erroros(PEGASUS_STD(cerr));
  cmdlinedata.set_warningos(PEGASUS_STD(cerr));
  cmdlinedata.reset_operationType();
  cmdlinedata.reset_xmloutput();
  cmdlinedata.reset_update_class();
  cmdlinedata.reset_allow_experimental();
  cmdlinedata.reset_allow_version();
#ifdef PEGASUS_OS_OS400
  cmdlinedata.reset_quiet();
#endif
}

#if defined(PEGASUS_PLATFORM_WIN32_IX86_MSVC)
# define SEPCHAR '\\'
#else
# define SEPCHAR '/'
#endif

// This function looks at a string (which we suppose to be argv[0]
// and decides if it represents cimmof or cimmofl
int
getType(const char *name)
{
#if defined(PEGASUS_PLATFORM_OS400_ISERIES_IBM)
    // Only the local compiler is shipped on OS/400, and
    // it is called QYCMMOF.  Force the local return code.
    return 1;
#else
  const char *pos;
  pos = strrchr(name, SEPCHAR);
  if (!pos)
    pos = name;
  else
    pos++;
  if (*pos != 'c' && *pos != 'C') return -1;
  pos++;
  if (*pos != 'i' && *pos != 'I') return -1;
  pos++;
  if (*pos != 'm' && *pos != 'M') return -1;
  pos++;
  if (*pos != 'm' && *pos != 'M') return -1;
  pos++;
  if (*pos != 'o' && *pos != 'O') return -1;
  pos++;
  if (*pos != 'f' && *pos != 'F') return -1;
  pos++;
  if (*pos != 'l' && *pos != 'L') return 0;
  return 1;
#endif
}

extern "C++" int processCmdline(int, char **, mofCompilerOptions &, ostream&);
int
processCmdLine(int argc, char **argv, mofCompilerOptions &cmdlinedata,
	       ostream &helpos = cerr) {
  getoopt cmdline;
  setCmdLineOpts(cmdline);
  cmdline.parse(argc, argv);
  switch (getType(argv[0])) {
  case 1: cmdlinedata.set_is_local();
    #ifdef PEGASUS_OS_OS400  
    	// check if we are in qsh, if we are NOT running in a qsh environment then
      // send and escape message,
      // if we ARE then call ycmServerIsActive without the quiet option
#pragma convert(37)
      if( getenv("SHLVL") == NULL ){  // native mode
	  if(ycmServerIsActive(YCMSERVER_ACTIVE, YCMCIMMOFL, 1)) {
	      // previous call's message was suppressed,
              // server is running, send escape message and return
	      ycmSend_Message_Escape(CPFDF81_RC, "01", "QYCMMOFL",4);
	      return CPFDF81_RC;
	  }
      }
      else{ // qsh mode
	  if(ycmServerIsActive(YCMSERVER_ACTIVE, YCMCIMMOFL)) {
	      // server is running, the previous call sent a
	      // diagnostic message, lets return
	      return CPFDF81_RC;
	  }
      }
#pragma convert(0) 
#endif 
    break;
  default: cmdlinedata.reset_is_local();
  }
  applyDefaults(cmdlinedata);
  if (cmdline.hasErrors()) {
    
    //  throw an exception and hande it in the caller
    //l10n
    //String msg = "Command line errors:\n";
    MessageLoaderParms parms("Compiler.cmdline.cimmof.CMDLINE_ERRORS",
    						 "Command line errors:\n");
    String msg = MessageLoader::getMessage(parms);						 
    cmdline.printErrors(msg);
    throw ArgumentErrorsException(msg);
  }
  for (unsigned int i = cmdline.first(); i < cmdline.last(); i++) {
    const Optarg &arg = cmdline[i];
    opttypes c = catagorize(arg);
    switch (c)
      {
      case HELPFLAG:  help(helpos);
	return(-1);
        break;
      case INCLUDEPATH:cmdlinedata.add_include_path(arg.optarg());
        break;
      case SUPPRESSFLAG: cmdlinedata.set_suppress_warnings();
        break;
      case NAMESPACE: cmdlinedata.set_namespacePath(arg.optarg());
        break;
      case REPOSITORYNAME:  cmdlinedata.set_repository_name(arg.optarg());
	break;
      case UPDATEFLAG:
        { 
          if (arg.optarg().size() == 1)
          {
              for (unsigned int i = 0; i < arg.optarg().size(); i++)
              {
                  if (arg.optarg()[i] == 'c')
                      cmdlinedata.set_update_class();
                  else
                  {
                  	//l10n
                      //throw ArgumentErrorsException(
                          //"Unknown value specified for option -u.");
                      MessageLoaderParms parms("Compiler.cmdline.cimmof.UNKNOWN_VALUE_OPTION_U",
					       "Unknown value specified for option -u.");
                      throw ArgumentErrorsException(parms);
                  }
              }
          }
          else
          {
          	//l10n
            //throw ArgumentErrorsException(
              //  "Too many values specified for option -u.");
            MessageLoaderParms parms("Compiler.cmdline.cimmof.TOO_MANY_VALUES_OPTION_U",
				     "Too many values specified for option -u.");
            throw ArgumentErrorsException(parms);
          }
        }
        break;
      case ALLOWFLAG:
        { 
          if ((arg.optarg().size() <= 2) && (arg.optarg().size() != 0))
          {
              for (unsigned int i = 0; i < arg.optarg().size(); i++)
              {
                 if (arg.optarg()[i] == 'E')
                     cmdlinedata.set_allow_experimental();
                 else
                 if (arg.optarg()[i] == 'V')
                     cmdlinedata.set_allow_version();
                 else
                  {
                  	//l10n
                      //throw ArgumentErrorsException(
                          //"Unknown value specified for option -a.");
                      MessageLoaderParms parms("Compiler.cmdline.cimmof.UNKNOWN_VALUE_OPTION_A",
					       "Unknown value specified for option -a.");
                      throw ArgumentErrorsException(parms);
                  }
              }
          }
          else
          {
          	//l10n
            //throw ArgumentErrorsException(
                //"Too many values specified for option -a.");
            MessageLoaderParms parms("Compiler.cmdline.cimmof.TOO_MANY_VALUES_OPTION_A",
				     "Too many values specified for option -a.");
            throw ArgumentErrorsException(parms);
          }
        }
        break;
#ifndef PEGASUS_OS_HPUX
      case SYNTAXFLAG: cmdlinedata.set_syntax_only();
		       cmdlinedata.set_operationType(
				compilerCommonDefs::DO_NOT_ADD_TO_REPOSITORY);
	break;
      case TRACEFLAG: 
	{
	  cmdlinedata.set_trace();
	  const String &s = arg.optarg();
	  if (s != "") {
	    // ATTN: P3 BB Mar 2001  No way to close the trace stream
	    // or to delete the ostream object.  It's OK for now because
	    // the program terminates when we're done with the stream.
#if defined(PEGASUS_OS_OS400)
	    ofstream *tracefile = new ofstream(s.getCStringUTF8(),PEGASUS_STD(_CCSID_T(1208)));
#else
	    ofstream *tracefile = new ofstream(s.getCStringUTF8());
#endif
	    if (tracefile && *tracefile)
	      cmdlinedata.set_traceos(*tracefile);
	  }
	}
	break;
      case XMLFLAG: cmdlinedata.set_xmloutput();
	break;
      case FILELIST: {
	int stat = process_filelist(arg.optarg(), cmdlinedata);
	// ATTN: P1 BB 2001 On Process filelist error should throw an exception
	if (stat != 1) {
	  return stat;
	}
	break;
      }
#endif
#ifdef PEGASUS_OS_OS400
      case QUIETFLAG: cmdlinedata.set_quiet();
	break;
#endif
      case FILESPEC: cmdlinedata.add_filespecs(arg.optarg());
	break;
      case OPTEND: return -1;  // shouldn't happen
	break;
      }
  }
  if (String::equal(cmdlinedata.get_repository_name(), String::EMPTY)) {
  	//l10n
    //throw CmdlineNoRepository(
          //"You must specify -R or set PEGASUS_HOME environment variable");
    MessageLoaderParms parms("Compiler.cmdline.cimmof.MUST_SPECIFY_R_OR_HOME",
    						 "You must specify -R or set PEGASUS_HOME environment variable");
    throw CmdlineNoRepository(parms);
  }

#ifdef PEGASUS_OS_OS400 
  // Force a mof to be specified on OS/400
  if (cmdlinedata.get_filespec_list().size() == 0) {
  	//l10n
    //throw ArgumentErrorsException(
          //"You must specify some MOF files to process.");
    MessageLoaderParms parms("Compiler.cmdline.cimmof.MUST_SPECIFY_MOF_FILES",
    						 "You must specify some MOF files to process.");
    throw ArgumentErrorsException(parms);
  }
#endif

  return 0;
}
