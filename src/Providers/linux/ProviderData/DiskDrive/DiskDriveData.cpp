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
#include "DiskDriveData.h"

PEGASUS_NAMESPACE_BEGIN

DiskDriveData::DiskDriveData(void)
{
}

DiskDriveData::~DiskDriveData(void)
{
}

int DiskDriveData::initialize(void)
{
  return 0;
}

void DiskDriveData::terminate(void)
{
}

#if 0
/* Get all the diskdrives and put them in an array */
Array <MediaAccessDeviceInformation *> DiskDriveData::GetAllDiskDrives(void)
{
	MediaAccessDeviceInformation *diskdrive;
	Array <MediaAccessDeviceInformation> diskdrives;

	diskdrive = (MediaAccessDeviceInformation *) GetFirstDiskDrive();
	while(diskdrive){
		diskdrives.append((MediaAccessDeviceInformation *) diskdrive);
		/* Need to keep diskdrives lingering because we are using an 
		   array of pointers to objects */
		diskdrive=(MediaAccessDeviceInformation *)GetNextDiskDrive();
	}
	EndGetDiskDrive();
	return diskdrives;
}
#endif

/* Get the first diskdrive in the database */
MediaAccessDeviceInformation * DiskDriveData::GetFirstDiskDrive(void)
{
	/* Set up the search criteria */
	deviceLocator.setDeviceSearchCriteria(Device_MassStorageDevice,Device_MassStorageDevice_Disk);
	return(GetNextDiskDrive());
}

/* Get the next diskdrive in the database */
MediaAccessDeviceInformation * DiskDriveData::GetNextDiskDrive(void)
{
	return((MediaAccessDeviceInformation *)deviceLocator.getNextDevice());

}

void DiskDriveData::EndGetDiskDrive(void)
{
}

/* Get one diskdrive. */
MediaAccessDeviceInformation * DiskDriveData::GetDiskDrive(const String &inName)
{
	MediaAccessDeviceInformation *curDiskDrive;
	curDiskDrive=GetFirstDiskDrive();
	while(curDiskDrive){
		if(inName==curDiskDrive->getName()){
			EndGetDiskDrive();
			return curDiskDrive;
		}
		delete curDiskDrive;
		curDiskDrive=GetNextDiskDrive();
	}
	return NULL;
}

PEGASUS_NAMESPACE_END
