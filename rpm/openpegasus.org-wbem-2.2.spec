#%/////////////////////////////////////////////////////////////////////////////
#
# Copyright (c) 2001,2002,2003 BMC Software, Hewlett-Packard Company, IBM,
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
# Konrad Rzeszutek <konradr@us.ibm.com>
#
# -------------------------------------------------------------------------------
# Changes:
# 09-25-2003 by Martin Knipper <knipper@de.ibm.com>
# - added "export SYS_INCLUDES=-I/usr/kerberos/include" for correct RedHat9 compile
# - removed the install -o 0 -g 0 flags from the specfile 
#	-> this way a non root user can build the RPM's since a normal user (UID>0) can not
#		change a files owner or group
# - changed the files %attr section with correct filepermissons that were taken from the
#	install flags 
# - RedHat 7.x has some trouble starting pegasus in the %post-step
#	- added a workaround recieved by Konrad Rzeszutek <konradr@us.ibm.com>
# - added a rm -Rf $PEGASUS_HOME at the end of the install section
#	-> otherwise the rpm build process will break on RedHat 9
#
#%/////////////////////////////////////////////////////////////////////////////
#
# openpegasus.org-wbem-2.2.spec
#
# Package spec for PEGASUS 2.2
#

Summary: WBEM Services for Linux
Name: pegasus-wbem
Version: 2.3
Release: 2
Group: Systems Management/Base
Copyright: Open Group Pegasus Open Source
BuildRoot: %{_tmppath}/%{name}-%{version}-%{release}
Source: ftp://www.opengroup.org/pegasus/pegasus-wbem-%{version}-%{release}.tar.gz
Requires: openssl-devel >= 0.9.6
Provides: cimserver pegasus-wbem-2.3

%description
WBEM Services for Red Hat Linux enables management solutions that deliver
increased control of enterprise resources. WBEM is a platform and resource
independent DMTF standard that defines a common information model and
communication protocol for monitoring and controlling resources from diverse
sources.

%package devel
Summary:      The Pegasus source tree
Group:        Systems Management/Base
Autoreq: 0
Requires: pegasus-wbem >= 2.3 openssl-devel >= 0.9.6

%description devel
This package contains the Pegasus source tree, header files and
static libraries (if any).


%prep
[ "$RPM_BUILD_ROOT" != "/" ] && [ -d $RPM_BUILD_ROOT ] && rm -rf $RPM_BUILD_ROOT;

%setup
# Copy the necessary include files
export PEGASUS_ENABLE_SLP=1
export PEGASUS_ROOT=$RPM_BUILD_DIR/$RPM_PACKAGE_NAME-$RPM_PACKAGE_VERSION
export LISTLOC=$RPM_BUILD_DIR/$RPM_PACKAGE_NAME-$RPM_PACKAGE_VERSION
rm -f $LISTLOC/rpm_pegasus_include_files
pushd  $PEGASUS_ROOT/src
list=`find Pegasus -iname '*.h'`
popd
for i in $list;do
    j=`dirname $i`;
    echo "/usr/include/"$i >> $LISTLOC/rpm_pegasus_include_files
    mkdir -p $RPM_BUILD_ROOT/usr/include/$j
    cp $PEGASUS_ROOT/src/$i $RPM_BUILD_ROOT/usr/include/$i
done;

# Needed for CMPI patch
ln -s $RPM_BUILD_DIR/$RPM_PACKAGE_NAME-$RPM_PACKAGE_VERSION $RPM_BUILD_DIR/$RPM_PACKAGE_NAME


%build
export PEGASUS_ENABLE_SLP=1
export PEGASUS_ROOT=$RPM_BUILD_DIR/$RPM_PACKAGE_NAME-$RPM_PACKAGE_VERSION
export PEGASUS_HOME=$RPM_BUILD_ROOT/usr/pegasus
%ifarch ia64
export PEGASUS_PLATFORM=LINUX_IA64_GNU
%else
export PEGASUS_PLATFORM=LINUX_IX86_GNU
%endif
# Modify this when a new version of OpenSSL appears.
# export OPENSSL_HOME=$RPM_BUILD_DIR/openssl/
export PEGASUS_HAS_SSL=yes

# per bug #368
export PEGASUS_USE_RELEASE_DIRS=true
export SYS_INCLUDES=-I/usr/kerberos/include
make

%install
#
# Make directories
mkdir -p $RPM_BUILD_ROOT/var/log/pegasus
mkdir -p $RPM_BUILD_ROOT/var/cache/pegasus/localauth
mkdir -p $RPM_BUILD_ROOT/usr/lib/pegasus/providers
mkdir -p $RPM_BUILD_ROOT/usr/share/man/{man1,man1m}
mkdir -p $RPM_BUILD_ROOT/etc/pegasus/mof


export PEGASUS_ROOT=$RPM_BUILD_DIR/$RPM_PACKAGE_NAME-$RPM_PACKAGE_VERSION
export PEGASUS_HOME=$RPM_BUILD_ROOT/usr/pegasus

#
# Init scripts
install -D $PEGASUS_ROOT/rpm/wbem22.lnx $RPM_BUILD_ROOT/etc/init.d/pegasus-wbem

#
# Programs
install -D $PEGASUS_HOME/bin/cimauth $RPM_BUILD_ROOT/usr/sbin/cimauth
install -D $PEGASUS_HOME/bin/cimconfig $RPM_BUILD_ROOT/usr/sbin/cimconfig
install -D $PEGASUS_HOME/bin/cimmofl $RPM_BUILD_ROOT/usr/bin/cimmofl
install -D $PEGASUS_HOME/bin/cimmof $RPM_BUILD_ROOT/usr/bin/cimmof
install -D $PEGASUS_HOME/bin/cimprovider $RPM_BUILD_ROOT/usr/bin/cimprovider
install -D $PEGASUS_HOME/bin/cimserver $RPM_BUILD_ROOT/usr/sbin/cimserver
install -D $PEGASUS_HOME/bin/cimuser $RPM_BUILD_ROOT/usr/sbin/cimuser
install -D $PEGASUS_HOME/bin/CLI $RPM_BUILD_ROOT/usr/bin/CLI
install -D $PEGASUS_HOME/bin/ipinfo $RPM_BUILD_ROOT/usr/bin/ipinfo
install -D $PEGASUS_HOME/bin/osinfo $RPM_BUILD_ROOT/usr/bin/osinfo
install -D $PEGASUS_HOME/bin/tomof $RPM_BUILD_ROOT/usr/bin/tomof
install -D $PEGASUS_HOME/bin/wbemexec $RPM_BUILD_ROOT/usr/bin/wbemexec

# Libraries

