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
//%/////////////////////////////////////////////////////////////////////////////


#ifndef Pegasus_AsyncResponse_h
#define Pegasus_AsyncResponse_h

#include <Pegasus/Common/IPC.h>
#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/Thread.h>
#include <Pegasus/Common/Exception.h>
#include <Pegasus/Common/DQueue.h>
#include <Pegasus/Common/ResponseHandler.h>
#include <Pegasus/Common/Array.h>
#include <Pegasus/Common/AsyncOpNode.h>
#include <Pegasus/Common/OperationContext.h>
#include <Pegasus/Provider2/CIMBaseProviderHandle.h>


PEGASUS_NAMESPACE_BEGIN
template<class object_type>
class PEGASUS_COMMON_LINKAGE AsyncResponseHandler : public ResponseHandler<object_type>
{
   public:
  
      AsyncResponseHandler(ResponseHandlerType type) ;

      ~AsyncResponseHandler(void) ;
      
      virtual void deliver(const object_type & object) ;

      /** ATTN:
      */
      virtual void deliver(const Array<object_type> & objects) ;

      /** ATTN:
      */
      virtual void reserve(const Uint32 size) ;

      /** ATTN:
      */
      virtual void processing(void) ;
      
      virtual void processing(OperationContext *context) ;
      
      /** ATTN:
      */
      virtual void complete(void) ;
      
      virtual void complete(OperationContext *context) ;

      void set_thread(Thread *thread);

      void set_parent(AsyncOpNode *parent);
      
      void set_provider(CIMBaseProviderHandle *provider);


      virtual Boolean operator == (const void *key) const;
      virtual Boolean operator == (ResponseHandlerType type) const;
      virtual Boolean operator == (const AsyncResponseHandler & rh) const;
      

   private:

      AsyncResponseHandler(void);
      AsyncOpNode *_parent;
      CIMBaseProviderHandle *_provider;
      // to gain access to the Thread object's utility routines 
      Thread *_thread;
      Array<object_type> *_objects;
      ResponseHandlerType _type;
      struct timeval _key;
      void _clear(void);
      friend class AsyncOpNode;
      
};


template<class object_type>
inline void AsyncResponseHandler<object_type>::deliver(const object_type & object)
{
   _objects->append(object);
   _parent->notify(&_key,  NULL, AsyncOpFlags::DELIVER, 
		   AsyncOpState::SINGLE | AsyncOpState::NORMAL , _type);
}

template<class object_type>
inline void AsyncResponseHandler<object_type>::deliver(const Array<object_type> & objects) 
{
   _objects->appendArray(objects);
   _parent->notify(&_key,  NULL, AsyncOpFlags::DELIVER, 
		   AsyncOpState::MULTIPLE | AsyncOpState::NORMAL , _type);
}


template<class object_type>
inline void AsyncResponseHandler<object_type>::processing(void) 
{
   _parent->notify(&_key, NULL, AsyncOpFlags::PROCESSING, 
		   AsyncOpState::NORMAL, _type);
}

template<class object_type>
inline void AsyncResponseHandler<object_type>::processing(OperationContext *context) 
{
   _parent->notify(&_key, context, AsyncOpFlags::PROCESSING, 
		   AsyncOpState::NORMAL, _type);
}

template<class object_type>
inline void AsyncResponseHandler<object_type>::complete(void) 
{
   _parent->notify(&_key, NULL, AsyncOpFlags::COMPLETE, 
		   AsyncOpState::NORMAL, _type);
}

template<class object_type>
inline void AsyncResponseHandler<object_type>::complete(OperationContext *context) 
{
   _parent->notify(&_key, context, AsyncOpFlags::COMPLETE,
		   AsyncOpState::NORMAL, _type);
}

template<class object_type>
inline void AsyncResponseHandler<object_type>::set_thread(Thread *thread)
{
   _thread = thread;
   
}

template<class object_type>
inline void AsyncResponseHandler<object_type>::set_parent(AsyncOpNode *parent)
{
   _parent = parent;
}

template<class object_type>
inline void AsyncResponseHandler<object_type>::set_provider(CIMBaseProviderHandle *provider)
{
   _provider = provider;
}


template<class object_type>
inline Boolean AsyncResponseHandler<object_type>::operator == (const void *key) const
{
   if( ! memcmp(&_key, key, sizeof(struct timeval)))
      return true;
   return false;
}

template<class object_type>
inline Boolean AsyncResponseHandler<object_type>::operator == (ResponseHandlerType type) const
{
   if(_type == type)
      return true;
   return false;
}

template<class object_type>
inline Boolean AsyncResponseHandler<object_type>::operator == (
   const AsyncResponseHandler<object_type> & rh) const
{
   return(this->operator ==((void *)&(rh._key)));
}



