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
install -D -m 0555  $PEGASUS_HOME/bin/cimconfig $RPM_BUILD_ROOT%PEGASUS_SBIN_DIR/cimconfig
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
install -D -m 0644  $PEGASUS_ROOT/Schemas/CIM28/Application28_AppRuntime.mof $RPM_BUILD_ROOT%PEGASUS_MOF_DIR/CIM28/Application28_AppRuntime.mof
install -D -m 0644  $PEGASUS_ROOT/Schemas/CIM28/Application28_BIOS.mof $RPM_BUILD_ROOT%PEGASUS_MOF_DIR/CIM28/Application28_BIOS.mof
install -D -m 0644  $PEGASUS_ROOT/Schemas/CIM28/Application28_CheckAction.mof $RPM_BUILD_ROOT%PEGASUS_MOF_DIR/CIM28/Application28_CheckAction.mof
install -D -m 0644  $PEGASUS_ROOT/Schemas/CIM28/Application28_DeploymentModel.mof $RPM_BUILD_ROOT%PEGASUS_MOF_DIR/CIM28/Application28_DeploymentModel.mof
install -D -m 0644  $PEGASUS_ROOT/Schemas/CIM28/Application28_InstalledProduct.mof $RPM_BUILD_ROOT%PEGASUS_MOF_DIR/CIM28/Application28_InstalledProduct.mof
install -D -m 0644  $PEGASUS_ROOT/Schemas/CIM28/Application28_J2eeAppServer.mof $RPM_BUILD_ROOT%PEGASUS_MOF_DIR/CIM28/Application28_J2eeAppServer.mof
install -D -m 0644  $PEGASUS_ROOT/Schemas/CIM28/Application28_J2eeAppServerStats.mof $RPM_BUILD_ROOT%PEGASUS_MOF_DIR/CIM28/Application28_J2eeAppServerStats.mof
install -D -m 0644  $PEGASUS_ROOT/Schemas/CIM28/Application28_SystemSoftware.mof $RPM_BUILD_ROOT%PEGASUS_MOF_DIR/CIM28/Application28_SystemSoftware.mof
install -D -m 0644  $PEGASUS_ROOT/Schemas/CIM28/CIM_Application28.mof $RPM_BUILD_ROOT%PEGASUS_MOF_DIR/CIM28/CIM_Application28.mof
install -D -m 0644  $PEGASUS_ROOT/Schemas/CIM28/CIM_Core28.mof $RPM_BUILD_ROOT%PEGASUS_MOF_DIR/CIM28/CIM_Core28.mof
install -D -m 0644  $PEGASUS_ROOT/Schemas/CIM28/CIM_Database28.mof $RPM_BUILD_ROOT%PEGASUS_MOF_DIR/CIM28/CIM_Database28.mof
install -D -m 0644  $PEGASUS_ROOT/Schemas/CIM28/CIM_Device28.mof $RPM_BUILD_ROOT%PEGASUS_MOF_DIR/CIM28/CIM_Device28.mof
install -D -m 0644  $PEGASUS_ROOT/Schemas/CIM28/CIM_Event28.mof $RPM_BUILD_ROOT%PEGASUS_MOF_DIR/CIM28/CIM_Event28.mof
install -D -m 0644  $PEGASUS_ROOT/Schemas/CIM28/CIM_Interop28.mof $RPM_BUILD_ROOT%PEGASUS_MOF_DIR/CIM28/CIM_Interop28.mof 
install -D -m 0644  $PEGASUS_ROOT/Schemas/CIM28/CIM_IPsecPolicy28.mof $RPM_BUILD_ROOT%PEGASUS_MOF_DIR/CIM28/CIM_IPsecPolicy28.mof 
install -D -m 0644  $PEGASUS_ROOT/Schemas/CIM28/CIM_Metrics28.mof $RPM_BUILD_ROOT%PEGASUS_MOF_DIR/CIM28/CIM_Metrics28.mof
install -D -m 0644  $PEGASUS_ROOT/Schemas/CIM28/CIM_Network28.mof $RPM_BUILD_ROOT%PEGASUS_MOF_DIR/CIM28/CIM_Network28.mof
install -D -m 0644  $PEGASUS_ROOT/Schemas/CIM28/CIM_Physical28.mof $RPM_BUILD_ROOT%PEGASUS_MOF_DIR/CIM28/CIM_Physical28.mof
install -D -m 0644  $PEGASUS_ROOT/Schemas/CIM28/CIM_Policy28.mof $RPM_BUILD_ROOT%PEGASUS_MOF_DIR/CIM28/CIM_Policy28.mof
install -D -m 0644  $PEGASUS_ROOT/Schemas/CIM28/CIM_Schema28.mof $RPM_BUILD_ROOT%PEGASUS_MOF_DIR/CIM28/CIM_Schema28.mof
install -D -m 0644  $PEGASUS_ROOT/Schemas/CIM28/CIM_Support28.mof $RPM_BUILD_ROOT%PEGASUS_MOF_DIR/CIM28/CIM_Support28.mof
install -D -m 0644  $PEGASUS_ROOT/Schemas/CIM28/CIM_System28.mof $RPM_BUILD_ROOT%PEGASUS_MOF_DIR/CIM28/CIM_System28.mof
install -D -m 0644  $PEGASUS_ROOT/Schemas/CIM28/CIM_User28.mof $RPM_BUILD_ROOT%PEGASUS_MOF_DIR/CIM28/CIM_User28.mof
install -D -m 0644  $PEGASUS_ROOT/Schemas/CIM28/Core28_Capabilities.mof $RPM_BUILD_ROOT%PEGASUS_MOF_DIR/CIM28/Core28_Capabilities.mof
install -D -m 0644  $PEGASUS_ROOT/Schemas/CIM28/Core28_Collection.mof $RPM_BUILD_ROOT%PEGASUS_MOF_DIR/CIM28/Core28_Collection.mof
install -D -m 0644  $PEGASUS_ROOT/Schemas/CIM28/Core28_CoreElements.mof $RPM_BUILD_ROOT%PEGASUS_MOF_DIR/CIM28/Core28_CoreElements.mof
install -D -m 0644  $PEGASUS_ROOT/Schemas/CIM28/Core28_Device.mof $RPM_BUILD_ROOT%PEGASUS_MOF_DIR/CIM28/Core28_Device.mof
install -D -m 0644  $PEGASUS_ROOT/Schemas/CIM28/Core28_MethodParms.mof $RPM_BUILD_ROOT%PEGASUS_MOF_DIR/CIM28/Core28_MethodParms.mof
install -D -m 0644  $PEGASUS_ROOT/Schemas/CIM28/Core28_Physical.mof $RPM_BUILD_ROOT%PEGASUS_MOF_DIR/CIM28/Core28_Physical.mof
install -D -m 0644  $PEGASUS_ROOT/Schemas/CIM28/Core28_PowerMgmt.mof $RPM_BUILD_ROOT%PEGASUS_MOF_DIR/CIM28/Core28_PowerMgmt.mof
install -D -m 0644  $PEGASUS_ROOT/Schemas/CIM28/Core28_ProductFRU.mof $RPM_BUILD_ROOT%PEGASUS_MOF_DIR/CIM28/Core28_ProductFRU.mof
install -D -m 0644  $PEGASUS_ROOT/Schemas/CIM28/Core28_Qualifiers.mof $RPM_BUILD_ROOT%PEGASUS_MOF_DIR/CIM28/Core28_Qualifiers.mof
install -D -m 0644  $PEGASUS_ROOT/Schemas/CIM28/Core28_Redundancy.mof $RPM_BUILD_ROOT%PEGASUS_MOF_DIR/CIM28/Core28_Redundancy.mof
install -D -m 0644  $PEGASUS_ROOT/Schemas/CIM28/Core28_Service.mof $RPM_BUILD_ROOT%PEGASUS_MOF_DIR/CIM28/Core28_Service.mof
install -D -m 0644  $PEGASUS_ROOT/Schemas/CIM28/Core28_Settings.mof $RPM_BUILD_ROOT%PEGASUS_MOF_DIR/CIM28/Core28_Settings.mof
install -D -m 0644  $PEGASUS_ROOT/Schemas/CIM28/Core28_Software.mof $RPM_BUILD_ROOT%PEGASUS_MOF_DIR/CIM28/Core28_Software.mof
install -D -m 0644  $PEGASUS_ROOT/Schemas/CIM28/Core28_Statistics.mof $RPM_BUILD_ROOT%PEGASUS_MOF_DIR/CIM28/Core28_Statistics.mof
install -D -m 0644  $PEGASUS_ROOT/Schemas/CIM28/Core28_StorageExtent.mof $RPM_BUILD_ROOT%PEGASUS_MOF_DIR/CIM28/Core28_StorageExtent.mof
install -D -m 0644  $PEGASUS_ROOT/Schemas/CIM28/Database28_Statistics.mof $RPM_BUILD_ROOT%PEGASUS_MOF_DIR/CIM28/Database28_Statistics.mof
install -D -m 0644  $PEGASUS_ROOT/Schemas/CIM28/Database28_Storage.mof $RPM_BUILD_ROOT%PEGASUS_MOF_DIR/CIM28/Database28_Storage.mof
install -D -m 0644  $PEGASUS_ROOT/Schemas/CIM28/Database28_System.mof $RPM_BUILD_ROOT%PEGASUS_MOF_DIR/CIM28/Database28_System.mof
install -D -m 0644  $PEGASUS_ROOT/Schemas/CIM28/Device28_Controller.mof $RPM_BUILD_ROOT%PEGASUS_MOF_DIR/CIM28/Device28_Controller.mof
install -D -m 0644  $PEGASUS_ROOT/Schemas/CIM28/Device28_CoolingAndPower.mof $RPM_BUILD_ROOT%PEGASUS_MOF_DIR/CIM28/Device28_CoolingAndPower.mof
install -D -m 0644  $PEGASUS_ROOT/Schemas/CIM28/Device28_DeviceElements.mof $RPM_BUILD_ROOT%PEGASUS_MOF_DIR/CIM28/Device28_DeviceElements.mof
install -D -m 0644  $PEGASUS_ROOT/Schemas/CIM28/Device28_DiskGroup.mof $RPM_BUILD_ROOT%PEGASUS_MOF_DIR/CIM28/Device28_DiskGroup.mof
install -D -m 0644  $PEGASUS_ROOT/Schemas/CIM28/Device28_FC.mof $RPM_BUILD_ROOT%PEGASUS_MOF_DIR/CIM28/Device28_FC.mof
install -D -m 0644  $PEGASUS_ROOT/Schemas/CIM28/Device28_IB.mof $RPM_BUILD_ROOT%PEGASUS_MOF_DIR/CIM28/Device28_IB.mof
install -D -m 0644  $PEGASUS_ROOT/Schemas/CIM28/Device28_Memory.mof $RPM_BUILD_ROOT%PEGASUS_MOF_DIR/CIM28/Device28_Memory.mof
install -D -m 0644  $PEGASUS_ROOT/Schemas/CIM28/Device28_Modems.mof $RPM_BUILD_ROOT%PEGASUS_MOF_DIR/CIM28/Device28_Modems.mof
install -D -m 0644  $PEGASUS_ROOT/Schemas/CIM28/Device28_NetworkAdapter.mof $RPM_BUILD_ROOT%PEGASUS_MOF_DIR/CIM28/Device28_NetworkAdapter.mof
install -D -m 0644  $PEGASUS_ROOT/Schemas/CIM28/Device28_Ports.mof $RPM_BUILD_ROOT%PEGASUS_MOF_DIR/CIM28/Device28_Ports.mof
install -D -m 0644  $PEGASUS_ROOT/Schemas/CIM28/Device28_Printing.mof $RPM_BUILD_ROOT%PEGASUS_MOF_DIR/CIM28/Device28_Printing.mof
install -D -m 0644  $PEGASUS_ROOT/Schemas/CIM28/Device28_Processor.mof $RPM_BUILD_ROOT%PEGASUS_MOF_DIR/CIM28/Device28_Processor.mof
install -D -m 0644  $PEGASUS_ROOT/Schemas/CIM28/Device28_ProtocolController.mof $RPM_BUILD_ROOT%PEGASUS_MOF_DIR/CIM28/Device28_ProtocolController.mof
install -D -m 0644  $PEGASUS_ROOT/Schemas/CIM28/Device28_SccExtents.mof $RPM_BUILD_ROOT%PEGASUS_MOF_DIR/CIM28/Device28_SccExtents.mof
install -D -m 0644  $PEGASUS_ROOT/Schemas/CIM28/Device28_Sensors.mof $RPM_BUILD_ROOT%PEGASUS_MOF_DIR/CIM28/Device28_Sensors.mof
install -D -m 0644  $PEGASUS_ROOT/Schemas/CIM28/Device28_StorageDevices.mof $RPM_BUILD_ROOT%PEGASUS_MOF_DIR/CIM28/Device28_StorageDevices.mof
install -D -m 0644  $PEGASUS_ROOT/Schemas/CIM28/Device28_StorageExtents.mof $RPM_BUILD_ROOT%PEGASUS_MOF_DIR/CIM28/Device28_StorageExtents.mof
install -D -m 0644  $PEGASUS_ROOT/Schemas/CIM28/Device28_StorageLib.mof $RPM_BUILD_ROOT%PEGASUS_MOF_DIR/CIM28/Device28_StorageLib.mof
install -D -m 0644  $PEGASUS_ROOT/Schemas/CIM28/Device28_StorageServices.mof $RPM_BUILD_ROOT%PEGASUS_MOF_DIR/CIM28/Device28_StorageServices.mof
install -D -m 0644  $PEGASUS_ROOT/Schemas/CIM28/Device28_USB.mof $RPM_BUILD_ROOT%PEGASUS_MOF_DIR/CIM28/Device28_USB.mof
install -D -m 0644  $PEGASUS_ROOT/Schemas/CIM28/Device28_UserDevices.mof $RPM_BUILD_ROOT%PEGASUS_MOF_DIR/CIM28/Device28_UserDevices.mof
install -D -m 0644  $PEGASUS_ROOT/Schemas/CIM28/Metrics28_BaseMetric.mof $RPM_BUILD_ROOT%PEGASUS_MOF_DIR/CIM28/Metrics28_BaseMetric.mof
install -D -m 0644  $PEGASUS_ROOT/Schemas/CIM28/Metrics28_UnitOfWork.mof $RPM_BUILD_ROOT%PEGASUS_MOF_DIR/CIM28/Metrics28_UnitOfWork.mof
install -D -m 0644  $PEGASUS_ROOT/Schemas/CIM28/Network28_BGP.mof $RPM_BUILD_ROOT%PEGASUS_MOF_DIR/CIM28/Network28_BGP.mof
install -D -m 0644  $PEGASUS_ROOT/Schemas/CIM28/Network28_Buffers.mof $RPM_BUILD_ROOT%PEGASUS_MOF_DIR/CIM28/Network28_Buffers.mof
install -D -m 0644  $PEGASUS_ROOT/Schemas/CIM28/Network28_Collections.mof $RPM_BUILD_ROOT%PEGASUS_MOF_DIR/CIM28/Network28_Collections.mof
install -D -m 0644  $PEGASUS_ROOT/Schemas/CIM28/Network28_Filtering.mof $RPM_BUILD_ROOT%PEGASUS_MOF_DIR/CIM28/Network28_Filtering.mof
install -D -m 0644  $PEGASUS_ROOT/Schemas/CIM28/Network28_IPsec.mof $RPM_BUILD_ROOT%PEGASUS_MOF_DIR/CIM28/Network28_IPsec.mof
install -D -m 0644  $PEGASUS_ROOT/Schemas/CIM28/Network28_OSPF.mof $RPM_BUILD_ROOT%PEGASUS_MOF_DIR/CIM28/Network28_OSPF.mof
install -D -m 0644  $PEGASUS_ROOT/Schemas/CIM28/Network28_Pipes.mof $RPM_BUILD_ROOT%PEGASUS_MOF_DIR/CIM28/Network28_Pipes.mof
install -D -m 0644  $PEGASUS_ROOT/Schemas/CIM28/Network28_ProtocolEndpoints.mof $RPM_BUILD_ROOT%PEGASUS_MOF_DIR/CIM28/Network28_ProtocolEndpoints.mof
install -D -m 0644  $PEGASUS_ROOT/Schemas/CIM28/Network28_QoS.mof $RPM_BUILD_ROOT%PEGASUS_MOF_DIR/CIM28/Network28_QoS.mof
install -D -m 0644  $PEGASUS_ROOT/Schemas/CIM28/Network28_Routes.mof $RPM_BUILD_ROOT%PEGASUS_MOF_DIR/CIM28/Network28_Routes.mof
install -D -m 0644  $PEGASUS_ROOT/Schemas/CIM28/Network28_RoutingForwarding.mof $RPM_BUILD_ROOT%PEGASUS_MOF_DIR/CIM28/Network28_RoutingForwarding.mof
install -D -m 0644  $PEGASUS_ROOT/Schemas/CIM28/Network28_SNMP.mof $RPM_BUILD_ROOT%PEGASUS_MOF_DIR/CIM28/Network28_SNMP.mof
install -D -m 0644  $PEGASUS_ROOT/Schemas/CIM28/Network28_SwitchingBridging.mof $RPM_BUILD_ROOT%PEGASUS_MOF_DIR/CIM28/Network28_SwitchingBridging.mof
install -D -m 0644  $PEGASUS_ROOT/Schemas/CIM28/Network28_Systems.mof $RPM_BUILD_ROOT%PEGASUS_MOF_DIR/CIM28/Network28_Systems.mof
install -D -m 0644  $PEGASUS_ROOT/Schemas/CIM28/Network28_VLAN.mof $RPM_BUILD_ROOT%PEGASUS_MOF_DIR/CIM28/Network28_VLAN.mof
install -D -m 0644  $PEGASUS_ROOT/Schemas/CIM28/Physical28_Component.mof $RPM_BUILD_ROOT%PEGASUS_MOF_DIR/CIM28/Physical28_Component.mof
install -D -m 0644  $PEGASUS_ROOT/Schemas/CIM28/Physical28_Connector.mof $RPM_BUILD_ROOT%PEGASUS_MOF_DIR/CIM28/Physical28_Connector.mof
install -D -m 0644  $PEGASUS_ROOT/Schemas/CIM28/Physical28_Link.mof $RPM_BUILD_ROOT%PEGASUS_MOF_DIR/CIM28/Physical28_Link.mof
install -D -m 0644  $PEGASUS_ROOT/Schemas/CIM28/Physical28_Misc.mof $RPM_BUILD_ROOT%PEGASUS_MOF_DIR/CIM28/Physical28_Misc.mof
install -D -m 0644  $PEGASUS_ROOT/Schemas/CIM28/Physical28_Package.mof $RPM_BUILD_ROOT%PEGASUS_MOF_DIR/CIM28/Physical28_Package.mof
install -D -m 0644  $PEGASUS_ROOT/Schemas/CIM28/Physical28_PhysicalMedia.mof $RPM_BUILD_ROOT%PEGASUS_MOF_DIR/CIM28/Physical28_PhysicalMedia.mof
install -D -m 0644  $PEGASUS_ROOT/Schemas/CIM28/Physical28_StoragePackage.mof $RPM_BUILD_ROOT%PEGASUS_MOF_DIR/CIM28/Physical28_StoragePackage.mof
install -D -m 0644  $PEGASUS_ROOT/Schemas/CIM28/System28_Boot.mof $RPM_BUILD_ROOT%PEGASUS_MOF_DIR/CIM28/System28_Boot.mof
install -D -m 0644  $PEGASUS_ROOT/Schemas/CIM28/System28_Diagnostics.mof $RPM_BUILD_ROOT%PEGASUS_MOF_DIR/CIM28/System28_Diagnostics.mof
install -D -m 0644  $PEGASUS_ROOT/Schemas/CIM28/System28_FileElements.mof $RPM_BUILD_ROOT%PEGASUS_MOF_DIR/CIM28/System28_FileElements.mof
install -D -m 0644  $PEGASUS_ROOT/Schemas/CIM28/System28_Logs.mof $RPM_BUILD_ROOT%PEGASUS_MOF_DIR/CIM28/System28_Logs.mof
install -D -m 0644  $PEGASUS_ROOT/Schemas/CIM28/System28_OperatingSystem.mof $RPM_BUILD_ROOT%PEGASUS_MOF_DIR/CIM28/System28_OperatingSystem.mof
install -D -m 0644  $PEGASUS_ROOT/Schemas/CIM28/System28_Processing.mof $RPM_BUILD_ROOT%PEGASUS_MOF_DIR/CIM28/System28_Processing.mof
install -D -m 0644  $PEGASUS_ROOT/Schemas/CIM28/System28_SystemElements.mof $RPM_BUILD_ROOT%PEGASUS_MOF_DIR/CIM28/System28_SystemElements.mof
install -D -m 0644  $PEGASUS_ROOT/Schemas/CIM28/System28_SystemResources.mof $RPM_BUILD_ROOT%PEGASUS_MOF_DIR/CIM28/System28_SystemResources.mof
install -D -m 0644  $PEGASUS_ROOT/Schemas/CIM28/System28_Time.mof $RPM_BUILD_ROOT%PEGASUS_MOF_DIR/CIM28/System28_Time.mof
install -D -m 0644  $PEGASUS_ROOT/Schemas/CIM28/System28_Unix.mof $RPM_BUILD_ROOT%PEGASUS_MOF_DIR/CIM28/System28_Unix.mof
install -D -m 0644  $PEGASUS_ROOT/Schemas/CIM28/User28_AccessControl.mof $RPM_BUILD_ROOT%PEGASUS_MOF_DIR/CIM28/User28_AccessControl.mof
install -D -m 0644  $PEGASUS_ROOT/Schemas/CIM28/User28_Account.mof $RPM_BUILD_ROOT%PEGASUS_MOF_DIR/CIM28/User28_Account.mof
install -D -m 0644  $PEGASUS_ROOT/Schemas/CIM28/User28_AuthenticationReqmt.mof $RPM_BUILD_ROOT%PEGASUS_MOF_DIR/CIM28/User28_AuthenticationReqmt.mof
install -D -m 0644  $PEGASUS_ROOT/Schemas/CIM28/User28_Credential.mof $RPM_BUILD_ROOT%PEGASUS_MOF_DIR/CIM28/User28_Credential.mof
install -D -m 0644  $PEGASUS_ROOT/Schemas/CIM28/User28_Group.mof $RPM_BUILD_ROOT%PEGASUS_MOF_DIR/CIM28/User28_Group.mof
install -D -m 0644  $PEGASUS_ROOT/Schemas/CIM28/User28_Identity.mof $RPM_BUILD_ROOT%PEGASUS_MOF_DIR/CIM28/User28_Identity.mof
install -D -m 0644  $PEGASUS_ROOT/Schemas/CIM28/User28_Kerberos.mof $RPM_BUILD_ROOT%PEGASUS_MOF_DIR/CIM28/User28_Kerberos.mof
install -D -m 0644  $PEGASUS_ROOT/Schemas/CIM28/User28_Org.mof $RPM_BUILD_ROOT%PEGASUS_MOF_DIR/CIM28/User28_Org.mof
install -D -m 0644  $PEGASUS_ROOT/Schemas/CIM28/User28_PrivilegeManagementService.mof $RPM_BUILD_ROOT%PEGASUS_MOF_DIR/CIM28/User28_PrivilegeManagementService.mof
install -D -m 0644  $PEGASUS_ROOT/Schemas/CIM28/User28_Privilege.mof $RPM_BUILD_ROOT%PEGASUS_MOF_DIR/CIM28/User28_Privilege.mof
install -D -m 0644  $PEGASUS_ROOT/Schemas/CIM28/User28_PublicKey.mof $RPM_BUILD_ROOT%PEGASUS_MOF_DIR/CIM28/User28_PublicKey.mof
install -D -m 0644  $PEGASUS_ROOT/Schemas/CIM28/User28_Role.mof $RPM_BUILD_ROOT%PEGASUS_MOF_DIR/CIM28/User28_Role.mof
install -D -m 0644  $PEGASUS_ROOT/Schemas/CIM28/User28_SecurityLevel.mof $RPM_BUILD_ROOT%PEGASUS_MOF_DIR/CIM28/User28_SecurityLevel.mof
install -D -m 0644  $PEGASUS_ROOT/Schemas/CIM28/User28_SecurityServices.mof $RPM_BUILD_ROOT%PEGASUS_MOF_DIR/CIM28/User28_SecurityServices.mof
install -D -m 0644  $PEGASUS_ROOT/Schemas/CIM28/User28_SharedSecret.mof $RPM_BUILD_ROOT%PEGASUS_MOF_DIR/CIM28/User28_SharedSecret.mof
install -D -m 0644  $PEGASUS_ROOT/Schemas/CIM28/User28_StorageHardwareID.mof $RPM_BUILD_ROOT%PEGASUS_MOF_DIR/CIM28/User28_StorageHardwareID.mof
install -D -m 0644  $PEGASUS_ROOT/Schemas/CIM28/User28_UsersAccess.mof $RPM_BUILD_ROOT%PEGASUS_MOF_DIR/CIM28/User28_UsersAccess.mof

#
# Pegasus' schema
#

# Internal:
install -D -m 0644  $PEGASUS_ROOT/Schemas/Pegasus/Internal/VER20/PG_Authorization20.mof  $RPM_BUILD_ROOT%PEGASUS_MOF_DIR/Pegasus/PG_Authorization20.mof
install -D -m 0644  $PEGASUS_ROOT/Schemas/Pegasus/Internal/VER20/PG_ConfigSetting20.mof  $RPM_BUILD_ROOT%PEGASUS_MOF_DIR/Pegasus/PG_ConfigSetting20.mof
install -D -m 0644  $PEGASUS_ROOT/Schemas/Pegasus/Internal/VER20/PG_InternalSchema20.mof $RPM_BUILD_ROOT%PEGASUS_MOF_DIR/Pegasus/PG_InternalSchema20.mof
install -D -m 0644  $PEGASUS_ROOT/Schemas/Pegasus/Internal/VER20/PG_ShutdownService20.mof           $RPM_BUILD_ROOT%PEGASUS_MOF_DIR/Pegasus/PG_ShutdownService20.mof
install -D -m 0644  $PEGASUS_ROOT/Schemas/Pegasus/Internal/VER20/PG_User20.mof           $RPM_BUILD_ROOT%PEGASUS_MOF_DIR/Pegasus/PG_User20.mof

# InterOp:
install -D -m 0644  $PEGASUS_ROOT/Schemas/Pegasus/InterOp/VER20/PG_Events20.mof   $RPM_BUILD_ROOT%PEGASUS_MOF_DIR/Pegasus/PG_Events20.mof
install -D -m 0644  $PEGASUS_ROOT/Schemas/Pegasus/InterOp/VER20/PG_InterOpSchema20.mof   $RPM_BUILD_ROOT%PEGASUS_MOF_DIR/Pegasus/PG_InterOpSchema20.mof
install -D -m 0644  $PEGASUS_ROOT/Schemas/Pegasus/InterOp/VER20/PG_ProviderModule20.mof  $RPM_BUILD_ROOT%PEGASUS_MOF_DIR/Pegasus/PG_ProviderModule20.mof

