#%2003////////////////////////////////////////////////////////////////////////
#
# Copyright (c) 2000, 2001, 2002 BMC Software; Hewlett-Packard Development
# Company, L.P.; IBM Corp.; The Open Group; Tivoli Systems.
# Copyright (c) 2003 BMC Software; Hewlett-Packard Development Company, L.P.;
# IBM Corp.; EMC Corporation, The Open Group.
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
#%/////////////////////////////////////////////////////////////////////////////
#
# tog-pegasus.spec
#
# Package spec for PEGASUS 2.4
#
Summary: WBEM Services for Linux
Name: tog-pegasus
Version: 2.4Prerelease
Release: 1
Group: Systems Management/Base
Copyright: Open Group Pegasus Open Source
BuildRoot: %{_tmppath}/%{name}-%{version}-%{release}
Source: ftp://www.opengroup.org/pegasus/tog-pegasus-%{version}-%{release}.tar.gz
Requires: openssl-devel >= 0.9.6
Provides: cimserver tog-pegasus-2.4

%description
WBEM Services for Red Hat Linux enables management solutions that deliver
increased control of enterprise resources. WBEM is a platform and resource
independent DMTF standard that defines a common information model and
communication protocol for monitoring and controlling resources from diverse
sources.

%package sdk
Summary:      The Pegasus source tree
Group:        Systems Management/Base
Autoreq: 0
Requires: tog-pegasus >= 2.4, openssl-devel >= 0.9.6

%description sdk
This package contains the Pegasus source tree, header files and
static libraries (if any).


%prep
[ "$RPM_BUILD_ROOT" != "/" ] && [ -d $RPM_BUILD_ROOT ] && rm -rf $RPM_BUILD_ROOT;

%setup

export PEGASUS_ROOT=$RPM_BUILD_DIR/$RPM_PACKAGE_NAME-$RPM_PACKAGE_VERSION

# Needed for CMPI patch
ln -s $RPM_BUILD_DIR/$RPM_PACKAGE_NAME-$RPM_PACKAGE_VERSION $RPM_BUILD_DIR/$RPM_PACKAGE_NAME

%build
export PEGASUS_ROOT=$RPM_BUILD_DIR/$RPM_PACKAGE_NAME-$RPM_PACKAGE_VERSION
export PEGASUS_HOME=$RPM_BUILD_ROOT/usr/pegasus
%ifarch ia64
export PEGASUS_PLATFORM=LINUX_IA64_GNU
%else
export PEGASUS_PLATFORM=LINUX_IX86_GNU
%endif

export OPENSSL_HOME=/usr
export PEGASUS_HAS_SSL=yes

# per bug #368
export PEGASUS_USE_RELEASE_DIRS=true
export PEGASUS_USE_RELEASE_CONFIG_OPTIONS=true
export SYS_INCLUDES=-I/usr/kerberos/include

# per PEP #144
export PEGASUS_PAM_AUTHENTICATION=true
export PEGASUS_USE_PAM_STANDALONE_PROC=true
export PEGASUS_LOCAL_DOMAIN_SOCKET=true
export PEGASUS_USE_SYSLOGS=true
export PEGASUS_ENABLE_CMPI_PROVIDER_MANAGER=true

# per PEP #174
export PEGASUS_ENABLE_USERGROUP_AUTHORIZATION=true
export PEGASUS_HAS_PERFSTATS=true
export PEGASUS_NOASSERTS=true

make
export LD_LIBRARY_PATH=$PEGASUS_HOME/lib
export PATH=$PEGASUS_HOME/bin:$PATH
make repository

cd $PEGASUS_ROOT/mak
make -f SDKMakefile stageSDK

%install
%define PEGASUS_PLATFORM       $PEGASUS_PLATFORM
%define PEGASUS_PROD_DIR       /opt/tog-pegasus
%define PEGASUS_DEST_LIB_DIR   %PEGASUS_PROD_DIR/lib
%define PEGASUS_PROVIDER_DIR   %PEGASUS_PROD_DIR/providers
%define PEGASUS_PROVIDER_LIB_DIR    %PEGASUS_PROVIDER_DIR/lib
%define PEGASUS_SBIN_DIR       %PEGASUS_PROD_DIR/sbin
%define PEGASUS_BIN_DIR        %PEGASUS_PROD_DIR/bin
%define PEGASUS_PRODSHARE_DIR  %PEGASUS_PROD_DIR/share
%define PEGASUS_MAN_DIR        %PEGASUS_PRODSHARE_DIR/man
%define PEGASUS_MANUSER_DIR    %PEGASUS_MAN_DIR/man1
%define PEGASUS_MANADMIN_DIR   %PEGASUS_MAN_DIR/man8
%define PEGASUS_VARDATA_DIR    /var/opt/tog-pegasus
%define PEGASUS_REPOSITORY_DIR      %PEGASUS_VARDATA_DIR/repository
%define PEGASUS_VARDATA_CACHE_DIR   %PEGASUS_VARDATA_DIR/cache
%define PEGASUS_LOCAL_AUTH_DIR      %PEGASUS_VARDATA_CACHE_DIR/localauth
%define PEGASUS_MOF_DIR            %PEGASUS_PROD_DIR/mof
%define PEGASUS_CONFIG_DIR         /etc/opt/tog-pegasus
%define PEGASUS_VARRUN_DIR         /var/run/tog-pegasus
%define PEGASUS_LOCAL_DOMAIN_SOCKET_DIR %PEGASUS_VARRUN_DIR/socket
%define PAM_CONF                   /etc/pam.d
%define PEGASUS_LOG_DIR            %PEGASUS_VARDATA_DIR/log
%define PEGASUS_LICENSE_FILE       license.txt
%define PEGASUS_PLANNED_CONFIG_FILE cimserver_planned.conf
%define PEGASUS_PEM_DIR            %PEGASUS_CONFIG_DIR
%define PEGASUS_SSL_KEY_FILE       file.pem
%define PEGASUS_SSL_CERT_FILE      server.pem
%define PEGASUS_SSL_TRUSTSTORE     client.pem
#
# Make directories
mkdir -p $RPM_BUILD_ROOT%PEGASUS_VARDATA_DIR/{log,cache,repository}
mkdir -p $RPM_BUILD_ROOT/%PEGASUS_LOCAL_AUTH_DIR
mkdir -p $RPM_BUILD_ROOT/%PEGASUS_PROVIDER_LIB_DIR
mkdir -p $RPM_BUILD_ROOT/%PEGASUS_CONFIG_DIR 
mkdir -p $RPM_BUILD_ROOT/%PEGASUS_LOG_DIR
mkdir -p $RPM_BUILD_ROOT/%PEGASUS_MAN_DIR/{man1,man8}
mkdir -p $RPM_BUILD_ROOT%PEGASUS_MOF_DIR/{CIM28,Pegasus}
mkdir -p $RPM_BUILD_ROOT%PEGASUS_LOCAL_DOMAIN_SOCKET_DIR

export PEGASUS_ROOT=$RPM_BUILD_DIR/$RPM_PACKAGE_NAME-$RPM_PACKAGE_VERSION
export PEGASUS_HOME=$RPM_BUILD_ROOT/usr/pegasus

#
# Init scripts
install -D -m 0755  $PEGASUS_ROOT/rpm/tog-pegasus.rc $RPM_BUILD_ROOT/etc/init.d/tog-pegasus

#
# Programs
install -D -m 0544  $PEGASUS_HOME/bin/cimauth   $RPM_BUILD_ROOT%PEGASUS_SBIN_DIR/cimauth
install -D -m 0544  $PEGASUS_HOME/bin/cimserver $RPM_BUILD_ROOT%PEGASUS_SBIN_DIR/cimserver
install -D -m 0544  $PEGASUS_HOME/bin/cimservera $RPM_BUILD_ROOT%PEGASUS_SBIN_DIR/cimservera
install -D -m 0544  $PEGASUS_HOME/bin/cimuser   $RPM_BUILD_ROOT%PEGASUS_SBIN_DIR/cimuser
install -D -m 0544  $PEGASUS_HOME/bin/cimconfig $RPM_BUILD_ROOT%PEGASUS_SBIN_DIR/cimconfig
install -D -m 0555  $PEGASUS_HOME/bin/cimmof    $RPM_BUILD_ROOT%PEGASUS_BIN_DIR/cimmof
install -D -m 0555  $PEGASUS_HOME/bin/cimmofl   $RPM_BUILD_ROOT%PEGASUS_BIN_DIR/cimmofl
install -D -m 0555  $PEGASUS_HOME/bin/cimprovider $RPM_BUILD_ROOT%PEGASUS_BIN_DIR/cimprovider
install -D -m 0555  $PEGASUS_HOME/bin/osinfo    $RPM_BUILD_ROOT%PEGASUS_BIN_DIR/osinfo
install -D -m 0555  $PEGASUS_HOME/bin/wbemexec  $RPM_BUILD_ROOT%PEGASUS_BIN_DIR/wbemexec


# Libraries

install -D -m 0755  $PEGASUS_HOME/lib/libCIMxmlIndicationHandler.so.1    $RPM_BUILD_ROOT%PEGASUS_DEST_LIB_DIR/libCIMxmlIndicationHandler.so.1
install -D -m 0755  $PEGASUS_HOME/lib/libConfigSettingProvider.so.1    $RPM_BUILD_ROOT%PEGASUS_DEST_LIB_DIR/libConfigSettingProvider.so.1
install -D -m 0755  $PEGASUS_HOME/lib/libNamespaceProvider.so.1    $RPM_BUILD_ROOT%PEGASUS_DEST_LIB_DIR/libNamespaceProvider.so.1
install -D -m 0755  $PEGASUS_HOME/lib/libProviderRegistrationProvider.so.1    $RPM_BUILD_ROOT%PEGASUS_DEST_LIB_DIR/libProviderRegistrationProvider.so.1
install -D -m 0755  $PEGASUS_HOME/lib/libUserAuthProvider.so.1    $RPM_BUILD_ROOT%PEGASUS_DEST_LIB_DIR/libUserAuthProvider.so.1
install -D -m 0755  $PEGASUS_HOME/lib/libpegauthentication.so.1    $RPM_BUILD_ROOT%PEGASUS_DEST_LIB_DIR/libpegauthentication.so.1
install -D -m 0755  $PEGASUS_HOME/lib/libpegclient.so.1    $RPM_BUILD_ROOT%PEGASUS_DEST_LIB_DIR/libpegclient.so.1
install -D -m 0755  $PEGASUS_HOME/lib/libpegcliutils.so.1    $RPM_BUILD_ROOT%PEGASUS_DEST_LIB_DIR/libpegcliutils.so.1
install -D -m 0755  $PEGASUS_HOME/lib/libpegcommon.so.1    $RPM_BUILD_ROOT%PEGASUS_DEST_LIB_DIR/libpegcommon.so.1
install -D -m 0755  $PEGASUS_HOME/lib/libpegcompiler.so.1    $RPM_BUILD_ROOT%PEGASUS_DEST_LIB_DIR/libpegcompiler.so.1
install -D -m 0755  $PEGASUS_HOME/lib/libpegconfig.so.1    $RPM_BUILD_ROOT%PEGASUS_DEST_LIB_DIR/libpegconfig.so.1
install -D -m 0755  $PEGASUS_HOME/lib/libpegexportclient.so.1    $RPM_BUILD_ROOT%PEGASUS_DEST_LIB_DIR/libpegexportclient.so.1
install -D -m 0755  $PEGASUS_HOME/lib/libpegexportserver.so.1    $RPM_BUILD_ROOT%PEGASUS_DEST_LIB_DIR/libpegexportserver.so.1
install -D -m 0755  $PEGASUS_HOME/lib/libpeggetoopt.so.1    $RPM_BUILD_ROOT%PEGASUS_DEST_LIB_DIR/libpeggetoopt.so.1
install -D -m 0755  $PEGASUS_HOME/lib/libpeghandlerservice.so.1    $RPM_BUILD_ROOT%PEGASUS_DEST_LIB_DIR/libpeghandlerservice.so.1
install -D -m 0755  $PEGASUS_HOME/lib/libpegindicationservice.so.1    $RPM_BUILD_ROOT%PEGASUS_DEST_LIB_DIR/libpegindicationservice.so.1
install -D -m 0755  $PEGASUS_HOME/lib/libpeglistener.so.1    $RPM_BUILD_ROOT%PEGASUS_DEST_LIB_DIR/libpeglistener.so.1
install -D -m 0755  $PEGASUS_HOME/lib/libpegprm.so.1    $RPM_BUILD_ROOT%PEGASUS_DEST_LIB_DIR/libpegprm.so.1
install -D -m 0755  $PEGASUS_HOME/lib/libpegprovidermanager.so.1    $RPM_BUILD_ROOT%PEGASUS_DEST_LIB_DIR/libpegprovidermanager.so.1
install -D -m 0755  $PEGASUS_HOME/lib/libpegprovider.so.1    $RPM_BUILD_ROOT%PEGASUS_DEST_LIB_DIR/libpegprovider.so.1
install -D -m 0755  $PEGASUS_HOME/lib/libpegrepository.so.1    $RPM_BUILD_ROOT%PEGASUS_DEST_LIB_DIR/libpegrepository.so.1
install -D -m 0755  $PEGASUS_HOME/lib/libpegserver.so.1    $RPM_BUILD_ROOT%PEGASUS_DEST_LIB_DIR/libpegserver.so.1
install -D -m 0755  $PEGASUS_HOME/lib/libpeguser.so.1    $RPM_BUILD_ROOT%PEGASUS_DEST_LIB_DIR/libpeguser.so.1
install -D -m 0755  $PEGASUS_HOME/lib/libpegwql.so.1    $RPM_BUILD_ROOT%PEGASUS_DEST_LIB_DIR/libpegwql.so.1
install -D -m 0755  $PEGASUS_HOME/lib/libCMPIProviderManager.so.1   $RPM_BUILD_ROOT%PEGASUS_DEST_LIB_DIR/libCMPIProviderManager.so.1
install -D -m 0755  $PEGASUS_HOME/lib/libcmpiCppImpl.so.1   $RPM_BUILD_ROOT%PEGASUS_DEST_LIB_DIR/libcmpiCppImpl.so.1
install -D -m 0755  $PEGASUS_HOME/lib/libDefaultProviderManager.so.1   $RPM_BUILD_ROOT%PEGASUS_DEST_LIB_DIR/libDefaultProviderManager.so.1 

install -D -m 0755  $PEGASUS_HOME/lib/libComputerSystemProvider.so.1    $RPM_BUILD_ROOT%PEGASUS_PROVIDER_LIB_DIR/libComputerSystemProvider.so.1
install -D -m 0755  $PEGASUS_HOME/lib/libOSProvider.so.1    $RPM_BUILD_ROOT%PEGASUS_PROVIDER_LIB_DIR/libOSProvider.so.1
install -D -m 0755  $PEGASUS_HOME/lib/libProcessProvider.so.1    $RPM_BUILD_ROOT%PEGASUS_PROVIDER_LIB_DIR/libProcessProvider.so.1

