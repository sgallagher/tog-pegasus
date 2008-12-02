//%LICENSE////////////////////////////////////////////////////////////////
//
// Licensed to The Open Group (TOG) under one or more contributor license
// agreements.  Refer to the OpenPegasusNOTICE.txt file distributed with
// this work for additional information regarding copyright ownership.
// Each contributor licenses this file to you under the OpenPegasus Open
// Source License; you may not use this file except in compliance with the
// License.
//
// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation
// the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included
// in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
// OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
// IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
// CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
// TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
// SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
//
//////////////////////////////////////////////////////////////////////////
//
//%/////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_CimomMessage_h
#define Pegasus_CimomMessage_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/InternalException.h>
#include <Pegasus/Common/MessageQueue.h>
#include <Pegasus/Common/AsyncOpNode.h>
#include <Pegasus/Common/Array.h>
#include <Pegasus/Common/Linkage.h>

PEGASUS_NAMESPACE_BEGIN

//
// This identifier is the queue id for CIMOM queue. It is initialized in
// CimomMessage.cpp by calling MessageQueue::getNextQueueId(). Note that
// this value is passed in the constructor for the CIMOM queue.
//
extern const Uint32 CIMOM_Q_ID;

class PEGASUS_COMMON_LINKAGE async_results
{
public:
    static const Uint32 OK;
    static const Uint32 CIM_NAK;
    static const Uint32 CIM_SERVICE_STOPPED;
};

// Overloaded message types
#define ASYNC_HEARTBEAT DUMMY_MESSAGE
#define ASYNC_REPLY DUMMY_MESSAGE


class PEGASUS_COMMON_LINKAGE AsyncMessage : public Message
{
public:
    AsyncMessage(
        MessageType type,
        Uint32 destination,
        Uint32 mask,
        AsyncOpNode* operation);

    virtual ~AsyncMessage();

    AsyncOpNode* op;
};


class PEGASUS_COMMON_LINKAGE AsyncRequest : public AsyncMessage
{
public:
    AsyncRequest(
        MessageType type,
        Uint32 mask,
        AsyncOpNode* operation,
        Uint32 destination,
        Uint32 response,
        Boolean blocking);

    virtual ~AsyncRequest();

    Uint32 resp;
    Boolean block;
};

class PEGASUS_COMMON_LINKAGE AsyncReply : public AsyncMessage
{
public:
    AsyncReply(
        MessageType type,
        Uint32 mask,
        AsyncOpNode* operation,
        Uint32 resultCode,
        Uint32 destination,
        Boolean blocking);

    virtual ~AsyncReply() { }

    Uint32 result;
    Boolean block;
};

class PEGASUS_COMMON_LINKAGE AsyncIoClose : public AsyncRequest
{
public:
    AsyncIoClose(
        AsyncOpNode* operation,
        Uint32 destination,
        Uint32 response,
        Boolean blocking);

    virtual ~AsyncIoClose()
    {
    }
};

class PEGASUS_COMMON_LINKAGE CimServiceStart : public AsyncRequest
{
public:
    CimServiceStart(
        AsyncOpNode* operation,
        Uint32 destination,
        Uint32 response,
        Boolean blocking);

    virtual ~CimServiceStart()
    {
    }
};

class PEGASUS_COMMON_LINKAGE CimServiceStop : public AsyncRequest
{
public:
    CimServiceStop(
        AsyncOpNode* operation,
        Uint32 destination,
        Uint32 response,
        Boolean blocking);

    virtual ~CimServiceStop()
    {
    }
};

class PEGASUS_COMMON_LINKAGE CimProvidersStop : public AsyncRequest
{
public:
    CimProvidersStop(
        AsyncOpNode* operation,
        Uint32 destination,
        Uint32 response,
        Boolean blocking);

    virtual ~CimProvidersStop()
    {
    }
};

class PEGASUS_COMMON_LINKAGE AsyncOperationStart : public AsyncRequest
{
public:
    AsyncOperationStart(
        AsyncOpNode* operation,
        Uint32 destination,
        Uint32 response,
        Boolean blocking,
        Message* action);

    virtual ~AsyncOperationStart()
    {
        delete _act;
    }

