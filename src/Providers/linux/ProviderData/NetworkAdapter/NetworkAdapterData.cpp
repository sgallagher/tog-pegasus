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
//
//  Generic network adapter
//


#include "NetworkAdapterData.h"
#include "DeviceLocator.h"
#include "IPV4NetInformation.h"

PEGASUS_USING_STD;

PEGASUS_NAMESPACE_BEGIN

#define CLASSNAME "NetworkAdapterData"

/** This macro attempts to perform an operation, and catches and ignores
 *  the AccessedInvalidData exception if that data is not valid. */
#define SKIP_IF_UNSET(x) do { try { x; } catch(AccessedInvalidData &e) {} } while (0)


/** The initialize(void) method scans the interfaces on the system, looking
 *  for one which matches the name set in the constructor. */
int NetworkAdapterData::initialize(void)
{ 
  int retval;
  IPV4IFInformation *interface = NULL;
  DeviceLocator devfinder(Device_IPV4NetworkSettings,
			  Device_IPV4NetworkSettings_Interfaces);

  while ((interface = (IPV4IFInformation*)devfinder.getNextDevice()) != NULL &&
	 interface->GetInterface() != name) 
    delete interface;

  if (interface == NULL)
    return -1;

  SetLogicalDeviceID(String("Interface:") + name);
  retval = initialize(interface);
  delete interface;
  return retval;
}


/** This method is like initialize, but the caller must provide it with the
 *  located device */
int NetworkAdapterData::initialize(IPV4IFInformation const *interface)
{
  name = interface->GetInterface();
  SKIP_IF_UNSET(bytes_sent.setValue(interface->GetTxBytes()));
  SKIP_IF_UNSET(bytes_received.setValue(interface->GetRxBytes()));

  if (interface->GetEncapsulation() == "Ethernet")
    adapter_type = NETWORK_ADAPTER_ETHERNET;
  else if (interface->GetEncapsulation() == "Local Loopback")
    adapter_type = NETWORK_ADAPTER_LO;

  return 0;
}


vector<String> NetworkAdapterData::list_all_adapters(void)
{
  vector<String> retval;
  IPV4IFInformation *iface;
  DeviceLocator devfinder(Device_IPV4NetworkSettings,
			  Device_IPV4NetworkSettings_Interfaces);

  retval.clear();

  while ((iface = (IPV4IFInformation *) devfinder.getNextDevice()) != NULL) {
    retval.push_back(iface->GetInterface());
    delete iface;
  }

  return retval;  
}


PEGASUS_NAMESPACE_END
