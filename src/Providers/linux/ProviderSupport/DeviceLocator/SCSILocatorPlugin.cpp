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
//


#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/String.h>
#include <Pegasus/Provider/ProviderException.h>
#include "SCSILocatorPlugin.h"
#include "MediaAccessDeviceInformation.h"
#include "FileReader.h"
#include "DeviceTypes.h"
#include <stdio.h>

PEGASUS_NAMESPACE_BEGIN

#define N_IN_ARRAY(x) ((int) (sizeof(x) / sizeof(x[0])))

static char const *trial_filename_patterns[] = { 
  "^/proc/scsi/scsi$",
  NULL
};

enum type_of_match {
  MATCH_SCSI_HOST,
  MATCH_SCSI_VENDOR,
  MATCH_SCSI_TYPE
};

static struct {
  const char *regex;
  enum type_of_match match_type;
} lookup_info[] = {
  { "^Host: +(.+) +Channel: +(.+) +Id: +(.+) +Lun: +(.+)$",
    MATCH_SCSI_HOST
  },
  {
    "^ +Vendor: +(.+) +Model: +(.+) +Rev: +(.+)$",
    MATCH_SCSI_VENDOR
  },
  {
    "^ +Type: +([^ ]+) +ANSI SCSI revision: +(.+)$",
    MATCH_SCSI_TYPE
  }
};

static struct {
  const char *name;
  Uint16 subClass;
} device_type_list[] = {
  { "Direct-Access", Device_MassStorageDevice_Disk },
  { "Sequential-Access", Device_MassStorageDevice_Tape },
  { "CD-ROM", Device_MassStorageDevice_CDROM }
};


SCSILocatorPlugin::SCSILocatorPlugin(){
	fileReader.reset();
}

/* Sets the device search criteria.
 * Returns 0 for no error or -1 if it is unable to search for that type. 
 * Calling this function resets the locator pointer */
int SCSILocatorPlugin::setDeviceSearchCriteria(Uint16 base_class, Uint16 sub_class, Uint16 prog_if){

	/* Set the internal search criteria */
	baseClass=base_class;
	subClass=sub_class;
	progIF=prog_if;

	int index;
	char const * regex_patts[N_IN_ARRAY(lookup_info) + 1];

	/* We only search for mass storage devices */
	if(base_class != WILDCARD_DEVICE &&
	   base_class!=Device_MassStorageDevice) { 
		return -1;
	}

	/* Create a new file reader */
    fileReader.reset(new FileReader);
	if((fileReader.get())==NULL){
		return -1;
	}

	/* Because the Processor information is only in one file so we can search for 
	 * and grab that file now. */
	if (fileReader->SearchFileRegexList(trial_filename_patterns, &index) == NULL) {
		/* Didn't find any files */
		return -1;
	}
	/* Check to see that we found the file we are looking for */
	if(index!=0){
		return -1;
	}

	int i;
	/* Set up the regex array */
	for (i = 0; i < N_IN_ARRAY(lookup_info); i++)
		regex_patts[i] = lookup_info[i].regex;
	regex_patts[i] = NULL;

	/* Again, this can be here because it's only one file and it of known format */
	/* Compile the regexs for use in getNextDevice */
	try {
		fileReader->SetSearchRegexps(regex_patts);
	}
	catch (Exception&) {
		/* Error compiling regexs */
		return -1;
	}


	/* We are all set up ready to go! */
	return 0;
}

/* Returns a pointer to a Information class or NULL if the last device of that type was located.
 * This method allocated the Information object */
DeviceInformation *SCSILocatorPlugin::getNextDevice(void){
  int index;
  String line;
  vector<String> matches;
  AutoPtr<MediaAccessDeviceInformation> curDevice; 
  String descriptionString;
  int i;

  /* Check to see if we have been set up correctly */
  if(!fileReader.get()) return NULL;

  while (fileReader->GetNextMatchingLine(line, &index, matches) != -1) {

    switch(lookup_info[index].match_type) {
    case MATCH_SCSI_HOST:
      curDevice.reset(new MediaAccessDeviceInformation);
      curDevice->setName(matches[1]);
      break;
    case MATCH_SCSI_VENDOR:
      /* Description is: manufacturer model revision */
      if (curDevice.get() == NULL)
        // ATTN: Add more useful failure explanation
        throw CIMOperationFailedException("Bad format");
	
      descriptionString.append(matches[1]);
      descriptionString.append(" ");
      descriptionString.append(matches[2]);
      descriptionString.append(" ");
      descriptionString.append(matches[3]);
      curDevice->setDescription(descriptionString);
      break;
    case MATCH_SCSI_TYPE:
      if(curDevice.get()){
	for(i=0;i<N_IN_ARRAY(device_type_list);i++){
      	  if(matches[1]==device_type_list[i].name){
	    curDevice->setSubClass(device_type_list[i].subClass);
          }
	}
        /* See if we are to return this device or not */
        if(subClass==WILDCARD_DEVICE||subClass==curDevice->getSubClass()){
          return curDevice.release();
        }
        else {
	  continue;
        }
      } else {
        // ATTN: Add more useful failure explanation
        throw CIMOperationFailedException("Bad format");
      }

      break;
    }
  }   
  return NULL;
}


PEGASUS_NAMESPACE_END
