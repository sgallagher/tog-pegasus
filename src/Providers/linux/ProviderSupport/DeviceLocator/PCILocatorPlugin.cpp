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
//              Josephine Eskaline Joyce (jojustin@in.ibm.com) for PEP#101
//
//%////////////////////////////////////////////////////////////////////////////
//
//  Device locator plugin for reading the devices on the PCI bus.
//

#include <Pegasus/Common/System.h>
#include <Pegasus/Common/String.h>

#include "ControllerInformation.h"
#include "PCILocatorPlugin.h"
#include "PCIControllerInformation.h"
#include "FileReader.h"
#include "DeviceTypes.h"
#include <stdio.h>

PEGASUS_NAMESPACE_BEGIN

const char UnexpectedError::MSG[] = "Unexpected error in PCI locator: ";

#ifndef N_IN_ARRAY
#define N_IN_ARRAY(x) ((int) (sizeof(x) / sizeof(x[0])))
#endif

enum pci_lines {
  PCI_START_OF_NEW_DEVICE,
  PCI_CONTROL_INFO, PCI_STATUS_INFO,
  PCI_AGP_VERSION, PCI_AGP_STATUS_INFO, PCI_AGP_COMMAND_INFO,
  PCI_POWER_VERSION, PCI_POWER_FLAGS, PCI_POWER_STATUS,
  PCI_LATENCY_CACHE,
  PCI_LATENCY_NOCACHE,   // regexp must appear after PCI_LATENCY_CACHE's
  PCI_INTERRUPT,
  PCI_MEMORY_REGION,
  PCI_IO_PORT_REGION,
  PCI_EXPANSION_ROM,
  PCI_END_OF_DEVICE
};

static struct {
  const char *regex;
  enum pci_lines line_type;
} pci_detailed_info[] = {
  {
    "^([0-9a-fA-F]{2}:[0-9a-fA-F]{2}\\.[0-9a-fA-F]) Class ([0-9a-fA-F]{2})([0-9a-fA-F]{2}): ([0-9a-fA-F]{4}):([0-9a-fA-F]{4})",
    PCI_START_OF_NEW_DEVICE
  },
  {
    "^[ \t]*Control: I/O([+-]) Mem([+-]) BusMaster([+-]) SpecCycle([+-]) MemWINV([+-]) VGASnoop([+-]) ParErr([+-]) Stepping([+-]) SERR([+-]) FastB2B([+-])",
    PCI_CONTROL_INFO
  },
  {
    "^[ \t]*Status: Cap([+-]) 66Mhz([+-]) UDF([+-]) FastB2B([+-]) ParErr([+-]) DEVSEL=(fast|medium|slow) >TAbort([+-]) <TAbort([+-]) <MAbort([+-]) >SERR([+-]) <PERR([+-])",
    PCI_STATUS_INFO
  },
  {
    "^[ \t]*Capabilities: \\[[0-9a-fA-F]{2}\\] AGP version (.*)$",
    PCI_AGP_VERSION
  },
  {
    "^[ \t]*Status: RQ=([0-9]+) SBA([+-]) 64bit([+-]) FW([+-]) Rate=(.*)$",
    PCI_AGP_STATUS_INFO
  },
  {
    "^[ \t]*Command: RQ=([0-9]+) SBA([+-]) AGP([+-]) 64bit([+-]) Rate=(.*)$",
    PCI_AGP_COMMAND_INFO
  },
  {
    "^[ \t]*Capabilities: \\[[0-9a-fA-F]{2}\\] Power Management version (.*)$",
    PCI_POWER_VERSION
  },
  {
    "^[ \t]*Flags: PMEClk([+-]) DSI([+-]) D1([+-]) D2([+-]) AuxCurrent=([0-9]+)mA PME\\(D0([+-]),D1([+-]),D2([+-]),D3hot([+-]),D3cold([+-])\\)",
    PCI_POWER_FLAGS
  },
  {
    "^[ \t]*Status: D([0-9]+) PME-Enable([+-]) DSEL=([0-9]+) DScale=([0-9]+) PME([+-])",
    PCI_POWER_STATUS
  },
  {
    "^[ \t]*Latency: ([0-9]+), cache line size ([0-9]+)",
    PCI_LATENCY_CACHE
  },
  {
    "^[ \t]*Latency: ([0-9]+)",
    PCI_LATENCY_NOCACHE
  },
  { 
    "^[ \t]*Interrupt: pin ([A-D\\?]) routed to IRQ ([0-9a-fA-F]+)",
    PCI_INTERRUPT
  },
  {
    "^[ \t]*Region ([0-9]+): ((\\[virtual\\] )?)Memory at ([0-9A-Fa-f]+|<ignored>|<unassigned>) \\(32-bit|64-bit|low-1M|type 3), ((non-)?)prefetchable\\)(( \\[disabled\\])?) \\[size=([0-9]+)([KM]?)\\]",
    PCI_MEMORY_REGION
  },
  {
    "^[ \t]*Region ([0-9]+): ((\\[virtual\\] )?)I/O ports at ([0-9A-Fa-f]+|<ignored>|<unassigned>| \\[disabled\\])",
    PCI_IO_PORT_REGION
  },
  {
    "^[ \t]*Expansion ROM at ([0-9A-Fa-f]+|<unassigned>)(( \\[disabled\\])?) \\[size=([0-9]+)([KM]?)\\]",
    PCI_EXPANSION_ROM
  },
  {
    "^[ \t]*$",
    PCI_END_OF_DEVICE
  },
};



