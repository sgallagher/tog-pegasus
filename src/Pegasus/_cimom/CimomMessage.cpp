//%///-*-c++-*-/////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001 BMC Software, Hewlett-Packard Company, IBM,
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
// Author: Mike Day (mdday@us.ibm.com)
//
// Modified By: 
//
//%/////////////////////////////////////////////////////////////////////////////

#include "CimomMessage.h"

PEGASUS_NAMESPACE_BEGIN

const Uint32 CIMOM_Q_ID = MessageQueue::getNextQueueId();

const Uint32 async_results::OK =                        0x00000000;
const Uint32 async_results::PARAMETER_ERROR =           0x00000001;
const Uint32 async_results::MODULE_ALREADY_REGISTERED = 0x00000002;
const Uint32 async_results::MODULE_NOT_FOUND =          0x00000003;
const Uint32 async_results::INTERNAL_ERROR =            0x00000004;


const Uint32 async_results::ASYNC_STARTED =             0x00000005;
const Uint32 async_results::ASYNC_PROCESSING =          0x00000006;
const Uint32 async_results::ASYNC_COMPLETE =            0x00000007;
const Uint32 async_results::ASYNC_CANCELLED =           0x00000008;
const Uint32 async_results::ASYNC_PAUSED =              0x00000009;
const Uint32 async_results::ASYNC_RESUMED =             0x0000000a;

const Uint32 async_results::CIM_SERVICE_STARTED =       0x0000000b;
const Uint32 async_results::CIM_SERVICE_STOPPED  =      0x0000000c;

const Uint32 async_results::CIM_SERVICE_PAUSED  =       0x0000000d;
const Uint32 async_results::CIM_SERVICE_RESUMED =       0x0000000e;
const Uint32 async_results::CIM_NAK =                   0x0000000f;

const Uint32 async_results::ASYNC_PHASE_COMPLETE =      0x00000010;
const Uint32 async_results::ASYNC_CHILD_COMPLETE =      0x00000011;
const Uint32 async_results::ASYNC_PHASE_STARTED =       0x00000012;
const Uint32 async_results::ASYNC_CHILD_STARTED =       0x00000013;


const Uint32 async_messages::HEARTBEAT =                0x00000000;
const Uint32 async_messages::REPLY =                    0x00000000;
const Uint32 async_messages::REGISTER_CIM_SERVICE =     0x00000001;
const Uint32 async_messages::DEREGISTER_CIM_SERVICE =   0x00000002;
const Uint32 async_messages::UPDATE_CIM_SERVICE =       0x00000003;
const Uint32 async_messages::IOCTL =                    0x00000004;
const Uint32 async_messages::CIMSERVICE_START =         0x00000005;
const Uint32 async_messages::CIMSERVICE_STOP =          0x00000006;
const Uint32 async_messages::CIMSERVICE_PAUSE =         0x00000007;
const Uint32 async_messages::CIMSERVICE_RESUME =        0x00000008;

const Uint32 async_messages::ASYNC_OP_START =           0x00000009;
const Uint32 async_messages::ASYNC_OP_RESULT =          0x0000000a;
const Uint32 async_messages::ASYNC_LEGACY_OP_START =    0x0000000b;
const Uint32 async_messages::ASYNC_LEGACY_OP_RESULT =   0x0000000c;

const Uint32 async_messages::FIND_SERVICE_Q =           0x0000000d;
const Uint32 async_messages::FIND_SERVICE_Q_RESULT =    0x0000000e;
const Uint32 async_messages::ENUMERATE_SERVICE =        0x0000000f;
const Uint32 async_messages::ENUMERATE_SERVICE_RESULT = 0x00000010;



PEGASUS_NAMESPACE_END
