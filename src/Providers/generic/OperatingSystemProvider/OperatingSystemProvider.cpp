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
// Author: Chip Vincent (cvincent@us.ibm.com)
//
// Modified By: Roger Kumpf, Hewlett-Packard Company (roger_kumpf@hp.com)
//              Carol Ann Krug Graves, Hewlett-Packard Company
//                (carolann_graves@hp.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#include <Pegasus/Common/Config.h>

#include "OperatingSystemProvider.h"
#include "OperatingSystem.h"

PEGASUS_NAMESPACE_BEGIN

OperatingSystemProvider::OperatingSystemProvider(void)
{
}

OperatingSystemProvider::~OperatingSystemProvider(void)	
{
}

void OperatingSystemProvider::initialize(CIMOMHandle & cimom)
{
	_cimom = cimom;
}

void OperatingSystemProvider::terminate(void)
{
	delete this;
}

void OperatingSystemProvider::getInstance(
	const OperationContext & context,
	const CIMObjectPath & instanceReference,
	const Boolean includeQualifiers,
	const Boolean includeClassOrigin,
	const CIMPropertyList & propertyList,
	InstanceResponseHandler & handler)
{
	handler.processing();

	Array<CIMObjectPath> cimReferences = _enumerateInstanceNames(context, instanceReference);
	
	for(Uint32 i = 0, n = cimReferences.size(); i < n; i++)
	{
		if(instanceReference == cimReferences[i])
		{
			Array<CIMInstance> cimInstances = _enumerateInstances(context, instanceReference);

			handler.deliver(cimInstances[i]);

			break;
		}
	}
	
	handler.complete();
}

void OperatingSystemProvider::enumerateInstances(
	const OperationContext & context,
	const CIMObjectPath & classReference,
	const Boolean includeQualifiers,
	const Boolean includeClassOrigin,
	const CIMPropertyList & propertyList,
	InstanceResponseHandler & handler)
{
	handler.processing();

	if (classReference.getClassName().equal ("pegasus_operatingsystem"))
	{
		Array<CIMInstance> instances;
		instances = _enumerateInstances(context, classReference);

		// deliver reference
		handler.deliver(instances);
	}

	handler.complete();
}

void OperatingSystemProvider::enumerateInstanceNames(
	const OperationContext & context,
	const CIMObjectPath & classReference,
	ObjectPathResponseHandler & handler)
{
	// begin processing the request
	handler.processing();

	// get class definition from repository
	CIMClass cimclass = _cimom.getClass(
		OperationContext(),
		classReference.getNameSpace(),
		classReference.getClassName(),
		false,
		false,
		false,
		CIMPropertyList());

	Array<CIMInstance> cimInstances = _enumerateInstances(context, classReference);
	
	// convert instances to references;
	for(Uint32 i = 0, n = cimInstances.size(); i < n; i++)
	{
		CIMObjectPath instanceReference = cimInstances[i].buildPath(cimclass);

		// ensure references are fully qualified
		instanceReference.setHost(classReference.getHost());
		instanceReference.setNameSpace(classReference.getNameSpace());

		// deliver reference
		handler.deliver(instanceReference);
	}

	// complete processing the request
	handler.complete();
}

void OperatingSystemProvider::modifyInstance(
	const OperationContext & context,
	const CIMObjectPath & instanceReference,
	const CIMInstance & instanceObject,
	const Boolean includeQualifiers,
	const CIMPropertyList & propertyList,
	ResponseHandler & handler)
{
	throw CIMNotSupportedException("OperatingSystem::modifyInstance");
}

void OperatingSystemProvider::createInstance(
	const OperationContext & context,
	const CIMObjectPath & instanceReference,
	const CIMInstance & instanceObject,
	ObjectPathResponseHandler & handler)
{
	throw CIMNotSupportedException("OperatingSystem::createInstance");
}

void OperatingSystemProvider::deleteInstance(
	const OperationContext & context,
	const CIMObjectPath & instanceReference,
	ResponseHandler & handler)
{
	throw CIMNotSupportedException("OperatingSystem::deleteInstance");
}

