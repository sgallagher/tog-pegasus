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

#ifndef Pegasus_SSLDir_h
#define Pegasus_SSLDir_h

#ifdef PEGASUS_PLATFORM_LINUX_IA64_GNU
#include <Pegasus/Common/SSLDirLinuxIA64.h>
#elif defined(PEGASUS_OS_HPUX) && defined(PEGASUS_USE_RELEASE_DIRS)
#define PEGASUS_SSLCLIENT_CERTIFICATEFILE "/var/opt/wbem/server.pem"
#define PEGASUS_SSLCLIENT_RANDOMFILE      "/var/opt/wbem/ssl.rnd"
#define PEGASUS_LOCAL_AUTH_DIR            "/var/opt/wbem/localauth"
#else
#define PEGASUS_SSLCLIENT_CERTIFICATEFILE "server.pem"
#define PEGASUS_SSLCLIENT_RANDOMFILE      "ssl.rnd"
#define PEGASUS_LOCAL_AUTH_DIR            "/tmp"
#endif

#endif /* Pegasus_SSLDir_h */
