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
// Author: Roger Kumpf, Hewlett-Packard Company (roger_kumpf@hp.com)
//
// Modified By:
//
//%/////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_ProviderManagerRouter_h
#define Pegasus_ProviderManagerRouter_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/Message.h>
#include <Pegasus/ProviderManager2/Linkage.h>

PEGASUS_NAMESPACE_BEGIN

class PEGASUS_PPM_LINKAGE ProviderManagerRouter
{
public:
    ProviderManagerRouter();
    virtual ~ProviderManagerRouter();

    /**
        Process the specified message by routing it to the appropriate
        ProviderManager.  Note: This operation may take a long time to
        complete and should be called on a non-critical thread.
     */
    virtual Message* processMessage(Message* message) = 0;

    /**
        Indicates whether any of the active ProviderManagers managed by this
        ProviderManagerRouter has an active (e.g., loaded/initialized)
        provider.  If no providers are active, this ProviderManagerRouter
        may be destructed without harm.
     */
    virtual Boolean hasActiveProviders() = 0;

    /**
        Unload idle providers in all active ProviderManagers.
        Note: This operation may take a long time to complete and should
        be called on a non-critical thread.
     */
    virtual void unloadIdleProviders() = 0;
};

PEGASUS_NAMESPACE_END

#endif
