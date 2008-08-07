//%2008////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001, 2002 BMC Software; Hewlett-Packard Development
// Company, L.P.; IBM Corp.; The Open Group; Tivoli Systems.
// Copyright (c) 2003 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation, The Open Group.
// Copyright (c) 2004 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation; VERITAS Software Corporation; The Open Group.
// Copyright (c) 2005 Hewlett-Packard Development Company, L.P.; IBM Corp.;
// EMC Corporation; VERITAS Software Corporation; The Open Group.
// Copyright (c) 2006 Hewlett-Packard Development Company, L.P.; IBM Corp.;
// EMC Corporation; Symantec Corporation; The Open Group.
// Copyright (c) 2008 Hewlett-Packard Development Company, L.P.; IBM Corp.;
// EMC Corporation; Symantec Corporation; Novell, Inc.; The Open Group.
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
//%/////////////////////////////////////////////////////////////////////////////

#include "ProviderManagerMap.h"

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/PegasusVersion.h>
#include <Pegasus/Common/Constants.h>
#include <Pegasus/Common/Tracer.h>
#include <Pegasus/Common/FileSystem.h>
#include <Pegasus/Common/Logger.h>
#include <Pegasus/Common/String.h>
#include <Pegasus/Config/ConfigManager.h>

#include <Pegasus/Common/Dir.h>
#include <Pegasus/Common/DynamicLibrary.h>

PEGASUS_NAMESPACE_BEGIN


ProviderManagerMap& ProviderManagerMap::instance()
{
    static ProviderManagerMap singletonInst;
    return singletonInst;
}


ProviderManagerMap::ProviderManagerMap()
{
    initialize();
}

bool ProviderManagerMap::isValidProvMgrIfc(String &ifcType, String &ifcVersion)
{
    for (Uint32 ifc=0; ifc<_pmArray.size(); ifc++)
    {
        if (_pmArray[ifc].ifcName == ifcType)
        {
            if (ifcVersion.size()==0)
            { 
                return true;
            }
            else 
            {
                for (Uint32 ver=0; ver<_pmArray[ifc].ifcVersions.size(); ver++)
                {
                    if (_pmArray[ifc].ifcVersions[ver] == ifcVersion)
                    {
                        return true;
                    }
                }
            }
        }
    }
    return false;
}


bool ProviderManagerMap::getProvMgrPathForIfcType(String &ifcType,
    String &ifcVersion,
    String &path)
{
    path.clear();
    for (Uint32 ifc=0; ifc<_pmArray.size(); ifc++)
    {
        if (_pmArray[ifc].ifcName == ifcType)
        {
            if (ifcVersion.size()==0)
            {
                path = _pmArray[ifc].path;
                return true;
            }
            else for (Uint32 ver=0; ver<_pmArray[ifc].ifcVersions.size(); ver++)
            {
                if (_pmArray[ifc].ifcVersions[ver] == ifcVersion)
                {
                    path = _pmArray[ifc].path;
                    return true;
                }
            }
        }
    }
    return false;
}