#
# Pegasus Default Providers' schema
#
install -D -m 0644  $PEGASUS_ROOT/Schemas/Pegasus/ManagedSystem/VER20/PG_ComputerSystem20.mof                               $RPM_BUILD_ROOT%PEGASUS_MOF_DIR/Pegasus/PG_ComputerSystem20.mof
install -D -m 0644  $PEGASUS_ROOT/Schemas/Pegasus/ManagedSystem/VER20/PG_ComputerSystem20R.mof                               $RPM_BUILD_ROOT%PEGASUS_MOF_DIR/Pegasus/PG_ComputerSystem20R.mof
install -D -m 0644  $PEGASUS_ROOT/Schemas/Pegasus/ManagedSystem/VER20/PG_ManagedSystemSchema20.mof                               $RPM_BUILD_ROOT%PEGASUS_MOF_DIR/Pegasus/PG_ManagedSystemSchema20.mof
install -D -m 0644  $PEGASUS_ROOT/Schemas/Pegasus/ManagedSystem/VER20/PG_ManagedSystemSchema20R.mof                               $RPM_BUILD_ROOT%PEGASUS_MOF_DIR/Pegasus/PG_ManagedSystemSchema20R.mof
install -D -m 0644  $PEGASUS_ROOT/Schemas/Pegasus/ManagedSystem/VER20/PG_OperatingSystem20.mof                               $RPM_BUILD_ROOT%PEGASUS_MOF_DIR/Pegasus/PG_OperatingSystem20.mof
install -D -m 0644  $PEGASUS_ROOT/Schemas/Pegasus/ManagedSystem/VER20/PG_OperatingSystem20R.mof                               $RPM_BUILD_ROOT%PEGASUS_MOF_DIR/Pegasus/PG_OperatingSystem20R.mof
install -D -m 0644  $PEGASUS_ROOT/Schemas/Pegasus/ManagedSystem/VER20/PG_UnixProcess20.mof                               $RPM_BUILD_ROOT%PEGASUS_MOF_DIR/Pegasus/PG_UnixProcess20.mof
install -D -m 0644  $PEGASUS_ROOT/Schemas/Pegasus/ManagedSystem/VER20/PG_UnixProcess20R.mof                               $RPM_BUILD_ROOT%PEGASUS_MOF_DIR/Pegasus/PG_UnixProcess20R.mof
#
# Initial Repository
#
mkdir -p  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root
mkdir -p  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root/classes
mkdir -p  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root/instances
mkdir -p  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root/qualifiers
mkdir -p  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#PG_Internal
mkdir -p  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#PG_Internal/classes
mkdir -p  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#PG_Internal/instances
mkdir -p  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#PG_Internal/qualifiers
mkdir -p  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#PG_InterOp
mkdir -p  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#PG_InterOp/classes
mkdir -p  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#PG_InterOp/instances
mkdir -p  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#PG_InterOp/qualifiers
mkdir -p  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2
mkdir -p  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes
mkdir -p  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/instances
mkdir -p  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/qualifiers
install -D -m 0755  $PEGASUS_HOME/repository/root#PG_Internal/classes/PG_Authorization.#  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#PG_Internal/classes/PG_Authorization.#
install -D -m 0755  $PEGASUS_HOME/repository/root#PG_Internal/classes/PG_ConfigSetting.#  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#PG_Internal/classes/PG_ConfigSetting.#
install -D -m 0755  $PEGASUS_HOME/repository/root#PG_Internal/classes/PG_User.#  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#PG_Internal/classes/PG_User.#
install -D -m 0755  $PEGASUS_HOME/repository/root#PG_Internal/classes/PG_ShutdownService.#  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#PG_Internal/classes/PG_ShutdownService.#
install -D -m 0755  $PEGASUS_HOME/repository/root#PG_Internal/classes/PG_WBEMSLPTemplate.#  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#PG_Internal/classes/PG_WBEMSLPTemplate.#
install -D -m 0755  $PEGASUS_HOME/repository/root#PG_Internal/qualifiers/Abstract  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#PG_Internal/qualifiers/Abstract
install -D -m 0755  $PEGASUS_HOME/repository/root#PG_Internal/qualifiers/Aggregate  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#PG_Internal/qualifiers/Aggregate
install -D -m 0755  $PEGASUS_HOME/repository/root#PG_Internal/qualifiers/Aggregation  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#PG_Internal/qualifiers/Aggregation
install -D -m 0755  $PEGASUS_HOME/repository/root#PG_Internal/qualifiers/Alias  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#PG_Internal/qualifiers/Alias
install -D -m 0755  $PEGASUS_HOME/repository/root#PG_Internal/qualifiers/ArrayType  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#PG_Internal/qualifiers/ArrayType
install -D -m 0755  $PEGASUS_HOME/repository/root#PG_Internal/qualifiers/ASSOCIATION  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#PG_Internal/qualifiers/ASSOCIATION
install -D -m 0755  $PEGASUS_HOME/repository/root#PG_Internal/qualifiers/BitMap  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#PG_Internal/qualifiers/BitMap
install -D -m 0755  $PEGASUS_HOME/repository/root#PG_Internal/qualifiers/BitValues  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#PG_Internal/qualifiers/BitValues
install -D -m 0755  $PEGASUS_HOME/repository/root#PG_Internal/qualifiers/Composition  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#PG_Internal/qualifiers/Composition
install -D -m 0755  $PEGASUS_HOME/repository/root#PG_Internal/qualifiers/Counter  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#PG_Internal/qualifiers/Counter
install -D -m 0755  $PEGASUS_HOME/repository/root#PG_Internal/qualifiers/Delete  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#PG_Internal/qualifiers/Delete
install -D -m 0755  $PEGASUS_HOME/repository/root#PG_Internal/qualifiers/Deprecated  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#PG_Internal/qualifiers/Deprecated
install -D -m 0755  $PEGASUS_HOME/repository/root#PG_Internal/qualifiers/Description  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#PG_Internal/qualifiers/Description
install -D -m 0755  $PEGASUS_HOME/repository/root#PG_Internal/qualifiers/DisplayName  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#PG_Internal/qualifiers/DisplayName
install -D -m 0755  $PEGASUS_HOME/repository/root#PG_Internal/qualifiers/DN  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#PG_Internal/qualifiers/DN
install -D -m 0755  $PEGASUS_HOME/repository/root#PG_Internal/qualifiers/EmbeddedObject  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#PG_Internal/qualifiers/EmbeddedObject
install -D -m 0755  $PEGASUS_HOME/repository/root#PG_Internal/qualifiers/Exception  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#PG_Internal/qualifiers/Exception
install -D -m 0755  $PEGASUS_HOME/repository/root#PG_Internal/qualifiers/Expensive  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#PG_Internal/qualifiers/Expensive
install -D -m 0755  $PEGASUS_HOME/repository/root#PG_Internal/qualifiers/Experimental  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#PG_Internal/qualifiers/Experimental
install -D -m 0755  $PEGASUS_HOME/repository/root#PG_Internal/qualifiers/Gauge  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#PG_Internal/qualifiers/Gauge
install -D -m 0755  $PEGASUS_HOME/repository/root#PG_Internal/qualifiers/Ifdeleted  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#PG_Internal/qualifiers/Ifdeleted
install -D -m 0755  $PEGASUS_HOME/repository/root#PG_Internal/qualifiers/In  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#PG_Internal/qualifiers/In
install -D -m 0755  $PEGASUS_HOME/repository/root#PG_Internal/qualifiers/INDICATION  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#PG_Internal/qualifiers/INDICATION
install -D -m 0755  $PEGASUS_HOME/repository/root#PG_Internal/qualifiers/Invisible  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#PG_Internal/qualifiers/Invisible
install -D -m 0755  $PEGASUS_HOME/repository/root#PG_Internal/qualifiers/Key  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#PG_Internal/qualifiers/Key
install -D -m 0755  $PEGASUS_HOME/repository/root#PG_Internal/qualifiers/Large  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#PG_Internal/qualifiers/Large
install -D -m 0755  $PEGASUS_HOME/repository/root#PG_Internal/qualifiers/MappingStrings  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#PG_Internal/qualifiers/MappingStrings
install -D -m 0755  $PEGASUS_HOME/repository/root#PG_Internal/qualifiers/Max  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#PG_Internal/qualifiers/Max
install -D -m 0755  $PEGASUS_HOME/repository/root#PG_Internal/qualifiers/MaxLen  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#PG_Internal/qualifiers/MaxLen
install -D -m 0755  $PEGASUS_HOME/repository/root#PG_Internal/qualifiers/MaxValue  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#PG_Internal/qualifiers/MaxValue
install -D -m 0755  $PEGASUS_HOME/repository/root#PG_Internal/qualifiers/Min  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#PG_Internal/qualifiers/Min
install -D -m 0755  $PEGASUS_HOME/repository/root#PG_Internal/qualifiers/MinLen  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#PG_Internal/qualifiers/MinLen
install -D -m 0755  $PEGASUS_HOME/repository/root#PG_Internal/qualifiers/MinValue  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#PG_Internal/qualifiers/MinValue
install -D -m 0755  $PEGASUS_HOME/repository/root#PG_Internal/qualifiers/ModelCorrespondence  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#PG_Internal/qualifiers/ModelCorrespondence
install -D -m 0755  $PEGASUS_HOME/repository/root#PG_Internal/qualifiers/Nonlocal  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#PG_Internal/qualifiers/Nonlocal
install -D -m 0755  $PEGASUS_HOME/repository/root#PG_Internal/qualifiers/NonlocalType  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#PG_Internal/qualifiers/NonlocalType
install -D -m 0755  $PEGASUS_HOME/repository/root#PG_Internal/qualifiers/NullValue  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#PG_Internal/qualifiers/NullValue
install -D -m 0755  $PEGASUS_HOME/repository/root#PG_Internal/qualifiers/Octetstring  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#PG_Internal/qualifiers/Octetstring
install -D -m 0755  $PEGASUS_HOME/repository/root#PG_Internal/qualifiers/Out  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#PG_Internal/qualifiers/Out
install -D -m 0755  $PEGASUS_HOME/repository/root#PG_Internal/qualifiers/Override  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#PG_Internal/qualifiers/Override
install -D -m 0755  $PEGASUS_HOME/repository/root#PG_Internal/qualifiers/Propagated  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#PG_Internal/qualifiers/Propagated
install -D -m 0755  $PEGASUS_HOME/repository/root#PG_Internal/qualifiers/PropertyUsage  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#PG_Internal/qualifiers/PropertyUsage
install -D -m 0755  $PEGASUS_HOME/repository/root#PG_Internal/qualifiers/Provider  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#PG_Internal/qualifiers/Provider
install -D -m 0755  $PEGASUS_HOME/repository/root#PG_Internal/qualifiers/Read  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#PG_Internal/qualifiers/Read
install -D -m 0755  $PEGASUS_HOME/repository/root#PG_Internal/qualifiers/Required  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#PG_Internal/qualifiers/Required
install -D -m 0755  $PEGASUS_HOME/repository/root#PG_Internal/qualifiers/Revision  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#PG_Internal/qualifiers/Revision
install -D -m 0755  $PEGASUS_HOME/repository/root#PG_Internal/qualifiers/CLASS  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#PG_Internal/qualifiers/CLASS
install -D -m 0755  $PEGASUS_HOME/repository/root#PG_Internal/qualifiers/Source  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#PG_Internal/qualifiers/Source
install -D -m 0755  $PEGASUS_HOME/repository/root#PG_Internal/qualifiers/SourceType  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#PG_Internal/qualifiers/SourceType
install -D -m 0755  $PEGASUS_HOME/repository/root#PG_Internal/qualifiers/Static  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#PG_Internal/qualifiers/Static
install -D -m 0755  $PEGASUS_HOME/repository/root#PG_Internal/qualifiers/Syntax  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#PG_Internal/qualifiers/Syntax
install -D -m 0755  $PEGASUS_HOME/repository/root#PG_Internal/qualifiers/SyntaxType  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#PG_Internal/qualifiers/SyntaxType
install -D -m 0755  $PEGASUS_HOME/repository/root#PG_Internal/qualifiers/Terminal  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#PG_Internal/qualifiers/Terminal
install -D -m 0755  $PEGASUS_HOME/repository/root#PG_Internal/qualifiers/TriggerType  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#PG_Internal/qualifiers/TriggerType
install -D -m 0755  $PEGASUS_HOME/repository/root#PG_Internal/qualifiers/Units  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#PG_Internal/qualifiers/Units
install -D -m 0755  $PEGASUS_HOME/repository/root#PG_Internal/qualifiers/UnknownValues  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#PG_Internal/qualifiers/UnknownValues
install -D -m 0755  $PEGASUS_HOME/repository/root#PG_Internal/qualifiers/UnsupportedValues  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#PG_Internal/qualifiers/UnsupportedValues
install -D -m 0755  $PEGASUS_HOME/repository/root#PG_Internal/qualifiers/ValueMap  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#PG_Internal/qualifiers/ValueMap
install -D -m 0755  $PEGASUS_HOME/repository/root#PG_Internal/qualifiers/Values  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#PG_Internal/qualifiers/Values
install -D -m 0755  $PEGASUS_HOME/repository/root#PG_Internal/qualifiers/Version  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#PG_Internal/qualifiers/Version
install -D -m 0755  $PEGASUS_HOME/repository/root#PG_Internal/qualifiers/Weak  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#PG_Internal/qualifiers/Weak
install -D -m 0755  $PEGASUS_HOME/repository/root#PG_Internal/qualifiers/Write  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#PG_Internal/qualifiers/Write
install -D -m 0755  $PEGASUS_HOME/repository/root#PG_InterOp/classes/CIM_ManagedElement.#  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#PG_InterOp/classes/CIM_ManagedElement.#
install -D -m 0755  $PEGASUS_HOME/repository/root#PG_InterOp/classes/associations  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#PG_InterOp/classes/associations
install -D -m 0755  $PEGASUS_HOME/repository/root#PG_InterOp/classes/CIM_Dependency.#  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#PG_InterOp/classes/CIM_Dependency.#
install -D -m 0755  $PEGASUS_HOME/repository/root#PG_InterOp/classes/CIM_ConcreteDependency.CIM_Dependency  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#PG_InterOp/classes/CIM_ConcreteDependency.CIM_Dependency
install -D -m 0755  $PEGASUS_HOME/repository/root#PG_InterOp/classes/CIM_Component.#  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#PG_InterOp/classes/CIM_Component.#
install -D -m 0755  $PEGASUS_HOME/repository/root#PG_InterOp/classes/CIM_ConcreteComponent.CIM_Component  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#PG_InterOp/classes/CIM_ConcreteComponent.CIM_Component
install -D -m 0755  $PEGASUS_HOME/repository/root#PG_InterOp/classes/CIM_LogicalIdentity.#  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#PG_InterOp/classes/CIM_LogicalIdentity.#
install -D -m 0755  $PEGASUS_HOME/repository/root#PG_InterOp/classes/CIM_ConcreteIdentity.CIM_LogicalIdentity  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#PG_InterOp/classes/CIM_ConcreteIdentity.CIM_LogicalIdentity
install -D -m 0755  $PEGASUS_HOME/repository/root#PG_InterOp/classes/CIM_ManagedSystemElement.CIM_ManagedElement  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#PG_InterOp/classes/CIM_ManagedSystemElement.CIM_ManagedElement
install -D -m 0755  $PEGASUS_HOME/repository/root#PG_InterOp/classes/CIM_HostedDependency.CIM_Dependency  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#PG_InterOp/classes/CIM_HostedDependency.CIM_Dependency
install -D -m 0755  $PEGASUS_HOME/repository/root#PG_InterOp/classes/CIM_LogicalElement.CIM_ManagedSystemElement  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#PG_InterOp/classes/CIM_LogicalElement.CIM_ManagedSystemElement
install -D -m 0755  $PEGASUS_HOME/repository/root#PG_InterOp/classes/CIM_Job.CIM_LogicalElement  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#PG_InterOp/classes/CIM_Job.CIM_LogicalElement
install -D -m 0755  $PEGASUS_HOME/repository/root#PG_InterOp/classes/CIM_ConcreteJob.CIM_Job  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#PG_InterOp/classes/CIM_ConcreteJob.CIM_Job
install -D -m 0755  $PEGASUS_HOME/repository/root#PG_InterOp/classes/CIM_EnabledLogicalElement.CIM_LogicalElement  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#PG_InterOp/classes/CIM_EnabledLogicalElement.CIM_LogicalElement
install -D -m 0755  $PEGASUS_HOME/repository/root#PG_InterOp/classes/CIM_Synchronized.#  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#PG_InterOp/classes/CIM_Synchronized.#
install -D -m 0755  $PEGASUS_HOME/repository/root#PG_InterOp/classes/CIM_System.CIM_EnabledLogicalElement  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#PG_InterOp/classes/CIM_System.CIM_EnabledLogicalElement
install -D -m 0755  $PEGASUS_HOME/repository/root#PG_InterOp/classes/CIM_SystemComponent.CIM_Component  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#PG_InterOp/classes/CIM_SystemComponent.CIM_Component
install -D -m 0755  $PEGASUS_HOME/repository/root#PG_InterOp/classes/CIM_AdminDomain.CIM_System  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#PG_InterOp/classes/CIM_AdminDomain.CIM_System
install -D -m 0755  $PEGASUS_HOME/repository/root#PG_InterOp/classes/CIM_ContainedDomain.CIM_SystemComponent  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#PG_InterOp/classes/CIM_ContainedDomain.CIM_SystemComponent
install -D -m 0755  $PEGASUS_HOME/repository/root#PG_InterOp/classes/CIM_Service.CIM_EnabledLogicalElement  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#PG_InterOp/classes/CIM_Service.CIM_EnabledLogicalElement
install -D -m 0755  $PEGASUS_HOME/repository/root#PG_InterOp/classes/CIM_HostedService.CIM_HostedDependency  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#PG_InterOp/classes/CIM_HostedService.CIM_HostedDependency
install -D -m 0755  $PEGASUS_HOME/repository/root#PG_InterOp/classes/CIM_ProvidesServiceToElement.CIM_Dependency  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#PG_InterOp/classes/CIM_ProvidesServiceToElement.CIM_Dependency
install -D -m 0755  $PEGASUS_HOME/repository/root#PG_InterOp/classes/CIM_ServiceAvailableToElement.#  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#PG_InterOp/classes/CIM_ServiceAvailableToElement.#
install -D -m 0755  $PEGASUS_HOME/repository/root#PG_InterOp/classes/CIM_ServiceAffectsElement.#  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#PG_InterOp/classes/CIM_ServiceAffectsElement.#
install -D -m 0755  $PEGASUS_HOME/repository/root#PG_InterOp/classes/CIM_ServiceServiceDependency.CIM_ProvidesServiceToElement  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#PG_InterOp/classes/CIM_ServiceServiceDependency.CIM_ProvidesServiceToElement
install -D -m 0755  $PEGASUS_HOME/repository/root#PG_InterOp/classes/CIM_ServiceComponent.CIM_Component  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#PG_InterOp/classes/CIM_ServiceComponent.CIM_Component
install -D -m 0755  $PEGASUS_HOME/repository/root#PG_InterOp/classes/CIM_ServiceAccessPoint.CIM_EnabledLogicalElement  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#PG_InterOp/classes/CIM_ServiceAccessPoint.CIM_EnabledLogicalElement
install -D -m 0755  $PEGASUS_HOME/repository/root#PG_InterOp/classes/CIM_HostedAccessPoint.CIM_HostedDependency  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#PG_InterOp/classes/CIM_HostedAccessPoint.CIM_HostedDependency
install -D -m 0755  $PEGASUS_HOME/repository/root#PG_InterOp/classes/CIM_SAPAvailableForElement.#  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#PG_InterOp/classes/CIM_SAPAvailableForElement.#
install -D -m 0755  $PEGASUS_HOME/repository/root#PG_InterOp/classes/CIM_ServiceSAPDependency.CIM_Dependency  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#PG_InterOp/classes/CIM_ServiceSAPDependency.CIM_Dependency
install -D -m 0755  $PEGASUS_HOME/repository/root#PG_InterOp/classes/CIM_ServiceAccessBySAP.CIM_Dependency  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#PG_InterOp/classes/CIM_ServiceAccessBySAP.CIM_Dependency
install -D -m 0755  $PEGASUS_HOME/repository/root#PG_InterOp/classes/CIM_SAPSAPDependency.CIM_Dependency  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#PG_InterOp/classes/CIM_SAPSAPDependency.CIM_Dependency
install -D -m 0755  $PEGASUS_HOME/repository/root#PG_InterOp/classes/CIM_ActiveConnection.CIM_SAPSAPDependency  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#PG_InterOp/classes/CIM_ActiveConnection.CIM_SAPSAPDependency
install -D -m 0755  $PEGASUS_HOME/repository/root#PG_InterOp/classes/CIM_RemoteServiceAccessPoint.CIM_ServiceAccessPoint  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#PG_InterOp/classes/CIM_RemoteServiceAccessPoint.CIM_ServiceAccessPoint
install -D -m 0755  $PEGASUS_HOME/repository/root#PG_InterOp/classes/CIM_RemotePort.CIM_RemoteServiceAccessPoint  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#PG_InterOp/classes/CIM_RemotePort.CIM_RemoteServiceAccessPoint
install -D -m 0755  $PEGASUS_HOME/repository/root#PG_InterOp/classes/CIM_ServiceAccessURI.CIM_ServiceAccessPoint  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#PG_InterOp/classes/CIM_ServiceAccessURI.CIM_ServiceAccessPoint
install -D -m 0755  $PEGASUS_HOME/repository/root#PG_InterOp/classes/CIM_ProtocolEndpoint.CIM_ServiceAccessPoint  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#PG_InterOp/classes/CIM_ProtocolEndpoint.CIM_ServiceAccessPoint
install -D -m 0755  $PEGASUS_HOME/repository/root#PG_InterOp/classes/CIM_BindsTo.CIM_SAPSAPDependency  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#PG_InterOp/classes/CIM_BindsTo.CIM_SAPSAPDependency
install -D -m 0755  $PEGASUS_HOME/repository/root#PG_InterOp/classes/CIM_ProvidesEndpoint.CIM_ServiceAccessBySAP  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#PG_InterOp/classes/CIM_ProvidesEndpoint.CIM_ServiceAccessBySAP
install -D -m 0755  $PEGASUS_HOME/repository/root#PG_InterOp/classes/CIM_PhysicalElement.CIM_ManagedSystemElement  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#PG_InterOp/classes/CIM_PhysicalElement.CIM_ManagedSystemElement
install -D -m 0755  $PEGASUS_HOME/repository/root#PG_InterOp/classes/CIM_Location.CIM_ManagedElement  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#PG_InterOp/classes/CIM_Location.CIM_ManagedElement
install -D -m 0755  $PEGASUS_HOME/repository/root#PG_InterOp/classes/CIM_ElementLocation.#  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#PG_InterOp/classes/CIM_ElementLocation.#
install -D -m 0755  $PEGASUS_HOME/repository/root#PG_InterOp/classes/CIM_PhysicalElementLocation.CIM_ElementLocation  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#PG_InterOp/classes/CIM_PhysicalElementLocation.CIM_ElementLocation
install -D -m 0755  $PEGASUS_HOME/repository/root#PG_InterOp/classes/CIM_ContainedLocation.CIM_Component  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#PG_InterOp/classes/CIM_ContainedLocation.CIM_Component
install -D -m 0755  $PEGASUS_HOME/repository/root#PG_InterOp/classes/CIM_SystemPackaging.CIM_Dependency  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#PG_InterOp/classes/CIM_SystemPackaging.CIM_Dependency
install -D -m 0755  $PEGASUS_HOME/repository/root#PG_InterOp/classes/CIM_SoftwareIdentity.CIM_LogicalElement  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#PG_InterOp/classes/CIM_SoftwareIdentity.CIM_LogicalElement
install -D -m 0755  $PEGASUS_HOME/repository/root#PG_InterOp/classes/CIM_ElementSoftwareIdentity.CIM_Dependency  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#PG_InterOp/classes/CIM_ElementSoftwareIdentity.CIM_Dependency
install -D -m 0755  $PEGASUS_HOME/repository/root#PG_InterOp/classes/CIM_InstalledSoftwareIdentity.#  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#PG_InterOp/classes/CIM_InstalledSoftwareIdentity.#
install -D -m 0755  $PEGASUS_HOME/repository/root#PG_InterOp/classes/CIM_LogicalDevice.CIM_EnabledLogicalElement  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#PG_InterOp/classes/CIM_LogicalDevice.CIM_EnabledLogicalElement
install -D -m 0755  $PEGASUS_HOME/repository/root#PG_InterOp/classes/CIM_Realizes.CIM_Dependency  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#PG_InterOp/classes/CIM_Realizes.CIM_Dependency
install -D -m 0755  $PEGASUS_HOME/repository/root#PG_InterOp/classes/CIM_SystemDevice.CIM_SystemComponent  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#PG_InterOp/classes/CIM_SystemDevice.CIM_SystemComponent
install -D -m 0755  $PEGASUS_HOME/repository/root#PG_InterOp/classes/CIM_DeviceServiceImplementation.CIM_Dependency  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#PG_InterOp/classes/CIM_DeviceServiceImplementation.CIM_Dependency
install -D -m 0755  $PEGASUS_HOME/repository/root#PG_InterOp/classes/CIM_DeviceSAPImplementation.CIM_Dependency  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#PG_InterOp/classes/CIM_DeviceSAPImplementation.CIM_Dependency
install -D -m 0755  $PEGASUS_HOME/repository/root#PG_InterOp/classes/CIM_StorageExtent.CIM_LogicalDevice  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#PG_InterOp/classes/CIM_StorageExtent.CIM_LogicalDevice
install -D -m 0755  $PEGASUS_HOME/repository/root#PG_InterOp/classes/CIM_BasedOn.CIM_Dependency  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#PG_InterOp/classes/CIM_BasedOn.CIM_Dependency
install -D -m 0755  $PEGASUS_HOME/repository/root#PG_InterOp/classes/CIM_Collection.CIM_ManagedElement  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#PG_InterOp/classes/CIM_Collection.CIM_ManagedElement
install -D -m 0755  $PEGASUS_HOME/repository/root#PG_InterOp/classes/CIM_MemberOfCollection.#  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#PG_InterOp/classes/CIM_MemberOfCollection.#
install -D -m 0755  $PEGASUS_HOME/repository/root#PG_InterOp/classes/CIM_OrderedMemberOfCollection.CIM_MemberOfCollection  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#PG_InterOp/classes/CIM_OrderedMemberOfCollection.CIM_MemberOfCollection
install -D -m 0755  $PEGASUS_HOME/repository/root#PG_InterOp/classes/CIM_SystemSpecificCollection.CIM_Collection  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#PG_InterOp/classes/CIM_SystemSpecificCollection.CIM_Collection
install -D -m 0755  $PEGASUS_HOME/repository/root#PG_InterOp/classes/CIM_ConcreteCollection.CIM_Collection  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#PG_InterOp/classes/CIM_ConcreteCollection.CIM_Collection
install -D -m 0755  $PEGASUS_HOME/repository/root#PG_InterOp/classes/CIM_HostedCollection.CIM_HostedDependency  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#PG_InterOp/classes/CIM_HostedCollection.CIM_HostedDependency
install -D -m 0755  $PEGASUS_HOME/repository/root#PG_InterOp/classes/CIM_CollectionOfMSEs.CIM_Collection  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#PG_InterOp/classes/CIM_CollectionOfMSEs.CIM_Collection
install -D -m 0755  $PEGASUS_HOME/repository/root#PG_InterOp/classes/CIM_CollectedCollections.#  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#PG_InterOp/classes/CIM_CollectedCollections.#
install -D -m 0755  $PEGASUS_HOME/repository/root#PG_InterOp/classes/CIM_CollectedMSEs.CIM_MemberOfCollection  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#PG_InterOp/classes/CIM_CollectedMSEs.CIM_MemberOfCollection
install -D -m 0755  $PEGASUS_HOME/repository/root#PG_InterOp/classes/CIM_RedundancyGroup.CIM_LogicalElement  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#PG_InterOp/classes/CIM_RedundancyGroup.CIM_LogicalElement
install -D -m 0755  $PEGASUS_HOME/repository/root#PG_InterOp/classes/CIM_RedundancyComponent.CIM_Component  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#PG_InterOp/classes/CIM_RedundancyComponent.CIM_Component
install -D -m 0755  $PEGASUS_HOME/repository/root#PG_InterOp/classes/CIM_SpareGroup.CIM_RedundancyGroup  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#PG_InterOp/classes/CIM_SpareGroup.CIM_RedundancyGroup
install -D -m 0755  $PEGASUS_HOME/repository/root#PG_InterOp/classes/CIM_ActsAsSpare.#  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#PG_InterOp/classes/CIM_ActsAsSpare.#
install -D -m 0755  $PEGASUS_HOME/repository/root#PG_InterOp/classes/CIM_StorageRedundancyGroup.CIM_RedundancyGroup  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#PG_InterOp/classes/CIM_StorageRedundancyGroup.CIM_RedundancyGroup
install -D -m 0755  $PEGASUS_HOME/repository/root#PG_InterOp/classes/CIM_ExtraCapacityGroup.CIM_RedundancyGroup  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#PG_InterOp/classes/CIM_ExtraCapacityGroup.CIM_RedundancyGroup
install -D -m 0755  $PEGASUS_HOME/repository/root#PG_InterOp/classes/CIM_ExtentRedundancyComponent.CIM_RedundancyComponent  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#PG_InterOp/classes/CIM_ExtentRedundancyComponent.CIM_RedundancyComponent
install -D -m 0755  $PEGASUS_HOME/repository/root#PG_InterOp/classes/CIM_Product.CIM_ManagedElement  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#PG_InterOp/classes/CIM_Product.CIM_ManagedElement
install -D -m 0755  $PEGASUS_HOME/repository/root#PG_InterOp/classes/CIM_ProductComponent.CIM_Component  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#PG_InterOp/classes/CIM_ProductComponent.CIM_Component
install -D -m 0755  $PEGASUS_HOME/repository/root#PG_InterOp/classes/CIM_ProductParentChild.#  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#PG_InterOp/classes/CIM_ProductParentChild.#
install -D -m 0755  $PEGASUS_HOME/repository/root#PG_InterOp/classes/CIM_CompatibleProduct.#  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#PG_InterOp/classes/CIM_CompatibleProduct.#
install -D -m 0755  $PEGASUS_HOME/repository/root#PG_InterOp/classes/CIM_ProductProductDependency.#  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#PG_InterOp/classes/CIM_ProductProductDependency.#
install -D -m 0755  $PEGASUS_HOME/repository/root#PG_InterOp/classes/CIM_SupportAccess.CIM_ManagedElement  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#PG_InterOp/classes/CIM_SupportAccess.CIM_ManagedElement
install -D -m 0755  $PEGASUS_HOME/repository/root#PG_InterOp/classes/CIM_ProductSupport.#  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#PG_InterOp/classes/CIM_ProductSupport.#
install -D -m 0755  $PEGASUS_HOME/repository/root#PG_InterOp/classes/CIM_FRU.CIM_ManagedElement  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#PG_InterOp/classes/CIM_FRU.CIM_ManagedElement
install -D -m 0755  $PEGASUS_HOME/repository/root#PG_InterOp/classes/CIM_ProductFRU.#  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#PG_InterOp/classes/CIM_ProductFRU.#
install -D -m 0755  $PEGASUS_HOME/repository/root#PG_InterOp/classes/CIM_ReplaceableProductFRU.#  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#PG_InterOp/classes/CIM_ReplaceableProductFRU.#
install -D -m 0755  $PEGASUS_HOME/repository/root#PG_InterOp/classes/CIM_ProductPhysicalComponent.CIM_Component  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#PG_InterOp/classes/CIM_ProductPhysicalComponent.CIM_Component
install -D -m 0755  $PEGASUS_HOME/repository/root#PG_InterOp/classes/CIM_ProductSoftwareComponent.CIM_Component  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#PG_InterOp/classes/CIM_ProductSoftwareComponent.CIM_Component
install -D -m 0755  $PEGASUS_HOME/repository/root#PG_InterOp/classes/CIM_ProductServiceComponent.CIM_Component  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#PG_InterOp/classes/CIM_ProductServiceComponent.CIM_Component
install -D -m 0755  $PEGASUS_HOME/repository/root#PG_InterOp/classes/CIM_ProductPhysicalElements.#  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#PG_InterOp/classes/CIM_ProductPhysicalElements.#
install -D -m 0755  $PEGASUS_HOME/repository/root#PG_InterOp/classes/CIM_FRUPhysicalElements.#  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#PG_InterOp/classes/CIM_FRUPhysicalElements.#
install -D -m 0755  $PEGASUS_HOME/repository/root#PG_InterOp/classes/CIM_FRUIncludesProduct.#  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#PG_InterOp/classes/CIM_FRUIncludesProduct.#
install -D -m 0755  $PEGASUS_HOME/repository/root#PG_InterOp/classes/CIM_Capabilities.CIM_ManagedElement  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#PG_InterOp/classes/CIM_Capabilities.CIM_ManagedElement
install -D -m 0755  $PEGASUS_HOME/repository/root#PG_InterOp/classes/CIM_ElementCapabilities.#  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#PG_InterOp/classes/CIM_ElementCapabilities.#
install -D -m 0755  $PEGASUS_HOME/repository/root#PG_InterOp/classes/CIM_SettingData.CIM_ManagedElement  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#PG_InterOp/classes/CIM_SettingData.CIM_ManagedElement
install -D -m 0755  $PEGASUS_HOME/repository/root#PG_InterOp/classes/CIM_ScopedSettingData.CIM_SettingData  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#PG_InterOp/classes/CIM_ScopedSettingData.CIM_SettingData
install -D -m 0755  $PEGASUS_HOME/repository/root#PG_InterOp/classes/CIM_ScopedSetting.CIM_HostedDependency  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#PG_InterOp/classes/CIM_ScopedSetting.CIM_HostedDependency
install -D -m 0755  $PEGASUS_HOME/repository/root#PG_InterOp/classes/CIM_ElementSettingData.#  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#PG_InterOp/classes/CIM_ElementSettingData.#
install -D -m 0755  $PEGASUS_HOME/repository/root#PG_InterOp/classes/CIM_Profile.CIM_Collection  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#PG_InterOp/classes/CIM_Profile.CIM_Collection
install -D -m 0755  $PEGASUS_HOME/repository/root#PG_InterOp/classes/CIM_ElementProfile.#  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#PG_InterOp/classes/CIM_ElementProfile.#
install -D -m 0755  $PEGASUS_HOME/repository/root#PG_InterOp/classes/CIM_Setting.CIM_ManagedElement  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#PG_InterOp/classes/CIM_Setting.CIM_ManagedElement
install -D -m 0755  $PEGASUS_HOME/repository/root#PG_InterOp/classes/CIM_ElementSetting.#  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#PG_InterOp/classes/CIM_ElementSetting.#
install -D -m 0755  $PEGASUS_HOME/repository/root#PG_InterOp/classes/CIM_DefaultSetting.CIM_ElementSetting  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#PG_InterOp/classes/CIM_DefaultSetting.CIM_ElementSetting
install -D -m 0755  $PEGASUS_HOME/repository/root#PG_InterOp/classes/CIM_SystemSetting.CIM_Setting  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#PG_InterOp/classes/CIM_SystemSetting.CIM_Setting
install -D -m 0755  $PEGASUS_HOME/repository/root#PG_InterOp/classes/CIM_SettingForSystem.CIM_HostedDependency  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#PG_InterOp/classes/CIM_SettingForSystem.CIM_HostedDependency
install -D -m 0755  $PEGASUS_HOME/repository/root#PG_InterOp/classes/CIM_Configuration.CIM_ManagedElement  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#PG_InterOp/classes/CIM_Configuration.CIM_ManagedElement
install -D -m 0755  $PEGASUS_HOME/repository/root#PG_InterOp/classes/CIM_ConfigurationComponent.#  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#PG_InterOp/classes/CIM_ConfigurationComponent.#
install -D -m 0755  $PEGASUS_HOME/repository/root#PG_InterOp/classes/CIM_ElementConfiguration.#  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#PG_InterOp/classes/CIM_ElementConfiguration.#
install -D -m 0755  $PEGASUS_HOME/repository/root#PG_InterOp/classes/CIM_CollectionConfiguration.#  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#PG_InterOp/classes/CIM_CollectionConfiguration.#
install -D -m 0755  $PEGASUS_HOME/repository/root#PG_InterOp/classes/CIM_DependencyContext.#  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#PG_InterOp/classes/CIM_DependencyContext.#
install -D -m 0755  $PEGASUS_HOME/repository/root#PG_InterOp/classes/CIM_SettingContext.#  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#PG_InterOp/classes/CIM_SettingContext.#
install -D -m 0755  $PEGASUS_HOME/repository/root#PG_InterOp/classes/CIM_CollectionSetting.#  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#PG_InterOp/classes/CIM_CollectionSetting.#
install -D -m 0755  $PEGASUS_HOME/repository/root#PG_InterOp/classes/CIM_SystemConfiguration.CIM_ManagedElement  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#PG_InterOp/classes/CIM_SystemConfiguration.CIM_ManagedElement
install -D -m 0755  $PEGASUS_HOME/repository/root#PG_InterOp/classes/CIM_ConfigurationForSystem.CIM_HostedDependency  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#PG_InterOp/classes/CIM_ConfigurationForSystem.CIM_HostedDependency
install -D -m 0755  $PEGASUS_HOME/repository/root#PG_InterOp/classes/CIM_SystemSettingContext.#  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#PG_InterOp/classes/CIM_SystemSettingContext.#
install -D -m 0755  $PEGASUS_HOME/repository/root#PG_InterOp/classes/CIM_StatisticalData.CIM_ManagedElement  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#PG_InterOp/classes/CIM_StatisticalData.CIM_ManagedElement
install -D -m 0755  $PEGASUS_HOME/repository/root#PG_InterOp/classes/CIM_ElementStatisticalData.#  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#PG_InterOp/classes/CIM_ElementStatisticalData.#
install -D -m 0755  $PEGASUS_HOME/repository/root#PG_InterOp/classes/CIM_RelatedStatisticalData.#  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#PG_InterOp/classes/CIM_RelatedStatisticalData.#
install -D -m 0755  $PEGASUS_HOME/repository/root#PG_InterOp/classes/CIM_StatisticalInformation.CIM_ManagedElement  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#PG_InterOp/classes/CIM_StatisticalInformation.CIM_ManagedElement
install -D -m 0755  $PEGASUS_HOME/repository/root#PG_InterOp/classes/CIM_Statistics.#  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#PG_InterOp/classes/CIM_Statistics.#
install -D -m 0755  $PEGASUS_HOME/repository/root#PG_InterOp/classes/CIM_RelatedStatistics.#  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#PG_InterOp/classes/CIM_RelatedStatistics.#
install -D -m 0755  $PEGASUS_HOME/repository/root#PG_InterOp/classes/CIM_SystemStatisticalInformation.CIM_StatisticalInformation  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#PG_InterOp/classes/CIM_SystemStatisticalInformation.CIM_StatisticalInformation
install -D -m 0755  $PEGASUS_HOME/repository/root#PG_InterOp/classes/CIM_SystemStatistics.CIM_Statistics  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#PG_InterOp/classes/CIM_SystemStatistics.CIM_Statistics
install -D -m 0755  $PEGASUS_HOME/repository/root#PG_InterOp/classes/CIM_ServiceStatisticalInformation.CIM_StatisticalInformation  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#PG_InterOp/classes/CIM_ServiceStatisticalInformation.CIM_StatisticalInformation
install -D -m 0755  $PEGASUS_HOME/repository/root#PG_InterOp/classes/CIM_ServiceStatistics.CIM_Statistics  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#PG_InterOp/classes/CIM_ServiceStatistics.CIM_Statistics
install -D -m 0755  $PEGASUS_HOME/repository/root#PG_InterOp/classes/CIM_SAPStatisticalInformation.CIM_StatisticalInformation  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#PG_InterOp/classes/CIM_SAPStatisticalInformation.CIM_StatisticalInformation
install -D -m 0755  $PEGASUS_HOME/repository/root#PG_InterOp/classes/CIM_SAPStatistics.CIM_Statistics  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#PG_InterOp/classes/CIM_SAPStatistics.CIM_Statistics
install -D -m 0755  $PEGASUS_HOME/repository/root#PG_InterOp/classes/CIM_DeviceStatisticalInformation.CIM_StatisticalInformation  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#PG_InterOp/classes/CIM_DeviceStatisticalInformation.CIM_StatisticalInformation
install -D -m 0755  $PEGASUS_HOME/repository/root#PG_InterOp/classes/CIM_DeviceStatistics.CIM_Statistics  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#PG_InterOp/classes/CIM_DeviceStatistics.CIM_Statistics
install -D -m 0755  $PEGASUS_HOME/repository/root#PG_InterOp/classes/CIM_PhysicalStatisticalInformation.CIM_StatisticalInformation  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#PG_InterOp/classes/CIM_PhysicalStatisticalInformation.CIM_StatisticalInformation
install -D -m 0755  $PEGASUS_HOME/repository/root#PG_InterOp/classes/CIM_PhysicalStatistics.CIM_Statistics  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#PG_InterOp/classes/CIM_PhysicalStatistics.CIM_Statistics
install -D -m 0755  $PEGASUS_HOME/repository/root#PG_InterOp/classes/CIM_StatisticalSetting.CIM_SettingData  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#PG_InterOp/classes/CIM_StatisticalSetting.CIM_SettingData
install -D -m 0755  $PEGASUS_HOME/repository/root#PG_InterOp/classes/CIM_MethodParameters.CIM_ManagedElement  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#PG_InterOp/classes/CIM_MethodParameters.CIM_ManagedElement
install -D -m 0755  $PEGASUS_HOME/repository/root#PG_InterOp/classes/CIM_ParameterValueSources.#  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#PG_InterOp/classes/CIM_ParameterValueSources.#
install -D -m 0755  $PEGASUS_HOME/repository/root#PG_InterOp/classes/CIM_ParametersForMethod.#  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#PG_InterOp/classes/CIM_ParametersForMethod.#
install -D -m 0755  $PEGASUS_HOME/repository/root#PG_InterOp/classes/CIM_PowerManagementCapabilities.CIM_Capabilities  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#PG_InterOp/classes/CIM_PowerManagementCapabilities.CIM_Capabilities
install -D -m 0755  $PEGASUS_HOME/repository/root#PG_InterOp/classes/CIM_PowerManagementService.CIM_Service  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#PG_InterOp/classes/CIM_PowerManagementService.CIM_Service
install -D -m 0755  $PEGASUS_HOME/repository/root#PG_InterOp/classes/CIM_Indication.#  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#PG_InterOp/classes/CIM_Indication.#
install -D -m 0755  $PEGASUS_HOME/repository/root#PG_InterOp/classes/CIM_ClassIndication.CIM_Indication  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#PG_InterOp/classes/CIM_ClassIndication.CIM_Indication
install -D -m 0755  $PEGASUS_HOME/repository/root#PG_InterOp/classes/CIM_ClassCreation.CIM_ClassIndication  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#PG_InterOp/classes/CIM_ClassCreation.CIM_ClassIndication
install -D -m 0755  $PEGASUS_HOME/repository/root#PG_InterOp/classes/CIM_ClassDeletion.CIM_ClassIndication  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#PG_InterOp/classes/CIM_ClassDeletion.CIM_ClassIndication
install -D -m 0755  $PEGASUS_HOME/repository/root#PG_InterOp/classes/CIM_ClassModification.CIM_ClassIndication  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#PG_InterOp/classes/CIM_ClassModification.CIM_ClassIndication
install -D -m 0755  $PEGASUS_HOME/repository/root#PG_InterOp/classes/CIM_InstIndication.CIM_Indication  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#PG_InterOp/classes/CIM_InstIndication.CIM_Indication
install -D -m 0755  $PEGASUS_HOME/repository/root#PG_InterOp/classes/CIM_InstCreation.CIM_InstIndication  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#PG_InterOp/classes/CIM_InstCreation.CIM_InstIndication
install -D -m 0755  $PEGASUS_HOME/repository/root#PG_InterOp/classes/CIM_InstDeletion.CIM_InstIndication  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#PG_InterOp/classes/CIM_InstDeletion.CIM_InstIndication
install -D -m 0755  $PEGASUS_HOME/repository/root#PG_InterOp/classes/CIM_InstModification.CIM_InstIndication  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#PG_InterOp/classes/CIM_InstModification.CIM_InstIndication
install -D -m 0755  $PEGASUS_HOME/repository/root#PG_InterOp/classes/CIM_InstMethodCall.CIM_InstIndication  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#PG_InterOp/classes/CIM_InstMethodCall.CIM_InstIndication
install -D -m 0755  $PEGASUS_HOME/repository/root#PG_InterOp/classes/CIM_InstRead.CIM_InstIndication  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#PG_InterOp/classes/CIM_InstRead.CIM_InstIndication
install -D -m 0755  $PEGASUS_HOME/repository/root#PG_InterOp/classes/CIM_ProcessIndication.CIM_Indication  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#PG_InterOp/classes/CIM_ProcessIndication.CIM_Indication
install -D -m 0755  $PEGASUS_HOME/repository/root#PG_InterOp/classes/CIM_SNMPTrapIndication.CIM_ProcessIndication  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#PG_InterOp/classes/CIM_SNMPTrapIndication.CIM_ProcessIndication
install -D -m 0755  $PEGASUS_HOME/repository/root#PG_InterOp/classes/CIM_AlertIndication.CIM_ProcessIndication  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#PG_InterOp/classes/CIM_AlertIndication.CIM_ProcessIndication
install -D -m 0755  $PEGASUS_HOME/repository/root#PG_InterOp/classes/CIM_ThresholdIndication.CIM_AlertIndication  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#PG_InterOp/classes/CIM_ThresholdIndication.CIM_AlertIndication
install -D -m 0755  $PEGASUS_HOME/repository/root#PG_InterOp/classes/CIM_AlertInstIndication.CIM_AlertIndication  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#PG_InterOp/classes/CIM_AlertInstIndication.CIM_AlertIndication
install -D -m 0755  $PEGASUS_HOME/repository/root#PG_InterOp/classes/CIM_IndicationFilter.CIM_ManagedElement  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#PG_InterOp/classes/CIM_IndicationFilter.CIM_ManagedElement
install -D -m 0755  $PEGASUS_HOME/repository/root#PG_InterOp/classes/CIM_ListenerDestination.CIM_ManagedElement  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#PG_InterOp/classes/CIM_ListenerDestination.CIM_ManagedElement
install -D -m 0755  $PEGASUS_HOME/repository/root#PG_InterOp/classes/CIM_ListenerDestinationCIMXML.CIM_ListenerDestination  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#PG_InterOp/classes/CIM_ListenerDestinationCIMXML.CIM_ListenerDestination
install -D -m 0755  $PEGASUS_HOME/repository/root#PG_InterOp/classes/CIM_IndicationHandler.CIM_ListenerDestination  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#PG_InterOp/classes/CIM_IndicationHandler.CIM_ListenerDestination
install -D -m 0755  $PEGASUS_HOME/repository/root#PG_InterOp/classes/CIM_IndicationHandlerCIMXML.CIM_IndicationHandler  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#PG_InterOp/classes/CIM_IndicationHandlerCIMXML.CIM_IndicationHandler
install -D -m 0755  $PEGASUS_HOME/repository/root#PG_InterOp/classes/CIM_IndicationSubscription.#  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#PG_InterOp/classes/CIM_IndicationSubscription.#
install -D -m 0755  $PEGASUS_HOME/repository/root#PG_InterOp/classes/PG_IndicationHandlerSNMPMapper.CIM_IndicationHandler  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#PG_InterOp/classes/PG_IndicationHandlerSNMPMapper.CIM_IndicationHandler
install -D -m 0755  $PEGASUS_HOME/repository/root#PG_InterOp/classes/PG_ProviderModule.CIM_LogicalElement  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#PG_InterOp/classes/PG_ProviderModule.CIM_LogicalElement
install -D -m 0755  $PEGASUS_HOME/repository/root#PG_InterOp/classes/PG_Provider.CIM_LogicalElement  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#PG_InterOp/classes/PG_Provider.CIM_LogicalElement
install -D -m 0755  $PEGASUS_HOME/repository/root#PG_InterOp/classes/PG_CapabilitiesRegistration.CIM_ManagedElement  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#PG_InterOp/classes/PG_CapabilitiesRegistration.CIM_ManagedElement
install -D -m 0755  $PEGASUS_HOME/repository/root#PG_InterOp/classes/PG_ProviderCapabilities.PG_CapabilitiesRegistration  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#PG_InterOp/classes/PG_ProviderCapabilities.PG_CapabilitiesRegistration
install -D -m 0755  $PEGASUS_HOME/repository/root#PG_InterOp/classes/PG_ConsumerCapabilities.PG_CapabilitiesRegistration  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#PG_InterOp/classes/PG_ConsumerCapabilities.PG_CapabilitiesRegistration
install -D -m 0755  $PEGASUS_HOME/repository/root#PG_InterOp/classes/PG_ProviderCapabilitiesElements.CIM_Dependency  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#PG_InterOp/classes/PG_ProviderCapabilitiesElements.CIM_Dependency
install -D -m 0755  $PEGASUS_HOME/repository/root#PG_InterOp/classes/PG_ProviderModuleElements.CIM_Dependency  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#PG_InterOp/classes/PG_ProviderModuleElements.CIM_Dependency
install -D -m 0755  $PEGASUS_HOME/repository/root#PG_InterOp/instances/PG_ProviderCapabilities.idx  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#PG_InterOp/instances/PG_ProviderCapabilities.idx
install -D -m 0755  $PEGASUS_HOME/repository/root#PG_InterOp/instances/PG_ProviderModule.instances  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#PG_InterOp/instances/PG_ProviderModule.instances
install -D -m 0755  $PEGASUS_HOME/repository/root#PG_InterOp/instances/PG_ProviderModule.idx  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#PG_InterOp/instances/PG_ProviderModule.idx
install -D -m 0755  $PEGASUS_HOME/repository/root#PG_InterOp/instances/PG_Provider.instances  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#PG_InterOp/instances/PG_Provider.instances
install -D -m 0755  $PEGASUS_HOME/repository/root#PG_InterOp/instances/PG_Provider.idx  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#PG_InterOp/instances/PG_Provider.idx
install -D -m 0755  $PEGASUS_HOME/repository/root#PG_InterOp/instances/PG_ProviderCapabilities.instances  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#PG_InterOp/instances/PG_ProviderCapabilities.instances
install -D -m 0755  $PEGASUS_HOME/repository/root#PG_InterOp/qualifiers/Abstract  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#PG_InterOp/qualifiers/Abstract
install -D -m 0755  $PEGASUS_HOME/repository/root#PG_InterOp/qualifiers/Aggregate  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#PG_InterOp/qualifiers/Aggregate
install -D -m 0755  $PEGASUS_HOME/repository/root#PG_InterOp/qualifiers/Aggregation  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#PG_InterOp/qualifiers/Aggregation
install -D -m 0755  $PEGASUS_HOME/repository/root#PG_InterOp/qualifiers/Alias  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#PG_InterOp/qualifiers/Alias
install -D -m 0755  $PEGASUS_HOME/repository/root#PG_InterOp/qualifiers/ArrayType  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#PG_InterOp/qualifiers/ArrayType
install -D -m 0755  $PEGASUS_HOME/repository/root#PG_InterOp/qualifiers/ASSOCIATION  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#PG_InterOp/qualifiers/ASSOCIATION
install -D -m 0755  $PEGASUS_HOME/repository/root#PG_InterOp/qualifiers/BitMap  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#PG_InterOp/qualifiers/BitMap
install -D -m 0755  $PEGASUS_HOME/repository/root#PG_InterOp/qualifiers/BitValues  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#PG_InterOp/qualifiers/BitValues
install -D -m 0755  $PEGASUS_HOME/repository/root#PG_InterOp/qualifiers/Composition  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#PG_InterOp/qualifiers/Composition
install -D -m 0755  $PEGASUS_HOME/repository/root#PG_InterOp/qualifiers/Counter  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#PG_InterOp/qualifiers/Counter
install -D -m 0755  $PEGASUS_HOME/repository/root#PG_InterOp/qualifiers/Delete  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#PG_InterOp/qualifiers/Delete
install -D -m 0755  $PEGASUS_HOME/repository/root#PG_InterOp/qualifiers/Deprecated  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#PG_InterOp/qualifiers/Deprecated
install -D -m 0755  $PEGASUS_HOME/repository/root#PG_InterOp/qualifiers/Description  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#PG_InterOp/qualifiers/Description
install -D -m 0755  $PEGASUS_HOME/repository/root#PG_InterOp/qualifiers/DisplayName  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#PG_InterOp/qualifiers/DisplayName
install -D -m 0755  $PEGASUS_HOME/repository/root#PG_InterOp/qualifiers/DN  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#PG_InterOp/qualifiers/DN
install -D -m 0755  $PEGASUS_HOME/repository/root#PG_InterOp/qualifiers/EmbeddedObject  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#PG_InterOp/qualifiers/EmbeddedObject
install -D -m 0755  $PEGASUS_HOME/repository/root#PG_InterOp/qualifiers/Exception  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#PG_InterOp/qualifiers/Exception
install -D -m 0755  $PEGASUS_HOME/repository/root#PG_InterOp/qualifiers/Expensive  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#PG_InterOp/qualifiers/Expensive
install -D -m 0755  $PEGASUS_HOME/repository/root#PG_InterOp/qualifiers/Experimental  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#PG_InterOp/qualifiers/Experimental
install -D -m 0755  $PEGASUS_HOME/repository/root#PG_InterOp/qualifiers/Gauge  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#PG_InterOp/qualifiers/Gauge
install -D -m 0755  $PEGASUS_HOME/repository/root#PG_InterOp/qualifiers/Ifdeleted  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#PG_InterOp/qualifiers/Ifdeleted
install -D -m 0755  $PEGASUS_HOME/repository/root#PG_InterOp/qualifiers/In  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#PG_InterOp/qualifiers/In
install -D -m 0755  $PEGASUS_HOME/repository/root#PG_InterOp/qualifiers/INDICATION  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#PG_InterOp/qualifiers/INDICATION
install -D -m 0755  $PEGASUS_HOME/repository/root#PG_InterOp/qualifiers/Invisible  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#PG_InterOp/qualifiers/Invisible
install -D -m 0755  $PEGASUS_HOME/repository/root#PG_InterOp/qualifiers/Key  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#PG_InterOp/qualifiers/Key
install -D -m 0755  $PEGASUS_HOME/repository/root#PG_InterOp/qualifiers/Large  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#PG_InterOp/qualifiers/Large
install -D -m 0755  $PEGASUS_HOME/repository/root#PG_InterOp/qualifiers/MappingStrings  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#PG_InterOp/qualifiers/MappingStrings
install -D -m 0755  $PEGASUS_HOME/repository/root#PG_InterOp/qualifiers/Max  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#PG_InterOp/qualifiers/Max
install -D -m 0755  $PEGASUS_HOME/repository/root#PG_InterOp/qualifiers/MaxLen  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#PG_InterOp/qualifiers/MaxLen
install -D -m 0755  $PEGASUS_HOME/repository/root#PG_InterOp/qualifiers/MaxValue  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#PG_InterOp/qualifiers/MaxValue
install -D -m 0755  $PEGASUS_HOME/repository/root#PG_InterOp/qualifiers/Min  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#PG_InterOp/qualifiers/Min
install -D -m 0755  $PEGASUS_HOME/repository/root#PG_InterOp/qualifiers/MinLen  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#PG_InterOp/qualifiers/MinLen
install -D -m 0755  $PEGASUS_HOME/repository/root#PG_InterOp/qualifiers/MinValue  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#PG_InterOp/qualifiers/MinValue
install -D -m 0755  $PEGASUS_HOME/repository/root#PG_InterOp/qualifiers/ModelCorrespondence  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#PG_InterOp/qualifiers/ModelCorrespondence
install -D -m 0755  $PEGASUS_HOME/repository/root#PG_InterOp/qualifiers/Nonlocal  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#PG_InterOp/qualifiers/Nonlocal
install -D -m 0755  $PEGASUS_HOME/repository/root#PG_InterOp/qualifiers/NonlocalType  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#PG_InterOp/qualifiers/NonlocalType
install -D -m 0755  $PEGASUS_HOME/repository/root#PG_InterOp/qualifiers/NullValue  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#PG_InterOp/qualifiers/NullValue
install -D -m 0755  $PEGASUS_HOME/repository/root#PG_InterOp/qualifiers/Octetstring  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#PG_InterOp/qualifiers/Octetstring
install -D -m 0755  $PEGASUS_HOME/repository/root#PG_InterOp/qualifiers/Out  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#PG_InterOp/qualifiers/Out
install -D -m 0755  $PEGASUS_HOME/repository/root#PG_InterOp/qualifiers/Override  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#PG_InterOp/qualifiers/Override
install -D -m 0755  $PEGASUS_HOME/repository/root#PG_InterOp/qualifiers/Propagated  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#PG_InterOp/qualifiers/Propagated
install -D -m 0755  $PEGASUS_HOME/repository/root#PG_InterOp/qualifiers/PropertyUsage  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#PG_InterOp/qualifiers/PropertyUsage
install -D -m 0755  $PEGASUS_HOME/repository/root#PG_InterOp/qualifiers/Provider  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#PG_InterOp/qualifiers/Provider
install -D -m 0755  $PEGASUS_HOME/repository/root#PG_InterOp/qualifiers/Read  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#PG_InterOp/qualifiers/Read
install -D -m 0755  $PEGASUS_HOME/repository/root#PG_InterOp/qualifiers/Required  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#PG_InterOp/qualifiers/Required
install -D -m 0755  $PEGASUS_HOME/repository/root#PG_InterOp/qualifiers/Revision  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#PG_InterOp/qualifiers/Revision
install -D -m 0755  $PEGASUS_HOME/repository/root#PG_InterOp/qualifiers/CLASS  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#PG_InterOp/qualifiers/CLASS
install -D -m 0755  $PEGASUS_HOME/repository/root#PG_InterOp/qualifiers/Source  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#PG_InterOp/qualifiers/Source
install -D -m 0755  $PEGASUS_HOME/repository/root#PG_InterOp/qualifiers/SourceType  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#PG_InterOp/qualifiers/SourceType
install -D -m 0755  $PEGASUS_HOME/repository/root#PG_InterOp/qualifiers/Static  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#PG_InterOp/qualifiers/Static
install -D -m 0755  $PEGASUS_HOME/repository/root#PG_InterOp/qualifiers/Syntax  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#PG_InterOp/qualifiers/Syntax
install -D -m 0755  $PEGASUS_HOME/repository/root#PG_InterOp/qualifiers/SyntaxType  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#PG_InterOp/qualifiers/SyntaxType
install -D -m 0755  $PEGASUS_HOME/repository/root#PG_InterOp/qualifiers/Terminal  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#PG_InterOp/qualifiers/Terminal
install -D -m 0755  $PEGASUS_HOME/repository/root#PG_InterOp/qualifiers/TriggerType  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#PG_InterOp/qualifiers/TriggerType
install -D -m 0755  $PEGASUS_HOME/repository/root#PG_InterOp/qualifiers/Units  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#PG_InterOp/qualifiers/Units
install -D -m 0755  $PEGASUS_HOME/repository/root#PG_InterOp/qualifiers/UnknownValues  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#PG_InterOp/qualifiers/UnknownValues
install -D -m 0755  $PEGASUS_HOME/repository/root#PG_InterOp/qualifiers/UnsupportedValues  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#PG_InterOp/qualifiers/UnsupportedValues
install -D -m 0755  $PEGASUS_HOME/repository/root#PG_InterOp/qualifiers/ValueMap  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#PG_InterOp/qualifiers/ValueMap
install -D -m 0755  $PEGASUS_HOME/repository/root#PG_InterOp/qualifiers/Values  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#PG_InterOp/qualifiers/Values
install -D -m 0755  $PEGASUS_HOME/repository/root#PG_InterOp/qualifiers/Version  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#PG_InterOp/qualifiers/Version
install -D -m 0755  $PEGASUS_HOME/repository/root#PG_InterOp/qualifiers/Weak  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#PG_InterOp/qualifiers/Weak
install -D -m 0755  $PEGASUS_HOME/repository/root#PG_InterOp/qualifiers/Write  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#PG_InterOp/qualifiers/Write
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_ManagedElement.#  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_ManagedElement.#
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/associations  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/associations
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_Dependency.#  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_Dependency.#
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_ConcreteDependency.CIM_Dependency  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_ConcreteDependency.CIM_Dependency
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_Component.#  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_Component.#
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_ConcreteComponent.CIM_Component  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_ConcreteComponent.CIM_Component
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_LogicalIdentity.#  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_LogicalIdentity.#
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_ConcreteIdentity.CIM_LogicalIdentity  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_ConcreteIdentity.CIM_LogicalIdentity
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_ManagedSystemElement.CIM_ManagedElement  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_ManagedSystemElement.CIM_ManagedElement
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_HostedDependency.CIM_Dependency  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_HostedDependency.CIM_Dependency
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_LogicalElement.CIM_ManagedSystemElement  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_LogicalElement.CIM_ManagedSystemElement
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_Job.CIM_LogicalElement  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_Job.CIM_LogicalElement
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_ConcreteJob.CIM_Job  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_ConcreteJob.CIM_Job
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_EnabledLogicalElement.CIM_LogicalElement  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_EnabledLogicalElement.CIM_LogicalElement
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_Synchronized.#  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_Synchronized.#
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_System.CIM_EnabledLogicalElement  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_System.CIM_EnabledLogicalElement
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_SystemComponent.CIM_Component  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_SystemComponent.CIM_Component
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_AdminDomain.CIM_System  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_AdminDomain.CIM_System
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_ContainedDomain.CIM_SystemComponent  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_ContainedDomain.CIM_SystemComponent
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_Service.CIM_EnabledLogicalElement  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_Service.CIM_EnabledLogicalElement
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_HostedService.CIM_HostedDependency  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_HostedService.CIM_HostedDependency
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_ProvidesServiceToElement.CIM_Dependency  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_ProvidesServiceToElement.CIM_Dependency
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_ServiceAvailableToElement.#  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_ServiceAvailableToElement.#
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_ServiceAffectsElement.#  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_ServiceAffectsElement.#
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_ServiceServiceDependency.CIM_ProvidesServiceToElement  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_ServiceServiceDependency.CIM_ProvidesServiceToElement
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_ServiceComponent.CIM_Component  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_ServiceComponent.CIM_Component
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_ServiceAccessPoint.CIM_EnabledLogicalElement  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_ServiceAccessPoint.CIM_EnabledLogicalElement
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_HostedAccessPoint.CIM_HostedDependency  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_HostedAccessPoint.CIM_HostedDependency
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_SAPAvailableForElement.#  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_SAPAvailableForElement.#
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_ServiceSAPDependency.CIM_Dependency  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_ServiceSAPDependency.CIM_Dependency
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_ServiceAccessBySAP.CIM_Dependency  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_ServiceAccessBySAP.CIM_Dependency
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_SAPSAPDependency.CIM_Dependency  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_SAPSAPDependency.CIM_Dependency
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_ActiveConnection.CIM_SAPSAPDependency  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_ActiveConnection.CIM_SAPSAPDependency
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_RemoteServiceAccessPoint.CIM_ServiceAccessPoint  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_RemoteServiceAccessPoint.CIM_ServiceAccessPoint
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_RemotePort.CIM_RemoteServiceAccessPoint  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_RemotePort.CIM_RemoteServiceAccessPoint
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_ServiceAccessURI.CIM_ServiceAccessPoint  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_ServiceAccessURI.CIM_ServiceAccessPoint
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_ProtocolEndpoint.CIM_ServiceAccessPoint  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_ProtocolEndpoint.CIM_ServiceAccessPoint
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_BindsTo.CIM_SAPSAPDependency  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_BindsTo.CIM_SAPSAPDependency
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_ProvidesEndpoint.CIM_ServiceAccessBySAP  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_ProvidesEndpoint.CIM_ServiceAccessBySAP
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_PhysicalElement.CIM_ManagedSystemElement  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_PhysicalElement.CIM_ManagedSystemElement
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_Location.CIM_ManagedElement  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_Location.CIM_ManagedElement
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_ElementLocation.#  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_ElementLocation.#
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_PhysicalElementLocation.CIM_ElementLocation  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_PhysicalElementLocation.CIM_ElementLocation
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_ContainedLocation.CIM_Component  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_ContainedLocation.CIM_Component
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_SystemPackaging.CIM_Dependency  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_SystemPackaging.CIM_Dependency
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_SoftwareIdentity.CIM_LogicalElement  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_SoftwareIdentity.CIM_LogicalElement
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_ElementSoftwareIdentity.CIM_Dependency  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_ElementSoftwareIdentity.CIM_Dependency
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_InstalledSoftwareIdentity.#  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_InstalledSoftwareIdentity.#
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_LogicalDevice.CIM_EnabledLogicalElement  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_LogicalDevice.CIM_EnabledLogicalElement
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_Realizes.CIM_Dependency  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_Realizes.CIM_Dependency
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_SystemDevice.CIM_SystemComponent  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_SystemDevice.CIM_SystemComponent
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_DeviceServiceImplementation.CIM_Dependency  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_DeviceServiceImplementation.CIM_Dependency
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_DeviceSAPImplementation.CIM_Dependency  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_DeviceSAPImplementation.CIM_Dependency
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_StorageExtent.CIM_LogicalDevice  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_StorageExtent.CIM_LogicalDevice
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_BasedOn.CIM_Dependency  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_BasedOn.CIM_Dependency
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_Collection.CIM_ManagedElement  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_Collection.CIM_ManagedElement
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_MemberOfCollection.#  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_MemberOfCollection.#
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_OrderedMemberOfCollection.CIM_MemberOfCollection  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_OrderedMemberOfCollection.CIM_MemberOfCollection
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_SystemSpecificCollection.CIM_Collection  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_SystemSpecificCollection.CIM_Collection
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_ConcreteCollection.CIM_Collection  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_ConcreteCollection.CIM_Collection
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_HostedCollection.CIM_HostedDependency  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_HostedCollection.CIM_HostedDependency
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_CollectionOfMSEs.CIM_Collection  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_CollectionOfMSEs.CIM_Collection
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_CollectedCollections.#  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_CollectedCollections.#
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_CollectedMSEs.CIM_MemberOfCollection  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_CollectedMSEs.CIM_MemberOfCollection
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_RedundancyGroup.CIM_LogicalElement  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_RedundancyGroup.CIM_LogicalElement
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_RedundancyComponent.CIM_Component  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_RedundancyComponent.CIM_Component
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_SpareGroup.CIM_RedundancyGroup  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_SpareGroup.CIM_RedundancyGroup
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_ActsAsSpare.#  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_ActsAsSpare.#
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_StorageRedundancyGroup.CIM_RedundancyGroup  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_StorageRedundancyGroup.CIM_RedundancyGroup
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_ExtraCapacityGroup.CIM_RedundancyGroup  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_ExtraCapacityGroup.CIM_RedundancyGroup
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_ExtentRedundancyComponent.CIM_RedundancyComponent  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_ExtentRedundancyComponent.CIM_RedundancyComponent
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_Product.CIM_ManagedElement  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_Product.CIM_ManagedElement
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_ProductComponent.CIM_Component  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_ProductComponent.CIM_Component
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_ProductParentChild.#  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_ProductParentChild.#
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_CompatibleProduct.#  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_CompatibleProduct.#
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_ProductProductDependency.#  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_ProductProductDependency.#
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_SupportAccess.CIM_ManagedElement  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_SupportAccess.CIM_ManagedElement
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_ProductSupport.#  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_ProductSupport.#
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_FRU.CIM_ManagedElement  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_FRU.CIM_ManagedElement
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_ProductFRU.#  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_ProductFRU.#
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_ReplaceableProductFRU.#  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_ReplaceableProductFRU.#
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_ProductPhysicalComponent.CIM_Component  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_ProductPhysicalComponent.CIM_Component
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_ProductSoftwareComponent.CIM_Component  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_ProductSoftwareComponent.CIM_Component
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_ProductServiceComponent.CIM_Component  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_ProductServiceComponent.CIM_Component
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_ProductPhysicalElements.#  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_ProductPhysicalElements.#
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_FRUPhysicalElements.#  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_FRUPhysicalElements.#
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_FRUIncludesProduct.#  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_FRUIncludesProduct.#
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_Capabilities.CIM_ManagedElement  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_Capabilities.CIM_ManagedElement
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_ElementCapabilities.#  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_ElementCapabilities.#
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_SettingData.CIM_ManagedElement  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_SettingData.CIM_ManagedElement
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_ScopedSettingData.CIM_SettingData  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_ScopedSettingData.CIM_SettingData
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_ScopedSetting.CIM_HostedDependency  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_ScopedSetting.CIM_HostedDependency
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_ElementSettingData.#  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_ElementSettingData.#
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_Profile.CIM_Collection  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_Profile.CIM_Collection
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_ElementProfile.#  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_ElementProfile.#
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_Setting.CIM_ManagedElement  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_Setting.CIM_ManagedElement
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_ElementSetting.#  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_ElementSetting.#
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_DefaultSetting.CIM_ElementSetting  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_DefaultSetting.CIM_ElementSetting
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_SystemSetting.CIM_Setting  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_SystemSetting.CIM_Setting
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_SettingForSystem.CIM_HostedDependency  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_SettingForSystem.CIM_HostedDependency
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_Configuration.CIM_ManagedElement  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_Configuration.CIM_ManagedElement
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_ConfigurationComponent.#  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_ConfigurationComponent.#
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_ElementConfiguration.#  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_ElementConfiguration.#
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_CollectionConfiguration.#  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_CollectionConfiguration.#
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_DependencyContext.#  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_DependencyContext.#
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_SettingContext.#  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_SettingContext.#
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_CollectionSetting.#  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_CollectionSetting.#
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_SystemConfiguration.CIM_ManagedElement  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_SystemConfiguration.CIM_ManagedElement
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_ConfigurationForSystem.CIM_HostedDependency  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_ConfigurationForSystem.CIM_HostedDependency
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_SystemSettingContext.#  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_SystemSettingContext.#
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_StatisticalData.CIM_ManagedElement  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_StatisticalData.CIM_ManagedElement
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_ElementStatisticalData.#  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_ElementStatisticalData.#
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_RelatedStatisticalData.#  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_RelatedStatisticalData.#
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_StatisticalInformation.CIM_ManagedElement  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_StatisticalInformation.CIM_ManagedElement
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_Statistics.#  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_Statistics.#
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_RelatedStatistics.#  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_RelatedStatistics.#
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_SystemStatisticalInformation.CIM_StatisticalInformation  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_SystemStatisticalInformation.CIM_StatisticalInformation
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_SystemStatistics.CIM_Statistics  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_SystemStatistics.CIM_Statistics
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_ServiceStatisticalInformation.CIM_StatisticalInformation  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_ServiceStatisticalInformation.CIM_StatisticalInformation
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_ServiceStatistics.CIM_Statistics  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_ServiceStatistics.CIM_Statistics
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_SAPStatisticalInformation.CIM_StatisticalInformation  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_SAPStatisticalInformation.CIM_StatisticalInformation
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_SAPStatistics.CIM_Statistics  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_SAPStatistics.CIM_Statistics
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_DeviceStatisticalInformation.CIM_StatisticalInformation  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_DeviceStatisticalInformation.CIM_StatisticalInformation
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_DeviceStatistics.CIM_Statistics  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_DeviceStatistics.CIM_Statistics
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_PhysicalStatisticalInformation.CIM_StatisticalInformation  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_PhysicalStatisticalInformation.CIM_StatisticalInformation
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_PhysicalStatistics.CIM_Statistics  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_PhysicalStatistics.CIM_Statistics
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_StatisticalSetting.CIM_SettingData  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_StatisticalSetting.CIM_SettingData
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_MethodParameters.CIM_ManagedElement  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_MethodParameters.CIM_ManagedElement
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_ParameterValueSources.#  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_ParameterValueSources.#
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_ParametersForMethod.#  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_ParametersForMethod.#
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_PowerManagementCapabilities.CIM_Capabilities  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_PowerManagementCapabilities.CIM_Capabilities
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_PowerManagementService.CIM_Service  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_PowerManagementService.CIM_Service
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_PhysicalPackage.CIM_PhysicalElement  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_PhysicalPackage.CIM_PhysicalElement
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_Container.CIM_Component  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_Container.CIM_Component
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_PhysicalFrame.CIM_PhysicalPackage  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_PhysicalFrame.CIM_PhysicalPackage
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_Rack.CIM_PhysicalFrame  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_Rack.CIM_PhysicalFrame
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_Chassis.CIM_PhysicalFrame  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_Chassis.CIM_PhysicalFrame
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_PhysicalConnector.CIM_PhysicalElement  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_PhysicalConnector.CIM_PhysicalElement
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_ChassisInRack.CIM_Container  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_ChassisInRack.CIM_Container
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_PackageInChassis.CIM_Container  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_PackageInChassis.CIM_Container
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_Docked.CIM_Dependency  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_Docked.CIM_Dependency
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_Card.CIM_PhysicalPackage  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_Card.CIM_PhysicalPackage
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_SystemBusCard.CIM_Card  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_SystemBusCard.CIM_Card
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_CardOnCard.CIM_Container  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_CardOnCard.CIM_Container
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_PackageLocation.CIM_PhysicalPackage  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_PackageLocation.CIM_PhysicalPackage
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_StorageMediaLocation.CIM_PackageLocation  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_StorageMediaLocation.CIM_PackageLocation
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_Magazine.CIM_StorageMediaLocation  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_Magazine.CIM_StorageMediaLocation
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_MediaPhysicalStatData.CIM_StatisticalData  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_MediaPhysicalStatData.CIM_StatisticalData
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_MediaPhysicalStatInfo.CIM_PhysicalStatisticalInformation  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_MediaPhysicalStatInfo.CIM_PhysicalStatisticalInformation
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_ConnectedTo.CIM_Dependency  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_ConnectedTo.CIM_Dependency
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_Slot.CIM_PhysicalConnector  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_Slot.CIM_PhysicalConnector
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_SlotInSlot.CIM_ConnectedTo  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_SlotInSlot.CIM_ConnectedTo
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_AdjacentSlots.#  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_AdjacentSlots.#
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_PackageInConnector.CIM_Dependency  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_PackageInConnector.CIM_Dependency
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_PackageInSlot.CIM_PackageInConnector  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_PackageInSlot.CIM_PackageInConnector
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_CardInSlot.CIM_PackageInSlot  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_CardInSlot.CIM_PackageInSlot
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_ConnectorOnPackage.CIM_Container  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_ConnectorOnPackage.CIM_Container
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_PhysicalLink.CIM_PhysicalElement  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_PhysicalLink.CIM_PhysicalElement
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_ElementsLinked.CIM_Dependency  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_ElementsLinked.CIM_Dependency
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_LinkHasConnector.CIM_Component  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_LinkHasConnector.CIM_Component
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_PhysicalComponent.CIM_PhysicalElement  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_PhysicalComponent.CIM_PhysicalElement
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_PackagedComponent.CIM_Container  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_PackagedComponent.CIM_Container
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_Chip.CIM_PhysicalComponent  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_Chip.CIM_PhysicalComponent
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_PhysicalMemory.CIM_Chip  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_PhysicalMemory.CIM_Chip
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_MemoryOnCard.CIM_PackagedComponent  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_MemoryOnCard.CIM_PackagedComponent
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_PhysicalMedia.CIM_PhysicalComponent  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_PhysicalMedia.CIM_PhysicalComponent
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_MemoryWithMedia.CIM_Dependency  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_MemoryWithMedia.CIM_Dependency
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_PhysicalMediaInLocation.CIM_Dependency  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_PhysicalMediaInLocation.CIM_Dependency
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_PhysicalTape.CIM_PhysicalMedia  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_PhysicalTape.CIM_PhysicalMedia
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_HomeForMedia.CIM_Dependency  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_HomeForMedia.CIM_Dependency
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_ReplacementSet.CIM_Collection  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_ReplacementSet.CIM_Collection
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_ParticipatesInSet.#  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_ParticipatesInSet.#
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_PhysicalCapacity.CIM_ManagedElement  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_PhysicalCapacity.CIM_ManagedElement
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_ElementCapacity.#  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_ElementCapacity.#
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_MemoryCapacity.CIM_PhysicalCapacity  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_MemoryCapacity.CIM_PhysicalCapacity
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_ConfigurationCapacity.CIM_PhysicalCapacity  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_ConfigurationCapacity.CIM_PhysicalCapacity
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_ComputerSystem.CIM_System  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_ComputerSystem.CIM_System
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_ComputerSystemNodeCapabilities.CIM_Capabilities  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_ComputerSystemNodeCapabilities.CIM_Capabilities
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_ComponentCS.CIM_SystemComponent  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_ComponentCS.CIM_SystemComponent
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_UnitaryComputerSystem.CIM_ComputerSystem  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_UnitaryComputerSystem.CIM_ComputerSystem
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_SystemPartition.CIM_ComponentCS  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_SystemPartition.CIM_ComponentCS
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_VirtualComputerSystem.CIM_ComputerSystem  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_VirtualComputerSystem.CIM_ComputerSystem
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_HostingCS.CIM_HostedDependency  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_HostingCS.CIM_HostedDependency
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_Cluster.CIM_ComputerSystem  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_Cluster.CIM_ComputerSystem
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_ParticipatingCS.CIM_Dependency  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_ParticipatingCS.CIM_Dependency
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_Export.#  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_Export.#
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_ClusteringService.CIM_Service  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_ClusteringService.CIM_Service
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_ClusteringSAP.CIM_ServiceAccessPoint  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_ClusteringSAP.CIM_ServiceAccessPoint
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_ClusterServiceAccessBySAP.CIM_ServiceAccessBySAP  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_ClusterServiceAccessBySAP.CIM_ServiceAccessBySAP
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_HostedClusterService.CIM_HostedService  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_HostedClusterService.CIM_HostedService
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_HostedClusterSAP.CIM_HostedAccessPoint  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_HostedClusterSAP.CIM_HostedAccessPoint
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_ComputerSystemPackage.CIM_SystemPackaging  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_ComputerSystemPackage.CIM_SystemPackaging
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_FileSystem.CIM_EnabledLogicalElement  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_FileSystem.CIM_EnabledLogicalElement
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_LocalFileSystem.CIM_FileSystem  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_LocalFileSystem.CIM_FileSystem
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_RemoteFileSystem.CIM_FileSystem  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_RemoteFileSystem.CIM_FileSystem
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_NFS.CIM_RemoteFileSystem  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_NFS.CIM_RemoteFileSystem
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_FileSystemCapabilities.CIM_Capabilities  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_FileSystemCapabilities.CIM_Capabilities
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_FileSystemSettingData.CIM_ScopedSettingData  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_FileSystemSettingData.CIM_ScopedSettingData
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_LogicalFile.CIM_LogicalElement  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_LogicalFile.CIM_LogicalElement
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_DataFile.CIM_LogicalFile  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_DataFile.CIM_LogicalFile
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_Directory.CIM_LogicalFile  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_Directory.CIM_LogicalFile
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_DeviceFile.CIM_LogicalFile  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_DeviceFile.CIM_LogicalFile
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_FIFOPipeFile.CIM_LogicalFile  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_FIFOPipeFile.CIM_LogicalFile
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_SymbolicLink.CIM_LogicalFile  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_SymbolicLink.CIM_LogicalFile
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_DeviceAccessedByFile.CIM_Dependency  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_DeviceAccessedByFile.CIM_Dependency
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_DirectoryContainsFile.CIM_Component  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_DirectoryContainsFile.CIM_Component
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_Mount.CIM_Dependency  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_Mount.CIM_Dependency
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_HostedFileSystem.CIM_SystemComponent  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_HostedFileSystem.CIM_SystemComponent
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_FileStorage.CIM_Component  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_FileStorage.CIM_Component
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_ResidesOnExtent.CIM_Dependency  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_ResidesOnExtent.CIM_Dependency
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_OperatingSystem.CIM_EnabledLogicalElement  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_OperatingSystem.CIM_EnabledLogicalElement
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_InstalledOS.CIM_SystemComponent  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_InstalledOS.CIM_SystemComponent
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_RunningOS.CIM_Dependency  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_RunningOS.CIM_Dependency
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_BootOSFromFS.CIM_Dependency  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_BootOSFromFS.CIM_Dependency
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_Process.CIM_EnabledLogicalElement  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_Process.CIM_EnabledLogicalElement
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_OSProcess.CIM_Component  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_OSProcess.CIM_Component
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_ProcessExecutable.CIM_Dependency  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_ProcessExecutable.CIM_Dependency
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_ServiceProcess.#  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_ServiceProcess.#
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_Thread.CIM_EnabledLogicalElement  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_Thread.CIM_EnabledLogicalElement
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_ProcessThread.CIM_Component  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_ProcessThread.CIM_Component
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_OwningJobElement.#  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_OwningJobElement.#
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_AffectedJobElement.#  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_AffectedJobElement.#
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_ProcessOfJob.CIM_Component  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_ProcessOfJob.CIM_Component
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_JobDestination.CIM_EnabledLogicalElement  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_JobDestination.CIM_EnabledLogicalElement
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_HostedJobDestination.CIM_HostedDependency  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_HostedJobDestination.CIM_HostedDependency
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_JobDestinationJobs.CIM_Dependency  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_JobDestinationJobs.CIM_Dependency
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_BootService.CIM_Service  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_BootService.CIM_Service
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_BootSAP.CIM_ServiceAccessPoint  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_BootSAP.CIM_ServiceAccessPoint
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_BootServiceAccessBySAP.CIM_ServiceAccessBySAP  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_BootServiceAccessBySAP.CIM_ServiceAccessBySAP
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_HostedBootService.CIM_HostedService  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_HostedBootService.CIM_HostedService
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_HostedBootSAP.CIM_HostedAccessPoint  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_HostedBootSAP.CIM_HostedAccessPoint
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_OOBAlertService.CIM_Service  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_OOBAlertService.CIM_Service
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_WakeUpService.CIM_Service  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_WakeUpService.CIM_Service
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_TimeZone.CIM_Setting  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_TimeZone.CIM_Setting
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_UnixLocalFileSystem.CIM_LocalFileSystem  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_UnixLocalFileSystem.CIM_LocalFileSystem
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_UnixProcess.CIM_Process  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_UnixProcess.CIM_Process
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_UnixProcessStatisticalInformation.CIM_StatisticalInformation  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_UnixProcessStatisticalInformation.CIM_StatisticalInformation
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_UnixProcessStatistics.CIM_Statistics  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_UnixProcessStatistics.CIM_Statistics
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_UnixThread.CIM_Thread  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_UnixThread.CIM_Thread
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_UnixFile.CIM_LogicalElement  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_UnixFile.CIM_LogicalElement
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_FileIdentity.CIM_LogicalIdentity  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_FileIdentity.CIM_LogicalIdentity
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_UnixDirectory.CIM_Directory  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_UnixDirectory.CIM_Directory
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_UnixDeviceFile.CIM_DeviceFile  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_UnixDeviceFile.CIM_DeviceFile
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_SystemResource.CIM_EnabledLogicalElement  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_SystemResource.CIM_EnabledLogicalElement
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_IRQ.CIM_SystemResource  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_IRQ.CIM_SystemResource
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_MemoryMappedIO.CIM_SystemResource  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_MemoryMappedIO.CIM_SystemResource
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_MemoryResource.CIM_MemoryMappedIO  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_MemoryResource.CIM_MemoryMappedIO
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_PortResource.CIM_MemoryMappedIO  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_PortResource.CIM_MemoryMappedIO
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_DMA.CIM_SystemResource  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_DMA.CIM_SystemResource
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_ResourceOfSystem.CIM_SystemComponent  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_ResourceOfSystem.CIM_SystemComponent
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_ComputerSystemResource.CIM_ResourceOfSystem  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_ComputerSystemResource.CIM_ResourceOfSystem
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_ComputerSystemIRQ.CIM_ComputerSystemResource  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_ComputerSystemIRQ.CIM_ComputerSystemResource
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_ComputerSystemMappedIO.CIM_ComputerSystemResource  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_ComputerSystemMappedIO.CIM_ComputerSystemResource
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_ComputerSystemDMA.CIM_ComputerSystemResource  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_ComputerSystemDMA.CIM_ComputerSystemResource
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_AllocatedResource.CIM_Dependency  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_AllocatedResource.CIM_Dependency
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_AllocatedDMA.CIM_AllocatedResource  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_AllocatedDMA.CIM_AllocatedResource
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_MessageLog.CIM_EnabledLogicalElement  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_MessageLog.CIM_EnabledLogicalElement
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_LogRecord.CIM_ManagedElement  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_LogRecord.CIM_ManagedElement
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_RecordInLog.#  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_RecordInLog.#
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_LogInDeviceFile.CIM_Dependency  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_LogInDeviceFile.CIM_Dependency
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_LogInDataFile.CIM_Dependency  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_LogInDataFile.CIM_Dependency
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_LogInStorage.CIM_Dependency  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_LogInStorage.CIM_Dependency
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_UseOfMessageLog.CIM_Dependency  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_UseOfMessageLog.CIM_Dependency
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_OperationLog.CIM_UseOfMessageLog  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_OperationLog.CIM_UseOfMessageLog
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_DiagnosticSetting.CIM_Setting  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_DiagnosticSetting.CIM_Setting
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_DiagnosticResult.#  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_DiagnosticResult.#
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_DiagnosticTest.CIM_Service  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_DiagnosticTest.CIM_Service
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_DiagnosticResultForMSE.#  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_DiagnosticResultForMSE.#
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_DiagnosticResultForTest.#  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_DiagnosticResultForTest.#
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_DiagnosticTestForMSE.CIM_ProvidesServiceToElement  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_DiagnosticTestForMSE.CIM_ProvidesServiceToElement
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_DiagnosticTestInPackage.CIM_Component  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_DiagnosticTestInPackage.CIM_Component
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_DiagnosticResultInPackage.#  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_DiagnosticResultInPackage.#
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_DiagnosticSettingForTest.CIM_ElementSetting  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_DiagnosticSettingForTest.CIM_ElementSetting
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_Indication.#  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_Indication.#
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_ClassIndication.CIM_Indication  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_ClassIndication.CIM_Indication
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_ClassCreation.CIM_ClassIndication  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_ClassCreation.CIM_ClassIndication
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_ClassDeletion.CIM_ClassIndication  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_ClassDeletion.CIM_ClassIndication
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_IndicationSubscription.#  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_IndicationSubscription.#
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_ClassModification.CIM_ClassIndication  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_ClassModification.CIM_ClassIndication
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_InstIndication.CIM_Indication  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_InstIndication.CIM_Indication
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_InstCreation.CIM_InstIndication  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_InstCreation.CIM_InstIndication
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_InstDeletion.CIM_InstIndication  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_InstDeletion.CIM_InstIndication
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_InstModification.CIM_InstIndication  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_InstModification.CIM_InstIndication
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_InstMethodCall.CIM_InstIndication  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_InstMethodCall.CIM_InstIndication
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_InstRead.CIM_InstIndication  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_InstRead.CIM_InstIndication
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_ProcessIndication.CIM_Indication  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_ProcessIndication.CIM_Indication
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_SNMPTrapIndication.CIM_ProcessIndication  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_SNMPTrapIndication.CIM_ProcessIndication
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_AlertIndication.CIM_ProcessIndication  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_AlertIndication.CIM_ProcessIndication
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_ThresholdIndication.CIM_AlertIndication  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_ThresholdIndication.CIM_AlertIndication
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_AlertInstIndication.CIM_AlertIndication  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_AlertInstIndication.CIM_AlertIndication
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_IndicationFilter.CIM_ManagedElement  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_IndicationFilter.CIM_ManagedElement
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_ListenerDestination.CIM_ManagedElement  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_ListenerDestination.CIM_ManagedElement
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_ListenerDestinationCIMXML.CIM_ListenerDestination  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_ListenerDestinationCIMXML.CIM_ListenerDestination
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_IndicationHandler.CIM_ListenerDestination  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_IndicationHandler.CIM_ListenerDestination
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_IndicationHandlerCIMXML.CIM_IndicationHandler  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_IndicationHandlerCIMXML.CIM_IndicationHandler
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_WBEMService.CIM_Service  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_WBEMService.CIM_Service
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_ObjectManager.CIM_WBEMService  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_ObjectManager.CIM_WBEMService
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_ObjectManagerCommunicationMechanism.CIM_ServiceAccessPoint  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_ObjectManagerCommunicationMechanism.CIM_ServiceAccessPoint
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_CIMXMLCommunicationMechanism.CIM_ObjectManagerCommunicationMechanism  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_CIMXMLCommunicationMechanism.CIM_ObjectManagerCommunicationMechanism
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_CommMechanismForManager.CIM_ServiceAccessBySAP  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_CommMechanismForManager.CIM_ServiceAccessBySAP
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_ProtocolAdapter.CIM_WBEMService  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_ProtocolAdapter.CIM_WBEMService
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_ObjectManagerAdapter.CIM_WBEMService  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_ObjectManagerAdapter.CIM_WBEMService
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_CommMechanismForAdapter.CIM_Dependency  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_CommMechanismForAdapter.CIM_Dependency
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_CommMechanismForObjectManagerAdapter.CIM_Dependency  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_CommMechanismForObjectManagerAdapter.CIM_Dependency
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_Namespace.CIM_ManagedElement  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_Namespace.CIM_ManagedElement
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_SystemIdentification.CIM_ManagedElement  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_SystemIdentification.CIM_ManagedElement
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_NamespaceInManager.CIM_HostedDependency  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_NamespaceInManager.CIM_HostedDependency
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_SystemInNamespace.#  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_SystemInNamespace.#
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_IdentificationOfManagedSystem.CIM_Dependency  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_IdentificationOfManagedSystem.CIM_Dependency
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_CIMOMStatisticalData.CIM_StatisticalData  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_CIMOMStatisticalData.CIM_StatisticalData
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_RegisteredProfile.CIM_ManagedElement  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_RegisteredProfile.CIM_ManagedElement
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_RegisteredSubProfile.CIM_RegisteredProfile  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_RegisteredSubProfile.CIM_RegisteredProfile
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_ReferencedProfile.CIM_Dependency  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_ReferencedProfile.CIM_Dependency
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_SubProfileRequiresProfile.CIM_ReferencedProfile  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_SubProfileRequiresProfile.CIM_ReferencedProfile
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_ElementConformsToProfile.#  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_ElementConformsToProfile.#
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_MemberPrincipal.CIM_MemberOfCollection  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_MemberPrincipal.CIM_MemberOfCollection
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_CollectionInSystem.#  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_CollectionInSystem.#
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_OrganizationalEntity.CIM_ManagedElement  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_OrganizationalEntity.CIM_ManagedElement
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_OrgStructure.#  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_OrgStructure.#
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_CollectionInOrganization.#  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_CollectionInOrganization.#
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_Organization.CIM_OrganizationalEntity  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_Organization.CIM_OrganizationalEntity
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_OtherOrganizationInformation.CIM_ManagedElement  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_OtherOrganizationInformation.CIM_ManagedElement
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_MoreOrganizationInfo.CIM_Dependency  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_MoreOrganizationInfo.CIM_Dependency
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_OrgUnit.CIM_OrganizationalEntity  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_OrgUnit.CIM_OrganizationalEntity
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_OtherOrgUnitInformation.CIM_ManagedElement  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_OtherOrgUnitInformation.CIM_ManagedElement
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_MoreOrgUnitInfo.CIM_Dependency  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_MoreOrgUnitInfo.CIM_Dependency
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_UserEntity.CIM_OrganizationalEntity  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_UserEntity.CIM_OrganizationalEntity
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_UserContact.CIM_UserEntity  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_UserContact.CIM_UserEntity
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_Person.CIM_UserContact  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_Person.CIM_UserContact
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_SystemAdministrator.CIM_Dependency  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_SystemAdministrator.CIM_Dependency
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_OtherPersonInformation.CIM_ManagedElement  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_OtherPersonInformation.CIM_ManagedElement
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_MorePersonInfo.CIM_Dependency  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_MorePersonInfo.CIM_Dependency
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_Group.CIM_Collection  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_Group.CIM_Collection
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_OtherGroupInformation.CIM_ManagedElement  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_OtherGroupInformation.CIM_ManagedElement
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_MoreGroupInfo.CIM_Dependency  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_MoreGroupInfo.CIM_Dependency
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_SystemAdministratorGroup.CIM_Dependency  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_SystemAdministratorGroup.CIM_Dependency
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_Role.CIM_Collection  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_Role.CIM_Collection
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_OtherRoleInformation.CIM_ManagedElement  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_OtherRoleInformation.CIM_ManagedElement
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_MoreRoleInfo.CIM_Dependency  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_MoreRoleInfo.CIM_Dependency
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_SystemAdministratorRole.CIM_Dependency  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_SystemAdministratorRole.CIM_Dependency
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_SecurityService.CIM_Service  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_SecurityService.CIM_Service
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_ServiceUsesSecurityService.CIM_ServiceServiceDependency  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_ServiceUsesSecurityService.CIM_ServiceServiceDependency
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_SecurityServiceForSystem.CIM_ProvidesServiceToElement  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_SecurityServiceForSystem.CIM_ProvidesServiceToElement
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_AuthenticationService.CIM_SecurityService  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_AuthenticationService.CIM_SecurityService
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_VerificationService.CIM_AuthenticationService  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_VerificationService.CIM_AuthenticationService
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_CredentialManagementService.CIM_AuthenticationService  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_CredentialManagementService.CIM_AuthenticationService
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_TrustHierarchy.CIM_Dependency  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_TrustHierarchy.CIM_Dependency
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_CredentialManagementSAP.CIM_ServiceAccessPoint  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_CredentialManagementSAP.CIM_ServiceAccessPoint
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_LocalCredentialManagementService.CIM_CredentialManagementService  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_LocalCredentialManagementService.CIM_CredentialManagementService
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_AuthorizationService.CIM_SecurityService  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_AuthorizationService.CIM_SecurityService
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_Credential.CIM_ManagedElement  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_Credential.CIM_ManagedElement
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_ManagedCredential.CIM_Dependency  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_ManagedCredential.CIM_Dependency
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_Identity.CIM_ManagedElement  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_Identity.CIM_ManagedElement
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_IdentityContext.#  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_IdentityContext.#
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_AssignedIdentity.#  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_AssignedIdentity.#
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_IPNetworkIdentity.CIM_Identity  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_IPNetworkIdentity.CIM_Identity
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_SecuritySensitivity.CIM_ManagedElement  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_SecuritySensitivity.CIM_ManagedElement
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_ElementSecuritySensitivity.#  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_ElementSecuritySensitivity.#
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_AuthenticationRequirement.CIM_LogicalElement  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_AuthenticationRequirement.CIM_LogicalElement
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_HostedAuthenticationRequirement.CIM_HostedDependency  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_HostedAuthenticationRequirement.CIM_HostedDependency
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_SharedSecret.CIM_Credential  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_SharedSecret.CIM_Credential
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_AuthenticateForUse.CIM_Dependency  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_AuthenticateForUse.CIM_Dependency
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_RequireCredentialsFrom.CIM_Dependency  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_RequireCredentialsFrom.CIM_Dependency
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_AuthenticationTarget.CIM_Dependency  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_AuthenticationTarget.CIM_Dependency
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_KerberosKeyDistributionCenter.CIM_CredentialManagementService  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_KerberosKeyDistributionCenter.CIM_CredentialManagementService
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_KerberosTicket.CIM_Credential  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_KerberosTicket.CIM_Credential
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_KDCIssuesKerberosTicket.CIM_ManagedCredential  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_KDCIssuesKerberosTicket.CIM_ManagedCredential
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_CertificateAuthority.CIM_CredentialManagementService  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_CertificateAuthority.CIM_CredentialManagementService
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_PublicKeyManagementService.CIM_LocalCredentialManagementService  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_PublicKeyManagementService.CIM_LocalCredentialManagementService
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_PublicKeyCertificate.CIM_Credential  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_PublicKeyCertificate.CIM_Credential
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_CAHasPublicCertificate.CIM_Dependency  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_CAHasPublicCertificate.CIM_Dependency
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_CASignsPublicKeyCertificate.CIM_ManagedCredential  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_CASignsPublicKeyCertificate.CIM_ManagedCredential
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_UnsignedPublicKey.CIM_Credential  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_UnsignedPublicKey.CIM_Credential
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_LocallyManagedPublicKey.CIM_ManagedCredential  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_LocallyManagedPublicKey.CIM_ManagedCredential
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_SharedSecretService.CIM_LocalCredentialManagementService  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_SharedSecretService.CIM_LocalCredentialManagementService
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_SharedSecretIsShared.CIM_ManagedCredential  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_SharedSecretIsShared.CIM_ManagedCredential
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_NamedSharedIKESecret.CIM_Credential  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_NamedSharedIKESecret.CIM_Credential
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_IKESecretIsNamed.CIM_ManagedCredential  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_IKESecretIsNamed.CIM_ManagedCredential
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_UsersAccess.CIM_UserEntity  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_UsersAccess.CIM_UserEntity
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_ElementAsUser.CIM_Dependency  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_ElementAsUser.CIM_Dependency
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_UsersCredential.CIM_Dependency  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_UsersCredential.CIM_Dependency
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_Notary.CIM_CredentialManagementService  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_Notary.CIM_CredentialManagementService
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_NotaryVerifiesBiometric.CIM_Dependency  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_NotaryVerifiesBiometric.CIM_Dependency
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_PublicPrivateKeyPair.CIM_UsersCredential  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_PublicPrivateKeyPair.CIM_UsersCredential
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_Privilege.CIM_ManagedElement  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_Privilege.CIM_ManagedElement
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_AuthorizedPrivilege.CIM_Privilege  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_AuthorizedPrivilege.CIM_Privilege
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_AuthorizedSubject.#  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_AuthorizedSubject.#
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_AuthorizedTarget.#  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_AuthorizedTarget.#
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_Account.CIM_LogicalElement  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_Account.CIM_LogicalElement
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_AccountIdentity.CIM_LogicalIdentity  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_AccountIdentity.CIM_LogicalIdentity
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_AccountOnSystem.CIM_SystemComponent  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_AccountOnSystem.CIM_SystemComponent
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_UsersAccount.CIM_Dependency  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_UsersAccount.CIM_Dependency
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_AccountMapsToAccount.CIM_Dependency  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_AccountMapsToAccount.CIM_Dependency
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_SecurityServiceUsesAccount.CIM_Dependency  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_SecurityServiceUsesAccount.CIM_Dependency
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_AccountManagementService.CIM_SecurityService  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_AccountManagementService.CIM_SecurityService
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_ManagesAccount.CIM_Dependency  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_ManagesAccount.CIM_Dependency
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_ManagesAccountOnSystem.CIM_SecurityServiceForSystem  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_ManagesAccountOnSystem.CIM_SecurityServiceForSystem
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_AccessControlInformation.CIM_LogicalElement  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_AccessControlInformation.CIM_LogicalElement
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_HostedACI.CIM_HostedDependency  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_HostedACI.CIM_HostedDependency
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_AuthorizedUse.CIM_Dependency  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_AuthorizedUse.CIM_Dependency
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_AuthorizationSubject.CIM_Dependency  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_AuthorizationSubject.CIM_Dependency
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_AuthorizationTarget.CIM_Dependency  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_AuthorizationTarget.CIM_Dependency
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_PrivilegeManagementService.CIM_AuthorizationService  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_PrivilegeManagementService.CIM_AuthorizationService
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_StorageClientSettingData.CIM_SettingData  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_StorageClientSettingData.CIM_SettingData
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_StorageHardwareID.CIM_Identity  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_StorageHardwareID.CIM_Identity
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_GatewayPathID.CIM_StorageHardwareID  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_GatewayPathID.CIM_StorageHardwareID
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_StorageHardwareIDManagementService.CIM_AuthenticationService  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_StorageHardwareIDManagementService.CIM_AuthenticationService
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_ApplicationSystem.CIM_System  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_ApplicationSystem.CIM_System
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_ApplicationSystemDirectory.#  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_ApplicationSystemDirectory.#
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_SoftwareElement.CIM_LogicalElement  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_SoftwareElement.CIM_LogicalElement
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_SoftwareFeature.CIM_LogicalElement  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_SoftwareFeature.CIM_LogicalElement
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_SoftwareFeatureSoftwareElements.CIM_Component  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_SoftwareFeatureSoftwareElements.CIM_Component
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_SoftwareElementComponent.CIM_Component  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_SoftwareElementComponent.CIM_Component
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_SoftwareFeatureComponent.CIM_Component  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_SoftwareFeatureComponent.CIM_Component
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_ProductSoftwareFeatures.#  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_ProductSoftwareFeatures.#
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_ApplicationSystemSoftwareFeature.CIM_SystemComponent  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_ApplicationSystemSoftwareFeature.CIM_SystemComponent
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_SoftwareFeatureServiceImplementation.CIM_Dependency  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_SoftwareFeatureServiceImplementation.CIM_Dependency
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_SoftwareFeatureSAPImplementation.CIM_Dependency  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_SoftwareFeatureSAPImplementation.CIM_Dependency
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_FRUIncludesSoftwareFeature.#  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_FRUIncludesSoftwareFeature.#
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_SoftwareElementServiceImplementation.CIM_Dependency  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_SoftwareElementServiceImplementation.CIM_Dependency
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_SoftwareElementSAPImplementation.CIM_Dependency  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_SoftwareElementSAPImplementation.CIM_Dependency
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_InstalledProduct.CIM_Collection  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_InstalledProduct.CIM_Collection
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_InstalledProductImage.#  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_InstalledProductImage.#
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_CollectedSoftwareElements.CIM_MemberOfCollection  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_CollectedSoftwareElements.CIM_MemberOfCollection
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_CollectedSoftwareFeatures.CIM_MemberOfCollection  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_CollectedSoftwareFeatures.CIM_MemberOfCollection
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_Check.CIM_ManagedElement  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_Check.CIM_ManagedElement
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_DirectorySpecification.CIM_Check  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_DirectorySpecification.CIM_Check
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_ArchitectureCheck.CIM_Check  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_ArchitectureCheck.CIM_Check
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_MemoryCheck.CIM_Check  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_MemoryCheck.CIM_Check
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_DiskSpaceCheck.CIM_Check  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_DiskSpaceCheck.CIM_Check
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_SwapSpaceCheck.CIM_Check  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_SwapSpaceCheck.CIM_Check
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_OSVersionCheck.CIM_Check  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_OSVersionCheck.CIM_Check
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_SoftwareElementVersionCheck.CIM_Check  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_SoftwareElementVersionCheck.CIM_Check
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_FileSpecification.CIM_Check  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_FileSpecification.CIM_Check
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_VersionCompatibilityCheck.CIM_Check  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_VersionCompatibilityCheck.CIM_Check
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_SettingCheck.CIM_Check  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_SettingCheck.CIM_Check
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_SoftwareElementChecks.#  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_SoftwareElementChecks.#
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_DirectorySpecificationFile.#  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_DirectorySpecificationFile.#
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_Action.CIM_ManagedElement  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_Action.CIM_ManagedElement
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_DirectoryAction.CIM_Action  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_DirectoryAction.CIM_Action
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_CreateDirectoryAction.CIM_DirectoryAction  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_CreateDirectoryAction.CIM_DirectoryAction
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_RemoveDirectoryAction.CIM_DirectoryAction  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_RemoveDirectoryAction.CIM_DirectoryAction
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_FileAction.CIM_Action  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_FileAction.CIM_Action
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_CopyFileAction.CIM_FileAction  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_CopyFileAction.CIM_FileAction
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_RemoveFileAction.CIM_FileAction  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_RemoveFileAction.CIM_FileAction
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_RebootAction.CIM_Action  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_RebootAction.CIM_Action
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_ExecuteProgram.CIM_Action  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_ExecuteProgram.CIM_Action
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_ModifySettingAction.CIM_Action  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_ModifySettingAction.CIM_Action
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_ActionSequence.#  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_ActionSequence.#
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_SoftwareElementActions.#  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_SoftwareElementActions.#
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_ToDirectorySpecification.#  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_ToDirectorySpecification.#
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_FromDirectorySpecification.#  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_FromDirectorySpecification.#
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_ToDirectoryAction.#  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_ToDirectoryAction.#
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_FromDirectoryAction.#  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_FromDirectoryAction.#
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_InstalledSoftwareElement.#  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_InstalledSoftwareElement.#
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_OperatingSystemSoftwareFeature.CIM_Component  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_OperatingSystemSoftwareFeature.CIM_Component
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_DiagnosticTestSoftware.CIM_Dependency  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_DiagnosticTestSoftware.CIM_Dependency
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_BIOSFeature.CIM_SoftwareFeature  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_BIOSFeature.CIM_SoftwareFeature
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_BIOSElement.CIM_SoftwareElement  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_BIOSElement.CIM_SoftwareElement
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_BIOSFeatureBIOSElements.CIM_SoftwareFeatureSoftwareElements  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_BIOSFeatureBIOSElements.CIM_SoftwareFeatureSoftwareElements
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_SystemBIOS.CIM_SystemComponent  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_SystemBIOS.CIM_SystemComponent
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_VideoBIOSFeature.CIM_SoftwareFeature  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_VideoBIOSFeature.CIM_SoftwareFeature
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_VideoBIOSElement.CIM_SoftwareElement  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_VideoBIOSElement.CIM_SoftwareElement
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_VideoBIOSFeatureVideoBIOSElements.CIM_SoftwareFeatureSoftwareElements  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_VideoBIOSFeatureVideoBIOSElements.CIM_SoftwareFeatureSoftwareElements
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_AGPSoftwareFeature.CIM_SoftwareFeature  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_AGPSoftwareFeature.CIM_SoftwareFeature
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_ApplicationSystemDependency.CIM_Dependency  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_ApplicationSystemDependency.CIM_Dependency
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_ApplicationSystemHierarchy.CIM_Component  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_ApplicationSystemHierarchy.CIM_Component
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_StatisticalRuntimeOverview.CIM_StatisticalData  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_StatisticalRuntimeOverview.CIM_StatisticalData
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_AssociatedAppSystemOverviewStatistics.CIM_ElementStatisticalData  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_AssociatedAppSystemOverviewStatistics.CIM_ElementStatisticalData
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_J2eeManagedObjectCapabilities.CIM_Capabilities  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_J2eeManagedObjectCapabilities.CIM_Capabilities
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_J2eeManagedObject.CIM_EnabledLogicalElement  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_J2eeManagedObject.CIM_EnabledLogicalElement
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_J2eeDomain.CIM_AdminDomain  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_J2eeDomain.CIM_AdminDomain
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_J2eeServer.CIM_ApplicationSystem  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_J2eeServer.CIM_ApplicationSystem
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_J2eeServerInDomain.CIM_SystemComponent  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_J2eeServerInDomain.CIM_SystemComponent
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_J2eeJVM.CIM_EnabledLogicalElement  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_J2eeJVM.CIM_EnabledLogicalElement
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_J2eeServerUsesJVM.CIM_Dependency  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_J2eeServerUsesJVM.CIM_Dependency
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_J2eeDeployedObject.CIM_Service  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_J2eeDeployedObject.CIM_Service
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_J2eeModule.CIM_J2eeDeployedObject  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_J2eeModule.CIM_J2eeDeployedObject
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_J2eeApplication.CIM_ApplicationSystem  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_J2eeApplication.CIM_ApplicationSystem
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_J2eeApplicationModule.CIM_SystemComponent  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_J2eeApplicationModule.CIM_SystemComponent
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_J2eeApplicationHostedOnServer.CIM_HostedDependency  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_J2eeApplicationHostedOnServer.CIM_HostedDependency
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_J2eeModuleUsesJVM.CIM_HostedDependency  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_J2eeModuleUsesJVM.CIM_HostedDependency
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_J2eeAppClientModule.CIM_J2eeModule  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_J2eeAppClientModule.CIM_J2eeModule
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_J2eeEJBModule.CIM_J2eeModule  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_J2eeEJBModule.CIM_J2eeModule
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_J2eeEJB.CIM_Service  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_J2eeEJB.CIM_Service
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_J2eeMessageDrivenBean.CIM_J2eeEJB  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_J2eeMessageDrivenBean.CIM_J2eeEJB
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_J2eeEntityBean.CIM_J2eeEJB  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_J2eeEntityBean.CIM_J2eeEJB
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_J2eeSessionBean.CIM_J2eeEJB  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_J2eeSessionBean.CIM_J2eeEJB
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_J2eeStatefulSessionBean.CIM_J2eeSessionBean  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_J2eeStatefulSessionBean.CIM_J2eeSessionBean
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_J2eeStatelessSessionBean.CIM_J2eeSessionBean  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_J2eeStatelessSessionBean.CIM_J2eeSessionBean
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_J2eeEJBInModule.CIM_ServiceComponent  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_J2eeEJBInModule.CIM_ServiceComponent
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_J2eeWebModule.CIM_J2eeModule  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_J2eeWebModule.CIM_J2eeModule
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_J2eeServlet.CIM_Service  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_J2eeServlet.CIM_Service
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_J2eeServletInModule.CIM_ServiceComponent  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_J2eeServletInModule.CIM_ServiceComponent
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_J2eeResourceAdapterModule.CIM_J2eeModule  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_J2eeResourceAdapterModule.CIM_J2eeModule
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_J2eeResourceAdapter.CIM_Service  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_J2eeResourceAdapter.CIM_Service
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_J2eeResourceAdapterInModule.CIM_ServiceComponent  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_J2eeResourceAdapterInModule.CIM_ServiceComponent
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_J2eeResource.CIM_SystemResource  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_J2eeResource.CIM_SystemResource
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_J2eeResourceOnServer.CIM_ResourceOfSystem  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_J2eeResourceOnServer.CIM_ResourceOfSystem
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_J2eeJavaMailResource.CIM_J2eeResource  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_J2eeJavaMailResource.CIM_J2eeResource
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_J2eeJCAResource.CIM_J2eeResource  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_J2eeJCAResource.CIM_J2eeResource
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_J2eeJCAConnectionFactory.CIM_EnabledLogicalElement  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_J2eeJCAConnectionFactory.CIM_EnabledLogicalElement
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_J2eeJDBCResource.CIM_J2eeResource  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_J2eeJDBCResource.CIM_J2eeResource
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_J2eeJMSResource.CIM_J2eeResource  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_J2eeJMSResource.CIM_J2eeResource
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_J2eeJNDIResource.CIM_J2eeResource  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_J2eeJNDIResource.CIM_J2eeResource
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_J2eeJTAResource.CIM_J2eeResource  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_J2eeJTAResource.CIM_J2eeResource
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_J2eeRMI_IIOPResource.CIM_J2eeResource  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_J2eeRMI_IIOPResource.CIM_J2eeResource
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_J2eeURLResource.CIM_J2eeResource  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_J2eeURLResource.CIM_J2eeResource
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_J2eeJCAManagedConnectionFactory.CIM_EnabledLogicalElement  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_J2eeJCAManagedConnectionFactory.CIM_EnabledLogicalElement
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_J2eeJDBCDataSource.CIM_EnabledLogicalElement  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_J2eeJDBCDataSource.CIM_EnabledLogicalElement
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_J2eeJDBCDriver.CIM_EnabledLogicalElement  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_J2eeJDBCDriver.CIM_EnabledLogicalElement
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_J2eeJDBCResourceUsesDataSource.CIM_HostedDependency  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_J2eeJDBCResourceUsesDataSource.CIM_HostedDependency
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_J2eeJDBCDataSourceDriver.CIM_HostedDependency  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_J2eeJDBCDataSourceDriver.CIM_HostedDependency
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_J2eeConnectionFactoryAvailableToJCAResource.CIM_HostedDependency  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_J2eeConnectionFactoryAvailableToJCAResource.CIM_HostedDependency
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_J2eeJCAConnectionFactoryManagedConnectionFactory.CIM_HostedDependency  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_J2eeJCAConnectionFactoryManagedConnectionFactory.CIM_HostedDependency
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_J2eeNotification.CIM_ProcessIndication  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_J2eeNotification.CIM_ProcessIndication
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_J2eeStatistic.CIM_StatisticalData  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_J2eeStatistic.CIM_StatisticalData
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_J2eeEJBStats.CIM_J2eeStatistic  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_J2eeEJBStats.CIM_J2eeStatistic
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_J2eeEntityBeanStats.CIM_J2eeEJBStats  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_J2eeEntityBeanStats.CIM_J2eeEJBStats
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_J2eeMessageDrivenBeanStats.CIM_J2eeEJBStats  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_J2eeMessageDrivenBeanStats.CIM_J2eeEJBStats
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_J2eeSessionBeanStats.CIM_J2eeEJBStats  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_J2eeSessionBeanStats.CIM_J2eeEJBStats
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_J2eeStatefulSessionBeanStats.CIM_J2eeSessionBeanStats  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_J2eeStatefulSessionBeanStats.CIM_J2eeSessionBeanStats
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_J2eeStatelessSessionBeanStats.CIM_J2eeSessionBeanStats  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_J2eeStatelessSessionBeanStats.CIM_J2eeSessionBeanStats
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_J2eeJavaMailStats.CIM_J2eeStatistic  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_J2eeJavaMailStats.CIM_J2eeStatistic
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_J2eeJCAStats.CIM_J2eeStatistic  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_J2eeJCAStats.CIM_J2eeStatistic
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_J2eeConnectionStats.CIM_J2eeStatistic  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_J2eeConnectionStats.CIM_J2eeStatistic
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_J2eeConnectionPoolStats.CIM_J2eeConnectionStats  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_J2eeConnectionPoolStats.CIM_J2eeConnectionStats
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_J2eeJCANonpooledConnections.CIM_RelatedStatisticalData  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_J2eeJCANonpooledConnections.CIM_RelatedStatisticalData
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_J2eeJCAConnectionPools.CIM_RelatedStatisticalData  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_J2eeJCAConnectionPools.CIM_RelatedStatisticalData
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_J2eeJDBCStats.CIM_J2eeStatistic  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_J2eeJDBCStats.CIM_J2eeStatistic
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_J2eeJDBCNonpooledConnections.CIM_RelatedStatisticalData  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_J2eeJDBCNonpooledConnections.CIM_RelatedStatisticalData
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_J2eeJDBCConnectionPools.CIM_RelatedStatisticalData  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_J2eeJDBCConnectionPools.CIM_RelatedStatisticalData
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_J2eeJMSStats.CIM_J2eeStatistic  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_J2eeJMSStats.CIM_J2eeStatistic
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_J2eeJMSConnectionStats.CIM_J2eeStatistic  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_J2eeJMSConnectionStats.CIM_J2eeStatistic
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_J2eeJMSStatConnections.CIM_RelatedStatisticalData  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_J2eeJMSStatConnections.CIM_RelatedStatisticalData
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_J2eeJMSSessionStats.CIM_J2eeStatistic  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_J2eeJMSSessionStats.CIM_J2eeStatistic
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_J2eeJMSEndpointStats.CIM_J2eeStatistic  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_J2eeJMSEndpointStats.CIM_J2eeStatistic
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_J2eeJMSProducerStats.CIM_J2eeJMSEndpointStats  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_J2eeJMSProducerStats.CIM_J2eeJMSEndpointStats
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_J2eeJMSConsumerStats.CIM_J2eeJMSEndpointStats  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_J2eeJMSConsumerStats.CIM_J2eeJMSEndpointStats
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_J2eeJMSConnectionSessions.CIM_RelatedStatisticalData  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_J2eeJMSConnectionSessions.CIM_RelatedStatisticalData
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_J2eeJMSSessionProducers.CIM_RelatedStatisticalData  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_J2eeJMSSessionProducers.CIM_RelatedStatisticalData
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_J2eeJMSSessionConsumers.CIM_RelatedStatisticalData  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_J2eeJMSSessionConsumers.CIM_RelatedStatisticalData
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_J2eeJTAStats.CIM_J2eeStatistic  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_J2eeJTAStats.CIM_J2eeStatistic
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_J2eeJVMStats.CIM_J2eeStatistic  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_J2eeJVMStats.CIM_J2eeStatistic
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_J2eeServletStats.CIM_J2eeStatistic  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_J2eeServletStats.CIM_J2eeStatistic
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_J2eeURLStats.CIM_J2eeStatistic  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_J2eeURLStats.CIM_J2eeStatistic
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_BaseMetricDefinition.CIM_ManagedElement  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_BaseMetricDefinition.CIM_ManagedElement
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_BaseMetricValue.CIM_ManagedElement  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_BaseMetricValue.CIM_ManagedElement
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_MetricInstance.CIM_Dependency  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_MetricInstance.CIM_Dependency
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_MetricDefForME.CIM_Dependency  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_MetricDefForME.CIM_Dependency
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_MetricForME.CIM_Dependency  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_MetricForME.CIM_Dependency
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_UnitOfWorkDefinition.CIM_LogicalElement  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_UnitOfWorkDefinition.CIM_LogicalElement
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_MetricDefinition.CIM_BaseMetricDefinition  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_MetricDefinition.CIM_BaseMetricDefinition
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_TraceLevelType.CIM_ManagedElement  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_TraceLevelType.CIM_ManagedElement
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_UoWMetricDefinition.CIM_Dependency  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_UoWMetricDefinition.CIM_Dependency
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_UnitOfWork.CIM_ManagedElement  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_UnitOfWork.CIM_ManagedElement
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_UoWMetric.CIM_Dependency  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_UoWMetric.CIM_Dependency
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_StartedUoW.CIM_Dependency  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_StartedUoW.CIM_Dependency
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_LogicalElementUnitOfWorkDef.CIM_Dependency  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_LogicalElementUnitOfWorkDef.CIM_Dependency
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_SubUoWDef.CIM_Dependency  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_SubUoWDef.CIM_Dependency
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_LogicalElementPerformsUoW.CIM_Dependency  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_LogicalElementPerformsUoW.CIM_Dependency
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_SubUoW.CIM_Dependency  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_SubUoW.CIM_Dependency
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_UoWDefTraceLevelType.#  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_UoWDefTraceLevelType.#
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_Network.CIM_AdminDomain  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_Network.CIM_AdminDomain
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_AutonomousSystem.CIM_AdminDomain  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_AutonomousSystem.CIM_AdminDomain
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_RoutersInAS.CIM_SystemComponent  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_RoutersInAS.CIM_SystemComponent
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_LogicalNetwork.CIM_CollectionOfMSEs  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_LogicalNetwork.CIM_CollectionOfMSEs
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_NetworksInAdminDomain.#  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_NetworksInAdminDomain.#
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_IPSubnet.CIM_LogicalNetwork  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_IPSubnet.CIM_LogicalNetwork
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_LANSegment.CIM_LogicalNetwork  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_LANSegment.CIM_LogicalNetwork
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_IPXNetwork.CIM_LogicalNetwork  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_IPXNetwork.CIM_LogicalNetwork
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_InLogicalNetwork.CIM_CollectedMSEs  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_InLogicalNetwork.CIM_CollectedMSEs
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_RangeOfIPAddresses.CIM_SystemSpecificCollection  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_RangeOfIPAddresses.CIM_SystemSpecificCollection
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_IPAddressRange.CIM_CollectionOfMSEs  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_IPAddressRange.CIM_CollectionOfMSEs
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_NamedAddressCollection.CIM_SystemSpecificCollection  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_NamedAddressCollection.CIM_SystemSpecificCollection
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_ConnectivityCollection.CIM_SystemSpecificCollection  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_ConnectivityCollection.CIM_SystemSpecificCollection
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_IPConnectivitySubnet.CIM_ConnectivityCollection  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_IPConnectivitySubnet.CIM_ConnectivityCollection
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_LANConnectivitySegment.CIM_ConnectivityCollection  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_LANConnectivitySegment.CIM_ConnectivityCollection
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_IPXConnectivityNetwork.CIM_ConnectivityCollection  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_IPXConnectivityNetwork.CIM_ConnectivityCollection
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_ConnectivityMembershipSettingData.CIM_SettingData  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_ConnectivityMembershipSettingData.CIM_SettingData
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_LANEndpoint.CIM_ProtocolEndpoint  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_LANEndpoint.CIM_ProtocolEndpoint
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_InSegment.CIM_InLogicalNetwork  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_InSegment.CIM_InLogicalNetwork
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_BindsToLANEndpoint.CIM_BindsTo  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_BindsToLANEndpoint.CIM_BindsTo
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_IPProtocolEndpoint.CIM_ProtocolEndpoint  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_IPProtocolEndpoint.CIM_ProtocolEndpoint
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_BGPProtocolEndpoint.CIM_ProtocolEndpoint  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_BGPProtocolEndpoint.CIM_ProtocolEndpoint
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_ASBGPEndpoints.CIM_SystemComponent  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_ASBGPEndpoints.CIM_SystemComponent
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_IPXProtocolEndpoint.CIM_ProtocolEndpoint  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_IPXProtocolEndpoint.CIM_ProtocolEndpoint
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_TCPProtocolEndpoint.CIM_ProtocolEndpoint  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_TCPProtocolEndpoint.CIM_ProtocolEndpoint
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_UDPProtocolEndpoint.CIM_ProtocolEndpoint  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_UDPProtocolEndpoint.CIM_ProtocolEndpoint
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_OSPFProtocolEndpointBase.CIM_ProtocolEndpoint  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_OSPFProtocolEndpointBase.CIM_ProtocolEndpoint
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_OSPFProtocolEndpoint.CIM_OSPFProtocolEndpointBase  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_OSPFProtocolEndpoint.CIM_OSPFProtocolEndpointBase
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_OSPFVirtualInterface.CIM_OSPFProtocolEndpointBase  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_OSPFVirtualInterface.CIM_OSPFProtocolEndpointBase
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_EndpointIdentity.CIM_LogicalIdentity  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_EndpointIdentity.CIM_LogicalIdentity
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_SwitchPort.CIM_ProtocolEndpoint  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_SwitchPort.CIM_ProtocolEndpoint
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_FilterEntryBase.CIM_LogicalElement  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_FilterEntryBase.CIM_LogicalElement
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_FilterEntry.CIM_FilterEntryBase  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_FilterEntry.CIM_FilterEntryBase
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_IPHeadersFilter.CIM_FilterEntryBase  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_IPHeadersFilter.CIM_FilterEntryBase
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_Hdr8021Filter.CIM_FilterEntryBase  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_Hdr8021Filter.CIM_FilterEntryBase
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_PreambleFilter.CIM_FilterEntryBase  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_PreambleFilter.CIM_FilterEntryBase
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_FilterList.CIM_LogicalElement  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_FilterList.CIM_LogicalElement
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_FilterEntryInSystem.CIM_SystemComponent  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_FilterEntryInSystem.CIM_SystemComponent
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_FilterListInSystem.CIM_SystemComponent  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_FilterListInSystem.CIM_SystemComponent
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_HostedFilterEntryBase.CIM_HostedDependency  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_HostedFilterEntryBase.CIM_HostedDependency
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_HostedFilterList.CIM_HostedDependency  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_HostedFilterList.CIM_HostedDependency
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_EntriesInFilterList.CIM_Component  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_EntriesInFilterList.CIM_Component
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_X509CredentialFilterEntry.CIM_FilterEntryBase  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_X509CredentialFilterEntry.CIM_FilterEntryBase
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_IPSOFilterEntry.CIM_FilterEntryBase  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_IPSOFilterEntry.CIM_FilterEntryBase
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_PeerIDPayloadFilterEntry.CIM_FilterEntryBase  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_PeerIDPayloadFilterEntry.CIM_FilterEntryBase
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_NetworkService.CIM_Service  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_NetworkService.CIM_Service
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_NetworkServicesInAdminDomain.CIM_HostedDependency  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_NetworkServicesInAdminDomain.CIM_HostedDependency
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_LogicalNetworkService.#  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_LogicalNetworkService.#
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_ForwardingService.CIM_NetworkService  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_ForwardingService.CIM_NetworkService
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_ForwardsAmong.CIM_ServiceSAPDependency  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_ForwardsAmong.CIM_ServiceSAPDependency
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_HostedForwardingServices.CIM_HostedService  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_HostedForwardingServices.CIM_HostedService
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_RouteCalculationService.CIM_NetworkService  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_RouteCalculationService.CIM_NetworkService
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_CalculatesAmong.CIM_ServiceSAPDependency  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_CalculatesAmong.CIM_ServiceSAPDependency
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_HostedRoutingServices.CIM_HostedService  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_HostedRoutingServices.CIM_HostedService
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_RoutingProtocolDomain.CIM_AdminDomain  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_RoutingProtocolDomain.CIM_AdminDomain
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_RoutingProtocolDomainInAS.CIM_ContainedDomain  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_RoutingProtocolDomainInAS.CIM_ContainedDomain
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_RoutingPolicy.CIM_LogicalElement  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_RoutingPolicy.CIM_LogicalElement
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_HostedRoutingPolicy.CIM_HostedDependency  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_HostedRoutingPolicy.CIM_HostedDependency
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_ListsInRoutingPolicy.CIM_Component  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_ListsInRoutingPolicy.CIM_Component
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_NextHopRoute.CIM_ManagedElement  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_NextHopRoute.CIM_ManagedElement
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_HostedRoute.CIM_HostedDependency  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_HostedRoute.CIM_HostedDependency
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_NextHopIPRoute.CIM_NextHopRoute  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_NextHopIPRoute.CIM_NextHopRoute
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_RouteUsesEndpoint.CIM_Dependency  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_RouteUsesEndpoint.CIM_Dependency
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_AssociatedNextHop.CIM_Dependency  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_AssociatedNextHop.CIM_Dependency
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_NextHopRouting.CIM_LogicalElement  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_NextHopRouting.CIM_LogicalElement
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_IPRoute.CIM_NextHopRouting  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_IPRoute.CIM_NextHopRouting
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_RouteForwardedByService.CIM_Dependency  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_RouteForwardedByService.CIM_Dependency
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_ForwardedRoutes.CIM_RouteForwardedByService  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_ForwardedRoutes.CIM_RouteForwardedByService
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_CalculatedRoutes.CIM_Dependency  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_CalculatedRoutes.CIM_Dependency
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_AdministrativeDistance.CIM_LogicalElement  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_AdministrativeDistance.CIM_LogicalElement
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_HostedAdminDistance.CIM_HostedDependency  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_HostedAdminDistance.CIM_HostedDependency
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_NetworkPipe.CIM_EnabledLogicalElement  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_NetworkPipe.CIM_EnabledLogicalElement
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_HostedNetworkPipe.CIM_HostedDependency  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_HostedNetworkPipe.CIM_HostedDependency
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_EndpointOfNetworkPipe.CIM_Dependency  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_EndpointOfNetworkPipe.CIM_Dependency
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_NetworkPipeComposition.CIM_Component  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_NetworkPipeComposition.CIM_Component
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_BufferPool.CIM_CollectionOfMSEs  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_BufferPool.CIM_CollectionOfMSEs
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_CollectedBufferPool.CIM_MemberOfCollection  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_CollectedBufferPool.CIM_MemberOfCollection
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_SNMPService.CIM_Service  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_SNMPService.CIM_Service
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_SNMPCommunityString.CIM_ServiceAccessPoint  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_SNMPCommunityString.CIM_ServiceAccessPoint
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_SNMPTrapTarget.CIM_RemotePort  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_SNMPTrapTarget.CIM_RemotePort
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_TrapSourceForSNMPService.CIM_ServiceSAPDependency  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_TrapSourceForSNMPService.CIM_ServiceSAPDependency
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_OSPFArea.CIM_RoutingProtocolDomain  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_OSPFArea.CIM_RoutingProtocolDomain
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_OSPFService.CIM_RouteCalculationService  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_OSPFService.CIM_RouteCalculationService
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_OSPFServiceCapabilities.CIM_Capabilities  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_OSPFServiceCapabilities.CIM_Capabilities
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_OSPFAreaConfiguration.CIM_LogicalElement  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_OSPFAreaConfiguration.CIM_LogicalElement
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_OSPFLink.CIM_ConnectivityCollection  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_OSPFLink.CIM_ConnectivityCollection
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_AreaOfConfiguration.CIM_Dependency  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_AreaOfConfiguration.CIM_Dependency
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_OSPFServiceConfiguration.CIM_Dependency  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_OSPFServiceConfiguration.CIM_Dependency
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_RangesOfConfiguration.CIM_Dependency  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_RangesOfConfiguration.CIM_Dependency
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_EndpointInLink.CIM_MemberOfCollection  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_EndpointInLink.CIM_MemberOfCollection
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_EndpointInArea.CIM_Component  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_EndpointInArea.CIM_Component
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_BGPService.CIM_RouteCalculationService  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_BGPService.CIM_RouteCalculationService
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_BGPAdminDistance.CIM_Dependency  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_BGPAdminDistance.CIM_Dependency
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_BGPCluster.CIM_CollectionOfMSEs  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_BGPCluster.CIM_CollectionOfMSEs
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_RoutersInBGPCluster.CIM_CollectedMSEs  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_RoutersInBGPCluster.CIM_CollectedMSEs
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_BGPClustersInAS.#  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_BGPClustersInAS.#
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_Confederation.CIM_SystemComponent  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_Confederation.CIM_SystemComponent
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_BGPIPRoute.CIM_IPRoute  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_BGPIPRoute.CIM_IPRoute
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_RoutesBGP.CIM_Dependency  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_RoutesBGP.CIM_Dependency
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_EGPRouteCalcDependency.CIM_ServiceServiceDependency  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_EGPRouteCalcDependency.CIM_ServiceServiceDependency
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_BGPPeerGroup.CIM_CollectionOfMSEs  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_BGPPeerGroup.CIM_CollectionOfMSEs
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_HostedBGPPeerGroup.CIM_HostedDependency  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_HostedBGPPeerGroup.CIM_HostedDependency
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_InBGPPeerGroup.CIM_CollectedMSEs  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_InBGPPeerGroup.CIM_CollectedMSEs
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_BGPPeerGroupService.CIM_Dependency  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_BGPPeerGroupService.CIM_Dependency
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_ReflectorService.CIM_Dependency  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_ReflectorService.CIM_Dependency
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_ReflectorClientService.CIM_Dependency  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_ReflectorClientService.CIM_Dependency
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_ReflectorNonClientService.CIM_Dependency  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_ReflectorNonClientService.CIM_Dependency
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_BGPRoutingPolicy.CIM_Dependency  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_BGPRoutingPolicy.CIM_Dependency
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_BGPAttributes.CIM_LogicalElement  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_BGPAttributes.CIM_LogicalElement
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_HostedBGPAttributes.CIM_HostedDependency  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_HostedBGPAttributes.CIM_HostedDependency
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_FilteredBGPAttributes.CIM_Component  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_FilteredBGPAttributes.CIM_Component
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_BGPRouteMap.CIM_LogicalElement  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_BGPRouteMap.CIM_LogicalElement
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_HostedBGPRouteMap.CIM_HostedDependency  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_HostedBGPRouteMap.CIM_HostedDependency
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_BGPRouteMapsInRoutingPolicy.CIM_Component  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_BGPRouteMapsInRoutingPolicy.CIM_Component
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_BGPPeerUsesRouteMap.CIM_CollectedMSEs  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_BGPPeerUsesRouteMap.CIM_CollectedMSEs
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_FilterListsInBGPRouteMap.CIM_Dependency  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_FilterListsInBGPRouteMap.CIM_Dependency
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_BGPPathAttributes.CIM_LogicalElement  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_BGPPathAttributes.CIM_LogicalElement
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_BGPAttributesForRoute.CIM_Dependency  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_BGPAttributesForRoute.CIM_Dependency
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_BGPServiceAttributes.CIM_Component  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_BGPServiceAttributes.CIM_Component
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_BGPEndpointStatistics.CIM_StatisticalData  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_BGPEndpointStatistics.CIM_StatisticalData
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_BGPStatistics.CIM_ServiceStatisticalInformation  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_BGPStatistics.CIM_ServiceStatisticalInformation
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_BGPServiceStatistics.CIM_ServiceStatistics  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_BGPServiceStatistics.CIM_ServiceStatistics
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_SwitchService.CIM_ForwardingService  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_SwitchService.CIM_ForwardingService
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_SwitchesAmong.CIM_ForwardsAmong  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_SwitchesAmong.CIM_ForwardsAmong
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_Switchable.CIM_BindsToLANEndpoint  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_Switchable.CIM_BindsToLANEndpoint
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_TransparentBridgingService.CIM_ForwardingService  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_TransparentBridgingService.CIM_ForwardingService
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_SwitchServiceTransparentBridging.CIM_ServiceComponent  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_SwitchServiceTransparentBridging.CIM_ServiceComponent
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_SpanningTreeService.CIM_ForwardingService  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_SpanningTreeService.CIM_ForwardingService
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_SwitchServiceSpanningTree.CIM_ServiceComponent  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_SwitchServiceSpanningTree.CIM_ServiceComponent
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_SwitchPortSpanningTree.CIM_ServiceSAPDependency  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_SwitchPortSpanningTree.CIM_ServiceSAPDependency
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_DynamicForwardingEntry.CIM_LogicalElement  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_DynamicForwardingEntry.CIM_LogicalElement
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_TransparentBridgingDynamicForwarding.CIM_Dependency  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_TransparentBridgingDynamicForwarding.CIM_Dependency
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_SwitchPortDynamicForwarding.CIM_Dependency  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_SwitchPortDynamicForwarding.CIM_Dependency
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_StaticForwardingEntry.CIM_LogicalElement  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_StaticForwardingEntry.CIM_LogicalElement
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_TransparentBridgingStaticForwarding.CIM_Dependency  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_TransparentBridgingStaticForwarding.CIM_Dependency
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_SwitchPortStaticForwarding.CIM_Dependency  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_SwitchPortStaticForwarding.CIM_Dependency
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_SourceRoutingService.CIM_ForwardingService  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_SourceRoutingService.CIM_ForwardingService
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_SwitchServiceSourceRouting.CIM_ServiceComponent  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_SwitchServiceSourceRouting.CIM_ServiceComponent
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_SwitchPortPair.CIM_SAPSAPDependency  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_SwitchPortPair.CIM_SAPSAPDependency
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_SwitchPortSourceRouting.CIM_ServiceSAPDependency  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_SwitchPortSourceRouting.CIM_ServiceSAPDependency
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_RelatedSpanningTree.CIM_ServiceServiceDependency  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_RelatedSpanningTree.CIM_ServiceServiceDependency
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_TransparentBridgingStatistics.CIM_ServiceStatisticalInformation  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_TransparentBridgingStatistics.CIM_ServiceStatisticalInformation
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_SpanningTreeStatistics.CIM_ServiceStatisticalInformation  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_SpanningTreeStatistics.CIM_ServiceStatisticalInformation
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_SwitchPortStatistics.CIM_SAPStatisticalInformation  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_SwitchPortStatistics.CIM_SAPStatisticalInformation
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_SwitchPortTransparentBridgingStatistics.CIM_SAPStatisticalInformation  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_SwitchPortTransparentBridgingStatistics.CIM_SAPStatisticalInformation
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_SwitchPortSpanningTreeStatistics.CIM_SAPStatisticalInformation  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_SwitchPortSpanningTreeStatistics.CIM_SAPStatisticalInformation
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_SwitchPortSourceRoutingStatistics.CIM_SAPStatisticalInformation  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_SwitchPortSourceRoutingStatistics.CIM_SAPStatisticalInformation
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_QoSService.CIM_Service  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_QoSService.CIM_Service
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_QoSSubService.CIM_ServiceComponent  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_QoSSubService.CIM_ServiceComponent
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_PrecedenceService.CIM_QoSService  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_PrecedenceService.CIM_QoSService
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_DiffServService.CIM_QoSService  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_DiffServService.CIM_QoSService
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_Hdr8021PService.CIM_QoSService  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_Hdr8021PService.CIM_QoSService
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_AFService.CIM_DiffServService  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_AFService.CIM_DiffServService
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_FlowService.CIM_QoSService  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_FlowService.CIM_QoSService
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_EFService.CIM_DiffServService  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_EFService.CIM_DiffServService
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_AFRelatedServices.#  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_AFRelatedServices.#
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_ConditioningService.CIM_Service  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_ConditioningService.CIM_Service
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_QoSConditioningSubService.CIM_ServiceComponent  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_QoSConditioningSubService.CIM_ServiceComponent
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_ConditioningServiceOnEndpoint.CIM_Dependency  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_ConditioningServiceOnEndpoint.CIM_Dependency
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_IngressConditioningServiceOnEndpoint.CIM_ServiceSAPDependency  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_IngressConditioningServiceOnEndpoint.CIM_ServiceSAPDependency
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_EgressConditioningServiceOnEndpoint.CIM_ServiceSAPDependency  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_EgressConditioningServiceOnEndpoint.CIM_ServiceSAPDependency
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_NextService.#  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_NextService.#
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_ClassifierService.CIM_ConditioningService  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_ClassifierService.CIM_ConditioningService
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_ClassifierElement.CIM_ClassifierService  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_ClassifierElement.CIM_ClassifierService
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_ClassifierElementInClassifierService.CIM_ServiceComponent  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_ClassifierElementInClassifierService.CIM_ServiceComponent
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_ClassifierElementUsesFilterList.CIM_Dependency  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_ClassifierElementUsesFilterList.CIM_Dependency
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_NextServiceAfterClassifierElement.CIM_NextService  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_NextServiceAfterClassifierElement.CIM_NextService
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_ClassifierFilterSet.CIM_Dependency  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_ClassifierFilterSet.CIM_Dependency
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_MeterService.CIM_ConditioningService  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_MeterService.CIM_ConditioningService
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_AverageRateMeterService.CIM_MeterService  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_AverageRateMeterService.CIM_MeterService
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_EWMAMeterService.CIM_MeterService  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_EWMAMeterService.CIM_MeterService
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_TokenBucketMeterService.CIM_MeterService  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_TokenBucketMeterService.CIM_MeterService
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_NextServiceAfterMeter.#  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_NextServiceAfterMeter.#
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_MarkerService.CIM_ConditioningService  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_MarkerService.CIM_ConditioningService
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_PreambleMarkerService.CIM_MarkerService  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_PreambleMarkerService.CIM_MarkerService
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_ToSMarkerService.CIM_MarkerService  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_ToSMarkerService.CIM_MarkerService
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_DSCPMarkerService.CIM_MarkerService  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_DSCPMarkerService.CIM_MarkerService
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_Priority8021QMarkerService.CIM_MarkerService  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_Priority8021QMarkerService.CIM_MarkerService
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_DropperService.CIM_ConditioningService  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_DropperService.CIM_ConditioningService
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_REDDropperService.CIM_DropperService  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_REDDropperService.CIM_DropperService
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_WeightedREDDropperService.CIM_DropperService  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_WeightedREDDropperService.CIM_DropperService
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_HeadTailDropper.CIM_DropperService  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_HeadTailDropper.CIM_DropperService
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_QueuingService.CIM_ConditioningService  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_QueuingService.CIM_ConditioningService
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_DropThresholdCalculationService.CIM_Service  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_DropThresholdCalculationService.CIM_Service
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_QueueHierarchy.CIM_ServiceServiceDependency  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_QueueHierarchy.CIM_ServiceServiceDependency
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_CalculationServiceForDropper.CIM_ServiceServiceDependency  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_CalculationServiceForDropper.CIM_ServiceServiceDependency
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_CalculationBasedOnQueue.CIM_ServiceServiceDependency  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_CalculationBasedOnQueue.CIM_ServiceServiceDependency
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_HeadTailDropQueueBinding.CIM_Dependency  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_HeadTailDropQueueBinding.CIM_Dependency
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_QueueAllocation.CIM_Dependency  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_QueueAllocation.CIM_Dependency
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_PacketSchedulingService.CIM_ConditioningService  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_PacketSchedulingService.CIM_ConditioningService
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_NextScheduler.CIM_NextService  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_NextScheduler.CIM_NextService
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_SchedulerUsed.CIM_ServiceServiceDependency  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_SchedulerUsed.CIM_ServiceServiceDependency
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_NonWorkConservingSchedulingService.CIM_PacketSchedulingService  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_NonWorkConservingSchedulingService.CIM_PacketSchedulingService
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_FailNextScheduler.CIM_NextScheduler  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_FailNextScheduler.CIM_NextScheduler
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_SchedulingElement.CIM_ManagedElement  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_SchedulingElement.CIM_ManagedElement
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_AllocationSchedulingElement.CIM_SchedulingElement  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_AllocationSchedulingElement.CIM_SchedulingElement
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_WRRSchedulingElement.CIM_SchedulingElement  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_WRRSchedulingElement.CIM_SchedulingElement
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_PrioritySchedulingElement.CIM_SchedulingElement  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_PrioritySchedulingElement.CIM_SchedulingElement
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_QueueToSchedule.#  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_QueueToSchedule.#
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_BoundedPrioritySchedulingElement.CIM_PrioritySchedulingElement  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_BoundedPrioritySchedulingElement.CIM_PrioritySchedulingElement
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_SchedulingServiceToSchedule.#  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_SchedulingServiceToSchedule.#
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_ElementInSchedulingService.CIM_Component  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_ElementInSchedulingService.CIM_Component
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_SecurityAssociationEndpoint.CIM_ProtocolEndpoint  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_SecurityAssociationEndpoint.CIM_ProtocolEndpoint
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_FilterOfSecurityAssociation.CIM_Dependency  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_FilterOfSecurityAssociation.CIM_Dependency
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_SAEndpointConnectionStatistics.CIM_StatisticalData  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_SAEndpointConnectionStatistics.CIM_StatisticalData
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_SAEndpointRefreshSettings.CIM_SettingData  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_SAEndpointRefreshSettings.CIM_SettingData
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_IPsecSAEndpoint.CIM_SecurityAssociationEndpoint  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_IPsecSAEndpoint.CIM_SecurityAssociationEndpoint
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_PeerOfSAEndpoint.CIM_Dependency  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_PeerOfSAEndpoint.CIM_Dependency
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_IKESAEndpoint.CIM_SecurityAssociationEndpoint  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_IKESAEndpoint.CIM_SecurityAssociationEndpoint
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_Phase1SAUsedForPhase2.CIM_Dependency  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_Phase1SAUsedForPhase2.CIM_Dependency
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_SATransform.CIM_ScopedSettingData  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_SATransform.CIM_ScopedSettingData
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_TransformOfSecurityAssociation.CIM_ElementSettingData  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_TransformOfSecurityAssociation.CIM_ElementSettingData
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_AHTransform.CIM_SATransform  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_AHTransform.CIM_SATransform
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_ESPTransform.CIM_SATransform  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_ESPTransform.CIM_SATransform
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_IPCOMPTransform.CIM_SATransform  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_IPCOMPTransform.CIM_SATransform
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_VLAN.CIM_ServiceAccessPoint  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_VLAN.CIM_ServiceAccessPoint
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_InboundVLAN.CIM_SAPSAPDependency  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_InboundVLAN.CIM_SAPSAPDependency
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_OutboundVLAN.CIM_SAPSAPDependency  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_OutboundVLAN.CIM_SAPSAPDependency
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_VLANService.CIM_Service  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_VLANService.CIM_Service
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_Specific802dot1QVLANService.CIM_VLANService  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_Specific802dot1QVLANService.CIM_VLANService
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_VLANFor.CIM_ServiceSAPDependency  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_VLANFor.CIM_ServiceSAPDependency
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_SwitchServiceVLAN.CIM_ServiceComponent  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_SwitchServiceVLAN.CIM_ServiceComponent
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_RelatedTransparentBridgingService.CIM_ServiceSAPDependency  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_RelatedTransparentBridgingService.CIM_ServiceSAPDependency
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_DatabaseSystem.CIM_ApplicationSystem  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_DatabaseSystem.CIM_ApplicationSystem
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_CommonDatabase.CIM_EnabledLogicalElement  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_CommonDatabase.CIM_EnabledLogicalElement
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_AssociatedDatabaseSystem.CIM_Dependency  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_AssociatedDatabaseSystem.CIM_Dependency
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_DatabaseAdministrator.CIM_Dependency  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_DatabaseAdministrator.CIM_Dependency
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_DatabaseService.CIM_Service  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_DatabaseService.CIM_Service
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_ServiceAvailableToDatabase.CIM_ServiceAvailableToElement  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_ServiceAvailableToDatabase.CIM_ServiceAvailableToElement
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_DatabaseParameter.CIM_ScopedSettingData  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_DatabaseParameter.CIM_ScopedSettingData
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_SNMPDatabaseParameter.CIM_DatabaseParameter  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_SNMPDatabaseParameter.CIM_DatabaseParameter
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_DatabaseSegment.CIM_StorageExtent  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_DatabaseSegment.CIM_StorageExtent
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_DatabaseStorageArea.CIM_FileSystem  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_DatabaseStorageArea.CIM_FileSystem
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_DatabaseFile.CIM_Component  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_DatabaseFile.CIM_Component
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_DatabaseStorage.CIM_Component  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_DatabaseStorage.CIM_Component
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_DatabaseSegmentSettingData.CIM_ScopedSettingData  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_DatabaseSegmentSettingData.CIM_ScopedSettingData
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_DatabaseControlFile.CIM_HostedDependency  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_DatabaseControlFile.CIM_HostedDependency
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_CommonDatabaseSettingData.CIM_SettingData  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_CommonDatabaseSettingData.CIM_SettingData
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_CommonDatabaseCapabilities.CIM_Capabilities  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_CommonDatabaseCapabilities.CIM_Capabilities
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_DatabaseServiceStatistics.CIM_StatisticalData  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_DatabaseServiceStatistics.CIM_StatisticalData
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_CommonDatabaseStatistics.CIM_StatisticalData  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_CommonDatabaseStatistics.CIM_StatisticalData
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_DatabaseResourceStatistics.CIM_StatisticalData  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_DatabaseResourceStatistics.CIM_StatisticalData
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_DeviceConnection.CIM_Dependency  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_DeviceConnection.CIM_Dependency
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_DeviceIdentity.CIM_LogicalIdentity  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_DeviceIdentity.CIM_LogicalIdentity
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_PackageDependency.CIM_Dependency  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_PackageDependency.CIM_Dependency
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_DeviceSoftware.CIM_Dependency  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_DeviceSoftware.CIM_Dependency
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_DeviceErrorCounts.CIM_StatisticalInformation  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_DeviceErrorCounts.CIM_StatisticalInformation
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_ErrorCountersForDevice.CIM_Statistics  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_ErrorCountersForDevice.CIM_Statistics
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_DeviceErrorData.CIM_StatisticalData  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_DeviceErrorData.CIM_StatisticalData
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_CoolingDevice.CIM_LogicalDevice  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_CoolingDevice.CIM_LogicalDevice
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_Refrigeration.CIM_CoolingDevice  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_Refrigeration.CIM_CoolingDevice
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_HeatPipe.CIM_CoolingDevice  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_HeatPipe.CIM_CoolingDevice
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_Fan.CIM_CoolingDevice  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_Fan.CIM_CoolingDevice
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_AssociatedCooling.CIM_Dependency  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_AssociatedCooling.CIM_Dependency
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_Battery.CIM_LogicalDevice  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_Battery.CIM_LogicalDevice
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_AssociatedBattery.CIM_Dependency  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_AssociatedBattery.CIM_Dependency
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_PowerSupply.CIM_LogicalDevice  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_PowerSupply.CIM_LogicalDevice
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_SuppliesPower.CIM_Dependency  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_SuppliesPower.CIM_Dependency
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_UninterruptiblePowerSupply.CIM_PowerSupply  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_UninterruptiblePowerSupply.CIM_PowerSupply
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_PackageCooling.CIM_PackageDependency  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_PackageCooling.CIM_PackageDependency
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_Watchdog.CIM_LogicalDevice  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_Watchdog.CIM_LogicalDevice
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_Processor.CIM_LogicalDevice  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_Processor.CIM_LogicalDevice
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_ComputerSystemProcessor.CIM_SystemDevice  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_ComputerSystemProcessor.CIM_SystemDevice
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_Controller.CIM_LogicalDevice  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_Controller.CIM_LogicalDevice
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_ControlledBy.CIM_DeviceConnection  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_ControlledBy.CIM_DeviceConnection
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_ESCONController.CIM_Controller  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_ESCONController.CIM_Controller
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_IDEController.CIM_Controller  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_IDEController.CIM_Controller
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_InfraredController.CIM_Controller  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_InfraredController.CIM_Controller
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_ManagementController.CIM_Controller  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_ManagementController.CIM_Controller
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_ParallelController.CIM_Controller  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_ParallelController.CIM_Controller
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_PCMCIAController.CIM_Controller  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_PCMCIAController.CIM_Controller
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_SCSIController.CIM_Controller  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_SCSIController.CIM_Controller
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_SCSIInterface.CIM_ControlledBy  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_SCSIInterface.CIM_ControlledBy
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_SerialController.CIM_Controller  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_SerialController.CIM_Controller
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_SerialInterface.CIM_ControlledBy  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_SerialInterface.CIM_ControlledBy
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_SSAController.CIM_Controller  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_SSAController.CIM_Controller
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_USBController.CIM_Controller  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_USBController.CIM_Controller
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_VideoController.CIM_Controller  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_VideoController.CIM_Controller
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_PCVideoController.CIM_VideoController  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_PCVideoController.CIM_VideoController
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_AGPVideoController.CIM_VideoController  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_AGPVideoController.CIM_VideoController
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_VideoControllerResolution.CIM_Setting  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_VideoControllerResolution.CIM_Setting
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_VideoSetting.CIM_ElementSetting  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_VideoSetting.CIM_ElementSetting
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_PCIController.CIM_Controller  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_PCIController.CIM_Controller
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_PCIDevice.CIM_PCIController  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_PCIDevice.CIM_PCIController
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_PCIBridge.CIM_PCIController  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_PCIBridge.CIM_PCIController
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_PortController.CIM_Controller  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_PortController.CIM_Controller
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_LogicalPort.CIM_LogicalDevice  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_LogicalPort.CIM_LogicalDevice
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_PortOnDevice.CIM_HostedDependency  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_PortOnDevice.CIM_HostedDependency
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_LogicalPortGroup.CIM_SystemSpecificCollection  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_LogicalPortGroup.CIM_SystemSpecificCollection
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_NetworkPort.CIM_LogicalPort  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_NetworkPort.CIM_LogicalPort
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_EthernetPort.CIM_NetworkPort  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_EthernetPort.CIM_NetworkPort
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_TokenRingPort.CIM_NetworkPort  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_TokenRingPort.CIM_NetworkPort
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_OOBAlertServiceOnNetworkPort.CIM_DeviceServiceImplementation  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_OOBAlertServiceOnNetworkPort.CIM_DeviceServiceImplementation
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_WakeUpServiceOnNetworkPort.CIM_DeviceServiceImplementation  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_WakeUpServiceOnNetworkPort.CIM_DeviceServiceImplementation
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_PortImplementsEndpoint.CIM_DeviceSAPImplementation  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_PortImplementsEndpoint.CIM_DeviceSAPImplementation
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_NetworkPortStatistics.CIM_StatisticalData  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_NetworkPortStatistics.CIM_StatisticalData
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_EthernetPortStatistics.CIM_NetworkPortStatistics  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_EthernetPortStatistics.CIM_NetworkPortStatistics
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_TokenRingPortStatistics.CIM_NetworkPortStatistics  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_TokenRingPortStatistics.CIM_NetworkPortStatistics
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_LogicalModule.CIM_LogicalDevice  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_LogicalModule.CIM_LogicalDevice
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_ModulePort.CIM_Component  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_ModulePort.CIM_Component
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_PortActiveConnection.CIM_Dependency  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_PortActiveConnection.CIM_Dependency
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_ProtocolController.CIM_LogicalDevice  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_ProtocolController.CIM_LogicalDevice
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_SCSIProtocolController.CIM_ProtocolController  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_SCSIProtocolController.CIM_ProtocolController
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_ProtocolControllerForDevice.CIM_Dependency  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_ProtocolControllerForDevice.CIM_Dependency
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_AssociatedProtocolController.CIM_ProtocolControllerForDevice  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_AssociatedProtocolController.CIM_ProtocolControllerForDevice
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_ProtocolControllerForPort.CIM_ProtocolControllerForDevice  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_ProtocolControllerForPort.CIM_ProtocolControllerForDevice
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_ProtocolControllerForUnit.CIM_ProtocolControllerForDevice  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_ProtocolControllerForUnit.CIM_ProtocolControllerForDevice
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_ProtocolControllerAccessesUnit.CIM_ProtocolControllerForDevice  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_ProtocolControllerAccessesUnit.CIM_ProtocolControllerForDevice
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_ControllerConfigurationService.CIM_Service  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_ControllerConfigurationService.CIM_Service
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_ProtocolControllerMaskingCapabilities.CIM_Capabilities  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_ProtocolControllerMaskingCapabilities.CIM_Capabilities
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_NetworkAdapter.CIM_LogicalDevice  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_NetworkAdapter.CIM_LogicalDevice
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_EthernetAdapter.CIM_NetworkAdapter  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_EthernetAdapter.CIM_NetworkAdapter
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_TokenRingAdapter.CIM_NetworkAdapter  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_TokenRingAdapter.CIM_NetworkAdapter
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_OOBAlertServiceOnNetworkAdapter.CIM_DeviceServiceImplementation  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_OOBAlertServiceOnNetworkAdapter.CIM_DeviceServiceImplementation
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_WakeUpServiceOnNetworkAdapter.CIM_DeviceServiceImplementation  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_WakeUpServiceOnNetworkAdapter.CIM_DeviceServiceImplementation
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_NetworkAdapterRedundancyComponent.CIM_RedundancyComponent  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_NetworkAdapterRedundancyComponent.CIM_RedundancyComponent
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_NetworkVirtualAdapter.CIM_LogicalIdentity  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_NetworkVirtualAdapter.CIM_LogicalIdentity
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_AdapterActiveConnection.CIM_Dependency  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_AdapterActiveConnection.CIM_Dependency
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_FibreChannelAdapter.CIM_NetworkAdapter  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_FibreChannelAdapter.CIM_NetworkAdapter
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_FibrePort.CIM_LogicalPort  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_FibrePort.CIM_LogicalPort
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_FibrePortOnFCAdapter.CIM_PortOnDevice  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_FibrePortOnFCAdapter.CIM_PortOnDevice
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_FibrePortActiveLogin.#  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_FibrePortActiveLogin.#
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_FCAdapterEventCounters.CIM_DeviceStatisticalInformation  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_FCAdapterEventCounters.CIM_DeviceStatisticalInformation
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_FibrePortEventCounters.CIM_DeviceStatisticalInformation  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_FibrePortEventCounters.CIM_DeviceStatisticalInformation
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_FCPort.CIM_NetworkPort  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_FCPort.CIM_NetworkPort
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_ZoneSet.CIM_ConnectivityCollection  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_ZoneSet.CIM_ConnectivityCollection
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_Zone.CIM_ConnectivityCollection  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_Zone.CIM_ConnectivityCollection
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_ZoneMembershipSettingData.CIM_ConnectivityMembershipSettingData  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_ZoneMembershipSettingData.CIM_ConnectivityMembershipSettingData
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_ZoneService.CIM_Service  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_ZoneService.CIM_Service
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_FibreProtocolService.CIM_Service  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_FibreProtocolService.CIM_Service
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_FCPortStatistics.CIM_NetworkPortStatistics  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_FCPortStatistics.CIM_NetworkPortStatistics
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_FCPortRateStatistics.CIM_StatisticalData  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_FCPortRateStatistics.CIM_StatisticalData
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_ZoneCapabilities.CIM_Capabilities  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_ZoneCapabilities.CIM_Capabilities
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_IBSubnetManager.CIM_Service  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_IBSubnetManager.CIM_Service
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_MediaAccessDevice.CIM_LogicalDevice  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_MediaAccessDevice.CIM_LogicalDevice
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_DiskDrive.CIM_MediaAccessDevice  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_DiskDrive.CIM_MediaAccessDevice
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_DisketteDrive.CIM_MediaAccessDevice  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_DisketteDrive.CIM_MediaAccessDevice
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_CDROMDrive.CIM_MediaAccessDevice  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_CDROMDrive.CIM_MediaAccessDevice
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_DVDDrive.CIM_MediaAccessDevice  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_DVDDrive.CIM_MediaAccessDevice
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_WORMDrive.CIM_MediaAccessDevice  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_WORMDrive.CIM_MediaAccessDevice
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_MagnetoOpticalDrive.CIM_MediaAccessDevice  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_MagnetoOpticalDrive.CIM_MediaAccessDevice
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_TapeDrive.CIM_MediaAccessDevice  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_TapeDrive.CIM_MediaAccessDevice
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_MediaAccessStatInfo.CIM_DeviceStatisticalInformation  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_MediaAccessStatInfo.CIM_DeviceStatisticalInformation
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_MediaAccessStatData.CIM_StatisticalData  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_MediaAccessStatData.CIM_StatisticalData
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_MediaPresent.CIM_Dependency  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_MediaPresent.CIM_Dependency
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_RealizesExtent.CIM_Realizes  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_RealizesExtent.CIM_Realizes
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_RealizedOnSide.CIM_RealizesExtent  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_RealizedOnSide.CIM_RealizesExtent
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_StorageVolume.CIM_StorageExtent  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_StorageVolume.CIM_StorageExtent
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_MediaPartition.CIM_StorageExtent  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_MediaPartition.CIM_StorageExtent
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_DiskPartition.CIM_MediaPartition  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_DiskPartition.CIM_MediaPartition
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_DiskPartitionBasedOnVolume.CIM_BasedOn  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_DiskPartitionBasedOnVolume.CIM_BasedOn
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_RealizesDiskPartition.CIM_RealizesExtent  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_RealizesDiskPartition.CIM_RealizesExtent
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_TapePartition.CIM_MediaPartition  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_TapePartition.CIM_MediaPartition
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_TapePartitionOnSurface.CIM_BasedOn  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_TapePartitionOnSurface.CIM_BasedOn
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_RealizesTapePartition.CIM_RealizesExtent  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_RealizesTapePartition.CIM_RealizesExtent
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_LogicalDisk.CIM_StorageExtent  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_LogicalDisk.CIM_StorageExtent
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_LogicalDiskBasedOnExtent.CIM_BasedOn  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_LogicalDiskBasedOnExtent.CIM_BasedOn
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_LogicalDiskBasedOnVolume.CIM_LogicalDiskBasedOnExtent  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_LogicalDiskBasedOnVolume.CIM_LogicalDiskBasedOnExtent
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_LogicalDiskBasedOnPartition.CIM_LogicalDiskBasedOnExtent  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_LogicalDiskBasedOnPartition.CIM_LogicalDiskBasedOnExtent
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_ProtectedExtentBasedOn.CIM_BasedOn  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_ProtectedExtentBasedOn.CIM_BasedOn
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_CompositeExtent.CIM_StorageExtent  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_CompositeExtent.CIM_StorageExtent
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_CompositeExtentBasedOn.CIM_BasedOn  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_CompositeExtentBasedOn.CIM_BasedOn
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_StorageError.CIM_ManagedElement  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_StorageError.CIM_ManagedElement
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_StorageDefect.#  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_StorageDefect.#
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_PhysicalExtent.CIM_StorageExtent  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_PhysicalExtent.CIM_StorageExtent
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_ProtectedSpaceExtent.CIM_StorageExtent  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_ProtectedSpaceExtent.CIM_StorageExtent
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_PSExtentBasedOnPExtent.CIM_BasedOn  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_PSExtentBasedOnPExtent.CIM_BasedOn
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_VolumeSet.CIM_StorageVolume  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_VolumeSet.CIM_StorageVolume
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_VolumeSetBasedOnPSExtent.CIM_BasedOn  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_VolumeSetBasedOnPSExtent.CIM_BasedOn
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_LogicalDiskBasedOnVolumeSet.CIM_LogicalDiskBasedOnVolume  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_LogicalDiskBasedOnVolumeSet.CIM_LogicalDiskBasedOnVolume
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_AggregatePExtent.CIM_StorageExtent  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_AggregatePExtent.CIM_StorageExtent
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_AggregatePSExtent.CIM_StorageExtent  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_AggregatePSExtent.CIM_StorageExtent
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_AggregatePSExtentBasedOnPExtent.CIM_BasedOn  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_AggregatePSExtentBasedOnPExtent.CIM_BasedOn
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_AggregatePSExtentBasedOnAggregatePExtent.CIM_BasedOn  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_AggregatePSExtentBasedOnAggregatePExtent.CIM_BasedOn
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_PExtentRedundancyComponent.CIM_ExtentRedundancyComponent  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_PExtentRedundancyComponent.CIM_ExtentRedundancyComponent
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_AggregateRedundancyComponent.CIM_ExtentRedundancyComponent  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_AggregateRedundancyComponent.CIM_ExtentRedundancyComponent
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_RealizesPExtent.CIM_RealizesExtent  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_RealizesPExtent.CIM_RealizesExtent
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_RealizesAggregatePExtent.CIM_RealizesExtent  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_RealizesAggregatePExtent.CIM_RealizesExtent
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_Snapshot.CIM_StorageExtent  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_Snapshot.CIM_StorageExtent
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_SnapshotOfExtent.CIM_Dependency  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_SnapshotOfExtent.CIM_Dependency
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_StorageSynchronized.CIM_Synchronized  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_StorageSynchronized.CIM_Synchronized
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_StorageSetting.CIM_SettingData  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_StorageSetting.CIM_SettingData
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_StoragePool.CIM_LogicalElement  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_StoragePool.CIM_LogicalElement
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_StorageCapabilities.CIM_Capabilities  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_StorageCapabilities.CIM_Capabilities
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_StorageConfigurationService.CIM_Service  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_StorageConfigurationService.CIM_Service
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_StorageConfigurationCapabilities.CIM_Capabilities  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_StorageConfigurationCapabilities.CIM_Capabilities
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_StorageSettingWithHints.CIM_StorageSetting  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_StorageSettingWithHints.CIM_StorageSetting
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_AllocatedFromStoragePool.CIM_Dependency  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_AllocatedFromStoragePool.CIM_Dependency
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_HostedStoragePool.CIM_SystemComponent  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_HostedStoragePool.CIM_SystemComponent
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_ConfigurationReportingService.CIM_Service  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_ConfigurationReportingService.CIM_Service
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_StorageLibrary.CIM_System  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_StorageLibrary.CIM_System
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_LibraryPackage.CIM_SystemPackaging  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_LibraryPackage.CIM_SystemPackaging
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_MediaTransferDevice.CIM_LogicalDevice  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_MediaTransferDevice.CIM_LogicalDevice
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_DeviceServicesLocation.CIM_Dependency  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_DeviceServicesLocation.CIM_Dependency
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_PickerElement.CIM_MediaTransferDevice  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_PickerElement.CIM_MediaTransferDevice
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_ChangerDevice.CIM_MediaTransferDevice  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_ChangerDevice.CIM_MediaTransferDevice
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_PickerForChanger.CIM_Dependency  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_PickerForChanger.CIM_Dependency
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_LimitedAccessPort.CIM_MediaTransferDevice  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_LimitedAccessPort.CIM_MediaTransferDevice
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_InterLibraryPort.CIM_MediaTransferDevice  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_InterLibraryPort.CIM_MediaTransferDevice
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_LibraryExchange.CIM_Dependency  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_LibraryExchange.CIM_Dependency
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_LabelReader.CIM_LogicalDevice  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_LabelReader.CIM_LogicalDevice
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_AssociatedLabelReader.CIM_Dependency  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_AssociatedLabelReader.CIM_Dependency
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_PickerLabelReader.CIM_AssociatedLabelReader  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_PickerLabelReader.CIM_AssociatedLabelReader
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_AccessLabelReader.CIM_AssociatedLabelReader  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_AccessLabelReader.CIM_AssociatedLabelReader
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_LabelReaderStatInfo.CIM_DeviceStatisticalInformation  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_LabelReaderStatInfo.CIM_DeviceStatisticalInformation
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_PickerStatInfo.CIM_DeviceStatisticalInformation  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_PickerStatInfo.CIM_DeviceStatisticalInformation
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_LabelReaderStatData.CIM_StatisticalData  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_LabelReaderStatData.CIM_StatisticalData
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_PickerStatData.CIM_StatisticalData  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_PickerStatData.CIM_StatisticalData
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_UserDevice.CIM_LogicalDevice  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_UserDevice.CIM_LogicalDevice
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_PointingDevice.CIM_UserDevice  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_PointingDevice.CIM_UserDevice
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_Keyboard.CIM_UserDevice  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_Keyboard.CIM_UserDevice
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_Display.CIM_UserDevice  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_Display.CIM_UserDevice
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_DesktopMonitor.CIM_Display  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_DesktopMonitor.CIM_Display
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_FlatPanel.CIM_Display  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_FlatPanel.CIM_Display
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_Scanner.CIM_LogicalDevice  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_Scanner.CIM_LogicalDevice
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_Door.CIM_LogicalDevice  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_Door.CIM_LogicalDevice
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_DoorAccessToPhysicalElement.CIM_Dependency  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_DoorAccessToPhysicalElement.CIM_Dependency
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_DoorAccessToDevice.CIM_Dependency  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_DoorAccessToDevice.CIM_Dependency
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_MonitorResolution.CIM_Setting  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_MonitorResolution.CIM_Setting
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_MonitorSetting.CIM_ElementSetting  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_MonitorSetting.CIM_ElementSetting
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_Memory.CIM_StorageExtent  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_Memory.CIM_StorageExtent
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_AssociatedMemory.CIM_Dependency  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_AssociatedMemory.CIM_Dependency
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_MemoryError.CIM_StorageError  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_MemoryError.CIM_StorageError
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_ComputerSystemMemory.CIM_SystemDevice  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_ComputerSystemMemory.CIM_SystemDevice
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_AssociatedProcessorMemory.CIM_AssociatedMemory  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_AssociatedProcessorMemory.CIM_AssociatedMemory
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_AssociatedCacheMemory.CIM_AssociatedMemory  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_AssociatedCacheMemory.CIM_AssociatedMemory
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_NonVolatileStorage.CIM_Memory  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_NonVolatileStorage.CIM_Memory
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_BIOSLoadedInNV.CIM_Dependency  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_BIOSLoadedInNV.CIM_Dependency
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_VolatileStorage.CIM_Memory  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_VolatileStorage.CIM_Memory
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_CacheMemory.CIM_Memory  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_CacheMemory.CIM_Memory
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_Modem.CIM_LogicalDevice  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_Modem.CIM_LogicalDevice
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_ConnectionBasedModem.CIM_Modem  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_ConnectionBasedModem.CIM_Modem
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_DSLModem.CIM_ConnectionBasedModem  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_DSLModem.CIM_ConnectionBasedModem
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_ADSLModem.CIM_DSLModem  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_ADSLModem.CIM_DSLModem
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_CableModem.CIM_ConnectionBasedModem  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_CableModem.CIM_ConnectionBasedModem
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_HDSLModem.CIM_DSLModem  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_HDSLModem.CIM_DSLModem
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_SDSLModem.CIM_DSLModem  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_SDSLModem.CIM_DSLModem
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_VDSLModem.CIM_DSLModem  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_VDSLModem.CIM_DSLModem
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_CallBasedModem.CIM_Modem  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_CallBasedModem.CIM_Modem
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_ISDNModem.CIM_CallBasedModem  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_ISDNModem.CIM_CallBasedModem
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_POTSModem.CIM_CallBasedModem  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_POTSModem.CIM_CallBasedModem
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_Unimodem.CIM_POTSModem  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_Unimodem.CIM_POTSModem
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_OOBAlertServiceOnModem.CIM_DeviceServiceImplementation  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_OOBAlertServiceOnModem.CIM_DeviceServiceImplementation
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_WakeUpServiceOnModem.CIM_DeviceServiceImplementation  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_WakeUpServiceOnModem.CIM_DeviceServiceImplementation
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_Printer.CIM_LogicalDevice  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_Printer.CIM_LogicalDevice
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_PrintQueue.CIM_JobDestination  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_PrintQueue.CIM_JobDestination
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_PrintJob.CIM_Job  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_PrintJob.CIM_Job
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_PrintSAP.CIM_ServiceAccessPoint  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_PrintSAP.CIM_ServiceAccessPoint
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_PrintService.CIM_Service  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_PrintService.CIM_Service
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_PrinterServicingQueue.CIM_Dependency  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_PrinterServicingQueue.CIM_Dependency
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_PrinterServicingJob.CIM_Dependency  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_PrinterServicingJob.CIM_Dependency
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_PrintJobFile.CIM_Component  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_PrintJobFile.CIM_Component
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_QueueForPrintService.CIM_Dependency  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_QueueForPrintService.CIM_Dependency
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_QueueForwardsToPrintSAP.CIM_Dependency  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_QueueForwardsToPrintSAP.CIM_Dependency
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_OwningPrintQueue.CIM_JobDestinationJobs  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_OwningPrintQueue.CIM_JobDestinationJobs
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_Sensor.CIM_LogicalDevice  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_Sensor.CIM_LogicalDevice
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_AssociatedSensor.CIM_Dependency  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_AssociatedSensor.CIM_Dependency
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_BinarySensor.CIM_Sensor  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_BinarySensor.CIM_Sensor
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_MultiStateSensor.CIM_Sensor  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_MultiStateSensor.CIM_Sensor
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_CollectionOfSensors.CIM_Component  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_CollectionOfSensors.CIM_Component
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_NumericSensor.CIM_Sensor  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_NumericSensor.CIM_Sensor
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_DiscreteSensor.CIM_Sensor  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_DiscreteSensor.CIM_Sensor
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_TemperatureSensor.CIM_NumericSensor  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_TemperatureSensor.CIM_NumericSensor
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_CurrentSensor.CIM_NumericSensor  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_CurrentSensor.CIM_NumericSensor
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_VoltageSensor.CIM_NumericSensor  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_VoltageSensor.CIM_NumericSensor
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_Tachometer.CIM_NumericSensor  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_Tachometer.CIM_NumericSensor
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_AssociatedSupplyVoltageSensor.CIM_AssociatedSensor  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_AssociatedSupplyVoltageSensor.CIM_AssociatedSensor
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_AssociatedSupplyCurrentSensor.CIM_AssociatedSensor  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_AssociatedSupplyCurrentSensor.CIM_AssociatedSensor
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_PackageTempSensor.CIM_PackageDependency  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_PackageTempSensor.CIM_PackageDependency
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_AlarmDevice.CIM_LogicalDevice  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_AlarmDevice.CIM_LogicalDevice
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_AssociatedAlarm.CIM_Dependency  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_AssociatedAlarm.CIM_Dependency
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_PackageAlarm.CIM_PackageDependency  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_PackageAlarm.CIM_PackageDependency
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_USBDevice.CIM_LogicalDevice  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_USBDevice.CIM_LogicalDevice
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_USBHub.CIM_USBDevice  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_USBHub.CIM_USBDevice
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_USBPort.CIM_LogicalPort  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_USBPort.CIM_LogicalPort
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_USBPortOnHub.CIM_PortOnDevice  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_USBPortOnHub.CIM_PortOnDevice
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_USBControllerHasHub.CIM_ControlledBy  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_USBControllerHasHub.CIM_ControlledBy
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_USBConnection.CIM_DeviceConnection  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_USBConnection.CIM_DeviceConnection
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_DiskGroup.CIM_CollectionOfMSEs  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_DiskGroup.CIM_CollectionOfMSEs
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_DriveInDiskGroup.CIM_CollectedMSEs  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_DriveInDiskGroup.CIM_CollectedMSEs
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_ExtentInDiskGroup.CIM_CollectedMSEs  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_ExtentInDiskGroup.CIM_CollectedMSEs
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_GroupInDiskGroup.CIM_CollectedCollections  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_GroupInDiskGroup.CIM_CollectedCollections
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_Policy.CIM_ManagedElement  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_Policy.CIM_ManagedElement
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_PolicySet.CIM_Policy  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_PolicySet.CIM_Policy
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_PolicyGroup.CIM_PolicySet  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_PolicyGroup.CIM_PolicySet
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_PolicyRule.CIM_PolicySet  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_PolicyRule.CIM_PolicySet
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_AuthenticationRule.CIM_PolicyRule  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_AuthenticationRule.CIM_PolicyRule
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_ReusablePolicyContainer.CIM_AdminDomain  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_ReusablePolicyContainer.CIM_AdminDomain
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_PolicyRepository.CIM_AdminDomain  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_PolicyRepository.CIM_AdminDomain
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_PolicyCondition.CIM_Policy  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_PolicyCondition.CIM_Policy
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_PolicyTimePeriodCondition.CIM_PolicyCondition  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_PolicyTimePeriodCondition.CIM_PolicyCondition
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_CompoundPolicyCondition.CIM_PolicyCondition  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_CompoundPolicyCondition.CIM_PolicyCondition
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_AuthenticationCondition.CIM_PolicyCondition  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_AuthenticationCondition.CIM_PolicyCondition
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_SharedSecretAuthentication.CIM_AuthenticationCondition  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_SharedSecretAuthentication.CIM_AuthenticationCondition
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_AccountAuthentication.CIM_AuthenticationCondition  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_AccountAuthentication.CIM_AuthenticationCondition
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_BiometricAuthentication.CIM_AuthenticationCondition  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_BiometricAuthentication.CIM_AuthenticationCondition
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_NetworkingIDAuthentication.CIM_AuthenticationCondition  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_NetworkingIDAuthentication.CIM_AuthenticationCondition
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_PublicPrivateKeyAuthentication.CIM_AuthenticationCondition  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_PublicPrivateKeyAuthentication.CIM_AuthenticationCondition
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_KerberosAuthentication.CIM_AuthenticationCondition  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_KerberosAuthentication.CIM_AuthenticationCondition
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_DocumentAuthentication.CIM_AuthenticationCondition  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_DocumentAuthentication.CIM_AuthenticationCondition
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_PhysicalCredentialAuthentication.CIM_AuthenticationCondition  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_PhysicalCredentialAuthentication.CIM_AuthenticationCondition
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_VendorPolicyCondition.CIM_PolicyCondition  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_VendorPolicyCondition.CIM_PolicyCondition
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_PacketFilterCondition.CIM_PolicyCondition  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_PacketFilterCondition.CIM_PolicyCondition
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_PolicyAction.CIM_Policy  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_PolicyAction.CIM_Policy
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_VendorPolicyAction.CIM_PolicyAction  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_VendorPolicyAction.CIM_PolicyAction
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_CompoundPolicyAction.CIM_PolicyAction  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_CompoundPolicyAction.CIM_PolicyAction
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_NetworkPacketAction.CIM_PolicyAction  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_NetworkPacketAction.CIM_PolicyAction
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_RejectConnectionAction.CIM_PolicyAction  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_RejectConnectionAction.CIM_PolicyAction
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_PolicyRoleCollection.CIM_SystemSpecificCollection  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_PolicyRoleCollection.CIM_SystemSpecificCollection
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_PolicyComponent.CIM_Component  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_PolicyComponent.CIM_Component
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_PolicyInSystem.CIM_HostedDependency  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_PolicyInSystem.CIM_HostedDependency
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_PolicySetInSystem.CIM_PolicyInSystem  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_PolicySetInSystem.CIM_PolicyInSystem
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_PolicyGroupInSystem.CIM_PolicySetInSystem  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_PolicyGroupInSystem.CIM_PolicySetInSystem
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_PolicyRuleInSystem.CIM_PolicySetInSystem  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_PolicyRuleInSystem.CIM_PolicySetInSystem
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_PolicySetComponent.CIM_PolicyComponent  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_PolicySetComponent.CIM_PolicyComponent
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_PolicyGroupInPolicyGroup.CIM_PolicyComponent  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_PolicyGroupInPolicyGroup.CIM_PolicyComponent
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_PolicyRuleInPolicyGroup.CIM_PolicyComponent  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_PolicyRuleInPolicyGroup.CIM_PolicyComponent
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_PolicySetValidityPeriod.CIM_PolicyComponent  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_PolicySetValidityPeriod.CIM_PolicyComponent
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_PolicyRuleValidityPeriod.CIM_PolicyComponent  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_PolicyRuleValidityPeriod.CIM_PolicyComponent
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_PolicyConditionStructure.CIM_PolicyComponent  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_PolicyConditionStructure.CIM_PolicyComponent
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_PolicyConditionInPolicyRule.CIM_PolicyConditionStructure  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_PolicyConditionInPolicyRule.CIM_PolicyConditionStructure
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_PolicyConditionInPolicyCondition.CIM_PolicyConditionStructure  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_PolicyConditionInPolicyCondition.CIM_PolicyConditionStructure
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_PolicyActionStructure.CIM_PolicyComponent  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_PolicyActionStructure.CIM_PolicyComponent
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_PolicyActionInPolicyRule.CIM_PolicyActionStructure  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_PolicyActionInPolicyRule.CIM_PolicyActionStructure
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_PolicyActionInPolicyAction.CIM_PolicyActionStructure  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_PolicyActionInPolicyAction.CIM_PolicyActionStructure
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_PolicyContainerInPolicyContainer.CIM_SystemComponent  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_PolicyContainerInPolicyContainer.CIM_SystemComponent
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_PolicyRepositoryInPolicyRepository.CIM_SystemComponent  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_PolicyRepositoryInPolicyRepository.CIM_SystemComponent
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_ReusablePolicy.CIM_PolicyInSystem  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_ReusablePolicy.CIM_PolicyInSystem
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_ElementInPolicyRoleCollection.CIM_MemberOfCollection  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_ElementInPolicyRoleCollection.CIM_MemberOfCollection
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_PolicyRoleCollectionInSystem.CIM_HostedCollection  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_PolicyRoleCollectionInSystem.CIM_HostedCollection
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_PolicyConditionInPolicyRepository.CIM_PolicyInSystem  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_PolicyConditionInPolicyRepository.CIM_PolicyInSystem
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_PolicyActionInPolicyRepository.CIM_PolicyInSystem  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_PolicyActionInPolicyRepository.CIM_PolicyInSystem
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_PolicySetInRoleCollection.CIM_MemberOfCollection  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_PolicySetInRoleCollection.CIM_MemberOfCollection
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_PolicySetAppliesToElement.#  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_PolicySetAppliesToElement.#
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_FilterOfPacketCondition.CIM_Dependency  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_FilterOfPacketCondition.CIM_Dependency
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_AcceptCredentialFrom.CIM_Dependency  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_AcceptCredentialFrom.CIM_Dependency
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_SARule.CIM_PolicyRule  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_SARule.CIM_PolicyRule
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_RuleThatGeneratedSA.CIM_Dependency  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_RuleThatGeneratedSA.CIM_Dependency
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_IKERule.CIM_SARule  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_IKERule.CIM_SARule
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_IPsecRule.CIM_SARule  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_IPsecRule.CIM_SARule
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_IPsecPolicyForSystem.CIM_Dependency  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_IPsecPolicyForSystem.CIM_Dependency
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_IPsecPolicyForEndpoint.CIM_Dependency  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_IPsecPolicyForEndpoint.CIM_Dependency
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_PacketConditionInSARule.CIM_PolicyConditionInPolicyRule  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_PacketConditionInSARule.CIM_PolicyConditionInPolicyRule
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_SAAction.CIM_PolicyAction  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_SAAction.CIM_PolicyAction
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_SAStaticAction.CIM_SAAction  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_SAStaticAction.CIM_SAAction
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_PreconfiguredSAAction.CIM_SAStaticAction  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_PreconfiguredSAAction.CIM_SAStaticAction
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_TransformOfPreconfiguredAction.CIM_Dependency  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_TransformOfPreconfiguredAction.CIM_Dependency
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_PreconfiguredTransportAction.CIM_PreconfiguredSAAction  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_PreconfiguredTransportAction.CIM_PreconfiguredSAAction
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_PreconfiguredTunnelAction.CIM_PreconfiguredSAAction  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_PreconfiguredTunnelAction.CIM_PreconfiguredSAAction
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_PeerGatewayForPreconfiguredTunnel.CIM_Dependency  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_PeerGatewayForPreconfiguredTunnel.CIM_Dependency
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_SANegotiationAction.CIM_SAAction  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_SANegotiationAction.CIM_SAAction
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_IKEAction.CIM_SANegotiationAction  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_IKEAction.CIM_SANegotiationAction
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_IPsecAction.CIM_SANegotiationAction  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_IPsecAction.CIM_SANegotiationAction
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_IPsecTransportAction.CIM_IPsecAction  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_IPsecTransportAction.CIM_IPsecAction
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_IPsecTunnelAction.CIM_IPsecAction  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_IPsecTunnelAction.CIM_IPsecAction
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_PeerGatewayForTunnel.CIM_Dependency  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_PeerGatewayForTunnel.CIM_Dependency
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_SAProposal.CIM_ScopedSettingData  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_SAProposal.CIM_ScopedSettingData
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_ContainedProposal.CIM_Component  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_ContainedProposal.CIM_Component
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_IKEProposal.CIM_SAProposal  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_IKEProposal.CIM_SAProposal
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_IPsecProposal.CIM_SAProposal  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_IPsecProposal.CIM_SAProposal
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/CIM_ContainedTransform.CIM_Component  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_ContainedTransform.CIM_Component
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/PRS_ExchangeElement.CIM_ManagedElement  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/PRS_ExchangeElement.CIM_ManagedElement
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/PRS_Solution.PRS_ExchangeElement  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/PRS_Solution.PRS_ExchangeElement
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/PRS_SolutionElement.PRS_ExchangeElement  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/PRS_SolutionElement.PRS_ExchangeElement
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/PRS_Reference.PRS_SolutionElement  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/PRS_Reference.PRS_SolutionElement
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/PRS_Problem.PRS_SolutionElement  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/PRS_Problem.PRS_SolutionElement
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/PRS_Resolution.PRS_SolutionElement  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/PRS_Resolution.PRS_SolutionElement
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/PRS_SolutionHasElement.CIM_Dependency  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/PRS_SolutionHasElement.CIM_Dependency
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/PRS_SolutionReference.PRS_SolutionHasElement  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/PRS_SolutionReference.PRS_SolutionHasElement
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/PRS_SolutionProblem.PRS_SolutionHasElement  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/PRS_SolutionProblem.PRS_SolutionHasElement
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/PRS_SolutionResolution.PRS_SolutionHasElement  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/PRS_SolutionResolution.PRS_SolutionHasElement
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/PRS_Category.PRS_ExchangeElement  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/PRS_Category.PRS_ExchangeElement
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/PRS_SolutionCategory.CIM_Dependency  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/PRS_SolutionCategory.CIM_Dependency
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/PRS_Categorization.CIM_Dependency  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/PRS_Categorization.CIM_Dependency
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/PRS_CategoryParentChild.CIM_HostedDependency  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/PRS_CategoryParentChild.CIM_HostedDependency
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/PRS_ExpressionElement.PRS_ExchangeElement  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/PRS_ExpressionElement.PRS_ExchangeElement
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/PRS_Expression.PRS_ExpressionElement  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/PRS_Expression.PRS_ExpressionElement
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/PRS_Product.PRS_ExpressionElement  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/PRS_Product.PRS_ExpressionElement
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/PRS_Statement.PRS_ExpressionElement  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/PRS_Statement.PRS_ExpressionElement
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/PRS_Feature.PRS_ExchangeElement  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/PRS_Feature.PRS_ExchangeElement
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/PRS_Resource.PRS_ExchangeElement  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/PRS_Resource.PRS_ExchangeElement
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/PRS_ExpressionLink.CIM_HostedDependency  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/PRS_ExpressionLink.CIM_HostedDependency
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/PRS_ProductParentChild.CIM_Dependency  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/PRS_ProductParentChild.CIM_Dependency
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/PRS_ProductComponent.CIM_Dependency  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/PRS_ProductComponent.CIM_Dependency
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/PRS_ProductAsset.CIM_Dependency  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/PRS_ProductAsset.CIM_Dependency
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/PRS_StatementFeature.CIM_Dependency  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/PRS_StatementFeature.CIM_Dependency
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/PRS_FeatureResource.CIM_Dependency  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/PRS_FeatureResource.CIM_Dependency
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/PRS_ResolutionResource.CIM_Dependency  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/PRS_ResolutionResource.CIM_Dependency
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/PRS_SolutionExpression.CIM_Dependency  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/PRS_SolutionExpression.CIM_Dependency
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/PRS_Administrative.PRS_ExchangeElement  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/PRS_Administrative.PRS_ExchangeElement
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/PRS_AdminAssociation.CIM_Dependency  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/PRS_AdminAssociation.CIM_Dependency
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/PRS_Revision.PRS_ExchangeElement  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/PRS_Revision.PRS_ExchangeElement
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/PRS_AdministrativeRevision.CIM_HostedDependency  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/PRS_AdministrativeRevision.CIM_HostedDependency
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/PRS_Contact.PRS_ExchangeElement  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/PRS_Contact.PRS_ExchangeElement
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/PRS_ContactItem.PRS_ExchangeElement  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/PRS_ContactItem.PRS_ExchangeElement
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/PRS_AdministrativeContact.CIM_Dependency  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/PRS_AdministrativeContact.CIM_Dependency
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/PRS_RevisionContact.CIM_Dependency  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/PRS_RevisionContact.CIM_Dependency
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/PRS_Organization.PRS_ContactItem  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/PRS_Organization.PRS_ContactItem
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/PRS_Person.PRS_ContactItem  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/PRS_Person.PRS_ContactItem
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/PRS_ContactContactItem.CIM_Dependency  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/PRS_ContactContactItem.CIM_Dependency
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/PRS_ContactOrganization.PRS_ContactContactItem  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/PRS_ContactOrganization.PRS_ContactContactItem
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/PRS_ContactPerson.PRS_ContactContactItem  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/PRS_ContactPerson.PRS_ContactContactItem
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/PRS_Address.PRS_ExchangeElement  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/PRS_Address.PRS_ExchangeElement
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/PRS_ContactItemAddress.CIM_Dependency  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/PRS_ContactItemAddress.CIM_Dependency
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/PRS_Location.PRS_ExchangeElement  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/PRS_Location.PRS_ExchangeElement
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/PRS_AddressLocation.CIM_Dependency  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/PRS_AddressLocation.CIM_Dependency
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/PRS_Attachment.PRS_ExchangeElement  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/PRS_Attachment.PRS_ExchangeElement
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/PRS_Attached.CIM_Dependency  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/PRS_Attached.CIM_Dependency
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/PRS_ServiceIncident.PRS_ExchangeElement  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/PRS_ServiceIncident.PRS_ExchangeElement
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/PRS_ServiceRequester.CIM_Dependency  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/PRS_ServiceRequester.CIM_Dependency
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/PRS_ServiceProvider.CIM_Dependency  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/PRS_ServiceProvider.CIM_Dependency
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/PRS_ServiceProblem.CIM_Dependency  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/PRS_ServiceProblem.CIM_Dependency
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/PRS_ServiceResolutionSolution.CIM_Dependency  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/PRS_ServiceResolutionSolution.CIM_Dependency
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/PRS_Activity.PRS_ExchangeElement  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/PRS_Activity.PRS_ExchangeElement
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/PRS_Agreement.PRS_ExchangeElement  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/PRS_Agreement.PRS_ExchangeElement
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/PRS_ServiceAgreement.CIM_Dependency  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/PRS_ServiceAgreement.CIM_Dependency
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/PRS_Transaction.PRS_ExchangeElement  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/PRS_Transaction.PRS_ExchangeElement
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/PRS_SISService.CIM_Service  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/PRS_SISService.CIM_Service
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/PRS_ServiceActivity.CIM_Dependency  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/PRS_ServiceActivity.CIM_Dependency
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/PRS_ActivityContact.CIM_Dependency  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/PRS_ActivityContact.CIM_Dependency
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/PRS_ActivityResource.CIM_Dependency  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/PRS_ActivityResource.CIM_Dependency
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/PRS_ActivityTransaction.CIM_Dependency  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/PRS_ActivityTransaction.CIM_Dependency
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/PG_OperatingSystem.CIM_OperatingSystem  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/PG_OperatingSystem.CIM_OperatingSystem
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/PG_ComputerSystem.CIM_UnitaryComputerSystem  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/PG_ComputerSystem.CIM_UnitaryComputerSystem
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/PG_UnixProcess.CIM_Process  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/PG_UnixProcess.CIM_Process
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/PG_UnixProcessStatisticalInformation.CIM_StatisticalInformation  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/PG_UnixProcessStatisticalInformation.CIM_StatisticalInformation
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/PG_UnixProcessStatistics.CIM_Statistics  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/PG_UnixProcessStatistics.CIM_Statistics
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/PG_UnixThread.CIM_Thread  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/PG_UnixThread.CIM_Thread
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/PG_Processor.CIM_Processor  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/PG_Processor.CIM_Processor
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/PG_BindsIPToLANEndpoint.CIM_BindsToLANEndpoint  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/PG_BindsIPToLANEndpoint.CIM_BindsToLANEndpoint
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/classes/PG_IPRoute.CIM_IPRoute  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/classes/PG_IPRoute.CIM_IPRoute
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/qualifiers/Abstract  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/qualifiers/Abstract
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/qualifiers/Aggregate  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/qualifiers/Aggregate
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/qualifiers/Aggregation  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/qualifiers/Aggregation
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/qualifiers/Alias  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/qualifiers/Alias
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/qualifiers/ArrayType  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/qualifiers/ArrayType
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/qualifiers/ASSOCIATION  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/qualifiers/ASSOCIATION
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/qualifiers/BitMap  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/qualifiers/BitMap
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/qualifiers/BitValues  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/qualifiers/BitValues
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/qualifiers/Composition  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/qualifiers/Composition
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/qualifiers/Counter  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/qualifiers/Counter
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/qualifiers/Delete  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/qualifiers/Delete
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/qualifiers/Deprecated  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/qualifiers/Deprecated
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/qualifiers/Description  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/qualifiers/Description
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/qualifiers/DisplayName  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/qualifiers/DisplayName
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/qualifiers/DN  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/qualifiers/DN
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/qualifiers/EmbeddedObject  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/qualifiers/EmbeddedObject
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/qualifiers/Exception  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/qualifiers/Exception
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/qualifiers/Expensive  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/qualifiers/Expensive
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/qualifiers/Experimental  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/qualifiers/Experimental
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/qualifiers/Gauge  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/qualifiers/Gauge
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/qualifiers/Ifdeleted  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/qualifiers/Ifdeleted
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/qualifiers/In  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/qualifiers/In
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/qualifiers/INDICATION  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/qualifiers/INDICATION
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/qualifiers/Invisible  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/qualifiers/Invisible
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/qualifiers/Key  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/qualifiers/Key
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/qualifiers/Large  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/qualifiers/Large
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/qualifiers/MappingStrings  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/qualifiers/MappingStrings
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/qualifiers/Max  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/qualifiers/Max
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/qualifiers/MaxLen  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/qualifiers/MaxLen
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/qualifiers/MaxValue  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/qualifiers/MaxValue
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/qualifiers/Min  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/qualifiers/Min
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/qualifiers/MinLen  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/qualifiers/MinLen
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/qualifiers/MinValue  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/qualifiers/MinValue
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/qualifiers/ModelCorrespondence  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/qualifiers/ModelCorrespondence
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/qualifiers/Nonlocal  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/qualifiers/Nonlocal
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/qualifiers/NonlocalType  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/qualifiers/NonlocalType
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/qualifiers/NullValue  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/qualifiers/NullValue
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/qualifiers/Octetstring  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/qualifiers/Octetstring
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/qualifiers/Out  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/qualifiers/Out
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/qualifiers/Override  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/qualifiers/Override
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/qualifiers/Propagated  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/qualifiers/Propagated
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/qualifiers/PropertyUsage  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/qualifiers/PropertyUsage
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/qualifiers/Provider  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/qualifiers/Provider
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/qualifiers/Read  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/qualifiers/Read
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/qualifiers/Required  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/qualifiers/Required
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/qualifiers/Revision  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/qualifiers/Revision
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/qualifiers/CLASS  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/qualifiers/CLASS
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/qualifiers/Source  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/qualifiers/Source
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/qualifiers/SourceType  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/qualifiers/SourceType
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/qualifiers/Static  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/qualifiers/Static
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/qualifiers/Syntax  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/qualifiers/Syntax
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/qualifiers/SyntaxType  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/qualifiers/SyntaxType
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/qualifiers/Terminal  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/qualifiers/Terminal
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/qualifiers/TriggerType  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/qualifiers/TriggerType
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/qualifiers/Units  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/qualifiers/Units
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/qualifiers/UnknownValues  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/qualifiers/UnknownValues
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/qualifiers/UnsupportedValues  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/qualifiers/UnsupportedValues
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/qualifiers/ValueMap  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/qualifiers/ValueMap
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/qualifiers/Values  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/qualifiers/Values
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/qualifiers/Version  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/qualifiers/Version
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/qualifiers/Weak  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/qualifiers/Weak
install -D -m 0755  $PEGASUS_HOME/repository/root#cimv2/qualifiers/Write  $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR/root#cimv2/qualifiers/Write

