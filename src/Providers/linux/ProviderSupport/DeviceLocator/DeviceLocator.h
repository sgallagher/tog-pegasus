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

#ifndef Pegasus_DeviceLocator_h
#define Pegasus_DeviceLocator_h

/* The Device Locator class is responsible for providing a consistant
 * interface to enumerating devices of many different types.
 * The initial implementation of this class will be for Linux but
 * it also could be ported to other operating systems.
 *
 * This classes uses plugins to perform the enumeration of all devices.
 * These plugins follow a consistant API and are responsible for 
 * enumerating specific devices 
 */

/* The device types to be used will be the PCI device types.  The PCI 
 * device types is a hierarchy of maximum depth three that enumerates
 * the different types of devices possible.  Because this class will
 * enumerate devices of types not only found in PCI busses the device
 * list will be extended using Pegasus specifics as necessary.
 *
 */

#include <Pegasus/Common/Config.h>

#include "DeviceLocator.h"
#include "DeviceLocatorPlugin.h"

#include <vector>

/* Device list */
#include "DeviceTypes.h"

PEGASUS_NAMESPACE_BEGIN

/** PluginData a struct containing the information about a specific plugin */
struct PluginData {
	DeviceLocatorPlugin *plugin;
	enum {UNUSED, UNCHECKED, PENDING, COMPLETE} status;
};

class DeviceLocator {

public:
	DeviceLocator();
	DeviceLocator(Uint16 base_class, Uint16 subclass = WILDCARD_DEVICE,
		      Uint16 prog_if = WILDCARD_DEVICE);

	~DeviceLocator();

	/** setDeviceSearchCriteria(): Returns 0 if it is possible to search for that 
	 *  criteria or -1 if no plugins support that criteria */
	int setDeviceSearchCriteria(Uint16 base_class, Uint16 subclass = WILDCARD_DEVICE, Uint16 prog_if = WILDCARD_DEVICE);

	/** getNextDevice(): This method returns a pointer to next device located based on
	 *  the search critiera */
	DeviceInformation * getNextDevice(void);

private:

	/** loadPlugins(): This function loads all the plugins.  */
	void loadPlugins(void);
	/** This vector contains all of the plugins currently loaded */
	vector<PluginData> plugins;
};

PEGASUS_NAMESPACE_END

#endif
