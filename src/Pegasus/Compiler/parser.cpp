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
//              Sean Keenan, Hewlett-Packard Company (sean.keenan@hp.com)
//
//%/////////////////////////////////////////////////////////////////////////////


//
// implementation of valueFactory 
//
//
//
// implementation of those methods  of class parser which are not pure
// virtual
//

#include "parser.h"
//#include <sstream>
#include "parserExceptions.h"

//---------------------------------------------------------------------
// Take a YY_BUFFERSTATE off the stack of saved contexts
//---------------------------------------------------------------------
bufstate *
parser::pop_statebuff() {
    bufstate *v = 0;
    if (!_include_stack.isEmpty()) {
      v = _include_stack.top();
      _include_stack.pop();
    }
    return v;
}

//-------------------------------------------------------------------
// Create a flex input buffer from a String containing the file name
//-------------------------------------------------------------------
int 
parser::setInputBufferFromName(const String &filename) {
#if defined PEGASUS_PLATFORM_OS400_ISERIES_IBM || defined (PEGASUS_OS_VMS)
  // 't' not supported on OS/400
  FILE *f = fopen(filename.getCString(),"r");
#else
  FILE *f = fopen(filename.getCString(),"rt");
#endif
  if (f) {
    set_current_filename(filename);
    set_lineno(1);
    return setInputBuffer(f, false);
  } else {
    return 1;
  }
}

//-----------------------------------------------------------------
// Handle the end of an input buffer.  Either there is saved context
// or there isn't.  If there is, restore the saved particulars
// of that context so we can keep on parsing
//-----------------------------------------------------------------
int
parser::wrap() {
  bufstate *v = pop_statebuff();
  if (v) {
    setInputBuffer(v->buffer_state, true);
    set_current_filename(v->filename);
    set_lineno(v->lineno);
    delete v;
    return 0;  // more data available
  } else {
    return 1;  // end of it all
  }
}

#ifdef PEGASUS_HAVE_NAMESPACES
using namespace ParserExceptions;
#endif /* PEGASUS_HAVE_NAMESPACES */

//----------------------------------------------------------------
// ATTN: P2 BB 2001 Log where an error occured.  This is lame, so it needs work
// 
//----------------------------------------------------------------
void
parser::log_parse_error(char *token, const char *errmsg) const {
  char buf[40];
  sprintf(buf, "%d", _lineno);
  String s = _current_filename + ":" + buf + ": " + errmsg + " before `" 
    + token + "'\n";
  throw ParserLexException(s);
}
