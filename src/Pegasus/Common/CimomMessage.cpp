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

#include "CimomMessage.h"

PEGASUS_NAMESPACE_BEGIN

const Uint32 async_results::OK =                        0x00000000;
const Uint32 async_results::CIM_NAK =                   0x00000001;
const Uint32 async_results::CIM_SERVICE_STOPPED =       0x00000002;


AsyncMessage::AsyncMessage(
    MessageType type,
    Uint32 destination,
    Uint32 mask,
    AsyncOpNode* operation)
    : Message(type, destination, mask | MessageMask::ha_async),
      op(operation)
{
}

AsyncRequest::AsyncRequest(
    MessageType type,
    Uint32 mask,
    AsyncOpNode* operation,
    Uint32 destination,
    Uint32 response,
    Boolean blocking)
    : AsyncMessage(
          type, destination, mask | MessageMask::ha_request, operation),
      resp(response),
      block(blocking)
{
    if (op != 0)
        op->setRequest(this);
}

AsyncReply::AsyncReply(
    MessageType type,
    Uint32 mask,
    AsyncOpNode* operation,
    Uint32 resultCode,
    Uint32 destination,
    Boolean blocking)
    : AsyncMessage(
          type, destination, mask | MessageMask::ha_reply, operation),
      result(resultCode),
      block(blocking)
{
    if (op != 0)
        op->setResponse(this);
}

AsyncIoctl::AsyncIoctl(
    AsyncOpNode* operation,
    Uint32 destination,
    Uint32 response,
    Boolean blocking,
    Uint32 code,
    Uint32 intParam,
    void* pParam)
    : AsyncRequest(
          ASYNC_IOCTL,
          0, operation,
          destination, response, blocking),
      ctl(code),
      intp(intParam),
      voidp(pParam)
{
}

CimServiceStart::CimServiceStart(
    AsyncOpNode* operation,
    Uint32 destination,
    Uint32 response,
    Boolean blocking)
    : AsyncRequest(
          ASYNC_CIMSERVICE_START,
          0, operation, destination,
          response, blocking)
{
}


CimServiceStop::CimServiceStop(
    AsyncOpNode* operation,
    Uint32 destination,
    Uint32 response,
    Boolean blocking)
    : AsyncRequest(
          ASYNC_CIMSERVICE_STOP,
          0, operation, destination,
          response, blocking)
{
}

AsyncOperationStart::AsyncOperationStart(
    AsyncOpNode* operation,
    Uint32 destination,
    Uint32 response,
    Boolean blocking,
    Message* action)
    : AsyncRequest(
          ASYNC_ASYNC_OP_START,
          0,
          operation,
          destination, response, blocking),
      _act(action)
{
}

Message* AsyncOperationStart::get_action()
{
    Message* ret = _act;
    _act = 0;
    ret->put_async(0);
    return ret;
}

AsyncOperationResult::AsyncOperationResult(
    AsyncOpNode* operation,
    Uint32 resultCode,
    Uint32 destination,
    Boolean blocking)
    : AsyncReply(
          ASYNC_ASYNC_OP_RESULT,
          0,
          operation,
          resultCode,
          destination,
          blocking)
{
}


AsyncModuleOperationStart::AsyncModuleOperationStart(
    AsyncOpNode* operation,
    Uint32 destination,
    Uint32 response,
    Boolean blocking,
    const String& targetModule,
    Message* action)
    : AsyncRequest(
          ASYNC_ASYNC_MODULE_OP_START,
          0,
          operation,
          destination,
          response,
          blocking),
      _target_module(targetModule),
      _act(action)
{
    _act->put_async(this);
}


Message* AsyncModuleOperationStart::get_action()
{
    Message* ret = _act;
    _act = 0;
    ret->put_async(0);
    return ret;
}

AsyncModuleOperationResult::AsyncModuleOperationResult(
    AsyncOpNode* operation,
    Uint32 resultCode,
    Uint32 destination,
    Boolean blocking,
    const String& targetModule,
    Message* result)
    : AsyncReply(
          ASYNC_ASYNC_MODULE_OP_RESULT,
          0,
          operation, resultCode, destination,
          blocking),
      _targetModule(targetModule),
      _res(result)
{
    _res->put_async(this);
}

Message* AsyncModuleOperationResult::get_result()
{
    Message* ret = _res;
    _res = 0;
    ret->put_async(0);
    return ret;
}


AsyncLegacyOperationStart::AsyncLegacyOperationStart(
    AsyncOpNode* operation,
    Uint32 destination,
    Message* action)
    : AsyncRequest(
          ASYNC_ASYNC_LEGACY_OP_START,
          0,
          operation, destination, CIMOM_Q_ID, false),
      _act(action)
{
    _act->put_async(this);
}


Message* AsyncLegacyOperationStart::get_action()
{
    Message* ret = _act;
    _act = 0;
//    ret->put_async(0);
    return ret;

}

AsyncLegacyOperationResult::AsyncLegacyOperationResult(
    AsyncOpNode* operation,
    Message* result)
    : AsyncReply(
          ASYNC_ASYNC_LEGACY_OP_RESULT,
          0, operation,
          0, CIMOM_Q_ID, false),
      _res(result)
{
    _res->put_async(this);
}

Message* AsyncLegacyOperationResult::get_result()
{
    Message* ret = _res;
    _res = 0;
//    ret->put_async(0);
    return ret;
}

AsyncMessage::~AsyncMessage()
{
}

AsyncRequest::~AsyncRequest()
{
}

PEGASUS_NAMESPACE_END
