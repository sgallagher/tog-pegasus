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


#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/System.h>
#include <Pegasus/Common/CIMObjectPath.h>
#include "IPRouteProvider.h"
#include "IPV4NetInformation.h"
#include "DeviceLocator.h"
#include "network_defines.h"

#include <iostream>

PEGASUS_USING_STD;

PEGASUS_NAMESPACE_BEGIN


LinuxIPRouteProvider::LinuxIPRouteProvider(void)
{
}

LinuxIPRouteProvider::~LinuxIPRouteProvider(void)
{
}


void
LinuxIPRouteProvider::getInstance(const OperationContext& context,
				  const CIMObjectPath& ref,
				  const Boolean includeQualifiers,
				  const Boolean includeClassOrigin,
				  const CIMPropertyList& propertyList,
				  InstanceResponseHandler& handler)
{
   Array<CIMKeyBinding> keys = ref.getKeyBindings();
   Uint32 i;
   const struct route_data* located_route;
   String in_addr, in_mask, in_hop, in_type;
 
 
   for (i = 0; i < keys.size(); i++)
   {
      if (keys[i].getName() == CIMName("IPDestinationAddress"))
      {
         in_addr.append(keys[i].getValue());
         break;
      }
   }
 
   for (i = 0; i < keys.size(); i++)
   {
      if (keys[i].getName() == CIMName("IPDestinationMask"))
      {
         in_mask.append(keys[i].getValue());
         break;
      }
   }
 
   for (i = 0; i < keys.size(); i++)
   {
      if (keys[i].getName() == CIMName("NextHop"))
      {
         in_hop.append(keys[i].getValue());
         break;
      }
   }
 
   for (i = 0; i < keys.size(); i++)
   {
      if (keys[i].getName() == CIMName("AddressType"))
      {
         in_type.append(keys[i].getValue());
         break;
      }
   }
 
   handler.processing();
 
   if (allroutes.empty())
      load_all_route_data();
 
   located_route = LocateRoute(in_addr,in_mask,in_hop,in_type);
   
   if (located_route != NULL)
   {
      CIMInstance instance = build_instance(IPROUTECLASSNAME, located_route);
      handler.deliver(instance);
   }
 
   handler.complete();
   return;
}


void
LinuxIPRouteProvider::enumerateInstances(
      				const OperationContext& context, 
				const CIMObjectPath& ref, 
				const Boolean includeQualifiers,
				const Boolean includeClassOrigin,
				const CIMPropertyList& propertyList,
				InstanceResponseHandler& handler)
{
   unsigned int i;

   // Load the routes fresh, in case they've changed.
   load_all_route_data();

   handler.processing();

   for (i = 0; i < allroutes.size(); i++)
      handler.deliver(build_instance(IPROUTECLASSNAME, &allroutes[i]));

   handler.complete();
}


void 
LinuxIPRouteProvider::enumerateInstanceNames(
      				const OperationContext& context,
			        const CIMObjectPath& ref,
			        ObjectPathResponseHandler& handler )
{
   unsigned int i;

   // always load the route data, it might have changed since the last time
   // we enumerated the routes.
   load_all_route_data();

   handler.processing();

   for (i = 0; i < allroutes.size(); i++)
   {
      handler.deliver(fill_reference(ref.getNameSpace(), 
	       			     IPROUTECLASSNAME,
				     &allroutes[i]));
   }

   handler.complete();
}


void
LinuxIPRouteProvider::modifyInstance(
      				const OperationContext& context,
			  	const CIMObjectPath& ref,
			  	const CIMInstance& instanceObject,
				const Boolean includeQualifiers,
			  	const CIMPropertyList& propertyList,
			  	ResponseHandler& handler )
{
   throw CIMNotSupportedException(IPROUTECLASSNAME"::modifyInstance");
}

void 
LinuxIPRouteProvider::createInstance(
      				const OperationContext& context,
			  	const CIMObjectPath& ref,
			  	const CIMInstance& instanceObject,
			  	ObjectPathResponseHandler& handler )
{
   throw CIMNotSupportedException(IPROUTECLASSNAME"::createInstance");
}

void 
LinuxIPRouteProvider::deleteInstance(const OperationContext& context,
			  	     const CIMObjectPath& ref,
			  	     ResponseHandler& handler )
{
   throw CIMNotSupportedException(IPROUTECLASSNAME"::deleteInstance");
}

