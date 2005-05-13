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

#ifndef Pegasus_DefaultPropertyTableOS400_h
#define Pegasus_DefaultPropertyTableOS400_h

    {"httpPort", "5988", IS_STATIC, 0, 0, IS_VISIBLE},
    {"httpsPort", "5989", IS_STATIC, 0, 0, IS_VISIBLE},
    {"enableHttpConnection", "false", IS_STATIC, 0, 0, IS_VISIBLE},
    {"enableHttpsConnection", "true", IS_STATIC, 0, 0, IS_VISIBLE},
    {"home", "/QIBM/UserData/OS400/CIM", IS_STATIC, 0, 0, IS_VISIBLE},
    {"daemon", "false", IS_STATIC, 0, 0, IS_VISIBLE},
    {"slp", "false", IS_STATIC, 0, 0, IS_VISIBLE},
    {"enableAssociationTraversal", "true", IS_STATIC, 0, 0, IS_VISIBLE},
    {"enableIndicationService", "true", IS_STATIC, 0, 0, IS_VISIBLE},
    {"sslClientVerificationMode", "optional", IS_STATIC, 0, 0, IS_VISIBLE},
    {"httpAuthType","Basic",IS_STATIC, 0, 0, IS_VISIBLE},
    {"messageDir","/QIBM/ProdData/OS400/CIM/msg",IS_STATIC, 0, 0, IS_VISIBLE},
    {"forceProviderProcesses","false",IS_STATIC, 0, 0, IS_VISIBLE},
    {"maxProviderProcesses","0",IS_STATIC, 0, 0, IS_VISIBLE},
#endif /* Pegasus_DefaultPropertyTableOS400_h */
