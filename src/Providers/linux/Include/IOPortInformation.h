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

#ifndef Pegasus_MediaInformation_h
#define Pegasus_MediaInformation_h

#include <Pegasus/Common/Config.h>
#include "DeviceInformation.h"

#include "DeviceTypes.h"

PEGASUS_NAMESPACE_BEGIN

class IOPortInformation : public DeviceInformation {

public:
	IOPortInformation(){
		 baseClass=Device_SystemResources;
		 subClass=0;
		 progIF=0;
		 manufacturerID=0;
		 deviceID=0;
	}

	/* Set the IO port type */
	IOPortInformation(Uint16 inSub){
		baseClass=Device_SystemResources;
		subClass=inSub;
		progIF=0;
	}

	void setSubClass(Uint16 value) {
		subClass=value;
	}
	Uint16 getSubClass(void) {
		return subClass;
	}

	void setStartingAddress(String value) {
		startingAddress=value;
	}
	void setEndingAddress(String value) {
		endingAddress=value;
	}
	void setMappedResource(String value) {
		mappedResource=value;
	}
	void setMemoryMapped(void) {
		mappedResource="2";
	}
	void setIOPortMapped(void) {
		mappedResource="3";
	}
	void setOwner(String value) {
		owner=value;
	}

	String getStartingAddress(void) const{
		return startingAddress;
	}
	String getEndingAddress(void) const{
		return endingAddress;
	}
	String getMappedResource(void) const{
		return mappedResource;
	}
	String getOwner(void) const{
		return owner;
	}

protected:

	String startingAddress;
	String endingAddress;
	String mappedResource;
	String owner;

};

PEGASUS_NAMESPACE_END

#endif
