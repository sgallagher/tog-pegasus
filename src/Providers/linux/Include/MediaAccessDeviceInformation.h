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

class MediaAccessDeviceInformation : public DeviceInformation {

public:
	MediaAccessDeviceInformation(){
		 baseClass=Device_MassStorageDevice;
		 subClass=0;
		 progIF=0;
		 manufacturerID=0;
		 deviceID=0;
		 availability=0;
		 statusInfo=0;
		 maxMediaSize=0;
	}

	MediaAccessDeviceInformation(Uint16 inSub){
		baseClass=Device_MassStorageDevice;
		subClass=inSub;
		progIF=0;
	}

	void setSubClass(Uint16 value) {
		subClass=value;
	}
	Uint16 getSubClass(void) {
		return subClass;
	}

	void setAvailability(Uint16 value) {
		availability=value;
	}
	void setStatusInfo(Uint16 value) {
		statusInfo=value;
	}
	void setName(String value) {
		name=value;
	}
	void setDescription(String value) {
		description=value;
	}
	void setMaxMediaSize(Uint64 value) {
		maxMediaSize=value;
	}

	Uint16 getAvailability(void) const{
		return availability;
	}
	Uint16 getStatusInfo(void) const{
		return statusInfo;
	}
	String getName(void) const{
		return name;
	}
	String getDescription(void) const{
		return description;
	}
	Uint64 getMaxMediaSize(void) const{
		return maxMediaSize;
	}

protected:

	String description;
	String name;
	Uint16 statusInfo;
	Uint16 availability;
	Uint64 maxMediaSize;

};

typedef MediaAccessDeviceInformation * MediaAccessDeviceInformationPtr;

#if 0
#define PEGASUS_ARRAY_T MediaAccessDeviceInformationPtr
#include <Pegasus/Common/ArrayInter.h>
#undef PEGASUS_ARRAY_T

#define PEGASUS_ARRAY_T MediaAccessDeviceInformation
#include <Pegasus/Common/ArrayInter.h>
#undef PEGASUS_ARRAY_T
#endif

PEGASUS_NAMESPACE_END

#endif
