//%///////////-*-c++-*-//////////////////////////////////////////////////////
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
// Author: Mike Day (mdday@us.ibm.com)
//
// Modified By: 
//
//%////////////////////////////////////////////////////////////////////

#include <Pegasus/Common/ResponseHandler.h>
#include <Pegasus/Common/AsyncResponseHandler.h>

PEGASUS_NAMESPACE_BEGIN

PEGASUS_EXPORT void delete_rh(void *handler, int type) 
   throw(TypeMismatch)
{
   if(handler == NULL)
      return;
   switch(type)
   {
      case RESPONSE_HANDLER_TYPE_UNDEFINED:
	 break;
      case RESPONSE_HANDLER_TYPE_CIM_CLASS:
	 delete reinterpret_cast< AsyncResponseHandler<CIMClass> *>(handler);
	 break;
      case RESPONSE_HANDLER_TYPE_CIM_INSTANCE:
	 delete reinterpret_cast<AsyncResponseHandler<CIMInstance> *>(handler);
	 break;
      case RESPONSE_HANDLER_TYPE_CIM_OBJECT:
	 delete reinterpret_cast<AsyncResponseHandler<CIMObject> *>(handler);
	 break;
      case RESPONSE_HANDLER_TYPE_CIM_OBJECT_WITH_PATH:
	 delete reinterpret_cast<AsyncResponseHandler<CIMObjectWithPath> *>(handler);
	 break;
      case RESPONSE_HANDLER_TYPE_CIM_VALUE:
	 delete reinterpret_cast<AsyncResponseHandler<CIMValue> *>(handler);
	 break;
      case RESPONSE_HANDLER_TYPE_CIM_INDICATION:
	 delete reinterpret_cast<AsyncResponseHandler<CIMIndication> *>(handler);
	 break;
      case RESPONSE_HANDLER_TYPE_CIM_REFERENCE:
	 delete reinterpret_cast<AsyncResponseHandler<CIMReference> *>(handler);
      default:
	 throw TypeMismatch();
   }
}

PEGASUS_EXPORT void * create_rh(int type) 
  throw(TypeMismatch)
{
   void *ret = 0;
   switch(type)
   {
      case RESPONSE_HANDLER_TYPE_UNDEFINED:
	 break;
      case RESPONSE_HANDLER_TYPE_CIM_CLASS:
	 ret = (void *) new AsyncResponseHandler<CIMClass>(type);
	 break;	 
      case RESPONSE_HANDLER_TYPE_CIM_INSTANCE:
	 ret = (void *)new AsyncResponseHandler<CIMInstance>(type);
	 break;
      case RESPONSE_HANDLER_TYPE_CIM_OBJECT:
	 ret = (void *) new AsyncResponseHandler<CIMObject>(type);
	 break;
      case RESPONSE_HANDLER_TYPE_CIM_OBJECT_WITH_PATH:
	 ret = (void *) new AsyncResponseHandler<CIMObjectWithPath>(type);
	 break;
      case RESPONSE_HANDLER_TYPE_CIM_VALUE:
	 ret = (void *) new AsyncResponseHandler<CIMValue>(type);
	 break;
      case RESPONSE_HANDLER_TYPE_CIM_INDICATION:
	 ret = (void *) new AsyncResponseHandler<CIMIndication>(type);
	 break;
      case RESPONSE_HANDLER_TYPE_CIM_REFERENCE:
	 ret = (void *) new AsyncResponseHandler<CIMReference>(type);
      default:
	 throw TypeMismatch();
   }
   return ret;
}

template<class object_type>
void AsyncResponseHandler<object_type>::reserve(const Uint32 size) 
{
   _objects->reserve(size);
   // parent will destroy this op context
//   OperationContext *context = new OperationContext();
//   context->set_uint_val(size);
   // parent can inspect the context and set the total operations value
   //  _parent->notify(&_key, context, ASYNC_OPFLAGS_RESERVE, 
// 		   ASYNC_OPSTATE_NORMAL, _type);
}


template<class object_type>
void AsyncResponseHandler<object_type>::_clear(void)
{
   _parent = 0;
   _provider = 0;
   _thread = 0;
   if(_objects->getCapacity() > 3)
   {
      delete _objects;
      _objects = (Array<object_type> *) new Array<object_type>();
   }
   else
      _objects->clear();
}


PEGASUS_NAMESPACE_END

/****

Wed Oct 10 13:17:06 2001 Roger Kumpf - indications

subscription, processing

HP working on subscriptions.

one consumer may have multiple subscriptions, and needs to handle the subscription 
differently based on the particular subscription. 

subscriber will pass indication on to logical consumers. 

which parts is HP doing? 

intances of subscriptions will be stored in the repository to provide persistence. 
providers will need to store and retrieve subscriptions. 

outbound indications go to the indication dispatcher (processor). then
goes to the export client which then routes around to the export
server.

we will use cim instance instead of cim indication 

roger will look at how to hook a subscription into a provider
I take subscription and give to chip
I create the response handler, which will give it to the indication dispatcher. 

(indication, request, export server) 

easiest thing right now - 


// for this simple implementation we will pass the filter to the provider

   responsehandler.deliver(cimindication, array<subscription>) <- let the provider
"source route" the indication.

roger says cimom does not have enough information to take two indication instances
and know if they represent the same event. (the subscription, predicate, asyncresponsehandler
comprehend this. 

how do we know that a provider supports an indication class?

how does roger instantiate a subscription

we put a qualifier on each indication instance that maps to the indication provider
so we can dispatch to the appropriate provider.

how do we pass the subscription info to the provider. 


roger is the subscription provider. he will instantiate the subscription and then enqueue it to 
the dispatcher. I need to modify the dispatcher to handle the subscription. need the processor
   q id.

checkpoint after next 

****/


