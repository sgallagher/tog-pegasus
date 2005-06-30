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
// Modified By: Sean Keenan, Hewlett-Packard Company (sean.keenan@hp.com)
//              Nag Boranna, Hewlett-Packard Company (nagaraja.boranna@hp.com)
//            
//
//%/////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_FixedPropertyTableVms_h
#define Pegasus_FixedPropertyTableVms_h

#ifdef PEGASUS_USE_RELEASE_CONFIG_OPTIONS
    {"httpPort",            "5988"},
    {"httpsPort",           "5989"},
    {"home",                ""},
    {"daemon",              "false"},
    {"slp",                 "false"},
    {"enableIndicationService", "true"},
    {"sslClientVerificationMode", "disabled"},
    {"httpAuthType",        "Basic"},
    {"repositoryIsDefaultInstanceProvider", "false"},
#endif

#ifdef PEGASUS_USE_RELEASE_DIRS
    {"traceFilePath",       "/wbem_var/opt/wbem/cimserver.trc"},
    {"passwordFilePath",    "/wbem_etc/opt/wbem/cimserver.passwd"},
    {"sslCertificateFilePath", "/wbem_etc/opt/hp/sslshare/cert.pem"},
    {"sslKeyFilePath",      "/wbem_etc/opt/hp/sslshare/file.pem"},
//
    {"sslTrustStore",       "/wbem_etc/opt/hp/sslshare/cimserver_trust"},
    {"exportSSLTrustStore", "/wbem_etc/opt/hp/sslshare/indication_trust"},
    {"crlStore",            "/wbem_etc/opt/hp/sslshare/crl"},
//
    {"tempLocalAuthDir",    PEGASUS_LOCAL_AUTH_DIR},
    {"logdir",              "/wbem_var/opt/wbem/logs"},
    {"repositoryDir",       "/wbem_var/opt/wbem/repository"},
    {"providerDir",         "/wbem_opt/wbem/providers/lib"},
#endif

#if !defined(PEGASUS_USE_RELEASE_CONFIG_OPTIONS) && !defined(PEGASUS_USE_RELEASE_DIRS)
    {"bogus", "MyBogusValue"}      // Remove this line if others are added
#endif

#endif /* Pegasus_FixedPropertyTableVms_h */
