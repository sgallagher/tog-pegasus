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
//  A sample use of the FileReader class.
//
//  We want to retrieve the processor number and cpu MHz for all CPUs on
//  the box.  Also, we pull /proc/dma, just so we can demonstrate looping
//  over multiple files.


#include <stdio.h>
#include <unistd.h>


// #define COMPILE_STAND_ALONE

#include "FileReader.h"
#include "ExecScanner.h"

#define N_IN_ARRAY(x) ((int) (sizeof(x) / sizeof(x[0])))

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/String.h>

PEGASUS_NAMESPACE_BEGIN

static char const *trial_filename_patterns[] = { 
  "^/proc/no_such_file$",
  "^/proc/cpuinfo$",
  "^/proc/dma$",
};

enum type_of_match {
  MATCH_PROCESSOR_BEGIN, MATCH_PROCESSOR_END, MATCH_CPU_SPEED,
  MATCH_CPU_MODEL, MATCH_DMA_STRING
};


static struct {
  char *regex;
  enum type_of_match match_type;
} lookup_info[] = {
  { "^processor +: +([0-9]+)[ \t]*$",
    MATCH_PROCESSOR_BEGIN
  },
  {
    "^[ \t]*$",
    MATCH_PROCESSOR_END
  },
  {
    "^cpu MHz +: +([0-9])+[ \t]*$",
    MATCH_CPU_SPEED
  },
  {
    "^cpu MHz +: +([0-9]+\\.[0-9]*)[ \t]*$",
    MATCH_CPU_SPEED
  },
  {
    "^model +: +(.*)$",
    MATCH_CPU_MODEL
  },
  {
    "^(.* DMA)$",
    MATCH_DMA_STRING
  },
};


enum type_of_if_match {
  MATCH_INTERFACE_NAME, MATCH_INTERFACE_ADDRESS_W_BCAST, 
  MATCH_INTERFACE_ADDRESS_WO_BCAST, MATCH_RX_PACKETS,
  MATCH_TX_PACKETS, MATCH_BLOCK_END
};

static struct {
  char *regex;
  enum type_of_if_match match_type;
} if_info[] = {
  { 
    "^([^ \t]+)[ \t]",
    MATCH_INTERFACE_NAME
  },
  {
    "inet addr:([0-9.]+)[ \t]+Bcast:([0-9.]+)[ \t]+Mask:([0-9.]+)[ \t]*$",
    MATCH_INTERFACE_ADDRESS_W_BCAST
  },
  {
    "inet addr:([0-9.]+)[ \t]+Mask:([0-9.]+)[ \t]*$",
    MATCH_INTERFACE_ADDRESS_WO_BCAST
  },
  {
    "RX packets:([0-9]+)[ \t]",
    MATCH_RX_PACKETS
  },
  {
    "TX packets:([0-9]+)[ \t]",
    MATCH_TX_PACKETS
  },
  {
    "^[ \t]*$",
    MATCH_BLOCK_END
  },
};



PEGASUS_NAMESPACE_END


static void use_file_reader(void)
{
  FileReader walker;
  int i, index;
  char const * fname_patts[N_IN_ARRAY(trial_filename_patterns) + 1];
  char const * regex_patts[N_IN_ARRAY(lookup_info) + 1];
  String line;
  vector<String> matches;

  for (i = 0; i < N_IN_ARRAY(trial_filename_patterns); i++)
    fname_patts[i] = trial_filename_patterns[i];
  fname_patts[i] = NULL;

  while (walker.SearchFileRegexList(fname_patts, &index) != NULL) {

  
    for (i = 0; i < N_IN_ARRAY(lookup_info); i++)
      regex_patts[i] = lookup_info[i].regex;
    regex_patts[i] = NULL;

    try {
      walker.SetSearchRegexps(regex_patts);
    }
    catch(Exception &e) {
      cerr << "Error: " << e.getMessage() << endl;
      exit(EXIT_FAILURE);
    }

    while (walker.GetNextMatchingLine(line, &index, matches) != -1) {

      switch(lookup_info[index].match_type) {
      case MATCH_PROCESSOR_BEGIN:
	cout << "Beginning of data block for processor " << matches[1] << endl;
	break;
      case MATCH_CPU_SPEED:
	cout << "Speed of CPU:  " << matches[1] << endl;
	break;
      case MATCH_CPU_MODEL:
	cout << "CPU model identified as:  \"" << matches[1] << "\"" << endl;
	break;
      case MATCH_PROCESSOR_END:
	cout << "End of data block for this processor." << endl << endl;
	break;
      case MATCH_DMA_STRING:
	cout << "DMA String:  \"" << matches[1] << "\"" << endl;
	break;
      }
    }
  }
}


static void use_exec_scanner(void)
{
  ExecScanner scanner = ExecScanner();
  int i;
  char const *regex_patts[N_IN_ARRAY(if_info) + 1];
  String line;
  vector<String> matches;
  int index;
  char * const args[] = { "ifconfig", NULL };

  if (scanner.ExecuteForScan("ifconfig", args) != 0) {
    fprintf(stderr, "Failed to execute ifconfig for scanning.\n");
    exit(EXIT_FAILURE);
  }

  for (i = 0; i < N_IN_ARRAY(if_info); i++)
    regex_patts[i] = if_info[i].regex;
  regex_patts[i] = NULL;

  try {
    scanner.SetSearchRegexps(regex_patts);
  }    
  catch(Exception& e) {
    cerr << "Error: " << e.getMessage() << endl;
    exit(EXIT_FAILURE);
  }

  while (scanner.GetNextMatchingLine(line, &index, matches) != -1) {
    switch(if_info[index].match_type) {
    case MATCH_INTERFACE_NAME:
      cout << "Beginning of data for interface \""
	   << matches[1] << "\"" << endl;
      break;
    case MATCH_INTERFACE_ADDRESS_W_BCAST:
      cout << "Interface addr: " << matches[1] 
	   << "  Broadcast addr: " << matches[2]
	   << "  Netmask: " << matches[3] << endl
      break;
    case MATCH_INTERFACE_ADDRESS_WO_BCAST:
      cout << "Interface addr: " << matches[1]
	   << "  No broadcast address.  Netmask: " << matches[2] << endl;
      break;
    case MATCH_RX_PACKETS:
      cout << "Packets received on interface: " << matches[1] << endl;
      break;
    case MATCH_TX_PACKETS:
      cout << "Packets transmitted on interface: " << matches[1] << endl;
      break;
    case MATCH_BLOCK_END:
      cout << "End of data for this interface." << endl << endl;
    }
  }
}

int main(void)
{
  use_file_reader();
  use_exec_scanner();

  exit(EXIT_SUCCESS);
}
