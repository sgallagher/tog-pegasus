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
// Author: Jenny Yu, Hewlett-Packard Company (jenny.yu@hp.com)
//
// Modified By: Sushma Fernandes,  Hewlett-Packard Company
//                 sushma_fernandes@hp.com
//
//%/////////////////////////////////////////////////////////////////////////////

#ifdef PEGASUS_USE_RELEASE_CONFIG_OPTIONS
    {"logLevel",            "INFORMATION"},
    {"httpPort",            "5988"},
    {"httpsPort",           "5989"},
    {"home",                ""},
    {"daemon",              "true"},
    {"install",             "false"},
    {"remove",              "false"},
    {"slp",                 "false"},
    {"enableAuthentication", "true"},
    {"enableAssociationTraversal", "true"},
    {"enableIndicationService", "true"},
    {"enableClientCertification", "false"},
    {"httpAuthType",        "Basic"},
    {"repositoryIsDefaultInstanceProvider", "false"},
#endif

#ifdef PEGASUS_USE_RELEASE_DIRS
//    {"traceFilePath",       "/opt/freeware/cimom/pegasus/logs/cimserver.trc"},
//    {"logdir",              "/opt/freeware/cimom/pegasus/logs"},
//    {"sslCertificateFilePath", "/opt/freeware/cimom/pegasus/etc/cert.pem"},
//    {"sslKeyFilePath",      "/opt/freeware/cimom/pegasus/etc/file.pem"},
//    {"sslTrustFilePath",      "/opt/freeware/cimom/pegasus/etc/client.pem"},
//    {"passwordFilePath",    "/opt/freeware/cimom/pegasus/etc/cimserver.passwd"},
    {"messageDir",          "/opt/freeware/cimom/pegasus/msg"},
    {"repositoryDir",       "/opt/freeware/cimom/pegasus/etc/repository"},
    {"providerDir",         "/usr/lib:/usr/pegasus/provider/lib"},
#endif

#if !defined(PEGASUS_USE_RELEASE_CONFIG_OPTIONS) && !defined(PEGASUS_USE_RELEASE_DIRS)
    {"bogus", "MyBogusValue"}      // Remove this line if others are added
#endif
