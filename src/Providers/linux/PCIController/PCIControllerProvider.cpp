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
//              Josephine Eskaline Joyce (jojustin@in.ibm.com) for PEP#101
//
//%////////////////////////////////////////////////////////////////////////////


#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/System.h>
#include <Pegasus/Common/Logger.h>
#include <Pegasus/Common/CIMObjectPath.h>
#include <Pegasus/Common/Exception.h>
#include <Pegasus/Common/AutoPtr.h>

#include "PCIControllerProvider.h"
#include "PCIControllerData.h"


PEGASUS_USING_STD;

PEGASUS_NAMESPACE_BEGIN

#define DEBUG(X) Logger::put(Logger::DEBUG_LOG, "Linux_PCIControllerProvider", Logger::INFORMATION, "$0", X)

void
LinuxPCIControllerProvider::getInstance(const OperationContext& context,
					const CIMObjectPath& ref,
					const Boolean includeQualifiers,
					const Boolean includeClassOrigin,
					const CIMPropertyList& propertyList,
					InstanceResponseHandler& handler)
{
   AutoPtr<PCIControllerData> dptr1; 
   PCIControllerData *dptr2;
   Array<CIMKeyBinding> keys = ref.getKeyBindings();
   Uint32 i;
   String uniqueKeyID;
 
 
   /* Get the unique ID of the controller that was requested */
   for (i = 0; i < keys.size(); i++)
      if(keys[i].getName() == CIMName("DeviceID"))
         uniqueKeyID = keys[i].getValue();
 
   if (i == keys.size())
      return;   // didn't find the key
 
   handler.processing();
 
   dptr1.reset(new PCIControllerData);
 
   DEBUG("lpcp-> built PCIControllerData");

   while (dptr1.get() != NULL)
   {
      if (dptr1->GetLogicalDeviceID() == uniqueKeyID)
      {
         CIMInstance instance = build_instance(classname, dptr1.get());
         handler.deliver(instance);

	 DEBUG("lpcp-> delivered instance");
         dptr1.reset();
         break;
      }
 
      dptr2 = dptr1->GetNext();
      dptr1.reset(dptr2);
   }
 
   handler.complete();
   return;
}


void 
LinuxPCIControllerProvider::enumerateInstances(
      				const OperationContext& context, 
				const CIMObjectPath& ref, 
				const Boolean includeQualifiers,
				const Boolean includeClassOrigin,
				const CIMPropertyList& propertyList,
				InstanceResponseHandler& handler )
{
   AutoPtr<PCIControllerData> dptr1; 
   PCIControllerData *dptr2;

   handler.processing();
  
   dptr1.reset(new PCIControllerData);

   while (dptr1.get() != NULL)
   {
      handler.deliver(build_instance(classname, dptr1.get()));
      dptr2 = dptr1->GetNext();
      dptr1.reset(dptr2);
   }
   
   handler.complete();

}

void
LinuxPCIControllerProvider::enumerateInstanceNames(
      				const OperationContext& context,
			  	const CIMObjectPath& ref,
			  	ObjectPathResponseHandler& handler )
{
   PCIControllerData *dptr2;
   AutoPtr<PCIControllerData> dptr1; 

   handler.processing();
  
   dptr1.reset(new PCIControllerData);
   dptr1->initialize();

   while (dptr1.get() != NULL)
   {
      handler.deliver(fill_reference(ref.getNameSpace(), classname, dptr1.get()));
      dptr2 = dptr1->GetNext();
      dptr1.reset(dptr2);
   }
 
   handler.complete();
}


void 
LinuxPCIControllerProvider::modifyInstance(
      				const OperationContext& context,
			  	const CIMObjectPath& ref,
			  	const CIMInstance& instanceObject,
				const Boolean includeQualifiers,
			  	const CIMPropertyList& propertyList,
			  	ResponseHandler& handler )
{
   throw CIMNotSupportedException(classname + "::modifyInstance");
}

