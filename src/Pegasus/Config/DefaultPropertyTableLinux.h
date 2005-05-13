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
// Modified By: Roger Kumpf, Hewlett-Packard Company (roger_kumpf@hp.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_DefaultPropertyTableLinux_h
#define Pegasus_DefaultPropertyTableLinux_h


#ifdef PEGASUS_USE_RELEASE_CONFIG_OPTIONS
    {"logLevel", "SEVERE", IS_STATIC, 0, 0, IS_VISIBLE},
    {"httpPort", "", IS_STATIC, 0, 0, IS_VISIBLE},
    {"httpsPort", "", IS_STATIC, 0, 0, IS_VISIBLE},
    {"enableHttpConnection", "false", IS_STATIC, 0, 0, IS_VISIBLE},
    {"enableHttpsConnection", "true", IS_STATIC, 0, 0, IS_VISIBLE},
    {"home", "", IS_STATIC, 0, 0, IS_VISIBLE},
    {"daemon", "true", IS_STATIC, 0, 0, IS_HIDDEN},
    {"install", "false", IS_STATIC, 0, 0, IS_VISIBLE},
    {"remove", "false", IS_STATIC, 0, 0, IS_VISIBLE},
    {"slp", "false", IS_STATIC, 0, 0, IS_VISIBLE},
    {"enableAssociationTraversal", "true", IS_STATIC, 0, 0, IS_VISIBLE},
    {"enableAuthentication", "true", IS_STATIC, 0, 0, IS_VISIBLE},
    {"enableNamespaceAuthorization", "false", IS_STATIC, 0, 0, IS_VISIBLE},
    {"enableIndicationService", "true", IS_STATIC, 0, 0, IS_VISIBLE},
    {"enableSubscriptionsForNonprivilegedUsers", "false", IS_STATIC, 0, 0, IS_VISIBLE},
    {"enableRemotePrivilegedUserAccess", "true", IS_STATIC, 0, 0, IS_VISIBLE},
    // Removed because unresolved PEP 66 KS{"maximumEnumerationBreadth", "50", IS_STATIC, 0, 0},
    {"sslClientVerificationMode", "disabled", IS_STATIC, 0, 0, IS_VISIBLE},
    {"forceProviderProcesses", "false", IS_STATIC, 0, 0, IS_VISIBLE},
    {"maxProviderProcesses", "0", IS_STATIC, 0, 0, IS_VISIBLE}
#else
    {"logLevel", "INFORMATION", IS_STATIC, 0, 0, IS_VISIBLE},
    {"httpPort", "", IS_STATIC, 0, 0, IS_VISIBLE},
    {"httpsPort", "", IS_STATIC, 0, 0, IS_VISIBLE},
    {"enableHttpConnection", "true", IS_STATIC, 0, 0, IS_VISIBLE},
    {"enableHttpsConnection", "true", IS_STATIC, 0, 0, IS_VISIBLE},
    {"home", "./", IS_STATIC, 0, 0, IS_VISIBLE},
    {"daemon", "true", IS_STATIC, 0, 0, IS_VISIBLE},
    {"install", "false", IS_STATIC, 0, 0, IS_VISIBLE},
    {"remove", "false", IS_STATIC, 0, 0, IS_VISIBLE},
    {"slp", "false", IS_STATIC, 0, 0, IS_VISIBLE},
    {"enableAssociationTraversal", "true", IS_STATIC, 0, 0, IS_VISIBLE},
    {"enableAuthentication", "false", IS_STATIC, 0, 0, IS_VISIBLE},
    {"enableNamespaceAuthorization", "false", IS_STATIC, 0, 0, IS_VISIBLE},
    {"enableIndicationService", "true", IS_STATIC, 0, 0, IS_VISIBLE},
    {"enableSubscriptionsForNonprivilegedUsers", "true", IS_STATIC, 0, 0, IS_VISIBLE},
    {"enableRemotePrivilegedUserAccess", "true", IS_STATIC, 0, 0, IS_VISIBLE},
    {"maximumEnumerationBreadth", "50", IS_STATIC, 0, 0, IS_VISIBLE},
    {"sslClientVerificationMode", "disabled", IS_STATIC, 0, 0, IS_VISIBLE},
    {"forceProviderProcesses", "false", IS_STATIC, 0, 0, IS_VISIBLE},
    {"maxProviderProcesses", "0", IS_STATIC, 0, 0, IS_VISIBLE}
#endif


#endif /* Pegasus_DefaultPropertyTableLinux_h */
