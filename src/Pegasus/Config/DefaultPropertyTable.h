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
// Author: Warren Otsuka, Hewlett-Packard Company (warren.otsuka@hp.com)
//
// Modified By:
//
//%/////////////////////////////////////////////////////////////////////////////

#ifdef PEGASUS_PLATFORM_LINUX_IA64_GNU
#include "DefaultPropertyTableLinuxIA64.h"
#else
    {"httpPort", "5988", 0, 0, 0},
    {"httpsPort", "5989", 0, 0, 0},
    {"enableHttpConnection", "true", 0, 0, 0},
    {"enableHttpsConnection", "false", 0, 0, 0},
    {"home", "./", 0, 0, 0},
    {"daemon", "true", 0, 0, 0},
    {"install", "false", 0, 0, 0},
    {"remove", "false", 0, 0, 0},
    {"slp", "false", 0, 0, 0},
    {"SSL", "false", 0, 0, 0},
    {"enableAssociationTraversal", "true", 0, 0, 0},
    {"enableIndicationService", "true", 0, 0, 0},
    {"tempLocalAuthDir", "/tmp", 0, 0, 0},
    {"enableClientCertification", "false", 0, 0, 0}
#endif
