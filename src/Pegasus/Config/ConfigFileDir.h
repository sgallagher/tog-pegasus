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
// Author: Warren Otsuka, Hewlett-Packard Company (warren.otsuka@hp.com)
//
// Modified By:  Jenny Yu, Hewlett-Packard Company (jenny.yu@hp.com)
//              Sean Keenan, Hewlett-Packard Company (sean.keenan@hp.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_ConfigFileDir_h
#define Pegasus_ConfigFileDir_h

#if defined(PEGASUS_PLATFORM_LINUX_GENERIC_GNU)
#include "ConfigFileDirLinux.h"
#elif PEGASUS_PLATFORM_HPUX_ACC
#include "ConfigFileDirHpux.h"
#elif PEGASUS_PLATFORM_ZOS_ZSERIES_IBM
#include "ConfigFileDirZos.h"
#elif PEGASUS_PLATFORM_AIX_RS_IBMCXX
#include "ConfigFileDirAix.h"
#elif PEGASUS_PLATFORM_DARWIN_PPC_GNU
#include "ConfigFileDirDarwin.h"
#elif defined (PEGASUS_OS_VMS)
#include "ConfigFileDirVms.h"
#else
/**
    Default file name for the current configuration.
*/
static char CURRENT_CONFIG_FILE [] = "cimserver_current.conf";

/**
    Default file name for the planned configuration.
*/
static char PLANNED_CONFIG_FILE [] = "cimserver_planned.conf";

/**
    Default file name for the cimserver startup file containing the PID.
*/
static char CIMSERVER_START_FILE [] = "/tmp/cimserver_start.conf";
#endif

#endif /* Pegasus_ConfigFileDir_h */
