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
// Modified By:
//
//%/////////////////////////////////////////////////////////////////////////////


//
// Header for a class to generate CIMValue objects from String values
//
//
//
// This is a generic parser class from which controllers for particular
// yacc parsers can be derived.  It keeps enough state information that
// you should be able to get by without a reentrant parser.  You should
// compile both parser and lexer with a C++ compiler, although there
// is no need to generate a C++ lexer.
//
// The include file and compile-from-String techniques used here are
// supported only by bison and flex.
//

#ifndef _PARSER_H_
#define _PARSER_H_

#include <Pegasus/Common/Config.h>
#include <cstdio>
#include <Pegasus/Common/String.h>
#include <Pegasus/Common/Stack.h>
#include <Pegasus/Compiler/Linkage.h>

PEGASUS_USING_STD;
PEGASUS_USING_PEGASUS;

#define CIMMOF_CONSTANT_VALUE  1
#define CIMMOF_ARRAY_VALUE     2
#define CIMMOF_REFERENCE_VALUE 3
#define CIMMOF_NULL_VALUE      4

typedef struct typedInitializerValue {
    Uint16 type;
    const String *value;
    } TYPED_INITIALIZER_VALUE;

struct bufstate {
	void *buffer_state; // the YY_BUFFER_STATE of the stacked context
	String filename;    // the name of the file open in the stacked context
	int    lineno;      // the line number of the file
};

class PEGASUS_COMPILER_LINKAGE  parser {
 private:
  unsigned int _buffer_size;   // the value of the YY_BUFFER_SIZE macro
  Stack<bufstate*> _include_stack;  // a stack of YY_BUFFER_STATEs
  String _current_filename; // name of the file being parsed
  unsigned int _lineno;     // current line number in the file 
 protected:
  void push_statebuff(bufstate *statebuff) { _include_stack.push(statebuff); }
  bufstate *pop_statebuff();
 public:

  // Constructor, destructor
  parser() : _buffer_size(16384), _lineno(0) {;}
  virtual ~parser() {;}

  virtual int parse() = 0;    // call the parser main yy_parse()
  virtual int wrap();         // handle the end of the current stream

  int setInputBufferFromName(const String &filename); // start parsing this file
  virtual int setInputBuffer(const FILE *f, 
                       Boolean closeCurrent) = 0;  // start parsing this handle
  //  int setInputBuffer(const char *buf);   // start parsing this String
  virtual int setInputBuffer(void *buffstate,
                       Boolean closeCurrent) = 0; // start parsing this buffer

  // given a file stream, treat it as an include file
  virtual int enterInlineInclude(const FILE *f) = 0;
  virtual int wrapCurrentBuffer() = 0;

  unsigned int get_buffer_size() { return _buffer_size; }
  void set_buffer_size(unsigned int siz) { _buffer_size = siz; }

  // We keep track of the filename associated with the current input
  // buffer so we can report on it.
  void set_current_filename(const String &filename)
  	{ _current_filename = filename; }
  const String &get_current_filename() const { return _current_filename; }

  // Ditto the line number
  void set_lineno(int n) { _lineno = n; }
  void increment_lineno() { ++_lineno; }
  unsigned int get_lineno() const { return _lineno; }

  // This is the main entry point for parser error logging
  virtual void log_parse_error(char *token, const char *errmsg) const;
};
#endif
