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
#include <Pegasus/Common/CIMReference.h>
#include "ProcessorProvider.h"
#include "ProcessorData.h"
#include "ProcessorInformation.h"

#include <iostream>

PEGASUS_USING_STD;

PEGASUS_NAMESPACE_BEGIN

LinuxProcessorProvider::LinuxProcessorProvider(void)
{
}

LinuxProcessorProvider::~LinuxProcessorProvider(void)
{
}


void
LinuxProcessorProvider::getInstance(const OperationContext& context,
				    const CIMReference& ref,
				    const Uint32 flags,
				    const Array<String>& propertyList,
				    ResponseHandler<CIMInstance>& handler)
{
   ProcessorData processorData;
   ProcessorInformation* curProcessor;
   Array<KeyBinding> keys = ref.getKeyBindings();
   Uint32 i;
   String roleString;
 
 
   handler.processing();
 
   /* Get the processor that was requested */
   i = 0;
   while (i<keys.size())
   {
      if (keys[i].getName() == "Role")
         roleString = keys[i].getValue();
      i++;
   }
   if (roleString != String::EMPTY)
   {
      curProcessor = processorData.GetProcessor(roleString);    
      if (curProcessor!=NULL)
      {
         CIMInstance instance = 
 	               build_instance(PROCESSORCLASSNAME, curProcessor);
         handler.deliver(instance);
         delete curProcessor;
      }
   }
   handler.complete();
   return;
}


void 
LinuxProcessorProvider::enumerateInstances(
      				const OperationContext& context, 
				const CIMReference& ref, 
				const Uint32 flags, 
				const Array<String>& propertyList,
				ResponseHandler<CIMInstance>& handler )
{
   ProcessorData processorData;
   ProcessorInformation* curProcessor;
 
   handler.processing();
   
   curProcessor = processorData.GetFirstProcessor();
 
   while (curProcessor)
   {
      handler.deliver(build_instance(PROCESSORCLASSNAME, curProcessor));
      delete curProcessor;
      curProcessor = processorData.GetNextProcessor();
   }
   processorData.EndGetProcessor();
 
   handler.complete();
}

void 
LinuxProcessorProvider::enumerateInstanceNames(
      				const OperationContext& context,
			  	const CIMReference& ref,
			  	ResponseHandler<CIMReference>& handler )
{
   ProcessorData processorData;
   ProcessorInformation* curProcessor;

   handler.processing();

   curProcessor = processorData.GetFirstProcessor();
   while (curProcessor)
   {
      handler.deliver(fill_reference(ref.getNameSpace(), 
	       			     PROCESSORCLASSNAME,
				     curProcessor));
      delete curProcessor;
      curProcessor = processorData.GetNextProcessor();
   }
   processorData.EndGetProcessor();

   handler.complete();

}

void LinuxProcessorProvider::modifyInstance(
      				const OperationContext& context,
			  	const CIMReference& ref,
			  	const CIMInstance& instanceObject,
			  	const Uint32 flags, 
			  	const Array<String>& propertyList,
			  	ResponseHandler<CIMInstance>& handler )
{
   cout << "LinuxProcessorProvider::modifyInstance called" << endl;
   throw NotSupported(PROCESSORCLASSNAME "::modifyInstance");
}

void 
LinuxProcessorProvider::createInstance(
      				const OperationContext& context,
			  	const CIMReference& ref,
			  	const CIMInstance& instanceObject,
			  	ResponseHandler<CIMReference>& handler )
{
   cout << "LinuxProcessorProvider::createInstance called" << endl;
   throw NotSupported(PROCESSORCLASSNAME "::createInstance");
}

void LinuxProcessorProvider::deleteInstance(
      				const OperationContext& context,
			  	const CIMReference& ref,
			  	ResponseHandler<CIMInstance>& handler )
{
   cout << "LinuxProcessorProvider::deleteInstance called" << endl;
   throw NotSupported(PROCESSORCLASSNAME "::deleteInstance");
}

void LinuxProcessorProvider::initialize(CIMOMHandle& handle)
{
}


void LinuxProcessorProvider::terminate(void)
{
}


CIMReference
LinuxProcessorProvider::fill_reference(const String& nameSpace, 
      				       const String& className,
				       const ProcessorInformation* ptr)
{
   Array<KeyBinding> keys;

   keys.append(KeyBinding("Role", ptr->getRole(),
                          KeyBinding::STRING));
   keys.append(KeyBinding("Manufacturer", ptr->getManufacturerString(),
			  KeyBinding::STRING));
   keys.append(KeyBinding("Name", ptr->getDeviceString(),
                          KeyBinding::STRING));

   return CIMReference(System::getHostName(), nameSpace, 
		       className, keys);
}

CIMInstance 
LinuxProcessorProvider::build_instance(const String& className,
      				       const ProcessorInformation* ptr)
{
   CIMInstance instance(className);

   instance.addProperty(CIMProperty("Role",ptr->getRole()));
   instance.addProperty(CIMProperty("Family",ptr->getFamily()));
   instance.addProperty(CIMProperty("MaxClockSpeed",ptr->getMaxClockSpeed()));
   instance.addProperty(CIMProperty("CurrentClockSpeed",
	    			    ptr->getCurClockSpeed()));
   instance.addProperty(CIMProperty("DataWidth",ptr->getDataWidth()));
   instance.addProperty(CIMProperty("AddressWidth",ptr->getAddressWidth()));
   instance.addProperty(CIMProperty("Stepping",ptr->getStepping()));
   instance.addProperty(CIMProperty("CPUStatus",ptr->getCPUStatus()));
   instance.addProperty(CIMProperty("Name",ptr->getDeviceString()));
   instance.addProperty(CIMProperty("Manufacturer",
	    			    ptr->getManufacturerString()));

   return instance; 
}

PEGASUS_NAMESPACE_END
