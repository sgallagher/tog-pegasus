#%/////////////////////////////////////////////////////////////////////////////
#
# Copyright (c) 2000, 2001, 2002 BMC Software, Hewlett-Packard Company, IBM,
# The Open Group, Tivoli Systems
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to
# deal in the Software without restriction, including without limitation the
# rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
# sell copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
# 
# THE ABOVE COPYRIGHT NOTICE AND THIS PERMISSION NOTICE SHALL BE INCLUDED IN
# ALL COPIES OR SUBSTANTIAL PORTIONS OF THE SOFTWARE. THE SOFTWARE IS PROVIDED
# "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT
# LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
# PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
# HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
# ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
# WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
#
#==============================================================================
#
# Author: Warren Otsuka (warren.otsuka@hp.com)
#
# Modified By:
#
#%/////////////////////////////////////////////////////////////////////////////
#
# openpegasus.org-wbem-2.2.spec
#
# Package spec for PEGASUS 2.2
#

Summary: WBEM Services for Linux
Name: pegasus-wbem
Version: 2.2
Release: 2
Group: Systems Management/Base
Copyright: Open Group Pegasus Open Source
BuildRoot: %{_tmppath}/%{name}-%{version}-%{release}
Source: ftp://www.opengroup.org/pegasus/pegasus-%{version}.tar.gz
Requires: openssl-devel >= 0.9.6
Provides: cimserver pegasus-2.2

%description
WBEM Services for Red Hat Linux enables management solutions that deliver
increased control of enterprise resources. WBEM is a platform and resource
independent DMTF standard that defines a common information model and
communication protocol for monitoring and controlling resources from diverse
sources.

%prep
[ "$RPM_BUILD_ROOT" != "/" ] && [ -d $RPM_BUILD_ROOT ] && rm -rf $RPM_BUILD_ROOT;

%install
#
# Make directories
mkdir -p $RPM_BUILD_ROOT/etc/rc.d/init.d
mkdir -p $RPM_BUILD_ROOT/var/log/pegasus
mkdir -p $RPM_BUILD_ROOT/var/cache/pegasus/localauth
mkdir -p $RPM_BUILD_ROOT/usr/lib/pegasus/providers
mkdir -p $RPM_BUILD_ROOT/usr/share/man/{man1,man1m}
mkdir -p $RPM_BUILD_ROOT/etc/pegasus/mof/{InterOp,Internal,ManagedSystem}
#
# Init scripts
#install -D -m 0755 -o 0 -g 0 $PEGASUS_ROOT/rpm/wbem1.rh7 $RPM_BUILD_ROOT/etc/rc.d/init.d/pegasus-wbem
install -D -m 0755 -o 0 -g 0 $PEGASUS_ROOT/rpm/wbem1.rh7 $RPM_BUILD_ROOT/usr/sbin/pegasus-wbem
#
# Programs
install -D -m 0544 -o 0 -g 0 $PEGASUS_HOME/bin/cimserver $RPM_BUILD_ROOT/usr/sbin/cimserver
install -D -m 0544 -o 0 -g 0 $PEGASUS_HOME/bin/cimauth   $RPM_BUILD_ROOT/usr/sbin/cimauth
install -D -m 0544 -o 0 -g 0 $PEGASUS_HOME/bin/cimconfig $RPM_BUILD_ROOT/usr/sbin/cimconfig
install -D -m 0755 -o 0 -g 0 $PEGASUS_HOME/bin/cimmof    $RPM_BUILD_ROOT/usr/bin/cimmof
install -D -m 0755 -o 0 -g 0 $PEGASUS_HOME/bin/wbemexec  $RPM_BUILD_ROOT/usr/bin/wbemexec
install -D -m 0755 -o 0 -g 0 $PEGASUS_HOME/bin/osinfo    $RPM_BUILD_ROOT/usr/bin/osinfo
install -D -m 0755 -o 0 -g 0 $PEGASUS_HOME/bin/cimprovider $RPM_BUILD_ROOT/usr/bin/cimprovider

# Libraries
install -D -m 0755 -o 0 -g 0 $PEGASUS_HOME/lib/libCIMxmlIndicationHandler.so.1  $RPM_BUILD_ROOT/usr/lib/pegasus/libCIMxmlIndicationHandler.so.1
install -D -m 0755 -o 0 -g 0 $PEGASUS_HOME/lib/libComputerSystemProvider.so.1  $RPM_BUILD_ROOT/usr/lib/pegasus/libComputerSystemProvider.so.1
install -D -m 0755 -o 0 -g 0 $PEGASUS_HOME/lib/libConfigSettingProvider.so.1  $RPM_BUILD_ROOT/usr/lib/pegasus/libConfigSettingProvider.so.1
install -D -m 0755 -o 0 -g 0 $PEGASUS_HOME/lib/libNamespaceProvider.so.1  $RPM_BUILD_ROOT/usr/lib/pegasus/libNamespaceProvider.so.1
install -D -m 0755 -o 0 -g 0 $PEGASUS_HOME/lib/libpegCLIClientLib.so.1  $RPM_BUILD_ROOT/usr/lib/pegasus/libpegCLIClientLib.so.1
install -D -m 0755 -o 0 -g 0 $PEGASUS_HOME/lib/libOSProvider.so.1  $RPM_BUILD_ROOT/usr/lib/pegasus/libOSProvider.so.1
install -D -m 0755 -o 0 -g 0 $PEGASUS_HOME/lib/libpegauthentication.so.1  $RPM_BUILD_ROOT/usr/lib/pegasus/libpegauthentication.so.1
install -D -m 0755 -o 0 -g 0 $PEGASUS_HOME/lib/libpegclient.so.1  $RPM_BUILD_ROOT/usr/lib/pegasus/libpegclient.so.1
install -D -m 0755 -o 0 -g 0 $PEGASUS_HOME/lib/libpegcliutils.so.1  $RPM_BUILD_ROOT/usr/lib/pegasus/libpegcliutils.so.1
install -D -m 0755 -o 0 -g 0 $PEGASUS_HOME/lib/libpegcommon.so.1  $RPM_BUILD_ROOT/usr/lib/pegasus/libpegcommon.so.1
install -D -m 0755 -o 0 -g 0 $PEGASUS_HOME/lib/libpegcompiler.so.1  $RPM_BUILD_ROOT/usr/lib/pegasus/libpegcompiler.so.1
install -D -m 0755 -o 0 -g 0 $PEGASUS_HOME/lib/libpegconfig.so.1  $RPM_BUILD_ROOT/usr/lib/pegasus/libpegconfig.so.1
install -D -m 0755 -o 0 -g 0 $PEGASUS_HOME/lib/libpegexportclient.so.1  $RPM_BUILD_ROOT/usr/lib/pegasus/libpegexportclient.so.1
install -D -m 0755 -o 0 -g 0 $PEGASUS_HOME/lib/libpegexportserver.so.1  $RPM_BUILD_ROOT/usr/lib/pegasus/libpegexportserver.so.1
install -D -m 0755 -o 0 -g 0 $PEGASUS_HOME/lib/libpeggetoopt.so.1  $RPM_BUILD_ROOT/usr/lib/pegasus/libpeggetoopt.so.1
install -D -m 0755 -o 0 -g 0 $PEGASUS_HOME/lib/libpeghandlerservice.so.1  $RPM_BUILD_ROOT/usr/lib/pegasus/libpeghandlerservice.so.1
install -D -m 0755 -o 0 -g 0 $PEGASUS_HOME/lib/libpegindicationservice.so.1  $RPM_BUILD_ROOT/usr/lib/pegasus/libpegindicationservice.so.1
install -D -m 0755 -o 0 -g 0 $PEGASUS_HOME/lib/libpeglistener.so.1  $RPM_BUILD_ROOT/usr/lib/pegasus/libpeglistener.so.1
install -D -m 0755 -o 0 -g 0 $PEGASUS_HOME/lib/libpegprm.so.1  $RPM_BUILD_ROOT/usr/lib/pegasus/libpegprm.so.1
install -D -m 0755 -o 0 -g 0 $PEGASUS_HOME/lib/libpegprovidermanager.so.1  $RPM_BUILD_ROOT/usr/lib/pegasus/libpegprovidermanager.so.1
install -D -m 0755 -o 0 -g 0 $PEGASUS_HOME/lib/libpegprovider.so.1  $RPM_BUILD_ROOT/usr/lib/pegasus/libpegprovider.so.1
install -D -m 0755 -o 0 -g 0 $PEGASUS_HOME/lib/libpegrepository.so.1  $RPM_BUILD_ROOT/usr/lib/pegasus/libpegrepository.so.1
install -D -m 0755 -o 0 -g 0 $PEGASUS_HOME/lib/libpegserver.so.1  $RPM_BUILD_ROOT/usr/lib/pegasus/libpegserver.so.1
install -D -m 0755 -o 0 -g 0 $PEGASUS_HOME/lib/libpeguser.so.1  $RPM_BUILD_ROOT/usr/lib/pegasus/libpeguser.so.1
install -D -m 0755 -o 0 -g 0 $PEGASUS_HOME/lib/libpegwql.so.1  $RPM_BUILD_ROOT/usr/lib/pegasus/libpegwql.so.1
install -D -m 0755 -o 0 -g 0 $PEGASUS_HOME/lib/libPG_TestPropertyTypes.so.1  $RPM_BUILD_ROOT/usr/lib/pegasus/libPG_TestPropertyTypes.so.1
install -D -m 0755 -o 0 -g 0 $PEGASUS_HOME/lib/libProcessProvider.so.1  $RPM_BUILD_ROOT/usr/lib/pegasus/libProcessProvider.so.1
install -D -m 0755 -o 0 -g 0 $PEGASUS_HOME/lib/libProviderRegistrationProvider.so.1  $RPM_BUILD_ROOT/usr/lib/pegasus/libProviderRegistrationProvider.so.1
install -D -m 0755 -o 0 -g 0 $PEGASUS_HOME/lib/libRT_IndicationConsumer.so.1  $RPM_BUILD_ROOT/usr/lib/pegasus/libRT_IndicationConsumer.so.1
install -D -m 0755 -o 0 -g 0 $PEGASUS_HOME/lib/libRT_IndicationProvider.so.1  $RPM_BUILD_ROOT/usr/lib/pegasus/libRT_IndicationProvider.so.1
install -D -m 0755 -o 0 -g 0 $PEGASUS_HOME/lib/libSampleIndicationProvider.so.1  $RPM_BUILD_ROOT/usr/lib/pegasus/libSampleIndicationProvider.so.1
install -D -m 0755 -o 0 -g 0 $PEGASUS_HOME/lib/libSampleInstanceProvider.so.1  $RPM_BUILD_ROOT/usr/lib/pegasus/libSampleInstanceProvider.so.1
install -D -m 0755 -o 0 -g 0 $PEGASUS_HOME/lib/libSampleMethodProvider.so.1  $RPM_BUILD_ROOT/usr/lib/pegasus/libSampleMethodProvider.so.1
install -D -m 0755 -o 0 -g 0 $PEGASUS_HOME/lib/libsendmailIndicationHandler.so.1  $RPM_BUILD_ROOT/usr/lib/pegasus/libsendmailIndicationHandler.so.1
install -D -m 0755 -o 0 -g 0 $PEGASUS_HOME/lib/libsnmpIndicationHandler.so.1  $RPM_BUILD_ROOT/usr/lib/pegasus/libsnmpIndicationHandler.so.1
install -D -m 0755 -o 0 -g 0 $PEGASUS_HOME/lib/libUserAuthProvider.so.1  $RPM_BUILD_ROOT/usr/lib/pegasus/libUserAuthProvider.so.1

