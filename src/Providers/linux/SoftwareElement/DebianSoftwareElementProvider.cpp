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
#include "DebianSoftwareElementProvider.h"
#include "DebianPackageManagerData.h"

#include <iostream>
#include <string.h>
#include <stdio.h>

PEGASUS_USING_STD;

PEGASUS_NAMESPACE_BEGIN

DebianSoftwareElementProvider::DebianSoftwareElementProvider(void)
{
}

DebianSoftwareElementProvider::~DebianSoftwareElementProvider(void)
{
}


void 
DebianSoftwareElementProvider::getInstance(
      				const OperationContext& context,
			  	const CIMObjectPath& ref,
				const Boolean includeQualifiers,
				const Boolean includeClassOrigin,
			  	const CIMPropertyList& propertyList,
			  	InstanceResponseHandler& handler )
{
   Uint32 i;
   Array<CIMKeyBinding> keys = ref.getKeyBindings();
   PackageInformation* package;
   CIMInstance instance;
   String packageDatabaseDirectory;
   String packageName;
   String packageVersion;
 
   DEBUG("ldsep-> start getInstance");
   handler.processing();
   i = 0;
 
   /* Find out which package they are talking about */
 
   String more_junk;
   more_junk = "ldsep-> key is ";
   while (i < keys.size())
   {
      more_junk.append(keys[i].getName().getString() + "=");
      more_junk.append(keys[i].getValue() + ",");

      if (keys[i].getName() == CIMName("Name"))
 	 packageName=keys[i].getValue();
      else if (keys[i].getName() == CIMName("Version"))
    	 packageVersion=keys[i].getValue();
      else if (keys[i].getName() == CIMName("PackageDatabase"))
      	 packageDatabaseDirectory=keys[i].getValue();
      i++;
   }
   DEBUG(more_junk);
 
   /* If there is no PackageDatabase then use the default */
   if (packageDatabaseDirectory == String::EMPTY)
   {
      packageDatabaseDirectory = DEFAULT_DEBIAN_DATABASE;
   }

   DEBUG("ldsep-> package " + packageName);
   DEBUG("ldsep-> version " + packageVersion);
   DEBUG("ldsep-> db " + packageDatabaseDirectory);

   /* If the name or version is missing it's a mistake */
   if ((packageName == String::EMPTY) || (packageVersion == String::EMPTY))
      throw CIMException(CIM_ERR_INVALID_PARAMETER);
 
   DebianPackageManagerData packageManager(packageDatabaseDirectory);
 
   DEBUG("ldsep-> looking for package");
   package = packageManager.GetPackage(packageName, packageVersion);
   if (package && package->isValid())
   {
      DEBUG("ldsep-> found package");
      instance = build_instance(DEBIANCLASSNAME,package);
      DEBUG("ldsep-> built instance");
      delete package;
      handler.deliver(instance);
      handler.complete();
      DEBUG("ldsep-> normal return");
      return;
   }
   if (package) 
      delete package;
   package = NULL;
 
   handler.complete();
   DEBUG("ldsep-> ABnormal return");
 
   throw CIMException(CIM_ERR_NOT_FOUND);
}


void
DebianSoftwareElementProvider::enumerateInstances(
                          const OperationContext& context,
			  const CIMObjectPath& ref,
			  const Boolean includeQualifiers,
			  const Boolean includeClassOrigin,
			  const CIMPropertyList& propertyList,
			  InstanceResponseHandler& handler )
{
   DebianPackageManagerData packageManager(DEFAULT_DEBIAN_DATABASE);
   PackageInformation* curPackage;
 
   handler.processing();
 
   curPackage = packageManager.GetFirstPackage();
   while (curPackage && curPackage->isValid())
   {
      handler.deliver(build_instance(DEBIANCLASSNAME,curPackage));
      delete curPackage;
      curPackage = packageManager.GetNextPackage();
   }
   if (curPackage)
   {
      delete curPackage;
      curPackage = NULL;
   }
 
   packageManager.EndGetPackage();
   
   handler.complete();
}

