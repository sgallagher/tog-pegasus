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
#include <Pegasus/Common/Exception.h>
#include <Pegasus/Common/DQueue.h>
#include <Pegasus/Common/ResponseHandler.h>
#include <Pegasus/Common/Array.h>
#include <Pegasus/Common/CIMPredicate.h>

PEGASUS_NAMESPACE_BEGIN


/** 
    Derived Response Handler class specifically to enable asynchronous, multi-threaded
    CIM Providers. 

    To support CIM Operations that resolve to multiple providers, this class is designed
    to be linked into a tree or a list. Each specific provider invocation embedded in a 
    multi-provider CIM Operation causes the creation of a unique AsyncResponseHandler object. 
    
    For example, a CIM Get operation that asked for three instances from two providers would
    cause two AsyncResponseHandler object to be created, linked into a list as siblings, and 
    made children of a third AsyncResponseHandler:
    
    <pre>
    <code>
                              +-----------------------+
                              |Parent Response Handler|
			      +-----------------------+
			               |
			      ---------+
			      |
			      v
        +-----------------------------+    +----------------------------+
        |   Response Handler          |    | Response Handler           |
        |Get Instances from Provider A| -->|Get Instance from Provider B|
	+-----------------------------+    +----------------------------+

    </code>
    </pre>

    The example above shows a composite Response Handler, that is really two 
    distinct ResonseHandlers linked as siblings under the same parent. The CIMOM 
    Treats the Parent as a single operation that is complete when both providers have
    completed their discrete components of the composite. 
    
    Providers use the AsyncResponseHandler object as a callback mechanism when they 
    are fulfilling operations asynchronously. The CIMOM keeps a list of AsyncResponseHandler
    objects that represent pending or current CIM Operations.

    The AsyncResponseHandler object can also represent phased operations. That is, operations
    that can be streamed out to the client in pieces, before they are complete. Candidates for 
    phased operations include queries or other requests that collect hundreds or thousands of
    CIM Objects. Marshalling all such objects prior to transmitting them to the CIM Client may
    cause too much memory to be allocated by the CIMOM. The alternative is to stream the objects
    to the client one at a time, or in phases. 

*/
template<class object_type>
class PEGASUS_COMMON_LINKAGE AsyncResponseHandler : public ResponseHandler<object_type>
{
   public:
  
      AysncResponseHandler(CIMProvider *provider) ;
      ~AsyncResponseHandler(void) ;
      
      /** 
	  This method is the implementation of the virtual <code>deliver</code>
	  interface derived from the ResponseHandler class. 
      */
      virtual void deliver(const object_type & object);

      /** 
	  This method is the implementation of the virtual <code>deliver</code>
	  interface derived from the ResponseHandler class. 
      */
      virtual void deliver(const Array<object_type> & objects);
      /** 
	  This method is the implementation of the virtual <code>reserve</code>
	  interface derived from the ResponseHandler class. 
      */
      virtual void reserve(const Uint32 size);

      /** 
	  This method is the implementation of the virtual <code>processing</code>
	  interface derived from the ResponseHandler class. 
      */
      virtual void processing(void);

      /** 
	  This method is the implementation of the virtual <code>complete</code>
	  interface derived from the ResponseHandler class. 
      */
      virtual void complete(void);

   private:
      void _notify_parents(int notification, Uint32 state);
      void _notify_children(int notification, Uint32 state);
      void _set_thread(Thread *thread);
      Thread *_get_thread(void);
      

      // keep track of the thread running this operation so we can kill
      // it if necessary 
      Thread *_owner;
      CIMProvider *_provider;
      struct timeval _key;
      Uint32 _src_q;
      Uint32 _dest_q;
      Uint32 _rq_msg_key;
      Uint32 _rq_msg_type;
      Uint32 _rp_msg_key;
      Uint32 _rp_msg_type;

      Array<object_type> _objects;
      // this is a phased aggregate. when it is complete is will
      // be streamed to the client regardless of the state of 
      // siblings 
      Uint32 _phased;
      AtomicInt _total_values;
      AtomicInt _completed_values;
      AtomicInt_total_child_values;
      AtomicInt _completed_child_values;
      Uint32 _completion_state;
      struct timeval _last_update; 
      struct timeval _lifetime;

// these queues need to be untyped - i.e., a not a template.

      DQueue<AsyncResponseHandler<object_type>> _parents;
      // children may be phased or not phased
      DQueue<AsyncResponseHandler<object_type>> _children;
      // empty results that are filled by provider
      // array of predicates for events and 
      // stored queries (cursors)
      
};


