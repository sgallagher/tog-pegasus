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
// Author: Christopher Neufeld <neufeld@linuxcare.com>
//         David Kennedy       <dkennedy@linuxcare.com>
//
// Modified By: David Kennedy       <dkennedy@linuxcare.com>
//              Christopher Neufeld <neufeld@linuxcare.com>
//              Al Stone            <ahs3@fc.hp.com>
//
//%////////////////////////////////////////////////////////////////////////////
//
// A class used to extract information from /proc files (or whatever their
// equivalent might be in another OS).

#ifndef Pegasus_FileReader_h
#define Pegasus_FileReader_h

#include <unistd.h>
#include <dirent.h>
#include <stdio.h>
#include <regex.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <vector>

#include "StreamScanner.h"

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/String.h>
#include <Pegasus/Common/System.h>
#include <Pegasus/Common/Exception.h>


PEGASUS_USING_STD;
PEGASUS_USING_PEGASUS;

PEGASUS_NAMESPACE_BEGIN


/** We do a recursive descent of the directory tree, with the ability to
 *  return to the caller at any moment, and resume where we left off.  So,
 *  this recursion is done by manual maintenance of directory stack frames,
 *  rather than actually recursing the function.  This structure holds the
 *  stack frames. */
struct dir_stack_frame {
  DIR *dir_handle;
  String cwd_name;
};


/** A type of stream scanner which allows the user to search a directory
 *  tree for a filename which matches a given regular expression, returns
 *  matching file names and opens them for reading, then allows the user to
 *  scan the contents of those files, opened as text streams, for
 *  occurences of certain regular expressions.  In order to avoid
 *  confusion, it is important to note that there are two regular
 *  expression searches involved in using this class.  The first is a
 *  filename regular expression search, for locating files within a
 *  directory tree.  The second is a text regular expression search,
 *  looking for matching lines within files which were located in the first
 *  type of search.  The latter regular expression searches are handled by
 *  the StreamScanner parent class, the former are handled by this class
 *  itself. */
class FileReader : public StreamScanner {
  
 public:
  /** Constructor.  The optional argument sets the directory under which
   *  filename searches will occur.  By default, this is /proc.  */
  FileReader(char const *basedir = "/proc");
  
  ~FileReader();

  /** Searches for filenames (relative to base_directory) which match the
   *  POSIX extended regular expressions passed, in order.  Returns a
   *  pointer to the name of the first file found, or a NULL pointer if
   *  there are no files remaining in the tree which match any of the
   *  regular expressions.  Must be passed an array of pointers to const
   *  chars, and the last pointer must be NULL.  If index is non-NULL, it
   *  is the index into the regular expression array of the entry which
   *  matched the located file.  The search through the tree is unordered
   *  (ordering is actually determined by the order of occurence of dirents
   *  in a diropen()-ed stream), but depth-first (i.e. whenever a directory
   *  is encountered, it is immediately descended).  Note that the search
   *  patterns will only be compared against a regular file, not against a
   *  directory or a symbolic link.  If "filename_patterns" is NULL, the
   *  pattern list used in the previous call is re-used.  The first time
   *  this method is called, this array must not be NULL.  Because the
   *  returned files are not ordered in any useful fashion, there is
   *  probably no reason ever to change the search patterns after they are
   *  set by the first call. */
  char const *SearchFileRegexList(char const * const *filename_patterns = NULL, int *index = NULL);

  /** This method is exactly like the other overloaded method of the same
   *  name, but also returns substring matches within the filename search
   *  if that is deemed useful. */
  char const *SearchFileRegexList(char const * const *filename_patterns, int *index, vector<String> &matches);

 private:
  /// The directory under which we will do our search for matching filenames
  String base_directory;

  /// The name of the file which was returned by the last filename search
  String file_to_parse;

  /// The regular C string representation of "file_to_parse"
  char *cstring_file_to_parse;

  /// The regular expressions used to search for a file in the directory
  vector<regex_t> file_regexs;
  
  /// The stack of directory frames, marking where we are and where we've been
  vector<struct dir_stack_frame> dir_frames;

 private:

  /** Traverses directories from the current point in the search, looking
   *  for the next filename which matches the regular expression list in
   *  file_regexs.  Returns 0 if it locates a file, and sets
   *  "file_to_parse" and "cstring_file_to_parse".  Returns -1 if no more
   *  files are found which match the search patterns.  Assigns "index" and
   *  "matches" in the usual way. */
  int FindNextMatchingFile(int *index, vector<String> &matches);
  

 protected:
  /** This method is called whenever we reach the end of the file currently
   *  being scanned for text matches. */
  virtual void HandleEndOfStream(void);

};

/** Exception thrown when we are unable even to enter the directory
 *  specified for the search. */
class SearchDirectoryMissing : public Exception {
 public:
  static const char MSG[];
  
  SearchDirectoryMissing(const String &arg) : Exception(String(MSG) + "\"" + arg + "\"") { }
};


PEGASUS_NAMESPACE_END

#endif  /* Pegasus_FileReader_h  */
