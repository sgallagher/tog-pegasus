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
#include <Pegasus/Common/Logger.h>
#include "DebianPackageManagerData.h"

#include <strings.h>

PEGASUS_NAMESPACE_BEGIN

#define DEBUG(X) Logger::put(Logger::DEBUG_LOG, "DebianPackageManagerData", Logger::INFORMATION, "$0", X)

DebianPackageManagerData::DebianPackageManagerData(void)
{
	statusFile=NULL;
	databaseDirectory=DEFAULT_DEBIAN_DIRECTORY;
}

DebianPackageManagerData::DebianPackageManagerData(const String &inDatabaseDirectory)
{
	statusFile=NULL;
	databaseDirectory=inDatabaseDirectory;
}


DebianPackageManagerData::~DebianPackageManagerData(void)
{
	if(statusFile)
		CloseStatusFile(statusFile);
	statusFile=NULL;
		
}

int DebianPackageManagerData::initialize(void)
{
  return 0;
}

void DebianPackageManagerData::terminate(void)
{
}

/* Get all the packages and put them in an array */
Array <PackageInformation *> DebianPackageManagerData::GetAllPackages(void)
{
	DebianPackageInformation *package;
	Array <PackageInformation *> packages;

	package = (DebianPackageInformation *) GetFirstPackage();
	while(package&&package->isValid()){
		packages.append((PackageInformation *)package);
		/* Need to keep packages lingering because we are using an 
		   array of pointers to objects */
		package=(DebianPackageInformation *)GetNextPackage();
	}
	EndGetPackage();
	return packages;
}

/* Get the first package in the database */
PackageInformation * DebianPackageManagerData::GetFirstPackage(void)
{
	String statusFilename(databaseDirectory);
	if(statusFile!=NULL){
		CloseStatusFile(statusFile);
		statusFile=NULL;
	}
	statusFilename.append('/');
	statusFilename.append(DEBIAN_STATUS_FILENAME);
	statusFile=OpenStatusFile(statusFilename);
	if(statusFile==NULL){
		return NULL;
	}
	return(GetNextPackage());

}

/* Get the next package in the database */
PackageInformation * DebianPackageManagerData::GetNextPackage(void)
{
/*

The select file is in the form of:

Package: syslinux
Status: install ok installed
Priority: optional
Section: base
Installed-Size: 206
Maintainer: Juan Cespedes <cespedes@debian.org>
Version: 1.63-1
Depends: libc6 (>= 2.2.3-7)
Suggests: lilo (>= 20)
Description: Bootloader for Linux/i386 using MS-DOS floppies
 SYSLINUX is a boot loader for the Linux/i386 operating system which
 operates off an MS-DOS/Windows FAT filesystem.  It is intended to
 simplify first-time installation of Linux, and for creation of rescue
 and other special-purpose boot disks.
 .
 SYSLINUX is probably not suitable as a general purpose boot loader.
 However, SYSLINUX has shown itself to be quite useful in a number of
 special-purpose applications.
 .
 You will need support for `msdos' filesystem in order to use this program


To get packages: search for any line that starts with 'Package'
*/

/* Parse the file looking for the keys (delimited by ':') */
	return(ReadStatusPackage(statusFile));

}

void DebianPackageManagerData::EndGetPackage(void)
{
	if(statusFile)
		CloseStatusFile(statusFile);
	statusFile=NULL;
}

PackageInformation * DebianPackageManagerData::GetPackage(const String &inName, const String &inVersion)
{
	DebianPackageInformation * curPackage;

	String junk;
	junk = "dpmd-> look for package " + inName + ", version " + inVersion;
	DEBUG(junk);
	curPackage=(DebianPackageInformation *)GetFirstPackage();
	while(curPackage&&curPackage->isValid()){
		if (String::equal(curPackage->GetName(), inName) &&
		    String::equal(curPackage->GetVersion(), inVersion))
		{
			EndGetPackage();
		        DEBUG("dpmd-> FOUND package " + curPackage->GetName());
			return curPackage;
		}
		junk = "dpmd-> not package " + curPackage->GetName();
		junk.append(", version " + curPackage->GetVersion());
		DEBUG(junk);
		delete curPackage;
		curPackage=(DebianPackageInformation *)GetNextPackage();
	}
	if (curPackage) {
	  delete curPackage;
	  curPackage = NULL;
	}
	EndGetPackage();
	return NULL;
}

FILE * DebianPackageManagerData::OpenStatusFile(const String &filename){
	FILE * retval;
	retval= fopen(filename.getCString(),"r");
	return retval;
}

int DebianPackageManagerData::CloseStatusFile(FILE * handle){
	int retval=0;
	if(handle)
		retval=fclose(handle);
	handle=NULL;
	return retval;
}

String DebianPackageManagerData::ReadStatusLine(FILE * handle){
	char *result;
	char *returnLoc;
	static char buffer[BUFSIZ];
	String resultString;
	
	/* Assume that no line is longer than BUFSIZ */
	result=fgets(buffer,BUFSIZ,handle);

	if(result==NULL){
		return String::EMPTY;
	}
	/* Remove the trailing whitespace */
	returnLoc=rindex(buffer,'\n');
	if(returnLoc){
		*returnLoc='\0';
	}
	resultString.assign(buffer);
	return resultString;
	
}

/* Read in an entire package and return it */
DebianPackageInformation * DebianPackageManagerData::ReadStatusPackage(FILE * handle){
	String nextLine;
	enum parseStatus {NONE, LINE, DESCRIPTION } parser = LINE;
	unsigned int colonLocation;
	DebianPackageInformation *curPackage;
	String curKey;

	/* Assume that we are at the beginning of a package */
	nextLine=ReadStatusLine(handle);

	curPackage=new DebianPackageInformation;

	while((nextLine!=String::EMPTY)&&(nextLine!="")&&(nextLine!="\n")){
		/* Parse up the line  */
		switch(parser){
		case LINE:
			/* We are doing line by line reading */
			if(nextLine[0]!=' '){ /* If this is a description, let if fall through */
				colonLocation=nextLine.find(':');
				if(colonLocation==PEG_NOT_FOUND){
					/* parse error */
					delete curPackage;
					return NULL;
				}
				curKey=nextLine.subString(0,colonLocation);
				curPackage->add(curKey,nextLine.subString(colonLocation+2));
			
				break;
			}
			/* No break because the if falls through */
		case DESCRIPTION:
			/* We are reading in the description (multi line) */
			parser=DESCRIPTION;
			if(nextLine!="\n"){
				curPackage->add(curKey,nextLine);
			}
			else {
				parser=LINE;
				return curPackage;
			}
			break;
		default:
			/* A parsing error occurred */
			delete curPackage;
			return NULL;
		}
		nextLine=ReadStatusLine(handle);

	}
	return curPackage;
	
}

PEGASUS_NAMESPACE_END