void
LinuxPCIControllerProvider::createInstance(
      				const OperationContext& context,
			  	const CIMObjectPath& ref,
			  	const CIMInstance& instanceObject,
			  	ObjectPathResponseHandler& handler )
{
   throw CIMNotSupportedException(classname + "::createInstance");
}

void
LinuxPCIControllerProvider::deleteInstance(
      				const OperationContext& context,
			  	const CIMObjectPath& ref,
			  	ResponseHandler& handler )
{
   throw CIMNotSupportedException(classname + "::deleteInstance");
}

void LinuxPCIControllerProvider::initialize(CIMOMHandle& handle)
{
}


void LinuxPCIControllerProvider::terminate(void)
{
   delete this;
}


CIMObjectPath
LinuxPCIControllerProvider::fill_reference(const CIMNamespaceName& nameSpace,
					   const CIMName& className, 
					   PCIControllerData const* ptr)
{
   Array<CIMKeyBinding> keys;


   keys.append(CIMKeyBinding("CreationClassName", "PCIController",
			  CIMKeyBinding::STRING));
   keys.append(CIMKeyBinding("DeviceID", ptr->GetLogicalDeviceID(),
			  CIMKeyBinding::STRING));

   return CIMObjectPath(System::getHostName(), nameSpace, 
		       className, keys);
}


CIMInstance 
LinuxPCIControllerProvider::build_instance(const CIMName& className, 
					  PCIControllerData const* ptr)
{
   CIMInstance instance(className);
 
#define ADD_TO_INSTANCE(x) do { try { instance.addProperty(CIMProperty(#x, ptr->Get ## x())); } catch (AccessedInvalidData &e) { } } while (0)
 
   DEBUG("lpcp-> adding to the instance (logical device fields)");

   // Logical device fields:
   ADD_TO_INSTANCE(LogicalDeviceID);
   ADD_TO_INSTANCE(PowerManagementSupported);
   ADD_TO_INSTANCE(PowerManagementCapabilities);
   ADD_TO_INSTANCE(Availability);
   ADD_TO_INSTANCE(StatusInfo);
   ADD_TO_INSTANCE(LastErrorCode);
   ADD_TO_INSTANCE(ErrorDescription);
   ADD_TO_INSTANCE(ErrorCleared);
   ADD_TO_INSTANCE(OtherIdentifyingInfo);
   ADD_TO_INSTANCE(IdentifyingDescriptions);
   ADD_TO_INSTANCE(PowerOnHours);
   ADD_TO_INSTANCE(TotalPowerOnHours);
   ADD_TO_INSTANCE(AdditionalAvailability);
   ADD_TO_INSTANCE(MaxQuiesceTime);
 
   DEBUG("lpcp-> adding to the instance (controller device fields)");
   // Controller device fields
   ADD_TO_INSTANCE(TimeOfLastReset);
   ADD_TO_INSTANCE(ProtocolSupported);
   ADD_TO_INSTANCE(MaxNumberControlled);
   ADD_TO_INSTANCE(ProtocolDescription);
   
   DEBUG("lpcp-> adding to the instance (PCI controller device fields)");
   // PCI Controller device fields
   ADD_TO_INSTANCE(ClassCode);
   ADD_TO_INSTANCE(Capabilities);
   ADD_TO_INSTANCE(CapabilityDescriptions);
   ADD_TO_INSTANCE(DeviceSelectTiming);
   ADD_TO_INSTANCE(CacheLineSize);
   ADD_TO_INSTANCE(LatencyTimer);
   ADD_TO_INSTANCE(InterruptPin);
   ADD_TO_INSTANCE(ExpansionROMBaseAddress);
   ADD_TO_INSTANCE(SelfTestEnabled);
 
   DEBUG("lpcp-> adding to the instance done");
   return instance; 
}

PEGASUS_NAMESPACE_END
