//%2006////////////////////////////////////////////////////////////////////////
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
//=============================================================================
//
// Author: Tim Potter <tpot@hp.com>
//
//%////////////////////////////////////////////////////////////////////////////

#include <Pegasus/Provider/CIMProvider.h>
#include <Pegasus/Common/System.h>

PEGASUS_USING_PEGASUS;
PEGASUS_USING_STD;

#include <sstream>
#include "RPMProviderUtils.h"

#include <stdlib.h>

// Include operating system dependent

#if defined(PEGASUS_PLATFORM_LINUX_GENERIC_GNU)
#include "RPMProviderUtils_Linux.cpp"
#else
#include "RPMProviderUtils_Unknown.cpp"
#endif

// Constants

const String PackageClassName = "PG_RPMPackage";
const String FileClassName = "PG_RPMFileCheck";
const String ProductClassName = "PG_RPMProduct";
const String AssociatedFileClassName = "PG_RPMAssociatedFile";

const CIMNamespaceName cimv2NameSpace = "root/cimv2";

//
// RPMPackage helpers
//

static void
getHeaderNVR(Header h, String &name, String &version, String &release)
{
    const char *n, *v, *r;
    headerNVR(h, &n, &v, &r);

    name.assign(n);
    version.assign(v);
    release.assign(r);

    headerFreeTag(h, n, RPM_STRING_TYPE);
    headerFreeTag(h, v, RPM_STRING_TYPE);
    headerFreeTag(h, r, RPM_STRING_TYPE);
}

CIMObjectPath 
packageObjectPathFromHeader(Header h)
{ 
    String name, version, release;
    getHeaderNVR(h, name, version, release);

    // Create keybindings

    Array<CIMKeyBinding> kb;

    kb.append(CIMKeyBinding("Name", name));
    kb.append(CIMKeyBinding("Version", version));
    kb.append(CIMKeyBinding("TargetOperatingSystem", 
			    getTargetOperatingSystem()));
    kb.append(CIMKeyBinding("SoftwareElementState", Uint16(Executable)));

    stringstream sid;
    sid << name << "-" << version << "-" << release;

    kb.append(CIMKeyBinding("SoftwareElementID", 
                            String(sid.str().c_str())));

    return CIMObjectPath(System::getHostName(), cimv2NameSpace, 
			 PackageClassName, kb);
}

CIMInstance 
packageInstanceFromHeader(Header h)
{
    String name, version, release;
    getHeaderNVR(h, name, version, release);

    // Create instance

    CIMInstance instance(PackageClassName);

    instance.addProperty(CIMProperty("Name", name));
    instance.addProperty(CIMProperty("Version", version));
    instance.addProperty(CIMProperty("TargetOperatingSystem",
				     getTargetOperatingSystem()));
    instance.addProperty(CIMProperty("SoftwareElementState", Executable));
    
    stringstream sid;
    sid << name << "-" << version << "-" << release;

    instance.addProperty(CIMProperty("SoftwareElementID",
				     String(sid.str().c_str())));

    // Attach path

    instance.setPath(packageObjectPathFromHeader(h));
    
    return instance;
}

//
// RPMFileCheck helpers
//

Array<CIMObjectPath>
fileObjectPathsFromHeader(Header h)
{ 
    Array<CIMInstance> fileInstances(fileInstancesFromHeader(h));
    Array<CIMObjectPath> filePaths;

    for (Uint32 i = 0; i < fileInstances.size(); i++)
	filePaths.append(fileInstances[i].getPath());

    return filePaths;
}