PCILocatorPlugin::PCILocatorPlugin(void)
{
  detailedScanner.reset();
  textScanner.reset();
}

/* Sets the device search criteria.  Returns 0 on success or -1 if it is
 * unable to search for that type.  Calling this function resets the
 * locator pointer */
int PCILocatorPlugin::setDeviceSearchCriteria(Uint16 base_class, 
					      Uint16 sub_class,
					      Uint16 prog_if)
{
  int i;
  const char* args1[] = { "lspci", "-nvv", NULL };
  const char* command1 = args1[0];
  const char* args2[] = { "lspci", "-m", NULL };
  const char* command2 = args2[0];

  // These two regular expressions will walk their scanners together.
  char const *regex_patts1[N_IN_ARRAY(pci_detailed_info) + 1];
  char const *regex_patts2[] = {
    "^([0-9a-fA-F]{2}:[0-9a-fA-F]{2}\\.[0-9]) \"([^\"]*)\" \"([^\"]*)\" \"([^\"]*)\"",
    NULL
  };

  if (base_class != WILDCARD_PCI_DEVICE)
    if ((base_class >= 0x100 &&
	 base_class != WILDCARD_DEVICE) ||
	(sub_class >= 0x100 &&
	 sub_class != WILDCARD_DEVICE) ||
	(prog_if >= 0x100 &&
	 prog_if != WILDCARD_DEVICE))
      return -1;

  /* Set the internal search criteria */
  baseClass = base_class;
  subClass = sub_class;
  progIF = prog_if;


  /* Create a new file reader */
  detailedScanner.reset(new ExecScanner);
  textScanner.reset(new ExecScanner);
  if ((detailedScanner.get()) == NULL ||
      (textScanner.get()) == NULL) {
    detailedScanner.reset();
    textScanner.reset();
    return -1;
  }

  for (i = 0; i < N_IN_ARRAY(pci_detailed_info); i++)
    regex_patts1[i] = pci_detailed_info[i].regex;
  regex_patts1[i] = NULL;

  try {
    detailedScanner->SetSearchRegexps(regex_patts1);
    textScanner->SetSearchRegexps(regex_patts2);
  }
  catch (Exception & e) {
    return -1;
  }

  if (detailedScanner->ExecuteForScan(command1, args1) != 0 ||
      textScanner->ExecuteForScan(command2, args2) != 0)
    return -1;

  /* We are all set up ready to go! */
  return 0;
}


/* Returns a pointer to a DeviceInformation class or NULL if the last
 * device of that type was located.  This method allocated the
 * DeviceInformation object */
