//%2003////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001, 2002  BMC Software, Hewlett-Packard Development
// Company, L. P., IBM Corp., The Open Group, Tivoli Systems.
// Copyright (c) 2003 BMC Software; Hewlett-Packard Development Company, L. P.;
// IBM Corp.; EMC Corporation, The Open Group.
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
// Author: Al Stone <ahs3@fc.hp.com>
//         Christopher Neufeld <neufeld@linuxcare.com>
//
// Modified By: David Kennedy       <dkennedy@linuxcare.com>
//              Christopher Neufeld <neufeld@linuxcare.com>
//              Al Stone            <ahs3@fc.hp.com>
//              Mike Glantz         <michael_glantz@hp.com>
//              Chad Smith         <chad_smith@hp.com>
//
//%////////////////////////////////////////////////////////////////////////////

#include "ComputerSystemProvider.h"
#include "ComputerSystem.h"
#include <Pegasus/Common/Logger.h> // for Logger
#include <Pegasus/Common/FileSystem.h> // for FileSystem.renameFile
#include <Pegasus/Config/ConfigManager.h>     // for getHomedPath
#include <iostream.h>
#include <stdlib.h>		// for getenv
#include <unistd.h>
#include <netinet/in.h>
#include <netdb.h>
#include <sys/param.h>     // for MAXHOSTNAMELEN
#include <string.h>        // for strcmp
#include <errno.h>         // for errno

PEGASUS_USING_STD;
PEGASUS_USING_PEGASUS;


//  _ProviderCacheDir should eventually be global so all providers
//  can keep the values of modifiable properties saved
static String _providerCacheDir;

static String _pinFile;
static String _tmpFile;

static String _hostName;
static String _systemLocation;
static String _serialNumber;
static CIMDateTime _installDate;
static String _primaryOwnerName;
static String _primaryOwnerContact;
static String _primaryOwnerPager;
static String _secondaryOwnerName;
static String _secondaryOwnerContact;
static String _secondaryOwnerPager;

String getAttribute( char *myAttrId )
{
  String 		retVal("Not initialized");
  char                  inLine[1024];
  char                   *attrId;
  char                 *value = NULL;
  char                 *tokp = NULL;

  // open file
  ifstream mParmStream(_pinFile.getCString());
  // Values will be left blank if can't access file
  if (mParmStream == 0) return retVal;

  while (mParmStream.getline(inLine, sizeof(inLine)))
  {
    /* Parse out the line to get the attribute Id and value     */
    attrId = strtok_r(inLine, "|", &tokp);
    value = strtok_r(NULL, "\n", &tokp);

    if (NULL == attrId)
    {
      continue;
    }

    if ( strcmp(attrId,myAttrId)  == 0 )
    {
      retVal.assign(value);
      return retVal;
    }
  } /* while */

  retVal.assign("Not Found");
  return retVal;

}


