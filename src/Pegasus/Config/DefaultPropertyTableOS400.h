//%2003////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001, 2002  BMC Software, Hewlett-Packard Development
// Company, L. P., IBM Corp., The Open Group, Tivoli Systems.
// Copyright (c) 2003 BMC Software; Hewlett-Packard Development Company, L. P.;
// IBM Corp.; EMC Corporation, The Open Group.
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

    {"httpPort", "5988", 0, 0, 0, 1},
    {"httpsPort", "5989", 0, 0, 0, 1},
    {"enableHttpConnection", "true", 0, 0, 0, 1},
    {"enableHttpsConnection", "false", 0, 0, 0, 1},
    {"home", "/QIBM/UserData/OS400/CIM", 0, 0, 0, 1},
    {"daemon", "false", 0, 0, 0, 1},
    {"slp", "false", 0, 0, 0, 1},
    {"enableAssociationTraversal", "true", 0, 0, 0, 1},
    {"enableIndicationService", "false", 0, 0, 0, 1},
    {"tempLocalAuthDir", PEGASUS_LOCAL_AUTH_DIR, 0, 0, 0, 1},
    {"enableClientCertification", "false", 0, 0, 0, 1}

#endif /* Pegasus_DefaultPropertyTableOS400_h */
