#
# pegasus-1.0-spec
#
# Package spec for PEGASUS
#

Summary: PEGASUS CIMOM for Linux
Name: pegasus
Autoreqprov:  on
Version: 1.0
Release: 5
Group: Systems Management/Base
Copyright: MIT Public Licence
BuildRoot: %{_tmppath}/%{name}-%{version}-%{release}
Packager: Markus Mueller <markus_mueller@de.ibm.com>
Provides: cimserver
URL:    http://www.opengroup.org/pegasus
Source: ftp://www.opengroup.org/pegasus/pegasus-%{version}.tar.gz
#Source1: ftp://www.opengroup.org/pegasus/aic.tar.gz

#Patch0: cimom-peg1.patch.gz
#Patch1: cimom-peg2.patch.gz

%description
Pegasus CIMOM enables a Linux System for WBEM (visit http://www.dmtf.org for
more information about CIM and WBEM). With appropriate instrumentation
Linux can provide systems management information or be managed by a remote
client application.
This package requires the standard C-library, a threading library like
linuxthreads or GNU pth and openssl.

%package devel
Summary:      The Pegasus source tree
Group:        Systems Management/Base

%description devel
This package contains the Pegasus source tree, header files and
static libraries (if any).

%prep
rm -rf $RPM_BUILD_ROOT
# Copy the necessary include files
/var/tmp/buildincludes $PEGASUS_ROOT $RPM_BUILD_ROOT

%setup
%build
export PEGASUS_ROOT=/usr/src/packages/BUILD/pegasus-1.0
export PEGASUS_HOME=$RPM_BUILD_ROOT/usr/pegasus-1.0
export PEGASUS_PLATFORM=LINUX_IX86_GNU
export PEGASUS_HAS_SSL=yes
make depend
make
#%ifarch ppc s390 s390x sparc sparc64
#%endif

%install

# Copy binaries and libraries
mkdir -p $RPM_BUILD_ROOT/usr/bin
cp -a $RPM_BUILD_ROOT/usr/pegasus-1.0/bin/* $RPM_BUILD_ROOT/usr/bin
mkdir -p $RPM_BUILD_ROOT/usr/lib
cp -a $RPM_BUILD_ROOT/usr/pegasus-1.0/lib/* $RPM_BUILD_ROOT/usr/lib

# Copy the schema

mkdir -p $RPM_BUILD_ROOT/var/lib/pegasus/mof
mkdir -p $RPM_BUILD_ROOT/var/lib/pegasus/repository
cp -a $PEGASUS_ROOT/Schemas/CIM25/*.mof $RPM_BUILD_ROOT/var/lib/pegasus/mof

#
mkdir -p $RPM_BUILD_ROOT/etc/rc.d/
cp $PEGASUS_ROOT/rpm/pegasus $RPM_BUILD_ROOT/etc/rc.d/

mkdir -p $RPM_BUILD_ROOT/var/pegasus/log
mkdir -p $RPM_BUILD_ROOT/var/lib/pegasus
mkdir -p $RPM_BUILD_ROOT/etc/pegasus
touch $RPM_BUILD_ROOT/etc/pegasus/pegasus.conf

#make INSTALLROOT=$RPM_BUILD_ROOT install
#%__mkdir -p $RPM_BUILD_ROOT/usr/pegasus/lib/mof
#%__mkdir -p $RPM_BUILD_ROOT/var/pegasus

%clean
make clean
[ "$RPM_BUILD_ROOT" != "/" ] && [ -d $RPM_BUILD_ROOT ] && rm -rf $RPM_BUILD_ROOT;

%post
ldconfig
cimmof -R/var/lib/pegasus -I/var/lib/pegasus/mof -nroot /var/lib/pegasus/mof/CIM_Core25.mof
cimmof -R/var/lib/pegasus -I/var/lib/pegasus/mof -nroot/cimv2 /var/lib/pegasus/mof/CIM_Schema25.mof
sbin/insserv etc/init.d/pegasus
echo "please add the path /usr/lib/pegasus to the ld.so.conf"
%postun
rm -rf /var/lib/pegasus/repository
sbin/insserv etc/init.d

%files devel -f $RPM_BUILD_ROOT/rpm_pegasus_include_files

%defattr(-,root,root,0755)
%doc doc/*.txt doc/DOCREMARKS doc/HISTORY doc/NOTES
%doc doc/WorkPapers/WritingProviders.txt

%files

%dir %attr(-,root,root) /usr/bin
%dir %attr(-,root,root) /usr/lib
%dir %attr(-,root,root) /usr/include
%dir %attr(-,root,root) /var/pegasus/log
%dir %attr(-,root,root) /var/lib/pegasus
%dir %attr(-,root,root) /var/lib/pegasus/mof
%dir %attr(-,root,root) /var/lib/pegasus/repository

%config %attr(-,root,root) /etc/pegasus/pegasus.conf
%config %attr(-,root,root) /etc/rc.d/pegasus

%attr(-,root,root) /usr/lib/

%attr(-,root,root) /usr/bin/

%attr(-,root,root) /var/lib/pegasus/mof/
