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

#include <fstream>
#include <cstdlib>
#include <iostream>
#include <Pegasus/Common/String.h>

PEGASUS_USING_STD;
PEGASUS_USING_PEGASUS;

#include "../mofCompilerOptions.h"
#include "cmdlineExceptions.h"
#include "cmdline.h"
#include "cmdlineExceptions.h"
#include <Pegasus/getoopt/getoopt.h>
#include <fstream>
#include <Pegasus/Common/String.h>
//#include <strstream>
//#include <sstream>

PEGASUS_USING_PEGASUS;


// COMPILER VERSION ++++++++++++++++++++++++++++++++++++++++++++++++++++
#define COMPILER_VERSION "1.0.10" /* as of June 19, 2001 */
// COMPILER_VERSION ++++++++++++++++++++++++++++++++++++++++++++++++++++
 

static ostream & 
help(ostream &os) {
  os << endl << "MofCompiler version " << COMPILER_VERSION << endl << endl;
  os << "Usage: cimmof -hEw -Ipath -Rrepository -ffile" << endl;
  os << "  -h, --help -- show this help." << endl;
  os << "  -E -- syntax check only." << endl;
  os << "  -w -- suppress warnings." << endl;
  os << "  -Rrepository -- specify the repository path." << endl;
  os << "  --CIMRepository=repository -- specify repository path." << endl;
  os << "  -Ipath -- specify an include path." << endl;
  os << "  -ffile -- specify file containing a list of MOFs to compile."
       << endl;
  os << " --file=file -- specify file containing list of MOFs." << endl;
  os << " -npath -- override the default CIMRepository namespace." << endl;
  os << " --namespace=path -- override default CIMRepository namespace." 
       << endl;
  os << " --xml -- output XML only, to stdout.  Do not update repository."
     << endl;
  os << " --trace or --trace=ttracefile -- trace to file (default to stdout)."
     << endl;
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
    {(char*)"f", FILELIST, false, getoopt::MUSTHAVE},
    {(char*)"filelist", FILELIST, true, getoopt::MUSTHAVE},
    {(char*)"n", NAMESPACE, false, getoopt::MUSTHAVE},
    {(char*)"namespace", NAMESPACE, true, getoopt::MUSTHAVE}, 
    {(char*)"I", INCLUDEPATH, false, getoopt::MUSTHAVE},
    {(char*)"Include", INCLUDEPATH, true, getoopt::MUSTHAVE},
    {(char*)"R", REPOSITORYNAME, false, getoopt::MUSTHAVE},
    {(char*)"CIMRepository", REPOSITORYNAME, true, getoopt::MUSTHAVE},
    {(char*)"E", SYNTAXFLAG, false, getoopt::NOARG}, 
    {(char*)"w", SUPPRESSFLAG, false, getoopt::NOARG},
    {(char*)"trace", TRACEFLAG, true, getoopt::OPTIONAL},
    {(char*)"xml", XMLFLAG, true, getoopt::NOARG},
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
      cmdline.addFlagspec(o.flag[0], o.needsvalue == getoopt::MUSTHAVE ?
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
  char *peghome = getenv(PEGASUS_HOME);
  if (peghome) {
    cmdlinedata.set_repository_name(peghome);
  } else {
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
}

extern "C++" int processCmdline(int, char **, mofCompilerOptions &, ostream&);
int
processCmdLine(int argc, char **argv, mofCompilerOptions &cmdlinedata,
	       ostream &helpos = cerr) {
  getoopt cmdline;
  setCmdLineOpts(cmdline);
  cmdline.parse(argc, argv);
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
      case REPOSITORYNAME:  cmdlinedata.set_repository_name(arg.optarg());
	break;
      case SYNTAXFLAG: cmdlinedata.set_syntax_only();
		       cmdlinedata.set_operationType(
				compilerCommonDefs::DO_NOT_ADD_TO_REPOSITORY);
	break;
      case SUPPRESSFLAG: cmdlinedata.set_suppress_warnings();
	break;
      case TRACEFLAG: 
	{
	  cmdlinedata.set_trace();
	  const String &s = arg.optarg();
	  if (s != "") {
	    // FIXME:  This leaves no way to close the trace stream
	    // or to delete the ostream object.  It's OK for now because
	    // the program terminates when we're done with the stream.
	    char* tmp = s.allocateCString();
	    ofstream *tracefile = new ofstream(tmp);
	    delete [] tmp;
	    if (tracefile && *tracefile)
	      cmdlinedata.set_traceos(*tracefile);
	  }
	}
	break;
      case XMLFLAG: cmdlinedata.set_xmloutput();
	break;
      case FILELIST: {
	int stat = process_filelist(arg.optarg(), cmdlinedata);
	// FIXME:  throw an exception
	if (stat != 1) {
	  return stat;
	}
	break;
      }
      case FILESPEC: cmdlinedata.add_filespecs(arg.optarg());
	break;
      case NAMESPACE: cmdlinedata.set_namespacePath(arg.optarg());
	break;
      case OPTEND: return -1;  // shouldn't happen
	break;
      }
  }
  if (String::equal(cmdlinedata.get_repository_name(), String::EMPTY)) {
    throw CmdlineNoRepository(
          "You must specify -R or set PEGASUS_HOME environment variable");
  }
  return 0;
}
