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
// Author: Mike Day (mdday@us.ibm.com)
//
// Modified By: Markus Mueller
//              Ramnath Ravindran (Ramnath.Ravindran@compaq.com)
//              David Eger (dteger@us.ibm.com)
//              Amit K Arora, IBM (amita@in.ibm.com) for PEP#101
//              Sean Keenan, Hewlett-Packard Company (sean.keenan@hp.com)
//              Roger Kumpf, Hewlett-Packard Company (roger_kumpf@hp.com)
//              David Dillard, VERITAS Software Corp.
//                  (david.dillard@veritas.com)
//              Aruran, IBM (ashanmug@in.ibm.com) for BUG# 3518
//              Mike Brasher (mike-brasher@austin.rr.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_IPCTypes_h
#define Pegasus_IPCTypes_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/Linkage.h>
#include <Pegasus/Common/AutoPtr.h>

#if !defined(PEGASUS_OS_SOLARIS) && !defined(PEGASUS_OS_LSB) && !defined(PEGASUS_OS_VMS)
#define PEGASUS_NEED_CRITICAL_TYPE
#endif

#if defined(PEGASUS_PLATFORM_WIN32_IX86_MSVC)
# include "IPCWindows.h"
#elif defined(PEGASUS_PLATFORM_LINUX_GENERIC_GNU)
# include "IPCUnix.h"
#elif defined(PEGASUS_PLATFORM_HPUX_ACC)
# include "IPCHpux.h"
#elif defined(PEGASUS_PLATFORM_SOLARIS_SPARC_GNU)
# include "IPCUnix.h"
#elif defined(PEGASUS_PLATFORM_SOLARIS_SPARC_CC)
# include "IPCSun.h"
#elif defined(PEGASUS_PLATFORM_AIX_RS_IBMCXX)
# include "IPCAix.h"
#elif defined(PEGASUS_PLATFORM_ZOS_ZSERIES_IBM)
# include "IPCzOS.h"
#elif defined(PEGASUS_PLATFORM_TRU64_ALPHA_DECCXX)
# include "IPCTru64.h"
#elif defined(PEGASUS_PLATFORM_OS400_ISERIES_IBM)
# include "IPCOs400.h"
#elif defined(PEGASUS_PLATFORM_DARWIN_PPC_GNU)
# include "IPCUnix.h"
#elif defined(PEGASUS_OS_VMS)
# include "IPCVms.h"
#else
# error "Unsupported platform"
#endif

#endif /* Pegasus_IPCTypes_h */
