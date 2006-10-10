#%2005////////////////////////////////////////////////////////////////////////
#
# Copyright (c) 2000, 2001, 2002 BMC Software; Hewlett-Packard Development
# Company, L.P.; IBM Corp.; The Open Group; Tivoli Systems.
# Copyright (c) 2003 BMC Software; Hewlett-Packard Development Company, L.P.;
# IBM Corp.; EMC Corporation, The Open Group.
# Copyright (c) 2004 BMC Software; Hewlett-Packard Development Company, L.P.;
# IBM Corp.; EMC Corporation; VERITAS Software Corporation; The Open Group.
# Copyright (c) 2005 Hewlett-Packard Development Company, L.P.; IBM Corp.;
# EMC Corporation; VERITAS Software Corporation; The Open Group.
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

%{?!PEGASUS_BUILD_TEST_RPM:   %define PEGASUS_BUILD_TEST_RPM        0}
# do "rpmbuild --define 'PEGASUS_BUILD_TEST_RPM 1'" to build test RPM .

%define srcRelease 1
Summary: OpenPegasus WBEM Services for Linux
Name: tog-pegasus
Version: 2.4.4
Release: 1%{?LINUX_VERSION:.%{LINUX_VERSION}}
Group: Systems Management/Base
Copyright: Open Group Pegasus Open Source
BuildRoot: %{_tmppath}/%{name}-%{version}-%{release}
Source: ftp://www.opengroup.org/pegasus/tog-pegasus-%{version}-%{srcRelease}.tar.gz
Requires: openssl >= 0.9.6 lsb >= 1.3
Conflicts: openwbem
Provides: cimserver tog-pegasus-2.4
BuildRequires: openssl-devel >= 0.9.6
BuildConflicts: tog-pegasus

%description
OpenPegasus WBEM Services for Linux enables management solutions that deliver
increased control of enterprise resources. WBEM is a platform and resource
independent DMTF standard that defines a common information model and
communication protocol for monitoring and controlling resources from diverse
sources.

%package devel
Summary:      The OpenPegasus Software Development Kit
Group:        Systems Management/Base
Autoreq: 0
Requires: tog-pegasus >= 2.4
Obsoletes: tog-pegasus-sdk

%description devel
The OpenPegasus WBEM Services for Linux SDK is the developer's kit for the OpenPegasus WBEM
Services for Linux release. It provides Linux C++ developers with the WBEM files required to
build WBEM Clients and Providers. It also supports C provider developers via the CMPI interface.

%if %{PEGASUS_BUILD_TEST_RPM}
%package test
Summary:      The OpenPegasus Tests
Group:        Systems Management/Base
Autoreq: 0
Requires: tog-pegasus >= 2.4

%description test
The OpenPegasus WBEM tests for the OpenPegasus 2.4 Linux rpm.
%endif

%prep
[ "$RPM_BUILD_ROOT" != "/" ] && [ -d $RPM_BUILD_ROOT ] && rm -rf $RPM_BUILD_ROOT;

%setup -n %{name}-%{version}
export PEGASUS_ROOT=$RPM_BUILD_DIR/$RPM_PACKAGE_NAME-$RPM_PACKAGE_VERSION

# Needed for CMPI patch
ln -s $RPM_BUILD_DIR/$RPM_PACKAGE_NAME-$RPM_PACKAGE_VERSION $RPM_BUILD_DIR/$RPM_PACKAGE_NAME

%build
export PEGASUS_EXTRA_C_FLAGS="-g $RPM_OPT_FLAGS"
export PEGASUS_EXTRA_CXX_FLAGS="$PEGASUS_EXTRA_C_FLAGS"
export PEGASUS_ROOT=$RPM_BUILD_DIR/$RPM_PACKAGE_NAME-$RPM_PACKAGE_VERSION
export PEGASUS_HOME=$RPM_BUILD_ROOT/usr/pegasus
%ifarch ia64 x86_64
%define PEGASUS_HARDWARE_PLATFORM LINUX_IA64_GNU
%else
%ifarch ppc
%define PEGASUS_HARDWARE_PLATFORM LINUX_PPC_GNU
%else
%ifarch ppc64 pseries
%define PEGASUS_HARDWARE_PLATFORM LINUX_PPC64_GNU
%else
%ifarch s390
%define PEGASUS_HARDWARE_PLATFORM LINUX_ZSERIES_GNU
export  PEGASUS_EXTRA_C_FLAGS="$PEGASUS_EXTRA_C_FLAGS -fsigned-char"
%else
%ifarch s390x zseries
%define PEGASUS_HARDWARE_PLATFORM LINUX_ZSERIES64_GNU
export  PEGASUS_EXTRA_C_FLAGS="$PEGASUS_EXTRA_C_FLAGS -fsigned-char"
%else
%define PEGASUS_HARDWARE_PLATFORM LINUX_IX86_GNU
%endif
%endif
%endif
%endif
%endif
export PEGASUS_PLATFORM=%PEGASUS_HARDWARE_PLATFORM

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

%define PEGASUS_STAGING_DIR  $PEGASUS_HOME/stageSDK
%define PEGASUS_SAMPLES_DIR  /opt/tog-pegasus/samples 
%define PEGASUS_INCLUDE_DIR  /opt/tog-pegasus/include
%define PEGASUS_HTML_DIR     /opt/tog-pegasus/html

make --directory=mak -f SDKMakefile stageSDK \
          PEGASUS_STAGING_DIR=%PEGASUS_STAGING_DIR \
          PEGASUS_SAMPLES_DIR=%PEGASUS_SAMPLES_DIR \
          PEGASUS_INCLUDE_DIR=%PEGASUS_INCLUDE_DIR \
          PEGASUS_HTML_DIR=%PEGASUS_HTML_DIR

%if %{PEGASUS_BUILD_TEST_RPM}
make --directory=$PEGASUS_ROOT -f Makefile.ReleaseTest stageTEST \
	PEGASUS_ENVVAR_FILE=$PEGASUS_ROOT/env_var_Linux.status
%endif

%install
%define PEGASUS_PROD_DIR       /opt/tog-pegasus
%define PEGASUS_DEST_LIB_DIR   %PEGASUS_PROD_DIR/lib
%define PEGASUS_PROVIDER_DIR   %PEGASUS_PROD_DIR/providers
%define PEGASUS_PROVIDER_LIB_DIR    %PEGASUS_PROVIDER_DIR/lib
%define PEGASUS_SBIN_DIR       %PEGASUS_PROD_DIR/sbin
%define PEGASUS_BIN_DIR        %PEGASUS_PROD_DIR/bin
%define PEGASUS_PRODSHARE_DIR  %PEGASUS_PROD_DIR/share
%define PEGASUS_DOC_DIR        %PEGASUS_PRODSHARE_DIR/doc
%define PEGASUS_MAN_DIR        %PEGASUS_PRODSHARE_DIR/man
%define PEGASUS_MANUSER_DIR    %PEGASUS_MAN_DIR/man1
%define PEGASUS_MANADMIN_DIR   %PEGASUS_MAN_DIR/man8
%define PEGASUS_VARDATA_DIR    /var/opt/tog-pegasus
%define PEGASUS_REPOSITORY_DIR      %PEGASUS_VARDATA_DIR/repository
%define PEGASUS_NEW_REPOSITORY_DIR  %PEGASUS_VARDATA_DIR/new_repository
%define PEGASUS_PREV_REPOSITORY_DIR %PEGASUS_VARDATA_DIR/prev_repository
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
%define PEGASUS_INSTALL_SCRIPT_DIR $PEGASUS_ROOT/installs/scripts

%if %{PEGASUS_BUILD_TEST_RPM}
%define PEGASUS_TEST_DIR  /opt/tog-pegasus/test
%define PEGASUS_TEST_STAGING_DIR  $PEGASUS_HOME/stagingDir
%define PEGASUS_TEST_BIN_DIR  %PEGASUS_TEST_DIR/bin
%define PEGASUS_TEST_LIB_DIR  %PEGASUS_TEST_DIR/lib
%define PEGASUS_TEST_MAK_DIR  %PEGASUS_TEST_DIR/mak
%endif

#
# Make directories
mkdir -p $RPM_BUILD_ROOT%PEGASUS_VARDATA_DIR/{log,cache}
mkdir -p $RPM_BUILD_ROOT/%PEGASUS_LOCAL_AUTH_DIR
mkdir -p $RPM_BUILD_ROOT/%PEGASUS_PROVIDER_LIB_DIR
mkdir -p $RPM_BUILD_ROOT/%PEGASUS_CONFIG_DIR 
mkdir -p $RPM_BUILD_ROOT/%PEGASUS_LOG_DIR
mkdir -p $RPM_BUILD_ROOT/%PEGASUS_DOC_DIR
mkdir -p $RPM_BUILD_ROOT/%PEGASUS_MAN_DIR/{man1,man8}
mkdir -p $RPM_BUILD_ROOT%PEGASUS_MOF_DIR/{CIM28,Pegasus}
mkdir -p $RPM_BUILD_ROOT%PEGASUS_LOCAL_DOMAIN_SOCKET_DIR
mkdir -p $RPM_BUILD_ROOT%PEGASUS_REPOSITORY_DIR

export PEGASUS_ROOT=$RPM_BUILD_DIR/$RPM_PACKAGE_NAME-$RPM_PACKAGE_VERSION
export PEGASUS_HOME=$RPM_BUILD_ROOT/usr/pegasus

#
# Init scripts
install -D -m 0755  $PEGASUS_ROOT/rpm/tog-pegasus.rc $RPM_BUILD_ROOT/etc/init.d/tog-pegasus