#
# SSL Files
#
install -D -m 0644  $PEGASUS_ROOT/src/Server/ssl.cnf $RPM_BUILD_ROOT%PEGASUS_CONFIG_DIR/ssl.orig

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
install -D -m 0444  $PEGASUS_ROOT/rpm/manLinux/man1.Z/cimmof.1 $RPM_BUILD_ROOT%PEGASUS_MANUSER_DIR/cimmof.1.gz
install -D -m 0444  $PEGASUS_ROOT/rpm/manLinux/man1.Z/cimprovider.1 $RPM_BUILD_ROOT%PEGASUS_MANUSER_DIR/cimprovider.1.gz
install -D -m 0444  $PEGASUS_ROOT/rpm/manLinux/man1.Z/osinfo.1 $RPM_BUILD_ROOT%PEGASUS_MANUSER_DIR/osinfo.1.gz
install -D -m 0444  $PEGASUS_ROOT/rpm/manLinux/man1.Z/wbemexec.1 $RPM_BUILD_ROOT%PEGASUS_MANUSER_DIR/wbemexec.1.gz
install -D -m 0444  $PEGASUS_ROOT/rpm/manLinux/man1m.Z/cimauth.1m $RPM_BUILD_ROOT%PEGASUS_MANADMIN_DIR/cimauth.8.gz
install -D -m 0444  $PEGASUS_ROOT/rpm/manLinux/man1m.Z/cimconfig.1m $RPM_BUILD_ROOT%PEGASUS_MANADMIN_DIR/cimconfig.8.gz
install -D -m 0444  $PEGASUS_ROOT/rpm/manLinux/man1m.Z/cimprovider.1m $RPM_BUILD_ROOT%PEGASUS_MANADMIN_DIR/cimprovider.8.gz
install -D -m 0444  $PEGASUS_ROOT/rpm/manLinux/man1m.Z/cimserver.1m $RPM_BUILD_ROOT%PEGASUS_MANADMIN_DIR/cimserver.8.gz

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
install -D -m 0444 %SDK_STAGE_LOC/samples/mak/config.mak %SAMPLES_DEST_PATH/mak/config.mak 
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

