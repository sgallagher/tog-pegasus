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
Release: 1
Group: Systems Management/Base
Copyright: Open Group Pegasus Open Source
BuildRoot: %{_tmppath}/%{name}-%{version}-%{release}
Source: ftp://www.opengroup.org/pegasus/pegasus-%{version}.tar.gz
Requires: openssl-devel >= 0.9.6
Provides: cimserver pegasus-2.2

%description
WBEM Services for Linux enables management solutions that deliver increased
control of enterprise resources. WBEM is a platform and resource independent
DMTF standard that defines a common information model and communication
protocol for monitoring and controlling resources from diverse sources.

%prep
[ "$RPM_BUILD_ROOT" != "/" ] && [ -d $RPM_BUILD_ROOT ] && rm -rf $RPM_BUILD_ROOT;

%install
#
# Make directories
mkdir -p $RPM_BUILD_ROOT/etc/rc.d/init.d
mkdir -p $RPM_BUILD_ROOT/var/opt/wbem/{localauth,log}
mkdir -p $RPM_BUILD_ROOT/opt/wbem/{bin,sbin,etc,lib,mof,providers}
mkdir -p $RPM_BUILD_ROOT/opt/wbem/providers/lib
mkdir -p $RPM_BUILD_ROOT/opt/wbem/mof/{InterOp,Internal,ManagedSystem}
#
# Init scripts
#install -D -m 0755 -o 0 -g 0 $PEGASUS_ROOT/rpm/wbem1.rh7 $RPM_BUILD_ROOT/etc/rc.d/init.d/pegasus-wbem
install -D -m 0755 -o 0 -g 0 $PEGASUS_ROOT/rpm/wbem1.rh7 $RPM_BUILD_ROOT/opt/wbem/sbin/pegasus-wbem
#
# Programs
install -D -m 0544 -o 0 -g 0 $PEGASUS_HOME/bin/cimserver $RPM_BUILD_ROOT/opt/wbem/sbin/cimserver
install -D -m 0544 -o 0 -g 0 $PEGASUS_HOME/bin/cimauth   $RPM_BUILD_ROOT/opt/wbem/sbin/cimauth
install -D -m 0544 -o 0 -g 0 $PEGASUS_HOME/bin/cimconfig $RPM_BUILD_ROOT/opt/wbem/sbin/cimconfig
install -D -m 0755 -o 0 -g 0 $PEGASUS_HOME/bin/cimmof    $RPM_BUILD_ROOT/opt/wbem/bin/cimmof
install -D -m 0755 -o 0 -g 0 $PEGASUS_HOME/bin/wbemexec  $RPM_BUILD_ROOT/opt/wbem/bin/wbemexec
install -D -m 0755 -o 0 -g 0 $PEGASUS_HOME/bin/osinfo    $RPM_BUILD_ROOT/opt/wbem/bin/osinfo
install -D -m 0755 -o 0 -g 0 $PEGASUS_HOME/bin/cimprovider $RPM_BUILD_ROOT/opt/wbem/bin/cimprovider

