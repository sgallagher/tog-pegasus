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

#ifndef Pegasus_InterruptInformation_h
#define Pegasus_InterruptInformation_h

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
#include <Pegasus/Common/String.h>

/* Device list */

#include "DeviceTypes.h"
#include "DeviceInformation.h"

PEGASUS_NAMESPACE_BEGIN

class InterruptInformation : public DeviceInformation{

public:
	InterruptInformation(){
		 baseClass=Device_SystemResources;
		 subClass=Device_SystemResources_IRQ;
		 progIF=0;
		 manufacturerID=0;
		 deviceID=0;
	}

	InterruptInformation(String inIrq){
		 baseClass=Device_SystemResources;
		 subClass=Device_SystemResources_IRQ;
		 progIF=0;
		 manufacturerID=0;
		 IRQ=inIrq;
	}

	InterruptInformation(const InterruptInformation & inIRQ) :DeviceInformation(){
		baseClass=inIRQ.baseClass;
		subClass=inIRQ.subClass;
		progIF=inIRQ.progIF;
		manufacturerID=inIRQ.manufacturerID;
		IRQ=inIRQ.IRQ;
		type=inIRQ.type;
		owner=inIRQ.owner;
	}


	void setIRQ (String value){
		IRQ=value;
	}
	void setType (String value){
		type=value;
	}
	void setOwner (String value){
		owner=value;
	}

	String getIRQ (void){
		return IRQ;
	}
	String getType (void){
		return type;
	}
	String getOwner (void){
		return owner;
	}

protected:

	String IRQ;
	String type;
	String owner;

};

PEGASUS_NAMESPACE_END

#endif