    Message* get_action();

private:
    friend class MessageQueueService;
    friend class cimom;
    Message* _act;
};

class PEGASUS_COMMON_LINKAGE AsyncOperationResult : public AsyncReply
{
public:
    AsyncOperationResult(
        AsyncOpNode* operation,
        Uint32 resultCode,
        Uint32 destination,
        Boolean blocking);

    virtual ~AsyncOperationResult()
    {
    }
};


class PEGASUS_COMMON_LINKAGE AsyncModuleOperationStart : public AsyncRequest
{
public:
    AsyncModuleOperationStart(
        AsyncOpNode* operation,
        Uint32 destination,
        Uint32 response,
        Boolean blocking,
        const String& targetModule,
        Message* action);

    virtual ~AsyncModuleOperationStart()
    {
        delete _act;
    }

    Message* get_action();

private:
    friend class MessageQueueService;
    friend class cimom;
    friend class ModuleController;
    String _target_module;
    Message* _act;
};

class PEGASUS_COMMON_LINKAGE AsyncModuleOperationResult : public AsyncReply
{
public:
    AsyncModuleOperationResult(
        AsyncOpNode* operation,
        Uint32 resultCode,
        Uint32 destination,
        Boolean blocking,
        const String& targetModule,
        Message* action);

    virtual ~AsyncModuleOperationResult()
    {
        delete _res;
    }

    Message* get_result();

 private:
    friend class MessageQueueService;
    friend class cimom;
    friend class ModuleController;
    String _targetModule;
    Message* _res;
};

class PEGASUS_COMMON_LINKAGE AsyncLegacyOperationStart : public AsyncRequest
{
public:
    AsyncLegacyOperationStart(
        AsyncOpNode* operation,
        Uint32 destination,
        Message* action);

    virtual ~AsyncLegacyOperationStart()
    {
        delete _act;
    }

    Message* get_action();

private:
    friend class MessageQueueService;
    friend class cimom;
    Message* _act;
};

class PEGASUS_COMMON_LINKAGE AsyncLegacyOperationResult : public AsyncReply
{
public:
    AsyncLegacyOperationResult(
        AsyncOpNode* operation,
        Message* result);

    virtual ~AsyncLegacyOperationResult()
    {
        delete _res;
    }

    Message* get_result();

private:
    friend class MessageQueueService;
    friend class cimom;
    Message* _res;
};

class PEGASUS_COMMON_LINKAGE FindServiceQueue : public AsyncRequest
{
public:
    FindServiceQueue(
        AsyncOpNode* operation,
        Uint32 response,
        Boolean blocking,
        const String& serviceName,
        Uint32 serviceCapabilities,
        Uint32 serviceMask);

    virtual ~FindServiceQueue()
    {
    }

    String name;
    Uint32 capabilities;
    Uint32 mask;
};

class PEGASUS_COMMON_LINKAGE FindServiceQueueResult : public AsyncReply
{
public:
    FindServiceQueueResult(
        AsyncOpNode* operation,
        Uint32 resultCode,
        Uint32 destination,
        Boolean blocking,
        Array<Uint32> queueIds);

    virtual ~FindServiceQueueResult()
    {
    }

    Array<Uint32> qids;
};

class PEGASUS_COMMON_LINKAGE EnumerateService : public AsyncRequest
{
public:
    EnumerateService(
        AsyncOpNode* operation,
        Uint32 response,
        Boolean blocking,
        Uint32 queueId);

    virtual ~EnumerateService()
    {
    }

    Uint32 qid;
};

class PEGASUS_COMMON_LINKAGE EnumerateServiceResponse : public AsyncReply
{
public:
    EnumerateServiceResponse(
        AsyncOpNode* operation,
        Uint32 resultCode,
        Uint32 response,
        Boolean blocking,
        const String& serviceName,
        Uint32 serviceCapabilities,
        Uint32 serviceMask,
        Uint32 serviceQid);

    virtual ~EnumerateServiceResponse()
    {
    }

    String name;
    Uint32 capabilities;
    Uint32 mask;
    Uint32 qid;
};

PEGASUS_NAMESPACE_END

#endif // Pegasus_CimomMessage_h
