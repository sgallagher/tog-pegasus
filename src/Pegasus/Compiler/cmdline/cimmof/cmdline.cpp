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
#endif 


// COMPILER VERSION ++++++++++++++++++++++++++++++++++++++++++++++++++++
#define COMPILER_VERSION "1.1.00" /* as of March 10, 2002 */
// COMPILER_VERSION ++++++++++++++++++++++++++++++++++++++++++++++++++++
 
#define DEFAULT_SERVER_AND_PORT "localhost:5988"

ostream & 
help(ostream &os) {
  os << endl << "MOF Compiler version " << COMPILER_VERSION << endl << endl;
#ifdef PEGASUS_OS_HPUX
  os << "Usage: cimmof [-h] [-w] [-I path] [-n namespace] [file, ...]" << endl;
  os << "    -h           - show this help " << endl;
  os << "    -w           - suppress warnings " << endl;
  os << "    -I path      - specify an include path " << endl;
  os << "    -n namespace - override the default CIMRepository namespace "
     << endl;
#else
#ifndef PEGASUS_OS_OS400
  os << "Usage: cimmof [-h] [-E] [-w] [-R repository] [-I path] [-n namespace] [--xml] [--trace] -ffile" << endl;
  os << "       cimmof [-h] [-E] [-w] [-R repository] [-I path] [-n namespace] [--xml] [--trace] [mof_files...]" << endl;
#else
  os << "Usage: cimmof [-h] [-E] [-w] [-R repository] [-I path] [-n namespace] [--xml] [--trace] [-q] -ffile" << endl;
  os << "       cimmof [-h] [-E] [-w] [-R repository] [-I path] [-n namespace] [--xml] [--trace] [-q] mof_files..." << endl;
#endif
  os << "  -h, --help -- show this help." << endl;
  os << "  -E -- syntax check only." << endl;
  os << "  -w -- suppress warnings." << endl;
#ifdef PEGASUS_OS_OS400
  os << "  -q -- suppress all messages except command line usage errors." << endl;
#endif
  os << "  -Rrepository -- specify the repository path (cimmofl) or hostname:portnumber (cimmof)" << endl;
  os << "  --CIMRepository=repository -- specify repository path or hostname:portnumber." << endl;
  os << "  -Ipath -- specify an include path." << endl;
  os << "  -ffile -- specify file containing a list of MOFs to compile."
       << endl;
  os << " --file=file -- specify file containing list of MOFs." << endl;
  os << " -npath -- override the default CIMRepository namespace (root/cimv2)." << endl;
  os << " --namespace=path -- override default CIMRepository namespace (root/cimv2)." 
       << endl;
  os << " --xml -- output XML only, to stdout.  Do not update repository."
     << endl;
  os << " --trace or --trace=ttracefile -- trace to file (default to stdout)."
     << endl;
#endif
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
      cmdlinedata.set_repository_name(peghome);
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
    	if(ycmServerIsActive(YCMSERVER_ACTIVE, YCMCIMMOFL)) 
		// server is running, lets exit
		return -1; 
    #endif 
    break;
  default: cmdlinedata.reset_is_local();
  }
  applyDefaults(cmdlinedata);
  if (cmdline.hasErrors()) {
    String msg = "Command line errors:\n";
    //  throw an exception and hande it in the caller
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
	    ofstream *tracefile = new ofstream(s.getCString());
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
    throw CmdlineNoRepository(
          "You must specify -R or set PEGASUS_HOME environment variable");
  }

#ifdef PEGASUS_OS_OS400 
  // Force a mof to be specified on OS/400
  if (cmdlinedata.get_filespec_list().size() == 0) {
    throw ArgumentErrorsException(
          "You must specify some MOF files to process.");
  }
#endif

  return 0;
}
