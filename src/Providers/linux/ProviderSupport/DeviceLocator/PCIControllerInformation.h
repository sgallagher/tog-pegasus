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
//  PCI Controller information class
//

#ifndef Pegasus_PCIControllerInformation_h
#define Pegasus_PCIControllerInformation_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/String.h>
#include <Pegasus/Common/Array.h>

#include "ControllerInformation.h"

PEGASUS_USING_STD;
PEGASUS_USING_PEGASUS;

PEGASUS_NAMESPACE_BEGIN

/// The capability indices from CIM_Device25.mof
#define PCI_CAPABILITY_UNKNOWN 1
#define PCI_CAPABILITY_OTHER 2
#define PCI_CAPABILITY_66MHZ 3
#define PCI_CAPABILITY_UDF 4
#define PCI_CAPABILITY_B2B 5
#define PCI_CAPABILITY_PCI_X 6
#define PCI_CAPABILITY_POWER_MANAGEMENT 7
#define PCI_CAPABILITY_MSG_SGN_INTERRUPTS 8
#define PCI_CAPABILITY_PARITY_ERR 9
#define PCI_CAPABILITY_AGP 10
#define PCI_CAPABILITY_VITAL_DATA 11
#define PCI_CAPABILITY_SLOT_ID 12
#define PCI_CAPABILITY_HOT_SWAP 13


/** A class to provide information about generic PCI controllers.  That
 *  represents both PCI devices and PCI bridges. */
class PCIControllerInformation : public ControllerInformation {

private:
  Array<Uint16> capabilities;
  Array<String> capability_descriptions;
  validated<Uint16> select_timing;
  validated<Uint8> cache_line_size;
  validated<Uint8> latency_timer;
  validated<Uint16> interrupt_pin;
  validated<Uint32> expansion_rom_base_addr;
  validated<bool> self_test_enabled;

public:
  PCIControllerInformation(void) { select_timing.setValue(1); /* unknown */ }
  ~PCIControllerInformation(void) { }

  void AddCapability(Uint16 cap, String const &details, 
		     String const &yesno = "+");
  void SetDevsel(String const &devsel_string);
  void SetLatency(Uint32 val) { latency_timer.setValue((Uint8) val); }
  void SetCacheLineSize(Uint32 val) { cache_line_size.setValue((Uint8) val); }
  void SetInterruptPin(String const &pin);
  void SetExpansionROMBaseAddr(String const &address_text);

  Uint8 GetClassCode(void) const { return getBaseClass() & 0xff; }
  Array<Uint16> const &GetCapabilities(void) const { return capabilities; }
  Array<String> const &GetCapabilityDescriptions(void) const { 
    return capability_descriptions; 
  }
  Uint16 GetDeviceSelectTiming(void) const { return select_timing.getValue(); }
  Uint8 GetCacheLineSize(void) const { return cache_line_size.getValue(); }
  Uint8 GetLatencyTimer(void) const { return latency_timer.getValue(); }
  Uint16 GetInterruptPin(void) const { return interrupt_pin.getValue(); }
  Uint32 GetExpansionROMBaseAddress(void) const { 
    return expansion_rom_base_addr.getValue(); 
  }
  bool GetSelfTestEnabled(void) const { return self_test_enabled.getValue(); }
};  

PEGASUS_NAMESPACE_END


#endif  /* Pegasus_PCIControllerInformation_h  */