/** 
    Represents a WQL Select clause, or any grammar construct that specifies
    a collection of CIM classes, instances, or properties.
*/
class PEGASUS_EXPORT CIMSelect
{
      CIMSelect(void);
      ~CIMSelect(void);
};


/** 
    Represents a parsed query, irrespective of the language. 
    The PredicateTree represents the filter, and the CIMSelect 
    represents the collection of properties, instances, or classes
    to retrieve through the filter. 
*/
class PEGASUS_COMMON_LINKAGE CIMQuery
{
   public:
      CIMQuery(void);
      virtual ~CIMQuery(void);
      virtual Boolean parse(void) = 0;
      virtual PredicateTree & getPredicateTree(void) = 0;
      virtual CIMSelect & getSelect(void) = 0;
};


// deliver this indication once and then end the subscription
#define SUB_ONESHOT                 0x00000001
// allow this subscription to live forever
#define SUB_REPEAT_FOREVER          0x00000002
// deliver this indication n times, then delete the subscription
#define SUB_REPEAT_N                0x00000004
// delete this subscription after the lifetime expires
#define SUB_REPEAT_UNTIL            0x00000008
// this is an active subscription
#define SUB_ACTIVE                  0x10000000
// this subscription has expired
#define SUB_EXPIRED                 0x20000000

/**
   Represents a subscription to a CIM Indication. In the CIM Schema a 
   subscription is an association between an indication handler
   and an indication filter.

   The CIMSubscription class aggregates the information contained in the
   CIM filter and indication handler classes and transforms that information 
   into the Pegasus internal classes and constructs that are necessary for 
   asynchronous, multi-threaded indication processing. 

   This class is stored by the AsyncResponseHandler<CIMIndication> class. 
   The creation of an indication subscription causes the following processing
   to be done by the CIMOM (simplified for clarity):

   <ol>
   <li>Pegasus creates an AsyncResponseHandler&lt;CIMIndication&gt; </li>
   <li>Pegasus create a CIMSubscription and links it to an internal list within
   the AsyncResponseHandler.</li>
   <li>Pegasus links the AsyncResponseHandler to the list of pending operations. These
   are CIM operations that are being processed asynchronously by one or more
   providers. 
   <li>Pegasus causes the appropriate provider to begin supplying the 
   indication.</li>
   <li>The provider delivers the indication to the AsyncResponseHandler.</li>
   <li>Pegasus gets the indication from the AsyncResponseHandler and then traverses
   the list of CIMSubscription objects stored by the AsyncResponseHandler.</li>
   <li>Each CIMSubscription object contains a PredicateTree, which is the compiled
   indication filter.</li>
   <li>Pegasus evaluates the subscription's PredicateTree. If true, Pegasus 
   delivers the indication to the Indication Handler by putting it to that 
   handler's AsyncResponseHandler.</li>
   </ol>
*/
class PEGASUS_EXPORT CIMSubscription
{

   public:
      CIMSubscription(void) { };
      ~CIMSubscription(void) { };

      /** 
	  Construct the subscription using the referenced filter and handler instances, 
	  along with the flags.
	  @param filter Reference to the CIM filter instance.
	  @param handler Reference to the CIM handler instance.
	  @param flags bit mask controlling the lifetime of the subscription
      */
      CIMSubscription(CIMReference filter, CIMReference handler, Uint32 flags);
      /** 
	  Copy constructor
      */
      CIMSubscription(const CIMSubscription& subscription);
      CIMSubscription& operator=(const CIMSubscription& x);

      /**
	 Identity operator.
      */
      Boolean operator==(const CIMSubscription& x) const;
      
      /**
	 Lock the subscription. 
	 @exception IPCException
      */
      void lock(void) throws IPCException;
      /**
	 Unlock the subscription
      */
      void unlock(void);

      /**
	 Subscription lifetime flags accessors.
      */
      void set_flags(Uint32 flags);
      Uint32 get_flags(void);

      /**
	 The flags variable is a hack. It is storage for opaque data that
	 means different things according to the value of the lifetime flags. 
      */
      void set_flags_var(Uint32 var);
      Uint32 get_flags_var(void);

      /**
	 Accessors for the filter property.
      */
      void set_filter(const CIMReference& filter);
      CIMReference& get_filter(void);
      
      /** 
	  Accessors for the handler propery.
      */
      void set_handler(const CIMReference& handler);
      CIMReference& get_handler(void);
      
      /**
	 Accessors for the class that defines the indication.
      */
      void set_class(const CIMReference& class);
      CIMReference& get_class(void);
      
