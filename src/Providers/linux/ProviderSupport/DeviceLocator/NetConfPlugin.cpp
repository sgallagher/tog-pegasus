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
//  Return information about the networking parameters of the system.
//


#include <Pegasus/Common/String.h>
#include <Pegasus/Common/System.h>

#include "NetConfPlugin.h"
#include "DeviceTypes.h"

PEGASUS_NAMESPACE_BEGIN


/** a macro which, when called with an array in scope (not passed as a
 *  parameter to a function), returns the number of elements in the
 *  array. */
#define N_IN_ARRAY(x) ((int) (sizeof(x) / sizeof(x[0])))

/// a regular expression which identifies potential dotted-quads (IPv4 addresses)
#define DOTTEDQUAD "[0-9]+\\.[0-9]+\\.[0-9]+\\.[0-9]+"


/** A list of different types of output lines possibly returned by
 *  /sbin/ifconfig. */
enum if_data_format {
  IF_LINK_ENCAP_WITH_HWADDR, IF_LINK_ENCAP_WITHOUT_HWADDR,
  IF_IPV4_PARMS_WITHOUT_BCAST, IF_IPV4_PARMS_WITH_BCAST,
  IF_RX_INFO, IF_TX_INFO, IF_COLLISIONS, IF_RX_TX_BYTES,
  IF_STATUS_FLAGS_MTU, IF_INTERRUPT_BASEADDR, IF_END_OF_RECORD
};


/** An association set between types of ifconfig output lines and the
 *  regular expressions which characterize those lines. */
static const struct {
  char const *regex;
  enum if_data_format linetype;
} if_data[] = {
  {
    "^([^ \t]+)[ \t]+Link encap:(Ethernet|Local Loopback)[ \t]+HWaddr[ \t]+(.*)",
    IF_LINK_ENCAP_WITH_HWADDR
  },
  {
    "^([^ \t]+)[ \t]+Link encap:(.*)[ \t]*$",
    IF_LINK_ENCAP_WITHOUT_HWADDR
  },
  {
    "inet addr:(" DOTTEDQUAD ")[ \t]+Mask:(" DOTTEDQUAD ")",
    IF_IPV4_PARMS_WITHOUT_BCAST
  },
  {
    "inet addr:(" DOTTEDQUAD ")[ \t]+Bcast:(" DOTTEDQUAD ")[ \t]+Mask:(" DOTTEDQUAD ")",
    IF_IPV4_PARMS_WITH_BCAST
  },
  {
    "RX packets:([0-9]+)[ \t]+errors:([0-9]+)[ \t]+dropped:([0-9]+)[ \t]+overruns:([0-9]+)[ \t]+frame:([0-9])",
    IF_RX_INFO
  },
  {
    "TX packets:([0-9]+)[ \t]+errors:([0-9]+)[ \t]+dropped:([0-9]+)[ \t]+overruns:([0-9]+)[ \t]+carrier:([0-9])",
    IF_TX_INFO
  },
  {
    "^[ \t]*collisions:(0-9)[ \t]+",
    IF_COLLISIONS
  },
  {
    "^[ \t]*RX bytes:([0-9]+)[ \t]+.*TX bytes:([0-9]+)[ \t]+",
    IF_RX_TX_BYTES
  },
  {
    "^[ \t]+(UP|DOWN)[ \t]+(.*)MTU:([0-9]+)",
    IF_STATUS_FLAGS_MTU
  },
  {
    "^[ \t]+Interrupt:([0-9]+) Base address:(0x[0-9a-fA-F]+)",
    IF_INTERRUPT_BASEADDR
  },
  {
    "^[ \t]*$",
    IF_END_OF_RECORD
  },
};



NetConfPlugin::NetConfPlugin(void)
{
  routeScanner.reset();
  ifconfigScanner.reset();
}



/** Sets the device search criteria.  Returns 0 on success or -1 if it is
 *  unable to search for that type.  Calling this function resets the
 *  locator pointer */