#
# CIM schema
#
install -D -m 0444  $PEGASUS_ROOT/Schemas/CIM28/Application28_AppRuntime.mof $RPM_BUILD_ROOT%PEGASUS_MOF_DIR/CIM28/Application28_AppRuntime.mof
install -D -m 0444  $PEGASUS_ROOT/Schemas/CIM28/Application28_BIOS.mof $RPM_BUILD_ROOT%PEGASUS_MOF_DIR/CIM28/Application28_BIOS.mof
install -D -m 0444  $PEGASUS_ROOT/Schemas/CIM28/Application28_CheckAction.mof $RPM_BUILD_ROOT%PEGASUS_MOF_DIR/CIM28/Application28_CheckAction.mof
install -D -m 0444  $PEGASUS_ROOT/Schemas/CIM28/Application28_DeploymentModel.mof $RPM_BUILD_ROOT%PEGASUS_MOF_DIR/CIM28/Application28_DeploymentModel.mof
install -D -m 0444  $PEGASUS_ROOT/Schemas/CIM28/Application28_InstalledProduct.mof $RPM_BUILD_ROOT%PEGASUS_MOF_DIR/CIM28/Application28_InstalledProduct.mof
install -D -m 0444  $PEGASUS_ROOT/Schemas/CIM28/Application28_J2eeAppServer.mof $RPM_BUILD_ROOT%PEGASUS_MOF_DIR/CIM28/Application28_J2eeAppServer.mof
install -D -m 0444  $PEGASUS_ROOT/Schemas/CIM28/Application28_J2eeAppServerStats.mof $RPM_BUILD_ROOT%PEGASUS_MOF_DIR/CIM28/Application28_J2eeAppServerStats.mof
install -D -m 0444  $PEGASUS_ROOT/Schemas/CIM28/Application28_SystemSoftware.mof $RPM_BUILD_ROOT%PEGASUS_MOF_DIR/CIM28/Application28_SystemSoftware.mof
install -D -m 0444  $PEGASUS_ROOT/Schemas/CIM28/CIM_Application28.mof $RPM_BUILD_ROOT%PEGASUS_MOF_DIR/CIM28/CIM_Application28.mof
install -D -m 0444  $PEGASUS_ROOT/Schemas/CIM28/CIM_Core28.mof $RPM_BUILD_ROOT%PEGASUS_MOF_DIR/CIM28/CIM_Core28.mof
install -D -m 0444  $PEGASUS_ROOT/Schemas/CIM28/CIM_Database28.mof $RPM_BUILD_ROOT%PEGASUS_MOF_DIR/CIM28/CIM_Database28.mof
install -D -m 0444  $PEGASUS_ROOT/Schemas/CIM28/CIM_Device28.mof $RPM_BUILD_ROOT%PEGASUS_MOF_DIR/CIM28/CIM_Device28.mof
install -D -m 0444  $PEGASUS_ROOT/Schemas/CIM28/CIM_Event28.mof $RPM_BUILD_ROOT%PEGASUS_MOF_DIR/CIM28/CIM_Event28.mof
install -D -m 0444  $PEGASUS_ROOT/Schemas/CIM28/CIM_Interop28.mof $RPM_BUILD_ROOT%PEGASUS_MOF_DIR/CIM28/CIM_Interop28.mof 
install -D -m 0444  $PEGASUS_ROOT/Schemas/CIM28/CIM_IPsecPolicy28.mof $RPM_BUILD_ROOT%PEGASUS_MOF_DIR/CIM28/CIM_IPsecPolicy28.mof 
install -D -m 0444  $PEGASUS_ROOT/Schemas/CIM28/CIM_Metrics28.mof $RPM_BUILD_ROOT%PEGASUS_MOF_DIR/CIM28/CIM_Metrics28.mof
install -D -m 0444  $PEGASUS_ROOT/Schemas/CIM28/CIM_Network28.mof $RPM_BUILD_ROOT%PEGASUS_MOF_DIR/CIM28/CIM_Network28.mof
install -D -m 0444  $PEGASUS_ROOT/Schemas/CIM28/CIM_Physical28.mof $RPM_BUILD_ROOT%PEGASUS_MOF_DIR/CIM28/CIM_Physical28.mof
install -D -m 0444  $PEGASUS_ROOT/Schemas/CIM28/CIM_Policy28.mof $RPM_BUILD_ROOT%PEGASUS_MOF_DIR/CIM28/CIM_Policy28.mof
install -D -m 0444  $PEGASUS_ROOT/Schemas/CIM28/CIM_Schema28.mof $RPM_BUILD_ROOT%PEGASUS_MOF_DIR/CIM28/CIM_Schema28.mof
install -D -m 0444  $PEGASUS_ROOT/Schemas/CIM28/CIM_Support28.mof $RPM_BUILD_ROOT%PEGASUS_MOF_DIR/CIM28/CIM_Support28.mof
install -D -m 0444  $PEGASUS_ROOT/Schemas/CIM28/CIM_System28.mof $RPM_BUILD_ROOT%PEGASUS_MOF_DIR/CIM28/CIM_System28.mof
install -D -m 0444  $PEGASUS_ROOT/Schemas/CIM28/CIM_User28.mof $RPM_BUILD_ROOT%PEGASUS_MOF_DIR/CIM28/CIM_User28.mof
install -D -m 0444  $PEGASUS_ROOT/Schemas/CIM28/Core28_Capabilities.mof $RPM_BUILD_ROOT%PEGASUS_MOF_DIR/CIM28/Core28_Capabilities.mof
install -D -m 0444  $PEGASUS_ROOT/Schemas/CIM28/Core28_Collection.mof $RPM_BUILD_ROOT%PEGASUS_MOF_DIR/CIM28/Core28_Collection.mof
install -D -m 0444  $PEGASUS_ROOT/Schemas/CIM28/Core28_CoreElements.mof $RPM_BUILD_ROOT%PEGASUS_MOF_DIR/CIM28/Core28_CoreElements.mof
install -D -m 0444  $PEGASUS_ROOT/Schemas/CIM28/Core28_Device.mof $RPM_BUILD_ROOT%PEGASUS_MOF_DIR/CIM28/Core28_Device.mof
install -D -m 0444  $PEGASUS_ROOT/Schemas/CIM28/Core28_MethodParms.mof $RPM_BUILD_ROOT%PEGASUS_MOF_DIR/CIM28/Core28_MethodParms.mof
install -D -m 0444  $PEGASUS_ROOT/Schemas/CIM28/Core28_Physical.mof $RPM_BUILD_ROOT%PEGASUS_MOF_DIR/CIM28/Core28_Physical.mof
install -D -m 0444  $PEGASUS_ROOT/Schemas/CIM28/Core28_PowerMgmt.mof $RPM_BUILD_ROOT%PEGASUS_MOF_DIR/CIM28/Core28_PowerMgmt.mof
install -D -m 0444  $PEGASUS_ROOT/Schemas/CIM28/Core28_ProductFRU.mof $RPM_BUILD_ROOT%PEGASUS_MOF_DIR/CIM28/Core28_ProductFRU.mof
install -D -m 0444  $PEGASUS_ROOT/Schemas/CIM28/Core28_Qualifiers.mof $RPM_BUILD_ROOT%PEGASUS_MOF_DIR/CIM28/Core28_Qualifiers.mof
install -D -m 0444  $PEGASUS_ROOT/Schemas/CIM28/Core28_Redundancy.mof $RPM_BUILD_ROOT%PEGASUS_MOF_DIR/CIM28/Core28_Redundancy.mof
install -D -m 0444  $PEGASUS_ROOT/Schemas/CIM28/Core28_Service.mof $RPM_BUILD_ROOT%PEGASUS_MOF_DIR/CIM28/Core28_Service.mof
install -D -m 0444  $PEGASUS_ROOT/Schemas/CIM28/Core28_Settings.mof $RPM_BUILD_ROOT%PEGASUS_MOF_DIR/CIM28/Core28_Settings.mof
install -D -m 0444  $PEGASUS_ROOT/Schemas/CIM28/Core28_Software.mof $RPM_BUILD_ROOT%PEGASUS_MOF_DIR/CIM28/Core28_Software.mof
install -D -m 0444  $PEGASUS_ROOT/Schemas/CIM28/Core28_Statistics.mof $RPM_BUILD_ROOT%PEGASUS_MOF_DIR/CIM28/Core28_Statistics.mof
install -D -m 0444  $PEGASUS_ROOT/Schemas/CIM28/Core28_StorageExtent.mof $RPM_BUILD_ROOT%PEGASUS_MOF_DIR/CIM28/Core28_StorageExtent.mof
install -D -m 0444  $PEGASUS_ROOT/Schemas/CIM28/Database28_Statistics.mof $RPM_BUILD_ROOT%PEGASUS_MOF_DIR/CIM28/Database28_Statistics.mof
install -D -m 0444  $PEGASUS_ROOT/Schemas/CIM28/Database28_Storage.mof $RPM_BUILD_ROOT%PEGASUS_MOF_DIR/CIM28/Database28_Storage.mof
install -D -m 0444  $PEGASUS_ROOT/Schemas/CIM28/Database28_System.mof $RPM_BUILD_ROOT%PEGASUS_MOF_DIR/CIM28/Database28_System.mof
install -D -m 0444  $PEGASUS_ROOT/Schemas/CIM28/Device28_Controller.mof $RPM_BUILD_ROOT%PEGASUS_MOF_DIR/CIM28/Device28_Controller.mof
install -D -m 0444  $PEGASUS_ROOT/Schemas/CIM28/Device28_CoolingAndPower.mof $RPM_BUILD_ROOT%PEGASUS_MOF_DIR/CIM28/Device28_CoolingAndPower.mof
install -D -m 0444  $PEGASUS_ROOT/Schemas/CIM28/Device28_DeviceElements.mof $RPM_BUILD_ROOT%PEGASUS_MOF_DIR/CIM28/Device28_DeviceElements.mof
install -D -m 0444  $PEGASUS_ROOT/Schemas/CIM28/Device28_DiskGroup.mof $RPM_BUILD_ROOT%PEGASUS_MOF_DIR/CIM28/Device28_DiskGroup.mof
install -D -m 0444  $PEGASUS_ROOT/Schemas/CIM28/Device28_FC.mof $RPM_BUILD_ROOT%PEGASUS_MOF_DIR/CIM28/Device28_FC.mof
install -D -m 0444  $PEGASUS_ROOT/Schemas/CIM28/Device28_IB.mof $RPM_BUILD_ROOT%PEGASUS_MOF_DIR/CIM28/Device28_IB.mof
install -D -m 0444  $PEGASUS_ROOT/Schemas/CIM28/Device28_Memory.mof $RPM_BUILD_ROOT%PEGASUS_MOF_DIR/CIM28/Device28_Memory.mof
install -D -m 0444  $PEGASUS_ROOT/Schemas/CIM28/Device28_Modems.mof $RPM_BUILD_ROOT%PEGASUS_MOF_DIR/CIM28/Device28_Modems.mof
install -D -m 0444  $PEGASUS_ROOT/Schemas/CIM28/Device28_NetworkAdapter.mof $RPM_BUILD_ROOT%PEGASUS_MOF_DIR/CIM28/Device28_NetworkAdapter.mof
install -D -m 0444  $PEGASUS_ROOT/Schemas/CIM28/Device28_Ports.mof $RPM_BUILD_ROOT%PEGASUS_MOF_DIR/CIM28/Device28_Ports.mof
install -D -m 0444  $PEGASUS_ROOT/Schemas/CIM28/Device28_Printing.mof $RPM_BUILD_ROOT%PEGASUS_MOF_DIR/CIM28/Device28_Printing.mof
install -D -m 0444  $PEGASUS_ROOT/Schemas/CIM28/Device28_Processor.mof $RPM_BUILD_ROOT%PEGASUS_MOF_DIR/CIM28/Device28_Processor.mof
install -D -m 0444  $PEGASUS_ROOT/Schemas/CIM28/Device28_ProtocolController.mof $RPM_BUILD_ROOT%PEGASUS_MOF_DIR/CIM28/Device28_ProtocolController.mof
install -D -m 0444  $PEGASUS_ROOT/Schemas/CIM28/Device28_SccExtents.mof $RPM_BUILD_ROOT%PEGASUS_MOF_DIR/CIM28/Device28_SccExtents.mof
install -D -m 0444  $PEGASUS_ROOT/Schemas/CIM28/Device28_Sensors.mof $RPM_BUILD_ROOT%PEGASUS_MOF_DIR/CIM28/Device28_Sensors.mof
install -D -m 0444  $PEGASUS_ROOT/Schemas/CIM28/Device28_StorageDevices.mof $RPM_BUILD_ROOT%PEGASUS_MOF_DIR/CIM28/Device28_StorageDevices.mof
install -D -m 0444  $PEGASUS_ROOT/Schemas/CIM28/Device28_StorageExtents.mof $RPM_BUILD_ROOT%PEGASUS_MOF_DIR/CIM28/Device28_StorageExtents.mof
install -D -m 0444  $PEGASUS_ROOT/Schemas/CIM28/Device28_StorageLib.mof $RPM_BUILD_ROOT%PEGASUS_MOF_DIR/CIM28/Device28_StorageLib.mof
install -D -m 0444  $PEGASUS_ROOT/Schemas/CIM28/Device28_StorageServices.mof $RPM_BUILD_ROOT%PEGASUS_MOF_DIR/CIM28/Device28_StorageServices.mof
install -D -m 0444  $PEGASUS_ROOT/Schemas/CIM28/Device28_USB.mof $RPM_BUILD_ROOT%PEGASUS_MOF_DIR/CIM28/Device28_USB.mof
install -D -m 0444  $PEGASUS_ROOT/Schemas/CIM28/Device28_UserDevices.mof $RPM_BUILD_ROOT%PEGASUS_MOF_DIR/CIM28/Device28_UserDevices.mof
install -D -m 0444  $PEGASUS_ROOT/Schemas/CIM28/Metrics28_BaseMetric.mof $RPM_BUILD_ROOT%PEGASUS_MOF_DIR/CIM28/Metrics28_BaseMetric.mof
install -D -m 0444  $PEGASUS_ROOT/Schemas/CIM28/Metrics28_UnitOfWork.mof $RPM_BUILD_ROOT%PEGASUS_MOF_DIR/CIM28/Metrics28_UnitOfWork.mof
install -D -m 0444  $PEGASUS_ROOT/Schemas/CIM28/Network28_BGP.mof $RPM_BUILD_ROOT%PEGASUS_MOF_DIR/CIM28/Network28_BGP.mof
install -D -m 0444  $PEGASUS_ROOT/Schemas/CIM28/Network28_Buffers.mof $RPM_BUILD_ROOT%PEGASUS_MOF_DIR/CIM28/Network28_Buffers.mof
install -D -m 0444  $PEGASUS_ROOT/Schemas/CIM28/Network28_Collections.mof $RPM_BUILD_ROOT%PEGASUS_MOF_DIR/CIM28/Network28_Collections.mof
install -D -m 0444  $PEGASUS_ROOT/Schemas/CIM28/Network28_Filtering.mof $RPM_BUILD_ROOT%PEGASUS_MOF_DIR/CIM28/Network28_Filtering.mof
install -D -m 0444  $PEGASUS_ROOT/Schemas/CIM28/Network28_IPsec.mof $RPM_BUILD_ROOT%PEGASUS_MOF_DIR/CIM28/Network28_IPsec.mof
install -D -m 0444  $PEGASUS_ROOT/Schemas/CIM28/Network28_OSPF.mof $RPM_BUILD_ROOT%PEGASUS_MOF_DIR/CIM28/Network28_OSPF.mof
install -D -m 0444  $PEGASUS_ROOT/Schemas/CIM28/Network28_Pipes.mof $RPM_BUILD_ROOT%PEGASUS_MOF_DIR/CIM28/Network28_Pipes.mof
install -D -m 0444  $PEGASUS_ROOT/Schemas/CIM28/Network28_ProtocolEndpoints.mof $RPM_BUILD_ROOT%PEGASUS_MOF_DIR/CIM28/Network28_ProtocolEndpoints.mof
install -D -m 0444  $PEGASUS_ROOT/Schemas/CIM28/Network28_QoS.mof $RPM_BUILD_ROOT%PEGASUS_MOF_DIR/CIM28/Network28_QoS.mof
install -D -m 0444  $PEGASUS_ROOT/Schemas/CIM28/Network28_Routes.mof $RPM_BUILD_ROOT%PEGASUS_MOF_DIR/CIM28/Network28_Routes.mof
install -D -m 0444  $PEGASUS_ROOT/Schemas/CIM28/Network28_RoutingForwarding.mof $RPM_BUILD_ROOT%PEGASUS_MOF_DIR/CIM28/Network28_RoutingForwarding.mof
install -D -m 0444  $PEGASUS_ROOT/Schemas/CIM28/Network28_SNMP.mof $RPM_BUILD_ROOT%PEGASUS_MOF_DIR/CIM28/Network28_SNMP.mof
install -D -m 0444  $PEGASUS_ROOT/Schemas/CIM28/Network28_SwitchingBridging.mof $RPM_BUILD_ROOT%PEGASUS_MOF_DIR/CIM28/Network28_SwitchingBridging.mof
install -D -m 0444  $PEGASUS_ROOT/Schemas/CIM28/Network28_Systems.mof $RPM_BUILD_ROOT%PEGASUS_MOF_DIR/CIM28/Network28_Systems.mof
install -D -m 0444  $PEGASUS_ROOT/Schemas/CIM28/Network28_VLAN.mof $RPM_BUILD_ROOT%PEGASUS_MOF_DIR/CIM28/Network28_VLAN.mof
install -D -m 0444  $PEGASUS_ROOT/Schemas/CIM28/Physical28_Component.mof $RPM_BUILD_ROOT%PEGASUS_MOF_DIR/CIM28/Physical28_Component.mof
install -D -m 0444  $PEGASUS_ROOT/Schemas/CIM28/Physical28_Connector.mof $RPM_BUILD_ROOT%PEGASUS_MOF_DIR/CIM28/Physical28_Connector.mof
install -D -m 0444  $PEGASUS_ROOT/Schemas/CIM28/Physical28_Link.mof $RPM_BUILD_ROOT%PEGASUS_MOF_DIR/CIM28/Physical28_Link.mof
install -D -m 0444  $PEGASUS_ROOT/Schemas/CIM28/Physical28_Misc.mof $RPM_BUILD_ROOT%PEGASUS_MOF_DIR/CIM28/Physical28_Misc.mof
install -D -m 0444  $PEGASUS_ROOT/Schemas/CIM28/Physical28_Package.mof $RPM_BUILD_ROOT%PEGASUS_MOF_DIR/CIM28/Physical28_Package.mof
install -D -m 0444  $PEGASUS_ROOT/Schemas/CIM28/Physical28_PhysicalMedia.mof $RPM_BUILD_ROOT%PEGASUS_MOF_DIR/CIM28/Physical28_PhysicalMedia.mof
install -D -m 0444  $PEGASUS_ROOT/Schemas/CIM28/Physical28_StoragePackage.mof $RPM_BUILD_ROOT%PEGASUS_MOF_DIR/CIM28/Physical28_StoragePackage.mof
install -D -m 0444  $PEGASUS_ROOT/Schemas/CIM28/System28_Boot.mof $RPM_BUILD_ROOT%PEGASUS_MOF_DIR/CIM28/System28_Boot.mof
install -D -m 0444  $PEGASUS_ROOT/Schemas/CIM28/System28_Diagnostics.mof $RPM_BUILD_ROOT%PEGASUS_MOF_DIR/CIM28/System28_Diagnostics.mof
install -D -m 0444  $PEGASUS_ROOT/Schemas/CIM28/System28_FileElements.mof $RPM_BUILD_ROOT%PEGASUS_MOF_DIR/CIM28/System28_FileElements.mof
install -D -m 0444  $PEGASUS_ROOT/Schemas/CIM28/System28_Logs.mof $RPM_BUILD_ROOT%PEGASUS_MOF_DIR/CIM28/System28_Logs.mof
install -D -m 0444  $PEGASUS_ROOT/Schemas/CIM28/System28_OperatingSystem.mof $RPM_BUILD_ROOT%PEGASUS_MOF_DIR/CIM28/System28_OperatingSystem.mof
install -D -m 0444  $PEGASUS_ROOT/Schemas/CIM28/System28_Processing.mof $RPM_BUILD_ROOT%PEGASUS_MOF_DIR/CIM28/System28_Processing.mof
install -D -m 0444  $PEGASUS_ROOT/Schemas/CIM28/System28_SystemElements.mof $RPM_BUILD_ROOT%PEGASUS_MOF_DIR/CIM28/System28_SystemElements.mof
install -D -m 0444  $PEGASUS_ROOT/Schemas/CIM28/System28_SystemResources.mof $RPM_BUILD_ROOT%PEGASUS_MOF_DIR/CIM28/System28_SystemResources.mof
install -D -m 0444  $PEGASUS_ROOT/Schemas/CIM28/System28_Time.mof $RPM_BUILD_ROOT%PEGASUS_MOF_DIR/CIM28/System28_Time.mof
install -D -m 0444  $PEGASUS_ROOT/Schemas/CIM28/System28_Unix.mof $RPM_BUILD_ROOT%PEGASUS_MOF_DIR/CIM28/System28_Unix.mof
install -D -m 0444  $PEGASUS_ROOT/Schemas/CIM28/User28_AccessControl.mof $RPM_BUILD_ROOT%PEGASUS_MOF_DIR/CIM28/User28_AccessControl.mof
install -D -m 0444  $PEGASUS_ROOT/Schemas/CIM28/User28_Account.mof $RPM_BUILD_ROOT%PEGASUS_MOF_DIR/CIM28/User28_Account.mof
install -D -m 0444  $PEGASUS_ROOT/Schemas/CIM28/User28_AuthenticationReqmt.mof $RPM_BUILD_ROOT%PEGASUS_MOF_DIR/CIM28/User28_AuthenticationReqmt.mof
install -D -m 0444  $PEGASUS_ROOT/Schemas/CIM28/User28_Credential.mof $RPM_BUILD_ROOT%PEGASUS_MOF_DIR/CIM28/User28_Credential.mof
install -D -m 0444  $PEGASUS_ROOT/Schemas/CIM28/User28_Group.mof $RPM_BUILD_ROOT%PEGASUS_MOF_DIR/CIM28/User28_Group.mof
install -D -m 0444  $PEGASUS_ROOT/Schemas/CIM28/User28_Identity.mof $RPM_BUILD_ROOT%PEGASUS_MOF_DIR/CIM28/User28_Identity.mof
install -D -m 0444  $PEGASUS_ROOT/Schemas/CIM28/User28_Kerberos.mof $RPM_BUILD_ROOT%PEGASUS_MOF_DIR/CIM28/User28_Kerberos.mof
install -D -m 0444  $PEGASUS_ROOT/Schemas/CIM28/User28_Org.mof $RPM_BUILD_ROOT%PEGASUS_MOF_DIR/CIM28/User28_Org.mof
install -D -m 0444  $PEGASUS_ROOT/Schemas/CIM28/User28_PrivilegeManagementService.mof $RPM_BUILD_ROOT%PEGASUS_MOF_DIR/CIM28/User28_PrivilegeManagementService.mof
install -D -m 0444  $PEGASUS_ROOT/Schemas/CIM28/User28_Privilege.mof $RPM_BUILD_ROOT%PEGASUS_MOF_DIR/CIM28/User28_Privilege.mof
install -D -m 0444  $PEGASUS_ROOT/Schemas/CIM28/User28_PublicKey.mof $RPM_BUILD_ROOT%PEGASUS_MOF_DIR/CIM28/User28_PublicKey.mof
install -D -m 0444  $PEGASUS_ROOT/Schemas/CIM28/User28_Role.mof $RPM_BUILD_ROOT%PEGASUS_MOF_DIR/CIM28/User28_Role.mof
install -D -m 0444  $PEGASUS_ROOT/Schemas/CIM28/User28_SecurityLevel.mof $RPM_BUILD_ROOT%PEGASUS_MOF_DIR/CIM28/User28_SecurityLevel.mof
install -D -m 0444  $PEGASUS_ROOT/Schemas/CIM28/User28_SecurityServices.mof $RPM_BUILD_ROOT%PEGASUS_MOF_DIR/CIM28/User28_SecurityServices.mof
install -D -m 0444  $PEGASUS_ROOT/Schemas/CIM28/User28_SharedSecret.mof $RPM_BUILD_ROOT%PEGASUS_MOF_DIR/CIM28/User28_SharedSecret.mof
install -D -m 0444  $PEGASUS_ROOT/Schemas/CIM28/User28_StorageHardwareID.mof $RPM_BUILD_ROOT%PEGASUS_MOF_DIR/CIM28/User28_StorageHardwareID.mof
install -D -m 0444  $PEGASUS_ROOT/Schemas/CIM28/User28_UsersAccess.mof $RPM_BUILD_ROOT%PEGASUS_MOF_DIR/CIM28/User28_UsersAccess.mof

#
# Pegasus' schema
#

# Internal:
install -D -m 0444  $PEGASUS_ROOT/Schemas/Pegasus/Internal/VER20/PG_Authorization20.mof  $RPM_BUILD_ROOT%PEGASUS_MOF_DIR/Pegasus/PG_Authorization20.mof
install -D -m 0444  $PEGASUS_ROOT/Schemas/Pegasus/Internal/VER20/PG_ConfigSetting20.mof  $RPM_BUILD_ROOT%PEGASUS_MOF_DIR/Pegasus/PG_ConfigSetting20.mof
install -D -m 0444  $PEGASUS_ROOT/Schemas/Pegasus/Internal/VER20/PG_InternalSchema20.mof $RPM_BUILD_ROOT%PEGASUS_MOF_DIR/Pegasus/PG_InternalSchema20.mof
install -D -m 0444  $PEGASUS_ROOT/Schemas/Pegasus/Internal/VER20/PG_ShutdownService20.mof           $RPM_BUILD_ROOT%PEGASUS_MOF_DIR/Pegasus/PG_ShutdownService20.mof
install -D -m 0444  $PEGASUS_ROOT/Schemas/Pegasus/Internal/VER20/PG_User20.mof           $RPM_BUILD_ROOT%PEGASUS_MOF_DIR/Pegasus/PG_User20.mof

# InterOp:
install -D -m 0444  $PEGASUS_ROOT/Schemas/Pegasus/InterOp/VER20/PG_Events20.mof   $RPM_BUILD_ROOT%PEGASUS_MOF_DIR/Pegasus/PG_Events20.mof
install -D -m 0444  $PEGASUS_ROOT/Schemas/Pegasus/InterOp/VER20/PG_InterOpSchema20.mof   $RPM_BUILD_ROOT%PEGASUS_MOF_DIR/Pegasus/PG_InterOpSchema20.mof
install -D -m 0444  $PEGASUS_ROOT/Schemas/Pegasus/InterOp/VER20/PG_ProviderModule20.mof  $RPM_BUILD_ROOT%PEGASUS_MOF_DIR/Pegasus/PG_ProviderModule20.mof