# Libraries
install -D -m 0755 -o 0 -g 0 $PEGASUS_HOME/lib/libCIMxmlIndicationHandler.so  $RPM_BUILD_ROOT/opt/wbem/lib/libCIMxmlIndicationHandler.so
install -D -m 0755 -o 0 -g 0 $PEGASUS_HOME/lib/libCIMxmlIndicationHandler.1  $RPM_BUILD_ROOT/opt/wbem/lib/libCIMxmlIndicationHandler.1
install -D -m 0755 -o 0 -g 0 $PEGASUS_HOME/lib/libComputerSystemProvider.1  $RPM_BUILD_ROOT/opt/wbem/lib/libComputerSystemProvider.1
install -D -m 0755 -o 0 -g 0 $PEGASUS_HOME/lib/libConfigSettingProvider.so  $RPM_BUILD_ROOT/opt/wbem/lib/libConfigSettingProvider.so
install -D -m 0755 -o 0 -g 0 $PEGASUS_HOME/lib/libConfigSettingProvider.1  $RPM_BUILD_ROOT/opt/wbem/lib/libConfigSettingProvider.1
install -D -m 0755 -o 0 -g 0 $PEGASUS_HOME/lib/libNamespaceProvider.so  $RPM_BUILD_ROOT/opt/wbem/lib/libNamespaceProvider.so
install -D -m 0755 -o 0 -g 0 $PEGASUS_HOME/lib/libNamespaceProvider.1  $RPM_BUILD_ROOT/opt/wbem/lib/libNamespaceProvider.1
install -D -m 0755 -o 0 -g 0 $PEGASUS_HOME/lib/libpegCLIClientLib.so  $RPM_BUILD_ROOT/opt/wbem/lib/libpegCLIClientLib.so
install -D -m 0755 -o 0 -g 0 $PEGASUS_HOME/lib/libpegCLIClientLib.1  $RPM_BUILD_ROOT/opt/wbem/lib/libpegCLIClientLib.1
install -D -m 0755 -o 0 -g 0 $PEGASUS_HOME/lib/libOSProvider.1  $RPM_BUILD_ROOT/opt/wbem/lib/libOSProvider.1
install -D -m 0755 -o 0 -g 0 $PEGASUS_HOME/lib/libpegauthentication.so  $RPM_BUILD_ROOT/opt/wbem/lib/libpegauthentication.so
install -D -m 0755 -o 0 -g 0 $PEGASUS_HOME/lib/libpegauthentication.1  $RPM_BUILD_ROOT/opt/wbem/lib/libpegauthentication.1
install -D -m 0755 -o 0 -g 0 $PEGASUS_HOME/lib/libpegclient.so  $RPM_BUILD_ROOT/opt/wbem/lib/libpegclient.so
install -D -m 0755 -o 0 -g 0 $PEGASUS_HOME/lib/libpegclient.1  $RPM_BUILD_ROOT/opt/wbem/lib/libpegclient.1
install -D -m 0755 -o 0 -g 0 $PEGASUS_HOME/lib/libpegcliutils.so  $RPM_BUILD_ROOT/opt/wbem/lib/libpegcliutils.so
install -D -m 0755 -o 0 -g 0 $PEGASUS_HOME/lib/libpegcliutils.1  $RPM_BUILD_ROOT/opt/wbem/lib/libpegcliutils.1
install -D -m 0755 -o 0 -g 0 $PEGASUS_HOME/lib/libpegcommon.so  $RPM_BUILD_ROOT/opt/wbem/lib/libpegcommon.so
install -D -m 0755 -o 0 -g 0 $PEGASUS_HOME/lib/libpegcommon.1  $RPM_BUILD_ROOT/opt/wbem/lib/libpegcommon.1
install -D -m 0755 -o 0 -g 0 $PEGASUS_HOME/lib/libpegcompiler.so  $RPM_BUILD_ROOT/opt/wbem/lib/libpegcompiler.so
install -D -m 0755 -o 0 -g 0 $PEGASUS_HOME/lib/libpegcompiler.1  $RPM_BUILD_ROOT/opt/wbem/lib/libpegcompiler.1
install -D -m 0755 -o 0 -g 0 $PEGASUS_HOME/lib/libpegconfig.so  $RPM_BUILD_ROOT/opt/wbem/lib/libpegconfig.so
install -D -m 0755 -o 0 -g 0 $PEGASUS_HOME/lib/libpegconfig.1  $RPM_BUILD_ROOT/opt/wbem/lib/libpegconfig.1
install -D -m 0755 -o 0 -g 0 $PEGASUS_HOME/lib/libpegexportclient.so  $RPM_BUILD_ROOT/opt/wbem/lib/libpegexportclient.so
install -D -m 0755 -o 0 -g 0 $PEGASUS_HOME/lib/libpegexportclient.1  $RPM_BUILD_ROOT/opt/wbem/lib/libpegexportclient.1
install -D -m 0755 -o 0 -g 0 $PEGASUS_HOME/lib/libpegexportserver.so  $RPM_BUILD_ROOT/opt/wbem/lib/libpegexportserver.so
install -D -m 0755 -o 0 -g 0 $PEGASUS_HOME/lib/libpegexportserver.1  $RPM_BUILD_ROOT/opt/wbem/lib/libpegexportserver.1
install -D -m 0755 -o 0 -g 0 $PEGASUS_HOME/lib/libpeggetoopt.so  $RPM_BUILD_ROOT/opt/wbem/lib/libpeggetoopt.so
install -D -m 0755 -o 0 -g 0 $PEGASUS_HOME/lib/libpeggetoopt.1  $RPM_BUILD_ROOT/opt/wbem/lib/libpeggetoopt.1
install -D -m 0755 -o 0 -g 0 $PEGASUS_HOME/lib/libpeghandlerservice.so  $RPM_BUILD_ROOT/opt/wbem/lib/libpeghandlerservice.so
install -D -m 0755 -o 0 -g 0 $PEGASUS_HOME/lib/libpeghandlerservice.1  $RPM_BUILD_ROOT/opt/wbem/lib/libpeghandlerservice.1
install -D -m 0755 -o 0 -g 0 $PEGASUS_HOME/lib/libpegindicationservice.so  $RPM_BUILD_ROOT/opt/wbem/lib/libpegindicationservice.so
install -D -m 0755 -o 0 -g 0 $PEGASUS_HOME/lib/libpegindicationservice.1  $RPM_BUILD_ROOT/opt/wbem/lib/libpegindicationservice.1
install -D -m 0755 -o 0 -g 0 $PEGASUS_HOME/lib/libpeglistener.so  $RPM_BUILD_ROOT/opt/wbem/lib/libpeglistener.so
install -D -m 0755 -o 0 -g 0 $PEGASUS_HOME/lib/libpeglistener.1  $RPM_BUILD_ROOT/opt/wbem/lib/libpeglistener.1
install -D -m 0755 -o 0 -g 0 $PEGASUS_HOME/lib/libpegprm.so  $RPM_BUILD_ROOT/opt/wbem/lib/libpegprm.so
install -D -m 0755 -o 0 -g 0 $PEGASUS_HOME/lib/libpegprm.1  $RPM_BUILD_ROOT/opt/wbem/lib/libpegprm.1
install -D -m 0755 -o 0 -g 0 $PEGASUS_HOME/lib/libpegprovidermanager.so  $RPM_BUILD_ROOT/opt/wbem/lib/libpegprovidermanager.so
install -D -m 0755 -o 0 -g 0 $PEGASUS_HOME/lib/libpegprovidermanager.1  $RPM_BUILD_ROOT/opt/wbem/lib/libpegprovidermanager.1
install -D -m 0755 -o 0 -g 0 $PEGASUS_HOME/lib/libpegprovider.so  $RPM_BUILD_ROOT/opt/wbem/lib/libpegprovider.so
install -D -m 0755 -o 0 -g 0 $PEGASUS_HOME/lib/libpegprovider.1  $RPM_BUILD_ROOT/opt/wbem/lib/libpegprovider.1
install -D -m 0755 -o 0 -g 0 $PEGASUS_HOME/lib/libpegrepository.so  $RPM_BUILD_ROOT/opt/wbem/lib/libpegrepository.so
install -D -m 0755 -o 0 -g 0 $PEGASUS_HOME/lib/libpegrepository.1  $RPM_BUILD_ROOT/opt/wbem/lib/libpegrepository.1
install -D -m 0755 -o 0 -g 0 $PEGASUS_HOME/lib/libpegserver.so  $RPM_BUILD_ROOT/opt/wbem/lib/libpegserver.so
install -D -m 0755 -o 0 -g 0 $PEGASUS_HOME/lib/libpegserver.1  $RPM_BUILD_ROOT/opt/wbem/lib/libpegserver.1
install -D -m 0755 -o 0 -g 0 $PEGASUS_HOME/lib/libpeguser.so  $RPM_BUILD_ROOT/opt/wbem/lib/libpeguser.so
install -D -m 0755 -o 0 -g 0 $PEGASUS_HOME/lib/libpeguser.1  $RPM_BUILD_ROOT/opt/wbem/lib/libpeguser.1
install -D -m 0755 -o 0 -g 0 $PEGASUS_HOME/lib/libpegwql.so  $RPM_BUILD_ROOT/opt/wbem/lib/libpegwql.so
install -D -m 0755 -o 0 -g 0 $PEGASUS_HOME/lib/libpegwql.1  $RPM_BUILD_ROOT/opt/wbem/lib/libpegwql.1
install -D -m 0755 -o 0 -g 0 $PEGASUS_HOME/lib/libPG_TestPropertyTypes.so  $RPM_BUILD_ROOT/opt/wbem/lib/libPG_TestPropertyTypes.so
install -D -m 0755 -o 0 -g 0 $PEGASUS_HOME/lib/libPG_TestPropertyTypes.1  $RPM_BUILD_ROOT/opt/wbem/lib/libPG_TestPropertyTypes.1
install -D -m 0755 -o 0 -g 0 $PEGASUS_HOME/lib/libProcessProvider.1  $RPM_BUILD_ROOT/opt/wbem/lib/libProcessProvider.1
install -D -m 0755 -o 0 -g 0 $PEGASUS_HOME/lib/libProviderRegistrationProvider.so  $RPM_BUILD_ROOT/opt/wbem/lib/libProviderRegistrationProvider.so
install -D -m 0755 -o 0 -g 0 $PEGASUS_HOME/lib/libProviderRegistrationProvider.1  $RPM_BUILD_ROOT/opt/wbem/lib/libProviderRegistrationProvider.1
install -D -m 0755 -o 0 -g 0 $PEGASUS_HOME/lib/libRT_IndicationConsumer.so  $RPM_BUILD_ROOT/opt/wbem/lib/libRT_IndicationConsumer.so
install -D -m 0755 -o 0 -g 0 $PEGASUS_HOME/lib/libRT_IndicationConsumer.1  $RPM_BUILD_ROOT/opt/wbem/lib/libRT_IndicationConsumer.1
install -D -m 0755 -o 0 -g 0 $PEGASUS_HOME/lib/libRT_IndicationProvider.so  $RPM_BUILD_ROOT/opt/wbem/lib/libRT_IndicationProvider.so
install -D -m 0755 -o 0 -g 0 $PEGASUS_HOME/lib/libRT_IndicationProvider.1  $RPM_BUILD_ROOT/opt/wbem/lib/libRT_IndicationProvider.1
install -D -m 0755 -o 0 -g 0 $PEGASUS_HOME/lib/libSampleIndicationProvider.so  $RPM_BUILD_ROOT/opt/wbem/lib/libSampleIndicationProvider.so
install -D -m 0755 -o 0 -g 0 $PEGASUS_HOME/lib/libSampleIndicationProvider.1  $RPM_BUILD_ROOT/opt/wbem/lib/libSampleIndicationProvider.1
install -D -m 0755 -o 0 -g 0 $PEGASUS_HOME/lib/libSampleInstanceProvider.so  $RPM_BUILD_ROOT/opt/wbem/lib/libSampleInstanceProvider.so
install -D -m 0755 -o 0 -g 0 $PEGASUS_HOME/lib/libSampleInstanceProvider.1  $RPM_BUILD_ROOT/opt/wbem/lib/libSampleInstanceProvider.1
install -D -m 0755 -o 0 -g 0 $PEGASUS_HOME/lib/libSampleMethodProvider.so  $RPM_BUILD_ROOT/opt/wbem/lib/libSampleMethodProvider.so
install -D -m 0755 -o 0 -g 0 $PEGASUS_HOME/lib/libSampleMethodProvider.1  $RPM_BUILD_ROOT/opt/wbem/lib/libSampleMethodProvider.1
install -D -m 0755 -o 0 -g 0 $PEGASUS_HOME/lib/libsendmailIndicationHandler.so  $RPM_BUILD_ROOT/opt/wbem/lib/libsendmailIndicationHandler.so
install -D -m 0755 -o 0 -g 0 $PEGASUS_HOME/lib/libsendmailIndicationHandler.1  $RPM_BUILD_ROOT/opt/wbem/lib/libsendmailIndicationHandler.1
install -D -m 0755 -o 0 -g 0 $PEGASUS_HOME/lib/libsnmpIndicationHandler.so  $RPM_BUILD_ROOT/opt/wbem/lib/libsnmpIndicationHandler.so
install -D -m 0755 -o 0 -g 0 $PEGASUS_HOME/lib/libsnmpIndicationHandler.1  $RPM_BUILD_ROOT/opt/wbem/lib/libsnmpIndicationHandler.1
install -D -m 0755 -o 0 -g 0 $PEGASUS_HOME/lib/libUserAuthProvider.so  $RPM_BUILD_ROOT/opt/wbem/lib/libUserAuthProvider.so
install -D -m 0755 -o 0 -g 0 $PEGASUS_HOME/lib/libUserAuthProvider.1  $RPM_BUILD_ROOT/opt/wbem/lib/libUserAuthProvider.1

