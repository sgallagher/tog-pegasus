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

#include <iostream>
#include "../mofCompilerOptions.h"
#include "cmdlineExceptions.h"
#include "../cimmofParser.h"
#include "../parserExceptions.h"
//#include <Pegasus/Common/Exception.h>

using namespace std;

// This is used by the parsing routines to control flow
// through include files
static mofCompilerOptions cmdline;


extern "C++" int processCmdLine(int, char **, mofCompilerOptions &, ostream &);

#define NAMESPACE_ROOT "root/cimv20"

int
main(int argc, char ** argv) {
  int ret = 0;
  try {
    ret = processCmdLine(argc, argv, cmdline, cerr);
  } catch (ArgumentErrorsException &e) {
    cerr << e.getMessage() << endl;
    cerr << "Compilation terminating." << endl;
    ret =  -2;
  }
  if (ret)
    return ret;

  const Array<String>& filespecs = cmdline.get_filespec_list();

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

  if (filespecs.getSize())    // user specified command line args
    for (unsigned int i = 0; i < filespecs.getSize(); i++) {
      if (p->setInputBufferFromName((const String &)filespecs[i]) == 0) {
	try {
	  ret = p->parse();
	} catch(ParserExceptions::ParserLexException &e) {
	  cerr << "Lexer error: " << e.getMessage() << endl;
	} catch(Exception &e) {
	  cerr << "Parsing error: " << e.getMessage() << endl;
	}
      } else {
        cerr << "Can't open file " << filespecs[i] << endl;
      }
    }
  else {
    try {
    int ret =  p->parse();
    } catch(Exception &e) {
	    cerr << "Parsing error: "  << e.getMessage() << endl;
    } catch(ParserExceptions::ParserLexException &e) {
      cerr << "Lexer error: " << e.getMessage() << endl;
    }
  }
  
  return ret;
}
