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
//  Ethernet adapter information
//


#include <Pegasus/Common/System.h>

#include "EthernetAdapterData.h"
#include "DeviceLocator.h"
#include "IPV4NetInformation.h"
#include "ExecScanner.h"

PEGASUS_USING_STD;

PEGASUS_NAMESPACE_BEGIN

#define CLASSNAME "EthernetAdapterData"

#define N_IN_ARRAY(x) ((int) ( sizeof(x) / sizeof(x[0]) ))

#define SKIP_IF_UNSET(x) do { try { x; } catch(AccessedInvalidData &e) {} } while (0)

enum mii_line_type {
  MII_AUTONEG_MEDIA, MII_AUTONEG_ENABLED
};


/** Regular expressions for parsing the output of mii-diag. */
static struct {
  char const *regex;
  enum mii_line_type ltype;
} mii_info[] = {
  {
    "^The autonegotiated media type is (10|100)baseT[4x-]*((FD)?)$",
    MII_AUTONEG_MEDIA
  },
  {
    "^ Basic mode control register 0x[0-9a-fA-F]{4}: Auto-negotiation (enabled|disabled)",
    MII_AUTONEG_ENABLED
  },
};



EthernetAdapterData::EthernetAdapterData(String const &adap_name) : NetworkAdapterData(adap_name)
{
  capabilities.append(1);
  capability_descriptions.append("Unknown");
  enabled_capabilities.append(1);
  name=adap_name;
}


/** Search the interfaces on the system for the one matching the name
 *  provided in the constructor.  Returns -1 if that device turns out not
 *  to be an ethernet adapter, or if its initialize() or the parent class'
 *  initialize() method fails. */
int EthernetAdapterData::initialize(void)
{
  int retval;
  IPV4IFInformation *interface;
  DeviceLocator searcher(Device_IPV4NetworkSettings,
			 Device_IPV4NetworkSettings_Interfaces);

  while ((interface = (IPV4IFInformation*) searcher.getNextDevice()) != NULL &&
	 interface->GetInterface() != name) 
    delete interface;

  if (interface == NULL)
    return -1;

  if (interface->GetEncapsulation() != "Ethernet" ||
      NetworkAdapterData::initialize(interface) != 0) {
    delete interface;
    return -1;
  }

  retval = initialize(interface);
  delete interface;
  return retval;
}


/** Initializes the device.  Fills in data fields, both from /sbin/ifconfig
 *  and from mii-diag (if present). */
int EthernetAdapterData::initialize(IPV4IFInformation const *interface)
{
  CString name_cstring;
  char const *mii_args[] = { "mii-diag", "PLACEHOLDER", NULL };
  char const *mii_exename = mii_args[0];
  ExecScanner mii_runner;
  char const *mii_regexs[N_IN_ARRAY(mii_info) + 1];
  vector<String> matches;
  int index;
  String junk_line;
  int i;

  network_addresses.clear();
  network_addresses.append(interface->GetHWAddr());

  // It's not an error if any of these fields is unknown
  SKIP_IF_UNSET(maxdatasize.setValue(interface->GetMTU()));
  SKIP_IF_UNSET(total_packets_sent.setValue(interface->GetTxPackets()));
  SKIP_IF_UNSET(total_packets_received.setValue(interface->GetRxPackets()));

  for (i = 0; i < N_IN_ARRAY(mii_info); i++)
    mii_regexs[i] = mii_info[i].regex;
  mii_regexs[i] = NULL;

  name_cstring = name.getCString();
  mii_args[N_IN_ARRAY(mii_args) - 2] = (const char*)name_cstring;
  if (mii_runner.ExecuteForScan(mii_exename, mii_args) == 0) {

    mii_runner.SetSearchRegexps(mii_regexs);

    while (mii_runner.GetNextMatchingLine(junk_line, &index, matches) != -1) {

      switch(index) {
      case MII_AUTONEG_MEDIA:
      {
	SetMaxSpeed(strtoul(matches[1].getCString(), NULL, 10) * 1000000);
	if (matches[2] == "") // not full-duplex
	  SetFullDuplex(false);
	else
	  SetFullDuplex(true);
	break;
      }
      case MII_AUTONEG_ENABLED:
	if (matches[1] == "enabled")  // autosense enabled
	  SetAutoSense(true);
	else
	  SetAutoSense(false);
	break;
      }
    }
  }

  SetLogicalDeviceID(String("Interface:") + name);

  adapter_type = NETWORK_ADAPTER_ETHERNET;
  return 0;
}



Array<Uint16> const &EthernetAdapterData::GetCapabilities(void) const
{
  return capabilities;
}

Array<String> const &EthernetAdapterData::GetCapabilityDescriptions(void) const
{
  return capability_descriptions;
}

Array<Uint16> const &EthernetAdapterData::GetEnabledCapabilities(void) const
{
  return enabled_capabilities;
}

PEGASUS_NAMESPACE_END
