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


#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/String.h>
#include <Pegasus/Common/CIMDateTime.h>
#include <stdio.h>
#include "DebianPackageInformation.h"

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

PEGASUS_USING_STD;

PEGASUS_NAMESPACE_BEGIN

DebianPackageInformation::DebianPackageInformation(void){
	valid=FALSE;
	ID=String::EMPTY;
	Name=String::EMPTY;
	Version=String::EMPTY;
	State=String::EMPTY;
	InstallTime.clear();
	Maintainer=String::EMPTY;
	Description=String::EMPTY;

	/* Set the Architecture to the currnet architecture */
#if PEGASUS_LINUX_IA64
	Architecture="IA64";
#endif
}

DebianPackageInformation::~DebianPackageInformation(void){
}

String DebianPackageInformation::GetSoftwareElementID(void) const{
	return ID;
}

String DebianPackageInformation::GetName(void)  const{
	return Name;
}

String DebianPackageInformation::GetVersion(void) const{
	return Version;
}

String DebianPackageInformation::GetSoftwareElementState(void) const{
	return State;
}

CIMDateTime DebianPackageInformation::GetInstallTime(void) const{
	return InstallTime;
}

Uint16 DebianPackageInformation::GetTargetOperatingSystem(void) const{
	/* Linux */
	return /* PEGASUS_PLATFORM_ID */ 36;
}

String DebianPackageInformation::GetArchitecture(void) const{
	return Architecture;
}

String DebianPackageInformation::GetDistribution(void) const{
	return "Debian";
}

String DebianPackageInformation::GetMaintainer(void) const{
	return Maintainer;
}

String DebianPackageInformation::GetManufacturer(void) const{
	return Maintainer;
}

String DebianPackageInformation::GetCaption(void) const{
	return String::EMPTY;
}

String DebianPackageInformation::GetDescription(void) const{
	return Description;
}

String DebianPackageInformation::GetCopyright(void)  const{
	return String::EMPTY;
}

int DebianPackageInformation::isValid(void) const{
	return valid;
}


Array<String> DebianPackageInformation::GetFileList(void){
	Array<String> array1;
	return array1;
}


void DebianPackageInformation::add(String key,String value){
	int i;

	/* We are adding either a key or information to a key */
	valid=TRUE;

	/* First find the key */
	i=0;
	while(keys[i].key!=NULL){
		if(key==keys[i].key){
			switch(keys[i].rule){
        		case PARSE_PACKAGE:
				ID.append(value);
				Name.append(value);
				break;
        		case PARSE_STATUS:
				State.append(value);
				break;
        		case PARSE_MAINTAINER:
				Maintainer.append(value);
				Manufacturer.append(value);
				break;
        		case PARSE_ARCHITECTURE:
				Architecture.append(value);
				break;
        		case PARSE_VERSION:
				Version.append(value);
				break;
        		case PARSE_DESCRIPTION:
				Description.append(value);
				break;
			case PARSE_UNKNOWN:
			default:
				/* Silently ignore */
				break;
			}
			return;
		}
		i++;
	}

}

void DebianPackageInformation::debugDump(void) const{

	cout << "Package Dump:" << endl;
	cout << "valid" << valid << endl;
	cout << "ID" << ID << endl;
	cout << "Name" << Name << endl;
	cout << "Version" << Version << endl;
	cout << "State" << State << endl;
	cout << "Architecture" << Architecture << endl;
	cout << "Maintainer" << Maintainer << endl;
	cout << "Manufacturer" << Manufacturer << endl;
	cout << "Description" << Description << endl;
	cout << endl;
}

PEGASUS_NAMESPACE_END
