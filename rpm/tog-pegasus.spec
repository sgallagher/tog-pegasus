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
Summary: OpenPegasus WBEM Services for Linux
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
OpenPegasus WBEM Services for Linux enables management solutions that deliver
increased control of enterprise resources. WBEM is a platform and resource
independent DMTF standard that defines a common information model and
communication protocol for monitoring and controlling resources from diverse
sources.

%package sdk
Summary:      The OpenPegasus Software Development Kit
Group:        Systems Management/Base
Autoreq: 0
Requires: tog-pegasus >= 2.4, openssl-devel >= 0.9.6

%description sdk
The OpenPegasus WBEM Services for Linux SDK is the developer's kit for the OpenPegasus WBEM
Services for Linux release. It provides Linux C++ developers with the WBEM files required to
build WBEM Clients and Providers. It also supports C provider developers via the CMPI interface.

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
%define PEGASUS_PLATFORM LINUX_IA64_GNU
%else
%define PEGASUS_PLATFORM LINUX_IX86_GNU
%endif
export PEGASUS_PLATFORM=%PEGASUS_PLATFORM

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
install -D -m 0444  $PEGASUS_ROOT/rpm/manLinux/man8.Z/cimauth.8 $RPM_BUILD_ROOT%PEGASUS_MANADMIN_DIR/cimauth.8
install -D -m 0444  $PEGASUS_ROOT/rpm/manLinux/man8.Z/cimconfig.8 $RPM_BUILD_ROOT%PEGASUS_MANADMIN_DIR/cimconfig.8
install -D -m 0444  $PEGASUS_ROOT/rpm/manLinux/man8.Z/cimserver.8 $RPM_BUILD_ROOT%PEGASUS_MANADMIN_DIR/cimserver.8

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
install -D -m 0444 %SDK_STAGE_LOC/include/Pegasus/Common/Platform_%PEGASUS_PLATFORM.h %INCLUDE_DEST_PATH/Pegasus/Common/Platform_%PEGASUS_PLATFORM.h
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

cp -rf %SDK_STAGE_LOC/html $RPM_BUILD_ROOT%PEGASUS_PRODSHARE_DIR/

install -D -m 0444 %SDK_STAGE_LOC/samples/Makefile %SAMPLES_DEST_PATH/Makefile 
install -D -m 0444 %SDK_STAGE_LOC/samples/mak/common.mak %SAMPLES_DEST_PATH/mak/common.mak 

echo "PEGASUS_DEST_LIB_DIR =   "%PEGASUS_DEST_LIB_DIR > sampleconfig.txt
echo "PEGASUS_VARDATA_DIR =    "%PEGASUS_VARDATA_DIR >> sampleconfig.txt
echo "PEGASUS_PROVIDER_LIB_DIR="%PEGASUS_PROVIDER_LIB_DIR >> sampleconfig.txt
echo "PEGASUS_MOF_DIR =        "%PEGASUS_MOF_DIR>> sampleconfig.txt
echo "PEGASUS_INCLUDE_DIR =    "%PEGASUS_INCLUDE_DEST_PATH >> sampleconfig.txt
echo "PEGASUS_SAMPLES_DIR =    "%PEGASUS_SAMPLES_DEST_PATH >> sampleconfig.txt
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
HOSTNAME=`uname -n`
echo "[ req ]" > %PEGASUS_CONFIG_DIR/ssl.cnf
echo "distinguished_name     = req_distinguished_name"  >> %PEGASUS_CONFIG_DIR/ssl.cnf
echo "prompt                 = no"  >> %PEGASUS_CONFIG_DIR/ssl.cnf
echo "[ req_distinguished_name ]" >> %PEGASUS_CONFIG_DIR/ssl.cnf
echo "C                      = UK" >> %PEGASUS_CONFIG_DIR/ssl.cnf
echo "ST                     = Berkshire" >> %PEGASUS_CONFIG_DIR/ssl.cnf
echo "L                      = Reading" >> %PEGASUS_CONFIG_DIR/ssl.cnf
echo "O                      = The Open Group" >> %PEGASUS_CONFIG_DIR/ssl.cnf
echo "OU                     = The OpenPegasus Project" >> %PEGASUS_CONFIG_DIR/ssl.cnf
echo "CN                     = $HOSTNAME" >> %PEGASUS_CONFIG_DIR/ssl.cnf
chmod 400 %PEGASUS_CONFIG_DIR/ssl.cnf
chown root %PEGASUS_CONFIG_DIR/ssl.cnf
chgrp root %PEGASUS_CONFIG_DIR/ssl.cnf

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
%attr(-,root,root) %PEGASUS_INCLUDE_DEST_PATH/Pegasus/Common/Platform_%PEGASUS_PLATFORM.h
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
%PEGASUS_HTML_DEST_PATH
%attr(-,root,root) %PEGASUS_SAMPLES_DEST_PATH/Makefile 
%attr(-,root,root) %PEGASUS_SAMPLES_DEST_PATH/mak/common.mak 
%attr(-,root,root) %PEGASUS_SAMPLES_DEST_PATH/mak/config.mak 
%attr(-,root,root) %PEGASUS_SAMPLES_DEST_PATH/mak/library.mak 
%attr(-,root,root) %PEGASUS_SAMPLES_DEST_PATH/mak/%PEGASUS_PLATFORM.mak
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
