//%/////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001, 2002 BMC Software, Hewlett-Packard Company, IBM,
// The Open Group, Tivoli Systems
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
// Author: Bob Blair (bblair@bmc.com)
//
// Modified By:  Gerarda Marquez (gmarquez@us.ibm.com)
//               -- PEP 43 changes
//
//%/////////////////////////////////////////////////////////////////////////////


//
// Constants for use by cmdline.cpp
//

#ifndef _CMDLINE_CMDLINE_H_
#define _CMDLINE_CMDLINE_H_

#include <Pegasus/Common/String.h>

enum opttypes {FILESPEC,
	       HELPFLAG, 
	       INCLUDEPATH,
	       SUPPRESSFLAG,
	       NAMESPACE,
	       REPOSITORYNAME, 
	       UPDATEFLAG,
	       ALLOWFLAG,
#ifndef PEGASUS_OS_HPUX
	       SYNTAXFLAG,
	       FILELIST,
	       TRACEFLAG,
	       XMLFLAG,
#endif
#ifdef PEGASUS_OS_OS400
	       QUIETFLAG,
#endif
	       OPTEND};

struct optspec {
  char *flag;
  opttypes catagory;
  int islong;
  int needsvalue;
};

#define PEGASUS_HOME "PEGASUS_HOME"
#define ROOTCIMV2 "root/cimv2"
#endif
