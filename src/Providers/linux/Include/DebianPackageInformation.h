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

#ifndef DebianPackageInformation_h
#define DebianPackageInformation_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/String.h>
#include <Pegasus/Common/CIMDateTime.h>
#include "PackageInformation.h"

PEGASUS_NAMESPACE_BEGIN

enum e_parsingRule { 
	PARSE_UNKNOWN, 
	PARSE_PACKAGE, 
	PARSE_STATUS, 
	PARSE_MAINTAINER, 
	PARSE_ARCHITECTURE, 
	PARSE_VERSION, 
	PARSE_DESCRIPTION } ;

struct keys_s{ 
	const char * key; 
	enum e_parsingRule rule;
};

static const struct keys_s keys[] = {
	{ "Unknown", PARSE_UNKNOWN},
	{ "Package", PARSE_PACKAGE },
	{ "Essential", PARSE_UNKNOWN },
	{ "Status", PARSE_STATUS },
	{ "Priority", PARSE_UNKNOWN },
	{ "Section", PARSE_UNKNOWN },
	{ "Installed-Size", PARSE_UNKNOWN },
	{ "Origin", PARSE_UNKNOWN },
	{ "Maintainer", PARSE_MAINTAINER },
	{ "Bugs", PARSE_UNKNOWN },
	{ "Architecture", PARSE_ARCHITECTURE },
	{ "Source", PARSE_UNKNOWN },
	{ "Version", PARSE_VERSION },
	{ "Revision", PARSE_UNKNOWN },
	{ "Config-Version", PARSE_UNKNOWN },
	{ "Replaces", PARSE_UNKNOWN },
	{ "Provides", PARSE_UNKNOWN },
	{ "Depends", PARSE_UNKNOWN },
	{ "Pre-Depends", PARSE_UNKNOWN },
	{ "Recommends", PARSE_UNKNOWN },
	{ "Suggests", PARSE_UNKNOWN },
	{ "Conflicts", PARSE_UNKNOWN },
	{ "Enhances", PARSE_UNKNOWN },
	{ "Conffiles", PARSE_UNKNOWN },
	{ "Filename", PARSE_UNKNOWN },
	{ "Size", PARSE_UNKNOWN },
	{ "MD5sum", PARSE_UNKNOWN },
	{ "MSDOS-Filename", PARSE_UNKNOWN },
	{ "Description", PARSE_DESCRIPTION },
	{ NULL, PARSE_UNKNOWN } 
	};
	
class DebianPackageInformation : public PackageInformation
{
public:

	DebianPackageInformation(void);
	~DebianPackageInformation(void);

	String GetSoftwareElementID(void) const;
	String GetName(void) const;
	String GetVersion(void) const;
	String GetSoftwareElementState(void) const;
	CIMDateTime GetInstallTime(void) const;
	Uint16 GetTargetOperatingSystem(void) const;
	String GetCopyright(void) const;
	String GetArchitecture(void) const;
	String GetDistribution(void) const;
	String GetMaintainer(void) const;
	String GetManufacturer(void) const;
	String GetCaption(void) const;
	String GetDescription(void) const;

	int isValid(void) const;

	Array<String> GetFileList(void);

	void add(String key,String value);
	
private:

	int valid;
	String ID;
	String Name;
	String Version;
	String State;
	CIMDateTime InstallTime;
	String Architecture;
	String Maintainer;
	String Manufacturer;
	String Description;

	void debugDump(void) const;

};

PEGASUS_NAMESPACE_END

#endif