Array<CIMInstance>
fileInstancesFromHeader(Header h)
{
    Array<CIMInstance> result;

    // Get list of filenames for this package

    int_32 num_basenames, num_dirnames, num_dirindicies;
    void *basenames, *dirnames, *dirindicies;
    int_32 type;

    rpmHeaderGetEntry(h, RPMTAG_BASENAMES, &type, &basenames, &num_basenames);

    if (type != RPM_STRING_ARRAY_TYPE) {

        // There are no files in this package, e.g the basesystem RPM

        return result;
    }

    rpmHeaderGetEntry(h, RPMTAG_DIRNAMES, &type, &dirnames, &num_dirnames);
 
    if (type != RPM_STRING_ARRAY_TYPE) {

	// Type for DIRNAMES tag not an array of strings

        return result;
    }

    rpmHeaderGetEntry(h, RPMTAG_DIRINDEXES, &type, &dirindicies, 
		      &num_dirindicies);

    if (type != RPM_INT32_TYPE) {

	// Type for DIRINDEXES tag not an int32

        return result;
    }

    if (num_basenames != num_dirindicies) {

	// Inconsistent number of basenames and dirindicies

        return result;
    }

    // Create keybindings template

    String name, version, release;
    getHeaderNVR(h, name, version, release);

    Array<CIMKeyBinding> kb;

    stringstream sid;
    sid << name << "-" << version << "-" << release;

    kb.append(CIMKeyBinding("SoftwareElementID", String(sid.str().c_str())));
    kb.append(CIMKeyBinding("Name", name));
    kb.append(CIMKeyBinding("Version", version));
    kb.append(CIMKeyBinding("TargetOperatingSystem", 
			    getTargetOperatingSystem()));
    kb.append(CIMKeyBinding("SoftwareElementState", Uint16(Executable)));

    for (int_32 i = 0; i < num_dirindicies; i++) {
	Array<CIMKeyBinding> file_kb(kb);

        stringstream filename;

        int_32 ndx = ((int_32 *)dirindicies)[i];

        if (ndx >= num_dirindicies) {

	    // dirindex to large for package

            return result;
        }

        filename << ((char **)dirnames)[ndx] << ((char **)basenames)[i];
                
        file_kb.append(CIMKeyBinding("CheckID",
                                     String(filename.str().c_str())));

        CIMInstance obj(FileClassName);

        obj.addProperty(CIMProperty("SoftwareElementID", 
                                    String(sid.str().c_str())));
        obj.addProperty(CIMProperty("Name", name));
        obj.addProperty(CIMProperty("Version", version));
        obj.addProperty(CIMProperty("TargetOperatingSystem", 
                                    getTargetOperatingSystem()));
        obj.addProperty(CIMProperty("CheckID", 
                                    String(filename.str().c_str())));
        obj.addProperty(CIMProperty("SoftwareElementState",
                                    Uint16(Executable)));

	obj.setPath(CIMObjectPath(System::getHostName(),
				  cimv2NameSpace,
				  obj.getClassName(),
				  file_kb));

        result.append(obj);
    }

    headerFreeData(basenames, RPM_STRING_ARRAY_TYPE);
    headerFreeData(dirnames, RPM_STRING_ARRAY_TYPE);
    headerFreeData(dirindicies, RPM_INT32_TYPE);

    return result;
}

//
// RPMProduct helpers
//

static String
getVendor(Header h)
{
    int_32 type, count;
    char *value;
    String vendor("Unknown");
    
    if (rpmHeaderGetEntry(h, RPMTAG_VENDOR, &type, (void **)&value, 
			  &count) == 1 && type == RPM_STRING_TYPE) {
	vendor.assign(value);
	headerFreeTag(h, value, RPM_STRING_TYPE);
    }
    
    return vendor;
}

CIMObjectPath 
productObjectPathFromHeader(Header h)
{ 
    String name, version, release;
    getHeaderNVR(h, name, version, release);

    // Create keybindings

    Array<CIMKeyBinding> kb;

    kb.append(CIMKeyBinding("Version", version));
    kb.append(CIMKeyBinding("IdentifyingNumber", String("None")));
    kb.append(CIMKeyBinding("Vendor", getVendor(h)));
    kb.append(CIMKeyBinding("Name", name));

    return CIMObjectPath(System::getHostName(), cimv2NameSpace,
			 PackageClassName, kb);
}