#
# CIM schema
#
install -D -m 0644 -o 0 -g 0 $PEGASUS_ROOT/Schemas/CIM25/CIM_Application25.mof $RPM_BUILD_ROOT/opt/wbem/mof/CIM_Application25.mof
install -D -m 0644 -o 0 -g 0 $PEGASUS_ROOT/Schemas/CIM25/CIM_Core25.mof        $RPM_BUILD_ROOT/opt/wbem/mof/CIM_Core25.mof
install -D -m 0644 -o 0 -g 0 $PEGASUS_ROOT/Schemas/CIM25/CIM_Core25_Add.mof    $RPM_BUILD_ROOT/opt/wbem/mof/CIM_Core25_Add.mof
install -D -m 0644 -o 0 -g 0 $PEGASUS_ROOT/Schemas/CIM25/CIM_Device25.mof      $RPM_BUILD_ROOT/opt/wbem/mof/CIM_Device25.mof
install -D -m 0644 -o 0 -g 0 $PEGASUS_ROOT/Schemas/CIM25/CIM_Events25.mof      $RPM_BUILD_ROOT/opt/wbem/mof/CIM_Events25.mof
install -D -m 0644 -o 0 -g 0 $PEGASUS_ROOT/Schemas/CIM25/CIM_Metrics25.mof     $RPM_BUILD_ROOT/opt/wbem/mof/CIM_Metrics25.mof
install -D -m 0644 -o 0 -g 0 $PEGASUS_ROOT/Schemas/CIM25/CIM_Network25.mof     $RPM_BUILD_ROOT/opt/wbem/mof/CIM_Network25.mof
install -D -m 0644 -o 0 -g 0 $PEGASUS_ROOT/Schemas/CIM25/CIM_Physical25.mof    $RPM_BUILD_ROOT/opt/wbem/mof/CIM_Physical25.mof
install -D -m 0644 -o 0 -g 0 $PEGASUS_ROOT/Schemas/CIM25/CIM_Policy25.mof      $RPM_BUILD_ROOT/opt/wbem/mof/CIM_Policy25.mof
install -D -m 0644 -o 0 -g 0 $PEGASUS_ROOT/Schemas/CIM25/CIM_Schema25.mof      $RPM_BUILD_ROOT/opt/wbem/mof/CIM_Schema25.mof
install -D -m 0644 -o 0 -g 0 $PEGASUS_ROOT/Schemas/CIM25/CIM_Support25.mof     $RPM_BUILD_ROOT/opt/wbem/mof/CIM_Support25.mof
install -D -m 0644 -o 0 -g 0 $PEGASUS_ROOT/Schemas/CIM25/CIM_System25.mof      $RPM_BUILD_ROOT/opt/wbem/mof/CIM_System25.mof
install -D -m 0644 -o 0 -g 0 $PEGASUS_ROOT/Schemas/CIM25/CIM_System25_Add.mof  $RPM_BUILD_ROOT/opt/wbem/mof/CIM_System25_Add.mof
install -D -m 0644 -o 0 -g 0 $PEGASUS_ROOT/Schemas/CIM25/CIM_User25.mof        $RPM_BUILD_ROOT/opt/wbem/mof/CIM_User25.mof

