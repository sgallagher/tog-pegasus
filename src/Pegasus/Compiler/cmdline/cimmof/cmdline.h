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
           REPOSITORYDIR,

           UPDATEFLAG,
           ALLOWFLAG,
#ifndef PEGASUS_OS_HPUX
           SYNTAXFLAG,
//PEP167     FILELIST,
           TRACEFLAG,
           XMLFLAG,
#endif
#ifdef PEGASUS_OS_PASE
           QUIETFLAG, //PASE env ship q option
#endif
#ifdef PEGASUS_ENABLE_MRR_GENERATION
           MRRFLAG,
           DISCARDFLAG,
#endif
           VERSIONFLAG,
           OPTEND_CIMMOF,    //PEP167
           REPOSITORYNAME,
           REPOSITORYMODE,
           NO_USAGE_WARNING,
           OPTEND_CIMMOFL};  //PEP167

struct optspec
{
    char *flag;
    opttypes catagory;
    int islong;
    int needsvalue;
};

// Wrap this around the PEGASUS_HOME define for OS/400

#define PEGASUS_HOME "PEGASUS_HOME"

#define PEGASUS_CIMMOF_NO_DEFAULTNAMESPACEPATH    -9
#define PEGASUS_CIMMOF_COMPILER_GENERAL_EXCEPTION -8
#define PEGASUS_CIMMOF_BAD_FILENAME               -7
#define PEGASUS_CIMMOF_PARSING_ERROR              -6
#define PEGASUS_CIMMOF_PARSER_LEXER_ERROR         -5
#define PEGASUS_CIMMOF_UNEXPECTED_CONDITION       -4
#define PEGASUS_CIMMOF_CMDLINE_NOREPOSITORY       -3
#define PEGASUS_CIMMOF_CIM_EXCEPTION              -2

#define ROOTCIMV2 "root/cimv2"
#define REPOSITORY_NAME_DEFAULT "repository"
#define REPOSITORY_MODE_DEFAULT "XML"
#endif