install -D $PEGASUS_HOME/lib/libAlertIndicationProvider.so.1 $RPM_BUILD_ROOT/usr/lib/pegasus/libAlertIndicationProvider.so.1
install -D $PEGASUS_HOME/lib/libCIMxmlIndicationHandler.so.1 $RPM_BUILD_ROOT/usr/lib/pegasus/libCIMxmlIndicationHandler.so.1
install -D $PEGASUS_HOME/lib/libComputerSystemProvider.so.1 $RPM_BUILD_ROOT/usr/lib/pegasus/libComputerSystemProvider.so.1
install -D $PEGASUS_HOME/lib/libConfigSettingProvider.so.1 $RPM_BUILD_ROOT/usr/lib/pegasus/libConfigSettingProvider.so.1
install -D $PEGASUS_HOME/lib/libDisplayConsumer.so.1 $RPM_BUILD_ROOT/usr/lib/pegasus/libDisplayConsumer.so.1
install -D $PEGASUS_HOME/lib/libDynLib.so.1 $RPM_BUILD_ROOT/usr/lib/pegasus/libDynLib.so.1
install -D $PEGASUS_HOME/lib/libIBM_CIMOMStatDataProvider.so.1 $RPM_BUILD_ROOT/usr/lib/pegasus/libIBM_CIMOMStatDataProvider.so.1
install -D $PEGASUS_HOME/lib/libIPProviderModule.so.1 $RPM_BUILD_ROOT/usr/lib/pegasus/libIPProviderModule.so.1
install -D $PEGASUS_HOME/lib/libNamespaceProvider.so.1 $RPM_BUILD_ROOT/usr/lib/pegasus/libNamespaceProvider.so.1
install -D $PEGASUS_HOME/lib/libnsatrap.so.1 $RPM_BUILD_ROOT/usr/lib/pegasus/libnsatrap.so.1
install -D $PEGASUS_HOME/lib/libOperatingSystemProvider.so.1 $RPM_BUILD_ROOT/usr/lib/pegasus/libOperatingSystemProvider.so.1
install -D $PEGASUS_HOME/lib/libOSProvider.so.1 $RPM_BUILD_ROOT/usr/lib/pegasus/libOSProvider.so.1
install -D $PEGASUS_HOME/lib/libpegauthentication.so.1 $RPM_BUILD_ROOT/usr/lib/pegasus/libpegauthentication.so.1
install -D $PEGASUS_HOME/lib/libpegCLIClientLib.so.1 $RPM_BUILD_ROOT/usr/lib/pegasus/libpegCLIClientLib.so.1
install -D $PEGASUS_HOME/lib/libpegclient.so.1 $RPM_BUILD_ROOT/usr/lib/pegasus/libpegclient.so.1
install -D $PEGASUS_HOME/lib/libpegcliutils.so.1 $RPM_BUILD_ROOT/usr/lib/pegasus/libpegcliutils.so.1
install -D $PEGASUS_HOME/lib/libpegcommon.so.1 $RPM_BUILD_ROOT/usr/lib/pegasus/libpegcommon.so.1
install -D $PEGASUS_HOME/lib/libpegcompiler.so.1 $RPM_BUILD_ROOT/usr/lib/pegasus/libpegcompiler.so.1
install -D $PEGASUS_HOME/lib/libpegconfig.so.1 $RPM_BUILD_ROOT/usr/lib/pegasus/libpegconfig.so.1
install -D $PEGASUS_HOME/lib/libpegexportclient.so.1 $RPM_BUILD_ROOT/usr/lib/pegasus/libpegexportclient.so.1
install -D $PEGASUS_HOME/lib/libpegexportserver.so.1 $RPM_BUILD_ROOT/usr/lib/pegasus/libpegexportserver.so.1
install -D $PEGASUS_HOME/lib/libpeggetoopt.so.1 $RPM_BUILD_ROOT/usr/lib/pegasus/libpeggetoopt.so.1
install -D $PEGASUS_HOME/lib/libpeghandlerservice.so.1 $RPM_BUILD_ROOT/usr/lib/pegasus/libpeghandlerservice.so.1
install -D $PEGASUS_HOME/lib/libpegindicationservice.so.1 $RPM_BUILD_ROOT/usr/lib/pegasus/libpegindicationservice.so.1
install -D $PEGASUS_HOME/lib/libpeglistener.so.1 $RPM_BUILD_ROOT/usr/lib/pegasus/libpeglistener.so.1
install -D $PEGASUS_HOME/lib/libpegprm.so.1 $RPM_BUILD_ROOT/usr/lib/pegasus/libpegprm.so.1
install -D $PEGASUS_HOME/lib/libpegprovidermanager.so.1 $RPM_BUILD_ROOT/usr/lib/pegasus/libpegprovidermanager.so.1
install -D $PEGASUS_HOME/lib/libpegprovider.so.1 $RPM_BUILD_ROOT/usr/lib/pegasus/libpegprovider.so.1
install -D $PEGASUS_HOME/lib/libpegrepository.so.1 $RPM_BUILD_ROOT/usr/lib/pegasus/libpegrepository.so.1
install -D $PEGASUS_HOME/lib/libpegserver.so.1 $RPM_BUILD_ROOT/usr/lib/pegasus/libpegserver.so.1
install -D $PEGASUS_HOME/lib/libpeguser.so.1 $RPM_BUILD_ROOT/usr/lib/pegasus/libpeguser.so.1
install -D $PEGASUS_HOME/lib/libpegwql.so.1 $RPM_BUILD_ROOT/usr/lib/pegasus/libpegwql.so.1
install -D $PEGASUS_HOME/lib/libPG_TestPropertyTypes.so.1 $RPM_BUILD_ROOT/usr/lib/pegasus/libPG_TestPropertyTypes.so.1
install -D $PEGASUS_HOME/lib/libProcessIndicationProvider.so.1 $RPM_BUILD_ROOT/usr/lib/pegasus/libProcessIndicationProvider.so.1
install -D $PEGASUS_HOME/lib/libProcessorProviderModule.so.1 $RPM_BUILD_ROOT/usr/lib/pegasus/libProcessorProviderModule.so.1
install -D $PEGASUS_HOME/lib/libProcessProvider.so.1 $RPM_BUILD_ROOT/usr/lib/pegasus/libProcessProvider.so.1
install -D $PEGASUS_HOME/lib/libProviderRegistrationProvider.so.1 $RPM_BUILD_ROOT/usr/lib/pegasus/libProviderRegistrationProvider.so.1
install -D $PEGASUS_HOME/lib/libRT_IndicationConsumer.so.1 $RPM_BUILD_ROOT/usr/lib/pegasus/libRT_IndicationConsumer.so.1
install -D $PEGASUS_HOME/lib/libRT_IndicationProvider.so.1 $RPM_BUILD_ROOT/usr/lib/pegasus/libRT_IndicationProvider.so.1
install -D $PEGASUS_HOME/lib/libSampleFamilyProvider.so.1 $RPM_BUILD_ROOT/usr/lib/pegasus/libSampleFamilyProvider.so.1
install -D $PEGASUS_HOME/lib/libSampleIndicationProvider.so.1 $RPM_BUILD_ROOT/usr/lib/pegasus/libSampleIndicationProvider.so.1
install -D $PEGASUS_HOME/lib/libSampleInstanceProvider.so.1 $RPM_BUILD_ROOT/usr/lib/pegasus/libSampleInstanceProvider.so.1
install -D $PEGASUS_HOME/lib/libSampleMethodProvider.so.1 $RPM_BUILD_ROOT/usr/lib/pegasus/libSampleMethodProvider.so.1
install -D $PEGASUS_HOME/lib/libsendmailIndicationHandler.so.1 $RPM_BUILD_ROOT/usr/lib/pegasus/libsendmailIndicationHandler.so.1
install -D $PEGASUS_HOME/lib/libSimpleDisplayConsumer.so.1 $RPM_BUILD_ROOT/usr/lib/pegasus/libSimpleDisplayConsumer.so.1
install -D $PEGASUS_HOME/lib/libsnmpIndicationHandler.so.1 $RPM_BUILD_ROOT/usr/lib/pegasus/libsnmpIndicationHandler.so.1
install -D $PEGASUS_HOME/lib/libUserAuthProvider.so.1 $RPM_BUILD_ROOT/usr/lib/pegasus/libUserAuthProvider.so.1
%ifnarch ia64
install -D $PEGASUS_HOME/lib/libslp.so.1 $RPM_BUILD_ROOT/usr/lib/pegasus/libslp.so.1
%endif



# CIM schema
#