#
# Pegasus' schema
#
install -D -m 0644 -o 0 -g 0 $PEGASUS_ROOT/Schemas/Pegasus/InterOp/VER20/PG_Events20.mof          $RPM_BUILD_ROOT/opt/wbem/mof/PG_Events20.mof
install -D -m 0644 -o 0 -g 0 $PEGASUS_ROOT/Schemas/Pegasus/InterOp/VER20/PG_InterOpSchema20.mof   $RPM_BUILD_ROOT/opt/wbem/mof/InterOpSchema.mof
install -D -m 0644 -o 0 -g 0 $PEGASUS_ROOT/Schemas/Pegasus/InterOp/VER20/PG_ProviderModule20.mof  $RPM_BUILD_ROOT/opt/wbem/mof/PG_ProviderModule20.mof
install -D -m 0644 -o 0 -g 0 $PEGASUS_ROOT/Schemas/Pegasus/Internal/VER20/PG_Authorization20.mof  $RPM_BUILD_ROOT/opt/wbem/mof/PG_Authorization20.mof
install -D -m 0644 -o 0 -g 0 $PEGASUS_ROOT/Schemas/Pegasus/Internal/VER20/PG_ConfigSetting20.mof  $RPM_BUILD_ROOT/opt/wbem/mof/PG_ConfigSetting20.mof
install -D -m 0644 -o 0 -g 0 $PEGASUS_ROOT/Schemas/Pegasus/Internal/VER20/PG_InternalSchema20.mof $RPM_BUILD_ROOT/opt/wbem/mof/InternalSchema.mof
install -D -m 0644 -o 0 -g 0 $PEGASUS_ROOT/Schemas/Pegasus/Internal/VER20/PG_User20.mof           $RPM_BUILD_ROOT/opt/wbem/mof/PG_User20.mof
install -D -m 0644 -o 0 -g 0 $PEGASUS_ROOT/Schemas/Pegasus/Internal/VER20/PG_ShutdownService20.mof           $RPM_BUILD_ROOT/opt/wbem/mof/PG_ShutdownService20.mof
install -D -m 0644 -o 0 -g 0 $PEGASUS_ROOT/Schemas/Pegasus/CIM_Qualifiers25.mof                   $RPM_BUILD_ROOT/opt/wbem/mof/CIM_Qualifiers25.mof

