//BEGIN_LICENSE
//
// Copyright (c) 2000 The Open Group, BMC Software, Tivoli Systems, IBM
//
// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation
// the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following conditions:
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
// THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.
//
//END_LICENSE

#include <Pegasus/Common/Config.h>
#include <Pegasus/Provider/CIMProvider.h>
#include <Pegasus/Provider2/CIMInstanceProvider.h>
#include <Pegasus/Provider2/CIMProviderStub.h>
#include <Pegasus/Repository/CIMRepository.h>

#include "OperatingSystemProvider.h"
#include "OperatingSystem.h"

PEGASUS_NAMESPACE_BEGIN

extern "C" PEGASUS_EXPORT CIMProvider * PegasusCreateProvider_OperatingSystemProvider()
{
   return(new CIMProviderStub((CIMInstanceProvider *)new OperatingSystemProvider()));
}

OperatingSystemProvider::OperatingSystemProvider(void) : _pRepository(0)
{
}

OperatingSystemProvider::~OperatingSystemProvider()
{
}

void OperatingSystemProvider::initialize(CIMOMHandle& cimomHandle)
{
   // derefence repository pointer and save for later
   _pRepository = cimomHandle.getRepository();
}

void OperatingSystemProvider::terminate(void)
{
}

CIMInstance OperatingSystemProvider::getInstance(const OperationContext & context, const CIMReference & ref, const Uint32 flags, const Array<String> & propertyList)
{
   OperatingSystem os;

   CIMInstance instance("Pegasus_OperatingSystem");

   instance.addProperty(CIMProperty("CSCreationClassName", os.GetCSCreationClassName()));
   instance.addProperty(CIMProperty("CSName", os.GetCSName()));
   instance.addProperty(CIMProperty("CreationClassName", os.GetCreationClassName()));
   instance.addProperty(CIMProperty("Name", os.GetName()));
   instance.addProperty(CIMProperty("OSType", os.GetOSType()));
   instance.addProperty(CIMProperty("OtherTypeDescription", os.GetOtherTypeDescription()));
	instance.addProperty(CIMProperty("CurrentTimeZone", os.GetCurrentTimeZone()));
   instance.addProperty(CIMProperty("LocalDateTime", os.GetLocalDateTime()));
   //instance.addProperty(CIMProperty("LastBootUpTime", os.GetLastBootUpTime()));
   instance.addProperty(CIMProperty("NumberOfLicensedUsers", os.GetNumberOfLicensedUsers()));
   instance.addProperty(CIMProperty("NumberOfUsers", os.GetNumberOfUsers()));
   instance.addProperty(CIMProperty("NumberOfProcesses", os.GetNumberOfProcesses()));
   instance.addProperty(CIMProperty("MaxNumberOfProcesses", os.GetMaxNumberOfProcesses()));
   //instance.addProperty(CIMProperty("InstallDate", os.GetInstallDate()));
   instance.addProperty(CIMProperty("Version", os.GetVersion()));
   instance.addProperty(CIMProperty("TotalSwapSpaceSize", os.GetTotalSwapSpaceSize()));
   instance.addProperty(CIMProperty("TotalVirtualMemorySize", os.GetTotalVirtualMemorySize()));
   instance.addProperty(CIMProperty("FreeVirtualMemory", os.GetFreeVirtualMemory()));
   instance.addProperty(CIMProperty("FreePhysicalMemory", os.GetFreePhysicalMemory()));
   instance.addProperty(CIMProperty("TotalVisibleMemorySize", os.GetTotalVisibleMemorySize()));
   instance.addProperty(CIMProperty("SizeStoredInPagingFiles", os.GetSizeStoredInPagingFiles()));
   instance.addProperty(CIMProperty("FreeSpaceInPagingFiles", os.GetFreeSpaceInPagingFiles()));
   instance.addProperty(CIMProperty("MaxProcessMemorySize", os.GetMaxProcessMemorySize()));
	instance.addProperty(CIMProperty("Distributed", os.GetDistributed()));

   return(instance);
}

Array<CIMInstance> OperatingSystemProvider::enumerateInstances(const OperationContext & context, const CIMReference & ref, const Uint32 flags, const Array<String> & propertyList)
{
   Array<CIMInstance> instances;

   instances.append(getInstance(context, ref));

   return(instances);
}

Array<CIMReference> OperatingSystemProvider::enumerateInstanceNames(const OperationContext & context, const CIMReference & ref)
{
   // check dependencies
   if(_pRepository == 0) {
      throw(UnitializedHandle());
   }

   Array<CIMReference> references;

   return(references);
}

PEGASUS_NAMESPACE_END
