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
#include "IDELocatorPlugin.h"
#include "ProcessorInformation.h"
#include "FileReader.h"
#include "DeviceTypes.h"
#include <stdio.h>

PEGASUS_NAMESPACE_BEGIN

#define N_IN_ARRAY(x) ((int) (sizeof(x) / sizeof(x[0])))

static char const *trial_filename_patterns[] = { 
  "^/proc/ide/ide[0-9]/(hd[abcd])/capacity$",
  "^/proc/ide/ide[0-9]/(hd[abcd])/media$",
  "^/proc/ide/ide[0-9]/(hd[abcd])/model$",
  NULL
};

static char const *trial_regex_patterns[] = { 
  "^(.*)$",
  NULL
};

enum type_of_match {
  MATCH_CDROM,
  MATCH_FLOPPY,
  MATCH_DISK
};

static struct {
  const char *regex;
  enum type_of_match match_type;
} lookup_info[] = {
  { "cdrom", MATCH_CDROM },
  { "floppy", MATCH_FLOPPY },
  { "disk", MATCH_DISK }
};


IDELocatorPlugin::IDELocatorPlugin(){
	fileReader.reset();
}

/* Sets the device search criteria.
 * Returns 0 for no error or -1 if it is unable to search for that type. 
 * Calling this function resets the locator pointer */
int IDELocatorPlugin::setDeviceSearchCriteria(Uint16 base_class, Uint16 sub_class, Uint16 prog_if){

	/* Set the internal search criteria */
	baseClass=base_class;
	subClass=sub_class;
	progIF=prog_if;

	/* We will search for most anything */
	/* But right now we support floppies, cdroms and disks */


	/* If we aren't asked for a mass storage device, don't bother playing with the FileReader. */
	if(base_class != WILDCARD_DEVICE &&
	   base_class!=Device_MassStorageDevice) { 
		return -1;
	}

	/* Create a new file reader */
    fileReader.reset(new FileReader);
	if(fileReader.get()==NULL){
		return -1;
	}

	/* Because the ide information is in separate files don't play with the 
	 * file reader here */

	/* We are all set up ready to go! */
	return 0;
}

/* Returns a pointer to a DeviceInformation class or NULL if the last device of that type was located.
 * This method allocated the DeviceInformation object */
DeviceInformation *IDELocatorPlugin::getNextDevice(void){
  int dirIndex;
  vector<String> matches;
  MediaAccessDeviceInformation *curDeviceInformation=NULL;
  String roleString;

  /* Check to see if we have been set up correctly */
  if(!fileReader.get()) return NULL;

  /* We are looking for (another) file.  If we find a file then we grab the 
   * device name from that */
  while (fileReader->SearchFileRegexList(trial_filename_patterns, &dirIndex, matches) != NULL) {
    /* Ok, check to see if this is still the same device or a new one */
    if(curDevice!=matches[1]&&curDevice!=String::EMPTY){
      /* This is a different device, generate the device information */
      curDeviceInformation=createDevice();

      /* set the current device and return the previous one */
      curDevice=matches[1];
      /* Clear out the information */
      capacity=String::EMPTY;
      media=String::EMPTY;
      model=String::EMPTY;
      /* Set the current option */
      try {
        setOption(dirIndex);
      }
      catch (Exception&){
	delete curDeviceInformation;
        return NULL;
      }

      if (curDeviceInformation == NULL)  // got the wrong device, keep trying
	continue;

      return curDeviceInformation;
    }
    else {
      curDevice=matches[1];
    }

    /* Same device: Update our internal variables */
    setOption(dirIndex);
  }
  /* Return the last device */
  if(curDevice!=String::EMPTY){
  	curDeviceInformation=createDevice();
  	curDevice=String::EMPTY;
  	capacity=String::EMPTY;
  	media=String::EMPTY;
  	model=String::EMPTY;
	fileReader.reset();
  	return curDeviceInformation;
  }
  return NULL;
}


void IDELocatorPlugin::setOption(int dirIndex){
      String line;
      vector<String> matches;
      int fileIndex;
      fileReader->SetSearchRegexps(trial_regex_patterns);
      while (fileReader->GetNextMatchingLine(line, &fileIndex, matches) != -1) {
        switch(dirIndex){
        case 0: /* Capacity */
          capacity=matches[0];
          break;
        case 1: /* Media */
          media=matches[0];
          break;
        case 2: /* Model */
          model=matches[0];
          break;
        };
     }
}


MediaAccessDeviceInformation *IDELocatorPlugin::createDevice(void){
    AutoPtr<MediaAccessDeviceInformation> curDeviceInformation(new MediaAccessDeviceInformation()); 
        int i;
	/* Media */
	for(i=0;i<N_IN_ARRAY(lookup_info);i++){
		if(media==lookup_info[i].regex){
			switch(lookup_info[i].match_type){
			case MATCH_CDROM:
				curDeviceInformation->setSubClass(Device_MassStorageDevice_CDROM);
				break;
			case MATCH_FLOPPY:
				curDeviceInformation->setSubClass(Device_MassStorageDevice_Floppy);
				break;
			case MATCH_DISK:
				curDeviceInformation->setSubClass(Device_MassStorageDevice_Disk);
				break;
			default:
				break;
			}
		}
	}
	/* Model */
	curDeviceInformation->setName(curDevice);
	curDeviceInformation->setDescription(model);
	/* Capacity */
        unsigned long ul;
        ul = strtoul(capacity.getCString(), NULL, 10);
	curDeviceInformation->setMaxMediaSize(ul);

	/* Now see if the user actually wants this type of device */
	if((subClass!=WILDCARD_DEVICE)&&(curDeviceInformation->getSubClass()!=subClass)){
		return NULL;
	}
	return curDeviceInformation.get();
}

PEGASUS_NAMESPACE_END
