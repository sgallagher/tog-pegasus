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

#ifndef Pegasus_LocatedProcessor_h
#define Pegasus_LocatedProcessor_h

#include <Pegasus/Common/Config.h>
#include "DeviceInformation.h"

#include "DeviceTypes.h"

PEGASUS_NAMESPACE_BEGIN

class ProcessorInformation : public DeviceInformation {

/* Note: subClass and progIF are unused for processors */

public:
	ProcessorInformation(){
		 baseClass=Device_Processor;
		 subClass=0;
		 progIF=0;
		 manufacturerID=0;
		 deviceID=0;
	}

	/* Set the processor type */
	ProcessorInformation(Uint16 inSub){
		baseClass=Device_Processor;
		subClass=inSub;
		progIF=0;
	}

	void setRole(String inValue){
		role=inValue;
	}
	void setFamily(Uint16 inValue){
		family=inValue;
	}
	void setUpdateMethod(Uint16 inValue){
		updateMethod=inValue;
	}
	void setMaxClockSpeed(Uint32 inValue){
		maxClockSpeed=inValue;
	}
	void setCurClockSpeed(Uint32 inValue){
		curClockSpeed=inValue;
	}
	void setDataWidth(Uint16 inValue){
		dataWidth=inValue;
	}
	void setAddressWidth(Uint16 inValue){
		addressWidth=inValue;
	}
	void setLoadPercentage(Uint16 inValue){
		loadPercentage=inValue;
	}
	void setStepping(Uint16 inValue){
		stepping=inValue;
	}
	void setUniqueID(String inValue){
		uniqueID=inValue;
	}
	void setCPUStatus(Uint16 inValue){
		cpuStatus=inValue;
	}

	String getRole(void) const{
		return role;
	}
	Uint16 getFamily(void) const{
		return family;
	}
	Uint16 getUpdateMethod(void) const{
		return updateMethod;
	}
	Uint32 getMaxClockSpeed(void) const{
		return maxClockSpeed;
	}
	Uint32 getCurClockSpeed(void) const{
		return curClockSpeed;
	}
	Uint16 getDataWidth(void) const{
		return dataWidth;
	}
	Uint16 getAddressWidth(void) const{
		return addressWidth;
	}
	Uint16 getLoadPercentage(void) const{
		return loadPercentage;
	}
	Uint16 getStepping(void) const{
		return stepping;
	}
	String getUniqueID(void) const{
		return uniqueID;
	}
	Uint16 getCPUStatus(void) const{
		return cpuStatus;
	}

private:

	String role;
	Uint16 family;
	Uint16 updateMethod;
	Uint32 maxClockSpeed;
	Uint32 curClockSpeed;
	Uint16 dataWidth;
	Uint16 addressWidth;
	Uint16 loadPercentage;
	Uint16 stepping;
	String uniqueID;
	Uint16 cpuStatus;
	

};

PEGASUS_NAMESPACE_END

#endif
