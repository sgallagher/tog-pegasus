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
// Author: Mike Day (mdday@us.ibm.com)
//
// Modified By: 
//
//%////////////////////////////////////////////////////////////////////////////


#ifndef Pegasus_reg_table_h
#define Pegasus_reg_table_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/IPC.h>
#include <Pegasus/Common/String.h>
#include <Pegasus/Common/ArrayInter.h>
#include <Pegasus/Common/HashTable.h>
#include <Pegasus/Common/System.h>
#include <Pegasus/Common/CIMName.h>
#include <Pegasus/Common/MessageQueueService.h>
#include <Pegasus/Server/Linkage.h>

PEGASUS_NAMESPACE_BEGIN

class reg_table_rep;

class PEGASUS_SERVER_LINKAGE DynamicRoutingTable  
{
   public:
      DynamicRoutingTable(void);
      DynamicRoutingTable(const DynamicRoutingTable & table);
      DynamicRoutingTable & operator =(const DynamicRoutingTable & table);
      ~DynamicRoutingTable(void);

      static const DynamicRoutingTable get_ro_routing_table(void);
      static DynamicRoutingTable get_rw_routing_table(void);
      
      // get a single service that can route this spec. 
      MessageQueueService *get_routing(const CIMName& classname, 
				       const CIMNamespaceName& ns, 
				       Uint32 type,
				       Uint32 flags) const;

      // get a single service that can route this spec. 
      MessageQueueService *get_routing(const CIMName& classname, 
				       const CIMNamespaceName& ns, 
				       Uint32 type,
				       Uint32 flags,
				       String & provider,
				       String & module) const;

      
      // get a single service that can route this extended spec. 
      MessageQueueService *get_routing(const CIMName& classname,
				       const CIMNamespaceName& ns,
				       Uint32 type,
				       const Array<Uint8>& extended_type,
				       Uint32 flags,
				       const Array<Uint8>& extended_flags) const ;

      // get a single service that can route this extended spec. 
      MessageQueueService *get_routing(const CIMName& classname,
				       const CIMNamespaceName& ns,
				       Uint32 type,
				       const Array<Uint8>& extended_type,
				       Uint32 flags,
				       const Array<Uint8>& extended_flags,
				       String & provider, 
				       String & module) const ;

      // get an array of services that can route this spec. 
      void get_routing(const CIMName& classname, 
		       const CIMNamespaceName& ns, 
		       Uint32 type,
		       Uint32 flags,
		       Array<MessageQueueService *>& results) const;
      
      // get an array of services that can route this extended spec. 
      void get_routing(const CIMName& classname,
		       const CIMNamespaceName& ns,
		       Uint32 type,
		       const Array<Uint8>& extended_type,
		       Uint32 flags,
		       const Array<Uint8>& extended_flags, 
		       Array<MessageQueueService *>& results) const ;
      
      Boolean insert_record(const CIMName& classname, 
			    const CIMNamespaceName& ns, 
			    Uint32 type,
			    Uint32 flags,
			    MessageQueueService* svce) ;
      
      Boolean insert_record(const CIMName& classname, 
			    const CIMNamespaceName& ns, 
			    Uint32 type,
			    const Array<Uint8>& extended_type,
			    Uint32 flags,
			    const Array<Uint8>& extended_flags, 
			    MessageQueueService* svce) ;

      Boolean insert_record(const CIMName& classname, 
			    const CIMNamespaceName& ns, 
			    Uint32 type,
			    Uint32 flags,
			    MessageQueueService* svce, 
			    const String& provider, 
			    const String& module) ;
      
      Boolean insert_record(const CIMName& classname, 
			    const CIMNamespaceName& ns, 
			    Uint32 type,
			    const Array<Uint8>& extended_type,
			    Uint32 flags,
			    const Array<Uint8>& extended_flags, 
			    MessageQueueService* svce,
			    const String& provider,
			    const String& module) ;

      MessageQueueService * remove_record(const CIMName& classname, 
					   const CIMNamespaceName& ns, 
					   Uint32 type,
					   Uint32 flags);
      
      MessageQueueService * remove_record(const CIMName& classname, 
					  const CIMNamespaceName& ns, 
					  Uint32 type,
					  const Array<Uint8>& extended_type,
					  Uint32 flags,
					  const Array<Uint8>& extended_flags);
      Uint32 remove_multiple_records(const CIMName& classname, 
				     const CIMNamespaceName& ns, 
				     Uint32 type,
				     Uint32 flags);
      
      Uint32 remove_multiple_records(const CIMName& classname, 
				     const CIMNamespaceName& ns, 
				     Uint32 type,
				     const Array<Uint8>& extended_type,
				     Uint32 flags,
				     const Array<Uint8>& extended_flags);
      Uint32 remove_router_records(const MessageQueueService* router);

      /**
	 Router "types". These are actually keys used in the routing table. 
	 A provider manager (or other service provider) that can route messages
	 must register as being of a certain type. These types classify the routers
	 according to function and also make queries faster. 
      
	 Most of the current types correspond to provider interfaces. However, some
	 are generic pegasus services, and others are specific services.

	 The extended type is an escape mechanism that allows unlimited expansion
	 of "types" in the future. 
      */

      static const Uint32 INTERNAL; // "control providers"
      static const Uint32 INSTANCE;
      static const Uint32 CLASS;
      static const Uint32 METHOD;
      static const Uint32 ASSOCIATION;
      static const Uint32 QUERY;
      static const Uint32 INDICATION;
      static const Uint32 CONSUMER;
      static const Uint32 SERVICE; // derived from MessageQueueService
      static const Uint32 AUTHORIZATION; // authorization plug-in
      static const Uint32 AUTHENTICATION; // authentication plug-in
      static const Uint32 ENCODE_HOOK;  // hook to process outbound messages
      static const Uint32 DECODE_HOOK; // hook to process incoming messages
      static const Uint32 EXTENDED;


      void dump_table(void);
      
   private:
      reg_table_rep *_rep;
};

PEGASUS_NAMESPACE_END
#endif // Pegasus_reg_table_h
