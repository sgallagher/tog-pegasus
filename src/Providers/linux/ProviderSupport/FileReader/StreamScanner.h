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
//  A class to analyze the contents of a FILE*, returning lines which match
//  particular regular expressions.  This is an abstract base class, and it
//  has no mechanism for setting the FILE*.
//

#ifndef Pegasus_StreamScanner_h
#define Pegasus_StreamScanner_h

#include <Pegasus/Common/String.h>
#include <unistd.h>
#include <dirent.h>
#include <stdio.h>
#include <regex.h>

#include <vector>

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/String.h>
#include <Pegasus/Common/Exception.h>


PEGASUS_USING_STD;
PEGASUS_USING_PEGASUS;

PEGASUS_NAMESPACE_BEGIN


#define MAX_REG_MATCHES 20


/** An abstract base class which provides the ability to read a FILE* text
 *  stream and locate lines within the stream which match any of a set of
 *  regular expressions.  Optionally loads a vector of String types with
 *  substring matches from the regular expression. */
class StreamScanner {
  
 public:
  StreamScanner(void);
  virtual ~StreamScanner(void);


  /** Sets the POSIX extended regular expressions to be used on the stream
   *  when it is read.  Lines which match any regexp in the list are
   *  returned in the order in which they are encountered in the stream,
   *  once for every call to get_next_matching_line().  The last element in
   *  the "regexps" array must be a NULL pointer.  It is safe to change the
   *  regular expression list in the middle of parsing a stream, but this
   *  will not reset the stream position to the beginning of the file.
   *  Throws an exception if problems occur compiling the regular
   *  expressions. */
  void SetSearchRegexps(char const * const *regexps);
  
  /** Scans the stream.  Must not be called before SetSearchRegexps() is
   *  called.  When GetNextMatchingLine() is called, it scans the stream
   *  for the next line which matches a regular expression in the search
   *  set.  The stream position in the file moves forward every time
   *  get_next_matching_line() is called.  Each line is checked against all
   *  regexps, before the next line is read.  When a match is found,
   *  reading of the stream suspends, the matching line is inserted into
   *  "line", and the method returns 0.  The method returns -1 if it
   *  reaches the end of the stream without finding a match.  If "index" is
   *  non-NULL, returns the index of the regular expression array passed to
   *  SetSearchRegexps() which matched.  Note that if a line matches more
   *  than one regular expression, "index" will hold the value of the first
   *  regular expression in the search set which matched.  The end of line
   *  character, '\n', is removed *before* the regular expression is
   *  compared.  The contents of a non-NULL "index" are always destroyed,
   *  even if no match is found. */
  int GetNextMatchingLine(String &line, int *index = NULL);

  /** This is a much more powerful interface for GetNextMatchingLine.  In
   *  "matches", it returns String objects holding each of the regular
   *  expression \<NUM> subStrings, up to a maximum of MAX_REG_MATCHES.
   *  "index" must be supplied, but may be NULL.  Substrings are set in a
   *  regular expression by enclosing the substring to match within
   *  parentheses.  The substrings are numbered from the left of the
   *  regular expression, starting at 1.  The 0 index holds the text which
   *  matched the entire regular expression.  Both the contents of "index"
   *  and the "matches" vector are changed, regardless of whether or not a
   *  match is obtained. */
  int GetNextMatchingLine(String &line, int *index, vector<String> &matches);

 private:
  /// The set of regular expressions to compare against each input line
  vector<regex_t> line_regexs;

 protected:
  /** The data stream.  Note that StreamScanner objects provide no methods
   *  for setting "stream", that is the responsibility of classes derived
   *  from this class. */
  FILE *stream;

  /** Compiled POSIX regular expressions must be freed.  This is the
   *  destructor for vectors of regexs. */
  void CleanupRegexVector(vector<regex_t> &rlist);

  /** Returns TRUE if the string in "text" matches any of the regular
   *  expressions in "rlist".  If "index" is non-NULL and a match is found,
   *  sets the contents to the first regular expression which matched the
   *  line of input.  The contents of "index" are never preserved. */
  bool TextMatchesRegex(char const *text, vector<regex_t> &rlist,
		       int *index = NULL) const;

  /** Another method for locating regular expressions.  Acts just like the
   *  other TextMatchesRegex() method, but additionally loads "matches"
   *  with the substrings from the matching regular expression, if the text
   *  matches. */
  bool TextMatchesRegex(char const *text, vector<regex_t> &rlist,
		       int *index, vector<String> &matches) const;

  /** Pure virtual method.  Derived classes must define how they will clean
   *  up after the stream ends. */
  virtual void HandleEndOfStream(void) = 0;
};


/** An exception thrown when a regex fails to compile. */
class BadRegularExpression : public Exception {
 public:
  static const char MSG[];
  
  BadRegularExpression(const String &exp) : Exception(String(MSG) + "\"" + exp + "\"") {}
};

/** An exception thrown when a NULL pointer was encountered where that is
 *  not allowed. */
class InvalidNullPtr : public Exception {
 public:
  static const char MSG[];
  
  InvalidNullPtr(void) : Exception(String(MSG)) { }
};

/** An exception thrown when required arguments are not provided. */
class MissingRequiredArgument : public Exception {
 public:
  static const char MSG[];
  
  MissingRequiredArgument(void) : Exception(String(MSG)) { }
};

/** An exception thrown when an argument is out of range. */
class BadArgumentValue : public Exception {
 public:
  static const char MSG[];
  
  BadArgumentValue(const String &arg) : Exception(String(MSG) + "\"" + arg + "\"") { }
};


PEGASUS_NAMESPACE_END

#endif  /* Pegasus_StreamScanner_h  */
