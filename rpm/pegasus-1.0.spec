#
# pegasus-1.0-spec
#
# Package spec for PEGASUS
#

Summary: PEGASUS CIMOM for Linux
Name: pegasus
Version: 1.0
Release: 1
Group: Systems Management/Base
Copyright: MIT Public Licence
BuildRoot: %{_tmppath}/PEGASUS-root
Packager: Markus Mueller <markus_mueller@de.ibm.com>

Source: ftp://www.opengroup.org/pegasus/pegasus-1.0.tar.gz
#Source1: ftp://www.opengroup.org/pegasus/aic.tar.gz

#Patch0: cimom-peg1.patch.gz
#Patch1: cimom-peg2.patch.gz

%description
Pegasus CIMOM enables a Linux System for WBEM (visit http://www.dmtf.org for
more information about CIM and WBEM). With appropriate instrumentation
Linux can provide systems management information or be managed by a remote
client application.
This package requires only the standard C-library.

%prep
rm -rf $RPM_BUILD_ROOT

#%setup -c -T -a 0
#%setup -c -T -a 1 -D
#%setup -c -T -a 2 -D
#%setup -c -T -a 3 -D
%setup
export PEGASUS_ROOT=/usr/src/packages/BUILD/pegasus-1.0
export PEGASUS_HOME=$RPM_BUILD_ROOT/usr/local/pegasus-1.0
export PEGASUS_PLATFORM=LINUX_IX86_GNU
export PEGASUS_CONCURRENT=yes
make depend

#%patch0 -p1
#%patch1 -p1

%build

export PEGASUS_ROOT=/usr/src/packages/BUILD/pegasus-1.0
export PEGASUS_HOME=$RPM_BUILD_ROOT/usr/local
export PEGASUS_PLATFORM=LINUX_IX86_GNU
export PEGASUS_CONCURRENT=yes
#export PATH=$RPM_BUILD_ROOT/usr/local/pegasus-1.0/bin:$PATH
make


%install

# Copy binaries and libraries
#mkdir -p $RPM_BUILD_ROOT/usr/local/bin
#cp -a $RPM_BUILD_ROOT/usr/local/pegasus-1.0/bin/* $RPM_BUILD_ROOT/usr/local/bin
#mkdir -p $RPM_BUILD_ROOT/usr/local/lib
#cp -a $RPM_BUILD_ROOT/usr/local/pegasus-1.0/lib/* $RPM_BUILD_ROOT/usr/local/lib

# Copy the schema