#
# CIM schema
#
install -D -m 0644 -o 0 -g 0 $PEGASUS_ROOT/Schemas/CIM27/CIM_Core27.mof $RPM_BUILD_ROOT/etc/pegasus/mof/CIM_Core27.mof
install -D -m 0644 -o 0 -g 0 $PEGASUS_ROOT/Schemas/CIM27/Core27_Qualifiers.mof $RPM_BUILD_ROOT/etc/pegasus/mof/Core27_Qualifiers.mof
install -D -m 0644 -o 0 -g 0 $PEGASUS_ROOT/Schemas/CIM27/Core27_CoreElements.mof $RPM_BUILD_ROOT/etc/pegasus/mof/Core27_CoreElements.mof
install -D -m 0644 -o 0 -g 0 $PEGASUS_ROOT/Schemas/CIM27/Core27_Physical.mof $RPM_BUILD_ROOT/etc/pegasus/mof/Core27_Physical.mof
install -D -m 0644 -o 0 -g 0 $PEGASUS_ROOT/Schemas/CIM27/Core27_Software.mof $RPM_BUILD_ROOT/etc/pegasus/mof/Core27_Software.mof
install -D -m 0644 -o 0 -g 0 $PEGASUS_ROOT/Schemas/CIM27/Core27_Device.mof $RPM_BUILD_ROOT/etc/pegasus/mof/Core27_Device.mof
install -D -m 0644 -o 0 -g 0 $PEGASUS_ROOT/Schemas/CIM27/Core27_StorageExtent.mof $RPM_BUILD_ROOT/etc/pegasus/mof/Core27_StorageExtent.mof
install -D -m 0644 -o 0 -g 0 $PEGASUS_ROOT/Schemas/CIM27/Core27_Collection.mof $RPM_BUILD_ROOT/etc/pegasus/mof/Core27_Collection.mof
install -D -m 0644 -o 0 -g 0 $PEGASUS_ROOT/Schemas/CIM27/Core27_Redundancy.mof $RPM_BUILD_ROOT/etc/pegasus/mof/Core27_Redundancy.mof
install -D -m 0644 -o 0 -g 0 $PEGASUS_ROOT/Schemas/CIM27/Core27_ProductFRU.mof $RPM_BUILD_ROOT/etc/pegasus/mof/Core27_ProductFRU.mof
install -D -m 0644 -o 0 -g 0 $PEGASUS_ROOT/Schemas/CIM27/Core27_Statistics.mof $RPM_BUILD_ROOT/etc/pegasus/mof/Core27_Statistics.mof
install -D -m 0644 -o 0 -g 0 $PEGASUS_ROOT/Schemas/CIM27/Core27_Capabilities.mof $RPM_BUILD_ROOT/etc/pegasus/mof/Core27_Capabilities.mof
install -D -m 0644 -o 0 -g 0 $PEGASUS_ROOT/Schemas/CIM27/Core27_Settings.mof $RPM_BUILD_ROOT/etc/pegasus/mof/Core27_Settings.mof
install -D -m 0644 -o 0 -g 0 $PEGASUS_ROOT/Schemas/CIM27/Core27_MethodParms.mof $RPM_BUILD_ROOT/etc/pegasus/mof/Core27_MethodParms.mof
install -D -m 0644 -o 0 -g 0 $PEGASUS_ROOT/Schemas/CIM27/Core27_PowerMgmt.mof $RPM_BUILD_ROOT/etc/pegasus/mof/Core27_PowerMgmt.mof
install -D -m 0644 -o 0 -g 0 $PEGASUS_ROOT/Schemas/CIM27/Application27_BIOS.mof $RPM_BUILD_ROOT/etc/pegasus/mof/Application27_BIOS.mof
install -D -m 0644 -o 0 -g 0 $PEGASUS_ROOT/Schemas/CIM27/Application27_CheckAction.mof $RPM_BUILD_ROOT/etc/pegasus/mof/Application27_CheckAction.mof
install -D -m 0644 -o 0 -g 0 $PEGASUS_ROOT/Schemas/CIM27/Application27_DeploymentModel.mof $RPM_BUILD_ROOT/etc/pegasus/mof/Application27_DeploymentModel.mof
install -D -m 0644 -o 0 -g 0 $PEGASUS_ROOT/Schemas/CIM27/Application27_InstalledProduct.mof $RPM_BUILD_ROOT/etc/pegasus/mof/Application27_InstalledProduct.mof
install -D -m 0644 -o 0 -g 0 $PEGASUS_ROOT/Schemas/CIM27/Application27_SystemSoftware.mof $RPM_BUILD_ROOT/etc/pegasus/mof/Application27_SystemSoftware.mof
install -D -m 0644 -o 0 -g 0 $PEGASUS_ROOT/Schemas/CIM27/CIM_Application27.mof $RPM_BUILD_ROOT/etc/pegasus/mof/CIM_Application27.mof
install -D -m 0644 -o 0 -g 0 $PEGASUS_ROOT/Schemas/CIM27/CIM_Database27.mof $RPM_BUILD_ROOT/etc/pegasus/mof/CIM_Database27.mof
install -D -m 0644 -o 0 -g 0 $PEGASUS_ROOT/Schemas/CIM27/CIM_Device27.mof $RPM_BUILD_ROOT/etc/pegasus/mof/CIM_Device27.mof
install -D -m 0644 -o 0 -g 0 $PEGASUS_ROOT/Schemas/CIM27/CIM_Event27.mof $RPM_BUILD_ROOT/etc/pegasus/mof/CIM_Event27.mof
install -D -m 0644 -o 0 -g 0 $PEGASUS_ROOT/Schemas/CIM27/CIM_Interop27.mof $RPM_BUILD_ROOT/etc/pegasus/mof/CIM_Interop27.mof 
install -D -m 0644 -o 0 -g 0 $PEGASUS_ROOT/Schemas/CIM27/CIM_Metrics27.mof $RPM_BUILD_ROOT/etc/pegasus/mof/CIM_Metrics27.mof
install -D -m 0644 -o 0 -g 0 $PEGASUS_ROOT/Schemas/CIM27/CIM_Network27.mof $RPM_BUILD_ROOT/etc/pegasus/mof/CIM_Network27.mof
install -D -m 0644 -o 0 -g 0 $PEGASUS_ROOT/Schemas/CIM27/CIM_Physical27.mof $RPM_BUILD_ROOT/etc/pegasus/mof/CIM_Physical27.mof
install -D -m 0644 -o 0 -g 0 $PEGASUS_ROOT/Schemas/CIM27/CIM_Policy27.mof $RPM_BUILD_ROOT/etc/pegasus/mof/CIM_Policy27.mof
install -D -m 0644 -o 0 -g 0 $PEGASUS_ROOT/Schemas/CIM27/CIM_Schema27.mof $RPM_BUILD_ROOT/etc/pegasus/mof/CIM_Schema27.mof
install -D -m 0644 -o 0 -g 0 $PEGASUS_ROOT/Schemas/CIM27/CIM_Support27.mof $RPM_BUILD_ROOT/etc/pegasus/mof/CIM_Support27.mof
install -D -m 0644 -o 0 -g 0 $PEGASUS_ROOT/Schemas/CIM27/CIM_System27.mof $RPM_BUILD_ROOT/etc/pegasus/mof/CIM_System27.mof
install -D -m 0644 -o 0 -g 0 $PEGASUS_ROOT/Schemas/CIM27/CIM_User27.mof $RPM_BUILD_ROOT/etc/pegasus/mof/CIM_User27.mof
install -D -m 0644 -o 0 -g 0 $PEGASUS_ROOT/Schemas/CIM27/Device27_Controller.mof $RPM_BUILD_ROOT/etc/pegasus/mof/Device27_Controller.mof
install -D -m 0644 -o 0 -g 0 $PEGASUS_ROOT/Schemas/CIM27/Device27_CoolingAndPower.mof $RPM_BUILD_ROOT/etc/pegasus/mof/Device27_CoolingAndPower.mof
install -D -m 0644 -o 0 -g 0 $PEGASUS_ROOT/Schemas/CIM27/Device27_DeviceElements.mof $RPM_BUILD_ROOT/etc/pegasus/mof/Device27_DeviceElements.mof
install -D -m 0644 -o 0 -g 0 $PEGASUS_ROOT/Schemas/CIM27/Device27_DiskGroup.mof $RPM_BUILD_ROOT/etc/pegasus/mof/Device27_DiskGroup.mof
install -D -m 0644 -o 0 -g 0 $PEGASUS_ROOT/Schemas/CIM27/Device27_FC.mof $RPM_BUILD_ROOT/etc/pegasus/mof/Device27_FC.mof
install -D -m 0644 -o 0 -g 0 $PEGASUS_ROOT/Schemas/CIM27/Device27_IB.mof $RPM_BUILD_ROOT/etc/pegasus/mof/Device27_IB.mof
install -D -m 0644 -o 0 -g 0 $PEGASUS_ROOT/Schemas/CIM27/Device27_Memory.mof $RPM_BUILD_ROOT/etc/pegasus/mof/Device27_Memory.mof
install -D -m 0644 -o 0 -g 0 $PEGASUS_ROOT/Schemas/CIM27/Device27_Modems.mof $RPM_BUILD_ROOT/etc/pegasus/mof/Device27_Modems.mof
install -D -m 0644 -o 0 -g 0 $PEGASUS_ROOT/Schemas/CIM27/Device27_NetworkAdapter.mof $RPM_BUILD_ROOT/etc/pegasus/mof/Device27_NetworkAdapter.mof
install -D -m 0644 -o 0 -g 0 $PEGASUS_ROOT/Schemas/CIM27/Device27_Ports.mof $RPM_BUILD_ROOT/etc/pegasus/mof/Device27_Ports.mof
install -D -m 0644 -o 0 -g 0 $PEGASUS_ROOT/Schemas/CIM27/Device27_Printing.mof $RPM_BUILD_ROOT/etc/pegasus/mof/Device27_Printing.mof
install -D -m 0644 -o 0 -g 0 $PEGASUS_ROOT/Schemas/CIM27/Device27_Processor.mof $RPM_BUILD_ROOT/etc/pegasus/mof/Device27_Processor.mof
install -D -m 0644 -o 0 -g 0 $PEGASUS_ROOT/Schemas/CIM27/Device27_SccExtents.mof $RPM_BUILD_ROOT/etc/pegasus/mof/Device27_SccExtents.mof
install -D -m 0644 -o 0 -g 0 $PEGASUS_ROOT/Schemas/CIM27/Device27_Sensors.mof $RPM_BUILD_ROOT/etc/pegasus/mof/Device27_Sensors.mof
install -D -m 0644 -o 0 -g 0 $PEGASUS_ROOT/Schemas/CIM27/Device27_StorageDevices.mof $RPM_BUILD_ROOT/etc/pegasus/mof/Device27_StorageDevices.mof
install -D -m 0644 -o 0 -g 0 $PEGASUS_ROOT/Schemas/CIM27/Device27_StorageExtents.mof $RPM_BUILD_ROOT/etc/pegasus/mof/Device27_StorageExtents.mof
install -D -m 0644 -o 0 -g 0 $PEGASUS_ROOT/Schemas/CIM27/Device27_StorageLib.mof $RPM_BUILD_ROOT/etc/pegasus/mof/Device27_StorageLib.mof
install -D -m 0644 -o 0 -g 0 $PEGASUS_ROOT/Schemas/CIM27/Device27_StorageServices.mof $RPM_BUILD_ROOT/etc/pegasus/mof/Device27_StorageServices.mof
install -D -m 0644 -o 0 -g 0 $PEGASUS_ROOT/Schemas/CIM27/Device27_USB.mof $RPM_BUILD_ROOT/etc/pegasus/mof/Device27_USB.mof
install -D -m 0644 -o 0 -g 0 $PEGASUS_ROOT/Schemas/CIM27/Device27_UserDevices.mof $RPM_BUILD_ROOT/etc/pegasus/mof/Device27_UserDevices.mof
install -D -m 0644 -o 0 -g 0 $PEGASUS_ROOT/Schemas/CIM27/Metrics27_BaseMetric.mof $RPM_BUILD_ROOT/etc/pegasus/mof/Metrics27_BaseMetric.mof
install -D -m 0644 -o 0 -g 0 $PEGASUS_ROOT/Schemas/CIM27/Metrics27_UnitOfWork.mof $RPM_BUILD_ROOT/etc/pegasus/mof/Metrics27_UnitOfWork.mof
install -D -m 0644 -o 0 -g 0 $PEGASUS_ROOT/Schemas/CIM27/Network27_BGP.mof $RPM_BUILD_ROOT/etc/pegasus/mof/Network27_BGP.mof
install -D -m 0644 -o 0 -g 0 $PEGASUS_ROOT/Schemas/CIM27/Network27_Buffers.mof $RPM_BUILD_ROOT/etc/pegasus/mof/Network27_Buffers.mof
install -D -m 0644 -o 0 -g 0 $PEGASUS_ROOT/Schemas/CIM27/Network27_Collections.mof $RPM_BUILD_ROOT/etc/pegasus/mof/Network27_Collections.mof
install -D -m 0644 -o 0 -g 0 $PEGASUS_ROOT/Schemas/CIM27/Network27_Filtering.mof $RPM_BUILD_ROOT/etc/pegasus/mof/Network27_Filtering.mof
install -D -m 0644 -o 0 -g 0 $PEGASUS_ROOT/Schemas/CIM27/Network27_OSPF.mof $RPM_BUILD_ROOT/etc/pegasus/mof/Network27_OSPF.mof
install -D -m 0644 -o 0 -g 0 $PEGASUS_ROOT/Schemas/CIM27/Network27_Pipes.mof $RPM_BUILD_ROOT/etc/pegasus/mof/Network27_Pipes.mof
install -D -m 0644 -o 0 -g 0 $PEGASUS_ROOT/Schemas/CIM27/Network27_ProtocolEndpoints.mof $RPM_BUILD_ROOT/etc/pegasus/mof/Network27_ProtocolEndpoints.mof
install -D -m 0644 -o 0 -g 0 $PEGASUS_ROOT/Schemas/CIM27/Network27_QoS.mof $RPM_BUILD_ROOT/etc/pegasus/mof/Network27_QoS.mof
install -D -m 0644 -o 0 -g 0 $PEGASUS_ROOT/Schemas/CIM27/Network27_Routes.mof $RPM_BUILD_ROOT/etc/pegasus/mof/Network27_Routes.mof
install -D -m 0644 -o 0 -g 0 $PEGASUS_ROOT/Schemas/CIM27/Network27_RoutingForwarding.mof $RPM_BUILD_ROOT/etc/pegasus/mof/Network27_RoutingForwarding.mof
install -D -m 0644 -o 0 -g 0 $PEGASUS_ROOT/Schemas/CIM27/Network27_SNMP.mof $RPM_BUILD_ROOT/etc/pegasus/mof/Network27_SNMP.mof
install -D -m 0644 -o 0 -g 0 $PEGASUS_ROOT/Schemas/CIM27/Network27_SwitchingBridging.mof $RPM_BUILD_ROOT/etc/pegasus/mof/Network27_SwitchingBridging.mof
install -D -m 0644 -o 0 -g 0 $PEGASUS_ROOT/Schemas/CIM27/Network27_Systems.mof $RPM_BUILD_ROOT/etc/pegasus/mof/Network27_Systems.mof
install -D -m 0644 -o 0 -g 0 $PEGASUS_ROOT/Schemas/CIM27/Network27_VLAN.mof $RPM_BUILD_ROOT/etc/pegasus/mof/Network27_VLAN.mof
install -D -m 0644 -o 0 -g 0 $PEGASUS_ROOT/Schemas/CIM27/Physical27_Component.mof $RPM_BUILD_ROOT/etc/pegasus/mof/Physical27_Component.mof
install -D -m 0644 -o 0 -g 0 $PEGASUS_ROOT/Schemas/CIM27/Physical27_Connector.mof $RPM_BUILD_ROOT/etc/pegasus/mof/Physical27_Connector.mof
install -D -m 0644 -o 0 -g 0 $PEGASUS_ROOT/Schemas/CIM27/Physical27_Link.mof $RPM_BUILD_ROOT/etc/pegasus/mof/Physical27_Link.mof
install -D -m 0644 -o 0 -g 0 $PEGASUS_ROOT/Schemas/CIM27/Physical27_Misc.mof $RPM_BUILD_ROOT/etc/pegasus/mof/Physical27_Misc.mof
install -D -m 0644 -o 0 -g 0 $PEGASUS_ROOT/Schemas/CIM27/Physical27_Package.mof $RPM_BUILD_ROOT/etc/pegasus/mof/Physical27_Package.mof
install -D -m 0644 -o 0 -g 0 $PEGASUS_ROOT/Schemas/CIM27/Physical27_PhysicalMedia.mof $RPM_BUILD_ROOT/etc/pegasus/mof/Physical27_PhysicalMedia.mof
install -D -m 0644 -o 0 -g 0 $PEGASUS_ROOT/Schemas/CIM27/Physical27_StoragePackage.mof $RPM_BUILD_ROOT/etc/pegasus/mof/Physical27_StoragePackage.mof
install -D -m 0644 -o 0 -g 0 $PEGASUS_ROOT/Schemas/CIM27/System27_Boot.mof $RPM_BUILD_ROOT/etc/pegasus/mof/System27_Boot.mof
install -D -m 0644 -o 0 -g 0 $PEGASUS_ROOT/Schemas/CIM27/System27_Diagnostics.mof $RPM_BUILD_ROOT/etc/pegasus/mof/System27_Diagnostics.mof
install -D -m 0644 -o 0 -g 0 $PEGASUS_ROOT/Schemas/CIM27/System27_FileElements.mof $RPM_BUILD_ROOT/etc/pegasus/mof/System27_FileElements.mof
install -D -m 0644 -o 0 -g 0 $PEGASUS_ROOT/Schemas/CIM27/System27_Logs.mof $RPM_BUILD_ROOT/etc/pegasus/mof/System27_Logs.mof
install -D -m 0644 -o 0 -g 0 $PEGASUS_ROOT/Schemas/CIM27/System27_OperatingSystem.mof $RPM_BUILD_ROOT/etc/pegasus/mof/System27_OperatingSystem.mof
install -D -m 0644 -o 0 -g 0 $PEGASUS_ROOT/Schemas/CIM27/System27_Processing.mof $RPM_BUILD_ROOT/etc/pegasus/mof/System27_Processing.mof
install -D -m 0644 -o 0 -g 0 $PEGASUS_ROOT/Schemas/CIM27/System27_SystemElements.mof $RPM_BUILD_ROOT/etc/pegasus/mof/System27_SystemElements.mof
install -D -m 0644 -o 0 -g 0 $PEGASUS_ROOT/Schemas/CIM27/System27_SystemResources.mof $RPM_BUILD_ROOT/etc/pegasus/mof/System27_SystemResources.mof
install -D -m 0644 -o 0 -g 0 $PEGASUS_ROOT/Schemas/CIM27/System27_Time.mof $RPM_BUILD_ROOT/etc/pegasus/mof/System27_Time.mof
install -D -m 0644 -o 0 -g 0 $PEGASUS_ROOT/Schemas/CIM27/System27_Unix.mof $RPM_BUILD_ROOT/etc/pegasus/mof/System27_Unix.mof
install -D -m 0644 -o 0 -g 0 $PEGASUS_ROOT/Schemas/CIM27/User27_AccessControl.mof $RPM_BUILD_ROOT/etc/pegasus/mof/User27_AccessControl.mof
install -D -m 0644 -o 0 -g 0 $PEGASUS_ROOT/Schemas/CIM27/User27_Account.mof $RPM_BUILD_ROOT/etc/pegasus/mof/User27_Account.mof
install -D -m 0644 -o 0 -g 0 $PEGASUS_ROOT/Schemas/CIM27/User27_AuthenticationReqmt.mof $RPM_BUILD_ROOT/etc/pegasus/mof/User27_AuthenticationReqmt.mof
install -D -m 0644 -o 0 -g 0 $PEGASUS_ROOT/Schemas/CIM27/User27_Credential.mof $RPM_BUILD_ROOT/etc/pegasus/mof/User27_Credential.mof
install -D -m 0644 -o 0 -g 0 $PEGASUS_ROOT/Schemas/CIM27/User27_Group.mof $RPM_BUILD_ROOT/etc/pegasus/mof/User27_Group.mof
install -D -m 0644 -o 0 -g 0 $PEGASUS_ROOT/Schemas/CIM27/User27_Kerberos.mof $RPM_BUILD_ROOT/etc/pegasus/mof/User27_Kerberos.mof
install -D -m 0644 -o 0 -g 0 $PEGASUS_ROOT/Schemas/CIM27/User27_Org.mof $RPM_BUILD_ROOT/etc/pegasus/mof/User27_Org.mof
install -D -m 0644 -o 0 -g 0 $PEGASUS_ROOT/Schemas/CIM27/User27_PublicKey.mof $RPM_BUILD_ROOT/etc/pegasus/mof/User27_PublicKey.mof
install -D -m 0644 -o 0 -g 0 $PEGASUS_ROOT/Schemas/CIM27/User27_Role.mof $RPM_BUILD_ROOT/etc/pegasus/mof/User27_Role.mof
install -D -m 0644 -o 0 -g 0 $PEGASUS_ROOT/Schemas/CIM27/User27_SecurityServices.mof $RPM_BUILD_ROOT/etc/pegasus/mof/User27_SecurityServices.mof
install -D -m 0644 -o 0 -g 0 $PEGASUS_ROOT/Schemas/CIM27/User27_SharedSecret.mof $RPM_BUILD_ROOT/etc/pegasus/mof/User27_SharedSecret.mof
install -D -m 0644 -o 0 -g 0 $PEGASUS_ROOT/Schemas/CIM27/User27_UsersAccess.mof $RPM_BUILD_ROOT/etc/pegasus/mof/User27_UsersAccess.mof