#
# Pegasus Default Providers' schema
#
install -D -m 0444  $PEGASUS_ROOT/Schemas/Pegasus/ManagedSystem/VER20/PG_ComputerSystem20.mof                               $RPM_BUILD_ROOT%PEGASUS_MOF_DIR/Pegasus/PG_ComputerSystem20.mof
install -D -m 0444  $PEGASUS_ROOT/Schemas/Pegasus/ManagedSystem/VER20/PG_ComputerSystem20R.mof                               $RPM_BUILD_ROOT%PEGASUS_MOF_DIR/Pegasus/PG_ComputerSystem20R.mof
install -D -m 0444  $PEGASUS_ROOT/Schemas/Pegasus/ManagedSystem/VER20/PG_ManagedSystemSchema20.mof                               $RPM_BUILD_ROOT%PEGASUS_MOF_DIR/Pegasus/PG_ManagedSystemSchema20.mof
install -D -m 0444  $PEGASUS_ROOT/Schemas/Pegasus/ManagedSystem/VER20/PG_ManagedSystemSchema20R.mof                               $RPM_BUILD_ROOT%PEGASUS_MOF_DIR/Pegasus/PG_ManagedSystemSchema20R.mof
install -D -m 0444  $PEGASUS_ROOT/Schemas/Pegasus/ManagedSystem/VER20/PG_OperatingSystem20.mof                               $RPM_BUILD_ROOT%PEGASUS_MOF_DIR/Pegasus/PG_OperatingSystem20.mof
install -D -m 0444  $PEGASUS_ROOT/Schemas/Pegasus/ManagedSystem/VER20/PG_OperatingSystem20R.mof                               $RPM_BUILD_ROOT%PEGASUS_MOF_DIR/Pegasus/PG_OperatingSystem20R.mof
install -D -m 0444  $PEGASUS_ROOT/Schemas/Pegasus/ManagedSystem/VER20/PG_UnixProcess20.mof                               $RPM_BUILD_ROOT%PEGASUS_MOF_DIR/Pegasus/PG_UnixProcess20.mof
install -D -m 0444  $PEGASUS_ROOT/Schemas/Pegasus/ManagedSystem/VER20/PG_UnixProcess20R.mof                               $RPM_BUILD_ROOT%PEGASUS_MOF_DIR/Pegasus/PG_UnixProcess20R.mof
#
# Initial Repository
#
mkdir -p  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR
cp -rf $PEGASUS_HOME/repository/*  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR
#
# SSL Files
#
install -D -m 0444  $PEGASUS_ROOT/src/Server/ssl.cnf $RPM_BUILD_ROOT%PEGASUS_CONFIG_DIR/ssl.orig

#
# cimserver config files
#
install -D -m 0644  $PEGASUS_ROOT/src/Server/%PEGASUS_PLANNED_CONFIG_FILE $RPM_BUILD_ROOT%PEGASUS_VARDATA_DIR/%PEGASUS_PLANNED_CONFIG_FILE

#
# WBEM pam authentication
#
install -D -m 0644  $PEGASUS_ROOT/rpm/wbem $RPM_BUILD_ROOT%PAM_CONF/wbem

#
# repository install script
#
echo "PEGASUS_MOF_DIR="%PEGASUS_MOF_DIR > initrepository.in
echo "PEGASUS_REPOSITORY_DIR="%PEGASUS_REPOSITORY_DIR >> initrepository.in
echo "PEGASUS_BIN_DIR="%PEGASUS_BIN_DIR >> initrepository.in
echo "PEGASUS_CIM_SCHEMA=28" >> initrepository.in
cat initrepository.in $PEGASUS_ROOT/installs/scripts/init_repository > $RPM_BUILD_ROOT%PEGASUS_SBIN_DIR/init_repository
chmod 0544 $RPM_BUILD_ROOT%PEGASUS_SBIN_DIR/init_repository

#
# man pages
#
install -D -m 0444  $PEGASUS_ROOT/rpm/manLinux/man1.Z/cimmof.1 $RPM_BUILD_ROOT%PEGASUS_MANUSER_DIR/cimmof.1
install -D -m 0444  $PEGASUS_ROOT/rpm/manLinux/man1.Z/cimprovider.1 $RPM_BUILD_ROOT%PEGASUS_MANUSER_DIR/cimprovider.1
install -D -m 0444  $PEGASUS_ROOT/rpm/manLinux/man1.Z/osinfo.1 $RPM_BUILD_ROOT%PEGASUS_MANUSER_DIR/osinfo.1
install -D -m 0444  $PEGASUS_ROOT/rpm/manLinux/man1.Z/wbemexec.1 $RPM_BUILD_ROOT%PEGASUS_MANUSER_DIR/wbemexec.1
install -D -m 0444  $PEGASUS_ROOT/rpm/manLinux/man1m.Z/cimauth.1m $RPM_BUILD_ROOT%PEGASUS_MANADMIN_DIR/cimauth.8
install -D -m 0444  $PEGASUS_ROOT/rpm/manLinux/man1m.Z/cimconfig.1m $RPM_BUILD_ROOT%PEGASUS_MANADMIN_DIR/cimconfig.8
install -D -m 0444  $PEGASUS_ROOT/rpm/manLinux/man1m.Z/cimserver.1m $RPM_BUILD_ROOT%PEGASUS_MANADMIN_DIR/cimserver.8

install -D -m 0444  $PEGASUS_ROOT/doc/%PEGASUS_LICENSE_FILE $RPM_BUILD_ROOT%PEGASUS_PROD_DIR/%PEGASUS_LICENSE_FILE
#
# SDK
#

%define PEGASUS_INCLUDE_DEST_PATH /opt/tog-pegasus/include
%define PEGASUS_HTML_DEST_PATH    /opt/tog-pegasus/share/html
%define PEGASUS_SAMPLES_DEST_PATH /opt/tog-pegasus/share/samples
%define INCLUDE_DEST_PATH    $RPM_BUILD_ROOT%PEGASUS_INCLUDE_DEST_PATH
%define HTML_DEST_PATH       $RPM_BUILD_ROOT%PEGASUS_HTML_DEST_PATH
%define SAMPLES_DEST_PATH     $RPM_BUILD_ROOT%PEGASUS_SAMPLES_DEST_PATH
%define SDK_STAGE_LOC        $PEGASUS_HOME/stageSDK
mkdir -p %INCLUDE_DEST_PATH/{Client,Common,Consumer,Handler,Provider}
mkdir -p %INCLUDE_DEST_PATH/Provider/CMPI
mkdir -p %HTML_DEST_PATH
mkdir -p %SAMPLES_DEST_PATH/{Clients,mak,Providers}
mkdir -p %SAMPLES_DEST_PATH/Providers/Load
install -D -m 0444 %SDK_STAGE_LOC/include/Pegasus/Client/CIMClientException.h %INCLUDE_DEST_PATH/Pegasus/Client/CIMClientException.h
install -D -m 0444 %SDK_STAGE_LOC/include/Pegasus/Client/CIMClient.h %INCLUDE_DEST_PATH/Pegasus/Client/CIMClient.h
install -D -m 0444 %SDK_STAGE_LOC/include/Pegasus/Client/Linkage.h %INCLUDE_DEST_PATH/Pegasus/Client/Linkage.h
install -D -m 0444 %SDK_STAGE_LOC/include/Pegasus/Common/AcceptLanguages.h %INCLUDE_DEST_PATH/Pegasus/Common/AcceptLanguages.h
install -D -m 0444 %SDK_STAGE_LOC/include/Pegasus/Common/AcceptLanguageElement.h %INCLUDE_DEST_PATH/Pegasus/Common/AcceptLanguageElement.h
install -D -m 0444 %SDK_STAGE_LOC/include/Pegasus/Common/Array.h %INCLUDE_DEST_PATH/Pegasus/Common/Array.h
install -D -m 0444 %SDK_STAGE_LOC/include/Pegasus/Common/ArrayInter.h %INCLUDE_DEST_PATH/Pegasus/Common/ArrayInter.h
install -D -m 0444 %SDK_STAGE_LOC/include/Pegasus/Common/Char16.h %INCLUDE_DEST_PATH/Pegasus/Common/Char16.h
install -D -m 0444 %SDK_STAGE_LOC/include/Pegasus/Common/CIMClass.h %INCLUDE_DEST_PATH/Pegasus/Common/CIMClass.h 
install -D -m 0444 %SDK_STAGE_LOC/include/Pegasus/Common/CIMDateTime.h %INCLUDE_DEST_PATH/Pegasus/Common/CIMDateTime.h
install -D -m 0444 %SDK_STAGE_LOC/include/Pegasus/Common/CIMFlavor.h %INCLUDE_DEST_PATH/Pegasus/Common/CIMFlavor.h
install -D -m 0444 %SDK_STAGE_LOC/include/Pegasus/Common/CIMIndication.h %INCLUDE_DEST_PATH/Pegasus/Common/CIMIndication.h 
install -D -m 0444 %SDK_STAGE_LOC/include/Pegasus/Common/CIMInstance.h %INCLUDE_DEST_PATH/Pegasus/Common/CIMInstance.h
install -D -m 0444 %SDK_STAGE_LOC/include/Pegasus/Common/CIMMethod.h %INCLUDE_DEST_PATH/Pegasus/Common//CIMMethod.h
install -D -m 0444 %SDK_STAGE_LOC/include/Pegasus/Common/CIMName.h %INCLUDE_DEST_PATH/Pegasus/Common/CIMName.h
install -D -m 0444 %SDK_STAGE_LOC/include/Pegasus/Common/CIMObject.h %INCLUDE_DEST_PATH/Pegasus/Common/CIMObject.h
install -D -m 0444 %SDK_STAGE_LOC/include/Pegasus/Common/CIMObjectPath.h %INCLUDE_DEST_PATH/Pegasus/Common/CIMObjectPath.h
install -D -m 0444 %SDK_STAGE_LOC/include/Pegasus/Common/CIMParameter.h %INCLUDE_DEST_PATH/Pegasus/Common/CIMParameter.h
install -D -m 0444 %SDK_STAGE_LOC/include/Pegasus/Common/CIMParamValue.h %INCLUDE_DEST_PATH/Pegasus/Common/CIMParamValue.h
install -D -m 0444 %SDK_STAGE_LOC/include/Pegasus/Common/CIMProperty.h %INCLUDE_DEST_PATH/Pegasus/Common/CIMProperty.h
install -D -m 0444 %SDK_STAGE_LOC/include/Pegasus/Common/CIMPropertyList.h %INCLUDE_DEST_PATH/Pegasus/Common/CIMPropertyList.h
install -D -m 0444 %SDK_STAGE_LOC/include/Pegasus/Common/CIMQualifierDecl.h %INCLUDE_DEST_PATH/Pegasus/Common/CIMQualifierDecl.h
install -D -m 0444 %SDK_STAGE_LOC/include/Pegasus/Common/CIMQualifier.h %INCLUDE_DEST_PATH/Pegasus/Common/CIMQualifier.h
install -D -m 0444 %SDK_STAGE_LOC/include/Pegasus/Common/CIMScope.h %INCLUDE_DEST_PATH/Pegasus/Common/CIMScope.h
install -D -m 0444 %SDK_STAGE_LOC/include/Pegasus/Common/CIMStatusCode.h %INCLUDE_DEST_PATH/Pegasus/Common/CIMStatusCode.h
install -D -m 0444 %SDK_STAGE_LOC/include/Pegasus/Common/CIMType.h %INCLUDE_DEST_PATH/Pegasus/Common/CIMType.h
install -D -m 0444 %SDK_STAGE_LOC/include/Pegasus/Common/CIMValue.h %INCLUDE_DEST_PATH/Pegasus/Common/CIMValue.h
install -D -m 0444 %SDK_STAGE_LOC/include/Pegasus/Common/Config.h %INCLUDE_DEST_PATH/Pegasus/Common/Config.h
install -D -m 0444 %SDK_STAGE_LOC/include/Pegasus/Common/ContentLanguageElement.h %INCLUDE_DEST_PATH/Pegasus/Common/ContentLanguageElement.h
install -D -m 0444 %SDK_STAGE_LOC/include/Pegasus/Common/ContentLanguages.h %INCLUDE_DEST_PATH/Pegasus/Common/ContentLanguages.h
install -D -m 0444 %SDK_STAGE_LOC/include/Pegasus/Common/Exception.h %INCLUDE_DEST_PATH/Pegasus/Common/Exception.h
install -D -m 0444 %SDK_STAGE_LOC/include/Pegasus/Common/Formatter.h %INCLUDE_DEST_PATH/Pegasus/Common/Formatter.h
install -D -m 0444 %SDK_STAGE_LOC/include/Pegasus/Common/LanguageElementContainer.h %INCLUDE_DEST_PATH/Pegasus/Common/LanguageElementContainer.h
install -D -m 0444 %SDK_STAGE_LOC/include/Pegasus/Common/LanguageElement.h %INCLUDE_DEST_PATH/Pegasus/Common/LanguageElement.h
install -D -m 0444 %SDK_STAGE_LOC/include/Pegasus/Common/Linkage.h %INCLUDE_DEST_PATH/Pegasus/Common/Linkage.h
install -D -m 0444 %SDK_STAGE_LOC/include/Pegasus/Common/MessageLoader.h %INCLUDE_DEST_PATH/Pegasus/Common/MessageLoader.h
install -D -m 0444 %SDK_STAGE_LOC/include/Pegasus/Common/OperationContext.h %INCLUDE_DEST_PATH/Pegasus/Common/OperationContext.h
%ifarch ia64
install -D -m 0444 %SDK_STAGE_LOC/include/Pegasus/Common/Platform_LINUX_IA64_GNU.h %INCLUDE_DEST_PATH/Pegasus/Common/Platform_LINUX_IX86_GNU.h
%else
install -D -m 0444 %SDK_STAGE_LOC/include/Pegasus/Common/Platform_LINUX_IX86_GNU.h %INCLUDE_DEST_PATH/Pegasus/Common/Platform_LINUX_IX86_GNU.h
%endif
install -D -m 0444 %SDK_STAGE_LOC/include/Pegasus/Common/ResponseHandler.h %INCLUDE_DEST_PATH/Pegasus/Common/ResponseHandler.h
install -D -m 0444 %SDK_STAGE_LOC/include/Pegasus/Common/SSLContext.h %INCLUDE_DEST_PATH/Pegasus/Common/SSLContext.h
install -D -m 0444 %SDK_STAGE_LOC/include/Pegasus/Common/String.h %INCLUDE_DEST_PATH/Pegasus/Common/String.h
install -D -m 0444 %SDK_STAGE_LOC/include/Pegasus/Consumer/CIMIndicationConsumer.h %INCLUDE_DEST_PATH/Pegasus/Consumer/CIMIndicationConsumer.h
install -D -m 0444 %SDK_STAGE_LOC/include/Pegasus/Consumer/Linkage.h %INCLUDE_DEST_PATH/Pegasus/Consumer/Linkage.h
install -D -m 0444 %SDK_STAGE_LOC/include/Pegasus/Provider/CIMAssociationProvider.h %INCLUDE_DEST_PATH/Pegasus/Provider/CIMAssociationProvider.h
install -D -m 0444 %SDK_STAGE_LOC/include/Pegasus/Provider/CIMIndicationConsumerProvider.h %INCLUDE_DEST_PATH/Pegasus/Provider/CIMIndicationConsumerProvider.h
install -D -m 0444 %SDK_STAGE_LOC/include/Pegasus/Provider/CIMIndicationProvider.h %INCLUDE_DEST_PATH/Pegasus/Provider/CIMIndicationProvider.h
install -D -m 0444 %SDK_STAGE_LOC/include/Pegasus/Provider/CIMInstanceProvider.h %INCLUDE_DEST_PATH/Pegasus/Provider/CIMInstanceProvider.h
install -D -m 0444 %SDK_STAGE_LOC/include/Pegasus/Provider/CIMMethodProvider.h %INCLUDE_DEST_PATH/Pegasus/Provider/CIMMethodProvider.h
install -D -m 0444 %SDK_STAGE_LOC/include/Pegasus/Provider/CIMOMHandle.h %INCLUDE_DEST_PATH/Pegasus/Provider/CIMOMHandle.h
install -D -m 0444 %SDK_STAGE_LOC/include/Pegasus/Provider/CIMProvider.h %INCLUDE_DEST_PATH/Pegasus/Provider/CIMProvider.h
install -D -m 0444 %SDK_STAGE_LOC/include/Pegasus/Provider/Linkage.h %INCLUDE_DEST_PATH/Pegasus/Provider/Linkage.h
install -D -m 0444 %SDK_STAGE_LOC/include/Pegasus/Provider/ProviderException.h %INCLUDE_DEST_PATH/Pegasus/Provider/ProviderException.h
install -D -m 0444 %SDK_STAGE_LOC/include/Pegasus/Provider/CMPI/CmpiArgs.h %INCLUDE_DEST_PATH/Pegasus/Provider/CMPI/CmpiArgs.h
install -D -m 0444 %SDK_STAGE_LOC/include/Pegasus/Provider/CMPI/CmpiArray.h %INCLUDE_DEST_PATH/Pegasus/Provider/CMPI/CmpiArray.h
install -D -m 0444 %SDK_STAGE_LOC/include/Pegasus/Provider/CMPI/CmpiAssociationMI.h %INCLUDE_DEST_PATH/Pegasus/Provider/CMPI/CmpiAssociationMI.h
install -D -m 0444 %SDK_STAGE_LOC/include/Pegasus/Provider/CMPI/CmpiBaseMI.h %INCLUDE_DEST_PATH/Pegasus/Provider/CMPI/CmpiBaseMI.h
install -D -m 0444 %SDK_STAGE_LOC/include/Pegasus/Provider/CMPI/CmpiBooleanData.h %INCLUDE_DEST_PATH/Pegasus/Provider/CMPI/CmpiBooleanData.h
install -D -m 0444 %SDK_STAGE_LOC/include/Pegasus/Provider/CMPI/CmpiBroker.h %INCLUDE_DEST_PATH/Pegasus/Provider/CMPI/CmpiBroker.h
install -D -m 0444 %SDK_STAGE_LOC/include/Pegasus/Provider/CMPI/CmpiCharData.h %INCLUDE_DEST_PATH/Pegasus/Provider/CMPI/CmpiCharData.h
install -D -m 0444 %SDK_STAGE_LOC/include/Pegasus/Provider/CMPI/CmpiContext.h %INCLUDE_DEST_PATH/Pegasus/Provider/CMPI/CmpiContext.h
install -D -m 0444 %SDK_STAGE_LOC/include/Pegasus/Provider/CMPI/CmpiData.h %INCLUDE_DEST_PATH/Pegasus/Provider/CMPI/CmpiData.h
install -D -m 0444 %SDK_STAGE_LOC/include/Pegasus/Provider/CMPI/CmpiDateTime.h %INCLUDE_DEST_PATH/Pegasus/Provider/CMPI/CmpiDateTime.h
install -D -m 0444 %SDK_STAGE_LOC/include/Pegasus/Provider/CMPI/cmpidt.h %INCLUDE_DEST_PATH/Pegasus/Provider/CMPI/cmpidt.h
install -D -m 0444 %SDK_STAGE_LOC/include/Pegasus/Provider/CMPI/CmpiEnumeration.h %INCLUDE_DEST_PATH/Pegasus/Provider/CMPI/CmpiEnumeration.h
install -D -m 0444 %SDK_STAGE_LOC/include/Pegasus/Provider/CMPI/cmpift.h %INCLUDE_DEST_PATH/Pegasus/Provider/CMPI/cmpift.h
install -D -m 0444 %SDK_STAGE_LOC/include/Pegasus/Provider/CMPI/CmpiIndicationMI.h %INCLUDE_DEST_PATH/Pegasus/Provider/CMPI/CmpiIndicationMI.h
install -D -m 0444 %SDK_STAGE_LOC/include/Pegasus/Provider/CMPI/CmpiInstance.h %INCLUDE_DEST_PATH/Pegasus/Provider/CMPI/CmpiInstance.h
install -D -m 0444 %SDK_STAGE_LOC/include/Pegasus/Provider/CMPI/CmpiInstanceMI.h %INCLUDE_DEST_PATH/Pegasus/Provider/CMPI/CmpiInstanceMI.h
install -D -m 0444 %SDK_STAGE_LOC/include/Pegasus/Provider/CMPI/cmpimacs.h %INCLUDE_DEST_PATH/Pegasus/Provider/CMPI/cmpimacs.h
install -D -m 0444 %SDK_STAGE_LOC/include/Pegasus/Provider/CMPI/CmpiMethodMI.h %INCLUDE_DEST_PATH/Pegasus/Provider/CMPI/CmpiMethodMI.h
install -D -m 0444 %SDK_STAGE_LOC/include/Pegasus/Provider/CMPI/CmpiObject.h %INCLUDE_DEST_PATH/Pegasus/Provider/CMPI/CmpiObject.h
install -D -m 0444 %SDK_STAGE_LOC/include/Pegasus/Provider/CMPI/CmpiObjectPath.h %INCLUDE_DEST_PATH/Pegasus/Provider/CMPI/CmpiObjectPath.h
install -D -m 0444 $PEGASUS_ROOT/src/Pegasus/Provider/CMPI/cmpipl.h %INCLUDE_DEST_PATH/Pegasus/Provider/CMPI/cmpipl.h
install -D -m 0444 %SDK_STAGE_LOC/include/Pegasus/Provider/CMPI/CmpiPropertyMI.h %INCLUDE_DEST_PATH/Pegasus/Provider/CMPI/CmpiPropertyMI.h
install -D -m 0444 %SDK_STAGE_LOC/include/Pegasus/Provider/CMPI/CmpiProviderBase.h %INCLUDE_DEST_PATH/Pegasus/Provider/CMPI/CmpiProviderBase.h
install -D -m 0444 %SDK_STAGE_LOC/include/Pegasus/Provider/CMPI/CmpiResult.h %INCLUDE_DEST_PATH/Pegasus/Provider/CMPI/CmpiResult.h
install -D -m 0444 %SDK_STAGE_LOC/include/Pegasus/Provider/CMPI/CmpiSelectExp.h %INCLUDE_DEST_PATH/Pegasus/Provider/CMPI/CmpiSelectExp.h
install -D -m 0444 %SDK_STAGE_LOC/include/Pegasus/Provider/CMPI/CmpiStatus.h %INCLUDE_DEST_PATH/Pegasus/Provider/CMPI/CmpiStatus.h
install -D -m 0444 %SDK_STAGE_LOC/include/Pegasus/Provider/CMPI/CmpiString.h %INCLUDE_DEST_PATH/Pegasus/Provider/CMPI/CmpiString.h
install -D -m 0444 %SDK_STAGE_LOC/html/AcceptLanguageElement.html %HTML_DEST_PATH/AcceptLanguageElement.html
install -D -m 0444 %SDK_STAGE_LOC/html/AcceptLanguages.html %HTML_DEST_PATH/AcceptLanguages.html
install -D -m 0444 %SDK_STAGE_LOC/html/AlreadyConnectedException.html %HTML_DEST_PATH/AlreadyConnectedException.html
install -D -m 0444 %SDK_STAGE_LOC/html/AlreadyExistsException.html %HTML_DEST_PATH/AlreadyExistsException.html
install -D -m 0444 %SDK_STAGE_LOC/html/ArrayPEGASUS_ARRAY_T.html %HTML_DEST_PATH/ArrayPEGASUS_ARRAY_T.html
install -D -m 0444 %SDK_STAGE_LOC/html/BindFailedException.html %HTML_DEST_PATH/BindFailedException.html
install -D -m 0444 %SDK_STAGE_LOC/html/CannotConnectException.html %HTML_DEST_PATH/CannotConnectException.html
install -D -m 0444 %SDK_STAGE_LOC/html/CannotCreateSocketException.html %HTML_DEST_PATH/CannotCreateSocketException.html
install -D -m 0444 %SDK_STAGE_LOC/html/CBAssociatorNames.html %HTML_DEST_PATH/CBAssociatorNames.html
install -D -m 0444 %SDK_STAGE_LOC/html/CBAssociators.html %HTML_DEST_PATH/CBAssociators.html
install -D -m 0444 %SDK_STAGE_LOC/html/CBAttachThread.html %HTML_DEST_PATH/CBAttachThread.html
install -D -m 0444 %SDK_STAGE_LOC/html/CBbrokerName.html %HTML_DEST_PATH/CBbrokerName.html
install -D -m 0444 %SDK_STAGE_LOC/html/CBBrokerVersion.html %HTML_DEST_PATH/CBBrokerVersion.html
install -D -m 0444 %SDK_STAGE_LOC/html/CBCreateInstance.html %HTML_DEST_PATH/CBCreateInstance.html
install -D -m 0444 %SDK_STAGE_LOC/html/CBDeleteInstance.html %HTML_DEST_PATH/CBDeleteInstance.html
install -D -m 0444 %SDK_STAGE_LOC/html/CBDeliverIndication.html %HTML_DEST_PATH/CBDeliverIndication.html
install -D -m 0444 %SDK_STAGE_LOC/html/CBDetachThread.html %HTML_DEST_PATH/CBDetachThread.html
install -D -m 0444 %SDK_STAGE_LOC/html/CBEnumInstanceNames.html %HTML_DEST_PATH/CBEnumInstanceNames.html
install -D -m 0444 %SDK_STAGE_LOC/html/CBEnumInstances.html %HTML_DEST_PATH/CBEnumInstances.html
install -D -m 0444 %SDK_STAGE_LOC/html/CBExecQuery.html %HTML_DEST_PATH/CBExecQuery.html
install -D -m 0444 %SDK_STAGE_LOC/html/CBGetClassification.html %HTML_DEST_PATH/CBGetClassification.html
install -D -m 0444 %SDK_STAGE_LOC/html/CBGetInstance.html %HTML_DEST_PATH/CBGetInstance.html
install -D -m 0444 %SDK_STAGE_LOC/html/CBGetProperty.html %HTML_DEST_PATH/CBGetProperty.html
install -D -m 0444 %SDK_STAGE_LOC/html/CBInvokeMethod.html %HTML_DEST_PATH/CBInvokeMethod.html
install -D -m 0444 %SDK_STAGE_LOC/html/CBPrepareAttachThread.html %HTML_DEST_PATH/CBPrepareAttachThread.html
install -D -m 0444 %SDK_STAGE_LOC/html/CBReferenceNames.html %HTML_DEST_PATH/CBReferenceNames.html
install -D -m 0444 %SDK_STAGE_LOC/html/CBReferences.html %HTML_DEST_PATH/CBReferences.html
install -D -m 0444 %SDK_STAGE_LOC/html/CBSetInstance.html %HTML_DEST_PATH/CBSetInstance.html
install -D -m 0444 %SDK_STAGE_LOC/html/CBSetProperty.html %HTML_DEST_PATH/CBSetProperty.html
install -D -m 0444 %SDK_STAGE_LOC/html/CDGetType.html %HTML_DEST_PATH/CDGetType.html
install -D -m 0444 %SDK_STAGE_LOC/html/CDIsOfType.html %HTML_DEST_PATH/CDIsOfType.html
install -D -m 0444 %SDK_STAGE_LOC/html/CDToString.html %HTML_DEST_PATH/CDToString.html
install -D -m 0444 %SDK_STAGE_LOC/html/Char16.html %HTML_DEST_PATH/Char16.html
install -D -m 0444 %SDK_STAGE_LOC/html/CIMAccessDeniedException.html %HTML_DEST_PATH/CIMAccessDeniedException.html
install -D -m 0444 %SDK_STAGE_LOC/html/CIMAssociationProvider.html %HTML_DEST_PATH/CIMAssociationProvider.html
install -D -m 0444 %SDK_STAGE_LOC/html/CIMClassHier.html %HTML_DEST_PATH/CIMClassHier.html
install -D -m 0444 %SDK_STAGE_LOC/html/CIMClass.html %HTML_DEST_PATH/CIMClass.html
install -D -m 0444 %SDK_STAGE_LOC/html/CIMClient.html %HTML_DEST_PATH/CIMClient.html
install -D -m 0444 %SDK_STAGE_LOC/html/CIMClientHTTPErrorException.html %HTML_DEST_PATH/CIMClientHTTPErrorException.html
install -D -m 0444 %SDK_STAGE_LOC/html/CIMClientMalformedHTTPException.html %HTML_DEST_PATH/CIMClientMalformedHTTPException.html
install -D -m 0444 %SDK_STAGE_LOC/html/CIMClientResponseException.html %HTML_DEST_PATH/CIMClientResponseException.html
install -D -m 0444 %SDK_STAGE_LOC/html/CIMClientXmlException.html %HTML_DEST_PATH/CIMClientXmlException.html
install -D -m 0444 %SDK_STAGE_LOC/html/CIMConstClass.html %HTML_DEST_PATH/CIMConstClass.html
install -D -m 0444 %SDK_STAGE_LOC/html/CIMConstInstance.html %HTML_DEST_PATH/CIMConstInstance.html
install -D -m 0444 %SDK_STAGE_LOC/html/CIMConstMethod.html %HTML_DEST_PATH/CIMConstMethod.html
install -D -m 0444 %SDK_STAGE_LOC/html/CIMConstObject.html %HTML_DEST_PATH/CIMConstObject.html
install -D -m 0444 %SDK_STAGE_LOC/html/CIMConstParameter.html %HTML_DEST_PATH/CIMConstParameter.html
install -D -m 0444 %SDK_STAGE_LOC/html/CIMConstProperty.html %HTML_DEST_PATH/CIMConstProperty.html
install -D -m 0444 %SDK_STAGE_LOC/html/CIMConstQualifierDecl.html %HTML_DEST_PATH/CIMConstQualifierDecl.html
install -D -m 0444 %SDK_STAGE_LOC/html/CIMConstQualifier.html %HTML_DEST_PATH/CIMConstQualifier.html
install -D -m 0444 %SDK_STAGE_LOC/html/CIMDateTime.html %HTML_DEST_PATH/CIMDateTime.html
install -D -m 0444 %SDK_STAGE_LOC/html/CIMException.html %HTML_DEST_PATH/CIMException.html
install -D -m 0444 %SDK_STAGE_LOC/html/CIMFlavor.html %HTML_DEST_PATH/CIMFlavor.html
install -D -m 0444 %SDK_STAGE_LOC/html/CIMIndicationConsumer.html %HTML_DEST_PATH/CIMIndicationConsumer.html
install -D -m 0444 %SDK_STAGE_LOC/html/CIMIndicationConsumerProvider.html %HTML_DEST_PATH/CIMIndicationConsumerProvider.html
install -D -m 0444 %SDK_STAGE_LOC/html/CIMIndicationProvider.html %HTML_DEST_PATH/CIMIndicationProvider.html
install -D -m 0444 %SDK_STAGE_LOC/html/CIMInstance.html %HTML_DEST_PATH/CIMInstance.html
install -D -m 0444 %SDK_STAGE_LOC/html/CIMInstanceProvider.html %HTML_DEST_PATH/CIMInstanceProvider.html
install -D -m 0444 %SDK_STAGE_LOC/html/CIMInvalidClassException.html %HTML_DEST_PATH/CIMInvalidClassException.html
install -D -m 0444 %SDK_STAGE_LOC/html/CIMInvalidParameterException.html %HTML_DEST_PATH/CIMInvalidParameterException.html
install -D -m 0444 %SDK_STAGE_LOC/html/CIMInvalidQueryException.html %HTML_DEST_PATH/CIMInvalidQueryException.html
install -D -m 0444 %SDK_STAGE_LOC/html/CIMKeyBinding.html %HTML_DEST_PATH/CIMKeyBinding.html
install -D -m 0444 %SDK_STAGE_LOC/html/CIMMethod.html %HTML_DEST_PATH/CIMMethod.html
install -D -m 0444 %SDK_STAGE_LOC/html/CIMMethodNotFoundException.html %HTML_DEST_PATH/CIMMethodNotFoundException.html
install -D -m 0444 %SDK_STAGE_LOC/html/CIMMethodProvider.html %HTML_DEST_PATH/CIMMethodProvider.html
install -D -m 0444 %SDK_STAGE_LOC/html/CIMName.html %HTML_DEST_PATH/CIMName.html
install -D -m 0444 %SDK_STAGE_LOC/html/CIMNamespaceName.html %HTML_DEST_PATH/CIMNamespaceName.html
install -D -m 0444 %SDK_STAGE_LOC/html/CIMNotSupportedException.html %HTML_DEST_PATH/CIMNotSupportedException.html
install -D -m 0444 %SDK_STAGE_LOC/html/CIMObjectAlreadyExistsException.html %HTML_DEST_PATH/CIMObjectAlreadyExistsException.html
install -D -m 0444 %SDK_STAGE_LOC/html/CIMObject.html %HTML_DEST_PATH/CIMObject.html
install -D -m 0444 %SDK_STAGE_LOC/html/CIMObjectNotFoundException.html %HTML_DEST_PATH/CIMObjectNotFoundException.html
install -D -m 0444 %SDK_STAGE_LOC/html/CIMObjectPath.html %HTML_DEST_PATH/CIMObjectPath.html
install -D -m 0444 %SDK_STAGE_LOC/html/CIMOperationFailedException.html %HTML_DEST_PATH/CIMOperationFailedException.html
install -D -m 0444 %SDK_STAGE_LOC/html/CIMParameter.html %HTML_DEST_PATH/CIMParameter.html
install -D -m 0444 %SDK_STAGE_LOC/html/CIMParamValue.html %HTML_DEST_PATH/CIMParamValue.html
install -D -m 0444 %SDK_STAGE_LOC/html/CIMProperty.html %HTML_DEST_PATH/CIMProperty.html
install -D -m 0444 %SDK_STAGE_LOC/html/CIMPropertyList.html %HTML_DEST_PATH/CIMPropertyList.html
install -D -m 0444 %SDK_STAGE_LOC/html/CIMPropertyNotFoundException.html %HTML_DEST_PATH/CIMPropertyNotFoundException.html
install -D -m 0444 %SDK_STAGE_LOC/html/CIMProvider.html %HTML_DEST_PATH/CIMProvider.html
install -D -m 0444 %SDK_STAGE_LOC/html/CIMQualifierDecl.html %HTML_DEST_PATH/CIMQualifierDecl.html
install -D -m 0444 %SDK_STAGE_LOC/html/CIMQualifier.html %HTML_DEST_PATH/CIMQualifier.html
install -D -m 0444 %SDK_STAGE_LOC/html/CIMScope.html %HTML_DEST_PATH/CIMScope.html
install -D -m 0444 %SDK_STAGE_LOC/html/CIMStatusCode.html %HTML_DEST_PATH/CIMStatusCode.html
install -D -m 0444 %SDK_STAGE_LOC/html/CIMType.html %HTML_DEST_PATH/CIMType.html
install -D -m 0444 %SDK_STAGE_LOC/html/cimTypeToString.html %HTML_DEST_PATH/cimTypeToString.html
install -D -m 0444 %SDK_STAGE_LOC/html/CIMValue.html %HTML_DEST_PATH/CIMValue.html
install -D -m 0444 %SDK_STAGE_LOC/html/ClientClassHier.html %HTML_DEST_PATH/ClientClassHier.html
install -D -m 0444 %SDK_STAGE_LOC/html/CMAddArg.html %HTML_DEST_PATH/CMAddArg.html
install -D -m 0444 %SDK_STAGE_LOC/html/CMAddContextEntry.html %HTML_DEST_PATH/CMAddContextEntry.html
install -D -m 0444 %SDK_STAGE_LOC/html/CMAddKey.html %HTML_DEST_PATH/CMAddKey.html
install -D -m 0444 %SDK_STAGE_LOC/html/CMAssociationMIFactory.html %HTML_DEST_PATH/CMAssociationMIFactory.html
install -D -m 0444 %SDK_STAGE_LOC/html/CMAssociationMIStub.html %HTML_DEST_PATH/CMAssociationMIStub.html
install -D -m 0444 %SDK_STAGE_LOC/html/CMClassPathIsA.html %HTML_DEST_PATH/CMClassPathIsA.html
install -D -m 0444 %SDK_STAGE_LOC/html/CMEvaluatePredicate.html %HTML_DEST_PATH/CMEvaluatePredicate.html
install -D -m 0444 %SDK_STAGE_LOC/html/CMEvaluateSelExp.html %HTML_DEST_PATH/CMEvaluateSelExp.html
install -D -m 0444 %SDK_STAGE_LOC/html/CMGetArgAt.html %HTML_DEST_PATH/CMGetArgAt.html
install -D -m 0444 %SDK_STAGE_LOC/html/CMGetArgCount.html %HTML_DEST_PATH/CMGetArgCount.html
install -D -m 0444 %SDK_STAGE_LOC/html/CMGetArg.html %HTML_DEST_PATH/CMGetArg.html
install -D -m 0444 %SDK_STAGE_LOC/html/CMGetArrayCount.html %HTML_DEST_PATH/CMGetArrayCount.html
install -D -m 0444 %SDK_STAGE_LOC/html/CMGetArrayElementAt.html %HTML_DEST_PATH/CMGetArrayElementAt.html
install -D -m 0444 %SDK_STAGE_LOC/html/CMGetArrayType.html %HTML_DEST_PATH/CMGetArrayType.html
install -D -m 0444 %SDK_STAGE_LOC/html/CMGetBinaryFormat.html %HTML_DEST_PATH/CMGetBinaryFormat.html
install -D -m 0444 %SDK_STAGE_LOC/html/CMGetCharsPtr.html %HTML_DEST_PATH/CMGetCharsPtr.html
install -D -m 0444 %SDK_STAGE_LOC/html/CMGetClassName.html %HTML_DEST_PATH/CMGetClassName.html
install -D -m 0444 %SDK_STAGE_LOC/html/CMGetCod.html %HTML_DEST_PATH/CMGetCod.html
install -D -m 0444 %SDK_STAGE_LOC/html/CMGetContextEntryAt.html %HTML_DEST_PATH/CMGetContextEntryAt.html
install -D -m 0444 %SDK_STAGE_LOC/html/CMGetContextEntryCount.html %HTML_DEST_PATH/CMGetContextEntryCount.html
install -D -m 0444 %SDK_STAGE_LOC/html/CMGetContextEntry.html %HTML_DEST_PATH/CMGetContextEntry.html
install -D -m 0444 %SDK_STAGE_LOC/html/CMGetDoc.html %HTML_DEST_PATH/CMGetDoc.html
install -D -m 0444 %SDK_STAGE_LOC/html/CMGetHostname.html %HTML_DEST_PATH/CMGetHostname.html
install -D -m 0444 %SDK_STAGE_LOC/html/CMGetKeyAt.html %HTML_DEST_PATH/CMGetKeyAt.html
install -D -m 0444 %SDK_STAGE_LOC/html/CMGetKeyCount.html %HTML_DEST_PATH/CMGetKeyCount.html
install -D -m 0444 %SDK_STAGE_LOC/html/CMGetKey.html %HTML_DEST_PATH/CMGetKey.html
install -D -m 0444 %SDK_STAGE_LOC/html/CMGetNameSpace.html %HTML_DEST_PATH/CMGetNameSpace.html
install -D -m 0444 %SDK_STAGE_LOC/html/CMGetNext.html %HTML_DEST_PATH/CMGetNext.html
install -D -m 0444 %SDK_STAGE_LOC/html/CMGetObjectPath.html %HTML_DEST_PATH/CMGetObjectPath.html
install -D -m 0444 %SDK_STAGE_LOC/html/CMGetPredicateAt.html %HTML_DEST_PATH/CMGetPredicateAt.html
install -D -m 0444 %SDK_STAGE_LOC/html/CMGetPredicateCount.html %HTML_DEST_PATH/CMGetPredicateCount.html
install -D -m 0444 %SDK_STAGE_LOC/html/CMGetPredicateData.html %HTML_DEST_PATH/CMGetPredicateData.html
install -D -m 0444 %SDK_STAGE_LOC/html/CMGetPredicate.html %HTML_DEST_PATH/CMGetPredicate.html
install -D -m 0444 %SDK_STAGE_LOC/html/CMGetPropertyAt.html %HTML_DEST_PATH/CMGetPropertyAt.html
install -D -m 0444 %SDK_STAGE_LOC/html/CMGetPropertyCount.html %HTML_DEST_PATH/CMGetPropertyCount.html
install -D -m 0444 %SDK_STAGE_LOC/html/CMGetProperty.html %HTML_DEST_PATH/CMGetProperty.html
install -D -m 0444 %SDK_STAGE_LOC/html/CMGetSelExpString.html %HTML_DEST_PATH/CMGetSelExpString.html
install -D -m 0444 %SDK_STAGE_LOC/html/CMGetStringFormat.html %HTML_DEST_PATH/CMGetStringFormat.html
install -D -m 0444 %SDK_STAGE_LOC/html/CMGetSubCondAt.html %HTML_DEST_PATH/CMGetSubCondAt.html
install -D -m 0444 %SDK_STAGE_LOC/html/CMGetSubCondCountAndType.html %HTML_DEST_PATH/CMGetSubCondCountAndType.html
install -D -m 0444 %SDK_STAGE_LOC/html/CMHasNext.html %HTML_DEST_PATH/CMHasNext.html
install -D -m 0444 %SDK_STAGE_LOC/html/CMIndicationMIStub.html %HTML_DEST_PATH/CMIndicationMIStub.html
install -D -m 0444 %SDK_STAGE_LOC/html/CMInstanceMIFactory.html %HTML_DEST_PATH/CMInstanceMIFactory.html
install -D -m 0444 %SDK_STAGE_LOC/html/CMInstanceMIStub.html %HTML_DEST_PATH/CMInstanceMIStub.html
install -D -m 0444 %SDK_STAGE_LOC/html/CMIsArray.html %HTML_DEST_PATH/CMIsArray.html
install -D -m 0444 %SDK_STAGE_LOC/html/CMIsInterval.html %HTML_DEST_PATH/CMIsInterval.html
install -D -m 0444 %SDK_STAGE_LOC/html/CMIsKeyValue.html %HTML_DEST_PATH/CMIsKeyValue.html
install -D -m 0444 %SDK_STAGE_LOC/html/CMIsNullObject.html %HTML_DEST_PATH/CMIsNullObject.html
install -D -m 0444 %SDK_STAGE_LOC/html/CMIsNullValue.html %HTML_DEST_PATH/CMIsNullValue.html
install -D -m 0444 %SDK_STAGE_LOC/html/CMMethodMIFactory.html %HTML_DEST_PATH/CMMethodMIFactory.html
install -D -m 0444 %SDK_STAGE_LOC/html/CMMethodMIStub.html %HTML_DEST_PATH/CMMethodMIStub.html
install -D -m 0444 %SDK_STAGE_LOC/html/CMNewArgs.html %HTML_DEST_PATH/CMNewArgs.html
install -D -m 0444 %SDK_STAGE_LOC/html/CMNewArray.html %HTML_DEST_PATH/CMNewArray.html
install -D -m 0444 %SDK_STAGE_LOC/html/CMNewDateTimeFromBinary.html %HTML_DEST_PATH/CMNewDateTimeFromBinary.html
install -D -m 0444 %SDK_STAGE_LOC/html/CMNewDateTimeFromChars.html %HTML_DEST_PATH/CMNewDateTimeFromChars.html
install -D -m 0444 %SDK_STAGE_LOC/html/CMNewDateTime.html %HTML_DEST_PATH/CMNewDateTime.html
install -D -m 0444 %SDK_STAGE_LOC/html/CMNewInstance.html %HTML_DEST_PATH/CMNewInstance.html
install -D -m 0444 %SDK_STAGE_LOC/html/CMNewObjectPath.html %HTML_DEST_PATH/CMNewObjectPath.html
install -D -m 0444 %SDK_STAGE_LOC/html/CMNewSelectExp.html %HTML_DEST_PATH/CMNewSelectExp.html
install -D -m 0444 %SDK_STAGE_LOC/html/CMNewString.html %HTML_DEST_PATH/CMNewString.html
install -D -m 0444 %SDK_STAGE_LOC/html/_CMPIArgsFT.html %HTML_DEST_PATH/_CMPIArgsFT.html
install -D -m 0444 %SDK_STAGE_LOC/html/CmpiArgs.html %HTML_DEST_PATH/CmpiArgs.html
install -D -m 0444 %SDK_STAGE_LOC/html/_CMPIArgs.html %HTML_DEST_PATH/_CMPIArgs.html
install -D -m 0444 %SDK_STAGE_LOC/html/_CMPIArrayFT.html %HTML_DEST_PATH/_CMPIArrayFT.html
install -D -m 0444 %SDK_STAGE_LOC/html/CmpiArray.html %HTML_DEST_PATH/CmpiArray.html
install -D -m 0444 %SDK_STAGE_LOC/html/_CMPIArray.html %HTML_DEST_PATH/_CMPIArray.html
install -D -m 0444 %SDK_STAGE_LOC/html/_CMPIAssociationMIFT.html %HTML_DEST_PATH/_CMPIAssociationMIFT.html
install -D -m 0444 %SDK_STAGE_LOC/html/CMPIAssociationMIFT.html %HTML_DEST_PATH/CMPIAssociationMIFT.html
install -D -m 0444 %SDK_STAGE_LOC/html/CmpiBooleanData.html %HTML_DEST_PATH/CmpiBooleanData.html
install -D -m 0444 %SDK_STAGE_LOC/html/_CMPIBrokerEncFT.html %HTML_DEST_PATH/_CMPIBrokerEncFT.html
install -D -m 0444 %SDK_STAGE_LOC/html/_CMPIBrokerExtFT.html %HTML_DEST_PATH/_CMPIBrokerExtFT.html
install -D -m 0444 %SDK_STAGE_LOC/html/_CMPIBrokerFT.html %HTML_DEST_PATH/_CMPIBrokerFT.html
install -D -m 0444 %SDK_STAGE_LOC/html/CmpiBroker.html %HTML_DEST_PATH/CmpiBroker.html
install -D -m 0444 %SDK_STAGE_LOC/html/_CMPIBroker.html %HTML_DEST_PATH/_CMPIBroker.html
install -D -m 0444 %SDK_STAGE_LOC/html/CmpiCharData.html %HTML_DEST_PATH/CmpiCharData.html
install -D -m 0444 %SDK_STAGE_LOC/html/CMPI_C_Hier.html %HTML_DEST_PATH/CMPI_C_Hier.html
install -D -m 0444 %SDK_STAGE_LOC/html/CMPIClassHier.html %HTML_DEST_PATH/CMPIClassHier.html
install -D -m 0444 %SDK_STAGE_LOC/html/_CMPIContextFT.html %HTML_DEST_PATH/_CMPIContextFT.html
install -D -m 0444 %SDK_STAGE_LOC/html/CmpiContext.html %HTML_DEST_PATH/CmpiContext.html
install -D -m 0444 %SDK_STAGE_LOC/html/_CMPIContext.html %HTML_DEST_PATH/_CMPIContext.html
install -D -m 0444 %SDK_STAGE_LOC/html/CmpiData.html %HTML_DEST_PATH/CmpiData.html
install -D -m 0444 %SDK_STAGE_LOC/html/_CMPIDateTimeFT.html %HTML_DEST_PATH/_CMPIDateTimeFT.html
install -D -m 0444 %SDK_STAGE_LOC/html/CmpiDateTime.html %HTML_DEST_PATH/CmpiDateTime.html
install -D -m 0444 %SDK_STAGE_LOC/html/_CMPIDateTime.html %HTML_DEST_PATH/_CMPIDateTime.html
install -D -m 0444 %SDK_STAGE_LOC/html/_CMPIEnumerationFT.html %HTML_DEST_PATH/_CMPIEnumerationFT.html
install -D -m 0444 %SDK_STAGE_LOC/html/CmpiEnumeration.html %HTML_DEST_PATH/CmpiEnumeration.html
install -D -m 0444 %SDK_STAGE_LOC/html/_CMPIEnumeration.html %HTML_DEST_PATH/_CMPIEnumeration.html
install -D -m 0444 %SDK_STAGE_LOC/html/_CMPIIndicationMIFT.html %HTML_DEST_PATH/_CMPIIndicationMIFT.html
install -D -m 0444 %SDK_STAGE_LOC/html/CMPIIndicationMIFT.html %HTML_DEST_PATH/CMPIIndicationMIFT.html
install -D -m 0444 %SDK_STAGE_LOC/html/_CMPIInstanceFT.html %HTML_DEST_PATH/_CMPIInstanceFT.html
install -D -m 0444 %SDK_STAGE_LOC/html/CmpiInstance.html %HTML_DEST_PATH/CmpiInstance.html
install -D -m 0444 %SDK_STAGE_LOC/html/_CMPIInstance.html %HTML_DEST_PATH/_CMPIInstance.html
install -D -m 0444 %SDK_STAGE_LOC/html/_CMPIInstanceMIFT.html %HTML_DEST_PATH/_CMPIInstanceMIFT.html
install -D -m 0444 %SDK_STAGE_LOC/html/CMPIInstanceMIFT.html %HTML_DEST_PATH/CMPIInstanceMIFT.html
install -D -m 0444 %SDK_STAGE_LOC/html/_CMPIMethodMIFT.html %HTML_DEST_PATH/_CMPIMethodMIFT.html
install -D -m 0444 %SDK_STAGE_LOC/html/CMPIMethodMIFT.html %HTML_DEST_PATH/CMPIMethodMIFT.html
install -D -m 0444 %SDK_STAGE_LOC/html/CmpiObject.html %HTML_DEST_PATH/CmpiObject.html
install -D -m 0444 %SDK_STAGE_LOC/html/_CMPIObjectPathFT.html %HTML_DEST_PATH/_CMPIObjectPathFT.html
install -D -m 0444 %SDK_STAGE_LOC/html/CmpiObjectPath.html %HTML_DEST_PATH/CmpiObjectPath.html
install -D -m 0444 %SDK_STAGE_LOC/html/_CMPIObjectPath.html %HTML_DEST_PATH/_CMPIObjectPath.html
install -D -m 0444 %SDK_STAGE_LOC/html/_CMPIPredicateFT.html %HTML_DEST_PATH/_CMPIPredicateFT.html
install -D -m 0444 %SDK_STAGE_LOC/html/_CMPIPredicate.html %HTML_DEST_PATH/_CMPIPredicate.html
install -D -m 0444 %SDK_STAGE_LOC/html/_CMPIPropertyMIFT.html %HTML_DEST_PATH/_CMPIPropertyMIFT.html
install -D -m 0444 %SDK_STAGE_LOC/html/CMPIPropertyMIFT.html %HTML_DEST_PATH/CMPIPropertyMIFT.html
install -D -m 0444 %SDK_STAGE_LOC/html/_CMPIResultFT.html %HTML_DEST_PATH/_CMPIResultFT.html
install -D -m 0444 %SDK_STAGE_LOC/html/CmpiResult.html %HTML_DEST_PATH/CmpiResult.html
install -D -m 0444 %SDK_STAGE_LOC/html/_CMPIResult.html %HTML_DEST_PATH/_CMPIResult.html
install -D -m 0444 %SDK_STAGE_LOC/html/_CMPISelectCondFT.html %HTML_DEST_PATH/_CMPISelectCondFT.html
install -D -m 0444 %SDK_STAGE_LOC/html/_CMPISelectCond.html %HTML_DEST_PATH/_CMPISelectCond.html
install -D -m 0444 %SDK_STAGE_LOC/html/_CMPISelectExpFT.html %HTML_DEST_PATH/_CMPISelectExpFT.html
install -D -m 0444 %SDK_STAGE_LOC/html/CmpiSelectExp.html %HTML_DEST_PATH/CmpiSelectExp.html
install -D -m 0444 %SDK_STAGE_LOC/html/_CMPISelectExp.html %HTML_DEST_PATH/_CMPISelectExp.html
install -D -m 0444 %SDK_STAGE_LOC/html/CmpiStatus.html %HTML_DEST_PATH/CmpiStatus.html
install -D -m 0444 %SDK_STAGE_LOC/html/_CMPIStringFT.html %HTML_DEST_PATH/_CMPIStringFT.html
install -D -m 0444 %SDK_STAGE_LOC/html/CmpiString.html %HTML_DEST_PATH/CmpiString.html
install -D -m 0444 %SDK_STAGE_LOC/html/_CMPIString.html %HTML_DEST_PATH/_CMPIString.html
install -D -m 0444 %SDK_STAGE_LOC/html/_CMPISubCondFT.html %HTML_DEST_PATH/_CMPISubCondFT.html
install -D -m 0444 %SDK_STAGE_LOC/html/_CMPISubCond.html %HTML_DEST_PATH/_CMPISubCond.html
install -D -m 0444 %SDK_STAGE_LOC/html/CMPropertyMIFactory.html %HTML_DEST_PATH/CMPropertyMIFactory.html
install -D -m 0444 %SDK_STAGE_LOC/html/CMPropertyMIStub.html %HTML_DEST_PATH/CMPropertyMIStub.html
install -D -m 0444 %SDK_STAGE_LOC/html/CMReturnData.html %HTML_DEST_PATH/CMReturnData.html
install -D -m 0444 %SDK_STAGE_LOC/html/CMReturnDone.html %HTML_DEST_PATH/CMReturnDone.html
install -D -m 0444 %SDK_STAGE_LOC/html/CMReturn.html %HTML_DEST_PATH/CMReturn.html
install -D -m 0444 %SDK_STAGE_LOC/html/CMReturnInstance.html %HTML_DEST_PATH/CMReturnInstance.html
install -D -m 0444 %SDK_STAGE_LOC/html/CMReturnObjectPath.html %HTML_DEST_PATH/CMReturnObjectPath.html
install -D -m 0444 %SDK_STAGE_LOC/html/CMReturnWithChars.html %HTML_DEST_PATH/CMReturnWithChars.html
install -D -m 0444 %SDK_STAGE_LOC/html/CMReturnWithString.html %HTML_DEST_PATH/CMReturnWithString.html
install -D -m 0444 %SDK_STAGE_LOC/html/CMSetArrayElementAt.html %HTML_DEST_PATH/CMSetArrayElementAt.html
install -D -m 0444 %SDK_STAGE_LOC/html/CMSetClassName.html %HTML_DEST_PATH/CMSetClassName.html
install -D -m 0444 %SDK_STAGE_LOC/html/CMSetHostAndNameSpaceFromObjectPath.html %HTML_DEST_PATH/CMSetHostAndNameSpaceFromObjectPath.html
install -D -m 0444 %SDK_STAGE_LOC/html/CMSetHostname.html %HTML_DEST_PATH/CMSetHostname.html
install -D -m 0444 %SDK_STAGE_LOC/html/CMSetNameSpaceFromObjectPath.html %HTML_DEST_PATH/CMSetNameSpaceFromObjectPath.html
install -D -m 0444 %SDK_STAGE_LOC/html/CMSetNameSpace.html %HTML_DEST_PATH/CMSetNameSpace.html
install -D -m 0444 %SDK_STAGE_LOC/html/CMSetPropertyFilter.html %HTML_DEST_PATH/CMSetPropertyFilter.html
install -D -m 0444 %SDK_STAGE_LOC/html/CMSetProperty.html %HTML_DEST_PATH/CMSetProperty.html
install -D -m 0444 %SDK_STAGE_LOC/html/CMSetStatus.html %HTML_DEST_PATH/CMSetStatus.html
install -D -m 0444 %SDK_STAGE_LOC/html/CMSetStatusWithChars.html %HTML_DEST_PATH/CMSetStatusWithChars.html
install -D -m 0444 %SDK_STAGE_LOC/html/CMSetStatusWithString.html %HTML_DEST_PATH/CMSetStatusWithString.html
install -D -m 0444 %SDK_STAGE_LOC/html/CMToArray.html %HTML_DEST_PATH/CMToArray.html
install -D -m 0444 %SDK_STAGE_LOC/html/CommonClassHier.html %HTML_DEST_PATH/CommonClassHier.html
install -D -m 0444 %SDK_STAGE_LOC/html/ConnectionTimeoutException.html %HTML_DEST_PATH/ConnectionTimeoutException.html
install -D -m 0444 %SDK_STAGE_LOC/html/Container.html %HTML_DEST_PATH/Container.html
install -D -m 0444 %SDK_STAGE_LOC/html/ContentLanguageListContainer.html %HTML_DEST_PATH/ContentLanguageListContainer.html
install -D -m 0444 %SDK_STAGE_LOC/html/CString.html %HTML_DEST_PATH/CString.html
install -D -m 0444 %SDK_STAGE_LOC/html/DateTimeOutOfRangeException.html %HTML_DEST_PATH/DateTimeOutOfRangeException.html
install -D -m 0444 %SDK_STAGE_LOC/html/DynamicCastFailedException.html %HTML_DEST_PATH/DynamicCastFailedException.html
install -D -m 0444 %SDK_STAGE_LOC/html/Exception.html %HTML_DEST_PATH/Exception.html
install -D -m 0444 %SDK_STAGE_LOC/html/Formatter.html %HTML_DEST_PATH/Formatter.html
install -D -m 0444 %SDK_STAGE_LOC/html/General.html %HTML_DEST_PATH/General.html
install -D -m 0444 %SDK_STAGE_LOC/html/HIER.html %HTML_DEST_PATH/HIER.html
install -D -m 0444 %SDK_STAGE_LOC/html/icon1.gif %HTML_DEST_PATH/icon1.gif
install -D -m 0444 %SDK_STAGE_LOC/html/icon2.gif %HTML_DEST_PATH/icon2.gif
install -D -m 0444 %SDK_STAGE_LOC/html/IdentityContainer.html %HTML_DEST_PATH/IdentityContainer.html
install -D -m 0444 %SDK_STAGE_LOC/html/index.html %HTML_DEST_PATH/index.html
install -D -m 0444 %SDK_STAGE_LOC/html/IndexOutOfBoundsException.html %HTML_DEST_PATH/IndexOutOfBoundsException.html
install -D -m 0444 %SDK_STAGE_LOC/html/IndicationConsumerClassHier.html %HTML_DEST_PATH/IndicationConsumerClassHier.html
install -D -m 0444 %SDK_STAGE_LOC/html/IndicationResponseHandler.html %HTML_DEST_PATH/IndicationResponseHandler.html
install -D -m 0444 %SDK_STAGE_LOC/html/InstanceResponseHandler.html %HTML_DEST_PATH/InstanceResponseHandler.html
install -D -m 0444 %SDK_STAGE_LOC/html/InvalidDateTimeFormatException.html %HTML_DEST_PATH/InvalidDateTimeFormatException.html
install -D -m 0444 %SDK_STAGE_LOC/html/InvalidLocatorException.html %HTML_DEST_PATH/InvalidLocatorException.html
install -D -m 0444 %SDK_STAGE_LOC/html/InvalidNameException.html %HTML_DEST_PATH/InvalidNameException.html
install -D -m 0444 %SDK_STAGE_LOC/html/InvalidNamespaceNameException.html %HTML_DEST_PATH/InvalidNamespaceNameException.html
install -D -m 0444 %SDK_STAGE_LOC/html/LanguageElementContainer.html %HTML_DEST_PATH/LanguageElementContainer.html
install -D -m 0444 %SDK_STAGE_LOC/html/LanguageElement.html %HTML_DEST_PATH/LanguageElement.html
install -D -m 0444 %SDK_STAGE_LOC/html/MalformedObjectNameException.html %HTML_DEST_PATH/MalformedObjectNameException.html
install -D -m 0444 %SDK_STAGE_LOC/html/MessageLoaderParms.html %HTML_DEST_PATH/MessageLoaderParms.html
install -D -m 0444 %SDK_STAGE_LOC/html/MethodResultResponseHandler.html %HTML_DEST_PATH/MethodResultResponseHandler.html
install -D -m 0444 %SDK_STAGE_LOC/html/NotConnectedException.html %HTML_DEST_PATH/NotConnectedException.html
install -D -m 0444 %SDK_STAGE_LOC/html/ObjectPathResponseHandler.html %HTML_DEST_PATH/ObjectPathResponseHandler.html
install -D -m 0444 %SDK_STAGE_LOC/html/ObjectResponseHandler.html %HTML_DEST_PATH/ObjectResponseHandler.html
install -D -m 0444 %SDK_STAGE_LOC/html/OperationContext.html %HTML_DEST_PATH/OperationContext.html
install -D -m 0444 %SDK_STAGE_LOC/html/operator.10.html %HTML_DEST_PATH/operator.10.html
install -D -m 0444 %SDK_STAGE_LOC/html/operator.11.html %HTML_DEST_PATH/operator.11.html
install -D -m 0444 %SDK_STAGE_LOC/html/operator.12.html %HTML_DEST_PATH/operator.12.html
install -D -m 0444 %SDK_STAGE_LOC/html/operator.2.html %HTML_DEST_PATH/operator.2.html
install -D -m 0444 %SDK_STAGE_LOC/html/operator.3.html %HTML_DEST_PATH/operator.3.html
install -D -m 0444 %SDK_STAGE_LOC/html/operator.4.html %HTML_DEST_PATH/operator.4.html
install -D -m 0444 %SDK_STAGE_LOC/html/operator.5.html %HTML_DEST_PATH/operator.5.html
install -D -m 0444 %SDK_STAGE_LOC/html/operator.6.html %HTML_DEST_PATH/operator.6.html
install -D -m 0444 %SDK_STAGE_LOC/html/operator.7.html %HTML_DEST_PATH/operator.7.html
install -D -m 0444 %SDK_STAGE_LOC/html/operator.8.html %HTML_DEST_PATH/operator.8.html
install -D -m 0444 %SDK_STAGE_LOC/html/operator.9.html %HTML_DEST_PATH/operator.9.html
install -D -m 0444 %SDK_STAGE_LOC/html/operator.html %HTML_DEST_PATH/operator.html
install -D -m 0444 %SDK_STAGE_LOC/html/ostream.html %HTML_DEST_PATH/ostream.html
install -D -m 0444 %SDK_STAGE_LOC/html/Pegasus_Array_h.html %HTML_DEST_PATH/Pegasus_Array_h.html
install -D -m 0444 %SDK_STAGE_LOC/html/Pegasus_MessageLoader_h.html %HTML_DEST_PATH/Pegasus_MessageLoader_h.html
install -D -m 0444 %SDK_STAGE_LOC/html/ProviderClassHier.html %HTML_DEST_PATH/ProviderClassHier.html
install -D -m 0444 %SDK_STAGE_LOC/html/ResponseHandler.html %HTML_DEST_PATH/ResponseHandler.html
install -D -m 0444 %SDK_STAGE_LOC/html/SSLCallbackInfo.html %HTML_DEST_PATH/SSLCallbackInfo.html
install -D -m 0444 %SDK_STAGE_LOC/html/SSLCertificateInfo.html %HTML_DEST_PATH/SSLCertificateInfo.html
install -D -m 0444 %SDK_STAGE_LOC/html/SSLContext.html %HTML_DEST_PATH/SSLContext.html
install -D -m 0444 %SDK_STAGE_LOC/html/SSLException.html %HTML_DEST_PATH/SSLException.htm
install -D -m 0444 %SDK_STAGE_LOC/html/String.html %HTML_DEST_PATH/String.html
install -D -m 0444 %SDK_STAGE_LOC/html/SubscriptionInstanceNamesContainer.html %HTML_DEST_PATH/SubscriptionInstanceNamesContainer.html
install -D -m 0444 %SDK_STAGE_LOC/html/SubscriptionLanguageListContainer.html %HTML_DEST_PATH/SubscriptionLanguageListContainer.html
install -D -m 0444 %SDK_STAGE_LOC/html/TimeoutContainer.html %HTML_DEST_PATH/TimeoutContainer.html
install -D -m 0444 %SDK_STAGE_LOC/html/TypeMismatchException.html %HTML_DEST_PATH/TypeMismatchException.html
install -D -m 0444 %SDK_STAGE_LOC/html/UninitializedObjectException.html %HTML_DEST_PATH/UninitializedObjectException.html
install -D -m 0444 %SDK_STAGE_LOC/samples/Makefile %SAMPLES_DEST_PATH/Makefile 
install -D -m 0444 %SDK_STAGE_LOC/samples/mak/common.mak %SAMPLES_DEST_PATH/mak/common.mak 

echo "PEGASUS_DEST_LIB_DIR =   "%PEGASUS_DEST_LIB_DIR > sampleconfig.txt
echo "PEGASUS_VARDATA_DIR =    "%PEGASUS_VARDATA_DIR >> sampleconfig.txt
echo "PEGASUS_PROVIDER_LIB_DIR="%PEGASUS_PROVIDER_LIB_DIR >> sampleconfig.txt
echo "CIM_MOF_PATH =           "%PEGASUS_MOF_DIR>> sampleconfig.txt
echo "PEGASUS_INCLUDE_DIR =    "%PEGASUS_INCLUDE_DEST_PATH >> sampleconfig.txt
echo "SAMPLES_DIR =            "%PEGASUS_SAMPLES_DEST_PATH >> sampleconfig.txt
echo "PEGASUS_BIN_DIR =        "%PEGASUS_BIN_DIR >> sampleconfig.txt
echo "PEGASUS_PLATFORM =       "%PEGASUS_PLATFORM >> sampleconfig.txt
cat sampleconfig.txt %SDK_STAGE_LOC/samples/mak/config.mak > sampleconfig.mak
install -D -m 0444 sampleconfig.mak %SAMPLES_DEST_PATH/mak/config.mak 
install -D -m 0444  %SDK_STAGE_LOC/samples/mak/%PEGASUS_PLATFORM.mak %SAMPLES_DEST_PATH/mak/%PEGASUS_PLATFORM.mak 
install -D -m 0444 %SDK_STAGE_LOC/samples/mak/library.mak %SAMPLES_DEST_PATH/mak/library.mak 
install -D -m 0444 %SDK_STAGE_LOC/samples/mak/program.mak %SAMPLES_DEST_PATH/mak/program.mak
install -D -m 0444 %SDK_STAGE_LOC/samples/mak/recurse.mak %SAMPLES_DEST_PATH/mak/recurse.mak
install -D -m 0444 %SDK_STAGE_LOC/samples/Clients/Makefile %SAMPLES_DEST_PATH/Clients/Makefile
install -D -m 0444 %SDK_STAGE_LOC/samples/Clients/DefaultC++/Makefile %SAMPLES_DEST_PATH/Clients/DefaultC++/Makefile
install -D -m 0444 %SDK_STAGE_LOC/samples/Clients/DefaultC++/EnumInstances/EnumInstances.cpp %SAMPLES_DEST_PATH/Clients/DefaultC++/EnumInstances/EnumInstances.cpp 
install -D -m 0444 %SDK_STAGE_LOC/samples/Clients/DefaultC++/EnumInstances/Makefile %SAMPLES_DEST_PATH/Clients/DefaultC++/EnumInstances/Makefile 
install -D -m 0444 %SDK_STAGE_LOC/samples/Clients/DefaultC++/InvokeMethod/InvokeMethod.cpp %SAMPLES_DEST_PATH/Clients/DefaultC++/InvokeMethod/InvokeMethod.cpp 
install -D -m 0444 %SDK_STAGE_LOC/samples/Clients/DefaultC++/InvokeMethod/Makefile %SAMPLES_DEST_PATH/Clients/DefaultC++/InvokeMethod/Makefile 
install -D -m 0444 %SDK_STAGE_LOC/samples/Providers/Makefile %SAMPLES_DEST_PATH/Providers/Makefile
install -D -m 0444 $PEGASUS_ROOT/src/SDK/samples/Providers/CMPI/Makefile %SAMPLES_DEST_PATH/Providers/CMPI/Makefile
install -D -m 0444 %SDK_STAGE_LOC/samples/Providers/CMPI/FilesAndDirectories/CWS_Directory.c %SAMPLES_DEST_PATH/Providers/CMPI/FilesAndDirectories/CWS_Directory.c 
install -D -m 0444 %SDK_STAGE_LOC/samples/Providers/CMPI/FilesAndDirectories/CWS_DirectoryContainsFile.c %SAMPLES_DEST_PATH/Providers/CMPI/FilesAndDirectories/CWS_DirectoryContainsFile.c 
install -D -m 0444 %SDK_STAGE_LOC/samples/Providers/CMPI/FilesAndDirectories/CWS_FileUtils.c %SAMPLES_DEST_PATH/Providers/CMPI/FilesAndDirectories/CWS_FileUtils.c 
install -D -m 0444 %SDK_STAGE_LOC/samples/Providers/CMPI/FilesAndDirectories/CWS_FileUtils.h %SAMPLES_DEST_PATH/Providers/CMPI/FilesAndDirectories/CWS_FileUtils.h 
install -D -m 0444 %SDK_STAGE_LOC/samples/Providers/CMPI/FilesAndDirectories/CWS_PlainFile.c %SAMPLES_DEST_PATH/Providers/CMPI/FilesAndDirectories/CWS_PlainFile.c 
install -D -m 0444 %SDK_STAGE_LOC/samples/Providers/CMPI/FilesAndDirectories/cwssimdata.c %SAMPLES_DEST_PATH/Providers/CMPI/FilesAndDirectories/cwssimdata.c 
install -D -m 0444 %SDK_STAGE_LOC/samples/Providers/CMPI/FilesAndDirectories/cwstest.c %SAMPLES_DEST_PATH/Providers/CMPI/FilesAndDirectories/cwstest.c 
install -D -m 0444 %SDK_STAGE_LOC/samples/Providers/CMPI/FilesAndDirectories/cwsutil.c %SAMPLES_DEST_PATH/Providers/CMPI/FilesAndDirectories/cwsutil.c 
install -D -m 0444 %SDK_STAGE_LOC/samples/Providers/CMPI/FilesAndDirectories/cwsutil.h %SAMPLES_DEST_PATH/Providers/CMPI/FilesAndDirectories/cwsutil.h 
install -D -m 0444 %SDK_STAGE_LOC/samples/Providers/CMPI/FilesAndDirectories/Makefile %SAMPLES_DEST_PATH/Providers/CMPI/FilesAndDirectories/Makefile 
install -D -m 0444 %SDK_STAGE_LOC/samples/Providers/DefaultC++/Makefile %SAMPLES_DEST_PATH/Providers/DefaultC++/Makefile
install -D -m 0444 %SDK_STAGE_LOC/samples/Providers/DefaultC++/InstanceProvider/EnumerateInstancesrspgood.xml %SAMPLES_DEST_PATH/Providers/DefaultC++/InstanceProvider/EnumerateInstancesrspgood.xml 
install -D -m 0444 %SDK_STAGE_LOC/samples/Providers/DefaultC++/InstanceProvider/EnumerateInstances.xml %SAMPLES_DEST_PATH/Providers/DefaultC++/InstanceProvider/EnumerateInstances.xml 
install -D -m 0444 %SDK_STAGE_LOC/samples/Providers/DefaultC++/InstanceProvider/InstanceProvider.cpp %SAMPLES_DEST_PATH/Providers/DefaultC++/InstanceProvider/InstanceProvider.cpp 
install -D -m 0444 %SDK_STAGE_LOC/samples/Providers/DefaultC++/InstanceProvider/InstanceProvider.h %SAMPLES_DEST_PATH/Providers/DefaultC++/InstanceProvider/InstanceProvider.h 
install -D -m 0444 %SDK_STAGE_LOC/samples/Providers/DefaultC++/InstanceProvider/InstanceProviderMain.cpp %SAMPLES_DEST_PATH/Providers/DefaultC++/InstanceProvider/InstanceProviderMain.cpp 
install -D -m 0444 %SDK_STAGE_LOC/samples/Providers/DefaultC++/InstanceProvider/Makefile %SAMPLES_DEST_PATH/Providers/DefaultC++/InstanceProvider/Makefile 
install -D -m 0444 %SDK_STAGE_LOC/samples/Providers/DefaultC++/MethodProvider/InvokeMethodrspgood.xml %SAMPLES_DEST_PATH/Providers/DefaultC++/MethodProvider/InvokeMethodrspgood.xml 
install -D -m 0444 %SDK_STAGE_LOC/samples/Providers/DefaultC++/MethodProvider/InvokeMethod.xml %SAMPLES_DEST_PATH/Providers/DefaultC++/MethodProvider/InvokeMethod.xml 
install -D -m 0444 %SDK_STAGE_LOC/samples/Providers/DefaultC++/MethodProvider/Makefile %SAMPLES_DEST_PATH/Providers/DefaultC++/MethodProvider/Makefile 
install -D -m 0444 %SDK_STAGE_LOC/samples/Providers/DefaultC++/MethodProvider/MethodProvider.cpp %SAMPLES_DEST_PATH/Providers/DefaultC++/MethodProvider/MethodProvider.cpp 
install -D -m 0444 %SDK_STAGE_LOC/samples/Providers/DefaultC++/MethodProvider/MethodProvider.h %SAMPLES_DEST_PATH/Providers/DefaultC++/MethodProvider/MethodProvider.h 
install -D -m 0444 %SDK_STAGE_LOC/samples/Providers/DefaultC++/MethodProvider/MethodProviderMain.cpp %SAMPLES_DEST_PATH/Providers/DefaultC++/MethodProvider/MethodProviderMain.cpp 
install -D -m 0444 %SDK_STAGE_LOC/samples/Providers/CMPI/FilesAndDirectories/tests/AssocDirNames12001rspgood.xml %SAMPLES_DEST_PATH/Providers/CMPI/FilesAndDirectories/tests/AssocDirNames12001rspgood.xml 
install -D -m 0444 %SDK_STAGE_LOC/samples/Providers/CMPI/FilesAndDirectories/tests/AssocDirNames12001.xml %SAMPLES_DEST_PATH/Providers/CMPI/FilesAndDirectories/tests/AssocDirNames12001.xml 
install -D -m 0444 %SDK_STAGE_LOC/samples/Providers/CMPI/FilesAndDirectories/tests/DeleteDirInstance10003rspgood.xml %SAMPLES_DEST_PATH/Providers/CMPI/FilesAndDirectories/tests/DeleteDirInstance10003rspgood.xml 
install -D -m 0444 %SDK_STAGE_LOC/samples/Providers/CMPI/FilesAndDirectories/tests/DeleteDirInstance10003.xml %SAMPLES_DEST_PATH/Providers/CMPI/FilesAndDirectories/tests/DeleteDirInstance10003.xml 
install -D -m 0444 %SDK_STAGE_LOC/samples/Providers/CMPI/FilesAndDirectories/tests/EnumerateDir10001rspgood.xml %SAMPLES_DEST_PATH/Providers/CMPI/FilesAndDirectories/tests/EnumerateDir10001rspgood.xml 
install -D -m 0444 %SDK_STAGE_LOC/samples/Providers/CMPI/FilesAndDirectories/tests/EnumerateDir10001.xml %SAMPLES_DEST_PATH/Providers/CMPI/FilesAndDirectories/tests/EnumerateDir10001.xml 
install -D -m 0444 %SDK_STAGE_LOC/samples/Providers/CMPI/FilesAndDirectories/tests/EnumerateDirNames10000rspgood.xml %SAMPLES_DEST_PATH/Providers/CMPI/FilesAndDirectories/tests/EnumerateDirNames10000rspgood.xml 
install -D -m 0444 %SDK_STAGE_LOC/samples/Providers/CMPI/FilesAndDirectories/tests/EnumerateDirNames10000.xml %SAMPLES_DEST_PATH/Providers/CMPI/FilesAndDirectories/tests/EnumerateDirNames10000.xml 
install -D -m 0444 %SDK_STAGE_LOC/samples/Providers/CMPI/FilesAndDirectories/tests/GetDirInstance10002rspgood.xml %SAMPLES_DEST_PATH/Providers/CMPI/FilesAndDirectories/tests/GetDirInstance10002rspgood.xml 
install -D -m 0444 %SDK_STAGE_LOC/samples/Providers/CMPI/FilesAndDirectories/tests/GetDirInstance10002.xml %SAMPLES_DEST_PATH/Providers/CMPI/FilesAndDirectories/tests/GetDirInstance10002.xml 
install -D -m 0444 %SDK_STAGE_LOC/samples/Providers/CMPI/FilesAndDirectories/tests/Makefile %SAMPLES_DEST_PATH/Providers/CMPI/FilesAndDirectories/tests/Makefile 
install -D -m 0444 %SDK_STAGE_LOC/samples/Providers/CMPI/FilesAndDirectories/tests/PlainFileMethodCall11001rspgood.xml %SAMPLES_DEST_PATH/Providers/CMPI/FilesAndDirectories/tests/PlainFileMethodCall11001rspgood.xml 
install -D -m 0444 %SDK_STAGE_LOC/samples/Providers/CMPI/FilesAndDirectories/tests/PlainFileMethodCall11001.xml %SAMPLES_DEST_PATH/Providers/CMPI/FilesAndDirectories/tests/PlainFileMethodCall11001.xml 
install -D -m 0444 %SDK_STAGE_LOC/samples/Providers/CMPI/FilesAndDirectories/tests/ReferenceDirNames12002rspgood.xml %SAMPLES_DEST_PATH/Providers/CMPI/FilesAndDirectories/tests/ReferenceDirNames12002rspgood.xml 
install -D -m 0444 %SDK_STAGE_LOC/samples/Providers/CMPI/FilesAndDirectories/tests/ReferenceDirNames12002.xml %SAMPLES_DEST_PATH/Providers/CMPI/FilesAndDirectories/tests/ReferenceDirNames12002.xml 
install -D -m 0444 $PEGASUS_ROOT/src/SDK/samples/Providers/Load/Makefile %SAMPLES_DEST_PATH/Providers/Load/Makefile
install -D -m 0444 $PEGASUS_ROOT/src/Providers/sample/Load/CWS_FilesAndDir.mof %SAMPLES_DEST_PATH/Providers/Load/CWS_FilesAndDir.mof
install -D -m 0444 $PEGASUS_ROOT/src/Providers/sample/Load/CWS_FilesAndDirR.mof %SAMPLES_DEST_PATH/Providers/Load/CWS_FilesAndDirR.mof
install -D -m 0444 $PEGASUS_ROOT/src/Providers/sample/Load/InstanceProviderR.mof %SAMPLES_DEST_PATH/Providers/Load/InstanceProviderR.mof
install -D -m 0444 $PEGASUS_ROOT/src/Providers/sample/Load/MethodProviderR.mof %SAMPLES_DEST_PATH/Providers/Load/MethodProviderR.mof
install -D -m 0444 $PEGASUS_ROOT/src/Providers/sample/Load/SampleProviderSchema.mof %SAMPLES_DEST_PATH/Providers/Load/SampleProviderSchema.mof

rm -Rf $PEGASUS_HOME

%clean
#make clean
[ "$RPM_BUILD_ROOT" != "/" ] && [ -d $RPM_BUILD_ROOT ] && rm -rf $RPM_BUILD_ROOT;

%pre
if [ -d %PEGASUS_REPOSITORY_DIR"/root#PG_Internal" ]
then
  #
  # Save the current repository to prev_repository.
  #

  REPOSITORY_LOC=%PEGASUS_REPOSITORY_DIR
  PREV_REPOSITORY_LOC=%PEGASUS_REPOSITORY_DIR"/prev_repository"

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

%post
mkdir -p %PEGASUS_LOG_DIR
%define INSTALL_LOG %PEGASUS_LOG_DIR/install.log
echo `date` >%INSTALL_LOG 2>&1

/usr/lib/lsb/install_initd /etc/init.d/tog-pegasus

/bin/chmod -f +w %PEGASUS_LOCAL_DOMAIN_SOCKET_DIR/cimxml.socket

# Create symbolic links for client libs
#
cd %PEGASUS_DEST_LIB_DIR
ln -sf libpegcommon.so.1 libpegcommon.so
ln -sf libpegclient.so.1 libpegclient.so
ln -sf libpegprovider.so.1 libpegprovider.so
ln -sf libDefaultProviderManager.so.1 libDefaultProviderManager.so
ln -sf libCMPIProviderManager.so.1 libCMPIProviderManager.so

# Create symbolic links for provider libs
#
cd %PEGASUS_PROVIDER_LIB_DIR
ln -sf libComputerSystemProvider.so.1 libComputerSystemProvider.so
ln -sf libOSProvider.so.1 libOSProvider.so
ln -sf libProcessProvider.so.1 libProcessProvider.so

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
    -e "s/$EMAIL/root@$HOSTNAME/" %PEGASUS_CONFIG_DIR/ssl.orig \
    > %PEGASUS_CONFIG_DIR/ssl.cnf
chmod 400 %PEGASUS_CONFIG_DIR/ssl.cnf
chown bin %PEGASUS_CONFIG_DIR/ssl.cnf
chgrp bin %PEGASUS_CONFIG_DIR/ssl.cnf

openssl req -x509 -days 365 -newkey rsa:2048 \
   -nodes -config %PEGASUS_CONFIG_DIR/ssl.cnf   \
   -keyout %PEGASUS_CONFIG_DIR/key.pem -out %PEGASUS_CONFIG_DIR/cert.pem 2>>%INSTALL_LOG

cat %PEGASUS_CONFIG_DIR/key.pem > %PEGASUS_CONFIG_DIR/file_2048.pem
cat %PEGASUS_CONFIG_DIR/cert.pem > %PEGASUS_CONFIG_DIR/server_2048.pem
cat %PEGASUS_CONFIG_DIR/cert.pem > %PEGASUS_CONFIG_DIR/client_2048.pem
chmod 700 %PEGASUS_CONFIG_DIR/*.pem

rm -f %PEGASUS_CONFIG_DIR/key.pem %PEGASUS_CONFIG_DIR/cert.pem

if [ -f %PEGASUS_CONFIG_DIR/%PEGASUS_SSL_CERT_FILE ] 
then
    echo "WARNING: %PEGASUS_CONFIG_DIR/%PEGASUS_SSL_CERT_FILE SSL Certificate file already exists."
else
    cp %PEGASUS_CONFIG_DIR/server_2048.pem %PEGASUS_CONFIG_DIR/%PEGASUS_SSL_CERT_FILE
    cp %PEGASUS_CONFIG_DIR/file_2048.pem %PEGASUS_CONFIG_DIR/%PEGASUS_SSL_KEY_FILE
    chmod 400 %PEGASUS_CONFIG_DIR/%PEGASUS_SSL_CERT_FILE %PEGASUS_CONFIG_DIR/%PEGASUS_SSL_KEY_FILE
fi

if [ -f %PEGASUS_CONFIG_DIR/%PEGASUS_SSL_TRUSTSTORE ]
then
    echo "WARNING: %PEGASUS_CONFIG_DIR/%PEGASUS_SSL_TRUSTSTORE SSL Certificate trust store already exists."
else
    cp %PEGASUS_CONFIG_DIR/client_2048.pem %PEGASUS_CONFIG_DIR/%PEGASUS_SSL_TRUSTSTORE
    chmod 400 %PEGASUS_CONFIG_DIR/%PEGASUS_SSL_TRUSTSTORE
fi

# Start the cimserver
(/etc/init.d/tog-pegasus start || /opt/tog-pegasus/sbin/cimserver) >> %INSTALL_LOG 2>&1

if [ $? != 0 ];
then
  echo "Brute-starting Pegasus. If the installation fails:"
  echo " - Start Pegasus: '/etc/init.d/tog-pegasus start'"
  echo " - Run '/opt/tog-pegasus/sbin/init_repository'"
  # No need to try it again, since it already failed.
  # Just giving the above notice should be sufficent
  # /etc/init.d/tog-pegasus start || /opt/tog-pegasus/sbin/cimserver
fi

echo " To start Pegasus manually:"
echo " /etc/init.d/tog-pegasus start"
echo " Stop it:"
echo " /etc/init.d/tog-pegasus stop"

%preun

# Check if the cimserver is running
isRunning=`ps -el | grep cimserver | grep -v "grep cimserver"`
if [ "$isRunning" ]; then
	%PEGASUS_SBIN_DIR/cimserver -s	
fi

# Delete the Link to the rc.* Startup Directories
/usr/lib/lsb/remove_initd /etc/init.d/tog-pegasus

%postun
if [ $1 = 0 ]; then
	rm -rf %PEGASUS_VARDATA_DIR
        rm -rf %PEGASUS_PROD_DIR
	rm -rf %PEGASUS_CONFIG_DIR
	export LC_ALL=C
fi

%files
%defattr(-,root,root)
%dir %attr(-,root,root) %PEGASUS_PROD_DIR
%dir %attr(-,root,root) %PEGASUS_LOCAL_AUTH_DIR
%dir %attr(-,root,root) %PEGASUS_CONFIG_DIR
%dir %attr(-,root,root) %PEGASUS_VARDATA_DIR
%dir %attr(-,root,root) %PEGASUS_LOG_DIR
%dir %attr(-,root,root) %PEGASUS_PROVIDER_LIB_DIR
%dir %attr(1555,root,root) %PEGASUS_LOCAL_DOMAIN_SOCKET_DIR
%defattr(0755,root,root)
%PEGASUS_REPOSITORY_DIR
%defattr(-,root,root)
%doc %PEGASUS_PROD_DIR/%PEGASUS_LICENSE_FILE
%doc %PEGASUS_MANUSER_DIR/cimmof.1
%doc %PEGASUS_MANUSER_DIR/cimprovider.1
%doc %PEGASUS_MANUSER_DIR/osinfo.1
%doc %PEGASUS_MANUSER_DIR/wbemexec.1
%doc %PEGASUS_MANADMIN_DIR/cimauth.8
%doc %PEGASUS_MANADMIN_DIR/cimconfig.8
%doc %PEGASUS_MANADMIN_DIR/cimserver.8
%attr(-,root,root) %PEGASUS_MOF_DIR/CIM28/CIM_Core28.mof
%attr(-,root,root) %PEGASUS_MOF_DIR/CIM28/Core28_Qualifiers.mof
%attr(-,root,root) %PEGASUS_MOF_DIR/CIM28/Core28_CoreElements.mof
%attr(-,root,root) %PEGASUS_MOF_DIR/CIM28/Core28_Physical.mof
%attr(-,root,root) %PEGASUS_MOF_DIR/CIM28/Core28_Software.mof
%attr(-,root,root) %PEGASUS_MOF_DIR/CIM28/Core28_Device.mof
%attr(-,root,root) %PEGASUS_MOF_DIR/CIM28/Core28_StorageExtent.mof
%attr(-,root,root) %PEGASUS_MOF_DIR/CIM28/Core28_Collection.mof
%attr(-,root,root) %PEGASUS_MOF_DIR/CIM28/Core28_Redundancy.mof
%attr(-,root,root) %PEGASUS_MOF_DIR/CIM28/Core28_Service.mof
%attr(-,root,root) %PEGASUS_MOF_DIR/CIM28/Core28_ProductFRU.mof
%attr(-,root,root) %PEGASUS_MOF_DIR/CIM28/Core28_Statistics.mof
%attr(-,root,root) %PEGASUS_MOF_DIR/CIM28/Core28_Capabilities.mof
%attr(-,root,root) %PEGASUS_MOF_DIR/CIM28/Core28_Settings.mof
%attr(-,root,root) %PEGASUS_MOF_DIR/CIM28/Core28_MethodParms.mof
%attr(-,root,root) %PEGASUS_MOF_DIR/CIM28/Core28_PowerMgmt.mof
%attr(-,root,root) %PEGASUS_MOF_DIR/CIM28/Application28_AppRuntime.mof
%attr(-,root,root) %PEGASUS_MOF_DIR/CIM28/Application28_BIOS.mof
%attr(-,root,root) %PEGASUS_MOF_DIR/CIM28/Application28_CheckAction.mof
%attr(-,root,root) %PEGASUS_MOF_DIR/CIM28/Application28_DeploymentModel.mof
%attr(-,root,root) %PEGASUS_MOF_DIR/CIM28/Application28_InstalledProduct.mof
%attr(-,root,root) %PEGASUS_MOF_DIR/CIM28/Application28_J2eeAppServer.mof
%attr(-,root,root) %PEGASUS_MOF_DIR/CIM28/Application28_J2eeAppServerStats.mof
%attr(-,root,root) %PEGASUS_MOF_DIR/CIM28/Application28_SystemSoftware.mof
%attr(-,root,root) %PEGASUS_MOF_DIR/CIM28/CIM_Application28.mof
%attr(-,root,root) %PEGASUS_MOF_DIR/CIM28/CIM_Database28.mof
%attr(-,root,root) %PEGASUS_MOF_DIR/CIM28/CIM_Device28.mof
%attr(-,root,root) %PEGASUS_MOF_DIR/CIM28/CIM_Event28.mof
%attr(-,root,root) %PEGASUS_MOF_DIR/CIM28/CIM_Interop28.mof
%attr(-,root,root) %PEGASUS_MOF_DIR/CIM28/CIM_IPsecPolicy28.mof
%attr(-,root,root) %PEGASUS_MOF_DIR/CIM28/CIM_Metrics28.mof
%attr(-,root,root) %PEGASUS_MOF_DIR/CIM28/CIM_Network28.mof
%attr(-,root,root) %PEGASUS_MOF_DIR/CIM28/CIM_Physical28.mof
%attr(-,root,root) %PEGASUS_MOF_DIR/CIM28/CIM_Policy28.mof
%attr(-,root,root) %PEGASUS_MOF_DIR/CIM28/CIM_Schema28.mof
%attr(-,root,root) %PEGASUS_MOF_DIR/CIM28/CIM_Support28.mof
%attr(-,root,root) %PEGASUS_MOF_DIR/CIM28/CIM_System28.mof
%attr(-,root,root) %PEGASUS_MOF_DIR/CIM28/CIM_User28.mof
%attr(-,root,root) %PEGASUS_MOF_DIR/CIM28/Database28_Statistics.mof
%attr(-,root,root) %PEGASUS_MOF_DIR/CIM28/Database28_Storage.mof
%attr(-,root,root) %PEGASUS_MOF_DIR/CIM28/Database28_System.mof
%attr(-,root,root) %PEGASUS_MOF_DIR/CIM28/Device28_Controller.mof
%attr(-,root,root) %PEGASUS_MOF_DIR/CIM28/Device28_CoolingAndPower.mof
%attr(-,root,root) %PEGASUS_MOF_DIR/CIM28/Device28_DeviceElements.mof
%attr(-,root,root) %PEGASUS_MOF_DIR/CIM28/Device28_DiskGroup.mof
%attr(-,root,root) %PEGASUS_MOF_DIR/CIM28/Device28_FC.mof
%attr(-,root,root) %PEGASUS_MOF_DIR/CIM28/Device28_IB.mof
%attr(-,root,root) %PEGASUS_MOF_DIR/CIM28/Device28_Memory.mof
%attr(-,root,root) %PEGASUS_MOF_DIR/CIM28/Device28_Modems.mof
%attr(-,root,root) %PEGASUS_MOF_DIR/CIM28/Device28_NetworkAdapter.mof
%attr(-,root,root) %PEGASUS_MOF_DIR/CIM28/Device28_Ports.mof
%attr(-,root,root) %PEGASUS_MOF_DIR/CIM28/Device28_Printing.mof
%attr(-,root,root) %PEGASUS_MOF_DIR/CIM28/Device28_Processor.mof
%attr(-,root,root) %PEGASUS_MOF_DIR/CIM28/Device28_ProtocolController.mof
%attr(-,root,root) %PEGASUS_MOF_DIR/CIM28/Device28_SccExtents.mof
%attr(-,root,root) %PEGASUS_MOF_DIR/CIM28/Device28_Sensors.mof
%attr(-,root,root) %PEGASUS_MOF_DIR/CIM28/Device28_StorageDevices.mof
%attr(-,root,root) %PEGASUS_MOF_DIR/CIM28/Device28_StorageExtents.mof
%attr(-,root,root) %PEGASUS_MOF_DIR/CIM28/Device28_StorageLib.mof
%attr(-,root,root) %PEGASUS_MOF_DIR/CIM28/Device28_StorageServices.mof
%attr(-,root,root) %PEGASUS_MOF_DIR/CIM28/Device28_USB.mof
%attr(-,root,root) %PEGASUS_MOF_DIR/CIM28/Device28_UserDevices.mof
%attr(-,root,root) %PEGASUS_MOF_DIR/CIM28/Metrics28_BaseMetric.mof
%attr(-,root,root) %PEGASUS_MOF_DIR/CIM28/Metrics28_UnitOfWork.mof
%attr(-,root,root) %PEGASUS_MOF_DIR/CIM28/Network28_BGP.mof
%attr(-,root,root) %PEGASUS_MOF_DIR/CIM28/Network28_Buffers.mof
%attr(-,root,root) %PEGASUS_MOF_DIR/CIM28/Network28_Collections.mof
%attr(-,root,root) %PEGASUS_MOF_DIR/CIM28/Network28_Filtering.mof
%attr(-,root,root) %PEGASUS_MOF_DIR/CIM28/Network28_IPsec.mof
%attr(-,root,root) %PEGASUS_MOF_DIR/CIM28/Network28_OSPF.mof
%attr(-,root,root) %PEGASUS_MOF_DIR/CIM28/Network28_Pipes.mof
%attr(-,root,root) %PEGASUS_MOF_DIR/CIM28/Network28_ProtocolEndpoints.mof
%attr(-,root,root) %PEGASUS_MOF_DIR/CIM28/Network28_QoS.mof
%attr(-,root,root) %PEGASUS_MOF_DIR/CIM28/Network28_Routes.mof
%attr(-,root,root) %PEGASUS_MOF_DIR/CIM28/Network28_RoutingForwarding.mof
%attr(-,root,root) %PEGASUS_MOF_DIR/CIM28/Network28_SNMP.mof
%attr(-,root,root) %PEGASUS_MOF_DIR/CIM28/Network28_SwitchingBridging.mof
%attr(-,root,root) %PEGASUS_MOF_DIR/CIM28/Network28_Systems.mof
%attr(-,root,root) %PEGASUS_MOF_DIR/CIM28/Network28_VLAN.mof
%attr(-,root,root) %PEGASUS_MOF_DIR/CIM28/Physical28_Component.mof
%attr(-,root,root) %PEGASUS_MOF_DIR/CIM28/Physical28_Connector.mof
%attr(-,root,root) %PEGASUS_MOF_DIR/CIM28/Physical28_Link.mof
%attr(-,root,root) %PEGASUS_MOF_DIR/CIM28/Physical28_Misc.mof
%attr(-,root,root) %PEGASUS_MOF_DIR/CIM28/Physical28_Package.mof
%attr(-,root,root) %PEGASUS_MOF_DIR/CIM28/Physical28_PhysicalMedia.mof
%attr(-,root,root) %PEGASUS_MOF_DIR/CIM28/Physical28_StoragePackage.mof
%attr(-,root,root) %PEGASUS_MOF_DIR/CIM28/System28_Boot.mof
%attr(-,root,root) %PEGASUS_MOF_DIR/CIM28/System28_Diagnostics.mof
%attr(-,root,root) %PEGASUS_MOF_DIR/CIM28/System28_FileElements.mof
%attr(-,root,root) %PEGASUS_MOF_DIR/CIM28/System28_Logs.mof
%attr(-,root,root) %PEGASUS_MOF_DIR/CIM28/System28_OperatingSystem.mof
%attr(-,root,root) %PEGASUS_MOF_DIR/CIM28/System28_Processing.mof
%attr(-,root,root) %PEGASUS_MOF_DIR/CIM28/System28_SystemElements.mof
%attr(-,root,root) %PEGASUS_MOF_DIR/CIM28/System28_SystemResources.mof
%attr(-,root,root) %PEGASUS_MOF_DIR/CIM28/System28_Time.mof
%attr(-,root,root) %PEGASUS_MOF_DIR/CIM28/System28_Unix.mof
%attr(-,root,root) %PEGASUS_MOF_DIR/CIM28/User28_AccessControl.mof
%attr(-,root,root) %PEGASUS_MOF_DIR/CIM28/User28_Account.mof
%attr(-,root,root) %PEGASUS_MOF_DIR/CIM28/User28_AuthenticationReqmt.mof
%attr(-,root,root) %PEGASUS_MOF_DIR/CIM28/User28_Credential.mof
%attr(-,root,root) %PEGASUS_MOF_DIR/CIM28/User28_Group.mof
%attr(-,root,root) %PEGASUS_MOF_DIR/CIM28/User28_Identity.mof
%attr(-,root,root) %PEGASUS_MOF_DIR/CIM28/User28_Kerberos.mof
%attr(-,root,root) %PEGASUS_MOF_DIR/CIM28/User28_Org.mof
%attr(-,root,root) %PEGASUS_MOF_DIR/CIM28/User28_Privilege.mof
%attr(-,root,root) %PEGASUS_MOF_DIR/CIM28/User28_PrivilegeManagementService.mof
%attr(-,root,root) %PEGASUS_MOF_DIR/CIM28/User28_PublicKey.mof
%attr(-,root,root) %PEGASUS_MOF_DIR/CIM28/User28_Role.mof
%attr(-,root,root) %PEGASUS_MOF_DIR/CIM28/User28_SecurityLevel.mof
%attr(-,root,root) %PEGASUS_MOF_DIR/CIM28/User28_SecurityServices.mof
%attr(-,root,root) %PEGASUS_MOF_DIR/CIM28/User28_SharedSecret.mof
%attr(-,root,root) %PEGASUS_MOF_DIR/CIM28/User28_StorageHardwareID.mof
%attr(-,root,root) %PEGASUS_MOF_DIR/CIM28/User28_UsersAccess.mof
%attr(-,root,root) %PEGASUS_MOF_DIR/Pegasus/PG_InterOpSchema20.mof
%attr(-,root,root) %PEGASUS_MOF_DIR/Pegasus/PG_InternalSchema20.mof
%attr(-,root,root) %PEGASUS_MOF_DIR/Pegasus/PG_ManagedSystemSchema20.mof
%attr(-,root,root) %PEGASUS_MOF_DIR/Pegasus/PG_ManagedSystemSchema20R.mof
%attr(-,root,root) %PEGASUS_MOF_DIR/Pegasus/PG_Authorization20.mof
%attr(-,root,root) %PEGASUS_MOF_DIR/Pegasus/PG_ConfigSetting20.mof
%attr(-,root,root) %PEGASUS_MOF_DIR/Pegasus/PG_ShutdownService20.mof
%attr(-,root,root) %PEGASUS_MOF_DIR/Pegasus/PG_User20.mof
%attr(-,root,root) %PEGASUS_MOF_DIR/Pegasus/PG_Events20.mof
%attr(-,root,root) %PEGASUS_MOF_DIR/Pegasus/PG_ProviderModule20.mof
%attr(-,root,root) %PEGASUS_MOF_DIR/Pegasus/PG_ComputerSystem20.mof
%attr(-,root,root) %PEGASUS_MOF_DIR/Pegasus/PG_ComputerSystem20R.mof
%attr(-,root,root) %PEGASUS_MOF_DIR/Pegasus/PG_OperatingSystem20.mof
%attr(-,root,root) %PEGASUS_MOF_DIR/Pegasus/PG_OperatingSystem20R.mof
%attr(-,root,root) %PEGASUS_MOF_DIR/Pegasus/PG_UnixProcess20.mof
%attr(-,root,root) %PEGASUS_MOF_DIR/Pegasus/PG_UnixProcess20R.mof
%config %attr(-,root,root) %PEGASUS_VARDATA_DIR/%PEGASUS_PLANNED_CONFIG_FILE
%config %attr(-,root,root) /etc/init.d/tog-pegasus
%config %attr(-,root,root) %PAM_CONF/wbem
%attr(-,root,root) %PEGASUS_CONFIG_DIR/ssl.orig
%attr(-,root,root) %PEGASUS_SBIN_DIR/cimauth
%attr(-,root,root) %PEGASUS_SBIN_DIR/cimserver
%attr(-,root,root) %PEGASUS_SBIN_DIR/cimservera
%attr(-,root,root) %PEGASUS_SBIN_DIR/cimuser
%attr(-,root,root) %PEGASUS_SBIN_DIR/cimconfig
%attr(-,root,root) %PEGASUS_SBIN_DIR/init_repository
%attr(-,root,root) %PEGASUS_BIN_DIR/cimmof
%attr(-,root,root) %PEGASUS_BIN_DIR/cimmofl
%attr(-,root,root) %PEGASUS_BIN_DIR/cimprovider
%attr(-,root,root) %PEGASUS_BIN_DIR/osinfo
%attr(-,root,root) %PEGASUS_BIN_DIR/wbemexec
%attr(-,root,root) %PEGASUS_DEST_LIB_DIR/libCIMxmlIndicationHandler.so.1
%attr(-,root,root) %PEGASUS_DEST_LIB_DIR/libConfigSettingProvider.so.1
%attr(-,root,root) %PEGASUS_DEST_LIB_DIR/libNamespaceProvider.so.1
%attr(-,root,root) %PEGASUS_DEST_LIB_DIR/libProviderRegistrationProvider.so.1
%attr(-,root,root) %PEGASUS_DEST_LIB_DIR/libUserAuthProvider.so.1
%attr(-,root,root) %PEGASUS_DEST_LIB_DIR/libpegauthentication.so.1
%attr(-,root,root) %PEGASUS_DEST_LIB_DIR/libpegclient.so.1
%attr(-,root,root) %PEGASUS_DEST_LIB_DIR/libpegcliutils.so.1
%attr(-,root,root) %PEGASUS_DEST_LIB_DIR/libpegcommon.so.1
%attr(-,root,root) %PEGASUS_DEST_LIB_DIR/libpegcompiler.so.1
%attr(-,root,root) %PEGASUS_DEST_LIB_DIR/libpegconfig.so.1
%attr(-,root,root) %PEGASUS_DEST_LIB_DIR/libpegexportclient.so.1
%attr(-,root,root) %PEGASUS_DEST_LIB_DIR/libpegexportserver.so.1
%attr(-,root,root) %PEGASUS_DEST_LIB_DIR/libpeggetoopt.so.1
%attr(-,root,root) %PEGASUS_DEST_LIB_DIR/libpeghandlerservice.so.1
%attr(-,root,root) %PEGASUS_DEST_LIB_DIR/libpegindicationservice.so.1
%attr(-,root,root) %PEGASUS_DEST_LIB_DIR/libpeglistener.so.1
%attr(-,root,root) %PEGASUS_DEST_LIB_DIR/libpegprm.so.1
%attr(-,root,root) %PEGASUS_DEST_LIB_DIR/libpegprovidermanager.so.1
%attr(-,root,root) %PEGASUS_DEST_LIB_DIR/libpegprovider.so.1
%attr(-,root,root) %PEGASUS_DEST_LIB_DIR/libpegrepository.so.1
%attr(-,root,root) %PEGASUS_DEST_LIB_DIR/libpegserver.so.1
%attr(-,root,root) %PEGASUS_DEST_LIB_DIR/libpeguser.so.1
%attr(-,root,root) %PEGASUS_DEST_LIB_DIR/libpegwql.so.1
%attr(-,root,root) %PEGASUS_DEST_LIB_DIR/libCMPIProviderManager.so.1
%attr(-,root,root) %PEGASUS_DEST_LIB_DIR/libcmpiCppImpl.so.1
%attr(-,root,root) %PEGASUS_DEST_LIB_DIR/libDefaultProviderManager.so.1
%attr(-,root,root) %PEGASUS_PROVIDER_LIB_DIR/libComputerSystemProvider.so.1
%attr(-,root,root) %PEGASUS_PROVIDER_LIB_DIR/libOSProvider.so.1
%attr(-,root,root) %PEGASUS_PROVIDER_LIB_DIR/libProcessProvider.so.1


%files sdk
%defattr(0444,root,root)
%attr(-,root,root) %PEGASUS_INCLUDE_DEST_PATH/Pegasus/Client/CIMClientException.h
%attr(-,root,root) %PEGASUS_INCLUDE_DEST_PATH/Pegasus/Client/CIMClient.h
%attr(-,root,root) %PEGASUS_INCLUDE_DEST_PATH/Pegasus/Client/Linkage.h
%attr(-,root,root) %PEGASUS_INCLUDE_DEST_PATH/Pegasus/Common/AcceptLanguages.h
%attr(-,root,root) %PEGASUS_INCLUDE_DEST_PATH/Pegasus/Common/AcceptLanguageElement.h
%attr(-,root,root) %PEGASUS_INCLUDE_DEST_PATH/Pegasus/Common/Array.h
%attr(-,root,root) %PEGASUS_INCLUDE_DEST_PATH/Pegasus/Common/ArrayInter.h
%attr(-,root,root) %PEGASUS_INCLUDE_DEST_PATH/Pegasus/Common/Char16.h
%attr(-,root,root) %PEGASUS_INCLUDE_DEST_PATH/Pegasus/Common/CIMClass.h 
%attr(-,root,root) %PEGASUS_INCLUDE_DEST_PATH/Pegasus/Common/CIMDateTime.h
%attr(-,root,root) %PEGASUS_INCLUDE_DEST_PATH/Pegasus/Common/CIMFlavor.h
%attr(-,root,root) %PEGASUS_INCLUDE_DEST_PATH/Pegasus/Common/CIMIndication.h 
%attr(-,root,root) %PEGASUS_INCLUDE_DEST_PATH/Pegasus/Common/CIMInstance.h
%attr(-,root,root) %PEGASUS_INCLUDE_DEST_PATH/Pegasus/Common//CIMMethod.h
%attr(-,root,root) %PEGASUS_INCLUDE_DEST_PATH/Pegasus/Common/CIMName.h
%attr(-,root,root) %PEGASUS_INCLUDE_DEST_PATH/Pegasus/Common/CIMObject.h
%attr(-,root,root) %PEGASUS_INCLUDE_DEST_PATH/Pegasus/Common/CIMObjectPath.h
%attr(-,root,root) %PEGASUS_INCLUDE_DEST_PATH/Pegasus/Common/CIMParameter.h
%attr(-,root,root) %PEGASUS_INCLUDE_DEST_PATH/Pegasus/Common/CIMParamValue.h
%attr(-,root,root) %PEGASUS_INCLUDE_DEST_PATH/Pegasus/Common/CIMProperty.h
%attr(-,root,root) %PEGASUS_INCLUDE_DEST_PATH/Pegasus/Common/CIMPropertyList.h
%attr(-,root,root) %PEGASUS_INCLUDE_DEST_PATH/Pegasus/Common/CIMQualifierDecl.h
%attr(-,root,root) %PEGASUS_INCLUDE_DEST_PATH/Pegasus/Common/CIMQualifier.h
%attr(-,root,root) %PEGASUS_INCLUDE_DEST_PATH/Pegasus/Common/CIMScope.h
%attr(-,root,root) %PEGASUS_INCLUDE_DEST_PATH/Pegasus/Common/CIMStatusCode.h
%attr(-,root,root) %PEGASUS_INCLUDE_DEST_PATH/Pegasus/Common/CIMType.h
%attr(-,root,root) %PEGASUS_INCLUDE_DEST_PATH/Pegasus/Common/CIMValue.h
%attr(-,root,root) %PEGASUS_INCLUDE_DEST_PATH/Pegasus/Common/Config.h
%attr(-,root,root) %PEGASUS_INCLUDE_DEST_PATH/Pegasus/Common/ContentLanguageElement.h
%attr(-,root,root) %PEGASUS_INCLUDE_DEST_PATH/Pegasus/Common/ContentLanguages.h
%attr(-,root,root) %PEGASUS_INCLUDE_DEST_PATH/Pegasus/Common/Exception.h
%attr(-,root,root) %PEGASUS_INCLUDE_DEST_PATH/Pegasus/Common/Formatter.h
%attr(-,root,root) %PEGASUS_INCLUDE_DEST_PATH/Pegasus/Common/LanguageElementContainer.h
%attr(-,root,root) %PEGASUS_INCLUDE_DEST_PATH/Pegasus/Common/LanguageElement.h
%attr(-,root,root) %PEGASUS_INCLUDE_DEST_PATH/Pegasus/Common/Linkage.h
%attr(-,root,root) %PEGASUS_INCLUDE_DEST_PATH/Pegasus/Common/MessageLoader.h
%attr(-,root,root) %PEGASUS_INCLUDE_DEST_PATH/Pegasus/Common/OperationContext.h
%ifarch ia64
%attr(-,root,root) %PEGASUS_INCLUDE_DEST_PATH/Pegasus/Common/Platform_LINUX_IA64_GNU.h
%else
%attr(-,root,root) %PEGASUS_INCLUDE_DEST_PATH/Pegasus/Common/Platform_LINUX_IX86_GNU.h
%endif
%attr(-,root,root) %PEGASUS_INCLUDE_DEST_PATH/Pegasus/Common/ResponseHandler.h
%attr(-,root,root) %PEGASUS_INCLUDE_DEST_PATH/Pegasus/Common/SSLContext.h
%attr(-,root,root) %PEGASUS_INCLUDE_DEST_PATH/Pegasus/Common/String.h
%attr(-,root,root) %PEGASUS_INCLUDE_DEST_PATH/Pegasus/Consumer/CIMIndicationConsumer.h
%attr(-,root,root) %PEGASUS_INCLUDE_DEST_PATH/Pegasus/Consumer/Linkage.h
%attr(-,root,root) %PEGASUS_INCLUDE_DEST_PATH/Pegasus/Provider/CIMAssociationProvider.h
%attr(-,root,root) %PEGASUS_INCLUDE_DEST_PATH/Pegasus/Provider/CIMIndicationConsumerProvider.h
%attr(-,root,root) %PEGASUS_INCLUDE_DEST_PATH/Pegasus/Provider/CIMIndicationProvider.h
%attr(-,root,root) %PEGASUS_INCLUDE_DEST_PATH/Pegasus/Provider/CIMInstanceProvider.h
%attr(-,root,root) %PEGASUS_INCLUDE_DEST_PATH/Pegasus/Provider/CIMMethodProvider.h
%attr(-,root,root) %PEGASUS_INCLUDE_DEST_PATH/Pegasus/Provider/CIMOMHandle.h
%attr(-,root,root) %PEGASUS_INCLUDE_DEST_PATH/Pegasus/Provider/CIMProvider.h
%attr(-,root,root) %PEGASUS_INCLUDE_DEST_PATH/Pegasus/Provider/Linkage.h
%attr(-,root,root) %PEGASUS_INCLUDE_DEST_PATH/Pegasus/Provider/ProviderException.h
%attr(-,root,root) %PEGASUS_INCLUDE_DEST_PATH/Pegasus/Provider/CMPI/CmpiArgs.h
%attr(-,root,root) %PEGASUS_INCLUDE_DEST_PATH/Pegasus/Provider/CMPI/CmpiArray.h
%attr(-,root,root) %PEGASUS_INCLUDE_DEST_PATH/Pegasus/Provider/CMPI/CmpiAssociationMI.h
%attr(-,root,root) %PEGASUS_INCLUDE_DEST_PATH/Pegasus/Provider/CMPI/CmpiBaseMI.h
%attr(-,root,root) %PEGASUS_INCLUDE_DEST_PATH/Pegasus/Provider/CMPI/CmpiBooleanData.h
%attr(-,root,root) %PEGASUS_INCLUDE_DEST_PATH/Pegasus/Provider/CMPI/CmpiBroker.h
%attr(-,root,root) %PEGASUS_INCLUDE_DEST_PATH/Pegasus/Provider/CMPI/CmpiCharData.h
%attr(-,root,root) %PEGASUS_INCLUDE_DEST_PATH/Pegasus/Provider/CMPI/CmpiContext.h
%attr(-,root,root) %PEGASUS_INCLUDE_DEST_PATH/Pegasus/Provider/CMPI/CmpiData.h
%attr(-,root,root) %PEGASUS_INCLUDE_DEST_PATH/Pegasus/Provider/CMPI/CmpiDateTime.h
%attr(-,root,root) %PEGASUS_INCLUDE_DEST_PATH/Pegasus/Provider/CMPI/cmpidt.h
%attr(-,root,root) %PEGASUS_INCLUDE_DEST_PATH/Pegasus/Provider/CMPI/CmpiEnumeration.h
%attr(-,root,root) %PEGASUS_INCLUDE_DEST_PATH/Pegasus/Provider/CMPI/cmpift.h
%attr(-,root,root) %PEGASUS_INCLUDE_DEST_PATH/Pegasus/Provider/CMPI/CmpiIndicationMI.h
%attr(-,root,root) %PEGASUS_INCLUDE_DEST_PATH/Pegasus/Provider/CMPI/CmpiInstance.h
%attr(-,root,root) %PEGASUS_INCLUDE_DEST_PATH/Pegasus/Provider/CMPI/CmpiInstanceMI.h
%attr(-,root,root) %PEGASUS_INCLUDE_DEST_PATH/Pegasus/Provider/CMPI/cmpimacs.h
%attr(-,root,root) %PEGASUS_INCLUDE_DEST_PATH/Pegasus/Provider/CMPI/CmpiMethodMI.h
%attr(-,root,root) %PEGASUS_INCLUDE_DEST_PATH/Pegasus/Provider/CMPI/CmpiObject.h
%attr(-,root,root) %PEGASUS_INCLUDE_DEST_PATH/Pegasus/Provider/CMPI/CmpiObjectPath.h
%attr(-,root,root) %PEGASUS_INCLUDE_DEST_PATH/Pegasus/Provider/CMPI/cmpipl.h
%attr(-,root,root) %PEGASUS_INCLUDE_DEST_PATH/Pegasus/Provider/CMPI/CmpiPropertyMI.h
%attr(-,root,root) %PEGASUS_INCLUDE_DEST_PATH/Pegasus/Provider/CMPI/CmpiProviderBase.h
%attr(-,root,root) %PEGASUS_INCLUDE_DEST_PATH/Pegasus/Provider/CMPI/CmpiResult.h
%attr(-,root,root) %PEGASUS_INCLUDE_DEST_PATH/Pegasus/Provider/CMPI/CmpiSelectExp.h
%attr(-,root,root) %PEGASUS_INCLUDE_DEST_PATH/Pegasus/Provider/CMPI/CmpiStatus.h
%attr(-,root,root) %PEGASUS_INCLUDE_DEST_PATH/Pegasus/Provider/CMPI/CmpiString.h
%attr(-,root,root) %PEGASUS_HTML_DEST_PATH/AcceptLanguageElement.html
%attr(-,root,root) %PEGASUS_HTML_DEST_PATH/AcceptLanguages.html
%attr(-,root,root) %PEGASUS_HTML_DEST_PATH/AlreadyConnectedException.html
%attr(-,root,root) %PEGASUS_HTML_DEST_PATH/AlreadyExistsException.html
%attr(-,root,root) %PEGASUS_HTML_DEST_PATH/ArrayPEGASUS_ARRAY_T.html
%attr(-,root,root) %PEGASUS_HTML_DEST_PATH/BindFailedException.html
%attr(-,root,root) %PEGASUS_HTML_DEST_PATH/CannotConnectException.html
%attr(-,root,root) %PEGASUS_HTML_DEST_PATH/CannotCreateSocketException.html
%attr(-,root,root) %PEGASUS_HTML_DEST_PATH/CBAssociatorNames.html
%attr(-,root,root) %PEGASUS_HTML_DEST_PATH/CBAssociators.html
%attr(-,root,root) %PEGASUS_HTML_DEST_PATH/CBAttachThread.html
%attr(-,root,root) %PEGASUS_HTML_DEST_PATH/CBbrokerName.html
%attr(-,root,root) %PEGASUS_HTML_DEST_PATH/CBBrokerVersion.html
%attr(-,root,root) %PEGASUS_HTML_DEST_PATH/CBCreateInstance.html
%attr(-,root,root) %PEGASUS_HTML_DEST_PATH/CBDeleteInstance.html
%attr(-,root,root) %PEGASUS_HTML_DEST_PATH/CBDeliverIndication.html
%attr(-,root,root) %PEGASUS_HTML_DEST_PATH/CBDetachThread.html
%attr(-,root,root) %PEGASUS_HTML_DEST_PATH/CBEnumInstanceNames.html
%attr(-,root,root) %PEGASUS_HTML_DEST_PATH/CBEnumInstances.html
%attr(-,root,root) %PEGASUS_HTML_DEST_PATH/CBExecQuery.html
%attr(-,root,root) %PEGASUS_HTML_DEST_PATH/CBGetClassification.html
%attr(-,root,root) %PEGASUS_HTML_DEST_PATH/CBGetInstance.html
%attr(-,root,root) %PEGASUS_HTML_DEST_PATH/CBGetProperty.html
%attr(-,root,root) %PEGASUS_HTML_DEST_PATH/CBInvokeMethod.html
%attr(-,root,root) %PEGASUS_HTML_DEST_PATH/CBPrepareAttachThread.html
%attr(-,root,root) %PEGASUS_HTML_DEST_PATH/CBReferenceNames.html
%attr(-,root,root) %PEGASUS_HTML_DEST_PATH/CBReferences.html
%attr(-,root,root) %PEGASUS_HTML_DEST_PATH/CBSetInstance.html
%attr(-,root,root) %PEGASUS_HTML_DEST_PATH/CBSetProperty.html
%attr(-,root,root) %PEGASUS_HTML_DEST_PATH/CDGetType.html
%attr(-,root,root) %PEGASUS_HTML_DEST_PATH/CDIsOfType.html
%attr(-,root,root) %PEGASUS_HTML_DEST_PATH/CDToString.html
%attr(-,root,root) %PEGASUS_HTML_DEST_PATH/Char16.html
%attr(-,root,root) %PEGASUS_HTML_DEST_PATH/CIMAccessDeniedException.html
%attr(-,root,root) %PEGASUS_HTML_DEST_PATH/CIMAssociationProvider.html
%attr(-,root,root) %PEGASUS_HTML_DEST_PATH/CIMClassHier.html
%attr(-,root,root) %PEGASUS_HTML_DEST_PATH/CIMClass.html
%attr(-,root,root) %PEGASUS_HTML_DEST_PATH/CIMClient.html
%attr(-,root,root) %PEGASUS_HTML_DEST_PATH/CIMClientHTTPErrorException.html
%attr(-,root,root) %PEGASUS_HTML_DEST_PATH/CIMClientMalformedHTTPException.html
%attr(-,root,root) %PEGASUS_HTML_DEST_PATH/CIMClientResponseException.html
%attr(-,root,root) %PEGASUS_HTML_DEST_PATH/CIMClientXmlException.html
%attr(-,root,root) %PEGASUS_HTML_DEST_PATH/CIMConstClass.html
%attr(-,root,root) %PEGASUS_HTML_DEST_PATH/CIMConstInstance.html
%attr(-,root,root) %PEGASUS_HTML_DEST_PATH/CIMConstMethod.html
%attr(-,root,root) %PEGASUS_HTML_DEST_PATH/CIMConstObject.html
%attr(-,root,root) %PEGASUS_HTML_DEST_PATH/CIMConstParameter.html
%attr(-,root,root) %PEGASUS_HTML_DEST_PATH/CIMConstProperty.html
%attr(-,root,root) %PEGASUS_HTML_DEST_PATH/CIMConstQualifierDecl.html
%attr(-,root,root) %PEGASUS_HTML_DEST_PATH/CIMConstQualifier.html
%attr(-,root,root) %PEGASUS_HTML_DEST_PATH/CIMDateTime.html
%attr(-,root,root) %PEGASUS_HTML_DEST_PATH/CIMException.html
%attr(-,root,root) %PEGASUS_HTML_DEST_PATH/CIMFlavor.html
%attr(-,root,root) %PEGASUS_HTML_DEST_PATH/CIMIndicationConsumer.html
%attr(-,root,root) %PEGASUS_HTML_DEST_PATH/CIMIndicationConsumerProvider.html
%attr(-,root,root) %PEGASUS_HTML_DEST_PATH/CIMIndicationProvider.html
%attr(-,root,root) %PEGASUS_HTML_DEST_PATH/CIMInstance.html
%attr(-,root,root) %PEGASUS_HTML_DEST_PATH/CIMInstanceProvider.html
%attr(-,root,root) %PEGASUS_HTML_DEST_PATH/CIMInvalidClassException.html
%attr(-,root,root) %PEGASUS_HTML_DEST_PATH/CIMInvalidParameterException.html
%attr(-,root,root) %PEGASUS_HTML_DEST_PATH/CIMInvalidQueryException.html
%attr(-,root,root) %PEGASUS_HTML_DEST_PATH/CIMKeyBinding.html
%attr(-,root,root) %PEGASUS_HTML_DEST_PATH/CIMMethod.html
%attr(-,root,root) %PEGASUS_HTML_DEST_PATH/CIMMethodNotFoundException.html
%attr(-,root,root) %PEGASUS_HTML_DEST_PATH/CIMMethodProvider.html
%attr(-,root,root) %PEGASUS_HTML_DEST_PATH/CIMName.html
%attr(-,root,root) %PEGASUS_HTML_DEST_PATH/CIMNamespaceName.html
%attr(-,root,root) %PEGASUS_HTML_DEST_PATH/CIMNotSupportedException.html
%attr(-,root,root) %PEGASUS_HTML_DEST_PATH/CIMObjectAlreadyExistsException.html
%attr(-,root,root) %PEGASUS_HTML_DEST_PATH/CIMObject.html
%attr(-,root,root) %PEGASUS_HTML_DEST_PATH/CIMObjectNotFoundException.html
%attr(-,root,root) %PEGASUS_HTML_DEST_PATH/CIMObjectPath.html
%attr(-,root,root) %PEGASUS_HTML_DEST_PATH/CIMOperationFailedException.html
%attr(-,root,root) %PEGASUS_HTML_DEST_PATH/CIMParameter.html
%attr(-,root,root) %PEGASUS_HTML_DEST_PATH/CIMParamValue.html
%attr(-,root,root) %PEGASUS_HTML_DEST_PATH/CIMProperty.html
%attr(-,root,root) %PEGASUS_HTML_DEST_PATH/CIMPropertyList.html
%attr(-,root,root) %PEGASUS_HTML_DEST_PATH/CIMPropertyNotFoundException.html
%attr(-,root,root) %PEGASUS_HTML_DEST_PATH/CIMProvider.html
%attr(-,root,root) %PEGASUS_HTML_DEST_PATH/CIMQualifierDecl.html
%attr(-,root,root) %PEGASUS_HTML_DEST_PATH/CIMQualifier.html
%attr(-,root,root) %PEGASUS_HTML_DEST_PATH/CIMScope.html
%attr(-,root,root) %PEGASUS_HTML_DEST_PATH/CIMStatusCode.html
%attr(-,root,root) %PEGASUS_HTML_DEST_PATH/CIMType.html
%attr(-,root,root) %PEGASUS_HTML_DEST_PATH/cimTypeToString.html
%attr(-,root,root) %PEGASUS_HTML_DEST_PATH/CIMValue.html
%attr(-,root,root) %PEGASUS_HTML_DEST_PATH/ClientClassHier.html
%attr(-,root,root) %PEGASUS_HTML_DEST_PATH/CMAddArg.html
%attr(-,root,root) %PEGASUS_HTML_DEST_PATH/CMAddContextEntry.html
%attr(-,root,root) %PEGASUS_HTML_DEST_PATH/CMAddKey.html
%attr(-,root,root) %PEGASUS_HTML_DEST_PATH/CMAssociationMIFactory.html
%attr(-,root,root) %PEGASUS_HTML_DEST_PATH/CMAssociationMIStub.html
%attr(-,root,root) %PEGASUS_HTML_DEST_PATH/CMClassPathIsA.html
%attr(-,root,root) %PEGASUS_HTML_DEST_PATH/CMEvaluatePredicate.html
%attr(-,root,root) %PEGASUS_HTML_DEST_PATH/CMEvaluateSelExp.html
%attr(-,root,root) %PEGASUS_HTML_DEST_PATH/CMGetArgAt.html
%attr(-,root,root) %PEGASUS_HTML_DEST_PATH/CMGetArgCount.html
%attr(-,root,root) %PEGASUS_HTML_DEST_PATH/CMGetArg.html
%attr(-,root,root) %PEGASUS_HTML_DEST_PATH/CMGetArrayCount.html
%attr(-,root,root) %PEGASUS_HTML_DEST_PATH/CMGetArrayElementAt.html
%attr(-,root,root) %PEGASUS_HTML_DEST_PATH/CMGetArrayType.html
%attr(-,root,root) %PEGASUS_HTML_DEST_PATH/CMGetBinaryFormat.html
%attr(-,root,root) %PEGASUS_HTML_DEST_PATH/CMGetCharsPtr.html
%attr(-,root,root) %PEGASUS_HTML_DEST_PATH/CMGetClassName.html
%attr(-,root,root) %PEGASUS_HTML_DEST_PATH/CMGetCod.html
%attr(-,root,root) %PEGASUS_HTML_DEST_PATH/CMGetContextEntryAt.html
%attr(-,root,root) %PEGASUS_HTML_DEST_PATH/CMGetContextEntryCount.html
%attr(-,root,root) %PEGASUS_HTML_DEST_PATH/CMGetContextEntry.html
%attr(-,root,root) %PEGASUS_HTML_DEST_PATH/CMGetDoc.html
%attr(-,root,root) %PEGASUS_HTML_DEST_PATH/CMGetHostname.html
%attr(-,root,root) %PEGASUS_HTML_DEST_PATH/CMGetKeyAt.html
%attr(-,root,root) %PEGASUS_HTML_DEST_PATH/CMGetKeyCount.html
%attr(-,root,root) %PEGASUS_HTML_DEST_PATH/CMGetKey.html
%attr(-,root,root) %PEGASUS_HTML_DEST_PATH/CMGetNameSpace.html
%attr(-,root,root) %PEGASUS_HTML_DEST_PATH/CMGetNext.html
%attr(-,root,root) %PEGASUS_HTML_DEST_PATH/CMGetObjectPath.html
%attr(-,root,root) %PEGASUS_HTML_DEST_PATH/CMGetPredicateAt.html
%attr(-,root,root) %PEGASUS_HTML_DEST_PATH/CMGetPredicateCount.html
%attr(-,root,root) %PEGASUS_HTML_DEST_PATH/CMGetPredicateData.html
%attr(-,root,root) %PEGASUS_HTML_DEST_PATH/CMGetPredicate.html
%attr(-,root,root) %PEGASUS_HTML_DEST_PATH/CMGetPropertyAt.html
%attr(-,root,root) %PEGASUS_HTML_DEST_PATH/CMGetPropertyCount.html
%attr(-,root,root) %PEGASUS_HTML_DEST_PATH/CMGetProperty.html
%attr(-,root,root) %PEGASUS_HTML_DEST_PATH/CMGetSelExpString.html
%attr(-,root,root) %PEGASUS_HTML_DEST_PATH/CMGetStringFormat.html
%attr(-,root,root) %PEGASUS_HTML_DEST_PATH/CMGetSubCondAt.html
%attr(-,root,root) %PEGASUS_HTML_DEST_PATH/CMGetSubCondCountAndType.html
%attr(-,root,root) %PEGASUS_HTML_DEST_PATH/CMHasNext.html
%attr(-,root,root) %PEGASUS_HTML_DEST_PATH/CMIndicationMIStub.html
%attr(-,root,root) %PEGASUS_HTML_DEST_PATH/CMInstanceMIFactory.html
%attr(-,root,root) %PEGASUS_HTML_DEST_PATH/CMInstanceMIStub.html
%attr(-,root,root) %PEGASUS_HTML_DEST_PATH/CMIsArray.html
%attr(-,root,root) %PEGASUS_HTML_DEST_PATH/CMIsInterval.html
%attr(-,root,root) %PEGASUS_HTML_DEST_PATH/CMIsKeyValue.html
%attr(-,root,root) %PEGASUS_HTML_DEST_PATH/CMIsNullObject.html
%attr(-,root,root) %PEGASUS_HTML_DEST_PATH/CMIsNullValue.html
%attr(-,root,root) %PEGASUS_HTML_DEST_PATH/CMMethodMIFactory.html
%attr(-,root,root) %PEGASUS_HTML_DEST_PATH/CMMethodMIStub.html
%attr(-,root,root) %PEGASUS_HTML_DEST_PATH/CMNewArgs.html
%attr(-,root,root) %PEGASUS_HTML_DEST_PATH/CMNewArray.html
%attr(-,root,root) %PEGASUS_HTML_DEST_PATH/CMNewDateTimeFromBinary.html
%attr(-,root,root) %PEGASUS_HTML_DEST_PATH/CMNewDateTimeFromChars.html
%attr(-,root,root) %PEGASUS_HTML_DEST_PATH/CMNewDateTime.html
%attr(-,root,root) %PEGASUS_HTML_DEST_PATH/CMNewInstance.html
%attr(-,root,root) %PEGASUS_HTML_DEST_PATH/CMNewObjectPath.html
%attr(-,root,root) %PEGASUS_HTML_DEST_PATH/CMNewSelectExp.html
%attr(-,root,root) %PEGASUS_HTML_DEST_PATH/CMNewString.html
%attr(-,root,root) %PEGASUS_HTML_DEST_PATH/_CMPIArgsFT.html
%attr(-,root,root) %PEGASUS_HTML_DEST_PATH/CmpiArgs.html
%attr(-,root,root) %PEGASUS_HTML_DEST_PATH/_CMPIArgs.html
%attr(-,root,root) %PEGASUS_HTML_DEST_PATH/_CMPIArrayFT.html
%attr(-,root,root) %PEGASUS_HTML_DEST_PATH/CmpiArray.html
%attr(-,root,root) %PEGASUS_HTML_DEST_PATH/_CMPIArray.html
%attr(-,root,root) %PEGASUS_HTML_DEST_PATH/_CMPIAssociationMIFT.html
%attr(-,root,root) %PEGASUS_HTML_DEST_PATH/CMPIAssociationMIFT.html
%attr(-,root,root) %PEGASUS_HTML_DEST_PATH/CmpiBooleanData.html
%attr(-,root,root) %PEGASUS_HTML_DEST_PATH/_CMPIBrokerEncFT.html
%attr(-,root,root) %PEGASUS_HTML_DEST_PATH/_CMPIBrokerExtFT.html
%attr(-,root,root) %PEGASUS_HTML_DEST_PATH/_CMPIBrokerFT.html
%attr(-,root,root) %PEGASUS_HTML_DEST_PATH/CmpiBroker.html
%attr(-,root,root) %PEGASUS_HTML_DEST_PATH/_CMPIBroker.html
%attr(-,root,root) %PEGASUS_HTML_DEST_PATH/CmpiCharData.html
%attr(-,root,root) %PEGASUS_HTML_DEST_PATH/CMPI_C_Hier.html
%attr(-,root,root) %PEGASUS_HTML_DEST_PATH/CMPIClassHier.html
%attr(-,root,root) %PEGASUS_HTML_DEST_PATH/_CMPIContextFT.html
%attr(-,root,root) %PEGASUS_HTML_DEST_PATH/CmpiContext.html
%attr(-,root,root) %PEGASUS_HTML_DEST_PATH/_CMPIContext.html
%attr(-,root,root) %PEGASUS_HTML_DEST_PATH/CmpiData.html
%attr(-,root,root) %PEGASUS_HTML_DEST_PATH/_CMPIDateTimeFT.html
%attr(-,root,root) %PEGASUS_HTML_DEST_PATH/CmpiDateTime.html
%attr(-,root,root) %PEGASUS_HTML_DEST_PATH/_CMPIDateTime.html
%attr(-,root,root) %PEGASUS_HTML_DEST_PATH/_CMPIEnumerationFT.html
%attr(-,root,root) %PEGASUS_HTML_DEST_PATH/CmpiEnumeration.html
%attr(-,root,root) %PEGASUS_HTML_DEST_PATH/_CMPIEnumeration.html
%attr(-,root,root) %PEGASUS_HTML_DEST_PATH/_CMPIIndicationMIFT.html
%attr(-,root,root) %PEGASUS_HTML_DEST_PATH/CMPIIndicationMIFT.html
%attr(-,root,root) %PEGASUS_HTML_DEST_PATH/_CMPIInstanceFT.html
%attr(-,root,root) %PEGASUS_HTML_DEST_PATH/CmpiInstance.html
%attr(-,root,root) %PEGASUS_HTML_DEST_PATH/_CMPIInstance.html
%attr(-,root,root) %PEGASUS_HTML_DEST_PATH/_CMPIInstanceMIFT.html
%attr(-,root,root) %PEGASUS_HTML_DEST_PATH/CMPIInstanceMIFT.html
%attr(-,root,root) %PEGASUS_HTML_DEST_PATH/_CMPIMethodMIFT.html
%attr(-,root,root) %PEGASUS_HTML_DEST_PATH/CMPIMethodMIFT.html
%attr(-,root,root) %PEGASUS_HTML_DEST_PATH/CmpiObject.html
%attr(-,root,root) %PEGASUS_HTML_DEST_PATH/_CMPIObjectPathFT.html
%attr(-,root,root) %PEGASUS_HTML_DEST_PATH/CmpiObjectPath.html
%attr(-,root,root) %PEGASUS_HTML_DEST_PATH/_CMPIObjectPath.html
%attr(-,root,root) %PEGASUS_HTML_DEST_PATH/_CMPIPredicateFT.html
%attr(-,root,root) %PEGASUS_HTML_DEST_PATH/_CMPIPredicate.html
%attr(-,root,root) %PEGASUS_HTML_DEST_PATH/_CMPIPropertyMIFT.html
%attr(-,root,root) %PEGASUS_HTML_DEST_PATH/CMPIPropertyMIFT.html
%attr(-,root,root) %PEGASUS_HTML_DEST_PATH/_CMPIResultFT.html
%attr(-,root,root) %PEGASUS_HTML_DEST_PATH/CmpiResult.html
%attr(-,root,root) %PEGASUS_HTML_DEST_PATH/_CMPIResult.html
%attr(-,root,root) %PEGASUS_HTML_DEST_PATH/_CMPISelectCondFT.html
%attr(-,root,root) %PEGASUS_HTML_DEST_PATH/_CMPISelectCond.html
%attr(-,root,root) %PEGASUS_HTML_DEST_PATH/_CMPISelectExpFT.html
%attr(-,root,root) %PEGASUS_HTML_DEST_PATH/CmpiSelectExp.html
%attr(-,root,root) %PEGASUS_HTML_DEST_PATH/_CMPISelectExp.html
%attr(-,root,root) %PEGASUS_HTML_DEST_PATH/CmpiStatus.html
%attr(-,root,root) %PEGASUS_HTML_DEST_PATH/_CMPIStringFT.html
%attr(-,root,root) %PEGASUS_HTML_DEST_PATH/CmpiString.html
%attr(-,root,root) %PEGASUS_HTML_DEST_PATH/_CMPIString.html
%attr(-,root,root) %PEGASUS_HTML_DEST_PATH/_CMPISubCondFT.html
%attr(-,root,root) %PEGASUS_HTML_DEST_PATH/_CMPISubCond.html
%attr(-,root,root) %PEGASUS_HTML_DEST_PATH/CMPropertyMIFactory.html
%attr(-,root,root) %PEGASUS_HTML_DEST_PATH/CMPropertyMIStub.html
%attr(-,root,root) %PEGASUS_HTML_DEST_PATH/CMReturnData.html
%attr(-,root,root) %PEGASUS_HTML_DEST_PATH/CMReturnDone.html
%attr(-,root,root) %PEGASUS_HTML_DEST_PATH/CMReturn.html
%attr(-,root,root) %PEGASUS_HTML_DEST_PATH/CMReturnInstance.html
%attr(-,root,root) %PEGASUS_HTML_DEST_PATH/CMReturnObjectPath.html
%attr(-,root,root) %PEGASUS_HTML_DEST_PATH/CMReturnWithChars.html
%attr(-,root,root) %PEGASUS_HTML_DEST_PATH/CMReturnWithString.html
%attr(-,root,root) %PEGASUS_HTML_DEST_PATH/CMSetArrayElementAt.html
%attr(-,root,root) %PEGASUS_HTML_DEST_PATH/CMSetClassName.html
%attr(-,root,root) %PEGASUS_HTML_DEST_PATH/CMSetHostAndNameSpaceFromObjectPath.html
%attr(-,root,root) %PEGASUS_HTML_DEST_PATH/CMSetHostname.html
%attr(-,root,root) %PEGASUS_HTML_DEST_PATH/CMSetNameSpaceFromObjectPath.html
%attr(-,root,root) %PEGASUS_HTML_DEST_PATH/CMSetNameSpace.html
%attr(-,root,root) %PEGASUS_HTML_DEST_PATH/CMSetPropertyFilter.html
%attr(-,root,root) %PEGASUS_HTML_DEST_PATH/CMSetProperty.html
%attr(-,root,root) %PEGASUS_HTML_DEST_PATH/CMSetStatus.html
%attr(-,root,root) %PEGASUS_HTML_DEST_PATH/CMSetStatusWithChars.html
%attr(-,root,root) %PEGASUS_HTML_DEST_PATH/CMSetStatusWithString.html
%attr(-,root,root) %PEGASUS_HTML_DEST_PATH/CMToArray.html
%attr(-,root,root) %PEGASUS_HTML_DEST_PATH/CommonClassHier.html
%attr(-,root,root) %PEGASUS_HTML_DEST_PATH/ConnectionTimeoutException.html
%attr(-,root,root) %PEGASUS_HTML_DEST_PATH/Container.html
%attr(-,root,root) %PEGASUS_HTML_DEST_PATH/ContentLanguageListContainer.html
%attr(-,root,root) %PEGASUS_HTML_DEST_PATH/CString.html
%attr(-,root,root) %PEGASUS_HTML_DEST_PATH/DateTimeOutOfRangeException.html
%attr(-,root,root) %PEGASUS_HTML_DEST_PATH/DynamicCastFailedException.html
%attr(-,root,root) %PEGASUS_HTML_DEST_PATH/Exception.html
%attr(-,root,root) %PEGASUS_HTML_DEST_PATH/Formatter.html
%attr(-,root,root) %PEGASUS_HTML_DEST_PATH/General.html
%attr(-,root,root) %PEGASUS_HTML_DEST_PATH/HIER.html
%attr(-,root,root) %PEGASUS_HTML_DEST_PATH/icon1.gif
%attr(-,root,root) %PEGASUS_HTML_DEST_PATH/icon2.gif
%attr(-,root,root) %PEGASUS_HTML_DEST_PATH/IdentityContainer.html
%attr(-,root,root) %PEGASUS_HTML_DEST_PATH/index.html
%attr(-,root,root) %PEGASUS_HTML_DEST_PATH/IndexOutOfBoundsException.html
%attr(-,root,root) %PEGASUS_HTML_DEST_PATH/IndicationConsumerClassHier.html
%attr(-,root,root) %PEGASUS_HTML_DEST_PATH/IndicationResponseHandler.html
%attr(-,root,root) %PEGASUS_HTML_DEST_PATH/InstanceResponseHandler.html
%attr(-,root,root) %PEGASUS_HTML_DEST_PATH/InvalidDateTimeFormatException.html
%attr(-,root,root) %PEGASUS_HTML_DEST_PATH/InvalidLocatorException.html
%attr(-,root,root) %PEGASUS_HTML_DEST_PATH/InvalidNameException.html
%attr(-,root,root) %PEGASUS_HTML_DEST_PATH/InvalidNamespaceNameException.html
%attr(-,root,root) %PEGASUS_HTML_DEST_PATH/LanguageElementContainer.html
%attr(-,root,root) %PEGASUS_HTML_DEST_PATH/LanguageElement.html
%attr(-,root,root) %PEGASUS_HTML_DEST_PATH/MalformedObjectNameException.html
%attr(-,root,root) %PEGASUS_HTML_DEST_PATH/MessageLoaderParms.html
%attr(-,root,root) %PEGASUS_HTML_DEST_PATH/MethodResultResponseHandler.html
%attr(-,root,root) %PEGASUS_HTML_DEST_PATH/NotConnectedException.html
%attr(-,root,root) %PEGASUS_HTML_DEST_PATH/ObjectPathResponseHandler.html
%attr(-,root,root) %PEGASUS_HTML_DEST_PATH/ObjectResponseHandler.html
%attr(-,root,root) %PEGASUS_HTML_DEST_PATH/OperationContext.html
%attr(-,root,root) %PEGASUS_HTML_DEST_PATH/operator.10.html
%attr(-,root,root) %PEGASUS_HTML_DEST_PATH/operator.11.html
%attr(-,root,root) %PEGASUS_HTML_DEST_PATH/operator.12.html
%attr(-,root,root) %PEGASUS_HTML_DEST_PATH/operator.2.html
%attr(-,root,root) %PEGASUS_HTML_DEST_PATH/operator.3.html
%attr(-,root,root) %PEGASUS_HTML_DEST_PATH/operator.4.html
%attr(-,root,root) %PEGASUS_HTML_DEST_PATH/operator.5.html
%attr(-,root,root) %PEGASUS_HTML_DEST_PATH/operator.6.html
%attr(-,root,root) %PEGASUS_HTML_DEST_PATH/operator.7.html
%attr(-,root,root) %PEGASUS_HTML_DEST_PATH/operator.8.html
%attr(-,root,root) %PEGASUS_HTML_DEST_PATH/operator.9.html
%attr(-,root,root) %PEGASUS_HTML_DEST_PATH/operator.html
%attr(-,root,root) %PEGASUS_HTML_DEST_PATH/ostream.html
%attr(-,root,root) %PEGASUS_HTML_DEST_PATH/Pegasus_Array_h.html
%attr(-,root,root) %PEGASUS_HTML_DEST_PATH/Pegasus_MessageLoader_h.html
%attr(-,root,root) %PEGASUS_HTML_DEST_PATH/ProviderClassHier.html
%attr(-,root,root) %PEGASUS_HTML_DEST_PATH/ResponseHandler.html
%attr(-,root,root) %PEGASUS_HTML_DEST_PATH/SSLCallbackInfo.html
%attr(-,root,root) %PEGASUS_HTML_DEST_PATH/SSLCertificateInfo.html
%attr(-,root,root) %PEGASUS_HTML_DEST_PATH/SSLContext.html
%attr(-,root,root) %PEGASUS_HTML_DEST_PATH/SSLException.htm
%attr(-,root,root) %PEGASUS_HTML_DEST_PATH/String.html
%attr(-,root,root) %PEGASUS_HTML_DEST_PATH/SubscriptionInstanceNamesContainer.html
%attr(-,root,root) %PEGASUS_HTML_DEST_PATH/SubscriptionLanguageListContainer.html
%attr(-,root,root) %PEGASUS_HTML_DEST_PATH/TimeoutContainer.html
%attr(-,root,root) %PEGASUS_HTML_DEST_PATH/TypeMismatchException.html
%attr(-,root,root) %PEGASUS_HTML_DEST_PATH/UninitializedObjectException.html
%attr(-,root,root) %PEGASUS_SAMPLES_DEST_PATH/Makefile 
%attr(-,root,root) %PEGASUS_SAMPLES_DEST_PATH/mak/common.mak 
%attr(-,root,root) %PEGASUS_SAMPLES_DEST_PATH/mak/config.mak 
%attr(-,root,root) %PEGASUS_SAMPLES_DEST_PATH/mak/library.mak 
%ifarch ia64
%attr(-,root,root) %PEGASUS_SAMPLES_DEST_PATH/mak/LINUX_IA64_GNU.mak
%else
%attr(-,root,root) %PEGASUS_SAMPLES_DEST_PATH/mak/LINUX_IX86_GNU.mak
%endif
%attr(-,root,root) %PEGASUS_SAMPLES_DEST_PATH/mak/program.mak 
%attr(-,root,root) %PEGASUS_SAMPLES_DEST_PATH/mak/recurse.mak 
%attr(-,root,root) %PEGASUS_SAMPLES_DEST_PATH/Clients/Makefile 
%attr(-,root,root) %PEGASUS_SAMPLES_DEST_PATH/Clients/DefaultC++/Makefile
%attr(-,root,root) %PEGASUS_SAMPLES_DEST_PATH/Clients/DefaultC++/EnumInstances/EnumInstances.cpp 
%attr(-,root,root) %PEGASUS_SAMPLES_DEST_PATH/Clients/DefaultC++/EnumInstances/Makefile 
%attr(-,root,root) %PEGASUS_SAMPLES_DEST_PATH/Clients/DefaultC++/InvokeMethod/InvokeMethod.cpp 
%attr(-,root,root) %PEGASUS_SAMPLES_DEST_PATH/Clients/DefaultC++/InvokeMethod/Makefile 
%attr(-,root,root) %PEGASUS_SAMPLES_DEST_PATH/Providers/Makefile
%attr(-,root,root) %PEGASUS_SAMPLES_DEST_PATH/Providers/CMPI/Makefile
%attr(-,root,root) %PEGASUS_SAMPLES_DEST_PATH/Providers/CMPI/FilesAndDirectories/CWS_Directory.c 
%attr(-,root,root) %PEGASUS_SAMPLES_DEST_PATH/Providers/CMPI/FilesAndDirectories/CWS_DirectoryContainsFile.c 
%attr(-,root,root) %PEGASUS_SAMPLES_DEST_PATH/Providers/CMPI/FilesAndDirectories/CWS_FileUtils.c 
%attr(-,root,root) %PEGASUS_SAMPLES_DEST_PATH/Providers/CMPI/FilesAndDirectories/CWS_FileUtils.h 
%attr(-,root,root) %PEGASUS_SAMPLES_DEST_PATH/Providers/CMPI/FilesAndDirectories/CWS_PlainFile.c 
%attr(-,root,root) %PEGASUS_SAMPLES_DEST_PATH/Providers/CMPI/FilesAndDirectories/cwssimdata.c 
%attr(-,root,root) %PEGASUS_SAMPLES_DEST_PATH/Providers/CMPI/FilesAndDirectories/cwstest.c 
%attr(-,root,root) %PEGASUS_SAMPLES_DEST_PATH/Providers/CMPI/FilesAndDirectories/cwsutil.c 
%attr(-,root,root) %PEGASUS_SAMPLES_DEST_PATH/Providers/CMPI/FilesAndDirectories/cwsutil.h 
%attr(-,root,root) %PEGASUS_SAMPLES_DEST_PATH/Providers/CMPI/FilesAndDirectories/Makefile 
%attr(-,root,root) %PEGASUS_SAMPLES_DEST_PATH/Providers/DefaultC++/Makefile
%attr(-,root,root) %PEGASUS_SAMPLES_DEST_PATH/Providers/DefaultC++/InstanceProvider/EnumerateInstancesrspgood.xml 
%attr(-,root,root) %PEGASUS_SAMPLES_DEST_PATH/Providers/DefaultC++/InstanceProvider/EnumerateInstances.xml 
%attr(-,root,root) %PEGASUS_SAMPLES_DEST_PATH/Providers/DefaultC++/InstanceProvider/InstanceProvider.cpp 
%attr(-,root,root) %PEGASUS_SAMPLES_DEST_PATH/Providers/DefaultC++/InstanceProvider/InstanceProvider.h 
%attr(-,root,root) %PEGASUS_SAMPLES_DEST_PATH/Providers/DefaultC++/InstanceProvider/InstanceProviderMain.cpp 
%attr(-,root,root) %PEGASUS_SAMPLES_DEST_PATH/Providers/DefaultC++/InstanceProvider/Makefile 
%attr(-,root,root) %PEGASUS_SAMPLES_DEST_PATH/Providers/DefaultC++/MethodProvider/InvokeMethodrspgood.xml 
%attr(-,root,root) %PEGASUS_SAMPLES_DEST_PATH/Providers/DefaultC++/MethodProvider/InvokeMethod.xml 
%attr(-,root,root) %PEGASUS_SAMPLES_DEST_PATH/Providers/DefaultC++/MethodProvider/Makefile 
%attr(-,root,root) %PEGASUS_SAMPLES_DEST_PATH/Providers/DefaultC++/MethodProvider/MethodProvider.cpp 
%attr(-,root,root) %PEGASUS_SAMPLES_DEST_PATH/Providers/DefaultC++/MethodProvider/MethodProvider.h 
%attr(-,root,root) %PEGASUS_SAMPLES_DEST_PATH/Providers/DefaultC++/MethodProvider/MethodProviderMain.cpp 
%attr(-,root,root) %PEGASUS_SAMPLES_DEST_PATH/Providers/CMPI/FilesAndDirectories/tests/AssocDirNames12001rspgood.xml 
%attr(-,root,root) %PEGASUS_SAMPLES_DEST_PATH/Providers/CMPI/FilesAndDirectories/tests/AssocDirNames12001.xml 
%attr(-,root,root) %PEGASUS_SAMPLES_DEST_PATH/Providers/CMPI/FilesAndDirectories/tests/DeleteDirInstance10003rspgood.xml 
%attr(-,root,root) %PEGASUS_SAMPLES_DEST_PATH/Providers/CMPI/FilesAndDirectories/tests/DeleteDirInstance10003.xml 
%attr(-,root,root) %PEGASUS_SAMPLES_DEST_PATH/Providers/CMPI/FilesAndDirectories/tests/EnumerateDir10001rspgood.xml 
%attr(-,root,root) %PEGASUS_SAMPLES_DEST_PATH/Providers/CMPI/FilesAndDirectories/tests/EnumerateDir10001.xml 
%attr(-,root,root) %PEGASUS_SAMPLES_DEST_PATH/Providers/CMPI/FilesAndDirectories/tests/EnumerateDirNames10000rspgood.xml 
%attr(-,root,root) %PEGASUS_SAMPLES_DEST_PATH/Providers/CMPI/FilesAndDirectories/tests/EnumerateDirNames10000.xml 
%attr(-,root,root) %PEGASUS_SAMPLES_DEST_PATH/Providers/CMPI/FilesAndDirectories/tests/GetDirInstance10002rspgood.xml 
%attr(-,root,root) %PEGASUS_SAMPLES_DEST_PATH/Providers/CMPI/FilesAndDirectories/tests/GetDirInstance10002.xml 
%attr(-,root,root) %PEGASUS_SAMPLES_DEST_PATH/Providers/CMPI/FilesAndDirectories/tests/Makefile 
%attr(-,root,root) %PEGASUS_SAMPLES_DEST_PATH/Providers/CMPI/FilesAndDirectories/tests/PlainFileMethodCall11001rspgood.xml 
%attr(-,root,root) %PEGASUS_SAMPLES_DEST_PATH/Providers/CMPI/FilesAndDirectories/tests/PlainFileMethodCall11001.xml 
%attr(-,root,root) %PEGASUS_SAMPLES_DEST_PATH/Providers/CMPI/FilesAndDirectories/tests/ReferenceDirNames12002rspgood.xml 
%attr(-,root,root) %PEGASUS_SAMPLES_DEST_PATH/Providers/CMPI/FilesAndDirectories/tests/ReferenceDirNames12002.xml 
%attr(-,root,root) %PEGASUS_SAMPLES_DEST_PATH/Providers/Load/Makefile
%attr(-,root,root) %PEGASUS_SAMPLES_DEST_PATH/Providers/Load/CWS_FilesAndDir.mof
%attr(-,root,root) %PEGASUS_SAMPLES_DEST_PATH/Providers/Load/CWS_FilesAndDirR.mof
%attr(-,root,root) %PEGASUS_SAMPLES_DEST_PATH/Providers/Load/InstanceProviderR.mof
%attr(-,root,root) %PEGASUS_SAMPLES_DEST_PATH/Providers/Load/MethodProviderR.mof
%attr(-,root,root) %PEGASUS_SAMPLES_DEST_PATH/Providers/Load/SampleProviderSchema.mof