echo "PEGASUS_LIB_DIR="%PEGASUS_DEST_LIB_DIR > samplebld.txt
echo "PEGASUS_PROVIDER_LIB_DIR="%PEGASUS_PROVIDER_LIB_DIR >> samplebld.txt
echo "CIM_MOF_PATH="%PEGASUS_MOF_DIR>> samplebld.txt
echo "PEGASUS_INCLUDE_DIR="%PEGASUS_INCLUDE_DEST_PATH >> samplebld.txt
echo "SAMPLES_DIR="%PEGASUS_SAMPLES_DEST_PATH >> samplebld.txt
echo "PEGASUS_BIN_DIR="%PEGASUS_BIN_DIR >> samplebld.txt
%ifarch ia64
cat samplebld.txt %SDK_STAGE_LOC/samples/mak/LINUX_IA64_GNU.mak > samplebld.mak
install -D -m 0444 samplebld.mak %SAMPLES_DEST_PATH/mak/LINUX_IA64_GNU.mak 
%else
cat samplebld.txt %SDK_STAGE_LOC/samples/mak/LINUX_IX86_GNU.mak > samplebld.mak
install -D -m 0444 samplebld.mak %SAMPLES_DEST_PATH/mak/LINUX_IX86_GNU.mak 
%endif

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
chmod 644 %PEGASUS_CONFIG_DIR/ssl.cnf
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
%doc %PEGASUS_PROD_DIR/%PEGASUS_LICENSE_FILE
%doc %PEGASUS_MANUSER_DIR/cimmof.1.gz
%doc %PEGASUS_MANUSER_DIR/cimprovider.1.gz
%doc %PEGASUS_MANUSER_DIR/osinfo.1.gz
%doc %PEGASUS_MANUSER_DIR/wbemexec.1.gz
%doc %PEGASUS_MANADMIN_DIR/cimauth.8.gz
%doc %PEGASUS_MANADMIN_DIR/cimconfig.8.gz
%doc %PEGASUS_MANADMIN_DIR/cimprovider.8.gz
%doc %PEGASUS_MANADMIN_DIR/cimserver.8.gz
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
%dir %attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root
%dir %attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root/classes
%dir %attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root/instances
%dir %attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root/qualifiers
%dir %attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#PG_Internal
%dir %attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#PG_Internal/classes
%dir %attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#PG_Internal/instances
%dir %attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#PG_Internal/qualifiers
%dir %attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#PG_InterOp
%dir %attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#PG_InterOp/classes
%dir %attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#PG_InterOp/instances
%dir %attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#PG_InterOp/qualifiers
%dir %attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2
%dir %attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes
%dir %attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/instances
%dir %attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/qualifiers
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#PG_Internal/classes/PG_Authorization.#
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#PG_Internal/classes/PG_ConfigSetting.#
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#PG_Internal/classes/PG_User.#
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#PG_Internal/classes/PG_ShutdownService.#
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#PG_Internal/classes/PG_WBEMSLPTemplate.#
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#PG_Internal/qualifiers/Abstract
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#PG_Internal/qualifiers/Aggregate
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#PG_Internal/qualifiers/Aggregation
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#PG_Internal/qualifiers/Alias
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#PG_Internal/qualifiers/ArrayType
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#PG_Internal/qualifiers/ASSOCIATION
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#PG_Internal/qualifiers/BitMap
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#PG_Internal/qualifiers/BitValues
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#PG_Internal/qualifiers/Composition
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#PG_Internal/qualifiers/Counter
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#PG_Internal/qualifiers/Delete
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#PG_Internal/qualifiers/Deprecated
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#PG_Internal/qualifiers/Description
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#PG_Internal/qualifiers/DisplayName
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#PG_Internal/qualifiers/DN
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#PG_Internal/qualifiers/EmbeddedObject
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#PG_Internal/qualifiers/Exception
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#PG_Internal/qualifiers/Expensive
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#PG_Internal/qualifiers/Experimental
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#PG_Internal/qualifiers/Gauge
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#PG_Internal/qualifiers/Ifdeleted
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#PG_Internal/qualifiers/In
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#PG_Internal/qualifiers/INDICATION
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#PG_Internal/qualifiers/Invisible
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#PG_Internal/qualifiers/Key
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#PG_Internal/qualifiers/Large
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#PG_Internal/qualifiers/MappingStrings
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#PG_Internal/qualifiers/Max
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#PG_Internal/qualifiers/MaxLen
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#PG_Internal/qualifiers/MaxValue
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#PG_Internal/qualifiers/Min
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#PG_Internal/qualifiers/MinLen
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#PG_Internal/qualifiers/MinValue
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#PG_Internal/qualifiers/ModelCorrespondence
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#PG_Internal/qualifiers/Nonlocal
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#PG_Internal/qualifiers/NonlocalType
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#PG_Internal/qualifiers/NullValue
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#PG_Internal/qualifiers/Octetstring
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#PG_Internal/qualifiers/Out
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#PG_Internal/qualifiers/Override
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#PG_Internal/qualifiers/Propagated
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#PG_Internal/qualifiers/PropertyUsage
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#PG_Internal/qualifiers/Provider
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#PG_Internal/qualifiers/Read
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#PG_Internal/qualifiers/Required
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#PG_Internal/qualifiers/Revision
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#PG_Internal/qualifiers/CLASS
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#PG_Internal/qualifiers/Source
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#PG_Internal/qualifiers/SourceType
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#PG_Internal/qualifiers/Static
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#PG_Internal/qualifiers/Syntax
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#PG_Internal/qualifiers/SyntaxType
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#PG_Internal/qualifiers/Terminal
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#PG_Internal/qualifiers/TriggerType
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#PG_Internal/qualifiers/Units
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#PG_Internal/qualifiers/UnknownValues
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#PG_Internal/qualifiers/UnsupportedValues
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#PG_Internal/qualifiers/ValueMap
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#PG_Internal/qualifiers/Values
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#PG_Internal/qualifiers/Version
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#PG_Internal/qualifiers/Weak
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#PG_Internal/qualifiers/Write
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#PG_InterOp/classes/CIM_ManagedElement.#
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#PG_InterOp/classes/associations
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#PG_InterOp/classes/CIM_Dependency.#
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#PG_InterOp/classes/CIM_ConcreteDependency.CIM_Dependency
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#PG_InterOp/classes/CIM_Component.#
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#PG_InterOp/classes/CIM_ConcreteComponent.CIM_Component
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#PG_InterOp/classes/CIM_LogicalIdentity.#
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#PG_InterOp/classes/CIM_ConcreteIdentity.CIM_LogicalIdentity
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#PG_InterOp/classes/CIM_ManagedSystemElement.CIM_ManagedElement
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#PG_InterOp/classes/CIM_HostedDependency.CIM_Dependency
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#PG_InterOp/classes/CIM_LogicalElement.CIM_ManagedSystemElement
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#PG_InterOp/classes/CIM_Job.CIM_LogicalElement
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#PG_InterOp/classes/CIM_ConcreteJob.CIM_Job
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#PG_InterOp/classes/CIM_EnabledLogicalElement.CIM_LogicalElement
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#PG_InterOp/classes/CIM_Synchronized.#
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#PG_InterOp/classes/CIM_System.CIM_EnabledLogicalElement
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#PG_InterOp/classes/CIM_SystemComponent.CIM_Component
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#PG_InterOp/classes/CIM_AdminDomain.CIM_System
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#PG_InterOp/classes/CIM_ContainedDomain.CIM_SystemComponent
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#PG_InterOp/classes/CIM_Service.CIM_EnabledLogicalElement
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#PG_InterOp/classes/CIM_HostedService.CIM_HostedDependency
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#PG_InterOp/classes/CIM_ProvidesServiceToElement.CIM_Dependency
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#PG_InterOp/classes/CIM_ServiceAvailableToElement.#
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#PG_InterOp/classes/CIM_ServiceAffectsElement.#
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#PG_InterOp/classes/CIM_ServiceServiceDependency.CIM_ProvidesServiceToElement
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#PG_InterOp/classes/CIM_ServiceComponent.CIM_Component
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#PG_InterOp/classes/CIM_ServiceAccessPoint.CIM_EnabledLogicalElement
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#PG_InterOp/classes/CIM_HostedAccessPoint.CIM_HostedDependency
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#PG_InterOp/classes/CIM_SAPAvailableForElement.#
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#PG_InterOp/classes/CIM_ServiceSAPDependency.CIM_Dependency
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#PG_InterOp/classes/CIM_ServiceAccessBySAP.CIM_Dependency
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#PG_InterOp/classes/CIM_SAPSAPDependency.CIM_Dependency
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#PG_InterOp/classes/CIM_ActiveConnection.CIM_SAPSAPDependency
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#PG_InterOp/classes/CIM_RemoteServiceAccessPoint.CIM_ServiceAccessPoint
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#PG_InterOp/classes/CIM_RemotePort.CIM_RemoteServiceAccessPoint
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#PG_InterOp/classes/CIM_ServiceAccessURI.CIM_ServiceAccessPoint
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#PG_InterOp/classes/CIM_ProtocolEndpoint.CIM_ServiceAccessPoint
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#PG_InterOp/classes/CIM_BindsTo.CIM_SAPSAPDependency
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#PG_InterOp/classes/CIM_ProvidesEndpoint.CIM_ServiceAccessBySAP
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#PG_InterOp/classes/CIM_PhysicalElement.CIM_ManagedSystemElement
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#PG_InterOp/classes/CIM_Location.CIM_ManagedElement
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#PG_InterOp/classes/CIM_ElementLocation.#
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#PG_InterOp/classes/CIM_PhysicalElementLocation.CIM_ElementLocation
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#PG_InterOp/classes/CIM_ContainedLocation.CIM_Component
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#PG_InterOp/classes/CIM_SystemPackaging.CIM_Dependency
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#PG_InterOp/classes/CIM_SoftwareIdentity.CIM_LogicalElement
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#PG_InterOp/classes/CIM_ElementSoftwareIdentity.CIM_Dependency
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#PG_InterOp/classes/CIM_InstalledSoftwareIdentity.#
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#PG_InterOp/classes/CIM_LogicalDevice.CIM_EnabledLogicalElement
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#PG_InterOp/classes/CIM_Realizes.CIM_Dependency
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#PG_InterOp/classes/CIM_SystemDevice.CIM_SystemComponent
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#PG_InterOp/classes/CIM_DeviceServiceImplementation.CIM_Dependency
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#PG_InterOp/classes/CIM_DeviceSAPImplementation.CIM_Dependency
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#PG_InterOp/classes/CIM_StorageExtent.CIM_LogicalDevice
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#PG_InterOp/classes/CIM_BasedOn.CIM_Dependency
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#PG_InterOp/classes/CIM_Collection.CIM_ManagedElement
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#PG_InterOp/classes/CIM_MemberOfCollection.#
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#PG_InterOp/classes/CIM_OrderedMemberOfCollection.CIM_MemberOfCollection
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#PG_InterOp/classes/CIM_SystemSpecificCollection.CIM_Collection
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#PG_InterOp/classes/CIM_ConcreteCollection.CIM_Collection
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#PG_InterOp/classes/CIM_HostedCollection.CIM_HostedDependency
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#PG_InterOp/classes/CIM_CollectionOfMSEs.CIM_Collection
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#PG_InterOp/classes/CIM_CollectedCollections.#
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#PG_InterOp/classes/CIM_CollectedMSEs.CIM_MemberOfCollection
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#PG_InterOp/classes/CIM_RedundancyGroup.CIM_LogicalElement
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#PG_InterOp/classes/CIM_RedundancyComponent.CIM_Component
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#PG_InterOp/classes/CIM_SpareGroup.CIM_RedundancyGroup
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#PG_InterOp/classes/CIM_ActsAsSpare.#
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#PG_InterOp/classes/CIM_StorageRedundancyGroup.CIM_RedundancyGroup
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#PG_InterOp/classes/CIM_ExtraCapacityGroup.CIM_RedundancyGroup
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#PG_InterOp/classes/CIM_ExtentRedundancyComponent.CIM_RedundancyComponent
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#PG_InterOp/classes/CIM_Product.CIM_ManagedElement
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#PG_InterOp/classes/CIM_ProductComponent.CIM_Component
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#PG_InterOp/classes/CIM_ProductParentChild.#
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#PG_InterOp/classes/CIM_CompatibleProduct.#
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#PG_InterOp/classes/CIM_ProductProductDependency.#
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#PG_InterOp/classes/CIM_SupportAccess.CIM_ManagedElement
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#PG_InterOp/classes/CIM_ProductSupport.#
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#PG_InterOp/classes/CIM_FRU.CIM_ManagedElement
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#PG_InterOp/classes/CIM_ProductFRU.#
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#PG_InterOp/classes/CIM_ReplaceableProductFRU.#
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#PG_InterOp/classes/CIM_ProductPhysicalComponent.CIM_Component
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#PG_InterOp/classes/CIM_ProductSoftwareComponent.CIM_Component
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#PG_InterOp/classes/CIM_ProductServiceComponent.CIM_Component
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#PG_InterOp/classes/CIM_ProductPhysicalElements.#
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#PG_InterOp/classes/CIM_FRUPhysicalElements.#
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#PG_InterOp/classes/CIM_FRUIncludesProduct.#
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#PG_InterOp/classes/CIM_Capabilities.CIM_ManagedElement
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#PG_InterOp/classes/CIM_ElementCapabilities.#
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#PG_InterOp/classes/CIM_SettingData.CIM_ManagedElement
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#PG_InterOp/classes/CIM_ScopedSettingData.CIM_SettingData
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#PG_InterOp/classes/CIM_ScopedSetting.CIM_HostedDependency
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#PG_InterOp/classes/CIM_ElementSettingData.#
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#PG_InterOp/classes/CIM_Profile.CIM_Collection
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#PG_InterOp/classes/CIM_ElementProfile.#
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#PG_InterOp/classes/CIM_Setting.CIM_ManagedElement
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#PG_InterOp/classes/CIM_ElementSetting.#
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#PG_InterOp/classes/CIM_DefaultSetting.CIM_ElementSetting
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#PG_InterOp/classes/CIM_SystemSetting.CIM_Setting
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#PG_InterOp/classes/CIM_SettingForSystem.CIM_HostedDependency
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#PG_InterOp/classes/CIM_Configuration.CIM_ManagedElement
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#PG_InterOp/classes/CIM_ConfigurationComponent.#
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#PG_InterOp/classes/CIM_ElementConfiguration.#
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#PG_InterOp/classes/CIM_CollectionConfiguration.#
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#PG_InterOp/classes/CIM_DependencyContext.#
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#PG_InterOp/classes/CIM_SettingContext.#
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#PG_InterOp/classes/CIM_CollectionSetting.#
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#PG_InterOp/classes/CIM_SystemConfiguration.CIM_ManagedElement
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#PG_InterOp/classes/CIM_ConfigurationForSystem.CIM_HostedDependency
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#PG_InterOp/classes/CIM_SystemSettingContext.#
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#PG_InterOp/classes/CIM_StatisticalData.CIM_ManagedElement
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#PG_InterOp/classes/CIM_ElementStatisticalData.#
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#PG_InterOp/classes/CIM_RelatedStatisticalData.#
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#PG_InterOp/classes/CIM_StatisticalInformation.CIM_ManagedElement
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#PG_InterOp/classes/CIM_Statistics.#
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#PG_InterOp/classes/CIM_RelatedStatistics.#
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#PG_InterOp/classes/CIM_SystemStatisticalInformation.CIM_StatisticalInformation
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#PG_InterOp/classes/CIM_SystemStatistics.CIM_Statistics
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#PG_InterOp/classes/CIM_ServiceStatisticalInformation.CIM_StatisticalInformation
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#PG_InterOp/classes/CIM_ServiceStatistics.CIM_Statistics
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#PG_InterOp/classes/CIM_SAPStatisticalInformation.CIM_StatisticalInformation
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#PG_InterOp/classes/CIM_SAPStatistics.CIM_Statistics
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#PG_InterOp/classes/CIM_DeviceStatisticalInformation.CIM_StatisticalInformation
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#PG_InterOp/classes/CIM_DeviceStatistics.CIM_Statistics
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#PG_InterOp/classes/CIM_PhysicalStatisticalInformation.CIM_StatisticalInformation
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#PG_InterOp/classes/CIM_PhysicalStatistics.CIM_Statistics
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#PG_InterOp/classes/CIM_StatisticalSetting.CIM_SettingData
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#PG_InterOp/classes/CIM_MethodParameters.CIM_ManagedElement
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#PG_InterOp/classes/CIM_ParameterValueSources.#
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#PG_InterOp/classes/CIM_ParametersForMethod.#
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#PG_InterOp/classes/CIM_PowerManagementCapabilities.CIM_Capabilities
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#PG_InterOp/classes/CIM_PowerManagementService.CIM_Service
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#PG_InterOp/classes/CIM_Indication.#
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#PG_InterOp/classes/CIM_ClassIndication.CIM_Indication
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#PG_InterOp/classes/CIM_ClassCreation.CIM_ClassIndication
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#PG_InterOp/classes/CIM_ClassDeletion.CIM_ClassIndication
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#PG_InterOp/classes/CIM_ClassModification.CIM_ClassIndication
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#PG_InterOp/classes/CIM_InstIndication.CIM_Indication
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#PG_InterOp/classes/CIM_InstCreation.CIM_InstIndication
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#PG_InterOp/classes/CIM_InstDeletion.CIM_InstIndication
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#PG_InterOp/classes/CIM_InstModification.CIM_InstIndication
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#PG_InterOp/classes/CIM_InstMethodCall.CIM_InstIndication
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#PG_InterOp/classes/CIM_InstRead.CIM_InstIndication
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#PG_InterOp/classes/CIM_ProcessIndication.CIM_Indication
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#PG_InterOp/classes/CIM_SNMPTrapIndication.CIM_ProcessIndication
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#PG_InterOp/classes/CIM_AlertIndication.CIM_ProcessIndication
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#PG_InterOp/classes/CIM_ThresholdIndication.CIM_AlertIndication
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#PG_InterOp/classes/CIM_AlertInstIndication.CIM_AlertIndication
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#PG_InterOp/classes/CIM_IndicationFilter.CIM_ManagedElement
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#PG_InterOp/classes/CIM_ListenerDestination.CIM_ManagedElement
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#PG_InterOp/classes/CIM_ListenerDestinationCIMXML.CIM_ListenerDestination
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#PG_InterOp/classes/CIM_IndicationHandler.CIM_ListenerDestination
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#PG_InterOp/classes/CIM_IndicationHandlerCIMXML.CIM_IndicationHandler
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#PG_InterOp/classes/CIM_IndicationSubscription.#
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#PG_InterOp/classes/PG_IndicationHandlerSNMPMapper.CIM_IndicationHandler
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#PG_InterOp/classes/PG_ProviderModule.CIM_LogicalElement
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#PG_InterOp/classes/PG_Provider.CIM_LogicalElement
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#PG_InterOp/classes/PG_CapabilitiesRegistration.CIM_ManagedElement
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#PG_InterOp/classes/PG_ProviderCapabilities.PG_CapabilitiesRegistration
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#PG_InterOp/classes/PG_ConsumerCapabilities.PG_CapabilitiesRegistration
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#PG_InterOp/classes/PG_ProviderCapabilitiesElements.CIM_Dependency
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#PG_InterOp/classes/PG_ProviderModuleElements.CIM_Dependency
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#PG_InterOp/instances/PG_ProviderCapabilities.idx
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#PG_InterOp/instances/PG_ProviderModule.instances
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#PG_InterOp/instances/PG_ProviderModule.idx
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#PG_InterOp/instances/PG_Provider.instances
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#PG_InterOp/instances/PG_Provider.idx
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#PG_InterOp/instances/PG_ProviderCapabilities.instances
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#PG_InterOp/qualifiers/Abstract
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#PG_InterOp/qualifiers/Aggregate
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#PG_InterOp/qualifiers/Aggregation
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#PG_InterOp/qualifiers/Alias
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#PG_InterOp/qualifiers/ArrayType
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#PG_InterOp/qualifiers/ASSOCIATION
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#PG_InterOp/qualifiers/BitMap
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#PG_InterOp/qualifiers/BitValues
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#PG_InterOp/qualifiers/Composition
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#PG_InterOp/qualifiers/Counter
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#PG_InterOp/qualifiers/Delete
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#PG_InterOp/qualifiers/Deprecated
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#PG_InterOp/qualifiers/Description
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#PG_InterOp/qualifiers/DisplayName
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#PG_InterOp/qualifiers/DN
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#PG_InterOp/qualifiers/EmbeddedObject
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#PG_InterOp/qualifiers/Exception
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#PG_InterOp/qualifiers/Expensive
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#PG_InterOp/qualifiers/Experimental
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#PG_InterOp/qualifiers/Gauge
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#PG_InterOp/qualifiers/Ifdeleted
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#PG_InterOp/qualifiers/In
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#PG_InterOp/qualifiers/INDICATION
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#PG_InterOp/qualifiers/Invisible
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#PG_InterOp/qualifiers/Key
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#PG_InterOp/qualifiers/Large
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#PG_InterOp/qualifiers/MappingStrings
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#PG_InterOp/qualifiers/Max
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#PG_InterOp/qualifiers/MaxLen
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#PG_InterOp/qualifiers/MaxValue
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#PG_InterOp/qualifiers/Min
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#PG_InterOp/qualifiers/MinLen
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#PG_InterOp/qualifiers/MinValue
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#PG_InterOp/qualifiers/ModelCorrespondence
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#PG_InterOp/qualifiers/Nonlocal
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#PG_InterOp/qualifiers/NonlocalType
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#PG_InterOp/qualifiers/NullValue
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#PG_InterOp/qualifiers/Octetstring
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#PG_InterOp/qualifiers/Out
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#PG_InterOp/qualifiers/Override
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#PG_InterOp/qualifiers/Propagated
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#PG_InterOp/qualifiers/PropertyUsage
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#PG_InterOp/qualifiers/Provider
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#PG_InterOp/qualifiers/Read
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#PG_InterOp/qualifiers/Required
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#PG_InterOp/qualifiers/Revision
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#PG_InterOp/qualifiers/CLASS
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#PG_InterOp/qualifiers/Source
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#PG_InterOp/qualifiers/SourceType
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#PG_InterOp/qualifiers/Static
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#PG_InterOp/qualifiers/Syntax
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#PG_InterOp/qualifiers/SyntaxType
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#PG_InterOp/qualifiers/Terminal
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#PG_InterOp/qualifiers/TriggerType
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#PG_InterOp/qualifiers/Units
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#PG_InterOp/qualifiers/UnknownValues
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#PG_InterOp/qualifiers/UnsupportedValues
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#PG_InterOp/qualifiers/ValueMap
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#PG_InterOp/qualifiers/Values
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#PG_InterOp/qualifiers/Version
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#PG_InterOp/qualifiers/Weak
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#PG_InterOp/qualifiers/Write
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_ManagedElement.#
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/associations
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_Dependency.#
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_ConcreteDependency.CIM_Dependency
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_Component.#
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_ConcreteComponent.CIM_Component
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_LogicalIdentity.#
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_ConcreteIdentity.CIM_LogicalIdentity
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_ManagedSystemElement.CIM_ManagedElement
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_HostedDependency.CIM_Dependency
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_LogicalElement.CIM_ManagedSystemElement
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_Job.CIM_LogicalElement
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_ConcreteJob.CIM_Job
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_EnabledLogicalElement.CIM_LogicalElement
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_Synchronized.#
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_System.CIM_EnabledLogicalElement
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_SystemComponent.CIM_Component
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_AdminDomain.CIM_System
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_ContainedDomain.CIM_SystemComponent
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_Service.CIM_EnabledLogicalElement
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_HostedService.CIM_HostedDependency
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_ProvidesServiceToElement.CIM_Dependency
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_ServiceAvailableToElement.#
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_ServiceAffectsElement.#
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_ServiceServiceDependency.CIM_ProvidesServiceToElement
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_ServiceComponent.CIM_Component
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_ServiceAccessPoint.CIM_EnabledLogicalElement
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_HostedAccessPoint.CIM_HostedDependency
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_SAPAvailableForElement.#
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_ServiceSAPDependency.CIM_Dependency
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_ServiceAccessBySAP.CIM_Dependency
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_SAPSAPDependency.CIM_Dependency
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_ActiveConnection.CIM_SAPSAPDependency
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_RemoteServiceAccessPoint.CIM_ServiceAccessPoint
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_RemotePort.CIM_RemoteServiceAccessPoint
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_ServiceAccessURI.CIM_ServiceAccessPoint
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_ProtocolEndpoint.CIM_ServiceAccessPoint
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_BindsTo.CIM_SAPSAPDependency
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_ProvidesEndpoint.CIM_ServiceAccessBySAP
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_PhysicalElement.CIM_ManagedSystemElement
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_Location.CIM_ManagedElement
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_ElementLocation.#
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_PhysicalElementLocation.CIM_ElementLocation
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_ContainedLocation.CIM_Component
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_SystemPackaging.CIM_Dependency
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_SoftwareIdentity.CIM_LogicalElement
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_ElementSoftwareIdentity.CIM_Dependency
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_InstalledSoftwareIdentity.#
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_LogicalDevice.CIM_EnabledLogicalElement
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_Realizes.CIM_Dependency
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_SystemDevice.CIM_SystemComponent
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_DeviceServiceImplementation.CIM_Dependency
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_DeviceSAPImplementation.CIM_Dependency
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_StorageExtent.CIM_LogicalDevice
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_BasedOn.CIM_Dependency
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_Collection.CIM_ManagedElement
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_MemberOfCollection.#
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_OrderedMemberOfCollection.CIM_MemberOfCollection
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_SystemSpecificCollection.CIM_Collection
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_ConcreteCollection.CIM_Collection
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_HostedCollection.CIM_HostedDependency
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_CollectionOfMSEs.CIM_Collection
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_CollectedCollections.#
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_CollectedMSEs.CIM_MemberOfCollection
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_RedundancyGroup.CIM_LogicalElement
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_RedundancyComponent.CIM_Component
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_SpareGroup.CIM_RedundancyGroup
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_ActsAsSpare.#
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_StorageRedundancyGroup.CIM_RedundancyGroup
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_ExtraCapacityGroup.CIM_RedundancyGroup
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_ExtentRedundancyComponent.CIM_RedundancyComponent
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_Product.CIM_ManagedElement
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_ProductComponent.CIM_Component
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_ProductParentChild.#
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_CompatibleProduct.#
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_ProductProductDependency.#
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_SupportAccess.CIM_ManagedElement
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_ProductSupport.#
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_FRU.CIM_ManagedElement
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_ProductFRU.#
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_ReplaceableProductFRU.#
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_ProductPhysicalComponent.CIM_Component
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_ProductSoftwareComponent.CIM_Component
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_ProductServiceComponent.CIM_Component
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_ProductPhysicalElements.#
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_FRUPhysicalElements.#
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_FRUIncludesProduct.#
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_Capabilities.CIM_ManagedElement
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_ElementCapabilities.#
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_SettingData.CIM_ManagedElement
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_ScopedSettingData.CIM_SettingData
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_ScopedSetting.CIM_HostedDependency
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_ElementSettingData.#
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_Profile.CIM_Collection
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_ElementProfile.#
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_Setting.CIM_ManagedElement
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_ElementSetting.#
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_DefaultSetting.CIM_ElementSetting
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_SystemSetting.CIM_Setting
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_SettingForSystem.CIM_HostedDependency
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_Configuration.CIM_ManagedElement
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_ConfigurationComponent.#
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_ElementConfiguration.#
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_CollectionConfiguration.#
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_DependencyContext.#
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_SettingContext.#
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_CollectionSetting.#
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_SystemConfiguration.CIM_ManagedElement
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_ConfigurationForSystem.CIM_HostedDependency
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_SystemSettingContext.#
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_StatisticalData.CIM_ManagedElement
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_ElementStatisticalData.#
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_RelatedStatisticalData.#
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_StatisticalInformation.CIM_ManagedElement
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_Statistics.#
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_RelatedStatistics.#
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_SystemStatisticalInformation.CIM_StatisticalInformation
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_SystemStatistics.CIM_Statistics
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_ServiceStatisticalInformation.CIM_StatisticalInformation
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_ServiceStatistics.CIM_Statistics
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_SAPStatisticalInformation.CIM_StatisticalInformation
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_SAPStatistics.CIM_Statistics
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_DeviceStatisticalInformation.CIM_StatisticalInformation
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_DeviceStatistics.CIM_Statistics
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_PhysicalStatisticalInformation.CIM_StatisticalInformation
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_PhysicalStatistics.CIM_Statistics
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_StatisticalSetting.CIM_SettingData
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_MethodParameters.CIM_ManagedElement
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_ParameterValueSources.#
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_ParametersForMethod.#
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_PowerManagementCapabilities.CIM_Capabilities
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_PowerManagementService.CIM_Service
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_PhysicalPackage.CIM_PhysicalElement
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_Container.CIM_Component
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_PhysicalFrame.CIM_PhysicalPackage
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_Rack.CIM_PhysicalFrame
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_Chassis.CIM_PhysicalFrame
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_PhysicalConnector.CIM_PhysicalElement
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_ChassisInRack.CIM_Container
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_PackageInChassis.CIM_Container
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_Docked.CIM_Dependency
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_Card.CIM_PhysicalPackage
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_SystemBusCard.CIM_Card
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_CardOnCard.CIM_Container
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_PackageLocation.CIM_PhysicalPackage
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_StorageMediaLocation.CIM_PackageLocation
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_Magazine.CIM_StorageMediaLocation
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_MediaPhysicalStatData.CIM_StatisticalData
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_MediaPhysicalStatInfo.CIM_PhysicalStatisticalInformation
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_ConnectedTo.CIM_Dependency
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_Slot.CIM_PhysicalConnector
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_SlotInSlot.CIM_ConnectedTo
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_AdjacentSlots.#
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_PackageInConnector.CIM_Dependency
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_PackageInSlot.CIM_PackageInConnector
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_CardInSlot.CIM_PackageInSlot
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_ConnectorOnPackage.CIM_Container
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_PhysicalLink.CIM_PhysicalElement
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_ElementsLinked.CIM_Dependency
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_LinkHasConnector.CIM_Component
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_PhysicalComponent.CIM_PhysicalElement
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_PackagedComponent.CIM_Container
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_Chip.CIM_PhysicalComponent
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_PhysicalMemory.CIM_Chip
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_MemoryOnCard.CIM_PackagedComponent
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_PhysicalMedia.CIM_PhysicalComponent
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_MemoryWithMedia.CIM_Dependency
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_PhysicalMediaInLocation.CIM_Dependency
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_PhysicalTape.CIM_PhysicalMedia
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_HomeForMedia.CIM_Dependency
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_ReplacementSet.CIM_Collection
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_ParticipatesInSet.#
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_PhysicalCapacity.CIM_ManagedElement
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_ElementCapacity.#
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_MemoryCapacity.CIM_PhysicalCapacity
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_ConfigurationCapacity.CIM_PhysicalCapacity
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_ComputerSystem.CIM_System
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_ComputerSystemNodeCapabilities.CIM_Capabilities
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_ComponentCS.CIM_SystemComponent
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_UnitaryComputerSystem.CIM_ComputerSystem
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_SystemPartition.CIM_ComponentCS
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_VirtualComputerSystem.CIM_ComputerSystem
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_HostingCS.CIM_HostedDependency
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_Cluster.CIM_ComputerSystem
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_ParticipatingCS.CIM_Dependency
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_Export.#
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_ClusteringService.CIM_Service
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_ClusteringSAP.CIM_ServiceAccessPoint
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_ClusterServiceAccessBySAP.CIM_ServiceAccessBySAP
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_HostedClusterService.CIM_HostedService
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_HostedClusterSAP.CIM_HostedAccessPoint
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_ComputerSystemPackage.CIM_SystemPackaging
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_FileSystem.CIM_EnabledLogicalElement
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_LocalFileSystem.CIM_FileSystem
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_RemoteFileSystem.CIM_FileSystem
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_NFS.CIM_RemoteFileSystem
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_FileSystemCapabilities.CIM_Capabilities
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_FileSystemSettingData.CIM_ScopedSettingData
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_LogicalFile.CIM_LogicalElement
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_DataFile.CIM_LogicalFile
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_Directory.CIM_LogicalFile
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_DeviceFile.CIM_LogicalFile
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_FIFOPipeFile.CIM_LogicalFile
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_SymbolicLink.CIM_LogicalFile
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_DeviceAccessedByFile.CIM_Dependency
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_DirectoryContainsFile.CIM_Component
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_Mount.CIM_Dependency
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_HostedFileSystem.CIM_SystemComponent
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_FileStorage.CIM_Component
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_ResidesOnExtent.CIM_Dependency
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_OperatingSystem.CIM_EnabledLogicalElement
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_InstalledOS.CIM_SystemComponent
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_RunningOS.CIM_Dependency
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_BootOSFromFS.CIM_Dependency
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_Process.CIM_EnabledLogicalElement
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_OSProcess.CIM_Component
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_ProcessExecutable.CIM_Dependency
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_ServiceProcess.#
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_Thread.CIM_EnabledLogicalElement
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_ProcessThread.CIM_Component
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_OwningJobElement.#
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_AffectedJobElement.#
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_ProcessOfJob.CIM_Component
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_JobDestination.CIM_EnabledLogicalElement
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_HostedJobDestination.CIM_HostedDependency
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_JobDestinationJobs.CIM_Dependency
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_BootService.CIM_Service
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_BootSAP.CIM_ServiceAccessPoint
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_BootServiceAccessBySAP.CIM_ServiceAccessBySAP
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_HostedBootService.CIM_HostedService
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_HostedBootSAP.CIM_HostedAccessPoint
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_OOBAlertService.CIM_Service
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_WakeUpService.CIM_Service
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_TimeZone.CIM_Setting
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_UnixLocalFileSystem.CIM_LocalFileSystem
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_UnixProcess.CIM_Process
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_UnixProcessStatisticalInformation.CIM_StatisticalInformation
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_UnixProcessStatistics.CIM_Statistics
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_UnixThread.CIM_Thread
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_UnixFile.CIM_LogicalElement
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_FileIdentity.CIM_LogicalIdentity
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_UnixDirectory.CIM_Directory
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_UnixDeviceFile.CIM_DeviceFile
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_SystemResource.CIM_EnabledLogicalElement
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_IRQ.CIM_SystemResource
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_MemoryMappedIO.CIM_SystemResource
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_MemoryResource.CIM_MemoryMappedIO
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_PortResource.CIM_MemoryMappedIO
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_DMA.CIM_SystemResource
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_ResourceOfSystem.CIM_SystemComponent
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_ComputerSystemResource.CIM_ResourceOfSystem
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_ComputerSystemIRQ.CIM_ComputerSystemResource
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_ComputerSystemMappedIO.CIM_ComputerSystemResource
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_ComputerSystemDMA.CIM_ComputerSystemResource
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_AllocatedResource.CIM_Dependency
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_AllocatedDMA.CIM_AllocatedResource
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_MessageLog.CIM_EnabledLogicalElement
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_LogRecord.CIM_ManagedElement
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_RecordInLog.#
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_LogInDeviceFile.CIM_Dependency
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_LogInDataFile.CIM_Dependency
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_LogInStorage.CIM_Dependency
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_UseOfMessageLog.CIM_Dependency
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_OperationLog.CIM_UseOfMessageLog
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_DiagnosticSetting.CIM_Setting
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_DiagnosticResult.#
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_DiagnosticTest.CIM_Service
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_DiagnosticResultForMSE.#
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_DiagnosticResultForTest.#
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_DiagnosticTestForMSE.CIM_ProvidesServiceToElement
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_DiagnosticTestInPackage.CIM_Component
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_DiagnosticResultInPackage.#
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_DiagnosticSettingForTest.CIM_ElementSetting
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_Indication.#
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_ClassIndication.CIM_Indication
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_ClassCreation.CIM_ClassIndication
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_ClassDeletion.CIM_ClassIndication
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_IndicationSubscription.#
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_ClassModification.CIM_ClassIndication
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_InstIndication.CIM_Indication
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_InstCreation.CIM_InstIndication
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_InstDeletion.CIM_InstIndication
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_InstModification.CIM_InstIndication
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_InstMethodCall.CIM_InstIndication
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_InstRead.CIM_InstIndication
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_ProcessIndication.CIM_Indication
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_SNMPTrapIndication.CIM_ProcessIndication
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_AlertIndication.CIM_ProcessIndication
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_ThresholdIndication.CIM_AlertIndication
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_AlertInstIndication.CIM_AlertIndication
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_IndicationFilter.CIM_ManagedElement
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_ListenerDestination.CIM_ManagedElement
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_ListenerDestinationCIMXML.CIM_ListenerDestination
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_IndicationHandler.CIM_ListenerDestination
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_IndicationHandlerCIMXML.CIM_IndicationHandler
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_WBEMService.CIM_Service
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_ObjectManager.CIM_WBEMService
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_ObjectManagerCommunicationMechanism.CIM_ServiceAccessPoint
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_CIMXMLCommunicationMechanism.CIM_ObjectManagerCommunicationMechanism
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_CommMechanismForManager.CIM_ServiceAccessBySAP
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_ProtocolAdapter.CIM_WBEMService
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_ObjectManagerAdapter.CIM_WBEMService
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_CommMechanismForAdapter.CIM_Dependency
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_CommMechanismForObjectManagerAdapter.CIM_Dependency
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_Namespace.CIM_ManagedElement
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_SystemIdentification.CIM_ManagedElement
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_NamespaceInManager.CIM_HostedDependency
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_SystemInNamespace.#
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_IdentificationOfManagedSystem.CIM_Dependency
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_CIMOMStatisticalData.CIM_StatisticalData
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_RegisteredProfile.CIM_ManagedElement
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_RegisteredSubProfile.CIM_RegisteredProfile
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_ReferencedProfile.CIM_Dependency
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_SubProfileRequiresProfile.CIM_ReferencedProfile
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_ElementConformsToProfile.#
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_MemberPrincipal.CIM_MemberOfCollection
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_CollectionInSystem.#
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_OrganizationalEntity.CIM_ManagedElement
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_OrgStructure.#
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_CollectionInOrganization.#
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_Organization.CIM_OrganizationalEntity
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_OtherOrganizationInformation.CIM_ManagedElement
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_MoreOrganizationInfo.CIM_Dependency
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_OrgUnit.CIM_OrganizationalEntity
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_OtherOrgUnitInformation.CIM_ManagedElement
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_MoreOrgUnitInfo.CIM_Dependency
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_UserEntity.CIM_OrganizationalEntity
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_UserContact.CIM_UserEntity
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_Person.CIM_UserContact
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_SystemAdministrator.CIM_Dependency
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_OtherPersonInformation.CIM_ManagedElement
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_MorePersonInfo.CIM_Dependency
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_Group.CIM_Collection
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_OtherGroupInformation.CIM_ManagedElement
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_MoreGroupInfo.CIM_Dependency
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_SystemAdministratorGroup.CIM_Dependency
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_Role.CIM_Collection
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_OtherRoleInformation.CIM_ManagedElement
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_MoreRoleInfo.CIM_Dependency
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_SystemAdministratorRole.CIM_Dependency
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_SecurityService.CIM_Service
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_ServiceUsesSecurityService.CIM_ServiceServiceDependency
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_SecurityServiceForSystem.CIM_ProvidesServiceToElement
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_AuthenticationService.CIM_SecurityService
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_VerificationService.CIM_AuthenticationService
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_CredentialManagementService.CIM_AuthenticationService
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_TrustHierarchy.CIM_Dependency
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_CredentialManagementSAP.CIM_ServiceAccessPoint
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_LocalCredentialManagementService.CIM_CredentialManagementService
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_AuthorizationService.CIM_SecurityService
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_Credential.CIM_ManagedElement
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_ManagedCredential.CIM_Dependency
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_Identity.CIM_ManagedElement
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_IdentityContext.#
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_AssignedIdentity.#
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_IPNetworkIdentity.CIM_Identity
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_SecuritySensitivity.CIM_ManagedElement
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_ElementSecuritySensitivity.#
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_AuthenticationRequirement.CIM_LogicalElement
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_HostedAuthenticationRequirement.CIM_HostedDependency
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_SharedSecret.CIM_Credential
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_AuthenticateForUse.CIM_Dependency
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_RequireCredentialsFrom.CIM_Dependency
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_AuthenticationTarget.CIM_Dependency
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_KerberosKeyDistributionCenter.CIM_CredentialManagementService
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_KerberosTicket.CIM_Credential
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_KDCIssuesKerberosTicket.CIM_ManagedCredential
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_CertificateAuthority.CIM_CredentialManagementService
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_PublicKeyManagementService.CIM_LocalCredentialManagementService
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_PublicKeyCertificate.CIM_Credential
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_CAHasPublicCertificate.CIM_Dependency
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_CASignsPublicKeyCertificate.CIM_ManagedCredential
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_UnsignedPublicKey.CIM_Credential
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_LocallyManagedPublicKey.CIM_ManagedCredential
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_SharedSecretService.CIM_LocalCredentialManagementService
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_SharedSecretIsShared.CIM_ManagedCredential
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_NamedSharedIKESecret.CIM_Credential
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_IKESecretIsNamed.CIM_ManagedCredential
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_UsersAccess.CIM_UserEntity
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_ElementAsUser.CIM_Dependency
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_UsersCredential.CIM_Dependency
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_Notary.CIM_CredentialManagementService
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_NotaryVerifiesBiometric.CIM_Dependency
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_PublicPrivateKeyPair.CIM_UsersCredential
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_Privilege.CIM_ManagedElement
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_AuthorizedPrivilege.CIM_Privilege
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_AuthorizedSubject.#
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_AuthorizedTarget.#
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_Account.CIM_LogicalElement
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_AccountIdentity.CIM_LogicalIdentity
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_AccountOnSystem.CIM_SystemComponent
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_UsersAccount.CIM_Dependency
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_AccountMapsToAccount.CIM_Dependency
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_SecurityServiceUsesAccount.CIM_Dependency
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_AccountManagementService.CIM_SecurityService
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_ManagesAccount.CIM_Dependency
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_ManagesAccountOnSystem.CIM_SecurityServiceForSystem
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_AccessControlInformation.CIM_LogicalElement
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_HostedACI.CIM_HostedDependency
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_AuthorizedUse.CIM_Dependency
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_AuthorizationSubject.CIM_Dependency
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_AuthorizationTarget.CIM_Dependency
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_PrivilegeManagementService.CIM_AuthorizationService
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_StorageClientSettingData.CIM_SettingData
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_StorageHardwareID.CIM_Identity
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_GatewayPathID.CIM_StorageHardwareID
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_StorageHardwareIDManagementService.CIM_AuthenticationService
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_ApplicationSystem.CIM_System
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_ApplicationSystemDirectory.#
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_SoftwareElement.CIM_LogicalElement
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_SoftwareFeature.CIM_LogicalElement
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_SoftwareFeatureSoftwareElements.CIM_Component
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_SoftwareElementComponent.CIM_Component
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_SoftwareFeatureComponent.CIM_Component
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_ProductSoftwareFeatures.#
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_ApplicationSystemSoftwareFeature.CIM_SystemComponent
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_SoftwareFeatureServiceImplementation.CIM_Dependency
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_SoftwareFeatureSAPImplementation.CIM_Dependency
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_FRUIncludesSoftwareFeature.#
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_SoftwareElementServiceImplementation.CIM_Dependency
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_SoftwareElementSAPImplementation.CIM_Dependency
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_InstalledProduct.CIM_Collection
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_InstalledProductImage.#
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_CollectedSoftwareElements.CIM_MemberOfCollection
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_CollectedSoftwareFeatures.CIM_MemberOfCollection
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_Check.CIM_ManagedElement
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_DirectorySpecification.CIM_Check
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_ArchitectureCheck.CIM_Check
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_MemoryCheck.CIM_Check
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_DiskSpaceCheck.CIM_Check
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_SwapSpaceCheck.CIM_Check
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_OSVersionCheck.CIM_Check
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_SoftwareElementVersionCheck.CIM_Check
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_FileSpecification.CIM_Check
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_VersionCompatibilityCheck.CIM_Check
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_SettingCheck.CIM_Check
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_SoftwareElementChecks.#
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_DirectorySpecificationFile.#
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_Action.CIM_ManagedElement
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_DirectoryAction.CIM_Action
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_CreateDirectoryAction.CIM_DirectoryAction
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_RemoveDirectoryAction.CIM_DirectoryAction
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_FileAction.CIM_Action
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_CopyFileAction.CIM_FileAction
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_RemoveFileAction.CIM_FileAction
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_RebootAction.CIM_Action
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_ExecuteProgram.CIM_Action
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_ModifySettingAction.CIM_Action
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_ActionSequence.#
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_SoftwareElementActions.#
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_ToDirectorySpecification.#
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_FromDirectorySpecification.#
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_ToDirectoryAction.#
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_FromDirectoryAction.#
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_InstalledSoftwareElement.#
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_OperatingSystemSoftwareFeature.CIM_Component
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_DiagnosticTestSoftware.CIM_Dependency
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_BIOSFeature.CIM_SoftwareFeature
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_BIOSElement.CIM_SoftwareElement
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_BIOSFeatureBIOSElements.CIM_SoftwareFeatureSoftwareElements
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_SystemBIOS.CIM_SystemComponent
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_VideoBIOSFeature.CIM_SoftwareFeature
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_VideoBIOSElement.CIM_SoftwareElement
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_VideoBIOSFeatureVideoBIOSElements.CIM_SoftwareFeatureSoftwareElements
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_AGPSoftwareFeature.CIM_SoftwareFeature
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_ApplicationSystemDependency.CIM_Dependency
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_ApplicationSystemHierarchy.CIM_Component
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_StatisticalRuntimeOverview.CIM_StatisticalData
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_AssociatedAppSystemOverviewStatistics.CIM_ElementStatisticalData
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_J2eeManagedObjectCapabilities.CIM_Capabilities
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_J2eeManagedObject.CIM_EnabledLogicalElement
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_J2eeDomain.CIM_AdminDomain
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_J2eeServer.CIM_ApplicationSystem
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_J2eeServerInDomain.CIM_SystemComponent
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_J2eeJVM.CIM_EnabledLogicalElement
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_J2eeServerUsesJVM.CIM_Dependency
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_J2eeDeployedObject.CIM_Service
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_J2eeModule.CIM_J2eeDeployedObject
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_J2eeApplication.CIM_ApplicationSystem
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_J2eeApplicationModule.CIM_SystemComponent
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_J2eeApplicationHostedOnServer.CIM_HostedDependency
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_J2eeModuleUsesJVM.CIM_HostedDependency
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_J2eeAppClientModule.CIM_J2eeModule
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_J2eeEJBModule.CIM_J2eeModule
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_J2eeEJB.CIM_Service
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_J2eeMessageDrivenBean.CIM_J2eeEJB
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_J2eeEntityBean.CIM_J2eeEJB
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_J2eeSessionBean.CIM_J2eeEJB
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_J2eeStatefulSessionBean.CIM_J2eeSessionBean
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_J2eeStatelessSessionBean.CIM_J2eeSessionBean
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_J2eeEJBInModule.CIM_ServiceComponent
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_J2eeWebModule.CIM_J2eeModule
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_J2eeServlet.CIM_Service
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_J2eeServletInModule.CIM_ServiceComponent
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_J2eeResourceAdapterModule.CIM_J2eeModule
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_J2eeResourceAdapter.CIM_Service
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_J2eeResourceAdapterInModule.CIM_ServiceComponent
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_J2eeResource.CIM_SystemResource
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_J2eeResourceOnServer.CIM_ResourceOfSystem
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_J2eeJavaMailResource.CIM_J2eeResource
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_J2eeJCAResource.CIM_J2eeResource
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_J2eeJCAConnectionFactory.CIM_EnabledLogicalElement
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_J2eeJDBCResource.CIM_J2eeResource
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_J2eeJMSResource.CIM_J2eeResource
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_J2eeJNDIResource.CIM_J2eeResource
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_J2eeJTAResource.CIM_J2eeResource
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_J2eeRMI_IIOPResource.CIM_J2eeResource
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_J2eeURLResource.CIM_J2eeResource
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_J2eeJCAManagedConnectionFactory.CIM_EnabledLogicalElement
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_J2eeJDBCDataSource.CIM_EnabledLogicalElement
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_J2eeJDBCDriver.CIM_EnabledLogicalElement
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_J2eeJDBCResourceUsesDataSource.CIM_HostedDependency
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_J2eeJDBCDataSourceDriver.CIM_HostedDependency
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_J2eeConnectionFactoryAvailableToJCAResource.CIM_HostedDependency
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_J2eeJCAConnectionFactoryManagedConnectionFactory.CIM_HostedDependency
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_J2eeNotification.CIM_ProcessIndication
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_J2eeStatistic.CIM_StatisticalData
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_J2eeEJBStats.CIM_J2eeStatistic
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_J2eeEntityBeanStats.CIM_J2eeEJBStats
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_J2eeMessageDrivenBeanStats.CIM_J2eeEJBStats
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_J2eeSessionBeanStats.CIM_J2eeEJBStats
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_J2eeStatefulSessionBeanStats.CIM_J2eeSessionBeanStats
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_J2eeStatelessSessionBeanStats.CIM_J2eeSessionBeanStats
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_J2eeJavaMailStats.CIM_J2eeStatistic
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_J2eeJCAStats.CIM_J2eeStatistic
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_J2eeConnectionStats.CIM_J2eeStatistic
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_J2eeConnectionPoolStats.CIM_J2eeConnectionStats
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_J2eeJCANonpooledConnections.CIM_RelatedStatisticalData
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_J2eeJCAConnectionPools.CIM_RelatedStatisticalData
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_J2eeJDBCStats.CIM_J2eeStatistic
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_J2eeJDBCNonpooledConnections.CIM_RelatedStatisticalData
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_J2eeJDBCConnectionPools.CIM_RelatedStatisticalData
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_J2eeJMSStats.CIM_J2eeStatistic
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_J2eeJMSConnectionStats.CIM_J2eeStatistic
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_J2eeJMSStatConnections.CIM_RelatedStatisticalData
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_J2eeJMSSessionStats.CIM_J2eeStatistic
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_J2eeJMSEndpointStats.CIM_J2eeStatistic
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_J2eeJMSProducerStats.CIM_J2eeJMSEndpointStats
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_J2eeJMSConsumerStats.CIM_J2eeJMSEndpointStats
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_J2eeJMSConnectionSessions.CIM_RelatedStatisticalData
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_J2eeJMSSessionProducers.CIM_RelatedStatisticalData
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_J2eeJMSSessionConsumers.CIM_RelatedStatisticalData
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_J2eeJTAStats.CIM_J2eeStatistic
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_J2eeJVMStats.CIM_J2eeStatistic
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_J2eeServletStats.CIM_J2eeStatistic
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_J2eeURLStats.CIM_J2eeStatistic
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_BaseMetricDefinition.CIM_ManagedElement
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_BaseMetricValue.CIM_ManagedElement
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_MetricInstance.CIM_Dependency
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_MetricDefForME.CIM_Dependency
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_MetricForME.CIM_Dependency
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_UnitOfWorkDefinition.CIM_LogicalElement
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_MetricDefinition.CIM_BaseMetricDefinition
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_TraceLevelType.CIM_ManagedElement
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_UoWMetricDefinition.CIM_Dependency
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_UnitOfWork.CIM_ManagedElement
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_UoWMetric.CIM_Dependency
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_StartedUoW.CIM_Dependency
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_LogicalElementUnitOfWorkDef.CIM_Dependency
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_SubUoWDef.CIM_Dependency
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_LogicalElementPerformsUoW.CIM_Dependency
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_SubUoW.CIM_Dependency
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_UoWDefTraceLevelType.#
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_Network.CIM_AdminDomain
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_AutonomousSystem.CIM_AdminDomain
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_RoutersInAS.CIM_SystemComponent
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_LogicalNetwork.CIM_CollectionOfMSEs
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_NetworksInAdminDomain.#
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_IPSubnet.CIM_LogicalNetwork
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_LANSegment.CIM_LogicalNetwork
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_IPXNetwork.CIM_LogicalNetwork
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_InLogicalNetwork.CIM_CollectedMSEs
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_RangeOfIPAddresses.CIM_SystemSpecificCollection
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_IPAddressRange.CIM_CollectionOfMSEs
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_NamedAddressCollection.CIM_SystemSpecificCollection
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_ConnectivityCollection.CIM_SystemSpecificCollection
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_IPConnectivitySubnet.CIM_ConnectivityCollection
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_LANConnectivitySegment.CIM_ConnectivityCollection
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_IPXConnectivityNetwork.CIM_ConnectivityCollection
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_ConnectivityMembershipSettingData.CIM_SettingData
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_LANEndpoint.CIM_ProtocolEndpoint
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_InSegment.CIM_InLogicalNetwork
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_BindsToLANEndpoint.CIM_BindsTo
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_IPProtocolEndpoint.CIM_ProtocolEndpoint
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_BGPProtocolEndpoint.CIM_ProtocolEndpoint
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_ASBGPEndpoints.CIM_SystemComponent
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_IPXProtocolEndpoint.CIM_ProtocolEndpoint
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_TCPProtocolEndpoint.CIM_ProtocolEndpoint
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_UDPProtocolEndpoint.CIM_ProtocolEndpoint
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_OSPFProtocolEndpointBase.CIM_ProtocolEndpoint
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_OSPFProtocolEndpoint.CIM_OSPFProtocolEndpointBase
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_OSPFVirtualInterface.CIM_OSPFProtocolEndpointBase
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_EndpointIdentity.CIM_LogicalIdentity
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_SwitchPort.CIM_ProtocolEndpoint
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_FilterEntryBase.CIM_LogicalElement
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_FilterEntry.CIM_FilterEntryBase
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_IPHeadersFilter.CIM_FilterEntryBase
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_Hdr8021Filter.CIM_FilterEntryBase
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_PreambleFilter.CIM_FilterEntryBase
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_FilterList.CIM_LogicalElement
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_FilterEntryInSystem.CIM_SystemComponent
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_FilterListInSystem.CIM_SystemComponent
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_HostedFilterEntryBase.CIM_HostedDependency
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_HostedFilterList.CIM_HostedDependency
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_EntriesInFilterList.CIM_Component
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_X509CredentialFilterEntry.CIM_FilterEntryBase
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_IPSOFilterEntry.CIM_FilterEntryBase
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_PeerIDPayloadFilterEntry.CIM_FilterEntryBase
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_NetworkService.CIM_Service
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_NetworkServicesInAdminDomain.CIM_HostedDependency
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_LogicalNetworkService.#
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_ForwardingService.CIM_NetworkService
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_ForwardsAmong.CIM_ServiceSAPDependency
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_HostedForwardingServices.CIM_HostedService
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_RouteCalculationService.CIM_NetworkService
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_CalculatesAmong.CIM_ServiceSAPDependency
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_HostedRoutingServices.CIM_HostedService
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_RoutingProtocolDomain.CIM_AdminDomain
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_RoutingProtocolDomainInAS.CIM_ContainedDomain
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_RoutingPolicy.CIM_LogicalElement
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_HostedRoutingPolicy.CIM_HostedDependency
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_ListsInRoutingPolicy.CIM_Component
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_NextHopRoute.CIM_ManagedElement
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_HostedRoute.CIM_HostedDependency
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_NextHopIPRoute.CIM_NextHopRoute
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_RouteUsesEndpoint.CIM_Dependency
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_AssociatedNextHop.CIM_Dependency
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_NextHopRouting.CIM_LogicalElement
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_IPRoute.CIM_NextHopRouting
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_RouteForwardedByService.CIM_Dependency
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_ForwardedRoutes.CIM_RouteForwardedByService
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_CalculatedRoutes.CIM_Dependency
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_AdministrativeDistance.CIM_LogicalElement
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_HostedAdminDistance.CIM_HostedDependency
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_NetworkPipe.CIM_EnabledLogicalElement
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_HostedNetworkPipe.CIM_HostedDependency
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_EndpointOfNetworkPipe.CIM_Dependency
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_NetworkPipeComposition.CIM_Component
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_BufferPool.CIM_CollectionOfMSEs
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_CollectedBufferPool.CIM_MemberOfCollection
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_SNMPService.CIM_Service
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_SNMPCommunityString.CIM_ServiceAccessPoint
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_SNMPTrapTarget.CIM_RemotePort
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_TrapSourceForSNMPService.CIM_ServiceSAPDependency
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_OSPFArea.CIM_RoutingProtocolDomain
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_OSPFService.CIM_RouteCalculationService
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_OSPFServiceCapabilities.CIM_Capabilities
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_OSPFAreaConfiguration.CIM_LogicalElement
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_OSPFLink.CIM_ConnectivityCollection
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_AreaOfConfiguration.CIM_Dependency
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_OSPFServiceConfiguration.CIM_Dependency
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_RangesOfConfiguration.CIM_Dependency
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_EndpointInLink.CIM_MemberOfCollection
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_EndpointInArea.CIM_Component
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_BGPService.CIM_RouteCalculationService
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_BGPAdminDistance.CIM_Dependency
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_BGPCluster.CIM_CollectionOfMSEs
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_RoutersInBGPCluster.CIM_CollectedMSEs
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_BGPClustersInAS.#
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_Confederation.CIM_SystemComponent
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_BGPIPRoute.CIM_IPRoute
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_RoutesBGP.CIM_Dependency
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_EGPRouteCalcDependency.CIM_ServiceServiceDependency
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_BGPPeerGroup.CIM_CollectionOfMSEs
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_HostedBGPPeerGroup.CIM_HostedDependency
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_InBGPPeerGroup.CIM_CollectedMSEs
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_BGPPeerGroupService.CIM_Dependency
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_ReflectorService.CIM_Dependency
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_ReflectorClientService.CIM_Dependency
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_ReflectorNonClientService.CIM_Dependency
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_BGPRoutingPolicy.CIM_Dependency
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_BGPAttributes.CIM_LogicalElement
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_HostedBGPAttributes.CIM_HostedDependency
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_FilteredBGPAttributes.CIM_Component
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_BGPRouteMap.CIM_LogicalElement
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_HostedBGPRouteMap.CIM_HostedDependency
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_BGPRouteMapsInRoutingPolicy.CIM_Component
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_BGPPeerUsesRouteMap.CIM_CollectedMSEs
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_FilterListsInBGPRouteMap.CIM_Dependency
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_BGPPathAttributes.CIM_LogicalElement
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_BGPAttributesForRoute.CIM_Dependency
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_BGPServiceAttributes.CIM_Component
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_BGPEndpointStatistics.CIM_StatisticalData
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_BGPStatistics.CIM_ServiceStatisticalInformation
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_BGPServiceStatistics.CIM_ServiceStatistics
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_SwitchService.CIM_ForwardingService
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_SwitchesAmong.CIM_ForwardsAmong
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_Switchable.CIM_BindsToLANEndpoint
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_TransparentBridgingService.CIM_ForwardingService
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_SwitchServiceTransparentBridging.CIM_ServiceComponent
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_SpanningTreeService.CIM_ForwardingService
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_SwitchServiceSpanningTree.CIM_ServiceComponent
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_SwitchPortSpanningTree.CIM_ServiceSAPDependency
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_DynamicForwardingEntry.CIM_LogicalElement
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_TransparentBridgingDynamicForwarding.CIM_Dependency
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_SwitchPortDynamicForwarding.CIM_Dependency
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_StaticForwardingEntry.CIM_LogicalElement
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_TransparentBridgingStaticForwarding.CIM_Dependency
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_SwitchPortStaticForwarding.CIM_Dependency
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_SourceRoutingService.CIM_ForwardingService
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_SwitchServiceSourceRouting.CIM_ServiceComponent
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_SwitchPortPair.CIM_SAPSAPDependency
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_SwitchPortSourceRouting.CIM_ServiceSAPDependency
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_RelatedSpanningTree.CIM_ServiceServiceDependency
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_TransparentBridgingStatistics.CIM_ServiceStatisticalInformation
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_SpanningTreeStatistics.CIM_ServiceStatisticalInformation
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_SwitchPortStatistics.CIM_SAPStatisticalInformation
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_SwitchPortTransparentBridgingStatistics.CIM_SAPStatisticalInformation
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_SwitchPortSpanningTreeStatistics.CIM_SAPStatisticalInformation
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_SwitchPortSourceRoutingStatistics.CIM_SAPStatisticalInformation
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_QoSService.CIM_Service
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_QoSSubService.CIM_ServiceComponent
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_PrecedenceService.CIM_QoSService
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_DiffServService.CIM_QoSService
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_Hdr8021PService.CIM_QoSService
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_AFService.CIM_DiffServService
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_FlowService.CIM_QoSService
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_EFService.CIM_DiffServService
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_AFRelatedServices.#
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_ConditioningService.CIM_Service
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_QoSConditioningSubService.CIM_ServiceComponent
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_ConditioningServiceOnEndpoint.CIM_Dependency
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_IngressConditioningServiceOnEndpoint.CIM_ServiceSAPDependency
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_EgressConditioningServiceOnEndpoint.CIM_ServiceSAPDependency
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_NextService.#
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_ClassifierService.CIM_ConditioningService
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_ClassifierElement.CIM_ClassifierService
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_ClassifierElementInClassifierService.CIM_ServiceComponent
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_ClassifierElementUsesFilterList.CIM_Dependency
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_NextServiceAfterClassifierElement.CIM_NextService
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_ClassifierFilterSet.CIM_Dependency
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_MeterService.CIM_ConditioningService
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_AverageRateMeterService.CIM_MeterService
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_EWMAMeterService.CIM_MeterService
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_TokenBucketMeterService.CIM_MeterService
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_NextServiceAfterMeter.#
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_MarkerService.CIM_ConditioningService
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_PreambleMarkerService.CIM_MarkerService
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_ToSMarkerService.CIM_MarkerService
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_DSCPMarkerService.CIM_MarkerService
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_Priority8021QMarkerService.CIM_MarkerService
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_DropperService.CIM_ConditioningService
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_REDDropperService.CIM_DropperService
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_WeightedREDDropperService.CIM_DropperService
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_HeadTailDropper.CIM_DropperService
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_QueuingService.CIM_ConditioningService
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_DropThresholdCalculationService.CIM_Service
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_QueueHierarchy.CIM_ServiceServiceDependency
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_CalculationServiceForDropper.CIM_ServiceServiceDependency
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_CalculationBasedOnQueue.CIM_ServiceServiceDependency
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_HeadTailDropQueueBinding.CIM_Dependency
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_QueueAllocation.CIM_Dependency
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_PacketSchedulingService.CIM_ConditioningService
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_NextScheduler.CIM_NextService
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_SchedulerUsed.CIM_ServiceServiceDependency
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_NonWorkConservingSchedulingService.CIM_PacketSchedulingService
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_FailNextScheduler.CIM_NextScheduler
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_SchedulingElement.CIM_ManagedElement
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_AllocationSchedulingElement.CIM_SchedulingElement
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_WRRSchedulingElement.CIM_SchedulingElement
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_PrioritySchedulingElement.CIM_SchedulingElement
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_QueueToSchedule.#
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_BoundedPrioritySchedulingElement.CIM_PrioritySchedulingElement
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_SchedulingServiceToSchedule.#
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_ElementInSchedulingService.CIM_Component
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_SecurityAssociationEndpoint.CIM_ProtocolEndpoint
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_FilterOfSecurityAssociation.CIM_Dependency
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_SAEndpointConnectionStatistics.CIM_StatisticalData
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_SAEndpointRefreshSettings.CIM_SettingData
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_IPsecSAEndpoint.CIM_SecurityAssociationEndpoint
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_PeerOfSAEndpoint.CIM_Dependency
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_IKESAEndpoint.CIM_SecurityAssociationEndpoint
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_Phase1SAUsedForPhase2.CIM_Dependency
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_SATransform.CIM_ScopedSettingData
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_TransformOfSecurityAssociation.CIM_ElementSettingData
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_AHTransform.CIM_SATransform
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_ESPTransform.CIM_SATransform
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_IPCOMPTransform.CIM_SATransform
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_VLAN.CIM_ServiceAccessPoint
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_InboundVLAN.CIM_SAPSAPDependency
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_OutboundVLAN.CIM_SAPSAPDependency
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_VLANService.CIM_Service
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_Specific802dot1QVLANService.CIM_VLANService
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_VLANFor.CIM_ServiceSAPDependency
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_SwitchServiceVLAN.CIM_ServiceComponent
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_RelatedTransparentBridgingService.CIM_ServiceSAPDependency
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_DatabaseSystem.CIM_ApplicationSystem
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_CommonDatabase.CIM_EnabledLogicalElement
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_AssociatedDatabaseSystem.CIM_Dependency
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_DatabaseAdministrator.CIM_Dependency
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_DatabaseService.CIM_Service
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_ServiceAvailableToDatabase.CIM_ServiceAvailableToElement
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_DatabaseParameter.CIM_ScopedSettingData
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_SNMPDatabaseParameter.CIM_DatabaseParameter
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_DatabaseSegment.CIM_StorageExtent
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_DatabaseStorageArea.CIM_FileSystem
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_DatabaseFile.CIM_Component
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_DatabaseStorage.CIM_Component
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_DatabaseSegmentSettingData.CIM_ScopedSettingData
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_DatabaseControlFile.CIM_HostedDependency
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_CommonDatabaseSettingData.CIM_SettingData
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_CommonDatabaseCapabilities.CIM_Capabilities
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_DatabaseServiceStatistics.CIM_StatisticalData
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_CommonDatabaseStatistics.CIM_StatisticalData
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_DatabaseResourceStatistics.CIM_StatisticalData
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_DeviceConnection.CIM_Dependency
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_DeviceIdentity.CIM_LogicalIdentity
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_PackageDependency.CIM_Dependency
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_DeviceSoftware.CIM_Dependency
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_DeviceErrorCounts.CIM_StatisticalInformation
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_ErrorCountersForDevice.CIM_Statistics
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_DeviceErrorData.CIM_StatisticalData
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_CoolingDevice.CIM_LogicalDevice
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_Refrigeration.CIM_CoolingDevice
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_HeatPipe.CIM_CoolingDevice
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_Fan.CIM_CoolingDevice
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_AssociatedCooling.CIM_Dependency
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_Battery.CIM_LogicalDevice
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_AssociatedBattery.CIM_Dependency
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_PowerSupply.CIM_LogicalDevice
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_SuppliesPower.CIM_Dependency
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_UninterruptiblePowerSupply.CIM_PowerSupply
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_PackageCooling.CIM_PackageDependency
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_Watchdog.CIM_LogicalDevice
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_Processor.CIM_LogicalDevice
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_ComputerSystemProcessor.CIM_SystemDevice
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_Controller.CIM_LogicalDevice
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_ControlledBy.CIM_DeviceConnection
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_ESCONController.CIM_Controller
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_IDEController.CIM_Controller
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_InfraredController.CIM_Controller
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_ManagementController.CIM_Controller
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_ParallelController.CIM_Controller
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_PCMCIAController.CIM_Controller
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_SCSIController.CIM_Controller
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_SCSIInterface.CIM_ControlledBy
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_SerialController.CIM_Controller
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_SerialInterface.CIM_ControlledBy
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_SSAController.CIM_Controller
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_USBController.CIM_Controller
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_VideoController.CIM_Controller
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_PCVideoController.CIM_VideoController
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_AGPVideoController.CIM_VideoController
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_VideoControllerResolution.CIM_Setting
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_VideoSetting.CIM_ElementSetting
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_PCIController.CIM_Controller
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_PCIDevice.CIM_PCIController
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_PCIBridge.CIM_PCIController
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_PortController.CIM_Controller
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_LogicalPort.CIM_LogicalDevice
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_PortOnDevice.CIM_HostedDependency
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_LogicalPortGroup.CIM_SystemSpecificCollection
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_NetworkPort.CIM_LogicalPort
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_EthernetPort.CIM_NetworkPort
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_TokenRingPort.CIM_NetworkPort
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_OOBAlertServiceOnNetworkPort.CIM_DeviceServiceImplementation
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_WakeUpServiceOnNetworkPort.CIM_DeviceServiceImplementation
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_PortImplementsEndpoint.CIM_DeviceSAPImplementation
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_NetworkPortStatistics.CIM_StatisticalData
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_EthernetPortStatistics.CIM_NetworkPortStatistics
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_TokenRingPortStatistics.CIM_NetworkPortStatistics
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_LogicalModule.CIM_LogicalDevice
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_ModulePort.CIM_Component
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_PortActiveConnection.CIM_Dependency
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_ProtocolController.CIM_LogicalDevice
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_SCSIProtocolController.CIM_ProtocolController
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_ProtocolControllerForDevice.CIM_Dependency
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_AssociatedProtocolController.CIM_ProtocolControllerForDevice
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_ProtocolControllerForPort.CIM_ProtocolControllerForDevice
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_ProtocolControllerForUnit.CIM_ProtocolControllerForDevice
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_ProtocolControllerAccessesUnit.CIM_ProtocolControllerForDevice
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_ControllerConfigurationService.CIM_Service
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_ProtocolControllerMaskingCapabilities.CIM_Capabilities
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_NetworkAdapter.CIM_LogicalDevice
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_EthernetAdapter.CIM_NetworkAdapter
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_TokenRingAdapter.CIM_NetworkAdapter
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_OOBAlertServiceOnNetworkAdapter.CIM_DeviceServiceImplementation
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_WakeUpServiceOnNetworkAdapter.CIM_DeviceServiceImplementation
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_NetworkAdapterRedundancyComponent.CIM_RedundancyComponent
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_NetworkVirtualAdapter.CIM_LogicalIdentity
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_AdapterActiveConnection.CIM_Dependency
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_FibreChannelAdapter.CIM_NetworkAdapter
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_FibrePort.CIM_LogicalPort
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_FibrePortOnFCAdapter.CIM_PortOnDevice
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_FibrePortActiveLogin.#
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_FCAdapterEventCounters.CIM_DeviceStatisticalInformation
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_FibrePortEventCounters.CIM_DeviceStatisticalInformation
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_FCPort.CIM_NetworkPort
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_ZoneSet.CIM_ConnectivityCollection
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_Zone.CIM_ConnectivityCollection
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_ZoneMembershipSettingData.CIM_ConnectivityMembershipSettingData
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_ZoneService.CIM_Service
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_FibreProtocolService.CIM_Service
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_FCPortStatistics.CIM_NetworkPortStatistics
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_FCPortRateStatistics.CIM_StatisticalData
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_ZoneCapabilities.CIM_Capabilities
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_IBSubnetManager.CIM_Service
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_MediaAccessDevice.CIM_LogicalDevice
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_DiskDrive.CIM_MediaAccessDevice
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_DisketteDrive.CIM_MediaAccessDevice
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_CDROMDrive.CIM_MediaAccessDevice
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_DVDDrive.CIM_MediaAccessDevice
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_WORMDrive.CIM_MediaAccessDevice
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_MagnetoOpticalDrive.CIM_MediaAccessDevice
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_TapeDrive.CIM_MediaAccessDevice
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_MediaAccessStatInfo.CIM_DeviceStatisticalInformation
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_MediaAccessStatData.CIM_StatisticalData
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_MediaPresent.CIM_Dependency
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_RealizesExtent.CIM_Realizes
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_RealizedOnSide.CIM_RealizesExtent
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_StorageVolume.CIM_StorageExtent
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_MediaPartition.CIM_StorageExtent
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_DiskPartition.CIM_MediaPartition
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_DiskPartitionBasedOnVolume.CIM_BasedOn
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_RealizesDiskPartition.CIM_RealizesExtent
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_TapePartition.CIM_MediaPartition
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_TapePartitionOnSurface.CIM_BasedOn
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_RealizesTapePartition.CIM_RealizesExtent
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_LogicalDisk.CIM_StorageExtent
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_LogicalDiskBasedOnExtent.CIM_BasedOn
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_LogicalDiskBasedOnVolume.CIM_LogicalDiskBasedOnExtent
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_LogicalDiskBasedOnPartition.CIM_LogicalDiskBasedOnExtent
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_ProtectedExtentBasedOn.CIM_BasedOn
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_CompositeExtent.CIM_StorageExtent
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_CompositeExtentBasedOn.CIM_BasedOn
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_StorageError.CIM_ManagedElement
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_StorageDefect.#
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_PhysicalExtent.CIM_StorageExtent
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_ProtectedSpaceExtent.CIM_StorageExtent
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_PSExtentBasedOnPExtent.CIM_BasedOn
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_VolumeSet.CIM_StorageVolume
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_VolumeSetBasedOnPSExtent.CIM_BasedOn
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_LogicalDiskBasedOnVolumeSet.CIM_LogicalDiskBasedOnVolume
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_AggregatePExtent.CIM_StorageExtent
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_AggregatePSExtent.CIM_StorageExtent
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_AggregatePSExtentBasedOnPExtent.CIM_BasedOn
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_AggregatePSExtentBasedOnAggregatePExtent.CIM_BasedOn
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_PExtentRedundancyComponent.CIM_ExtentRedundancyComponent
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_AggregateRedundancyComponent.CIM_ExtentRedundancyComponent
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_RealizesPExtent.CIM_RealizesExtent
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_RealizesAggregatePExtent.CIM_RealizesExtent
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_Snapshot.CIM_StorageExtent
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_SnapshotOfExtent.CIM_Dependency
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_StorageSynchronized.CIM_Synchronized
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_StorageSetting.CIM_SettingData
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_StoragePool.CIM_LogicalElement
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_StorageCapabilities.CIM_Capabilities
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_StorageConfigurationService.CIM_Service
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_StorageConfigurationCapabilities.CIM_Capabilities
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_StorageSettingWithHints.CIM_StorageSetting
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_AllocatedFromStoragePool.CIM_Dependency
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_HostedStoragePool.CIM_SystemComponent
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_ConfigurationReportingService.CIM_Service
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_StorageLibrary.CIM_System
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_LibraryPackage.CIM_SystemPackaging
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_MediaTransferDevice.CIM_LogicalDevice
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_DeviceServicesLocation.CIM_Dependency
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_PickerElement.CIM_MediaTransferDevice
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_ChangerDevice.CIM_MediaTransferDevice
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_PickerForChanger.CIM_Dependency
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_LimitedAccessPort.CIM_MediaTransferDevice
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_InterLibraryPort.CIM_MediaTransferDevice
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_LibraryExchange.CIM_Dependency
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_LabelReader.CIM_LogicalDevice
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_AssociatedLabelReader.CIM_Dependency
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_PickerLabelReader.CIM_AssociatedLabelReader
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_AccessLabelReader.CIM_AssociatedLabelReader
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_LabelReaderStatInfo.CIM_DeviceStatisticalInformation
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_PickerStatInfo.CIM_DeviceStatisticalInformation
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_LabelReaderStatData.CIM_StatisticalData
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_PickerStatData.CIM_StatisticalData
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_UserDevice.CIM_LogicalDevice
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_PointingDevice.CIM_UserDevice
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_Keyboard.CIM_UserDevice
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_Display.CIM_UserDevice
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_DesktopMonitor.CIM_Display
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_FlatPanel.CIM_Display
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_Scanner.CIM_LogicalDevice
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_Door.CIM_LogicalDevice
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_DoorAccessToPhysicalElement.CIM_Dependency
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_DoorAccessToDevice.CIM_Dependency
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_MonitorResolution.CIM_Setting
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_MonitorSetting.CIM_ElementSetting
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_Memory.CIM_StorageExtent
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_AssociatedMemory.CIM_Dependency
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_MemoryError.CIM_StorageError
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_ComputerSystemMemory.CIM_SystemDevice
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_AssociatedProcessorMemory.CIM_AssociatedMemory
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_AssociatedCacheMemory.CIM_AssociatedMemory
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_NonVolatileStorage.CIM_Memory
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_BIOSLoadedInNV.CIM_Dependency
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_VolatileStorage.CIM_Memory
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_CacheMemory.CIM_Memory
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_Modem.CIM_LogicalDevice
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_ConnectionBasedModem.CIM_Modem
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_DSLModem.CIM_ConnectionBasedModem
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_ADSLModem.CIM_DSLModem
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_CableModem.CIM_ConnectionBasedModem
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_HDSLModem.CIM_DSLModem
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_SDSLModem.CIM_DSLModem
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_VDSLModem.CIM_DSLModem
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_CallBasedModem.CIM_Modem
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_ISDNModem.CIM_CallBasedModem
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_POTSModem.CIM_CallBasedModem
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_Unimodem.CIM_POTSModem
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_OOBAlertServiceOnModem.CIM_DeviceServiceImplementation
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_WakeUpServiceOnModem.CIM_DeviceServiceImplementation
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_Printer.CIM_LogicalDevice
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_PrintQueue.CIM_JobDestination
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_PrintJob.CIM_Job
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_PrintSAP.CIM_ServiceAccessPoint
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_PrintService.CIM_Service
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_PrinterServicingQueue.CIM_Dependency
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_PrinterServicingJob.CIM_Dependency
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_PrintJobFile.CIM_Component
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_QueueForPrintService.CIM_Dependency
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_QueueForwardsToPrintSAP.CIM_Dependency
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_OwningPrintQueue.CIM_JobDestinationJobs
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_Sensor.CIM_LogicalDevice
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_AssociatedSensor.CIM_Dependency
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_BinarySensor.CIM_Sensor
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_MultiStateSensor.CIM_Sensor
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_CollectionOfSensors.CIM_Component
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_NumericSensor.CIM_Sensor
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_DiscreteSensor.CIM_Sensor
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_TemperatureSensor.CIM_NumericSensor
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_CurrentSensor.CIM_NumericSensor
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_VoltageSensor.CIM_NumericSensor
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_Tachometer.CIM_NumericSensor
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_AssociatedSupplyVoltageSensor.CIM_AssociatedSensor
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_AssociatedSupplyCurrentSensor.CIM_AssociatedSensor
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_PackageTempSensor.CIM_PackageDependency
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_AlarmDevice.CIM_LogicalDevice
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_AssociatedAlarm.CIM_Dependency
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_PackageAlarm.CIM_PackageDependency
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_USBDevice.CIM_LogicalDevice
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_USBHub.CIM_USBDevice
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_USBPort.CIM_LogicalPort
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_USBPortOnHub.CIM_PortOnDevice
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_USBControllerHasHub.CIM_ControlledBy
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_USBConnection.CIM_DeviceConnection
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_DiskGroup.CIM_CollectionOfMSEs
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_DriveInDiskGroup.CIM_CollectedMSEs
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_ExtentInDiskGroup.CIM_CollectedMSEs
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_GroupInDiskGroup.CIM_CollectedCollections
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_Policy.CIM_ManagedElement
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_PolicySet.CIM_Policy
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_PolicyGroup.CIM_PolicySet
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_PolicyRule.CIM_PolicySet
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_AuthenticationRule.CIM_PolicyRule
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_ReusablePolicyContainer.CIM_AdminDomain
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_PolicyRepository.CIM_AdminDomain
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_PolicyCondition.CIM_Policy
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_PolicyTimePeriodCondition.CIM_PolicyCondition
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_CompoundPolicyCondition.CIM_PolicyCondition
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_AuthenticationCondition.CIM_PolicyCondition
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_SharedSecretAuthentication.CIM_AuthenticationCondition
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_AccountAuthentication.CIM_AuthenticationCondition
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_BiometricAuthentication.CIM_AuthenticationCondition
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_NetworkingIDAuthentication.CIM_AuthenticationCondition
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_PublicPrivateKeyAuthentication.CIM_AuthenticationCondition
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_KerberosAuthentication.CIM_AuthenticationCondition
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_DocumentAuthentication.CIM_AuthenticationCondition
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_PhysicalCredentialAuthentication.CIM_AuthenticationCondition
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_VendorPolicyCondition.CIM_PolicyCondition
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_PacketFilterCondition.CIM_PolicyCondition
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_PolicyAction.CIM_Policy
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_VendorPolicyAction.CIM_PolicyAction
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_CompoundPolicyAction.CIM_PolicyAction
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_NetworkPacketAction.CIM_PolicyAction
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_RejectConnectionAction.CIM_PolicyAction
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_PolicyRoleCollection.CIM_SystemSpecificCollection
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_PolicyComponent.CIM_Component
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_PolicyInSystem.CIM_HostedDependency
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_PolicySetInSystem.CIM_PolicyInSystem
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_PolicyGroupInSystem.CIM_PolicySetInSystem
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_PolicyRuleInSystem.CIM_PolicySetInSystem
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_PolicySetComponent.CIM_PolicyComponent
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_PolicyGroupInPolicyGroup.CIM_PolicyComponent
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_PolicyRuleInPolicyGroup.CIM_PolicyComponent
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_PolicySetValidityPeriod.CIM_PolicyComponent
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_PolicyRuleValidityPeriod.CIM_PolicyComponent
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_PolicyConditionStructure.CIM_PolicyComponent
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_PolicyConditionInPolicyRule.CIM_PolicyConditionStructure
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_PolicyConditionInPolicyCondition.CIM_PolicyConditionStructure
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_PolicyActionStructure.CIM_PolicyComponent
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_PolicyActionInPolicyRule.CIM_PolicyActionStructure
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_PolicyActionInPolicyAction.CIM_PolicyActionStructure
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_PolicyContainerInPolicyContainer.CIM_SystemComponent
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_PolicyRepositoryInPolicyRepository.CIM_SystemComponent
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_ReusablePolicy.CIM_PolicyInSystem
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_ElementInPolicyRoleCollection.CIM_MemberOfCollection
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_PolicyRoleCollectionInSystem.CIM_HostedCollection
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_PolicyConditionInPolicyRepository.CIM_PolicyInSystem
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_PolicyActionInPolicyRepository.CIM_PolicyInSystem
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_PolicySetInRoleCollection.CIM_MemberOfCollection
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_PolicySetAppliesToElement.#
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_FilterOfPacketCondition.CIM_Dependency
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_AcceptCredentialFrom.CIM_Dependency
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_SARule.CIM_PolicyRule
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_RuleThatGeneratedSA.CIM_Dependency
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_IKERule.CIM_SARule
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_IPsecRule.CIM_SARule
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_IPsecPolicyForSystem.CIM_Dependency
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_IPsecPolicyForEndpoint.CIM_Dependency
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_PacketConditionInSARule.CIM_PolicyConditionInPolicyRule
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_SAAction.CIM_PolicyAction
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_SAStaticAction.CIM_SAAction
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_PreconfiguredSAAction.CIM_SAStaticAction
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_TransformOfPreconfiguredAction.CIM_Dependency
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_PreconfiguredTransportAction.CIM_PreconfiguredSAAction
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_PreconfiguredTunnelAction.CIM_PreconfiguredSAAction
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_PeerGatewayForPreconfiguredTunnel.CIM_Dependency
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_SANegotiationAction.CIM_SAAction
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_IKEAction.CIM_SANegotiationAction
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_IPsecAction.CIM_SANegotiationAction
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_IPsecTransportAction.CIM_IPsecAction
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_IPsecTunnelAction.CIM_IPsecAction
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_PeerGatewayForTunnel.CIM_Dependency
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_SAProposal.CIM_ScopedSettingData
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_ContainedProposal.CIM_Component
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_IKEProposal.CIM_SAProposal
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_IPsecProposal.CIM_SAProposal
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/CIM_ContainedTransform.CIM_Component
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/PRS_ExchangeElement.CIM_ManagedElement
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/PRS_Solution.PRS_ExchangeElement
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/PRS_SolutionElement.PRS_ExchangeElement
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/PRS_Reference.PRS_SolutionElement
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/PRS_Problem.PRS_SolutionElement
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/PRS_Resolution.PRS_SolutionElement
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/PRS_SolutionHasElement.CIM_Dependency
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/PRS_SolutionReference.PRS_SolutionHasElement
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/PRS_SolutionProblem.PRS_SolutionHasElement
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/PRS_SolutionResolution.PRS_SolutionHasElement
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/PRS_Category.PRS_ExchangeElement
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/PRS_SolutionCategory.CIM_Dependency
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/PRS_Categorization.CIM_Dependency
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/PRS_CategoryParentChild.CIM_HostedDependency
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/PRS_ExpressionElement.PRS_ExchangeElement
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/PRS_Expression.PRS_ExpressionElement
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/PRS_Product.PRS_ExpressionElement
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/PRS_Statement.PRS_ExpressionElement
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/PRS_Feature.PRS_ExchangeElement
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/PRS_Resource.PRS_ExchangeElement
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/PRS_ExpressionLink.CIM_HostedDependency
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/PRS_ProductParentChild.CIM_Dependency
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/PRS_ProductComponent.CIM_Dependency
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/PRS_ProductAsset.CIM_Dependency
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/PRS_StatementFeature.CIM_Dependency
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/PRS_FeatureResource.CIM_Dependency
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/PRS_ResolutionResource.CIM_Dependency
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/PRS_SolutionExpression.CIM_Dependency
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/PRS_Administrative.PRS_ExchangeElement
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/PRS_AdminAssociation.CIM_Dependency
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/PRS_Revision.PRS_ExchangeElement
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/PRS_AdministrativeRevision.CIM_HostedDependency
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/PRS_Contact.PRS_ExchangeElement
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/PRS_ContactItem.PRS_ExchangeElement
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/PRS_AdministrativeContact.CIM_Dependency
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/PRS_RevisionContact.CIM_Dependency
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/PRS_Organization.PRS_ContactItem
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/PRS_Person.PRS_ContactItem
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/PRS_ContactContactItem.CIM_Dependency
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/PRS_ContactOrganization.PRS_ContactContactItem
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/PRS_ContactPerson.PRS_ContactContactItem
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/PRS_Address.PRS_ExchangeElement
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/PRS_ContactItemAddress.CIM_Dependency
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/PRS_Location.PRS_ExchangeElement
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/PRS_AddressLocation.CIM_Dependency
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/PRS_Attachment.PRS_ExchangeElement
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/PRS_Attached.CIM_Dependency
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/PRS_ServiceIncident.PRS_ExchangeElement
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/PRS_ServiceRequester.CIM_Dependency
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/PRS_ServiceProvider.CIM_Dependency
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/PRS_ServiceProblem.CIM_Dependency
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/PRS_ServiceResolutionSolution.CIM_Dependency
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/PRS_Activity.PRS_ExchangeElement
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/PRS_Agreement.PRS_ExchangeElement
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/PRS_ServiceAgreement.CIM_Dependency
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/PRS_Transaction.PRS_ExchangeElement
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/PRS_SISService.CIM_Service
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/PRS_ServiceActivity.CIM_Dependency
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/PRS_ActivityContact.CIM_Dependency
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/PRS_ActivityResource.CIM_Dependency
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/PRS_ActivityTransaction.CIM_Dependency
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/PG_OperatingSystem.CIM_OperatingSystem
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/PG_ComputerSystem.CIM_UnitaryComputerSystem
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/PG_UnixProcess.CIM_Process
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/PG_UnixProcessStatisticalInformation.CIM_StatisticalInformation
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/PG_UnixProcessStatistics.CIM_Statistics
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/PG_UnixThread.CIM_Thread
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/PG_Processor.CIM_Processor
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/PG_BindsIPToLANEndpoint.CIM_BindsToLANEndpoint
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/classes/PG_IPRoute.CIM_IPRoute
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/qualifiers/Abstract
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/qualifiers/Aggregate
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/qualifiers/Aggregation
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/qualifiers/Alias
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/qualifiers/ArrayType
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/qualifiers/ASSOCIATION
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/qualifiers/BitMap
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/qualifiers/BitValues
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/qualifiers/Composition
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/qualifiers/Counter
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/qualifiers/Delete
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/qualifiers/Deprecated
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/qualifiers/Description
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/qualifiers/DisplayName
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/qualifiers/DN
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/qualifiers/EmbeddedObject
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/qualifiers/Exception
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/qualifiers/Expensive
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/qualifiers/Experimental
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/qualifiers/Gauge
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/qualifiers/Ifdeleted
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/qualifiers/In
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/qualifiers/INDICATION
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/qualifiers/Invisible
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/qualifiers/Key
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/qualifiers/Large
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/qualifiers/MappingStrings
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/qualifiers/Max
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/qualifiers/MaxLen
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/qualifiers/MaxValue
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/qualifiers/Min
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/qualifiers/MinLen
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/qualifiers/MinValue
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/qualifiers/ModelCorrespondence
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/qualifiers/Nonlocal
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/qualifiers/NonlocalType
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/qualifiers/NullValue
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/qualifiers/Octetstring
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/qualifiers/Out
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/qualifiers/Override
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/qualifiers/Propagated
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/qualifiers/PropertyUsage
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/qualifiers/Provider
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/qualifiers/Read
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/qualifiers/Required
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/qualifiers/Revision
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/qualifiers/CLASS
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/qualifiers/Source
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/qualifiers/SourceType
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/qualifiers/Static
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/qualifiers/Syntax
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/qualifiers/SyntaxType
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/qualifiers/Terminal
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/qualifiers/TriggerType
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/qualifiers/Units
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/qualifiers/UnknownValues
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/qualifiers/UnsupportedValues
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/qualifiers/ValueMap
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/qualifiers/Values
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/qualifiers/Version
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/qualifiers/Weak
%attr(-,root,root) %PEGASUS_REPOSITORY_DIR/root#cimv2/qualifiers/Write

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
%attr(-,root,root) %PEGASUS_INCLUDE_DEST_PATH/Pegasus/Common/Platform_LINUX_IX86_GNU.h
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
