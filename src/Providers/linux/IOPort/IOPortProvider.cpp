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
#include "IOPortProvider.h"
#include "DeviceTypes.h"
#include "DeviceLocator.h"

#include <iostream>

PEGASUS_USING_STD;

PEGASUS_NAMESPACE_BEGIN

LinuxIOPortProvider::LinuxIOPortProvider(void)
{
}

LinuxIOPortProvider::~LinuxIOPortProvider(void)
{
}


void 
LinuxIOPortProvider::getInstance(const OperationContext& context,
				 const CIMObjectPath& ref,
				 const Boolean includeQualifiers,
				 const Boolean includeClassOrigin,
				 const CIMPropertyList& propertyList,
				 InstanceResponseHandler& handler)
{
   Array<CIMKeyBinding> keys = ref.getKeyBindings();
   Uint32 i;
   String in_addr;
   IOPortInformation* located_port;
   CIMName className;
   Uint16 subClass;

   className = ref.getClassName();
   if (className.equal(IOPORTCLASSNAME))
   {
      subClass = Device_SystemResources_IOPort;
   }
   else if (className.equal(IOMEMORYCLASSNAME))
   {
      subClass = Device_SystemResources_IOMemory;
   }
   else
   {
      throw CIMNotSupportedException(className.getString() + "::getInstance");
   }
 
   DeviceLocator deviceLocator(Device_SystemResources,subClass);

   for (i = 0; i < keys.size(); i++)
   {
      if (keys[i].getName() == CIMName("StartingAddress"))
      {
         in_addr = keys[i].getValue();
         break;
      }
   }

   handler.processing();

   located_port = (IOPortInformation *) deviceLocator.getNextDevice();
   while(located_port)
   {
      if(located_port->getStartingAddress()==in_addr)
      {
    	 CIMInstance instance = build_instance(className, located_port);
    	 handler.deliver(instance);
      }
      delete located_port;
      located_port = (IOPortInformation *) deviceLocator.getNextDevice();
   }
  
   handler.complete();
   return;
}


void 
LinuxIOPortProvider::enumerateInstances(
      				const OperationContext& context, 
				const CIMObjectPath& ref, 
				const Boolean includeQualifiers,
				const Boolean includeClassOrigin,
				const CIMPropertyList& propertyList,
				InstanceResponseHandler& handler)
{
   IOPortInformation* located_port;
   CIMName className;
   Uint16 subClass;

   className = ref.getClassName();
   if (className.equal(IOPORTCLASSNAME))
   {
      subClass = Device_SystemResources_IOPort;
   }
   else if (className.equal(IOMEMORYCLASSNAME))
   {
      subClass = Device_SystemResources_IOMemory;
   }
   else
   {
      throw CIMNotSupportedException(className.getString() +
                                     "::enumerateInstances");
   }
 
   DeviceLocator deviceLocator(Device_SystemResources,subClass);

   handler.processing();

   located_port = (IOPortInformation *)deviceLocator.getNextDevice();
   while (located_port)
   {
      handler.deliver(build_instance(className, located_port));
      delete located_port;
      located_port = (IOPortInformation *)deviceLocator.getNextDevice();
   }

   handler.complete();
}


void 
LinuxIOPortProvider::enumerateInstanceNames(
      				const OperationContext& context,
				const CIMObjectPath& ref,
				ObjectPathResponseHandler& handler )
{
   IOPortInformation* located_port;
   CIMName className;
   Uint16 subClass;

   className = ref.getClassName();
   if (className.equal(IOPORTCLASSNAME))
   {
      subClass = Device_SystemResources_IOPort;
   }
   else if (className.equal(IOMEMORYCLASSNAME))
   {
      subClass = Device_SystemResources_IOMemory;
   }
   else
   {
      throw CIMNotSupportedException(className.getString() +
                                     "::enumerateInstanceNames");
   }
 
   DeviceLocator deviceLocator(Device_SystemResources,subClass);

   handler.processing();

   located_port = (IOPortInformation *)deviceLocator.getNextDevice();
   while (located_port)
   {
      handler.deliver(fill_reference(ref.getNameSpace(), 
	       			     className,
				     located_port));
      delete located_port;
      located_port = (IOPortInformation *) deviceLocator.getNextDevice();
   }

   handler.complete();
}

void
LinuxIOPortProvider::modifyInstance(
      				const OperationContext& context,
			  	const CIMObjectPath& ref,
			  	const CIMInstance& instanceObject,
				const Boolean includeQualifiers,
			  	const CIMPropertyList& propertyList,
			  	ResponseHandler& handler )
{
   cout << "LinuxIOPortProvider::modifyInstance called" << endl;
   throw CIMNotSupportedException(IOPORTCLASSNAME "::modifyInstance");
}

void 
LinuxIOPortProvider::createInstance(
      				const OperationContext& context,
			  	const CIMObjectPath& ref,
			  	const CIMInstance& instanceObject,
			  	ObjectPathResponseHandler& handler )
{
   cout << "LinuxIOPortProvider::createInstance called" << endl;
   throw CIMNotSupportedException(IOPORTCLASSNAME "::createInstance");
}

void 
LinuxIOPortProvider::deleteInstance(
      				const OperationContext& context,
			  	const CIMObjectPath& ref,
			  	ResponseHandler& handler )
{
   cout << "LinuxIOPortProvider::deleteInstance called" << endl;
   throw CIMNotSupportedException(IOPORTCLASSNAME"::deleteInstance");
}

void LinuxIOPortProvider::initialize(CIMOMHandle& handle)
{
}

void LinuxIOPortProvider::terminate(void)
{
   delete this;
}

CIMObjectPath 
LinuxIOPortProvider::fill_reference(const CIMNamespaceName& nameSpace, 
				    const CIMName& className, 
				    struct IOPortInformation const* ptr)
{
   Array<CIMKeyBinding> keys;

   keys.append(CIMKeyBinding("CreationClassName", className.getString(),
                          CIMKeyBinding::STRING));
   keys.append(CIMKeyBinding("StartingAddress", ptr->getStartingAddress(),
                          CIMKeyBinding::STRING));

   return CIMObjectPath(System::getHostName(), nameSpace, 
		       className, keys);
}

CIMInstance 
LinuxIOPortProvider::build_instance(const CIMName& className, 
				    struct IOPortInformation const* ptr)
{
   CIMInstance instance(className);

#define ADD_TO_INSTANCE(x) instance.addProperty(CIMProperty(#x, ptr->get ## x()))  

   instance.addProperty(CIMProperty("CreationClassName",className.getString()));
   ADD_TO_INSTANCE(StartingAddress);
   ADD_TO_INSTANCE(EndingAddress);
   ADD_TO_INSTANCE(MappedResource);

#undef ADD_TO_INSTANCE

   return instance; 
}

PEGASUS_NAMESPACE_END