#
# Pegasus Default Providers' schema
#
install -D -m 0644 -o 0 -g 0 $PEGASUS_ROOT/Schemas/Pegasus/ManagedSystem/VER20/PG_OperatingSystem20.mof                               $RPM_BUILD_ROOT/opt/wbem/mof/PG_OperatingSystem20.mof
install -D -m 0644 -o 0 -g 0 $PEGASUS_ROOT/Schemas/Pegasus/ManagedSystem/VER20/PG_OperatingSystem20R.mof                               $RPM_BUILD_ROOT/opt/wbem/mof/PG_OperatingSystem20R.mof
install -D -m 0644 -o 0 -g 0 $PEGASUS_ROOT/Schemas/Pegasus/ManagedSystem/VER20/PG_ComputerSystem20.mof                               $RPM_BUILD_ROOT/opt/wbem/mof/PG_ComputerSystem20.mof
install -D -m 0644 -o 0 -g 0 $PEGASUS_ROOT/Schemas/Pegasus/ManagedSystem/VER20/PG_ComputerSystem20R.mof                               $RPM_BUILD_ROOT/opt/wbem/mof/PG_ComputerSystem20R.mof
install -D -m 0644 -o 0 -g 0 $PEGASUS_ROOT/Schemas/Pegasus/ManagedSystem/VER20/PG_ManagedSystemSchema20.mof                               $RPM_BUILD_ROOT/opt/wbem/mof/ManagedSystemSchema.mof
install -D -m 0644 -o 0 -g 0 $PEGASUS_ROOT/Schemas/Pegasus/ManagedSystem/VER20/PG_ManagedSystemSchema20R.mof                               $RPM_BUILD_ROOT/opt/wbem/mof/ManagedSystemSchemaR.mof
install -D -m 0644 -o 0 -g 0 $PEGASUS_ROOT/Schemas/Pegasus/ManagedSystem/VER20/PG_UnixProcess20.mof                               $RPM_BUILD_ROOT/opt/wbem/mof/PG_UnixProcess20.mof
install -D -m 0644 -o 0 -g 0 $PEGASUS_ROOT/Schemas/Pegasus/ManagedSystem/VER20/PG_UnixProcess20R.mof                               $RPM_BUILD_ROOT/opt/wbem/mof/PG_UnixProcess20R.mof

#
# SSL Files
#
install -D -m 0644 -o 0 -g 0 $PEGASUS_ROOT/ssl.cnf                   $RPM_BUILD_ROOT/var/opt/wbem/ssl.orig
install -D -m 0555 -o 0 -g 0 $OPENSSL_HOME/bin/openssl $RPM_BUILD_ROOT/opt/wbem/sbin/openssl

#
# cimserver config files
#
install -D -m 0644 -o 0 -g 0 $PEGASUS_ROOT/src/Server/cimserver_current.conf $RPM_BUILD_ROOT/opt/wbem/cimserver_current.conf
install -D -m 0644 -o 0 -g 0 $PEGASUS_ROOT/src/Server/cimserver_planned.conf $RPM_BUILD_ROOT/opt/wbem/cimserver_planned.conf

#
# WBEM pam authentication
#
install -D -m 0644 -o 0 -g 0 $PEGASUS_ROOT/rpm/wbem $RPM_BUILD_ROOT/opt/wbem/wbem

#
# repository install script
#
install -D -m 0544 -o 0 -g 0 $PEGASUS_ROOT/rpm/scripts/init_repository_Linux $RPM_BUILD_ROOT/opt/wbem/sbin/init_repository

#
# man pages
#
install -D -m 0444 -o 0 -g 0 $PEGASUS_ROOT/rpm/manLinux/man1.Z/cimmof.1 $RPM_BUILD_ROOT/opt/wbem/share/man/man1/cimmof.1
install -D -m 0444 -o 0 -g 0 $PEGASUS_ROOT/rpm/manLinux/man1.Z/cimprovider.1 $RPM_BUILD_ROOT/opt/wbem/share/man/man1/cimprovider.1
install -D -m 0444 -o 0 -g 0 $PEGASUS_ROOT/rpm/manLinux/man1.Z/osinfo.1 $RPM_BUILD_ROOT/opt/wbem/share/man/man1/osinfo.1
install -D -m 0444 -o 0 -g 0 $PEGASUS_ROOT/rpm/manLinux/man1.Z/wbemexec.1 $RPM_BUILD_ROOT/opt/wbem/share/man/man1/wbemexec.1
install -D -m 0444 -o 0 -g 0 $PEGASUS_ROOT/rpm/manLinux/man1m.Z/cimauth.1m $RPM_BUILD_ROOT/opt/wbem/share/man/man1m/cimauth.1m
install -D -m 0444 -o 0 -g 0 $PEGASUS_ROOT/rpm/manLinux/man1m.Z/cimconfig.1m $RPM_BUILD_ROOT/opt/wbem/share/man/man1m/cimconfig.1m
install -D -m 0444 -o 0 -g 0 $PEGASUS_ROOT/rpm/manLinux/man1m.Z/cimprovider.1m $RPM_BUILD_ROOT/opt/wbem/share/man/man1m/cimprovider.1m
install -D -m 0444 -o 0 -g 0 $PEGASUS_ROOT/rpm/manLinux/man1m.Z/cimserver.1m $RPM_BUILD_ROOT/opt/wbem/share/man/man1m/cimserver.1m

%clean
#make clean
[ "$RPM_BUILD_ROOT" != "/" ] && [ -d $RPM_BUILD_ROOT ] && rm -rf $RPM_BUILD_ROOT;

%post
%define INSTALL_LOG /var/opt/wbem/log/install.log
echo `date` >%{INSTALL_LOG} 2>&1

# cimserver config files
install -D -m 0644 -o 0 -g 0 /opt/wbem/cimserver_planned.conf /etc/opt/wbem/cimserver_planned.conf
install -D -m 0644 -o 0 -g 0 /opt/wbem/cimserver_current.conf /etc/opt/wbem/cimserver_current.conf

