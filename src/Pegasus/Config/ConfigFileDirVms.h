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
// Author: Sean Keenan, Hewlett-Packard Company (sean.keenan@hp.com)
//
// Modified By:
//
//%/////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_ConfigFileDirVms_h
#define Pegasus_ConfigFileDirVms_h

#ifdef PEGASUS_USE_RELEASE_DIRS
/**
    Default file name for the current configuration.
*/
static char CURRENT_CONFIG_FILE [] = "/wbem_var/opt/wbem/cimserver_current.conf";

/**
    Default file name for the planned configuration.
*/
static char PLANNED_CONFIG_FILE [] = "/wbem_var/opt/wbem/cimserver_planned.conf";

/**
    Default file name for the cimserver startup file containing the PID
*/
static char CIMSERVER_START_FILE [] = "/wbem_var/opt/wbem/cimserver_start.conf";
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
    Default file name for the cimserver startup file containing the PID
*/
static char CIMSERVER_START_FILE [] = "/tmp/cimserver_start.conf";
#endif

#endif /* Pegasus_ConfigFileDirVms_h */
