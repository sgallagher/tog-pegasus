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
// Author: Christopher Neufeld <neufeld@linuxcare.com>
//         David Kennedy       <dkennedy@linuxcare.com>
//
// Modified By: David Kennedy       <dkennedy@linuxcare.com>
//              Christopher Neufeld <neufeld@linuxcare.com>
//              Al Stone            <ahs3@fc.hp.com>
//
//%////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_NetworkAdapterProvider_h
#define Pegasus_NetworkAdapterProvider_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/Logger.h>
#include <Pegasus/Provider/CIMInstanceProvider.h>
#include <Pegasus/Provider/CIMMethodProvider.h>
#include <Pegasus/Common/OperationContext.h>
#include <Pegasus/Common/Array.h>
#include <Pegasus/Common/String.h>
#include <Pegasus/Common/CIMObjectPath.h>
#include <Pegasus/Common/CIMInstance.h>

#include "NetworkAdapterData.h"

#include <iostream>

PEGASUS_USING_STD;

PEGASUS_NAMESPACE_BEGIN

#define DEBUG(X) Logger::put(Logger::DEBUG_LOG, "Linux_NetworkAdapterProvider", Logger::INFORMATION, "$0", X)

/** The NetworkAdapter CIM class is usually used as a base class for real
 *  providers for Ethernet, token ring, and fibrechannel.  One exception
 *  would seem to be the local loopback interface, which is not described
 *  by any derived classes of the NetworkAdapter CIM class.  When a
 *  NetworkAdapter provider is created, it can be one of four different
 *  types: the three listed above, plus a generic "OTHER".  Currently, only
 *  the Ethernet provider type takes any special action, it extends the
 *  generic NetworkAdapter class with Ethernet-specific data.  All other
 *  types of provider return only the data from the NetworkAdapter base
 *  class.  A NetworkAdapter provider object returns only information about
 *  adapters of its own type.  This enumerated denotes the differnt types
 *  of providers. */
enum network_provider_types {
   NETWORK_ADAPTER_PROVIDER_ETHERNET, 
   NETWORK_ADAPTER_PROVIDER_TOKENRING,     // currently implemented as "OTHER"
   NETWORK_ADAPTER_PROVIDER_FIBRECHANNEL,  // currently implemented as "OTHER"
   NETWORK_ADAPTER_PROVIDER_OTHER,
   NETWORK_ADAPTER_PROVIDER_INVALID
};


class LinuxNetworkAdapterProvider : public CIMInstanceProvider
{
   public:

      LinuxNetworkAdapterProvider();
      ~LinuxNetworkAdapterProvider();

      void getInstance(const OperationContext& context,
		       const CIMObjectPath& ref,
		       const Boolean includeQualifiers,
		       const Boolean includeClassOrigin,
		       const CIMPropertyList& propertyList,
		       InstanceResponseHandler& handler );

      void enumerateInstances(const OperationContext& context,
			      const CIMObjectPath& ref,
			      const Boolean includeQualifiers,
			      const Boolean includeClassOrigin,
			      const CIMPropertyList& propertyList,
			      InstanceResponseHandler& handler );

      void enumerateInstanceNames(const OperationContext& context,
			          const CIMObjectPath &ref,
			          ObjectPathResponseHandler& handler );

      void modifyInstance(const OperationContext& context,
		          const CIMObjectPath& ref,
		          const CIMInstance& instanceObject,
			  const Boolean includeQualifiers,
		          const CIMPropertyList& propertyList,
		          ResponseHandler& handler );

      void createInstance(const OperationContext& context,
		          const CIMObjectPath& ref,
		          const CIMInstance& instanceObject,
		          ObjectPathResponseHandler& handler );

      void deleteInstance(const OperationContext& context,
		          const CIMObjectPath& ref,
		          ResponseHandler& handler );

      void initialize(CIMOMHandle& handle);
      void terminate(void);

   private:
       CIMObjectPath fill_reference(const CIMNamespaceName& nameSpace,
	     			   const CIMName &className,
			           NetworkAdapterData const* ptr);

       CIMInstance build_instance(const CIMName& className, 
                                  enum network_provider_types classType,
			          NetworkAdapterData const* ptr);

   private:
      /** Given a pointer to a NetworkAdapterData type, determines whether this
       *  provider is responsible for handling that type of Adapter.  */
      bool interface_is_my_type(enum network_provider_types classType,
                                NetworkAdapterData const* adapter) const;

      /** Given a unique identifier key, returns a new-ed pointer to a
       *  NetworkAdapterData object corresponding to that key.  They key used
       *  is the interface identification string (such as "eth0" under
       *  Linux). */
      NetworkAdapterData *LocateInterface(String const& name) const;
};


PEGASUS_NAMESPACE_END

#endif  
