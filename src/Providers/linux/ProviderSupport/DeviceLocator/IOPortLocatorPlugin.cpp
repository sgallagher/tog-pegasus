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
#include "IOPortLocatorPlugin.h"
#include "IOPortInformation.h"
#include "FileReader.h"
#include "DeviceTypes.h"
#include <stdio.h>

PEGASUS_NAMESPACE_BEGIN

/** trial_filename_patterns: The list of files to search for io ports and memory mapped io */
static char const *trial_filename_patterns[] = { 
  "^/proc/ioports$",
  "^/proc/iomem$",
  NULL
};

/** trial_regex_patterns: The regex to extract the appropriate information from 
 * the located files.  This regex determines the start and ending io address and 
 * the name of the subsystem that currently allocates that range. */
static char const *trial_regex_patterns[] = { 
  "^([^ -]+)-([^ -]+) +: +(.*)$",
  NULL
};


IOPortLocatorPlugin::IOPortLocatorPlugin(){
}

/* Sets the device search criteria.
 * Returns 0 for no error or -1 if it is unable to search for that type. 
 * Calling this function resets the locator pointer */
int IOPortLocatorPlugin::setDeviceSearchCriteria(Uint16 base_class, Uint16 sub_class, Uint16 prog_if){

	/* Set the internal search criteria */
	baseClass=base_class;
	subClass=sub_class;
	progIF=prog_if;

	/* We only search for mass storage devices */
	if(base_class != WILDCARD_DEVICE &&
	   base_class!=Device_SystemResources) { 
		return -1;
	}

	/* Create a new file reader */
    fileReader.reset(new FileReader);
    if(fileReader.get()==NULL){
		return -1;
	}

	/* Because the IOPort information could be in more than one file,
	 * don't play here */
	if (fileReader->SearchFileRegexList(trial_filename_patterns, &dirIndex) == NULL) {
		/* Didn't find any files */
		return -1;
	}
  	/* Compile the regexs for use in getNextDevice */
  	try {
	  	fileReader->SetSearchRegexps(trial_regex_patterns);
  	}
  	catch (Exception&) {
	  	/* Error compiling regexs */
	  	return -1;
  	}
	/* We are all set up ready to go! */
	return 0;
}

/* Returns a pointer to a DeviceInformation class or NULL if the last device of that type was located.
 * This method allocated the DeviceInformation object */
DeviceInformation *IOPortLocatorPlugin::getNextDevice(void){
  int index;
  String line;
  vector<String> matches;
  AutoPtr<IOPortInformation> curDevice; 
  String descriptionString;

  /* Check to see if we have been set up correctly */
  if(!fileReader.get()) return NULL;

  do {
	
    while (fileReader->GetNextMatchingLine(line, &index, matches) != -1) {
	/* We found a line.  Each line in the file is an entry */
	curDevice.reset(new IOPortInformation());
	if(dirIndex==0&&(subClass==Device_SystemResources_IOPort||subClass==WILDCARD_DEVICE)){
		curDevice->setSubClass(Device_SystemResources_IOPort);
		curDevice->setIOPortMapped();
	}
	else if(dirIndex==1&&(subClass==Device_SystemResources_IOMemory||subClass==WILDCARD_DEVICE)){
		curDevice->setSubClass(Device_SystemResources_IOMemory);
		curDevice->setMemoryMapped();
	}
	else {
		continue;
	}
	curDevice->setStartingAddress(matches[1]);
	curDevice->setEndingAddress(matches[2]);
	curDevice->setOwner(matches[3]);
	return curDevice.release();
    }
  } while (fileReader->SearchFileRegexList(trial_filename_patterns, &dirIndex, matches) != NULL);

  return NULL;
}


PEGASUS_NAMESPACE_END