install -D $PEGASUS_ROOT/Schemas/CIM27/Application27_BIOS.mof $RPM_BUILD_ROOT/etc/pegasus/mof/Application27_BIOS.mof
install -D $PEGASUS_ROOT/Schemas/CIM27/Application27_CheckAction.mof $RPM_BUILD_ROOT/etc/pegasus/mof/Application27_CheckAction.mof
install -D $PEGASUS_ROOT/Schemas/CIM27/Application27_DeploymentModel.mof $RPM_BUILD_ROOT/etc/pegasus/mof/Application27_DeploymentModel.mof
install -D $PEGASUS_ROOT/Schemas/CIM27/Application27_InstalledProduct.mof $RPM_BUILD_ROOT/etc/pegasus/mof/Application27_InstalledProduct.mof
install -D $PEGASUS_ROOT/Schemas/CIM27/Application27_SystemSoftware.mof $RPM_BUILD_ROOT/etc/pegasus/mof/Application27_SystemSoftware.mof
install -D $PEGASUS_ROOT/Schemas/CIM27/CIM_Application27.mof $RPM_BUILD_ROOT/etc/pegasus/mof/CIM_Application27.mof
install -D $PEGASUS_ROOT/Schemas/CIM27/CIM_Core27.mof $RPM_BUILD_ROOT/etc/pegasus/mof/CIM_Core27.mof
install -D $PEGASUS_ROOT/Schemas/CIM27/CIM_Database27.mof $RPM_BUILD_ROOT/etc/pegasus/mof/CIM_Database27.mof
install -D $PEGASUS_ROOT/Schemas/CIM27/CIM_Device27.mof $RPM_BUILD_ROOT/etc/pegasus/mof/CIM_Device27.mof
install -D $PEGASUS_ROOT/Schemas/CIM27/CIM_Event27.mof $RPM_BUILD_ROOT/etc/pegasus/mof/CIM_Event27.mof
install -D $PEGASUS_ROOT/Schemas/CIM27/CIM_Interop27.mof $RPM_BUILD_ROOT/etc/pegasus/mof/CIM_Interop27.mof
install -D $PEGASUS_ROOT/Schemas/CIM27/CIM_Metrics27.mof $RPM_BUILD_ROOT/etc/pegasus/mof/CIM_Metrics27.mof
install -D $PEGASUS_ROOT/Schemas/CIM27/CIM_Network27.mof $RPM_BUILD_ROOT/etc/pegasus/mof/CIM_Network27.mof
install -D $PEGASUS_ROOT/Schemas/CIM27/CIM_Physical27.mof $RPM_BUILD_ROOT/etc/pegasus/mof/CIM_Physical27.mof
install -D $PEGASUS_ROOT/Schemas/CIM27/CIM_Policy27.mof $RPM_BUILD_ROOT/etc/pegasus/mof/CIM_Policy27.mof
install -D $PEGASUS_ROOT/Schemas/CIM27/CIM_Schema27.mof $RPM_BUILD_ROOT/etc/pegasus/mof/CIM_Schema27.mof
install -D $PEGASUS_ROOT/Schemas/CIM27/CIM_Support27.mof $RPM_BUILD_ROOT/etc/pegasus/mof/CIM_Support27.mof
install -D $PEGASUS_ROOT/Schemas/CIM27/CIM_System27.mof $RPM_BUILD_ROOT/etc/pegasus/mof/CIM_System27.mof
install -D $PEGASUS_ROOT/Schemas/CIM27/CIM_User27.mof $RPM_BUILD_ROOT/etc/pegasus/mof/CIM_User27.mof
install -D $PEGASUS_ROOT/Schemas/CIM27/Core27_Capabilities.mof $RPM_BUILD_ROOT/etc/pegasus/mof/Core27_Capabilities.mof
install -D $PEGASUS_ROOT/Schemas/CIM27/Core27_Collection.mof $RPM_BUILD_ROOT/etc/pegasus/mof/Core27_Collection.mof
install -D $PEGASUS_ROOT/Schemas/CIM27/Core27_CoreElements.mof $RPM_BUILD_ROOT/etc/pegasus/mof/Core27_CoreElements.mof
install -D $PEGASUS_ROOT/Schemas/CIM27/Core27_Device.mof $RPM_BUILD_ROOT/etc/pegasus/mof/Core27_Device.mof
install -D $PEGASUS_ROOT/Schemas/CIM27/Core27_MethodParms.mof $RPM_BUILD_ROOT/etc/pegasus/mof/Core27_MethodParms.mof
install -D $PEGASUS_ROOT/Schemas/CIM27/Core27_Physical.mof $RPM_BUILD_ROOT/etc/pegasus/mof/Core27_Physical.mof
install -D $PEGASUS_ROOT/Schemas/CIM27/Core27_PowerMgmt.mof $RPM_BUILD_ROOT/etc/pegasus/mof/Core27_PowerMgmt.mof
install -D $PEGASUS_ROOT/Schemas/CIM27/Core27_ProductFRU.mof $RPM_BUILD_ROOT/etc/pegasus/mof/Core27_ProductFRU.mof
install -D $PEGASUS_ROOT/Schemas/CIM27/Core27_Qualifiers.mof $RPM_BUILD_ROOT/etc/pegasus/mof/Core27_Qualifiers.mof
install -D $PEGASUS_ROOT/Schemas/CIM27/Core27_Redundancy.mof $RPM_BUILD_ROOT/etc/pegasus/mof/Core27_Redundancy.mof
install -D $PEGASUS_ROOT/Schemas/CIM27/Core27_Settings.mof $RPM_BUILD_ROOT/etc/pegasus/mof/Core27_Settings.mof
install -D $PEGASUS_ROOT/Schemas/CIM27/Core27_Software.mof $RPM_BUILD_ROOT/etc/pegasus/mof/Core27_Software.mof
install -D $PEGASUS_ROOT/Schemas/CIM27/Core27_Statistics.mof $RPM_BUILD_ROOT/etc/pegasus/mof/Core27_Statistics.mof
install -D $PEGASUS_ROOT/Schemas/CIM27/Core27_StorageExtent.mof $RPM_BUILD_ROOT/etc/pegasus/mof/Core27_StorageExtent.mof
install -D $PEGASUS_ROOT/Schemas/CIM27/Device27_Controller.mof $RPM_BUILD_ROOT/etc/pegasus/mof/Device27_Controller.mof
install -D $PEGASUS_ROOT/Schemas/CIM27/Device27_CoolingAndPower.mof $RPM_BUILD_ROOT/etc/pegasus/mof/Device27_CoolingAndPower.mof
install -D $PEGASUS_ROOT/Schemas/CIM27/Device27_DeviceElements.mof $RPM_BUILD_ROOT/etc/pegasus/mof/Device27_DeviceElements.mof
install -D $PEGASUS_ROOT/Schemas/CIM27/Device27_DiskGroup.mof $RPM_BUILD_ROOT/etc/pegasus/mof/Device27_DiskGroup.mof
install -D $PEGASUS_ROOT/Schemas/CIM27/Device27_FC.mof $RPM_BUILD_ROOT/etc/pegasus/mof/Device27_FC.mof
install -D $PEGASUS_ROOT/Schemas/CIM27/Device27_IB.mof $RPM_BUILD_ROOT/etc/pegasus/mof/Device27_IB.mof
install -D $PEGASUS_ROOT/Schemas/CIM27/Device27_Memory.mof $RPM_BUILD_ROOT/etc/pegasus/mof/Device27_Memory.mof
install -D $PEGASUS_ROOT/Schemas/CIM27/Device27_Modems.mof $RPM_BUILD_ROOT/etc/pegasus/mof/Device27_Modems.mof
install -D $PEGASUS_ROOT/Schemas/CIM27/Device27_NetworkAdapter.mof $RPM_BUILD_ROOT/etc/pegasus/mof/Device27_NetworkAdapter.mof
install -D $PEGASUS_ROOT/Schemas/CIM27/Device27_Ports.mof $RPM_BUILD_ROOT/etc/pegasus/mof/Device27_Ports.mof
install -D $PEGASUS_ROOT/Schemas/CIM27/Device27_Printing.mof $RPM_BUILD_ROOT/etc/pegasus/mof/Device27_Printing.mof
install -D $PEGASUS_ROOT/Schemas/CIM27/Device27_Processor.mof $RPM_BUILD_ROOT/etc/pegasus/mof/Device27_Processor.mof
install -D $PEGASUS_ROOT/Schemas/CIM27/Device27_SccExtents.mof $RPM_BUILD_ROOT/etc/pegasus/mof/Device27_SccExtents.mof
install -D $PEGASUS_ROOT/Schemas/CIM27/Device27_Sensors.mof $RPM_BUILD_ROOT/etc/pegasus/mof/Device27_Sensors.mof
install -D $PEGASUS_ROOT/Schemas/CIM27/Device27_StorageDevices.mof $RPM_BUILD_ROOT/etc/pegasus/mof/Device27_StorageDevices.mof
install -D $PEGASUS_ROOT/Schemas/CIM27/Device27_StorageExtents.mof $RPM_BUILD_ROOT/etc/pegasus/mof/Device27_StorageExtents.mof
install -D $PEGASUS_ROOT/Schemas/CIM27/Device27_StorageLib.mof $RPM_BUILD_ROOT/etc/pegasus/mof/Device27_StorageLib.mof
install -D $PEGASUS_ROOT/Schemas/CIM27/Device27_StorageServices.mof $RPM_BUILD_ROOT/etc/pegasus/mof/Device27_StorageServices.mof
install -D $PEGASUS_ROOT/Schemas/CIM27/Device27_USB.mof $RPM_BUILD_ROOT/etc/pegasus/mof/Device27_USB.mof
install -D $PEGASUS_ROOT/Schemas/CIM27/Device27_UserDevices.mof $RPM_BUILD_ROOT/etc/pegasus/mof/Device27_UserDevices.mof
install -D $PEGASUS_ROOT/Schemas/CIM27/Metrics27_BaseMetric.mof $RPM_BUILD_ROOT/etc/pegasus/mof/Metrics27_BaseMetric.mof
install -D $PEGASUS_ROOT/Schemas/CIM27/Metrics27_UnitOfWork.mof $RPM_BUILD_ROOT/etc/pegasus/mof/Metrics27_UnitOfWork.mof
install -D $PEGASUS_ROOT/Schemas/CIM27/Network27_BGP.mof $RPM_BUILD_ROOT/etc/pegasus/mof/Network27_BGP.mof
install -D $PEGASUS_ROOT/Schemas/CIM27/Network27_Buffers.mof $RPM_BUILD_ROOT/etc/pegasus/mof/Network27_Buffers.mof
install -D $PEGASUS_ROOT/Schemas/CIM27/Network27_Collections.mof $RPM_BUILD_ROOT/etc/pegasus/mof/Network27_Collections.mof
install -D $PEGASUS_ROOT/Schemas/CIM27/Network27_Filtering.mof $RPM_BUILD_ROOT/etc/pegasus/mof/Network27_Filtering.mof
install -D $PEGASUS_ROOT/Schemas/CIM27/Network27_OSPF.mof $RPM_BUILD_ROOT/etc/pegasus/mof/Network27_OSPF.mof
install -D $PEGASUS_ROOT/Schemas/CIM27/Network27_Pipes.mof $RPM_BUILD_ROOT/etc/pegasus/mof/Network27_Pipes.mof
install -D $PEGASUS_ROOT/Schemas/CIM27/Network27_ProtocolEndpoints.mof $RPM_BUILD_ROOT/etc/pegasus/mof/Network27_ProtocolEndpoints.mof
install -D $PEGASUS_ROOT/Schemas/CIM27/Network27_QoS.mof $RPM_BUILD_ROOT/etc/pegasus/mof/Network27_QoS.mof
install -D $PEGASUS_ROOT/Schemas/CIM27/Network27_Routes.mof $RPM_BUILD_ROOT/etc/pegasus/mof/Network27_Routes.mof
install -D $PEGASUS_ROOT/Schemas/CIM27/Network27_RoutingForwarding.mof $RPM_BUILD_ROOT/etc/pegasus/mof/Network27_RoutingForwarding.mof
install -D $PEGASUS_ROOT/Schemas/CIM27/Network27_SNMP.mof $RPM_BUILD_ROOT/etc/pegasus/mof/Network27_SNMP.mof
install -D $PEGASUS_ROOT/Schemas/CIM27/Network27_SwitchingBridging.mof $RPM_BUILD_ROOT/etc/pegasus/mof/Network27_SwitchingBridging.mof
install -D $PEGASUS_ROOT/Schemas/CIM27/Network27_Systems.mof $RPM_BUILD_ROOT/etc/pegasus/mof/Network27_Systems.mof
install -D $PEGASUS_ROOT/Schemas/CIM27/Network27_VLAN.mof $RPM_BUILD_ROOT/etc/pegasus/mof/Network27_VLAN.mof
install -D $PEGASUS_ROOT/Schemas/CIM27/Physical27_Component.mof $RPM_BUILD_ROOT/etc/pegasus/mof/Physical27_Component.mof
install -D $PEGASUS_ROOT/Schemas/CIM27/Physical27_Connector.mof $RPM_BUILD_ROOT/etc/pegasus/mof/Physical27_Connector.mof
install -D $PEGASUS_ROOT/Schemas/CIM27/Physical27_Link.mof $RPM_BUILD_ROOT/etc/pegasus/mof/Physical27_Link.mof
install -D $PEGASUS_ROOT/Schemas/CIM27/Physical27_Misc.mof $RPM_BUILD_ROOT/etc/pegasus/mof/Physical27_Misc.mof
install -D $PEGASUS_ROOT/Schemas/CIM27/Physical27_Package.mof $RPM_BUILD_ROOT/etc/pegasus/mof/Physical27_Package.mof
install -D $PEGASUS_ROOT/Schemas/CIM27/Physical27_PhysicalMedia.mof $RPM_BUILD_ROOT/etc/pegasus/mof/Physical27_PhysicalMedia.mof
install -D $PEGASUS_ROOT/Schemas/CIM27/Physical27_StoragePackage.mof $RPM_BUILD_ROOT/etc/pegasus/mof/Physical27_StoragePackage.mof
install -D $PEGASUS_ROOT/Schemas/CIM27/System27_Boot.mof $RPM_BUILD_ROOT/etc/pegasus/mof/System27_Boot.mof
install -D $PEGASUS_ROOT/Schemas/CIM27/System27_Diagnostics.mof $RPM_BUILD_ROOT/etc/pegasus/mof/System27_Diagnostics.mof
install -D $PEGASUS_ROOT/Schemas/CIM27/System27_FileElements.mof $RPM_BUILD_ROOT/etc/pegasus/mof/System27_FileElements.mof
install -D $PEGASUS_ROOT/Schemas/CIM27/System27_Logs.mof $RPM_BUILD_ROOT/etc/pegasus/mof/System27_Logs.mof
install -D $PEGASUS_ROOT/Schemas/CIM27/System27_OperatingSystem.mof $RPM_BUILD_ROOT/etc/pegasus/mof/System27_OperatingSystem.mof
install -D $PEGASUS_ROOT/Schemas/CIM27/System27_Processing.mof $RPM_BUILD_ROOT/etc/pegasus/mof/System27_Processing.mof
install -D $PEGASUS_ROOT/Schemas/CIM27/System27_SystemElements.mof $RPM_BUILD_ROOT/etc/pegasus/mof/System27_SystemElements.mof
install -D $PEGASUS_ROOT/Schemas/CIM27/System27_SystemResources.mof $RPM_BUILD_ROOT/etc/pegasus/mof/System27_SystemResources.mof
install -D $PEGASUS_ROOT/Schemas/CIM27/System27_Time.mof $RPM_BUILD_ROOT/etc/pegasus/mof/System27_Time.mof
install -D $PEGASUS_ROOT/Schemas/CIM27/System27_Unix.mof $RPM_BUILD_ROOT/etc/pegasus/mof/System27_Unix.mof
install -D $PEGASUS_ROOT/Schemas/CIM27/User27_AccessControl.mof $RPM_BUILD_ROOT/etc/pegasus/mof/User27_AccessControl.mof
install -D $PEGASUS_ROOT/Schemas/CIM27/User27_Account.mof $RPM_BUILD_ROOT/etc/pegasus/mof/User27_Account.mof
install -D $PEGASUS_ROOT/Schemas/CIM27/User27_AuthenticationReqmt.mof $RPM_BUILD_ROOT/etc/pegasus/mof/User27_AuthenticationReqmt.mof
install -D $PEGASUS_ROOT/Schemas/CIM27/User27_Credential.mof $RPM_BUILD_ROOT/etc/pegasus/mof/User27_Credential.mof
install -D $PEGASUS_ROOT/Schemas/CIM27/User27_Group.mof $RPM_BUILD_ROOT/etc/pegasus/mof/User27_Group.mof
install -D $PEGASUS_ROOT/Schemas/CIM27/User27_Kerberos.mof $RPM_BUILD_ROOT/etc/pegasus/mof/User27_Kerberos.mof
install -D $PEGASUS_ROOT/Schemas/CIM27/User27_Org.mof $RPM_BUILD_ROOT/etc/pegasus/mof/User27_Org.mof
install -D $PEGASUS_ROOT/Schemas/CIM27/User27_PublicKey.mof $RPM_BUILD_ROOT/etc/pegasus/mof/User27_PublicKey.mof
install -D $PEGASUS_ROOT/Schemas/CIM27/User27_Role.mof $RPM_BUILD_ROOT/etc/pegasus/mof/User27_Role.mof
install -D $PEGASUS_ROOT/Schemas/CIM27/User27_SecurityServices.mof $RPM_BUILD_ROOT/etc/pegasus/mof/User27_SecurityServices.mof
install -D $PEGASUS_ROOT/Schemas/CIM27/User27_SharedSecret.mof $RPM_BUILD_ROOT/etc/pegasus/mof/User27_SharedSecret.mof
install -D $PEGASUS_ROOT/Schemas/CIM27/User27_UsersAccess.mof $RPM_BUILD_ROOT/etc/pegasus/mof/User27_UsersAccess.mof


