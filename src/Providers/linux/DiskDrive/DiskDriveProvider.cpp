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
#include "DiskDriveProvider.h"
#include "DiskDriveData.h"
#include "MediaAccessDeviceInformation.h"

#include <iostream>

PEGASUS_USING_STD;

PEGASUS_NAMESPACE_BEGIN

LinuxDiskDriveProvider::LinuxDiskDriveProvider(void)
{
}

LinuxDiskDriveProvider::~LinuxDiskDriveProvider(void)
{
}


void LinuxDiskDriveProvider::getInstance(const OperationContext& context,
					 const CIMObjectPath& ref,
					 const Boolean includeQualifiers,
					 const Boolean includeClassOrigin,
					 const CIMPropertyList& propertyList,
					 InstanceResponseHandler& handler)
{
   DiskDriveData diskdriveData;
   MediaAccessDeviceInformation *curDiskDrive;
   Array<CIMKeyBinding> keys = ref.getKeyBindings();
   Uint32 i;
   String nameString;

   handler.processing();

   /* Get the diskdrive that was requested */
   i = 0;
   while (i < keys.size())
   {
      if (keys[i].getName() == CIMName("Name"))
         nameString = keys[i].getValue();
      i++;
   }
   if (nameString != String::EMPTY)
   {
      curDiskDrive = diskdriveData.GetDiskDrive(nameString);    
      if (curDiskDrive!=NULL)
      {
         CIMInstance instance =
	         build_instance(DISKDRIVECLASSNAME, curDiskDrive);
         handler.deliver(instance);
         delete curDiskDrive;
      }
   }
   handler.complete();
   return;
}


void 
LinuxDiskDriveProvider::enumerateInstances(
      				const OperationContext& context, 
				const CIMObjectPath& ref, 
				const Boolean includeQualifiers,
				const Boolean includeClassOrigin,
				const CIMPropertyList& propertyList,
				InstanceResponseHandler& handler )
{
   DiskDriveData diskdriveData;
   MediaAccessDeviceInformation *curDiskDrive;

   handler.processing();
  
   curDiskDrive = diskdriveData.GetFirstDiskDrive();

   while (curDiskDrive)
   {
      handler.deliver(build_instance(DISKDRIVECLASSNAME, curDiskDrive));
      delete curDiskDrive;
      curDiskDrive = diskdriveData.GetNextDiskDrive();
   }
   diskdriveData.EndGetDiskDrive();

   handler.complete();
}

void
LinuxDiskDriveProvider::enumerateInstanceNames(
      				const OperationContext& context,
			  	const CIMObjectPath& ref,
			  	ObjectPathResponseHandler& handler )
{
   DiskDriveData diskdriveData;
   MediaAccessDeviceInformation *curDiskDrive;

   handler.processing();

   curDiskDrive = diskdriveData.GetFirstDiskDrive();
   while (curDiskDrive)
   {
      handler.deliver(fill_reference(ref.getNameSpace(), 
	       			     DISKDRIVECLASSNAME,
				     curDiskDrive));
      delete curDiskDrive;
      curDiskDrive = diskdriveData.GetNextDiskDrive();
   }
   diskdriveData.EndGetDiskDrive();
 
   handler.complete();
}

void
LinuxDiskDriveProvider::modifyInstance(
      				const OperationContext& context,
			  	const CIMObjectPath& ref,
			  	const CIMInstance& instanceObject,
				const Boolean includeQualifiers,
			  	const CIMPropertyList& propertyList,
			  	ResponseHandler& handler )
{
   cout << "LinuxDiskDriveProvider::modifyInstance called" << endl;
   throw CIMNotSupportedException(DISKDRIVECLASSNAME"::modifyInstance");
}

void
LinuxDiskDriveProvider::createInstance(
      				const OperationContext& context,
			  	const CIMObjectPath& ref,
			  	const CIMInstance& instanceObject,
			  	ObjectPathResponseHandler& handler )
{
   cout << "LinuxDiskDriveProvider::createInstance called" << endl;
   throw CIMNotSupportedException(DISKDRIVECLASSNAME"::createInstance");
}

void
LinuxDiskDriveProvider::deleteInstance(
      				const OperationContext& context,
			  	const CIMObjectPath& ref,
			  	ResponseHandler& handler )
{
   cout << "LinuxDiskDriveProvider::deleteInstance called" << endl;
   throw CIMNotSupportedException(DISKDRIVECLASSNAME"::deleteInstance");
}

void LinuxDiskDriveProvider::initialize(CIMOMHandle& handle)
{
}

void LinuxDiskDriveProvider::terminate(void)
{
   delete this;
}

CIMObjectPath
LinuxDiskDriveProvider::fill_reference(const CIMNamespaceName& nameSpace, 
      				       const CIMName& className, 
				       const MediaAccessDeviceInformation* ptr)
{
   Array<CIMKeyBinding> keys;

   keys.append(CIMKeyBinding("Name", ptr->getName(), CIMKeyBinding::STRING));

   return CIMObjectPath(System::getHostName(), nameSpace, className, keys);
}

CIMInstance 
LinuxDiskDriveProvider::build_instance(const CIMName& className,
      				       const MediaAccessDeviceInformation* ptr)
{
   CIMInstance instance(className);

   instance.addProperty(CIMProperty("Name",ptr->getName()));
   instance.addProperty(CIMProperty("Description",ptr->getDescription()));
   instance.addProperty(CIMProperty("MaxMediaSize",ptr->getMaxMediaSize()));

   return instance; 
}

PEGASUS_NAMESPACE_END
