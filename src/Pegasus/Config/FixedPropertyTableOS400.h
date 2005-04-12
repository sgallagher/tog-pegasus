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
// Author: David Rosckes (rosckes@us.ibm.com)
//
// Modified By: 
//
//%/////////////////////////////////////////////////////////////////////////////

#ifdef PEGASUS_USE_RELEASE_CONFIG_OPTIONS
  //  {"enableAuthentication",                     "true"},
    {"enableRemotePrivilegedUserAccess",         "true"},
    {"enableNamespaceAuthorization",             "true"},
//    {"enableHttpConnection",                     "true"},
    {"enableBinaryRepository",                  "false"},
    // Do not fix the daemon property.  Otherwise, daemon=true cannot
    // be passed on the CIMOM command line.
    // Note: fixing the daemon property to true could
    // lead to an infintite loop of QYCMCIMOM jobs being submitted.
//  {"daemon",                                   "false"},
//    {"enableIndicationService",                  "false"},
    // The following properties are not supported by OS400        
    {"providerDir",                              "lib"},		
    {"logdir",                                   "./logs"},		
    {"passwordFilePath",                         "cimserver.passwd"},
//    {"sslCertificateFilePath",                   "server.pem"},
//    {"sslKeyFilePath",                           "file.pem"},
//    {"sslTrustStore",                            "client.pem"},
//    {"sslClientVerificationMode",                "disabled"},
//    {"enableSubscriptionsForNonprivilegedUsers", "true"},
//    {"httpsPort",                                "5989"},
//    {"enableHttpsConnection",                    "false"},
    {"slp",                                      "false"},
    {"authorizedUserGroups",                          ""},
    {"home",                  "/QIBM/UserData/OS400/CIM"},
    {"forceProviderProcesses",                   "false"},
    {"maxProviderProcesses",                         "0"},
#endif

#if !defined(PEGASUS_USE_RELEASE_CONFIG_OPTIONS)
    {"bogus", "MyBogusValue"} // Remove this line if others are added
#endif