// /** 
//     Represents a WQL Select clause, or any grammar construct that specifies
//     a collection of CIM classes, instances, or properties.
// */
// class PEGASUS_EXPORT CIMSelect
// {
//       CIMSelect(void);
//       ~CIMSelect(void);
// };


// /** 
//     Represents a parsed query, irrespective of the language. 
//     The PredicateTree represents the filter, and the CIMSelect 
//     represents the collection of properties, instances, or classes
//     to retrieve through the filter. 
// */
// class PEGASUS_COMMON_LINKAGE CIMQuery
// {
//    public:
//       CIMQuery(void);
//       virtual ~CIMQuery(void);
//       virtual Boolean parse(void) = 0;
//       virtual PredicateTree & getPredicateTree(void) = 0;
//       virtual CIMSelect & getSelect(void) = 0;
// };


// // deliver this indication once and then end the subscription
// #define SUB_ONESHOT                 0x00000001
// // allow this subscription to live forever
// #define SUB_REPEAT_FOREVER          0x00000002
// // deliver this indication n times, then delete the subscription
// #define SUB_REPEAT_N                0x00000004
// // delete this subscription after the lifetime expires
// #define SUB_REPEAT_UNTIL            0x00000008
// // this is an active subscription
// #define SUB_ACTIVE                  0x10000000
// // this subscription has expired
// #define SUB_EXPIRED                 0x20000000

// /**
//    Represents a subscription to a CIM Indication. In the CIM Schema a 
//    subscription is an association between an indication handler
//    and an indication filter.

//    The CIMSubscription class aggregates the information contained in the
//    CIM filter and indication handler classes and transforms that information 
//    into the Pegasus internal classes and constructs that are necessary for 
//    asynchronous, multi-threaded indication processing. 

//    This class is stored by the AsyncResponseHandler<CIMIndication> class. 
//    The creation of an indication subscription causes the following processing
//    to be done by the CIMOM (simplified for clarity):

//    <ol>
//    <li>Pegasus creates an AsyncResponseHandler&lt;CIMIndication&gt; </li>
//    <li>Pegasus create a CIMSubscription and links it to an internal list within
//    the AsyncResponseHandler.</li>
//    <li>Pegasus links the AsyncResponseHandler to the list of pending operations. These
//    are CIM operations that are being processed asynchronously by one or more
//    providers. 
//    <li>Pegasus causes the appropriate provider to begin supplying the 
//    indication.</li>
//    <li>The provider delivers the indication to the AsyncResponseHandler.</li>
//    <li>Pegasus gets the indication from the AsyncResponseHandler and then traverses
//    the list of CIMSubscription objects stored by the AsyncResponseHandler.</li>
//    <li>Each CIMSubscription object contains a PredicateTree, which is the compiled
//    indication filter.</li>
//    <li>Pegasus evaluates the subscription's PredicateTree. If true, Pegasus 
//    delivers the indication to the Indication Handler by putting it to that 
//    handler's AsyncResponseHandler.</li>
//    </ol>
// */
// class PEGASUS_EXPORT CIMSubscription
// {

//    public:
//       CIMSubscription(void) { };
//       ~CIMSubscription(void) { };

//       /** 
// 	  Construct the subscription using the referenced filter and handler instances, 
// 	  along with the flags.
// 	  @param filter Reference to the CIM filter instance.
// 	  @param handler Reference to the CIM handler instance.
// 	  @param flags bit mask controlling the lifetime of the subscription
//       */
//       CIMSubscription(CIMReference filter, CIMReference handler, Uint32 flags);
//       /** 
// 	  Copy constructor
//       */
//       CIMSubscription(const CIMSubscription& subscription);
//       CIMSubscription& operator=(const CIMSubscription& x);

//       /**
// 	 Identity operator.
//       */
//       Boolean operator==(const CIMSubscription& x) const;
      
//       /**
// 	 Lock the subscription. 
// 	 @exception IPCException
//       */
//       void lock(void) throws IPCException;
//       /**
// 	 Unlock the subscription
//       */
//       void unlock(void);

//       /**
// 	 Subscription lifetime flags accessors.
//       */
//       void set_flags(Uint32 flags);
//       Uint32 get_flags(void);

//       /**
// 	 The flags variable is a hack. It is storage for opaque data that
// 	 means different things according to the value of the lifetime flags. 
//       */
//       void set_flags_var(Uint32 var);
//       Uint32 get_flags_var(void);

//       /**
// 	 Accessors for the filter property.
//       */
//       void set_filter(const CIMReference& filter);
//       CIMReference& get_filter(void);
      
//       /** 
// 	  Accessors for the handler propery.
//       */
//       void set_handler(const CIMReference& handler);
//       CIMReference& get_handler(void);
      
//       /**
// 	 Accessors for the class that defines the indication.
//       */
//       void set_class(const CIMReference& class);
//       CIMReference& get_class(void);
      