Boolean setAttribute( char *newAttrId, String newValue )
{
    char                 inLine[1024];
    char                 *attrId = NULL;
    char                 *oldValue = NULL;
    char                 *tokp = NULL;



    // open file
    ifstream mParmStream(_pinFile.getCString());
    ofstream ofs(_tmpFile.getCString());

    // Values will be left blank if can't access file
    if (mParmStream == 0) return false;

    while (mParmStream.getline(inLine, sizeof(inLine)))
    {
      /* Parse out the line to get the attribute Id and oldValue     */
      attrId = strtok_r(inLine, "|", &tokp);
      oldValue = strtok_r(NULL, "\n", &tokp);
  
      //  Now we want to reset the local oldValue
      if ( strcmp(attrId,newAttrId)  == 0 )
      {
        _primaryOwnerName = newValue;
	ofs << attrId << "|" << newValue << endl;
	continue;
      }
      else if ( strcmp(attrId,newAttrId)  == 0 )
      {
        _primaryOwnerContact = newValue;
	ofs << attrId << "|" << newValue << endl;
	continue;
      }
      else if ( strcmp(attrId,newAttrId)  == 0 )
      {
        _secondaryOwnerName = newValue;
	ofs << attrId << "|" << newValue << endl;
	continue;
      }
      else if ( strcmp(attrId,newAttrId)  == 0 )
      {
        _secondaryOwnerContact = newValue;
	ofs << attrId << "|" << newValue << endl;
	continue;
      }
      else if ( strcmp(attrId,newAttrId)  == 0 )
      {
        _primaryOwnerPager = newValue;
	ofs << attrId << "|" << newValue << endl;
	continue;
      }
      else if ( strcmp(attrId,newAttrId)  == 0 )
      {
        _secondaryOwnerPager = newValue;
	ofs << attrId << "|" << newValue << endl;
	continue;
      }
      else
      {
  	Logger::put(Logger::ERROR_LOG, CLASS_EXTENDED_COMPUTER_SYSTEM, Logger::WARNING,
		"You tried to set the value of an invalid attribute in PG_ComputerSystem", 234);
      }

	ofs << attrId << "|" << oldValue << endl;
    } /* while */
    

    ofs.close();

    // Let's replace pinFile with out new pin file (tmpFile)

    if ( ! FileSystem::removeFile(_pinFile))
    {
       Logger::put(Logger::ERROR_LOG, "PG_ComputerSystem",
		       Logger::SEVERE,
		        "Cannot remove PG_ComputerSystem pin file: $0.",_pinFile);
       return false;
    }
      
    if ( ! FileSystem::renameFile(_tmpFile, _pinFile))
    {
       Logger::put(Logger::ERROR_LOG, "CIMPassword", Logger::SEVERE,
        "Cannot overwrite pinFile $0.", _tmpFile);
       return false; 
    } 

    return true;
}


ComputerSystem::ComputerSystem()
{
}

ComputerSystem::~ComputerSystem()
{
}


Boolean ComputerSystem::getCaption(CIMProperty& p)
{
  // hardcoded
  p = CIMProperty(PROPERTY_CAPTION, String(CAPTION));
  return true;
}

Boolean ComputerSystem::getDescription(CIMProperty& p)
{
  static char description[256];

  ifstream version("/proc/version");

  if (!version) {
	Logger::put(Logger::ERROR_LOG, CLASS_EXTENDED_COMPUTER_SYSTEM, Logger::WARNING,
		"error opening file \"/proc/version\" (%0)", errno);
	return false;
  }
	
  version.getline(description, sizeof(description));
  p = CIMProperty(PROPERTY_DESCRIPTION, String(description));
  return true;
}

Boolean ComputerSystem::getInstallDate(CIMProperty& p)
{
  return false;
}

Boolean ComputerSystem::getCreationClassName(CIMProperty& p)
{
  // can vary, depending on class
  p = CIMProperty(PROPERTY_CREATION_CLASS_NAME,
                  String(CLASS_EXTENDED_COMPUTER_SYSTEM));
    return true;
}


Boolean ComputerSystem::getName(CIMProperty& p)
{
  p = CIMProperty(PROPERTY_NAME,String(getHostName()));
  return true;
}

Boolean ComputerSystem::getStatus(CIMProperty& p)
{
  // hardcoded 
  p = CIMProperty(PROPERTY_STATUS,String(STATUS));
  return true;
}

Boolean ComputerSystem::getNameFormat(CIMProperty& p)
{
  // hardcoded
  p = CIMProperty(PROPERTY_NAME_FORMAT,String(NAME_FORMAT));
  return true;
}

Boolean ComputerSystem::getPrimaryOwnerName(CIMProperty& p)
{
  // set in initialize() 
  _primaryOwnerName.assign(getAttribute(PROPERTY_PRIMARY_OWNER_NAME));
  p = CIMProperty(PROPERTY_PRIMARY_OWNER_NAME,_primaryOwnerName);
  return true;
}

Boolean ComputerSystem::setPrimaryOwnerName(const String& name)
{
  setAttribute(PROPERTY_PRIMARY_OWNER_NAME, name); 
  return true;
}