# RH dependency
#/sbin/chkconfig --add pegasus-wbem
grep "^/opt/wbem/lib$" /etc/ld.so.conf > /dev/null 2> /dev/null
[ $? -ne 0 ] && echo "/opt/wbem/lib" >> /etc/ld.so.conf
/sbin/ldconfig

#linking config file out to /etc/opt as defined by FHS
mkdir -p /etc/opt/wbem	
ln -s $RPM_BUILD_ROOT/opt/wbem/mof $RPM_BUILD_ROOT/etc/opt/wbem/mof 

#link provider shared libraries
ln -s $RPM_BUILD_ROOT/opt/wbem/lib/libComputerSystemProvider.1 $RPM_BUILD_ROOT/opt/wbem/providers/lib/libComputerSystemProvider.so
ln -s $RPM_BUILD_ROOT/opt/wbem/lib/libComputerSystemProvider.1 $RPM_BUILD_ROOT/opt/wbem/providers/lib/libComputerSystemProvider.1
ln -s $RPM_BUILD_ROOT/opt/wbem/lib/libOSProvider.1 $RPM_BUILD_ROOT/opt/wbem/providers/lib/libOSProvider.1
ln -s $RPM_BUILD_ROOT/opt/wbem/lib/libOSProvider.1 $RPM_BUILD_ROOT/opt/wbem/providers/lib/libOSProvider.so
ln -s $RPM_BUILD_ROOT/opt/wbem/lib/libProcessProvider.1 $RPM_BUILD_ROOT/opt/wbem/providers/lib/libProcessProvider.1
ln -s $RPM_BUILD_ROOT/opt/wbem/lib/libProcessProvider.1 $RPM_BUILD_ROOT/opt/wbem/providers/lib/libProcessProvider.so

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
    -e "s/$EMAIL/root@$HOSTNAME/" /var/opt/wbem/ssl.orig \
    > /var/opt/wbem/ssl.cnf
chmod 644 /var/opt/wbem/ssl.cnf
chown bin /var/opt/wbem/ssl.cnf
chgrp bin /var/opt/wbem/ssl.cnf

/bin/rpm -qa >/var/opt/wbem/ssl.rnd 2>>%{INSTALL_LOG}
/bin/netstat -a >>/var/opt/wbem/ssl.rnd 2>>%{INSTALL_LOG}

/opt/wbem/sbin/openssl req -x509 -days 365 -newkey rsa:512          \
   -rand /var/opt/wbem/ssl.rnd -nodes -config /var/opt/wbem/ssl.cnf   \
   -keyout /var/opt/wbem/key.pem -out /var/opt/wbem/cert.pem 2>>%{INSTALL_LOG}

cat /var/opt/wbem/key.pem /var/opt/wbem/cert.pem > /var/opt/wbem/server.pem
rm /var/opt/wbem/key.pem /var/opt/wbem/cert.pem
cp /var/opt/wbem/server.pem /var/opt/wbem/client.pem

if [ -d "/var/opt/wbem/repository/root#PG_Internal" ]
then
  #
  # Save the current /var/opt/wbem/repository to
  # /var/opt/wbem/prev_repository.
  #

  REPOSITORY_LOC="/var/opt/wbem/repository"
  PREV_REPOSITORY_LOC="/var/opt/wbem/prev_repository"

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
/opt/wbem/sbin/cimserver
sleep 5
echo " Compiling mof files will take a few minutes."
echo " Output will be logged to %{INSTALL_LOG}."
. /opt/wbem/sbin/init_repository 2>>%{INSTALL_LOG}
#/sbin/service pegasus-wbem stop 2>>%{INSTALL_LOG}
/opt/wbem/sbin/cimserver -s
#
#  Set up links for the WBEM executables
#
ln -s /opt/wbem/sbin/cimauth /usr/local/sbin/cimauth
ln -s /opt/wbem/sbin/cimconfig /usr/local/sbin/cimconfig
ln -s /opt/wbem/sbin/cimserver /usr/local/sbin/cimserver
ln -s /opt/wbem/sbin/init_repository /usr/local/sbin/init_repository
ln -s /opt/wbem/sbin/openssl /usr/local/sbin/openssl
ln -s /opt/wbem/bin/cimmof /usr/local/bin/cimmof
ln -s /opt/wbem/bin/cimprovider /usr/local/bin/cimprovider
ln -s /opt/wbem/bin/osinfo /usr/local/bin/osinfo
ln -s /opt/wbem/bin/wbemexec /usr/local/bin/wbemexec
#
#  Set up links for the man pages
#
mkdir -p /usr/local/man/man1
mkdir -p /usr/local/man/man1m
ln -s /opt/wbem/share/man/man1/cimmof.1        /usr/local/man/man1/cimmof.1
ln -s /opt/wbem/share/man/man1/cimprovider.1   /usr/local/man/man1/cimprovider.1
ln -s /opt/wbem/share/man/man1/osinfo.1        /usr/local/man/man1/osinfo.1
ln -s /opt/wbem/share/man/man1/wbemexec.1      /usr/local/man/man1/wbemexec.1
ln -s /opt/wbem/share/man/man1m/cimauth.1m     /usr/local/man/man1m/cimauth.1m
ln -s /opt/wbem/share/man/man1m/cimconfig.1m   /usr/local/man/man1m/cimconfig.1m
ln -s /opt/wbem/share/man/man1m/cimprovider.1m /usr/local/man/man1m/cimprovider.1m
ln -s /opt/wbem/share/man/man1m/cimserver.1m   /usr/local/man/man1m/cimserver.1m