Array<CIMInstance> OperatingSystemProvider::_enumerateInstances(
	const OperationContext & context,
	const CIMObjectPath & classReference)
{
	OperatingSystem os;

	CIMInstance cimInstance("Pegasus_OperatingSystem");

	cimInstance.addProperty(CIMProperty("CSCreationClassName", os.GetCSCreationClassName()));
	cimInstance.addProperty(CIMProperty("CSName", os.GetCSName()));
	cimInstance.addProperty(CIMProperty("CreationClassName", os.GetCreationClassName()));
	cimInstance.addProperty(CIMProperty("Name", os.GetName()));
	cimInstance.addProperty(CIMProperty("OSType", os.GetOSType()));
	cimInstance.addProperty(CIMProperty("OtherTypeDescription", os.GetOtherTypeDescription()));
	cimInstance.addProperty(CIMProperty("CurrentTimeZone", os.GetCurrentTimeZone()));
	cimInstance.addProperty(CIMProperty("LocalDateTime", os.GetLocalDateTime()));
	cimInstance.addProperty(CIMProperty("LastBootUpTime", os.GetLastBootUpTime()));
	cimInstance.addProperty(CIMProperty("NumberOfLicensedUsers", os.GetNumberOfLicensedUsers()));
	cimInstance.addProperty(CIMProperty("NumberOfUsers", os.GetNumberOfUsers()));
	cimInstance.addProperty(CIMProperty("NumberOfProcesses", os.GetNumberOfProcesses()));
	cimInstance.addProperty(CIMProperty("MaxNumberOfProcesses", os.GetMaxNumberOfProcesses()));
	cimInstance.addProperty(CIMProperty("InstallDate", os.GetInstallDate()));
	cimInstance.addProperty(CIMProperty("Version", os.GetVersion()));
	cimInstance.addProperty(CIMProperty("TotalSwapSpaceSize", os.GetTotalSwapSpaceSize()));
	cimInstance.addProperty(CIMProperty("TotalVirtualMemorySize", os.GetTotalVirtualMemorySize()));
	cimInstance.addProperty(CIMProperty("FreeVirtualMemory", os.GetFreeVirtualMemory()));
	cimInstance.addProperty(CIMProperty("FreePhysicalMemory", os.GetFreePhysicalMemory()));
	cimInstance.addProperty(CIMProperty("TotalVisibleMemorySize", os.GetTotalVisibleMemorySize()));
	cimInstance.addProperty(CIMProperty("SizeStoredInPagingFiles", os.GetSizeStoredInPagingFiles()));
	cimInstance.addProperty(CIMProperty("FreeSpaceInPagingFiles", os.GetFreeSpaceInPagingFiles()));
	cimInstance.addProperty(CIMProperty("MaxProcessMemorySize", os.GetMaxProcessMemorySize()));
	cimInstance.addProperty(CIMProperty("Distributed", os.GetDistributed()));

	//
	// Construct the instance name to return
	// This could be done more efficiently by creating from scratch
	//

	// get class definition from repository
	CIMClass cimclass = _cimom.getClass(
		OperationContext(),
		classReference.getNameSpace(),
		classReference.getClassName(),
		false,
		false,
		false,
		CIMPropertyList());

	CIMObjectPath instanceName = cimInstance.buildPath(cimclass);

	// ensure references are fully qualified
	instanceName.setHost(classReference.getHost());
	instanceName.setNameSpace(classReference.getNameSpace());

	cimInstance.setPath(instanceName);

	// deliver the instance
        Array<CIMInstance> theInstance;
        theInstance.append(cimInstance);
	return theInstance;
}

Array<CIMObjectPath> OperatingSystemProvider::_enumerateInstanceNames(
	const OperationContext & context,
	const CIMObjectPath & classReference)
{
	Array<CIMInstance> cimInstances = _enumerateInstances(context, classReference);
	Array<CIMObjectPath> cimInstanceNames;
	
	// convert instances to references;
	for(Uint32 i = 0, n = cimInstances.size(); i < n; i++)
	{
		cimInstanceNames.append(cimInstances[i].getPath());
	}

	return cimInstanceNames;
}

PEGASUS_NAMESPACE_END
