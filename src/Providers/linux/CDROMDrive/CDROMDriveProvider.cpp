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
#include "CDROMDriveProvider.h"
#include "CDROMDriveData.h"
#include "MediaAccessDeviceInformation.h"

#include <iostream>

PEGASUS_USING_STD;

PEGASUS_NAMESPACE_BEGIN

LinuxCDROMDriveProvider::LinuxCDROMDriveProvider(void)
{
}

LinuxCDROMDriveProvider::~LinuxCDROMDriveProvider(void)
{
}


void LinuxCDROMDriveProvider::getInstance(const OperationContext& context,
					  const CIMObjectPath& ref,
					  const Boolean includeQualifiers,
					  const Boolean includeClassOrigin,
					  const CIMPropertyList& propertyList,
					  InstanceResponseHandler& handler)
{
   CDROMDriveData diskdriveData;
   MediaAccessDeviceInformation *curCDROMDrive;
   Array<CIMKeyBinding> keys= ref.getKeyBindings();
   Uint32 i;
   String nameString;

   handler.processing();

   /* Get the disk drive that was requested */
   i = 0;
   while(i < keys.size())
   {
      if(keys[i].getName() == CIMName("Name"))
         nameString = keys[i].getValue();
      i++;
   }
   if(nameString != String::EMPTY)
   {
      curCDROMDrive = diskdriveData.GetCDROMDrive(nameString);    
      if(curCDROMDrive != NULL)
      {
         CIMInstance instance =
	            build_instance(CDROMDRIVECLASSNAME, curCDROMDrive);
         handler.deliver(instance);
         delete curCDROMDrive;
      }
   }
   handler.complete();
   return;
}


void 
LinuxCDROMDriveProvider::enumerateInstances(
      				const OperationContext& context, 
			        const CIMObjectPath& ref, 
				const Boolean includeQualifiers,
				const Boolean includeClassOrigin,
			        const CIMPropertyList& propertyList,
			        InstanceResponseHandler& handler )
{
   CDROMDriveData diskdriveData;
   MediaAccessDeviceInformation *curCDROMDrive;

   handler.processing();
  
   curCDROMDrive = diskdriveData.GetFirstCDROMDrive();

   while(curCDROMDrive)
   {
      handler.deliver(build_instance(CDROMDRIVECLASSNAME, curCDROMDrive));
      delete curCDROMDrive;
      curCDROMDrive = diskdriveData.GetNextCDROMDrive();
   }
   diskdriveData.EndGetCDROMDrive();

   handler.complete();
}

void 
LinuxCDROMDriveProvider::enumerateInstanceNames(
      				const OperationContext& context,
			  	const CIMObjectPath& ref,
			  	ObjectPathResponseHandler& handler )
{
   CDROMDriveData diskdriveData;
   MediaAccessDeviceInformation *curCDROMDrive;

   handler.processing();

   curCDROMDrive = diskdriveData.GetFirstCDROMDrive();
   while(curCDROMDrive)
   {
      handler.deliver(fill_reference(ref.getNameSpace(), 
	                             CDROMDRIVECLASSNAME,
				     curCDROMDrive));
      delete curCDROMDrive;
      curCDROMDrive = diskdriveData.GetNextCDROMDrive();
   }
   diskdriveData.EndGetCDROMDrive();

   handler.complete();
}

void
LinuxCDROMDriveProvider::modifyInstance(
      				const OperationContext& context,
			  	const CIMObjectPath& ref,
			  	const CIMInstance& instanceObject,
				const Boolean includeQualifiers,
		          	const CIMPropertyList& propertyList,
			  	ResponseHandler& handler )
{
   cout << "LinuxCDROMDriveProvider::modifyInstance called" << endl;
   throw CIMNotSupportedException(CDROMDRIVECLASSNAME"::modifyInstance");
}

void
LinuxCDROMDriveProvider::createInstance(
      				const OperationContext& context,
			  	const CIMObjectPath& ref,
			  	const CIMInstance& instanceObject,
			  	ObjectPathResponseHandler& handler )
{
   cout << "LinuxCDROMDriveProvider::createInstance called" << endl;
   throw CIMNotSupportedException(CDROMDRIVECLASSNAME"::createInstance");
}

void
LinuxCDROMDriveProvider::deleteInstance(
      				const OperationContext& context,
			  	const CIMObjectPath& ref,
			  	ResponseHandler& handler )
{
   cout << "LinuxCDROMDriveProvider::deleteInstance called" << endl;
   throw CIMNotSupportedException(CDROMDRIVECLASSNAME"::deleteInstance");
}

void LinuxCDROMDriveProvider::initialize(CIMOMHandle& handle)
{
}


void LinuxCDROMDriveProvider::terminate(void)
{
   delete this;
}

CIMObjectPath
LinuxCDROMDriveProvider::fill_reference(const CIMNamespaceName& nameSpace, 
      					const CIMName& className, 
					const MediaAccessDeviceInformation *ptr)
{
   Array<CIMKeyBinding> keys;

   keys.append(CIMKeyBinding("Name", ptr->getName(),
                          CIMKeyBinding::STRING));

   return CIMObjectPath(System::getHostName(), nameSpace, 
		       className, keys);
}

CIMInstance
LinuxCDROMDriveProvider::build_instance(const CIMName& className, 
      					const MediaAccessDeviceInformation *ptr)
{
   CIMInstance instance(className);

   instance.addProperty(CIMProperty("Name",ptr->getName()));
   instance.addProperty(CIMProperty("Description",ptr->getDescription()));
   instance.addProperty(CIMProperty("MaxMediaSize",ptr->getMaxMediaSize()));

   return instance; 
}

PEGASUS_NAMESPACE_END