Boolean ComputerSystem::getPrimaryOwnerContact(CIMProperty& p)
{
  // set in initialize() 
  _primaryOwnerContact.assign(getAttribute(PROPERTY_PRIMARY_OWNER_CONTACT));
  p = CIMProperty(PROPERTY_PRIMARY_OWNER_CONTACT,_primaryOwnerContact);
  return true;
}

Boolean ComputerSystem::setPrimaryOwnerContact(const String& contact)
{
  setAttribute(PROPERTY_PRIMARY_OWNER_CONTACT, contact); 
  return true;
}

Boolean ComputerSystem::getRoles(CIMProperty& p)
{
  return false;
}

Boolean ComputerSystem::getOtherIdentifyingInfo(CIMProperty& p)
{
  return false;
}

Boolean ComputerSystem::getIdentifyingDescriptions(CIMProperty& p)
{
  return false;
}

Boolean ComputerSystem::getDedicated(CIMProperty& p)
{
  return false;
}

Boolean ComputerSystem::getResetCapability(CIMProperty& p)
{
  return false;
}

Boolean ComputerSystem::getPowerManagementCapabilities(CIMProperty& p)
{
  // Since PowerManagementSupported == FALSE
  // Capabilities must be defined to return "1" which signifies
  // "Not Supported" from Capabilities enum
  p = CIMProperty(PROPERTY_POWER_MANAGEMENT_CAPABILITIES,1); 
  return true;
}

Boolean ComputerSystem::getInitialLoadInfo(CIMProperty& p)
{
  return false;
}

Boolean ComputerSystem::getLastLoadInfo(CIMProperty& p)
{
  return false;
}

Boolean ComputerSystem::getPowerManagementSupported(CIMProperty& p)
{
  // hardcoded
  p = CIMProperty(PROPERTY_POWER_MANAGEMENT_SUPPORTED,false); // on PA-RISC!!
  return true;
}

Boolean ComputerSystem::getPowerState(CIMProperty& p)
{
  // hardcoded
/*
ValueMap {"1", "2", "3", "4", "5", "6", "7", "8"},
Values {"Full Power", "Power Save - Low Power Mode", 
                   "Power Save - Standby", "Power Save - Other", 
                   "Power Cycle", "Power Off", "Hibernate", "Soft Off"}
*/
  p = CIMProperty(PROPERTY_POWER_STATE,Uint16(1));  
  return true;
  return false;
}

Boolean ComputerSystem::getWakeUpType(CIMProperty& p)
{
  return false;
}

Boolean ComputerSystem::getPrimaryOwnerPager(CIMProperty& p)
{
  // set in initialize() 
  _primaryOwnerPager.assign(getAttribute(PROPERTY_PRIMARY_OWNER_PAGER));
  p = CIMProperty(PROPERTY_PRIMARY_OWNER_PAGER,_primaryOwnerPager);
  return true;
}

Boolean ComputerSystem::setPrimaryOwnerPager(const String& pager)
{
  setAttribute(PROPERTY_PRIMARY_OWNER_PAGER, pager); 
  return true;
}

Boolean ComputerSystem::getSecondaryOwnerName(CIMProperty& p)
{
  // set in initialize() 
  _secondaryOwnerName.assign(getAttribute(PROPERTY_SECONDARY_OWNER_NAME));
  p = CIMProperty(PROPERTY_SECONDARY_OWNER_NAME,_secondaryOwnerName);
  return true;
}

Boolean ComputerSystem::setSecondaryOwnerName(const String& name)
{
  setAttribute(PROPERTY_SECONDARY_OWNER_NAME, name); 
  return true;
}

Boolean ComputerSystem::getSecondaryOwnerContact(CIMProperty& p)
{
  // set in initialize() 
  _secondaryOwnerContact.assign(getAttribute(PROPERTY_SECONDARY_OWNER_CONTACT));
  p = CIMProperty(PROPERTY_SECONDARY_OWNER_CONTACT,_secondaryOwnerContact);
  return true;
}

