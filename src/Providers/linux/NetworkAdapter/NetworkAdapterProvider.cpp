//%////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2002 BMC Software, Hewlett-Packard Company, IBM,
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
//=============================================================================
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
#include <Pegasus/Common/Exception.h>

#include "NetworkAdapterProvider.h"
#include "network_defines.h"

#include "EthernetAdapterData.h"

#include <iostream>

PEGASUS_USING_STD;

PEGASUS_NAMESPACE_BEGIN


LinuxNetworkAdapterProvider::LinuxNetworkAdapterProvider()
{
}

LinuxNetworkAdapterProvider::~LinuxNetworkAdapterProvider(void)
{
}


void
LinuxNetworkAdapterProvider::getInstance(const OperationContext& context,
					 const CIMObjectPath& ref,
					 const Uint32 flags,
					 const CIMPropertyList& propertyList,
					 ResponseHandler<CIMInstance>& handler)
{
   Array<KeyBinding> keys = ref.getKeyBindings();
   Uint32 i;
   NetworkAdapterData* located_interface;
   String unique_name;
   String className;
   enum network_provider_types classType;

   className = ref.getClassName();
   if (String::equalNoCase(className, "Linux_EthernetAdapter"))
   {
      classType = NETWORK_ADAPTER_PROVIDER_ETHERNET;
   }
   else if (String::equalNoCase(className, "Linux_NetworkAdapter"))
   {
      classType = NETWORK_ADAPTER_PROVIDER_OTHER;
   }
   else
   {
      throw NotSupported(className + "::getInstance");
   }
 
   for (i = 0; i < keys.size(); i++)
      if (keys[i].getName() == "Name")
         unique_name = keys[i].getValue();
 
   handler.processing();
 
   located_interface = LocateInterface(unique_name);
   
   if (located_interface != NULL && interface_is_my_type(classType,located_interface))
   {
      CIMInstance instance = build_instance(className, classType, located_interface);
      handler.deliver(instance);
   }
 
   delete located_interface;
 
   handler.complete();
   return;
}


void
LinuxNetworkAdapterProvider::enumerateInstances(
      				const OperationContext& context, 
				const CIMObjectPath& ref, 
				const Uint32 flags, 
				const CIMPropertyList& propertyList,
				ResponseHandler<CIMInstance>& handler)
{
   int i;
   NetworkAdapterData *iface;
   vector<String> adapter_names;
   String className;
   enum network_provider_types classType;

   className = ref.getClassName();
   if (String::equalNoCase(className, "Linux_EthernetAdapter"))
   {
      classType = NETWORK_ADAPTER_PROVIDER_ETHERNET;
   }
   else if (String::equalNoCase(className, "Linux_NetworkAdapter"))
   {
      classType = NETWORK_ADAPTER_PROVIDER_OTHER;
   }
   else
   {
      throw NotSupported(className + "::enumerateInstances");
   }

   adapter_names = NetworkAdapterData::list_all_adapters();

   handler.processing();

   for (i = 0; i < (int) adapter_names.size(); i++)
   {
      iface = LocateInterface(adapter_names[i]);
      if (iface != NULL && interface_is_my_type(classType, iface))
         handler.deliver(build_instance(className, classType, iface));

      delete iface;
   }

   handler.complete();
}


void
LinuxNetworkAdapterProvider::enumerateInstanceNames(
      				   const OperationContext& context,
				   const CIMObjectPath& ref,
				   ResponseHandler<CIMObjectPath>& handler )
{
   int i;
   vector<String> adapter_names;
   NetworkAdapterData *iface;
   String className;
   enum network_provider_types classType;

   className = ref.getClassName();
   if (String::equalNoCase(className, "Linux_EthernetAdapter"))
   {
      classType = NETWORK_ADAPTER_PROVIDER_ETHERNET;
   }
   else if (String::equalNoCase(className, "Linux_NetworkAdapter"))
   {
      classType = NETWORK_ADAPTER_PROVIDER_OTHER;
   }
   else
   {
      throw NotSupported(className + "::enumerateInstanceNames");
   }

   adapter_names = NetworkAdapterData::list_all_adapters();

   handler.processing();

   for (i = 0; i < (int) adapter_names.size(); i++)
   {
      iface = LocateInterface(adapter_names[i]);
      if (iface != NULL && interface_is_my_type(classType, iface))
         handler.deliver(fill_reference(ref.getNameSpace(), 
				        className, iface));
      delete iface;
   }

   handler.complete();
}