#
# Pegasus' schema
#
# InterOp:
install -D $PEGASUS_ROOT/Schemas/Pegasus/InterOp/VER20/PG_Events20.mof $RPM_BUILD_ROOT/etc/pegasus/mof/PG_Events20.mof
install -D $PEGASUS_ROOT/Schemas/Pegasus/InterOp/VER20/PG_InterOpSchema20.mof $RPM_BUILD_ROOT/etc/pegasus/mof/InterOpSchema.mof
install -D $PEGASUS_ROOT/Schemas/Pegasus/InterOp/VER20/PG_ProviderModule20.mof $RPM_BUILD_ROOT/etc/pegasus/mof/PG_ProviderModule20.mof

# Internal:
install -D $PEGASUS_ROOT/Schemas/Pegasus/Internal/VER20/PG_Authorization20.mof $RPM_BUILD_ROOT/etc/pegasus/mof/PG_Authorization20.mof
install -D $PEGASUS_ROOT/Schemas/Pegasus/Internal/VER20/PG_ConfigSetting20.mof $RPM_BUILD_ROOT/etc/pegasus/mof/PG_ConfigSetting20.mof
install -D $PEGASUS_ROOT/Schemas/Pegasus/Internal/VER20/PG_InternalSchema20.mof $RPM_BUILD_ROOT/etc/pegasus/mof/InternalSchema.mof
install -D $PEGASUS_ROOT/Schemas/Pegasus/Internal/VER20/PG_ShutdownService20.mof $RPM_BUILD_ROOT/etc/pegasus/mof/PG_ShutdownService20.mof
install -D $PEGASUS_ROOT/Schemas/Pegasus/Internal/VER20/PG_User20.mof $RPM_BUILD_ROOT/etc/pegasus/mof/PG_User20.mof

