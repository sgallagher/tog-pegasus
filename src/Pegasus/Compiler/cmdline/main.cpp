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
// A main for the cimmof_parser.  It can be embedded elsewhere, too. 

// #include "cimmof_tab.h"
#include <iostream>
#include "../mofCompilerOptions.h"
#include "../cimmofParser.h"
#include <Pegasus/Common/Exception.h>

using namespace std;

// This global is used by the parsing routines to control flow
// through include files
static mofCompilerOptions cmdline;
//cimmofParser g_cimmofParser(cmdline);

// Note:  moved to cimmof.y
//extern "C" {
//int 
//cimmof_wrap() {
//  int ret =  g_cimmofParser.wrapCurrentBuffer();
//  return ret;
//}
//	   }

// int cimmof_parse();

extern int processCmdLine(int, char **, mofCompilerOptions &);

#define NAMESPACE_ROOT "cim2m2/root"

int
main(int argc, char ** argv) {
  int ret = 0;
  if (int stat = processCmdLine(argc, argv, cmdline))
    return stat;

  const vector<string>& filespecs = cmdline.get_filespec_list();

  // For most options, a real repository is required.  If we can't
  // create one and we need to, bail. 
  cimmofParser *p = cimmofParser::Instance(); 
  p->setCompilerOptions(&cmdline);
  if ( p->setRepository() ) {
    p->setDefaultNamespacePath(NAMESPACE_ROOT);
  } else {
    // FIXME:  We may need to log an error here.
    return ret;
  }

  if (filespecs.size())    // user specified command line args
    for (unsigned int i = 0; i < filespecs.size(); i++) {
      if (p->setInputBufferFromName((const string &)filespecs[i]) == 0) {
	try {
	  ret = p->parse();
	} catch(Exception e) {
		cerr << p->get_current_filename() << "(";
		cerr << p->get_lineno() << "): ";
		cerr << "Parsing error: " << e.getMessage() << endl;
	}
      } else {
        cerr << "Can't open file " << (filespecs[i]).c_str()  << endl;
      }
    }
  else {
    try {
    int ret =  p->parse();
    }
    catch(Exception e) {
	    cerr << p->get_current_filename() << "(";
	    cerr << p->get_lineno() << "): ";
	    cerr << "Parsing error: "  << e.getMessage() << endl;
    }
  }
  cout <<  "return value" << ret << endl;
  return ret;
}
