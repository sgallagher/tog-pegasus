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

#ifndef Pegasus_DeviceInformation_h
#define Pegasus_DeviceInformation_h

/* The Device Information class is responsible for providing a consistant
 * interface to enumerating devices of many different types.  The initial
 * implementation of this class will be for Linux but it also could be
 * ported to other operating systems.
 *
 * This classes uses plugins to perform the enumeration of all devices.
 * These plugins follow a consistant API and are responsible for 
 * enumerating specific devices */

/* The device types to be used will be the PCI device types.  The PCI 
 * device types is a hierarchy of maximum depth three that enumerates
 * the different types of devices possible.  Because this class will
 * enumerate devices of types not only found in PCI busses the device
 * list will be extended using Pegasus specifics as necessary.
 *
 */

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/String.h>

/* Device list */

#include "ValidatedTypes.h"
#include "DeviceTypes.h"

PEGASUS_NAMESPACE_BEGIN


// If we pass 0xffff, we want any and all devices returned by the bus walker
#define WILDCARD_DEVICE ((Uint16) 0xffff)

// If we pass 0xfffe, we want any PCI device (i.e. baseClass < 0x100)
#define WILDCARD_PCI_DEVICE ((Uint16) 0xfffe)


/** The base class for DeviceInformation classes.  These are used to hold
 *  data obtained by the DeviceLocator plugins, but are distinct from the
 *  ProviderData types. */
class DeviceInformation {

public:
	DeviceInformation(){
		 baseClass=0;
		 subClass=0;
		 progIF=0;
		 manufacturerID=0;
		 deviceID=0;
	}

	/* Set the device type */
	DeviceInformation(Uint16 inBase,Uint16 inSub,Uint16 inProg=0){
		baseClass=inBase;
		subClass=inSub;
		progIF=inProg;
	}

	virtual ~DeviceInformation(void) {}

	/* Return the base class id of this device */
	Uint16 getBaseClass() const{
		return baseClass;
	}
	/* Return the sub class id of this device */
	Uint16 getSubClass() const{
		return subClass;
	}
	/* Return the prog if id of this device */
	Uint16 getProgIF() const{
		return progIF;
	}
	/* Return the manufacturer id of this device if it exists.  Returns
	 * 0 if unknown.  A device with an unknown manufacturer ID might
	 * still have a valid Manufacturer String */
	const Uint16 getManufacturerID() const{
		return manufacturerID;
	}
	/* Return the device id of this device if it exists.  Returns 0 if
	 * unknown.  A device with an unknown device ID might still have a
	 * valid device String */
	const Uint16 getDeviceID() const{
		return deviceID;
	}

	/* Returns a string describing the manufacturer. */
	const String getManufacturerString() const {
		return manufacturerName;
	}
	/* Returns a string describing the device. */
	const String getDeviceString() const{
		return deviceName;
	}

	/* Set the base class ID of this device */
	void setBaseClass(Uint16 id){
		baseClass=id;
	}
	/* Set the sub class id of this device */
	void setSubClass(Uint16 id){
		subClass=id;
	}
	/* Set the prog if id of this device */
	void setProgIF(Uint16 id){
		progIF=id;
	}
	/* Set the manufacturer id of this device */
	void setManufacturerID(Uint16 id){
		manufacturerID=id;
	}
	/* Set the device id of this device */
	void setDeviceID(Uint16 id){
		deviceID=id;
	}
	/* Sets a string describing the manufacturer. */
	void setManufacturerString(String inManufacturer){
		manufacturerName=inManufacturer;
	}
	/* Sets the string describing the device. */
	void setDeviceString(String inDevice){
		deviceName=inDevice;
	}

	/* For convenience, we may wish to assign a unique key to any
         * located logical device.  This sets that value.  It is used only
         * to facilitate communication between the DeviceInformation and
         * the ProviderData classes, and need not be used if the programmer
         * has no need of it. */
	void setUniqueKey(String const &s) { unique_key = s; }

	/* Retrieves the unique key, if any. */
	String const &getUniqueKey(void) const { return unique_key; }

protected:

	/** We have created a superset to the 8-bit PCI base class
         *  definition in the PCI spec.  Additional codes have been
         *  introduced for concepts such as IP Routes.  Further, two
         *  wildcard values are added: 
	 *  0xffff - for any device at all
	 *  0xfffe - for any PCI device (baseClass <= 0xff).
	 */
	Uint16 baseClass;

	/** Just as with the baseClass, we have extended the subClass in
	 *  much the same way.  Only the 0xffff wildcard is defined for
	 *  subClasses. */
	Uint16 subClass;

	/** Same notes as the case of subClass. */
	Uint16 progIF;

	/** The 16-bit numeric value assigned to the manufacturer of this
	 *  device. */
	Uint16 manufacturerID;

	/** The manufacturer's 16-bit numeric device type code. */
	Uint16 deviceID;

	/** A string holding the manufacturer's real name. */
	String manufacturerName;

	/** A string holding the manufacturer's name for the device. */
	String deviceName;

private:
	/** Storage for the unique key. */
	String unique_key;
};

PEGASUS_NAMESPACE_END

#endif