//       /** 
// 	  Accessors for the hysterisis intervals. 
//       */
//       void set_min_interval(const CIMDateTime& interval);
//       void set_max_interval(const CIMDateTime& interval);
      
//       CIMDateTime& get_min_interval(void);
//       CIMDateTime& get_max_interval(void);
      
//       /** 
// 	  Gets the time of the last time this indication was delivered
// 	  for this subscription. 
//       */
//       CIMDateTime& get_last(void);

//       /** 
// 	  Gets the lifetime of this subscription. Will have a different
// 	  meaning depending upon the value of the lifetime flags. 
//       */
//       CIMDateTime& get_life(void);
      
//       /**
// 	 Parses a CIM filter instance and creates a PredicateTree and possibly 
// 	 a CIMSelect object. Initializes the CIMSubscription with the resulting
// 	 PredicateTree and CIMSelect. 

// 	 @param filter CIMReference that specifies the filter instance that is 
// 	 to be parsed into a PredicateTree and CIMSelect.

// 	 @exception parseException
//       */
//       void set_predicate_and_select(CIMReference& filter) throws parseException;

//       /**
// 	 Sets the subscription PredicateTree property by creating a 
// 	 new object using the copy constructor and initializing the 
// 	 subscription. 
	 
// 	 @param predicate Reference to a predicate object to be copied
//       */
//       void set_predicate(const PredicateTree *predicate);
      
//       /**
// 	 Initializes the subscription's list of properties using 
// 	 an exisitng property array. 

// 	 The properties of a subscription determine which values will
// 	 get delivered with the indication. Another way of specifying 
// 	 the list of properties is using a WQL SELECT clause.
	 
// 	 @param properties Reference to a string array. Each element 
// 	 of the array refers to a property of the indication class that 
// 	 should be sent to the handler along with the indication.
//       */
//       void set_properties(const Array<String>& properties);

//       /** 
// 	  Gets the subscription's property list. 
//       */
//       Array<String>& get_properties(void);
      

//       /**
// 	 Sets the select member of the subscription. 
// 	 The select member represents a parsed WQL SELECT clause, 
// 	 which in turn initializes the subscription's property list 

// 	 @param sel Pointer to an existing CIMSelect object.
//       */
//       void set_select(const CIMSelect *sel);
                  
//    private:
      
//       Uint32 _flags;
//       Uint32 _flag_var;
      
//       CIMReference _sub_filter;  
//       CIMReference _sub_handler;
//       PredicateTree *_sub_predicate;
//       CIMSelect *_sub_select;
//       CIMReference _classReference;
//       CIMDateTime _minimumInterval;
//       CIMDateTime _maximumInterval;
//       CIMDateTime _lastDelivery;
//       CIMDateTime _lifetime;
      
//       Array<String> _propertyList;
      
//       // a specific subscription may receive different types of 
//       // indications (depending upon the filter)
//       // each indication, when received, will be evaluated against the 
//       // predicate and either delivered (if true) or not
//       // note also that different indications may be provided
//       // by the same provider or by different providers

//       // isolate the actual indication providers from the subscription 
//       // objects using the indication response handler object.

//       // use the dqueue's mutex as a write lock for the entire
//       // object
//       DQueue<AsyncResponseHandler<CIMIndication>> _providers;
// };


// /**
//    Specialization of the template class AsyncResponseHandler for CIMIndication. 
//    This is the response handler that will be passed to all asynchronous 
//    indication providers. 
// */
// template<>
// class AsyncResponseHandler<CIMIndication> : public ResponseHandler<CIMIndication>
// {
//    public:
  
//       AsyncResponseHandler<CIMIndication>(void) { } ;
//       ~AsyncResponseHandler<CIMIndication>(void) { };

//       AsyncResponseHandler<CIMIndication>(CIMIndicationProvider & provider)
//       {
// 	 _provider = provider;
	 
//       }
      
//       virtual void deliver(const CIMIndication & object);
//       virtual void deliver(const Array<CIMIndication> & objects);
//       virtual void reserve(const Uint32 size);
//       virtual void processing(void);
//       virtual void complete(void);
      
//    private:
//       // don't keep track of the thread, because there may not be one
//       // instead, keep track of the indication provider
//       CIMIndicationProvider *_provider;
//       // this response handler is shared among all subscriptions to 
//       // a specific provider for the following specific indication class
//       CIMReference _indicationClass;
//       // some objects will be discarded or replaced many times 
//       // before this object delivers the indication to the subscription object
//       // in this case a linked list is more efficient than an array
//       DQueue<CIMIndication> _indications;

//       // when the subscriber list is empty this object should 
//       // call the provider and cancel the indication and possibly 
//       // unload the provider
//       DQueue<CIMSubscription> _subscribers;

// };

PEGASUS_NAMESPACE_END

#endif //Pegasus_AsyncResponse_h
