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
#include "Ix86ProcessorLocatorPlugin.h"
#include "ProcessorInformation.h"
#include "FileReader.h"
#include "DeviceTypes.h"
#include <stdio.h>

PEGASUS_NAMESPACE_BEGIN

/* The processor numbers:

"1", "Other", 
"2", "Unknown",
"3", "8086",
"4", "80286",
"5", "80386",
"6", "80486",
"7", "8087",
"8", "80287",
"9", "80387",
"10", "80487",
"11", "Pentium(R) brand", 
"12", "Pentium(R) Pro",
"13", "Pentium(R) II",
"14", "Pentium(R) processor with MMX(TM) technology", 
"15", "Celeron(TM)",
"16", "Pentium(R) II Xeon(TM)",
"17", "Pentium(R) III",
"18", "M1 Family",
"19", "M2 Family",
"24", "K5 Family",
"25", "K6 Family",
"26", "K6-2",
"27", "K6-3",
"28", "AMD Athlon(TM) Processor Family",
"29", "AMD(R) Duron(TM) Processor",
"30", "AMD29000 Family",
"31", "K6-2+",
"32", "Power PC Family", 
"33", "Power PC 601",
"34", "Power PC 603",
"35", "Power PC 603+",
"36", "Power PC 604",
"37", "Power PC 620",
"38", "Power PC X704",
"39", "Power PC 750",
"48", "Alpha Family",
"49", "Alpha 21064",
"50", "Alpha 21066",
"51", "Alpha 21164",
"52", "Alpha 21164PC",
"53", "Alpha 21164a",
"54", "Alpha 21264",
"55", "Alpha 21364",
"64", "MIPS Family",
"65", "MIPS R4000",
"66", "MIPS R4200",
"67", "MIPS R4400",
"68", "MIPS R4600",
"69", "MIPS R10000",
"80", "SPARC Family",
"81", "SuperSPARC",
"82", "microSPARC II",
"83", "microSPARC IIep",
"84", "UltraSPARC",
"85", "UltraSPARC II",
"86", "UltraSPARC IIi",
"87", "UltraSPARC III",
"88", "UltraSPARC IIIi",
"96", "68040",
"97", "68xxx Family",
"98", "68000",
"99", "68010",
"100", "68020",
"101", "68030",
"112", "Hobbit Family",
"120", "Crusoe(TM) TM5000 Family",
"121", "Crusoe(TM) TM3000 Family",
"128", "Weitek",
"130", "Itanium(TM) Processor",
"144", "PA-RISC Family",
"145", "PA-RISC 8500",
"146", "PA-RISC 8000",
"147", "PA-RISC 7300LC",
"148", "PA-RISC 7200",
"149", "PA-RISC 7100LC",
"150", "PA-RISC 7100",
"160", "V30 Family",
"176", "Pentium(R) III Xeon(TM)",
"177", "Pentium(R) III Processor with Intel(R) SpeedStep(TM) Technology",
"180", "AS400 Family",
"200", "IBM390 Family",
"201", "G4",
"202", "G5",
"250", "i860",
"251", "i960",
"260", "SH-3",
"261", "SH-4",
"280", "ARM",
"281", "StrongARM",
"300", "6x86",
"301", "MediaGX",
"302", "MII",
"320", "WinChip",
"350", "DSP",
"500", "Video Processor",

*/

#define N_IN_ARRAY(x) ((int) (sizeof(x) / sizeof(x[0])))

static char const *trial_filename_patterns[] = { 
  "^/proc/cpuinfo$",
  NULL
};

enum type_of_match {
  MATCH_PROCESSOR_BEGIN, MATCH_PROCESSOR_END, 
  MATCH_CPU_MANUFACTURER,
  MATCH_CPU_FAMILY,
  MATCH_CPU_MODEL,
  MATCH_CPU_MODEL_NAME,
  MATCH_CPU_STEPPING,
  MATCH_CPU_SPEED,
  MATCH_FOOF_BUG
};