mkdir -p $RPM_BUILD_ROOT/etc/pegasus/mof
mkdir -p $RPM_BUILD_ROOT/etc/pegasus/repository
cp -a $PEGASUS_ROOT/Schemas/CIM25/*.mof $RPM_BUILD_ROOT/etc/pegasus/mof

#
mkdir -p $RPM_BUILD_ROOT/etc/rc.d/
cp $PEGASUS_ROOT/rpm/pegasus $RPM_BUILD_ROOT/etc/rc.d/

mkdir -p $RPM_BUILD_ROOT/var/pegasus/log
mkdir -p $RPM_BUILD_ROOT/etc/pegasus
touch $RPM_BUILD_ROOT/etc/pegasus/pegasus.conf

# Copy the necessary include files

mkdir -p $RPM_BUILD_ROOT/usr/local/include/Pegasus/Client
cp $PEGASUS_ROOT/src/Pegasus/Client/*.h $RPM_BUILD_ROOT/usr/local/include/Pegasus/Client
mkdir -p $RPM_BUILD_ROOT/usr/local/include/Pegasus/Common
cp $PEGASUS_ROOT/src/Pegasus/Common/*.h $RPM_BUILD_ROOT/usr/local/include/Pegasus/Common
mkdir -p $RPM_BUILD_ROOT/usr/local/include/Pegasus/Compiler
cp $PEGASUS_ROOT/src/Pegasus/Compiler/*.h $RPM_BUILD_ROOT/usr/local/include/Pegasus/Compiler
mkdir -p $RPM_BUILD_ROOT/usr/local/include/Pegasus/Config
cp $PEGASUS_ROOT/src/Pegasus/Config/*.h $RPM_BUILD_ROOT/usr/local/include/Pegasus/Config
mkdir -p $RPM_BUILD_ROOT/usr/local/include/Pegasus/Handler
cp $PEGASUS_ROOT/src/Pegasus/Handler/*.h $RPM_BUILD_ROOT/usr/local/include/Pegasus/Handler
mkdir -p $RPM_BUILD_ROOT/usr/local/include/Pegasus/Protocol
cp $PEGASUS_ROOT/src/Pegasus/Protocol/*.h $RPM_BUILD_ROOT/usr/local/include/Pegasus/Protocol
mkdir -p $RPM_BUILD_ROOT/usr/local/include/Pegasus/Provider
cp $PEGASUS_ROOT/src/Pegasus/Provider/*.h $RPM_BUILD_ROOT/usr/local/include/Pegasus/Provider
mkdir -p $RPM_BUILD_ROOT/usr/local/include/Pegasus/Provider2
cp $PEGASUS_ROOT/src/Pegasus/Provider2/*.h $RPM_BUILD_ROOT/usr/local/include/Pegasus/Provider2
mkdir -p $RPM_BUILD_ROOT/usr/local/include/Pegasus/Repository
cp $PEGASUS_ROOT/src/Pegasus/Repository/*.h $RPM_BUILD_ROOT/usr/local/include/Pegasus/Repository
mkdir -p $RPM_BUILD_ROOT/usr/local/include/Pegasus/Security/Authentication
cp $PEGASUS_ROOT/src/Pegasus/Security/Authentication/*.h $RPM_BUILD_ROOT/usr/local/include/Pegasus/Security/Authentication
mkdir -p $RPM_BUILD_ROOT/usr/local/include/Pegasus/Server
cp $PEGASUS_ROOT/src/Pegasus/Server/*.h $RPM_BUILD_ROOT/usr/local/include/Pegasus/Server
mkdir -p $RPM_BUILD_ROOT/usr/local/include/Pegasus/getoopt
cp $PEGASUS_ROOT/src/Pegasus/getoopt/*.h $RPM_BUILD_ROOT/usr/local/include/Pegasus/getoopt

#make INSTALLROOT=$RPM_BUILD_ROOT install
#%__mkdir -p $RPM_BUILD_ROOT/usr/pegasus/lib/mof
#%__mkdir -p $RPM_BUILD_ROOT/var/pegasus

%post
ldconfig
sbin/insserv etc/init.d/pegasus
%postun
sbin/insserv etc/init.d

%files

%defattr(-,root,root,0755)
%doc doc/*.txt doc/DOCREMARKS doc/HISTORY doc/NOTES
%doc doc/WorkPapers/WritingProviders.txt

%dir %attr(-,root,root) /usr/local/bin
%dir %attr(-,root,root) /usr/local/lib
%dir %attr(-,root,root) /usr/local/include
%dir %attr(-,root,root) /var/pegasus/log
%dir %attr(-,root,root) /etc/pegasus/mof
%dir %attr(-,root,root) /etc/pegasus/repository
%dir %attr(-,root,root) /usr/local/include/Pegasus
%dir %attr(-,root,root) /usr/local/include/Pegasus/Client
%dir %attr(-,root,root) /usr/local/include/Pegasus/Common
%dir %attr(-,root,root) /usr/local/include/Pegasus/Compiler
%dir %attr(-,root,root) /usr/local/include/Pegasus/Config
%dir %attr(-,root,root) /usr/local/include/Pegasus/Handler
%dir %attr(-,root,root) /usr/local/include/Pegasus/Protocol
%dir %attr(-,root,root) /usr/local/include/Pegasus/Provider
%dir %attr(-,root,root) /usr/local/include/Pegasus/Provider2
%dir %attr(-,root,root) /usr/local/include/Pegasus/Repository
%dir %attr(-,root,root) /usr/local/include/Pegasus/Security
%dir %attr(-,root,root) /usr/local/include/Pegasus/Security/Authentication
%dir %attr(-,root,root) /usr/local/include/Pegasus/Server
%dir %attr(-,root,root) /usr/local/include/Pegasus/getoopt

%config %attr(-,root,root) /etc/pegasus/pegasus.conf
%config %attr(-,root,root) /etc/rc.d/pegasus

#%attr(-,root,root) /usr/local/lib/libCIMxmlIndicationHandler.so
%attr(-,root,root) /usr/local/lib/libConfigSettingProvider.so
%attr(-,root,root) /usr/local/lib/libDynLib.so
#%attr(-,root,root) /usr/local/lib/libHelloWorldProvider.so
%attr(-,root,root) /usr/local/lib/libMyProvider.so
%attr(-,root,root) /usr/local/lib/lib__NamespaceProvider.so
%attr(-,root,root) /usr/local/lib/libpegclient.so
%attr(-,root,root) /usr/local/lib/libpegcliutils.so
%attr(-,root,root) /usr/local/lib/libpegcommon.so
%attr(-,root,root) /usr/local/lib/libpegcompiler.so
%attr(-,root,root) /usr/local/lib/libpegconfig.so
%attr(-,root,root) /usr/local/lib/libpegexportclient.so
%attr(-,root,root) /usr/local/lib/libpeggetoopt.so
%attr(-,root,root) /usr/local/lib/libpegprotocol.so
%attr(-,root,root) /usr/local/lib/libpegprovider.so
%attr(-,root,root) /usr/local/lib/libpegprovider2.so
%attr(-,root,root) /usr/local/lib/libpegrepository.so
%attr(-,root,root) /usr/local/lib/libpegserver.so
#%attr(-,root,root) /usr/local/lib/libsendmailIndicationHandler.so
%attr(-,root,root) /usr/local/lib/libslp.so

%attr(-,root,root) /usr/local/bin/CGIClient
%attr(-,root,root) /usr/local/bin/Client
%attr(-,root,root) /usr/local/bin/Repository
%attr(-,root,root) /usr/local/bin/cimconfig
%attr(-,root,root) /usr/local/bin/cimmof
%attr(-,root,root) /usr/local/bin/cimserver
%attr(-,root,root) /usr/local/bin/wbemexec

%attr(-,root,root) /etc/pegasus/mof/CIM_Application25.mof
%attr(-,root,root) /etc/pegasus/mof/CIM_Core25.mof
%attr(-,root,root) /etc/pegasus/mof/CIM_Core25_Add.mof
%attr(-,root,root) /etc/pegasus/mof/CIM_Device25.mof
%attr(-,root,root) /etc/pegasus/mof/CIM_Events25.mof
%attr(-,root,root) /etc/pegasus/mof/CIM_Metrics25.mof
%attr(-,root,root) /etc/pegasus/mof/CIM_Network25.mof
%attr(-,root,root) /etc/pegasus/mof/CIM_Physical25.mof
%attr(-,root,root) /etc/pegasus/mof/CIM_Policy25.mof
%attr(-,root,root) /etc/pegasus/mof/CIM_Schema25.mof
%attr(-,root,root) /etc/pegasus/mof/CIM_Support25.mof
%attr(-,root,root) /etc/pegasus/mof/CIM_System25.mof
%attr(-,root,root) /etc/pegasus/mof/CIM_System25_Add.mof
%attr(-,root,root) /etc/pegasus/mof/CIM_User25.mof

%attr(-,root,root) /usr/local/include/Pegasus/Client/*.h
%attr(-,root,root) /usr/local/include/Pegasus/Common/*.h
%attr(-,root,root) /usr/local/include/Pegasus/Compiler/*.h
%attr(-,root,root) /usr/local/include/Pegasus/Config/*.h
%attr(-,root,root) /usr/local/include/Pegasus/Handler/*.h
%attr(-,root,root) /usr/local/include/Pegasus/Protocol/*.h
%attr(-,root,root) /usr/local/include/Pegasus/Provider/*.h
%attr(-,root,root) /usr/local/include/Pegasus/Provider2/*.h
%attr(-,root,root) /usr/local/include/Pegasus/Repository/*.h
%attr(-,root,root) /usr/local/include/Pegasus/Security/Authentication/*.h
%attr(-,root,root) /usr/local/include/Pegasus/Server/*.h
%attr(-,root,root) /usr/local/include/Pegasus/getoopt/*.h

