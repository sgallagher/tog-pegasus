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
//
//  A sample use of the FileReader class.
//
//  We want to retrieve the processor number and cpu MHz for all CPUs on
//  the box


#include <Pegasus/Common/String.h>
#include "IRQLocatorPlugin.h"
#include "InterruptInformation.h"
#include "FileReader.h"
#include "DeviceTypes.h"
#include <stdio.h>

PEGASUS_NAMESPACE_BEGIN

#define N_IN_ARRAY(x) ((int) (sizeof(x) / sizeof(x[0])))

static char const *trial_filename_patterns[] = { 
  "^/proc/irq/([0-9]+) *$",
  NULL
};

static char const *interrupt_filename_patterns[] = { 
  "^/proc/interrupts$",
  NULL
};

static char const *interrupt_regex_patterns[] = { 
  "^ *([0-9]+):([ \t]*([0-9]+))*[ \t]+([^ \t]+)[ \t]+(.+)$",
  NULL
};


IRQLocatorPlugin::IRQLocatorPlugin(){
	fileReader.reset();
}

/* Sets the device search criteria.
 * Returns 0 for no error or -1 if it is unable to search for that type. 
 * Calling this function resets the locator pointer */
int IRQLocatorPlugin::setDeviceSearchCriteria(Uint16 base_class, Uint16 sub_class, Uint16 prog_if) {

	int index;
	int dirIndex;
	vector<String> matches;
	String line;

	/* Set the internal search criteria */
	baseClass=base_class;
	subClass=sub_class;
	progIF=prog_if;

	/* We only search for irqs */
	if(base_class != WILDCARD_DEVICE &&
	   base_class!=Device_SystemResources) { 
		return -1;
	}
	/* We only search for irqs */
	if(sub_class != WILDCARD_DEVICE &&
	   sub_class!=Device_SystemResources_IRQ) { 
		return -1;
	}

	/* Create a new file reader */
    fileReader.reset(new FileReader);
	if(fileReader.get()==NULL){
		return -1;
	}

	/* enumerate all the interrupts */
	/* one IRQ per file in Linux 2.4 */
	while (fileReader->SearchFileRegexList(trial_filename_patterns, &dirIndex, matches) != NULL) {
		/* New irq */
		addInterrupt(matches[1]);
	}
	/* Now we have all the available irqs */
	if (fileReader->SearchFileRegexList(interrupt_filename_patterns, &dirIndex, matches) == NULL) {
		/* Don't have /proc/interrupts */
		return -1;
	}
  	/* Compile the regexs for use in getNextDevice */
  	try {
	  	fileReader->SetSearchRegexps(interrupt_regex_patterns);
  	}
  	catch (Exception&) {
	  	/* Error compiling regexs */
	  	return -1;
  	}
	/* Now add in all the currently used interrupts */
    	while (fileReader->GetNextMatchingLine(line, &index, matches) != -1) {
		addInterrupt(matches[1],matches[4],matches[5]);
	}
	irqIterator=irqs.begin();
	
	/* We are all set up ready to go! */
	return 0;
}

/* Returns a pointer to a DeviceInformation class or NULL if the last device of that type was located.
 * This method allocated the LocatedDevice object */
DeviceInformation *IRQLocatorPlugin::getNextDevice(void){
    AutoPtr<InterruptInformation> curIRQ; 

	if(irqIterator!=irqs.end()){
        curIRQ.reset(new InterruptInformation(*irqIterator));
		irqIterator++;
		return curIRQ.release();
		
	}
	return NULL;
}

void IRQLocatorPlugin::addInterrupt(String irq, String type, String owner){
	vector<InterruptInformation>::iterator i;

	i=irqs.begin();
	while(i!=irqs.end()){
		if(i->getIRQ()==irq){
			/* We found the appropriate interrupt */
			i->setType(type);
			i->setOwner(owner);
			return;
		}
		i++;
	}
	/* Didn't find it.. at it to the end */
	InterruptInformation curIRQ(irq);
	curIRQ.setType(type);
	curIRQ.setOwner(owner);
	irqs.push_back(curIRQ);
	
}

PEGASUS_NAMESPACE_END
