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
// Test for the StreamScanner derived classes, FileReader and ExecReader.
//
//  We want to retrieve the processor number and cpu MHz for all CPUs on 
//  the box.  Also, we pull /proc/dma, just so we can demonstrate looping 
//  over multiple files.


#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/param.h>


#include "FileReader.h"
#include "ExecScanner.h"

#define N_IN_ARRAY(x) ((int) (sizeof(x) / sizeof(x[0])))

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/String.h>
#include <Pegasus/Common/Exception.h>


PEGASUS_USING_PEGASUS;

class PEGASUS_COMMON_LINKAGE ScannerTestFailed : public Exception {
 public:
  static const char MSG[];
  
  ScannerTestFailed(String const &s) : Exception(String(MSG) + s) { }
};


static char *tmpdirname = NULL;

const char ScannerTestFailed::MSG[] = "Scanner test failed: ";

enum type_of_match {
  MATCH_KEY, MATCH_SECRET
};


static struct {
  const char* regex;
  enum type_of_match match_type;
} lookup_info[] = {
  {
    "Key #([0-9]+)$",
    MATCH_KEY
  },
  {
    "You found a secret!$",
    MATCH_SECRET
  },
};


static struct {
  char const *relpathname;
  char const *contents;
} file_contents[] = {
  { "testfile1",
    "Contents of test file #1\n"
    "Second line of this file.  Key #1\n"
  },
  { "dir1/testfile2",
    "Contents of test file #2\n"
    "Second line of this file.  Key #2\n"
  },
  { "dir1/dir2/testfile3",
    "Contents of test file #3\n"
    "Second line of this file.  Key #3\n"
    "Third line of this file. You found a secret!\n"
  },
};

static char const *trial_filename_patterns[] = { 
  "testfile1$",
  "dir1/testfile2$",
  "dir1/dir2/testfile3$",
};



#define MAXTRIES 10

static void build_tmp_tree(void)
{
  int giveup_counter;
  int i;
  struct stat statbuff;
  char const *mkdirnames[] = { "dir1", "dir1/dir2" };  // put in parent-first order
  char pathname[MAXPATHLEN];
  FILE *ofile;

  giveup_counter = 0;
  while (giveup_counter < MAXTRIES &&
	 tmpdirname == NULL) {

    tmpdirname = tempnam(NULL, NULL);

    /* Paranoid /tmp race checks */
    if (tmpdirname == NULL ||
	mkdir(tmpdirname, 0700) == -1 ||
	lstat(tmpdirname, &statbuff) == -1 ||
	!S_ISDIR(statbuff.st_mode) ||
	statbuff.st_uid != getuid()) {
      free(tmpdirname);
      tmpdirname = NULL;
    }

    giveup_counter++;
  }

  if (giveup_counter == MAXTRIES)
    throw ScannerTestFailed("Unable to create a working tmp directory.");

  for (i = 0; i < N_IN_ARRAY(mkdirnames); i++) {
    if (snprintf(pathname, MAXPATHLEN, "%s/%s", tmpdirname, mkdirnames[i]) == -1)
      throw ScannerTestFailed("Problem with file names too long.");
    
    if (mkdir(pathname, 0700) == -1)
      throw ScannerTestFailed("Unable to make a directory for the test.");

  }

  for (i = 0; i < N_IN_ARRAY(file_contents); i++) {
    if (snprintf(pathname, MAXPATHLEN, "%s/%s", tmpdirname, file_contents[i].relpathname) == -1)
      throw ScannerTestFailed("Problem with file names too long.");

    if ((ofile = fopen(pathname, "w")) == NULL)
      throw ScannerTestFailed("Failed to write to a test file.");

    if (fputs(file_contents[i].contents, ofile) == EOF) {
      fclose(ofile);
      throw ScannerTestFailed("Failed to write to a test file.");
    }

    if (fclose(ofile))
      throw ScannerTestFailed("Failed to close test file after writing.");
  }
}


#define CBUFF_LEN 1024

static void delete_tmp_tree(void)
{
  char commandbuffer[CBUFF_LEN];

  if (tmpdirname == NULL)
    return;

  if (snprintf(commandbuffer, CBUFF_LEN, "/bin/rm -fr %s", tmpdirname) == -1 ||
      system(commandbuffer))
    throw ScannerTestFailed("Unable to clean up temp files.");

  free(tmpdirname);
  tmpdirname = NULL;
}



static void use_file_reader(void)
{
  FileReader walker(tmpdirname);
  int i, fileindex, lineindex;
  char const * fname_patts[N_IN_ARRAY(trial_filename_patterns) + 1];
  char const * regex_patts[N_IN_ARRAY(lookup_info) + 1];
  String line;
  int key_found;
  vector<String> matches;
  bool foundfile[3], foundsecret;

  foundfile[0] = foundfile[1] = foundfile[2] = foundsecret = false;

  for (i = 0; i < N_IN_ARRAY(trial_filename_patterns); i++)
    fname_patts[i] = trial_filename_patterns[i];
  fname_patts[i] = NULL;

  while (walker.SearchFileRegexList(fname_patts, &fileindex) != NULL) {
  
    for (i = 0; i < N_IN_ARRAY(lookup_info); i++)
      regex_patts[i] = lookup_info[i].regex;
    regex_patts[i] = NULL;

    walker.SetSearchRegexps(regex_patts);


    while (walker.GetNextMatchingLine(line, &lineindex, matches) != -1) {

      switch(lookup_info[lineindex].match_type) {
      case MATCH_KEY:
	key_found = (int) atoi(matches[1].getCString());

	if (key_found != fileindex + 1)
	  throw ScannerTestFailed("Regular expression matched on wrong file.");
	
	foundfile[fileindex] = true;
	break;

      case MATCH_SECRET:
	if (fileindex != 2)
	  throw ScannerTestFailed("Secret found in the wrong file.");
	
	foundsecret = true;
	break;
      }
    }
  }

  if (!foundfile[0] ||
      !foundfile[1] ||
      !foundfile[2] ||
      !foundsecret)
    //  throw UndeclaredQualifier("Failed");
    throw ScannerTestFailed("Failed to locate all elements in the file scan.");

  return;
}


static void use_exec_scanner(void)
{
  ExecScanner scanner = ExecScanner();
  int i;
  const char* regex_patts[2];
  String line;
  int index;
  const char* args[] = { "/bin/sh", "-c", "echo Detect this test string", 
			  NULL };

  if (scanner.ExecuteForScan(args[0], args) != 0)
    throw ScannerTestFailed(String("Unable to execute ") + args[0]);


  regex_patts[0] = "^Detect this test string$";
  regex_patts[1] = NULL;

  scanner.SetSearchRegexps(regex_patts);

  // We now want to read exactly one line from the executable, and
  // have it match.

  if (scanner.GetNextMatchingLine(line, &index) == -1 ||
      index != 0)
    throw ScannerTestFailed("Failed to find output of ExecScanner");

  if (scanner.GetNextMatchingLine(line, &index) != -1)
    throw ScannerTestFailed("More data appeared where not expected");

  return;
}


int main(void)
{
  cout << "Beginning tests." << endl;

  try {
    build_tmp_tree();
    use_file_reader();
    use_exec_scanner();
  } 
  catch(Exception &e) {
    cerr << "Test failed: " << e.getMessage() << endl;
    delete_tmp_tree();
    exit(EXIT_FAILURE);
  }

  delete_tmp_tree();
  cout << "All tests passed." << endl;

  exit(EXIT_SUCCESS);
}