#
# Programs
install -D -m 0744  $PEGASUS_HOME/bin/cimauth   $RPM_BUILD_ROOT%PEGASUS_SBIN_DIR/cimauth
install -D -m 0744  $PEGASUS_HOME/bin/cimserver $RPM_BUILD_ROOT%PEGASUS_SBIN_DIR/cimserver
install -D -m 0744  $PEGASUS_HOME/bin/cimservera $RPM_BUILD_ROOT%PEGASUS_SBIN_DIR/cimservera
install -D -m 0744  $PEGASUS_HOME/bin/cimuser   $RPM_BUILD_ROOT%PEGASUS_SBIN_DIR/cimuser
install -D -m 0744  $PEGASUS_HOME/bin/cimconfig $RPM_BUILD_ROOT%PEGASUS_SBIN_DIR/cimconfig
install -D -m 0744  $PEGASUS_HOME/bin/cimprovagt $RPM_BUILD_ROOT%PEGASUS_SBIN_DIR/cimprovagt
install -D -m 0755  $PEGASUS_HOME/bin/cimmof    $RPM_BUILD_ROOT%PEGASUS_BIN_DIR/cimmof
install -D -m 0755  $PEGASUS_HOME/bin/cimmofl   $RPM_BUILD_ROOT%PEGASUS_BIN_DIR/cimmofl
install -D -m 0755  $PEGASUS_HOME/bin/cimprovider $RPM_BUILD_ROOT%PEGASUS_BIN_DIR/cimprovider
install -D -m 0755  $PEGASUS_HOME/bin/osinfo    $RPM_BUILD_ROOT%PEGASUS_BIN_DIR/osinfo
install -D -m 0755  $PEGASUS_HOME/bin/wbemexec  $RPM_BUILD_ROOT%PEGASUS_BIN_DIR/wbemexec


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

touch $RPM_BUILD_ROOT%PEGASUS_DEST_LIB_DIR/libpegcommon.so
touch $RPM_BUILD_ROOT%PEGASUS_DEST_LIB_DIR/libpegclient.so
touch $RPM_BUILD_ROOT%PEGASUS_DEST_LIB_DIR/libpegprovider.so
touch $RPM_BUILD_ROOT%PEGASUS_DEST_LIB_DIR/libDefaultProviderManager.so
touch $RPM_BUILD_ROOT%PEGASUS_DEST_LIB_DIR/libCMPIProviderManager.so
touch $RPM_BUILD_ROOT%PEGASUS_DEST_LIB_DIR/libCIMxmlIndicationHandler.so

install -D -m 0755  $PEGASUS_HOME/lib/libComputerSystemProvider.so.1    $RPM_BUILD_ROOT%PEGASUS_PROVIDER_LIB_DIR/libComputerSystemProvider.so.1
install -D -m 0755  $PEGASUS_HOME/lib/libOSProvider.so.1    $RPM_BUILD_ROOT%PEGASUS_PROVIDER_LIB_DIR/libOSProvider.so.1
install -D -m 0755  $PEGASUS_HOME/lib/libProcessProvider.so.1    $RPM_BUILD_ROOT%PEGASUS_PROVIDER_LIB_DIR/libProcessProvider.so.1

touch $RPM_BUILD_ROOT%PEGASUS_PROVIDER_LIB_DIR/libComputerSystemProvider.so
touch $RPM_BUILD_ROOT%PEGASUS_PROVIDER_LIB_DIR/libOSProvider.so
touch $RPM_BUILD_ROOT%PEGASUS_PROVIDER_LIB_DIR/libProcessProvider.so
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
chmod 0744 $RPM_BUILD_ROOT%PEGASUS_SBIN_DIR/init_repository

#
# script to add tog-pegasus paths to /etc/profile
#
install -D -m 0744 %PEGASUS_INSTALL_SCRIPT_DIR/settogpath $RPM_BUILD_ROOT%PEGASUS_SBIN_DIR/settogpath

#
# doc files
#
install -D -m 0444  $PEGASUS_ROOT/doc/Admin_Guide_Release_2.4.pdf $RPM_BUILD_ROOT%PEGASUS_DOC_DIR/Admin_Guide_Release_2.4.pdf

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

%define INCLUDE_DEST_PATH    $RPM_BUILD_ROOT%PEGASUS_INCLUDE_DIR
%define HTML_DEST_PATH       $RPM_BUILD_ROOT%PEGASUS_HTML_DIR
%define SAMPLES_DEST_PATH     $RPM_BUILD_ROOT%PEGASUS_SAMPLES_DIR
mkdir -p %INCLUDE_DEST_PATH/Pegasus/{Client,Common,Consumer,Handler,Provider}
mkdir -p %HTML_DEST_PATH
mkdir -p %SAMPLES_DEST_PATH/{Clients,mak,Providers}
mkdir -p %SAMPLES_DEST_PATH/Providers/Load
install -D -m 0444 %PEGASUS_STAGING_DIR%PEGASUS_INCLUDE_DIR/Pegasus/Client/CIMClientException.h %INCLUDE_DEST_PATH/Pegasus/Client/CIMClientException.h
install -D -m 0444 %PEGASUS_STAGING_DIR%PEGASUS_INCLUDE_DIR/Pegasus/Client/CIMClient.h %INCLUDE_DEST_PATH/Pegasus/Client/CIMClient.h
install -D -m 0444 %PEGASUS_STAGING_DIR%PEGASUS_INCLUDE_DIR/Pegasus/Client/Linkage.h %INCLUDE_DEST_PATH/Pegasus/Client/Linkage.h
install -D -m 0444 %PEGASUS_STAGING_DIR%PEGASUS_INCLUDE_DIR/Pegasus/Common/AcceptLanguages.h %INCLUDE_DEST_PATH/Pegasus/Common/AcceptLanguages.h
install -D -m 0444 %PEGASUS_STAGING_DIR%PEGASUS_INCLUDE_DIR/Pegasus/Common/AcceptLanguageElement.h %INCLUDE_DEST_PATH/Pegasus/Common/AcceptLanguageElement.h
install -D -m 0444 %PEGASUS_STAGING_DIR%PEGASUS_INCLUDE_DIR/Pegasus/Common/Array.h %INCLUDE_DEST_PATH/Pegasus/Common/Array.h
install -D -m 0444 %PEGASUS_STAGING_DIR%PEGASUS_INCLUDE_DIR/Pegasus/Common/ArrayInter.h %INCLUDE_DEST_PATH/Pegasus/Common/ArrayInter.h
install -D -m 0444 %PEGASUS_STAGING_DIR%PEGASUS_INCLUDE_DIR/Pegasus/Common/Char16.h %INCLUDE_DEST_PATH/Pegasus/Common/Char16.h
install -D -m 0444 %PEGASUS_STAGING_DIR%PEGASUS_INCLUDE_DIR/Pegasus/Common/CIMClass.h %INCLUDE_DEST_PATH/Pegasus/Common/CIMClass.h 
install -D -m 0444 %PEGASUS_STAGING_DIR%PEGASUS_INCLUDE_DIR/Pegasus/Common/CIMDateTime.h %INCLUDE_DEST_PATH/Pegasus/Common/CIMDateTime.h
install -D -m 0444 %PEGASUS_STAGING_DIR%PEGASUS_INCLUDE_DIR/Pegasus/Common/CIMFlavor.h %INCLUDE_DEST_PATH/Pegasus/Common/CIMFlavor.h
install -D -m 0444 %PEGASUS_STAGING_DIR%PEGASUS_INCLUDE_DIR/Pegasus/Common/CIMIndication.h %INCLUDE_DEST_PATH/Pegasus/Common/CIMIndication.h 
install -D -m 0444 %PEGASUS_STAGING_DIR%PEGASUS_INCLUDE_DIR/Pegasus/Common/CIMInstance.h %INCLUDE_DEST_PATH/Pegasus/Common/CIMInstance.h
install -D -m 0444 %PEGASUS_STAGING_DIR%PEGASUS_INCLUDE_DIR/Pegasus/Common/CIMMethod.h %INCLUDE_DEST_PATH/Pegasus/Common//CIMMethod.h
install -D -m 0444 %PEGASUS_STAGING_DIR%PEGASUS_INCLUDE_DIR/Pegasus/Common/CIMName.h %INCLUDE_DEST_PATH/Pegasus/Common/CIMName.h
install -D -m 0444 %PEGASUS_STAGING_DIR%PEGASUS_INCLUDE_DIR/Pegasus/Common/CIMObject.h %INCLUDE_DEST_PATH/Pegasus/Common/CIMObject.h
install -D -m 0444 %PEGASUS_STAGING_DIR%PEGASUS_INCLUDE_DIR/Pegasus/Common/CIMObjectPath.h %INCLUDE_DEST_PATH/Pegasus/Common/CIMObjectPath.h
install -D -m 0444 %PEGASUS_STAGING_DIR%PEGASUS_INCLUDE_DIR/Pegasus/Common/CIMParameter.h %INCLUDE_DEST_PATH/Pegasus/Common/CIMParameter.h
install -D -m 0444 %PEGASUS_STAGING_DIR%PEGASUS_INCLUDE_DIR/Pegasus/Common/CIMParamValue.h %INCLUDE_DEST_PATH/Pegasus/Common/CIMParamValue.h
install -D -m 0444 %PEGASUS_STAGING_DIR%PEGASUS_INCLUDE_DIR/Pegasus/Common/CIMProperty.h %INCLUDE_DEST_PATH/Pegasus/Common/CIMProperty.h
install -D -m 0444 %PEGASUS_STAGING_DIR%PEGASUS_INCLUDE_DIR/Pegasus/Common/CIMPropertyList.h %INCLUDE_DEST_PATH/Pegasus/Common/CIMPropertyList.h
install -D -m 0444 %PEGASUS_STAGING_DIR%PEGASUS_INCLUDE_DIR/Pegasus/Common/CIMQualifierDecl.h %INCLUDE_DEST_PATH/Pegasus/Common/CIMQualifierDecl.h
install -D -m 0444 %PEGASUS_STAGING_DIR%PEGASUS_INCLUDE_DIR/Pegasus/Common/CIMQualifier.h %INCLUDE_DEST_PATH/Pegasus/Common/CIMQualifier.h
install -D -m 0444 %PEGASUS_STAGING_DIR%PEGASUS_INCLUDE_DIR/Pegasus/Common/CIMScope.h %INCLUDE_DEST_PATH/Pegasus/Common/CIMScope.h
install -D -m 0444 %PEGASUS_STAGING_DIR%PEGASUS_INCLUDE_DIR/Pegasus/Common/CIMStatusCode.h %INCLUDE_DEST_PATH/Pegasus/Common/CIMStatusCode.h
install -D -m 0444 %PEGASUS_STAGING_DIR%PEGASUS_INCLUDE_DIR/Pegasus/Common/CIMType.h %INCLUDE_DEST_PATH/Pegasus/Common/CIMType.h
install -D -m 0444 %PEGASUS_STAGING_DIR%PEGASUS_INCLUDE_DIR/Pegasus/Common/CIMValue.h %INCLUDE_DEST_PATH/Pegasus/Common/CIMValue.h
install -D -m 0444 %PEGASUS_STAGING_DIR%PEGASUS_INCLUDE_DIR/Pegasus/Common/Config.h %INCLUDE_DEST_PATH/Pegasus/Common/Config.h
install -D -m 0444 %PEGASUS_STAGING_DIR%PEGASUS_INCLUDE_DIR/Pegasus/Common/ContentLanguageElement.h %INCLUDE_DEST_PATH/Pegasus/Common/ContentLanguageElement.h
install -D -m 0444 %PEGASUS_STAGING_DIR%PEGASUS_INCLUDE_DIR/Pegasus/Common/ContentLanguages.h %INCLUDE_DEST_PATH/Pegasus/Common/ContentLanguages.h
install -D -m 0444 %PEGASUS_STAGING_DIR%PEGASUS_INCLUDE_DIR/Pegasus/Common/Exception.h %INCLUDE_DEST_PATH/Pegasus/Common/Exception.h
install -D -m 0444 %PEGASUS_STAGING_DIR%PEGASUS_INCLUDE_DIR/Pegasus/Common/Formatter.h %INCLUDE_DEST_PATH/Pegasus/Common/Formatter.h
install -D -m 0444 %PEGASUS_STAGING_DIR%PEGASUS_INCLUDE_DIR/Pegasus/Common/LanguageElementContainer.h %INCLUDE_DEST_PATH/Pegasus/Common/LanguageElementContainer.h
install -D -m 0444 %PEGASUS_STAGING_DIR%PEGASUS_INCLUDE_DIR/Pegasus/Common/LanguageElement.h %INCLUDE_DEST_PATH/Pegasus/Common/LanguageElement.h
install -D -m 0444 %PEGASUS_STAGING_DIR%PEGASUS_INCLUDE_DIR/Pegasus/Common/Linkage.h %INCLUDE_DEST_PATH/Pegasus/Common/Linkage.h
install -D -m 0444 %PEGASUS_STAGING_DIR%PEGASUS_INCLUDE_DIR/Pegasus/Common/MessageLoader.h %INCLUDE_DEST_PATH/Pegasus/Common/MessageLoader.h
install -D -m 0444 %PEGASUS_STAGING_DIR%PEGASUS_INCLUDE_DIR/Pegasus/Common/OperationContext.h %INCLUDE_DEST_PATH/Pegasus/Common/OperationContext.h
install -D -m 0444 %PEGASUS_STAGING_DIR%PEGASUS_INCLUDE_DIR/Pegasus/Common/Platform_%PEGASUS_HARDWARE_PLATFORM.h %INCLUDE_DEST_PATH/Pegasus/Common/Platform_%PEGASUS_HARDWARE_PLATFORM.h
install -D -m 0444 %PEGASUS_STAGING_DIR%PEGASUS_INCLUDE_DIR/Pegasus/Common/ResponseHandler.h %INCLUDE_DEST_PATH/Pegasus/Common/ResponseHandler.h
install -D -m 0444 %PEGASUS_STAGING_DIR%PEGASUS_INCLUDE_DIR/Pegasus/Common/SSLContext.h %INCLUDE_DEST_PATH/Pegasus/Common/SSLContext.h
install -D -m 0444 %PEGASUS_STAGING_DIR%PEGASUS_INCLUDE_DIR/Pegasus/Common/String.h %INCLUDE_DEST_PATH/Pegasus/Common/String.h
install -D -m 0444 %PEGASUS_STAGING_DIR%PEGASUS_INCLUDE_DIR/Pegasus/Consumer/CIMIndicationConsumer.h %INCLUDE_DEST_PATH/Pegasus/Consumer/CIMIndicationConsumer.h
install -D -m 0444 %PEGASUS_STAGING_DIR%PEGASUS_INCLUDE_DIR/Pegasus/Consumer/Linkage.h %INCLUDE_DEST_PATH/Pegasus/Consumer/Linkage.h
install -D -m 0444 %PEGASUS_STAGING_DIR%PEGASUS_INCLUDE_DIR/Pegasus/Provider/CIMAssociationProvider.h %INCLUDE_DEST_PATH/Pegasus/Provider/CIMAssociationProvider.h
install -D -m 0444 %PEGASUS_STAGING_DIR%PEGASUS_INCLUDE_DIR/Pegasus/Provider/CIMIndicationConsumerProvider.h %INCLUDE_DEST_PATH/Pegasus/Provider/CIMIndicationConsumerProvider.h
install -D -m 0444 %PEGASUS_STAGING_DIR%PEGASUS_INCLUDE_DIR/Pegasus/Provider/CIMIndicationProvider.h %INCLUDE_DEST_PATH/Pegasus/Provider/CIMIndicationProvider.h
install -D -m 0444 %PEGASUS_STAGING_DIR%PEGASUS_INCLUDE_DIR/Pegasus/Provider/CIMInstanceProvider.h %INCLUDE_DEST_PATH/Pegasus/Provider/CIMInstanceProvider.h
install -D -m 0444 %PEGASUS_STAGING_DIR%PEGASUS_INCLUDE_DIR/Pegasus/Provider/CIMMethodProvider.h %INCLUDE_DEST_PATH/Pegasus/Provider/CIMMethodProvider.h
install -D -m 0444 %PEGASUS_STAGING_DIR%PEGASUS_INCLUDE_DIR/Pegasus/Provider/CIMOMHandle.h %INCLUDE_DEST_PATH/Pegasus/Provider/CIMOMHandle.h
install -D -m 0444 %PEGASUS_STAGING_DIR%PEGASUS_INCLUDE_DIR/Pegasus/Provider/CIMProvider.h %INCLUDE_DEST_PATH/Pegasus/Provider/CIMProvider.h
install -D -m 0444 %PEGASUS_STAGING_DIR%PEGASUS_INCLUDE_DIR/Pegasus/Provider/Linkage.h %INCLUDE_DEST_PATH/Pegasus/Provider/Linkage.h
install -D -m 0444 %PEGASUS_STAGING_DIR%PEGASUS_INCLUDE_DIR/Pegasus/Provider/ProviderException.h %INCLUDE_DEST_PATH/Pegasus/Provider/ProviderException.h