CIMInstance 
productInstanceFromHeader(Header h)
{
    String name, version, release;
    getHeaderNVR(h, name, version, release);

    // Create instance

    CIMInstance instance(ProductClassName);

    instance.addProperty(CIMProperty("Version", version));
    instance.addProperty(CIMProperty("IdentifyingNumber", String("None")));
    instance.addProperty(CIMProperty("Vendor", getVendor(h)));
    instance.addProperty(CIMProperty("Name", name));

    String description;
    int_32 type, count;
    char *value;

    if (rpmHeaderGetEntry(h, RPMTAG_DESCRIPTION, &type, (void **)&value, 
			  &count) == 1 && type == RPM_STRING_TYPE) {
            description.assign(value);
	    headerFreeTag(h, value, RPM_STRING_TYPE);
    }

    instance.addProperty(CIMProperty("Description", description));

    String elementName;

    if (rpmHeaderGetEntry(h, RPMTAG_SUMMARY, &type, (void **)&value, 
			  &count) == 1 && type == RPM_STRING_TYPE) {
            elementName.assign(value);
	    headerFreeTag(h, value, RPM_STRING_TYPE);

    }

    instance.addProperty(CIMProperty("ElementName", elementName));        

    // Attach path

    instance.setPath(productObjectPathFromHeader(h));

    return instance;
}

//
// Association provider helpers
//

CIMObjectPath 
packageReferenceNameFromHeader(Header h, const CIMObjectPath &file)
{
    CIMObjectPath element(packageObjectPathFromHeader(h));
    Array<CIMKeyBinding> kb;

    kb.append(CIMKeyBinding("Element", element));
    kb.append(CIMKeyBinding("Check", file));

    return CIMObjectPath(System::getHostName(), cimv2NameSpace,
			 AssociatedFileClassName, kb);
}

CIMInstance 
packageReferenceFromHeader(Header h, const CIMObjectPath &file)
{
    CIMObjectPath element(packageObjectPathFromHeader(h));

    CIMInstance obj(AssociatedFileClassName);

    obj.addProperty(CIMProperty("Element", element));
    obj.addProperty(CIMProperty("Check", file));
    
    obj.setPath(packageReferenceNameFromHeader(h, file));

    return obj;
}

Array<CIMObjectPath> 
fileReferenceNamesFromHeader(Header h)
{
    CIMObjectPath package(packageObjectPathFromHeader(h));
    Array<CIMObjectPath> filePaths(fileObjectPathsFromHeader(h)), result;

    for (Uint32 i = 0; i < filePaths.size(); i++) {
	Array<CIMKeyBinding> kb;

	kb.append(CIMKeyBinding("Element", package));
	kb.append(CIMKeyBinding("Check", filePaths[i]));

	result.append(CIMObjectPath(System::getHostName(),
				    cimv2NameSpace,
				    AssociatedFileClassName,
				    kb));
    }

    return result;
}

Array<CIMInstance> 
fileReferencesFromHeader(Header h)
{
    CIMObjectPath package(packageObjectPathFromHeader(h));
    Array<CIMObjectPath> files(fileObjectPathsFromHeader(h));

    Array<CIMInstance> result;

    for (Uint32 i = 0; i < files.size(); i++) {

	Array<CIMKeyBinding> kb;

	kb.append(CIMKeyBinding("Element", package));
	kb.append(CIMKeyBinding("Check", files[i]));

	CIMInstance obj(AssociatedFileClassName);

	obj.addProperty(CIMProperty("Element", package));
	obj.addProperty(CIMProperty("Check", files[i]));

	obj.setPath(CIMObjectPath(System::getHostName(),
				  cimv2NameSpace,
				  obj.getClassName(),
				  kb));

	result.append(obj);
    }

    return result;
}
