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
// Header for a class to generate CIMValue objects from String values
//
//
//
// Defines the mofCompilerOptions class, which contains all you
// want to know about the command line that invoked the compiler,
// including
//             The include (-I) paths
//             The repository to talk to (-R)
//             Other compiler options
//                 syntax check only (-E)
//                 suppress warnings (-w)
//             The files to be compiled.
//
// FIXME:  This needs to be reworked so that the same options offered
// by the Microsoft compiler are supported.  They allow modes for
//      -- Marking as an error an attempt to create an existing object
//      -- Marking as an error an attempt to create an existing object
//         with different elements (i.e. overwriting)

#ifndef _MOFCOMPILEROPTIONS_H_
#define _MOFCOMPILEROPTIONS_H_

#include <Pegasus/Common/String.h>
#include <Pegasus/Common/Array.h>
#include <iostream>

PEGASUS_USING_STD;
PEGASUS_USING_PEGASUS;

class PEGASUS_COMPILER_LINKAGE mofCompilerOptions {
 private:
  Array<String> _include_paths;
  Array<String> _filespecs;
  String       _repository;
  String                _namespacePath;
  Boolean         _syntax_only;
  Boolean         _suppress_warnings;
  Boolean         _suppress_all_messages;
  Boolean         _trace;
  PEGASUS_STD(ostream)      *_traceos;
  PEGASUS_STD(ostream)      *_erroros;
  PEGASUS_STD(ostream)      *_warningos;
 public:
  mofCompilerOptions() : _repository(String::EMPTY), 
    _namespacePath(""),
    _syntax_only(false),
    _suppress_warnings(false),
    _suppress_all_messages(false),
    _trace(false),
    _traceos(0),
    _erroros(0),
    _warningos(0)
    {;}

  ~mofCompilerOptions() {;}

  void add_include_path(const String& path) { _include_paths.append(path); }
  const Array<String> &get_include_paths() const { return _include_paths; }
  void add_filespecs(const String &spec) { _filespecs.append(spec); }
  const Array<String>& get_include_path_list() const {return _include_paths; }
  const Array<String>& get_filespec_list() const { return _filespecs; }
  void  set_repository_name(const String &repository) { \
    _repository = repository; }
  const String &get_repository_name() const { return _repository; }
  void set_syntax_only() { _syntax_only = true; }
  void reset_syntax_only() { _syntax_only = false; }
  Boolean syntax_only() const { return _syntax_only; }
  void set_suppress_warnings() { _suppress_warnings = true; }
  void reset_suppress_warnings() { _suppress_warnings = false; }
  Boolean suppress_warnings() const { return _suppress_warnings; }
  void set_suppress_all_messages() { _suppress_all_messages = true; }
  void reset_suppress_all_messages() { _suppress_all_messages = false; }
  Boolean suppress_all_messages() const { return _suppress_all_messages; }
  void set_trace() { _trace = true; }
  void reset_trace() { _trace = false; }
  Boolean trace() const { return _trace; }
  void set_traceos(PEGASUS_STD(ostream) &os) { _traceos = &os; }
  void reset_traceos() { _traceos = 0; }
  ostream &traceos() const 
      { return _traceos ? (ostream&)*_traceos : (ostream&)cout; }
  void set_erroros(ostream &os) { _erroros = &os; }
  void reset_erroros() { _erroros = 0; }
  ostream &erroros() const 
      { return _erroros ? (ostream&)*_erroros : (ostream&)cerr; }
  void set_warningos(ostream &os) { _warningos = &os; }
  void reset_warningos() { _warningos = 0; }
  ostream &warningos() const 
      {return _warningos ? (ostream&)*_warningos : (ostream&)cerr;}
  void set_namespacePath(const String &path) { _namespacePath = path; }
  const String &get_namespacePath() const  { return _namespacePath; }
};

#endif
