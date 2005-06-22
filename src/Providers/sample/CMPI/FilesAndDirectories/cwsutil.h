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

#ifndef CWSUTIL_H
#define CWSUTIL_H

#include <time.h>

#ifdef __cplusplus 
/*extern "C" {*/
#endif
  
/* ------------------------------------------------------------------
 * Utilities for file info retrieval
 * ----------------------------------------------------------------- */

#define CWS_MAXPATH    1025

#define CWS_TYPE_DIR   'd'
#define CWS_TYPE_PLAIN 'f'

#ifdef PEGASUS_PLATFORM_WIN32_IX86_MSVC
#define SINT64 __int64
#define strcasecmp _stricmp
char * dirname(char *path);
#else
#define SINT64 long long
#endif

struct _CWS_FILE {
  char      cws_name[CWS_MAXPATH];
  SINT64 cws_size;
  time_t    cws_ctime;
  time_t    cws_mtime;
  time_t    cws_atime;
  unsigned  cws_mode;
};
typedef struct _CWS_FILE CWS_FILE;

/* ------------------------------------------------------------------
 * File Enumeration Support, use like this:
 *
 *  CWS_FILE filebuf;
 *  void * hdl = CWS_Begin_Enum("/test",CWS_TYPE_FILE);
 *  if (hdl) {
 *    while(CWS_Next_Enum(hdl,&filebuf) {...}
 *    CWS_End_Enum(hdl);
 *  }
 * ----------------------------------------------------------------- */



void* CWS_Begin_Enum(const char *topdir, int filetype);
int CWS_Next_Enum(void *handle, CWS_FILE* cwsf);
void CWS_End_Enum(void *handle);

int CWS_Get_File(const char *file, CWS_FILE* cwsf);
int CWS_Update_File(CWS_FILE* cwsf);
int CWS_Create_Directory(CWS_FILE* cwsf);
int CWS_Get_FileType(const char *file, char* typestring, size_t tslen);

#ifdef __cplusplus 
/*}*/
#endif

#endif