void 
LinuxNetworkAdapterProvider::modifyInstance(
      				const OperationContext& context,
			  	const CIMObjectPath& ref,
			  	const CIMInstance& instanceObject,
			  	const Uint32 flags,
			  	const CIMPropertyList& propertyList,
			  	ResponseHandler<CIMInstance>& handler )
{
   throw NotSupported(ref.getClassName() + "::modifyInstance");
}

void
LinuxNetworkAdapterProvider::createInstance(
      				const OperationContext& context,
			  	const CIMObjectPath& ref,
			  	const CIMInstance& instanceObject,
			  	ResponseHandler<CIMObjectPath>& handler )
{
   throw NotSupported(ref.getClassName() + "::createInstance");
}

void
LinuxNetworkAdapterProvider::deleteInstance(
      				const OperationContext& context,
			  	const CIMObjectPath& ref,
			  	ResponseHandler<CIMInstance>& handler )
{
   throw NotSupported(ref.getClassName() + "::deleteInstance");
}

void LinuxNetworkAdapterProvider::initialize(CIMOMHandle& handle)
{
}


void LinuxNetworkAdapterProvider::terminate(void)
{
}

CIMObjectPath
LinuxNetworkAdapterProvider::fill_reference(const String& nameSpace, 
					    const String& className, 
					    NetworkAdapterData const* ptr)
{
   Array<KeyBinding> keys;

   keys.append(KeyBinding("CreationClassName", className, KeyBinding::STRING));

   // LogicalDevice keys
   keys.append(KeyBinding("Name", ptr->GetName(),
                          KeyBinding::STRING));

   return CIMObjectPath(System::getHostName(), nameSpace, 
		       className, keys);
}


