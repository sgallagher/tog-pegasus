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
//  Controller Data class
//

#ifndef Pegasus_PCIControllerData_h
#define Pegasus_PCIControllerData_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/String.h>
#include <Pegasus/Common/Array.h>


#include "DeviceLocator.h"
#include "ValidatedTypes.h"
#include "ProviderData.h"
#include "ControllerData.h"
#include "PCIControllerInformation.h"


PEGASUS_USING_STD;
PEGASUS_USING_PEGASUS;

PEGASUS_NAMESPACE_BEGIN

// a lot of things should just be drawn from the information class
#define BOUNCE_TO_PINFO(type, x, y) type x y const { return ((PCIControllerInformation *)info)->x(); }

/** This class implements the CIM_PCIController class.  Any object on the
 *  PCI bus is a PCIController, both devices and bridges. */
class PCIControllerData : public ControllerData {

private:
  /// A handle to the device locator which is scanning for PCI devices
  DeviceLocator *searcher;

public:
  PCIControllerData(void) { searcher = NULL; }
  ~PCIControllerData(void) { delete searcher; }

  int initialize(void);

  /** The initialize(DeviceLocator *) method sets the searcher and then
   *  runs the initialize(void).  */
  int initialize(DeviceLocator *s) { searcher = s; return initialize(); }

  void terminate(void) { }

  PCIControllerData *GetNext(void);
  
  BOUNCE_TO_PINFO(Uint8, GetClassCode, (void));
  BOUNCE_TO_PINFO(Array<Uint16> const &, GetCapabilities, (void));
  BOUNCE_TO_PINFO(Array<String> const &, GetCapabilityDescriptions, (void));
  BOUNCE_TO_PINFO(Uint16, GetDeviceSelectTiming, (void));
  BOUNCE_TO_PINFO(Uint8, GetCacheLineSize, (void));
  BOUNCE_TO_PINFO(Uint8, GetLatencyTimer, (void));
  BOUNCE_TO_PINFO(Uint16, GetInterruptPin, (void));
  BOUNCE_TO_PINFO(Uint32, GetExpansionROMBaseAddress, (void));
  BOUNCE_TO_PINFO(bool, GetSelfTestEnabled, (void));
};

PEGASUS_NAMESPACE_END


#endif  /* Pegasus_PCIControllerData_h  */