#
# Pegasus' schema
#
install -D -m 0644 -o 0 -g 0 $PEGASUS_ROOT/Schemas/Pegasus/InterOp/VER20/PG_Events20.mof   $RPM_BUILD_ROOT/etc/pegasus/mof/PG_Events20.mof
install -D -m 0644 -o 0 -g 0 $PEGASUS_ROOT/Schemas/Pegasus/InterOp/VER20/PG_InterOpSchema20.mof   $RPM_BUILD_ROOT/etc/pegasus/mof/InterOpSchema.mof
install -D -m 0644 -o 0 -g 0 $PEGASUS_ROOT/Schemas/Pegasus/InterOp/VER20/PG_ProviderModule20.mof  $RPM_BUILD_ROOT/etc/pegasus/mof/PG_ProviderModule20.mof
install -D -m 0644 -o 0 -g 0 $PEGASUS_ROOT/Schemas/Pegasus/Internal/VER20/PG_Authorization20.mof  $RPM_BUILD_ROOT/etc/pegasus/mof/PG_Authorization20.mof
install -D -m 0644 -o 0 -g 0 $PEGASUS_ROOT/Schemas/Pegasus/Internal/VER20/PG_ConfigSetting20.mof  $RPM_BUILD_ROOT/etc/pegasus/mof/PG_ConfigSetting20.mof
install -D -m 0644 -o 0 -g 0 $PEGASUS_ROOT/Schemas/Pegasus/Internal/VER20/PG_InternalSchema20.mof $RPM_BUILD_ROOT/etc/pegasus/mof/InternalSchema.mof
install -D -m 0644 -o 0 -g 0 $PEGASUS_ROOT/Schemas/Pegasus/Internal/VER20/PG_User20.mof           $RPM_BUILD_ROOT/etc/pegasus/mof/PG_User20.mof
install -D -m 0644 -o 0 -g 0 $PEGASUS_ROOT/Schemas/Pegasus/Internal/VER20/PG_ShutdownService20.mof           $RPM_BUILD_ROOT/etc/pegasus/mof/PG_ShutdownService20.mof