void 
DebianSoftwareElementProvider::enumerateInstanceNames(
      				const OperationContext& context,
			  	const CIMObjectPath& ref,
			  	ObjectPathResponseHandler& handler )
{
   DebianPackageManagerData packageManager(DEFAULT_DEBIAN_DATABASE);
   PackageInformation* curPackage;
 
   handler.processing();
 
   curPackage = packageManager.GetFirstPackage();
   while (curPackage && curPackage->isValid())
   {
      handler.deliver(fill_reference(ref.getNameSpace(),
	       			     DEBIANCLASSNAME,
				     DEFAULT_DEBIAN_DATABASE,
				     curPackage));
      delete curPackage;
      curPackage = packageManager.GetNextPackage();
   }
   if (curPackage)
   {
      delete curPackage;
      curPackage = NULL;
   }
 
   packageManager.EndGetPackage();
   
   handler.complete();
}

void 
DebianSoftwareElementProvider::modifyInstance(
      			  	const OperationContext& context,
			  	const CIMObjectPath& ref,
			  	const CIMInstance& instanceObject,
				const Boolean includeQualifiers,
			  	const CIMPropertyList& propertyList,
			  	ResponseHandler& handler )
{
   cout << "DebianSoftwareElementProvider::modifyInstance called" << endl;
   throw CIMNotSupportedException(DEBIANCLASSNAME "::modifyInstance");
}

void 
DebianSoftwareElementProvider::createInstance(
      			  	const OperationContext& context,
			  	const CIMObjectPath& ref,
			  	const CIMInstance& instanceObject,
			  	ObjectPathResponseHandler& handler )
{
   cout << "DebianSoftwareElementProvider::createInstance called" << endl;
   throw CIMNotSupportedException(DEBIANCLASSNAME "::createInstance");
}

void 
DebianSoftwareElementProvider::deleteInstance(
      				const OperationContext& context,
			  	const CIMObjectPath& ref,
			  	ResponseHandler& handler )
{
   cout << "DebianSoftwareElementProvider::deleteInstance called" << endl;
   throw CIMNotSupportedException(DEBIANCLASSNAME "::deleteInstance");
}

void DebianSoftwareElementProvider::initialize(CIMOMHandle& handle)
{
}

void DebianSoftwareElementProvider::terminate(void)
{
   delete this;
}

CIMInstance 
DebianSoftwareElementProvider::build_instance(const CIMName& className, 
      					      const PackageInformation* ptr)
{
   CIMInstance instance(className);

#define ADD_TO_INSTANCE(x) instance.addProperty(CIMProperty(#x, ptr->Get ## x()))

   ADD_TO_INSTANCE(Name);
   ADD_TO_INSTANCE(Version);
   ADD_TO_INSTANCE(SoftwareElementState);
   ADD_TO_INSTANCE(SoftwareElementID);
   ADD_TO_INSTANCE(TargetOperatingSystem);
   ADD_TO_INSTANCE(Manufacturer);

#undef ADD_TO_INSTANCE

   return instance;
}


CIMObjectPath 
DebianSoftwareElementProvider::fill_reference(const CIMNamespaceName& nameSpace,
      					      const CIMName& className,
					      const String& packageDirectory,
					      const PackageInformation* ptr)
{
   /* Build a reference to a package.  */
   Array<CIMKeyBinding> keys;

   keys.append(CIMKeyBinding("Name", ptr->GetName(),
			  CIMKeyBinding::STRING));
   keys.append(CIMKeyBinding("Version", ptr->GetVersion(),
			  CIMKeyBinding::STRING));
   String junk;
   junk = "ldsep-> fill_ref to " + ptr->GetName();
   junk.append(", version " + ptr->GetVersion());
   DEBUG(junk);
   keys.append(CIMKeyBinding("SoftwareElementState", 
	                  ptr->GetSoftwareElementState(),
			  CIMKeyBinding::STRING));
   keys.append(CIMKeyBinding("SoftwareElementID", 
	                  ptr->GetSoftwareElementID(),
			  CIMKeyBinding::STRING));

   String value;
   char buf[256];
   memset(buf, 0, 256);
   sprintf(buf, "%d", ptr->GetTargetOperatingSystem());
   value = buf;
   keys.append(CIMKeyBinding("TargetOperatingSystem", value, CIMKeyBinding::NUMERIC));

   return CIMObjectPath(System::getHostName(), nameSpace,
		       className, keys);
}


PEGASUS_NAMESPACE_END