Boolean ComputerSystem::setSecondaryOwnerContact(const String& contact)
{
  setAttribute(PROPERTY_SECONDARY_OWNER_CONTACT, contact); 
  return true;
}

Boolean ComputerSystem::getSecondaryOwnerPager(CIMProperty& p)
{
  // set in initialize() 
  _secondaryOwnerPager.assign(getAttribute(PROPERTY_SECONDARY_OWNER_PAGER));
  p = CIMProperty(PROPERTY_SECONDARY_OWNER_PAGER,_secondaryOwnerPager);
  return true;
}

Boolean ComputerSystem::setSecondaryOwnerPager(const String& pager)
{
  setAttribute(PROPERTY_SECONDARY_OWNER_PAGER, pager); 
  return true;
}

Boolean ComputerSystem::getSerialNumber(CIMProperty& p)
{
  return false;
}

Boolean ComputerSystem::getIdentificationNumber(CIMProperty& p)
{
  return false;
}




/**
 * initialize primarily functions to initialize static global variables
 * that will not be changed frequently. These variables are currently
 * _hostName, _primaryOwner* and _secondaryOwner*
 *
 */
void ComputerSystem::initialize(void)
{
  char    hostName[MAXHOSTNAMELEN];
  struct  hostent *he;

  if (gethostname(hostName, MAXHOSTNAMELEN) != 0)
  {
      _hostName.assign("Not initialized");
  }

  // Now get the official hostname.  If this call fails then return
  // the value from gethostname().

  he=gethostbyname(hostName);
  if (he)
  {
     strcpy(hostName, he->h_name);
  }

  _hostName.assign(hostName);

  /*
     // Now initializing "configurable" attributes such as primaryOwnerName
     // Providers are not allowed to include -lpegconfig in the makefile script
     // if someone can fix this so I can use the global env variable 
     // $PEGASUS_HOME it would be appreciated then we can replace the hard-
     // coded _providerCacheDir with something like these two lines
  
  _providerCacheDir = ConfigManager::getHomedPath(ConfigManager::getInstance()->getCurrentValue("home"));
  _providerCacheDir.append("/providercachedir");

  */

_providerCacheDir.assign("/var/cache/pegasus/providercache");
_pinFile = _providerCacheDir;
_pinFile.append("/PG_ComputerSystem.pin");
_tmpFile = _providerCacheDir;
_tmpFile.append("/PG_ComputerSystem.tmp");


  if (!FileSystem::isDirectory(_providerCacheDir))
  {
    if(! FileSystem::makeDirectory(_providerCacheDir))
    {
        Logger::put(Logger::ERROR_LOG, CLASS_EXTENDED_COMPUTER_SYSTEM, Logger::WARNING,
	"Couldn't create provider cache directory: $0.", _providerCacheDir);
	return;
    }
  }
  if (!FileSystem::exists(_pinFile))
  {
    // Then let's create the file
    Logger::put(Logger::STANDARD_LOG, CLASS_EXTENDED_COMPUTER_SYSTEM, Logger::INFORMATION, 
    "Creating initial pinFile for PG_ComputerSystem: $0.", _pinFile);
    ofstream ofs(_pinFile.getCString());

    ofs << "SystemLocation|Not Set" << endl;
    ofs << PROPERTY_PRIMARY_OWNER_NAME << "|Not Set" << endl;
    ofs << PROPERTY_PRIMARY_OWNER_CONTACT << "|Not Set" << endl;
    ofs << PROPERTY_SECONDARY_OWNER_NAME << "|Not Set" << endl;
    ofs << PROPERTY_SECONDARY_OWNER_CONTACT << "|Not Set" << endl;
    ofs << PROPERTY_PRIMARY_OWNER_PAGER << "|Not Set" << endl;
    ofs << PROPERTY_SECONDARY_OWNER_PAGER << "|Not Set" << endl;
    ofs.close();

  }
        Logger::put(Logger::ERROR_LOG, CLASS_EXTENDED_COMPUTER_SYSTEM, Logger::WARNING,
	"Ran Initialize:");
}

String ComputerSystem::getHostName(void)
{
  return _hostName;
}