#
# Pegasus Default Providers' schema
#
install -D -m 0644 -o 0 -g 0 $PEGASUS_ROOT/Schemas/Pegasus/ManagedSystem/VER20/PG_OperatingSystem20.mof                               $RPM_BUILD_ROOT/etc/pegasus/mof/PG_OperatingSystem20.mof
install -D -m 0644 -o 0 -g 0 $PEGASUS_ROOT/Schemas/Pegasus/ManagedSystem/VER20/PG_OperatingSystem20R.mof                               $RPM_BUILD_ROOT/etc/pegasus/mof/PG_OperatingSystem20R.mof
install -D -m 0644 -o 0 -g 0 $PEGASUS_ROOT/Schemas/Pegasus/ManagedSystem/VER20/PG_ComputerSystem20.mof                               $RPM_BUILD_ROOT/etc/pegasus/mof/PG_ComputerSystem20.mof
install -D -m 0644 -o 0 -g 0 $PEGASUS_ROOT/Schemas/Pegasus/ManagedSystem/VER20/PG_ComputerSystem20R.mof                               $RPM_BUILD_ROOT/etc/pegasus/mof/PG_ComputerSystem20R.mof
install -D -m 0644 -o 0 -g 0 $PEGASUS_ROOT/Schemas/Pegasus/ManagedSystem/VER20/PG_ManagedSystemSchema20.mof                               $RPM_BUILD_ROOT/etc/pegasus/mof/ManagedSystemSchema.mof
install -D -m 0644 -o 0 -g 0 $PEGASUS_ROOT/Schemas/Pegasus/ManagedSystem/VER20/PG_ManagedSystemSchema20R.mof                               $RPM_BUILD_ROOT/etc/pegasus/mof/ManagedSystemSchemaR.mof
install -D -m 0644 -o 0 -g 0 $PEGASUS_ROOT/Schemas/Pegasus/ManagedSystem/VER20/PG_UnixProcess20.mof                               $RPM_BUILD_ROOT/etc/pegasus/mof/PG_UnixProcess20.mof
install -D -m 0644 -o 0 -g 0 $PEGASUS_ROOT/Schemas/Pegasus/ManagedSystem/VER20/PG_UnixProcess20R.mof                               $RPM_BUILD_ROOT/etc/pegasus/mof/PG_UnixProcess20R.mof

