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


/* The Device Locator class is responsible for providing a consistant
 * interface to enumerating devices of many different types.
 * The initial implementation of this class will be for Linux but
 * it also could be ported to other operating systems.
 *
 * This classes uses plugins to perform the enumeration of all devices.
 * These plugins follow a consistant API and are responsible for 
 * enumerating specific devices 
 */

/* The device types to be used will be the PCI device types.  The PCI 
 * device types is a hierarchy of maximum depth three that enumerates
 * the different types of devices possible.  Because this class will
 * enumerate devices of types not only found in PCI busses the device
 * list will be extended using Pegasus specifics as necessary.
 *
 */

#include "DeviceLocator.h"

/* plugins */
#include "IA64ProcessorLocatorPlugin.h"
#include "Ix86ProcessorLocatorPlugin.h"
#include "SCSILocatorPlugin.h"
#include "IDELocatorPlugin.h"
#include "PCILocatorPlugin.h"
#include "NetConfPlugin.h"
#include "IOPortLocatorPlugin.h"
#include "IRQLocatorPlugin.h"

/* Device list */
#include "DeviceTypes.h"

PEGASUS_NAMESPACE_BEGIN

#define NUM_PLUGINS 10

void DeviceLocator::loadPlugins(){
	PluginData curPlugin;
	/* Load all the plugins */
	plugins.reserve(NUM_PLUGINS);

	curPlugin.status=PluginData::UNUSED;

	curPlugin.plugin=new IA64ProcessorLocatorPlugin();
	plugins.push_back(curPlugin);
	curPlugin.plugin=new Ix86ProcessorLocatorPlugin();
	plugins.push_back(curPlugin);
	curPlugin.plugin=new SCSILocatorPlugin();
	plugins.push_back(curPlugin);
	curPlugin.plugin=new IDELocatorPlugin();
	plugins.push_back(curPlugin);
	curPlugin.plugin=new PCILocatorPlugin();
	plugins.push_back(curPlugin);
	curPlugin.plugin=new NetConfPlugin();
	plugins.push_back(curPlugin);
	curPlugin.plugin=new IOPortLocatorPlugin();
	plugins.push_back(curPlugin);
	curPlugin.plugin=new IRQLocatorPlugin();
	plugins.push_back(curPlugin);
	/* Add more plugins as necessary */
}

DeviceLocator::DeviceLocator(){
	loadPlugins();
}

/** This constructor can be called to set the initial search criteria */
DeviceLocator::	DeviceLocator(Uint16 base_class, 
			      Uint16 subclass,
			      Uint16 prog_if)
{
  loadPlugins();
  setDeviceSearchCriteria(base_class, subclass, prog_if);
}


DeviceLocator::~DeviceLocator(){
  vector<PluginData>::iterator i;

  i = plugins.begin();
  while (i != plugins.end()) {
    delete i->plugin;
    i++;
    }

}

/** setDeviceSearchCriteria(): Returns 0 if it is possible to search for 
 *  that criteria or -1 if no plugins support that criteria.  This function 
 *  is called to set the search criteria.  The search criteria must be set
 *  before any searches can be performed. */
int DeviceLocator::setDeviceSearchCriteria(Uint16 base_class, Uint16 subclass, Uint16 prog_if){
  vector<PluginData>::iterator i;

	/* Find out if any of the plugins support this search criteria */

	/* Set all the status to unchecked */
  int retval=-1;
  
  i = plugins.begin();
  while (i != plugins.end()) {
    if((i->plugin->setDeviceSearchCriteria(base_class,subclass,prog_if))==0){
	/* This plugin will accept searches for the specified criteria */
    	i->status=PluginData::UNCHECKED;
        retval=0;
    }
    else {
	/* This plugin does not accept searches for the specified criteria */
    	i->status=PluginData::UNUSED;
    }
    i++;
  }

  /* Return -1 if no plugins support that criteria */
  return retval;  
}

/** getNextDevice(): This method returns a pointer to an allocated object */
DeviceInformation * DeviceLocator::getNextDevice(void){
  vector<PluginData>::iterator i;
  DeviceInformation *curDevice;

  i = plugins.begin();
  while (i != plugins.end()) {
    /* Find the next UNCHECKED and PENDING plugin */
    if(i->status==PluginData::UNCHECKED||i->status==PluginData::PENDING){
       curDevice=i->plugin->getNextDevice();
       if(curDevice){
          i->status=PluginData::PENDING;
	  /* return the device that was located */
          return curDevice;
       }

       /* Mark this plugin as completed */
       i->status=PluginData::COMPLETE;
    }
    i++;
  }
  return NULL;
}

PEGASUS_NAMESPACE_END