int NetConfPlugin::setDeviceSearchCriteria(Uint16 base_class, 
					   Uint16 sub_class,
					   Uint16 prog_if)
{
  /// execv() invocation to run "route"
  const char* args1[] = { "route", "-n", NULL };
  const char* command1 = args1[0];

  /// execv() invocation to run "ifconfig"
  const char* args2[] = { "ifconfig", NULL };
  const char* command2 = args2[0];

  int i;


  /** Regular expression to parse the output of route -n, and retrieve
   *  fields 1, 2, 3, 4, 8.  Will skip routes which have no FLAGS assigned,
   *  but those routes cannot be active (the U flag would be set if
   *  active). */
  char const *regex_patts1[] = { 
    "^(" DOTTEDQUAD ")[ \t]+(" DOTTEDQUAD ")[ \t]+(" DOTTEDQUAD ")[ \t]+([^ \t]+)[ \t]+([^ \t]+[ \t]+){3}([^ \t]+.*)$",
    NULL 
  };

  char const *regex_patts2[N_IN_ARRAY(if_data) + 1];

  if (base_class != WILDCARD_DEVICE &&
      base_class != Device_IPV4NetworkSettings)
    return -1;


  /* Set the internal search criteria */
  baseClass = base_class;
  subClass = sub_class;
  progIF = prog_if;


  /* Delete the old exec readers (if any). */
  routeScanner.reset();
  ifconfigScanner.reset();

  if (sub_class == WILDCARD_DEVICE ||
      sub_class == Device_IPV4NetworkSettings_IPRoutes)
    routeScanner.reset(new ExecScanner);
    if ((routeScanner.get()) == NULL) {
      ifconfigScanner.reset();
      return -1;
    }

  if (sub_class == WILDCARD_DEVICE ||
      sub_class == Device_IPV4NetworkSettings_Interfaces)
    ifconfigScanner.reset(new ExecScanner);
    if ((ifconfigScanner.get()) == NULL) {
      routeScanner.reset();
      return -1;
    }
  

  for (i = 0; i < N_IN_ARRAY(if_data); i++)
    regex_patts2[i] = if_data[i].regex;
  regex_patts2[i] = NULL;

  try {
    if (routeScanner.get() != NULL)
      routeScanner->SetSearchRegexps(regex_patts1);

    if (ifconfigScanner.get() != NULL)
      ifconfigScanner->SetSearchRegexps(regex_patts2);
  }
  catch (Exception & e) {
    return -1;
  }

  // Spawn the jobs which will feed us data.
  if ((routeScanner.get() != NULL &&
       routeScanner->ExecuteForScan(command1, args1) != 0) ||
      (ifconfigScanner.get() != NULL &&
       ifconfigScanner->ExecuteForScan(command2, args2) != 0)) {
    routeScanner.reset();
    ifconfigScanner.reset();
    return -1;
  }

  // If the setup doesn't correspond to anything I can do, shut down.
  if (routeScanner.get() == NULL &&
      ifconfigScanner.get() == NULL)
    return -1;


  /* We are all set up ready to go! */
  return 0;
}



/* Returns a pointer to a DeviceInformation class or NULL if the last device of
 * that type was located.  This method allocated the DeviceInformation object
 * */