#
# SSL Files
#
install -D -m 0644 -o 0 -g 0 $PEGASUS_ROOT/ssl.cnf                   $RPM_BUILD_ROOT/var/cache/pegasus/ssl.orig

#
# cimserver config files
#
install -D -m 0644 -o 0 -g 0 $PEGASUS_ROOT/src/Server/cimserver_current.conf $RPM_BUILD_ROOT/etc/pegasus/cimserver_current.conf
install -D -m 0644 -o 0 -g 0 $PEGASUS_ROOT/src/Server/cimserver_planned.conf $RPM_BUILD_ROOT/etc/pegasus/cimserver_planned.conf

#
# WBEM pam authentication
#
install -D -m 0644 -o 0 -g 0 $PEGASUS_ROOT/rpm/wbem $RPM_BUILD_ROOT/etc/pam.d/wbem

#
# repository install script
#
install -D -m 0544 -o 0 -g 0 $PEGASUS_ROOT/rpm/scripts/init_repository_Linux_rh32 $RPM_BUILD_ROOT/usr/sbin/init_repository

#
# man pages
#
install -D -m 0444 -o 0 -g 0 $PEGASUS_ROOT/rpm/manLinux/man1.Z/cimmof.1 $RPM_BUILD_ROOT/usr/share/man/man1/cimmof.1
install -D -m 0444 -o 0 -g 0 $PEGASUS_ROOT/rpm/manLinux/man1.Z/cimprovider.1 $RPM_BUILD_ROOT/usr/share/man/man1/cimprovider.1
install -D -m 0444 -o 0 -g 0 $PEGASUS_ROOT/rpm/manLinux/man1.Z/osinfo.1 $RPM_BUILD_ROOT/usr/share/man/man1/osinfo.1
install -D -m 0444 -o 0 -g 0 $PEGASUS_ROOT/rpm/manLinux/man1.Z/wbemexec.1 $RPM_BUILD_ROOT/usr/share/man/man1/wbemexec.1
install -D -m 0444 -o 0 -g 0 $PEGASUS_ROOT/rpm/manLinux/man1m.Z/cimauth.1m $RPM_BUILD_ROOT/usr/share/man/man1m/cimauth.1m
install -D -m 0444 -o 0 -g 0 $PEGASUS_ROOT/rpm/manLinux/man1m.Z/cimconfig.1m $RPM_BUILD_ROOT/usr/share/man/man1m/cimconfig.1m
install -D -m 0444 -o 0 -g 0 $PEGASUS_ROOT/rpm/manLinux/man1m.Z/cimprovider.1m $RPM_BUILD_ROOT/usr/share/man/man1m/cimprovider.1m
install -D -m 0444 -o 0 -g 0 $PEGASUS_ROOT/rpm/manLinux/man1m.Z/cimserver.1m $RPM_BUILD_ROOT/usr/share/man/man1m/cimserver.1m

