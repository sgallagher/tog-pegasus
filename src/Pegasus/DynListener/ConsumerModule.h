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
// Author: Chip Vincent (cvincent@us.ibm.com)
//
// Modified By: Yi Zhou, Hewlett-Packard Company(yi_zhou@hp.com)
//              Mike Day, IBM (mdday@us.ibm.com)
//              Adrian Schuur, IBM (schuur@de.ibm.com)
//              Heather Sterling, IBM (hsterl@us.ibm.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_ConsumerModule_h
#define Pegasus_ConsumerModule_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/String.h>
#include <Pegasus/Common/System.h>
#include <Pegasus/Common/IPC.h>

#include <Pegasus/Provider/CIMProvider.h>
#include <Pegasus/Provider/CIMIndicationConsumerProvider.h>

#include <Pegasus/DynListener/Linkage.h>

PEGASUS_NAMESPACE_BEGIN

// The ConsumerModule class represents the physical module, as defined by the
// operation, that contains a provider. This class effectively encapsulates the
// "physical" portion of a consumer.
// 
// There can be multiple "logical" consumers in memory, but there is only one "physical" consumer.
// This class keeps track of how many logical consumers are using it.


class PEGASUS_DYNLISTENER_LINKAGE ConsumerModule
{
    friend class ConsumerManager;

public:
    virtual ~ConsumerModule(void);
    const String & getFileName(void) const;
    CIMIndicationConsumerProvider* load(const String & consumerName, const String & libraryPath);
    void unloadModule(void);

protected:
    String _libraryPath;
    AtomicInt _ref_count;
    DynamicLibraryHandle _libraryHandle;

private:
    ConsumerModule();
    const String & getConsumerName(void) const;
    virtual CIMIndicationConsumerProvider* getConsumer(void) const;

    String _consumerName;
    CIMIndicationConsumerProvider* _consumer;
};

inline const String & ConsumerModule::getFileName(void) const
{
    return(_libraryPath);
}

inline const String & ConsumerModule::getConsumerName(void) const
{
    return(_consumerName);
}

inline CIMIndicationConsumerProvider* ConsumerModule::getConsumer(void) const
{
    return(_consumer);
}

PEGASUS_NAMESPACE_END

#endif