#
# Pegasus Default Providers' schema
#
install -D $PEGASUS_ROOT/Schemas/Pegasus/ManagedSystem/VER20/IBM_CIMOMStatData.mof $RPM_BUILD_ROOT/etc/pegasus/mof/IBM_CIMOMStatData.mof
install -D $PEGASUS_ROOT/Schemas/Pegasus/ManagedSystem/VER20/IBM_CIMOMStatDataR.mof $RPM_BUILD_ROOT/etc/pegasus/mof/IBM_CIMOMStatDataR.mof
install -D $PEGASUS_ROOT/Schemas/Pegasus/ManagedSystem/VER20/PG_ComputerSystem20.mof $RPM_BUILD_ROOT/etc/pegasus/mof/PG_ComputerSystem20.mof
install -D $PEGASUS_ROOT/Schemas/Pegasus/ManagedSystem/VER20/PG_ComputerSystem20R.mof $RPM_BUILD_ROOT/etc/pegasus/mof/PG_ComputerSystem20R.mof
install -D $PEGASUS_ROOT/Schemas/Pegasus/ManagedSystem/VER20/PG_DNSAdminDomain20.mof $RPM_BUILD_ROOT/etc/pegasus/mof/PG_DNSAdminDomain20.mof
install -D $PEGASUS_ROOT/Schemas/Pegasus/ManagedSystem/VER20/PG_DNSAdminDomain20R.mof $RPM_BUILD_ROOT/etc/pegasus/mof/PG_DNSAdminDomain20R.mof
install -D $PEGASUS_ROOT/Schemas/Pegasus/ManagedSystem/VER20/PG_DNSService20.mof $RPM_BUILD_ROOT/etc/pegasus/mof/PG_DNSService20.mof
install -D $PEGASUS_ROOT/Schemas/Pegasus/ManagedSystem/VER20/PG_DNSService20R.mof $RPM_BUILD_ROOT/etc/pegasus/mof/PG_DNSService20R.mof
install -D $PEGASUS_ROOT/Schemas/Pegasus/ManagedSystem/VER20/PG_IP20.mof $RPM_BUILD_ROOT/etc/pegasus/mof/PG_IP20.mof
install -D $PEGASUS_ROOT/Schemas/Pegasus/ManagedSystem/VER20/PG_IP20R.mof $RPM_BUILD_ROOT/etc/pegasus/mof/PG_IP20R.mof
install -D $PEGASUS_ROOT/Schemas/Pegasus/ManagedSystem/VER20/PG_ManagedSystemSchema20.mof $RPM_BUILD_ROOT/etc/pegasus/mof/ManagedSystemSchema.mof
install -D $PEGASUS_ROOT/Schemas/Pegasus/ManagedSystem/VER20/PG_ManagedSystemSchema20R.mof $RPM_BUILD_ROOT/etc/pegasus/mof/ManagedSystemSchemaR.mof
install -D $PEGASUS_ROOT/Schemas/Pegasus/ManagedSystem/VER20/PG_NTPAdminDomain20.mof $RPM_BUILD_ROOT/etc/pegasus/mof/PG_NTPAdminDomain20.mof
install -D $PEGASUS_ROOT/Schemas/Pegasus/ManagedSystem/VER20/PG_NTPAdminDomain20R.mof $RPM_BUILD_ROOT/etc/pegasus/mof/PG_NTPAdminDomain20R.mof
install -D $PEGASUS_ROOT/Schemas/Pegasus/ManagedSystem/VER20/PG_NTPService20.mof $RPM_BUILD_ROOT/etc/pegasus/mof/PG_NTPService20.mof
install -D $PEGASUS_ROOT/Schemas/Pegasus/ManagedSystem/VER20/PG_NTPService20R.mof $RPM_BUILD_ROOT/etc/pegasus/mof/PG_NTPService20R.mof
install -D $PEGASUS_ROOT/Schemas/Pegasus/ManagedSystem/VER20/PG_OperatingSystem20.mof $RPM_BUILD_ROOT/etc/pegasus/mof/PG_OperatingSystem20.mof
install -D $PEGASUS_ROOT/Schemas/Pegasus/ManagedSystem/VER20/PG_OperatingSystem20R.mof $RPM_BUILD_ROOT/etc/pegasus/mof/PG_OperatingSystem20R.mof
install -D $PEGASUS_ROOT/Schemas/Pegasus/ManagedSystem/VER20/PG_Processor20.mof $RPM_BUILD_ROOT/etc/pegasus/mof/PG_Processor20.mof
install -D $PEGASUS_ROOT/Schemas/Pegasus/ManagedSystem/VER20/PG_Processor20R.mof $RPM_BUILD_ROOT/etc/pegasus/mof/PG_Processor20R.mof
install -D $PEGASUS_ROOT/Schemas/Pegasus/ManagedSystem/VER20/PG_UnixProcess20.mof $RPM_BUILD_ROOT/etc/pegasus/mof/PG_UnixProcess20.mof
install -D $PEGASUS_ROOT/Schemas/Pegasus/ManagedSystem/VER20/PG_UnixProcess20R.mof $RPM_BUILD_ROOT/etc/pegasus/mof/PG_UnixProcess20R.mof

#
# SSL Files
#
install -D $PEGASUS_ROOT/src/Server/ssl.cnf $RPM_BUILD_ROOT/var/cache/pegasus/ssl.orig

#
# cimserver config files
#
install -D $PEGASUS_ROOT/src/Server/cimserver_current.conf $RPM_BUILD_ROOT/etc/pegasus/cimserver_current.conf
install -D $PEGASUS_ROOT/rpm/cimserver_planned.conf $RPM_BUILD_ROOT/etc/pegasus/cimserver_planned.conf

#
# WBEM pam authentication
#
install -D $PEGASUS_ROOT/rpm/wbem $RPM_BUILD_ROOT/etc/pam.d/wbem

#
# repository install script
#
install -D $PEGASUS_ROOT/rpm/scripts/init_repository_Linux_rh32 $RPM_BUILD_ROOT/usr/sbin/init_repository

#
# man pages
#
install -D $PEGASUS_ROOT/rpm/manLinux/man1m.Z/cimauth.1m $RPM_BUILD_ROOT/usr/share/man/man1m/cimauth.1m
install -D $PEGASUS_ROOT/rpm/manLinux/man1m.Z/cimconfig.1m $RPM_BUILD_ROOT/usr/share/man/man1m/cimconfig.1m
install -D $PEGASUS_ROOT/rpm/manLinux/man1m.Z/cimprovider.1m $RPM_BUILD_ROOT/usr/share/man/man1m/cimprovider.1m
install -D $PEGASUS_ROOT/rpm/manLinux/man1m.Z/cimserver.1m $RPM_BUILD_ROOT/usr/share/man/man1m/cimserver.1m
install -D $PEGASUS_ROOT/rpm/manLinux/man1.Z/cimmof.1 $RPM_BUILD_ROOT/usr/share/man/man1/cimmof.1
install -D $PEGASUS_ROOT/rpm/manLinux/man1.Z/cimprovider.1 $RPM_BUILD_ROOT/usr/share/man/man1/cimprovider.1
install -D $PEGASUS_ROOT/rpm/manLinux/man1.Z/osinfo.1 $RPM_BUILD_ROOT/usr/share/man/man1/osinfo.1
install -D $PEGASUS_ROOT/rpm/manLinux/man1.Z/wbemexec.1 $RPM_BUILD_ROOT/usr/share/man/man1/wbemexec.1

rm -Rf $PEGASUS_HOME
%clean
#make clean
[ "$RPM_BUILD_ROOT" != "/" ] && [ -d $RPM_BUILD_ROOT ] && rm -rf $RPM_BUILD_ROOT;

%post
%{fillup_and_insserv -npy pegasus-wbem pegasus-wbem}
mkdir -p /var/log/pegasus
export INSTALL_LOG=/var/log/pegasus/install.log
echo `date` >$INSTALL_LOG 2>&1

isUnited=`grep "UnitedLinux" /etc/issue`
isSUSE=`grep "SuSE" /etc/issue`

if [ "$isUnited" ] || [ "$isSUSE" ]; then
	chkconfig --add pegasus-wbem
else
# RH dependency
	/sbin/chkconfig --add pegasus-wbem
	/sbin/chkconfig --level 2345 pegasus-wbem on