%clean
#make clean
[ "$RPM_BUILD_ROOT" != "/" ] && [ -d $RPM_BUILD_ROOT ] && rm -rf $RPM_BUILD_ROOT;

%post
%define INSTALL_LOG /var/log/pegasus/install.log
echo `date` >%{INSTALL_LOG} 2>&1

# RH dependency
#/sbin/chkconfig --add pegasus-wbem
grep "^/usr/lib/pegasus$" /etc/ld.so.conf > /dev/null 2> /dev/null
[ $? -ne 0 ] && echo "/usr/lib/pegasus" >> /etc/ld.so.conf
/sbin/ldconfig

#link provider shared libraries
ln -s $RPM_BUILD_ROOT/usr/lib/pegasus/libComputerSystemProvider.so.1 $RPM_BUILD_ROOT/usr/lib/pegasus/providers/libComputerSystemProvider.so
ln -s $RPM_BUILD_ROOT/usr/lib/pegasus/libComputerSystemProvider.so.1 $RPM_BUILD_ROOT/usr/lib/pegasus/providers/libComputerSystemProvider.so.1
ln -s $RPM_BUILD_ROOT/usr/lib/pegasus/libOSProvider.so.1 $RPM_BUILD_ROOT/usr/lib/pegasus/providers/libOSProvider.so.1
ln -s $RPM_BUILD_ROOT/usr/lib/pegasus/libOSProvider.so.1 $RPM_BUILD_ROOT/usr/lib/pegasus/providers/libOSProvider.so
ln -s $RPM_BUILD_ROOT/usr/lib/pegasus/libProcessProvider.so.1 $RPM_BUILD_ROOT/usr/lib/pegasus/providers/libProcessProvider.so.1
ln -s $RPM_BUILD_ROOT/usr/lib/pegasus/libProcessProvider.so.1 $RPM_BUILD_ROOT/usr/lib/pegasus/providers/libProcessProvider.so

#
#  Set up the openssl certificate
#
#  Modify entries in ssl.cnf, then
#  Create big random ssl.rnd file, then 
#  Generate a self signed node certificate
#

CN="Common Name"
EMAIL="test@email.address"
HOSTNAME=`uname -n`
sed -e "s/$CN/$HOSTNAME/"  \
    -e "s/$EMAIL/root@$HOSTNAME/" /var/cache/pegasus/ssl.orig \
    > /var/cache/pegasus/ssl.cnf
chmod 644 /var/cache/pegasus/ssl.cnf
chown bin /var/cache/pegasus/ssl.cnf
chgrp bin /var/cache/pegasus/ssl.cnf

/bin/rpm -qa >/var/cache/pegasus/ssl.rnd 2>>%{INSTALL_LOG}
/bin/netstat -a >>/var/cache/pegasus/ssl.rnd 2>>%{INSTALL_LOG}

/usr/bin/openssl req -x509 -days 365 -newkey rsa:2048 \
   -rand /var/cache/pegasus/ssl.rnd -nodes -config /var/cache/pegasus/ssl.cnf   \
   -keyout /var/cache/pegasus/key.pem -out /var/cache/pegasus/cert.pem 2>>%{INSTALL_LOG}

cat /var/cache/pegasus/key.pem /var/cache/pegasus/cert.pem > /var/cache/pegasus/server_2048.pem
cat /var/cache/pegasus/cert.pem > /var/cache/pegasus/client_2048.pem
rm /var/cache/pegasus/key.pem /var/cache/pegasus/cert.pem

if [ -f /var/cache/pegasus/server.pem ] 
then
    echo "WARNING: /var/cache/pegasus/server.pem SSL Certificate file already exists."
else
    cp /var/cache/pegasus/server_2048.pem /var/cache/pegasus/server.pem
fi

if [ -f /var/cache/pegasus/client.pem ]
then
    echo "WARNING: /var/cache/pegasus/client.pem SSL Certificate trust store already exists."
else
    cp /var/cache/pegasus/client_2048.pem /var/cache/pegasus/client.pem
fi

if [ -d "/var/cache/pegasus/repository/root#PG_Internal" ]
then
  #
  # Save the current /var/cache/pegasus/repository to
  # /var/cache/pegasus/prev_repository.
  #

  REPOSITORY_LOC="/var/cache/pegasus/repository"
  PREV_REPOSITORY_LOC="/var/cache/pegasus/prev_repository"

  if [[ -d $REPOSITORY_LOC ]]
  then
      if [[ -d $PREV_REPOSITORY_LOC ]]
      then
          rm -rf $PREV_REPOSITORY_LOC
      fi

      mv $REPOSITORY_LOC $PREV_REPOSITORY_LOC
      mkdir $REPOSITORY_LOC
  fi
fi
#/sbin/service pegasus-wbem start 2>>%{INSTALL_LOG}
/usr/sbin/cimserver
sleep 5
echo " Compiling mof files will take a few minutes."
echo " Output will be logged to %{INSTALL_LOG}."
. /usr/sbin/init_repository 2>>%{INSTALL_LOG}
#/sbin/service pegasus-wbem stop 2>>%{INSTALL_LOG}
/usr/sbin/cimserver -s

# Create symbolic links for client libs
#
cd /usr/lib/pegasus
ln -s libCIMxmlIndicationHandler.so.1       libCIMxmlIndicationHandler.so
ln -s libConfigSettingProvider.so.1         libConfigSettingProvider.so
ln -s libNamespaceProvider.so.1             libNamespaceProvider.so
ln -s libpegCLIClientLib.so.1               libpegCLIClientLib.so
ln -s libpegauthentication.so.1             libpegauthentication.so
ln -s libpegclient.so.1                     libpegclient.so
ln -s libpegcliutils.so.1                   libpegcliutils.so
ln -s libpegcommon.so.1                     libpegcommon.so
ln -s libpegcompiler.so.1                   libpegcompiler.so
ln -s libpegconfig.so.1                     libpegconfig.so
ln -s libpegexportclient.so.1               libpegexportclient.so
ln -s libpegexportserver.so.1               libpegexportserver.so
ln -s libpeggetoopt.so.1                    libpeggetoopt.so
ln -s libpeghandlerservice.so.1             libpeghandlerservice.so
ln -s libpegindicationservice.so.1          libpegindicationservice.so
ln -s libpeglistener.so.1                   libpeglistener.so
ln -s libpegprm.so.1                        libpegprm.so
ln -s libpegprovidermanager.so.1            libpegprovidermanager.so
ln -s libpegprovider.so.1                   libpegprovider.so
ln -s libpegrepository.so.1                 libpegrepository.so
ln -s libpegserver.so.1                     libpegserver.so
ln -s libpeguser.so.1                       libpeguser.so
ln -s libpegwql.so.1                        libpegwql.so
ln -s libPG_TestPropertyTypes.so.1          libPG_TestPropertyTypes.so
ln -s libProviderRegistrationProvider.so.1  libProviderRegistrationProvider.so
ln -s libRT_IndicationConsumer.so.1         libRT_IndicationConsumer.so
ln -s libRT_IndicationProvider.so.1         libRT_IndicationProvider.so
ln -s libSampleIndicationProvider.so.1      libSampleIndicationProvider.so
ln -s libSampleInstanceProvider.so.1        libSampleInstanceProvider.so
ln -s libSampleMethodProvider.so.1          libSampleMethodProvider.so
ln -s libsendmailIndicationHandler.so.1     libsendmailIndicationHandler.so
ln -s libsnmpIndicationHandler.so.1         libsnmpIndicationHandler.so
ln -s libUserAuthProvider.so.1              libUserAuthProvider.so