static struct {
  const char *regex;
  enum type_of_match match_type;
} lookup_info[] = {
  { "^processor[ \t]+: +([0-9]+)[ \t]*$",
    MATCH_PROCESSOR_BEGIN
  },
  {
    "^[ \t]*$",
    MATCH_PROCESSOR_END
  },
  {
    "^vendor_id[ \t]+: +(.*)$",
    MATCH_CPU_MANUFACTURER
  },
  {
    "^family[ \t]+: +([0-9]+)$",
    MATCH_CPU_FAMILY
  },
  {
    "^cpu family[ \t]+: +([0-9]+)$",
    MATCH_CPU_FAMILY
  },
  {
    "^stepping[ \t]+: +([0-9]+)[ \t]*$",
    MATCH_CPU_STEPPING
  },
  {
    "^cpu MHz[ \t]+: +([0-9])+[ \t]*$",
    MATCH_CPU_SPEED
  },
  {
    "^cpu MHz[ \t]+: +([0-9]+\\.[0-9]*)[ \t]*$",
    MATCH_CPU_SPEED
  },
  {
    "^model[ \t]+: +([0-9]+)$",
    MATCH_CPU_MODEL
  },
  {
    "^model name[ \t]+: +(.*)$",
    MATCH_CPU_MODEL_NAME
  },
  {
    "^f00f_bug[ \t]+: +(.*)$",
    MATCH_FOOF_BUG
  },
};


Ix86ProcessorLocatorPlugin::Ix86ProcessorLocatorPlugin(){
    fileReader.reset();
}

/* Sets the device search criteria.
 * Returns 0 for no error or -1 if it is unable to search for that type. 
 * Calling this function resets the locator pointer */
int Ix86ProcessorLocatorPlugin::setDeviceSearchCriteria(Uint16 base_class, Uint16 sub_class, Uint16 prog_if){

	/* Set the internal search criteria */
	baseClass=base_class;
	subClass=sub_class;
	progIF=prog_if;

	int index;
	char const * regex_patts[N_IN_ARRAY(lookup_info) + 1];

	/* We only search for processors */
	/* If we aren't asked for a processor, don't bother playing with the FileReader. */
	if(base_class != WILDCARD_DEVICE &&
	   base_class!=Device_Processor) { 
		return -1;
	}

	/* Create a new file reader */
    fileReader.reset(new FileReader);
	if(fileReader.get()==NULL){
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
	catch (Exception & e) {
		/* Error compiling regexs */
		return -1;
	}


	/* We are all set up ready to go! */
	return 0;
}

/* Returns a pointer to a DeviceInformation class or NULL if the last device of that type was located.
 * This method allocated the DeviceInformation object */
DeviceInformation *Ix86ProcessorLocatorPlugin::getNextDevice(void){
  int index;
  String line;
  vector<String> matches;
  AutoPtr<ProcessorInformation> curDevice; 
  String roleString;
  int found_x86_processor=0;


  /* Check to see if we have been set up correctly */
  if(!fileReader.get()) return NULL;

  while (fileReader->GetNextMatchingLine(line, &index, matches) != -1) {

    switch(lookup_info[index].match_type) {
    case MATCH_FOOF_BUG:
      found_x86_processor=1;
      break;
    case MATCH_PROCESSOR_BEGIN:
      curDevice.reset(new ProcessorInformation());
      roleString="Processor ";
      roleString.append(matches[1]);
      curDevice->setRole(roleString);
      /* Ix86 processors are 32 bit */
      curDevice->setDataWidth(32);
      curDevice->setAddressWidth(32);
      /* If it is in cpuinfo then it is active */
      curDevice->setCPUStatus(1);

      break;
    case MATCH_CPU_MODEL_NAME:
      if(curDevice.get()){
      	if(matches[1]=="Mobile Pentium II"){
		curDevice->setFamily(13); /* Pentium II */
	}
      	else if(matches[1]=="Pentium III (Coppermine)"){
		curDevice->setFamily(17); /* Pentium III */
	}
	else {
		curDevice->setFamily(2); /* Unknown */
	}
	curDevice->setDeviceString(matches[1]);
      }
      break;
    case MATCH_PROCESSOR_END:
      if(found_x86_processor==1){
        return curDevice.release();
      }
      if(curDevice.get()) {
        curDevice.reset();
      }
      return NULL;
      break;
    case MATCH_CPU_MANUFACTURER:
      if(curDevice.get()){
        curDevice->setManufacturerString(matches[1]);
      }
      break;
    case MATCH_CPU_STEPPING:
      if(curDevice.get())
      {
        unsigned long ul;
        ul = strtoul(matches[1].getCString(), NULL, 10);
	curDevice->setStepping(ul);
      }
      break;
    case MATCH_CPU_SPEED:
      if(curDevice.get()){
        unsigned long ul;
        ul = strtoul(matches[1].getCString(), NULL, 10);
	curDevice->setMaxClockSpeed(ul);
	curDevice->setCurClockSpeed(ul);
      }
      break;

    }
  }   
  return NULL;
}


PEGASUS_NAMESPACE_END
