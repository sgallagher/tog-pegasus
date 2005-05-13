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
//               Heather Sterling, IBM (hsterl@us.ibm.com)
//               Roger Kumpf, Hewlett-Packard Company (roger_kumpf@hp.com)
//               Sean Keenan, Hewlett-Packard Company (sean.keenan@hp.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_DefaultPropertyTable_h
#define Pegasus_DefaultPropertyTable_h

#if defined(PEGASUS_PLATFORM_LINUX_GENERIC_GNU)
#include "DefaultPropertyTableLinux.h"
#elif PEGASUS_PLATFORM_HPUX_ACC
#include "DefaultPropertyTableHpux.h"
#elif defined(PEGASUS_PLATFORM_OS400_ISERIES_IBM)
#include "DefaultPropertyTableOS400.h"
#elif defined (PEGASUS_OS_VMS)
#include "DefaultPropertyTableVms.h"
#else
    {"httpPort", "", IS_STATIC, 0, 0, IS_VISIBLE},
    {"httpsPort", "", IS_STATIC, 0, 0, IS_VISIBLE},
    {"enableHttpConnection", "true", IS_STATIC, 0, 0, IS_VISIBLE},
    {"enableHttpsConnection", "false", IS_STATIC, 0, 0, IS_VISIBLE},
    {"home", "./", IS_STATIC, 0, 0, IS_VISIBLE},
    {"daemon", "true", IS_STATIC, 0, 0, IS_VISIBLE},
    {"slp", "false", IS_STATIC, 0, 0, IS_VISIBLE},
    {"enableAssociationTraversal", "true", IS_STATIC, 0, 0, IS_VISIBLE},
    {"enableIndicationService", "true", IS_STATIC, 0, 0, IS_VISIBLE},
    // Removed because unresolved PEP 66 KS{"maximumEnumerationBreadth", "50", IS_STATIC, 0, 0},
    {"sslClientVerificationMode", "disabled", IS_STATIC, 0, 0, IS_VISIBLE},
    {"forceProviderProcesses", "false", IS_STATIC, 0, 0, IS_VISIBLE},
    {"maxProviderProcesses", "0", IS_STATIC, 0, 0, IS_VISIBLE}
#endif

#endif /* Pegasus_DefaultPropertyTable_h */
