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

const char SearchDirectoryMissing::MSG[] = "Base directory for search unreadable: ";

#define N_IN_ARRAY(x) ((int)( sizeof(x) / sizeof(x[0])))

/* A list of files which must never be read.  For instance, /proc/kmsg
 * blocks readers forever, even those which open it with O_NONBLOCK.  We
 * don't want our methods trying to read such a tar baby. */
static char const *readfile_blacklist[] = { "/proc/kmsg", };


FileReader::FileReader(char const *basedir)
{
  if (basedir == NULL)
    throw InvalidNullPtr();
  if (basedir[0] != '/')   // Absolute pathnames only.
    throw BadArgumentValue(basedir);

  base_directory = basedir;
  file_to_parse = "";
  cstring_file_to_parse = NULL;
}


FileReader::~FileReader(void)
{
  if (stream != NULL)
    fclose(stream);

  {
    vector<struct dir_stack_frame>::reverse_iterator i = dir_frames.rbegin();
    while (i != dir_frames.rend()) {
      closedir(i->dir_handle);
      i++;
    }
  }

  CleanupRegexVector(file_regexs);

  delete cstring_file_to_parse;
}

char const *FileReader::SearchFileRegexList(char const * const *filename_patterns, int *index) 
{
  vector<String> junk_matches;

  return SearchFileRegexList(filename_patterns, index, junk_matches);
}


char const *FileReader::SearchFileRegexList(char const * const *filename_patterns, int *index, vector<String> &matches)
{
  regex_t new_entry;
  int i;

  if (stream != NULL)
    fclose(stream);

  stream = NULL;
  
  if (filename_patterns != NULL) {
    CleanupRegexVector(file_regexs);

    i = 0;
    while (filename_patterns[i] != NULL) {
      if (regcomp(&new_entry, filename_patterns[i], REG_EXTENDED)) {
	CleanupRegexVector(file_regexs);
	throw BadRegularExpression(filename_patterns[i]);
      }
      file_regexs.push_back(new_entry);
      i++;
    }
    if (file_regexs.empty())
      throw MissingRequiredArgument(); // need at least one regular expression
  }

  if (FindNextMatchingFile(index, matches) != -1) {

    delete cstring_file_to_parse;

    cstring_file_to_parse = strdup(file_to_parse.getCString());

    if ((stream = (FILE *) fopen(cstring_file_to_parse, "r")) == NULL)
      return NULL;

    return cstring_file_to_parse;

  } else {
    return NULL;
  }
}


// Searches for the next matching filename.  Loads the name into
// "file_to_parse" if it is found, and returns 0, otherwise returns -1 and
// makes that string empty.
int FileReader::FindNextMatchingFile(int *index, vector<String> &matches)
{
  struct dir_stack_frame *working_frame, new_frame;
  struct dirent entry, *result;
  String filename_to_test;
  bool found;
  struct stat file_stat;
  CString cstring_test;

  file_to_parse = "";

  if (dir_frames.empty()) {  // We have no active frames.  Start one.
    new_frame.cwd_name = base_directory;
    if ((new_frame.dir_handle = (DIR *) opendir(new_frame.cwd_name.getCString())) == NULL)
    {
      throw SearchDirectoryMissing(new_frame.cwd_name);
    }

    dir_frames.push_back(new_frame);
  }
  
  working_frame = &dir_frames.back();

  found = false;

  do {
    if (readdir_r(working_frame->dir_handle, &entry, &result) != 0 ||  //error
	result == NULL) {   // end of directory reached.

      //  Pop the frame
      closedir(working_frame->dir_handle);
      dir_frames.pop_back();

      if (dir_frames.empty())   // we've completed our scan
	return -1;

      working_frame = &dir_frames.back();
      continue;
    }

    // If we get here, we've found a valid direntry.
    filename_to_test = working_frame->cwd_name + "/" + entry.d_name;

    if (lstat(filename_to_test.getCString(), &file_stat) != 0)
    {
      continue;   // failed to locate file.  Give up and go on.
    }

    if (S_ISDIR(file_stat.st_mode) &&
	strcmp(entry.d_name, ".") != 0 &&
	strcmp(entry.d_name, "..") != 0)  {   // A non-trivial directory.

      //  Descend the directory.  Push a new frame
      new_frame.cwd_name = filename_to_test;
      if ((new_frame.dir_handle = (DIR *) opendir(new_frame.cwd_name.getCString())) == NULL)
      {
	continue;   // Failed to open the directory.  Don't push the frame.
      }

      dir_frames.push_back(new_frame);

      working_frame = &dir_frames.back();
      continue;
    }

    // We're interested only in regular files.
    if (!S_ISREG(file_stat.st_mode))
      continue;

    cstring_test = filename_to_test.getCString();
    if (TextMatchesRegex(cstring_test, file_regexs, index, matches)) {
      
      int i;

      for (i = 0; i < N_IN_ARRAY(readfile_blacklist); i++)
	if (strcmp(cstring_test, readfile_blacklist[i]) == 0) {
	  fprintf(stderr, "WARNING:  file regular expression matches\n");
	  fprintf(stderr, "blacklisted file %s\n", readfile_blacklist[i]);
	  break;
	}

      if (i < N_IN_ARRAY(readfile_blacklist)) {  // hit a forbidden file
	continue;
      }

      file_to_parse = filename_to_test;
      found = true;
    }

  } while (!found);

  return 0;
}

void FileReader::HandleEndOfStream(void)
{
  fclose(stream);
  stream = NULL;
  file_to_parse = "";
  delete cstring_file_to_parse;
  cstring_file_to_parse = NULL;
}


PEGASUS_NAMESPACE_END
