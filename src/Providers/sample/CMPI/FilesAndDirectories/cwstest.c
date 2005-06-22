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
//%/////////////////////////////////////////////////////////////////////////////

#include "cwsutil.h"

#include <stdio.h>
#include <stdlib.h>

int main(int argc, char * argv[])
{
  void     *enumhdl;
  CWS_FILE  filebuf;

  if (argc != 2) {
    fprintf(stderr,"usage: %s directory\n",argv[0]);
    exit(-1);
  }

  printf("=== Searching for plain files in %s \n",argv[1]);
  enumhdl = CWS_Begin_Enum(argv[1],CWS_TYPE_PLAIN);
  if (enumhdl) {
    while (CWS_Next_Enum(enumhdl,&filebuf))
#if defined (CMPI_PLATFORM_WIN32_IX86_MSVC)
      printf("--- File: %s Size: %I64d Mode: %u\n",
#else
      printf("--- File: %s Size: %lld Mode: %u\n",
#endif
	     filebuf.cws_name, filebuf.cws_size, filebuf.cws_mode);
    CWS_End_Enum(enumhdl);
  }

  printf("=== Searching for directories in %s \n",argv[1]);
  enumhdl = CWS_Begin_Enum(argv[1],CWS_TYPE_DIR);
  if (enumhdl) {
    while (CWS_Next_Enum(enumhdl,&filebuf))
#if defined (CMPI_PLATFORM_WIN32_IX86_MSVC)
      printf("--- Dir: %s Size: %I64d Mode: %u\n",
#else
      printf("--- Dir: %s Size: %lld Mode: %u\n",
#endif

	     filebuf.cws_name, filebuf.cws_size, filebuf.cws_mode);
    CWS_End_Enum(enumhdl);
  }

  printf("=== Direct Access to Directory %s \n",argv[1]);
  if (CWS_Get_File(argv[1],&filebuf))
#if defined (CMPI_PLATFORM_WIN32_IX86_MSVC)
    printf("--- Dir: %s Size: %I64d Mode: %u\n",
#else
    printf("--- Dir: %s Size: %lld Mode: %u\n",
#endif
	   filebuf.cws_name, filebuf.cws_size, filebuf.cws_mode);
  
  return 0;
}
