//%2006////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001, 2002 BMC Software; Hewlett-Packard Development
// Company, L.P.; IBM Corp.; The Open Group; Tivoli Systems.
// Copyright (c) 2003 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation, The Open Group.
// Copyright (c) 2004 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation; VERITAS Software Corporation; The Open Group.
// Copyright (c) 2005 Hewlett-Packard Development Company, L.P.; IBM Corp.;
// EMC Corporation; VERITAS Software Corporation; The Open Group.
// Copyright (c) 2006 Hewlett-Packard Development Company, L.P.; IBM Corp.;
// EMC Corporation; Symantec Corporation; The Open Group.
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

// Please be aware that the CMPI C++ API is NOT a standard currently.

#ifndef CWS_FILEUTILS_H
#define CWS_FILEUTILS_H

#include <Providers/sample/CMPI/CWS_Util/cwsutil.h>
#include <Pegasus/Provider/CMPI/CmpiBroker.h>
#include <Pegasus/Provider/CMPI/CmpiInstance.h>
#include <Pegasus/Provider/CMPI/CmpiObjectPath.h>

#if defined SIMULATED
 #define CWS_FILEROOT  "/Simulated/CMPI/tests/"
#else
 #define CWS_FILEROOT  "/tmp"
#endif

char * CSCreationClassName();
char * CSName();
char * FSCreationClassName();
char * FSName();


CmpiObjectPath makePath(const char *classname, 
			const char *nameSpace, const CWS_FILE *cwsf);
CmpiInstance   makeInstance(const char *classname, 
			    const char *nameSpace, const CWS_FILE *cwsf, 
			    const char **projection);
int            makeFileBuf(const CmpiInstance& instance, CWS_FILE *cwsf);
char         **projection2Filter(const CmpiArray& ar);
void           freeFilter(char**);


#endif
