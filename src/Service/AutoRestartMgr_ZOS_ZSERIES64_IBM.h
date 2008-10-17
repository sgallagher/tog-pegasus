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
// Author: Thilo Boehm (tboehm@de.ibm.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#ifndef AutoRestartMgr_ZOS_ZSERIES64_IBM_h
#define AutoRestartMgr_ZOS_ZSERIES64_IBM_h

////////////////////////////////////////////////////////////////////////////////
// internal prototypes for the assembler services called 
////////////////////////////////////////////////////////////////////////////////
 
#ifdef __cplusplus
extern "OS_NOSTACK" {
#endif


////////////////////////////////////////////////////////////////////////////////
// assembler routine which registers with ARM 
////////////////////////////////////////////////////////////////////////////////

#pragma map    (__register_arm,"REGARM")
void __register_arm (char * elemname,  // elementname (16bytes) right-padded
                                     // with blanks 
                  char * buffer,     // must point to a local buffer which has
                                     // a length of least 128 bytes 
                  int * retcode,     // contains returncode on return 
                  int * reasoncode); // contains reasoncode on return

////////////////////////////////////////////////////////////////////////////////
// assembler routine put element as READY with ARM 
////////////////////////////////////////////////////////////////////////////////

#pragma map    (__ready_arm,"READYARM")
void __ready_arm ( char * buffer,   // must point to a local buffer which has
                                    // a length of least 128 bytes 
                 int * retcode,     // contains returncode on return 
                 int * reasoncode); // contains reasoncode on return

////////////////////////////////////////////////////////////////////////////////
// assembler routine which deregisters element from ARM
////////////////////////////////////////////////////////////////////////////////

#pragma map    (__deregister_arm,"DEREGARM")
void __deregister_arm ( char * buffer, // must point to a local buffer which has
                                    // a length of least 128 bytes 
                 int * retcode,     // contains returncode on return 
                 int * reasoncode); // contains reasoncode on return

#ifdef __cplusplus
}
#endif

#endif  /* ifndef ARM_H */