install -D -m 0644 -o 0 -g 0 /opt/wbem/wbem /etc/pam.d/wbem
%preun
if [ $1 = 0 ]; then
	# RH dependency
        /opt/wbem/sbin/cimserver -s
#	/sbin/service pegasus-wbem stop > /dev/null 2> /dev/null; :
#	/sbin/chkconfig --del pegasus-wbem
fi

%postun
if [ $1 = 0 ]; then
	grep -v "/opt/wbem/lib" /etc/ld.so.conf > /etc/ld.so.conf.new
	mv -f /etc/ld.so.conf.new /etc/ld.so.conf
	/sbin/ldconfig
	rm -fr /opt/wbem
#	rm -fr /etc/init.d/pegasus-wbem
	rm -rf /etc/opt/wbem
	rm -rf /var/opt/wbem
        rm -f /usr/local/sbin/cimauth
        rm -f /usr/local/sbin/cimconfig
        rm -f /usr/local/sbin/cimserver
        rm -f /usr/local/sbin/init_repository
        rm -f /usr/local/sbin/openssl
        rm -f /usr/local/bin/cimmof
        rm -f /usr/local/bin/cimprovider
        rm -f /usr/local/bin/osinfo
        rm -f /usr/local/bin/wbemexec
        rm -f /usr/local/man/man1/cimmof.1
        rm -f /usr/local/man/man1/cimprovider.1
        rm -f /usr/local/man/man1/osinfo.1
        rm -f /usr/local/man/man1/wbemexec.1
        rm -f /usr/local/man/man1m/cimauth.1m
        rm -f /usr/local/man/man1m/cimconfig.1m
        rm -f /usr/local/man/man1m/cimprovider.1m
        rm -f /usr/local/man/man1m/cimserver.1m
        rm -f /etc/pam.d/wbem
fi

