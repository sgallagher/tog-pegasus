//%/////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001, 2002 BMC Software, Hewlett-Packard Company, IBM,
// The Open Group, Tivoli Systems
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

#ifndef Pegasus_CIMOMHandleInternal_h
#define Pegasus_CIMOMHandleInternal_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Provider/Linkage.h>
#include <Pegasus/Provider/CIMOMHandle.h>
#include <Pegasus/Common/ModuleController.h>

PEGASUS_NAMESPACE_BEGIN

class PEGASUS_PROVIDER_LINKAGE CIMOMHandleRep
{
public:
    CIMOMHandleRep() : id(peg_credential_types::PROVIDER) {}

    MessageQueueService * service;
    MessageQueueService * cimom;
    pegasus_internal_identity id;
    ModuleController * controller;
    ModuleController::client_handle * client_handle;
};

class PEGASUS_PROVIDER_LINKAGE CIMOMHandleInternal : public CIMOMHandle
{
public:
    CIMOMHandleInternal(MessageQueueService * service)
    {
        _rep->service = service;

        MessageQueue * queue =
            MessageQueue::lookup(PEGASUS_QUEUENAME_OPREQDISPATCHER);
        _rep->cimom = dynamic_cast<MessageQueueService *>(queue);
        _rep->controller = &(ModuleController::get_client_handle(_rep->id,
                                                   &_rep->client_handle));

        if((_rep->service == 0) || (_rep->cimom == 0) ||
           (_rep->client_handle == 0))
        {
            throw UninitializedObjectException();
        }
    }
};

PEGASUS_NAMESPACE_END

#endif
