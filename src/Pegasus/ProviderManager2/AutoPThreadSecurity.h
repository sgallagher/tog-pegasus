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

#ifndef Pegasus_AutoPThreadSecurity_h
#define Pegasus_AutoPThreadSecurity_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/OperationContextInternal.h>
#include <Pegasus/Common/Exception.h>
#include <Pegasus/ProviderManager2/Linkage.h>

#ifdef PEGASUS_ZOS_SECURITY
// This include file will not be provided in the OpenGroup CVS for now.
// Do NOT try to include it in your compile
# include <Pegasus/ProviderManager2/ProviderManagerzOS_inline.h>
#endif

PEGASUS_NAMESPACE_BEGIN

// Auto class to encapsulate enabling and disabling
// of the pthread_security on z/OS
// For all other platforms this should be an empty class
// Targets: avoid ifdefs and keep code readable(clean)
#ifndef PEGASUS_ZOS_THREADLEVEL_SECURITY

// not z/OS == empty class
class AutoPThreadSecurity
{
public:
    AutoPThreadSecurity(const OperationContext& context) {};
};

#else

class AutoPThreadSecurity
{
public:
    AutoPThreadSecurity(const OperationContext& context)
    {
        int err_num=enablePThreadSecurity(context);
        if (err_num!=0)
        {
            // need a new CIMException for this
            throw CIMException(CIM_ERR_ACCESS_DENIED,String(strerror(err_num)));
        }
    };

    ~AutoPThreadSecurity()
    {
        disablePThreadSecurity();
    };
};

#endif

PEGASUS_NAMESPACE_END

#endif