CIMInstance
LinuxNetworkAdapterProvider::build_instance(const String& className, 
                                            enum network_provider_types classType,
					    NetworkAdapterData const* ptr)
{
   EthernetAdapterData* eptr;
   CIMInstance instance(className);
 
   // this macro adds to the instance, and adds nothing if the function
   // throws AccessedInvalidData (which indicates unimplemented or
   // initialized fields)
#define ADD_TO_INSTANCE(x, y) do { try { instance.addProperty(CIMProperty(#y, (x)->Get ## y ())); } catch (AccessedInvalidData &e) {} } while (0)
 
 
   // LogicalDevice parameters
   ADD_TO_INSTANCE(ptr, Name);
   ADD_TO_INSTANCE(ptr, LogicalDeviceID);
   ADD_TO_INSTANCE(ptr, PowerManagementSupported);
   ADD_TO_INSTANCE(ptr, PowerManagementCapabilities);
   ADD_TO_INSTANCE(ptr, Availability);
   ADD_TO_INSTANCE(ptr, StatusInfo);
   ADD_TO_INSTANCE(ptr, LastErrorCode);
   ADD_TO_INSTANCE(ptr, ErrorDescription);
   ADD_TO_INSTANCE(ptr, ErrorCleared);
   ADD_TO_INSTANCE(ptr, OtherIdentifyingInfo);
   ADD_TO_INSTANCE(ptr, IdentifyingDescriptions);
   ADD_TO_INSTANCE(ptr, PowerOnHours);
   ADD_TO_INSTANCE(ptr, TotalPowerOnHours);
   ADD_TO_INSTANCE(ptr, AdditionalAvailability);
   ADD_TO_INSTANCE(ptr, MaxQuiesceTime);
 
   // NetworkAdapter parameters
   ADD_TO_INSTANCE(ptr, PermanentAddress);
   ADD_TO_INSTANCE(ptr, NetworkAddresses);
   ADD_TO_INSTANCE(ptr, Speed);
   ADD_TO_INSTANCE(ptr, MaxSpeed);
   ADD_TO_INSTANCE(ptr, FullDuplex);
   ADD_TO_INSTANCE(ptr, AutoSense);
   ADD_TO_INSTANCE(ptr, OctetsTransmitted);
   ADD_TO_INSTANCE(ptr, OctetsReceived);
 
   // Add ethernet data, if this is an ethernet adapter provider.  Other
   // adapter types currently unimplemented (but should follow this
   // framework)
   switch(classType) {
   case NETWORK_ADAPTER_PROVIDER_ETHERNET:
      if (ptr->data_type() != NETWORK_ADAPTER_ETHERNET)
      {
         cerr << "Serious error.  Bad flow in " << __FILE__ << endl;
         abort();
      }
 
      eptr = (EthernetAdapterData *) ptr;
 
      ADD_TO_INSTANCE(eptr, MaxDataSize);
      ADD_TO_INSTANCE(eptr, Capabilities);
      ADD_TO_INSTANCE(eptr, CapabilityDescriptions);
      ADD_TO_INSTANCE(eptr, EnabledCapabilities);
      ADD_TO_INSTANCE(eptr, SymbolErrors);
      ADD_TO_INSTANCE(eptr, TotalPacketsTransmitted);
      ADD_TO_INSTANCE(eptr, TotalPacketsReceived);
      ADD_TO_INSTANCE(eptr, AlignmentErrors);
      ADD_TO_INSTANCE(eptr, FCSErrors);
      ADD_TO_INSTANCE(eptr, SingleCollisionFrames);
      ADD_TO_INSTANCE(eptr, MultipleCollisionFrames);
      ADD_TO_INSTANCE(eptr, SQETestErrors);
      ADD_TO_INSTANCE(eptr, DeferredTransmissions);
      ADD_TO_INSTANCE(eptr, LateCollisions);
      ADD_TO_INSTANCE(eptr, ExcessiveCollisions);
      ADD_TO_INSTANCE(eptr, InternalMACTransmitErrors);
      ADD_TO_INSTANCE(eptr, InternalMACReceiveErrors);
      ADD_TO_INSTANCE(eptr, CarrierSenseErrors);
      ADD_TO_INSTANCE(eptr, FrameTooLongs);
 
      break;
   default:
      // do nothing
      break;
   }
 
   return instance; 
}


NetworkAdapterData*
LinuxNetworkAdapterProvider::LocateInterface(String const &name) const
{
   NetworkAdapterData* retval;
 
   retval = new NetworkAdapterData(name);
   if (retval->initialize() != 0)
   {
      delete retval;
      return NULL;
   }
 
   /* Once we have located the interface, we check to see if the adapter
    * type is covered by one of the derived classes.  If so, we delete the
    * located object and reload it from the appropriate derived class. */
   switch(retval->data_type()) {
   case NETWORK_ADAPTER_ETHERNET:
      delete retval;
      retval = new EthernetAdapterData(name);
      if (retval->initialize() != 0)
      {
         delete retval;
         return NULL;
      }
      break;
   case NETWORK_ADAPTER_LO:
   case NETWORK_ADAPTER_TOKENRING:
   case NETWORK_ADAPTER_FIBRECHANNEL:
      break;
   }
 
   return retval;
}


bool
LinuxNetworkAdapterProvider::interface_is_my_type(
                                    enum network_provider_types classType,
      				    NetworkAdapterData const *adapter) const
{
   enum network_provider_types must_match = NETWORK_ADAPTER_PROVIDER_INVALID;

   /* Sanity check on the data. */
   if (classType == NETWORK_ADAPTER_PROVIDER_INVALID ||
       adapter->data_type() == NETWORK_ADAPTER_INVALID)
      throw NullType();

   switch (adapter->data_type()) {
   case NETWORK_ADAPTER_ETHERNET:
      must_match = NETWORK_ADAPTER_PROVIDER_ETHERNET;
      break;
   case NETWORK_ADAPTER_TOKENRING:
   case NETWORK_ADAPTER_FIBRECHANNEL:
   case NETWORK_ADAPTER_LO:
      must_match = NETWORK_ADAPTER_PROVIDER_OTHER;
      break;
   }
  
   return (classType == must_match);
}


PEGASUS_NAMESPACE_END

