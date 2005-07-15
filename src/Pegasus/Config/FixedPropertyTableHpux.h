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
// Modified By: Sushma Fernandes,  Hewlett-Packard Company sushma_fernandes@hp.com
//              Roger Kumpf, Hewlett-Packard Company (roger_kumpf@hp.com)
//            
//
//%/////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_FixedPropertyTableHpux_h
#define Pegasus_FixedPropertyTableHpux_h

#ifdef PEGASUS_USE_RELEASE_CONFIG_OPTIONS
    {"logLevel",            "INFORMATION"},
    {"httpPort",            "5988"},
    {"httpsPort",           "5989"},
    {"home",                ""},
    {"daemon",              "true"},
    {"slp",                 "false"},
    {"enableAuthentication", "true"},
    {"enableAssociationTraversal", "true"},
    // Removed because unresolved PEP 66 KS{"maximumEnumerationBreadth", "50", 0, 0, 0},
    {"enableIndicationService", "true"},
    {"sslClientVerificationMode", "disabled"},
    {"httpAuthType",        "Basic"},
    {"repositoryIsDefaultInstanceProvider", "false"},
    {"enableBinaryRepository", "false"},
    {"maxProviderProcesses", "0"},
#endif

#ifdef PEGASUS_USE_RELEASE_DIRS
    {"traceFilePath",       "/var/opt/wbem/trace/cimserver.trc"},
#ifndef PEGASUS_USE_SYSLOGS
    {"logdir",              "/var/opt/wbem/logs"},
#endif
    {"passwordFilePath",    "/etc/opt/wbem/cimserver.passwd"},
    {"sslCertificateFilePath", "/etc/opt/hp/sslshare/cert.pem"},
    {"sslKeyFilePath",      "/etc/opt/hp/sslshare/file.pem"},
    {"sslTrustStore",       "/etc/opt/hp/sslshare/cimserver_trust"},
    {"exportSSLTrustStore", "/etc/opt/hp/sslshare/indication_trust"},
    {"crlStore",            "/etc/opt/hp/sslshare/crl"},
    {"repositoryDir",       "/var/opt/wbem/repository"},
    {"providerDir",         "/opt/wbem/providers/lib"},
    {"messageDir",         "/opt/wbem/share/locale/ICU_Messages"},
#endif

#if !defined(PEGASUS_USE_RELEASE_CONFIG_OPTIONS) && !defined(PEGASUS_USE_RELEASE_DIRS)
    {"bogus", "MyBogusValue"}      // Remove this line if others are added
#endif

#endif /* Pegasus_FixedPropertyTableHpux_h */
