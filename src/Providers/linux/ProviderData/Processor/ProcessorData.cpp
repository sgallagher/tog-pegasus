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
#include "ProcessorData.h"

PEGASUS_NAMESPACE_BEGIN

ProcessorData::ProcessorData(void)
{
}

ProcessorData::~ProcessorData(void)
{
}

int ProcessorData::initialize(void)
{
  return 0;
}

void ProcessorData::terminate(void)
{
}

/* Get all the processors and put them in an array */
Array <ProcessorInformation *> ProcessorData::GetAllProcessors(void)
{
	ProcessorInformation *processor;
	Array <ProcessorInformation *> processors;

	processor = (ProcessorInformation *) GetFirstProcessor();
	while(processor){
		processors.append((ProcessorInformation *)processor);
		/* Need to keep processors lingering because we are using an 
		   array of pointers to objects */
		processor=(ProcessorInformation *)GetNextProcessor();
	}
	EndGetProcessor();
	return processors;
}

/* Get the first processor in the database */
ProcessorInformation * ProcessorData::GetFirstProcessor(void)
{
	/* Set up the search criteria */
	deviceLocator.setDeviceSearchCriteria(Device_Processor);
	return(GetNextProcessor());
}

/* Get the next processor in the database */
ProcessorInformation * ProcessorData::GetNextProcessor(void)
{
	return((ProcessorInformation *)deviceLocator.getNextDevice());

}

void ProcessorData::EndGetProcessor(void)
{
}

/* Get one processor. */
ProcessorInformation * ProcessorData::GetProcessor(const String &role)
{
	ProcessorInformation *curProcessor;
	/* The role is in the form of: 'Processor 0' */
	curProcessor=GetFirstProcessor();
	while(curProcessor){
		if(role==curProcessor->getRole()){
			EndGetProcessor();
			return curProcessor;
		}
		delete curProcessor;
		curProcessor=GetNextProcessor();
	}
	return NULL;
}

PEGASUS_NAMESPACE_END