      /** 
	  Accessors for the hysterisis intervals. 
      */
      void set_min_interval(const CIMDateTime& interval);
      void set_max_interval(const CIMDateTime& interval);
      
      CIMDateTime& get_min_interval(void);
      CIMDateTime& get_max_interval(void);
      
      /** 
	  Gets the time of the last time this indication was delivered
	  for this subscription. 
      */
      CIMDateTime& get_last(void);

      /** 
	  Gets the lifetime of this subscription. Will have a different
	  meaning depending upon the value of the lifetime flags. 
      */
      CIMDateTime& get_life(void);
      
      /**
	 Parses a CIM filter instance and creates a PredicateTree and possibly 
	 a CIMSelect object. Initializes the CIMSubscription with the resulting
	 PredicateTree and CIMSelect. 

	 @param filter CIMReference that specifies the filter instance that is 
	 to be parsed into a PredicateTree and CIMSelect.

	 @exception parseException
      */
      void set_predicate_and_select(CIMReference& filter) throws parseException;

      /**
	 Sets the subscription PredicateTree property by creating a 
	 new object using the copy constructor and initializing the 
	 subscription. 
	 
	 @param predicate Reference to a predicate object to be copied
      */
      void set_predicate(const PredicateTree *predicate);
      
      /**
	 Initializes the subscription's list of properties using 
	 an exisitng property array. 

	 The properties of a subscription determine which values will
	 get delivered with the indication. Another way of specifying 
	 the list of properties is using a WQL SELECT clause.
	 
	 @param properties Reference to a string array. Each element 
	 of the array refers to a property of the indication class that 
	 should be sent to the handler along with the indication.
      */
      void set_properties(const Array<String>& properties);

      /** 
	  Gets the subscription's property list. 
      */
      Array<String>& get_properties(void);
      

      /**
	 Sets the select member of the subscription. 
	 The select member represents a parsed WQL SELECT clause, 
	 which in turn initializes the subscription's property list 

	 @param sel Pointer to an existing CIMSelect object.
      */
      void set_select(const CIMSelect *sel);
                  
   private:
      
      Uint32 _flags;
      Uint32 _flag_var;
      
      CIMReference _sub_filter;  
      CIMReference _sub_handler;
      PredicateTree *_sub_predicate;
      CIMSelect *_sub_select;
      CIMReference _classReference;
      CIMDateTime _minimumInterval;
      CIMDateTime _maximumInterval;
      CIMDateTime _lastDelivery;
      CIMDateTime _lifetime;
      
      Array<String> _propertyList;
      
      // a specific subscription may receive different types of 
      // indications (depending upon the filter)
      // each indication, when received, will be evaluated against the 
      // predicate and either delivered (if true) or not
      // note also that different indications may be provided
      // by the same provider or by different providers

      // isolate the actual indication providers from the subscription 
      // objects using the indication response handler object.

      // use the dqueue's mutex as a write lock for the entire
      // object
      DQueue<AsyncResponseHandler<CIMIndication>> _providers;
};


/**
   Specialization of the template class AsyncResponseHandler for CIMIndication. 
   This is the response handler that will be passed to all asynchronous 
   indication providers. 
*/
template<>
class AsyncResponseHandler<CIMIndication> : public ResponseHandler<CIMIndication>
{
   public:
  
      AsyncResponseHandler<CIMIndication>(void) { } ;
      ~AsyncResponseHandler<CIMIndication>(void) { };

      AsyncResponseHandler<CIMIndication>(CIMIndicationProvider & provider)
      {
	 _provider = provider;
	 
      }
      
      virtual void deliver(const CIMIndication & object);
      virtual void deliver(const Array<CIMIndication> & objects);
      virtual void reserve(const Uint32 size);
      virtual void processing(void);
      virtual void complete(void);
      
   private:
      // don't keep track of the thread, because there may not be one
      // instead, keep track of the indication provider
      CIMIndicationProvider *_provider;
      // this response handler is shared among all subscriptions to 
      // a specific provider for the following specific indication class
      CIMReference _indicationClass;
      // some objects will be discarded or replaced many times 
      // before this object delivers the indication to the subscription object
      // in this case a linked list is more efficient than an array
      DQueue<CIMIndication> _indications;

      // when the subscriber list is empty this object should 
      // call the provider and cancel the indication and possibly 
      // unload the provider
      DQueue<CIMSubscription> _subscribers;

};

PEGASUS_NAMESPACE_END

#endif //Pegasus_AsyncResponse_h