cd %PEGASUS_STAGING_DIR%PEGASUS_PROD_DIR/
cp -rf html $RPM_BUILD_ROOT%PEGASUS_PROD_DIR

install -D -m 0444 %PEGASUS_STAGING_DIR%PEGASUS_SAMPLES_DIR/Makefile %SAMPLES_DEST_PATH/Makefile 
install -D -m 0444 %PEGASUS_STAGING_DIR%PEGASUS_SAMPLES_DIR/mak/common.mak %SAMPLES_DEST_PATH/mak/common.mak 

echo "PEGASUS_DEST_LIB_DIR =   "%PEGASUS_DEST_LIB_DIR > sampleconfig.txt
echo "PEGASUS_VARDATA_DIR =    "%PEGASUS_VARDATA_DIR >> sampleconfig.txt
echo "PEGASUS_PROVIDER_LIB_DIR="%PEGASUS_PROVIDER_LIB_DIR >> sampleconfig.txt
echo "PEGASUS_MOF_DIR =        "%PEGASUS_MOF_DIR>> sampleconfig.txt
echo "PEGASUS_INCLUDE_DIR =    "%PEGASUS_INCLUDE_DIR >> sampleconfig.txt
echo "PEGASUS_SAMPLES_DIR =    "%PEGASUS_SAMPLES_DIR >> sampleconfig.txt
echo "PEGASUS_BIN_DIR =        "%PEGASUS_BIN_DIR >> sampleconfig.txt
cat sampleconfig.txt %PEGASUS_STAGING_DIR%PEGASUS_SAMPLES_DIR/mak/config.mak > sampleconfig.mak
install -D -m 0444 sampleconfig.mak %SAMPLES_DEST_PATH/mak/config.mak 
install -D -m 0444  %PEGASUS_STAGING_DIR%PEGASUS_SAMPLES_DIR/mak/%PEGASUS_HARDWARE_PLATFORM.mak %SAMPLES_DEST_PATH/mak/%PEGASUS_HARDWARE_PLATFORM.mak 
install -D -m 0444 %PEGASUS_STAGING_DIR%PEGASUS_SAMPLES_DIR/mak/library.mak %SAMPLES_DEST_PATH/mak/library.mak 
install -D -m 0444 %PEGASUS_STAGING_DIR%PEGASUS_SAMPLES_DIR/mak/program.mak %SAMPLES_DEST_PATH/mak/program.mak
install -D -m 0444 %PEGASUS_STAGING_DIR%PEGASUS_SAMPLES_DIR/mak/recurse.mak %SAMPLES_DEST_PATH/mak/recurse.mak
install -D -m 0444 %PEGASUS_STAGING_DIR%PEGASUS_SAMPLES_DIR/Clients/Makefile %SAMPLES_DEST_PATH/Clients/Makefile
install -D -m 0444 %PEGASUS_STAGING_DIR%PEGASUS_SAMPLES_DIR/Clients/DefaultC++/Makefile %SAMPLES_DEST_PATH/Clients/DefaultC++/Makefile
install -D -m 0444 %PEGASUS_STAGING_DIR%PEGASUS_SAMPLES_DIR/Clients/DefaultC++/EnumInstances/EnumInstances.cpp %SAMPLES_DEST_PATH/Clients/DefaultC++/EnumInstances/EnumInstances.cpp 
install -D -m 0444 %PEGASUS_STAGING_DIR%PEGASUS_SAMPLES_DIR/Clients/DefaultC++/EnumInstances/Makefile %SAMPLES_DEST_PATH/Clients/DefaultC++/EnumInstances/Makefile 
install -D -m 0444 %PEGASUS_STAGING_DIR%PEGASUS_SAMPLES_DIR/Clients/DefaultC++/InvokeMethod/InvokeMethod.cpp %SAMPLES_DEST_PATH/Clients/DefaultC++/InvokeMethod/InvokeMethod.cpp 
install -D -m 0444 %PEGASUS_STAGING_DIR%PEGASUS_SAMPLES_DIR/Clients/DefaultC++/InvokeMethod/Makefile %SAMPLES_DEST_PATH/Clients/DefaultC++/InvokeMethod/Makefile 
install -D -m 0444 %PEGASUS_STAGING_DIR%PEGASUS_SAMPLES_DIR/Clients/DefaultC++/SendTestIndications/Makefile %SAMPLES_DEST_PATH/Clients/DefaultC++/SendTestIndications/Makefile 
install -D -m 0444 %PEGASUS_STAGING_DIR%PEGASUS_SAMPLES_DIR/Clients/DefaultC++/SendTestIndications/SendTestIndications.cpp %SAMPLES_DEST_PATH/Clients/DefaultC++/SendTestIndications/SendTestIndications.cpp
install -D -m 0444 %PEGASUS_STAGING_DIR%PEGASUS_SAMPLES_DIR/Providers/Makefile %SAMPLES_DEST_PATH/Providers/Makefile
install -D -m 0444 %PEGASUS_STAGING_DIR%PEGASUS_SAMPLES_DIR/Providers/DefaultC++/Makefile %SAMPLES_DEST_PATH/Providers/DefaultC++/Makefile
install -D -m 0444 %PEGASUS_STAGING_DIR%PEGASUS_SAMPLES_DIR/Providers/DefaultC++/IndicationProvider/IndicationProvider.h %SAMPLES_DEST_PATH/Providers/DefaultC++/IndicationProvider/IndicationProvider.h
install -D -m 0444 %PEGASUS_STAGING_DIR%PEGASUS_SAMPLES_DIR/Providers/DefaultC++/IndicationProvider/IndicationProvider.cpp %SAMPLES_DEST_PATH/Providers/DefaultC++/IndicationProvider/IndicationProvider.cpp 
install -D -m 0444 %PEGASUS_STAGING_DIR%PEGASUS_SAMPLES_DIR/Providers/DefaultC++/IndicationProvider/IndicationProviderMain.cpp %SAMPLES_DEST_PATH/Providers/DefaultC++/IndicationProvider/IndicationProviderMain.cpp 
install -D -m 0444 %PEGASUS_STAGING_DIR%PEGASUS_SAMPLES_DIR/Providers/DefaultC++/IndicationProvider/Makefile %SAMPLES_DEST_PATH/Providers/DefaultC++/IndicationProvider/Makefile
install -D -m 0444 %PEGASUS_STAGING_DIR%PEGASUS_SAMPLES_DIR/Providers/DefaultC++/InstanceProvider/EnumerateInstancesrspgood.xml %SAMPLES_DEST_PATH/Providers/DefaultC++/InstanceProvider/EnumerateInstancesrspgood.xml 
install -D -m 0444 %PEGASUS_STAGING_DIR%PEGASUS_SAMPLES_DIR/Providers/DefaultC++/InstanceProvider/EnumerateInstances.xml %SAMPLES_DEST_PATH/Providers/DefaultC++/InstanceProvider/EnumerateInstances.xml 
install -D -m 0444 %PEGASUS_STAGING_DIR%PEGASUS_SAMPLES_DIR/Providers/DefaultC++/InstanceProvider/InstanceProvider.cpp %SAMPLES_DEST_PATH/Providers/DefaultC++/InstanceProvider/InstanceProvider.cpp 
install -D -m 0444 %PEGASUS_STAGING_DIR%PEGASUS_SAMPLES_DIR/Providers/DefaultC++/InstanceProvider/InstanceProvider.h %SAMPLES_DEST_PATH/Providers/DefaultC++/InstanceProvider/InstanceProvider.h 
install -D -m 0444 %PEGASUS_STAGING_DIR%PEGASUS_SAMPLES_DIR/Providers/DefaultC++/InstanceProvider/InstanceProviderMain.cpp %SAMPLES_DEST_PATH/Providers/DefaultC++/InstanceProvider/InstanceProviderMain.cpp 
install -D -m 0444 %PEGASUS_STAGING_DIR%PEGASUS_SAMPLES_DIR/Providers/DefaultC++/InstanceProvider/Makefile %SAMPLES_DEST_PATH/Providers/DefaultC++/InstanceProvider/Makefile 
install -D -m 0444 %PEGASUS_STAGING_DIR%PEGASUS_SAMPLES_DIR/Providers/DefaultC++/MethodProvider/InvokeMethodrspgood.xml %SAMPLES_DEST_PATH/Providers/DefaultC++/MethodProvider/InvokeMethodrspgood.xml 
install -D -m 0444 %PEGASUS_STAGING_DIR%PEGASUS_SAMPLES_DIR/Providers/DefaultC++/MethodProvider/InvokeMethod.xml %SAMPLES_DEST_PATH/Providers/DefaultC++/MethodProvider/InvokeMethod.xml 
install -D -m 0444 %PEGASUS_STAGING_DIR%PEGASUS_SAMPLES_DIR/Providers/DefaultC++/MethodProvider/Makefile %SAMPLES_DEST_PATH/Providers/DefaultC++/MethodProvider/Makefile 
install -D -m 0444 %PEGASUS_STAGING_DIR%PEGASUS_SAMPLES_DIR/Providers/DefaultC++/MethodProvider/MethodProvider.cpp %SAMPLES_DEST_PATH/Providers/DefaultC++/MethodProvider/MethodProvider.cpp 
install -D -m 0444 %PEGASUS_STAGING_DIR%PEGASUS_SAMPLES_DIR/Providers/DefaultC++/MethodProvider/MethodProvider.h %SAMPLES_DEST_PATH/Providers/DefaultC++/MethodProvider/MethodProvider.h 
install -D -m 0444 %PEGASUS_STAGING_DIR%PEGASUS_SAMPLES_DIR/Providers/DefaultC++/MethodProvider/MethodProviderMain.cpp %SAMPLES_DEST_PATH/Providers/DefaultC++/MethodProvider/MethodProviderMain.cpp 
install -D -m 0444 %PEGASUS_STAGING_DIR%PEGASUS_SAMPLES_DIR/Providers/DefaultC++/SimpleDisplayConsumer/SimpleDisplayConsumerMain.cpp %SAMPLES_DEST_PATH/Providers/DefaultC++/SimpleDisplayConsumer/SimpleDisplayConsumerMain.cpp 
install -D -m 0444 %PEGASUS_STAGING_DIR%PEGASUS_SAMPLES_DIR/Providers/DefaultC++/SimpleDisplayConsumer/SimpleDisplayConsumer.cpp %SAMPLES_DEST_PATH/Providers/DefaultC++/SimpleDisplayConsumer/SimpleDisplayConsumer.cpp 
install -D -m 0444 %PEGASUS_STAGING_DIR%PEGASUS_SAMPLES_DIR/Providers/DefaultC++/SimpleDisplayConsumer/SimpleDisplayConsumer.h %SAMPLES_DEST_PATH/Providers/DefaultC++/SimpleDisplayConsumer/SimpleDisplayConsumer.h
install -D -m 0444 %PEGASUS_STAGING_DIR%PEGASUS_SAMPLES_DIR/Providers/DefaultC++/SimpleDisplayConsumer/Makefile %SAMPLES_DEST_PATH/Providers/DefaultC++/SimpleDisplayConsumer/Makefile
install -D -m 0444 %PEGASUS_STAGING_DIR%PEGASUS_SAMPLES_DIR/Providers/Load/Makefile %SAMPLES_DEST_PATH/Providers/Load/Makefile
install -D -m 0444 %PEGASUS_STAGING_DIR%PEGASUS_SAMPLES_DIR/Providers/Load/IndicationProviderR.mof %SAMPLES_DEST_PATH/Providers/Load/IndicationProviderR.mof
install -D -m 0444 %PEGASUS_STAGING_DIR%PEGASUS_SAMPLES_DIR/Providers/Load/InstanceProviderR.mof %SAMPLES_DEST_PATH/Providers/Load/InstanceProviderR.mof
install -D -m 0444 %PEGASUS_STAGING_DIR%PEGASUS_SAMPLES_DIR/Providers/Load/MethodProviderR.mof %SAMPLES_DEST_PATH/Providers/Load/MethodProviderR.mof
install -D -m 0444 %PEGASUS_STAGING_DIR%PEGASUS_SAMPLES_DIR/Providers/Load/SampleProviderSchema.mof %SAMPLES_DEST_PATH/Providers/Load/SampleProviderSchema.mof
install -D -m 0444 %PEGASUS_STAGING_DIR%PEGASUS_SAMPLES_DIR/Providers/Load/SimpleDisplayConsumerR.mof %SAMPLES_DEST_PATH/Providers/Load/SimpleDisplayConsumerR.mof