void ProviderManagerMap::initialize()
{
    String libExt = FileSystem::getDynamicLibraryExtension(); 
    // first add the default:

    ProvMgrIfcInfo defaultPMEntry;
    defaultPMEntry.path.clear();
    defaultPMEntry.ifcName = "C++Default";
    defaultPMEntry.ifcVersions.append(String("2.1.0"));
    defaultPMEntry.ifcVersions.append(String("2.2.0"));
    defaultPMEntry.ifcVersions.append(String("2.3.0"));
    defaultPMEntry.ifcVersions.append(String("2.5.0"));
    defaultPMEntry.ifcVersions.append(String("2.6.0"));
    _pmArray.append(defaultPMEntry);

    // now check for plugins

    String dirName = ConfigManager::getInstance()->getCurrentValue(
            "providerManagerDir");
    dirName = ConfigManager::getHomedPath(dirName); 

    PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL3,
        "Looking for ProviderManagers in " + dirName + "."); 

    // check to make sure that this ifc type is handled by one of the
    // provider managers in the directory
    String testname = String("providermanager")+libExt;
    for (Dir dir(dirName); dir.more(); dir.next())
    {
        String filename = dir.getName();
        String lowerFilename = filename;
        lowerFilename.toLower();
        if ((lowerFilename.subString(lowerFilename.size()-testname.size()) ==
                 testname) &&
            (lowerFilename !=
                 FileSystem::buildLibraryFileName("defaultprovidermanager")) &&
            (lowerFilename !=
                 FileSystem::buildLibraryFileName("pegprovidermanager")))
        {
            String fullPath = dirName + "/" + filename;
            // found a file... assume it's a ProviderManager library
            PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
                "Found file " + fullPath + ".  Checking to see if it is "
                "a ProviderManager."); 
            DynamicLibrary dl(fullPath);
            if (!dl.load())
            {
                Logger::put_l(Logger::ERROR_LOG, System::CIMSERVER,
                    Logger::SEVERE,
                    "Server.ProviderRegistrationManager.ProviderManagerMap."
                    "LOAD_ERROR",
                    "Error loading library $0: $1.",
                    fullPath, dl.getLoadErrorMessage());
                continue;    // to the next file
            }

            Uint32 (*get_peg_ver)() = 
                (Uint32(*)()) dl.getSymbol("getPegasusVersion"); 

            if (get_peg_ver == 0)
            {
                Logger::put_l(Logger::ERROR_LOG, System::CIMSERVER,
                    Logger::SEVERE,
                    "Server.ProviderRegistrationManager.ProviderManagerMap."
                    "MISSING_GET_PG_VERSION",
                    "Library $0 does not contain expected function "
                    "'getPegasusVersion'.",
                    fullPath); 
                continue;
            }

            Uint32 peg_ver = get_peg_ver(); 
            if (peg_ver != PEGASUS_VERSION_NUMBER)
            {
                Logger::put_l(Logger::ERROR_LOG, System::CIMSERVER, 
                    Logger::SEVERE, 
                    "Server.ProviderRegistrationManager.ProviderManagerMap."
                    "WRONG_VERSION",
                    "Provider Manager $0 returned Pegasus "
                    "version $1.  Expected $2.",
                    fullPath, peg_ver, PEGASUS_VERSION_NUMBER); 
                continue; 
            }

            const char** (*get_ifc)() = (const char**(*)()) dl.getSymbol(
                "getProviderManagerInterfaceNames");
            const char** (*get_ver)(const char *) = 
                (const char**(*)(const char *)) dl.getSymbol(
                    "getProviderManagerInterfaceVersions");
            if (get_ifc == 0)
            {
                Logger::put_l(Logger::ERROR_LOG, System::CIMSERVER, 
                    Logger::SEVERE, 
                    "Server.ProviderRegistrationManager.ProviderManagerMap."
                    "MISSING_GET_IFC_NAMES",
                    "Provider Manager $0 does not contain expected "
                    "function 'getProviderManagerInterfaceNames'", 
                    fullPath); 
                continue;    // to the next file
            }
            if (get_ver == 0)
            {
                Logger::put_l(Logger::ERROR_LOG, System::CIMSERVER, 
                    Logger::SEVERE, 
                    "Server.ProviderRegistrationManager.ProviderManagerMap."
                    "MISSING_GET_IFC_VERSIONS",
                    "Provider Manager $0 does not contain expected "
                    "function 'getProviderManagerInterfaceVersions'", 
                            fullPath); 
                continue;    // to the next file
            }

            const char ** ifcNames = get_ifc();
            if ((ifcNames!=NULL) && (*ifcNames!=NULL))
            {
                for (int i=0; ifcNames[i]!=NULL; i++)
                {
                    const char *ifcName = ifcNames[i];

                    ProvMgrIfcInfo entry;
                    entry.path = fullPath;
                    entry.ifcName = ifcName;

                    // now get the versions
                    const char ** ifcVersions = get_ver(ifcName);
                    for (int j=0; ifcVersions[j]!=NULL; j++)
                    {
                        entry.ifcVersions.append(String(ifcVersions[j]));
                        PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL3,
                            String("Adding Provider type " + String(ifcName) + 
                                " version " + String(ifcVersions[j]) + 
                                " handled by ProviderManager " + fullPath)); 
                    }
                    _pmArray.append(entry);
                }
            }
        }
    }
    _bInitialized = true;
}

PEGASUS_NAMESPACE_END

