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

#include <Pegasus/Common/AsyncResponseHandler.h>

const Uint32 AsyncOpFlags::DELIVER     = 0x00000001;
const Uint32 AsyncOpFlags::RESERVE     = 0x00000002;
const Uint32 AsyncOpFlags::PROCESSING  = 0x00000004;
const Uint32 AsyncOpFlags::COMPLETE    = 0x00000008;

const Uint32 AsyncOpState::NORMAL      = 0x00000000;
const Uint32 AsyncOpState::PHASED      = 0x00000001;
const Uint32 AsyncOpState::PARTIAL     = 0x00000002;
const Uint32 AsyncOpState::TIMEOUT     = 0x00000004;
const Uint32 AsyncOpState::SINGLE      = 0x00000008;
const Uint32 AsyncOpState::MULTIPLE    = 0x00000010;
const Uint32 AsyncOpState::TOTAL       = 0x00000020;



template<class object_type>
AsyncResponseHandler<object_type>::AsyncResponseHandler(
                         const CIMProvider *provider = NULL, 
			 Uint32 phased = 0)
   :_owner(NULL), _provider(provider), _objects(), _phased(phased),
    _parents(true), _children(true)
    
{
   pegasus_gettimeofday(&_key);
}

template<class object_type>
AsyncResponseHandler<object_type>::~AsyncResponseHandler(void)
{
   _children.empty_list();
}


template<class object_type>
void AsyncResponseHandler<object_type>::deliver(const object_type & object)
{
   _objects.insert(0, object);
   _completed_values++;
   _notify_parents(AsyncOpFlags::DELIVER, (AsyncOpState::SINGLE | _phased));
   _completion_state |= AsyncOpFlags::DELIVER;
   pegasus_gettimeofday(&_last_update);
}

template<class object_type>
void AsyncResponseHandler<object_type>::deliver(const Array<object_type> & objects)
{
   _objects.appendArray(objects);
   _completed_values = _objects.size();
   _notify_parents(AsyncOpFlags::DELIVER, (AsyncOpState::MULTIPLE | _phased));
   _completion_state |= AsyncOpFlags::DELIVER;
   pegasus_gettimeofday(&_last_update);
} 

template<class object_type>
void AsyncResponseHandler<object_type>::reserve(Uint32 size)
{
   _objects.reserve(size);
   _total_values = size;
   _notify_parents(AsyncOpFlags::RESERVE, _phased);
   _completion_state |= AsyncOpFlags::RESERVE;
   pegasus_gettimeofday(&_last_update);
}

template<class object_type>
void AsyncResponseHandler<object_type>::processing(void)
{
   _notify_parents(AsyncOpFlags::PROCESSING, _phased);
   _completion_state |= AsyncOpFlags::PROCESSING;
   pegasus_gettimeofday(&_last_update);
}

template<class object_type>
void AsyncResponseHandler<object_type>::complete(void)
{
   _notify_parents(AsyncOpFlags::COMPLETE, _phased);
   _completion_state |= AsyncOpFlags::COMPLETE;
   pegasus_gettimeofday(&_last_update);
}

template<class object_type>
void 



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


