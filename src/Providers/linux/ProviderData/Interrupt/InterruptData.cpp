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


#include <Pegasus/Common/System.h>
#include "InterruptData.h"
#include "InterruptInformation.h"

PEGASUS_NAMESPACE_BEGIN

InterruptData::InterruptData(void)
{
	irq=NULL;
}

InterruptData::~InterruptData(void)
{
	if(irq) {
		delete irq;
	}
}

int InterruptData::initialize(void)
{
  return 0;
}

void InterruptData::terminate(void)
{
}

/** GetAllInterrupts(): Get all the curIRQs and put them in an array */
Array <InterruptData *> InterruptData::GetAllInterrupts(void)
{
	InterruptData *curIRQ;
	Array <InterruptData *> curIRQs;

	curIRQ = (InterruptData *) GetFirstInterrupt();
	while(curIRQ){
		curIRQs.append((InterruptData *)curIRQ);
		/* Need to keep curIRQs lingering because we are using an 
		   array of pointers to objects */
		curIRQ=(InterruptData *)GetNextInterrupt();
	}
	EndGetInterrupt();
	return curIRQs;
}

/** GetFirstInterrupt(): Get the first curIRQ in the database and return it */
InterruptData * InterruptData::GetFirstInterrupt(void)
{
	InterruptData *curData;
	/* Set up the search criteria */
	deviceLocator.setDeviceSearchCriteria(Device_SystemResources,Device_SystemResources_IRQ);
	curData=GetNextInterrupt();
	return(curData);
}

/** GetNextInterrupt(): Get the next curIRQ in the database */
InterruptData * InterruptData::GetNextInterrupt(void)
{
	InterruptData *curData;
	InterruptInformation *curInfo;
	curInfo=(InterruptInformation *)deviceLocator.getNextDevice();
	if(curInfo){
		curData=new InterruptData();
		curData->irq=curInfo;
		return(curData);
	}
	return NULL;

}

/** EndGetInterrupt(): Clean up the iterator. */
void InterruptData::EndGetInterrupt(void)
{
}

/** GetInterrupt(): Get a specific curIRQ. */
InterruptData * InterruptData::GetInterrupt(const String &inIrq)
{
	InterruptData *curInterrupt;
	curInterrupt=GetFirstInterrupt();
	while(curInterrupt){
		if(inIrq==curInterrupt->getIRQNumber()){
			EndGetInterrupt();
			return curInterrupt;
		}
		delete curInterrupt;
		curInterrupt=GetNextInterrupt();
	}
	return NULL;
}

/** getIRQNumber(): Return the irq number of this irq */
String InterruptData::getIRQNumber() const{
	if(irq){
		return irq->getIRQ();
	}
	return String::EMPTY;
}

/** getAvailability(): Return the availability of this irq */
String InterruptData::getAvailability() const{
	if(irq&&(irq->getOwner()==String::EMPTY)){
		return "TRUE";
	}
	return "FALSE";
}

/** getTriggerType(): Return the trigger type of this irq */
String InterruptData::getTriggerType() const{
	String irqType;

	if(!irq){
		return "2";
	}
	irqType=irq->getType();
	if(irqType==String::EMPTY){
		return "2";
	}
	if(irqType.find("edge")!=PEG_NOT_FOUND){
		return "4";
	}
	if(irqType.find("level")!=PEG_NOT_FOUND){
		return "3";
	}
	if(irqType.find("XT")!=PEG_NOT_FOUND){
		return "4";
	}
	return "2";
}


PEGASUS_NAMESPACE_END
