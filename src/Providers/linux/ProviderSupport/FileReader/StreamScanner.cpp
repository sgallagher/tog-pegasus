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
//
// Implementation of the FileReader class.
//
//


#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>


#include "FileReader.h"

PEGASUS_USING_STD;
PEGASUS_USING_PEGASUS;

PEGASUS_NAMESPACE_BEGIN


#define N_IN_ARRAY(x) ((int)( sizeof(x) / sizeof(x[0])))


const char BadRegularExpression::MSG[] = "Invalid regular expression: ";
const char InvalidNullPtr::MSG[] = "Null pointer forbidden here.";
const char MissingRequiredArgument::MSG[] = "Missing required argument(s)";
const char BadArgumentValue::MSG[] = "Invalid parameter ";


StreamScanner::StreamScanner(void)
{
  stream = NULL;
}


StreamScanner::~StreamScanner(void)
{
  CleanupRegexVector(line_regexs);
}


void StreamScanner::SetSearchRegexps(char const * const *regexps)
{
  regex_t new_entry;
  int i;

  if (regexps == NULL)
    throw InvalidNullPtr();

  CleanupRegexVector(line_regexs);

  i = 0;
  while (regexps[i] != NULL) {
    if (regcomp(&new_entry, regexps[i], REG_EXTENDED)) {
      CleanupRegexVector(line_regexs);
      throw BadRegularExpression(regexps[i]);
    }
    
    line_regexs.push_back(new_entry);
    i++;
  }

  if (line_regexs.empty())
    throw MissingRequiredArgument();

  return;
}


/* Read this much from the stream at a time.  This does not limit the
 * maximum parsed length of a line read from the stream, if a line is
 * longer than this, it will be safely handled.  */
#define READBUFFER_LEN 128


int StreamScanner::GetNextMatchingLine(String &line, int *index) 
{
  vector<String> junk_matches;

  return GetNextMatchingLine(line, index, junk_matches);
}


int StreamScanner::GetNextMatchingLine(String &line, int *index,
				       vector<String> &matches)
{
  char readbuffer[READBUFFER_LEN];
  String current_line;

  if (stream == NULL)
    return -1;
    
  while (true) {

    current_line = "";

    /* Read the stream, appending the read text to "current_line", until we
     * get to the end of the line. */
    do {
      readbuffer[READBUFFER_LEN - 2] = 0;
      if (fgets(readbuffer, READBUFFER_LEN, stream) == NULL) {
	HandleEndOfStream();
	return -1;
      }

      current_line.append(readbuffer);
    } while (readbuffer[READBUFFER_LEN - 2] != 0 &&
	     readbuffer[READBUFFER_LEN - 2] != '\n');

    /* Strip the trailing newline. */
    if (current_line.size() > 0 &&
	current_line[current_line.size() - 1] == '\n')
      current_line.remove(current_line.size() - 1);

    /* Check for a match */
    if (TextMatchesRegex(current_line.getCString(), line_regexs, index, matches)) {
      line = current_line;
      return 0;
    }
  }
}


bool StreamScanner::TextMatchesRegex(char const *text, vector<regex_t> &rlist,
				    int *index) const
{
  vector<String> junk_matches;

  return TextMatchesRegex(text, rlist, index, junk_matches);
}


bool StreamScanner::TextMatchesRegex(char const *text, vector<regex_t> &rlist,
				     int *index, vector<String> &matches) const
{
  int i, j;
  int junkvar;
  regmatch_t matchlist[MAX_REG_MATCHES + 1];
  String hold_a_match;
  char const *mptr;
  int mlen;

  if (index == NULL)
    index = &junkvar;

  /* Spin through the regular expressions for this line of text.  On a
   * match, load the "matches" vector and return true. */
  for (*index = 0; *index < (int) rlist.size(); (*index)++)
    if (regexec(&rlist[*index], text, MAX_REG_MATCHES+1, matchlist, 0) == 0) {
      matches.clear();
      for (i = 0; i <= MAX_REG_MATCHES && matchlist[i].rm_so != -1; i++) {
	mlen = matchlist[i].rm_eo - matchlist[i].rm_so;
	hold_a_match = "";
	mptr = text + matchlist[i].rm_so;
	for (j = 0; j < mlen && *mptr != 0; j++, mptr++)
	  hold_a_match.append(*mptr);

	matches.push_back(hold_a_match);
      }
      return true;
    }

  return false;
}


void StreamScanner::CleanupRegexVector(vector<regex_t> &rlist)
{
  regex_t t;
  
  for (unsigned int i = 0; i < rlist.size(); i++)
  {
    t = rlist[i];
    regfree(&t);
  }
  
  rlist.clear();
}

PEGASUS_NAMESPACE_END
