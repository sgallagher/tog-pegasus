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
				    const CIMObjectPath& ref,
				    const Boolean includeQualifiers,
				    const Boolean includeClassOrigin,
				    const CIMPropertyList& propertyList,
				    InstanceResponseHandler& handler)
{
   ProcessorData processorData;
   ProcessorInformation* curProcessor;
   Array<CIMKeyBinding> keys = ref.getKeyBindings();
   Uint32 i;
   String roleString;
 
 
   handler.processing();
 
   /* Get the processor that was requested */
   i = 0;
   while (i<keys.size())
   {
      if (keys[i].getName() == CIMName("Role"))
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
				const CIMObjectPath& ref, 
				const Boolean includeQualifiers,
				const Boolean includeClassOrigin,
				const CIMPropertyList& propertyList,
				InstanceResponseHandler& handler )
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
			  	const CIMObjectPath& ref,
			  	ObjectPathResponseHandler& handler )
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
			  	const CIMObjectPath& ref,
			  	const CIMInstance& instanceObject,
				const Boolean includeQualifiers,
			  	const CIMPropertyList& propertyList,
			  	ResponseHandler& handler )
{
   cout << "LinuxProcessorProvider::modifyInstance called" << endl;
   throw CIMNotSupportedException(PROCESSORCLASSNAME "::modifyInstance");
}

void 
LinuxProcessorProvider::createInstance(
      				const OperationContext& context,
			  	const CIMObjectPath& ref,
			  	const CIMInstance& instanceObject,
			  	ObjectPathResponseHandler& handler )
{
   cout << "LinuxProcessorProvider::createInstance called" << endl;
   throw CIMNotSupportedException(PROCESSORCLASSNAME "::createInstance");
}

void LinuxProcessorProvider::deleteInstance(
      				const OperationContext& context,
			  	const CIMObjectPath& ref,
			  	ResponseHandler& handler )
{
   cout << "LinuxProcessorProvider::deleteInstance called" << endl;
   throw CIMNotSupportedException(PROCESSORCLASSNAME "::deleteInstance");
}

void LinuxProcessorProvider::initialize(CIMOMHandle& handle)
{
}


void LinuxProcessorProvider::terminate(void)
{
   delete this;
}


CIMObjectPath
LinuxProcessorProvider::fill_reference(const CIMNamespaceName& nameSpace, 
      				       const CIMName& className,
				       const ProcessorInformation* ptr)
{
   Array<CIMKeyBinding> keys;

   keys.append(CIMKeyBinding("Role", ptr->getRole(),
                          CIMKeyBinding::STRING));
   keys.append(CIMKeyBinding("Manufacturer", ptr->getManufacturerString(),
			  CIMKeyBinding::STRING));
   keys.append(CIMKeyBinding("Name", ptr->getDeviceString(),
                          CIMKeyBinding::STRING));

   return CIMObjectPath(System::getHostName(), nameSpace, 
		       className, keys);
}

CIMInstance 
LinuxProcessorProvider::build_instance(const CIMName& className,
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
