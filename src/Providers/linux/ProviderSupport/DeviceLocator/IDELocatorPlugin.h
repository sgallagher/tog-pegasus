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

#ifndef Pegasus_IDELocatorPlugin_h
#define Pegasus_IDELocatorPlugin_h

/* The Device Locator class is responsible for providing a consistant
 * interface to enumerating devices of many different types.
 * The initial implementation of this class will be for Linux but
 * it also could be ported to other operating systems.
 *
 * This file is a virtual base class used to build plugins for the 
 * Device Locator class
 */

#include <Pegasus/Common/Config.h>
#include "DeviceLocatorPlugin.h"
#include "MediaAccessDeviceInformation.h"
#include "FileReader.h"
#include <Pegasus/Common/AutoPtr.h>

/* Device list */

#include "DeviceTypes.h"

PEGASUS_NAMESPACE_BEGIN

class IDELocatorPlugin : public DeviceLocatorPlugin {

public:
	IDELocatorPlugin();

	~IDELocatorPlugin();

	/* Sets the device search criteria.
	 * Returns 0 for no error or -1 if it is unable to search for that type. 
	 * Calling this function resets the locator pointer */
	int setDeviceSearchCriteria(Uint16 base_class, Uint16 subclass = WILDCARD_DEVICE, Uint16 prog_if = WILDCARD_DEVICE);
	/* Returns a pointer to a DeviceInformation class or NULL if the last device of that type was located.
	 * This method allocated the DeviceInformation object */
	DeviceInformation *getNextDevice(void);

private:
	MediaAccessDeviceInformation *createDevice(void);
	void setOption(int dirIndex);

	Uint16 baseClass;
	Uint16 subClass;
	Uint16 progIF;

    AutoPtr<FileReader> fileReader; 

	String curDevice;
	String model;
	String capacity;
	String media;
};

PEGASUS_NAMESPACE_END

#endif