DeviceInformation *PCILocatorPlugin::getNextDevice(void)
{
  String junk_line;
  vector<String> detailed_matches, text_matches;
  Uint16 PCI_base = 0, PCI_sub = 0;
  AutoPtr<PCIControllerInformation> curDevice; 

  /* Check to see if we have been set up correctly */
  if (detailedScanner.get() == NULL ||
      textScanner.get() == NULL)
    return NULL;

  while (textScanner->GetNextMatchingLine(junk_line, NULL, text_matches) != -1) {
    bool pci_start_seen, pci_end_seen;
    int index;

    curDevice.reset(new PCIControllerInformation);

    // Start of a new PCI device.  Scarf down the details.
    pci_start_seen = pci_end_seen = false;
    while (!pci_end_seen &&
	   detailedScanner->GetNextMatchingLine(junk_line, &index, detailed_matches) != -1) {

      switch(index) {

      case PCI_START_OF_NEW_DEVICE:
	pci_start_seen = true;
	if (detailed_matches[1] != text_matches[1]) {
	  throw UnexpectedError("The two invocations of lspci "
				"were not synchronized.");
	}

	curDevice->setUniqueKey(String("PCIController: ") + detailed_matches[1]);

	PCI_base = (Uint16) strtoul(detailed_matches[2].getCString(), NULL, 16);
	PCI_sub = (Uint16) strtoul(detailed_matches[3].getCString(), NULL, 16);
	
	curDevice->setBaseClass(PCI_base);
	curDevice->setSubClass(PCI_sub);

	curDevice->setManufacturerID(strtoul(detailed_matches[4].getCString(), NULL, 16));
	curDevice->setDeviceID((Uint16) strtoul(detailed_matches[5].getCString(), NULL, 16));
	curDevice->setManufacturerString(text_matches[3]);
	curDevice->setDeviceString(text_matches[4]);

	curDevice->SetProtocolSupported(CONTROLLER_PROTOCOL_PCI);

	break;

      case PCI_CONTROL_INFO:  // we don't seem interested in this
	break;

      case PCI_STATUS_INFO:
	if (detailed_matches[1] == "-") {  // device doesn't support cap query
	  curDevice->AddCapability(PCI_CAPABILITY_UNKNOWN,
				   "Device does not support capability queries");
	} else {
	  curDevice->AddCapability(PCI_CAPABILITY_66MHZ, "Supports 66MHz",
				   detailed_matches[2]);
	  curDevice->AddCapability(PCI_CAPABILITY_UDF,
				   "Supports user defined features",
				   detailed_matches[3]);
	  curDevice->AddCapability(PCI_CAPABILITY_B2B, 
				   "Supports fast back-to-back transactions",
				   detailed_matches[4]);
	  curDevice->AddCapability(PCI_CAPABILITY_66MHZ, 
				   "Supports parity error recovery",
				   detailed_matches[5]);
	  curDevice->SetDevsel(detailed_matches[6]);
	}
	break;

      case PCI_AGP_VERSION:     
	curDevice->AddCapability(PCI_CAPABILITY_AGP, "Supports AGP");
	break;

      case PCI_AGP_STATUS_INFO:	   // no interest in other AGP data
      case PCI_AGP_COMMAND_INFO:
	break;

	// the power management data doesn't seem to map onto the .mof class
      case PCI_POWER_VERSION:      
      case PCI_POWER_FLAGS:
      case PCI_POWER_STATUS:
	break;

      case PCI_LATENCY_CACHE:
      {
	curDevice->SetLatency(
          strtoul(detailed_matches[1].getCString(), NULL, 10));
	curDevice->SetCacheLineSize(
          strtoul(detailed_matches[2].getCString(), NULL, 10));
	break;
      }
      case PCI_LATENCY_NOCACHE:	
      {
	curDevice->SetLatency(
          strtoul(detailed_matches[1].getCString(), NULL, 10));
	break;
      }
	
      case PCI_INTERRUPT:
	curDevice->SetInterruptPin(detailed_matches[1]);
	break;

      case PCI_MEMORY_REGION:    // we have no interest in memory or I/O ports
      case PCI_IO_PORT_REGION:
	break;

      case PCI_EXPANSION_ROM:
	curDevice->SetExpansionROMBaseAddr(detailed_matches[1]);
	break;

      case PCI_END_OF_DEVICE:
	pci_end_seen = true;
	break;
      }
    }
    
    if (!pci_start_seen || !pci_end_seen) {
      throw UnexpectedError("The two invocations of lspci "
			    "were not synchronized.");
    }

    // Skip this device if it isn't in our search criteria.

    if (baseClass != WILDCARD_PCI_DEVICE)
      if ((baseClass != WILDCARD_DEVICE &&
	   baseClass != PCI_base) ||
	  (subClass != WILDCARD_DEVICE &&
	   subClass != PCI_sub))
	continue;
    
    return (DeviceInformation *) curDevice.release();
  }

  detailedScanner.reset();
  textScanner.reset();

  return NULL;
}


PEGASUS_NAMESPACE_END