fi

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
ln -s $RPM_BUILD_ROOT/usr/lib/pegasus/libAlertIndicationProvider.so.1    $RPM_BUILD_ROOT/usr/lib/pegasus/providers/libAlertIndicationProvider.so
ln -s $RPM_BUILD_ROOT/usr/lib/pegasus/libAlertIndicationProvider.so.1    $RPM_BUILD_ROOT/usr/lib/pegasus/providers/libAlertIndicationProvider.so.1
ln -s $RPM_BUILD_ROOT/usr/lib/pegasus/libIBM_CIMOMStatDataProvider.so.1    $RPM_BUILD_ROOT/usr/lib/pegasus/providers/libIBM_CIMOMStatDataProvider.so
ln -s $RPM_BUILD_ROOT/usr/lib/pegasus/libIBM_CIMOMStatDataProvider.so.1    $RPM_BUILD_ROOT/usr/lib/pegasus/providers/libIBM_CIMOMStatDataProvider.so.1
ln -s $RPM_BUILD_ROOT/usr/lib/pegasus/libOperatingSystemProvider.so.1    $RPM_BUILD_ROOT/usr/lib/pegasus/providers/libOperatingSystemProvider.so
ln -s $RPM_BUILD_ROOT/usr/lib/pegasus/libOperatingSystemProvider.so.1    $RPM_BUILD_ROOT/usr/lib/pegasus/providers/libOperatingSystemProvider.so.1
ln -s $RPM_BUILD_ROOT/usr/lib/pegasus/libSampleFamilyProvider.so.1    $RPM_BUILD_ROOT/usr/lib/pegasus/providers/libSampleFamilyProvider.so
ln -s $RPM_BUILD_ROOT/usr/lib/pegasus/libSampleFamilyProvider.so.1    $RPM_BUILD_ROOT/usr/lib/pegasus/providers/libSampleFamilyProvider.so.1
ln -s $RPM_BUILD_ROOT/usr/lib/pegasus/libSampleIndicationProvider.so.1    $RPM_BUILD_ROOT/usr/lib/pegasus/providers/libSampleIndicationProvider.so
ln -s $RPM_BUILD_ROOT/usr/lib/pegasus/libSampleIndicationProvider.so.1    $RPM_BUILD_ROOT/usr/lib/pegasus/providers/libSampleIndicationProvider.so.1
ln -s $RPM_BUILD_ROOT/usr/lib/pegasus/libSampleInstanceProvider.so.1    $RPM_BUILD_ROOT/usr/lib/pegasus/providers/libSampleInstanceProvider.so
ln -s $RPM_BUILD_ROOT/usr/lib/pegasus/libSampleInstanceProvider.so.1    $RPM_BUILD_ROOT/usr/lib/pegasus/providers/libSampleInstanceProvider.so.1
ln -s $RPM_BUILD_ROOT/usr/lib/pegasus/libSampleMethodProvider.so.1    $RPM_BUILD_ROOT/usr/lib/pegasus/providers/libSampleMethodProvider.so
ln -s $RPM_BUILD_ROOT/usr/lib/pegasus/libSampleMethodProvider.so.1    $RPM_BUILD_ROOT/usr/lib/pegasus/providers/libSampleMethodProvider.so.1

# Create symbolic links for client libs
#
cd /usr/lib/pegasus
for a in `ls -1 *.so.1 | sed s/\.so\.1/\.so/` 
do
 ln -s "$a.1" $a
done
# link directories

mkdir -p /var/cache/pegasus/repository
ln -s /var/cache/pegasus/repository  /etc/pegasus/repository
#
#  Set up the openssl certificate
#
#  Modify entries in ssl.cnf, then
#  Create big random ssl.rnd file, then 
#  Generate a self signed node certificate
#
echo " Generating SSL certificates... "
CN="Common Name"
EMAIL="test@email.address"
HOSTNAME=`uname -n`
sed -e "s/$CN/$HOSTNAME/"  \
    -e "s/$EMAIL/root@$HOSTNAME/" /var/cache/pegasus/ssl.orig \
    > /var/cache/pegasus/ssl.cnf
chmod 644 /var/cache/pegasus/ssl.cnf
chown bin /var/cache/pegasus/ssl.cnf
chgrp bin /var/cache/pegasus/ssl.cnf

# This random-enthropy-gathering is NOT random at all. Use the default one SSL has - (/dev/random).
# /bin/rpm -qa >/var/cache/pegasus/ssl.rnd 2>>$INSTALL_LOG
# /bin/netstat -a >>/var/cache/pegasus/ssl.rnd 2>>$INSTALL_LOG
#   -rand /var/cache/pegasus/ssl.rnd 
openssl req -x509 -days 365 -newkey rsa:2048 \
   -nodes -config /var/cache/pegasus/ssl.cnf   \
   -keyout /var/cache/pegasus/key.pem -out /var/cache/pegasus/cert.pem 2>>$INSTALL_LOG

