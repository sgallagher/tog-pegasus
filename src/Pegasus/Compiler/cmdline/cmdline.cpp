//BEGIN_LICENSE
//
// Copyright (c) 2000 The Open Group, BMC Software, Tivoli Systems, IBM
//
// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation
// the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following conditions:
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
// THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.
//
//END_LICENSE
//BEGIN_HISTORY
//
// Author: Bob Blair (bblair@bmc.com)
//
//
//END_HISTORY
//
//
// process the command line, getting the repository location and
// the include path list and the other options
//

#include "../mofCompilerOptions.h"
#include "cmdline.h"
#include <Pegasus/getoopt/getoopt.h>
#include <fstream>
#include <iostream>

//extern "C" {
//extern int getopt(int argc, char **argv, const char *optstring);
//extern char *optarg;
//extern int optind;
//}

static void 
help() {
  cout << endl << "MofCompiler version 0.0" << endl << endl;
  cout << "Usage: cimmof -hEw -Ipath -Rrepository -ffile" << endl;
  cout << "  -h, --help -- show this help." << endl;
  cout << "  -E -- syntax check only." << endl;
  cout << "  -w -- suppress warnings." << endl;
  cout << "  -Rrepository -- specify the repository path." << endl;
  cout << "  --CIMRepository=repository -- specify repository path." << endl;
  cout << "  -Ipath -- specify an include path." << endl;
  cout << "  -ffile -- specify file containing a list of MOFs to compile."
       << endl;
  cout << " --file=file -- specify file containing list of MOFs." << endl;
  cout << " -npath -- override the default CIMRepository namespace." << endl;
  cout << " --namespace=path -- override default CIMRepository namespace." 
       << endl;
  cout << " -t -- trace." << endl;
}

// If the 'f' flag is encountered, it names a file that contains the
// names of files to be processed.  Open the file and stick them into
// the filelist.
static int
process_filelist(const string &filename, mofCompilerOptions &cmdlinedata)
{
  string line;

  ifstream ifs(filename.c_str());
  while (ifs) {
    getline(ifs, line);   
    cmdlinedata.add_filespecs(line);
  }
  return 0;
}

static struct optspec 
optspecs[] = {{"", FILESPEC, false, true},
{"h", HELPFLAG, false, false},
{"help", HELPFLAG, true, false},
{"f", FILELIST, false, true},
{"filelist", FILELIST, true, true},
{"n", NAMESPACE, false, true},
{"namespace", NAMESPACE, true, true}, 
{"I", INCLUDEPATH, false, true},
{"Include", INCLUDEPATH, true, true},
{"R", REPOSITORYNAME, false, true},
{"CIMRepository", REPOSITORYNAME, true, true},
{"E", SYNTAXFLAG, false, true}, 
{"w", SUPPRESSFLAG, false, true},
{"t", TRACEFLAG, false, false},
{"", OPTEND, false, false}};

static void
setCmdLineOpts(getoopt &cmdline) {
  for (unsigned int i = 0; ; i++) {
    const optspec &o = optspecs[i];
    if (o.catagory == OPTEND)
      break;
    if (o.flag == "")
      continue;
    if (o.islong)
      cmdline.addLongFlagspec(o.flag, o.needsvalue ? 
			      getoopt::MUSTHAVE : getoopt::NOARG);
    else
      cmdline.addFlagspec(o.flag[0], o.needsvalue ?
			  getoopt::MUSTHAVE : getoopt::NOARG);
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
  if (peghome)
    cmdlinedata.set_repository_name(peghome);
  cmdlinedata.reset_syntax_only();
  cmdlinedata.reset_suppress_warnings();
  cmdlinedata.reset_trace();
  cmdlinedata.add_include_path(".");
  cmdlinedata.set_namespacePath(ROOTCIMV20);
}

int
processCmdLine(int argc, char **argv, mofCompilerOptions &cmdlinedata) {
  getoopt cmdline;
  setCmdLineOpts(cmdline);
  cmdline.parse(argc, argv);
  applyDefaults(cmdlinedata);
  if (cmdline.hasErrors()) {
    // FIXME:  throw an exception and hande it in the caller
    cerr << "Command line errors:" << endl;
    cmdline.printErrors(cerr);
    return -1;
  }
  for (unsigned int i = cmdline.first(); i < cmdline.last(); i++) {
    const Optarg &arg = cmdline[i];
    opttypes c = catagorize(arg);
    switch (c)
      {
      case HELPFLAG:  help();
	return(-1);
        break;
      case INCLUDEPATH:cmdlinedata.add_include_path(arg.optarg());
        break;
      case REPOSITORYNAME:  cmdlinedata.set_repository_name(arg.optarg());
	break;
      case SYNTAXFLAG: cmdlinedata.set_syntax_only();
	break;
      case SUPPRESSFLAG: cmdlinedata.set_suppress_warnings();
	break;
      case TRACEFLAG: cmdlinedata.set_trace();
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
  if (cmdlinedata.get_repository_name() == String::EMPTY) {
    // FIXME
    cerr << "You must specify -R or set PEGASUS_HOME environment variable"
	    << endl;
    return -1;
  }
  return 0;
}
