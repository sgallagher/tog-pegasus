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

#ifndef Pegasus_FixedPropertyTableVms_h
#define Pegasus_FixedPropertyTableVms_h

#ifdef PEGASUS_USE_RELEASE_CONFIG_OPTIONS
    {"httpPort",            "5988"},
    {"httpsPort",           "5989"},
    {"daemon",              "false"},
#ifdef PEGASUS_ENABLE_SLP
    {"slp",                 "false"},
#endif
    {"enableAuthentication", "true"},
    {"enableIndicationService", "true"},
    {"sslClientVerificationMode", "disabled"},
    {"httpAuthType",        "Basic"},
    {"repositoryIsDefaultInstanceProvider", "false"},
#endif

#ifdef PEGASUS_USE_RELEASE_DIRS
    {"traceFilePath",       "/wbem_var/opt/wbem/trace/cimserver.trc"},
    {"passwordFilePath",    "/wbem_etc/opt/wbem/cimserver.passwd"},
    {"sslCertificateFilePath", "/wbem_etc/opt/hp/sslshare/server.pem"},
    {"sslKeyFilePath",      "/wbem_etc/opt/hp/sslshare/file.pem"},
//
    {"sslTrustStore",       "/wbem_etc/opt/hp/sslshare/cimserver_trust"},
    {"crlStore",            "/wbem_etc/opt/hp/sslshare/crl"},
//
    {"logdir",              "/wbem_var/opt/wbem/logs"},
    {"repositoryDir",       "/wbem_var/opt/wbem/repository"},
    {"providerDir",         "/wbem_lib"},
    {"providerManagerDir",  "/wbem_lib"},
#else
    {"traceFilePath",       "trace/cimserver.trc"},
    {"logdir",              "logs"},
#endif

#if !defined(PEGASUS_USE_RELEASE_CONFIG_OPTIONS) && \
    !defined(PEGASUS_USE_RELEASE_DIRS)
    {"bogus", "MyBogusValue"}      // Remove this line if others are added
#endif

#endif /* Pegasus_FixedPropertyTableVms_h */