void LinuxIPRouteProvider::initialize(CIMOMHandle& handle)
{
}


void LinuxIPRouteProvider::terminate(void)
{
   delete this;
}

CIMObjectPath 
LinuxIPRouteProvider::fill_reference(const CIMNamespaceName& nameSpace, 
				     const CIMName& className, 
				     struct route_data const* ptr)
{
   Array<CIMKeyBinding> keys;

   keys.append(CIMKeyBinding("CreationClassName", className.getString(),
                          CIMKeyBinding::STRING));

   keys.append(CIMKeyBinding("IPDestinationAddress", ptr->IPDestinationAddress,
                          CIMKeyBinding::STRING));
   keys.append(CIMKeyBinding("IPDestinationMask", ptr->IPDestinationMask,
                          CIMKeyBinding::STRING));
   keys.append(CIMKeyBinding("NextHop", ptr->NextHop,
                          CIMKeyBinding::STRING));
   keys.append(CIMKeyBinding("AddressType", ptr->AddressType,
                          CIMKeyBinding::NUMERIC));

   return CIMObjectPath(System::getHostName(), nameSpace, 
		       className, keys);
}

CIMInstance 
LinuxIPRouteProvider::build_instance(const CIMName& className, 
				     struct route_data const* ptr)
{
   CIMInstance instance(className);

#define ADD_TO_INSTANCE(x) instance.addProperty(CIMProperty(#x, ptr-> x))  

   ADD_TO_INSTANCE(IPDestinationAddress);
   ADD_TO_INSTANCE(IPDestinationMask);
   ADD_TO_INSTANCE(NextHop);
   ADD_TO_INSTANCE(AddressType);

#undef ADD_TO_INSTANCE

  return instance; 
}

void LinuxIPRouteProvider::load_all_route_data(void)
{
   vector<struct interface_to_address> if_addrs;
   struct interface_to_address this_if;
   struct route_data this_route;
   DeviceLocator *if_reader, *route_reader;
   IPV4IFInformation *next_device;
   IPV4RouteInformation *next_route;
   unsigned int i;
   char addr_buff[10];  /* Big enough to hold a single digit plus NUL */

   // First, look up all the interfaces, and get the associations between
   // interface names and addresses.
   if_addrs.clear();
   if_reader = new DeviceLocator(Device_IPV4NetworkSettings,
				 Device_IPV4NetworkSettings_Interfaces);

   while ((next_device = (IPV4IFInformation *)if_reader->getNextDevice())
	  != NULL)
   {
      this_if.interface_name = next_device->GetInterface();
      this_if.address = next_device->GetAddress();
      if_addrs.push_back(this_if);
   }

   // Now, load the routes
   allroutes.clear();
   route_reader = new DeviceLocator(Device_IPV4NetworkSettings,
				    Device_IPV4NetworkSettings_IPRoutes);

   while ((next_route = (IPV4RouteInformation *)route_reader->getNextDevice())
	  != NULL)
   {
      this_route.IPDestinationAddress = next_route->GetDestination();
      this_route.IPDestinationMask = next_route->GetMask();
      sprintf(addr_buff, "%u", CIM_IPRoute_NextHop_IPv4);
      this_route.AddressType= addr_buff;
      this_route.IsStatic = !next_route->GetRouteIsDynamic();

      this_route.NextHop = next_route->GetGateway();
      if (this_route.NextHop == "0.0.0.0")   // look up the interface
      {
         for (i = 0; i < if_addrs.size(); i++)
	 {
	    if (next_route->GetInterface() == if_addrs[i].interface_name)
	    {
	       this_route.NextHop = if_addrs[i].address;
	       break;
	    }
	 }

         if (i == if_addrs.size())
	    this_route.NextHop = "UnknownAddress";
      }
 
      allroutes.push_back(this_route);
   }
}


const struct route_data* 
LinuxIPRouteProvider::LocateRoute(String const& dest_sought, 
      				  String const& mask_sought, 
				  String const& hop_sought, 
				  String const& type_sought) const
{
   unsigned int i;

   for (i = 0; i < allroutes.size(); i++)
   {
      if (allroutes[i].IPDestinationAddress == dest_sought &&
	  allroutes[i].IPDestinationMask == mask_sought &&
	  allroutes[i].NextHop == hop_sought &&
	  allroutes[i].AddressType== type_sought)
         return &allroutes[i];
   }

   return NULL;
}


PEGASUS_NAMESPACE_END