%if %{PEGASUS_BUILD_TEST_RPM}
#
# Tests
#
%define TEST_DEST_PATH     $RPM_BUILD_ROOT%PEGASUS_TEST_DIR
mkdir -p %TEST_DEST_PATH/{bin,lib,mak}
#
# Test Repository
#
%define PEGASUS_TEST_REPOSITORY_DIR %PEGASUS_VARDATA_DIR/testrepository
mkdir -p  $RPM_BUILD_ROOT%PEGASUS_TEST_REPOSITORY_DIR
cp -rf %PEGASUS_TEST_STAGING_DIR%PEGASUS_TEST_REPOSITORY_DIR/*  $RPM_BUILD_ROOT%PEGASUS_TEST_REPOSITORY_DIR

install -D -m 0444 %PEGASUS_TEST_STAGING_DIR/%PEGASUS_TEST_DIR/Makefile %TEST_DEST_PATH/Makefile
install -D -m 0755 %PEGASUS_TEST_STAGING_DIR%PEGASUS_TEST_BIN_DIR/InvokeMethod2 $RPM_BUILD_ROOT%PEGASUS_TEST_BIN_DIR/InvokeMethod2
install -D -m 0755 %PEGASUS_TEST_STAGING_DIR%PEGASUS_TEST_BIN_DIR/IPC $RPM_BUILD_ROOT%PEGASUS_TEST_BIN_DIR/IPC
install -D -m 0755 %PEGASUS_TEST_STAGING_DIR%PEGASUS_TEST_BIN_DIR/TestAbstract $RPM_BUILD_ROOT%PEGASUS_TEST_BIN_DIR/TestAbstract
install -D -m 0755 %PEGASUS_TEST_STAGING_DIR%PEGASUS_TEST_BIN_DIR/TestArray $RPM_BUILD_ROOT%PEGASUS_TEST_BIN_DIR/TestArray
install -D -m 0755 %PEGASUS_TEST_STAGING_DIR%PEGASUS_TEST_BIN_DIR/TestBase64 $RPM_BUILD_ROOT%PEGASUS_TEST_BIN_DIR/TestBase64
install -D -m 0755 %PEGASUS_TEST_STAGING_DIR%PEGASUS_TEST_BIN_DIR/TestClassDecl $RPM_BUILD_ROOT%PEGASUS_TEST_BIN_DIR/TestClassDecl
install -D -m 0755 %PEGASUS_TEST_STAGING_DIR%PEGASUS_TEST_BIN_DIR/TestClient $RPM_BUILD_ROOT%PEGASUS_TEST_BIN_DIR/TestClient
install -D -m 0755 %PEGASUS_TEST_STAGING_DIR%PEGASUS_TEST_BIN_DIR/TestDateTime $RPM_BUILD_ROOT%PEGASUS_TEST_BIN_DIR/TestDateTime
install -D -m 0755 %PEGASUS_TEST_STAGING_DIR%PEGASUS_TEST_BIN_DIR/TestFlavor $RPM_BUILD_ROOT%PEGASUS_TEST_BIN_DIR/TestFlavor
install -D -m 0755 %PEGASUS_TEST_STAGING_DIR%PEGASUS_TEST_BIN_DIR/TestFormatter $RPM_BUILD_ROOT%PEGASUS_TEST_BIN_DIR/TestFormatter
install -D -m 0755 %PEGASUS_TEST_STAGING_DIR%PEGASUS_TEST_BIN_DIR/TestHashTable $RPM_BUILD_ROOT%PEGASUS_TEST_BIN_DIR/TestHashTable
install -D -m 0755 %PEGASUS_TEST_STAGING_DIR%PEGASUS_TEST_BIN_DIR/TestInstanceDecl $RPM_BUILD_ROOT%PEGASUS_TEST_BIN_DIR/TestInstanceDecl
install -D -m 0755 %PEGASUS_TEST_STAGING_DIR%PEGASUS_TEST_BIN_DIR/TestLogger $RPM_BUILD_ROOT%PEGASUS_TEST_BIN_DIR/TestLogger
install -D -m 0755 %PEGASUS_TEST_STAGING_DIR%PEGASUS_TEST_BIN_DIR/TestMethod $RPM_BUILD_ROOT%PEGASUS_TEST_BIN_DIR/TestMethod
install -D -m 0755 %PEGASUS_TEST_STAGING_DIR%PEGASUS_TEST_BIN_DIR/TestObject $RPM_BUILD_ROOT%PEGASUS_TEST_BIN_DIR/TestObject
install -D -m 0755 %PEGASUS_TEST_STAGING_DIR%PEGASUS_TEST_BIN_DIR/TestOperationContext $RPM_BUILD_ROOT%PEGASUS_TEST_BIN_DIR/TestOperationContext
install -D -m 0755 %PEGASUS_TEST_STAGING_DIR%PEGASUS_TEST_BIN_DIR/TestParameter $RPM_BUILD_ROOT%PEGASUS_TEST_BIN_DIR/TestParameter
install -D -m 0755 %PEGASUS_TEST_STAGING_DIR%PEGASUS_TEST_BIN_DIR/TestParamValue $RPM_BUILD_ROOT%PEGASUS_TEST_BIN_DIR/TestParamValue
install -D -m 0755 %PEGASUS_TEST_STAGING_DIR%PEGASUS_TEST_BIN_DIR/TestProperty $RPM_BUILD_ROOT%PEGASUS_TEST_BIN_DIR/TestProperty
install -D -m 0755 %PEGASUS_TEST_STAGING_DIR%PEGASUS_TEST_BIN_DIR/TestQualifier $RPM_BUILD_ROOT%PEGASUS_TEST_BIN_DIR/TestQualifier
install -D -m 0755 %PEGASUS_TEST_STAGING_DIR%PEGASUS_TEST_BIN_DIR/TestQualifierDecl $RPM_BUILD_ROOT%PEGASUS_TEST_BIN_DIR/TestQualifierDecl
install -D -m 0755 %PEGASUS_TEST_STAGING_DIR%PEGASUS_TEST_BIN_DIR/TestQualifierList $RPM_BUILD_ROOT%PEGASUS_TEST_BIN_DIR/TestQualifierList
install -D -m 0755 %PEGASUS_TEST_STAGING_DIR%PEGASUS_TEST_BIN_DIR/TestQueue $RPM_BUILD_ROOT%PEGASUS_TEST_BIN_DIR/TestQueue
install -D -m 0755 %PEGASUS_TEST_STAGING_DIR%PEGASUS_TEST_BIN_DIR/TestReference $RPM_BUILD_ROOT%PEGASUS_TEST_BIN_DIR/TestReference
install -D -m 0755 %PEGASUS_TEST_STAGING_DIR%PEGASUS_TEST_BIN_DIR/TestResolve $RPM_BUILD_ROOT%PEGASUS_TEST_BIN_DIR/TestResolve
install -D -m 0755 %PEGASUS_TEST_STAGING_DIR%PEGASUS_TEST_BIN_DIR/TestResponseHandler $RPM_BUILD_ROOT%PEGASUS_TEST_BIN_DIR/TestResponseHandler
install -D -m 0755 %PEGASUS_TEST_STAGING_DIR%PEGASUS_TEST_BIN_DIR/TestScope $RPM_BUILD_ROOT%PEGASUS_TEST_BIN_DIR/TestScope
install -D -m 0755 %PEGASUS_TEST_STAGING_DIR%PEGASUS_TEST_BIN_DIR/TestStack $RPM_BUILD_ROOT%PEGASUS_TEST_BIN_DIR/TestStack
install -D -m 0755 %PEGASUS_TEST_STAGING_DIR%PEGASUS_TEST_BIN_DIR/TestStopwatch $RPM_BUILD_ROOT%PEGASUS_TEST_BIN_DIR/TestStopwatch
install -D -m 0755 %PEGASUS_TEST_STAGING_DIR%PEGASUS_TEST_BIN_DIR/TestString $RPM_BUILD_ROOT%PEGASUS_TEST_BIN_DIR/TestString
install -D -m 0755 %PEGASUS_TEST_STAGING_DIR%PEGASUS_TEST_BIN_DIR/TestStrToInstName $RPM_BUILD_ROOT%PEGASUS_TEST_BIN_DIR/TestStrToInstName
install -D -m 0755 %PEGASUS_TEST_STAGING_DIR%PEGASUS_TEST_BIN_DIR/TestTimeValue $RPM_BUILD_ROOT%PEGASUS_TEST_BIN_DIR/TestTimeValue
install -D -m 0755 %PEGASUS_TEST_STAGING_DIR%PEGASUS_TEST_BIN_DIR/TestToMof $RPM_BUILD_ROOT%PEGASUS_TEST_BIN_DIR/TestToMof
install -D -m 0755 %PEGASUS_TEST_STAGING_DIR%PEGASUS_TEST_BIN_DIR/TestValidateClass $RPM_BUILD_ROOT%PEGASUS_TEST_BIN_DIR/TestValidateClass
install -D -m 0755 %PEGASUS_TEST_STAGING_DIR%PEGASUS_TEST_BIN_DIR/TestValue $RPM_BUILD_ROOT%PEGASUS_TEST_BIN_DIR/TestValue
install -D -m 0755 %PEGASUS_TEST_STAGING_DIR%PEGASUS_TEST_BIN_DIR/TracerTest $RPM_BUILD_ROOT%PEGASUS_TEST_BIN_DIR/TracerTest
install -D -m 0755 %PEGASUS_TEST_STAGING_DIR%PEGASUS_TEST_LIB_DIR/libSampleFamilyProvider.so.1   $RPM_BUILD_ROOT%PEGASUS_TEST_LIB_DIR/libSampleFamilyProvider.so.1
install -D -m 0755 %PEGASUS_TEST_STAGING_DIR%PEGASUS_TEST_LIB_DIR/libSampleInstanceProvider.so.1 $RPM_BUILD_ROOT%PEGASUS_TEST_LIB_DIR/libSampleInstanceProvider.so.1
install -D -m 0755 %PEGASUS_TEST_STAGING_DIR%PEGASUS_TEST_LIB_DIR/libSampleMethodProvider.so.1   $RPM_BUILD_ROOT%PEGASUS_TEST_LIB_DIR/libSampleMethodProvider.so.1
install -D -m 0755 %PEGASUS_TEST_STAGING_DIR%PEGASUS_TEST_MAK_DIR/commands.mak $RPM_BUILD_ROOT%PEGASUS_TEST_MAK_DIR/commands.mak
%endif

cd $RPM_BUILD_ROOT
rm -Rf $PEGASUS_HOME

%clean
#make clean
[ "$RPM_BUILD_ROOT" != "/" ] && [ -d $RPM_BUILD_ROOT ] && rm -rf $RPM_BUILD_ROOT;

%pre
# Check if the cimserver is running
rm -f %PEGASUS_SBIN_DIR/RPM_CIMSERVER_STOPPED
isRunning=`ps -el | grep cimserver | grep -v "grep cimserver"`
if [ "$isRunning" ]; then
     %PEGASUS_SBIN_DIR/cimserver -s
     touch %PEGASUS_SBIN_DIR/RPM_CIMSERVER_STOPPED
fi
if [ -d %PEGASUS_PREV_REPOSITORY_DIR ]
then
   mv %PEGASUS_PREV_REPOSITORY_DIR %PEGASUS_PREV_REPOSITORY_DIR`date '+%Y-%m-%d-%s.%N'`.rpmsave;
fi
if [ -d %PEGASUS_REPOSITORY_DIR ]
then
   mv %PEGASUS_REPOSITORY_DIR %PEGASUS_PREV_REPOSITORY_DIR
fi

%post
mkdir -p %PEGASUS_LOG_DIR
%define INSTALL_LOG %PEGASUS_LOG_DIR/install.log
echo `date` >%INSTALL_LOG 2>&1

# Create symbolic links for client libs
#
cd %PEGASUS_DEST_LIB_DIR
ln -sf libpegcommon.so.1 libpegcommon.so
ln -sf libpegclient.so.1 libpegclient.so
ln -sf libpegprovider.so.1 libpegprovider.so
ln -sf libDefaultProviderManager.so.1 libDefaultProviderManager.so
ln -sf libCMPIProviderManager.so.1 libCMPIProviderManager.so
ln -sf libCIMxmlIndicationHandler.so.1 libCIMxmlIndicationHandler.so

# Create symbolic links for provider libs
#
cd %PEGASUS_PROVIDER_LIB_DIR
ln -sf libComputerSystemProvider.so.1 libComputerSystemProvider.so
ln -sf libOSProvider.so.1 libOSProvider.so
ln -sf libProcessProvider.so.1 libProcessProvider.so

if [ $1 -eq 1 ]; then
/usr/lib/lsb/install_initd /etc/init.d/tog-pegasus
#
#  Set up the openssl certificate
#
#  Modify entries in ssl.cnf, then
#  Create big random ssl.rnd file, then 
#  Generate a self signed node certificate
#
echo " Generating SSL certificates... "
echo "[ req ]" > %PEGASUS_CONFIG_DIR/ssl.cnf
echo "distinguished_name     = req_distinguished_name"  >> %PEGASUS_CONFIG_DIR/ssl.cnf
echo "prompt                 = no"  >> %PEGASUS_CONFIG_DIR/ssl.cnf
echo "[ req_distinguished_name ]" >> %PEGASUS_CONFIG_DIR/ssl.cnf
echo "C                      = UK" >> %PEGASUS_CONFIG_DIR/ssl.cnf
echo "ST                     = Berkshire" >> %PEGASUS_CONFIG_DIR/ssl.cnf
echo "L                      = Reading" >> %PEGASUS_CONFIG_DIR/ssl.cnf
echo "O                      = The Open Group" >> %PEGASUS_CONFIG_DIR/ssl.cnf
echo "OU                     = The OpenPegasus Project" >> %PEGASUS_CONFIG_DIR/ssl.cnf
DN=`hostname`;
if [ -z "$DN" ] || [ "$DN" = "(none)" ]; then
        DN='localhost.localdomain';
fi;
FQDN=`{ host -W1 $DN 2>/dev/null || echo "$DN has address "; } |\
        grep 'has address' | head -1 | sed 's/\ .*$//'`;
if [ -z "$FQDN" ] ; then
    FQDN="$DN";
fi;
# cannot use 'hostname --fqdn' because this can hang indefinitely
echo "CN                     = $FQDN"  >> %PEGASUS_CONFIG_DIR/ssl.cnf
chmod 400 %PEGASUS_CONFIG_DIR/ssl.cnf
chown root %PEGASUS_CONFIG_DIR/ssl.cnf
chgrp root %PEGASUS_CONFIG_DIR/ssl.cnf

openssl req -x509 -days 3650 -newkey rsa:2048 \
   -nodes -config %PEGASUS_CONFIG_DIR/ssl.cnf   \
   -keyout %PEGASUS_CONFIG_DIR/key.pem -out %PEGASUS_CONFIG_DIR/cert.pem 2>>%INSTALL_LOG
chmod 700 %PEGASUS_CONFIG_DIR/*.pem
cat %PEGASUS_CONFIG_DIR/key.pem > %PEGASUS_CONFIG_DIR/file_2048.pem
chmod 400 %PEGASUS_CONFIG_DIR/file_2048.pem
cat %PEGASUS_CONFIG_DIR/cert.pem > %PEGASUS_CONFIG_DIR/server_2048.pem
chmod 400 %PEGASUS_CONFIG_DIR/server_2048.pem
cat %PEGASUS_CONFIG_DIR/cert.pem > %PEGASUS_CONFIG_DIR/client_2048.pem
chmod 400 %PEGASUS_CONFIG_DIR/client_2048.pem

rm -f %PEGASUS_CONFIG_DIR/key.pem %PEGASUS_CONFIG_DIR/cert.pem

if [ -f %PEGASUS_CONFIG_DIR/%PEGASUS_SSL_CERT_FILE ]
then
    echo "WARNING: %PEGASUS_CONFIG_DIR/%PEGASUS_SSL_CERT_FILE SSL Certificate file already exists."
else
    cp %PEGASUS_CONFIG_DIR/server_2048.pem %PEGASUS_CONFIG_DIR/%PEGASUS_SSL_CERT_FILE
    cp %PEGASUS_CONFIG_DIR/file_2048.pem %PEGASUS_CONFIG_DIR/%PEGASUS_SSL_KEY_FILE
    chmod 400 %PEGASUS_CONFIG_DIR/%PEGASUS_SSL_KEY_FILE
    chmod 444 %PEGASUS_CONFIG_DIR/%PEGASUS_SSL_CERT_FILE
fi

if [ -f %PEGASUS_CONFIG_DIR/%PEGASUS_SSL_TRUSTSTORE ]
then
    echo "WARNING: %PEGASUS_CONFIG_DIR/%PEGASUS_SSL_TRUSTSTORE SSL Certificate trust store already exists."
else
    cp %PEGASUS_CONFIG_DIR/client_2048.pem %PEGASUS_CONFIG_DIR/%PEGASUS_SSL_TRUSTSTORE
    chmod 444 %PEGASUS_CONFIG_DIR/%PEGASUS_SSL_TRUSTSTORE
fi

echo " To start Pegasus manually:"
echo " /etc/init.d/tog-pegasus start"
echo " Stop it:"
echo " /etc/init.d/tog-pegasus stop"
echo " To set up PATH and MANPATH in /etc/profile"
echo " run /opt/tog-pegasus/sbin/settogpath.";
fi
if  [ -d %PEGASUS_PREV_REPOSITORY_DIR ]
then
  mv %PEGASUS_REPOSITORY_DIR %PEGASUS_NEW_REPOSITORY_DIR
  mv %PEGASUS_PREV_REPOSITORY_DIR %PEGASUS_REPOSITORY_DIR
fi
if [ -f %PEGASUS_SBIN_DIR/RPM_CIMSERVER_STOPPED ]
then
    rm %PEGASUS_SBIN_DIR/RPM_CIMSERVER_STOPPED
    %PEGASUS_SBIN_DIR/cimserver
fi

%if %{PEGASUS_BUILD_TEST_RPM}
%post test
/etc/init.d/tog-pegasus stop
cd %PEGASUS_VARDATA_DIR
mv repository repository.bak
mv testrepository repository

cd %PEGASUS_TEST_DIR
make create_providerlinks
make tests
%endif

%preun
if [ $1 -eq 0 ]; then
   # Check if the cimserver is running
   isRunning=`ps -el | grep cimserver | grep -v "grep cimserver"`
   if [ "$isRunning" ]; then
        %PEGASUS_SBIN_DIR/cimserver -s  
   fi
   rm -f %PEGASUS_VARDATA_DIR/cimserver_current.conf;
   rm -f %PEGASUS_LOG_DIR/install.log;
   [ -d %PEGASUS_NEW_REPOSITORY_DIR ] && rm -rf  %PEGASUS_NEW_REPOSITORY_DIR;
   [ -d %PEGASUS_REPOSITORY_DIR ] && rm -rf  %PEGASUS_REPOSITORY_DIR;
   [ -d %PEGASUS_VARDATA_CACHE_DIR ] && rm -rf  %PEGASUS_VARDATA_CACHE_DIR;
   rm -f %PEGASUS_LOCAL_DOMAIN_SOCKET_DIR/cimxml.socket;
   rm -f %PEGASUS_VARRUN_DIR/cimserver.pid;
   # Delete the Link to the rc.* Startup Directories
   /usr/lib/lsb/remove_initd /etc/init.d/tog-pegasus;
fi

%preun devel
if [ $1 -eq 0 ]; then
   # Check if the cimserver is running
   rm -f %PEGASUS_SBIN_DIR/RPM_CIMSERVER_STARTED
   isRunning=`ps -el | grep cimserver | grep -v "grep cimserver"`
   if [ ! "$isRunning" ]; then
        %PEGASUS_SBIN_DIR/cimserver 
        touch %PEGASUS_SBIN_DIR/RPM_CIMSERVER_STARTED
   fi 
   make --directory  %PEGASUS_SAMPLES_DIR -s clean
   make --directory  %PEGASUS_SAMPLES_DIR/Providers/Load -i deregisterproviders
   if [ -f %PEGASUS_SBIN_DIR/RPM_CIMSERVER_STARTED ]
   then
        rm %PEGASUS_SBIN_DIR/RPM_CIMSERVER_STARTED
        %PEGASUS_SBIN_DIR/cimserver -s
   fi
fi

%if %{PEGASUS_BUILD_TEST_RPM}
%preun test
make --directory  %PEGASUS_TEST_DIR -s clean
%endif

%postun
if [ $1 -eq 0 ]; then
        export LC_ALL=C
fi

%files
%defattr(-,root,root)
%dir %attr(-,root,root) %PEGASUS_PROD_DIR
%dir %attr(-,root,root) %PEGASUS_BIN_DIR
%dir %attr(-,root,root) %PEGASUS_SBIN_DIR
%dir %attr(-,root,root) %PEGASUS_DEST_LIB_DIR
%dir %attr(-,root,root) %PEGASUS_LOCAL_AUTH_DIR
%dir %attr(-,root,root) %PEGASUS_CONFIG_DIR
%dir %attr(-,root,root) %PEGASUS_VARDATA_DIR
%dir %attr(-,root,root) %PEGASUS_VARDATA_CACHE_DIR
%dir %attr(-,root,root) %PEGASUS_LOG_DIR
%dir %attr(-,root,root) %PEGASUS_PRODSHARE_DIR
%dir %attr(-,root,root) %PEGASUS_DOC_DIR
%dir %attr(-,root,root) %PEGASUS_MAN_DIR
%dir %attr(-,root,root) %PEGASUS_MAN_DIR/man1
%dir %attr(-,root,root) %PEGASUS_MAN_DIR/man8
%dir %attr(-,root,root) %PEGASUS_PROVIDER_DIR
%dir %attr(-,root,root) %PEGASUS_PROVIDER_LIB_DIR
%dir %attr(-,root,root) %PEGASUS_MOF_DIR
%dir %attr(-,root,root) %PEGASUS_MOF_DIR/CIM28
%dir %attr(-,root,root) %PEGASUS_MOF_DIR/Pegasus
%dir %attr(-,root,root) %PEGASUS_VARRUN_DIR
%dir %attr(1555,root,root) %PEGASUS_LOCAL_DOMAIN_SOCKET_DIR
%defattr(-,root,root)
%PEGASUS_REPOSITORY_DIR
%doc %PEGASUS_PROD_DIR/%PEGASUS_LICENSE_FILE
%doc %PEGASUS_DOC_DIR/Admin_Guide_Release_2.4.pdf
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
%config(noreplace) %attr(-,root,root) %PEGASUS_VARDATA_DIR/%PEGASUS_PLANNED_CONFIG_FILE
%config(noreplace) %attr(-,root,root) /etc/init.d/tog-pegasus
%config(noreplace) %attr(-,root,root) %PAM_CONF/wbem
%attr(0500,root,root) %PEGASUS_SBIN_DIR/cimauth
%attr(0500,root,root) %PEGASUS_SBIN_DIR/cimserver
%attr(0500,root,root) %PEGASUS_SBIN_DIR/cimservera
%attr(0500,root,root) %PEGASUS_SBIN_DIR/cimuser
%attr(0500,root,root) %PEGASUS_SBIN_DIR/cimconfig
%attr(0500,root,root) %PEGASUS_SBIN_DIR/init_repository
%attr(0500,root,root) %PEGASUS_SBIN_DIR/settogpath
%attr(0500,root,root) %PEGASUS_SBIN_DIR/cimprovagt
%attr(0555,root,root) %PEGASUS_BIN_DIR/cimmof
%attr(0555,root,root) %PEGASUS_BIN_DIR/cimmofl
%attr(0555,root,root) %PEGASUS_BIN_DIR/cimprovider
%attr(0555,root,root) %PEGASUS_BIN_DIR/osinfo
%attr(0555,root,root) %PEGASUS_BIN_DIR/wbemexec
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

%attr(-,root,root) %PEGASUS_DEST_LIB_DIR/libpegcommon.so
%attr(-,root,root) %PEGASUS_DEST_LIB_DIR/libpegclient.so
%attr(-,root,root) %PEGASUS_DEST_LIB_DIR/libpegprovider.so
%attr(-,root,root) %PEGASUS_DEST_LIB_DIR/libDefaultProviderManager.so
%attr(-,root,root) %PEGASUS_DEST_LIB_DIR/libCMPIProviderManager.so
%attr(-,root,root) %PEGASUS_DEST_LIB_DIR/libCIMxmlIndicationHandler.so

%attr(-,root,root) %PEGASUS_PROVIDER_LIB_DIR/libComputerSystemProvider.so
%attr(-,root,root) %PEGASUS_PROVIDER_LIB_DIR/libOSProvider.so
%attr(-,root,root) %PEGASUS_PROVIDER_LIB_DIR/libProcessProvider.so

%files devel
%defattr(0444,root,root)
%dir %attr(-,root,root) %PEGASUS_INCLUDE_DIR
%dir %attr(-,root,root) %PEGASUS_INCLUDE_DIR/Pegasus
%dir %attr(-,root,root) %PEGASUS_INCLUDE_DIR/Pegasus/Client
%dir %attr(-,root,root) %PEGASUS_INCLUDE_DIR/Pegasus/Common
%dir %attr(-,root,root) %PEGASUS_INCLUDE_DIR/Pegasus/Consumer
%dir %attr(-,root,root) %PEGASUS_INCLUDE_DIR/Pegasus/Provider
%dir %attr(-,root,root) %PEGASUS_SAMPLES_DIR
%dir %attr(-,root,root) %PEGASUS_SAMPLES_DIR/Clients
%dir %attr(-,root,root) %PEGASUS_SAMPLES_DIR/Clients/DefaultC++
%dir %attr(-,root,root) %PEGASUS_SAMPLES_DIR/Clients/DefaultC++/EnumInstances
%dir %attr(-,root,root) %PEGASUS_SAMPLES_DIR/Clients/DefaultC++/InvokeMethod
%dir %attr(-,root,root) %PEGASUS_SAMPLES_DIR/Clients/DefaultC++/SendTestIndications
%dir %attr(-,root,root) %PEGASUS_SAMPLES_DIR/Providers/DefaultC++
%dir %attr(-,root,root) %PEGASUS_SAMPLES_DIR/Providers/DefaultC++/IndicationProvider
%dir %attr(-,root,root) %PEGASUS_SAMPLES_DIR/Providers/DefaultC++/InstanceProvider
%dir %attr(-,root,root) %PEGASUS_SAMPLES_DIR/Providers/DefaultC++/MethodProvider
%dir %attr(-,root,root) %PEGASUS_SAMPLES_DIR/Providers/DefaultC++/SimpleDisplayConsumer
%dir %attr(-,root,root) %PEGASUS_SAMPLES_DIR/Providers/Load
%dir %attr(-,root,root) %PEGASUS_SAMPLES_DIR/Providers
%dir %attr(-,root,root) %PEGASUS_SAMPLES_DIR/mak
%attr(-,root,root) %PEGASUS_INCLUDE_DIR/Pegasus/Client/CIMClientException.h
%attr(-,root,root) %PEGASUS_INCLUDE_DIR/Pegasus/Client/CIMClient.h
%attr(-,root,root) %PEGASUS_INCLUDE_DIR/Pegasus/Client/Linkage.h
%attr(-,root,root) %PEGASUS_INCLUDE_DIR/Pegasus/Common/AcceptLanguages.h
%attr(-,root,root) %PEGASUS_INCLUDE_DIR/Pegasus/Common/AcceptLanguageElement.h
%attr(-,root,root) %PEGASUS_INCLUDE_DIR/Pegasus/Common/Array.h
%attr(-,root,root) %PEGASUS_INCLUDE_DIR/Pegasus/Common/ArrayInter.h
%attr(-,root,root) %PEGASUS_INCLUDE_DIR/Pegasus/Common/Char16.h
%attr(-,root,root) %PEGASUS_INCLUDE_DIR/Pegasus/Common/CIMClass.h 
%attr(-,root,root) %PEGASUS_INCLUDE_DIR/Pegasus/Common/CIMDateTime.h
%attr(-,root,root) %PEGASUS_INCLUDE_DIR/Pegasus/Common/CIMFlavor.h
%attr(-,root,root) %PEGASUS_INCLUDE_DIR/Pegasus/Common/CIMIndication.h 
%attr(-,root,root) %PEGASUS_INCLUDE_DIR/Pegasus/Common/CIMInstance.h
%attr(-,root,root) %PEGASUS_INCLUDE_DIR/Pegasus/Common//CIMMethod.h
%attr(-,root,root) %PEGASUS_INCLUDE_DIR/Pegasus/Common/CIMName.h
%attr(-,root,root) %PEGASUS_INCLUDE_DIR/Pegasus/Common/CIMObject.h
%attr(-,root,root) %PEGASUS_INCLUDE_DIR/Pegasus/Common/CIMObjectPath.h
%attr(-,root,root) %PEGASUS_INCLUDE_DIR/Pegasus/Common/CIMParameter.h
%attr(-,root,root) %PEGASUS_INCLUDE_DIR/Pegasus/Common/CIMParamValue.h
%attr(-,root,root) %PEGASUS_INCLUDE_DIR/Pegasus/Common/CIMProperty.h
%attr(-,root,root) %PEGASUS_INCLUDE_DIR/Pegasus/Common/CIMPropertyList.h
%attr(-,root,root) %PEGASUS_INCLUDE_DIR/Pegasus/Common/CIMQualifierDecl.h
%attr(-,root,root) %PEGASUS_INCLUDE_DIR/Pegasus/Common/CIMQualifier.h
%attr(-,root,root) %PEGASUS_INCLUDE_DIR/Pegasus/Common/CIMScope.h
%attr(-,root,root) %PEGASUS_INCLUDE_DIR/Pegasus/Common/CIMStatusCode.h
%attr(-,root,root) %PEGASUS_INCLUDE_DIR/Pegasus/Common/CIMType.h
%attr(-,root,root) %PEGASUS_INCLUDE_DIR/Pegasus/Common/CIMValue.h
%attr(-,root,root) %PEGASUS_INCLUDE_DIR/Pegasus/Common/Config.h
%attr(-,root,root) %PEGASUS_INCLUDE_DIR/Pegasus/Common/ContentLanguageElement.h
%attr(-,root,root) %PEGASUS_INCLUDE_DIR/Pegasus/Common/ContentLanguages.h
%attr(-,root,root) %PEGASUS_INCLUDE_DIR/Pegasus/Common/Exception.h
%attr(-,root,root) %PEGASUS_INCLUDE_DIR/Pegasus/Common/Formatter.h
%attr(-,root,root) %PEGASUS_INCLUDE_DIR/Pegasus/Common/LanguageElementContainer.h
%attr(-,root,root) %PEGASUS_INCLUDE_DIR/Pegasus/Common/LanguageElement.h
%attr(-,root,root) %PEGASUS_INCLUDE_DIR/Pegasus/Common/Linkage.h
%attr(-,root,root) %PEGASUS_INCLUDE_DIR/Pegasus/Common/MessageLoader.h
%attr(-,root,root) %PEGASUS_INCLUDE_DIR/Pegasus/Common/OperationContext.h
%attr(-,root,root) %PEGASUS_INCLUDE_DIR/Pegasus/Common/Platform_%PEGASUS_HARDWARE_PLATFORM.h
%attr(-,root,root) %PEGASUS_INCLUDE_DIR/Pegasus/Common/ResponseHandler.h
%attr(-,root,root) %PEGASUS_INCLUDE_DIR/Pegasus/Common/SSLContext.h
%attr(-,root,root) %PEGASUS_INCLUDE_DIR/Pegasus/Common/String.h
%attr(-,root,root) %PEGASUS_INCLUDE_DIR/Pegasus/Consumer/CIMIndicationConsumer.h
%attr(-,root,root) %PEGASUS_INCLUDE_DIR/Pegasus/Consumer/Linkage.h
%attr(-,root,root) %PEGASUS_INCLUDE_DIR/Pegasus/Provider/CIMAssociationProvider.h
%attr(-,root,root) %PEGASUS_INCLUDE_DIR/Pegasus/Provider/CIMIndicationConsumerProvider.h
%attr(-,root,root) %PEGASUS_INCLUDE_DIR/Pegasus/Provider/CIMIndicationProvider.h
%attr(-,root,root) %PEGASUS_INCLUDE_DIR/Pegasus/Provider/CIMInstanceProvider.h
%attr(-,root,root) %PEGASUS_INCLUDE_DIR/Pegasus/Provider/CIMMethodProvider.h
%attr(-,root,root) %PEGASUS_INCLUDE_DIR/Pegasus/Provider/CIMOMHandle.h
%attr(-,root,root) %PEGASUS_INCLUDE_DIR/Pegasus/Provider/CIMProvider.h
%attr(-,root,root) %PEGASUS_INCLUDE_DIR/Pegasus/Provider/Linkage.h
%attr(-,root,root) %PEGASUS_INCLUDE_DIR/Pegasus/Provider/ProviderException.h
%PEGASUS_HTML_DIR
%attr(-,root,root) %PEGASUS_SAMPLES_DIR/Makefile 
%attr(-,root,root) %PEGASUS_SAMPLES_DIR/mak/common.mak 
%attr(-,root,root) %PEGASUS_SAMPLES_DIR/mak/config.mak 
%attr(-,root,root) %PEGASUS_SAMPLES_DIR/mak/library.mak 
%attr(-,root,root) %PEGASUS_SAMPLES_DIR/mak/%PEGASUS_HARDWARE_PLATFORM.mak
%attr(-,root,root) %PEGASUS_SAMPLES_DIR/mak/program.mak 
%attr(-,root,root) %PEGASUS_SAMPLES_DIR/mak/recurse.mak 
%attr(-,root,root) %PEGASUS_SAMPLES_DIR/Clients/Makefile 
%attr(-,root,root) %PEGASUS_SAMPLES_DIR/Clients/DefaultC++/Makefile
%attr(-,root,root) %PEGASUS_SAMPLES_DIR/Clients/DefaultC++/EnumInstances/EnumInstances.cpp 
%attr(-,root,root) %PEGASUS_SAMPLES_DIR/Clients/DefaultC++/EnumInstances/Makefile 
%attr(-,root,root) %PEGASUS_SAMPLES_DIR/Clients/DefaultC++/InvokeMethod/InvokeMethod.cpp 
%attr(-,root,root) %PEGASUS_SAMPLES_DIR/Clients/DefaultC++/InvokeMethod/Makefile 
%attr(-,root,root) %PEGASUS_SAMPLES_DIR/Clients/DefaultC++/SendTestIndications/Makefile 
%attr(-,root,root) %PEGASUS_SAMPLES_DIR/Clients/DefaultC++/SendTestIndications/SendTestIndications.cpp
%attr(-,root,root) %PEGASUS_SAMPLES_DIR/Providers/Makefile
%attr(-,root,root) %PEGASUS_SAMPLES_DIR/Providers/DefaultC++/Makefile
%attr(-,root,root) %PEGASUS_SAMPLES_DIR/Providers/DefaultC++/IndicationProvider/IndicationProvider.cpp
%attr(-,root,root) %PEGASUS_SAMPLES_DIR/Providers/DefaultC++/IndicationProvider/IndicationProvider.h
%attr(-,root,root) %PEGASUS_SAMPLES_DIR/Providers/DefaultC++/IndicationProvider/IndicationProviderMain.cpp
%attr(-,root,root) %PEGASUS_SAMPLES_DIR/Providers/DefaultC++/IndicationProvider/Makefile
%attr(-,root,root) %PEGASUS_SAMPLES_DIR/Providers/DefaultC++/InstanceProvider/EnumerateInstancesrspgood.xml 
%attr(-,root,root) %PEGASUS_SAMPLES_DIR/Providers/DefaultC++/InstanceProvider/EnumerateInstances.xml 
%attr(-,root,root) %PEGASUS_SAMPLES_DIR/Providers/DefaultC++/InstanceProvider/InstanceProvider.cpp 
%attr(-,root,root) %PEGASUS_SAMPLES_DIR/Providers/DefaultC++/InstanceProvider/InstanceProvider.h 
%attr(-,root,root) %PEGASUS_SAMPLES_DIR/Providers/DefaultC++/InstanceProvider/InstanceProviderMain.cpp 
%attr(-,root,root) %PEGASUS_SAMPLES_DIR/Providers/DefaultC++/InstanceProvider/Makefile 
%attr(-,root,root) %PEGASUS_SAMPLES_DIR/Providers/DefaultC++/MethodProvider/InvokeMethodrspgood.xml 
%attr(-,root,root) %PEGASUS_SAMPLES_DIR/Providers/DefaultC++/MethodProvider/InvokeMethod.xml 
%attr(-,root,root) %PEGASUS_SAMPLES_DIR/Providers/DefaultC++/MethodProvider/Makefile 
%attr(-,root,root) %PEGASUS_SAMPLES_DIR/Providers/DefaultC++/MethodProvider/MethodProvider.cpp 
%attr(-,root,root) %PEGASUS_SAMPLES_DIR/Providers/DefaultC++/MethodProvider/MethodProvider.h 
%attr(-,root,root) %PEGASUS_SAMPLES_DIR/Providers/DefaultC++/MethodProvider/MethodProviderMain.cpp 
%attr(-,root,root) %PEGASUS_SAMPLES_DIR/Providers/DefaultC++/SimpleDisplayConsumer/SimpleDisplayConsumer.cpp 
%attr(-,root,root) %PEGASUS_SAMPLES_DIR/Providers/DefaultC++/SimpleDisplayConsumer/SimpleDisplayConsumer.h
%attr(-,root,root) %PEGASUS_SAMPLES_DIR/Providers/DefaultC++/SimpleDisplayConsumer/SimpleDisplayConsumerMain.cpp
%attr(-,root,root) %PEGASUS_SAMPLES_DIR/Providers/DefaultC++/SimpleDisplayConsumer/Makefile
%attr(-,root,root) %PEGASUS_SAMPLES_DIR/Providers/Load/Makefile
%attr(-,root,root) %PEGASUS_SAMPLES_DIR/Providers/Load/IndicationProviderR.mof
%attr(-,root,root) %PEGASUS_SAMPLES_DIR/Providers/Load/InstanceProviderR.mof
%attr(-,root,root) %PEGASUS_SAMPLES_DIR/Providers/Load/MethodProviderR.mof
%attr(-,root,root) %PEGASUS_SAMPLES_DIR/Providers/Load/SampleProviderSchema.mof
%attr(-,root,root) %PEGASUS_SAMPLES_DIR/Providers/Load/SimpleDisplayConsumerR.mof

%if %{PEGASUS_BUILD_TEST_RPM}
%files test
%dir %attr(-,root,root) %PEGASUS_TEST_DIR
%dir %attr(-,root,root) %PEGASUS_TEST_DIR/bin
%dir %attr(-,root,root) %PEGASUS_TEST_DIR/lib
%dir %attr(-,root,root) %PEGASUS_TEST_DIR/mak
%attr(-,root,root) %PEGASUS_TEST_DIR/Makefile
%attr(0555,root,root) %PEGASUS_TEST_BIN_DIR/InvokeMethod2
%attr(0555,root,root) %PEGASUS_TEST_BIN_DIR/IPC
%attr(0555,root,root) %PEGASUS_TEST_BIN_DIR/TestAbstract
%attr(0555,root,root) %PEGASUS_TEST_BIN_DIR/TestArray
%attr(0555,root,root) %PEGASUS_TEST_BIN_DIR/TestBase64
%attr(0555,root,root) %PEGASUS_TEST_BIN_DIR/TestClassDecl
%attr(0555,root,root) %PEGASUS_TEST_BIN_DIR/TestClient
%attr(0555,root,root) %PEGASUS_TEST_BIN_DIR/TestDateTime
%attr(0555,root,root) %PEGASUS_TEST_BIN_DIR/TestFlavor
%attr(0555,root,root) %PEGASUS_TEST_BIN_DIR/TestFormatter
%attr(0555,root,root) %PEGASUS_TEST_BIN_DIR/TestHashTable
%attr(0555,root,root) %PEGASUS_TEST_BIN_DIR/TestInstanceDecl
%attr(0555,root,root) %PEGASUS_TEST_BIN_DIR/TestLogger
%attr(0555,root,root) %PEGASUS_TEST_BIN_DIR/TestMethod
%attr(0555,root,root) %PEGASUS_TEST_BIN_DIR/TestObject
%attr(0555,root,root) %PEGASUS_TEST_BIN_DIR/TestOperationContext
%attr(0555,root,root) %PEGASUS_TEST_BIN_DIR/TestParameter
%attr(0555,root,root) %PEGASUS_TEST_BIN_DIR/TestParamValue
%attr(0555,root,root) %PEGASUS_TEST_BIN_DIR/TestProperty
%attr(0555,root,root) %PEGASUS_TEST_BIN_DIR/TestQualifier
%attr(0555,root,root) %PEGASUS_TEST_BIN_DIR/TestQualifierDecl
%attr(0555,root,root) %PEGASUS_TEST_BIN_DIR/TestQualifierList
%attr(0555,root,root) %PEGASUS_TEST_BIN_DIR/TestQueue
%attr(0555,root,root) %PEGASUS_TEST_BIN_DIR/TestReference
%attr(0555,root,root) %PEGASUS_TEST_BIN_DIR/TestResolve
%attr(0555,root,root) %PEGASUS_TEST_BIN_DIR/TestResponseHandler
%attr(0555,root,root) %PEGASUS_TEST_BIN_DIR/TestScope
%attr(0555,root,root) %PEGASUS_TEST_BIN_DIR/TestStack
%attr(0555,root,root) %PEGASUS_TEST_BIN_DIR/TestStopwatch
%attr(0555,root,root) %PEGASUS_TEST_BIN_DIR/TestString
%attr(0555,root,root) %PEGASUS_TEST_BIN_DIR/TestStrToInstName
%attr(0555,root,root) %PEGASUS_TEST_BIN_DIR/TestTimeValue
%attr(0555,root,root) %PEGASUS_TEST_BIN_DIR/TestToMof
%attr(0555,root,root) %PEGASUS_TEST_BIN_DIR/TestValidateClass
%attr(0555,root,root) %PEGASUS_TEST_BIN_DIR/TestValue
%attr(0555,root,root) %PEGASUS_TEST_BIN_DIR/TracerTest
%attr(-,root,root) %PEGASUS_TEST_LIB_DIR/libSampleFamilyProvider.so.1
%attr(-,root,root) %PEGASUS_TEST_LIB_DIR/libSampleInstanceProvider.so.1
%attr(-,root,root) %PEGASUS_TEST_LIB_DIR/libSampleMethodProvider.so.1
%attr(-,root,root) %PEGASUS_TEST_MAK_DIR/commands.mak
%PEGASUS_TEST_REPOSITORY_DIR
%endif

