//%/////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001, 2002 BMC Software, Hewlett-Packard Company, IBM,
// The Open Group, Tivoli Systems
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
//  The PCI Controller information class.  Lumps all the relevant data into
//  something the corresponding ProviderData can use.
//

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/System.h>
#include <Pegasus/Common/String.h>

#include "PCIControllerInformation.h"


PEGASUS_NAMESPACE_BEGIN

/** Add the numbered capability to the list of supported capabilities, with
 *  an associated description string, if the "yesno" variable is set to "+" */
void PCIControllerInformation::AddCapability(Uint16 cap, String const &details,
					     String const &yesno)
{
  if (yesno == "-")  // nothing to do
    return;
  else if (yesno != "+")
    throw BadFormat();

  capabilities.append(cap);
  capability_descriptions.append(details);
  return;
}


void PCIControllerInformation::SetDevsel(String const &dstr)
{
  if (dstr == "fast")
    select_timing.setValue(3);
  if (dstr == "medium")
    select_timing.setValue(4);
  if (dstr == "slow")
    select_timing.setValue(5);
}


void PCIControllerInformation::SetInterruptPin(String const &pin)
{
  switch(pin[0].getCode()) {
  case 'A':
    interrupt_pin.setValue(2);
    break;
  case 'B':
    interrupt_pin.setValue(3);
    break;
  case 'C':
    interrupt_pin.setValue(4);
    break;
  case 'D':
    interrupt_pin.setValue(5);
    break;
  case '?':
    throw BadFormat();
    break;
  }
}


void PCIControllerInformation::SetExpansionROMBaseAddr(String const &address_text)
{
  if (address_text == "<unassigned>")
    return;
  
  char *p;
  p = address_text.allocateCString();
  expansion_rom_base_addr.setValue(strtoul(p, NULL, 16));
  delete [] p;
  return;
}

PEGASUS_NAMESPACE_END