%files
%dir /var/opt/wbem/localauth
%dir /var/opt/wbem/log
%dir /opt/wbem/providers/lib
%attr(-,root,root) /opt/wbem/sbin/pegasus-wbem
%attr(-,root,root) /var/opt/wbem/ssl.orig
%attr(-,root,root) /opt/wbem/cimserver_planned.conf
%attr(-,root,root) /opt/wbem/cimserver_current.conf
%attr(-,root,root) /opt/wbem/wbem
%attr(-,root,root) /opt/wbem/sbin/cimauth
%attr(-,root,root) /opt/wbem/sbin/cimconfig
%attr(-,root,root) /opt/wbem/bin/cimmof
%attr(-,root,root) /opt/wbem/sbin/cimserver
%attr(-,root,root) /opt/wbem/sbin/openssl
%attr(-,root,root) /opt/wbem/sbin/init_repository
%attr(-,root,root) /opt/wbem/bin/wbemexec
%attr(-,root,root) /opt/wbem/bin/cimprovider
%attr(-,root,root) /opt/wbem/bin/osinfo
%attr(-,root,root) /opt/wbem/lib/libCIMxmlIndicationHandler.so
%attr(-,root,root) /opt/wbem/lib/libCIMxmlIndicationHandler.1
%attr(-,root,root) /opt/wbem/lib/libComputerSystemProvider.1
%attr(-,root,root) /opt/wbem/lib/libConfigSettingProvider.so
%attr(-,root,root) /opt/wbem/lib/libConfigSettingProvider.1
%attr(-,root,root) /opt/wbem/lib/libNamespaceProvider.so
%attr(-,root,root) /opt/wbem/lib/libNamespaceProvider.1
%attr(-,root,root) /opt/wbem/lib/libOSProvider.1
%attr(-,root,root) /opt/wbem/lib/libpegauthentication.so
%attr(-,root,root) /opt/wbem/lib/libpegauthentication.1
%attr(-,root,root) /opt/wbem/lib/libpegclient.so
%attr(-,root,root) /opt/wbem/lib/libpegclient.1
%attr(-,root,root) /opt/wbem/lib/libpegcliutils.so
%attr(-,root,root) /opt/wbem/lib/libpegcliutils.1
%attr(-,root,root) /opt/wbem/lib/libpegcommon.so
%attr(-,root,root) /opt/wbem/lib/libpegcommon.1
%attr(-,root,root) /opt/wbem/lib/libpegcompiler.so
%attr(-,root,root) /opt/wbem/lib/libpegcompiler.1
%attr(-,root,root) /opt/wbem/lib/libpegconfig.so
%attr(-,root,root) /opt/wbem/lib/libpegconfig.1
%attr(-,root,root) /opt/wbem/lib/libpegexportclient.so
%attr(-,root,root) /opt/wbem/lib/libpegexportclient.1
%attr(-,root,root) /opt/wbem/lib/libpegexportserver.so
%attr(-,root,root) /opt/wbem/lib/libpegexportserver.1
%attr(-,root,root) /opt/wbem/lib/libpeggetoopt.so
%attr(-,root,root) /opt/wbem/lib/libpeggetoopt.1
%attr(-,root,root) /opt/wbem/lib/libpeghandlerservice.so
%attr(-,root,root) /opt/wbem/lib/libpeghandlerservice.1
%attr(-,root,root) /opt/wbem/lib/libpegindicationservice.so
%attr(-,root,root) /opt/wbem/lib/libpegindicationservice.1
%attr(-,root,root) /opt/wbem/lib/libpeglistener.so
%attr(-,root,root) /opt/wbem/lib/libpeglistener.1
%attr(-,root,root) /opt/wbem/lib/libpegprm.so
%attr(-,root,root) /opt/wbem/lib/libpegprm.1
%attr(-,root,root) /opt/wbem/lib/libpegprovidermanager.so
%attr(-,root,root) /opt/wbem/lib/libpegprovidermanager.1
%attr(-,root,root) /opt/wbem/lib/libpegprovider.so
%attr(-,root,root) /opt/wbem/lib/libpegprovider.1
%attr(-,root,root) /opt/wbem/lib/libpegrepository.so
%attr(-,root,root) /opt/wbem/lib/libpegrepository.1
%attr(-,root,root) /opt/wbem/lib/libpegserver.so
%attr(-,root,root) /opt/wbem/lib/libpegserver.1
%attr(-,root,root) /opt/wbem/lib/libpeguser.so
%attr(-,root,root) /opt/wbem/lib/libpeguser.1
%attr(-,root,root) /opt/wbem/lib/libpegwql.so
%attr(-,root,root) /opt/wbem/lib/libpegwql.1
%attr(-,root,root) /opt/wbem/lib/libProcessProvider.1
%attr(-,root,root) /opt/wbem/lib/libProviderRegistrationProvider.so
%attr(-,root,root) /opt/wbem/lib/libProviderRegistrationProvider.1
%attr(-,root,root) /opt/wbem/lib/libRT_IndicationConsumer.so
%attr(-,root,root) /opt/wbem/lib/libRT_IndicationConsumer.1
%attr(-,root,root) /opt/wbem/lib/libRT_IndicationProvider.so
%attr(-,root,root) /opt/wbem/lib/libRT_IndicationProvider.1
%attr(-,root,root) /opt/wbem/lib/libSampleIndicationProvider.so
%attr(-,root,root) /opt/wbem/lib/libSampleIndicationProvider.1
%attr(-,root,root) /opt/wbem/lib/libSampleInstanceProvider.so
%attr(-,root,root) /opt/wbem/lib/libSampleInstanceProvider.1
%attr(-,root,root) /opt/wbem/lib/libSampleMethodProvider.so
%attr(-,root,root) /opt/wbem/lib/libSampleMethodProvider.1
%attr(-,root,root) /opt/wbem/lib/libsendmailIndicationHandler.so
%attr(-,root,root) /opt/wbem/lib/libsendmailIndicationHandler.1
%attr(-,root,root) /opt/wbem/lib/libsnmpIndicationHandler.so
%attr(-,root,root) /opt/wbem/lib/libsnmpIndicationHandler.1
%attr(-,root,root) /opt/wbem/lib/libUserAuthProvider.so
%attr(-,root,root) /opt/wbem/lib/libUserAuthProvider.1
%attr(-,root,root) /opt/wbem/mof/CIM_Application25.mof
%attr(-,root,root) /opt/wbem/mof/CIM_Core25.mof
%attr(-,root,root) /opt/wbem/mof/CIM_Core25_Add.mof
%attr(-,root,root) /opt/wbem/mof/CIM_Device25.mof
%attr(-,root,root) /opt/wbem/mof/CIM_Events25.mof
%attr(-,root,root) /opt/wbem/mof/CIM_Metrics25.mof
%attr(-,root,root) /opt/wbem/mof/CIM_Network25.mof
%attr(-,root,root) /opt/wbem/mof/CIM_Physical25.mof
%attr(-,root,root) /opt/wbem/mof/CIM_Policy25.mof
%attr(-,root,root) /opt/wbem/mof/CIM_Schema25.mof
%attr(-,root,root) /opt/wbem/mof/CIM_Support25.mof
%attr(-,root,root) /opt/wbem/mof/CIM_System25.mof
%attr(-,root,root) /opt/wbem/mof/CIM_System25_Add.mof
%attr(-,root,root) /opt/wbem/mof/CIM_User25.mof
%attr(-,root,root) /opt/wbem/mof/PG_Events20.mof
%attr(-,root,root) /opt/wbem/mof/InterOpSchema.mof
%attr(-,root,root) /opt/wbem/mof/PG_ProviderModule20.mof
%attr(-,root,root) /opt/wbem/mof/PG_Authorization20.mof
%attr(-,root,root) /opt/wbem/mof/PG_ConfigSetting20.mof
%attr(-,root,root) /opt/wbem/mof/InternalSchema.mof
%attr(-,root,root) /opt/wbem/mof/PG_User20.mof
%attr(-,root,root) /opt/wbem/mof/PG_ShutdownService20.mof
%attr(-,root,root) /opt/wbem/mof/PG_ComputerSystem20.mof
%attr(-,root,root) /opt/wbem/mof/PG_ComputerSystem20R.mof
%attr(-,root,root) /opt/wbem/mof/PG_OperatingSystem20.mof
%attr(-,root,root) /opt/wbem/mof/PG_OperatingSystem20R.mof
%attr(-,root,root) /opt/wbem/mof/ManagedSystemSchema.mof
%attr(-,root,root) /opt/wbem/mof/ManagedSystemSchemaR.mof
%attr(-,root,root) /opt/wbem/mof/PG_UnixProcess20.mof
%attr(-,root,root) /opt/wbem/mof/PG_UnixProcess20R.mof
%attr(-,root,root) /opt/wbem/mof/CIM_Qualifiers25.mof
%attr(-,root,root) /opt/wbem/share/man/man1/cimmof.1
%attr(-,root,root) /opt/wbem/share/man/man1/cimprovider.1
%attr(-,root,root) /opt/wbem/share/man/man1/osinfo.1
%attr(-,root,root) /opt/wbem/share/man/man1/wbemexec.1
%attr(-,root,root) /opt/wbem/share/man/man1m/cimauth.1m
%attr(-,root,root) /opt/wbem/share/man/man1m/cimconfig.1m
%attr(-,root,root) /opt/wbem/share/man/man1m/cimprovider.1m
%attr(-,root,root) /opt/wbem/share/man/man1m/cimserver.1m