%preun
if [ $1 = 0 ]; then
	# RH dependency
        /usr/sbin/cimserver -s
#	/sbin/service pegasus-wbem stop > /dev/null 2> /dev/null; :
#	/sbin/chkconfig --del pegasus-wbem
fi

%postun
if [ $1 = 0 ]; then
	grep -v "/usr/lib/pegasus" /etc/ld.so.conf > /etc/ld.so.conf.new
	mv -f /etc/ld.so.conf.new /etc/ld.so.conf
	/sbin/ldconfig
#	rm -fr /etc/init.d/pegasus-wbem
	rm -rf /etc/pegasus
	rm -rf /var/cache/pegasus
        rm -f /usr/sbin/init_repository
        rm -rf /usr/lib/pegasus
        rm -f /etc/pam.d/wbem
fi

%files
%dir /var/cache/pegasus/localauth
%dir /var/log/pegasus
%dir /usr/lib/pegasus/providers
%attr(-,root,root) /usr/sbin/pegasus-wbem
%attr(-,root,root) /var/cache/pegasus/ssl.orig
%attr(-,root,root) /etc/pegasus/cimserver_planned.conf
%attr(-,root,root) /etc/pegasus/cimserver_current.conf
%attr(-,root,root) /etc/pam.d/wbem
%attr(-,root,root) /usr/sbin/cimauth
%attr(-,root,root) /usr/sbin/cimconfig
%attr(-,root,root) /usr/bin/cimmof
%attr(-,root,root) /usr/sbin/cimserver
%attr(-,root,root) /usr/sbin/init_repository
%attr(-,root,root) /usr/bin/wbemexec
%attr(-,root,root) /usr/bin/cimprovider
%attr(-,root,root) /usr/bin/osinfo
%attr(-,root,root) /usr/lib/pegasus/libCIMxmlIndicationHandler.so.1
%attr(-,root,root) /usr/lib/pegasus/libComputerSystemProvider.so.1
%attr(-,root,root) /usr/lib/pegasus/libConfigSettingProvider.so.1
%attr(-,root,root) /usr/lib/pegasus/libNamespaceProvider.so.1
%attr(-,root,root) /usr/lib/pegasus/libOSProvider.so.1
%attr(-,root,root) /usr/lib/pegasus/libpegauthentication.so.1
%attr(-,root,root) /usr/lib/pegasus/libpegclient.so.1
%attr(-,root,root) /usr/lib/pegasus/libpegcliutils.so.1
%attr(-,root,root) /usr/lib/pegasus/libpegcommon.so.1
%attr(-,root,root) /usr/lib/pegasus/libpegcompiler.so.1
%attr(-,root,root) /usr/lib/pegasus/libpegconfig.so.1
%attr(-,root,root) /usr/lib/pegasus/libpegexportclient.so.1
%attr(-,root,root) /usr/lib/pegasus/libpegexportserver.so.1
%attr(-,root,root) /usr/lib/pegasus/libpeggetoopt.so.1
%attr(-,root,root) /usr/lib/pegasus/libpeghandlerservice.so.1
%attr(-,root,root) /usr/lib/pegasus/libpegindicationservice.so.1
%attr(-,root,root) /usr/lib/pegasus/libpeglistener.so.1
%attr(-,root,root) /usr/lib/pegasus/libpegprm.so.1
%attr(-,root,root) /usr/lib/pegasus/libpegprovidermanager.so.1
%attr(-,root,root) /usr/lib/pegasus/libpegprovider.so.1
%attr(-,root,root) /usr/lib/pegasus/libpegrepository.so.1
%attr(-,root,root) /usr/lib/pegasus/libpegserver.so.1
%attr(-,root,root) /usr/lib/pegasus/libpeguser.so.1
%attr(-,root,root) /usr/lib/pegasus/libpegwql.so.1
%attr(-,root,root) /usr/lib/pegasus/libProcessProvider.so.1
%attr(-,root,root) /usr/lib/pegasus/libProviderRegistrationProvider.so.1
%attr(-,root,root) /usr/lib/pegasus/libRT_IndicationConsumer.so.1
%attr(-,root,root) /usr/lib/pegasus/libRT_IndicationProvider.so.1
%attr(-,root,root) /usr/lib/pegasus/libSampleIndicationProvider.so.1
%attr(-,root,root) /usr/lib/pegasus/libSampleInstanceProvider.so.1
%attr(-,root,root) /usr/lib/pegasus/libSampleMethodProvider.so.1
%attr(-,root,root) /usr/lib/pegasus/libsendmailIndicationHandler.so.1
%attr(-,root,root) /usr/lib/pegasus/libsnmpIndicationHandler.so.1
%attr(-,root,root) /usr/lib/pegasus/libUserAuthProvider.so.1
%attr(-,root,root) /etc/pegasus/mof/CIM_Core27.mof
%attr(-,root,root) /etc/pegasus/mof/Core27_Qualifiers.mof
%attr(-,root,root) /etc/pegasus/mof/Core27_CoreElements.mof
%attr(-,root,root) /etc/pegasus/mof/Core27_Physical.mof
%attr(-,root,root) /etc/pegasus/mof/Core27_Software.mof
%attr(-,root,root) /etc/pegasus/mof/Core27_Device.mof
%attr(-,root,root) /etc/pegasus/mof/Core27_StorageExtent.mof
%attr(-,root,root) /etc/pegasus/mof/Core27_Collection.mof
%attr(-,root,root) /etc/pegasus/mof/Core27_Redundancy.mof
%attr(-,root,root) /etc/pegasus/mof/Core27_ProductFRU.mof
%attr(-,root,root) /etc/pegasus/mof/Core27_Statistics.mof
%attr(-,root,root) /etc/pegasus/mof/Core27_Capabilities.mof
%attr(-,root,root) /etc/pegasus/mof/Core27_Settings.mof
%attr(-,root,root) /etc/pegasus/mof/Core27_MethodParms.mof
%attr(-,root,root) /etc/pegasus/mof/Core27_PowerMgmt.mof
%attr(-,root,root) /etc/pegasus/mof/Application27_BIOS.mof
%attr(-,root,root) /etc/pegasus/mof/Application27_CheckAction.mof
%attr(-,root,root) /etc/pegasus/mof/Application27_DeploymentModel.mof
%attr(-,root,root) /etc/pegasus/mof/Application27_InstalledProduct.mof
%attr(-,root,root) /etc/pegasus/mof/Application27_SystemSoftware.mof
%attr(-,root,root) /etc/pegasus/mof/CIM_Application27.mof
%attr(-,root,root) /etc/pegasus/mof/CIM_Database27.mof
%attr(-,root,root) /etc/pegasus/mof/CIM_Device27.mof
%attr(-,root,root) /etc/pegasus/mof/CIM_Event27.mof
%attr(-,root,root) /etc/pegasus/mof/CIM_Interop27.mof 
%attr(-,root,root) /etc/pegasus/mof/CIM_Metrics27.mof
%attr(-,root,root) /etc/pegasus/mof/CIM_Network27.mof
%attr(-,root,root) /etc/pegasus/mof/CIM_Physical27.mof
%attr(-,root,root) /etc/pegasus/mof/CIM_Policy27.mof
%attr(-,root,root) /etc/pegasus/mof/CIM_Schema27.mof
%attr(-,root,root) /etc/pegasus/mof/CIM_Support27.mof
%attr(-,root,root) /etc/pegasus/mof/CIM_System27.mof
%attr(-,root,root) /etc/pegasus/mof/CIM_User27.mof
%attr(-,root,root) /etc/pegasus/mof/Device27_Controller.mof
%attr(-,root,root) /etc/pegasus/mof/Device27_CoolingAndPower.mof
%attr(-,root,root) /etc/pegasus/mof/Device27_DeviceElements.mof
%attr(-,root,root) /etc/pegasus/mof/Device27_DiskGroup.mof
%attr(-,root,root) /etc/pegasus/mof/Device27_FC.mof
%attr(-,root,root) /etc/pegasus/mof/Device27_IB.mof
%attr(-,root,root) /etc/pegasus/mof/Device27_Memory.mof
%attr(-,root,root) /etc/pegasus/mof/Device27_Modems.mof
%attr(-,root,root) /etc/pegasus/mof/Device27_NetworkAdapter.mof
%attr(-,root,root) /etc/pegasus/mof/Device27_Ports.mof
%attr(-,root,root) /etc/pegasus/mof/Device27_Printing.mof
%attr(-,root,root) /etc/pegasus/mof/Device27_Processor.mof
%attr(-,root,root) /etc/pegasus/mof/Device27_SccExtents.mof
%attr(-,root,root) /etc/pegasus/mof/Device27_Sensors.mof
%attr(-,root,root) /etc/pegasus/mof/Device27_StorageDevices.mof
%attr(-,root,root) /etc/pegasus/mof/Device27_StorageExtents.mof
%attr(-,root,root) /etc/pegasus/mof/Device27_StorageLib.mof
%attr(-,root,root) /etc/pegasus/mof/Device27_StorageServices.mof
%attr(-,root,root) /etc/pegasus/mof/Device27_USB.mof
%attr(-,root,root) /etc/pegasus/mof/Device27_UserDevices.mof
%attr(-,root,root) /etc/pegasus/mof/Metrics27_BaseMetric.mof
%attr(-,root,root) /etc/pegasus/mof/Metrics27_UnitOfWork.mof
%attr(-,root,root) /etc/pegasus/mof/Network27_BGP.mof
%attr(-,root,root) /etc/pegasus/mof/Network27_Buffers.mof
%attr(-,root,root) /etc/pegasus/mof/Network27_Collections.mof
%attr(-,root,root) /etc/pegasus/mof/Network27_Filtering.mof
%attr(-,root,root) /etc/pegasus/mof/Network27_OSPF.mof
%attr(-,root,root) /etc/pegasus/mof/Network27_Pipes.mof
%attr(-,root,root) /etc/pegasus/mof/Network27_ProtocolEndpoints.mof
%attr(-,root,root) /etc/pegasus/mof/Network27_QoS.mof
%attr(-,root,root) /etc/pegasus/mof/Network27_Routes.mof
%attr(-,root,root) /etc/pegasus/mof/Network27_RoutingForwarding.mof
%attr(-,root,root) /etc/pegasus/mof/Network27_SNMP.mof
%attr(-,root,root) /etc/pegasus/mof/Network27_SwitchingBridging.mof
%attr(-,root,root) /etc/pegasus/mof/Network27_Systems.mof
%attr(-,root,root) /etc/pegasus/mof/Network27_VLAN.mof
%attr(-,root,root) /etc/pegasus/mof/Physical27_Component.mof
%attr(-,root,root) /etc/pegasus/mof/Physical27_Connector.mof
%attr(-,root,root) /etc/pegasus/mof/Physical27_Link.mof
%attr(-,root,root) /etc/pegasus/mof/Physical27_Misc.mof
%attr(-,root,root) /etc/pegasus/mof/Physical27_Package.mof
%attr(-,root,root) /etc/pegasus/mof/Physical27_PhysicalMedia.mof
%attr(-,root,root) /etc/pegasus/mof/Physical27_StoragePackage.mof
%attr(-,root,root) /etc/pegasus/mof/System27_Boot.mof
%attr(-,root,root) /etc/pegasus/mof/System27_Diagnostics.mof
%attr(-,root,root) /etc/pegasus/mof/System27_FileElements.mof
%attr(-,root,root) /etc/pegasus/mof/System27_Logs.mof
%attr(-,root,root) /etc/pegasus/mof/System27_OperatingSystem.mof
%attr(-,root,root) /etc/pegasus/mof/System27_Processing.mof
%attr(-,root,root) /etc/pegasus/mof/System27_SystemElements.mof
%attr(-,root,root) /etc/pegasus/mof/System27_SystemResources.mof
%attr(-,root,root) /etc/pegasus/mof/System27_Time.mof
%attr(-,root,root) /etc/pegasus/mof/System27_Unix.mof
%attr(-,root,root) /etc/pegasus/mof/User27_AccessControl.mof
%attr(-,root,root) /etc/pegasus/mof/User27_Account.mof
%attr(-,root,root) /etc/pegasus/mof/User27_AuthenticationReqmt.mof
%attr(-,root,root) /etc/pegasus/mof/User27_Credential.mof
%attr(-,root,root) /etc/pegasus/mof/User27_Group.mof
%attr(-,root,root) /etc/pegasus/mof/User27_Kerberos.mof
%attr(-,root,root) /etc/pegasus/mof/User27_Org.mof
%attr(-,root,root) /etc/pegasus/mof/User27_PublicKey.mof
%attr(-,root,root) /etc/pegasus/mof/User27_Role.mof
%attr(-,root,root) /etc/pegasus/mof/User27_SecurityServices.mof
%attr(-,root,root) /etc/pegasus/mof/User27_SharedSecret.mof
%attr(-,root,root) /etc/pegasus/mof/User27_UsersAccess.mof
%attr(-,root,root) /etc/pegasus/mof/PG_Events20.mof
%attr(-,root,root) /etc/pegasus/mof/InterOpSchema.mof
%attr(-,root,root) /etc/pegasus/mof/PG_ProviderModule20.mof
%attr(-,root,root) /etc/pegasus/mof/PG_Authorization20.mof
%attr(-,root,root) /etc/pegasus/mof/PG_ConfigSetting20.mof
%attr(-,root,root) /etc/pegasus/mof/InternalSchema.mof
%attr(-,root,root) /etc/pegasus/mof/PG_User20.mof
%attr(-,root,root) /etc/pegasus/mof/PG_ShutdownService20.mof
%attr(-,root,root) /etc/pegasus/mof/PG_ComputerSystem20.mof
%attr(-,root,root) /etc/pegasus/mof/PG_ComputerSystem20R.mof
%attr(-,root,root) /etc/pegasus/mof/PG_OperatingSystem20.mof
%attr(-,root,root) /etc/pegasus/mof/PG_OperatingSystem20R.mof
%attr(-,root,root) /etc/pegasus/mof/ManagedSystemSchema.mof
%attr(-,root,root) /etc/pegasus/mof/ManagedSystemSchemaR.mof
%attr(-,root,root) /etc/pegasus/mof/PG_UnixProcess20.mof
%attr(-,root,root) /etc/pegasus/mof/PG_UnixProcess20R.mof
%attr(-,root,root) /usr/share/man/man1/cimmof.1.gz
%attr(-,root,root) /usr/share/man/man1/cimprovider.1.gz
%attr(-,root,root) /usr/share/man/man1/osinfo.1.gz
%attr(-,root,root) /usr/share/man/man1/wbemexec.1.gz
%attr(-,root,root) /usr/share/man/man1m/cimauth.1m.gz
%attr(-,root,root) /usr/share/man/man1m/cimconfig.1m.gz
%attr(-,root,root) /usr/share/man/man1m/cimprovider.1m.gz
%attr(-,root,root) /usr/share/man/man1m/cimserver.1m.gz