IPV4NetInformation *NetConfPlugin::getNextDevice(void)
{
  String junk_line;
  vector<String> matches;
  IPV4RouteInformation *route;
  IPV4IFInformation *iface;
  int index, i;
  bool end_of_record, valid_data;
  static bool end_of_data = false;
  int matchp = -1;

  // First, do the routes.
  if (routeScanner.get() != NULL) {
    while (routeScanner->GetNextMatchingLine(junk_line, NULL, matches) != -1) {

      route = new IPV4RouteInformation;
      route->SetDestination(matches[1]);
      route->SetGateway(matches[2]);
      route->SetMask(matches[3]);
      route->SetRouteDynamicFromFlags(matches[4]);
      route->SetInterface(matches[6]);
      route->setBaseClass(Device_IPV4NetworkSettings);
      route->setSubClass(Device_IPV4NetworkSettings_IPRoutes);
      return (IPV4NetInformation *) route;
    }
    
    // If we get here, we've run out of routes data.
    routeScanner.reset();
  }
  

  // Now do the interfaces.
  if (ifconfigScanner.get() != NULL) {

    while (!end_of_data) {

      iface = new IPV4IFInformation;
      end_of_record = valid_data = false;

      while (!end_of_record &&
	     (matchp = ifconfigScanner->GetNextMatchingLine(junk_line, &index, matches)) != -1) {
	valid_data = true;
	switch(if_data[index].linetype) {
	case IF_LINK_ENCAP_WITH_HWADDR:
	  iface->SetHWAddr(matches[3]);
	  // Fall through 
	case IF_LINK_ENCAP_WITHOUT_HWADDR:
	  iface->SetInterface(matches[1]);
	  /* Remove extra spaces off the end because the regex is greedy */
	  try {
	  	while(matches[2][matches[2].size()-1]==' '){
			matches[2].remove(matches[2].size()-1);
	  	}
	  } catch (IndexOutOfBoundsException&){
		/* Just break out if we get out of bounds */
	  }
	  iface->SetEncapsulation(matches[2]);
	  break;
	case IF_IPV4_PARMS_WITHOUT_BCAST:
	  iface->SetAddress(matches[1]);
	  iface->SetNetmask(matches[2]);
	  break;
	case IF_IPV4_PARMS_WITH_BCAST:
	  iface->SetAddress(matches[1]);
	  iface->SetBcast(matches[2]);
	  iface->SetNetmask(matches[3]);
	  break;
	case IF_RX_INFO:
        {
          unsigned long ul[6];
          for (int ii = 1; ii < 6; ii++)
          {
             ul[ii] = strtoul(matches[ii].getCString(), NULL, 10);
          }
	  iface->SetRxPackets(ul[1]);
	  iface->SetRxErrStats(ul[2], ul[3], ul[4], ul[5]);
	  break;
        }
	case IF_TX_INFO:
        {
          unsigned long ul[6];
          for (int ii = 1; ii < 6; ii++)
          {
             ul[ii] = strtoul(matches[ii].getCString(), NULL, 10);
          }
	  iface->SetTxPackets(ul[1]);
	  iface->SetTxErrStats(ul[2], ul[3], ul[4], ul[5]);
	  break;
        }
	case IF_COLLISIONS:
        {
	  iface->SetCollisions(strtoul(matches[1].getCString(), NULL, 10));
	  break;
        }
	case IF_RX_TX_BYTES:
        {
	  iface->SetRxBytes(strtoul(matches[1].getCString(), NULL, 10));
	  iface->SetTxBytes(strtoul(matches[2].getCString(), NULL, 10));
	  break;
        }
	case IF_STATUS_FLAGS_MTU:
	  iface->SetStatus(matches[1]);
	  iface->SetFlags(matches[2]);
	  iface->SetMTU(matches[3]);
	  break;
	case IF_INTERRUPT_BASEADDR:
	  iface->SetInterrupt(matches[1]);
	  iface->SetBaseAddr(matches[2]);
	  break;
	case IF_END_OF_RECORD:
	  end_of_record = true;
	  break;
	}
      }

      if (!valid_data) {   // happens when we get to the end of the stream
    ifconfigScanner.reset();
	return NULL;
      }

      iface->setBaseClass(Device_IPV4NetworkSettings);
      iface->setSubClass(Device_IPV4NetworkSettings_Interfaces);
      for (i = 0; i < N_IN_ARRAY(encap_lookup); i++)
	if (iface->GetEncapsulation() == encap_lookup[i].ifconfig_string)
	  iface->setProgIF(encap_lookup[i].prog_if_value);


      if (matchp == -1) {
	end_of_data = true; 
    ifconfigScanner.reset();
      }


      if (progIF != WILDCARD_DEVICE) {

	for (i = 0; i < N_IN_ARRAY(encap_lookup); i++) {
	  if (progIF == encap_lookup[i].prog_if_value &&
	      iface->GetEncapsulation() == encap_lookup[i].ifconfig_string)
	    return (IPV4NetInformation *) iface;
	}

	delete iface;
	continue;
      }

      return (IPV4NetInformation *) iface;
    }
  }

  ifconfigScanner.reset(NULL);

  return NULL;
}


PEGASUS_NAMESPACE_END
