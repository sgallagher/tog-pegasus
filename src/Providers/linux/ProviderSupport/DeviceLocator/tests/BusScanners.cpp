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


#include <stdio.h>

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/Exception.h>

#include "DeviceLocator.h"
#include "DeviceLocatorPlugin.h"
#include "IPV4NetInformation.h"


PEGASUS_USING_PEGASUS;

/** This is a stand-alone program which links against the DeviceLocator .so
 *  library and does a wildcard search for all devices available to
 *  DeviceLocator objects.  The tests succeed if the DeviceLocator is
 *  successfully invoked and all plugins execute without throwing
 *  exceptions.  If the BusScanners binary is given any command line
 *  arguments, it also dumps information about every located pseudo-device
 *  to the screen. */
int main(int argc, char **argv)
{
  DeviceLocator locator;
  DeviceInformation *device;
  bool verbose_dumps;

  verbose_dumps = false;
  if (argc == 2)  //  Any command line arguments, and we get talky.
    verbose_dumps = true;

  try {
    if (locator.setDeviceSearchCriteria(WILDCARD_DEVICE) != 0) {
      cout << "Test failed." << endl;
      exit(EXIT_FAILURE);
    }

    while ((device = locator.getNextDevice()) != NULL)
      if (verbose_dumps) {
	printf("Found device:  %04x/%04x  %04x : ",
	       device->getBaseClass(), device->getSubClass(),
	       device->getProgIF());

	if (device->getBaseClass() == Device_IPV4NetworkSettings)
	  cout << ((IPV4NetInformation *) device)->print() << endl;
	else
	  cout << device->getManufacturerString() << "  " 
	       << device->getDeviceString() << endl;
      }
  } 
  catch (Exception &e) {
    cout << "Test failed: " << e.getMessage() << endl;
    exit(EXIT_FAILURE);
  }

  cout << "All tests passed." << endl;
  exit(EXIT_SUCCESS);
}

