//%/////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001 The Open group, BMC Software, Tivoli Systems, IBM
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
// Author: Chip Vincent (cvincent@us.ibm.com)
//
// Modified By: Yi Zhou, Hewlett-Packard Company(yi_zhou@hp.com)
//
//
//%/////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_Provider_h
#define Pegasus_Provider_h

#include <Pegasus/Common/Config.h>

#include <Pegasus/ProviderManager/ProviderModule.h>
#include <Pegasus/ProviderManager/ProviderFacade.h>

PEGASUS_NAMESPACE_BEGIN

// The Provider class represents the logical provider extracted from a
// provider module. It is wrapped in a facade to stabalize the interface
// and is directly tied to a module.
class PEGASUS_SERVER_LINKAGE Provider : public ProviderFacade
{
public:
    Provider(const String & name, const String & path);
    virtual ~Provider(void);

    virtual void initialize(CIMOMHandle & cimom);
    virtual void terminate(void);
    virtual void terminateProvider(void);

    void unloadModule(void);
    String getName(void) const;

protected:
    ProviderModule _module;

};

PEGASUS_NAMESPACE_END

#endif
