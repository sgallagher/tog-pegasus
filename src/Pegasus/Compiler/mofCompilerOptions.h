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
// $Log: mofCompilerOptions.h,v $
// Revision 1.2  2001/03/04 22:18:00  bob
// Cleanup, support for reference, message moving, start of instance support
//
// Revision 1.1  2001/02/16 23:59:09  bob
// Initial checkin
//
//
//
//END_HISTORY
//
// Header for a class to generate CIMValue objects from string values
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
#include <string>
#include <vector>
#include <iostream>

using namespace std;
using namespace Pegasus;

class PEGASUS_COMPILER_LINKAGE mofCompilerOptions {
 private:
  vector<string> _include_paths;
  vector<string> _filespecs;
  Pegasus::String       _repository;
  string                _namespacePath;
  bool         _syntax_only;
  bool         _suppress_warnings;
  bool         _suppress_all_messages;
  bool         _trace;
  std::ostream      *_traceos;
  std::ostream      *_erroros;
  std::ostream      *_warningos;
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

  void add_include_path(const string& path) { _include_paths.push_back(path); }
  const vector<string> &get_include_paths() const { return _include_paths; }
  void add_filespecs(const string &spec) { _filespecs.push_back(spec); }
  const vector<string>& get_include_path_list() const {return _include_paths; }
  const vector<string>& get_filespec_list() const { return _filespecs; }
  void  set_repository_name(const string &repository) { \
    _repository = repository.c_str(); }
  const Pegasus::String &get_repository_name() const { return _repository; }
  void set_syntax_only() { _syntax_only = true; }
  void reset_syntax_only() { _syntax_only = false; }
  bool syntax_only() const { return _syntax_only; }
  void set_suppress_warnings() { _suppress_warnings = true; }
  void reset_suppress_warnings() { _suppress_warnings = false; }
  bool suppress_warnings() const { return _suppress_warnings; }
  void set_suppress_all_messages() { _suppress_all_messages = true; }
  void reset_suppress_all_messages() { _suppress_all_messages = false; }
  bool suppress_all_messages() const { return _suppress_all_messages; }
  void set_trace() { _trace = true; }
  void reset_trace() { _trace = false; }
  bool trace() const { return _trace; }
  void set_traceos(std::ostream &os) { _traceos = &os; }
  void reset_traceos() { _traceos = 0; }
  std::ostream &traceos() const { return _traceos ? *_traceos : std::cout; }
  void set_erroros(std::ostream &os) { _erroros = &os; }
  void reset_erroros() { _erroros = 0; }
  std::ostream &erroros() const { return _erroros ? *_erroros : std::cerr; }
  void set_warningos(std::ostream &os) { _warningos = &os; }
  void reset_warningos() { _warningos = 0; }
  std::ostream &warningos() const {return _warningos ? *_warningos : \
				                       std::cerr;}
  void set_namespacePath(const string &path) { _namespacePath \
						 = path.c_str(); }
  const string &get_namespacePath() const  { return _namespacePath; }
};

#endif
