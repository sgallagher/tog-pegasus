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
#include "InterruptProvider.h"
#include "InterruptData.h"

#include <iostream>

PEGASUS_USING_STD;

PEGASUS_NAMESPACE_BEGIN

LinuxInterruptProvider::LinuxInterruptProvider(void)
{
}

LinuxInterruptProvider::~LinuxInterruptProvider(void)
{
}


void LinuxInterruptProvider::getInstance(const OperationContext& context,
					 const CIMObjectPath& ref,
					 const Boolean includeQualifiers,
					 const Boolean includeClassOrigin,
					 const CIMPropertyList& propertyList,
					 InstanceResponseHandler& handler)
{
   InterruptData interruptData;
   InterruptData *curInterrupt;
   Array<CIMKeyBinding> keys = ref.getKeyBindings();
   Uint32 i;
   String nameString;
 
   handler.processing();
 
   /* Get the interrupt that was requested */
   i = 0;
   while (i < keys.size())
   {
      if (keys[i].getName() == CIMName("IRQNumber"))
         nameString = keys[i].getValue();
      i++;
   }
   if (nameString != String::EMPTY)
   {
      curInterrupt = interruptData.GetInterrupt(nameString);    
      if (curInterrupt != NULL)
      {
         CIMInstance instance = build_instance(INTERRUPTCLASSNAME,curInterrupt);
         handler.deliver(instance);
         delete curInterrupt;
      }
   }
   handler.complete();
   return;
}


void 
LinuxInterruptProvider::enumerateInstances(
      				const OperationContext& context, 
			        const CIMObjectPath& ref, 
				const Boolean includeQualifiers,
				const Boolean includeClassOrigin,
			        const CIMPropertyList& propertyList,
			        InstanceResponseHandler& handler )
{
   InterruptData interruptData;
   InterruptData* curInterrupt;
 
   handler.processing();
   
   curInterrupt = interruptData.GetFirstInterrupt();
 
   while (curInterrupt)
   {
      handler.deliver(build_instance(INTERRUPTCLASSNAME, curInterrupt));
      delete curInterrupt;
      curInterrupt = interruptData.GetNextInterrupt();
   }
   interruptData.EndGetInterrupt();
 
   handler.complete();
}

void LinuxInterruptProvider::enumerateInstanceNames(
      					const OperationContext& context,
			  		const CIMObjectPath& ref,
			  		ObjectPathResponseHandler& handler)
{
   InterruptData interruptData;
   InterruptData* curInterrupt;

   handler.processing();

   curInterrupt = interruptData.GetFirstInterrupt();
   while (curInterrupt)
   {
      handler.deliver(fill_reference(ref.getNameSpace(), 
	       			     INTERRUPTCLASSNAME,
				     curInterrupt));
      delete curInterrupt;
      curInterrupt = interruptData.GetNextInterrupt();
   }
   interruptData.EndGetInterrupt();

   handler.complete();
}

void LinuxInterruptProvider::modifyInstance(
      				const OperationContext& context,
			  	const CIMObjectPath& ref,
			  	const CIMInstance& instanceObject,
				const Boolean includeQualifiers,
			  	const CIMPropertyList& propertyList,
			  	ResponseHandler& handler )
{
   cout << "LinuxInterruptProvider::modifyInstance called" << endl;
   throw CIMNotSupportedException(INTERRUPTCLASSNAME"::modifyInstance");
}

void LinuxInterruptProvider::createInstance(
      				const OperationContext& context,
			  	const CIMObjectPath& ref,
			  	const CIMInstance& instanceObject,
			  	ObjectPathResponseHandler& handler )
{
   cout << "LinuxInterruptProvider::createInstance called" << endl;
   throw CIMNotSupportedException(INTERRUPTCLASSNAME"::createInstance");
}

void LinuxInterruptProvider::deleteInstance(
      				const OperationContext& context,
			  	const CIMObjectPath& ref,
			  	ResponseHandler& handler )
{
   cout << "LinuxInterruptProvider::deleteInstance called" << endl;
   throw CIMNotSupportedException(INTERRUPTCLASSNAME"::deleteInstance");
}

void LinuxInterruptProvider::initialize(CIMOMHandle& handle)
{
}


void LinuxInterruptProvider::terminate(void)
{
   delete this;
}

CIMObjectPath 
LinuxInterruptProvider::fill_reference(const CIMNamespaceName& nameSpace, 
      				       const CIMName& className,
				       const InterruptData* ptr)
{
   Array<CIMKeyBinding> keys;

   keys.append(CIMKeyBinding("IRQNumber", 
	    	          ptr->getIRQNumber(),
                          CIMKeyBinding::STRING));

   return CIMObjectPath(System::getHostName(), nameSpace, className, keys);
}

CIMInstance 
LinuxInterruptProvider::build_instance(const CIMName& className,
      				       const InterruptData* ptr)
{
   CIMInstance instance(className);

   instance.addProperty(CIMProperty("IRQNumber",ptr->getIRQNumber()));
   instance.addProperty(CIMProperty("Availability",ptr->getAvailability()));
   instance.addProperty(CIMProperty("TriggerType",ptr->getTriggerType()));
   instance.addProperty(CIMProperty("TriggerLevel",ptr->getTriggerLevel()));
   instance.addProperty(CIMProperty("Shareable",ptr->getShareable()));
   instance.addProperty(CIMProperty("Hardware",ptr->getHardware()));

   return instance; 
}

PEGASUS_NAMESPACE_END
