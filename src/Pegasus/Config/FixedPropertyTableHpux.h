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
// Author: Jenny Yu, Hewlett-Packard Company (jenny.yu@hp.com)
//
// Modified By:
//
//%/////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_FixedPropertyTableHpux_h
#define Pegasus_FixedPropertyTableHpux_h

#ifdef PEGASUS_USE_RELEASE_CONFIG_OPTIONS
    {"trace",               "false"},
    {"logtrace",            "false"},
    {"cleanlogs",           "false"},
    {"severity",            "ALL"},
    {"httpPort",            "5988"},
    {"httpsPort",           "5989"},
    {"home",                ""},
    {"daemon",              "true"},
    {"slp",                 "false"},
    {"SSL",                 "false"},
    {"enableAuthentication", "true"},
    {"enableAssociationTraversal", "false"},
    {"enableIndicationService", "false"},
    {"enableClientCertification", "false"},
    {"httpAuthType",        "Basic"},
    {"repositoryIsDefaultInstanceProvider", "false"},
    {"usePAMAuthentication", "true"},
#endif

#ifdef PEGASUS_USE_RELEASE_DIRS
    {"traceFilePath",       "/var/opt/wbem/cimserver.trc"},
    {"logdir",              "/var/opt/wbem/logs"},
    {"tempLocalAuthDir",    "/var/opt/wbem/localauth"},
    {"passwordFilePath",    "/etc/opt/wbem/cimserver.passwd"},
    {"sslCertificateFilePath", "/var/opt/wbem/server.pem"},
    {"repositoryDir",       "/var/opt/wbem/repository"},
    {"providerDir",         "/opt/wbem/providers/lib"},
    {"consumerDir",         "/opt/wbem/providers/lib"}
#endif

#if !defined(PEGASUS_USE_RELEASE_CONFIG_OPTIONS) && !defined(PEGASUS_USE_RELEASE_DIRS)
    {"bogus", "MyBogusValue"}      // Remove this line if others are added
#endif

#endif /* Pegasus_FixedPropertyTableHpux_h */