cat /var/cache/pegasus/key.pem /var/cache/pegasus/cert.pem > /var/cache/pegasus/server_2048.pem
cat /var/cache/pegasus/cert.pem > /var/cache/pegasus/client_2048.pem
chmod 700 /var/cache/pegasus/*.pem

rm -f /var/cache/pegasus/key.pem /var/cache/pegasus/cert.pem

if [ -f /var/cache/pegasus/server.pem ] 
then
    echo "WARNING: /var/cache/pegasus/server.pem SSL Certificate file already exists."
else
    cp /var/cache/pegasus/server_2048.pem /var/cache/pegasus/server.pem
    chmod 400 /var/cache/pegasus/server.pem
fi

if [ -f /var/cache/pegasus/client.pem ]
then
    echo "WARNING: /var/cache/pegasus/client.pem SSL Certificate trust store already exists."
else
    cp /var/cache/pegasus/client_2048.pem /var/cache/pegasus/client.pem
    chmod 400 /var/cache/pegasus/client.pem
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

# Start repository buildup
if [ "$isUnited" ] || [ "$isSUSE" ]; then
	/etc/init.d/pegasus-wbem start 2>>$INSTALL_LOG
else
	/sbin/service pegasus-wbem start 2>>$INSTALL_LOG
fi

if [ $? != 0 ];
then
  echo "Brute-starting Pegasus. If the installation fails:"
  echo " - Start Pegasus: '/etc/init.d/pegasus-wbem start'"
  echo " - Run '/usr/sbin/init_repository'"
  /etc/init.d/pegasus-wbem start
fi

sleep 5
echo " Compiling mof files will take a few minutes."
echo " Output will be logged to $INSTALL_LOG."
/usr/sbin/init_repository 2>>$INSTALL_LOG

if [ "$isUnited" ] || [ "$isSUSE" ]; then
	/etc/init.d/pegasus-wbem stop 2>>$INSTALL_LOG
else
	/sbin/service pegasus-wbem stop 2>>$INSTALL_LOG
fi

if [ $? != 0 ];
then
  ps -eff | grep cimserver | grep -v grep | awk '{ print "kill -9" $2 | "/bin/bash"}'
fi


echo " To start Pegasus manually:"
echo " /etc/init.d/pegasus-wbem start"
echo " Stop it:"
echo " /etc/init.d/pegasus-wbem stop"

%preun
isUnited=`grep "UnitedLinux" /etc/issue`
isSUSE=`grep "SuSE" /etc/issue`

if [ "$isUnited" ] || [ "$isSUSE" ]; then
	/etc/init.d/pegasus-wbem stop 
else
	/sbin/service pegasus-wbem stop > /dev/null 2> /dev/null; :
fi

sleep 10
chkconfig --del pegasus-wbem

%postun
if [ $1 = 0 ]; then
	grep -v "/usr/lib/pegasus" /etc/ld.so.conf > /etc/ld.so.conf.new
	mv -f /etc/ld.so.conf.new /etc/ld.so.conf
	/sbin/ldconfig
	rm -f /etc/pegasus/repository
	rm -rf /var/cache/pegasus
	export LC_ALL=C
	for file in `find /usr/lib/pegasus`;
	do
		ANS=`file $file | grep "broken symbolic link"`
		if [ "$ANS" != "" ]; then
			# Found it
			rm -f $file
		fi
	done
	rm /usr/lib/pegasus/ssl.rnd
#        rm -f /etc/pam.d/wbem
fi

%files
%dir %attr(-,root,root) /var/cache/pegasus/localauth
%dir %attr(-,root,root) /var/log/pegasus
%dir %attr(-,root,root) /usr/lib/pegasus/providers
%attr(0444,root,root) /usr/share/man/man1/cimmof.1.gz
%attr(0444,root,root) /usr/share/man/man1/cimprovider.1.gz
%attr(0444,root,root) /usr/share/man/man1m/cimauth.1m.gz
%attr(0444,root,root) /usr/share/man/man1m/cimconfig.1m.gz
%attr(0444,root,root) /usr/share/man/man1m/cimprovider.1m.gz
%attr(0444,root,root) /usr/share/man/man1m/cimserver.1m.gz
%attr(0444,root,root) /usr/share/man/man1/osinfo.1.gz
%attr(0444,root,root) /usr/share/man/man1/wbemexec.1.gz
%attr(0544,root,root) /usr/sbin/cimauth
%attr(0544,root,root) /usr/sbin/cimconfig
%attr(0544,root,root) /usr/sbin/cimserver
%attr(0544,root,root) /usr/sbin/cimuser
%attr(0544,root,root) /usr/sbin/init_repository
%attr(0644,root,root) /etc/pam.d/wbem
%attr(0644,root,root) /etc/pegasus/cimserver_current.conf
%attr(0644,root,root) /etc/pegasus/cimserver_planned.conf
%attr(0644,root,root) /etc/pegasus/mof/Application27_BIOS.mof
%attr(0644,root,root) /etc/pegasus/mof/Application27_CheckAction.mof
%attr(0644,root,root) /etc/pegasus/mof/Application27_DeploymentModel.mof
%attr(0644,root,root) /etc/pegasus/mof/Application27_InstalledProduct.mof
%attr(0644,root,root) /etc/pegasus/mof/Application27_SystemSoftware.mof
%attr(0644,root,root) /etc/pegasus/mof/CIM_Application27.mof
%attr(0644,root,root) /etc/pegasus/mof/CIM_Core27.mof
%attr(0644,root,root) /etc/pegasus/mof/CIM_Database27.mof
%attr(0644,root,root) /etc/pegasus/mof/CIM_Device27.mof
%attr(0644,root,root) /etc/pegasus/mof/CIM_Event27.mof
%attr(0644,root,root) /etc/pegasus/mof/CIM_Interop27.mof
%attr(0644,root,root) /etc/pegasus/mof/CIM_Metrics27.mof
%attr(0644,root,root) /etc/pegasus/mof/CIM_Network27.mof
%attr(0644,root,root) /etc/pegasus/mof/CIM_Physical27.mof
%attr(0644,root,root) /etc/pegasus/mof/CIM_Policy27.mof
%attr(0644,root,root) /etc/pegasus/mof/CIM_Schema27.mof
%attr(0644,root,root) /etc/pegasus/mof/CIM_Support27.mof
%attr(0644,root,root) /etc/pegasus/mof/CIM_System27.mof
%attr(0644,root,root) /etc/pegasus/mof/CIM_User27.mof
%attr(0644,root,root) /etc/pegasus/mof/Core27_Capabilities.mof
%attr(0644,root,root) /etc/pegasus/mof/Core27_Collection.mof
%attr(0644,root,root) /etc/pegasus/mof/Core27_CoreElements.mof
%attr(0644,root,root) /etc/pegasus/mof/Core27_Device.mof
%attr(0644,root,root) /etc/pegasus/mof/Core27_MethodParms.mof
%attr(0644,root,root) /etc/pegasus/mof/Core27_Physical.mof
%attr(0644,root,root) /etc/pegasus/mof/Core27_PowerMgmt.mof
%attr(0644,root,root) /etc/pegasus/mof/Core27_ProductFRU.mof
%attr(0644,root,root) /etc/pegasus/mof/Core27_Qualifiers.mof
%attr(0644,root,root) /etc/pegasus/mof/Core27_Redundancy.mof
%attr(0644,root,root) /etc/pegasus/mof/Core27_Settings.mof
%attr(0644,root,root) /etc/pegasus/mof/Core27_Software.mof
%attr(0644,root,root) /etc/pegasus/mof/Core27_Statistics.mof
%attr(0644,root,root) /etc/pegasus/mof/Core27_StorageExtent.mof
%attr(0644,root,root) /etc/pegasus/mof/Device27_Controller.mof
%attr(0644,root,root) /etc/pegasus/mof/Device27_CoolingAndPower.mof
%attr(0644,root,root) /etc/pegasus/mof/Device27_DeviceElements.mof
%attr(0644,root,root) /etc/pegasus/mof/Device27_DiskGroup.mof
%attr(0644,root,root) /etc/pegasus/mof/Device27_FC.mof
%attr(0644,root,root) /etc/pegasus/mof/Device27_IB.mof
%attr(0644,root,root) /etc/pegasus/mof/Device27_Memory.mof
%attr(0644,root,root) /etc/pegasus/mof/Device27_Modems.mof
%attr(0644,root,root) /etc/pegasus/mof/Device27_NetworkAdapter.mof
%attr(0644,root,root) /etc/pegasus/mof/Device27_Ports.mof
%attr(0644,root,root) /etc/pegasus/mof/Device27_Printing.mof
%attr(0644,root,root) /etc/pegasus/mof/Device27_Processor.mof
%attr(0644,root,root) /etc/pegasus/mof/Device27_SccExtents.mof
%attr(0644,root,root) /etc/pegasus/mof/Device27_Sensors.mof
%attr(0644,root,root) /etc/pegasus/mof/Device27_StorageDevices.mof
%attr(0644,root,root) /etc/pegasus/mof/Device27_StorageExtents.mof
%attr(0644,root,root) /etc/pegasus/mof/Device27_StorageLib.mof
%attr(0644,root,root) /etc/pegasus/mof/Device27_StorageServices.mof
%attr(0644,root,root) /etc/pegasus/mof/Device27_USB.mof
%attr(0644,root,root) /etc/pegasus/mof/Device27_UserDevices.mof
%attr(0644,root,root) /etc/pegasus/mof/IBM_CIMOMStatData.mof
%attr(0644,root,root) /etc/pegasus/mof/IBM_CIMOMStatDataR.mof
%attr(0644,root,root) /etc/pegasus/mof/InternalSchema.mof
%attr(0644,root,root) /etc/pegasus/mof/InterOpSchema.mof
%attr(0644,root,root) /etc/pegasus/mof/ManagedSystemSchema.mof
%attr(0644,root,root) /etc/pegasus/mof/ManagedSystemSchemaR.mof
%attr(0644,root,root) /etc/pegasus/mof/Metrics27_BaseMetric.mof
%attr(0644,root,root) /etc/pegasus/mof/Metrics27_UnitOfWork.mof
%attr(0644,root,root) /etc/pegasus/mof/Network27_BGP.mof
%attr(0644,root,root) /etc/pegasus/mof/Network27_Buffers.mof
%attr(0644,root,root) /etc/pegasus/mof/Network27_Collections.mof
%attr(0644,root,root) /etc/pegasus/mof/Network27_Filtering.mof
%attr(0644,root,root) /etc/pegasus/mof/Network27_OSPF.mof
%attr(0644,root,root) /etc/pegasus/mof/Network27_Pipes.mof
%attr(0644,root,root) /etc/pegasus/mof/Network27_ProtocolEndpoints.mof
%attr(0644,root,root) /etc/pegasus/mof/Network27_QoS.mof
%attr(0644,root,root) /etc/pegasus/mof/Network27_Routes.mof
%attr(0644,root,root) /etc/pegasus/mof/Network27_RoutingForwarding.mof
%attr(0644,root,root) /etc/pegasus/mof/Network27_SNMP.mof
%attr(0644,root,root) /etc/pegasus/mof/Network27_SwitchingBridging.mof
%attr(0644,root,root) /etc/pegasus/mof/Network27_Systems.mof
%attr(0644,root,root) /etc/pegasus/mof/Network27_VLAN.mof
%attr(0644,root,root) /etc/pegasus/mof/PG_Authorization20.mof
%attr(0644,root,root) /etc/pegasus/mof/PG_ComputerSystem20.mof
%attr(0644,root,root) /etc/pegasus/mof/PG_ComputerSystem20R.mof
%attr(0644,root,root) /etc/pegasus/mof/PG_ConfigSetting20.mof
%attr(0644,root,root) /etc/pegasus/mof/PG_DNSAdminDomain20.mof
%attr(0644,root,root) /etc/pegasus/mof/PG_DNSAdminDomain20R.mof
%attr(0644,root,root) /etc/pegasus/mof/PG_DNSService20.mof
%attr(0644,root,root) /etc/pegasus/mof/PG_DNSService20R.mof
%attr(0644,root,root) /etc/pegasus/mof/PG_Events20.mof
%attr(0644,root,root) /etc/pegasus/mof/PG_IP20.mof
%attr(0644,root,root) /etc/pegasus/mof/PG_IP20R.mof
%attr(0644,root,root) /etc/pegasus/mof/PG_NTPAdminDomain20.mof
%attr(0644,root,root) /etc/pegasus/mof/PG_NTPAdminDomain20R.mof
%attr(0644,root,root) /etc/pegasus/mof/PG_NTPService20.mof
%attr(0644,root,root) /etc/pegasus/mof/PG_NTPService20R.mof
%attr(0644,root,root) /etc/pegasus/mof/PG_OperatingSystem20.mof
%attr(0644,root,root) /etc/pegasus/mof/PG_OperatingSystem20R.mof
%attr(0644,root,root) /etc/pegasus/mof/PG_Processor20.mof
%attr(0644,root,root) /etc/pegasus/mof/PG_Processor20R.mof
%attr(0644,root,root) /etc/pegasus/mof/PG_ProviderModule20.mof
%attr(0644,root,root) /etc/pegasus/mof/PG_ShutdownService20.mof
%attr(0644,root,root) /etc/pegasus/mof/PG_UnixProcess20.mof
%attr(0644,root,root) /etc/pegasus/mof/PG_UnixProcess20R.mof
%attr(0644,root,root) /etc/pegasus/mof/PG_User20.mof
%attr(0644,root,root) /etc/pegasus/mof/Physical27_Component.mof
%attr(0644,root,root) /etc/pegasus/mof/Physical27_Connector.mof
%attr(0644,root,root) /etc/pegasus/mof/Physical27_Link.mof
%attr(0644,root,root) /etc/pegasus/mof/Physical27_Misc.mof
%attr(0644,root,root) /etc/pegasus/mof/Physical27_Package.mof
%attr(0644,root,root) /etc/pegasus/mof/Physical27_PhysicalMedia.mof
%attr(0644,root,root) /etc/pegasus/mof/Physical27_StoragePackage.mof
%attr(0644,root,root) /etc/pegasus/mof/System27_Boot.mof
%attr(0644,root,root) /etc/pegasus/mof/System27_Diagnostics.mof
%attr(0644,root,root) /etc/pegasus/mof/System27_FileElements.mof
%attr(0644,root,root) /etc/pegasus/mof/System27_Logs.mof
%attr(0644,root,root) /etc/pegasus/mof/System27_OperatingSystem.mof
%attr(0644,root,root) /etc/pegasus/mof/System27_Processing.mof
%attr(0644,root,root) /etc/pegasus/mof/System27_SystemElements.mof
%attr(0644,root,root) /etc/pegasus/mof/System27_SystemResources.mof
%attr(0644,root,root) /etc/pegasus/mof/System27_Time.mof
%attr(0644,root,root) /etc/pegasus/mof/System27_Unix.mof
%attr(0644,root,root) /etc/pegasus/mof/User27_AccessControl.mof
%attr(0644,root,root) /etc/pegasus/mof/User27_Account.mof
%attr(0644,root,root) /etc/pegasus/mof/User27_AuthenticationReqmt.mof
%attr(0644,root,root) /etc/pegasus/mof/User27_Credential.mof
%attr(0644,root,root) /etc/pegasus/mof/User27_Group.mof
%attr(0644,root,root) /etc/pegasus/mof/User27_Kerberos.mof
%attr(0644,root,root) /etc/pegasus/mof/User27_Org.mof
%attr(0644,root,root) /etc/pegasus/mof/User27_PublicKey.mof
%attr(0644,root,root) /etc/pegasus/mof/User27_Role.mof
%attr(0644,root,root) /etc/pegasus/mof/User27_SecurityServices.mof
%attr(0644,root,root) /etc/pegasus/mof/User27_SharedSecret.mof
%attr(0644,root,root) /etc/pegasus/mof/User27_UsersAccess.mof
%attr(0644,root,root) /var/cache/pegasus/ssl.orig
%attr(0755,root,root) /etc/init.d/pegasus-wbem
%attr(0755,root,root) /usr/bin/cimmof
%attr(0755,root,root) /usr/bin/cimmofl
%attr(0755,root,root) /usr/bin/cimprovider
%attr(0755,root,root) /usr/bin/CLI
%attr(0755,root,root) /usr/bin/ipinfo
%attr(0755,root,root) /usr/bin/osinfo
%attr(0755,root,root) /usr/bin/tomof
%attr(0755,root,root) /usr/bin/wbemexec
%attr(0755,root,root) /usr/lib/pegasus/libAlertIndicationProvider.so.1
%attr(0755,root,root) /usr/lib/pegasus/libCIMxmlIndicationHandler.so.1
%attr(0755,root,root) /usr/lib/pegasus/libComputerSystemProvider.so.1
%attr(0755,root,root) /usr/lib/pegasus/libConfigSettingProvider.so.1
%attr(0755,root,root) /usr/lib/pegasus/libDisplayConsumer.so.1
%attr(0755,root,root) /usr/lib/pegasus/libDynLib.so.1
%attr(0755,root,root) /usr/lib/pegasus/libIBM_CIMOMStatDataProvider.so.1
%attr(0755,root,root) /usr/lib/pegasus/libIPProviderModule.so.1
%attr(0755,root,root) /usr/lib/pegasus/libNamespaceProvider.so.1
%attr(0755,root,root) /usr/lib/pegasus/libnsatrap.so.1
%attr(0755,root,root) /usr/lib/pegasus/libOperatingSystemProvider.so.1
%attr(0755,root,root) /usr/lib/pegasus/libOSProvider.so.1
%attr(0755,root,root) /usr/lib/pegasus/libpegauthentication.so.1
%attr(0755,root,root) /usr/lib/pegasus/libpegCLIClientLib.so.1
%attr(0755,root,root) /usr/lib/pegasus/libpegclient.so.1
%attr(0755,root,root) /usr/lib/pegasus/libpegcliutils.so.1
%attr(0755,root,root) /usr/lib/pegasus/libpegcommon.so.1
%attr(0755,root,root) /usr/lib/pegasus/libpegcompiler.so.1
%attr(0755,root,root) /usr/lib/pegasus/libpegconfig.so.1
%attr(0755,root,root) /usr/lib/pegasus/libpegexportclient.so.1
%attr(0755,root,root) /usr/lib/pegasus/libpegexportserver.so.1
%attr(0755,root,root) /usr/lib/pegasus/libpeggetoopt.so.1
%attr(0755,root,root) /usr/lib/pegasus/libpeghandlerservice.so.1
%attr(0755,root,root) /usr/lib/pegasus/libpegindicationservice.so.1
%attr(0755,root,root) /usr/lib/pegasus/libpeglistener.so.1
%attr(0755,root,root) /usr/lib/pegasus/libpegprm.so.1
%attr(0755,root,root) /usr/lib/pegasus/libpegprovidermanager.so.1
%attr(0755,root,root) /usr/lib/pegasus/libpegprovider.so.1
%attr(0755,root,root) /usr/lib/pegasus/libpegrepository.so.1
%attr(0755,root,root) /usr/lib/pegasus/libpegserver.so.1
%attr(0755,root,root) /usr/lib/pegasus/libpeguser.so.1
%attr(0755,root,root) /usr/lib/pegasus/libpegwql.so.1
%attr(0755,root,root) /usr/lib/pegasus/libPG_TestPropertyTypes.so.1
%attr(0755,root,root) /usr/lib/pegasus/libProcessIndicationProvider.so.1
%attr(0755,root,root) /usr/lib/pegasus/libProcessorProviderModule.so.1
%attr(0755,root,root) /usr/lib/pegasus/libProcessProvider.so.1
%attr(0755,root,root) /usr/lib/pegasus/libProviderRegistrationProvider.so.1
%attr(0755,root,root) /usr/lib/pegasus/libRT_IndicationConsumer.so.1
%attr(0755,root,root) /usr/lib/pegasus/libRT_IndicationProvider.so.1
%attr(0755,root,root) /usr/lib/pegasus/libSampleFamilyProvider.so.1
%attr(0755,root,root) /usr/lib/pegasus/libSampleIndicationProvider.so.1
%attr(0755,root,root) /usr/lib/pegasus/libSampleInstanceProvider.so.1
%attr(0755,root,root) /usr/lib/pegasus/libSampleMethodProvider.so.1
%attr(0755,root,root) /usr/lib/pegasus/libsendmailIndicationHandler.so.1
%attr(0755,root,root) /usr/lib/pegasus/libSimpleDisplayConsumer.so.1

%attr(0755,root,root) /usr/lib/pegasus/libsnmpIndicationHandler.so.1
%attr(0755,root,root) /usr/lib/pegasus/libUserAuthProvider.so.1

%config %attr(0644,root,root) /etc/pegasus/cimserver_current.conf
%config %attr(0644,root,root) /etc/pegasus/cimserver_planned.conf
%config %attr(0755,root,root) /etc/init.d/pegasus-wbem
%config %attr(0644,root,root) /etc/pam.d/wbem


%ifnarch ia64
%attr(0755,root,root) /usr/lib/pegasus/libslp.so.1
%endif

%files devel -f rpm_pegasus_include_files

%defattr(-,root,root,0755)
%doc doc/*.txt doc/DOCREMARKS doc/HISTORY doc/NOTES doc/*.html doc/*.pdf


